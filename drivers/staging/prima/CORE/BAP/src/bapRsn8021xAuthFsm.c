/*
 * Copyright (c) 2012-2013, The Linux Foundation. All rights reserved.
 *
 * Previously licensed under the ISC license by Qualcomm Atheros, Inc.
 *
 *
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all
 * copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */
/*
 * Copyright (c) 2012, The Linux Foundation. All rights reserved.
 *
 * Previously licensed under the ISC license by Qualcomm Atheros, Inc.
 *
 *
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all
 * copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

/*
                                                           
                                                                                                                              
  
                                                        
                                                
  
                                  
                                
            
                                                        
                                                         
  
 */
#include "vos_types.h"
#include "bapRsnSsmServices.h"
#include "bapRsnSsmEapol.h"
#include "bapRsnErrors.h"
#include "bapInternal.h"
#include "bapRsn8021xFsm.h"
#include "bapRsn8021xAuthFsm.h"
#include "vos_utils.h"
#include "vos_memory.h"
#include "vos_timer.h"
#include "bapRsnTxRx.h"
#include "bapRsnSsmAesKeyWrap.h"
#include "btampFsm.h"

//                                                       
#define DISCONNECT           0
#define DISCONNECTED         1
#define INITIALIZE           2
#define AUTHENTICATION       3
#define AUTHENTICATION_2     4
#define GET_PSK              5   
#define GET_EAP_KEY          6
#define PTK_START            7
#define PTK_INIT_NEGO        8
#define PTK_INIT_NEGO_TX     9
#define PTK_INIT_DONE       10
#define UPDATE_KEYS_REQ     11
#define INTEG_FAILURE       12
#define KEY_UPDATE          13
#define NUM_STATES (KEY_UPDATE + 1)



static tAuthRsnFsmConsts authConsts = { 2000, 3 };  //                    
static v_U8_t aniSsmIeRsnOui[] = ANI_SSM_IE_RSN_OUI;


/*                                     
                                  
                                      */

static 
int zeroOutPtk(tAuthRsnFsm *fsm);
static 
int stopAllTimers(tAuthRsnFsm *fsm);

static
int checkMic(tAuthRsnFsm *fsm, 
             tAniEapolKeyAvailEventData *data);

static 
int checkLocalReplayCounter(tAuthRsnFsm *fsm, 
                            tAniEapolKeyAvailEventData *data);
static 
int checkPeerReplayCounter(tAuthRsnFsm *fsm, 
                           tAniEapolKeyAvailEventData *data);

static int checkInfoElement(tAuthRsnFsm *fsm,
                            tAniEapolKeyAvailEventData *data);

static
int derivePtk(tAuthRsnFsm *fsm, 
              tAniEapolKeyAvailEventData *data);


static int checkTransition(tAuthRsnFsm *fsm, void *arg);

static int
gotoStateInit(tAuthRsnFsm *fsm);

static void msg2TimerCallback( void * );
static void msg4TimerCallback( void * );

static int authRsnRxFrameHandler( v_PVOID_t pvosGCtx, vos_pkt_t *pPacket );
static int authRsnTxCompleteHandler( v_PVOID_t pvosGCtx, vos_pkt_t *pPacket, VOS_STATUS retStatus );

/*                               
                                 
                                */

int authRsnAuthStartEventHandler( tAuthRsnFsm *fsm );
int authRsnAuthDisconEventHandler( tAuthRsnFsm *fsm );

/*                        
                         
                         */

/* 
                 
  
            
                                                                 
          
  
                                                               
                                                                   
  
                                           
 */
int
authRsnFsmInit(tAuthRsnFsmConsts *constsIn)
{
    //                                        
    //                        
    authConsts.timeoutPeriod = 2000;    //  
    authConsts.maxTries = 3;

    return ANI_OK;
}

/* 
                   
  
           
                                                                     
                         
   
                                                               
                                                                 
                                                             
  
                                           
 */
int
authRsnFsmCreate(tBtampContext *ctx)
{
    int retVal = ANI_OK;
    tAuthRsnFsm *fsm = &ctx->uFsm.authFsm;

    //                            
    vos_mem_zero( fsm, sizeof(tAuthRsnFsm));

    if( !VOS_IS_STATUS_SUCCESS( bapRsnRegisterTxRxCallbacks( authRsnTxCompleteHandler,
                                            authRsnRxFrameHandler ) ) )
    {
        return ANI_ERROR;
    }

    if( !VOS_IS_STATUS_SUCCESS( bapRsnRegisterRxCallback( ctx->pvosGCtx ) ) )
    {
        return ANI_ERROR;
    }

    //                             
    fsm->staCtx = (tStaContext *)vos_mem_malloc( sizeof(tStaContext) );
    if (fsm->staCtx == NULL) 
    {
        retVal = ANI_E_MALLOC_FAILED;
        VOS_ASSERT( 0 );
        goto error;
    }
    //                              
    vos_mem_zero( fsm->staCtx, sizeof(tStaContext) );
    
    fsm->ctx = ctx;
    fsm->staCtx->authRsnFsm = fsm;
    //                 
    fsm->staCtx->pwCipherType = eCSR_ENCRYPT_TYPE_AES;

    if( !VOS_IS_STATUS_SUCCESS( vos_timer_init( &fsm->msg2Timer, VOS_TIMER_TYPE_SW, msg2TimerCallback, fsm ) ) )
    {
        retVal = ANI_E_MALLOC_FAILED;
        VOS_ASSERT( 0 );
        goto error;
    }

    if( !VOS_IS_STATUS_SUCCESS( vos_timer_init( &fsm->msg4Timer, VOS_TIMER_TYPE_SW, msg4TimerCallback, fsm ) ) )
    {
        retVal = ANI_E_MALLOC_FAILED;
        VOS_ASSERT( 0 );
        goto error;
    }

    retVal = aniAsfPacketAllocateExplicit(&fsm->lastEapol,
                                          RSN_MAX_PACKET_SIZE,
                                          EAPOL_TX_HEADER_SIZE );
    if (retVal != ANI_OK) 
    {
        VOS_ASSERT( 0 );
        goto error;
    }

    aniAsfPacketAllocate(&fsm->staCtx->pmk);
    if (fsm->staCtx->pmk == NULL) 
    {
        retVal = ANI_E_MALLOC_FAILED;
        VOS_ASSERT( 0 );
        goto error;
    }

    aniAsfPacketAllocateExplicit(&fsm->staCtx->ieSta,
                                 RSN_IE_MAX_PACKET_SIZE,
                                 RSN_IE_HEADER_SIZE );
    if (fsm->staCtx->ieSta == NULL) 
    {
        retVal = ANI_E_MALLOC_FAILED;
        VOS_ASSERT( 0 );
        goto error;
    }

    fsm->cryptHandle = 0;
    if( !VOS_IS_STATUS_SUCCESS( vos_crypto_init( &fsm->cryptHandle ) ) )
    {
        retVal = ANI_E_FAILED;
        VOS_ASSERT( 0 );
        goto error;
    }

    fsm->currentState = INITIALIZE;
    gotoStateInit(fsm);

    //                                                                   
    authRsnFsmProcessEvent( fsm, RSN_FSM_AUTH_START, NULL );

    return ANI_OK;

 error:
    authRsnFsmFree(ctx);

    return retVal;
    
}

/* 
                 
  
           
                                                                    
                                                           
   
                                                               
  
                                           
 */
int
authRsnFsmFree(tBtampContext *ctx)
{
    tAuthRsnFsm *fsm = &ctx->uFsm.authFsm;

    VOS_ASSERT(fsm);

    if( fsm->cryptHandle)
    {
        vos_crypto_deinit( fsm->cryptHandle );
    }
     
    bapRsnClearTxRxCallbacks(); 

    if ( fsm->staCtx )
    {
        fsm->staCtx->authRsnFsm = NULL;
    }

    if ( VOS_TIMER_STATE_UNUSED != fsm->msg2Timer.state ) vos_timer_destroy( &fsm->msg2Timer );
    if ( VOS_TIMER_STATE_UNUSED != fsm->msg4Timer.state ) vos_timer_destroy( &fsm->msg4Timer );

    if (fsm->lastEapol)
    {    
        aniAsfPacketFree(fsm->lastEapol);
        fsm->lastEapol = NULL; 
    }

    if( fsm->staCtx )
    {
        if( fsm->staCtx->pmk )
        {
            aniAsfPacketFree( fsm->staCtx->pmk );
            fsm->staCtx->pmk = NULL;
        }
       vos_mem_free(fsm->staCtx);
       fsm->staCtx = NULL; 
    }

    vos_mem_zero( fsm, sizeof(tAuthRsnFsm) );

    return ANI_OK;
}

/* 
                         
  
           
                                                                        
   
                                      
                                          
                                                 
  
                                           
 */
int
authRsnFsmProcessEvent(tAuthRsnFsm *fsm, tRsnFsmEvent eventId, void *arg)
{
    VOS_ASSERT(fsm);

    switch (eventId) {
    case RSN_FSM_TIMER_EXPIRED:
        //                                    
        break;
    case RSN_FSM_AUTH_START:
        fsm->authReq = eANI_BOOLEAN_TRUE;
        authRsnAuthStartEventHandler(fsm);
        break;
    case RSN_FSM_EAPOL_FRAME_AVAILABLE:
        fsm->eapolAvail = eANI_BOOLEAN_TRUE;
        break;
    case RSN_FSM_DISCONNECT:
        fsm->disconnect = eANI_BOOLEAN_TRUE;
        authRsnAuthDisconEventHandler(fsm);
        break;
    case RSN_FSM_INTEG_FAILED:
        fsm->integFailed = eANI_BOOLEAN_TRUE;
        break;
    default:
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR, 
            "Unknown event for Auth RSN Key Fsm: %d\n", eventId);
        VOS_ASSERT( 0 );
        return ANI_E_ILLEGAL_ARG;
        break;
    }

    checkTransition(fsm, arg);

    return ANI_OK;
}


int
authRsnAuthStartEventHandler(tAuthRsnFsm *fsm)
{
    static v_U8_t btampStaRSNIE[] = {0x30, 0x14, 0x01, 0x00, 0x00, 0x0f, 0xac, 0x04, 0x01, 0x00, 
        0x00, 0x0f, 0xac, 0x04, 0x01, 0x00, 0x00, 0x0f, 0xac, 0x02, 0x00, 0x00 };
    //                   
    vos_mem_copy( &fsm->staCtx->authMac, fsm->ctx->self_mac_addr, 6);
    vos_mem_copy( &fsm->staCtx->suppMac, fsm->ctx->peer_mac_addr, 6);
    aniAsfPacketAppendBuffer( fsm->staCtx->pmk, fsm->ctx->key_material, fsm->ctx->key_length);

    aniAsfPacketAppendBuffer( fsm->staCtx->ieSta, btampStaRSNIE, sizeof(btampStaRSNIE));
    return ANI_OK;
}

int
authRsnAuthDisconEventHandler(tAuthRsnFsm *fsm)
{
    //               
    return ANI_OK;
}

/*                      
                       
                       */

static int
gotoStateInit(tAuthRsnFsm *fsm)
{
    fsm->currentState = INITIALIZE;

    //                                                             
    //      
    //                                     

    fsm->authReq = eANI_BOOLEAN_FALSE;
    fsm->eapolAvail = eANI_BOOLEAN_FALSE;
    fsm->disconnect = eANI_BOOLEAN_FALSE;
    fsm->integFailed = eANI_BOOLEAN_FALSE;

    fsm->numTries = 0;

    //                                                               
    //                                                       
    aniSsmReplayCtrCreate(fsm->cryptHandle, &fsm->staCtx->localReplayCtr, 
                          ANI_EAPOL_KEY_RSN_RSC_SIZE, 0);
    aniSsmReplayCtrCreate(fsm->cryptHandle, &fsm->staCtx->peerReplayCtr, 
                          ANI_EAPOL_KEY_RSN_RSC_SIZE, 0);

    return ANI_OK;
}

static int
gotoStateAuthentication(tAuthRsnFsm *fsm)
{
    fsm->currentState = AUTHENTICATION;

    zeroOutPtk(fsm);
    fsm->authReq = eANI_BOOLEAN_FALSE;

    checkTransition(fsm, NULL); //         

    return ANI_OK;
}

static int
gotoStateAuthentication2(tAuthRsnFsm *fsm)
{
    fsm->currentState = AUTHENTICATION_2;

    if( !VOS_IS_STATUS_SUCCESS( vos_rand_get_bytes( fsm->cryptHandle, fsm->aNonce, ANI_EAPOL_KEY_RSN_NONCE_SIZE ) ) )
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
            "gotoStateAuthentication2 fail to get random number. Disconnect\n" );
        bapAuthDisconnect( fsm->ctx );
        return ANI_ERROR;
    }
    fsm->numTries = 0;

    checkTransition(fsm, NULL); //         

    return ANI_OK;
}


static int
gotoStateGetPsk(tAuthRsnFsm *fsm)
{
    //                                                                           
    fsm->currentState = GET_PSK;

    fsm->numTries = 0;
    
    checkTransition(fsm, NULL);
    
    return ANI_OK;
}

static int
gotoStatePtkStart(tAuthRsnFsm *fsm)
{
    tAniEapolRsnKeyDesc txDesc;
    int retVal;

    fsm->msg2TimeOut = VOS_FALSE;
    fsm->currentState = PTK_START;

    //                                                       
    //                                             
#if 0    
    if( fsm->lastEapol )
    {
        aniAsfPacketFree( fsm->lastEapol );
        fsm->lastEapol = NULL;

    retVal = aniAsfPacketAllocateExplicit(&fsm->lastEapol,
                                          RSN_MAX_PACKET_SIZE,
                                          EAPOL_TX_HEADER_SIZE );
#endif    
    aniAsfPacketEmptyExplicit(fsm->lastEapol, 
                              EAPOL_TX_HEADER_SIZE);
    // 
   //        
    // 

        vos_mem_zero( &txDesc, sizeof(txDesc) );

        //                            
        if (fsm->staCtx->pwCipherType == eCSR_ENCRYPT_TYPE_AES) 
        {
            txDesc.info.keyDescVers = ANI_EAPOL_KEY_DESC_VERS_AES;
        } 
        else {
            return ANI_E_ILLEGAL_ARG;
        }
        txDesc.info.unicastFlag = eANI_BOOLEAN_TRUE;
        txDesc.info.ackFlag = eANI_BOOLEAN_TRUE;

        //                    
        txDesc.keyLen = aagGetKeyMaterialLen(fsm->staCtx->pwCipherType);
        aniSsmReplayCtrNext(fsm->staCtx->localReplayCtr, txDesc.replayCounter);
        vos_mem_copy(txDesc.keyNonce, fsm->aNonce, sizeof(txDesc.keyNonce));

        retVal = aniEapolWriteKey(fsm->cryptHandle,
                                  fsm->lastEapol,
                                  fsm->staCtx->suppMac,
                                  fsm->staCtx->authMac,
                                  ANI_EAPOL_KEY_DESC_TYPE_RSN_NEW,
                                  &txDesc,
                                  NULL, 0);
        if( !ANI_IS_STATUS_SUCCESS( retVal ) )
        {
            return retVal;
        }
#if 0
    } 
    else {
        retransmit = eANI_BOOLEAN_TRUE;
    }
#endif    

    if( VOS_IS_STATUS_SUCCESS( bapRsnSendEapolFrame( fsm->ctx->pvosGCtx, fsm->lastEapol ) ) )
    {
        retVal = ANI_OK;
    }
    else
    {
        //                                          
        bapAuthDisconnect( fsm->ctx );
        retVal = ANI_ERROR;
    }

    return retVal;
}

static int
gotoStatePtkInitNego(tAuthRsnFsm *fsm, void *arg)
{
    fsm->currentState = PTK_INIT_NEGO;

    //                                                              
    //           

    fsm->numTries = 0;
    aniAsfPacketEmptyExplicit(fsm->lastEapol, 
                              EAPOL_TX_HEADER_SIZE);

    checkTransition(fsm, arg);

    return ANI_OK;
}

//                                                                
static int
getRsnIeFromAdvertizedIes(tAuthRsnFsm *fsm, v_U8_t **rsnIe)
{
    int retVal = ANI_E_ILLEGAL_ARG;
    v_U8_t *ptr = fsm->advertizedRsnIe;

    if (*ptr == ANI_SSM_IE_RSN_ELEM_ID) 
    {
        retVal = *(ptr + 1) + 2; //                                 
        *rsnIe = ptr;
    }

    return retVal;
}

//                                                                
static void
addPad( 
    v_U8_t *dataBytes,
    int dataLen,
    int padLen)
{
    int i;

    //                                                        
    //                                                              

    for ( i=dataLen ; i < dataLen+padLen; i++) 
    {
        if ( i == dataLen ) 
        {
            dataBytes[i] = 0xdd;
        } 
        else {
            dataBytes[i] = 0x00;
        }
    }

    return;
}

/* 
                          
  
                                                                          
  
                                          
                                                                  
  
                                           
 */
#define STATIC_WEP_KEY_LEN 16
#define GROUP_KEY_ID 0
#define ANI_SSM_IE_RSN_KEY_DATA_ENCAPS_ID       0xDD
#define ANI_SSM_IE_RSN_GROUP_KEY_DATA_ENCAPS_ID 1

int
aagAppendGroupKeyForRsn(tAniPacket *packet)
{
#if 0
    tAniPacket *groupKey = NULL;
#else
    tANI_U8 groupKey[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                          0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15};
#endif
    tANI_U8 *groupKeyBytes = NULL;
    tANI_U8 *lenPtr = NULL;
    tANI_U8 *endPtr = NULL;
    int groupKeyLen;
    int retVal;

#if 0
    groupKey = AAG_GROUP_KEY(radioId);
    if (groupKey == NULL) {
        ANI_AAG_LOG_E("Group key is not yet set on radio %d, id %d!",
                      radioId, AAG_GROUP_KEY_ID(radioId));
        assert(0 && "Group key is still NULL!");
        return ANI_E_FAILED;
    }

    groupKeyLen = aniAsfPacketGetBytes(groupKey, &groupKeyBytes);
    CHECK_NO_ERROR(groupKeyLen);

    if (aagConfig.logLevel >= LOG_INFO) {
        ANI_AAG_LOG_D("Will encapsulate group key bytes %s",
                      aniAsfHexStr(groupKeyBytes, groupKeyLen));
    }
#else
    groupKeyBytes = groupKey;

    groupKeyLen = STATIC_WEP_KEY_LEN;
#endif

    /*
                                                         
     */

    //          
    retVal = aniAsfPacketAppend8(packet, ANI_SSM_IE_RSN_KEY_DATA_ENCAPS_ID);
    //                       

    //                                   
    aniAsfPacketGetBytesFromTail(packet, &lenPtr);

    //                                                                 
    //                                        
    retVal = aniAsfPacketAppend8(packet, 0);
    //                       

    //                 
    retVal = aniAsfPacketAppendBuffer(packet, aniSsmIeRsnOui, sizeof(aniSsmIeRsnOui));
    //                       

    //                                        
    retVal = aniAsfPacketAppend8(packet, ANI_SSM_IE_RSN_GROUP_KEY_DATA_ENCAPS_ID);
    //                       
 
    //                                                       
    //                                               
    //                                                                    
    retVal = aniAsfPacketAppend8(
            packet,
            GROUP_KEY_ID );
            //                            
    //                       

    retVal = aniAsfPacketMoveRight(packet, 1); //                       
    //                       

    //                        
    retVal = aniAsfPacketAppendBuffer(packet, groupKeyBytes, groupKeyLen);
    //                       
    
    //                                                      
    aniAsfPacketGetBytesFromTail(packet, &endPtr);
    *lenPtr = endPtr - (lenPtr + 1) ; //                           

    return retVal;
}

static int
gotoStatePtkInitNegoTx(tAuthRsnFsm *fsm)
{
    tAniEapolRsnKeyDesc txDesc;
    v_BOOL_t retransmit = eANI_BOOLEAN_FALSE;
    v_U8_t *rsnWpaIe = NULL;
    int rsnWpaIeLen;
    static tAniPacket *keyData;
    //                                                                
    //              
    v_U8_t paddedGroupKeyEncaps[1024];
    int padLen = 0;
    v_U8_t *groupKeyBytes;
    int groupKeyLen;
    v_U8_t *wrappedKey = NULL;
    //                                
    //                                        
    //                 
    int retVal = 0;

    //               
    fsm->msg4TimeOut = VOS_FALSE;
    fsm->currentState = PTK_INIT_NEGO_TX ;

    if (keyData == NULL) 
    {
        //                                                         
        retVal = aniAsfPacketAllocateExplicit(&keyData, 1024, 10);
        if( !ANI_IS_STATUS_SUCCESS( retVal ) )
        {
            return retVal;
        }
    } 
    else {
        //                          
        aniAsfPacketEmptyExplicit(keyData, 10);
    }

    do
    {
        //                                                            
        //                                             
#if 0
        if( fsm->lastEapol )
         {
             aniAsfPacketFree( fsm->lastEapol );
             fsm->lastEapol = NULL;

             retVal = aniAsfPacketAllocateExplicit(&fsm->lastEapol,
                                          RSN_MAX_PACKET_SIZE,
                                          EAPOL_TX_HEADER_SIZE );
#endif     
             aniAsfPacketEmptyExplicit(fsm->lastEapol, 
                              EAPOL_TX_HEADER_SIZE);
      //   

        if (1) 
        {

            vos_mem_zero( &txDesc, sizeof(txDesc) );

            //                            
            if (fsm->staCtx->pwCipherType == eCSR_ENCRYPT_TYPE_AES) 
            {
                txDesc.info.keyDescVers = ANI_EAPOL_KEY_DESC_VERS_AES;
            } 
            else {
                txDesc.info.keyDescVers = ANI_EAPOL_KEY_DESC_VERS_RC4;
            }
            txDesc.info.unicastFlag = eANI_BOOLEAN_TRUE;
            txDesc.info.installFlag = eANI_BOOLEAN_TRUE;
            txDesc.info.ackFlag = eANI_BOOLEAN_TRUE;
            txDesc.info.micFlag = eANI_BOOLEAN_TRUE;

            txDesc.keyLen = aagGetKeyMaterialLen(fsm->staCtx->pwCipherType);
            aniSsmReplayCtrNext(fsm->staCtx->localReplayCtr, txDesc.replayCounter);
            vos_mem_copy(txDesc.keyNonce, fsm->aNonce, sizeof(txDesc.keyNonce));

            //                                    
            rsnWpaIeLen = getRsnIeFromAdvertizedIes(fsm, &rsnWpaIe);

            if( !ANI_IS_STATUS_SUCCESS( rsnWpaIeLen) ) break;

            retVal = aniAsfPacketAppendBuffer(keyData, rsnWpaIe, rsnWpaIeLen);
            if( !ANI_IS_STATUS_SUCCESS( retVal ) ) break;

            //                                    
            retVal = aagAppendGroupKeyForRsn ( keyData );

            if( !ANI_IS_STATUS_SUCCESS( retVal ) ) break;

            groupKeyLen = aniAsfPacketGetBytes(keyData, &groupKeyBytes);
            if( !ANI_IS_STATUS_SUCCESS( groupKeyLen ) ) 
            {
                retVal = ANI_E_FAILED;
                break;
            }

            txDesc.info.secureFlag = eANI_BOOLEAN_TRUE;
            txDesc.info.encKeyDataFlag = eANI_BOOLEAN_TRUE;

            if ( fsm->staCtx->pwCipherType == eCSR_ENCRYPT_TYPE_AES ) 
            {
                /*
                                                                               
                                                      
                  
                                                             
                                                                    
                                                                       
                         
                 */
                VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                           "AES Key Wrap invoked. groupKeyLen = %d", groupKeyLen);

                padLen = groupKeyLen % ANI_SSM_AES_KEY_WRAP_BLOCK_SIZE;
                if (padLen != 0) {
                    padLen = ANI_SSM_AES_KEY_WRAP_BLOCK_SIZE - padLen;
            
                    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                               "Before AES Key Wrap: padLen = %d", padLen);

                    if (groupKeyLen + padLen > sizeof(paddedGroupKeyEncaps)) {
#if 0
                        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                                   "Cannot encode group key encapsulation of len %d and cipher type %s "
                                   "to send to %s %s (aid %d, radio %d, user %s)",
                                   groupKeyLen,
                                   aniSsmIntGetCipherStr(AAG_GROUP_CIPHER(fsm->ctx->radioId)),
                                   (fsm->ctx->bpIndicator ? "BP" : "STA"),
                                   aniAsfHexStr(fsm->ctx->suppMac, sizeof(tAniMacAddr)),
                                   fsm->ctx->aid,
                                   fsm->ctx->radioId,
                                   aagGetStaUserId(fsm->ctx));
#endif
                         retVal = ANI_E_FAILED;
                    }
                    //                                                   
                    //                                           
                    addPad( groupKeyBytes , groupKeyLen , padLen );
                    //                       
                    groupKeyLen += padLen;
                    //                                                       
                    aniAsfPacketMoveRight(keyData, padLen); 
                    if( !ANI_IS_STATUS_SUCCESS( retVal) ) break;
                }
            
                VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                           "Before AES Key Wrap: padded groupKeyLen = %d", groupKeyLen);

                retVal = aniSsmAesKeyWrap(fsm->cryptHandle, groupKeyBytes, groupKeyLen,
                                          fsm->staCtx->ptk + ANI_EAPOL_KEY_RSN_MIC_SIZE,
                                          ANI_EAPOL_KEY_RSN_ENC_KEY_SIZE,
                                          &wrappedKey);
                if( !ANI_IS_STATUS_SUCCESS( retVal) ) break;
                //                     
                //                           
                //                                               
                //                                  
                //                                        
                if (NULL == wrappedKey)
                {
                    break;
                }
                groupKeyLen += ANI_SSM_AES_KEY_WRAP_BLOCK_SIZE;
                memcpy( groupKeyBytes, wrappedKey, groupKeyLen);
                //                                            
                if (wrappedKey) vos_mem_free( wrappedKey);
                //                                                      
                aniAsfPacketMoveRight(keyData, ANI_SSM_AES_KEY_WRAP_BLOCK_SIZE);
            } 
            else {
            
                VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                    "Auth RC4 Key Wrap invoked. groupKeyLen = %d", groupKeyLen);
            }
            txDesc.keyDataLen = aniAsfPacketGetBytes(keyData, &txDesc.keyData);

            retVal = aniEapolWriteKey(fsm->cryptHandle,
                                      fsm->lastEapol,
                                      fsm->staCtx->suppMac,
                                      fsm->staCtx->authMac,
                                      ANI_EAPOL_KEY_DESC_TYPE_RSN_NEW,
                                      &txDesc,
                                      fsm->staCtx->ptk,
                                      CSR_AES_KEY_LEN);
            if( !ANI_IS_STATUS_SUCCESS( retVal) ) break;
        } 
        else {
            retransmit = eANI_BOOLEAN_TRUE;
        }

        if( VOS_IS_STATUS_SUCCESS( bapRsnSendEapolFrame( fsm->ctx->pvosGCtx, fsm->lastEapol ) ) )
        {
            retVal = ANI_OK;
        }
        else
        {
            //                                          
            bapAuthDisconnect( fsm->ctx );
            retVal = ANI_ERROR;
        }

    }while( 0 );

    return retVal;
}

static int
gotoStatePtkInitDone(tAuthRsnFsm *fsm, tAniEapolKeyAvailEventData *data)
{
    int retVal;
    tAniEapolRsnKeyDesc *rxDesc;
    tCsrRoamSetKey setKeyInfo;

    fsm->currentState = PTK_INIT_DONE;

    rxDesc = data->keyDesc;

    vos_mem_zero( &setKeyInfo, sizeof( tCsrRoamSetKey ) );
    setKeyInfo.encType = eCSR_ENCRYPT_TYPE_AES;
    setKeyInfo.keyDirection = eSIR_TX_RX;
    vos_mem_copy( setKeyInfo.peerMac, fsm->staCtx->suppMac, sizeof( tAniMacAddr ) );
    setKeyInfo.paeRole = 0; //                    
    setKeyInfo.keyId = 0;   //      
    setKeyInfo.keyLength = CSR_AES_KEY_LEN; 
    vos_mem_copy( setKeyInfo.Key, (v_U8_t *)fsm->staCtx->ptk + (2 * CSR_AES_KEY_LEN ), CSR_AES_KEY_LEN );
    //                                                                     
    if( VOS_IS_STATUS_SUCCESS( bapSetKey( fsm->ctx->pvosGCtx, &setKeyInfo ) ) )
    {
        //    
        aniAsfPacketEmptyExplicit(fsm->lastEapol, EAPOL_TX_HEADER_SIZE);
        retVal = ANI_OK;
    }
    else
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR, "Auth: gotoStatePtkInitDone fail to set key\n" );
        retVal = ANI_ERROR;
    }

    return retVal;
}

static int
gotoStateUpdateKeysReq(tAuthRsnFsm *fsm, tAniEapolKeyAvailEventData *data)
{
    tAniEapolRsnKeyDesc *rxDesc;

    fsm->currentState = UPDATE_KEYS_REQ;

    rxDesc = data->keyDesc;

    aniSsmReplayCtrUpdate(fsm->staCtx->peerReplayCtr, rxDesc->replayCounter);

    checkTransition(fsm, data);

    return ANI_OK;
}

static int
gotoStateIntegFailure(tAuthRsnFsm *fsm, tSirMicFailureInfo *micFailureInfo)
{
    fsm->currentState = INTEG_FAILURE;

    fsm->integFailed = eANI_BOOLEAN_FALSE;

    checkTransition(fsm, NULL); //    

    return ANI_OK;
}

static int
gotoStateKeyUpdate(tAuthRsnFsm *fsm)
{
    fsm->currentState = KEY_UPDATE;

    if( VOS_IS_STATUS_SUCCESS( vos_rand_get_bytes(fsm->cryptHandle, fsm->aNonce, ANI_EAPOL_KEY_RSN_NONCE_SIZE) ) )
    {

        //                                                              
        //           
        
        checkTransition(fsm, NULL); //    

        return ANI_OK;
    }
    return ANI_ERROR;
}

static int
gotoStateDisconnect(tAuthRsnFsm *fsm)
{
    fsm->currentState = DISCONNECT;

    //                                                                        

    //                                 
    bapAuthDisconnect( fsm->ctx );

    return ANI_OK;
}

static 
int zeroOutPtk(tAuthRsnFsm *fsm)
{
    return ANI_OK;
}

static 
int stopAllTimers(tAuthRsnFsm *fsm)
{
    vos_timer_stop( &fsm->msg2Timer );
    vos_timer_stop( &fsm->msg4Timer );

    return ANI_OK;
}

static
int derivePtk(tAuthRsnFsm *fsm, tAniEapolKeyAvailEventData *data)
{
    v_U32_t prfLen;
    tAniEapolRsnKeyDesc *rxDesc;

    VOS_ASSERT(fsm->staCtx->pmk);

    switch (fsm->staCtx->pwCipherType) 
    {
    case eCSR_ENCRYPT_TYPE_AES:
        prfLen = AAG_RSN_PTK_PRF_LEN_CCMP;
        break;
    default:
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
            "Auth cannot generate PTK for invalid algorithm %d\n",
                      fsm->staCtx->pwCipherType);
        return ANI_E_ILLEGAL_ARG;
        break;
    };

    rxDesc = (tAniEapolRsnKeyDesc *) data->keyDesc;
    return aagPtkPrf(fsm->cryptHandle,
                     fsm->staCtx->ptk,
                     prfLen,
                     fsm->staCtx->pmk,
                     fsm->staCtx->authMac,
                     fsm->staCtx->suppMac,
                     fsm->aNonce,
                     rxDesc->keyNonce);
}

static int 
checkMic(tAuthRsnFsm *fsm, 
         tAniEapolKeyAvailEventData *data)
{
    int retVal;

    retVal = aniEapolKeyCheckMic(fsm->cryptHandle,
                                 data->eapolFrame,
                                 ANI_EAPOL_KEY_DESC_TYPE_RSN_NEW,
                                 data->keyDesc,
                                 fsm->staCtx->ptk,
                                 CSR_AES_KEY_LEN);

    if (retVal == ANI_E_MIC_FAILED) 
    {

        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                     "Auth failed EAPOL-MIC check in pairwise key exchange!\n"); 
    }

    return retVal;
}

static int 
checkLocalReplayCounter(tAuthRsnFsm *fsm, 
                        tAniEapolKeyAvailEventData *data)
{
    int retVal = ANI_E_NULL_VALUE;
    int cmp;
    tAniEapolRsnKeyDesc *rxDesc;
    
    rxDesc = data->keyDesc;
    if( rxDesc )
    {
        cmp = aniSsmReplayCtrCmp(fsm->staCtx->localReplayCtr, rxDesc->replayCounter);

        //                                                                    
        if (cmp != 0) 
        {
            retVal = ANI_E_REPLAY_CHECK_FAILED;
        }
        else
        {
            retVal = ANI_OK;
        }
    }

    return retVal;
}

static 
int checkPeerReplayCounter(tAuthRsnFsm *fsm, 
                           tAniEapolKeyAvailEventData *data)
{
    int retVal = ANI_E_NULL_VALUE;
    int cmp;
    tAniEapolRsnKeyDesc *rxDesc;
    
    rxDesc = data->keyDesc;
    if( rxDesc )
    {
        cmp = aniSsmReplayCtrCmp(fsm->staCtx->peerReplayCtr, rxDesc->replayCounter);

        //                                                         
        //                                                       
        if (fsm->staCtx->pastFirstPeerRequest && cmp >= 0) 
        {
            retVal = ANI_E_REPLAY_CHECK_FAILED;
        }

        fsm->staCtx->pastFirstPeerRequest = eANI_BOOLEAN_TRUE;
    }

    return retVal;
}

static int checkInfoElement(tAuthRsnFsm *fsm,
                            tAniEapolKeyAvailEventData *data)
{
    tAniEapolRsnKeyDesc *desc;
    v_U8_t *ieStaBytes;
    int ieStaLen;

    desc = (tAniEapolRsnKeyDesc *) data->keyDesc;
    if( desc )
    {
        ieStaLen = aniAsfPacketGetBytes(fsm->staCtx->ieSta, &ieStaBytes);
        if( !ANI_IS_STATUS_SUCCESS( ieStaLen ) )
        {
            return ieStaLen;
        }

        if ((desc->keyDataLen != ieStaLen) ||
            ( !vos_mem_compare(desc->keyData, ieStaBytes, ieStaLen-2) )) 
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                         "Auth STA sent inconsistent RSN IE!\n");
            return ANI_E_FAILED;
        }
        //            
        //                                                            
        vos_mem_copy(fsm->advertizedRsnIe, ieStaBytes, ieStaLen);

        return ANI_OK;
    }
    else
    {
        return ANI_E_NULL_VALUE;
    }

}

static 
int checkTransition(tAuthRsnFsm *fsm, void *arg)
{
    int retVal;
    tAniEapolKeyAvailEventData *data;
    tAniEapolRsnKeyDesc *rxDesc;
    tSirMicFailureInfo *micFailureInfo;

    if (fsm->disconnect) 
    {
        stopAllTimers(fsm);
        gotoStateDisconnect(fsm);
        return ANI_OK;
    }

    if (fsm->authReq) 
    {
        stopAllTimers(fsm);
        gotoStateAuthentication(fsm);
        return ANI_OK;
    }

    switch (fsm->currentState) 
    {
    case INITIALIZE:
        break;
    case AUTHENTICATION:
        gotoStateAuthentication2(fsm);
        break;
    case AUTHENTICATION_2:
        gotoStateGetPsk( fsm );
        break;
    case GET_PSK:
        //                                                  
        gotoStatePtkStart(fsm);
        break;
    case PTK_START:
        if ( fsm->eapolAvail ) 
        {
            fsm->eapolAvail = eANI_BOOLEAN_FALSE;
            if (NULL == arg)
            {
                VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                           "arg is NULL, exiting checkTransition()");
                return ANI_E_FAILED;
            }
            data = (tAniEapolKeyAvailEventData *) arg;
            retVal = checkLocalReplayCounter(fsm, data);
            if (retVal != ANI_OK)
                return ANI_OK; //                       
            retVal = derivePtk(fsm, data);
            if( !ANI_IS_STATUS_SUCCESS( retVal ) )
            {
                VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                         "Auth derivePtk failed with code %d!\n", retVal);
                return retVal;
            }
            retVal = checkMic(fsm, data);
            if (retVal != ANI_OK) 
            {
                bapAuthDisconnect( fsm->ctx );
                return retVal;
            }
            retVal = gotoStatePtkInitNego(fsm, arg);
        } 
        else if ( fsm->msg2TimeOut ) 
        {
            if (fsm->numTries <= authConsts.maxTries) 
            {
                VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                             "Auth Retransmitting EAPOL-Key Msg1\n");
                //                                          
                gotoStatePtkStart(fsm);
            } 
            else {
                VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                             "Auth failed to recv EAPOL-Key Msg2 "
                             "Disconnecting...\n");

                gotoStateDisconnect(fsm);
            }
        }
        break;
    case PTK_INIT_NEGO:
        if (NULL == arg)
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                       "arg is NULL, exiting checkTransition()");
            return ANI_E_FAILED;
        }
        data = (tAniEapolKeyAvailEventData *) arg;
        retVal = checkInfoElement(fsm, data);
        if (retVal != ANI_OK) 
        {
            gotoStateDisconnect(fsm);
        } 
        else {
            gotoStatePtkInitNegoTx(fsm);
        }
        break;
    case PTK_INIT_NEGO_TX:
        if (fsm->eapolAvail) 
        {
            fsm->eapolAvail = eANI_BOOLEAN_FALSE;
            if (NULL == arg)
            {
                VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                           "arg is NULL, exiting checkTransition()");
                return ANI_E_FAILED;
            }
            data = (tAniEapolKeyAvailEventData *) arg;
            retVal = checkLocalReplayCounter(fsm, data);
            if (retVal != ANI_OK)
                return ANI_OK; //                       
            retVal = checkMic(fsm, data);
            if (retVal != ANI_OK) 
            {
                bapAuthDisconnect( fsm->ctx );
                return retVal;
            }
            retVal = gotoStatePtkInitDone(fsm, data);
        } else if ( fsm->msg4TimeOut ) 
        {
            if (fsm->numTries <= authConsts.maxTries) 
            {
                VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                             "Auth retransmitting EAPOL-Key Msg3 \n");
                //                                          
                gotoStatePtkInitNegoTx(fsm);
            } 
            else {
                VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                             "Auth failed to recv EAPOL-Key Msg4 "
                             "Disconnecting...\n" );

                gotoStateDisconnect(fsm);
            }
        }
        break;
    case PTK_INIT_DONE:
        if (fsm->eapolAvail) {

            fsm->eapolAvail = eANI_BOOLEAN_FALSE;
            if (NULL == arg)
            {
                VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                           "arg is NULL, exiting checkTransition()");
                return ANI_E_FAILED;
            }
            data = (tAniEapolKeyAvailEventData *) arg;
            rxDesc = (tAniEapolRsnKeyDesc *) data->keyDesc;

            if (rxDesc->info.requestFlag) 
            {

                retVal = checkPeerReplayCounter(fsm, data);
                if (retVal != ANI_OK)
                    return ANI_OK; //                       

                retVal = checkMic(fsm, data);
                if (retVal != ANI_OK) 
                {
                    bapAuthDisconnect( fsm->ctx->pvosGCtx );
                    return retVal;
                }

                retVal = gotoStateUpdateKeysReq(fsm, arg);
            }
        } 
        else if (fsm->integFailed) {

            micFailureInfo = (tSirMicFailureInfo *) arg;
            gotoStateIntegFailure(fsm, arg);

        }
        break;
    case UPDATE_KEYS_REQ:

        if (NULL == arg)
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                       "arg is NULL, exiting checkTransition()");
            return ANI_E_FAILED;
        }
        data = (tAniEapolKeyAvailEventData *) arg;
        rxDesc = (tAniEapolRsnKeyDesc *) data->keyDesc;

        if (rxDesc->info.errorFlag) 
        {

            /*
                                                                                     
                                                                           
                                                                           
                                                                               
             */

            micFailureInfo = (tSirMicFailureInfo *) vos_mem_malloc( sizeof(tSirMicFailureInfo) );
            if( NULL == micFailureInfo )
            {
                VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                    "Fail to allocate memory for AuthRsnFsm: %d\n",
                      fsm->currentState);
                return ANI_E_MALLOC_FAILED;
            }

            vos_mem_copy(micFailureInfo->taMacAddr, fsm->staCtx->authMac, sizeof(tAniMacAddr));
            vos_mem_copy(micFailureInfo->dstMacAddr, fsm->staCtx->suppMac, sizeof(tAniMacAddr));
            micFailureInfo->multicast = eANI_BOOLEAN_FALSE;
            //                                                            
            vos_mem_copy(micFailureInfo->TSC, rxDesc->keyRecvSeqCounter, SIR_CIPHER_SEQ_CTR_SIZE);
            gotoStateIntegFailure(fsm, micFailureInfo);
            vos_mem_free(micFailureInfo);
        } 
        else {
            //                                                                                
            gotoStatePtkStart(fsm);
        }
        break;
    case INTEG_FAILURE:
        gotoStateKeyUpdate(fsm);
        break;
    case KEY_UPDATE:
        gotoStatePtkStart(fsm);
        break;
    default:
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
            "Nothing to do in this state for AuthRsnFsm: %d\n",
                      fsm->currentState);
        //                                          
        //                                                                  
        return ANI_E_FAILED;
    }

    return ANI_OK;
}


static void msg2TimerCallback( void *pv )
{
    tAuthRsnFsm *fsm = (tAuthRsnFsm *)pv;
    if (NULL == fsm) 
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                     "fsm is NULL in %s", __func__);

        return;
    }

    //                          
    if( PTK_START == fsm->currentState )
    {
        fsm->msg2TimeOut = eANI_BOOLEAN_TRUE;
    }
    //                                    
    authRsnFsmProcessEvent( fsm, RSN_FSM_TIMER_EXPIRED, NULL );
}

static void msg4TimerCallback( void *pv )
{
    tAuthRsnFsm *fsm = (tAuthRsnFsm *)pv;
    if (NULL == fsm) 
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                     "fsm is NULL in %s", __func__);

        return;
    }

    //                                 
    if( PTK_INIT_NEGO_TX == fsm->currentState )
    {
        fsm->msg4TimeOut = eANI_BOOLEAN_TRUE;
    }
    //                                    
    authRsnFsmProcessEvent( fsm, RSN_FSM_TIMER_EXPIRED, NULL );
}


//
//                                                                   
static int authRsnRxFrameHandler( v_PVOID_t pvosGCtx, vos_pkt_t *pPacket )
{
    int retVal = ANI_ERROR;
    tAniPacket *pAniPacket;
    tBtampContext *ctx;
    tAuthRsnFsm *fsm;

    /*                 */ 
    if ((pvosGCtx == NULL) || (NULL == pPacket))
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                     "param is NULL in %s", __func__);

        return retVal;
    }

    ctx = (tBtampContext *)VOS_GET_BAP_CB( pvosGCtx );
    if (NULL == ctx) 
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                     "ctx is NULL in %s", __func__);

        return retVal;
    }

    fsm = &ctx->uFsm.authFsm;
    if (NULL == fsm) 
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                     "fsm is NULL in %s", __func__);

        return retVal;
    }

    do
    {
        //                                                                                               
        if( fsm->fReceiving )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                " ******authRsnRxFrameHandler receive eapol packet while processing. Drop the new comer\n" );
            break;
        }
        fsm->fReceiving = VOS_TRUE;
        retVal = bapRsnFormPktFromVosPkt( &pAniPacket, pPacket );
        if( !ANI_IS_STATUS_SUCCESS( retVal ) ) break;
        //                                  
        //                                
        bapRsnEapolHandler( fsm, pAniPacket, VOS_TRUE );
    }while( 0 );

    fsm->fReceiving = VOS_FALSE;
    vos_pkt_return_packet( pPacket );

    return retVal;
}


static int authRsnTxCompleteHandler( v_PVOID_t pvosGCtx, vos_pkt_t *pPacket, VOS_STATUS retStatus )
{
    tBtampContext *ctx = (tBtampContext *)VOS_GET_BAP_CB( pvosGCtx );
    tAuthRsnFsm *fsm;

    vos_pkt_return_packet( pPacket );
    if (NULL == ctx) 
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                     "ctx is NULL in %s", __func__);

        return ANI_ERROR;
    }

    fsm = &ctx->uFsm.authFsm;
    if (NULL == fsm) 
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                     "fsm is NULL in %s", __func__);

        return ANI_ERROR;
    }

    if(!VOS_IS_STATUS_SUCCESS( retStatus ) )
    {
        //                                                        
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
            "Auth: TL Tx complete with error %d current state is %d \n", retStatus, fsm->currentState );
    }
    if( PTK_START == fsm->currentState )
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO,
            " Auth: start msg2 timer\n" );
        //               
        fsm->numTries++;
        vos_timer_stop( &fsm->msg2Timer );
        vos_timer_start(&fsm->msg2Timer, authConsts.timeoutPeriod);
    }
    else if( ( PTK_INIT_NEGO == fsm->currentState ) || 
        ( PTK_INIT_NEGO_TX == fsm->currentState ) )
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO,
            " Auth: start msg4 timer\n" );
        fsm->numTries++;
        vos_timer_stop( &fsm->msg4Timer );
        vos_timer_start(&fsm->msg4Timer, authConsts.timeoutPeriod);
    }

    return ANI_OK;
}


static int
authEapolKeyHandler( tAuthRsnFsm *fsm, tAniPacket *eapolFrame, tAniMacAddr staMac )
{
    int retVal;

    int descType;
    void *keyDesc;
    tAniEapolRsnKeyDesc *rsnDesc;
    tAniEapolKeyAvailEventData data;

    do
    {
        retVal = aniEapolParseKey(eapolFrame, &descType, &keyDesc);
        if( !ANI_IS_STATUS_SUCCESS( retVal ) ) break;

        if ((descType == ANI_EAPOL_KEY_DESC_TYPE_RSN_NEW) 
                || (descType == ANI_EAPOL_KEY_DESC_TYPE_RSN)) 
        {
            rsnDesc = (tAniEapolRsnKeyDesc *) keyDesc;
            data.keyDesc = keyDesc;
            data.eapolFrame = eapolFrame;
            //                                                                  
            if (rsnDesc->info.unicastFlag) 
            {
                retVal = authRsnFsmProcessEvent(fsm, 
                                                RSN_FSM_EAPOL_FRAME_AVAILABLE,
                                                &data);
            } 
            else {
                //                         
            }
        } 
        else {

            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                    "Got unexpected legacy 802.1x RC4 Key message \n" );
            retVal = ANI_E_FAILED;
            break;
        }
    }while( 0 );

    aniEapolKeyFreeDesc(descType, keyDesc);
    return retVal;
}


void authEapolHandler( tAuthRsnFsm *fsm, tAniPacket *eapolFrame,
                 tAniMacAddr dstMac, 
                 tAniMacAddr srcMac,
                 v_U8_t *type)
{
    switch (*type) 
    {
    case ANI_EAPOL_TYPE_START:
        //                                                  
        break;
    case ANI_EAPOL_TYPE_LOGOFF:
        //      
        break;
    case ANI_EAPOL_TYPE_KEY:
        authEapolKeyHandler(fsm, eapolFrame, srcMac);
        break;
    default:
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
            "Auth: EAPOL type not implemented: 0x%.2x\n", *type);
        break;
    }
}

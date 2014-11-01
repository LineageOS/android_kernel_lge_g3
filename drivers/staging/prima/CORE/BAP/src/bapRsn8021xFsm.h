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
#ifndef __AAG_FSM_H_
#define __AAG_FSM_H_

#include "vos_types.h"
#include "vos_trace.h"
#include "vos_timer.h"
#include <bapRsnSsmReplayCtr.h>
#include <bapRsnAsfPacket.h>
#include <bapRsnSsmEapol.h>
#include "bapRsn8021xPrf.h"
//                        
#include "csrApi.h"

typedef struct sBtampContext tBtampContext;
typedef struct tStaContext tStaContext;
typedef struct tSuppContext tSuppContext;

#define RSN_MAX_PACKET_SIZE 512
#define RSN_80211_KEY_LEN    16
#define RSN_IE_MAX_PACKET_SIZE 256
#define RSN_IE_HEADER_SIZE 0
#define ACCTG_SESSION_ID_SIZE 8
#define ANI_SSM_AES_KEY_WRAP_BLOCK_SIZE 8 //      

#define  BAP_SET_RSN_KEY   1
#define  BAP_RESET_RSN_KEY  0 


#define AAG_ACL_LOOKUP_NEEDED(ctx) \
            ((ctx)->authType == eANI_SSM_AT_NONE || \
             (ctx)->authType == eANI_SSM_AT_RSN_PSK || \
             (ctx)->authType == eANI_SSM_AT_SHARED_KEY)

#define AAG_ACL_LOOKUP_PENDING(ctx) \
            ((ctx)->aclLookupFsm != NULL && (ctx)->radiusInfo.req != NULL)

#define AAG_STA_AWAITING_CLEANUP(ctx) \
            ((ctx)->ssid == NULL)

#define AAG_MARK_STA_AS_AWAITING_CLEANUP(ctx) \
            ((ctx)->ssid = NULL)

/*                       
                        
                         */
typedef struct tagAuthRsnFsm 
{
    v_U8_t currentState;
    
    tBtampContext *ctx;
    tStaContext *staCtx; 

    //                                      
    v_U8_t aNonce[ANI_EAPOL_KEY_RSN_NONCE_SIZE];
    v_U8_t sNonce[ANI_EAPOL_KEY_RSN_NONCE_SIZE];
 
    //                             
    v_U8_t authReq;
    v_U8_t eapolAvail;
    v_U8_t disconnect;
    v_U8_t integFailed;
    v_U8_t pmkAvailable;

    //                                
    v_U8_t numTries;
    tAniPacket *lastEapol;  //  
    v_BOOL_t fReceiving;
    v_U32_t cryptHandle;

    //                                                
    vos_timer_t msg2Timer;
    vos_timer_t msg4Timer;
    v_BOOL_t msg2TimeOut;
    v_BOOL_t msg4TimeOut;
    v_U8_t advertizedRsnIe[256];
} tAuthRsnFsm;

/*                       
                        
                         */

typedef struct tagSuppRsnFsm {

    v_U8_t currentState;
    
    tBtampContext *ctx;
    tSuppContext *suppCtx;

    //                                      
    tAniSsmReplayCtr *localReplayCtr;
    tAniSsmReplayCtr *peerReplayCtr;
    v_U8_t aNonce[ANI_EAPOL_KEY_RSN_NONCE_SIZE];
    v_U8_t sNonce[ANI_EAPOL_KEY_RSN_NONCE_SIZE];

    //                             
    v_U8_t authReq;
    v_U8_t pmkAvail;
    v_U8_t eapolAvail;
    v_U8_t integFailed;
    v_U8_t updateKeys;

    //                                
    int numTries;
    tAniPacket *lastEapol;
    v_BOOL_t fReceiving;
    v_U32_t cryptHandle;
} tSuppRsnFsm;


typedef struct sAagZoneEntry tAagZoneEntry;
typedef struct sAagSsidEntry tAagSsidEntry;

typedef enum
{
    //               
    //                                       
    RSN_FSM_TIMER_EXPIRED,  
    //                                                          
    //                                                                
    //                          
    RSN_FSM_AUTH_START,    
    //               
    //                                                           
    RSN_FSM_EAPOL_FRAME_AVAILABLE,  
    //                                                              
    RSN_FSM_DISCONNECT,    
    //               
    //                                                             
    RSN_FSM_INTEG_FAILED,  

}tRsnFsmEvent;

/* 
                                                                    
                                      
  
                                                                
                                                                
                                                                    
                                                                  
                                                               
                                                                    
                                                                    
  
                                                                     
                                                                      
                                                                   
                                                                     
                                                                     
                    
                                                                   
                                                                
                                                                 
              
 */
typedef struct tEapInfo 

{
    tAniPacket *message;
    v_U8_t id;
} tEapInfo;


typedef enum eAniSsmAuthState {
    eANI_SSM_AUTH_STATE_INIT = 0,
    eANI_SSM_AS_PW_KEY_CONF_AWAITED,
    eANI_SSM_AS_PW_KEY_SET,
} tAniSsmAuthState;


/* 
                                                                 
                                                                     
                                                               
                                                                     
           
 */
struct tStaContext {

    //                               
    tAniMacAddr suppMac;
    v_BOOL_t bpIndicator;

    //                        
    tAniMacAddr authMac;
    v_U8_t ssidName[SIR_MAC_MAX_SSID_LENGTH + 1];    
    tAagSsidEntry *ssid;

    //                                                         
    tAuthRsnFsm *authRsnFsm;

    //                     
    v_U8_t ptk[AAG_PRF_MAX_OUTPUT_SIZE];
    tAniPacket *pmk; //                 
    tAniPacket *serverKey; //                 
    v_U8_t keyId;

    //                   
    v_U32_t  sessionTimeout;
    vos_timer_t reAuthTimer;
    vos_timer_t sessionCleanupTimer;

    //                                 
    v_U8_t *authClassAttr;

    //                                   
    eCsrAuthType authType;
    eCsrEncryptionType pwCipherType;
    tAniPacket *ieSta;
    tAniSsmAuthState authState;
    v_BOOL_t prmLookupInProgress;
    v_BOOL_t firstTimeAuth;
    v_BOOL_t secureLink; //                                                              
    tAniSsmReplayCtr *localReplayCtr;
    tAniSsmReplayCtr *peerReplayCtr; //                                  
    v_BOOL_t pastFirstPeerRequest; //                                 

    tAniPacket *cachedPmk; //                 
    tAniPacket *cachedServerKey; //                 
    v_U8_t cachedKeyId;

};


struct tSuppContext {

    //                                     
    tAniMacAddr authMac;
    v_U8_t *ssidStr;

    //                        
    tAniMacAddr suppMac;

    //                           
    v_U8_t ptk[AAG_PRF_MAX_OUTPUT_SIZE];
    v_U8_t pwKeyLen; //                                 
    tAniPacket *pmk; //                 
    tAniPacket *serverKey; //                 

    //                                   
    eCsrAuthType authType;
    eCsrEncryptionType pwCipherType;
    eCsrEncryptionType grpCipherType;
    tAniPacket *ieBp;
    tAniPacket *ieAp;
    v_BOOL_t firstTimeAuth;

};

typedef struct tAniEapolKeyAvailEventData {
    void *keyDesc;
    tAniPacket *eapolFrame;
} tAniEapolKeyAvailEventData;

typedef struct tAniAagTimerEventData {
    vos_timer_t timer;
    void *appData;
} tAniAagTimerEventData;


/* 
                                                                  
                                           
 */
#if 0
typedef int (*tAagSetStatus)(tStaContext *ctx);

typedef int (*tAagTxEapolSupp)(tSuppContext *ctx);
typedef int (*tAagSetStatusSupp)(tSuppContext *ctx);
typedef int (*tAagSendEventToFsmSupp)(tSuppContext *ctx);
#endif

/* 
                                                                      
                                                                    
 */
typedef void (*tAagTimerCallback)(void *data);

/* 
                                                                    
                                                             
                                                                      
               
 */
/*                                      
                                               
                                              
                                         
                        */

/* 
                                                                   
            
 */
typedef struct tAuthFsmConsts {
    //                                           
    v_U16_t quietPeriod;
    //                                                                
    v_U8_t reAuthMax;
    //                                             
    v_U16_t txPeriod;
} tAuthFsmConsts;



/* 
                                                                       
             
 */
typedef struct tAuthRsnFsmConsts {
    v_U32_t timeoutPeriod;
    v_U32_t maxTries;
} tAuthRsnFsmConsts;


/* 
                                                                   
            
 */
typedef struct tSuppFsmConsts {
    v_U16_t authPeriod;
    v_U16_t heldPeriod;
    v_U16_t startPeriod;
    v_U8_t maxStart;    
} tSuppFsmConsts;

/* 
                                                                       
             
 */
typedef struct tSuppRsnFsmConsts {
    v_U32_t timeoutPeriod;
    v_U32_t maxTries;
} tSuppRsnFsmConsts;


/* 
                                                                    
                        
 */
typedef struct tAuthRsnGroupKeyFsmConsts {
    v_U32_t timeoutPeriod;
    v_U32_t maxTries;
} tAuthRsnGroupKeyFsmConsts;

/* 
                 
  
           
                                                                    
                                                           
   
                                                               
  
                                           
 */
int
authRsnFsmFree(tBtampContext *ctx);

/* 
                         
  
           
                                                                        
   
                                      
                                          
                                                 
  
                                           
 */
int
authRsnFsmProcessEvent(tAuthRsnFsm *fsm, tRsnFsmEvent eventId, void *arg);


/* 
                
  
           
                                                                     
                     
   
                                                                  
  
                                           
 */
int
suppRsnFsmCreate(tBtampContext *ctx);

/* 
              
  
           
                                        
   
                                                                     
  
                                           
 */
int
suppRsnFsmFree(tBtampContext *ctx);

/* 
                      
  
           
                                                           
   
                                                                   
  
                             
                                          
  
                                           
 */
int
suppRsnFsmProcessEvent(tSuppRsnFsm *fsm, tRsnFsmEvent eventId, void *arg);

#endif //             

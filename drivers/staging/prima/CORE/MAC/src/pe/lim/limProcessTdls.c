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
/*===========================================================================

                      limProcessTdls.c 

  OVERVIEW:

  DEPENDENCIES:

  Are listed for each API below.

  Copyright (c) 2010 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary

===========================================================================*/

/*                                                                           

                                           


                                                                       
                                                                


                             


                                      
                                                                             
                                                                                
                                                              

                                                                           */


/* 
                         
  
                                                                         
  
 */

#ifdef FEATURE_WLAN_TDLS

#include "sirApi.h"
#include "aniGlobal.h"
#include "sirMacProtDef.h"
#include "cfgApi.h"
#include "utilsApi.h"
#include "limTypes.h"
#include "limUtils.h"
#include "limSecurityUtils.h"
#include "dot11f.h"
#include "limStaHashApi.h"
#include "schApi.h"
#include "limSendMessages.h"
#include "utilsParser.h"
#include "limAssocUtils.h"
#include "dphHashTable.h"
#include "wlan_qct_wda.h"

/*                                                               
                                                            
                                                                 
                                           
                                                                     
*/
#ifndef NO_PAD_TDLS_MIN_8023_SIZE
#define MIN_IEEE_8023_SIZE              46
#define MIN_VENDOR_SPECIFIC_IE_SIZE     5
#endif
#ifdef WLAN_FEATURE_TDLS_DEBUG
#define TDLS_DEBUG_LOG_LEVEL VOS_TRACE_LEVEL_ERROR
#else
#define TDLS_DEBUG_LOG_LEVEL VOS_TRACE_LEVEL_INFO
#endif

#ifdef FEATURE_WLAN_TDLS_INTERNAL
/*                      */
static tSirRetStatus limTdlsDisAddSta(tpAniSirGlobal pMac, tSirMacAddr peerMac,
                   tSirTdlsPeerInfo *peerInfo, tpPESession psessionEntry) ;
static eHalStatus limSendSmeTdlsLinkSetupInd(tpAniSirGlobal pMac, 
                                   tSirMacAddr peerMac, tANI_U8 status);
static eHalStatus limSendSmeTdlsDelPeerInd(tpAniSirGlobal pMac, 
                 tANI_U8 sessionId, tDphHashNode   *pStaDs, tANI_U8 status) ;
static tSirTdlsPeerInfo *limTdlsFindDisPeerByState(tpAniSirGlobal pMac, 
                                                            tANI_U8 state);
static tANI_U8 limTdlsFindSetupPeerByState(tpAniSirGlobal pMac, tANI_U8 state, 
                                     tLimTdlsLinkSetupPeer **setupPeer) ;
static tSirRetStatus limTdlsLinkEstablish(tpAniSirGlobal pMac, tSirMacAddr peer_mac);

static tSirRetStatus limTdlsLinkTeardown(tpAniSirGlobal pMac, tSirMacAddr peer_mac);
static tpDphHashNode limTdlsDelSta(tpAniSirGlobal pMac, tSirMacAddr peerMac, 
                                                 tpPESession psessionEntry) ;

#endif
static tSirRetStatus limTdlsSetupAddSta(tpAniSirGlobal pMac,
                                        tSirTdlsAddStaReq *pAddStaReq,
                                        tpPESession psessionEntry) ;
void PopulateDot11fLinkIden(tpAniSirGlobal pMac, tpPESession psessionEntry,
                          tDot11fIELinkIdentifier *linkIden, 
                             tSirMacAddr peerMac, tANI_U8 reqType) ;
void PopulateDot11fTdlsExtCapability(tpAniSirGlobal pMac, 
                                    tDot11fIEExtCap *extCapability) ;

void limLogVHTCap(tpAniSirGlobal pMac,
                              tDot11fIEVHTCaps *pDot11f);
tSirRetStatus limPopulateVhtMcsSet(tpAniSirGlobal pMac,
                                  tpSirSupportedRates pRates,
                                  tDot11fIEVHTCaps *pPeerVHTCaps,
                                  tpPESession psessionEntry);
ePhyChanBondState  limGetHTCBState(ePhyChanBondState aniCBMode);
/*
                                                        
                                               
 */
static const tANI_U8 eth_890d_header[] = 
{ 
    0xaa, 0xaa, 0x03, 0x00, 
    0x00, 0x00, 0x89, 0x0d,
} ;

/*
                              
 */
enum tdlsLinks
{
    TDLS_LINK_AP,
    TDLS_LINK_DIRECT
} eTdlsLink ;

/* 
                                
 */
enum tdlsLinkNodeStatus
{
    TDLS_NODE_NOT_FOUND,
    TDLS_NODE_FOUND
} eTdlsLinkNodeStatus ;


enum tdlsReqType
{
    TDLS_INITIATOR,
    TDLS_RESPONDER
} eTdlsReqType ;

typedef enum tdlsLinkSetupStatus
{
    TDLS_SETUP_STATUS_SUCCESS = 0,
    TDLS_SETUP_STATUS_FAILURE = 37
}etdlsLinkSetupStatus ;

/*                    */
#define LINK_IDEN_BSSID_OFFSET      (0)
#define PEER_MAC_OFFSET   (12) 
#define STA_MAC_OFFSET    (6)
#define LINK_IDEN_ELE_ID  (101)
//                                
#define LINK_IDEN_ADDR_OFFSET(x) (&x.LinkIdentifier)
#define PTI_LINK_IDEN_OFFSET     (5)
#define PTI_BUF_STATUS_OFFSET    (25)

/*                                             */
#define PEER_PSM_SUPPORT          (0)
#define PEER_BUFFER_STA_SUPPORT   (0)
#define CH_SWITCH_SUPPORT         (0)
#define TDLS_SUPPORT              (1)
#define TDLS_PROHIBITED           (0)
#define TDLS_CH_SWITCH_PROHIBITED (1)
/*                                    */
#define SET_BIT(value,mask)       ((value) |= (1 << (mask)))
/*                                      */
#define CLEAR_BIT(value,mask)     ((value) &= ~(1 << (mask)))
/*                                      */
#define CHECK_BIT(value, mask)    ((value) & (1 << (mask)))

#define SET_PEER_AID_BITMAP(peer_bitmap, aid) \
                                if ((aid) < (sizeof(tANI_U32) << 3)) \
                                        SET_BIT(peer_bitmap[0], (aid)); \
                                else if ((aid) < (sizeof(tANI_U32) << 4)) \
                                        SET_BIT(peer_bitmap[1], ((aid) - (sizeof(tANI_U32) << 3)));

#define CLEAR_PEER_AID_BITMAP(peer_bitmap, aid) \
                                if ((aid) < (sizeof(tANI_U32) << 3)) \
                                        CLEAR_BIT(peer_bitmap[0], (aid)); \
                                else if ((aid) < (sizeof(tANI_U32) << 4)) \
                                        CLEAR_BIT(peer_bitmap[1], ((aid) - (sizeof(tANI_U32) << 3)));


#ifdef LIM_DEBUG_TDLS
#define TDLS_CASE_RETURN_STRING(x) case (x): return( ((const tANI_U8*)#x) + 8);  /*                               */

#ifdef FEATURE_WLAN_TDLS
#define WNI_CFG_TDLS_DISCOVERY_RSP_WAIT             (100)
#define WNI_CFG_TDLS_LINK_SETUP_RSP_TIMEOUT         (800)
#define WNI_CFG_TDLS_LINK_SETUP_CNF_TIMEOUT         (200)
#endif

#define IS_QOS_ENABLED(psessionEntry) ((((psessionEntry)->limQosEnabled) && \
                                                  SIR_MAC_GET_QOS((psessionEntry)->limCurrentBssCaps)) || \
                                       (((psessionEntry)->limWmeEnabled ) && \
                                                  LIM_BSS_CAPS_GET(WME, (psessionEntry)->limCurrentBssQosCaps)))

#define TID_AC_VI                  4
#define TID_AC_BK                  1

const tANI_U8* limTraceTdlsActionString( tANI_U8 tdlsActionCode )
{
   switch( tdlsActionCode )
   {
       TDLS_CASE_RETURN_STRING(SIR_MAC_TDLS_SETUP_REQ);
       TDLS_CASE_RETURN_STRING(SIR_MAC_TDLS_SETUP_RSP);
       TDLS_CASE_RETURN_STRING(SIR_MAC_TDLS_SETUP_CNF);
       TDLS_CASE_RETURN_STRING(SIR_MAC_TDLS_TEARDOWN);
       TDLS_CASE_RETURN_STRING(SIR_MAC_TDLS_PEER_TRAFFIC_IND);
       TDLS_CASE_RETURN_STRING(SIR_MAC_TDLS_CH_SWITCH_REQ);
       TDLS_CASE_RETURN_STRING(SIR_MAC_TDLS_CH_SWITCH_RSP);
       TDLS_CASE_RETURN_STRING(SIR_MAC_TDLS_PEER_TRAFFIC_RSP);
       TDLS_CASE_RETURN_STRING(SIR_MAC_TDLS_DIS_REQ);
       TDLS_CASE_RETURN_STRING(SIR_MAC_TDLS_DIS_RSP);
   }
   return (const tANI_U8*)"UNKNOWN";
}
#endif
#if 0
static void printMacAddr(tSirMacAddr macAddr)
{
    int i = 0 ;
    VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR, (" Mac Addr: "));

    for(i = 0 ; i < 6; i++)
    {
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR, 
                                                 (" %02x "), macAddr[i]);
    }
    VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR, (""));
    return ;
}
#endif
/*
                                                          
 */
void limInitTdlsData(tpAniSirGlobal pMac, tpPESession pSessionEntry)
{
#ifdef FEATURE_WLAN_TDLS_INTERNAL
    pMac->lim.gLimTdlsDisResultList = NULL ;
    pMac->lim.gLimTdlsDisStaCount = 0 ;
    vos_mem_set(&pMac->lim.gLimTdlsDisReq, sizeof(tSirTdlsDisReq), 0);
    vos_mem_set(&pMac->lim.gLimTdlsLinkSetupInfo, sizeof(tLimTdlsLinkSetupInfo), 0);
    pMac->lim.gAddStaDisRspWait = 0 ;

#ifdef FEATURE_WLAN_TDLS_NEGATIVE
    /*                                                       */
    /*                                                                     */
    pMac->lim.gLimTdlsNegativeBehavior = 0;
#endif
#endif
    limInitPeerIdxpool(pMac, pSessionEntry) ;

    return ;
}
#ifdef FEATURE_WLAN_TDLS_NEGATIVE
void limTdlsSetNegativeBehavior(tpAniSirGlobal pMac, tANI_U8 value, tANI_BOOLEAN on)
{
    if(on) {
        if(value == 255)
            pMac->lim.gLimTdlsNegativeBehavior = 0XFFFFFFFF;
        else
            pMac->lim.gLimTdlsNegativeBehavior |= (1 << (value-1));
    }
    else {
        if(value == 255)
            pMac->lim.gLimTdlsNegativeBehavior = 0;
        else
            pMac->lim.gLimTdlsNegativeBehavior &= ~(1 << (value-1));
    }
    LIM_LOG_TDLS(VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR,("%d %d -> gLimTdlsNegativeBehavior= 0x%lx"), \
        value, on, pMac->lim.gLimTdlsNegativeBehavior));
}
#endif
#if 0
/*
                                                                 
                          
 */
static void limPreparesActionFrameHdr(tpAniSirGlobal pMac, tANI_U8 *pFrame,
                                         tANI_U8 type, tANI_U8 subType,
                                                   tANI_U8 *link_iden )
{
    tpSirMacMgmtHdr pMacHdr ;
    tANI_U8 *bssid = link_iden ;
#if 0     
    tANI_U8 *staMac = (tANI_U8 *)(bssid + sizeof(tSirMacAddr)) ;
    tANI_U8 *peerMac = (tANI_U8 *) (staMac + sizeof(tSirMacAddr)) ;
#else    
   tANI_U8 *peerMac = (tANI_U8 *) (bssid + sizeof(tSirMacAddr)) ;
   tANI_U8 *staMac = (tANI_U8 *)(peerMac + sizeof(tSirMacAddr)) ;
#endif    
    tANI_U8 toDs =  ANI_TXDIR_IBSS  ;

    pMacHdr = (tpSirMacMgmtHdr) (pFrame);

    /*
                            
     */ 
    pMacHdr->fc.protVer = SIR_MAC_PROTOCOL_VERSION;
    pMacHdr->fc.type    = type ;
    pMacHdr->fc.subType = subType ;
    /*
                                                                 
     */
    pMacHdr->fc.toDS    = toDs ;
    pMacHdr->fc.powerMgmt = 0 ;

     
    vos_mem_copy( (tANI_U8 *) pMacHdr->da, peerMac, sizeof( tSirMacAddr ));
    vos_mem_copy( (tANI_U8 *) pMacHdr->sa,
                   staMac, sizeof( tSirMacAddr ));

    vos_mem_copy( (tANI_U8 *) pMacHdr->bssId,
                                bssid, sizeof( tSirMacAddr ));
   
   LIM_LOG_TDLS(VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_WARN, ("Preparing TDLS action frame\n%02x:%02x:%02x:%02x:%02x:%02x/%02x:%02x:%02x:%02x:%02x:%02x/%02x:%02x:%02x:%02x:%02x:%02x"), \
       pMacHdr->da[0], pMacHdr->da[1], pMacHdr->da[2], pMacHdr->da[3], pMacHdr->da[4], pMacHdr->da[5], \
       pMacHdr->sa[0], pMacHdr->sa[1], pMacHdr->sa[2], pMacHdr->sa[3], pMacHdr->sa[4], pMacHdr->sa[5], \
       pMacHdr->bssId[0], pMacHdr->bssId[1], pMacHdr->bssId[2], \
       pMacHdr->bssId[3], pMacHdr->bssId[4], pMacHdr->bssId[5]));

    return ; 
}
#endif
/*
                                         
                                                  
                                                         
                                                  
 */
static tANI_U32 limPrepareTdlsFrameHeader(tpAniSirGlobal pMac, tANI_U8* pFrame, 
           tDot11fIELinkIdentifier *link_iden, tANI_U8 tdlsLinkType, tANI_U8 reqType,
           tANI_U8 tid, tpPESession psessionEntry)
{
    tpSirMacDataHdr3a pMacHdr ;
    tANI_U32 header_offset = 0 ;
    tANI_U8 *addr1 = NULL ;
    tANI_U8 *addr3 = NULL ;
    tANI_U8 toDs = (tdlsLinkType == TDLS_LINK_AP) 
                                       ? ANI_TXDIR_TODS :ANI_TXDIR_IBSS  ;
    tANI_U8 *peerMac = (reqType == TDLS_INITIATOR) 
                                       ? link_iden->RespStaAddr : link_iden->InitStaAddr; 
    tANI_U8 *staMac = (reqType == TDLS_INITIATOR) 
                                       ? link_iden->InitStaAddr : link_iden->RespStaAddr; 
   
    pMacHdr = (tpSirMacDataHdr3a) (pFrame);

    /* 
                                                                        
                                                                
                                                              
     */
    (tdlsLinkType == TDLS_LINK_AP) ? ((addr1 = (link_iden->bssid)),
                                      (addr3 = (peerMac))) 
                                   : ((addr1 = (peerMac)),
                                     (addr3 = (link_iden->bssid))) ;
    /*
                            
     */ 
    pMacHdr->fc.protVer = SIR_MAC_PROTOCOL_VERSION;
    pMacHdr->fc.type    = SIR_MAC_DATA_FRAME ;
    pMacHdr->fc.subType = IS_QOS_ENABLED(psessionEntry) ? SIR_MAC_DATA_QOS_DATA : SIR_MAC_DATA_DATA;

    /*
                                                                 
     */
    pMacHdr->fc.toDS    = toDs ;
    pMacHdr->fc.powerMgmt = 0 ;
    pMacHdr->fc.wep = (psessionEntry->encryptType == eSIR_ED_NONE)? 0 : 1;

     
    vos_mem_copy( (tANI_U8 *) pMacHdr->addr1,
                  (tANI_U8 *)addr1,
                  sizeof( tSirMacAddr ));
    vos_mem_copy( (tANI_U8 *) pMacHdr->addr2,
                  (tANI_U8 *) staMac,
                  sizeof( tSirMacAddr ));

    vos_mem_copy( (tANI_U8 *) pMacHdr->addr3,
                  (tANI_U8 *) (addr3),
                  sizeof( tSirMacAddr ));

    LIM_LOG_TDLS(VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_WARN, ("Preparing TDLS frame header to %s\n%02x:%02x:%02x:%02x:%02x:%02x/%02x:%02x:%02x:%02x:%02x:%02x/%02x:%02x:%02x:%02x:%02x:%02x"), \
       (tdlsLinkType == TDLS_LINK_AP) ? "AP" : "TD", \
        pMacHdr->addr1[0], pMacHdr->addr1[1], pMacHdr->addr1[2], pMacHdr->addr1[3], pMacHdr->addr1[4], pMacHdr->addr1[5], \
        pMacHdr->addr2[0], pMacHdr->addr2[1], pMacHdr->addr2[2], pMacHdr->addr2[3], pMacHdr->addr2[4], pMacHdr->addr2[5], \
        pMacHdr->addr3[0], pMacHdr->addr3[1], pMacHdr->addr3[2], pMacHdr->addr3[3], pMacHdr->addr3[4], pMacHdr->addr3[5]));

    //                              
    //                           
    //                           

    if (IS_QOS_ENABLED(psessionEntry))
    {
        pMacHdr->qosControl.tid = tid;
        header_offset += sizeof(tSirMacDataHdr3a);
    }
    else
        header_offset += sizeof(tSirMacMgmtHdr);

    /* 
                              
     */
    vos_mem_copy((tANI_U8 *)(pFrame + header_offset),
                 (tANI_U8 *)eth_890d_header, sizeof(eth_890d_header)) ;

    header_offset += sizeof(eth_890d_header) ; 

    /*                          */
    *(pFrame + header_offset) = PAYLOAD_TYPE_TDLS ;

    return(header_offset += PAYLOAD_TYPE_TDLS_SIZE) ; 
}

/*
                                    
 */
 eHalStatus limMgmtTXComplete(tpAniSirGlobal pMac,
                                   tANI_U32 txCompleteSuccess)
{
    tpPESession psessionEntry = NULL ;

    if (0xff != pMac->lim.mgmtFrameSessionId)
    {
        psessionEntry = peFindSessionBySessionId(pMac, pMac->lim.mgmtFrameSessionId);
        if (NULL == psessionEntry)
        {
            VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR,
                      ("%s: sessionID %d is not found"), __func__, pMac->lim.mgmtFrameSessionId);
            return eHAL_STATUS_FAILURE;
        }
        limSendSmeMgmtTXCompletion(pMac, psessionEntry, txCompleteSuccess);
        pMac->lim.mgmtFrameSessionId = 0xff;
    }
    return eHAL_STATUS_SUCCESS;
}

/*
                                                                   
                                                                              
 */
tSirRetStatus limSendTdlsDisReqFrame(tpAniSirGlobal pMac, tSirMacAddr peer_mac,
                                      tANI_U8 dialog, tpPESession psessionEntry)
{
    tDot11fTDLSDisReq   tdlsDisReq ;
    tANI_U32            status = 0 ;
    tANI_U32            nPayload = 0 ;
    tANI_U32            size = 0 ;
    tANI_U32            nBytes = 0 ;
    tANI_U32            header_offset = 0 ;
    tANI_U8            *pFrame;
    void               *pPacket;
    eHalStatus          halstatus;
#ifndef NO_PAD_TDLS_MIN_8023_SIZE
    tANI_U32            padLen = 0;
#endif

    /* 
                                                                       
                                                            
                                                                    
     */
    vos_mem_set( (tANI_U8*)&tdlsDisReq,
                  sizeof( tDot11fTDLSDisReq ), 0 );

    /*
                          
     */
    tdlsDisReq.Category.category = SIR_MAC_ACTION_TDLS ;
    tdlsDisReq.Action.action     = SIR_MAC_TDLS_DIS_REQ ;
    tdlsDisReq.DialogToken.token = dialog ;


    size = sizeof(tSirMacAddr) ;
   
    PopulateDot11fLinkIden( pMac, psessionEntry, &tdlsDisReq.LinkIdentifier, 
                                                 peer_mac, TDLS_INITIATOR) ;

    /* 
                                                                   
     */
    status = dot11fGetPackedTDLSDisReqSize( pMac, &tdlsDisReq, &nPayload);
    if ( DOT11F_FAILED( status ) )
    {
        limLog( pMac, LOGP, FL("Failed to calculate the packed size f"
                               "or a discovery Request (0x%08x)."), status );
        /*                                             */
        nPayload = sizeof( tDot11fTDLSDisReq );
    }
    else if ( DOT11F_WARNED( status ) )
    {
        limLog( pMac, LOGW, FL("There were warnings while calculating"
                               "the packed size for a discovery Request ("
                               "0x%08x)."), status );
    }

    /*
                                                                            
             
                                 
     */ 


    nBytes = nPayload + ((IS_QOS_ENABLED(psessionEntry))
                              ? sizeof(tSirMacDataHdr3a) : sizeof(tSirMacMgmtHdr))
                      + sizeof( eth_890d_header )
                      + PAYLOAD_TYPE_TDLS_SIZE ;

#ifndef NO_PAD_TDLS_MIN_8023_SIZE
    /*                                                                                                     
                                                                                     
                                                                                                      
     */
    if (nPayload + PAYLOAD_TYPE_TDLS_SIZE < MIN_IEEE_8023_SIZE)
    {
        padLen = MIN_IEEE_8023_SIZE - (nPayload + PAYLOAD_TYPE_TDLS_SIZE ) ;

        /*                                                                */
        if (padLen < MIN_VENDOR_SPECIFIC_IE_SIZE)
            padLen = MIN_VENDOR_SPECIFIC_IE_SIZE;

        nBytes += padLen;
    }
#endif

    /*                                                */

    halstatus = palPktAlloc( pMac->hHdd, HAL_TXRX_FRM_802_11_MGMT,
                             ( tANI_U16 )nBytes, ( void** ) &pFrame,
                             ( void** ) &pPacket );
    if ( ! HAL_STATUS_SUCCESS ( halstatus ) )
    {
        limLog( pMac, LOGP, FL("Failed to allocate %d bytes for a TDLS"
                               "Discovery Request."), nBytes );
        return eSIR_MEM_ALLOC_FAILED;
    }

    /*                     */
    vos_mem_set( pFrame, nBytes, 0 );

    /* 
                                                                            
                    
     */

    /*                                */

    header_offset = limPrepareTdlsFrameHeader(pMac, pFrame, 
           LINK_IDEN_ADDR_OFFSET(tdlsDisReq), TDLS_LINK_AP, TDLS_INITIATOR, TID_AC_VI, psessionEntry) ;

#ifdef FEATURE_WLAN_TDLS_NEGATIVE
    if(pMac->lim.gLimTdlsNegativeBehavior & LIM_TDLS_NEGATIVE_WRONG_BSSID_IN_DSCV_REQ)
    {
        tdlsDisReq.LinkIdentifier.bssid[4] = 0xde;
        tdlsDisReq.LinkIdentifier.bssid[5] = 0xad; 
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR, 
        ("TDLS negative running: wrong BSSID " MAC_ADDRESS_STR " in TDLS Discovery Req"),
        MAC_ADDR_ARRAY(tdlsDisReq.LinkIdentifier.bssid));
    }
#endif
    status = dot11fPackTDLSDisReq( pMac, &tdlsDisReq, pFrame 
                               + header_offset, nPayload, &nPayload );

    if ( DOT11F_FAILED( status ) )
    {
        limLog( pMac, LOGE, FL("Failed to pack a TDLS discovery req \
                                               (0x%08x)."), status );
        palPktFree( pMac->hHdd, HAL_TXRX_FRM_802_11_MGMT, 
                                   ( void* ) pFrame, ( void* ) pPacket );
        return eSIR_FAILURE;
    }
    else if ( DOT11F_WARNED( status ) )
    {
        limLog( pMac, LOGW, FL("There were warnings while packing TDLS"
                               "Discovery Request (0x%08x).") );
    }

#ifndef NO_PAD_TDLS_MIN_8023_SIZE
    if (padLen != 0)
    {
        /*                                                        */
        tANI_U8 *padVendorSpecific = pFrame + header_offset + nPayload;
        /*                                                                         */
        padVendorSpecific[0] = 221;
        padVendorSpecific[1] = padLen - 2;
        padVendorSpecific[2] = 0x00;
        padVendorSpecific[3] = 0xA0;
        padVendorSpecific[4] = 0xC6;

        LIM_LOG_TDLS(VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_INFO, ("Padding Vendor Specific Ie Len = %d"),
                padLen ));

        /*                                                */
        if (padLen > MIN_VENDOR_SPECIFIC_IE_SIZE)
            vos_mem_set( pFrame + header_offset + nPayload + MIN_VENDOR_SPECIFIC_IE_SIZE,
                         padLen - MIN_VENDOR_SPECIFIC_IE_SIZE, 0);
    }
#endif

    LIM_LOG_TDLS(VOS_TRACE(VOS_MODULE_ID_PE, TDLS_DEBUG_LOG_LEVEL, ("[TDLS] action %d (%s) -AP-> OTA "),
            SIR_MAC_TDLS_DIS_REQ, limTraceTdlsActionString(SIR_MAC_TDLS_DIS_REQ) ));

    halstatus = halTxFrameWithTxComplete( pMac, pPacket, ( tANI_U16 ) nBytes,
                            HAL_TXRX_FRM_802_11_DATA,
                            ANI_TXDIR_TODS,
                            TID_AC_VI,
                            limTxComplete, pFrame,
                            limMgmtTXComplete,
                            HAL_USE_BD_RATE2_FOR_MANAGEMENT_FRAME);
    if ( ! HAL_STATUS_SUCCESS ( halstatus ) )
    {
        pMac->lim.mgmtFrameSessionId = 0xff;
        limLog( pMac, LOGE, FL("could not send TDLS Dis Request frame!" ));
        return eSIR_FAILURE;
    }
    pMac->lim.mgmtFrameSessionId = psessionEntry->peSessionId;

    return eSIR_SUCCESS;

}

#ifdef FEATURE_WLAN_TDLS_INTERNAL
/*
                                                                             
                                          
 */
eHalStatus limTdlsDisRspTxComplete(tpAniSirGlobal pMac, 
                                           tANI_U32 txCompleteSuccess)
{
    eHalStatus status = eHAL_STATUS_SUCCESS ;
    tpDphHashNode pStaDs = NULL ;
    tSirTdlsPeerInfo *peerInfo = 0 ;

    /*                                                      */
    peerInfo = limTdlsFindDisPeerByState(pMac, TDLS_DIS_RSP_SENT_WAIT_STATE) ;

    if(NULL == peerInfo)
    {
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR, 
                                       ("DisRspTxComplete: No TDLS state machine waits for this event"));
        VOS_ASSERT(0) ;
        return eHAL_STATUS_FAILURE;
    }

    peerInfo->tdlsPeerState = TDLS_DIS_RSP_SENT_DONE_STATE ;

    if(peerInfo->delStaNeeded)
    {
        tpPESession psessionEntry;
        
        peerInfo->delStaNeeded = false ;
        psessionEntry = peFindSessionBySessionId (pMac, peerInfo->sessionId);

        if(NULL == psessionEntry) 
        {
            VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR, 
                                           ("DisRspTxComplete: sessionID %d is not found"), peerInfo->sessionId);
            return eHAL_STATUS_FAILURE;
        }
        /*                                                  */
        pStaDs = limTdlsDelSta(pMac, peerInfo->peerMac, psessionEntry) ;

        /*                                                            */
        if(pStaDs)
        {
            limSendSmeTdlsDelPeerInd(pMac, psessionEntry->smeSessionId, 
                                                     pStaDs, eSIR_SUCCESS) ;
        }
        else
        {
            VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR, 
                           ("DisRspTxComplete: staDs not found for " MAC_ADDRESS_STR),
                           MAC_ADDR_ARRAY((peerInfo)->peerMac));
            VOS_ASSERT(0) ;
            return eHAL_STATUS_FAILURE;
        }
    }
 
    if(!txCompleteSuccess)
     {
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR, 
                                       ("TX complete failure for Dis RSP"));
        limSendSmeTdlsDisRsp(pMac, eSIR_FAILURE, 
                                     eWNI_SME_TDLS_DISCOVERY_START_IND) ;
        status = eHAL_STATUS_FAILURE;
    }
    else
    {
        limSendSmeTdlsDisRsp(pMac, eSIR_SUCCESS, 
                                     eWNI_SME_TDLS_DISCOVERY_START_IND) ;
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_INFO, 
                                       ("TX complete Success for Dis RSP"));
        status = eHAL_STATUS_SUCCESS ;
    }
    //                                 
    return status ;

}
#endif

#ifdef FEATURE_WLAN_TDLS_INTERNAL
/*
                                                                    
                                          
 */
eHalStatus limTdlsSetupCnfTxComplete(tpAniSirGlobal pMac,
                                           tANI_U32 txCompleteSuccess)
{
    eHalStatus status = eHAL_STATUS_SUCCESS ;
    tLimTdlsLinkSetupPeer *peerInfo = 0 ;
    /*                                                      */
    limTdlsFindSetupPeerByState(pMac, 
                             TDLS_LINK_SETUP_RSP_WAIT_STATE, &peerInfo) ;
  
    if(NULL == peerInfo)
    {
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR, 
                                       ("limTdlsSetupCnfTxComplete: No TDLS state machine waits for this event"));
        VOS_ASSERT(0) ;
        return eHAL_STATUS_FAILURE;
    }
 
    (peerInfo)->tdls_prev_link_state = (peerInfo)->tdls_link_state ;
    (peerInfo)->tdls_link_state = TDLS_LINK_SETUP_DONE_STATE ; 

    if(!txCompleteSuccess)
    {
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR, 
                                   ("TX complete Failure for setup CNF"));
        limSendSmeTdlsLinkStartRsp(pMac, eSIR_FAILURE, (peerInfo)->peerMac,
                                               eWNI_SME_TDLS_LINK_START_RSP) ;
        status = eHAL_STATUS_FAILURE;
    }
    else
    {
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_INFO, 
              ("RSP-->SME peer MAC = " MAC_ADDRESS_STR),
               MAC_ADDR_ARRAY((peerInfo)->peerMac));
    
        limSendSmeTdlsLinkStartRsp(pMac, eSIR_SUCCESS, (peerInfo)->peerMac,
                                               eWNI_SME_TDLS_LINK_START_RSP) ;

        /*                                                     */
        limTdlsLinkEstablish(pMac, (peerInfo)->peerMac);

        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_INFO, 
                                 ("TX complete Success for setup CNF"));
        status = eHAL_STATUS_SUCCESS ;
    }
    //                                 
    return status ;
}
#endif

#ifdef FEATURE_WLAN_TDLS_INTERNAL
/*
                                 
 */
eHalStatus limTdlsTeardownTxComplete(tpAniSirGlobal pMac,
                                           tANI_U32 txCompleteSuccess)  
{
    eHalStatus status = eHAL_STATUS_SUCCESS ;
    tpDphHashNode pStaDs = NULL ;
    tLimTdlsLinkSetupPeer *peerInfo = 0 ;
    tpPESession psessionEntry = NULL ;
    //                      

    //                           
    /*                                                      */
    limTdlsFindSetupPeerByState(pMac, 
                             TDLS_LINK_TEARDOWN_START_STATE, &peerInfo) ;
  
    if(NULL == peerInfo)
    {
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR, 
                                       ("limTdlsTeardownTxComplete: No TDLS state machine waits for this event"));
        VOS_ASSERT(0) ;
        return eHAL_STATUS_FAILURE;
    }

    VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_INFO,
                  ("teardown peer Mac = " MAC_ADDRESS_STR),
                   MAC_ADDR_ARRAY((peerInfo)->peerMac));
             

    //                                 

    psessionEntry = peFindSessionBySessionId(pMac, (peerInfo)->tdls_sessionId);

    if(NULL == psessionEntry)
    {
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR, 
                                       ("limTdlsTeardownTxComplete: sessionID %d is not found"), (peerInfo)->tdls_sessionId);
        VOS_ASSERT(0) ;
        return eHAL_STATUS_FAILURE;
    }

    if(!txCompleteSuccess)
    {
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR, 
                         ("TX complete failure for Teardown  ")) ;

        /* 
                                                                
                                                                          
                                                                               
               
         */
        if(NULL != psessionEntry)
        {
            limSendTdlsTeardownFrame(pMac, (peerInfo)->peerMac, 
                     eSIR_MAC_TDLS_TEARDOWN_PEER_UNREACHABLE, psessionEntry, NULL, 0) ;
        }
    }

    if(TDLS_LINK_SETUP_WAIT_STATE != (peerInfo)->tdls_prev_link_state)
    {
        (peerInfo)->tdls_prev_link_state = (peerInfo)->tdls_link_state ;
        (peerInfo)->tdls_link_state = TDLS_LINK_TEARDOWN_DONE_STATE ; 
        /*                                                  */
        if(NULL != psessionEntry)
        {
            /*                                                      */
            limTdlsLinkTeardown(pMac, (peerInfo)->peerMac) ;

            pStaDs = limTdlsDelSta(pMac, (peerInfo)->peerMac, psessionEntry) ;
        }

        /*                                                            */
        if(!pStaDs)
        {
            VOS_ASSERT(0) ;
            return eSIR_FAILURE ;
        }
        limSendSmeTdlsDelPeerInd(pMac, psessionEntry->smeSessionId, 
                                                pStaDs, eSIR_SUCCESS) ;
 
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_INFO, 
                      ("TX complete SUCCESS for Teardown")) ;
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_INFO, 
                      ("Prev State = %d"), (peerInfo)->tdls_prev_link_state) ;
        limSendSmeTdlsTeardownRsp(pMac, eSIR_SUCCESS, (peerInfo)->peerMac,
                                                     eWNI_SME_TDLS_TEARDOWN_RSP) ;
        /*                                */
        limTdlsDelLinkPeer(pMac, (peerInfo)->peerMac) ;
    }
    else
    {
        (peerInfo)->tdls_prev_link_state = (peerInfo)->tdls_link_state ;
        (peerInfo)->tdls_link_state = TDLS_LINK_TEARDOWN_DONE_STATE ; 
        limSendSmeTdlsTeardownRsp(pMac, eSIR_SUCCESS, (peerInfo)->peerMac,
                                                eWNI_SME_TDLS_TEARDOWN_IND) ;
    }


#if 0
    /*                                                                  */
    if(TDLS_LINK_SETUP_RESTART_STATE == (peerInfo)->tdls_prev_link_state)
    {
        tLimTdlsLinkSetupInfo *setupInfo = &pMac->lim.gLimTdlsLinkSetupInfo ;
        limTdlsPrepareSetupReqFrame(pMac, setupInfo, 37, 
                                                   peerMac, psessionEntry) ;
    }
#endif  
    status = eHAL_STATUS_SUCCESS ;
    return status ;
}
#endif

/*
                                                                      
                                                                               
                                                            
 */
static void PopulateDot11fTdlsHtVhtCap(tpAniSirGlobal pMac, uint32 selfDot11Mode,
                                        tDot11fIEHTCaps *htCap, tDot11fIEVHTCaps *vhtCap,
                                        tpPESession psessionEntry)
{
    if (IS_DOT11_MODE_HT(selfDot11Mode))
    {
        /*                          */
        PopulateDot11fHTCaps( pMac, NULL, htCap );
        htCap->present = 1;
        if (psessionEntry->currentOperChannel <= SIR_11B_CHANNEL_END)
        {
            /*                                       */
            htCap->supportedChannelWidthSet = 0;
        }
        else
        {
            //                                                                      
            //                                                                               
            //                                                                 
            htCap->supportedChannelWidthSet = 1; //                   
        }
    }
    else
    {
        htCap->present = 0;
    }
#ifdef WLAN_FEATURE_11AC
    if (((psessionEntry->currentOperChannel <= SIR_11B_CHANNEL_END) &&
          pMac->roam.configParam.enableVhtFor24GHz) ||
         (psessionEntry->currentOperChannel >= SIR_11B_CHANNEL_END))
    {
        if (IS_DOT11_MODE_VHT(selfDot11Mode) &&
            IS_FEATURE_SUPPORTED_BY_FW(DOT11AC))
        {
            /*                           */
            PopulateDot11fVHTCaps( pMac, vhtCap );
        }
        else
        {
            vhtCap->present = 0;
        }
    }
    else
    {
        /*                                 */
        vhtCap->present = 0;
    }
#endif
}

/*
                                                     
 */

static tSirRetStatus limSendTdlsDisRspFrame(tpAniSirGlobal pMac, 
                     tSirMacAddr peerMac, tANI_U8 dialog, tpPESession psessionEntry)
{
    tDot11fTDLSDisRsp   tdlsDisRsp ;
    tANI_U16            caps = 0 ;            
    tANI_U32            status = 0 ;
    tANI_U32            nPayload = 0 ;
    tANI_U32            nBytes = 0 ;
    tANI_U8            *pFrame;
    void               *pPacket;
    eHalStatus          halstatus;
    uint32              selfDot11Mode;
//                                                                        
//                                                                               
//                                                                                 
//                                                                                              
//                                

    /* 
                                                                       
                                                            
                                                                    
     */
    vos_mem_set( ( tANI_U8* )&tdlsDisRsp,
                                      sizeof( tDot11fTDLSDisRsp ), 0 );

    /*
                          
     */
    tdlsDisRsp.Category.category = SIR_MAC_ACTION_PUBLIC_USAGE;
    tdlsDisRsp.Action.action     = SIR_MAC_TDLS_DIS_RSP ;
    tdlsDisRsp.DialogToken.token = dialog ;

    PopulateDot11fLinkIden( pMac, psessionEntry, &tdlsDisRsp.LinkIdentifier, 
                                           peerMac, TDLS_RESPONDER) ;

    if (cfgGetCapabilityInfo(pMac, &caps, psessionEntry) != eSIR_SUCCESS)
    {
        /*
                                           
                               
         */
         limLog(pMac, LOGP,
                   FL("could not retrieve Capabilities value"));
    }
    swapBitField16(caps, ( tANI_U16* )&tdlsDisRsp.Capabilities );

    /*                            */
    PopulateDot11fSuppRates( pMac, POPULATE_DOT11F_RATES_OPERATIONAL, 
                                     &tdlsDisRsp.SuppRates, psessionEntry );
   
    /*                                   */
    PopulateDot11fExtSuppRates( pMac, POPULATE_DOT11F_RATES_OPERATIONAL,
                                &tdlsDisRsp.ExtSuppRates, psessionEntry );

    /*                                   */
    PopulateDot11fTdlsExtCapability( pMac, &tdlsDisRsp.ExtCap );

    wlan_cfgGetInt(pMac,WNI_CFG_DOT11_MODE,&selfDot11Mode);

    /*                              */
    PopulateDot11fTdlsHtVhtCap( pMac, selfDot11Mode, &tdlsDisRsp.HTCaps,
                               &tdlsDisRsp.VHTCaps, psessionEntry );

    /* 
                                                                   
     */
    status = dot11fGetPackedTDLSDisRspSize( pMac, &tdlsDisRsp, &nPayload);
    if ( DOT11F_FAILED( status ) )
    {
        limLog( pMac, LOGP, FL("Failed to calculate the packed size f"
                               "or a discovery Request (0x%08x)."), status );
        /*                                             */
        nPayload = sizeof( tDot11fProbeRequest );
    }
    else if ( DOT11F_WARNED( status ) )
    {
        limLog( pMac, LOGW, FL("There were warnings while calculating"
                               "the packed size for a discovery Request ("
                               "0x%08x)."), status );
    }

    /*
                                                                            
             
                                 
     */ 


    nBytes = nPayload + sizeof( tSirMacMgmtHdr ) ;

    /*                                                */

    halstatus = palPktAlloc( pMac->hHdd, HAL_TXRX_FRM_802_11_MGMT,
                             ( tANI_U16 )nBytes, ( void** ) &pFrame,
                             ( void** ) &pPacket );
    if ( ! HAL_STATUS_SUCCESS ( halstatus ) )
    {
        limLog( pMac, LOGP, FL("Failed to allocate %d bytes for a TDLS"
                               "Discovery Request."), nBytes );
        return eSIR_MEM_ALLOC_FAILED;
    }

    /*                     */
    vos_mem_set( pFrame, nBytes, 0 );

    /* 
                                                                            
                     
     */

    /*                          */

#if 0
    limPreparesActionFrameHdr(pMac, pFrame, SIR_MAC_MGMT_FRAME,
                                          SIR_MAC_MGMT_ACTION, 
                                            LINK_IDEN_ADDR_OFFSET(tdlsDisRsp)) ;
#endif
    limPopulateMacHeader( pMac, pFrame, SIR_MAC_MGMT_FRAME,
              SIR_MAC_MGMT_ACTION, peerMac, psessionEntry->selfMacAddr);

    {
        tpSirMacMgmtHdr     pMacHdr;
        pMacHdr = ( tpSirMacMgmtHdr ) pFrame;
        pMacHdr->fc.toDS    = ANI_TXDIR_IBSS;
        pMacHdr->fc.powerMgmt = 0 ;
        sirCopyMacAddr(pMacHdr->bssId,psessionEntry->bssId);
    }

#ifdef FEATURE_WLAN_TDLS_NEGATIVE
    if(pMac->lim.gLimTdlsNegativeBehavior & LIM_TDLS_NEGATIVE_WRONG_BSSID_IN_DSCV_RSP)
    {
        tdlsDisRsp.LinkIdentifier.bssid[4] = 0xde;
        tdlsDisRsp.LinkIdentifier.bssid[5] = 0xad; 
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR, 
        ("TDLS negative running: wrong BSSID " MAC_ADDRESS_STR " in TDLS Discovery Rsp"),
         MAC_ADDR_ARRAY(tdlsDisRsp.LinkIdentifier.bssid));
    }
#endif
    status = dot11fPackTDLSDisRsp( pMac, &tdlsDisRsp, pFrame + 
                                              sizeof( tSirMacMgmtHdr ),
                                                  nPayload, &nPayload );

    if ( DOT11F_FAILED( status ) )
    {
        limLog( pMac, LOGE, FL("Failed to pack a TDLS discovery req \
                                               (0x%08x)."), status );
        palPktFree( pMac->hHdd, HAL_TXRX_FRM_802_11_MGMT, 
                                   ( void* ) pFrame, ( void* ) pPacket );
        return eSIR_FAILURE;
    }
    else if ( DOT11F_WARNED( status ) )
    {
        limLog( pMac, LOGW, FL("There were warnings while packing TDLS"
                               "Discovery Request (0x%08x).") );
    }

#if 0
    if(pMac->hal.pCBackFnTxComp == NULL) 
    {
        pMac->hal.pCBackFnTxComp = (tpCBackFnTxComp)limTdlsDisRspTxComplete;

        if(TX_SUCCESS != tx_timer_activate(&pMac->hal.txCompTimer)) 
        {
            status = eHAL_STATUS_FAILURE;
            return status;
                
        }
    }
#endif
    VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_INFO, 
                 ("transmitting Discovery response on direct link")) ;

    LIM_LOG_TDLS(VOS_TRACE(VOS_MODULE_ID_PE, TDLS_DEBUG_LOG_LEVEL, ("[TDLS] action %d (%s) -DIRECT-> OTA"),
            SIR_MAC_TDLS_DIS_RSP, limTraceTdlsActionString(SIR_MAC_TDLS_DIS_RSP) ));


    /*
                                                                    
                
     */
    halstatus = halTxFrameWithTxComplete( pMac, pPacket, ( tANI_U16 ) nBytes,
                            HAL_TXRX_FRM_802_11_DATA,
                            ANI_TXDIR_IBSS,
                            0,
                            limTxComplete, pFrame, 
                            limMgmtTXComplete,
                            HAL_USE_SELF_STA_REQUESTED_MASK );
    if ( ! HAL_STATUS_SUCCESS ( halstatus ) )
    {
        pMac->lim.mgmtFrameSessionId = 0xff;
        limLog( pMac, LOGE, FL("could not send TDLS Dis Request frame!" ));
        return eSIR_FAILURE;
    }
    pMac->lim.mgmtFrameSessionId = psessionEntry->peSessionId;

    return eSIR_SUCCESS;

}

/*
                                                                             
                                                                          
 */
static void PopulateDotfTdlsVhtAID(tpAniSirGlobal pMac, uint32 selfDot11Mode,
                                   tSirMacAddr peerMac, tDot11fIEAID *Aid,
                                   tpPESession psessionEntry)
{
    if (((psessionEntry->currentOperChannel <= SIR_11B_CHANNEL_END) &&
          pMac->roam.configParam.enableVhtFor24GHz) ||
         (psessionEntry->currentOperChannel >= SIR_11B_CHANNEL_END))
    {
        if (IS_DOT11_MODE_VHT(selfDot11Mode) &&
            IS_FEATURE_SUPPORTED_BY_FW(DOT11AC))
        {

            tANI_U16 aid;
            tpDphHashNode       pStaDs;

            pStaDs = dphLookupHashEntry(pMac, peerMac, &aid, &psessionEntry->dph.dphHashTable);
            if (NULL != pStaDs)
            {
                 Aid->present = 1;
                 Aid->assocId = aid | LIM_AID_MASK; //                      
            }
            else
            {
                Aid->present = 0;
                limLog( pMac, LOGE, FL("pStaDs is NULL for " MAC_ADDRESS_STR ),
                        MAC_ADDR_ARRAY(peerMac));
            }
        }
    }
    else
    {
        Aid->present = 0;
        limLog( pMac, LOGW, FL("Vht not enable from ini for 2.4GHz."));
    }
}

/*
                                      
 */

tSirRetStatus limSendTdlsLinkSetupReqFrame(tpAniSirGlobal pMac,
            tSirMacAddr peerMac, tANI_U8 dialog, tpPESession psessionEntry,
            tANI_U8 *addIe, tANI_U16 addIeLen)
{
    tDot11fTDLSSetupReq    tdlsSetupReq ;
    tANI_U16            caps = 0 ;
    tANI_U32            status = 0 ;
    tANI_U32            nPayload = 0 ;
    tANI_U32            nBytes = 0 ;
    tANI_U32            header_offset = 0 ;
    tANI_U8            *pFrame;
    void               *pPacket;
    eHalStatus          halstatus;
    uint32              selfDot11Mode;
//                                                                        
//                                                                               
//                                                                                 
//                                                                                              
//                                

    /*
                                                                       
                                                            
                                                                    
     */
    vos_mem_set(( tANI_U8* )&tdlsSetupReq, sizeof( tDot11fTDLSSetupReq ), 0);
    tdlsSetupReq.Category.category = SIR_MAC_ACTION_TDLS ;
    tdlsSetupReq.Action.action     = SIR_MAC_TDLS_SETUP_REQ ;
    tdlsSetupReq.DialogToken.token = dialog ;


    PopulateDot11fLinkIden( pMac, psessionEntry, &tdlsSetupReq.LinkIdentifier,
                                                    peerMac, TDLS_INITIATOR) ;

    if (cfgGetCapabilityInfo(pMac, &caps, psessionEntry) != eSIR_SUCCESS)
    {
        /*
                                           
                               
         */
         limLog(pMac, LOGP,
                   FL("could not retrieve Capabilities value"));
    }
    swapBitField16(caps, ( tANI_U16* )&tdlsSetupReq.Capabilities );

    /*                            */
    PopulateDot11fSuppRates( pMac, POPULATE_DOT11F_RATES_OPERATIONAL,
                              &tdlsSetupReq.SuppRates, psessionEntry );

    /*                                   */
    PopulateDot11fExtSuppRates( pMac, POPULATE_DOT11F_RATES_OPERATIONAL,
                                &tdlsSetupReq.ExtSuppRates, psessionEntry );

    /*                                   */
    PopulateDot11fTdlsExtCapability( pMac, &tdlsSetupReq.ExtCap );

   /*
                                                                          
                                                                        
                                                                       
                                                   
     */
    /*                                                   */
    /*                                                                                            
                                                   */
    tdlsSetupReq.QOSCapsStation.present = 1;
    tdlsSetupReq.QOSCapsStation.max_sp_length = 0;
    tdlsSetupReq.QOSCapsStation.qack = 0;
    tdlsSetupReq.QOSCapsStation.acbe_uapsd = ((pMac->lim.gLimTDLSUapsdMask & 0x08) >> 3) ;
    tdlsSetupReq.QOSCapsStation.acbk_uapsd = ((pMac->lim.gLimTDLSUapsdMask & 0x04)>> 2);
    tdlsSetupReq.QOSCapsStation.acvi_uapsd = ((pMac->lim.gLimTDLSUapsdMask & 0x02)>> 1);
    tdlsSetupReq.QOSCapsStation.acvo_uapsd = (pMac->lim.gLimTDLSUapsdMask & 0x01);


    /*
                                                                 
                                                                      
                   
     */

    wlan_cfgGetInt(pMac,WNI_CFG_DOT11_MODE,&selfDot11Mode);

    /*                              */
    PopulateDot11fTdlsHtVhtCap( pMac, selfDot11Mode, &tdlsSetupReq.HTCaps,
                               &tdlsSetupReq.VHTCaps, psessionEntry );

    /*              */
    PopulateDotfTdlsVhtAID( pMac, selfDot11Mode, peerMac,
                            &tdlsSetupReq.AID, psessionEntry );

    /*
                                                                   
     */
    status = dot11fGetPackedTDLSSetupReqSize( pMac, &tdlsSetupReq,
                                                              &nPayload);
    if ( DOT11F_FAILED( status ) )
    {
        limLog( pMac, LOGP, FL("Failed to calculate the packed size f"
                               "or a discovery Request (0x%08x)."), status );
        /*                                             */
        nPayload = sizeof( tDot11fProbeRequest );
    }
    else if ( DOT11F_WARNED( status ) )
    {
        limLog( pMac, LOGW, FL("There were warnings while calculating"
                               "the packed size for a discovery Request ("
                               "0x%08x)."), status );
    }


    /*
                                                                            
             
                                 
     */


    nBytes = nPayload + ((IS_QOS_ENABLED(psessionEntry))
                              ? sizeof(tSirMacDataHdr3a) : sizeof(tSirMacMgmtHdr))
                      + sizeof( eth_890d_header )
                      + PAYLOAD_TYPE_TDLS_SIZE
                      + addIeLen;

    /*                                                */

    halstatus = palPktAlloc( pMac->hHdd, HAL_TXRX_FRM_802_11_MGMT,
                             ( tANI_U16 )nBytes, ( void** ) &pFrame,
                             ( void** ) &pPacket );
    if ( ! HAL_STATUS_SUCCESS ( halstatus ) )
    {
        limLog( pMac, LOGP, FL("Failed to allocate %d bytes for a TDLS"
                               "Discovery Request."), nBytes );
        return eSIR_MEM_ALLOC_FAILED;
    }

    /*                     */
    vos_mem_set( pFrame, nBytes, 0);

    /*
                                                                            
                    
     */

    /*                                */

    header_offset = limPrepareTdlsFrameHeader(pMac, pFrame,
                     LINK_IDEN_ADDR_OFFSET(tdlsSetupReq), TDLS_LINK_AP, TDLS_INITIATOR, TID_AC_BK, psessionEntry) ;

#ifdef FEATURE_WLAN_TDLS_NEGATIVE
    if(pMac->lim.gLimTdlsNegativeBehavior & LIM_TDLS_NEGATIVE_WRONG_BSSID_IN_SETUP_REQ)
    {
        tdlsSetupReq.LinkIdentifier.bssid[4] = 0xde;
        tdlsSetupReq.LinkIdentifier.bssid[5] = 0xad;
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR,
        ("TDLS negative running: wrong BSSID " MAC_ADDRESS_STR " in TDLS Setup Req"),
         MAC_ADDR_ARRAY(tdlsSetupReq.LinkIdentifier.bssid));
    }
#endif
    limLog( pMac, LOGW, FL("%s: SupportedChnlWidth %x rxMCSMap %x rxMCSMap %x txSupDataRate %x"),
            __func__, tdlsSetupReq.VHTCaps.supportedChannelWidthSet, tdlsSetupReq.VHTCaps.rxMCSMap,
            tdlsSetupReq.VHTCaps.txMCSMap, tdlsSetupReq.VHTCaps.txSupDataRate );

    status = dot11fPackTDLSSetupReq( pMac, &tdlsSetupReq, pFrame
                               + header_offset, nPayload, &nPayload );

    if ( DOT11F_FAILED( status ) )
    {
        limLog( pMac, LOGE, FL("Failed to pack a TDLS discovery req \
                                               (0x%08x)."), status );
        palPktFree( pMac->hHdd, HAL_TXRX_FRM_802_11_MGMT, 
                                   ( void* ) pFrame, ( void* ) pPacket );
        return eSIR_FAILURE;
    }
    else if ( DOT11F_WARNED( status ) )
    {
        limLog( pMac, LOGW, FL("There were warnings while packing TDLS"
                               "Discovery Request (0x%08x).") );
    }

    //                       
    //                                                                   
    //                                                                         
    //                          
    if( addIeLen != 0 )
    {
    LIM_LOG_TDLS(VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR, ("Copy Additional Ie Len = %d"),
            addIeLen ));
       vos_mem_copy( pFrame + header_offset + nPayload, addIe, addIeLen );
    }

    LIM_LOG_TDLS(VOS_TRACE(VOS_MODULE_ID_PE, TDLS_DEBUG_LOG_LEVEL, ("[TDLS] action %d (%s) -AP-> OTA"),
            SIR_MAC_TDLS_SETUP_REQ, limTraceTdlsActionString(SIR_MAC_TDLS_SETUP_REQ) ));

    halstatus = halTxFrameWithTxComplete( pMac, pPacket, ( tANI_U16 ) nBytes,
                            HAL_TXRX_FRM_802_11_DATA,
                            ANI_TXDIR_TODS,
                            TID_AC_BK,
                            limTxComplete, pFrame,
                            limMgmtTXComplete,
                            HAL_USE_BD_RATE2_FOR_MANAGEMENT_FRAME );

    if ( ! HAL_STATUS_SUCCESS ( halstatus ) )
    {
        pMac->lim.mgmtFrameSessionId = 0xff;
        limLog( pMac, LOGE, FL("could not send TDLS Dis Request frame!" ));
        return eSIR_FAILURE;
    }
    pMac->lim.mgmtFrameSessionId = psessionEntry->peSessionId;

    return eSIR_SUCCESS;

}
/*
                                                                              
 */

tSirRetStatus limSendTdlsTeardownFrame(tpAniSirGlobal pMac,
            tSirMacAddr peerMac, tANI_U16 reason, tANI_U8 responder, tpPESession psessionEntry,
            tANI_U8 *addIe, tANI_U16 addIeLen)
{
    tDot11fTDLSTeardown teardown ;
    tANI_U32            status = 0 ;
    tANI_U32            nPayload = 0 ;
    tANI_U32            nBytes = 0 ;
    tANI_U32            header_offset = 0 ;
    tANI_U8            *pFrame;
    void               *pPacket;
    eHalStatus          halstatus;
#ifndef NO_PAD_TDLS_MIN_8023_SIZE
    tANI_U32            padLen = 0;
#endif
    /*
                                                                       
                                                            
                                                                    
     */
    vos_mem_set( ( tANI_U8* )&teardown, sizeof( tDot11fTDLSTeardown ), 0 );
    teardown.Category.category = SIR_MAC_ACTION_TDLS ;
    teardown.Action.action     = SIR_MAC_TDLS_TEARDOWN ;
    teardown.Reason.code       = reason ;

    PopulateDot11fLinkIden( pMac, psessionEntry, &teardown.LinkIdentifier,
                                                peerMac, (responder == TRUE) ? TDLS_RESPONDER : TDLS_INITIATOR) ;


    /*
                                                                   
     */
    status = dot11fGetPackedTDLSTeardownSize( pMac, &teardown, &nPayload);
    if ( DOT11F_FAILED( status ) )
    {
        limLog( pMac, LOGP, FL("Failed to calculate the packed size f"
                               "or a discovery Request (0x%08x)."), status );
        /*                                             */
        nPayload = sizeof( tDot11fProbeRequest );
    }
    else if ( DOT11F_WARNED( status ) )
    {
        limLog( pMac, LOGW, FL("There were warnings while calculating"
                               "the packed size for a discovery Request ("
                               "0x%08x)."), status );
    }


    /*
                                                                            
             
                                 
     */


    nBytes = nPayload + ((IS_QOS_ENABLED(psessionEntry))
                              ? sizeof(tSirMacDataHdr3a) : sizeof(tSirMacMgmtHdr))
                      + sizeof( eth_890d_header )
                      + PAYLOAD_TYPE_TDLS_SIZE
                      + addIeLen;

#ifndef NO_PAD_TDLS_MIN_8023_SIZE
    /*                                                                                                     
                                                                                     
                                                                                                      
     */
    if (nPayload + PAYLOAD_TYPE_TDLS_SIZE < MIN_IEEE_8023_SIZE)
    {
        padLen = MIN_IEEE_8023_SIZE - (nPayload + PAYLOAD_TYPE_TDLS_SIZE ) ;

        /*                                                                */
        if (padLen < MIN_VENDOR_SPECIFIC_IE_SIZE)
            padLen = MIN_VENDOR_SPECIFIC_IE_SIZE;

        nBytes += padLen;
    }
#endif

    /*                                                */

    halstatus = palPktAlloc( pMac->hHdd, HAL_TXRX_FRM_802_11_MGMT,
                             ( tANI_U16 )nBytes, ( void** ) &pFrame,
                             ( void** ) &pPacket );
    if ( ! HAL_STATUS_SUCCESS ( halstatus ) )
    {
        limLog( pMac, LOGP, FL("Failed to allocate %d bytes for a TDLS"
                               "Discovery Request."), nBytes );
        return eSIR_MEM_ALLOC_FAILED;
    }

    /*                     */
    vos_mem_set( pFrame, nBytes, 0 );

    /*
                                                                            
                    
     */

    /*                                */

    header_offset = limPrepareTdlsFrameHeader(pMac, pFrame,
                     LINK_IDEN_ADDR_OFFSET(teardown),
                          (reason == eSIR_MAC_TDLS_TEARDOWN_PEER_UNREACHABLE)
                              ? TDLS_LINK_AP : TDLS_LINK_DIRECT,
                              (responder == TRUE) ? TDLS_RESPONDER : TDLS_INITIATOR,
                              TID_AC_VI, psessionEntry) ;

    status = dot11fPackTDLSTeardown( pMac, &teardown, pFrame
                               + header_offset, nPayload, &nPayload );

    if ( DOT11F_FAILED( status ) )
    {
        limLog( pMac, LOGE, FL("Failed to pack a TDLS Teardown req \
                                               (0x%08x)."), status );
        palPktFree( pMac->hHdd, HAL_TXRX_FRM_802_11_MGMT,
                                   ( void* ) pFrame, ( void* ) pPacket );
        return eSIR_FAILURE;
    }
    else if ( DOT11F_WARNED( status ) )
    {
        limLog( pMac, LOGW, FL("There were warnings while packing TDLS"
                               "Teardown Request (0x%08x).") );
    }
#if 0
    if(pMac->hal.pCBackFnTxComp == NULL)
    {
        pMac->hal.pCBackFnTxComp = (tpCBackFnTxComp)limTdlsTeardownTxComplete;
        if(TX_SUCCESS != tx_timer_activate(&pMac->hal.txCompTimer))
        {
            status = eHAL_STATUS_FAILURE;
            return status;
                
        }
    }
    else
    {
        VOS_ASSERT(0) ;
        return status ;
    }
#endif

    if( addIeLen != 0 )
    {
    LIM_LOG_TDLS(VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR, ("Copy Additional Ie Len = %d"),
            addIeLen ));
       vos_mem_copy( pFrame + header_offset + nPayload, addIe, addIeLen );
    }

#ifndef NO_PAD_TDLS_MIN_8023_SIZE
    if (padLen != 0)
    {
        /*                                                        */
        tANI_U8 *padVendorSpecific = pFrame + header_offset + nPayload + addIeLen;
        /*                                                                         */
        padVendorSpecific[0] = 221;
        padVendorSpecific[1] = padLen - 2;
        padVendorSpecific[2] = 0x00;
        padVendorSpecific[3] = 0xA0;
        padVendorSpecific[4] = 0xC6;

        LIM_LOG_TDLS(VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_INFO, ("Padding Vendor Specific Ie Len = %d"),
                padLen ));

        /*                                                */
        if (padLen > MIN_VENDOR_SPECIFIC_IE_SIZE)
            vos_mem_set( pFrame + header_offset + nPayload + addIeLen + MIN_VENDOR_SPECIFIC_IE_SIZE,
                         padLen - MIN_VENDOR_SPECIFIC_IE_SIZE, 0);
    }
#endif
    LIM_LOG_TDLS(VOS_TRACE(VOS_MODULE_ID_PE, TDLS_DEBUG_LOG_LEVEL, ("[TDLS] action %d (%s) -%s-> OTA"),
         SIR_MAC_TDLS_TEARDOWN, limTraceTdlsActionString(SIR_MAC_TDLS_TEARDOWN),
         (reason == eSIR_MAC_TDLS_TEARDOWN_PEER_UNREACHABLE) ? "AP": "DIRECT" ));

    halstatus = halTxFrameWithTxComplete( pMac, pPacket, ( tANI_U16 ) nBytes,
                            HAL_TXRX_FRM_802_11_DATA,
                            ANI_TXDIR_TODS,
                            TID_AC_VI,
                            limTxComplete, pFrame,
                            limMgmtTXComplete,
                            HAL_USE_BD_RATE2_FOR_MANAGEMENT_FRAME );
    if ( ! HAL_STATUS_SUCCESS ( halstatus ) )
    {
        pMac->lim.mgmtFrameSessionId = 0xff;
        limLog( pMac, LOGE, FL("could not send TDLS Dis Request frame!" ));
        return eSIR_FAILURE;

    }
    pMac->lim.mgmtFrameSessionId = psessionEntry->peSessionId;
    return eSIR_SUCCESS;

}

/*
                                   
 */
static tSirRetStatus limSendTdlsSetupRspFrame(tpAniSirGlobal pMac, 
                    tSirMacAddr peerMac, tANI_U8 dialog, tpPESession psessionEntry, 
                    etdlsLinkSetupStatus setupStatus, tANI_U8 *addIe, tANI_U16 addIeLen )
{
    tDot11fTDLSSetupRsp  tdlsSetupRsp ;
    tANI_U32            status = 0 ;
    tANI_U16            caps = 0 ;            
    tANI_U32            nPayload = 0 ;
    tANI_U32            header_offset = 0 ;
    tANI_U32            nBytes = 0 ;
    tANI_U8            *pFrame;
    void               *pPacket;
    eHalStatus          halstatus;
    uint32             selfDot11Mode;
//                                                                        
//                                                                               
//                                                                                 
//                                                                                              
//                                

    /* 
                                                                       
                                                            
                                                                    
     */
    vos_mem_set( ( tANI_U8* )&tdlsSetupRsp, sizeof( tDot11fTDLSSetupRsp ),0 );

    /*
                          
     */
    tdlsSetupRsp.Category.category = SIR_MAC_ACTION_TDLS;
    tdlsSetupRsp.Action.action     = SIR_MAC_TDLS_SETUP_RSP ;
    tdlsSetupRsp.DialogToken.token = dialog;

    PopulateDot11fLinkIden( pMac, psessionEntry, &tdlsSetupRsp.LinkIdentifier,
                 peerMac, TDLS_RESPONDER) ;

    if (cfgGetCapabilityInfo(pMac, &caps, psessionEntry) != eSIR_SUCCESS)
    {
        /*
                                           
                               
         */
         limLog(pMac, LOGP,
                   FL("could not retrieve Capabilities value"));
    }
    swapBitField16(caps, ( tANI_U16* )&tdlsSetupRsp.Capabilities );

    /*                             */
    PopulateDot11fSuppRates( pMac, POPULATE_DOT11F_RATES_OPERATIONAL, 
                                &tdlsSetupRsp.SuppRates, psessionEntry );
   
    /*                                   */
    PopulateDot11fExtSuppRates( pMac, POPULATE_DOT11F_RATES_OPERATIONAL,
                                &tdlsSetupRsp.ExtSuppRates, psessionEntry );

    /*                                   */
    PopulateDot11fTdlsExtCapability( pMac, &tdlsSetupRsp.ExtCap );

    /* 
                                                                          
                                                                        
                                                                        
                                                   
     */
    /*                                                   */
    /*                                                                                            
                                                   */
    tdlsSetupRsp.QOSCapsStation.present = 1;
    tdlsSetupRsp.QOSCapsStation.max_sp_length = 0;
    tdlsSetupRsp.QOSCapsStation.qack = 0;
    tdlsSetupRsp.QOSCapsStation.acbe_uapsd = ((pMac->lim.gLimTDLSUapsdMask & 0x08) >> 3);
    tdlsSetupRsp.QOSCapsStation.acbk_uapsd = ((pMac->lim.gLimTDLSUapsdMask & 0x04) >> 2);
    tdlsSetupRsp.QOSCapsStation.acvi_uapsd = ((pMac->lim.gLimTDLSUapsdMask & 0x02) >> 1);
    tdlsSetupRsp.QOSCapsStation.acvo_uapsd = (pMac->lim.gLimTDLSUapsdMask & 0x01);

    wlan_cfgGetInt(pMac,WNI_CFG_DOT11_MODE,&selfDot11Mode);

    /*                              */
    PopulateDot11fTdlsHtVhtCap( pMac, selfDot11Mode, &tdlsSetupRsp.HTCaps,
                                &tdlsSetupRsp.VHTCaps, psessionEntry );

    /*              */
    PopulateDotfTdlsVhtAID( pMac, selfDot11Mode, peerMac,
                            &tdlsSetupRsp.AID, psessionEntry );

    tdlsSetupRsp.Status.status = setupStatus ;

    /* 
                                                                   
     */
    status = dot11fGetPackedTDLSSetupRspSize( pMac, &tdlsSetupRsp, 
                                                     &nPayload);
    if ( DOT11F_FAILED( status ) )
    {
        limLog( pMac, LOGP, FL("Failed to calculate the packed size f"
                               "or a discovery Request (0x%08x)."), status );
        /*                                             */
        nPayload = sizeof( tDot11fProbeRequest );
    }
    else if ( DOT11F_WARNED( status ) )
    {
        limLog( pMac, LOGW, FL("There were warnings while calculating"
                               "the packed size for a discovery Request ("
                               "0x%08x)."), status );
    }

    /*
                                                                            
             
                                 
     */ 


    nBytes = nPayload + ((IS_QOS_ENABLED(psessionEntry))
                              ? sizeof(tSirMacDataHdr3a) : sizeof(tSirMacMgmtHdr))
                      + sizeof( eth_890d_header )
                      + PAYLOAD_TYPE_TDLS_SIZE
                      + addIeLen;

    /*                                                */

    halstatus = palPktAlloc( pMac->hHdd, HAL_TXRX_FRM_802_11_MGMT,
                             ( tANI_U16 )nBytes, ( void** ) &pFrame,
                             ( void** ) &pPacket );
    if ( ! HAL_STATUS_SUCCESS ( halstatus ) )
    {
        limLog( pMac, LOGP, FL("Failed to allocate %d bytes for a TDLS"
                               "Discovery Request."), nBytes );
        return eSIR_MEM_ALLOC_FAILED;
    }

    /*                     */
    vos_mem_set(  pFrame, nBytes, 0 );

    /* 
                                                                            
                    
     */

    /*                                */

    header_offset = limPrepareTdlsFrameHeader(pMac, pFrame, 
                                 LINK_IDEN_ADDR_OFFSET(tdlsSetupRsp), 
                                       TDLS_LINK_AP, TDLS_RESPONDER,
                                       TID_AC_BK, psessionEntry) ;

#ifdef FEATURE_WLAN_TDLS_NEGATIVE
    if(pMac->lim.gLimTdlsNegativeBehavior & LIM_TDLS_NEGATIVE_WRONG_BSSID_IN_SETUP_RSP)
    {
        tdlsSetupRsp.LinkIdentifier.bssid[4] = 0xde;
        tdlsSetupRsp.LinkIdentifier.bssid[5] = 0xad; 
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR, 
        ("TDLS negative running: wrong BSSID " MAC_ADDRESS_STR " in TDLS Setup Rsp"),
         MAC_ADDR_ARRAY(tdlsSetupRsp.LinkIdentifier.bssid));
    }
#endif
    limLog( pMac, LOGW, FL("%s: SupportedChnlWidth %x rxMCSMap %x rxMCSMap %x txSupDataRate %x"),
            __func__, tdlsSetupRsp.VHTCaps.supportedChannelWidthSet, tdlsSetupRsp.VHTCaps.rxMCSMap,
            tdlsSetupRsp.VHTCaps.txMCSMap, tdlsSetupRsp.VHTCaps.txSupDataRate );
    status = dot11fPackTDLSSetupRsp( pMac, &tdlsSetupRsp, pFrame 
                               + header_offset, nPayload, &nPayload );

    if ( DOT11F_FAILED( status ) )
    {
        limLog( pMac, LOGE, FL("Failed to pack a TDLS discovery req \
                                               (0x%08x)."), status );
        palPktFree( pMac->hHdd, HAL_TXRX_FRM_802_11_MGMT, 
                                   ( void* ) pFrame, ( void* ) pPacket );
        return eSIR_FAILURE;
    }
    else if ( DOT11F_WARNED( status ) )
    {
        limLog( pMac, LOGW, FL("There were warnings while packing TDLS"
                               "Discovery Request (0x%08x).") );
    }

    //                        
    //                                                                   
    //                                                                         
    //                          
    if( addIeLen != 0 )
    {
       vos_mem_copy( pFrame + header_offset + nPayload, addIe, addIeLen );
    }

    LIM_LOG_TDLS(VOS_TRACE(VOS_MODULE_ID_PE, TDLS_DEBUG_LOG_LEVEL, ("[TDLS] action %d (%s) -AP-> OTA"),
         SIR_MAC_TDLS_SETUP_RSP, limTraceTdlsActionString(SIR_MAC_TDLS_SETUP_RSP) ));

    halstatus = halTxFrameWithTxComplete( pMac, pPacket, ( tANI_U16 ) nBytes,
                            HAL_TXRX_FRM_802_11_DATA,
                            ANI_TXDIR_TODS,
                            //               
                            TID_AC_BK,
                            limTxComplete, pFrame,
                            limMgmtTXComplete,
                            HAL_USE_BD_RATE2_FOR_MANAGEMENT_FRAME );
    if ( ! HAL_STATUS_SUCCESS ( halstatus ) )
    {
        pMac->lim.mgmtFrameSessionId = 0xff;
        limLog( pMac, LOGE, FL("could not send TDLS Dis Request frame!" ));
        return eSIR_FAILURE;
    }
    pMac->lim.mgmtFrameSessionId = psessionEntry->peSessionId;

    return eSIR_SUCCESS;

}

/*
                                       
 */

tSirRetStatus limSendTdlsLinkSetupCnfFrame(tpAniSirGlobal pMac, tSirMacAddr peerMac,
                    tANI_U8 dialog, tpPESession psessionEntry, tANI_U8* addIe, tANI_U16 addIeLen)  
{
    tDot11fTDLSSetupCnf  tdlsSetupCnf ;
    tANI_U32            status = 0 ;
    tANI_U32            nPayload = 0 ;
    tANI_U32            nBytes = 0 ;
    tANI_U32            header_offset = 0 ;
    tANI_U8            *pFrame;
    void               *pPacket;
    eHalStatus          halstatus;
#ifndef NO_PAD_TDLS_MIN_8023_SIZE
    tANI_U32            padLen = 0;
#endif

    /* 
                                                                       
                                                            
                                                                    
     */
    vos_mem_set( ( tANI_U8* )&tdlsSetupCnf, sizeof( tDot11fTDLSSetupCnf ), 0 );

    /*
                          
     */
    tdlsSetupCnf.Category.category = SIR_MAC_ACTION_TDLS;
    tdlsSetupCnf.Action.action     = SIR_MAC_TDLS_SETUP_CNF ;
    tdlsSetupCnf.DialogToken.token = dialog ;

#if 1
    PopulateDot11fLinkIden( pMac, psessionEntry, &tdlsSetupCnf.LinkIdentifier,
                      peerMac, TDLS_INITIATOR) ;
#else
    vos_mem_copy( (tANI_U8 *)&tdlsSetupCnf.LinkIdentifier,
                  (tANI_U8 *)&setupRsp->LinkIdentifier, sizeof(tDot11fIELinkIdentifier)) ;
#endif

    /* 
                                                                          
                                                                        
                                                                        
                                                   
     */

    /*                    */
    /*                                                       */
    if ( true == psessionEntry->htCapability)
    {
        PopulateDot11fHTInfo( pMac, &tdlsSetupCnf.HTInfo, psessionEntry );
    }
    if ( true == psessionEntry->vhtCapability)
    {
        PopulateDot11fVHTOperation( pMac, &tdlsSetupCnf.VHTOperation);
    }

    /* 
                                                                   
     */
    status = dot11fGetPackedTDLSSetupCnfSize( pMac, &tdlsSetupCnf, 
                                                     &nPayload);
    if ( DOT11F_FAILED( status ) )
    {
        limLog( pMac, LOGP, FL("Failed to calculate the packed size f"
                               "or a discovery Request (0x%08x)."), status );
        /*                                             */
        nPayload = sizeof( tDot11fProbeRequest );
    }
    else if ( DOT11F_WARNED( status ) )
    {
        limLog( pMac, LOGW, FL("There were warnings while calculating"
                               "the packed size for a discovery Request ("
                               "0x%08x)."), status );
    }

    /*
                                                                            
             
                                 
     */ 


    nBytes = nPayload + ((IS_QOS_ENABLED(psessionEntry))
                              ? sizeof(tSirMacDataHdr3a) : sizeof(tSirMacMgmtHdr))
                      + sizeof( eth_890d_header )
                      + PAYLOAD_TYPE_TDLS_SIZE
                      + addIeLen;

#ifndef NO_PAD_TDLS_MIN_8023_SIZE
    /*                                                                                                     
                                                                                     
                                                                                                      
     */
    if (nPayload + PAYLOAD_TYPE_TDLS_SIZE < MIN_IEEE_8023_SIZE)
    {
        padLen = MIN_IEEE_8023_SIZE - (nPayload + PAYLOAD_TYPE_TDLS_SIZE ) ;

        /*                                                                */
        if (padLen < MIN_VENDOR_SPECIFIC_IE_SIZE)
            padLen = MIN_VENDOR_SPECIFIC_IE_SIZE;

        nBytes += padLen;
    }
#endif


    /*                                                */

    halstatus = palPktAlloc( pMac->hHdd, HAL_TXRX_FRM_802_11_MGMT,
                             ( tANI_U16 )nBytes, ( void** ) &pFrame,
                             ( void** ) &pPacket );
    if ( ! HAL_STATUS_SUCCESS ( halstatus ) )
    {
        limLog( pMac, LOGP, FL("Failed to allocate %d bytes for a TDLS"
                               "Discovery Request."), nBytes );
        return eSIR_MEM_ALLOC_FAILED;
    }

    /*                     */
    vos_mem_set( pFrame, nBytes, 0 );

    /* 
                                                                            
                    
     */

    /*                                */

    header_offset = limPrepareTdlsFrameHeader(pMac, pFrame, 
                     LINK_IDEN_ADDR_OFFSET(tdlsSetupCnf), TDLS_LINK_AP, TDLS_INITIATOR,
                     TID_AC_VI, psessionEntry) ;

#ifdef FEATURE_WLAN_TDLS_NEGATIVE
    if(pMac->lim.gLimTdlsNegativeBehavior & LIM_TDLS_NEGATIVE_STATUS_37_IN_SETUP_CNF) {
        tdlsSetupCnf.StatusCode.statusCode = 37;
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR, 
        ("TDLS negative running: StatusCode = 37 in TDLS Setup Cnf"));
    }
#endif
    status = dot11fPackTDLSSetupCnf( pMac, &tdlsSetupCnf, pFrame 
                               + header_offset, nPayload, &nPayload );

    if ( DOT11F_FAILED( status ) )
    {
        limLog( pMac, LOGE, FL("Failed to pack a TDLS discovery req \
                                               (0x%08x)."), status );
        palPktFree( pMac->hHdd, HAL_TXRX_FRM_802_11_MGMT, 
                                   ( void* ) pFrame, ( void* ) pPacket );
        return eSIR_FAILURE;
    }
    else if ( DOT11F_WARNED( status ) )
    {
        limLog( pMac, LOGW, FL("There were warnings while packing TDLS"
                               "Discovery Request (0x%08x).") );
    }
#if 0
    if(pMac->hal.pCBackFnTxComp == NULL) 
    {
        pMac->hal.pCBackFnTxComp = (tpCBackFnTxComp)limTdlsSetupCnfTxComplete;
        if(TX_SUCCESS != tx_timer_activate(&pMac->hal.txCompTimer)) 
        {
            status = eHAL_STATUS_FAILURE;
            return status;
                
        }
    }
    else
    {
        VOS_ASSERT(0) ;
        return status ;
    }
#endif
    //                        
    //                                                                   
    //                                                                         
    //                          
    if( addIeLen != 0 )
    {
       vos_mem_copy( pFrame + header_offset + nPayload, addIe, addIeLen );
    }

#ifndef NO_PAD_TDLS_MIN_8023_SIZE
    if (padLen != 0)
    {
        /*                                                        */
        tANI_U8 *padVendorSpecific = pFrame + header_offset + nPayload + addIeLen;
        /*                                                                         */
        padVendorSpecific[0] = 221;
        padVendorSpecific[1] = padLen - 2;
        padVendorSpecific[2] = 0x00;
        padVendorSpecific[3] = 0xA0;
        padVendorSpecific[4] = 0xC6;

        LIM_LOG_TDLS(VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_INFO, ("Padding Vendor Specific Ie Len = %d"),
                padLen ));

        /*                                                */
        if (padLen > MIN_VENDOR_SPECIFIC_IE_SIZE)
            vos_mem_set( pFrame + header_offset + nPayload + addIeLen + MIN_VENDOR_SPECIFIC_IE_SIZE,
                         padLen - MIN_VENDOR_SPECIFIC_IE_SIZE, 0);
    }
#endif


    LIM_LOG_TDLS(VOS_TRACE(VOS_MODULE_ID_PE, TDLS_DEBUG_LOG_LEVEL, ("[TDLS] action %d (%s) -AP-> OTA"),
         SIR_MAC_TDLS_SETUP_CNF, limTraceTdlsActionString(SIR_MAC_TDLS_SETUP_CNF) ));

    halstatus = halTxFrameWithTxComplete( pMac, pPacket, ( tANI_U16 ) nBytes,
                            HAL_TXRX_FRM_802_11_DATA,
                            ANI_TXDIR_TODS,
                            TID_AC_VI,
                            limTxComplete, pFrame, 
                            limMgmtTXComplete,
                            HAL_USE_BD_RATE2_FOR_MANAGEMENT_FRAME );


    if ( ! HAL_STATUS_SUCCESS ( halstatus ) )
    {
        pMac->lim.mgmtFrameSessionId = 0xff;
        limLog( pMac, LOGE, FL("could not send TDLS Dis Request frame!" ));
        return eSIR_FAILURE;

    }
    pMac->lim.mgmtFrameSessionId = psessionEntry->peSessionId;

    return eSIR_SUCCESS;
}

#ifdef FEATURE_WLAN_TDLS_INTERNAL
/*
                                        
 */
static void limTdlsCovertHTCaps(tpAniSirGlobal pMac,
                         tSirTdlsPeerInfo *peerInfo, tDot11fIEHTCaps *HTCaps)
{

    /*                    */
    peerInfo->tdlsPeerHtCaps.advCodingCap = HTCaps->advCodingCap ;
    peerInfo->tdlsPeerHtCaps.supportedChannelWidthSet = 
                                            HTCaps->supportedChannelWidthSet ;
    peerInfo->tdlsPeerHtCaps.mimoPowerSave = HTCaps->mimoPowerSave ;
    peerInfo->tdlsPeerHtCaps.greenField = HTCaps->greenField ;
    peerInfo->tdlsPeerHtCaps.shortGI20MHz = HTCaps->shortGI20MHz ;
    peerInfo->tdlsPeerHtCaps.shortGI40MHz = HTCaps->shortGI40MHz ;
    peerInfo->tdlsPeerHtCaps.txSTBC = HTCaps->txSTBC ;
    peerInfo->tdlsPeerHtCaps.rxSTBC = HTCaps->rxSTBC ;
    peerInfo->tdlsPeerHtCaps.delayedBA = HTCaps->delayedBA;
    peerInfo->tdlsPeerHtCaps.maximalAMSDUsize = HTCaps->maximalAMSDUsize ;
    peerInfo->tdlsPeerHtCaps.dsssCckMode40MHz = HTCaps->dsssCckMode40MHz ;
    peerInfo->tdlsPeerHtCaps.psmp = HTCaps->stbcControlFrame ;
    peerInfo->tdlsPeerHtCaps.stbcControlFrame = HTCaps->stbcControlFrame ;
    peerInfo->tdlsPeerHtCaps.lsigTXOPProtection = 
                                                 HTCaps->lsigTXOPProtection ;

    /*                    */
    peerInfo->tdlsPeerHtParams.maxRxAMPDUFactor = HTCaps->maxRxAMPDUFactor ;
    peerInfo->tdlsPeerHtParams.mpduDensity = HTCaps->mpduDensity ;
    peerInfo->tdlsPeerHtParams.reserved = HTCaps->reserved1 ;
    
    /*                  */
    peerInfo->tdlsPeerHtExtCaps.pco = HTCaps->pco ;
    peerInfo->tdlsPeerHtExtCaps.transitionTime = HTCaps->transitionTime ;
    peerInfo->tdlsPeerHtExtCaps.mcsFeedback = HTCaps->mcsFeedback ;
    vos_mem_copy( peerInfo->supportedMCSSet,
                      HTCaps->supportedMCSSet, SIZE_OF_SUPPORTED_MCS_SET) ;

    return ;
}

/*
                           
 */
void tdlsUpdateCapInfo(tSirMacCapabilityInfo *capabilityInfo, 
                                tDot11fFfCapabilities *Capabilities)
{

    capabilityInfo->ess            = Capabilities->ess;
    capabilityInfo->ibss           = Capabilities->ibss;
    capabilityInfo->cfPollable     = Capabilities->cfPollable;
    capabilityInfo->cfPollReq      = Capabilities->cfPollReq;
    capabilityInfo->privacy        = Capabilities->privacy;
    capabilityInfo->shortPreamble  = Capabilities->shortPreamble;
    capabilityInfo->pbcc           = Capabilities->pbcc;
    capabilityInfo->channelAgility = Capabilities->channelAgility;
    capabilityInfo->spectrumMgt    = Capabilities->spectrumMgt;
    capabilityInfo->qos            = Capabilities->qos;
    capabilityInfo->shortSlotTime  = Capabilities->shortSlotTime;
    capabilityInfo->apsd           = Capabilities->apsd;
    capabilityInfo->rrm            = Capabilities->rrm;
    capabilityInfo->dsssOfdm       = Capabilities->dsssOfdm;
    capabilityInfo->immediateBA    = Capabilities->immediateBA;

    return ;
}

/*
                                                                    
                                                  
 */
void limTdlsUpdateLinkReqPeerInfo(tpAniSirGlobal pMac, 
                                 tLimTdlsLinkSetupPeer *setupPeer, 
                                             tDot11fTDLSSetupReq *setupReq)
{

    /*                                             */

    tdlsUpdateCapInfo(&setupPeer->capabilityInfo, &setupReq->Capabilities) ;

    if(setupReq->SuppRates.present)
    {
        ConvertSuppRates( pMac, &setupPeer->supportedRates, 
                                            &setupReq->SuppRates );
    }

    /*                                                 */
    if(setupReq->QOSCapsStation.present)
    {
       ConvertQOSCapsStation(pMac->hHdd, &setupPeer->qosCaps, 
                   &setupReq->QOSCapsStation) ;
       LIM_LOG_TDLS(VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR,("setupReq->SPLen=%d (be %d %d %d %d vo) more %d qack %d."), \
         setupReq->QOSCapsStation.max_sp_length, setupReq->QOSCapsStation.acbe_uapsd, \
         setupReq->QOSCapsStation.acbk_uapsd, setupReq->QOSCapsStation.acvi_uapsd, \
         setupReq->QOSCapsStation.acvo_uapsd, setupReq->QOSCapsStation.more_data_ack, \
         setupReq->QOSCapsStation.qack));
    }
    
    if (setupReq->ExtSuppRates.present)
    {
        setupPeer->ExtRatesPresent = 1;
        ConvertExtSuppRates( pMac, &setupPeer->extendedRates,
                                                &setupReq->ExtSuppRates );
    }
    /*                */
    if (setupReq->HTCaps.present)
    {
        vos_mem_copy( &setupPeer->tdlsPeerHTCaps,
                    &setupReq->HTCaps, sizeof(tDot11fIEHTCaps)) ;
    }
    /*                 */
    if (setupReq->ExtCap.present)
    {
        vos_mem_copy( &setupPeer->tdlsPeerExtCaps,
                    &setupReq->ExtCap, sizeof(tDot11fIEExtCap)) ;
    }    

    return ;
}

/*
                                                 
 */
void limTdlsUpdateLinkRspPeerInfo(tpAniSirGlobal pMac, 
                                   tLimTdlsLinkSetupPeer *setupPeer, 
                                             tDot11fTDLSSetupRsp *setupRsp)
{

    /*                                             */
    tdlsUpdateCapInfo(&setupPeer->capabilityInfo, &setupRsp->Capabilities) ;

    if(setupRsp->SuppRates.present)
    {
        tDot11fIESuppRates *suppRates = &setupRsp->SuppRates ;
        ConvertSuppRates( pMac, &setupPeer->supportedRates, suppRates);
    }

    /*                                                 */
    if(setupRsp->QOSCapsStation.present)
    {
       ConvertQOSCapsStation(pMac->hHdd, &setupPeer->qosCaps, 
                   &setupRsp->QOSCapsStation) ;
       LIM_LOG_TDLS(VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR, ("setupRsp->SPLen=%d (be %d %d %d %d vo) more %d qack %d."), \
         setupRsp->QOSCapsStation.max_sp_length, setupRsp->QOSCapsStation.acbe_uapsd, \
         setupRsp->QOSCapsStation.acbk_uapsd, setupRsp->QOSCapsStation.acvi_uapsd, \
         setupRsp->QOSCapsStation.acvo_uapsd, setupRsp->QOSCapsStation.more_data_ack, \
         setupRsp->QOSCapsStation.qack));
    }
    
    if(setupRsp->ExtSuppRates.present)
    {
        setupPeer->ExtRatesPresent = 1;
        ConvertExtSuppRates( pMac, &setupPeer->extendedRates,
                                                &setupRsp->ExtSuppRates );
    }
    /*                */
    if (setupRsp->HTCaps.present)
    {
        vos_mem_copy(&setupPeer->tdlsPeerHTCaps,
                    &setupRsp->HTCaps, sizeof(tDot11fIEHTCaps)) ;
    }

    /*                 */
    if (setupRsp->ExtCap.present)
    {
        vos_mem_copy( &setupPeer->tdlsPeerExtCaps,
                    &setupRsp->ExtCap, sizeof(tDot11fIEExtCap)) ;
    }

    return ;
}
#endif

/*                                                                                  
                                                                                       
 */
static tSirRetStatus limTdlsPopulateDot11fHTCaps(tpAniSirGlobal pMac, tpPESession psessionEntry,
            tSirTdlsAddStaReq *pTdlsAddStaReq, tDot11fIEHTCaps *pDot11f)
{
    tANI_U32                         nCfgValue;
    tANI_U8                          nCfgValue8;
    tSirMacHTParametersInfo         *pHTParametersInfo;
    union {
        tANI_U16                        nCfgValue16;
        tSirMacHTCapabilityInfo         htCapInfo;
        tSirMacExtendedHTCapabilityInfo extHtCapInfo;
    } uHTCapabilityInfo;

    tSirMacTxBFCapabilityInfo       *pTxBFCapabilityInfo;
    tSirMacASCapabilityInfo         *pASCapabilityInfo;

    nCfgValue = pTdlsAddStaReq->htCap.capInfo;

    uHTCapabilityInfo.nCfgValue16 = nCfgValue & 0xFFFF;

    pDot11f->advCodingCap             = uHTCapabilityInfo.htCapInfo.advCodingCap;
    pDot11f->mimoPowerSave            = uHTCapabilityInfo.htCapInfo.mimoPowerSave;
    pDot11f->greenField               = uHTCapabilityInfo.htCapInfo.greenField;
    pDot11f->shortGI20MHz             = uHTCapabilityInfo.htCapInfo.shortGI20MHz;
    pDot11f->shortGI40MHz             = uHTCapabilityInfo.htCapInfo.shortGI40MHz;
    pDot11f->txSTBC                   = uHTCapabilityInfo.htCapInfo.txSTBC;
    pDot11f->rxSTBC                   = uHTCapabilityInfo.htCapInfo.rxSTBC;
    pDot11f->delayedBA                = uHTCapabilityInfo.htCapInfo.delayedBA;
    pDot11f->maximalAMSDUsize         = uHTCapabilityInfo.htCapInfo.maximalAMSDUsize;
    pDot11f->dsssCckMode40MHz         = uHTCapabilityInfo.htCapInfo.dsssCckMode40MHz;
    pDot11f->psmp                     = uHTCapabilityInfo.htCapInfo.psmp;
    pDot11f->stbcControlFrame         = uHTCapabilityInfo.htCapInfo.stbcControlFrame;
    pDot11f->lsigTXOPProtection       = uHTCapabilityInfo.htCapInfo.lsigTXOPProtection;

    //                                                  
    if (psessionEntry == NULL) //                           
    {
        pDot11f->supportedChannelWidthSet = uHTCapabilityInfo.htCapInfo.supportedChannelWidthSet;
    }
    else
    {
        pDot11f->supportedChannelWidthSet = psessionEntry->htSupportedChannelWidthSet;
    }

    /*                                                                    
                               */
    if(pDot11f->supportedChannelWidthSet == eHT_CHANNEL_WIDTH_20MHZ)
    {
       pDot11f->shortGI40MHz = 0;
    }

    dot11fLog(pMac, LOG2, FL("SupportedChnlWidth: %d, mimoPS: %d, GF: %d, shortGI20:%d, shortGI40: %d, dsssCck: %d"),
                                            pDot11f->supportedChannelWidthSet, pDot11f->mimoPowerSave,  pDot11f->greenField,
                                            pDot11f->shortGI20MHz, pDot11f->shortGI40MHz, pDot11f->dsssCckMode40MHz);

    nCfgValue = pTdlsAddStaReq->htCap.ampduParamsInfo;

    nCfgValue8 = ( tANI_U8 ) nCfgValue;
    pHTParametersInfo = ( tSirMacHTParametersInfo* ) &nCfgValue8;

    pDot11f->maxRxAMPDUFactor = pHTParametersInfo->maxRxAMPDUFactor;
    pDot11f->mpduDensity      = pHTParametersInfo->mpduDensity;
    pDot11f->reserved1        = pHTParametersInfo->reserved;

    dot11fLog( pMac, LOG2, FL( "AMPDU Param: %x" ), nCfgValue);

    vos_mem_copy( pDot11f->supportedMCSSet, pTdlsAddStaReq->htCap.suppMcsSet,
                  SIZE_OF_SUPPORTED_MCS_SET);

    nCfgValue = pTdlsAddStaReq->htCap.extendedHtCapInfo;

    uHTCapabilityInfo.nCfgValue16 = nCfgValue & 0xFFFF;

    pDot11f->pco            = uHTCapabilityInfo.extHtCapInfo.pco;
    pDot11f->transitionTime = uHTCapabilityInfo.extHtCapInfo.transitionTime;
    pDot11f->mcsFeedback    = uHTCapabilityInfo.extHtCapInfo.mcsFeedback;

    nCfgValue = pTdlsAddStaReq->htCap.txBFCapInfo;

    pTxBFCapabilityInfo = ( tSirMacTxBFCapabilityInfo* ) &nCfgValue;
    pDot11f->txBF                                       = pTxBFCapabilityInfo->txBF;
    pDot11f->rxStaggeredSounding                        = pTxBFCapabilityInfo->rxStaggeredSounding;
    pDot11f->txStaggeredSounding                        = pTxBFCapabilityInfo->txStaggeredSounding;
    pDot11f->rxZLF                                      = pTxBFCapabilityInfo->rxZLF;
    pDot11f->txZLF                                      = pTxBFCapabilityInfo->txZLF;
    pDot11f->implicitTxBF                               = pTxBFCapabilityInfo->implicitTxBF;
    pDot11f->calibration                                = pTxBFCapabilityInfo->calibration;
    pDot11f->explicitCSITxBF                            = pTxBFCapabilityInfo->explicitCSITxBF;
    pDot11f->explicitUncompressedSteeringMatrix         = pTxBFCapabilityInfo->explicitUncompressedSteeringMatrix;
    pDot11f->explicitBFCSIFeedback                      = pTxBFCapabilityInfo->explicitBFCSIFeedback;
    pDot11f->explicitUncompressedSteeringMatrixFeedback = pTxBFCapabilityInfo->explicitUncompressedSteeringMatrixFeedback;
    pDot11f->explicitCompressedSteeringMatrixFeedback   = pTxBFCapabilityInfo->explicitCompressedSteeringMatrixFeedback;
    pDot11f->csiNumBFAntennae                           = pTxBFCapabilityInfo->csiNumBFAntennae;
    pDot11f->uncompressedSteeringMatrixBFAntennae       = pTxBFCapabilityInfo->uncompressedSteeringMatrixBFAntennae;
    pDot11f->compressedSteeringMatrixBFAntennae         = pTxBFCapabilityInfo->compressedSteeringMatrixBFAntennae;

    nCfgValue = pTdlsAddStaReq->htCap.antennaSelectionInfo;

    nCfgValue8 = ( tANI_U8 ) nCfgValue;

    pASCapabilityInfo = ( tSirMacASCapabilityInfo* ) &nCfgValue8;
    pDot11f->antennaSelection         = pASCapabilityInfo->antennaSelection;
    pDot11f->explicitCSIFeedbackTx    = pASCapabilityInfo->explicitCSIFeedbackTx;
    pDot11f->antennaIndicesFeedbackTx = pASCapabilityInfo->antennaIndicesFeedbackTx;
    pDot11f->explicitCSIFeedback      = pASCapabilityInfo->explicitCSIFeedback;
    pDot11f->antennaIndicesFeedback   = pASCapabilityInfo->antennaIndicesFeedback;
    pDot11f->rxAS                     = pASCapabilityInfo->rxAS;
    pDot11f->txSoundingPPDUs          = pASCapabilityInfo->txSoundingPPDUs;

    pDot11f->present = pTdlsAddStaReq->htcap_present;

    return eSIR_SUCCESS;

}

tSirRetStatus
limTdlsPopulateDot11fVHTCaps(tpAniSirGlobal pMac,
                      tSirTdlsAddStaReq *pTdlsAddStaReq,
                      tDot11fIEVHTCaps  *pDot11f)
{
    tANI_U32             nCfgValue=0;
    union {
        tANI_U32                       nCfgValue32;
        tSirMacVHTCapabilityInfo       vhtCapInfo;
    } uVHTCapabilityInfo;
    union {
        tANI_U16                       nCfgValue16;
        tSirMacVHTTxSupDataRateInfo    vhtTxSupDataRateInfo;
        tSirMacVHTRxSupDataRateInfo    vhtRxsupDataRateInfo;
    } uVHTSupDataRateInfo;

    pDot11f->present = pTdlsAddStaReq->vhtcap_present;

    nCfgValue = pTdlsAddStaReq->vhtCap.vhtCapInfo;
    uVHTCapabilityInfo.nCfgValue32 = nCfgValue;

    pDot11f->maxMPDULen =  uVHTCapabilityInfo.vhtCapInfo.maxMPDULen;
    pDot11f->supportedChannelWidthSet =  uVHTCapabilityInfo.vhtCapInfo.supportedChannelWidthSet;
    pDot11f->ldpcCodingCap =  uVHTCapabilityInfo.vhtCapInfo.ldpcCodingCap;
    pDot11f->shortGI80MHz =  uVHTCapabilityInfo.vhtCapInfo.shortGI80MHz;
    pDot11f->shortGI160and80plus80MHz =  uVHTCapabilityInfo.vhtCapInfo.shortGI160and80plus80MHz;
    pDot11f->txSTBC =  uVHTCapabilityInfo.vhtCapInfo.txSTBC;
    pDot11f->rxSTBC =  uVHTCapabilityInfo.vhtCapInfo.rxSTBC;
    pDot11f->suBeamFormerCap =  uVHTCapabilityInfo.vhtCapInfo.suBeamFormerCap;
    pDot11f->suBeamformeeCap =  uVHTCapabilityInfo.vhtCapInfo.suBeamformeeCap;
    pDot11f->csnofBeamformerAntSup =  uVHTCapabilityInfo.vhtCapInfo.csnofBeamformerAntSup;
    pDot11f->numSoundingDim =  uVHTCapabilityInfo.vhtCapInfo.numSoundingDim;
    pDot11f->muBeamformerCap =  uVHTCapabilityInfo.vhtCapInfo.muBeamformerCap;
    pDot11f->muBeamformeeCap =  uVHTCapabilityInfo.vhtCapInfo.muBeamformeeCap;
    pDot11f->vhtTXOPPS =  uVHTCapabilityInfo.vhtCapInfo.vhtTXOPPS;
    pDot11f->htcVHTCap =  uVHTCapabilityInfo.vhtCapInfo.htcVHTCap;
    pDot11f->maxAMPDULenExp =  uVHTCapabilityInfo.vhtCapInfo.maxAMPDULenExp;
    pDot11f->vhtLinkAdaptCap =  uVHTCapabilityInfo.vhtCapInfo.vhtLinkAdaptCap;
    pDot11f->rxAntPattern =  uVHTCapabilityInfo.vhtCapInfo.rxAntPattern;
    pDot11f->txAntPattern =  uVHTCapabilityInfo.vhtCapInfo.txAntPattern;
    pDot11f->reserved1= uVHTCapabilityInfo.vhtCapInfo.reserved1;

    pDot11f->rxMCSMap = pTdlsAddStaReq->vhtCap.suppMcs.rxMcsMap;

    nCfgValue = pTdlsAddStaReq->vhtCap.suppMcs.rxHighest;
    uVHTSupDataRateInfo.nCfgValue16 = nCfgValue & 0xffff;
    pDot11f->rxHighSupDataRate = uVHTSupDataRateInfo.vhtRxsupDataRateInfo.rxSupDataRate;

    pDot11f->txMCSMap = pTdlsAddStaReq->vhtCap.suppMcs.txMcsMap;

    nCfgValue = pTdlsAddStaReq->vhtCap.suppMcs.txHighest;
    uVHTSupDataRateInfo.nCfgValue16 = nCfgValue & 0xffff;
    pDot11f->txSupDataRate = uVHTSupDataRateInfo.vhtTxSupDataRateInfo.txSupDataRate;

    pDot11f->reserved3= uVHTSupDataRateInfo.vhtTxSupDataRateInfo.reserved;

    limLogVHTCap(pMac, pDot11f);

    return eSIR_SUCCESS;

}

static tSirRetStatus
limTdlsPopulateMatchingRateSet(tpAniSirGlobal pMac,
                           tpDphHashNode pStaDs,
                           tANI_U8 *pSupportedRateSet,
                           tANI_U8 supporteRatesLength,
                           tANI_U8* pSupportedMCSSet,
                           tSirMacPropRateSet *pAniLegRateSet,
                           tpPESession  psessionEntry,
                           tDot11fIEVHTCaps *pVHTCaps)

{
    tSirMacRateSet    tempRateSet;
    tANI_U32          i,j,val,min,isArate;
    tSirMacRateSet    tempRateSet2;
    tANI_U32 phyMode;
    tANI_U8 mcsSet[SIZE_OF_SUPPORTED_MCS_SET];
    isArate=0;

    //                               
    limGetPhyMode(pMac, &phyMode, NULL);

    //                 
    val = WNI_CFG_OPERATIONAL_RATE_SET_LEN;
    if (wlan_cfgGetStr(pMac, WNI_CFG_OPERATIONAL_RATE_SET,
                                          (tANI_U8 *) &tempRateSet.rate,
                                          &val) != eSIR_SUCCESS)
    {
        //                                            
        limLog(pMac, LOGP, FL("could not retrieve rateset"));
        val = 0;
    }
    tempRateSet.numRates = val;

    if (phyMode == WNI_CFG_PHY_MODE_11G)
    {

        //                          
        val = WNI_CFG_EXTENDED_OPERATIONAL_RATE_SET_LEN;
        if (wlan_cfgGetStr(pMac, WNI_CFG_EXTENDED_OPERATIONAL_RATE_SET,
                                                  (tANI_U8 *) &tempRateSet2.rate,
                                                  &val) != eSIR_SUCCESS)
        tempRateSet2.numRates = val;
    }
    else
        tempRateSet2.numRates = 0;

    if ((tempRateSet.numRates + tempRateSet2.numRates) > 12)
    {
        PELOGE(limLog(pMac, LOGE, FL("more than 12 rates in CFG"));)
        goto error;
    }

    /* 
                                                         
                                                                          
                                                         
         */

    //                                                      
    for (i = 0; i < tempRateSet2.numRates; i++)
        tempRateSet.rate[i + tempRateSet.numRates] = tempRateSet2.rate[i];

    tempRateSet.numRates += tempRateSet2.numRates;

    /* 
                                                                           
                                         
         */
    tempRateSet2.numRates = 0;

    for (i = 0;i < tempRateSet.numRates; i++)
    {
        min = 0;
        val = 0xff;

        for(j = 0;j < tempRateSet.numRates; j++)
            if ((tANI_U32) (tempRateSet.rate[j] & 0x7f) < val)
            {
                val = tempRateSet.rate[j] & 0x7f;
                min = j;
            }

        tempRateSet2.rate[tempRateSet2.numRates++] = tempRateSet.rate[min];
        tempRateSet.rate[min] = 0xff;
    }

    /* 
                                                                 
                                                                                
                  
     */
    for (i = 0; i < supporteRatesLength; i++)
    {
        tempRateSet.rate[i] = pSupportedRateSet[i];
    }

    tempRateSet.numRates = supporteRatesLength;

    {
        tpSirSupportedRates  rates = &pStaDs->supportedRates;
        tANI_U8 aRateIndex = 0;
        tANI_U8 bRateIndex = 0;
        vos_mem_set( (tANI_U8 *) rates, sizeof(tSirSupportedRates), 0);

        for (i = 0;i < tempRateSet2.numRates; i++)
        {
            for (j = 0;j < tempRateSet.numRates; j++)
            {
                if ((tempRateSet2.rate[i] & 0x7F) ==
                    (tempRateSet.rate[j] & 0x7F))
                {
#ifdef FEATURE_WLAN_NON_INTEGRATED_SOC
                    if ((bRateIndex > HAL_NUM_11B_RATES) || (aRateIndex > HAL_NUM_11A_RATES))
                    {
                        limLog(pMac, LOGE, FL("Invalid number of rates (11b->%d, 11a->%d)"),
                               bRateIndex, aRateIndex);
                        return eSIR_FAILURE;
                    }
#endif
                    if (sirIsArate(tempRateSet2.rate[i] & 0x7f))
                    {
                        isArate=1;
                        rates->llaRates[aRateIndex++] = tempRateSet2.rate[i];
                    }
                    else
                        rates->llbRates[bRateIndex++] = tempRateSet2.rate[i];
                    break;
                }
            }
        }
    }


    //                                                                              
#ifdef FEATURE_WLAN_TDLS
    if (pStaDs->mlmStaContext.htCapability)
#else
    if (IS_DOT11_MODE_HT(psessionEntry->dot11mode) &&
       (pStaDs->mlmStaContext.htCapability))
#endif
    {
        val = SIZE_OF_SUPPORTED_MCS_SET;
        if (wlan_cfgGetStr(pMac, WNI_CFG_SUPPORTED_MCS_SET,
                           mcsSet,
                           &val) != eSIR_SUCCESS)
        {
            //                                            
            limLog(pMac, LOGP, FL("could not retrieve supportedMCSSet"));
            goto error;
        }

        for (i=0; i<val; i++)
            pStaDs->supportedRates.supportedMCSSet[i] = mcsSet[i] & pSupportedMCSSet[i];

        PELOG2(limLog(pMac, LOG2, FL("limPopulateMatchingRateSet: MCS Rate Set Bitmap from CFG and DPH :"));)
        for (i=0; i<SIR_MAC_MAX_SUPPORTED_MCS_SET; i++)
        {
            PELOG2(limLog(pMac, LOG2,FL("%x %x "), mcsSet[i], pStaDs->supportedRates.supportedMCSSet[i]);)
        }
    }

#ifdef WLAN_FEATURE_11AC
    limPopulateVhtMcsSet(pMac, &pStaDs->supportedRates, pVHTCaps, psessionEntry);
#endif
    /* 
                                                                       
                                  
         */
    if ((phyMode == WNI_CFG_PHY_MODE_11G) && isArate)
        pStaDs->erpEnabled = eHAL_SET;



    return eSIR_SUCCESS;

 error:

    return eSIR_FAILURE;
}

static int limTdlsSelectCBMode(tDphHashNode *pStaDs, tpPESession psessionEntry)
{
    tANI_U8 channel = psessionEntry->currentOperChannel;

    if ( pStaDs->mlmStaContext.vhtCapability )
    {
        if ( channel== 36 || channel == 52 || channel == 100 ||
             channel == 116 || channel == 149 )
        {
           return PHY_QUADRUPLE_CHANNEL_20MHZ_LOW_40MHZ_LOW - 1;
        }
        else if ( channel == 40 || channel == 56 || channel == 104 ||
             channel == 120 || channel == 153 )
        {
           return PHY_QUADRUPLE_CHANNEL_20MHZ_HIGH_40MHZ_LOW - 1;
        }
        else if ( channel == 44 || channel == 60 || channel == 108 ||
                  channel == 124 || channel == 157 )
        {
           return PHY_QUADRUPLE_CHANNEL_20MHZ_LOW_40MHZ_HIGH -1;
        }
        else if ( channel == 48 || channel == 64 || channel == 112 ||
             channel == 128 || channel == 161 )
        {
            return PHY_QUADRUPLE_CHANNEL_20MHZ_HIGH_40MHZ_HIGH - 1;
        }
        else if ( channel == 165 )
        {
            return 0;
        }
    }
    else if ( pStaDs->mlmStaContext.htCapability )
    {
        if ( channel== 40 || channel == 48 || channel == 56 ||
             channel == 64 || channel == 104 || channel == 112 ||
             channel == 120 || channel == 128 || channel == 136 ||
             channel == 144 || channel == 153 || channel == 161 )
        {
           return 1;
        }
        else if ( channel== 36 || channel == 44 || channel == 52 ||
             channel == 60 || channel == 100 || channel == 108 ||
             channel == 116 || channel == 124 || channel == 132 ||
             channel == 140 || channel == 149 || channel == 157 )
        {
           return 2;
        }
        else if ( channel == 165 )
        {
           return 0;
        }
    }
    return 0;
}

/*
                              
 */
static void limTdlsUpdateHashNodeInfo(tpAniSirGlobal pMac, tDphHashNode *pStaDs,
              tSirTdlsAddStaReq *pTdlsAddStaReq, tpPESession psessionEntry)
{
    //                                                          
    tDot11fIEHTCaps htCap, *htCaps;
    tDot11fIEVHTCaps *pVhtCaps = NULL;
#ifdef WLAN_FEATURE_11AC
    tDot11fIEVHTCaps vhtCap;
    tANI_U8 cbMode;
#endif
    tpDphHashNode pSessStaDs = NULL;
    tANI_U16 aid;

    if (pTdlsAddStaReq->tdlsAddOper == TDLS_OPER_ADD)
    {
        PopulateDot11fHTCaps(pMac, psessionEntry, &htCap);
    }
    else if (pTdlsAddStaReq->tdlsAddOper == TDLS_OPER_UPDATE)
    {
        limTdlsPopulateDot11fHTCaps(pMac, NULL, pTdlsAddStaReq, &htCap);
    }
    htCaps = &htCap;
    if (htCaps->present)
    {
        pStaDs->mlmStaContext.htCapability = 1 ;
        pStaDs->htGreenfield = htCaps->greenField ;
        pStaDs->htSupportedChannelWidthSet =  htCaps->supportedChannelWidthSet ;
        pStaDs->htMIMOPSState =             htCaps->mimoPowerSave ;
        pStaDs->htMaxAmsduLength =  htCaps->maximalAMSDUsize;
        pStaDs->htAMpduDensity =    htCaps->mpduDensity;
        pStaDs->htDsssCckRate40MHzSupport = htCaps->dsssCckMode40MHz ;
        pStaDs->htShortGI20Mhz = htCaps->shortGI20MHz;
        pStaDs->htShortGI40Mhz = htCaps->shortGI40MHz;
        pStaDs->htMaxRxAMpduFactor = htCaps->maxRxAMPDUFactor;
        limFillRxHighestSupportedRate(pMac, 
                             &pStaDs->supportedRates.rxHighestDataRate, 
                                                 htCaps->supportedMCSSet);
        pStaDs->baPolicyFlag = 0xFF;
        pMac->lim.gLimTdlsLinkMode = TDLS_LINK_MODE_N ;
    }
    else
    {
        pStaDs->mlmStaContext.htCapability = 0 ;
        pMac->lim.gLimTdlsLinkMode = TDLS_LINK_MODE_BG ;
    }
#ifdef WLAN_FEATURE_11AC
    limTdlsPopulateDot11fVHTCaps(pMac, pTdlsAddStaReq, &vhtCap);
    pVhtCaps = &vhtCap;
    if (pVhtCaps->present)
    {
        pStaDs->mlmStaContext.vhtCapability = 1 ;

        if ((psessionEntry->currentOperChannel <= SIR_11B_CHANNEL_END) &&
            pMac->roam.configParam.enableVhtFor24GHz)
        {
            pStaDs->vhtSupportedChannelWidthSet = WNI_CFG_VHT_CHANNEL_WIDTH_20_40MHZ;
            pStaDs->htSupportedChannelWidthSet = eHT_CHANNEL_WIDTH_20MHZ;
        }
        else
        {
            pStaDs->vhtSupportedChannelWidthSet =  WNI_CFG_VHT_CHANNEL_WIDTH_80MHZ;
            pStaDs->htSupportedChannelWidthSet = eHT_CHANNEL_WIDTH_40MHZ ;
        }

        pStaDs->vhtLdpcCapable = pVhtCaps->ldpcCodingCap;
        pStaDs->vhtBeamFormerCapable= pVhtCaps->suBeamFormerCap;
        //                                  
        pMac->lim.gLimTdlsLinkMode = TDLS_LINK_MODE_AC;
    }
    else
    {
        pStaDs->mlmStaContext.vhtCapability = 0 ;
        pStaDs->vhtSupportedChannelWidthSet = WNI_CFG_VHT_CHANNEL_WIDTH_20_40MHZ;
    }
#endif
    /*                                       */
    cbMode = limTdlsSelectCBMode(pStaDs, psessionEntry);

    pStaDs->htSecondaryChannelOffset = cbMode;

#ifdef WLAN_FEATURE_11AC
    if ( pStaDs->mlmStaContext.vhtCapability )
    {
        pStaDs->htSecondaryChannelOffset = limGetHTCBState(cbMode);
    }
#endif
    
    pSessStaDs = dphLookupHashEntry(pMac, psessionEntry->bssId, &aid, 
                                          &psessionEntry->dph.dphHashTable) ;

    /*                           */
    pStaDs->qosMode    = 1;
    pStaDs->wmeEnabled = 1;
    pStaDs->lleEnabled = 0;
    /*                                                       
     */
    pStaDs->qos.capability.qosInfo = (*(tSirMacQosInfoStation *) &pTdlsAddStaReq->uapsd_queues);

    /*                            */

    /*                                                                      
     */
    limTdlsPopulateMatchingRateSet(pMac, pStaDs, pTdlsAddStaReq->supported_rates,
                                   pTdlsAddStaReq->supported_rates_length,
                                   (tANI_U8 *)pTdlsAddStaReq->htCap.suppMcsSet,
                                   &pStaDs->mlmStaContext.propRateSet,
                                   psessionEntry, pVhtCaps);

    /*                                                                
     */
    pStaDs->mlmStaContext.capabilityInfo = ( *(tSirMacCapabilityInfo *) &pTdlsAddStaReq->capability);

    return ; 
}

#ifdef FEATURE_WLAN_TDLS_INTERNAL
/*
                                
 */
 
tANI_U8 limTdlsFindLinkPeer(tpAniSirGlobal pMac, tSirMacAddr peerMac, 
                                            tLimTdlsLinkSetupPeer  **setupPeer)
{
    tLimTdlsLinkSetupInfo *setupInfo = &pMac->lim.gLimTdlsLinkSetupInfo ;
    tLimTdlsLinkSetupPeer *linkSetupList = setupInfo->tdlsLinkSetupList ;
    tANI_U8 checkNode = TDLS_NODE_NOT_FOUND ; 

    while (linkSetupList != NULL)
    {
        if (vos_mem_compare((tANI_U8 *) peerMac,
                            (tANI_U8 *) linkSetupList->peerMac,
                            sizeof(tSirMacAddr)) )
        {
            checkNode = TDLS_NODE_FOUND ;
            *setupPeer = linkSetupList ;
            break ;
        }
        linkSetupList = linkSetupList->next;
    }

    return ((TDLS_NODE_FOUND ==  checkNode) ? eSIR_SUCCESS : eSIR_FAILURE ) ;
}

/*
                               
                                                                               
                                                                             
 */
tSirTdlsPeerInfo *limTdlsFindDisPeer(tpAniSirGlobal pMac, tSirMacAddr peerMac)
{
    tLimDisResultList *discoveryList = pMac->lim.gLimTdlsDisResultList ;
    tSirTdlsPeerInfo *peerInfo = NULL ;

    while (discoveryList != NULL)
    {
        peerInfo = &discoveryList->tdlsDisPeerInfo ;
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_INFO, 
         ("Peer in discovery list = " MAC_ADDRESS_STR),
          MAC_ADDR_ARRAY(peerInfo->peerMac));

        if (vos_mem_compare((tANI_U8 *) peerMac,
                       (tANI_U8 *) &peerInfo->peerMac, sizeof(tSirMacAddr)) )
        {
            break ;
        }
        discoveryList = discoveryList->next;
    }

    return peerInfo ;
}

/*
                                                          
                                                                               
                                                                             
 */
static tSirTdlsPeerInfo *limTdlsFindDisPeerByState(tpAniSirGlobal pMac, 
                                                                tANI_U8 state)
{
    tLimDisResultList *discoveryList = pMac->lim.gLimTdlsDisResultList ;
    tSirTdlsPeerInfo *peerInfo = NULL ;

    while (discoveryList != NULL)
    {
        peerInfo = &discoveryList->tdlsDisPeerInfo ;
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_INFO, 
                     ("peerInfo Mac = " MAC_ADDRESS_STR),
                      MAC_ADDR_ARRAY(peerInfo->peerMac));

        if (peerInfo->tdlsPeerState == state)
        {
            break ;
        }
        discoveryList = discoveryList->next;
    }

    return peerInfo ;
}

/*
                                                      
                                                                            
                                                                         
 */
static tANI_U8 limTdlsFindSetupPeerByState(tpAniSirGlobal pMac, tANI_U8 state, 
                                              tLimTdlsLinkSetupPeer **setupPeer)
{    

    tLimTdlsLinkSetupInfo *setupInfo = &pMac->lim.gLimTdlsLinkSetupInfo ;
    tLimTdlsLinkSetupPeer *linkSetupList = setupInfo->tdlsLinkSetupList ;
    tANI_U8 checkNode = TDLS_NODE_NOT_FOUND ; 

    while (linkSetupList != NULL)
    {
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_INFO, 
                 ("peer state = %02x"), (linkSetupList)->tdls_link_state) ;
        if((linkSetupList)->tdls_link_state == state) 
        {
            checkNode = TDLS_NODE_FOUND ;
            *setupPeer = linkSetupList ;
            break ;
        }
        linkSetupList = (linkSetupList)->next;
    }

    return ((TDLS_NODE_FOUND == checkNode) ? eSIR_SUCCESS: eSIR_FAILURE) ;
}


/*
                              
 */
void limTdlsDelLinkPeer(tpAniSirGlobal pMac, tSirMacAddr peerMac)
{
    tLimTdlsLinkSetupInfo *setupInfo = &pMac->lim.gLimTdlsLinkSetupInfo ;
    tLimTdlsLinkSetupPeer **linkSetupList = &setupInfo->tdlsLinkSetupList ;
    tLimTdlsLinkSetupPeer *currentNode = NULL ;
    tLimTdlsLinkSetupPeer *prevNode = NULL ;

    for(currentNode = *linkSetupList ; currentNode != NULL ;
                    prevNode = currentNode, currentNode = currentNode->next)
    {
        if (vos_mem_compare( (tANI_U8 *) peerMac,
                        (tANI_U8 *) currentNode->peerMac, 
                                                 sizeof(tSirMacAddr)) )
        {
            VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_INFO, 
                    ("Del Node for Peer = " MAC_ADDRESS_STR),
                     MAC_ADDR_ARRAY(currentNode->peerMac));
            /*                    */
            if(NULL == prevNode)
            {
                *linkSetupList = currentNode->next ;
            }
            else
            {
                prevNode->next = currentNode->next ;
            }
            vos_mem_free(currentNode) ;
            return ;
        }
    }
        
    return ;
}
   


/*
                                                             
 */
static tSirRetStatus limProcessTdlsDisReqFrame(tpAniSirGlobal pMac, 
                                    tANI_U8 *pBody, tANI_U32 frmLen )
{
    tDot11fTDLSDisReq tdlsDisReq = {{0}} ;
    tANI_U32 status = 0 ;
    tLimDisResultList *tdlsDisResult = NULL ; 
    tLimDisResultList **disResultList = &pMac->lim.gLimTdlsDisResultList ;
    tSirMacAddr peerMac = {0} ;
    tLimTdlsLinkSetupPeer *setupPeer = NULL ;
    tSirTdlsPeerInfo *peerInfo = NULL ;
    tpPESession psessionEntry = NULL ;
    tANI_U8 sessionId = 0 ;

    status = dot11fUnpackTDLSDisReq(pMac, pBody, frmLen, &tdlsDisReq) ;

    VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_WARN, 
            ("TDLS dis request dialog = %d"), tdlsDisReq.DialogToken.token);

    if ( DOT11F_FAILED( status ) )
    {
        limLog(pMac, LOGE, FL("Failed to parse TDLS discovery Request \
                              frame (0x%08x, %d bytes):"),status, frmLen);
        PELOG2(sirDumpBuf(pMac, SIR_DBG_MODULE_ID, LOG2, pBody, frmLen);)
        return eSIR_FAILURE;
    }
    else if ( DOT11F_WARNED( status ) )
    {
        limLog( pMac, LOGW, FL("There were warnings while unpacking an\
                      TDLS discovery Request frame (0x%08x," "%d bytes):"),
                   status, frmLen );
        PELOG2(sirDumpBuf(pMac, SIR_DBG_MODULE_ID, LOG2, pBody, frmLen);)
    }

    /*
                                                                       
                                                                          
                           
     */
    psessionEntry = peFindSessionByBssid(pMac, 
                         &tdlsDisReq.LinkIdentifier.bssid[0], &sessionId) ;
    if(NULL == psessionEntry)
    {
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR,
                 ("no Session entry for TDLS session (bssid "MAC_ADDR_ARRAY")"),
                  MAC_ADDR_ARRAY(tdlsDisReq.LinkIdentifier.bssid));

        //               
        return eSIR_FAILURE;
    }
 
    /*              */
    status = vos_mem_compare( &psessionEntry->bssId[0],
                    &tdlsDisReq.LinkIdentifier.bssid[0], sizeof(tSirMacAddr)) ;
    VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_INFO, 
            ("lim BSSID "MAC_ADDRESS_STR),
             MAC_ADDR_ARRAY( psessionEntry->bssId));

    VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_INFO, 
            ("Dis req from BSSID "MAC_ADDRESS_STR),
             MAC_ADDR_ARRAY(tdlsDisReq.LinkIdentifier.bssid));
    if(!status)
    {
        limLog( pMac, LOGE, FL("TDLS discovery request frame from other BSS -> something wrong. Check RXP filter")) ;

        return eSIR_FAILURE ; 
    }

    /*
                                                                 
                                                     
     */    
    status = vos_mem_compare( psessionEntry->selfMacAddr,
                    &tdlsDisReq.LinkIdentifier.InitStaAddr[0],
                                                     sizeof(tSirMacAddr)) ;
    if(status)
    {
        limLog( pMac, LOGE, FL("Echo of our TDLS discovery request frame")) ;
        return eSIR_FAILURE ; 
    }

    /*
                                                    
                                                 
                                                       
     */
    vos_mem_copy( peerMac,
                        &tdlsDisReq.LinkIdentifier.InitStaAddr[0], 
                                                     sizeof(tSirMacAddr)) ;
    /*                          */
    
    /*                                         */
    peerInfo = limTdlsFindDisPeer(pMac, peerMac) ;

    if(NULL == peerInfo)
    {    
        /*
                                                                         
                                                                          
                                                                          
                                      
         */
        tdlsDisResult = vos_mem_malloc(sizeof(tLimDisResultList));
        if ( NULL == tdlsDisResult )
        {
            limLog(pMac, LOGP, FL("alloc fail for TDLS discovery \
                                                    reponse info")) ;
            return eSIR_FAILURE ;
        }

 
        peerInfo = &tdlsDisResult->tdlsDisPeerInfo ;
        peerInfo->tdlsPeerState = TDLS_DIS_REQ_PROCESS_STATE ;
        peerInfo->dialog = tdlsDisReq.DialogToken.token ;

        peerInfo->sessionId = psessionEntry->peSessionId;
        
        /*                                             */
        vos_mem_copy( peerInfo->peerMac, peerMac, sizeof(tSirMacAddr)) ;

         /*
                                                                            
                                                                           
                                         
         */
        do
        {
            tDot11fIESuppRates suppRates = {0} ;
            tDot11fIEExtSuppRates extSuppRates = {0} ;
            tANI_U16 caps = 0 ;
            tDot11fFfCapabilities capsInfo = {0} ;
            tDot11fIEHTCaps HTCaps = {0} ;
            /*                            */
            PopulateDot11fSuppRates( pMac, POPULATE_DOT11F_RATES_OPERATIONAL, 
                                                  &suppRates, psessionEntry );
            ConvertSuppRates( pMac, &peerInfo->tdlsPeerSuppRates, 
                                                            &suppRates);
            /*                                   */
            PopulateDot11fExtSuppRates( pMac, POPULATE_DOT11F_RATES_OPERATIONAL,
                                &extSuppRates, psessionEntry );

            peerInfo->ExtRatesPresent = 1;
            ConvertExtSuppRates( pMac, &peerInfo->tdlsPeerExtRates, 
                                                          &extSuppRates);
 
            if(cfgGetCapabilityInfo(pMac, &caps, psessionEntry) != eSIR_SUCCESS)
            {
                /*
                                                   
                                       
                 */
                 limLog(pMac, LOGP,
                   FL("could not retrieve Capabilities value"));
            }
            swapBitField16(caps, ( tANI_U16* )&capsInfo );
            /*                  */
            tdlsUpdateCapInfo(&peerInfo->capabilityInfo, &capsInfo) ;

            PopulateDot11fHTCaps( pMac, psessionEntry, &HTCaps );
            limTdlsCovertHTCaps(pMac, peerInfo, &HTCaps) ;

        } while (0) ;
    
        /*                                                                */
        tdlsDisResult->next = *disResultList ;
        *disResultList = tdlsDisResult ;
        pMac->lim.gLimTdlsDisStaCount++ ; 

        /*                                                  */ 
        limTdlsFindLinkPeer(pMac, peerMac, &setupPeer) ;

        /* 
                                                                        
                                                              
         */ 
        if(NULL == setupPeer)
        {
            /*                                            */
            pMac->lim.gLimAddStaTdls = true ;
            peerInfo->delStaNeeded = true ;

            if(eSIR_FAILURE == limTdlsDisAddSta(pMac, peerMac, 
                                                     peerInfo, psessionEntry))
            {
                VOS_ASSERT(0) ;
                limLog(pMac, LOGE, "Add STA for dis response is failed ") ;
                return eSIR_FAILURE ;
            }
        } /*                                         */
        else
        {
            peerInfo->delStaNeeded = false ;
            limSendTdlsDisRspFrame(pMac, peerInfo->peerMac, peerInfo->dialog, psessionEntry) ;
            peerInfo->tdlsPeerState = TDLS_DIS_RSP_SENT_WAIT_STATE ;
        }

    }
    else
    {
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_INFO, 
                    ("discovery procedure in progress for this peer")) ;
    } 

    return eSIR_SUCCESS ;
}

/*                                  */

static tSirRetStatus limProcessTdlsSetupReqFrame(tpAniSirGlobal pMac, 
                                         tANI_U8 *pBody, tANI_U32 frmLen)
{

    tDot11fTDLSSetupReq tdlsSetupReq = {{0}} ;
    tANI_U32 status = 0 ;
    tpPESession psessionEntry = NULL ;
    tANI_U8 sessionId = 0 ;
    tANI_U8 currentState = TDLS_LINK_SETUP_WAIT_STATE ;
    tANI_U8 previousState = TDLS_LINK_IDLE_STATE ;
    /*                            */
    tLimTdlsLinkSetupInfo *linkSetupInfo = &pMac->lim.gLimTdlsLinkSetupInfo ;
    tLimTdlsLinkSetupPeer *setupPeer = NULL ;
    tLimTdlsLinkSetupPeer *tmpSetupPeer = NULL ;

    status = dot11fUnpackTDLSSetupReq(pMac, pBody, frmLen, &tdlsSetupReq) ;

    if ( DOT11F_FAILED( status ) )
    {
        limLog(pMac, LOGE, FL("Failed to parse TDLS discovery Request \
                              frame (0x%08x, %d bytes):"),status, frmLen);
        PELOG2(sirDumpBuf(pMac, SIR_DBG_MODULE_ID, LOG2, pBody, frmLen);)
        return eSIR_FAILURE;
    }
    else if ( DOT11F_WARNED( status ) )
    {
        limLog( pMac, LOGW, FL("There were warnings while unpacking an\
                      TDLS setup Request frame (0x%08x," "%d bytes):"),
                   status, pBody );
        PELOG2(sirDumpBuf(pMac, SIR_DBG_MODULE_ID, LOG2, pBody, frmLen);)
    }
    /*
                                                                       
                                                                          
                           
     */
    psessionEntry = peFindSessionByBssid(pMac, 
                         &tdlsSetupReq.LinkIdentifier.bssid[0], &sessionId) ;
    if(NULL == psessionEntry)
    {
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR,
                 ("no Session entry for TDLS session (bssid "
                  MAC_ADDRESS_STR")"),
                  MAC_ADDR_ARRAY(tdlsSetupReq.LinkIdentifier.bssid));

        //               
        return eSIR_FAILURE ;
    }
    /*                                                  */
    status = vos_mem_compare( psessionEntry->bssId,
                    &tdlsSetupReq.LinkIdentifier.bssid[0], 
                                                     sizeof(tSirMacAddr)) ;
     
    if(!status)
    {
        limLog( pMac, LOGE, FL("TDLS setup request frame from other BSS -> something wrong. Check RXP filter")) ;

        limSendTdlsSetupRspFrame(pMac, tdlsSetupReq.LinkIdentifier.InitStaAddr,
                                 tdlsSetupReq.DialogToken.token, psessionEntry,
                                 TDLS_SETUP_STATUS_FAILURE, NULL, 0 ) ;
        return eSIR_FAILURE ; 
    }

#ifdef FEATURE_WLAN_TDLS_NEGATIVE
    if(pMac->lim.gLimTdlsNegativeBehavior & LIM_TDLS_NEGATIVE_RSP_TIMEOUT_TO_SETUP_REQ) 
    {
        /*                                         */
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR, 
        ("TDLS negative running: ignore TDLS Setup Req packet"));
        return eSIR_SUCCESS ;
    }
    if(pMac->lim.gLimTdlsNegativeBehavior & LIM_TDLS_NEGATIVE_SEND_REQ_TO_SETUP_REQ)
    {
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR, 
        ("TDLS negative running: send TDLS Setup Req to peer TDLS Setup Req"));
        /*                                                     */
        limSendTdlsLinkSetupReqFrame(pMac, tdlsSetupReq.LinkIdentifier.InitStaAddr, tdlsSetupReq.DialogToken.token, psessionEntry,
            NULL, 0) ;
    }    
#endif
    /*                          */
    
    if(!limTdlsFindLinkPeer(pMac, 
                  &tdlsSetupReq.LinkIdentifier.InitStaAddr[0],
                                                  &tmpSetupPeer))
    {
        tANI_U32 tdlsStateStatus = TDLS_LINK_SETUP_START_STATE ;

        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR, 
                        ("Link is already setup with this peer" )) ;
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR, 
                        ("state = %d"), tmpSetupPeer->tdls_link_state) ;
        //                      

        if(tmpSetupPeer == NULL)
        {
            VOS_ASSERT(0) ;
            return eSIR_FAILURE ; 
            
        }
        switch(tmpSetupPeer->tdls_link_state)
        {

            case TDLS_LINK_SETUP_START_STATE:
            {
                v_SINT_t macCompare = 0 ;
                macCompare= vos_mem_compare2(tmpSetupPeer->peerMac, 
                           psessionEntry->selfMacAddr, sizeof(tSirMacAddr)) ;
                VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR, 
                        ("MAC comparison Rslt = %d"), macCompare ) ;
                if(0 > macCompare)
                {
                    /* 
                                                                          
                                                                    
                     */
                    /*                      */
                    tx_timer_deactivate(&tmpSetupPeer->gLimTdlsLinkSetupRspTimeoutTimer) ;
#ifdef FEATURE_WLAN_TDLS_NEGATIVE
                    if((pMac->lim.gLimTdlsNegativeBehavior & LIM_TDLS_NEGATIVE_SEND_REQ_TO_SETUP_REQ) 
                        != LIM_TDLS_NEGATIVE_SEND_REQ_TO_SETUP_REQ)
#endif
                    limSendSmeTdlsLinkStartRsp(pMac, eSIR_FAILURE, 
                            tmpSetupPeer->peerMac, eWNI_SME_TDLS_LINK_START_RSP);

                    limTdlsDelLinkPeer(pMac, tmpSetupPeer->peerMac) ;
                    tdlsStateStatus = TDLS_LINK_IDLE_STATE ;
                }
                else if(0 < macCompare)
                {
                    /* 
                                                                       
                                                 
                     */
                    tdlsStateStatus = TDLS_LINK_SETUP_START_STATE ;
                }
                else
                {
                    /*                        */
                    VOS_ASSERT(0) ;
                }
            
                break ;
            }
#if 1
            case TDLS_LINK_SETUP_DONE_STATE:
            {
                tpDphHashNode pStaDs = NULL ;

                previousState = TDLS_LINK_SETUP_WAIT_STATE ;
                currentState = TDLS_LINK_TEARDOWN_START_STATE ;
                VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR, 
                        ("link Setup Done state "  )) ;
                tmpSetupPeer->tdls_prev_link_state =  previousState ;
                tmpSetupPeer->tdls_link_state = currentState ;
                setupPeer = tmpSetupPeer ;
#if 0                
                /*                                                        */
                limSendTdlsTeardownFrame(pMac, 
                      &tdlsSetupReq.LinkIdentifier.InitStaAddr[0], 
                        eSIR_MAC_TDLS_TEARDOWN_UNSPEC_REASON, psessionEntry) ;
#else
                
                /*                                                              */
                limTdlsLinkTeardown(pMac, (setupPeer)->peerMac) ;

                /*                                                  */
                pStaDs = limTdlsDelSta(pMac, (setupPeer)->peerMac, psessionEntry) ;

                /*                                                            */

                if(pStaDs)
                {
                    limSendSmeTdlsDelPeerInd(pMac, psessionEntry->smeSessionId,
                                                           pStaDs, eSIR_SUCCESS) ;

                    /*                          */
                    limSendSmeTdlsTeardownRsp(pMac, eSIR_SUCCESS, (setupPeer)->peerMac,
                                                  eWNI_SME_TDLS_TEARDOWN_IND) ;
                    /*                             */
                    limTdlsDelLinkPeer(pMac, (setupPeer)->peerMac) ;
                }
#endif
                //                                                                
                tdlsStateStatus = TDLS_LINK_IDLE_STATE ;
                break ;

            }
            default:
            {
                VOS_ASSERT(0) ;
                VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR, 
                        ("link Setup is Recieved in unknown state" )) ;
                break ;
            }
#endif
        }
        if(tdlsStateStatus == TDLS_LINK_SETUP_START_STATE) 
            return eSIR_FAILURE ;
    }

    if(currentState != TDLS_LINK_TEARDOWN_START_STATE)
    {  
        /* 
                                                                              
                                                                             
                                                                   
         */
        setupPeer = vos_mem_malloc(sizeof( tLimTdlsLinkSetupPeer ));
        if ( NULL == setupPeer )
        {
            VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR, 
                                 ( "Unable to allocate memory during ADD_STA" ));
            return eSIR_MEM_ALLOC_FAILED;
        }

        setupPeer->dialog = tdlsSetupReq.DialogToken.token ;
        //                                                               
        //                                                         
        setupPeer->tdls_prev_link_state =  previousState ;
        setupPeer->tdls_link_state = currentState ;
        /*                                                */
        setupPeer->tdls_sessionId = psessionEntry->peSessionId;
        setupPeer->tdls_bIsResponder = 0;

        vos_mem_copy(setupPeer->peerMac,
                     &tdlsSetupReq.LinkIdentifier.InitStaAddr[0], 
                                                     sizeof(tSirMacAddr)) ;

        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_INFO, 
                   ("Setup REQ MAC = " MAC_ADDRESS_STR),
                    MAC_ADDR_ARRAY(setupPeer->peerMac));
 
        limTdlsUpdateLinkReqPeerInfo(pMac, setupPeer, &tdlsSetupReq) ;
        pMac->lim.gLimAddStaTdls = true ;

        /*                                            */
        if(eSIR_FAILURE == limTdlsSetupAddSta(pMac, setupPeer->peerMac, 
                                                  setupPeer, psessionEntry))
        {
            VOS_ASSERT(0) ;
            vos_mem_free((void **) &setupPeer) ;
            return eSIR_FAILURE ;
        }

        limSendTdlsSetupRspFrame(pMac, tdlsSetupReq.LinkIdentifier.InitStaAddr,
                                  tdlsSetupReq.DialogToken.token, psessionEntry,
                                  TDLS_SETUP_STATUS_SUCCESS, NULL, 0) ;

        limStartTdlsTimer(pMac, psessionEntry->peSessionId, 
                                  &setupPeer->gLimTdlsLinkSetupCnfTimeoutTimer,
                               (tANI_U32)setupPeer->peerMac,
                                 WNI_CFG_TDLS_LINK_SETUP_CNF_TIMEOUT,
                                   SIR_LIM_TDLS_LINK_SETUP_CNF_TIMEOUT) ;

        /*                        */
        setupPeer->next = linkSetupInfo->tdlsLinkSetupList ;
        linkSetupInfo->tdlsLinkSetupList = setupPeer ;
    }
    else
    {
        setupPeer->dialog = tdlsSetupReq.DialogToken.token ;
        //                                                               
        //                                                         
        setupPeer->tdls_prev_link_state =  previousState ;
        setupPeer->tdls_link_state = currentState ;
        /*                                                */
        setupPeer->tdls_sessionId = psessionEntry->peSessionId;
        setupPeer->tdls_bIsResponder = 0;

        vos_mem_copy( setupPeer->peerMac,
                     &tdlsSetupReq.LinkIdentifier.InitStaAddr[0], 
                                                     sizeof(tSirMacAddr)) ;

        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_INFO, 
                   ("Setup REQ MAC = "MAC_ADDRESS_STR),
                    MAC_ADDR_ARRAY(setupPeer->peerMac));
 
        limTdlsUpdateLinkReqPeerInfo(pMac, setupPeer, &tdlsSetupReq) ;
        limSendTdlsSetupRspFrame(pMac, tdlsSetupReq.LinkIdentifier.InitStaAddr, 
                                 tdlsSetupReq.DialogToken.token, psessionEntry,
                                 TDLS_SETUP_STATUS_SUCCESS, NULL, 0) ;

        limStartTdlsTimer(pMac, psessionEntry->peSessionId, 
                                  &setupPeer->gLimTdlsLinkSetupCnfTimeoutTimer,
                               (tANI_U32)setupPeer->peerMac,
                                 WNI_CFG_TDLS_LINK_SETUP_CNF_TIMEOUT,
                                   SIR_LIM_TDLS_LINK_SETUP_CNF_TIMEOUT) ;
    }
 
   
    return eSIR_SUCCESS ;

}

/*
                                                             
 */
static tSirRetStatus limProcessTdlsSetupRspFrame(tpAniSirGlobal pMac, 
                                            tANI_U8 *pBody, tANI_U32 frmLen )
{
    tDot11fTDLSSetupRsp tdlsSetupRsp = {{0}} ;
    tANI_U32 status = 0 ;
    tSirMacAddr peerMac = {0} ;
    tLimTdlsLinkSetupPeer *setupPeer = NULL ;
    tpPESession psessionEntry = NULL ;
    tANI_U8 sessionId = 0 ;

    status = dot11fUnpackTDLSSetupRsp(pMac, pBody, frmLen, &tdlsSetupRsp) ;

    if ( DOT11F_FAILED( status ) )
    {
        limLog(pMac, LOGE, FL("Failed to parse TDLS discovery Request \
                              frame (0x%08x, %d bytes):"),status, frmLen);
        PELOG2(sirDumpBuf(pMac, SIR_DBG_MODULE_ID, LOG2, pBody, frmLen);)
        return eSIR_FAILURE;
    }
    else if ( DOT11F_WARNED( status ) )
    {
        limLog( pMac, LOGW, FL("There were warnings while unpacking an\
                      TDLS discovery Request frame (0x%08x," "%d bytes):"),
                   status, frmLen );
        PELOG2(sirDumpBuf(pMac, SIR_DBG_MODULE_ID, LOG2, pBody, frmLen);)
    }

    /*
                                                                       
                                                                          
                           
     */
    psessionEntry = peFindSessionByBssid(pMac, 
                         &tdlsSetupRsp.LinkIdentifier.bssid[0], &sessionId) ;
    if(NULL == psessionEntry)
    {
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR,
                  ("no Session entry for TDLS session (bssid "
                  MAC_ADDRESS_STR")"),
                  MAC_ADDR_ARRAY(tdlsSetupRsp.LinkIdentifier.bssid));

        //               
        return eSIR_FAILURE;
    }
  
    /*              */
    status = vos_mem_compare( psessionEntry->bssId,
                    &tdlsSetupRsp.LinkIdentifier.bssid[0], 
                                                  sizeof(tSirMacAddr)) ;
     
    if(!status)
    {
        limLog( pMac, LOGE, FL("TDLS discovery request frame from other BSS -> something wrong. Check RXP filter")) ;

        VOS_ASSERT(0) ;
        return eSIR_FAILURE ; 
    }
    vos_mem_copy( peerMac,
                      &tdlsSetupRsp.LinkIdentifier.RespStaAddr[0], 
                                                     sizeof(tSirMacAddr)) ;

    VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_INFO, 
             ("TDLS setup RSP peer = "MAC_ADDRESS_STR), MAC_ADDR_ARRAY(peerMac));
    limTdlsFindLinkPeer(pMac, peerMac, &setupPeer) ;

    if(NULL == setupPeer)
    {
        limLog( pMac, LOGE, FL(" unknown setup Response frame \
                                                        other BSS")) ;
        return eSIR_FAILURE ;
    }
                                                
    VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_INFO, 
                                      ("deactivating Setup RSP timer")) ;

    /*                      */
    tx_timer_deactivate(&(setupPeer)->gLimTdlsLinkSetupRspTimeoutTimer) ;

    /*
                                                                     
                                         
     */
    if(TDLS_SETUP_STATUS_SUCCESS != tdlsSetupRsp.Status.status)
    {
        limTdlsDelLinkPeer(pMac, (setupPeer)->peerMac) ;
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_INFO, 
                                    ("setup RSP with Failure Code")) ;
        return eSIR_FAILURE ;
    }    
    
    /*                  */
    limTdlsUpdateLinkRspPeerInfo(pMac, setupPeer, &tdlsSetupRsp) ;
 
    /*                          */
    

    /* 
                                                                        
                                                                          
     */

    pMac->lim.gLimAddStaTdls = true ;
    if(eSIR_FAILURE == limTdlsSetupAddSta(pMac, peerMac,  
                                                 setupPeer, psessionEntry))
    {
       /*               */
       VOS_ASSERT(0) ;
       return eSIR_FAILURE ;
    } 
    /*                                                              */
    
         
    /*                                         */           
    limSendTdlsLinkSetupCnfFrame(pMac, peerMac, tdlsSetupRsp.DialogToken.token, psessionEntry, NULL, 0) ;

    /* 
                                                                     
                                                                          
                                                                      
                 
     */
    (setupPeer)->tdls_prev_link_state = (setupPeer)->tdls_link_state ;
    (setupPeer)->tdls_link_state = TDLS_LINK_SETUP_RSP_WAIT_STATE ;

    return eSIR_SUCCESS ;
}
/*
                                      
 */

static tSirRetStatus limProcessTdlsSetupCnfFrame(tpAniSirGlobal pMac, 
                                            tANI_U8 *pBody, tANI_U32 frmLen)
{
    tDot11fTDLSSetupCnf tdlsSetupCnf = {{0}} ;
    tANI_U32 status = 0 ;
    tLimTdlsLinkSetupPeer *setupPeer = NULL ;
    tpPESession psessionEntry = NULL ;
    tANI_U8 sessionId = 0 ;

    status = dot11fUnpackTDLSSetupCnf(pMac, pBody, frmLen, &tdlsSetupCnf) ;

    if ( DOT11F_FAILED( status ) )
    {
        limLog(pMac, LOGE, FL("Failed to parse an TDLS discovery Response \
                              frame (0x%08x, %d bytes):"),status, frmLen);
        PELOG2(sirDumpBuf(pMac, SIR_DBG_MODULE_ID, LOG2, pBody, frmLen);)
        return eSIR_FAILURE;
    }
    else if ( DOT11F_WARNED( status ) )
    {
        limLog( pMac, LOGW, FL("There were warnings while unpacking an\
                      TDLS discovery Response frame (0x%08x," "%d bytes):"),
                   status, frmLen );
        PELOG2(sirDumpBuf(pMac, SIR_DBG_MODULE_ID, LOG2, pBody, frmLen);)
    }
    /*
                                                                       
                                                                          
                           
     */
    psessionEntry = peFindSessionByBssid(pMac, 
                         &tdlsSetupCnf.LinkIdentifier.bssid[0], &sessionId) ;
    if(NULL == psessionEntry)
    {
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR,
                  ("no Session entry for TDLS session (bssid "
                  MAC_ADDRESS_STR")"),
                  MAC_ADDR_ARRAY(tdlsSetupCnf.LinkIdentifier.bssid));

        //               
        return eSIR_FAILURE;
    }
 
    /*              */
    status = vos_mem_compare( psessionEntry->bssId,
                    &tdlsSetupCnf.LinkIdentifier.bssid[0], 
                                                     sizeof(tSirMacAddr)) ;

    if(!status)
    {
        limLog( pMac, LOGE, FL("TDLS setup CNF frame other BSS -> something wrong. Check RXP filter")) ;

        VOS_ASSERT(0) ;
        return eSIR_FAILURE ; 
    }
    /*                          */
    VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_INFO, 
               ("setup Cnf peer MAc = "MAC_ADDRESS_STR),
                MAC_ADDR_ARRAY(tdlsSetupCnf.LinkIdentifier.InitStaAddr));
    
    limTdlsFindLinkPeer(pMac, 
                   &tdlsSetupCnf.LinkIdentifier.InitStaAddr[0],
                            &setupPeer) ;

    if(NULL == setupPeer)
    {
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_INFO, 
                                          (" unknown setup CNF frame")) ;
        VOS_ASSERT(0) ;
        return eSIR_FAILURE ;
    }
    VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_INFO, 
                   ("setup CNF peer MAC = "MAC_ADDRESS_STR),
                    MAC_ADDR_ARRAY((setupPeer)->peerMac));
    /*                                                */
    if((setupPeer)->dialog != tdlsSetupCnf.DialogToken.token)
    {
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR, 
                          ("setup CNF frame not matching with setup RSP")) ;
        VOS_ASSERT(0) ;
        return eSIR_FAILURE ;
    }

    /* 
                                                              
                          
     */
    tx_timer_deactivate(&(setupPeer)->gLimTdlsLinkSetupCnfTimeoutTimer) ;

    /*                        */
    (setupPeer)->tdls_prev_link_state = (setupPeer)->tdls_link_state ;
    (setupPeer)->tdls_link_state = TDLS_LINK_SETUP_DONE_STATE ; 

    /*                                                */
    limSendSmeTdlsLinkSetupInd(pMac, (setupPeer)->peerMac, eSIR_SUCCESS) ;

    /*                                                     */
    limTdlsLinkEstablish(pMac, (setupPeer)->peerMac);

    return eSIR_SUCCESS ; 

}

/*
                                              
 */

static tSirRetStatus limProcessTdlsDisRspFrame(tpAniSirGlobal pMac, 
                              tANI_U8 *pBody, tANI_U32 frmLen, 
                                 tANI_S8 rssi, tpPESession psessionEntry)
{
    tDot11fTDLSDisRsp tdlsDisRsp = {{0}} ;
    tANI_U32 status = 0 ;
    tLimDisResultList *tdlsDisResult = NULL ; 
    tLimDisResultList **disResultList = &pMac->lim.gLimTdlsDisResultList ;
    tSirTdlsDisReq *prevDisReq = &pMac->lim.gLimTdlsDisReq ;

    status = dot11fUnpackTDLSDisRsp(pMac, pBody, frmLen, &tdlsDisRsp) ;

    if ( DOT11F_FAILED( status ) )
    {
        limLog(pMac, LOGE, FL("Failed to parse an TDLS discovery Response \
                              frame (0x%08x, %d bytes):"),status, frmLen);
        PELOG2(sirDumpBuf(pMac, SIR_DBG_MODULE_ID, LOG2, pBody, frmLen);)
        return eSIR_FAILURE;
    }
    else if ( DOT11F_WARNED( status ) )
    {
        limLog( pMac, LOGW, FL("There were warnings while unpacking an\
                      TDLS discovery Response frame (0x%08x," "%d bytes):"),
                   status, frmLen );
        PELOG2(sirDumpBuf(pMac, SIR_DBG_MODULE_ID, LOG2, pFrame, nFrame);)
    }
    /*                                                     */

    /*              */
    status = vos_mem_compare( psessionEntry->bssId,
                    &tdlsDisRsp.LinkIdentifier.bssid[0], 
                                                     sizeof(tSirMacAddr)) ;

    if(!status)
    {
        limLog( pMac, LOGW, FL(" TDLS discovery Response frame other BSS")) ;
        return eSIR_FAILURE ; 
    }
    /*                          */
  
    if(tdlsDisRsp.DialogToken.token != prevDisReq->dialog)
    {
        limLog( pMac, LOGW, FL(" wrong TDLS discovery Response frame")) ;
        return eSIR_FAILURE ;
    } 

    pMac->lim.gLimTdlsDisStaCount++ ;

    /*
                                                                     
                                                                      
                                                                      
                                  
     */
    tdlsDisResult = vos_mem_malloc(sizeof(tLimDisResultList));
    if ( NULL == tdlsDisResult )
    {
        limLog(pMac, LOGP, FL("alloc fail for TDLS discovery reponse info")) ;
        return eSIR_FAILURE ;
    }

    do
    {
        tSirTdlsPeerInfo *peerInfo = &tdlsDisResult->tdlsDisPeerInfo ;

        /*                                             */
        peerInfo->sessionId = psessionEntry->peSessionId;
        /*
                                                 
                                                                         
                              
         */
        vos_mem_copy( peerInfo->peerMac,
                    &tdlsDisRsp.LinkIdentifier.RespStaAddr[0], 
                                                     sizeof(tSirMacAddr)) ;

        /*                                    */
        peerInfo->tdlsPeerRssi = rssi ;

        /*                  */
        tdlsUpdateCapInfo(&peerInfo->capabilityInfo,
                                          &tdlsDisRsp.Capabilities) ;

        /*                   */
        if(tdlsDisRsp.SuppRates.present)
        { 
            ConvertSuppRates( pMac, &peerInfo->tdlsPeerSuppRates, 
                                             &tdlsDisRsp.SuppRates );
        }

        /*                       */
        if(tdlsDisRsp.ExtSuppRates.present) 
        {
            peerInfo->ExtRatesPresent = 1;
            ConvertExtSuppRates( pMac, &peerInfo->tdlsPeerExtRates, 
                                                    &tdlsDisRsp.ExtSuppRates );
        }
        /*                */
        if (tdlsDisRsp.HTCaps.present)
        {
            vos_mem_copy( &peerInfo->tdlsPeerHtCaps, &tdlsDisRsp.HTCaps,
                                               sizeof( tDot11fIEHTCaps ) );
        }
    } while(0) ;

    /*                                                                */
    tdlsDisResult->next = *disResultList ;
    *disResultList = tdlsDisResult ; 

    return eSIR_SUCCESS ; 
}

/* 
                                                         
 */
static tSirRetStatus limProcessTdlsTeardownFrame(tpAniSirGlobal pMac, 
                                      tANI_U8 *pBody, tANI_U32 frmLen )
{
    tDot11fTDLSTeardown tdlsTeardown = {{0}} ;
    tANI_U32 status = 0 ;
    tLimTdlsLinkSetupPeer *setupPeer = NULL ;
    tpPESession psessionEntry = NULL ;
    tANI_U8 sessionId = 0 ;

    status = dot11fUnpackTDLSTeardown(pMac, pBody, frmLen, &tdlsTeardown) ;

    if ( DOT11F_FAILED( status ) )
    {
        limLog(pMac, LOGE, FL("Failed to parse an TDLS discovery Response \
                              frame (0x%08x, %d bytes):"),status, frmLen);
        PELOG2(sirDumpBuf(pMac, SIR_DBG_MODULE_ID, LOG2, pBody, frmLen);)
        return eSIR_FAILURE;
    }
    else if ( DOT11F_WARNED( status ) )
    {
        limLog( pMac, LOGW, FL("There were warnings while unpacking an\
                      TDLS discovery Response frame (0x%08x," "%d bytes):"),
                   status, frmLen );
        PELOG2(sirDumpBuf(pMac, SIR_DBG_MODULE_ID, LOG2, pBody, frmLen);)
    }

    /*
                                                                       
                                                                          
                           
     */
    psessionEntry = peFindSessionByBssid(pMac, 
                         &tdlsTeardown.LinkIdentifier.bssid[0], &sessionId) ;
    if(NULL == psessionEntry)
    {
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR,
                  ("no Session entry for TDLS session (bssid "
                  MAC_ADDRESS_STR")"),
                  MAC_ADDR_ARRAY(tdlsTeardown.LinkIdentifier.bssid));

        //               
        return eSIR_FAILURE;
    }
 
    /*              */
    status = vos_mem_compare( psessionEntry->bssId,
                                  &tdlsTeardown.LinkIdentifier.bssid[0], 
                                                     sizeof(tSirMacAddr)) ;


    if(!status)
    {
        limLog( pMac, LOGE, FL("Teardown from other BSS -> something wrong. Check RXP filter")) ;
        VOS_ASSERT(0) ;
        return eSIR_FAILURE ; 
    }
    
    limTdlsFindLinkPeer(pMac, 
                     &tdlsTeardown.LinkIdentifier.InitStaAddr[0],
                                            &setupPeer) ;

    if(NULL == setupPeer)
    {
        //      
        //               
        limLog( pMac, LOGE, FL("Teardown from unknown peer. --> ignored") );
        
        return eSIR_FAILURE ;
    }
    VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_INFO, 
                     ("teardown for peer "MAC_ADDRESS_STR),
                          MAC_ADDR_ARRAY((setupPeer)->peerMac));

    switch(tdlsTeardown.Reason.code)
    {
        case eSIR_MAC_TDLS_TEARDOWN_UNSPEC_REASON:
        {
            VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_INFO, 
                                 ("teardown with unspecified reason")) ;
            break ;
        }
        case eSIR_MAC_TDLS_TEARDOWN_PEER_UNREACHABLE:
        {
            VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_INFO, 
                       (" Teardown from AP, TDLS peer unreachable")) ;
            break ;
        }
        default:
        {
            VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_INFO, 
                                            (" unknown teardown")) ;
            break ;
        }
    }

    /*                        */
    (setupPeer)->tdls_prev_link_state = (setupPeer)->tdls_link_state ;
    (setupPeer)->tdls_link_state = TDLS_LINK_TEARDOWN_START_STATE ; 

    do
    {
        tpDphHashNode pStaDs = NULL ;

        /*                                                              */
        limTdlsLinkTeardown(pMac, (setupPeer)->peerMac) ;

        /*                                                  */
        pStaDs = limTdlsDelSta(pMac, (setupPeer)->peerMac, psessionEntry) ;

        /*                                                            */

        if(pStaDs)
        {
            limSendSmeTdlsDelPeerInd(pMac, psessionEntry->smeSessionId,
                                                   pStaDs, eSIR_SUCCESS) ;
      
            /*                          */
            limSendSmeTdlsTeardownRsp(pMac, eSIR_SUCCESS, (setupPeer)->peerMac,
                                          eWNI_SME_TDLS_TEARDOWN_IND) ;
            /*                             */
            limTdlsDelLinkPeer(pMac, (setupPeer)->peerMac) ;
        }

    }while(0) ;
    
    return status ;
}

/*
                                                    
 */
void limProcessTdlsFrame(tpAniSirGlobal pMac, tANI_U32 *pBd)
{
    tANI_U8 *pBody = WDA_GET_RX_MPDU_DATA(pBd);
    tANI_U8 pOffset = ((0 == WDA_GET_RX_FT_DONE(pBd)) 
                         ? (( sizeof( eth_890d_header ))) :(0)) ;

    tANI_U8 category   = (pBody + pOffset + PAYLOAD_TYPE_TDLS_SIZE)[0] ; 
    tANI_U8 action     =   (pBody + pOffset + PAYLOAD_TYPE_TDLS_SIZE)[1] ; 
    tANI_U32 frameLen  = WDA_GET_RX_PAYLOAD_LEN(pBd) ;
    tANI_U8 *tdlsFrameBody = (pBody + pOffset + PAYLOAD_TYPE_TDLS_SIZE) ;
    //                                                   

    if(category != SIR_MAC_ACTION_TDLS)
    {
        limLog( pMac, LOGE, FL("Invalid TDLS action frame=(%d). Ignored"), category );
        return ; 
    }

    frameLen -= (pOffset + PAYLOAD_TYPE_TDLS_SIZE) ;
    LIM_LOG_TDLS(VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR, ("Received TDLS action %d (%s)"), \
        action, limTraceTdlsActionString(action) ));

    switch(action)
    {

        case SIR_MAC_TDLS_SETUP_REQ:
        {
            limProcessTdlsSetupReqFrame(pMac, tdlsFrameBody, frameLen) ;
            break ;
        }
        case SIR_MAC_TDLS_SETUP_RSP:
        {
            limProcessTdlsSetupRspFrame(pMac, tdlsFrameBody, frameLen) ;
            break ;
        }
        case SIR_MAC_TDLS_SETUP_CNF:
        {
            limProcessTdlsSetupCnfFrame(pMac, tdlsFrameBody, frameLen) ; 
            break ;
        }
        case SIR_MAC_TDLS_TEARDOWN: 
        {
            limProcessTdlsTeardownFrame(pMac, tdlsFrameBody, frameLen) ; 
            break ;
        }
        case SIR_MAC_TDLS_DIS_REQ:
        {
            limProcessTdlsDisReqFrame(pMac, tdlsFrameBody, frameLen) ;
            break ;
        }
        case SIR_MAC_TDLS_PEER_TRAFFIC_IND:
        case SIR_MAC_TDLS_CH_SWITCH_REQ:      
        case SIR_MAC_TDLS_CH_SWITCH_RSP:    
        case SIR_MAC_TDLS_PEER_TRAFFIC_RSP:
        default:
        {
            break ;
        }
    }
    
    return ;    
}

/*
                                                    
 */
static tSirRetStatus limTdlsDisAddSta(tpAniSirGlobal pMac, tSirMacAddr peerMac, 
                          tSirTdlsPeerInfo *peerInfo, tpPESession psessionEntry)
{
    tpDphHashNode pStaDs = NULL ;
    tSirRetStatus status = eSIR_SUCCESS ;
    tANI_U16 aid = 0 ;

    if(NULL == peerInfo)
    {
        VOS_ASSERT(0) ;
        return status ;

    } 
    VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_INFO, 
               ("ADD STA peer MAC: "MAC_ADDRESS_STR),
                MAC_ADDR_ARRAY(peerMac));


    if(NULL != dphLookupHashEntry(pMac, peerMac, 
                                  &aid, &psessionEntry->dph.dphHashTable))
    {
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR, 
                    (" there is hash entry for this client")) ;
        status = eSIR_FAILURE ;
        VOS_ASSERT(0) ;
        return status ;
    }

    aid = limAssignPeerIdx(pMac, psessionEntry) ;

    /*                                                                   */
    SET_PEER_AID_BITMAP(psessionEntry->peerAIDBitmap, aid);

    pStaDs = dphGetHashEntry(pMac, aid, &psessionEntry->dph.dphHashTable);

    if (pStaDs)
    {
        (void) limDelSta(pMac, pStaDs, false /*            */, psessionEntry);
        limDeleteDphHashEntry(pMac, pStaDs->staAddr, aid, psessionEntry);
    }
    pStaDs = dphAddHashEntry(pMac, peerMac, aid, 
                                         &psessionEntry->dph.dphHashTable) ;

    if(NULL == pStaDs)
    {
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR, 
                    (" add hash entry failed")) ;
        status = eSIR_FAILURE ;
        VOS_ASSERT(0) ;
        return status;
    }
    if(eSIR_SUCCESS == status)
    {
#ifdef TDLS_RATE_DEBUG
        tSirMacRateSet *suppRates = &peerInfo->tdlsPeerSuppRates ;
        tSirMacRateSet *extRates = &peerInfo->tdlsPeerExtRates ;
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_INFO, 
                                  ("pSta DS [%p] "), pStaDs) ;
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_INFO, 
                   ("peerInfo->tdlsPeerSuppRates = [%p]"),
                        (tANI_U8 *)&peerInfo->tdlsPeerSuppRates) ;
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_INFO, 
               ("peerInfo->tdlsPeerExtRates = [%p]"),
                        (tANI_U8 *)&peerInfo->tdlsPeerExtRates) ;
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_INFO, 
              ("peerInfo->tdlsPeerPropRates = [%p]"),
                        (tANI_U8 *)&pStaDs->mlmStaContext.propRateSet) ;
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_INFO, 
              ("peerInfo->mcs = [%p]"),
                        (tANI_U8 *)peerInfo->supportedMCSSet) ;
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_INFO, 
                  ("num of supp rates = %02x"), suppRates->numRates) ;
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_INFO, 
                      ("num of ext rates = %01x"), extRates->numRates) ;
#endif

        /*                            */
#ifdef WLAN_FEATURE_11AC
        if(eSIR_FAILURE == limPopulateMatchingRateSet(pMac, pStaDs, 
                                    &peerInfo->tdlsPeerSuppRates,
                                      &peerInfo->tdlsPeerExtRates, 
                                        peerInfo->supportedMCSSet,
                                         &pStaDs->mlmStaContext.propRateSet, 
                                                              psessionEntry, NULL))
#else
        if(eSIR_FAILURE == limPopulateMatchingRateSet(pMac, pStaDs, 
                                    &peerInfo->tdlsPeerSuppRates,
                                      &peerInfo->tdlsPeerExtRates, 
                                        peerInfo->supportedMCSSet,
                                         &pStaDs->mlmStaContext.propRateSet, 
                                                              psessionEntry))
#endif
        {
            VOS_ASSERT(0) ;
        }


        pStaDs->mlmStaContext.capabilityInfo = peerInfo->capabilityInfo;
        vos_mem_copy( pStaDs->staAddr, peerMac, sizeof(tSirMacAddr)) ;
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_INFO,
                ("Add STA for Peer: "MAC_ADDRESS_STR),
                 MAC_ADDR_ARRAY(pStaDs->staAddr));
    

        pStaDs->staType = STA_ENTRY_TDLS_PEER ;

        status = limAddSta(pMac, pStaDs, false, psessionEntry);

        if(eSIR_SUCCESS != status)
        {
            /*                 */
            VOS_ASSERT(0) ;
        }
    }
  
    return status ;
}
#endif
/*
                                    
 */ 
static tSirRetStatus limTdlsSetupAddSta(tpAniSirGlobal pMac,
                                        tSirTdlsAddStaReq *pAddStaReq,
                                        tpPESession psessionEntry)
{
    tpDphHashNode pStaDs = NULL ;
    tSirRetStatus status = eSIR_SUCCESS ;
    tANI_U16 aid = 0 ;

    pStaDs = dphLookupHashEntry(pMac, pAddStaReq->peerMac, &aid,
                                      &psessionEntry->dph.dphHashTable);
    if(NULL == pStaDs)
    {
        aid = limAssignPeerIdx(pMac, psessionEntry) ;

        if( !aid )
        {
            VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR,
              ("%s: No more free AID for peer " MAC_ADDRESS_STR),
                __func__, MAC_ADDR_ARRAY(pAddStaReq->peerMac)) ;
            return eSIR_FAILURE;
        }

        /*                                                                   */
        SET_PEER_AID_BITMAP(psessionEntry->peerAIDBitmap, aid);

        VOS_TRACE(VOS_MODULE_ID_PE, TDLS_DEBUG_LOG_LEVEL,
              ("limTdlsSetupAddSta: Aid = %d, for peer =" MAC_ADDRESS_STR),
                aid, MAC_ADDR_ARRAY(pAddStaReq->peerMac));
        pStaDs = dphGetHashEntry(pMac, aid, &psessionEntry->dph.dphHashTable);

        if (pStaDs)
        {
            (void) limDelSta(pMac, pStaDs, false /*            */, psessionEntry);
            limDeleteDphHashEntry(pMac, pStaDs->staAddr, aid, psessionEntry);
        }

        pStaDs = dphAddHashEntry(pMac, pAddStaReq->peerMac, aid,
                                             &psessionEntry->dph.dphHashTable) ;

        if(NULL == pStaDs)
        {
            VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR,
                        (" add hash entry failed")) ;
            VOS_ASSERT(0) ;
            return eSIR_FAILURE;
        }
    }

    limTdlsUpdateHashNodeInfo(pMac, pStaDs, pAddStaReq, psessionEntry) ;

    pStaDs->staType = STA_ENTRY_TDLS_PEER ;

    status = limAddSta(pMac, pStaDs, (pAddStaReq->tdlsAddOper == TDLS_OPER_UPDATE) ? true: false, psessionEntry);

    if(eSIR_SUCCESS != status)
    {
        /*                 */
        VOS_ASSERT(0) ;
    }
    return status ;
}

/*
                                                                            
 */
static tpDphHashNode limTdlsDelSta(tpAniSirGlobal pMac, tSirMacAddr peerMac, 
                                                    tpPESession psessionEntry)
{
    tSirRetStatus status = eSIR_SUCCESS ;
    tANI_U16 peerIdx = 0 ;
    tpDphHashNode pStaDs = NULL ;
 
    pStaDs = dphLookupHashEntry(pMac, peerMac, &peerIdx, 
                                         &psessionEntry->dph.dphHashTable) ;

    if(pStaDs)
    {
    
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_INFO, 
             ("DEL STA peer MAC: "MAC_ADDRESS_STR),
                                  MAC_ADDR_ARRAY(pStaDs->staAddr));

        VOS_TRACE(VOS_MODULE_ID_PE, TDLS_DEBUG_LOG_LEVEL,
                   ("limTdlsDelSta: STA type = %x, sta idx = %x"),pStaDs->staType,
                                                           pStaDs->staIndex) ;
 
        status = limDelSta(pMac, pStaDs, false, psessionEntry) ;
#ifdef FEATURE_WLAN_TDLS_INTERNAL
        if(eSIR_SUCCESS == status)
        {
            limDeleteDphHashEntry(pMac, pStaDs->staAddr, peerIdx, psessionEntry) ;
            limReleasePeerIdx(pMac, peerIdx, psessionEntry) ;
        }
        else
        {
            VOS_ASSERT(0) ;
        }
#endif
    }
           
    return pStaDs ;
}
     
#ifdef FEATURE_WLAN_TDLS_INTERNAL
/* 
                                                                 
 
*/   
static tSirRetStatus limTdlsLinkEstablish(tpAniSirGlobal pMac, tSirMacAddr peerMac)
{
    tANI_U8             pFrame[64] ;
    tDot11fTDLSPeerTrafficInd tdlsPtiTemplate ;
    tANI_U32            status = 0 ;
    tANI_U32            nPayload = 0 ;
    tANI_U32            nBytes = 0 ;
    tANI_U32            header_offset = 0 ;
    tANI_U16            aid = 0 ;
    tDphHashNode        *pStaDs = NULL ;
    tLimTdlsLinkSetupPeer *setupPeer = NULL ;
    tpPESession psessionEntry = NULL ;


    limTdlsFindLinkPeer(pMac, peerMac, &setupPeer) ;
    if(NULL == setupPeer) {
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR,
            ("limTdlsLinkEstablish: cannot find peer mac "
             "in tdls linksetup list: "MAC_ADDRESS_STR),
             MAC_ADDR_ARRAY(peerMac));
        return eSIR_FAILURE;
    }

    psessionEntry = peFindSessionBySessionId(pMac, 
                         setupPeer->tdls_sessionId) ;

    if(NULL == psessionEntry) 
    {
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR, 
             ("limTdlsLinkEstablish: sessionID %d is not found"), setupPeer->tdls_sessionId);
        VOS_ASSERT(0) ;
        return eHAL_STATUS_FAILURE;
    }

    /* */
    pStaDs = dphLookupHashEntry(pMac, peerMac, &aid, &psessionEntry->dph.dphHashTable) ;
    if(pStaDs == NULL) {
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR,
                  ("limTdlsLinkEstablish: cannot find peer mac "
                   "in tdls linksetup list: "MAC_ADDRESS_STR),
                   MAC_ADDR_ARRAY(peerMac));
        return eSIR_FAILURE;
    }

    vos_mem_set( ( tANI_U8* )&tdlsPtiTemplate,
               sizeof( tDot11fTDLSPeerTrafficInd ), 0 );

    /*
                         
    */
    tdlsPtiTemplate.Category.category = SIR_MAC_ACTION_TDLS;
    tdlsPtiTemplate.Action.action     = SIR_MAC_TDLS_PEER_TRAFFIC_IND;
    tdlsPtiTemplate.DialogToken.token = 0 ; /*                                                */
#if 1 
    /*                                                                                                              
                                      */
    PopulateDot11fLinkIden( pMac, psessionEntry, &tdlsPtiTemplate.LinkIdentifier,
        peerMac, !setupPeer->tdls_bIsResponder) ;
#else
   /*                                                                           */
    PopulateDot11fLinkIden( pMac, psessionEntry, &tdlsPtiTemplate.LinkIdentifier,
        peerMac, TDLS_INITIATOR) ;
#endif

    /*                                                                       */
    tdlsPtiTemplate.PUBufferStatus.present = 1;

    /*                                   */

    /* 
                                                                  
    */
    status = dot11fGetPackedTDLSPeerTrafficIndSize ( pMac, &tdlsPtiTemplate, &nPayload);
    if ( DOT11F_FAILED( status ) )
    {
        limLog( pMac, LOGP, FL("Failed to calculate the packed size for a PTI template (0x%08x)."), status );
        /*                                             */
        nPayload = sizeof( tdlsPtiTemplate );
    }
    else if ( DOT11F_WARNED( status ) )
    {
        limLog( pMac, LOGW, FL("There were warnings while calculating the packed size for a PTI template (0x%08x)."), status );
    }

    /*
                                                                           
            
                                
    */ 

    nBytes = nPayload + sizeof( tSirMacMgmtHdr ) 
            + sizeof( eth_890d_header ) 
            + PAYLOAD_TYPE_TDLS_SIZE ;

    if(nBytes > 64) {
        limLog( pMac, LOGE, FL("required memory for PTI frame is %ld, but reserved only 64."), nBytes);
        nBytes = 64;
    }
    /*                     */
    vos_mem_set( pFrame, sizeof(pFrame), 0 );

    /*                                */

    header_offset = limPrepareTdlsFrameHeader(pMac, pFrame, 
        LINK_IDEN_ADDR_OFFSET(tdlsPtiTemplate), TDLS_LINK_AP, !setupPeer->tdls_bIsResponder, psessionEntry) ;

    status = dot11fPackTDLSPeerTrafficInd ( pMac, &tdlsPtiTemplate, pFrame 
        + header_offset, nPayload, &nPayload );

    if ( DOT11F_FAILED( status ) )
    {
        limLog( pMac, LOGE, FL("Failed to pack a PTI template \
                            (0x%08x)."), status );
        return eSIR_FAILURE;
    }
    else if ( DOT11F_WARNED( status ) )
    {
        limLog( pMac, LOGW, FL("There were warnings while packing TDLS"
            "Peer Traffic Indication (0x%08x).") );
    }

    LIM_LOG_TDLS(VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR, ("bIsResponder=%d, header_offset=%ld, linkIdenOffset=%d, ptiBufStatusOffset=%d "), \
        setupPeer->tdls_bIsResponder, header_offset, PTI_LINK_IDEN_OFFSET, PTI_BUF_STATUS_OFFSET));

    limSendTdlsLinkEstablish(pMac, setupPeer->tdls_bIsResponder, 
        header_offset+PTI_LINK_IDEN_OFFSET, header_offset+PTI_BUF_STATUS_OFFSET, 
      nBytes, pFrame, (tANI_U8 *)&setupPeer->tdlsPeerExtCaps);

    return eSIR_SUCCESS;
}

/* 
                                                     
 
*/   
static tSirRetStatus limTdlsLinkTeardown(tpAniSirGlobal pMac, tSirMacAddr peerMac)
{
    tDphHashNode        *pStaDs = NULL ;
    tANI_U16            aid = 0 ;
    tLimTdlsLinkSetupPeer *setupPeer = NULL ;
    tpPESession psessionEntry = NULL ;


    limTdlsFindLinkPeer(pMac, peerMac, &setupPeer) ;
    if(NULL == setupPeer) {
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR,
                  ("limTdlsLinkTeardown: cannot find peer mac "
                   "in tdls linksetup list: "
                   MAC_ADDRESS_STR), MAC_ADDR_ARRAY(peerMac));
        return eSIR_FAILURE;
    }

    psessionEntry = peFindSessionBySessionId(pMac, 
                         setupPeer->tdls_sessionId) ;

    if(NULL == psessionEntry) 
    {
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR, 
             ("limTdlsLinkTeardown: sessionID %d is not found"), setupPeer->tdls_sessionId);
        VOS_ASSERT(0) ;
        return eHAL_STATUS_FAILURE;
    }


    /* */
    pStaDs = dphLookupHashEntry(pMac, peerMac, &aid, &psessionEntry->dph.dphHashTable);

    if(pStaDs == NULL) {
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR,
                  ("limTdlsLinkTeardown: cannot find peer mac "
                   "in hash table: "
                   MAC_ADDRESS_STR), MAC_ADDR_ARRAY(peerMac));
        return eSIR_FAILURE;
    }

    limSendTdlsLinkTeardown(pMac, pStaDs->staIndex);

    return eSIR_SUCCESS;
}

/* 
                                                                       
                                                           
 */   
 
static tSirTdlsDisRsp *tdlsPrepareTdlsDisRsp(tpAniSirGlobal pMac, 
                                 tSirTdlsDisRsp *disRsp, tANI_U8 disStaCount) 
{
    tANI_U32 disMsgRspSize = sizeof(tSirTdlsDisRsp);
    tANI_U8 status = eHAL_STATUS_SUCCESS ;

    /*
                                                                           
                                                      
                                                             
     */
    disMsgRspSize += (disStaCount * sizeof(tSirTdlsPeerInfo));
        
    /*                     */

    disRsp = vos_mem_malloc(disMsgRspSize);
    if ( NULL == disRsp )
    {
        limLog(pMac, LOGP, FL("AllocateMemory failed for DIS RSP"));
        return NULL ;
    }
        
    if(disStaCount)
    { 
        tLimDisResultList *tdlsDisRspList = pMac->lim.gLimTdlsDisResultList ;
        tSirTdlsPeerInfo *peerInfo = &disRsp->tdlsDisPeerInfo[0] ;
            
        tLimDisResultList *currentNode = tdlsDisRspList ;
        while(tdlsDisRspList != NULL)
        {

            vos_mem_copy( (tANI_U8 *)peerInfo,
                          (tANI_U8 *) &tdlsDisRspList->tdlsDisPeerInfo, 
                                                 sizeof(tSirTdlsPeerInfo));
        
            VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_INFO, 
            ("Msg Sent to PE, peer MAC: "MAC_ADDRESS_STR),
                                  MAC_ADDR_ARRAY(peerInfo->peerMac));
            disStaCount-- ;
            peerInfo++ ;
            currentNode = tdlsDisRspList ;
            tdlsDisRspList = tdlsDisRspList->next ;
            vos_mem_free(currentNode) ;
            /*                                        */
            if(((!disStaCount) && (tdlsDisRspList)) 
                            || ((!tdlsDisRspList) && disStaCount))
            {
                limLog(pMac, LOG1, FL("mismatch in dis sta count and\
                                        and number of nodes in list")) ;
                VOS_ASSERT(0) ;
                return NULL ;
            } 
        } /*               */

        /*                             */
        pMac->lim.gLimTdlsDisResultList = NULL ;

    } /*                         */
    
    return (disRsp) ;
}

/*                                   */

void limSendSmeTdlsTeardownRsp(tpAniSirGlobal pMac, tSirResultCodes statusCode,
                                        tSirMacAddr peerMac, tANI_U16 msgType)
{
    tSirMsgQ  mmhMsg = {0} ;
    tSirTdlsTeardownRsp *teardownRspMsg = NULL ;
    tANI_U8 status = eHAL_STATUS_SUCCESS ;
    
    mmhMsg.type = msgType ;

    teardownRspMsg = vos_mem_malloc(sizeof(tSirTdlsTeardownRsp));
    if ( NULL == teardownRspMsg )
    {
        VOS_ASSERT(0) ;
    } 
    vos_mem_copy( teardownRspMsg->peerMac, (tANI_U8 *)peerMac,
                                                   sizeof(tSirMacAddr)) ;
    teardownRspMsg->statusCode =  statusCode ;
    mmhMsg.bodyptr = teardownRspMsg ;
    mmhMsg.bodyval = 0;
    limSysProcessMmhMsgApi(pMac, &mmhMsg, ePROT);

    return ;

}

/*
                                                                
 */
void limSendSmeTdlsLinkStartRsp(tpAniSirGlobal pMac,
                                         tSirResultCodes statusCode,
                                          tSirMacAddr peerMac, 
                                                 tANI_U16 msgType)
{
    tSirMsgQ  mmhMsg = {0} ;
    tSirTdlsLinksetupRsp *setupRspMsg = NULL ;
    tANI_U8 status = eHAL_STATUS_SUCCESS ;

    mmhMsg.type = msgType ;

    setupRspMsg = vos_mem_malloc(sizeof(tSirTdlsLinksetupRsp));
    if ( NULL == setupRspMsg )
    {
        VOS_ASSERT(0) ;
    } 

    vos_mem_copy( setupRspMsg->peerMac, (tANI_U8 *)peerMac,
                                                   sizeof(tSirMacAddr)) ;
    setupRspMsg->statusCode =  statusCode ;
    mmhMsg.bodyptr = setupRspMsg ;
    mmhMsg.bodyval = 0;
    limSysProcessMmhMsgApi(pMac, &mmhMsg, ePROT);

    return ;
}

/*
                                       
 */
void limSendSmeTdlsDisRsp(tpAniSirGlobal pMac, tSirResultCodes statusCode,
                                                          tANI_U16 msgType)
{
    tSirMsgQ  mmhMsg = {0} ;
    tSirTdlsDisRsp *tdlsDisRsp = NULL ;

    mmhMsg.type = msgType ;

    if(eSIR_SME_SUCCESS == statusCode)
    {
        tANI_U8 tdlsStaCount = pMac->lim.gLimTdlsDisStaCount ;

        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR, 
                    ("no of TDLS STA discovered: %d"), tdlsStaCount) ;
        tdlsDisRsp = tdlsPrepareTdlsDisRsp(pMac, tdlsDisRsp, tdlsStaCount) ;

        if(tdlsDisRsp)
        {
            tdlsDisRsp->numDisSta = tdlsStaCount ;
        }
        else
        {
            limLog(pMac, LOGP, FL("fatal failure for TDLS DIS RSP"));
            VOS_ASSERT(0) ; 
            return ;
        }
        /*                                */
        pMac->lim.gLimTdlsDisStaCount = 0 ;
    }
    else
    {
        tdlsDisRsp = tdlsPrepareTdlsDisRsp(pMac, tdlsDisRsp, 0) ;
    }

    tdlsDisRsp->statusCode =  statusCode ;
    mmhMsg.bodyptr = tdlsDisRsp ;
    mmhMsg.bodyval = 0;
    limSysProcessMmhMsgApi(pMac, &mmhMsg, ePROT);

     return ;
}

/* 
                                                        
 */
static eHalStatus limSendSmeTdlsAddPeerInd(tpAniSirGlobal pMac, 
                   tANI_U8 sessionId, tDphHashNode   *pStaDs, tANI_U8 status)
{
    tSirMsgQ  mmhMsg = {0} ;
    tSirTdlsPeerInd *peerInd = NULL ;
    mmhMsg.type = eWNI_SME_ADD_TDLS_PEER_IND ;

    peerInd = vos_mem_malloc(sizeof(tSirTdlsPeerInd));
    if ( NULL == peerInd )
    {
        PELOGE(limLog(pMac, LOGE, FL("Failed to allocate memory"));)
        return eSIR_FAILURE;
    }

    vos_mem_copy( peerInd->peerMac,
                           (tANI_U8 *) pStaDs->staAddr, sizeof(tSirMacAddr));
    peerInd->sessionId = sessionId;
    peerInd->staId = pStaDs->staIndex ;
    peerInd->ucastSig = pStaDs->ucUcastSig ;
    peerInd->bcastSig = pStaDs->ucBcastSig ;
    peerInd->length = sizeof(tSmeIbssPeerInd) ;

    mmhMsg.bodyptr = peerInd ;
    mmhMsg.bodyval = 0;
    limSysProcessMmhMsgApi(pMac, &mmhMsg, ePROT);

    return eSIR_SUCCESS ;

}

/*
                                                  
 */
static eHalStatus limSendSmeTdlsDelPeerInd(tpAniSirGlobal pMac, 
                    tANI_U8 sessionId, tDphHashNode   *pStaDs, tANI_U8 status)
{
    tSirMsgQ  mmhMsg = {0} ;
    tSirTdlsPeerInd *peerInd = NULL ;
    mmhMsg.type = eWNI_SME_DELETE_TDLS_PEER_IND ;

    peerInd = vos_mem_malloc(sizeof(tSirTdlsPeerInd));
    if ( NULL == peerInd )
    {
        PELOGE(limLog(pMac, LOGE, FL("Failed to allocate memory"));)
        return eSIR_FAILURE;
    }

    vos_mem_copy( peerInd->peerMac,
                           (tANI_U8 *) pStaDs->staAddr, sizeof(tSirMacAddr));
    peerInd->sessionId = sessionId;
    peerInd->staId = pStaDs->staIndex ;
    peerInd->ucastSig = pStaDs->ucUcastSig ;
    peerInd->bcastSig = pStaDs->ucBcastSig ;
    peerInd->length = sizeof(tSmeIbssPeerInd) ;

    mmhMsg.bodyptr = peerInd ;

    //                               
    mmhMsg.bodyval = 0;
    limSysProcessMmhMsgApi(pMac, &mmhMsg, ePROT);
    return eSIR_SUCCESS ;

}

/*
                                                                     
                        
 */
static eHalStatus limSendSmeTdlsLinkSetupInd(tpAniSirGlobal pMac, 
                                   tSirMacAddr peerMac, tANI_U8 status)
{
    tSirMsgQ  mmhMsg = {0} ;
    tSirTdlsLinkSetupInd *setupInd = NULL ;

    mmhMsg.type = eWNI_SME_TDLS_LINK_START_IND ;
    setupInd = vos_mem_malloc(sizeof(tSirTdlsLinkSetupInd));
    if ( NULL == setupInd )
    {
        PELOGE(limLog(pMac, LOGE, FL("Failed to allocate memory"));)
        return eSIR_FAILURE;
    }

    vos_mem_copy( setupInd->peerMac,
                           (tANI_U8 *) peerMac, sizeof(tSirMacAddr));
    setupInd->length = sizeof(tSirTdlsLinkSetupInd);
    setupInd->statusCode = status ;
    mmhMsg.bodyptr = setupInd ;
    mmhMsg.bodyval = 0;
    limSysProcessMmhMsgApi(pMac, &mmhMsg, ePROT);

    return eSIR_SUCCESS ;

}

/*
                           
 */
void limTdlsLinkSetupRspTimerHandler(void *pMacGlobal, tANI_U32 timerId)
{

    tANI_U32         statusCode;
    tSirMsgQ    msg;
    tpAniSirGlobal pMac = (tpAniSirGlobal)pMacGlobal;

    /*                                               */

    msg.type = SIR_LIM_TDLS_LINK_SETUP_RSP_TIMEOUT;
    msg.bodyptr = NULL ;
    msg.bodyval = timerId ;

    if ((statusCode = limPostMsgApi(pMac, &msg)) != eSIR_SUCCESS)
        limLog(pMac, LOGE,
               FL("posting message %X to LIM failed, reason=%d"),
               msg.type, statusCode);
    return ;
}

/*
                       
 */
void limTdlsLinkSetupCnfTimerHandler(void *pMacGlobal, tANI_U32 timerId)
{

    tANI_U32         statusCode;
    tSirMsgQ    msg;
    tpAniSirGlobal pMac = (tpAniSirGlobal)pMacGlobal;

    //                                              

    msg.type = SIR_LIM_TDLS_LINK_SETUP_CNF_TIMEOUT;
    msg.bodyptr = NULL ;
    msg.bodyval = timerId ;

    if ((statusCode = limPostMsgApi(pMac, &msg)) != eSIR_SUCCESS)
        limLog(pMac, LOGE,
               FL("posting message %X to LIM failed, reason=%d"),
               msg.type, statusCode);
    return ;
}

/*
                   
 */
void limStartTdlsTimer(tpAniSirGlobal pMac, tANI_U8 sessionId, TX_TIMER *timer,
                        tANI_U32 timerId, tANI_U16 timerType, tANI_U32 timerMsg)
{
    tANI_U32 cfgValue = (timerMsg == SIR_LIM_TDLS_LINK_SETUP_RSP_TIMEOUT)
                           ? WNI_CFG_TDLS_LINK_SETUP_RSP_TIMEOUT
                            : WNI_CFG_TDLS_LINK_SETUP_CNF_TIMEOUT ;

    void *timerFunc = (timerMsg == SIR_LIM_TDLS_LINK_SETUP_RSP_TIMEOUT)
                                ? (limTdlsLinkSetupRspTimerHandler) 
                                    : limTdlsLinkSetupCnfTimerHandler ;

    /*                                */

    cfgValue = SYS_MS_TO_TICKS(cfgValue);
    /*
                                                                
     */
    if (tx_timer_create(timer, "TDLS link setup timers", timerFunc,
                        timerId, cfgValue, 0, TX_NO_ACTIVATE) != TX_SUCCESS)
    {
        limLog(pMac, LOGP,
           FL("could not create TDLS discovery response wait timer"));
        return;
    }

    //                                                
    timer->sessionId = sessionId; 
    
     MTRACE(macTrace(pMac, TRACE_CODE_TIMER_ACTIVATE, 0,
                                             eLIM_TDLS_DISCOVERY_RSP_WAIT));
    if (tx_timer_activate(timer) != TX_SUCCESS)
    {
        limLog(pMac, LOGP, FL("TDLS link setup timer activation failed!"));
        return ;
    }

    return ;

}
#endif

/* 
                                                        
 */
static eHalStatus limSendSmeTdlsAddStaRsp(tpAniSirGlobal pMac, 
                   tANI_U8 sessionId, tSirMacAddr peerMac, tANI_U8 updateSta,
                   tDphHashNode  *pStaDs, tANI_U8 status)
{
    tSirMsgQ  mmhMsg = {0} ;
    tSirTdlsAddStaRsp *addStaRsp = NULL ;
    mmhMsg.type = eWNI_SME_TDLS_ADD_STA_RSP ;

    addStaRsp = vos_mem_malloc(sizeof(tSirTdlsAddStaRsp));
    if ( NULL == addStaRsp )
    {
        PELOGE(limLog(pMac, LOGE, FL("Failed to allocate memory"));)
        return eSIR_FAILURE;
    }

    addStaRsp->sessionId = sessionId;
    addStaRsp->statusCode = status;
    if( pStaDs )
    {
        addStaRsp->staId = pStaDs->staIndex ;
        addStaRsp->ucastSig = pStaDs->ucUcastSig ;
        addStaRsp->bcastSig = pStaDs->ucBcastSig ;
    }
    if( peerMac )
    {
        vos_mem_copy( addStaRsp->peerMac,
                (tANI_U8 *) peerMac, sizeof(tSirMacAddr));
    }
    if (updateSta)
        addStaRsp->tdlsAddOper = TDLS_OPER_UPDATE;
    else
        addStaRsp->tdlsAddOper = TDLS_OPER_ADD;

    addStaRsp->length = sizeof(tSirTdlsAddStaRsp) ;
    addStaRsp->messageType = eWNI_SME_TDLS_ADD_STA_RSP ;

    mmhMsg.bodyptr = addStaRsp;
    mmhMsg.bodyval = 0;
    limSysProcessMmhMsgApi(pMac, &mmhMsg, ePROT);

    return eSIR_SUCCESS ;

}
/* 
                            
 */
eHalStatus limProcessTdlsAddStaRsp(tpAniSirGlobal pMac, void *msg, 
                                                   tpPESession psessionEntry)
{
    tAddStaParams  *pAddStaParams = (tAddStaParams *) msg ;
    tANI_U8        status = eSIR_SUCCESS ;
    tDphHashNode   *pStaDs = NULL ;
    tANI_U16        aid = 0 ;

    SET_LIM_PROCESS_DEFD_MESGS(pMac, true);
    VOS_TRACE(VOS_MODULE_ID_PE, TDLS_DEBUG_LOG_LEVEL,
            ("limTdlsAddStaRsp: staIdx=%d, staMac="MAC_ADDRESS_STR), pAddStaParams->staIdx,
                            MAC_ADDR_ARRAY(pAddStaParams->staMac));

    if (pAddStaParams->status != eHAL_STATUS_SUCCESS)
    {
        VOS_ASSERT(0) ;
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR,
                                                   ("Add sta failed ")) ;
        status = eSIR_FAILURE;
        goto add_sta_error;
    }

    pStaDs = dphLookupHashEntry(pMac, pAddStaParams->staMac, &aid, 
                                         &psessionEntry->dph.dphHashTable);
    if(NULL == pStaDs)
    {
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR,
                                                   ("pStaDs is NULL ")) ;
        status = eSIR_FAILURE;
        goto add_sta_error;
    }

    pStaDs->bssId                  = pAddStaParams->bssIdx;
    pStaDs->staIndex               = pAddStaParams->staIdx;
    pStaDs->ucUcastSig             = pAddStaParams->ucUcastSig;
    pStaDs->ucBcastSig             = pAddStaParams->ucBcastSig;
    pStaDs->mlmStaContext.mlmState = eLIM_MLM_LINK_ESTABLISHED_STATE;
    pStaDs->valid                  = 1 ;
#ifdef FEATURE_WLAN_TDLS_INTERNAL    
    status = limSendSmeTdlsAddPeerInd(pMac, psessionEntry->smeSessionId, 
                                                    pStaDs, eSIR_SUCCESS ) ;
    if(eSIR_FAILURE == status)
    {
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR,
                                         ("Peer IND msg to SME failed")) ;
        vos_mem_free( pAddStaParams );
        return eSIR_FAILURE ;
    }

    /* 
                                                                       
                                                   
                                                                      
                         
     */
    do
    {
        tSirTdlsPeerInfo *peerInfo = limTdlsFindDisPeer(pMac,
                                            pAddStaParams->staMac) ;

    
        if(peerInfo)
        {
            /* 
                                                                               
                                                                           
                                                                        
             */ 
            limSendTdlsDisRspFrame(pMac, peerInfo->peerMac, peerInfo->dialog, psessionEntry) ;
            peerInfo->tdlsPeerState = TDLS_DIS_RSP_SENT_WAIT_STATE ;
        }
    } while(0) ;
#endif
add_sta_error:
    status = limSendSmeTdlsAddStaRsp(pMac, psessionEntry->smeSessionId, 
                                        pAddStaParams->staMac, pAddStaParams->updateSta, pStaDs, status) ;
    vos_mem_free( pAddStaParams );
    return status ;
}

/*
                                                
  
 */

 
void PopulateDot11fLinkIden(tpAniSirGlobal pMac, tpPESession psessionEntry, 
                                 tDot11fIELinkIdentifier *linkIden,
                                       tSirMacAddr peerMac, tANI_U8 reqType)
{
    //                                     
    tANI_U8 *initStaAddr = NULL ;
    tANI_U8 *respStaAddr = NULL ;

    (reqType == TDLS_INITIATOR) ? ((initStaAddr = linkIden->InitStaAddr),
                                   (respStaAddr = linkIden->RespStaAddr))
                                : ((respStaAddr = linkIden->InitStaAddr ),
                                   (initStaAddr = linkIden->RespStaAddr)) ;
    vos_mem_copy( (tANI_U8 *)linkIden->bssid,
                     (tANI_U8 *) psessionEntry->bssId, sizeof(tSirMacAddr)) ; 

    vos_mem_copy( (tANI_U8 *)initStaAddr,
                          psessionEntry->selfMacAddr, sizeof(tSirMacAddr)) ;

    vos_mem_copy( (tANI_U8 *)respStaAddr, (tANI_U8 *) peerMac,
                                                       sizeof( tSirMacAddr ));

    linkIden->present = 1 ;
    return ;

}

void PopulateDot11fTdlsExtCapability(tpAniSirGlobal pMac, 
                                        tDot11fIEExtCap *extCapability)
{
    extCapability->TDLSPeerPSMSupp = PEER_PSM_SUPPORT ;
    extCapability->TDLSPeerUAPSDBufferSTA = pMac->lim.gLimTDLSBufStaEnabled;
    extCapability->TDLSChannelSwitching = CH_SWITCH_SUPPORT ;
    extCapability->TDLSSupport = TDLS_SUPPORT ;
    extCapability->TDLSProhibited = TDLS_PROHIBITED ;
    extCapability->TDLSChanSwitProhibited = TDLS_CH_SWITCH_PROHIBITED ;
    extCapability->present = 1 ;
    return ;
}
                                     
#ifdef FEATURE_WLAN_TDLS_INTERNAL
/*
                                        
                                                          
                                                         
              
 */
void limProcessTdlsPublicActionFrame(tpAniSirGlobal pMac, tANI_U32 *pBd, 
                                                  tpPESession psessionEntry)
{
    tANI_U32 frameLen = WDA_GET_RX_PAYLOAD_LEN(pBd) ;
    tANI_U8 *pBody = WDA_GET_RX_MPDU_DATA(pBd) ;
    tANI_S8 rssi = (tANI_S8)WDA_GET_RX_RSSI_DB(pBd) ;

    limProcessTdlsDisRspFrame(pMac, pBody, frameLen, rssi, psessionEntry) ;
    return ; 
}

eHalStatus limTdlsPrepareSetupReqFrame(tpAniSirGlobal pMac, 
                              tLimTdlsLinkSetupInfo *linkSetupInfo,
                                 tANI_U8 dialog, tSirMacAddr peerMac,
                                                 tpPESession psessionEntry)
{
    tLimTdlsLinkSetupPeer *setupPeer = NULL ;

    /*
                                                                      
                                                                 
                                                                       
    */

    setupPeer = vos_mem_malloc(sizeof( tLimTdlsLinkSetupPeer ));
    if ( NULL == setupPeer )
    {
        limLog( pMac, LOGP, 
                  FL( "Unable to allocate memory during ADD_STA" ));
         VOS_ASSERT(0) ;
         return eSIR_MEM_ALLOC_FAILED;
    }
    setupPeer->dialog = dialog ;
    setupPeer->tdls_prev_link_state =  setupPeer->tdls_link_state ;
    setupPeer->tdls_link_state = TDLS_LINK_SETUP_START_STATE ;

    /*                                                */
    setupPeer->tdls_sessionId = psessionEntry->peSessionId;
    setupPeer->tdls_bIsResponder = 1;

    /* 
                                                               
                                                        
    */
    vos_mem_copy(setupPeer->peerMac, peerMac,
                                              sizeof(tSirMacAddr)) ;
    /*                                                     */
    limSendTdlsLinkSetupReqFrame(pMac, peerMac, dialog, psessionEntry, NULL, 0) ;

    limStartTdlsTimer(pMac, psessionEntry->peSessionId, 
                        &setupPeer->gLimTdlsLinkSetupRspTimeoutTimer,
                            (tANI_U32)setupPeer->peerMac, 
                               WNI_CFG_TDLS_LINK_SETUP_RSP_TIMEOUT,
                                 SIR_LIM_TDLS_LINK_SETUP_RSP_TIMEOUT) ;
    /*                        */
    setupPeer->next = linkSetupInfo->tdlsLinkSetupList ;
    linkSetupInfo->tdlsLinkSetupList = setupPeer ;

    /*                                                                  
                                                                        
                                   
     */
    return eSIR_SUCCESS ; 
}
#endif

/*
                                                         
 */
tSirRetStatus limProcessSmeTdlsMgmtSendReq(tpAniSirGlobal pMac, 
                                                           tANI_U32 *pMsgBuf)
{
    /*                                      */
    tSirTdlsSendMgmtReq *pSendMgmtReq = (tSirTdlsSendMgmtReq*) pMsgBuf ;
    tpPESession psessionEntry;
    tANI_U8      sessionId;
    tSirResultCodes resultCode = eSIR_SME_INVALID_PARAMETERS;

    VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_INFO,
            ("Send Mgmt Recieved")) ;

    if((psessionEntry = peFindSessionByBssid(pMac, pSendMgmtReq->bssid, &sessionId)) 
            == NULL)
    {
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR, 
                "PE Session does not exist for given sme sessionId %d",
                pSendMgmtReq->sessionId);
        goto lim_tdls_send_mgmt_error;
    }

    /*                                                        */ 
    if (psessionEntry->limSystemRole != eLIM_STA_ROLE)
    {
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR, 
                "send mgmt received in wrong system Role %d",
                psessionEntry->limSystemRole);
        goto lim_tdls_send_mgmt_error;
    }

    /*
                                                                            
                                            
     */
    if ((psessionEntry->limSmeState != eLIM_SME_ASSOCIATED_STATE) &&
            (psessionEntry->limSmeState != eLIM_SME_LINK_EST_STATE))
    {

        limLog(pMac, LOGE, "send mgmt received in invalid LIMsme \
                state (%d)", psessionEntry->limSmeState);
        goto lim_tdls_send_mgmt_error;
    }

    switch( pSendMgmtReq->reqType )
    {
        case SIR_MAC_TDLS_DIS_REQ:
            VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_INFO,
                    "Transmit Discovery Request Frame") ;
            /*                                                     */
            limSendTdlsDisReqFrame(pMac, pSendMgmtReq->peerMac, pSendMgmtReq->dialog, 
                    psessionEntry) ;
            resultCode = eSIR_SME_SUCCESS;
            break;
        case SIR_MAC_TDLS_DIS_RSP:
            {
                //                                 
                limSendTdlsDisRspFrame(pMac, pSendMgmtReq->peerMac,
                        pSendMgmtReq->dialog, psessionEntry) ;
                resultCode = eSIR_SME_SUCCESS;
            }
            break;
        case SIR_MAC_TDLS_SETUP_REQ:
            {
                limSendTdlsLinkSetupReqFrame(pMac,
                        pSendMgmtReq->peerMac, pSendMgmtReq->dialog, psessionEntry,
                        &pSendMgmtReq->addIe[0], (pSendMgmtReq->length - sizeof(tSirTdlsSendMgmtReq))); 
                resultCode = eSIR_SME_SUCCESS;
            }
            break;
        case SIR_MAC_TDLS_SETUP_RSP:
            {
                limSendTdlsSetupRspFrame(pMac, 
                        pSendMgmtReq->peerMac, pSendMgmtReq->dialog, psessionEntry, pSendMgmtReq->statusCode,
                        &pSendMgmtReq->addIe[0], (pSendMgmtReq->length - sizeof(tSirTdlsSendMgmtReq)));
                resultCode = eSIR_SME_SUCCESS;
            }
            break;
        case SIR_MAC_TDLS_SETUP_CNF:
            {
                limSendTdlsLinkSetupCnfFrame(pMac, pSendMgmtReq->peerMac, pSendMgmtReq->dialog, 
                        psessionEntry, &pSendMgmtReq->addIe[0], (pSendMgmtReq->length - sizeof(tSirTdlsSendMgmtReq)));  
                resultCode = eSIR_SME_SUCCESS;
            }
            break;
        case SIR_MAC_TDLS_TEARDOWN:
            {
                limSendTdlsTeardownFrame(pMac,
                        pSendMgmtReq->peerMac, pSendMgmtReq->statusCode, pSendMgmtReq->responder, psessionEntry,
                        &pSendMgmtReq->addIe[0], (pSendMgmtReq->length - sizeof(tSirTdlsSendMgmtReq))); 
                resultCode = eSIR_SME_SUCCESS;
            }
            break;
        case SIR_MAC_TDLS_PEER_TRAFFIC_IND:
            {
            }
            break;
        case SIR_MAC_TDLS_CH_SWITCH_REQ:
            {
            }
            break;
        case SIR_MAC_TDLS_CH_SWITCH_RSP:
            {
            }
            break;
        case SIR_MAC_TDLS_PEER_TRAFFIC_RSP:
            {
            }
            break;
        default:
            break;
    }

lim_tdls_send_mgmt_error:

    limSendSmeRsp( pMac, eWNI_SME_TDLS_SEND_MGMT_RSP,
            resultCode, pSendMgmtReq->sessionId, pSendMgmtReq->transactionId);

    return eSIR_SUCCESS;
}

/*
                                                 
 */
void limSendSmeTdlsLinkEstablishReqRsp(tpAniSirGlobal pMac,
                    tANI_U8 sessionId, tSirMacAddr peerMac, tDphHashNode   *pStaDs,
                    tANI_U8 status)
{
    tSirMsgQ  mmhMsg = {0} ;

    tSirTdlsLinkEstablishReqRsp *pTdlsLinkEstablishReqRsp = NULL ;

    pTdlsLinkEstablishReqRsp = vos_mem_malloc(sizeof(tSirTdlsLinkEstablishReqRsp));
    if ( NULL == pTdlsLinkEstablishReqRsp )
    {
        PELOGE(limLog(pMac, LOGE, FL("Failed to allocate memory"));)
        return ;
    }
    pTdlsLinkEstablishReqRsp->statusCode = status ;
    if ( peerMac )
    {
        vos_mem_copy(pTdlsLinkEstablishReqRsp->peerMac, peerMac, sizeof(tSirMacAddr));
    }
    pTdlsLinkEstablishReqRsp->sessionId = sessionId;
    mmhMsg.type = eWNI_SME_TDLS_LINK_ESTABLISH_RSP ;
    mmhMsg.bodyptr = pTdlsLinkEstablishReqRsp;
    mmhMsg.bodyval = 0;
    limSysProcessMmhMsgApi(pMac, &mmhMsg, ePROT);
    return ;


}

/*
                                                  
 */
static eHalStatus limSendSmeTdlsDelStaRsp(tpAniSirGlobal pMac, 
                    tANI_U8 sessionId, tSirMacAddr peerMac, tDphHashNode   *pStaDs,
                    tANI_U8 status)
{
    tSirMsgQ  mmhMsg = {0} ;
    tSirTdlsDelStaRsp *pDelSta = NULL ;
    mmhMsg.type = eWNI_SME_TDLS_DEL_STA_RSP ;

    pDelSta = vos_mem_malloc(sizeof(tSirTdlsDelStaRsp));
    if ( NULL == pDelSta )
    {
        PELOGE(limLog(pMac, LOGE, FL("Failed to allocate memory"));)
            return eSIR_FAILURE;
    }

    pDelSta->sessionId = sessionId;
    pDelSta->statusCode =  status ;
    if( pStaDs )
    {
        pDelSta->staId = pStaDs->staIndex ;
    }
    else
        pDelSta->staId = HAL_STA_INVALID_IDX;

    if( peerMac )
    {
        vos_mem_copy(pDelSta->peerMac, peerMac, sizeof(tSirMacAddr));
    }

    pDelSta->length = sizeof(tSirTdlsDelStaRsp) ;
    pDelSta->messageType = eWNI_SME_TDLS_DEL_STA_RSP ;

    mmhMsg.bodyptr = pDelSta;

    mmhMsg.bodyval = 0;
    limSysProcessMmhMsgApi(pMac, &mmhMsg, ePROT);
    return eSIR_SUCCESS ;

}

/*
                                                         
 */
tSirRetStatus limProcessSmeTdlsAddStaReq(tpAniSirGlobal pMac, 
                                                           tANI_U32 *pMsgBuf)
{
    /*                                      */
    tSirTdlsAddStaReq *pAddStaReq = (tSirTdlsAddStaReq*) pMsgBuf ;
    tpPESession psessionEntry;
    tANI_U8      sessionId;

    VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_INFO,
                                  ("Send Mgmt Recieved")) ;

    if((psessionEntry = peFindSessionByBssid(pMac, pAddStaReq->bssid, &sessionId)) 
                                                                        == NULL)
    {
         VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR, 
                    "PE Session does not exist for given sme sessionId %d",
                                                            pAddStaReq->sessionId);
         goto lim_tdls_add_sta_error;
    }
    
    /*                                                        */ 
    if (psessionEntry->limSystemRole != eLIM_STA_ROLE)
    {
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR, 
                         "send mgmt received in wrong system Role %d",
                                             psessionEntry->limSystemRole);
        goto lim_tdls_add_sta_error;
    }

    /*
                                                                            
                                            
     */
     if ((psessionEntry->limSmeState != eLIM_SME_ASSOCIATED_STATE) &&
                (psessionEntry->limSmeState != eLIM_SME_LINK_EST_STATE))
     {
     
         limLog(pMac, LOGE, "send mgmt received in invalid LIMsme \
                               state (%d)", psessionEntry->limSmeState);
         goto lim_tdls_add_sta_error;
     }

     pMac->lim.gLimAddStaTdls = true ;

     /*                                            */
     if (eSIR_FAILURE == limTdlsSetupAddSta(pMac, pAddStaReq, psessionEntry))
     {
         limLog(pMac, LOGE, "%s: Add TDLS Station request failed ", __func__);
         goto lim_tdls_add_sta_error;
     }
     return eSIR_SUCCESS;
lim_tdls_add_sta_error:
     limSendSmeTdlsAddStaRsp(pMac, 
                   pAddStaReq->sessionId, pAddStaReq->peerMac,
                   (pAddStaReq->tdlsAddOper == TDLS_OPER_UPDATE), NULL, eSIR_FAILURE );

   return eSIR_SUCCESS;
}
/*
                                     
 */
tSirRetStatus limProcessSmeTdlsDelStaReq(tpAniSirGlobal pMac, 
                                                           tANI_U32 *pMsgBuf)
{
    /*                                      */
    tSirTdlsDelStaReq *pDelStaReq = (tSirTdlsDelStaReq*) pMsgBuf ;
    tpPESession psessionEntry;
    tANI_U8      sessionId;
    tpDphHashNode pStaDs = NULL ;

    VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_INFO,
            ("Send Mgmt Recieved")) ;

    if((psessionEntry = peFindSessionByBssid(pMac, pDelStaReq->bssid, &sessionId)) 
            == NULL)
    {
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR, 
                "PE Session does not exist for given sme sessionId %d",
                pDelStaReq->sessionId);
        limSendSmeTdlsDelStaRsp(pMac, pDelStaReq->sessionId, pDelStaReq->peerMac,
             NULL, eSIR_FAILURE) ;
        return eSIR_FAILURE;
    }

    /*                                                        */ 
    if (psessionEntry->limSystemRole != eLIM_STA_ROLE)
    {
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR, 
                "Del sta received in wrong system Role %d",
                psessionEntry->limSystemRole);
        goto lim_tdls_del_sta_error;
    }

    /*
                                                                            
                                            
     */
    if ((psessionEntry->limSmeState != eLIM_SME_ASSOCIATED_STATE) &&
            (psessionEntry->limSmeState != eLIM_SME_LINK_EST_STATE))
    {

        limLog(pMac, LOGE, "Del Sta received in invalid LIMsme \
                state (%d)", psessionEntry->limSmeState);
        goto lim_tdls_del_sta_error;
    }

    pStaDs = limTdlsDelSta(pMac, pDelStaReq->peerMac, psessionEntry) ;

    /*                                                            */

    if(pStaDs)
    {
        limSendSmeTdlsDelStaRsp(pMac, psessionEntry->smeSessionId, pDelStaReq->peerMac,
                pStaDs, eSIR_SUCCESS) ;
        limReleasePeerIdx(pMac, pStaDs->assocId, psessionEntry) ;

        /*                                                               */
        CLEAR_PEER_AID_BITMAP(psessionEntry->peerAIDBitmap, pStaDs->assocId);
        limDeleteDphHashEntry(pMac, pStaDs->staAddr, pStaDs->assocId, psessionEntry) ;

        return eSIR_SUCCESS;

    }

lim_tdls_del_sta_error:
     limSendSmeTdlsDelStaRsp(pMac, psessionEntry->smeSessionId, pDelStaReq->peerMac,
             NULL, eSIR_FAILURE) ;

    return eSIR_SUCCESS;
}


/*
                                                
 */
tSirRetStatus limProcesSmeTdlsLinkEstablishReq(tpAniSirGlobal pMac,
                                                           tANI_U32 *pMsgBuf)
{
    /*                                      */
    tSirTdlsLinkEstablishReq *pTdlsLinkEstablishReq = (tSirTdlsLinkEstablishReq*) pMsgBuf ;
    tpPESession psessionEntry;
    tANI_U8      sessionId;
    tpTdlsLinkEstablishParams pMsgTdlsLinkEstablishReq;
    tSirMsgQ msg;
    tANI_U16 peerIdx = 0 ;
    tpDphHashNode pStaDs = NULL ;

    VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_INFO,
            ("Send Mgmt Recieved\n")) ;

    if((psessionEntry = peFindSessionByBssid(pMac, pTdlsLinkEstablishReq->bssid, &sessionId))
            == NULL)
    {
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR,
                "PE Session does not exist for given sme sessionId %d\n",
                pTdlsLinkEstablishReq->sessionId);
        limSendSmeTdlsLinkEstablishReqRsp(pMac, pTdlsLinkEstablishReq->sessionId, pTdlsLinkEstablishReq->peerMac,
             NULL, eSIR_FAILURE) ;
        return eSIR_FAILURE;
    }

    /*                                                        */
    if (psessionEntry->limSystemRole != eLIM_STA_ROLE)
    {
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR,
                "TDLS Link Establish Request received in wrong system Role %d\n",
                psessionEntry->limSystemRole);
        goto lim_tdls_link_establish_error;
    }

    /*
                                                                            
                                            
     */
    if ((psessionEntry->limSmeState != eLIM_SME_ASSOCIATED_STATE) &&
            (psessionEntry->limSmeState != eLIM_SME_LINK_EST_STATE))
    {

        limLog(pMac, LOGE, "TDLS Link Establish Request received in invalid LIMsme \
                state (%d)\n", psessionEntry->limSmeState);
        goto lim_tdls_link_establish_error;
    }
    /*                                                    */
    pStaDs = dphLookupHashEntry(pMac, pTdlsLinkEstablishReq->peerMac, &peerIdx,
                                &psessionEntry->dph.dphHashTable) ;
    if ( NULL == pStaDs )
    {
        limLog( pMac, LOGE, FL( "pStaDs is NULL \n" ));
        goto lim_tdls_link_establish_error;

    }
    pMsgTdlsLinkEstablishReq = vos_mem_malloc(sizeof( tTdlsLinkEstablishParams ));
    if ( NULL == pMsgTdlsLinkEstablishReq )
    {
        limLog( pMac, LOGE,
                     FL( "Unable to allocate memory TDLS Link Establish Request \n" ));
        return eSIR_MEM_ALLOC_FAILED;
    }

    vos_mem_set( (tANI_U8 *)pMsgTdlsLinkEstablishReq, sizeof(tpTdlsLinkEstablishParams), 0);

    pMsgTdlsLinkEstablishReq->staIdx = pStaDs->staIndex;
    pMsgTdlsLinkEstablishReq->isResponder = pTdlsLinkEstablishReq->isResponder;
    pMsgTdlsLinkEstablishReq->uapsdQueues = pTdlsLinkEstablishReq->uapsdQueues;
    pMsgTdlsLinkEstablishReq->maxSp = pTdlsLinkEstablishReq->maxSp;
    pMsgTdlsLinkEstablishReq->isBufsta = pTdlsLinkEstablishReq->isBufSta;
    msg.type = WDA_SET_TDLS_LINK_ESTABLISH_REQ;
    msg.reserved = 0;
    msg.bodyptr = pMsgTdlsLinkEstablishReq;
    msg.bodyval = 0;
    if(eSIR_SUCCESS != wdaPostCtrlMsg(pMac, &msg))
    {
        limLog(pMac, LOGE, FL("halPostMsgApi failed\n"));
        goto lim_tdls_link_establish_error;
    }
    return eSIR_SUCCESS;
lim_tdls_link_establish_error:
     limSendSmeTdlsLinkEstablishReqRsp(pMac, psessionEntry->smeSessionId, pTdlsLinkEstablishReq->peerMac,
                                       NULL, eSIR_FAILURE) ;

    return eSIR_SUCCESS;
}


/*                                                           */
tSirRetStatus limDeleteTDLSPeers(tpAniSirGlobal pMac, tpPESession psessionEntry)
{
    tpDphHashNode pStaDs = NULL ;
    int i, aid;

    if (NULL == psessionEntry)
    {
        PELOGE(limLog(pMac, LOGE, FL("NULL psessionEntry"));)
        return eSIR_FAILURE;
    }

    /*                                                                                 
                                                        */
    for (i = 0; i < sizeof(psessionEntry->peerAIDBitmap)/sizeof(tANI_U32); i++)
    {
        for (aid = 0; aid < (sizeof(tANI_U32) << 3); aid++)
        {
            if (CHECK_BIT(psessionEntry->peerAIDBitmap[i], aid))
            {
                pStaDs = dphGetHashEntry(pMac, (aid + i*(sizeof(tANI_U32) << 3)), &psessionEntry->dph.dphHashTable);

                if (NULL != pStaDs)
                {
                    PELOGE(limLog(pMac, LOGE, FL("Deleting "MAC_ADDRESS_STR),
                           MAC_ADDR_ARRAY(pStaDs->staAddr)););

                    limSendDeauthMgmtFrame(pMac, eSIR_MAC_DEAUTH_LEAVING_BSS_REASON,
                                           pStaDs->staAddr, psessionEntry, FALSE);
                    dphDeleteHashEntry(pMac, pStaDs->staAddr, pStaDs->assocId, &psessionEntry->dph.dphHashTable);
                }
                limReleasePeerIdx(pMac, (aid + i*(sizeof(tANI_U32) << 3)), psessionEntry) ;
                CLEAR_BIT(psessionEntry->peerAIDBitmap[i], aid);
            }
        }
    }
    limSendSmeTDLSDeleteAllPeerInd(pMac, psessionEntry);

    return eSIR_SUCCESS;
}
#ifdef FEATURE_WLAN_TDLS_OXYGEN_DISAPPEAR_AP
/*                                                  */
int limGetTDLSPeerCount(tpAniSirGlobal pMac, tpPESession psessionEntry)
{
    int i,tdlsPeerCount = 0;
    /*                                                                                  */
    for (i = 0; i < sizeof(psessionEntry->peerAIDBitmap)/sizeof(tANI_U32); i++)
    {
        tANI_U32 bitmap;
        bitmap = psessionEntry->peerAIDBitmap[i];
        while (bitmap)
        {
            tdlsPeerCount++;
            bitmap >>= 1;
        }
    }
    return tdlsPeerCount;
}

void limTDLSDisappearAPTrickInd(tpAniSirGlobal pMac, tpDphHashNode pStaDs, tpPESession psessionEntry)
{
    tSirMsgQ  mmhMsg;
    tSirTdlsDisappearAPInd  *pSirTdlsDisappearAPInd;

    pSirTdlsDisappearAPInd = vos_mem_malloc(sizeof(tSirTdlsDisappearAPInd));
    if ( NULL == pSirTdlsDisappearAPInd )
    {
        limLog(pMac, LOGP, FL("AllocateMemory failed for eWNI_SME_TDLS_DEL_ALL_PEER_IND"));
        return;
    }

    //           
    pSirTdlsDisappearAPInd->messageType = eWNI_SME_TDLS_AP_DISAPPEAR_IND;
    pSirTdlsDisappearAPInd->length = sizeof(tSirTdlsDisappearAPInd);

    //         
    pSirTdlsDisappearAPInd->sessionId = psessionEntry->smeSessionId;
    pSirTdlsDisappearAPInd->staId = pStaDs->staIndex ;
    vos_mem_copy( pSirTdlsDisappearAPInd->staAddr,
                           (tANI_U8 *) pStaDs->staAddr, sizeof(tSirMacAddr));

    mmhMsg.type = eWNI_SME_TDLS_AP_DISAPPEAR_IND;
    mmhMsg.bodyptr = pSirTdlsDisappearAPInd;
    mmhMsg.bodyval = 0;


    limSysProcessMmhMsgApi(pMac, &mmhMsg, ePROT);
}
#endif
#endif

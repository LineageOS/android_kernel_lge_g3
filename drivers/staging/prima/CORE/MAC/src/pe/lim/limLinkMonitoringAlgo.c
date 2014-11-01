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

#include "aniGlobal.h"
#include "wniCfgSta.h"
#include "cfgApi.h"


#include "schApi.h"
#include "pmmApi.h"
#include "utilsApi.h"
#include "limAssocUtils.h"
#include "limTypes.h"
#include "limUtils.h"
#include "limPropExtsUtils.h"

#ifdef FEATURE_WLAN_DIAG_SUPPORT_LIM //                         
#include "vos_diag_core_log.h"
#endif //                         
#include "limSession.h"
#include "limSerDesUtils.h"


/* 
                           
  
           
                                                             
  
        
  
              
  
       
     
  
                                                        
               
 */

void
limSendKeepAliveToPeer(tpAniSirGlobal pMac)
{

} /*                              */


/*                                                           
                            
                                                    
                                                  
                                                              
                                                              
                            
                              
             
                                                             */
void
limDeleteStaContext(tpAniSirGlobal pMac, tpSirMsgQ limMsg)
{
    tpDeleteStaContext  pMsg = (tpDeleteStaContext)limMsg->bodyptr;
    tpDphHashNode       pStaDs;
    tpPESession psessionEntry ;
    tANI_U8     sessionId;

    if(NULL == pMsg)
    {
        PELOGE(limLog(pMac, LOGE,FL("Invalid body pointer in message"));)
        return;
    }
    if((psessionEntry = peFindSessionByBssid(pMac,pMsg->bssId,&sessionId))== NULL)
    {
        PELOGE(limLog(pMac, LOGE,FL("session does not exist for given BSSId"));)
        vos_mem_free(pMsg);
        return;
    }

    switch(pMsg->reasonCode)
    {
        case HAL_DEL_STA_REASON_CODE_KEEP_ALIVE:
        case HAL_DEL_STA_REASON_CODE_TIM_BASED:
             PELOGE(limLog(pMac, LOGE, FL(" Deleting station: staId = %d, reasonCode = %d"), pMsg->staId, pMsg->reasonCode);)
             if (eLIM_STA_IN_IBSS_ROLE == psessionEntry->limSystemRole)
                 return;

             pStaDs = dphLookupAssocId(pMac, pMsg->staId, &pMsg->assocId, &psessionEntry->dph.dphHashTable);

             if (!pStaDs)
             {
                 PELOGE(limLog(pMac, LOGE, FL("Skip STA deletion (invalid STA) limSystemRole=%d"),psessionEntry->limSystemRole);)
                 vos_mem_free(pMsg);
                 return;
             }

             /*                                                           
                                                                   
              */
             if (pStaDs->staIndex != pMsg->staId)
             {
                 PELOGE(limLog(pMac, LOGE, FL("staid mismatch: %d vs %d "), pStaDs->staIndex, pMsg->staId);)
                 vos_mem_free(pMsg);
                 return;
             }

             if((eLIM_BT_AMP_AP_ROLE == psessionEntry->limSystemRole) ||
                     (eLIM_AP_ROLE == psessionEntry->limSystemRole))
             {
                 PELOG1(limLog(pMac, LOG1, FL("SAP:lim Delete Station Context (staId: %d, assocId: %d) "),
                             pMsg->staId, pMsg->assocId);)
                 limTriggerSTAdeletion(pMac, pStaDs, psessionEntry);
             }
             else
             {
#ifdef FEATURE_WLAN_TDLS
                if(eLIM_STA_ROLE == psessionEntry->limSystemRole &&
                    STA_ENTRY_TDLS_PEER == pStaDs->staType)
                {
                    //                      
                    //                                                    
                    //                                       
                    limSendSmeTDLSDelStaInd(pMac, pStaDs, psessionEntry,
                    /*                */ eSIR_MAC_TDLS_TEARDOWN_PEER_UNREACHABLE);
                }
                else
                {
#endif
                    //                    
                    tLimMlmDeauthInd  mlmDeauthInd;
                    PELOGW(limLog(pMac, LOGW, FL("lim Delete Station Context (staId: %d, assocId: %d) "),
                                pMsg->staId, pMsg->assocId);)

                    pStaDs->mlmStaContext.disassocReason = eSIR_MAC_UNSPEC_FAILURE_REASON;
                    pStaDs->mlmStaContext.cleanupTrigger = eLIM_LINK_MONITORING_DEAUTH;

                    //                                
                    vos_mem_copy((tANI_U8 *) &mlmDeauthInd.peerMacAddr,
                                  pStaDs->staAddr, sizeof(tSirMacAddr));
                    mlmDeauthInd.reasonCode    = (tANI_U8) pStaDs->mlmStaContext.disassocReason;
                    mlmDeauthInd.deauthTrigger =  pStaDs->mlmStaContext.cleanupTrigger;

#ifdef FEATURE_WLAN_TDLS
#ifdef FEATURE_WLAN_TDLS_OXYGEN_DISAPPEAR_AP
                    if ((TRUE == pMac->lim.gLimTDLSOxygenSupport) &&
                        (limGetTDLSPeerCount(pMac, psessionEntry) != 0)) {
                            limTDLSDisappearAPTrickInd(pMac, pStaDs, psessionEntry);
                            vos_mem_free(pMsg);
                            return ;
                    }
#endif
                    /*                                                   */
                    limDeleteTDLSPeers(pMac, psessionEntry);
#endif
                    limPostSmeMessage(pMac, LIM_MLM_DEAUTH_IND, (tANI_U32 *) &mlmDeauthInd);

                    limSendSmeDeauthInd(pMac, pStaDs, psessionEntry);
#ifdef FEATURE_WLAN_TDLS
                 }
#endif
             }
             break;        

        case HAL_DEL_STA_REASON_CODE_UNKNOWN_A2:
             PELOGE(limLog(pMac, LOGE, FL(" Deleting Unknown station "));)
             limPrintMacAddr(pMac, pMsg->addr2, LOGE);
             limSendDeauthMgmtFrame( pMac, eSIR_MAC_CLASS3_FRAME_FROM_NON_ASSOC_STA_REASON, pMsg->addr2, psessionEntry, FALSE);
             break;

        default:
             PELOGE(limLog(pMac, LOGE, FL(" Unknown reason code "));)
             break;

    }
    vos_mem_free(pMsg);
    return;
}


/* 
                          
  
           
                                                          
  
        
  
              
  
       
     
  
                                                   
                                                         
               
 */
void
limTriggerSTAdeletion(tpAniSirGlobal pMac, tpDphHashNode pStaDs, tpPESession psessionEntry)
{
    tSirSmeDeauthReq    *pSmeDeauthReq;
    tANI_U8             *pBuf;
    tANI_U8             *pLen;
    tANI_U16            msgLength = 0;

    if (! pStaDs)
    {
        PELOGW(limLog(pMac, LOGW, FL("Skip STA deletion (invalid STA)"));)
        return;
    }
    /* 
                                                 
                                                   
                                           
     */

    pSmeDeauthReq = vos_mem_malloc(sizeof(tSirSmeDeauthReq));
    if (NULL == pSmeDeauthReq)
    {
        limLog(pMac, LOGP, FL("AllocateMemory failed for eWNI_SME_DEAUTH_REQ "));
        return;
    }

    pBuf = (tANI_U8 *) &pSmeDeauthReq->messageType;

    //           
    limCopyU16((tANI_U8*)pBuf, eWNI_SME_DISASSOC_REQ);
    pBuf += sizeof(tANI_U16);
    msgLength += sizeof(tANI_U16);

    //      
    pLen = pBuf;
    pBuf += sizeof(tANI_U16);
    msgLength += sizeof(tANI_U16);

    //         
    *pBuf = psessionEntry->smeSessionId;
    pBuf++;
    msgLength++;

    //             
    limCopyU16((tANI_U8*)pBuf, psessionEntry->transactionId);
    pBuf += sizeof(tANI_U16);
    msgLength += sizeof(tANI_U16);

    //     
    vos_mem_copy(pBuf, psessionEntry->bssId, sizeof(tSirMacAddr));
    pBuf += sizeof(tSirMacAddr);
    msgLength += sizeof(tSirMacAddr);

    //           
    vos_mem_copy(pBuf, pStaDs->staAddr, sizeof(tSirMacAddr));
    pBuf += sizeof(tSirMacAddr);
    msgLength += sizeof(tSirMacAddr);

    //           
    limCopyU16((tANI_U8*)pBuf, (tANI_U16)eLIM_LINK_MONITORING_DISASSOC);
    pBuf += sizeof(tANI_U16);
    msgLength += sizeof(tANI_U16);

    //                        
    //                                                             
    //                                                      
    pBuf[0]= 0;
    pBuf += sizeof(tANI_U8);
    msgLength += sizeof(tANI_U8);


  
    //              
    limCopyU16((tANI_U8*)pLen , msgLength);

    limPostSmeMessage(pMac, eWNI_SME_DISASSOC_REQ, (tANI_U32 *) pSmeDeauthReq);
    vos_mem_free(pSmeDeauthReq);

} /*                                 */



/* 
                          
  
           
                                                            
               
  
        
  
              
  
       
  
                                                 
               
 */

void
limTearDownLinkWithAp(tpAniSirGlobal pMac, tANI_U8 sessionId, tSirMacReasonCodes reasonCode)
{
    tpDphHashNode pStaDs = NULL;

    //                                    
    tpPESession psessionEntry;

    if((psessionEntry = peFindSessionBySessionId(pMac, sessionId))== NULL)
    {
        limLog(pMac, LOGP,FL("Session Does not exist for given sessionID"));
        return;
    }
    /* 
                                                 
                                                
                              
     */

    pMac->pmm.inMissedBeaconScenario = FALSE;
    limLog(pMac, LOGW,
       FL("No ProbeRsp from AP after HB failure. Tearing down link"));

    //                           
    limHeartBeatDeactivateAndChangeTimer(pMac, psessionEntry);

    //                                           
    //                                               

    pStaDs = dphGetHashEntry(pMac, DPH_STA_HASH_INDEX_PEER, &psessionEntry->dph.dphHashTable);

    
    if (pStaDs != NULL)
    {
        tLimMlmDeauthInd  mlmDeauthInd;

#ifdef FEATURE_WLAN_TDLS
#ifdef FEATURE_WLAN_TDLS_OXYGEN_DISAPPEAR_AP
        if ((TRUE == pMac->lim.gLimTDLSOxygenSupport) &&
            (limGetTDLSPeerCount(pMac, psessionEntry) != 0)) {
                limTDLSDisappearAPTrickInd(pMac, pStaDs, psessionEntry);
                return;
        }
#endif
        /*                                                   */
        limDeleteTDLSPeers(pMac, psessionEntry);
#endif

        pStaDs->mlmStaContext.disassocReason = reasonCode;
        pStaDs->mlmStaContext.cleanupTrigger = eLIM_LINK_MONITORING_DEAUTH;

        //                                 
        vos_mem_copy((tANI_U8 *) &mlmDeauthInd.peerMacAddr,
                      pStaDs->staAddr,
                      sizeof(tSirMacAddr));
        mlmDeauthInd.reasonCode    = (tANI_U8) pStaDs->mlmStaContext.disassocReason;
        mlmDeauthInd.deauthTrigger =  pStaDs->mlmStaContext.cleanupTrigger;

        limPostSmeMessage(pMac, LIM_MLM_DEAUTH_IND, (tANI_U32 *) &mlmDeauthInd);

        limSendSmeDeauthInd(pMac, pStaDs, psessionEntry);
    }    
} /*                             */




/* 
                              
  
           
                                                            
               
  
        
  
              
  
       
  
                                                 
               
 */

void limHandleHeartBeatFailure(tpAniSirGlobal pMac,tpPESession psessionEntry)
{

#ifdef FEATURE_WLAN_DIAG_SUPPORT_LIM //                         
    vos_log_beacon_update_pkt_type *log_ptr = NULL;
#endif //                          

    /*                                                                                    
                                                                                                   
                                                                                             
                                                                                         
                                                   
     */
    if(!IS_ACTIVEMODE_OFFLOAD_FEATURE_ENABLE && (!limIsSystemInActiveState(pMac)))
       return;

#ifdef FEATURE_WLAN_DIAG_SUPPORT_LIM //                         
    WLAN_VOS_DIAG_LOG_ALLOC(log_ptr, vos_log_beacon_update_pkt_type, LOG_WLAN_BEACON_UPDATE_C);
    if(log_ptr)
        log_ptr->bcn_rx_cnt = psessionEntry->LimRxedBeaconCntDuringHB;
    WLAN_VOS_DIAG_LOG_REPORT(log_ptr);
#endif //                         

    /*                                                   */
    psessionEntry->LimHBFailureStatus = eANI_BOOLEAN_FALSE;

    if (((psessionEntry->limSystemRole == eLIM_STA_ROLE)||(psessionEntry->limSystemRole == eLIM_BT_AMP_STA_ROLE))&&
         (psessionEntry->limMlmState == eLIM_MLM_LINK_ESTABLISHED_STATE))
    {
        if (!pMac->sys.gSysEnableLinkMonitorMode)
            return;

        /* 
                                                              
         */
        PELOGW(limLog(pMac, LOGW, FL("Heartbeat Failure"));)
        pMac->lim.gLimHBfailureCntInLinkEstState++;

        /* 
                                                                     
                                                                                    
         */
        if(!limIsconnectedOnDFSChannel(psessionEntry->currentOperChannel))
        {
            /*                                       */
             psessionEntry->LimHBFailureStatus= eANI_BOOLEAN_TRUE;
            /* 
                                                       
                                                     
                                                  
             */
            PELOGW(limLog(pMac, LOGW, FL("Heart Beat missed from AP. Sending Probe Req"));)
            /*                                                      */
            limSendProbeReqMgmtFrame(pMac, &psessionEntry->ssId, psessionEntry->bssId,
                                      psessionEntry->currentOperChannel,psessionEntry->selfMacAddr,
                                      psessionEntry->dot11mode, 0, NULL);
        }
        else
        {
            PELOGW(limLog(pMac, LOGW,
              FL("Heart Beat missed from AP on DFS chanel moving to passive"));)
            if (psessionEntry->currentOperChannel < SIR_MAX_24G_5G_CHANNEL_RANGE){
               limCovertChannelScanType(pMac, psessionEntry->currentOperChannel, false);
               pMac->lim.dfschannelList.timeStamp[psessionEntry->currentOperChannel] = 0;
            }
            /*                                                              
                                          */
            limTearDownLinkWithAp(pMac, psessionEntry->peSessionId, eSIR_MAC_UNSPEC_FAILURE_REASON);
        }
    }
    else
    {
        /* 
                                                 
                                                            
                                                             
                        
            */
        PELOG1(limLog(pMac, LOG1, FL("received heartbeat timeout in state %X"),
               psessionEntry->limMlmState);)
        limPrintMlmState(pMac, LOG1, psessionEntry->limMlmState);
        pMac->lim.gLimHBfailureCntInOtherStates++;
        limReactivateHeartBeatTimer(pMac, psessionEntry);
    }
} /*                                 */

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
                                                        
                                                         
                                      
                                  
                          
            
                                                         
                                                                       
  
 */
#include "palTypes.h"
#include "wniApi.h"
#include "sirApi.h"
#include "aniGlobal.h"
#include "wniCfgSta.h"

#include "utilsApi.h"
#include "limTypes.h"
#include "limUtils.h"
#include "limAssocUtils.h"
#include "limSecurityUtils.h"
#include "limSerDesUtils.h"
#include "limSendMessages.h"
#include "schApi.h"


/* 
                          
  
           
                                                           
                                  
  
        
  
              
                                                                   
  
       
  
                                                 
                                                                 
               
 */
void
limProcessDisassocFrame(tpAniSirGlobal pMac, tANI_U8 *pRxPacketInfo, tpPESession psessionEntry)
{
    tANI_U8                 *pBody;
    tANI_U16                aid, reasonCode;
    tpSirMacMgmtHdr    pHdr;
    tpDphHashNode      pStaDs;
    tLimMlmDisassocInd mlmDisassocInd;
#ifdef WLAN_FEATURE_11W
    tANI_U32            frameLen;
#endif

    pHdr = WDA_GET_RX_MAC_HEADER(pRxPacketInfo);
    pBody = WDA_GET_RX_MPDU_DATA(pRxPacketInfo);


    if (limIsGroupAddr(pHdr->sa))
    {
        //                                             
        //                        
        PELOG1(limLog(pMac, LOG1,
               FL("received Disassoc frame from a BC/MC address"));)

        return;
    }

    if (limIsGroupAddr(pHdr->da) && !limIsAddrBC(pHdr->da))
    {
        //                                         
        //                        
        PELOG1(limLog(pMac, LOG1,
               FL("received Disassoc frame for a MC address"));)

        return;
    }

#ifdef WLAN_FEATURE_11W
    /*                                                                                  */
    if(psessionEntry->limRmfEnabled  && (WDA_GET_RX_DPU_FEEDBACK(pRxPacketInfo) & DPU_FEEDBACK_UNPROTECTED_ERROR))
    {
        PELOGE(limLog(pMac, LOGE, FL("received an unprotected disassoc from AP"));)
        //                                                                               
        //            
        frameLen = WDA_GET_RX_PAYLOAD_LEN(pRxPacketInfo);
        //                                            
        limSendSmeUnprotectedMgmtFrameInd( pMac, pHdr->fc.subType,
                                           (tANI_U8*)pHdr, (frameLen + sizeof(tSirMacMgmtHdr)),
                                           psessionEntry->smeSessionId, psessionEntry);
        return;
    }
#endif

    //                                              
    reasonCode = sirReadU16(pBody);

#ifdef CUSTOMER_LGE_DEBUG_LOG
    PELOG2(limLog(pMac, LOGE,
        FL("Received Disassoc frame (mlm state %d sme state %d), with reason code %d from "MAC_ADDRESS_STR),
        psessionEntry->limMlmState, psessionEntry->limSmeState, reasonCode, MAC_ADDR_ARRAY(pHdr->sa));)
#endif

    PELOG2(limLog(pMac, LOG2,
        FL("Received Disassoc frame (mlm state %d sme state %d), with reason code %d from "MAC_ADDRESS_STR),
        psessionEntry->limMlmState, psessionEntry->limSmeState, reasonCode, MAC_ADDR_ARRAY(pHdr->sa));)

    /* 
                                                  
                                                  
   */
     pStaDs = dphLookupHashEntry(pMac, pHdr->sa, &aid, &psessionEntry->dph.dphHashTable);

    if (pStaDs == NULL)
    {
        /* 
                                                
                     
         */
        PELOG1(limLog(pMac, LOG1,
           FL("received Disassoc frame from STA that does not have context reasonCode=%d, addr "),
           reasonCode);
        limPrintMacAddr(pMac, pHdr->sa, LOG1);)

        return;
    }

    if (limCheckDisassocDeauthAckPending(pMac, (tANI_U8*)pHdr->sa))
    {
        PELOGW(limLog(pMac, LOGW,
                    FL("Ignore the DisAssoc received, while waiting for ack of disassoc/deauth"));)
        limCleanUpDisassocDeauthReq(pMac,(tANI_U8*)pHdr->sa, 1);
        return;
    }

    /*                                              */
    if (limIsReassocInProgress(pMac,psessionEntry)) {
        /*                                       
                                                                              
                           
                                             
        */
        if (!IS_REASSOC_BSSID(pMac,pHdr->sa,psessionEntry)) {
            PELOGW(limLog(pMac, LOGW, FL("Ignore the DisAssoc received, while Processing ReAssoc with different/unknown AP"));)
            return;
        }
        /*                                                                              
                                                                                 
         */
        if (!IS_CURRENT_BSSID(pMac, pHdr->sa,psessionEntry)) {
            PELOGW(limLog(pMac, LOGW, FL("received Disassoc from the New AP to which ReAssoc is sent "));)
            limRestorePreReassocState(pMac,
                                  eSIR_SME_REASSOC_REFUSED, reasonCode,psessionEntry);
            return;
        }
    }

    if ( (psessionEntry->limSystemRole == eLIM_AP_ROLE) ||
         (psessionEntry->limSystemRole == eLIM_BT_AMP_AP_ROLE) )
    {
        switch (reasonCode)
        {
            case eSIR_MAC_UNSPEC_FAILURE_REASON:
            case eSIR_MAC_DISASSOC_DUE_TO_INACTIVITY_REASON:
            case eSIR_MAC_DISASSOC_LEAVING_BSS_REASON:
            case eSIR_MAC_MIC_FAILURE_REASON:
            case eSIR_MAC_4WAY_HANDSHAKE_TIMEOUT_REASON:
            case eSIR_MAC_GR_KEY_UPDATE_TIMEOUT_REASON:
            case eSIR_MAC_RSN_IE_MISMATCH_REASON:
            case eSIR_MAC_1X_AUTH_FAILURE_REASON:
                //                                                  
                break;

            default:
                //                                                    
                PELOG1(limLog(pMac, LOG1,
                       FL("received Disassoc frame with invalid reasonCode %d from "),
                       reasonCode);
                limPrintMacAddr(pMac, pHdr->sa, LOG1);)
                break;
        }
    }
    else if (  ((psessionEntry->limSystemRole == eLIM_STA_ROLE) ||
                (psessionEntry->limSystemRole == eLIM_BT_AMP_STA_ROLE)) &&  
               ((psessionEntry->limSmeState != eLIM_SME_WT_JOIN_STATE) && 
                (psessionEntry->limSmeState != eLIM_SME_WT_AUTH_STATE)  &&
                (psessionEntry->limSmeState != eLIM_SME_WT_ASSOC_STATE)  &&
                (psessionEntry->limSmeState != eLIM_SME_WT_REASSOC_STATE) ))
    {
        switch (reasonCode)
        {
            case eSIR_MAC_UNSPEC_FAILURE_REASON:
            case eSIR_MAC_DISASSOC_DUE_TO_INACTIVITY_REASON:
            case eSIR_MAC_DISASSOC_DUE_TO_DISABILITY_REASON:
            case eSIR_MAC_CLASS2_FRAME_FROM_NON_AUTH_STA_REASON:
            case eSIR_MAC_CLASS3_FRAME_FROM_NON_ASSOC_STA_REASON:
            case eSIR_MAC_MIC_FAILURE_REASON:
            case eSIR_MAC_4WAY_HANDSHAKE_TIMEOUT_REASON:
            case eSIR_MAC_GR_KEY_UPDATE_TIMEOUT_REASON:
            case eSIR_MAC_RSN_IE_MISMATCH_REASON:
            case eSIR_MAC_1X_AUTH_FAILURE_REASON:
            case eSIR_MAC_PREV_AUTH_NOT_VALID_REASON:
                //                                                  
                break;

            case eSIR_MAC_DEAUTH_LEAVING_BSS_REASON:
            case eSIR_MAC_DISASSOC_LEAVING_BSS_REASON:
                //                                                  
                //                                             
                if(psessionEntry->gLimChannelSwitch.state != eLIM_CHANNEL_SWITCH_IDLE)
                {
                    limLog(pMac, LOGW,
                        FL("Ignoring disassoc frame due to upcoming "
                           "channel switch, from"),
                        reasonCode);
                    limPrintMacAddr(pMac, pHdr->sa, LOGW);
                    return;
                }
                break;

            default:
                //                                                    
                //                               
                PELOG1(limLog(pMac, LOG1,
                       FL("received Disassoc frame with invalid reasonCode %d from "),
                       reasonCode);
                limPrintMacAddr(pMac, pHdr->sa, LOG1);)
                return;
        }
    }
    else
    {
        //                                             
        //                                    
        limLog(pMac, LOG1,
               FL("received Disassoc frame with invalid reasonCode %d in role %d in sme state %d from "),
               reasonCode, psessionEntry->limSystemRole, psessionEntry->limSmeState);
        limPrintMacAddr(pMac, pHdr->sa, LOG1);

        return;
    }

    //                                    

   PELOG1(limLog(pMac, LOG1,
           FL("Received Disassoc frame from sta with assocId=%d with reasonCode=%d. Peer MAC is "MAC_ADDRESS_STR),
           pStaDs->assocId, reasonCode, MAC_ADDR_ARRAY(pHdr->sa));)

    if ((pStaDs->mlmStaContext.mlmState == eLIM_MLM_WT_DEL_STA_RSP_STATE) ||
        (pStaDs->mlmStaContext.mlmState == eLIM_MLM_WT_DEL_BSS_RSP_STATE))
    {
        /* 
                                                                  
                                                             
         */
        PELOG1(limLog(pMac, LOG1,
               FL("received Disassoc frame in state %d from"),
               pStaDs->mlmStaContext.mlmState);
        limPrintMacAddr(pMac, pHdr->sa, LOG1);)

        return;
    } 

    if (pStaDs->mlmStaContext.mlmState != eLIM_MLM_LINK_ESTABLISHED_STATE)
    {
        /* 
                                                       
                     
         */
        PELOG1(limLog(pMac, LOG1,
               FL("received Disassoc frame from peer that is in state %X, addr "),
               pStaDs->mlmStaContext.mlmState);
        limPrintMacAddr(pMac, pHdr->sa, LOG1);)
    } //                                                                       

    pStaDs->mlmStaContext.cleanupTrigger = eLIM_PEER_ENTITY_DISASSOC;
    pStaDs->mlmStaContext.disassocReason = (tSirMacReasonCodes) reasonCode;

    //                                  
    vos_mem_copy((tANI_U8 *) &mlmDisassocInd.peerMacAddr,
                 (tANI_U8 *) pStaDs->staAddr,
                 sizeof(tSirMacAddr));
    mlmDisassocInd.reasonCode =
        (tANI_U8) pStaDs->mlmStaContext.disassocReason;
    mlmDisassocInd.disassocTrigger = eLIM_PEER_ENTITY_DISASSOC;

    /*                       */
    mlmDisassocInd.sessionId = psessionEntry->peSessionId;

    if (limIsReassocInProgress(pMac,psessionEntry)) {

    /*                                                              
                                                                   
                                                                       
                                             
     */
        PELOGE(limLog(pMac, LOGE, FL("received Disassoc from AP while waiting for Reassoc Rsp"));)
     
        if (psessionEntry->limAssocResponseData) {
            vos_mem_free(psessionEntry->limAssocResponseData);
            psessionEntry->limAssocResponseData = NULL;                            
        }

        limRestorePreReassocState(pMac,eSIR_SME_REASSOC_REFUSED, reasonCode,psessionEntry);
        return;
    }
#if defined(FEATURE_WLAN_TDLS) && defined(FEATURE_WLAN_TDLS_OXYGEN_DISAPPEAR_AP)
    if ((TRUE == pMac->lim.gLimTDLSOxygenSupport) &&
        (limGetTDLSPeerCount(pMac, psessionEntry) != 0)) {
            limTDLSDisappearAPTrickInd(pMac, pStaDs, psessionEntry);
            return;
    }
#endif

    limPostSmeMessage(pMac, LIM_MLM_DISASSOC_IND,
                      (tANI_U32 *) &mlmDisassocInd);


    //                                    
    limSendSmeDisassocInd(pMac, pStaDs,psessionEntry);

    return;
} /*                                   */


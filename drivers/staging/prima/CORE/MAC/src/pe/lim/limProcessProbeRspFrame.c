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

#include "wniApi.h"
#include "wniCfgSta.h"
#include "aniGlobal.h"
#include "schApi.h"
#include "utilsApi.h"
#include "limApi.h"
#include "limTypes.h"
#include "limUtils.h"
#include "limAssocUtils.h"
#include "limPropExtsUtils.h"
#include "limSerDesUtils.h"
#include "limSendMessages.h"

#include "parserApi.h"

tSirRetStatus
limValidateIEInformationInProbeRspFrame (tANI_U8 *pRxPacketInfo)
{
   tSirRetStatus       status = eSIR_SUCCESS;

   if (WDA_GET_RX_PAYLOAD_LEN(pRxPacketInfo) < (SIR_MAC_B_PR_SSID_OFFSET + SIR_MAC_MIN_IE_LEN))
   {
      status = eSIR_FAILURE;
   }

   return status;
}

/* 
                          
  
           
                                                           
                                  
  
        
                                                         
  
              
  
       
                                               
                                                             
                                                                  
                                                                
  
                                                
                                                                           
               
 */


void
limProcessProbeRspFrame(tpAniSirGlobal pMac, tANI_U8 *pRxPacketInfo,tpPESession psessionEntry)
{
    tANI_U8                 *pBody;
    tANI_U32                frameLen = 0;
    tSirMacAddr             currentBssId;
    tpSirMacMgmtHdr         pHdr;
    tSirProbeRespBeacon    *pProbeRsp;
    tANI_U8 qosEnabled =    false;
    tANI_U8 wmeEnabled =    false;

    pProbeRsp = vos_mem_malloc(sizeof(tSirProbeRespBeacon));
    if ( NULL == pProbeRsp )
    {
        limLog(pMac, LOGE, FL("Unable to allocate memory in limProcessProbeRspFrame") );
        return;
    }

    pProbeRsp->ssId.length              = 0;
    pProbeRsp->wpa.length               = 0;
    pProbeRsp->propIEinfo.apName.length = 0;


    pHdr = WDA_GET_RX_MAC_HEADER(pRxPacketInfo);


   PELOG2(limLog(pMac, LOG2,
             FL("Received Probe Response frame with length=%d from "),
             WDA_GET_RX_MPDU_LEN(pRxPacketInfo));
    limPrintMacAddr(pMac, pHdr->sa, LOG2);)

   if (!pMac->fScanOffload)
   {
       if (limDeactivateMinChannelTimerDuringScan(pMac) != eSIR_SUCCESS)
       {
           vos_mem_free(pProbeRsp);
           return;
       }
   }

   //                                                               
   if (limValidateIEInformationInProbeRspFrame(pRxPacketInfo) != eSIR_SUCCESS)
   {
       PELOG1(limLog(pMac, LOG1,
                 FL("Parse error ProbeResponse, length=%d"), frameLen);)
       vos_mem_free(pProbeRsp);
       return;
   }

    /* 
                                      
                                                                  
                                                              
                         
                                                     
                                 
                                                                             
                                      
                                                                    
                                                                             
                                                    
                                                                 
      
                                                      
     */
        /*  */
   //                  
    if (((pMac->lim.gLimMlmState == eLIM_MLM_WT_PROBE_RESP_STATE) ||   //                                           
        (pMac->lim.gLimMlmState == eLIM_MLM_PASSIVE_SCAN_STATE) ||     //                                           
        (pMac->lim.gLimMlmState == eLIM_MLM_LEARN_STATE) ||            //                                            
        (psessionEntry->limMlmState == eLIM_MLM_WT_JOIN_BEACON_STATE) ||
        (psessionEntry->limMlmState == eLIM_MLM_LINK_ESTABLISHED_STATE) )||
        ((GET_LIM_SYSTEM_ROLE(psessionEntry) == eLIM_STA_IN_IBSS_ROLE) &&
        (psessionEntry->limMlmState == eLIM_MLM_BSS_STARTED_STATE)) ||
        pMac->fScanOffload)
    {
        frameLen = WDA_GET_RX_PAYLOAD_LEN(pRxPacketInfo);

        if (pMac->lim.gLimBackgroundScanMode == eSIR_ROAMING_SCAN)
        {
            VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO,
                      FL("Probe Resp Frame Received: BSSID " MAC_ADDRESS_STR " (RSSI %d)"),
                      MAC_ADDR_ARRAY(pHdr->bssId),
                      (uint)abs((tANI_S8)WDA_GET_RX_RSSI_DB(pRxPacketInfo)));
        }

        //                                         
        pBody = WDA_GET_RX_MPDU_DATA(pRxPacketInfo);

        if (sirConvertProbeFrame2Struct(pMac, pBody, frameLen, pProbeRsp) == eSIR_FAILURE ||
            !pProbeRsp->ssidPresent) //                      
        {
            PELOG1(limLog(pMac, LOG1,
               FL("Parse error ProbeResponse, length=%d"),
               frameLen);)
            vos_mem_free(pProbeRsp);
            return;
        }

        if (pMac->fScanOffload)
        {
            limCheckAndAddBssDescription(pMac, pProbeRsp, pRxPacketInfo,
                    eANI_BOOLEAN_FALSE, eANI_BOOLEAN_TRUE);
        }

        //                 
        if ((pMac->lim.gLimMlmState == eLIM_MLM_WT_PROBE_RESP_STATE) ||    //                                           
            (pMac->lim.gLimMlmState == eLIM_MLM_PASSIVE_SCAN_STATE))
            limCheckAndAddBssDescription(pMac, pProbeRsp, pRxPacketInfo, 
               ((pMac->lim.gLimHalScanState == eLIM_HAL_SCANNING_STATE) ? eANI_BOOLEAN_TRUE : eANI_BOOLEAN_FALSE), eANI_BOOLEAN_TRUE);
        else if (pMac->lim.gLimMlmState == eLIM_MLM_LEARN_STATE)           //                                           
        {
        }
        else if (psessionEntry->limMlmState ==
                                     eLIM_MLM_WT_JOIN_BEACON_STATE)
        {
            if( psessionEntry->beacon != NULL )//                                                                        
            {
                vos_mem_free(psessionEntry->beacon);
                psessionEntry->beacon = NULL;
            }
            psessionEntry->bcnLen = WDA_GET_RX_PAYLOAD_LEN(pRxPacketInfo);

            psessionEntry->beacon = vos_mem_malloc(psessionEntry->bcnLen);
            if ( NULL == psessionEntry->beacon )
            {
                PELOGE(limLog(pMac, LOGE,
                              FL("Unable to allocate memory to store beacon"));)
            }
            else
            {
                //                                                                         
                vos_mem_copy(psessionEntry->beacon,
                             WDA_GET_RX_MPDU_DATA(pRxPacketInfo),
                             psessionEntry->bcnLen);
            }

            //                            
            limCheckAndAnnounceJoinSuccess(pMac, pProbeRsp, pHdr, psessionEntry);
        }
        else if(psessionEntry->limMlmState == eLIM_MLM_LINK_ESTABLISHED_STATE)
        {
            tpDphHashNode pStaDs = NULL;
            /* 
                                                  
                                                  
                                  
            */
            #if 0
            if (wlan_cfgGetStr(pMac,
                          WNI_CFG_BSSID,
                          currentBssId,
                          &cfg) != eSIR_SUCCESS)
            {
                //                                          
                limLog(pMac, LOGP, FL("could not retrieve BSSID"));
            }
            #endif //                 
            sirCopyMacAddr(currentBssId,psessionEntry->bssId);

            if ( !vos_mem_compare(currentBssId, pHdr->bssId, sizeof(tSirMacAddr)) )
            {
                vos_mem_free(pProbeRsp);
                return;
            }

            if (!LIM_IS_CONNECTION_ACTIVE(psessionEntry))
            {
                limLog(pMac, LOGW,
                    FL("Received Probe Resp from AP. So it is alive!!"));

                if (pProbeRsp->HTInfo.present)
                    limReceivedHBHandler(pMac, (tANI_U8)pProbeRsp->HTInfo.primaryChannel, psessionEntry);
                else
                    limReceivedHBHandler(pMac, (tANI_U8)pProbeRsp->channelNumber, psessionEntry);
            }

            
            if (psessionEntry->limSystemRole == eLIM_STA_ROLE)
            {
                if (pProbeRsp->channelSwitchPresent ||
                    pProbeRsp->propIEinfo.propChannelSwitchPresent)
                {
                    limUpdateChannelSwitch(pMac, pProbeRsp, psessionEntry);
                }
                else if (psessionEntry->gLimSpecMgmt.dot11hChanSwState == eLIM_11H_CHANSW_RUNNING)
                {
                    limCancelDot11hChannelSwitch(pMac, psessionEntry);
                }
            }
        
            
            /* 
                                                                              
                                                                                                
                                                                        
                                                           
                                                                        
            */

            pStaDs = dphGetHashEntry(pMac, DPH_STA_HASH_INDEX_PEER, &psessionEntry->dph.dphHashTable);

            limGetQosMode(psessionEntry, &qosEnabled);
            limGetWmeMode(psessionEntry, &wmeEnabled);
           PELOG2(limLog(pMac, LOG2,
                    FL("wmeEdcaPresent: %d wmeEnabled: %d, edcaPresent: %d, qosEnabled: %d,  edcaParams.qosInfo.count: %d schObject.gLimEdcaParamSetCount: %d"),
                          pProbeRsp->wmeEdcaPresent, wmeEnabled, pProbeRsp->edcaPresent, qosEnabled,
                          pProbeRsp->edcaParams.qosInfo.count, psessionEntry->gLimEdcaParamSetCount);)
            if (((pProbeRsp->wmeEdcaPresent && wmeEnabled) ||
                (pProbeRsp->edcaPresent && qosEnabled)) &&
                (pProbeRsp->edcaParams.qosInfo.count != psessionEntry->gLimEdcaParamSetCount))
            {
                if (schBeaconEdcaProcess(pMac, &pProbeRsp->edcaParams, psessionEntry) != eSIR_SUCCESS)
                    PELOGE(limLog(pMac, LOGE, FL("EDCA parameter processing error"));)
                else if (pStaDs != NULL)
                {
                    //                                         
                    limSetActiveEdcaParams(pMac, psessionEntry->gLimEdcaParams, psessionEntry); 

                    if (pStaDs->aniPeer == eANI_BOOLEAN_TRUE)
                        limSendEdcaParams(pMac, psessionEntry->gLimEdcaParamsActive, pStaDs->bssId, eANI_BOOLEAN_TRUE);
                    else
                        limSendEdcaParams(pMac, psessionEntry->gLimEdcaParamsActive, pStaDs->bssId, eANI_BOOLEAN_FALSE);
                }
                else
                    PELOGE(limLog(pMac, LOGE, FL("Self Entry missing in Hash Table"));)

            }

           if (psessionEntry->fWaitForProbeRsp == true)
           {
               limLog(pMac, LOGW, FL("Checking probe response for capability change\n") );
               limDetectChangeInApCapabilities(pMac, pProbeRsp, psessionEntry);
           }
        }
        else if ((psessionEntry->limSystemRole == eLIM_STA_IN_IBSS_ROLE) &&
                 (psessionEntry->limMlmState == eLIM_MLM_BSS_STARTED_STATE))
                limHandleIBSScoalescing(pMac, pProbeRsp, pRxPacketInfo,psessionEntry);
    } //                                                                    

    vos_mem_free(pProbeRsp);
    //                                                
    return;
} /*                                   */


void
limProcessProbeRspFrameNoSession(tpAniSirGlobal pMac, tANI_U8 *pRxPacketInfo)
{
    tANI_U8                 *pBody;
    tANI_U32                frameLen = 0;
    tpSirMacMgmtHdr         pHdr;
    tSirProbeRespBeacon    *pProbeRsp;

    pProbeRsp = vos_mem_malloc(sizeof(tSirProbeRespBeacon));
    if ( NULL == pProbeRsp )
    {
        limLog(pMac, LOGE, FL("Unable to allocate memory in limProcessProbeRspFrameNoSession") );
        return;
    }

    pProbeRsp->ssId.length              = 0;
    pProbeRsp->wpa.length               = 0;
    pProbeRsp->propIEinfo.apName.length = 0;


    pHdr = WDA_GET_RX_MAC_HEADER(pRxPacketInfo);


    limLog(pMac, LOG2,
             FL("Received Probe Response frame with length=%d from "),
             WDA_GET_RX_MPDU_LEN(pRxPacketInfo));
    limPrintMacAddr(pMac, pHdr->sa, LOG2);

#ifdef WLAN_FEATURE_ROAM_SCAN_OFFLOAD
    if (!(WDA_GET_OFFLOADSCANLEARN(pRxPacketInfo) ||
          WDA_GET_ROAMCANDIDATEIND(pRxPacketInfo)))
    {
#endif
        if (!pMac->fScanOffload)
        {
            if (limDeactivateMinChannelTimerDuringScan(pMac) != eSIR_SUCCESS)
            {
                vos_mem_free(pProbeRsp);
                return;
            }
        }
#ifdef WLAN_FEATURE_ROAM_SCAN_OFFLOAD
    }
#endif
     //                                                               
    if (limValidateIEInformationInProbeRspFrame(pRxPacketInfo) != eSIR_SUCCESS)
    {
       PELOG1(limLog(pMac, LOG1,FL("Parse error ProbeResponse, length=%d"),
              frameLen);)
       vos_mem_free(pProbeRsp);
       return;
    }
    /*                                                                        
                                        
                                          
                                     
                                              
                                                                  
                                                     
        
                                                       
     */
#ifdef WLAN_FEATURE_ROAM_SCAN_OFFLOAD
    if (WDA_GET_OFFLOADSCANLEARN(pRxPacketInfo))
    {
        frameLen = WDA_GET_RX_PAYLOAD_LEN(pRxPacketInfo);

        if (pMac->lim.gLimBackgroundScanMode == eSIR_ROAMING_SCAN)
        {
            VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO,
                      FL("Probe Resp Frame Received: BSSID " MAC_ADDRESS_STR " (RSSI %d)"),
                      MAC_ADDR_ARRAY(pHdr->bssId),
                      (uint)abs((tANI_S8)WDA_GET_RX_RSSI_DB(pRxPacketInfo)));
        }

        //                                         
        pBody = WDA_GET_RX_MPDU_DATA(pRxPacketInfo);

        if (sirConvertProbeFrame2Struct(pMac, pBody, frameLen, pProbeRsp) == eSIR_FAILURE)
        {
            limLog(pMac, LOG1, FL("Parse error ProbeResponse, length=%d\n"), frameLen);
            vos_mem_free(pProbeRsp);
            return;
        }
        limLog( pMac, LOG2, FL("Save this probe rsp in LFR cache"));
        limCheckAndAddBssDescription(pMac, pProbeRsp, pRxPacketInfo,
                                     eANI_BOOLEAN_FALSE, eANI_BOOLEAN_TRUE);
    }
    else
#endif
    if (pMac->fScanOffload)
    {
        frameLen = WDA_GET_RX_PAYLOAD_LEN(pRxPacketInfo);

        //                                         
        pBody = WDA_GET_RX_MPDU_DATA(pRxPacketInfo);

        if (sirConvertProbeFrame2Struct(pMac, pBody, frameLen, pProbeRsp)
                == eSIR_FAILURE)
        {
            limLog(pMac, LOG1,
                    FL("Parse error ProbeResponse, length=%d\n"), frameLen);
            vos_mem_free(pProbeRsp);
            return;
        }
        limCheckAndAddBssDescription(pMac, pProbeRsp, pRxPacketInfo,
                eANI_BOOLEAN_FALSE, eANI_BOOLEAN_TRUE);
    }
    else if( (pMac->lim.gLimMlmState == eLIM_MLM_WT_PROBE_RESP_STATE) ||
        (pMac->lim.gLimMlmState == eLIM_MLM_PASSIVE_SCAN_STATE)  ||     //                                           
        (pMac->lim.gLimMlmState == eLIM_MLM_LEARN_STATE) )
    {
        frameLen = WDA_GET_RX_PAYLOAD_LEN(pRxPacketInfo);

        if (pMac->lim.gLimBackgroundScanMode == eSIR_ROAMING_SCAN)
        {
            VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO,
                      FL("Probe Resp Frame Received: BSSID " MAC_ADDRESS_STR " (RSSI %d)"),
                      MAC_ADDR_ARRAY(pHdr->bssId),
                      (uint)abs((tANI_S8)WDA_GET_RX_RSSI_DB(pRxPacketInfo)));
        }

        //                                         
        pBody = WDA_GET_RX_MPDU_DATA(pRxPacketInfo);

        if (sirConvertProbeFrame2Struct(pMac, pBody, frameLen, pProbeRsp) == eSIR_FAILURE)
        {
            limLog(pMac, LOG1, FL("Parse error ProbeResponse, length=%d"), frameLen);
            vos_mem_free(pProbeRsp);
            return;
        }

        if( (pMac->lim.gLimMlmState == eLIM_MLM_WT_PROBE_RESP_STATE) ||
             (pMac->lim.gLimMlmState == eLIM_MLM_PASSIVE_SCAN_STATE) )
            limCheckAndAddBssDescription(pMac, pProbeRsp, pRxPacketInfo, eANI_BOOLEAN_TRUE, eANI_BOOLEAN_TRUE);
        else if (pMac->lim.gLimMlmState == eLIM_MLM_LEARN_STATE)
        {
        }
    } 
    vos_mem_free(pProbeRsp);
    return;
} /*                                      */

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

#include "wniCfgSta.h"
#include "aniGlobal.h"
#include "cfgApi.h"
#include "schApi.h"
#include "utilsApi.h"
#include "limTypes.h"
#include "limUtils.h"
#include "limAssocUtils.h"
#include "limPropExtsUtils.h"
#include "limSerDesUtils.h"

/* 
                        
  
           
                                                                  
                   
  
        
  
              
  
       
                                                               
                               
  
                                                 
                                                                 
               
 */

void
limProcessBeaconFrame(tpAniSirGlobal pMac, tANI_U8 *pRxPacketInfo,tpPESession psessionEntry)
{
    tpSirMacMgmtHdr      pHdr;
    tSchBeaconStruct    *pBeacon;

    pMac->lim.gLimNumBeaconsRcvd++;

    /*                                                                            */  


    
    pHdr = WDA_GET_RX_MAC_HEADER(pRxPacketInfo);


    PELOG2(limLog(pMac, LOG2, FL("Received Beacon frame with length=%d from "),
           WDA_GET_RX_MPDU_LEN(pRxPacketInfo));
    limPrintMacAddr(pMac, pHdr->sa, LOG2);)

    if (!pMac->fScanOffload)
    {
        if (limDeactivateMinChannelTimerDuringScan(pMac) != eSIR_SUCCESS)
            return;
    }

    /* 
                              
                                                                  
                                                       
                                   
                                 
     */
    if ((pMac->lim.gLimMlmState == eLIM_MLM_WT_PROBE_RESP_STATE) ||
        (pMac->lim.gLimMlmState == eLIM_MLM_PASSIVE_SCAN_STATE) ||
        (pMac->lim.gLimMlmState == eLIM_MLM_LEARN_STATE) ||
        (psessionEntry->limMlmState == eLIM_MLM_WT_JOIN_BEACON_STATE)
        || pMac->fScanOffload
        )
    {
        pBeacon = vos_mem_malloc(sizeof(tSchBeaconStruct));
        if ( NULL == pBeacon )
        {
            limLog(pMac, LOGE, FL("Unable to allocate memory in limProcessBeaconFrame") );
            return;
        }

        //                      
        if (sirConvertBeaconFrame2Struct(pMac, (tANI_U8 *) pRxPacketInfo,
                                         pBeacon) != eSIR_SUCCESS)
        {
            //                                           
            //                       
            limLog(pMac, LOGW,
                   FL("Received invalid Beacon in state %X"),
                   psessionEntry->limMlmState);
            limPrintMlmState(pMac, LOGW,  psessionEntry->limMlmState);
            vos_mem_free(pBeacon);
            return;
        }

        /*                                                                     
                                                                     */
        if ((!psessionEntry->lastBeaconDtimPeriod) &&
            (sirCompareMacAddr( psessionEntry->bssId, pBeacon->bssid)))
        {
            vos_mem_copy(( tANI_U8* )&psessionEntry->lastBeaconTimeStamp,
                         ( tANI_U8* )pBeacon->timeStamp, sizeof(tANI_U64) );
            psessionEntry->lastBeaconDtimCount = pBeacon->tim.dtimCount;
            psessionEntry->lastBeaconDtimPeriod= pBeacon->tim.dtimPeriod;
            psessionEntry->currentBssBeaconCnt++;
        }

        MTRACE(macTrace(pMac, TRACE_CODE_RX_MGMT_TSF, 0, pBeacon->timeStamp[0]);)
        MTRACE(macTrace(pMac, TRACE_CODE_RX_MGMT_TSF, 0, pBeacon->timeStamp[1]);)

        if (pMac->fScanOffload)
        {
            limCheckAndAddBssDescription(pMac, pBeacon, pRxPacketInfo,
                    eANI_BOOLEAN_FALSE, eANI_BOOLEAN_TRUE);

        }

        if ((pMac->lim.gLimMlmState  == eLIM_MLM_WT_PROBE_RESP_STATE) ||
            (pMac->lim.gLimMlmState  == eLIM_MLM_PASSIVE_SCAN_STATE))
        {
            limCheckAndAddBssDescription(pMac, pBeacon, pRxPacketInfo,
                  ((pMac->lim.gLimHalScanState == eLIM_HAL_SCANNING_STATE) ?
                    eANI_BOOLEAN_TRUE : eANI_BOOLEAN_FALSE),
                    eANI_BOOLEAN_FALSE);
            /*                                                      
                                                                           */
             limSetDFSChannelList(pMac, pBeacon->channelNumber,
                                    &pMac->lim.dfschannelList);
        }
        else if (pMac->lim.gLimMlmState == eLIM_MLM_LEARN_STATE)
        {
        }
        else if (psessionEntry->limMlmState == eLIM_MLM_WT_JOIN_BEACON_STATE)
        {
            if( psessionEntry->beacon != NULL )
            {
                vos_mem_free(psessionEntry->beacon);
                psessionEntry->beacon = NULL;
             }
             psessionEntry->bcnLen = WDA_GET_RX_PAYLOAD_LEN(pRxPacketInfo);
             psessionEntry->beacon = vos_mem_malloc(psessionEntry->bcnLen);
             if ( NULL == psessionEntry->beacon )
             {
                PELOGE(limLog(pMac, LOGE, FL("Unable to allocate memory to store beacon"));)
              }
              else
              {
                //                                                                         
                vos_mem_copy(psessionEntry->beacon, WDA_GET_RX_MPDU_DATA(pRxPacketInfo),
                             psessionEntry->bcnLen);

               }
             
             //                                   
            limCheckAndAnnounceJoinSuccess(pMac, pBeacon, pHdr,psessionEntry);
        } //                                                            
        vos_mem_free(pBeacon);
    } //                                                                    
    else
    {
        //                                        
        if (psessionEntry->limMlmState == eLIM_MLM_JOINED_STATE ||
            psessionEntry->limMlmState  == eLIM_MLM_BSS_STARTED_STATE ||
            psessionEntry->limMlmState  == eLIM_MLM_WT_AUTH_FRAME2_STATE ||
            psessionEntry->limMlmState == eLIM_MLM_WT_AUTH_FRAME3_STATE ||
            psessionEntry->limMlmState  == eLIM_MLM_WT_AUTH_FRAME4_STATE ||
            psessionEntry->limMlmState  == eLIM_MLM_AUTH_RSP_TIMEOUT_STATE ||
            psessionEntry->limMlmState == eLIM_MLM_AUTHENTICATED_STATE ||
            psessionEntry->limMlmState  == eLIM_MLM_WT_ASSOC_RSP_STATE ||
            psessionEntry->limMlmState == eLIM_MLM_WT_REASSOC_RSP_STATE ||
            psessionEntry->limMlmState  == eLIM_MLM_ASSOCIATED_STATE ||
            psessionEntry->limMlmState  == eLIM_MLM_REASSOCIATED_STATE ||
            psessionEntry->limMlmState  == eLIM_MLM_WT_ASSOC_CNF_STATE ||
            limIsReassocInProgress(pMac,psessionEntry)) {
            //                                                
            pMac->lim.gLimNumBeaconsIgnored++;
        }
        else
        {
            PELOG1(limLog(pMac, LOG1, FL("Received Beacon in unexpected state %d"),
                   psessionEntry->limMlmState);
            limPrintMlmState(pMac, LOG1, psessionEntry->limMlmState);)
#ifdef WLAN_DEBUG                    
            pMac->lim.gLimUnexpBcnCnt++;
#endif
        }
    }

    return;
} /*                                 */


/*                                                                
                                      
                                                           
                               
 
           
                                                                
            
                                                                  */
void
limProcessBeaconFrameNoSession(tpAniSirGlobal pMac, tANI_U8 *pRxPacketInfo)
{
    tpSirMacMgmtHdr      pHdr;
    tSchBeaconStruct    *pBeacon;

    pMac->lim.gLimNumBeaconsRcvd++;
    pHdr = WDA_GET_RX_MAC_HEADER(pRxPacketInfo);

    limLog(pMac, LOG2, FL("Received Beacon frame with length=%d from "),
           WDA_GET_RX_MPDU_LEN(pRxPacketInfo));
    limPrintMacAddr(pMac, pHdr->sa, LOG2);

    if (!pMac->fScanOffload)
    {
        if (limDeactivateMinChannelTimerDuringScan(pMac) != eSIR_SUCCESS)
            return;
    }

    /* 
                                                               
                                                                  
                                 
     */
    if ((pMac->lim.gLimMlmState == eLIM_MLM_WT_PROBE_RESP_STATE) ||
        (pMac->lim.gLimMlmState == eLIM_MLM_PASSIVE_SCAN_STATE) ||
        (pMac->lim.gLimMlmState == eLIM_MLM_LEARN_STATE))
    {
        pBeacon = vos_mem_malloc(sizeof(tSchBeaconStruct));
        if ( NULL == pBeacon )
        {
            limLog(pMac, LOGE, FL("Unable to allocate memory in limProcessBeaconFrameNoSession") );
            return;
        }

        if (sirConvertBeaconFrame2Struct(pMac, (tANI_U8 *) pRxPacketInfo, pBeacon) != eSIR_SUCCESS)
        {
            //                                                                
            limLog(pMac, LOGW, FL("Received invalid Beacon in global MLM state %X"), pMac->lim.gLimMlmState);
            limPrintMlmState(pMac, LOGW,  pMac->lim.gLimMlmState);
            vos_mem_free(pBeacon);
            return;
        }

        if ( (pMac->lim.gLimMlmState == eLIM_MLM_WT_PROBE_RESP_STATE) ||
             (pMac->lim.gLimMlmState == eLIM_MLM_PASSIVE_SCAN_STATE) )
        {
            limCheckAndAddBssDescription(pMac, pBeacon, pRxPacketInfo,
                                         eANI_BOOLEAN_TRUE, eANI_BOOLEAN_FALSE);
            /*                                                      
                                                                           */
            limSetDFSChannelList(pMac, pBeacon->channelNumber,
                                    &pMac->lim.dfschannelList);
        }
        else if (pMac->lim.gLimMlmState == eLIM_MLM_LEARN_STATE)
        {
        }  //                                    
        vos_mem_free(pBeacon);
    } //                                                                       
    else
    {
        limLog(pMac, LOG1, FL("Rcvd Beacon in unexpected MLM state %d"), pMac->lim.gLimMlmState);
        limPrintMlmState(pMac, LOG1, pMac->lim.gLimMlmState);
#ifdef WLAN_DEBUG                    
        pMac->lim.gLimUnexpBcnCnt++;
#endif
    }

    return;
} /*                                          */


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
#include "cfgApi.h"

#include "utilsApi.h"
#include "pmmApi.h"
#include "limTypes.h"
#include "limUtils.h"
#include "limAssocUtils.h"
#include "limSecurityUtils.h"
#include "limSerDesUtils.h"
#include "limStaHashApi.h"
#include "limSendMessages.h"

#if defined(FEATURE_WLAN_CCX) && !defined(FEATURE_WLAN_CCX_UPLOAD)
#include "ccxApi.h"
#endif

extern tSirRetStatus schBeaconEdcaProcess(tpAniSirGlobal pMac, tSirMacEdcaParamSetIE *edca, tpPESession psessionEntry);


/* 
                                     
  
                                                                                        
                                                              
  
        
  
              
  
       
  
                                                    
                                               
                                                                   
  
               
 */
void limUpdateAssocStaDatas(tpAniSirGlobal pMac, tpDphHashNode pStaDs, tpSirAssocRsp pAssocRsp,tpPESession psessionEntry)
{
    tANI_U32        prop;
    tANI_U32        phyMode;
    tANI_U32        val;
    //                                  
    tANI_BOOLEAN    qosMode; 
    tANI_U16        rxHighestRate = 0;

    limGetPhyMode(pMac, &phyMode, psessionEntry);

    pStaDs->staType= STA_ENTRY_SELF;

    limGetQosMode(psessionEntry, &qosMode);    
    //                                                                   
    if(IS_DOT11_MODE_PROPRIETARY(psessionEntry->dot11mode)) 
    {
       wlan_cfgGetInt(pMac, WNI_CFG_PROPRIETARY_ANI_FEATURES_ENABLED, &prop);

       if (prop) 
       {
           pStaDs->aniPeer = eHAL_SET;
           pStaDs->propCapability = pAssocRsp->propIEinfo.capability;
       }
    }
    
       //                                                                 
       pStaDs->mlmStaContext.authType = psessionEntry->limCurrentAuthType;
    
       //                                            
       pStaDs->mlmStaContext.capabilityInfo = pAssocRsp->capabilityInfo;
       pStaDs->shortPreambleEnabled= (tANI_U8)pAssocRsp->capabilityInfo.shortPreamble;
    
       //                                                         
       if(IS_DOT11_MODE_HT(psessionEntry->dot11mode)) {
           pStaDs->mlmStaContext.htCapability = pAssocRsp->HTCaps.present;
    
           if ( pAssocRsp->HTCaps.present ) {
               pStaDs->htGreenfield = ( tANI_U8 ) pAssocRsp->HTCaps.greenField;
               pStaDs->htSupportedChannelWidthSet = ( tANI_U8 ) (pAssocRsp->HTCaps.supportedChannelWidthSet ? 
                                                                               pAssocRsp->HTInfo.recommendedTxWidthSet : 
                                                                               pAssocRsp->HTCaps.supportedChannelWidthSet );
                   pStaDs->htLsigTXOPProtection = ( tANI_U8 ) pAssocRsp->HTCaps.lsigTXOPProtection;
                   pStaDs->htMIMOPSState =  (tSirMacHTMIMOPowerSaveState)pAssocRsp->HTCaps.mimoPowerSave;
                   pStaDs->htMaxAmsduLength = ( tANI_U8 ) pAssocRsp->HTCaps.maximalAMSDUsize;
                   pStaDs->htAMpduDensity =             pAssocRsp->HTCaps.mpduDensity;
                   pStaDs->htDsssCckRate40MHzSupport = (tANI_U8)pAssocRsp->HTCaps.dsssCckMode40MHz;
                   pStaDs->htShortGI20Mhz = (tANI_U8)pAssocRsp->HTCaps.shortGI20MHz;
                   pStaDs->htShortGI40Mhz = (tANI_U8)pAssocRsp->HTCaps.shortGI40MHz;
                   pStaDs->htMaxRxAMpduFactor = pAssocRsp->HTCaps.maxRxAMPDUFactor;
                   limFillRxHighestSupportedRate(pMac, &rxHighestRate, pAssocRsp->HTCaps.supportedMCSSet);
                   pStaDs->supportedRates.rxHighestDataRate = rxHighestRate;
                   /*                                                                                                       */
                   pStaDs->htSecondaryChannelOffset = (tANI_U8)pAssocRsp->HTInfo.secondaryChannelOffset;

                   //           
                   //                                                              
                   //                                              
                   pStaDs->baPolicyFlag = 0xFF;
           }
       }

#ifdef WLAN_FEATURE_11AC
       if(IS_DOT11_MODE_VHT(psessionEntry->dot11mode))
       {
           pStaDs->mlmStaContext.vhtCapability = pAssocRsp->VHTCaps.present;
       }
       if (limPopulatePeerRateSet(pMac, &pStaDs->supportedRates,
                                pAssocRsp->HTCaps.supportedMCSSet,
                                false,psessionEntry , &pAssocRsp->VHTCaps) != eSIR_SUCCESS) 
#else
       if (limPopulatePeerRateSet(pMac, &pStaDs->supportedRates, pAssocRsp->HTCaps.supportedMCSSet, false,psessionEntry) != eSIR_SUCCESS)
#endif
       {
           limLog(pMac, LOGP, FL("could not get rateset and extended rate set"));
           return;
       }
   
       //                                                   
       if ((phyMode == WNI_CFG_PHY_MODE_11G) && sirIsArate(pStaDs->supportedRates.llaRates[0] & 0x7f))
           pStaDs->erpEnabled = eHAL_SET;
    
    
       val = WNI_CFG_PROPRIETARY_OPERATIONAL_RATE_SET_LEN;
       if (wlan_cfgGetStr(pMac, WNI_CFG_PROPRIETARY_OPERATIONAL_RATE_SET,
                     (tANI_U8 *) &pStaDs->mlmStaContext.propRateSet.propRate,
                     &val) != eSIR_SUCCESS) {
           //                                                 
           limLog(pMac, LOGP, FL("could not retrieve prop rateset"));
           return;
       }
       pStaDs->mlmStaContext.propRateSet.numPropRates = (tANI_U8) val;
    
       pStaDs->qosMode    = 0;
       pStaDs->lleEnabled = 0;
    
       //                          
       //                             
       if (qosMode) {
           if (pAssocRsp->edcaPresent) {
               tSirRetStatus status;
               status = schBeaconEdcaProcess(pMac,&pAssocRsp->edca, psessionEntry);
              PELOG2(limLog(pMac, LOG2, "Edca set update based on AssocRsp: status %d",
                      status);)
               if (status != eSIR_SUCCESS) {
                   PELOGE(limLog(pMac, LOGE, FL("Edca error in AssocResp "));)
               } else { //                                   
                   pStaDs->qosMode    = 1;
                   pStaDs->lleEnabled = 1;
               }
           }
       }
    
       pStaDs->wmeEnabled = 0;
       pStaDs->wsmEnabled = 0;
       if (psessionEntry->limWmeEnabled && pAssocRsp->wmeEdcaPresent)
       {
           tSirRetStatus status;
           status = schBeaconEdcaProcess(pMac,&pAssocRsp->edca, psessionEntry);
           PELOGW(limLog(pMac, LOGW, "WME Edca set update based on AssocRsp: status %d", status);)

           if (status != eSIR_SUCCESS)
               PELOGE(limLog(pMac, LOGE, FL("WME Edca error in AssocResp - ignoring"));)
           else { //                                       
               pStaDs->qosMode    = 1;
               pStaDs->wmeEnabled = 1;
           }
       } 
       else {
           /*                                                                
                                                                               
                                                                                  
            */ 
           schSetDefaultEdcaParams(pMac, psessionEntry);
       }

       if(qosMode && (!pStaDs->qosMode) && pStaDs->mlmStaContext.htCapability)
       {
           //                                                                        
           pStaDs->qosMode    = 1;
           pStaDs->wmeEnabled = 1;
       }

#ifdef WLAN_FEATURE_11W
       if(psessionEntry->limRmfEnabled)
       {
           pStaDs->rmfEnabled = 1;
       }
#endif
}

/* 
                                      
  
                                                                                
  
        
  
              
  
       
  
                                                    
                                                                   
  
               
 */

void limUpdateReAssocGlobals(tpAniSirGlobal pMac, tpSirAssocRsp pAssocRsp,tpPESession psessionEntry)
{
    /* 
                                       
     */
    pmmResetPmmState(pMac);

    //                                   
    vos_mem_copy(psessionEntry->bssId,
                 psessionEntry->limReAssocbssId, sizeof(tSirMacAddr));
    psessionEntry->currentOperChannel = psessionEntry->limReassocChannelId;
    psessionEntry->htSecondaryChannelOffset = psessionEntry->reAssocHtSupportedChannelWidthSet;
    psessionEntry->htRecommendedTxWidthSet = psessionEntry->reAssocHtRecommendedTxWidthSet;
    psessionEntry->htSecondaryChannelOffset = psessionEntry->reAssocHtSecondaryChannelOffset;
    psessionEntry->limCurrentBssCaps   = psessionEntry->limReassocBssCaps;
    psessionEntry->limCurrentBssQosCaps = psessionEntry->limReassocBssQosCaps;
    psessionEntry->limCurrentBssPropCap = psessionEntry->limReassocBssPropCap;

    vos_mem_copy((tANI_U8 *) &psessionEntry->ssId,
                 (tANI_U8 *) &psessionEntry->limReassocSSID,
                  psessionEntry->limReassocSSID.length+1);
    
    //                                      
    psessionEntry->limAID = pAssocRsp->aid & 0x3FFF;
    /*                                   */
    psessionEntry->limMlmState = eLIM_MLM_WT_REASSOC_RSP_STATE; 
    MTRACE(macTrace(pMac, TRACE_CODE_MLM_STATE, psessionEntry->peSessionId, psessionEntry->limMlmState));

    
}

/* 
                                      
  
                                                                     
                                                        
  
        
  
              
  
       
  
                                                    
                                                                    
                                                                         
                                                      
  
               
 */

void
limProcessAssocRspFrame(tpAniSirGlobal pMac, tANI_U8 *pRxPacketInfo, tANI_U8 subType,tpPESession psessionEntry)
{
    tANI_U8               *pBody;
    tANI_U16              caps;
    tANI_U32              frameLen;
    tSirMacAddr           currentBssId;
    tpSirMacMgmtHdr       pHdr;
    tSirMacCapabilityInfo localCapabilities;
    tpDphHashNode         pStaDs;
    tpSirAssocRsp         pAssocRsp;
    tLimMlmAssocCnf       mlmAssocCnf;
    
    tSchBeaconStruct *pBeaconStruct;

    //                                  

    pHdr = WDA_GET_RX_MAC_HEADER(pRxPacketInfo);

    mlmAssocCnf.resultCode = eSIR_SME_SUCCESS;
    /*                     */
    mlmAssocCnf.sessionId = psessionEntry->peSessionId;

    pBeaconStruct = vos_mem_malloc(sizeof(tSchBeaconStruct));
    if (NULL == pBeaconStruct)
    {
        limLog(pMac, LOGE, FL("Unable to allocate memory in limProcessAssocRspFrame") );
        return;
    }

   
    if (psessionEntry->limSystemRole == eLIM_AP_ROLE || psessionEntry->limSystemRole == eLIM_BT_AMP_AP_ROLE )
    {
        //                                                 
        //                       
        limLog(pMac, LOGE,
               FL("received Re/Assoc response frame on role %d "),
               psessionEntry->limSystemRole);

        vos_mem_free(pBeaconStruct);
        return;
    }


    pHdr = WDA_GET_RX_MAC_HEADER(pRxPacketInfo);
    frameLen = WDA_GET_RX_PAYLOAD_LEN(pRxPacketInfo);

    if (((subType == LIM_ASSOC) &&
         (psessionEntry->limMlmState != eLIM_MLM_WT_ASSOC_RSP_STATE)) ||
        ((subType == LIM_REASSOC) &&
         ((psessionEntry->limMlmState != eLIM_MLM_WT_REASSOC_RSP_STATE) 
#if defined (WLAN_FEATURE_VOWIFI_11R) || defined (FEATURE_WLAN_CCX) || defined(FEATURE_WLAN_LFR)
         && (psessionEntry->limMlmState != eLIM_MLM_WT_FT_REASSOC_RSP_STATE)
#endif
         )))
    {
        //                                                   

#ifdef WLAN_FEATURE_VOWIFI_11R_DEBUG
        PELOG1(limLog(pMac, LOG1,  FL("mlm state is set to %d session=%d"),
            psessionEntry->limMlmState, psessionEntry->peSessionId);)
#endif
        //          
        if (!pHdr->fc.retry)
        {
            limLog(pMac, LOGE,
               FL("received Re/Assoc rsp frame in unexpected state"));
            limPrintMlmState(pMac, LOGE, psessionEntry->limMlmState);
        }
        vos_mem_free(pBeaconStruct);
        return;
    }
#if 0
    if (wlan_cfgGetStr(pMac, WNI_CFG_BSSID, currentBssId, &cfg) !=
                                eSIR_SUCCESS)
    {
        //                                          
        limLog(pMac, LOGP, FL("could not retrieve BSSID"));
        vos_mem_free(pBeaconStruct);
        return;
    }
#endif //                 
    sirCopyMacAddr(currentBssId,psessionEntry->bssId);

    if (subType == LIM_ASSOC)
    {
        if (!vos_mem_compare(pHdr->sa, currentBssId, sizeof(tSirMacAddr)))
        {
            /* 
                                                                 
                                                             
                                                                      
             */

            //          
            PELOGW(limLog(pMac, LOGW,
                   FL("received AssocRsp frame from unexpected peer "MAC_ADDRESS_STR),
                   MAC_ADDR_ARRAY(pHdr->sa));)
            vos_mem_free(pBeaconStruct);
            return;
        }
    }
    else
    {
        if (!vos_mem_compare(pHdr->sa, psessionEntry->limReAssocbssId, sizeof(tSirMacAddr)))
        {
            /* 
                                                                   
                                                             
                                                                        
             */

            //          
            PELOGW(limLog(pMac, LOGW,
                   FL("received ReassocRsp frame from unexpected peer "MAC_ADDRESS_STR),
                   MAC_ADDR_ARRAY(pHdr->sa));)
            vos_mem_free(pBeaconStruct);

            return;
        }
    }

   pAssocRsp = vos_mem_malloc(sizeof(*pAssocRsp));
   if (NULL == pAssocRsp)
   {
        limLog(pMac, LOGP, FL("Allocate Memory failed in AssocRsp"));
        vos_mem_free(pBeaconStruct);

        return;
    }
   
   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_DEBUG,
             FL("Re/Assoc Resp Frame Received: BSSID " MAC_ADDRESS_STR " (RSSI %d)"),
             MAC_ADDR_ARRAY(pHdr->bssId),
             (uint)abs((tANI_S8)WDA_GET_RX_RSSI_DB(pRxPacketInfo)));

    //                                                  
    pBody = WDA_GET_RX_MPDU_DATA(pRxPacketInfo);

    //                                     
    if (sirConvertAssocRespFrame2Struct(
                        pMac, pBody, frameLen, pAssocRsp) == eSIR_FAILURE) 
    {
        vos_mem_free(pAssocRsp);
        PELOGE(limLog(pMac, LOGE, FL("Parse error Assoc resp subtype %d, length=%d"), frameLen,subType);)
        vos_mem_free(pBeaconStruct);

        return;
    }

    if(!pAssocRsp->suppRatesPresent)
    {
        PELOGE(limLog(pMac, LOGW, FL("assoc response does not have supported rate set"));)
        vos_mem_copy(&pAssocRsp->supportedRates,
                      &psessionEntry->rateSet, sizeof(tSirMacRateSet));
    }

    mlmAssocCnf.protStatusCode = pAssocRsp->statusCode;

    if( psessionEntry->assocRsp != NULL )
    {
        vos_mem_free(psessionEntry->assocRsp);
        psessionEntry->assocRsp = NULL;
    }

    psessionEntry->assocRsp = vos_mem_malloc(frameLen);
    if (NULL == psessionEntry->assocRsp)
    {
        PELOGE(limLog(pMac, LOGE, FL("Unable to allocate memory to store assoc response, len = %d"), frameLen);)
    }
    else
    {
        //                                                                        
        vos_mem_copy(psessionEntry->assocRsp, pBody, frameLen);
        psessionEntry->assocRspLen = frameLen;
    }

#ifdef WLAN_FEATURE_VOWIFI_11R
    if (psessionEntry->ricData != NULL) 
    {
        vos_mem_free(psessionEntry->ricData);
        psessionEntry->ricData = NULL;
    }
    if(pAssocRsp->ricPresent)
    {
        psessionEntry->RICDataLen = pAssocRsp->num_RICData * sizeof(tDot11fIERICDataDesc);
        psessionEntry->ricData = vos_mem_malloc(psessionEntry->RICDataLen);
        if ( NULL == psessionEntry->ricData )
        {
            PELOGE(limLog(pMac, LOGE, FL("Unable to allocate memory to store assoc response"));)
            psessionEntry->RICDataLen = 0; 
        }
        else
        {
            vos_mem_copy(psessionEntry->ricData,
                         &pAssocRsp->RICData[0], psessionEntry->RICDataLen);
        }
    }
    else
    {
        psessionEntry->RICDataLen = 0;
        psessionEntry->ricData = NULL;
    }
#endif    

#ifdef FEATURE_WLAN_CCX    
    if (psessionEntry->tspecIes != NULL) 
    {
        vos_mem_free(psessionEntry->tspecIes);
        psessionEntry->tspecIes = NULL;
    }
    if(pAssocRsp->tspecPresent)
    {
        psessionEntry->tspecLen = pAssocRsp->num_tspecs * sizeof(tDot11fIEWMMTSPEC);
        psessionEntry->tspecIes = vos_mem_malloc(psessionEntry->tspecLen);
        if ( NULL == psessionEntry->tspecIes )
        {
            PELOGE(limLog(pMac, LOGE, FL("Unable to allocate memory to store assoc response"));)
            psessionEntry->tspecLen = 0; 
        }
        else
        {
            vos_mem_copy(psessionEntry->tspecIes,
                         &pAssocRsp->TSPECInfo[0], psessionEntry->tspecLen);
        }
        PELOG1(limLog(pMac, LOG1, FL(" Tspec EID present in assoc rsp "));)
    }
    else
    {
        psessionEntry->tspecLen = 0;
        psessionEntry->tspecIes = NULL;
        PELOG1(limLog(pMac, LOG1, FL(" Tspec EID *NOT* present in assoc rsp "));)
    }
#endif

    if (pAssocRsp->capabilityInfo.ibss)
    {
        /* 
                                                     
                                    
                                                   
                           
         */
        
        //          
        limLog(pMac, LOGE,
               FL("received Re/AssocRsp frame with IBSS capability"));
        vos_mem_free(pAssocRsp);
        vos_mem_free(pBeaconStruct);

        return;
    }

    if (cfgGetCapabilityInfo(pMac, &caps,psessionEntry) != eSIR_SUCCESS)
    {
        /* 
                                           
                               
         */         
        vos_mem_free(pAssocRsp);
        vos_mem_free(pBeaconStruct);

        limLog(pMac, LOGP, FL("could not retrieve Capabilities value"));
        return;
    }
    limCopyU16((tANI_U8 *) &localCapabilities, caps);

    if (subType == LIM_ASSOC)        //                               
        limDeactivateAndChangeTimer(pMac, eLIM_ASSOC_FAIL_TIMER);
    else        //                                 
    {
#if  defined (WLAN_FEATURE_VOWIFI_11R) || defined (FEATURE_WLAN_CCX) || defined(FEATURE_WLAN_LFR)
        pMac->lim.reAssocRetryAttempt = 0;
        if ((NULL != pMac->lim.pSessionEntry) && (NULL != pMac->lim.pSessionEntry->pLimMlmReassocRetryReq))
        {
            vos_mem_free(pMac->lim.pSessionEntry->pLimMlmReassocRetryReq);
            pMac->lim.pSessionEntry->pLimMlmReassocRetryReq = NULL;
        }
#endif
        limDeactivateAndChangeTimer(pMac, eLIM_REASSOC_FAIL_TIMER);
    }

    if (pAssocRsp->statusCode != eSIR_MAC_SUCCESS_STATUS)
    {
        //                                     
        //                          
        //           
        PELOGE(limLog(pMac, LOGE, FL("received Re/AssocRsp frame failure code %d"), pAssocRsp->statusCode);)
        //                                                   
        //                       

        //                                                       

        if (pAssocRsp->propIEinfo.loadBalanceInfoPresent)
        {
            mlmAssocCnf.resultCode = eSIR_SME_TRANSFER_STA;
            vos_mem_copy(pMac->lim.gLimAlternateRadio.bssId,
                         pAssocRsp->propIEinfo.alternateRadio.bssId, sizeof(tSirMacAddr));
            pMac->lim.gLimAlternateRadio.channelId =
                          pAssocRsp->propIEinfo.alternateRadio.channelId;
        }else
            mlmAssocCnf.resultCode = eSIR_SME_ASSOC_REFUSED;

        //                                               
        if (limSearchPreAuthList(pMac, pHdr->sa))
            limDeletePreAuthNode(pMac, pHdr->sa);

        goto assocReject;
    }
    else if ((pAssocRsp->aid & 0x3FFF) > 2007)
    {
        //                                     
        //                       
        //          
        PELOGW(limLog(pMac, LOGW, FL("received Re/AssocRsp frame with invalid aid %X "),  pAssocRsp->aid);)
        mlmAssocCnf.resultCode = eSIR_SME_INVALID_ASSOC_RSP_RXED;
        mlmAssocCnf.protStatusCode = eSIR_MAC_UNSPEC_FAILURE_STATUS;

        //                                         
        limSendDisassocMgmtFrame(pMac, eSIR_MAC_UNSPEC_FAILURE_REASON,
                                 pHdr->sa, psessionEntry, FALSE);

        goto assocReject;
    }
    //                                                
    /*
                                                                
                             
                                                                         
     */
    if (!((psessionEntry->bssType == eSIR_BTAMP_STA_MODE) ||
          ((psessionEntry->bssType == eSIR_BTAMP_AP_MODE) &&
          (psessionEntry->limSystemRole == eLIM_BT_AMP_STA_ROLE))))
    {
            if (limSetLinkState(pMac, eSIR_LINK_POSTASSOC_STATE, psessionEntry->bssId,
                                psessionEntry->selfMacAddr, NULL, NULL) != eSIR_SUCCESS)
            {
                PELOGE(limLog(pMac, LOGE, FL("Set link state to POSTASSOC failed"));)
                vos_mem_free(pBeaconStruct);
                vos_mem_free(pAssocRsp);
                return;
            }
    }
    if (subType == LIM_REASSOC)
    {
        //            
        PELOG1(limLog(pMac, LOG1, FL("Successfully Reassociated with BSS"));)
#ifdef FEATURE_WLAN_CCX
        {
            tANI_U8 cnt = 0;
            if (pAssocRsp->tsmPresent)
            {
                limLog(pMac, LOGW, "TSM IE Present in Reassoc Rsp");
                //                                                                        
                if (pAssocRsp->tspecPresent) {
                    //                                        
                    for (cnt=0; cnt<pAssocRsp->num_tspecs; cnt++) {
                        if ( upToAc(pAssocRsp->TSPECInfo[cnt].user_priority) == EDCA_AC_VO) {
                            psessionEntry->ccxContext.tsm.tid = pAssocRsp->TSPECInfo[cnt].user_priority;
                            vos_mem_copy(&psessionEntry->ccxContext.tsm.tsmInfo,
                                    &pAssocRsp->tsmIE, sizeof(tSirMacCCXTSMIE));
#ifdef FEATURE_WLAN_CCX_UPLOAD
                            limSendSmeTsmIEInd(pMac,
                                               psessionEntry,
                                               pAssocRsp->tsmIE.tsid,
                                               pAssocRsp->tsmIE.state,
                                               pAssocRsp->tsmIE.msmt_interval);
#else
                            limActivateTSMStatsTimer(pMac, psessionEntry);
#endif /*                         */
                            if(psessionEntry->ccxContext.tsm.tsmInfo.state) {
                                psessionEntry->ccxContext.tsm.tsmMetrics.RoamingCount++;
                            }
                            break;
                        }
                    }
                } else {
                    limLog(pMac, LOGE, "TSM present but TSPEC IE not present in Reassoc Rsp");
                }
            }
        }
#endif
        if (psessionEntry->pLimMlmJoinReq)
        {
            vos_mem_free(psessionEntry->pLimMlmJoinReq);
            psessionEntry->pLimMlmJoinReq = NULL;
        }

        psessionEntry->limAssocResponseData  = (void *) pAssocRsp; /*                                                    
                                                                                                               */
        pStaDs = dphGetHashEntry(pMac, DPH_STA_HASH_INDEX_PEER, &psessionEntry->dph.dphHashTable);

        if(!pStaDs)
        {
            PELOGE(limLog(pMac, LOGE, FL("could not get hash entry at DPH for"));)
            limPrintMacAddr(pMac, pHdr->sa, LOGE);
            mlmAssocCnf.resultCode = eSIR_SME_INVALID_ASSOC_RSP_RXED;
            mlmAssocCnf.protStatusCode = eSIR_MAC_UNSPEC_FAILURE_STATUS;
            
            //                                         
            limSendDisassocMgmtFrame(pMac, eSIR_MAC_UNSPEC_FAILURE_REASON,
                                     pHdr->sa, psessionEntry, FALSE);
            
            goto assocReject;
        }

#if defined(WLAN_FEATURE_VOWIFI_11R) || defined (FEATURE_WLAN_CCX) || defined(FEATURE_WLAN_LFR)
        if (psessionEntry->limMlmState == eLIM_MLM_WT_FT_REASSOC_RSP_STATE)
        {
#ifdef WLAN_FEATURE_VOWIFI_11R_DEBUG
            PELOGE(limLog(pMac, LOG1, FL("Sending self sta"));)
#endif
            pmmResetPmmState(pMac);

            limUpdateAssocStaDatas(pMac, pStaDs, pAssocRsp,psessionEntry);

            //                                      
            psessionEntry->limAID = pAssocRsp->aid & 0x3FFF;

            //                                        
            limSetActiveEdcaParams(pMac, psessionEntry->gLimEdcaParams, psessionEntry);

            //                                       
            if (pStaDs->aniPeer == eANI_BOOLEAN_TRUE)
            {
                limSendEdcaParams(pMac, psessionEntry->gLimEdcaParamsActive, pStaDs->bssId, eANI_BOOLEAN_TRUE);
            }
            else
            {
                limSendEdcaParams(pMac, psessionEntry->gLimEdcaParamsActive, pStaDs->bssId, eANI_BOOLEAN_FALSE);
            }
            limAddFTStaSelf(pMac, (pAssocRsp->aid & 0x3FFF), psessionEntry);
            vos_mem_free(pBeaconStruct);

            return;
        }
#endif /*                         */

        /*                                                                        
                                                                                 
                                                                                  
                                                                                
                                                          
         */
        if (sirCompareMacAddr( psessionEntry->bssId, psessionEntry->limReAssocbssId))
            limHandleAddBssInReAssocContext(pMac, pStaDs, psessionEntry);
        else
        {
            //                                                                   
            pMac->lim.gUapsdPerAcDeliveryEnableMask = 0;
            pMac->lim.gUapsdPerAcTriggerEnableMask = 0;

            if (limCleanupRxPath(pMac, pStaDs,psessionEntry) != eSIR_SUCCESS)
                goto assocReject;
        }
        vos_mem_free(pBeaconStruct);

        return;
    }

    //            
    PELOG1(limLog(pMac, LOG1, FL("Successfully Associated with BSS "MAC_ADDRESS_STR),
           MAC_ADDR_ARRAY(pHdr->sa));)
#ifdef FEATURE_WLAN_CCX
    if(psessionEntry->ccxContext.tsm.tsmInfo.state)
    {
        psessionEntry->ccxContext.tsm.tsmMetrics.RoamingCount = 0;
    }
#endif
    /* 
                                       
     */
    pmmResetPmmState(pMac);

    //                                      
    psessionEntry->limAID = pAssocRsp->aid & 0x3FFF;


    //                                             
    if ((pStaDs = dphGetHashEntry(pMac, DPH_STA_HASH_INDEX_PEER, &psessionEntry->dph.dphHashTable)) == NULL) 
    {
        //                               
        PELOGE(limLog(pMac, LOGE, FL("could not get hash entry at DPH for "));)
        limPrintMacAddr(pMac, pHdr->sa, LOGE);

        mlmAssocCnf.resultCode = eSIR_SME_RESOURCES_UNAVAILABLE;
        mlmAssocCnf.protStatusCode = eSIR_SME_SUCCESS;
        

        limPostSmeMessage(pMac, LIM_MLM_ASSOC_CNF,
                              (tANI_U32 *) &mlmAssocCnf);
        vos_mem_free(pAssocRsp);
        vos_mem_free(pBeaconStruct);

        return;
    }
   
     //                                               
    if (limSearchPreAuthList(pMac, pHdr->sa))
        limDeletePreAuthNode(pMac, pHdr->sa);

    limUpdateAssocStaDatas(pMac, pStaDs, pAssocRsp,psessionEntry);
    //                                                                      
    //                                        
    limExtractApCapabilities( pMac,
                            (tANI_U8 *) psessionEntry->pLimJoinReq->bssDescription.ieFields,
                            limGetIElenFromBssDescription( &psessionEntry->pLimJoinReq->bssDescription ),
                            pBeaconStruct );

    if(pMac->lim.gLimProtectionControl != WNI_CFG_FORCE_POLICY_PROTECTION_DISABLE)
        limDecideStaProtectionOnAssoc(pMac, pBeaconStruct, psessionEntry);
    
    if(pBeaconStruct->erpPresent) {
        if (pBeaconStruct->erpIEInfo.barkerPreambleMode)
            psessionEntry->beaconParams.fShortPreamble = false;
        else
            psessionEntry->beaconParams.fShortPreamble = true;
    }
#ifdef FEATURE_WLAN_DIAG_SUPPORT_LIM //                         
    limDiagEventReport(pMac, WLAN_PE_DIAG_CONNECTED, psessionEntry, 0, 0);
#endif


     //                                                           
    if( eSIR_SUCCESS == limStaSendAddBss( pMac, pAssocRsp,  pBeaconStruct,
                   &psessionEntry->pLimJoinReq->bssDescription, true, psessionEntry))  
    {
        vos_mem_free(pAssocRsp);
        vos_mem_free(pBeaconStruct);
        return;
    }
    else
    {
        mlmAssocCnf.resultCode = eSIR_SME_RESOURCES_UNAVAILABLE;
        mlmAssocCnf.protStatusCode = eSIR_MAC_UNSPEC_FAILURE_STATUS;
    }

  

assocReject:
    if ((subType == LIM_ASSOC)
#ifdef WLAN_FEATURE_VOWIFI_11R
                    || ((subType == LIM_REASSOC) && (psessionEntry->limMlmState == eLIM_MLM_WT_FT_REASSOC_RSP_STATE))
#endif
       ) {
        PELOGE(limLog(pMac, LOGE,  FL("Assoc Rejected by the peer. Reason: %d"), mlmAssocCnf.resultCode);)
        psessionEntry->limMlmState = eLIM_MLM_IDLE_STATE;
        MTRACE(macTrace(pMac, TRACE_CODE_MLM_STATE, psessionEntry->peSessionId, psessionEntry->limMlmState));

        if (psessionEntry->pLimMlmJoinReq)
        {
            vos_mem_free(psessionEntry->pLimMlmJoinReq);
            psessionEntry->pLimMlmJoinReq = NULL;
        }

        if (subType == LIM_ASSOC)
        {
           limPostSmeMessage(pMac, LIM_MLM_ASSOC_CNF, (tANI_U32 *) &mlmAssocCnf);
        }
#ifdef WLAN_FEATURE_VOWIFI_11R
        else
        {
                mlmAssocCnf.resultCode = eSIR_SME_FT_REASSOC_FAILURE;
                limPostSmeMessage(pMac, LIM_MLM_REASSOC_CNF, (tANI_U32 *) &mlmAssocCnf);
        }
#endif /*                         */
    } else {
        limRestorePreReassocState( pMac, 
                  eSIR_SME_REASSOC_REFUSED, mlmAssocCnf.protStatusCode,psessionEntry); 
    }

    /*                                                                  */
    /*                                                                             */
    WLANTL_AssocFailed (psessionEntry->staId);

    vos_mem_free(pBeaconStruct);
    vos_mem_free(pAssocRsp);
    return;
} /*                                   */


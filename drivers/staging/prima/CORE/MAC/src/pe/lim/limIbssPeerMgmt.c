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
#include "palTypes.h"
#include "aniGlobal.h"
#include "sirCommon.h"
#include "wniCfgSta.h"
#include "limUtils.h"
#include "limAssocUtils.h"
#include "limStaHashApi.h"
#include "schApi.h"          //                                          
#include "limSecurityUtils.h"
#include "limSendMessages.h"
#include "limSession.h"
#include "limIbssPeerMgmt.h"


/* 
                 
  
           
                                                    
                                    
                                                      
                                                           
  
        
  
              
  
       
  
                                            
  
                                                   
 */

static tLimIbssPeerNode *
ibss_peer_find(
    tpAniSirGlobal  pMac,
    tSirMacAddr     macAddr)
{
    tLimIbssPeerNode *pTempNode = pMac->lim.gLimIbssPeerList;

    while (pTempNode != NULL)
    {
        if (vos_mem_compare((tANI_U8 *) macAddr,
                            (tANI_U8 *) &pTempNode->peerMacAddr,
                            sizeof(tSirMacAddr)))
            break;
        pTempNode = pTempNode->next;
    }
    return pTempNode;
} /*                          */

/* 
                
  
           
                                                         
                              
  
        
                                                
  
              
  
       
  
                                                      
                                                                    
  
               
 */

static tSirRetStatus
ibss_peer_add(tpAniSirGlobal pMac, tLimIbssPeerNode *pPeerNode)
{
#ifdef ANI_SIR_IBSS_PEER_CACHING
    tANI_U32 numIbssPeers = (2 * pMac->lim.maxStation);

    if (pMac->lim.gLimNumIbssPeers >= numIbssPeers)
    {
        /* 
                                                        
                                                              
         */
        tLimIbssPeerNode *pTemp, *pPrev;
        pTemp = pPrev = pMac->lim.gLimIbssPeerList;
        while (pTemp->next != NULL)
        {
            pPrev = pTemp;
            pTemp = pTemp->next;
        }
        if(pTemp->beacon)
        {
            vos_mem_free(pTemp->beacon);
        }

        vos_mem_free(pTemp);
        pPrev->next = NULL;
    }
    else
#endif
        pMac->lim.gLimNumIbssPeers++;

    pPeerNode->next = pMac->lim.gLimIbssPeerList;
    pMac->lim.gLimIbssPeerList = pPeerNode;

    return eSIR_SUCCESS;

} /*                                */

/* 
                    
  
           
                                                  
                                                     
  
        
  
              
  
       
  
                                                    
                                                  
                                           
                                              
  
               
 */

static void
ibss_peer_collect(
    tpAniSirGlobal      pMac,
    tpSchBeaconStruct   pBeacon,
    tpSirMacMgmtHdr     pHdr,
    tLimIbssPeerNode    *pPeer,
    tpPESession         psessionEntry)
{
    vos_mem_copy(pPeer->peerMacAddr, pHdr->sa, sizeof(tSirMacAddr));

    pPeer->capabilityInfo       = pBeacon->capabilityInfo;
    pPeer->extendedRatesPresent = pBeacon->extendedRatesPresent;
    pPeer->edcaPresent          = pBeacon->edcaPresent;
    pPeer->wmeEdcaPresent       = pBeacon->wmeEdcaPresent;
    pPeer->wmeInfoPresent       = pBeacon->wmeInfoPresent;

    if(IS_DOT11_MODE_HT(psessionEntry->dot11mode) &&
        (pBeacon->HTCaps.present))
    {
        pPeer->htCapable =  pBeacon->HTCaps.present;
        vos_mem_copy((tANI_U8 *)pPeer->supportedMCSSet,
                     (tANI_U8 *)pBeacon->HTCaps.supportedMCSSet,
                     sizeof(pPeer->supportedMCSSet));
        pPeer->htGreenfield = (tANI_U8)pBeacon->HTCaps.greenField;
        pPeer->htSupportedChannelWidthSet = ( tANI_U8 ) pBeacon->HTCaps.supportedChannelWidthSet;
        pPeer->htMIMOPSState =  (tSirMacHTMIMOPowerSaveState)pBeacon->HTCaps.mimoPowerSave;
        pPeer->htMaxAmsduLength = ( tANI_U8 ) pBeacon->HTCaps.maximalAMSDUsize;
        pPeer->htAMpduDensity =             pBeacon->HTCaps.mpduDensity;
        pPeer->htDsssCckRate40MHzSupport = (tANI_U8)pBeacon->HTCaps.dsssCckMode40MHz;
        pPeer->htShortGI20Mhz = (tANI_U8)pBeacon->HTCaps.shortGI20MHz;
        pPeer->htShortGI40Mhz = (tANI_U8)pBeacon->HTCaps.shortGI40MHz;
        pPeer->htMaxRxAMpduFactor = pBeacon->HTCaps.maxRxAMPDUFactor;
        pPeer->htSecondaryChannelOffset = pBeacon->HTInfo.secondaryChannelOffset;
    }

    /*                                                                          */
#ifdef WLAN_FEATURE_11AC
    if ( pBeacon->VHTCaps.present )
    {
        pPeer->vhtSupportedChannelWidthSet = pBeacon->VHTOperation.chanWidth;
        pPeer->vhtCapable = pBeacon->VHTCaps.present;

        //                                     
        vos_mem_copy((tANI_U8 *) &pPeer->VHTCaps,
                     (tANI_U8 *) &pBeacon->VHTCaps,
                     sizeof(tDot11fIEVHTCaps));
    }
#endif
    pPeer->erpIePresent = pBeacon->erpPresent;

    vos_mem_copy((tANI_U8 *) &pPeer->supportedRates,
                 (tANI_U8 *) &pBeacon->supportedRates,
                 pBeacon->supportedRates.numRates + 1);
    if (pPeer->extendedRatesPresent)
        vos_mem_copy((tANI_U8 *) &pPeer->extendedRates,
                     (tANI_U8 *) &pBeacon->extendedRates,
                     pBeacon->extendedRates.numRates + 1);
    else
        pPeer->extendedRates.numRates = 0;

    //                         
    //                   

    pPeer->next = NULL;
} /*                             */

//                                           
static void
ibss_sta_caps_update(
    tpAniSirGlobal    pMac,
    tLimIbssPeerNode *pPeerNode,
    tpPESession       psessionEntry)
{
    tANI_U16      peerIdx;
    tpDphHashNode pStaDs;

    pPeerNode->beaconHBCount++; //                    

    //                                                     
    if ((pStaDs = dphLookupHashEntry(pMac, pPeerNode->peerMacAddr, &peerIdx, &psessionEntry->dph.dphHashTable)) == NULL)
        return;


    //                      
    if(IS_DOT11_MODE_HT(psessionEntry->dot11mode))
    {
        pStaDs->mlmStaContext.htCapability = pPeerNode->htCapable;
        if (pPeerNode->htCapable)
        {
            pStaDs->htGreenfield = pPeerNode->htGreenfield;
            pStaDs->htSupportedChannelWidthSet =  pPeerNode->htSupportedChannelWidthSet;
            pStaDs->htMIMOPSState =             pPeerNode->htMIMOPSState;
            pStaDs->htMaxAmsduLength =  pPeerNode->htMaxAmsduLength;
            pStaDs->htAMpduDensity =             pPeerNode->htAMpduDensity;
            pStaDs->htDsssCckRate40MHzSupport = pPeerNode->htDsssCckRate40MHzSupport;
            pStaDs->htShortGI20Mhz = pPeerNode->htShortGI20Mhz;
            pStaDs->htShortGI40Mhz = pPeerNode->htShortGI40Mhz;
            pStaDs->htMaxRxAMpduFactor = pPeerNode->htMaxRxAMpduFactor;
            //                                                 
            //                                              
            pStaDs->baPolicyFlag = 0xFF;
        }
    }
#ifdef WLAN_FEATURE_11AC
    if ( IS_DOT11_MODE_VHT(psessionEntry->dot11mode) )
    {
        pStaDs->mlmStaContext.vhtCapability = pPeerNode->vhtCapable;
        if ( pPeerNode->vhtCapable )
        {
           pStaDs->vhtSupportedChannelWidthSet = pPeerNode->vhtSupportedChannelWidthSet;
        }
    }
#endif

    if(IS_DOT11_MODE_PROPRIETARY(psessionEntry->dot11mode) &&
      pPeerNode->aniIndicator)
    {
        pStaDs->aniPeer = pPeerNode->aniIndicator;
        pStaDs->propCapability = pPeerNode->propCapability;
    }


    //                                               
    //                                                                    
    //                                                                       
    //                                                                    
    if (pPeerNode->capabilityInfo.qos && (psessionEntry->limQosEnabled) && pPeerNode->edcaPresent)
    {
        pStaDs->qosMode    = 1;
        pStaDs->wmeEnabled = 0;
        if (! pStaDs->lleEnabled)
        {
            pStaDs->lleEnabled = 1;
            //                        
        }
        return;
    }
    //                                                        
    else if (pStaDs->lleEnabled)
    {
        pStaDs->qosMode      = 0;
        pStaDs->lleEnabled   = 0;
    }

    //                                               
    if (pPeerNode->wmeInfoPresent &&  psessionEntry->limWmeEnabled)
    {
        pStaDs->qosMode    = 1;
        pStaDs->lleEnabled = 0;
        if (! pStaDs->wmeEnabled)
        {
            pStaDs->wmeEnabled = 1;
            //                        
        }
        return;
    }
    /*                                                                 
                                                                          
                                                                            
    */
    if (pPeerNode->wmeEdcaPresent && psessionEntry->limWmeEnabled) {
        pStaDs->qosMode    = 1;
        pStaDs->lleEnabled = 0;
        if (! pStaDs->wmeEnabled) {
            pStaDs->wmeEnabled = 1;
        }
        return;
    }

    //                                                        
    else if (pStaDs->wmeEnabled)
    {
        pStaDs->qosMode      = 0;
        pStaDs->wmeEnabled   = 0;
    }

}

static void
ibss_sta_rates_update(
    tpAniSirGlobal   pMac,
    tpDphHashNode    pStaDs,
    tLimIbssPeerNode *pPeer,
    tpPESession       psessionEntry)
{
#ifdef WLAN_FEATURE_11AC
    limPopulateMatchingRateSet(pMac, pStaDs, &pPeer->supportedRates,
                               &pPeer->extendedRates, pPeer->supportedMCSSet,
                               &pStaDs->mlmStaContext.propRateSet,psessionEntry, &pPeer->VHTCaps);
#else
    //                           
    limPopulateMatchingRateSet(pMac, pStaDs, &pPeer->supportedRates,
                               &pPeer->extendedRates, pPeer->supportedMCSSet,
                               &pStaDs->mlmStaContext.propRateSet,psessionEntry);
#endif

    pStaDs->mlmStaContext.capabilityInfo = pPeer->capabilityInfo;
} /*                                */

/* 
                       
  
           
                                                      
                    
  
        
  
              
  
       
  
                                                   
                                       
                                             
  
               
 */

static void
ibss_sta_info_update(
    tpAniSirGlobal   pMac,
    tpDphHashNode    pStaDs,
    tLimIbssPeerNode *pPeer,
    tpPESession psessionEntry)
{
    pStaDs->staType = STA_ENTRY_PEER;
    ibss_sta_caps_update(pMac, pPeer,psessionEntry);
    ibss_sta_rates_update(pMac, pStaDs, pPeer,psessionEntry);
} /*                                */

static void
ibss_coalesce_free(
    tpAniSirGlobal pMac)
{
    if (pMac->lim.ibssInfo.pHdr != NULL)
        vos_mem_free(pMac->lim.ibssInfo.pHdr);
    if (pMac->lim.ibssInfo.pBeacon != NULL)
        vos_mem_free(pMac->lim.ibssInfo.pBeacon);

    pMac->lim.ibssInfo.pHdr    = NULL;
    pMac->lim.ibssInfo.pBeacon = NULL;
}

/*
                                                     
 */
static void
ibss_coalesce_save(
    tpAniSirGlobal      pMac,
    tpSirMacMgmtHdr     pHdr,
    tpSchBeaconStruct   pBeacon)
{
    //                          
    ibss_coalesce_free(pMac);

    pMac->lim.ibssInfo.pHdr = vos_mem_malloc(sizeof(*pHdr));
    if (NULL == pMac->lim.ibssInfo.pHdr)
    {
        PELOGE(limLog(pMac, LOGE, FL("ibbs-save: Failed malloc pHdr"));)
        return;
    }
    pMac->lim.ibssInfo.pBeacon = vos_mem_malloc(sizeof(*pBeacon));
    if (NULL == pMac->lim.ibssInfo.pBeacon)
    {
        PELOGE(limLog(pMac, LOGE, FL("ibbs-save: Failed malloc pBeacon"));)
        ibss_coalesce_free(pMac);
        return;
    }

    vos_mem_copy(pMac->lim.ibssInfo.pHdr, pHdr, sizeof(*pHdr));
    vos_mem_copy(pMac->lim.ibssInfo.pBeacon, pBeacon, sizeof(*pBeacon));
}

/*
                                            
                                                
 */
static tSirRetStatus
ibss_dph_entry_add(
    tpAniSirGlobal  pMac,
    tSirMacAddr     peerAddr,
    tpDphHashNode   *ppSta,
    tpPESession     psessionEntry)
{
    tANI_U16      peerIdx;
    tpDphHashNode pStaDs;

    *ppSta = NULL;

    pStaDs = dphLookupHashEntry(pMac, peerAddr, &peerIdx, &psessionEntry->dph.dphHashTable);
    if (pStaDs != NULL)
    {
        /*                                                        */
        PELOGE(limLog(pMac, LOGE, FL("STA exists already "));)
        limPrintMacAddr(pMac, peerAddr, LOGE);
        return eSIR_FAILURE;
    }

    /* 
                                                       
                                                         
                     
     */
    peerIdx = limAssignPeerIdx(pMac, psessionEntry);

    pStaDs = dphGetHashEntry(pMac, peerIdx, &psessionEntry->dph.dphHashTable);
    if (pStaDs)
    {
        (void) limDelSta(pMac, pStaDs, false /*            */,psessionEntry);
        limDeleteDphHashEntry(pMac, pStaDs->staAddr, peerIdx,psessionEntry);
    }

    pStaDs = dphAddHashEntry(pMac, peerAddr, peerIdx, &psessionEntry->dph.dphHashTable);
    if (pStaDs == NULL)
    {
        //                               
        PELOGE(limLog(pMac, LOGE, FL("could not add hash entry at DPH for peerIdx/aid=%d MACaddr:"), peerIdx);)
        limPrintMacAddr(pMac, peerAddr, LOGE);
        return eSIR_FAILURE;
    }

    *ppSta = pStaDs;
    return eSIR_SUCCESS;
}

//                                  
static void
ibss_status_chg_notify(
    tpAniSirGlobal          pMac,
    tSirMacAddr             peerAddr,
    tANI_U16                staIndex,
    tANI_U8                 ucastSig, 
    tANI_U8                 bcastSig, 
    tANI_U16                status,
    tANI_U8                 sessionId)
{

    tLimIbssPeerNode *peerNode;
    tANI_U8 *beacon = NULL;
    tANI_U16 bcnLen = 0;


    peerNode = ibss_peer_find(pMac,peerAddr);
    if(peerNode != NULL)
    {
        if(peerNode->beacon == NULL) peerNode->beaconLen = 0;
        beacon = peerNode->beacon;
        bcnLen = peerNode->beaconLen;
        peerNode->beacon = NULL;
        peerNode->beaconLen = 0;
    }

    limSendSmeIBSSPeerInd(pMac,peerAddr, staIndex, ucastSig, bcastSig,
                          beacon, bcnLen, status, sessionId);

    if(beacon != NULL)
    {
        vos_mem_free(beacon);
    }
}


static void
ibss_bss_add(
    tpAniSirGlobal    pMac,
    tpPESession       psessionEntry)
{
    tLimMlmStartReq   mlmStartReq;
    tANI_U32          cfg;
    tpSirMacMgmtHdr   pHdr    = (tpSirMacMgmtHdr)   pMac->lim.ibssInfo.pHdr;
    tpSchBeaconStruct pBeacon = (tpSchBeaconStruct) pMac->lim.ibssInfo.pBeacon;
    tANI_U8 numExtRates = 0;

    if ((pHdr == NULL) || (pBeacon == NULL))
    {
        PELOGE(limLog(pMac, LOGE, FL("Unable to add BSS (no cached BSS info)"));)
        return;
    }

    vos_mem_copy(psessionEntry->bssId, pHdr->bssId,
                 sizeof(tSirMacAddr));

    #if 0
    if (cfgSetStr(pMac, WNI_CFG_BSSID, (tANI_U8 *) pHdr->bssId, sizeof(tSirMacAddr))
        != eSIR_SUCCESS)
        limLog(pMac, LOGP, FL("could not update BSSID at CFG"));
    #endif //                 

    sirCopyMacAddr(pHdr->bssId,psessionEntry->bssId);
    /*                                                                         */
    //                               

#if 0
    if (wlan_cfgGetInt(pMac, WNI_CFG_BEACON_INTERVAL, &cfg) != eSIR_SUCCESS)
        limLog(pMac, LOGP, FL("Can't read beacon interval"));
#endif //                 
    /*                                        */
    cfg = psessionEntry->beaconParams.beaconInterval;
    if (cfg != pBeacon->beaconInterval)
        #if 0
        if (cfgSetInt(pMac, WNI_CFG_BEACON_INTERVAL, pBeacon->beaconInterval)
            != eSIR_SUCCESS)
            limLog(pMac, LOGP, FL("Can't update beacon interval"));
        #endif//                 
        psessionEntry->beaconParams.beaconInterval = pBeacon->beaconInterval;

    /*                                                                                                         
                                                                                                                     
                                                                                                                        
                                             
     */
    /*                                                                                 */
    if (psessionEntry->shortSlotTimeSupported != pBeacon->capabilityInfo.shortSlotTime)
    {
        psessionEntry->shortSlotTimeSupported = pBeacon->capabilityInfo.shortSlotTime;
    }
    vos_mem_copy((tANI_U8 *) &psessionEntry->pLimStartBssReq->operationalRateSet,
                 (tANI_U8 *) &pBeacon->supportedRates,
                  pBeacon->supportedRates.numRates);

    #if 0
    if (cfgSetStr(pMac, WNI_CFG_OPERATIONAL_RATE_SET,
           (tANI_U8 *) &pMac->lim.gpLimStartBssReq->operationalRateSet.rate,
           pMac->lim.gpLimStartBssReq->operationalRateSet.numRates)
        != eSIR_SUCCESS)
        limLog(pMac, LOGP, FL("could not update OperRateset at CFG"));
    #endif //                 

    /* 
                                                                       
                                                                                       
                                                                                              
                                                                                                    
                                                                                                                
                                                                                                                 
                                                                                                     
    */

    if(pBeacon->extendedRatesPresent)
        numExtRates = pBeacon->extendedRates.numRates;
    if (cfgSetStr(pMac, WNI_CFG_EXTENDED_OPERATIONAL_RATE_SET,
           (tANI_U8 *) &pBeacon->extendedRates.rate, numExtRates) != eSIR_SUCCESS)
    {
            limLog(pMac, LOGP, FL("could not update ExtendedOperRateset at CFG"));
        return;
    } 


    /*
                                                                                                          
                                                                                                             
                                                                                                                 
                                                        
                                                                                                                    
    */
    vos_mem_set((void *) &mlmStartReq, sizeof(mlmStartReq), 0);

    vos_mem_copy(mlmStartReq.bssId, pHdr->bssId, sizeof(tSirMacAddr));
    mlmStartReq.rateSet.numRates = psessionEntry->pLimStartBssReq->operationalRateSet.numRates;
    vos_mem_copy(&mlmStartReq.rateSet.rate[0],
                 &psessionEntry->pLimStartBssReq->operationalRateSet.rate[0],
                 mlmStartReq.rateSet.numRates);
    mlmStartReq.bssType             = eSIR_IBSS_MODE;
    mlmStartReq.beaconPeriod        = pBeacon->beaconInterval;
    mlmStartReq.nwType              = psessionEntry->pLimStartBssReq->nwType; //                                    
    mlmStartReq.htCapable           = psessionEntry->htCapability;
    mlmStartReq.htOperMode          = pMac->lim.gHTOperMode;
    mlmStartReq.dualCTSProtection   = pMac->lim.gHTDualCTSProtection;
    mlmStartReq.txChannelWidthSet   = psessionEntry->htRecommendedTxWidthSet;

    #if 0
    if (wlan_cfgGetInt(pMac, WNI_CFG_CURRENT_CHANNEL, &cfg) != eSIR_SUCCESS)
        limLog(pMac, LOGP, FL("CurrentChannel CFG get fialed!"));
    #endif

    //                                                       

    /*                                            */
    mlmStartReq.channelNumber = psessionEntry->currentOperChannel;

    mlmStartReq.cbMode              = psessionEntry->pLimStartBssReq->cbMode;

    //                                               
    vos_mem_copy((tANI_U8 *) &mlmStartReq.ssId,
                 (tANI_U8 *) &psessionEntry->pLimStartBssReq->ssId,
                  psessionEntry->pLimStartBssReq->ssId.length + 1);

    PELOG1(limLog(pMac, LOG1, FL("invoking ADD_BSS as part of coalescing!"));)
    if (limMlmAddBss(pMac, &mlmStartReq,psessionEntry) != eSIR_SME_SUCCESS)
    {
        PELOGE(limLog(pMac, LOGE, FL("AddBss failure"));)
        return;
    }

    //                        
    if (schSetFixedBeaconFields(pMac,psessionEntry) != eSIR_SUCCESS)
    {
        PELOGE(limLog(pMac, LOGE, FL("*** Unable to set fixed Beacon fields ***"));)
        return;
    }

}



/*                        */
static void
ibss_bss_delete(
    tpAniSirGlobal pMac,
    tpPESession    psessionEntry)
{
    tSirRetStatus status;
    PELOGW(limLog(pMac, LOGW, FL("Initiating IBSS Delete BSS"));)
    if (psessionEntry->limMlmState != eLIM_MLM_BSS_STARTED_STATE)
    {
        limLog(pMac, LOGW, FL("Incorrect LIM MLM state for delBss (%d)"),
               psessionEntry->limMlmState);
        return;
    }
    status = limDelBss(pMac, NULL, psessionEntry->bssIdx, psessionEntry);
    if (status != eSIR_SUCCESS)
        PELOGE(limLog(pMac, LOGE, FL("delBss failed for bss %d"), psessionEntry->bssIdx);)
}

/* 
              
  
           
                                                 
                                                  
  
        
  
              
  
       
  
                                                 
               
 */

void
limIbssInit(
    tpAniSirGlobal pMac)
{
    //                             
    pMac->lim.gLimIbssCoalescingHappened = 0;
    pMac->lim.gLimIbssPeerList = NULL;
    pMac->lim.gLimNumIbssPeers = 0;

    //                                                           
    vos_mem_set(&pMac->lim.ibssInfo, sizeof(tAniSirLimIbss), 0);
} /*                       */

/* 
                        
  
           
                                               
  
        
  
              
  
       
  
                                                 
               
 */

void limIbssDeleteAllPeers( tpAniSirGlobal pMac ,tpPESession psessionEntry)
{
    tLimIbssPeerNode    *pCurrNode, *pTempNode;
    tpDphHashNode pStaDs;
    tANI_U16 peerIdx;

    pCurrNode = pTempNode = pMac->lim.gLimIbssPeerList;

    while (pCurrNode != NULL)
    {
        if (!pMac->lim.gLimNumIbssPeers)
        {
            limLog(pMac, LOGP,
               FL("Number of peers in the list is zero and node present"));
            return;
        }
        /*                                     
                                                                             
                                                                                             
              */
        pStaDs = dphLookupHashEntry(pMac, pCurrNode->peerMacAddr, &peerIdx, &psessionEntry->dph.dphHashTable);
        if( pStaDs )
        {

            ibss_status_chg_notify( pMac, pCurrNode->peerMacAddr, pStaDs->staIndex, 
                                    pStaDs->ucUcastSig, pStaDs->ucBcastSig,
                                    eWNI_SME_IBSS_PEER_DEPARTED_IND, psessionEntry->smeSessionId );
            limReleasePeerIdx(pMac, peerIdx, psessionEntry);
            dphDeleteHashEntry(pMac, pStaDs->staAddr, peerIdx, &psessionEntry->dph.dphHashTable);
        }

        pTempNode = pCurrNode->next;

        /*                            */
        /*                                                                                      
                                                                                                       
                                                                                                             
                                                                                                            
                                                                                      
                */
        pMac->lim.gLimIbssPeerList = pTempNode;

        if(pCurrNode->beacon)
        {
            vos_mem_free(pCurrNode->beacon);
        }
        vos_mem_free(pCurrNode);
        if (pMac->lim.gLimNumIbssPeers > 0) //            
            pMac->lim.gLimNumIbssPeers--;
        pCurrNode = pTempNode;
    }

    if (pMac->lim.gLimNumIbssPeers)
        limLog(pMac, LOGP, FL("Number of peers[%d] in the list is non-zero"),
                pMac->lim.gLimNumIbssPeers);

    pMac->lim.gLimNumIbssPeers = 0;
    pMac->lim.gLimIbssPeerList = NULL;

}
/* 
                
  
           
                                                      
  
        
  
              
  
       
  
                                                 
               
 */

void
limIbssDelete(
    tpAniSirGlobal pMac,tpPESession psessionEntry)
{
    limIbssDeleteAllPeers(pMac,psessionEntry);

    ibss_coalesce_free(pMac);
} /*                         */

/*                                                            */
#if 0
/* 
                    
  
           
                                                       
                 
  
        
  
              
  
       
  
                                                 
                                                         
                                                           
  
               
 */

void
limIbssPeerDelete(tpAniSirGlobal pMac, tSirMacAddr macAddr)
{
    tLimIbssPeerNode    *pPrevNode, *pTempNode;

    pTempNode = pPrevNode = pMac->lim.gLimIbssPeerList;

    if (pTempNode == NULL)
        return;

    while (pTempNode != NULL)
    {
        if (vos_mem_compare((tANI_U8 *) macAddr,
                            (tANI_U8 *) &pTempNode->peerMacAddr,
                            sizeof(tSirMacAddr)) )
        {
            //                         
            if (pMac->lim.gLimIbssPeerList == pTempNode) /*                          */
                pMac->lim.gLimIbssPeerList = pTempNode->next;
            else
                pPrevNode->next = pTempNode->next;

            if(pTempNode->beacon)
            {
                vos_mem_free(pTempNode->beacon);
                pTempNode->beacon = NULL;
            }
            vos_mem_free(pTempNode);
            pMac->lim.gLimNumIbssPeers--;
            return;
        }

        pPrevNode = pTempNode;
        pTempNode = pTempNode->next;
    }

    //                   
    PELOGE(limLog(pMac, LOGE, FL("peer not found in the list, addr= "));)
    limPrintMacAddr(pMac, macAddr, LOGE);
} /*                             */

#endif


/*                                                               
                        
                                                      
 
                              
                               
                                          
            
                                                               */
static void
limIbssSetProtection(tpAniSirGlobal pMac, tANI_U8 enable, tpUpdateBeaconParams pBeaconParams,  tpPESession psessionEntry)
{

    if(!pMac->lim.cfgProtection.fromllb)
    {
        PELOG1(limLog(pMac, LOG1, FL("protection from 11b is disabled"));)
        return;
    }

    if (enable)
    {
        psessionEntry->gLim11bParams.protectionEnabled = true;
        if(false == psessionEntry->beaconParams.llbCoexist/*                    */)
        {
            PELOGE(limLog(pMac, LOGE, FL("=> IBSS: Enable Protection "));)
            pBeaconParams->llbCoexist = psessionEntry->beaconParams.llbCoexist = true;
            pBeaconParams->paramChangeBitmap |= PARAM_llBCOEXIST_CHANGED;
        }
    }
    else if (true == psessionEntry->beaconParams.llbCoexist/*                    */)
    {
        psessionEntry->gLim11bParams.protectionEnabled = false;
        PELOGE(limLog(pMac, LOGE, FL("===> IBSS: Disable protection "));)
        pBeaconParams->llbCoexist = psessionEntry->beaconParams.llbCoexist = false;
        pBeaconParams->paramChangeBitmap |= PARAM_llBCOEXIST_CHANGED;
    }
    return;
}


/*                                                               
                                 
                                                      
 
                              
                               
                                          
            
                                                               */
static void
limIbssUpdateProtectionParams(tpAniSirGlobal pMac,
        tSirMacAddr peerMacAddr, tLimProtStaCacheType protStaCacheType,
        tpPESession psessionEntry)
{
  tANI_U32 i;

  PELOG1(limLog(pMac,LOG1, FL("A STA is associated:"));
  limLog(pMac,LOG1, FL("Addr : "));
  limPrintMacAddr(pMac, peerMacAddr, LOG1);)

  for (i=0; i<LIM_PROT_STA_CACHE_SIZE; i++)
  {
      if (pMac->lim.protStaCache[i].active)
      {
          PELOG1(limLog(pMac, LOG1, FL("Addr: "));)
          PELOG1(limPrintMacAddr(pMac, pMac->lim.protStaCache[i].addr, LOG1);)

          if (vos_mem_compare(pMac->lim.protStaCache[i].addr,
              peerMacAddr, sizeof(tSirMacAddr)))
          {
              PELOG1(limLog(pMac, LOG1, FL("matching cache entry at %d already active."), i);)
              return;
          }
      }
  }

  for (i=0; i<LIM_PROT_STA_CACHE_SIZE; i++)
  {
      if (!pMac->lim.protStaCache[i].active)
          break;
  }

  if (i >= LIM_PROT_STA_CACHE_SIZE)
  {
      PELOGE(limLog(pMac, LOGE, FL("No space in ProtStaCache"));)
      return;
  }

  vos_mem_copy(pMac->lim.protStaCache[i].addr,
               peerMacAddr,
               sizeof(tSirMacAddr));

  pMac->lim.protStaCache[i].protStaCacheType = protStaCacheType;
  pMac->lim.protStaCache[i].active = true;
  if(eLIM_PROT_STA_CACHE_TYPE_llB == protStaCacheType)
  {
      psessionEntry->gLim11bParams.numSta++;
  }
  else if(eLIM_PROT_STA_CACHE_TYPE_llG == protStaCacheType)
  {
      psessionEntry->gLim11gParams.numSta++;
  }
}


/*                                                               
                           
                                                      
 
                              
                               
                                          
            
                                                               */
static void
limIbssDecideProtection(tpAniSirGlobal pMac, tpDphHashNode pStaDs, tpUpdateBeaconParams pBeaconParams,  tpPESession psessionEntry)
{
    tSirRFBand rfBand = SIR_BAND_UNKNOWN;
    tANI_U32 phyMode;
    tLimProtStaCacheType protStaCacheType = eLIM_PROT_STA_CACHE_TYPE_INVALID;

    pBeaconParams->paramChangeBitmap = 0;

    if(NULL == pStaDs)
    {
      PELOGE(limLog(pMac, LOGE, FL("pStaDs is NULL"));)
      return;
    }

    limGetRfBand(pMac, &rfBand, psessionEntry);
    if(SIR_BAND_2_4_GHZ== rfBand)
    {
        limGetPhyMode(pMac, &phyMode, psessionEntry);

        //                                                                 
        if ((phyMode == WNI_CFG_PHY_MODE_11G) || (psessionEntry->htCapability))
        {
            /*                                                             
                                                                          
                                                                        
             */
            if ((pStaDs->erpEnabled == eHAL_CLEAR) &&
                (!pStaDs->mlmStaContext.htCapability))
            {
                protStaCacheType = eLIM_PROT_STA_CACHE_TYPE_llB;
                PELOGE(limLog(pMac, LOGE, FL("Enable protection from 11B"));)
                limIbssSetProtection(pMac, true, pBeaconParams,psessionEntry);
            }
        }
    }
    limIbssUpdateProtectionParams(pMac, pStaDs->staAddr, protStaCacheType, psessionEntry);
    return;
}


/* 
                  
  
           
                                                             
                                                               
                      
  
        
  
              
     
  
       
     
  
                                                     
                                                         
                                                              
          
 */

tSirRetStatus
limIbssStaAdd(
    tpAniSirGlobal  pMac,
    void *pBody,
    tpPESession psessionEntry)
{
    tSirRetStatus       retCode = eSIR_SUCCESS;
    tpDphHashNode       pStaDs;
    tLimIbssPeerNode    *pPeerNode;
    tLimMlmStates       prevState;
    tSirMacAddr         *pPeerAddr = (tSirMacAddr *) pBody;
    tUpdateBeaconParams beaconParams; 

    vos_mem_set((tANI_U8 *) &beaconParams, sizeof(tUpdateBeaconParams), 0);

    if (pBody == 0)
    {
        PELOGE(limLog(pMac, LOGE, FL("Invalid IBSS AddSta"));)
        return eSIR_FAILURE;
    }

    PELOGE(limLog(pMac, LOGE, FL("Rx Add-Ibss-Sta for MAC:"));)
    limPrintMacAddr(pMac, *pPeerAddr, LOGE);

    pPeerNode = ibss_peer_find(pMac, *pPeerAddr);
    if (NULL != pPeerNode)
    {
        retCode = ibss_dph_entry_add(pMac, *pPeerAddr, &pStaDs, psessionEntry);
        if (eSIR_SUCCESS == retCode)
        {
            prevState = pStaDs->mlmStaContext.mlmState;
            pStaDs->erpEnabled = pPeerNode->erpIePresent;

            ibss_sta_info_update(pMac, pStaDs, pPeerNode, psessionEntry);
            PELOGW(limLog(pMac, LOGW, FL("initiating ADD STA for the IBSS peer."));)
            retCode = limAddSta(pMac, pStaDs, false, psessionEntry);
            if (retCode != eSIR_SUCCESS)
            {
                PELOGE(limLog(pMac, LOGE, FL("ibss-sta-add failed (reason %x)"),
                              retCode);)
                limPrintMacAddr(pMac, *pPeerAddr, LOGE);
                pStaDs->mlmStaContext.mlmState = prevState;
                dphDeleteHashEntry(pMac, pStaDs->staAddr, pStaDs->assocId,
                                   &psessionEntry->dph.dphHashTable);
            }
            else
            {
                if(pMac->lim.gLimProtectionControl != WNI_CFG_FORCE_POLICY_PROTECTION_DISABLE)
                    limIbssDecideProtection(pMac, pStaDs, &beaconParams , psessionEntry);

                if(beaconParams.paramChangeBitmap)
                {
                    PELOGE(limLog(pMac, LOGE, FL("---> Update Beacon Params "));)
                    schSetFixedBeaconFields(pMac, psessionEntry);
                    beaconParams.bssIdx = psessionEntry->bssIdx;
                    limSendBeaconParams(pMac, &beaconParams, psessionEntry );
                }
            }
        }
        else
        {
            PELOGE(limLog(pMac, LOGE, FL("hashTblAdd failed (reason %x)"), retCode);)
            limPrintMacAddr(pMac, *pPeerAddr, LOGE);
        }
    }
    else
    {
        retCode = eSIR_FAILURE;
    }

    return retCode;
}

/*                                                     */
tSirRetStatus
limIbssAddStaRsp(
    tpAniSirGlobal  pMac,
    void *msg,tpPESession psessionEntry)
{
    tpDphHashNode   pStaDs;
    tANI_U16        peerIdx;
    tpAddStaParams  pAddStaParams = (tpAddStaParams) msg;

    SET_LIM_PROCESS_DEFD_MESGS(pMac, true);
    if (pAddStaParams == NULL)
    {
        PELOGE(limLog(pMac, LOGE, FL("IBSS: ADD_STA_RSP with no body!"));)
        return eSIR_FAILURE;
    }

    pStaDs = dphLookupHashEntry(pMac, pAddStaParams->staMac, &peerIdx, &psessionEntry->dph.dphHashTable);
    if (pStaDs == NULL)
    {
        PELOGE(limLog(pMac, LOGE, FL("IBSS: ADD_STA_RSP for unknown MAC addr "));)
        limPrintMacAddr(pMac, pAddStaParams->staMac, LOGE);
        vos_mem_free(pAddStaParams);
        return eSIR_FAILURE;
    }

    if (pAddStaParams->status != eHAL_STATUS_SUCCESS)
    {
        PELOGE(limLog(pMac, LOGE, FL("IBSS: ADD_STA_RSP error (%x) "), pAddStaParams->status);)
        limPrintMacAddr(pMac, pAddStaParams->staMac, LOGE);
        vos_mem_free(pAddStaParams);
        return eSIR_FAILURE;
    }

    pStaDs->bssId                  = pAddStaParams->bssIdx;
    pStaDs->staIndex               = pAddStaParams->staIdx;
    pStaDs->ucUcastSig             = pAddStaParams->ucUcastSig;
    pStaDs->ucBcastSig             = pAddStaParams->ucBcastSig;
    pStaDs->valid                  = 1;
    pStaDs->mlmStaContext.mlmState = eLIM_MLM_LINK_ESTABLISHED_STATE;

    PELOGW(limLog(pMac, LOGW, FL("IBSS: sending IBSS_NEW_PEER msg to SME!"));)

    ibss_status_chg_notify(pMac, pAddStaParams->staMac, pStaDs->staIndex, 
                           pStaDs->ucUcastSig, pStaDs->ucBcastSig,
                           eWNI_SME_IBSS_NEW_PEER_IND,
                           psessionEntry->smeSessionId);
    vos_mem_free(pAddStaParams);

    return eSIR_SUCCESS;
}



void limIbssDelBssRspWhenCoalescing(tpAniSirGlobal  pMac,  void *msg,tpPESession psessionEntry)
{
   tpDeleteBssParams pDelBss = (tpDeleteBssParams) msg;

    PELOGW(limLog(pMac, LOGW, FL("IBSS: DEL_BSS_RSP Rcvd during coalescing!"));)

    if (pDelBss == NULL)
    {
        PELOGE(limLog(pMac, LOGE, FL("IBSS: DEL_BSS_RSP(coalesce) with no body!"));)
        goto end;
    }

    if (pDelBss->status != eHAL_STATUS_SUCCESS)
    {
        limLog(pMac, LOGE, FL("IBSS: DEL_BSS_RSP(coalesce) error (%x) Bss %d "),
               pDelBss->status, pDelBss->bssIdx);
        goto end;
    }
    //                    
    limIbssDeleteAllPeers(pMac,psessionEntry);

    /*                 */
    ibss_bss_add(pMac,psessionEntry);

    end:
    if(pDelBss != NULL)
        vos_mem_free(pDelBss);
}



void limIbssAddBssRspWhenCoalescing(tpAniSirGlobal  pMac, void *msg, tpPESession pSessionEntry)
{
    tANI_U8           infoLen;
    tSirSmeNewBssInfo newBssInfo;

    tpAddBssParams pAddBss = (tpAddBssParams) msg;

    tpSirMacMgmtHdr   pHdr    = (tpSirMacMgmtHdr)   pMac->lim.ibssInfo.pHdr;
    tpSchBeaconStruct pBeacon = (tpSchBeaconStruct) pMac->lim.ibssInfo.pBeacon;

    if ((pHdr == NULL) || (pBeacon == NULL))
    {
        PELOGE(limLog(pMac, LOGE, FL("Unable to handle AddBssRspWhenCoalescing (no cached BSS info)"));)
        goto end;
    }

    //                               
    infoLen = sizeof(tSirMacAddr) + sizeof(tSirMacChanNum) +
              sizeof(tANI_U8) + pBeacon->ssId.length + 1;

    vos_mem_set((void *) &newBssInfo, sizeof(newBssInfo), 0);
    vos_mem_copy(newBssInfo.bssId, pHdr->bssId, sizeof(tSirMacAddr));
    newBssInfo.channelNumber = (tSirMacChanNum) pAddBss->currentOperChannel;
    vos_mem_copy((tANI_U8 *) &newBssInfo.ssId,
                 (tANI_U8 *) &pBeacon->ssId, pBeacon->ssId.length + 1);

    PELOGW(limLog(pMac, LOGW, FL("Sending JOINED_NEW_BSS notification to SME."));)

    limSendSmeWmStatusChangeNtf(pMac, eSIR_SME_JOINED_NEW_BSS,
                                (tANI_U32 *) &newBssInfo,
                                infoLen,pSessionEntry->smeSessionId);
    {
        //                                        
        limSendBeaconInd(pMac, pSessionEntry);
    }

 end:
    ibss_coalesce_free(pMac);
}



void
limIbssDelBssRsp(
    tpAniSirGlobal  pMac,
    void *msg,tpPESession psessionEntry)
{
    tSirResultCodes rc = eSIR_SME_SUCCESS;
    tpDeleteBssParams pDelBss = (tpDeleteBssParams) msg;
    tSirMacAddr             nullBssid = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};


    SET_LIM_PROCESS_DEFD_MESGS(pMac, true);
    if (pDelBss == NULL)
    {
        PELOGE(limLog(pMac, LOGE, FL("IBSS: DEL_BSS_RSP with no body!"));)
        rc = eSIR_SME_REFUSED;
        goto end;
    }

    if((psessionEntry = peFindSessionBySessionId(pMac,pDelBss->sessionId))==NULL)
    {
           limLog(pMac, LOGP,FL("Session Does not exist for given sessionID"));
           goto end;
    }


    /*
                                                                                                    
                                                                                              
                                                                                                         
                                                                                                                
    */
    if(true == pMac->lim.gLimIbssCoalescingHappened)
    {
       
        limIbssDelBssRspWhenCoalescing(pMac,msg,psessionEntry);
        return;
    }



    if (pDelBss->status != eHAL_STATUS_SUCCESS)
    {
        PELOGE(limLog(pMac, LOGE, FL("IBSS: DEL_BSS_RSP error (%x) Bss %d "),
               pDelBss->status, pDelBss->bssIdx);)
        rc = eSIR_SME_STOP_BSS_FAILURE;
        goto end;
    }



    if(limSetLinkState(pMac, eSIR_LINK_IDLE_STATE, nullBssid,  
        psessionEntry->selfMacAddr, NULL, NULL) != eSIR_SUCCESS)
    {
        PELOGE(limLog(pMac, LOGE, FL("IBSS: DEL_BSS_RSP setLinkState failed"));)
        rc = eSIR_SME_REFUSED;
        goto end;
    }

    dphHashTableClassInit(pMac, &psessionEntry->dph.dphHashTable);
    limDeletePreAuthList(pMac);

    limIbssDelete(pMac,psessionEntry);
    psessionEntry->limMlmState = eLIM_MLM_IDLE_STATE;

    MTRACE(macTrace(pMac, TRACE_CODE_MLM_STATE, psessionEntry->peSessionId, psessionEntry->limMlmState));

    psessionEntry->limSystemRole = eLIM_STA_ROLE;

    /*                                                                                                                   
                                                                                                                                  
     */
    psessionEntry->shortSlotTimeSupported = WNI_CFG_SHORT_SLOT_TIME_STADEF;

    end:
    if(pDelBss != NULL)
        vos_mem_free(pDelBss);
    /*                                       */
    if (NULL != psessionEntry) {
        limSendSmeRsp(pMac, eWNI_SME_STOP_BSS_RSP, rc,psessionEntry->smeSessionId,psessionEntry->transactionId);
        peDeleteSession(pMac, psessionEntry);
        psessionEntry = NULL;
    }
}

static void
__limIbssSearchAndDeletePeer(tpAniSirGlobal    pMac,
                             tpPESession psessionEntry,
                             tSirMacAddr macAddr)
{
   tLimIbssPeerNode *pTempNode, *pPrevNode;
   tLimIbssPeerNode *pTempNextNode = NULL;
   tpDphHashNode     pStaDs=NULL;
   tANI_U16          peerIdx=0;
   tANI_U16          staIndex=0;
   tANI_U8           ucUcastSig;
   tANI_U8           ucBcastSig;

   pPrevNode = pTempNode  = pMac->lim.gLimIbssPeerList;

   limLog(pMac, LOG1, FL(" PEER ADDR :" MAC_ADDRESS_STR ),MAC_ADDR_ARRAY(macAddr));

   /*               */
   while (NULL != pTempNode)
   {
      pTempNextNode = pTempNode->next;

      /*                                 */
      if (vos_mem_compare( (tANI_U8 *) macAddr,
               (tANI_U8 *) &pTempNode->peerMacAddr,
               sizeof(tSirMacAddr)) )
      {
         pStaDs = dphLookupHashEntry(pMac, macAddr,
               &peerIdx, &psessionEntry->dph.dphHashTable);
         if (pStaDs)
         {
            staIndex = pStaDs->staIndex;
            ucUcastSig = pStaDs->ucUcastSig;
            ucBcastSig = pStaDs->ucBcastSig;

            (void) limDelSta(pMac, pStaDs, false /*            */, psessionEntry);
            limDeleteDphHashEntry(pMac, pStaDs->staAddr, peerIdx, psessionEntry);
            limReleasePeerIdx(pMac, peerIdx, psessionEntry);

            /*                                 */
            ibss_status_chg_notify(pMac, macAddr, staIndex,
                                   ucUcastSig, ucBcastSig,
                                   eWNI_SME_IBSS_PEER_DEPARTED_IND,
                                   psessionEntry->smeSessionId );
            if (pTempNode == pMac->lim.gLimIbssPeerList)
            {
               pMac->lim.gLimIbssPeerList = pTempNode->next;
               pPrevNode = pMac->lim.gLimIbssPeerList;
            }
            else
               pPrevNode->next = pTempNode->next;

            vos_mem_free(pTempNode);
            pMac->lim.gLimNumIbssPeers--;

            pTempNode = pTempNextNode;
            break;
         }
      }
      pPrevNode = pTempNode;
      pTempNode = pTempNextNode;
   }
}

/* 
                    
  
           
                                                               
                                
  
        
  
              
  
       
  
                                                    
                                                  
                                           
  
                                                                    
 */

tSirRetStatus
limIbssCoalesce(
    tpAniSirGlobal      pMac,
    tpSirMacMgmtHdr     pHdr,
    tpSchBeaconStruct   pBeacon,
    tANI_U8              *pIEs,
    tANI_U32            ieLen,
    tANI_U16            fTsfLater,
    tpPESession         psessionEntry)
{
    tANI_U16            peerIdx;
    tSirMacAddr         currentBssId;
    tLimIbssPeerNode    *pPeerNode;
    tpDphHashNode       pStaDs;
    tUpdateBeaconParams beaconParams; 

    vos_mem_set((tANI_U8 *)&beaconParams, sizeof(tUpdateBeaconParams), 0);

    sirCopyMacAddr(currentBssId,psessionEntry->bssId);

    limLog(pMac, LOG1, FL("Current BSSID :" MAC_ADDRESS_STR " Received BSSID :" MAC_ADDRESS_STR ),
                                  MAC_ADDR_ARRAY(currentBssId), MAC_ADDR_ARRAY(pHdr->bssId));

    /*                                                                */
    if ( !vos_mem_compare(currentBssId, pHdr->bssId, sizeof( tSirMacAddr )))
    {
       /*
                                                                             
                                                                                
                                                                                  
                                                                                 
                                                                              
                                                                                
                                                                                 
                                                                         
        */
       pPeerNode = ibss_peer_find(pMac, pHdr->sa);
       if (NULL != pPeerNode)
       {
          __limIbssSearchAndDeletePeer (pMac, psessionEntry, pHdr->sa);
          PELOGW(limLog(pMac, LOGW,
               FL("** Peer attempting to reconnect before HB timeout, deleted **"));)
          return eSIR_LIM_IGNORE_BEACON;
       }

       if (! fTsfLater) //                        
       {
          PELOGW(limLog(pMac, LOGW, FL("No Coalescing happened"));)
          return eSIR_LIM_IGNORE_BEACON;
       }
       /*
                                   
                                                                               
                                                                   
        */
       pMac->lim.gLimIbssCoalescingHappened = true;
       PELOGW(limLog(pMac, LOGW, FL("IBSS Coalescing happened"));)
          ibss_coalesce_save(pMac, pHdr, pBeacon);
       limLog(pMac, LOGW, FL("Delete BSSID :" MAC_ADDRESS_STR ),
             MAC_ADDR_ARRAY(currentBssId));
       ibss_bss_delete(pMac,psessionEntry);
       return eSIR_SUCCESS;
    }

    //                                            
    pPeerNode = ibss_peer_find(pMac, pHdr->sa);
    if (pPeerNode == NULL)
    {
        /*                                                                  */
        tANI_U32      frameLen;
        tSirRetStatus retCode;

        /*                                                                               
         */
#ifndef ANI_SIR_IBSS_PEER_CACHINGT
        if (pMac->lim.gLimNumIbssPeers >
              (pMac->lim.gLimIbssStaLimit - IBSS_STATIONS_USED_DURING_INIT))
        {
            PELOGE(limLog(pMac, LOGE, FL("**** MAX STA LIMIT HAS REACHED ****"));)
            return eSIR_LIM_MAX_STA_REACHED_ERROR;
        }
#endif
        PELOGW(limLog(pMac, LOGW, FL("IBSS Peer node does not exist, adding it***"));)
        frameLen = sizeof(tLimIbssPeerNode) + ieLen - sizeof(tANI_U32);

        pPeerNode = vos_mem_malloc((tANI_U16)frameLen);
        if (NULL == pPeerNode)
        {
            limLog(pMac, LOGP, FL("alloc fail (%d bytes) storing IBSS peer info"),
                   frameLen);
            return eSIR_MEM_ALLOC_FAILED;
        }

        pPeerNode->beacon = NULL;
        pPeerNode->beaconLen = 0;

        ibss_peer_collect(pMac, pBeacon, pHdr, pPeerNode,psessionEntry);
        pPeerNode->beacon = vos_mem_malloc(ieLen);
        if (NULL == pPeerNode->beacon)
        {
                PELOGE(limLog(pMac, LOGE, FL("Unable to allocate memory to store beacon"));)
        }
        else
        {
            vos_mem_copy(pPeerNode->beacon, pIEs, ieLen);
            pPeerNode->beaconLen = (tANI_U16)ieLen;
        }
        ibss_peer_add(pMac, pPeerNode);

        pStaDs = dphLookupHashEntry(pMac, pPeerNode->peerMacAddr, &peerIdx, &psessionEntry->dph.dphHashTable);
        if (pStaDs != NULL)
        {
            //                                      
            PELOGW(limLog(pMac, LOGW, FL("DPH Node present for just learned peer"));)
            PELOG1(limPrintMacAddr(pMac, pPeerNode->peerMacAddr, LOG1);)
            ibss_sta_info_update(pMac, pStaDs, pPeerNode,psessionEntry);
            return eSIR_SUCCESS;
        }
        retCode = limIbssStaAdd(pMac, pPeerNode->peerMacAddr,psessionEntry);
        if (retCode != eSIR_SUCCESS)
        {
            PELOGE(limLog(pMac, LOGE, FL("lim-ibss-sta-add failed (reason %x)"), retCode);)
            limPrintMacAddr(pMac, pPeerNode->peerMacAddr, LOGE);
            return retCode;
        }

        //                       
        pStaDs = dphLookupHashEntry(pMac, pPeerNode->peerMacAddr, &peerIdx, &psessionEntry->dph.dphHashTable);
        if(pMac->lim.gLimProtectionControl != WNI_CFG_FORCE_POLICY_PROTECTION_DISABLE)
            limIbssDecideProtection(pMac, pStaDs, &beaconParams, psessionEntry);

        if(beaconParams.paramChangeBitmap)
        {
            PELOGE(limLog(pMac, LOGE, FL("beaconParams.paramChangeBitmap=1 ---> Update Beacon Params "));)
            schSetFixedBeaconFields(pMac, psessionEntry);
            beaconParams.bssIdx = psessionEntry->bssIdx;
            limSendBeaconParams(pMac, &beaconParams, psessionEntry );
        }
    }
    else
        ibss_sta_caps_update(pMac, pPeerNode,psessionEntry);

    if (psessionEntry->limSmeState != eLIM_SME_NORMAL_STATE)
        return eSIR_SUCCESS;

    //                                     
    //                                            
    //                                    
    if (psessionEntry->limIbssActive == false)
    {
        limResetHBPktCount(psessionEntry);
        PELOGW(limLog(pMac, LOGW, FL("Partner joined our IBSS, Sending IBSS_ACTIVE Notification to SME"));)
        psessionEntry->limIbssActive = true;
        limSendSmeWmStatusChangeNtf(pMac, eSIR_SME_IBSS_ACTIVE, NULL, 0, psessionEntry->smeSessionId);
        limHeartBeatDeactivateAndChangeTimer(pMac, psessionEntry);
        MTRACE(macTrace(pMac, TRACE_CODE_TIMER_ACTIVATE, psessionEntry->peSessionId, eLIM_HEART_BEAT_TIMER));
        if (limActivateHearBeatTimer(pMac, psessionEntry) != TX_SUCCESS)
            limLog(pMac, LOGP, FL("could not activate Heartbeat timer"));
    }

    return eSIR_SUCCESS;
} /*                                   */


void limIbssHeartBeatHandle(tpAniSirGlobal pMac,tpPESession psessionEntry)
{
    tLimIbssPeerNode *pTempNode, *pPrevNode;
    tLimIbssPeerNode *pTempNextNode = NULL;
    tANI_U16      peerIdx=0;
    tpDphHashNode pStaDs=0;
    tANI_U32 threshold=0;
    tANI_U16 staIndex=0;
    tANI_U8 ucUcastSig=0;
    tANI_U8 ucBcastSig=0;

    /*                                                                                         
                                                                                                          
     */
    if(psessionEntry->limMlmState != eLIM_MLM_BSS_STARTED_STATE) {
        /*      
                                                            
                                                  
                                      
           
              */
        //                                                               
        limReactivateHeartBeatTimer(pMac, psessionEntry);
        return;
    }
    /*                             */
    if(!pMac->sys.gSysEnableLinkMonitorMode)
        return;

    pPrevNode = pTempNode  = pMac->lim.gLimIbssPeerList;
    threshold = (pMac->lim.gLimNumIbssPeers / 4 ) + 1;

    /*                                                              */
    while (pTempNode != NULL)
    {
        pTempNextNode = pTempNode->next;
        if(pTempNode->beaconHBCount) //                                                   
        {
            pTempNode->beaconHBCount = 0;
            pTempNode->heartbeatFailure = 0;
        }
        else //                                                       
        {
            pTempNode->heartbeatFailure++;
            PELOGE(limLog(pMac, LOGE, FL("Heartbeat fail = %d  thres = %d"), pTempNode->heartbeatFailure, pMac->lim.gLimNumIbssPeers);)
            if(pTempNode->heartbeatFailure >= threshold )
            {
                //                                
                pStaDs = dphLookupHashEntry(pMac, pTempNode->peerMacAddr, &peerIdx, &psessionEntry->dph.dphHashTable);
                if (pStaDs)
                {
                    staIndex = pStaDs->staIndex;
                    ucUcastSig = pStaDs->ucUcastSig;
                    ucBcastSig = pStaDs->ucBcastSig;

                    (void) limDelSta(pMac, pStaDs, false /*            */,psessionEntry);
                    limDeleteDphHashEntry(pMac, pStaDs->staAddr, peerIdx,psessionEntry);
                    limReleasePeerIdx(pMac, peerIdx, psessionEntry);
                    //                
                    ibss_status_chg_notify( pMac, pTempNode->peerMacAddr, staIndex, 
                                            ucUcastSig, ucBcastSig,
                                            eWNI_SME_IBSS_PEER_DEPARTED_IND,
                                            psessionEntry->smeSessionId );
                }
                if(pTempNode == pMac->lim.gLimIbssPeerList)
                {
                    pMac->lim.gLimIbssPeerList = pTempNode->next;
                    pPrevNode = pMac->lim.gLimIbssPeerList;
                }
                else
                    pPrevNode->next = pTempNode->next;

                vos_mem_free(pTempNode);
                pMac->lim.gLimNumIbssPeers--;

                pTempNode = pTempNextNode; //                                                     
                continue;
             }
         }

        pPrevNode = pTempNode;
        pTempNode = pTempNextNode;
    }

    /*                                                                                   */
    if(pMac->lim.gLimNumIbssPeers)
    {
        if(psessionEntry->LimRxedBeaconCntDuringHB < MAX_NO_BEACONS_PER_HEART_BEAT_INTERVAL)
            pMac->lim.gLimHeartBeatBeaconStats[psessionEntry->LimRxedBeaconCntDuringHB]++;
        else
            pMac->lim.gLimHeartBeatBeaconStats[0]++;

        limReactivateHeartBeatTimer(pMac, psessionEntry);

        //                                 
        limResetHBPktCount(psessionEntry);
        return;
    }
    else
    {

        PELOGW(limLog(pMac, LOGW, FL("Heartbeat Failure"));)
        pMac->lim.gLimHBfailureCntInLinkEstState++;

        if (psessionEntry->limIbssActive == true)
        {
            //                                        
            //                                          
            //                     
            PELOGW(limLog(pMac, LOGW, FL("Alone in IBSS"));)
            psessionEntry->limIbssActive = false;

            limSendSmeWmStatusChangeNtf(pMac, eSIR_SME_IBSS_INACTIVE,
                                          NULL, 0, psessionEntry->smeSessionId);
        }
    }
}


/*                                                               
                                   
                                                      
 
                              
                               
                                          
            
                                                               */
void
limIbssDecideProtectionOnDelete(tpAniSirGlobal pMac, 
      tpDphHashNode pStaDs, tpUpdateBeaconParams pBeaconParams,  tpPESession psessionEntry)
{
    tANI_U32 phyMode;
    tHalBitVal erpEnabled = eHAL_CLEAR;
    tSirRFBand rfBand = SIR_BAND_UNKNOWN;
    tANI_U32 i;    
    
    if(NULL == pStaDs)
      return;

    limGetRfBand(pMac, &rfBand, psessionEntry);
    if(SIR_BAND_2_4_GHZ == rfBand)
    {
        limGetPhyMode(pMac, &phyMode, psessionEntry);
        erpEnabled = pStaDs->erpEnabled;
        //                                                    
        if ( ((phyMode == WNI_CFG_PHY_MODE_11G) || psessionEntry->htCapability)
              && (erpEnabled == eHAL_CLEAR))
        {
            PELOGE(limLog(pMac, LOGE, FL("(%d) A legacy STA is disassociated. Addr is "),
                   psessionEntry->gLim11bParams.numSta);
            limPrintMacAddr(pMac, pStaDs->staAddr, LOGE);)
            if (psessionEntry->gLim11bParams.numSta > 0)
            {
                for (i=0; i<LIM_PROT_STA_CACHE_SIZE; i++)
                {
                    if (pMac->lim.protStaCache[i].active)
                    {
                        if (vos_mem_compare(pMac->lim.protStaCache[i].addr,
                                            pStaDs->staAddr, sizeof(tSirMacAddr)))
                        {
                            psessionEntry->gLim11bParams.numSta--;
                            pMac->lim.protStaCache[i].active = false;
                            break;
                        }
                    }
                }
            }

            if (psessionEntry->gLim11bParams.numSta == 0)
            {
                PELOGE(limLog(pMac, LOGE, FL("No more 11B STA exists. Disable protection. "));)
                limIbssSetProtection(pMac, false, pBeaconParams,psessionEntry);
            }
        }
    }
}

/*                                                                   
                                  
                                                                     
 
                              
                                       
                                                    
            
                                                                   */
static void
__limIbssPeerInactivityHandler(tpAniSirGlobal    pMac,
                               tpPESession psessionEntry,
                               tpSirIbssPeerInactivityInd peerInactivityInd)
{
   if(psessionEntry->limMlmState != eLIM_MLM_BSS_STARTED_STATE)
   {
      limReactivateHeartBeatTimer(pMac, psessionEntry);
      return;
   }

   /*                                                 */
   __limIbssSearchAndDeletePeer (pMac, psessionEntry, peerInactivityInd->peerAddr);

}

/*                                                               
                                
                                      
 
                              
                             
            
                                                               */
void
limProcessIbssPeerInactivity(tpAniSirGlobal pMac, void *buf)
{
   /*
                                                               
                                                                   
                                                                 
                                    
    */
   tANI_U8       i;

   tSirIbssPeerInactivityInd *peerInactivityInd =
      (tSirIbssPeerInactivityInd *) buf;

   /*
                                                                
                                       
    */
   if (eLIM_STA_IN_IBSS_ROLE != pMac->lim.gLimSystemRole &&
         !IS_IBSS_HEARTBEAT_OFFLOAD_FEATURE_ENABLE)
   {
      return;
   }

   /*                             */
   if (!pMac->sys.gSysEnableLinkMonitorMode)
   {
      return;
   }

   for (i = 0; i < pMac->lim.maxBssId; i++)
   {
      if (VOS_TRUE == pMac->lim.gpSession[i].valid &&
            eSIR_IBSS_MODE == pMac->lim.gpSession[i].bssType)
      {
         __limIbssPeerInactivityHandler(pMac,
               &pMac->lim.gpSession[i],
               peerInactivityInd);
         break;
      }
   }
}


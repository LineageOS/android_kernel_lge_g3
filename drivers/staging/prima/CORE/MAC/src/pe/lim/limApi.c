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
#include "wniCfgSta.h"
#include "wniApi.h"
#include "sirCommon.h"
#include "sirDebug.h"
#include "cfgApi.h"

#include "schApi.h"
#include "utilsApi.h"
#include "limApi.h"
#include "limGlobal.h"
#include "limTypes.h"
#include "limUtils.h"
#include "limAssocUtils.h"
#include "limPropExtsUtils.h"
#include "limSerDesUtils.h"
#include "limIbssPeerMgmt.h"
#include "limAdmitControl.h"
#include "pmmApi.h"
#include "logDump.h"
#include "limSendSmeRspMessages.h"
#include "wmmApsd.h"
#include "limTrace.h"
#include "limSession.h"
#include "wlan_qct_wda.h"

#if defined WLAN_FEATURE_VOWIFI
#include "rrmApi.h"
#endif

#include <limFT.h>
#include "vos_types.h"
#include "vos_packet.h"
#include "wlan_qct_tl.h"
#include "sysStartup.h"


static void __limInitScanVars(tpAniSirGlobal pMac)
{
    pMac->lim.gLimUseScanModeForLearnMode = 1;

    pMac->lim.gLimSystemInScanLearnMode = 0;

    //                            
    pMac->lim.gLimReturnAfterFirstMatch = 0;
    pMac->lim.gLim24Band11dScanDone = 0;
    pMac->lim.gLim50Band11dScanDone = 0;
    pMac->lim.gLimReturnUniqueResults = 0;

    //                                       
    pMac->lim.gLimNumOfBackgroundScanSuccess = 0;
    pMac->lim.gLimNumOfConsecutiveBkgndScanFailure = 0;
    pMac->lim.gLimNumOfForcedBkgndScan = 0;
    pMac->lim.gLimBackgroundScanDisable = false;      //                 
    pMac->lim.gLimForceBackgroundScanDisable = false; //                  
    pMac->lim.gLimBackgroundScanTerminate = TRUE;    //                 
    pMac->lim.gLimReportBackgroundScanResults = FALSE;    //                 

    pMac->lim.gLimCurrentScanChannelId = 0;
    pMac->lim.gpLimMlmScanReq = NULL;
    pMac->lim.gDeferMsgTypeForNOA = 0;
    pMac->lim.gpDefdSmeMsgForNOA = NULL;
    pMac->lim.gLimMlmScanResultLength = 0;
    pMac->lim.gLimSmeScanResultLength = 0;

    vos_mem_set(pMac->lim.gLimCachedScanHashTable,
                sizeof(pMac->lim.gLimCachedScanHashTable), 0);

#ifdef WLAN_FEATURE_ROAM_SCAN_OFFLOAD

    pMac->lim.gLimMlmLfrScanResultLength = 0;
    pMac->lim.gLimSmeLfrScanResultLength = 0;

    vos_mem_set(pMac->lim.gLimCachedLfrScanHashTable,
                sizeof(pMac->lim.gLimCachedLfrScanHashTable), 0);
#endif
    pMac->lim.gLimBackgroundScanChannelId = 0;
    pMac->lim.gLimBackgroundScanStarted = 0;
    pMac->lim.gLimRestoreCBNumScanInterval = LIM_RESTORE_CB_NUM_SCAN_INTERVAL_DEFAULT;
    pMac->lim.gLimRestoreCBCount = 0;
    vos_mem_set(pMac->lim.gLimLegacyBssidList,
                sizeof(pMac->lim.gLimLegacyBssidList), 0);

    /*                        */
    pMac->lim.gLimTriggerBackgroundScanDuringQuietBss = 0;


    //                                             
    pMac->lim.abortScan = 0;
    vos_mem_set(&pMac->lim.scanChnInfo, sizeof(tLimScanChnInfo), 0);
    vos_mem_set(&pMac->lim.dfschannelList, sizeof(tSirDFSChannelList), 0);

//                         
    pMac->lim.gpLimSuspendCallback = NULL;
    pMac->lim.gpLimResumeCallback = NULL;
//                             
}


static void __limInitBssVars(tpAniSirGlobal pMac)
{

    vos_mem_set((void*)pMac->lim.gpSession,
                 sizeof(*pMac->lim.gpSession)*pMac->lim.maxBssId, 0);


    //pMac->lim.gpLimStartBssReq = NULL;




/*                                                                                                                    */
#if 0

   //                                            
   //                      
   vos_mem_set(&pMac->lim.gLimCurrentBssId, sizeof(tSirMacAddr), 0);
   pMac->lim.gLimCurrentChannelId = HAL_INVALID_CHANNEL_ID;
   vos_mem_set(&pMac->lim.gLimCurrentSSID, sizeof(tSirMacSSid), 0);
   pMac->lim.gLimCurrentBssCaps = 0;
   QosCaps is a bit map of various qos capabilities - see defn above
   pMac->lim.gLimCurrentBssQosCaps = 0;
   pMac->lim.gLimCurrentBssPropCap = 0;
   pMac->lim.gLimSentCapsChangeNtf = 0;

   //                                      
   //                              
   vos_mem_set(&pMac->lim.gLimReassocBssId, sizeof(tSirMacAddr), 0);
   pMac->lim.gLimReassocChannelId = 0;
   vos_mem_set(&pMac->lim.gLimReassocSSID, sizeof(tSirMacSSid), 0);
   pMac->lim.gLimReassocBssCaps = 0;
   pMac->lim.gLimReassocBssQosCaps = 0;
   pMac->lim.gLimReassocBssPropCap = 0;
 #endif

    /*                                                                    */
    pMac->lim.gLimForceNoPropIE = 0;

   //                          

    pMac->lim.gpLimMlmSetKeysReq = NULL;
    pMac->lim.gpLimMlmRemoveKeyReq = NULL;
  //                                              

}


static void __limInitStatsVars(tpAniSirGlobal pMac)
{
    pMac->lim.gLimNumBeaconsRcvd = 0;
    pMac->lim.gLimNumBeaconsIgnored = 0;

    pMac->lim.gLimNumDeferredMsgs = 0;

    //                                                               
    //                                   
    pMac->lim.gLimNumOfAniSTAs = 0;      //                   

    //                                                        
    //                                        

    //                          
    pMac->lim.gLimHeartBeatCount = 0;

    //                                                                    
    vos_mem_set(pMac->lim.gLimHeartBeatBeaconStats,
                sizeof(pMac->lim.gLimHeartBeatBeaconStats), 0);

#ifdef WLAN_DEBUG    
    //               
    pMac->lim.numTot = 0;
    pMac->lim.numBbt = 0;
    pMac->lim.numProtErr = 0;
    pMac->lim.numLearn = 0;
    pMac->lim.numLearnIgnore = 0;
    pMac->lim.numSme = 0;
    vos_mem_set(pMac->lim.numMAC, sizeof(pMac->lim.numMAC), 0);
    pMac->lim.gLimNumAssocReqDropInvldState = 0;
    pMac->lim.gLimNumAssocReqDropACRejectTS = 0;
    pMac->lim.gLimNumAssocReqDropACRejectSta = 0;
    pMac->lim.gLimNumReassocReqDropInvldState = 0;
    pMac->lim.gLimNumHashMissIgnored = 0;
    pMac->lim.gLimUnexpBcnCnt = 0;
    pMac->lim.gLimBcnSSIDMismatchCnt = 0;
    pMac->lim.gLimNumLinkEsts = 0;
    pMac->lim.gLimNumRxCleanup = 0;
    pMac->lim.gLim11bStaAssocRejectCount = 0;
#endif    
}



static void __limInitStates(tpAniSirGlobal pMac)
{
    //                          
    pMac->lim.gLimHBfailureCntInLinkEstState = 0;
    pMac->lim.gLimProbeFailureAfterHBfailedCnt = 0;
    pMac->lim.gLimHBfailureCntInOtherStates = 0;
    pMac->lim.gLimRspReqd = 0;
    pMac->lim.gLimPrevSmeState = eLIM_SME_OFFLINE_STATE;

    //                                             
    MTRACE(macTrace(pMac, TRACE_CODE_MLM_STATE, NO_SESSION, eLIM_MLM_IDLE_STATE));
    pMac->lim.gLimMlmState = eLIM_MLM_IDLE_STATE;

    //                    
    pMac->lim.gLimPrevMlmState = eLIM_MLM_OFFLINE_STATE;

#ifdef GEN4_SCAN
    //                                                    
    pMac->lim.gLimHalScanState = eLIM_HAL_IDLE_SCAN_STATE;
#endif //          

    /* 
                                                 
     */
    pMac->lim.gLimSmeState     = eLIM_SME_OFFLINE_STATE;

    /* 
                                                             
                                          
     */

    vos_mem_set(&pMac->lim.gLimOverlap11gParams, sizeof(tLimProtStaParams), 0);
    vos_mem_set(&pMac->lim.gLimOverlap11aParams, sizeof(tLimProtStaParams), 0);
    vos_mem_set(&pMac->lim.gLimOverlapHt20Params, sizeof(tLimProtStaParams), 0);
    vos_mem_set(&pMac->lim.gLimOverlapNonGfParams, sizeof(tLimProtStaParams), 0);
    vos_mem_set(&pMac->lim.gLimNoShortParams, sizeof(tLimNoShortParams), 0);
    vos_mem_set(&pMac->lim.gLimNoShortSlotParams, sizeof(tLimNoShortSlotParams), 0);

    pMac->lim.gLimPhyMode = 0; 
    pMac->lim.scanStartTime = 0;    //                          

    vos_mem_set(pMac->lim.gLimMyMacAddr, sizeof(pMac->lim.gLimMyMacAddr), 0);
    pMac->lim.ackPolicy = 0;

#if 0 /*                                               */
    pMac->lim.gLimQosEnabled = 0; //   
    pMac->lim.gLimWmeEnabled = 0; //   
    pMac->lim.gLimWsmEnabled = 0; //   
    pMac->lim.gLimHcfEnabled = 0;
    pMac->lim.gLim11dEnabled = 0;
#endif

    pMac->lim.gLimProbeRespDisableFlag = 0; //                            
}

static void __limInitVars(tpAniSirGlobal pMac)
{


    //                                                      

    //         
    pMac->lim.gLimNumWdsInfoInd = 0;
    pMac->lim.gLimNumWdsInfoSet = 0;
    vos_mem_set(&pMac->lim.gLimWdsInfo, sizeof(tSirWdsInfo), 0);
    /*                            */
    limInitWdsInfoParams(pMac);

    //                         
    vos_mem_set(&pMac->lim.gLimDeferredMsgQ, sizeof(tSirAddtsReq), 0);

    //                                                               
    vos_mem_set(&pMac->lim.gLimAddtsReq, sizeof(tSirAddtsReq) , 0);
    pMac->lim.gLimAddtsSent = 0;
    pMac->lim.gLimAddtsRspTimerCount = 0;

    //                               
    vos_mem_set(&pMac->lim.cfgProtection, sizeof(tCfgProtection), 0);
    pMac->lim.gLimProtectionControl = 0;
    vos_mem_set(&pMac->lim.gLimAlternateRadio, sizeof(tSirAlternateRadioInfo), 0);
    SET_LIM_PROCESS_DEFD_MESGS(pMac, true);

#if 0
    //                                     
    LIM_SET_RADAR_DETECTED(pMac, eANI_BOOLEAN_FALSE);
    pMac->sys.gSysEnableLearnMode = eANI_BOOLEAN_TRUE;
#endif
    //                 
    pMac->lim.gUapsdEnable = 0;
    pMac->lim.gUapsdPerAcBitmask = 0;
    pMac->lim.gUapsdPerAcTriggerEnableMask = 0;
    pMac->lim.gUapsdPerAcDeliveryEnableMask = 0;

    //                                               
    pMac->lim.gAcAdmitMask[SIR_MAC_DIRECTION_UPLINK] = 0;
    pMac->lim.gAcAdmitMask[SIR_MAC_DIRECTION_DNLINK] = 0;

    //                                                             
    pMac->lim.pDialogueTokenHead = NULL;
    pMac->lim.pDialogueTokenTail = NULL;

    vos_mem_set(&pMac->lim.tspecInfo,
                sizeof(tLimTspecInfo) * LIM_NUM_TSPEC_MAX, 0);

    //                                     
    vos_mem_set(&pMac->lim.admitPolicyInfo, sizeof(tLimAdmitPolicyInfo), 0);

    pMac->lim.gLastBeaconDtimCount = 0;
    pMac->lim.gLastBeaconDtimPeriod = 0;

    //                       
    pMac->lim.gScanInPowersave = 0;
}

static void __limInitAssocVars(tpAniSirGlobal pMac)
{
    tANI_U32 val;
#if 0
    vos_mem_set(pMac->lim.gpLimAIDpool,
          sizeof(*pMac->lim.gpLimAIDpool) * (WNI_CFG_ASSOC_STA_LIMIT_STAMAX+1), 0);
    pMac->lim.freeAidHead = 0;
    pMac->lim.freeAidTail = 0;
#endif
    if(wlan_cfgGetInt(pMac, WNI_CFG_ASSOC_STA_LIMIT, &val) != eSIR_SUCCESS)
    {
        limLog( pMac, LOGP, FL( "cfg get assoc sta limit failed" ));
    }
    pMac->lim.gLimAssocStaLimit = val;
    pMac->lim.gLimIbssStaLimit = val;

    //                                                
    //              
    pMac->lim.gpLimMlmAuthReq = NULL;
    //pMac->lim.gpLimMlmJoinReq = NULL;

    //                                              
    pMac->lim.gLimPreAuthChannelNumber = 0;
    pMac->lim.gLimPreAuthType = eSIR_OPEN_SYSTEM;
    vos_mem_set(&pMac->lim.gLimPreAuthPeerAddr, sizeof(tSirMacAddr), 0);
    pMac->lim.gLimNumPreAuthContexts = 0;
    vos_mem_set(&pMac->lim.gLimPreAuthTimerTable, sizeof(tLimPreAuthTable), 0);

    //                               
    pMac->lim.gLimDeauthReasonCode = 0;

    //                                              
    pMac->lim.pLimPreAuthList = NULL;

    //                                             
    pMac->lim.gLimDisassocFrameThreshold = LIM_SEND_DISASSOC_FRAME_THRESHOLD;
    pMac->lim.gLimDisassocFrameCredit = 0;

    //                                               
    vos_mem_set(pMac->lim.protStaOverlapCache,
                sizeof(tCacheParams) * LIM_PROT_STA_OVERLAP_CACHE_SIZE, 0);
    vos_mem_set(pMac->lim.protStaCache,
                sizeof(tCacheParams) * LIM_PROT_STA_CACHE_SIZE, 0);

#if  defined (WLAN_FEATURE_VOWIFI_11R) || defined (FEATURE_WLAN_CCX) || defined(FEATURE_WLAN_LFR)
    pMac->lim.pSessionEntry = NULL;
    pMac->lim.reAssocRetryAttempt = 0;
#endif

}


static void __limInitTitanVars(tpAniSirGlobal pMac)
{
#if 0
    vos_mem_set(&pMac->lim.gLimChannelSwitch, sizeof(tLimChannelSwitchInfo), 0);
    pMac->lim.gLimChannelSwitch.state               = eLIM_CHANNEL_SWITCH_IDLE;
    pMac->lim.gLimChannelSwitch.secondarySubBand    = PHY_SINGLE_CHANNEL_CENTERED;
#endif
    //                              
    //                                     
    pMac->lim.gLimScanOverride = 1;
    pMac->lim.gLimScanOverrideSaved = eSIR_ACTIVE_SCAN;
    pMac->lim.gLimTitanStaCount = 0;
    pMac->lim.gLimBlockNonTitanSta = 0;
}

static void __limInitHTVars(tpAniSirGlobal pMac)
{
    pMac->lim.htCapabilityPresentInBeacon = 0;
    pMac->lim.gHTGreenfield = 0;
    pMac->lim.gHTShortGI40Mhz = 0;
    pMac->lim.gHTShortGI20Mhz = 0;
    pMac->lim.gHTMaxAmsduLength = 0;
    pMac->lim.gHTDsssCckRate40MHzSupport = 0;
    pMac->lim.gHTPSMPSupport = 0;
    pMac->lim.gHTLsigTXOPProtection = 0;
    pMac->lim.gHTMIMOPSState = eSIR_HT_MIMO_PS_STATIC;
    pMac->lim.gHTAMpduDensity = 0;

    pMac->lim.gMaxAmsduSizeEnabled = false;
    pMac->lim.gHTMaxRxAMpduFactor = 0;
    pMac->lim.gHTServiceIntervalGranularity = 0;
    pMac->lim.gHTControlledAccessOnly = 0;
    pMac->lim.gHTOperMode = eSIR_HT_OP_MODE_PURE;
    pMac->lim.gHTPCOActive = 0;

    pMac->lim.gHTPCOPhase = 0;
    pMac->lim.gHTSecondaryBeacon = 0;
    pMac->lim.gHTDualCTSProtection = 0;
    pMac->lim.gHTSTBCBasicMCS = 0;
    pMac->lim.gAddBA_Declined = 0;               //                                                                    
}

static tSirRetStatus __limInitConfig( tpAniSirGlobal pMac )
{
   tANI_U32 val1, val2, val3;
   tANI_U16 val16;
   tANI_U8 val8;
   tSirMacHTCapabilityInfo   *pHTCapabilityInfo;
   tSirMacHTInfoField1       *pHTInfoField1;
   tpSirPowerSaveCfg          pPowerSaveConfig;
   tSirMacHTParametersInfo   *pAmpduParamInfo;

   /*                                                                   */
   /*                                                                                        
                                                               
    */

   if(wlan_cfgGetInt(pMac, WNI_CFG_HT_CAP_INFO, &val1) != eSIR_SUCCESS) 
   {
      PELOGE(limLog(pMac, LOGE, FL("could not retrieve HT Cap CFG"));)
      return eSIR_FAILURE;
   }

   if(wlan_cfgGetInt(pMac, WNI_CFG_CHANNEL_BONDING_MODE, &val2) != eSIR_SUCCESS) 
   {
      PELOGE(limLog(pMac, LOGE, FL("could not retrieve Channel Bonding CFG"));)
      return eSIR_FAILURE;
   }
   val16 = ( tANI_U16 ) val1;
   pHTCapabilityInfo = ( tSirMacHTCapabilityInfo* ) &val16;
  
   //                                                                          
   //                                                                       
   pHTCapabilityInfo->supportedChannelWidthSet = val2 ? 
     WNI_CFG_CHANNEL_BONDING_MODE_ENABLE : WNI_CFG_CHANNEL_BONDING_MODE_DISABLE;
   if(cfgSetInt(pMac, WNI_CFG_HT_CAP_INFO, *(tANI_U16*)pHTCapabilityInfo) 
      != eSIR_SUCCESS)
   {
      PELOGE(limLog(pMac, LOGE, FL("could not update HT Cap Info CFG"));)
      return eSIR_FAILURE;
   }

   if(wlan_cfgGetInt(pMac, WNI_CFG_HT_INFO_FIELD1, &val1) != eSIR_SUCCESS)
   {
      PELOGE(limLog(pMac, LOGE, FL("could not retrieve HT INFO Field1 CFG"));)
      return eSIR_FAILURE;
   }

   val8 = ( tANI_U8 ) val1;
   pHTInfoField1 = ( tSirMacHTInfoField1* ) &val8;
   pHTInfoField1->recommendedTxWidthSet = 
     (tANI_U8)pHTCapabilityInfo->supportedChannelWidthSet;
   if(cfgSetInt(pMac, WNI_CFG_HT_INFO_FIELD1, *(tANI_U8*)pHTInfoField1) 
      != eSIR_SUCCESS)
   {
      PELOGE(limLog(pMac, LOGE, FL("could not update HT Info Field"));)
      return eSIR_FAILURE;
   }

   /*                              */
  
   if( wlan_cfgGetInt(pMac, WNI_CFG_HEART_BEAT_THRESHOLD, &val1) != 
       eSIR_SUCCESS ) 
   {
      PELOGE(limLog(pMac, LOGE, FL("could not retrieve WNI_CFG_HEART_BEAT_THRESHOLD CFG"));)
      return eSIR_FAILURE;
   }
   if(!val1) 
   {
      limDeactivateAndChangeTimer(pMac, eLIM_HEART_BEAT_TIMER);
      pMac->sys.gSysEnableLinkMonitorMode = 0;
   } 
   else 
   {
      //                                               
      pMac->sys.gSysEnableLinkMonitorMode = 1;
   }

   /*                        */

   if (wlan_cfgGetInt(pMac, WNI_CFG_HT_CAP_INFO, &val1) != eSIR_SUCCESS) 
   {
      PELOGE(limLog(pMac, LOGE, FL("could not retrieve HT Cap CFG"));)
      return eSIR_FAILURE;
   }
   if (wlan_cfgGetInt(pMac, WNI_CFG_SHORT_GI_20MHZ, &val2) != eSIR_SUCCESS) 
   {
      PELOGE(limLog(pMac, LOGE, FL("could not retrieve shortGI 20Mhz CFG"));)
      return eSIR_FAILURE;
   }
   if (wlan_cfgGetInt(pMac, WNI_CFG_SHORT_GI_40MHZ, &val3) != eSIR_SUCCESS) 
   {
      PELOGE(limLog(pMac, LOGE, FL("could not retrieve shortGI 40Mhz CFG"));)
      return eSIR_FAILURE;
   }

   val16 = ( tANI_U16 ) val1;
   pHTCapabilityInfo = ( tSirMacHTCapabilityInfo* ) &val16;
   pHTCapabilityInfo->shortGI20MHz = (tANI_U16)val2;
   pHTCapabilityInfo->shortGI40MHz = (tANI_U16)val3;

   if(cfgSetInt(pMac,  WNI_CFG_HT_CAP_INFO, *(tANI_U16*)pHTCapabilityInfo) != 
      eSIR_SUCCESS)
   {
      PELOGE(limLog(pMac, LOGE, FL("could not update HT Cap Info CFG"));)
      return eSIR_FAILURE;
   }

   /*                             */

   if (wlan_cfgGetInt(pMac, WNI_CFG_HT_AMPDU_PARAMS, &val1) != eSIR_SUCCESS) 
   {
      PELOGE(limLog(pMac, LOGE, FL("could not retrieve HT AMPDU Param CFG"));)
      return eSIR_FAILURE;
   }
   if (wlan_cfgGetInt(pMac, WNI_CFG_MAX_RX_AMPDU_FACTOR, &val2) != eSIR_SUCCESS) 
   {
      PELOGE(limLog(pMac, LOGE, FL("could not retrieve AMPDU Factor CFG"));)
      return eSIR_FAILURE;
   }
   val16 = ( tANI_U16 ) val1;
   pAmpduParamInfo = ( tSirMacHTParametersInfo* ) &val16;
   pAmpduParamInfo->maxRxAMPDUFactor = (tANI_U8)val2;
   if(cfgSetInt(pMac,  WNI_CFG_HT_AMPDU_PARAMS, *(tANI_U8*)pAmpduParamInfo) != 
      eSIR_SUCCESS)
   {
      PELOGE(limLog(pMac, LOGE, FL("could not update HT AMPDU Param CFG"));)
      return eSIR_FAILURE;
   }

   /*                                                     
                                                         */
   if(wlan_cfgGetInt(pMac, WNI_CFG_SHORT_PREAMBLE, &val1) != eSIR_SUCCESS)
   {
      limLog(pMac, LOGP, FL("cfg get short preamble failed"));
      return eSIR_FAILURE;
   }

   /*                     */

   /*                                                */
   pPowerSaveConfig = vos_mem_malloc(sizeof(tSirPowerSaveCfg));
   if (NULL == pPowerSaveConfig)
   {
      PELOGE(limLog(pMac, LOGE, FL("LIM: Cannot allocate memory for power save "
                                  "configuration"));)
      return eSIR_FAILURE;
   }
       
   /*                                                                          
                                                                            
                        
    */
   vos_mem_copy(pPowerSaveConfig, (tANI_U8 *)&pMac->pmm.gPmmCfg, sizeof(tSirPowerSaveCfg));

   /*                                                             */
   if ( (pmmSendPowerSaveCfg(pMac, pPowerSaveConfig)) != eSIR_SUCCESS)
   {
      PELOGE(limLog(pMac, LOGE, FL("LIM: pmmSendPowerSaveCfg() failed "));)
      return eSIR_FAILURE;
   }

   /*                                           */

   PELOG1(limLog(pMac, LOG1,
      FL("VALID_CHANNEL_LIST has changed, reset next bg scan channel"));)
   pMac->lim.gLimBackgroundScanChannelId = 0;

   /*                                                */

   /*                                                                        
                                                 */
   handleHTCapabilityandHTInfo(pMac, NULL);
   if(wlan_cfgGetInt(pMac, WNI_CFG_DISABLE_LDPC_WITH_TXBF_AP,(tANI_U32 *) &pMac->lim.disableLDPCWithTxbfAP) != eSIR_SUCCESS)
   {
      limLog(pMac, LOGP, FL("cfg get disableLDPCWithTxbfAP failed"));
      return eSIR_FAILURE;
   }
#ifdef FEATURE_WLAN_TDLS
   if(wlan_cfgGetInt(pMac, WNI_CFG_TDLS_BUF_STA_ENABLED,(tANI_U32 *) &pMac->lim.gLimTDLSBufStaEnabled) != eSIR_SUCCESS)
   {
       limLog(pMac, LOGP, FL("cfg get LimTDLSBufStaEnabled failed"));
       return eSIR_FAILURE;
   }
   if(wlan_cfgGetInt(pMac, WNI_CFG_TDLS_QOS_WMM_UAPSD_MASK,(tANI_U32 *) &pMac->lim.gLimTDLSUapsdMask) != eSIR_SUCCESS)
   {
       limLog(pMac, LOGP, FL("cfg get LimTDLSUapsdMask failed"));
       return eSIR_FAILURE;
   }
#endif
   return eSIR_SUCCESS;
}

/*
           
                                                                             
                                 
*/
tSirRetStatus limStart(tpAniSirGlobal pMac)
{
   tSirResultCodes retCode = eSIR_SUCCESS;

   PELOG1(limLog(pMac, LOG1, FL(" enter"));)

   if (pMac->lim.gLimSmeState == eLIM_SME_OFFLINE_STATE)
   {
      pMac->lim.gLimSmeState = eLIM_SME_IDLE_STATE;

      MTRACE(macTrace(pMac, TRACE_CODE_SME_STATE, NO_SESSION, pMac->lim.gLimSmeState));

      //                                                
      pMac->lim.gLimReturnAfterFirstMatch = 0;

      //                             
      limInitMlm(pMac);

      //                                      
      pMac->lim.gLimReturnUniqueResults = true;
      pMac->lim.gLimSmeScanResultLength = 0;
#ifdef WLAN_FEATURE_ROAM_SCAN_OFFLOAD
      pMac->lim.gLimSmeLfrScanResultLength = 0;
#endif
   }
   else
   {
      /* 
                                                             
                                                       
                 
      */
      limLog(pMac, LOGE, FL("Invalid SME state %X"),pMac->lim.gLimSmeState );
      retCode = eSIR_FAILURE;
   }
   
   return retCode;
}

/* 
                  
  
           
                                                          
                                                                       
  
        
     
  
              
     
  
       
     
  
                                                 
               
 */

tSirRetStatus
limInitialize(tpAniSirGlobal pMac)
{
    tSirRetStatus status = eSIR_SUCCESS;

    __limInitAssocVars(pMac);
    __limInitVars(pMac);
    __limInitStates(pMac);
    __limInitStatsVars(pMac);
    __limInitBssVars(pMac);
    __limInitScanVars(pMac);
    __limInitHTVars(pMac);
    __limInitTitanVars(pMac);

    status = limStart(pMac);
    if(eSIR_SUCCESS != status)
    {
        return status;
    }

    /*
                                                                   
                                                                 
                                                                  
                                                           
     */
    //                 
    //                                              
    limIbssInit(pMac);

    pmmInitialize(pMac);

    
#if defined WLAN_FEATURE_VOWIFI
    rrmInitialize(pMac);
#endif
#if defined WLAN_FEATURE_VOWIFI_11R
    limFTOpen(pMac);
#endif

    vos_list_init(&pMac->lim.gLimMgmtFrameRegistratinQueue);

#if 0

    vos_trace_setLevel(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_ERROR);
    vos_trace_setLevel(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_WARN);
    vos_trace_setLevel(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_FATAL);

    vos_trace_setLevel(VOS_MODULE_ID_HAL, VOS_TRACE_LEVEL_WARN);
    vos_trace_setLevel(VOS_MODULE_ID_HAL, VOS_TRACE_LEVEL_ERROR);

    vos_trace_setLevel(VOS_MODULE_ID_SYS, VOS_TRACE_LEVEL_WARN);
    vos_trace_setLevel(VOS_MODULE_ID_SYS, VOS_TRACE_LEVEL_ERROR);
    vos_trace_setLevel(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR);
    
    vos_trace_setLevel(VOS_MODULE_ID_SAL, VOS_TRACE_LEVEL_ERROR);
    
    vos_trace_setLevel(VOS_MODULE_ID_SSC, VOS_TRACE_LEVEL_ERROR);
    
    vos_trace_setLevel(VOS_MODULE_ID_SAL, VOS_TRACE_LEVEL_ERROR);
    vos_trace_setLevel(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR);

    vos_trace_setLevel(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR);

    
    vos_trace_setLevel(VOS_MODULE_ID_BAL, VOS_TRACE_LEVEL_ERROR);
    
    vos_trace_setLevel(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR);
#endif
    MTRACE(limTraceInit(pMac));

    //                                          
    if( eSIR_FAILURE == __limInitConfig(pMac))
    {
       //                                      
       limCleanupMlm(pMac);
       return eSIR_FAILURE;
    }

   //                                             
   //                                                                     
   //                                                      
   limAdmitControlInit(pMac);
   limRegisterHalIndCallBack(pMac);

   return status;
            
} /*                         */



/* 
               
  
           
                                                       
                       
  
        
     
  
              
     
  
       
     
  
                                                 
               
 */

void
limCleanup(tpAniSirGlobal pMac)
{
    v_PVOID_t pvosGCTx;
    VOS_STATUS retStatus;

//                                                                       
//                                                                     

    tpLimMgmtFrameRegistration pLimMgmtRegistration = NULL;
    
    while(vos_list_remove_front(&pMac->lim.gLimMgmtFrameRegistratinQueue,
            (vos_list_node_t**)&pLimMgmtRegistration) == VOS_STATUS_SUCCESS)
    {
        VOS_TRACE(VOS_MODULE_ID_PE, VOS_TRACE_LEVEL_INFO,
                FL("Fixing leak! Deallocating pLimMgmtRegistration node"));

        vos_mem_free(pLimMgmtRegistration);
    }

    vos_list_destroy(&pMac->lim.gLimMgmtFrameRegistratinQueue);

    limCleanupMlm(pMac);
    limCleanupLmm(pMac);

    //                      
    if (pMac->lim.gLimPreAuthTimerTable.pTable != NULL)
    {
        vos_mem_free(pMac->lim.gLimPreAuthTimerTable.pTable);
        pMac->lim.gLimPreAuthTimerTable.pTable = NULL;
        pMac->lim.gLimPreAuthTimerTable.numEntry = 0;
    }

    if(NULL != pMac->lim.pDialogueTokenHead)
    {
        limDeleteDialogueTokenList(pMac);
    }

    if(NULL != pMac->lim.pDialogueTokenTail)
    {
        vos_mem_free(pMac->lim.pDialogueTokenTail);
        pMac->lim.pDialogueTokenTail = NULL;
    }

    # if 0
    if (pMac->lim.gpLimStartBssReq != NULL)
    {
        vos_mem_free(pMac->lim.gpLimStartBssReq);
        pMac->lim.gpLimStartBssReq = NULL;
    }
    #endif

    if (pMac->lim.gpLimMlmSetKeysReq != NULL)
    {
        vos_mem_free(pMac->lim.gpLimMlmSetKeysReq);
        pMac->lim.gpLimMlmSetKeysReq = NULL;
    }

    #if 0
    if (pMac->lim.gpLimJoinReq != NULL)
    {
        vos_mem_free(pMac->lim.gpLimJoinReq);
        pMac->lim.gpLimJoinReq = NULL;
    }
    #endif

    if (pMac->lim.gpLimMlmAuthReq != NULL)
    {
        vos_mem_free(pMac->lim.gpLimMlmAuthReq);
        pMac->lim.gpLimMlmAuthReq = NULL;
    }

#if 0
    if (pMac->lim.gpLimMlmJoinReq != NULL)
    {
        vos_mem_free(pMac->lim.gpLimMlmJoinReq);
        pMac->lim.gpLimMlmJoinReq = NULL;
    }
#endif

    #if 0
    if (pMac->lim.gpLimReassocReq != NULL)
    {
        vos_mem_free(pMac->lim.gpLimReassocReq);
        pMac->lim.gpLimReassocReq = NULL;
    }
    #endif

    if (pMac->lim.gpLimMlmRemoveKeyReq != NULL)
    {
        vos_mem_free(pMac->lim.gpLimMlmRemoveKeyReq);
        pMac->lim.gpLimMlmRemoveKeyReq = NULL;
    }

    if (pMac->lim.gpDefdSmeMsgForNOA != NULL)
    {
        vos_mem_free(pMac->lim.gpDefdSmeMsgForNOA);
        pMac->lim.gpDefdSmeMsgForNOA = NULL;
    }

    if (pMac->lim.gpLimMlmScanReq != NULL)
    {
        vos_mem_free(pMac->lim.gpLimMlmScanReq);
        pMac->lim.gpLimMlmScanReq = NULL;
    }

#if 0
    if(NULL != pMac->lim.beacon)
    {
        vos_mem_free((void*) pMac->lim.beacon);
        pMac->lim.beacon = NULL;
     }
#endif
    #if 0
    if(NULL != pMac->lim.assocReq)
    {
        vos_mem_free((void*) pMac->lim.assocReq);
        pMac->lim.assocReq= NULL;
     }
    #endif

#if 0
    if(NULL != pMac->lim.assocRsp)
    {
        vos_mem_free((void*) pMac->lim.assocRsp);
        pMac->lim.assocRsp= NULL;
     }
#endif
    //                                                       
    limResetDeferredMsgQ(pMac);


    pvosGCTx = vos_get_global_context(VOS_MODULE_ID_PE, (v_VOID_t *) pMac);
    retStatus = WLANTL_DeRegisterMgmtFrmClient(pvosGCTx);

    if ( retStatus != VOS_STATUS_SUCCESS )
        PELOGE(limLog(pMac, LOGE, FL("DeRegistering the PE Handle with TL has failed bailing out..."));)

#if defined WLAN_FEATURE_VOWIFI
    rrmCleanup(pMac);
#endif
#if defined WLAN_FEATURE_VOWIFI_11R
    limFTCleanup(pMac);
#endif

} /*                      */


/*                                                               
          
                                                   
                            
                                          
                      
                                                               */

tSirRetStatus peOpen(tpAniSirGlobal pMac, tMacOpenParameters *pMacOpenParam)
{
    pMac->lim.maxBssId = pMacOpenParam->maxBssId;
    pMac->lim.maxStation = pMacOpenParam->maxStation;

    if ((pMac->lim.maxBssId == 0) || (pMac->lim.maxStation == 0))
    {
         PELOGE(limLog(pMac, LOGE, FL("max number of Bssid or Stations cannot be zero!"));)
         return eSIR_FAILURE;
    }

    pMac->lim.limTimers.gpLimCnfWaitTimer = vos_mem_malloc(sizeof(TX_TIMER) * pMac->lim.maxStation);
    if (NULL == pMac->lim.limTimers.gpLimCnfWaitTimer)
    {
        PELOGE(limLog(pMac, LOGE, FL("memory allocate failed!"));)
        return eSIR_FAILURE;
    }

#if 0
    pMac->lim.gpLimAIDpool = vos_mem_malloc(sizeof(*pMac->lim.gpLimAIDpool) * (WNI_CFG_ASSOC_STA_LIMIT_STAMAX+1));
    if (NULL == pMac->lim.gpLimAIDpool)
    {
        PELOGE(limLog(pMac, LOGE, FL("memory allocate failed!"));)
        return eSIR_FAILURE;
    }
#endif
    pMac->lim.gpSession = vos_mem_malloc(sizeof(tPESession)* pMac->lim.maxBssId);
    if (NULL == pMac->lim.gpSession)
    {
        limLog(pMac, LOGE, FL("memory allocate failed!"));
        return eSIR_FAILURE;
    }
 
    vos_mem_set(pMac->lim.gpSession, sizeof(tPESession)*pMac->lim.maxBssId, 0);


 /*
                                                                                                   
                                                  
     
                                                                  
                            
     

                                                                                                     
                                                     
     
                                                                  
                            
     
    */
    pMac->pmm.gPmmTim.pTim = vos_mem_malloc(sizeof(tANI_U8)*pMac->lim.maxStation);
    if (NULL == pMac->pmm.gPmmTim.pTim)
    {
        PELOGE(limLog(pMac, LOGE, FL("memory allocate failed for pTim!"));)
        return eSIR_FAILURE;
    }
    vos_mem_set(pMac->pmm.gPmmTim.pTim, sizeof(tANI_U8)*pMac->lim.maxStation, 0);

    pMac->lim.mgmtFrameSessionId = 0xff;
    pMac->lim.deferredMsgCnt = 0;

    if( !VOS_IS_STATUS_SUCCESS( vos_lock_init( &pMac->lim.lkPeGlobalLock ) ) )
    {
        PELOGE(limLog(pMac, LOGE, FL("pe lock init failed!"));)
        return eSIR_FAILURE;
    }
    pMac->lim.deauthMsgCnt = 0;
    return eSIR_SUCCESS;
}

/*                                                               
           
                                                     
                            
                      
                                                               */

tSirRetStatus peClose(tpAniSirGlobal pMac)
{
    tANI_U8 i;

    if (ANI_DRIVER_TYPE(pMac) == eDRIVER_TYPE_MFG)
        return eSIR_SUCCESS;
    
    for(i =0; i < pMac->lim.maxBssId; i++)
    {
        if(pMac->lim.gpSession[i].valid == TRUE)
        {
            peDeleteSession(pMac,&pMac->lim.gpSession[i]);
        }
    }
    vos_mem_free(pMac->lim.limTimers.gpLimCnfWaitTimer);
    pMac->lim.limTimers.gpLimCnfWaitTimer = NULL;
#if 0
    vos_mem_free(pMac->lim.gpLimAIDpool);
    pMac->lim.gpLimAIDpool = NULL;
#endif
    
    vos_mem_free(pMac->lim.gpSession);
    pMac->lim.gpSession = NULL;
    /*
                                                    
                                             
                                                       
                                                
    */
    vos_mem_free(pMac->pmm.gPmmTim.pTim);
    pMac->pmm.gPmmTim.pTim = NULL;
    if( !VOS_IS_STATUS_SUCCESS( vos_lock_destroy( &pMac->lim.lkPeGlobalLock ) ) )
    {
        return eSIR_FAILURE;
    }
    return eSIR_SUCCESS;
}

/*                                                               
           
                                                     
                            
            
                                                               */

tSirRetStatus peStart(tpAniSirGlobal pMac)
{
    tSirRetStatus status = eSIR_SUCCESS;

    status = limInitialize(pMac);
#if defined(ANI_LOGDUMP)
    limDumpInit(pMac);
#endif //                        

    return status;
}

/*                                                               
          
                                                   
                            
            
                                                               */

void peStop(tpAniSirGlobal pMac)
{
    limCleanup(pMac);
    SET_LIM_MLM_STATE(pMac, eLIM_MLM_OFFLINE_STATE);
    return;
}

/*                                                               
             
                                                                
                                                          
                                                              
                                                     
                            
                            
            
                                                                 */
v_VOID_t peFreeMsg( tpAniSirGlobal pMac, tSirMsgQ* pMsg)
{  
    if (pMsg != NULL)
    {
        if (NULL != pMsg->bodyptr)
        {
            if (SIR_BB_XPORT_MGMT_MSG == pMsg->type)
            {
                vos_pkt_return_packet((vos_pkt_t *)pMsg->bodyptr);
            }
            else
            {
                vos_mem_free((v_VOID_t*)pMsg->bodyptr);
            }
        }
        pMsg->bodyptr = 0;
        pMsg->bodyval = 0;
        pMsg->type = 0;
    }
    return;
}


/* 
                                                              
                                    
 */
tANI_U8 limIsTimerAllowedInPowerSaveState(tpAniSirGlobal pMac, tSirMsgQ *pMsg)
{
    tANI_U8 retStatus = TRUE;

    if(!limIsSystemInActiveState(pMac))
    {
        switch(pMsg->type)
        {
            /*                                                  */
            case SIR_LIM_MIN_CHANNEL_TIMEOUT:
            case SIR_LIM_MAX_CHANNEL_TIMEOUT:
            case SIR_LIM_PERIODIC_PROBE_REQ_TIMEOUT:
                retStatus = FALSE;
                break;
            /*                                                  */
            case SIR_LIM_HASH_MISS_THRES_TIMEOUT:

            /*                                                         
                                                                            
                                                                          
                                  
             */
            case SIR_LIM_QUIET_TIMEOUT:

            /*                                                         
                                                                            
                                                                          
                                  
             */
            case SIR_LIM_QUIET_BSS_TIMEOUT:

            /*                                                          
                                             
             */
            case SIR_LIM_CHANNEL_SCAN_TIMEOUT:

            /*                                                                   
                                                                                
                                                                               
                                                                        
             */
            case SIR_LIM_HEART_BEAT_TIMEOUT:
            case SIR_LIM_PROBE_HB_FAILURE_TIMEOUT:

            /*                                                                   
                                                                            
             */
            case SIR_LIM_REASSOC_FAIL_TIMEOUT:
            case SIR_LIM_JOIN_FAIL_TIMEOUT:
            case SIR_LIM_PERIODIC_JOIN_PROBE_REQ_TIMEOUT:
            case SIR_LIM_ASSOC_FAIL_TIMEOUT:
            case SIR_LIM_AUTH_FAIL_TIMEOUT:
            case SIR_LIM_ADDTS_RSP_TIMEOUT:
                retStatus = TRUE;
                break;

            /*                                   */
            default:
                retStatus = TRUE;
                break;


        }
    }

    return retStatus;

}



/* 
                  
  
           
                                                            
                                            
  
        
     
  
              
     
  
       
     
  
                                                 
                                                  
               
 */

tANI_U32
limPostMsgApi(tpAniSirGlobal pMac, tSirMsgQ *pMsg)
{
    return  vos_mq_post_message(VOS_MQ_ID_PE, (vos_msg_t *) pMsg);


} /*                         */


/*                                                                          

                                                                               

                                                                              
                                                                                    
                           

                                               
                                                

                                             

                                                                            */

tSirRetStatus pePostMsgApi(tpAniSirGlobal pMac, tSirMsgQ *pMsg)
{
   return (tSirRetStatus)limPostMsgApi(pMac, pMsg);
}

/*                                                                          

                                                       

                                                        

                                               
                                                

                                             

                                                                            */

tSirRetStatus peProcessMessages(tpAniSirGlobal pMac, tSirMsgQ* pMsg)
{
   if(pMac->gDriverType == eDRIVER_TYPE_MFG)
   {
      return eSIR_SUCCESS;
   }
   /* 
                                                                                   
                                              
    */
    if ( SIR_CFG_PARAM_UPDATE_IND != pMsg->type && IS_CFG_MSG(pMsg->type))
        cfgProcessMbMsg(pMac, (tSirMbMsg*)pMsg->bodyptr);
    else
        limMessageProcessor(pMac, pMsg);
    return eSIR_SUCCESS;
}



//                                                                            
/* 
                    
  
            
                                           
  
         
  
                                                                    
  
        
  
                                      
                          
               
 */

VOS_STATUS peHandleMgmtFrame( v_PVOID_t pvosGCtx, v_PVOID_t vosBuff)
{
    tpAniSirGlobal  pMac;
    tpSirMacMgmtHdr mHdr;
    tSirMsgQ        msg;
    vos_pkt_t      *pVosPkt;
    VOS_STATUS      vosStatus;
    v_U8_t         *pRxPacketInfo;

    pVosPkt = (vos_pkt_t *)vosBuff;
    if (NULL == pVosPkt)
    {
        return VOS_STATUS_E_FAILURE;
    }

    pMac = (tpAniSirGlobal)vos_get_context(VOS_MODULE_ID_PE, pvosGCtx);
    if (NULL == pMac)
    {
        //                                          
        vos_pkt_return_packet(pVosPkt);
        return VOS_STATUS_E_FAILURE;
    }

    vosStatus = WDA_DS_PeekRxPacketInfo( pVosPkt, (void *)&pRxPacketInfo, VOS_FALSE );

    if(!VOS_IS_STATUS_SUCCESS(vosStatus))
    {
        vos_pkt_return_packet(pVosPkt);
        return VOS_STATUS_E_FAILURE;
    }


    //
    //                                                         
    //                                         
    //
    mHdr = WDA_GET_RX_MAC_HEADER(pRxPacketInfo);
    if(mHdr->fc.type == SIR_MAC_MGMT_FRAME) 
    {
    PELOG1(limLog( pMac, LOG1,
       FL ( "RxBd=%p mHdr=%p Type: %d Subtype: %d  Sizes:FC%d Mgmt%d"),
       pRxPacketInfo, mHdr, mHdr->fc.type, mHdr->fc.subType, sizeof(tSirMacFrameCtl), sizeof(tSirMacMgmtHdr) );)

    MTRACE(macTrace(pMac, TRACE_CODE_RX_MGMT, NO_SESSION, 
                        LIM_TRACE_MAKE_RXMGMT(mHdr->fc.subType,  
                        (tANI_U16) (((tANI_U16) (mHdr->seqControl.seqNumHi << 4)) | mHdr->seqControl.seqNumLo)));)
    }


    //                                                
    msg.type = SIR_BB_XPORT_MGMT_MSG;
    msg.bodyptr = vosBuff;
    msg.bodyval = 0;

    if( eSIR_SUCCESS != sysBbtProcessMessageCore( pMac,
                                                  &msg,
                                                  mHdr->fc.type,
                                                  mHdr->fc.subType ))
    {
        vos_pkt_return_packet(pVosPkt);
        limLog( pMac, LOGW,
                FL ( "sysBbtProcessMessageCore failed to process SIR_BB_XPORT_MGMT_MSG" ));
        return VOS_STATUS_E_FAILURE;
    }

    return  VOS_STATUS_SUCCESS;
}

//                                                                            
/* 
                     
  
            
                                                                        
  
         
  
               
  
        
  
               
 */

void peRegisterTLHandle(tpAniSirGlobal pMac)
{
    v_PVOID_t pvosGCTx;
    VOS_STATUS retStatus;

    pvosGCTx = vos_get_global_context(VOS_MODULE_ID_PE, (v_VOID_t *) pMac);

    retStatus = WLANTL_RegisterMgmtFrmClient(pvosGCTx, peHandleMgmtFrame);

    if (retStatus != VOS_STATUS_SUCCESS)
        limLog( pMac, LOGP, FL("Registering the PE Handle with TL has failed bailing out..."));

}


/* 
                           
  
           
                                                             
                                             
  
        
     
  
              
     
  
       
  
                                                  
                                                
                                                    
 */

tANI_U8
limIsSystemInScanState(tpAniSirGlobal pMac)
{
    switch (pMac->lim.gLimSmeState)
    {
        case eLIM_SME_CHANNEL_SCAN_STATE:
        case eLIM_SME_NORMAL_CHANNEL_SCAN_STATE:
        case eLIM_SME_LINK_EST_WT_SCAN_STATE:
        case eLIM_SME_WT_SCAN_STATE:
            //                        
            return true;

        default:
            //                            
            return false;
    }
} /*                                  */



/* 
                             
  
           
                                                             
                                                
  
        
     
  
              
     
  
       
  
                                                  
                                            
                                                
 */

tANI_U8 limIsSystemInActiveState(tpAniSirGlobal pMac)
{
    switch (pMac->pmm.gPmmState)
    {
        case ePMM_STATE_BMPS_WAKEUP:
        case ePMM_STATE_IMPS_WAKEUP:
        case ePMM_STATE_READY:
            //                         
            return true;
        default:
            return false;
          //                             
    }
}





/*  
                               
  
                                                    
                                                       
                                                     
             
    
                                   
                                                                    
               
*/


void
limReceivedHBHandler(tpAniSirGlobal pMac, tANI_U8 channelId, tpPESession psessionEntry)
{
    if((channelId == 0 ) || (channelId == psessionEntry->currentOperChannel) )
    psessionEntry->LimRxedBeaconCntDuringHB++;

    pMac->pmm.inMissedBeaconScenario = FALSE;
} /*                                */



#if 0
void limResetHBPktCount(tpPESession psessionEntry)
{
    psessionEntry->LimRxedBeaconCntDuringHB = 0;
}
#endif


/*
                      
  
           
                                                      
  
         
                                                    
                                          
  
        
  
              
     
  
       
  
  
          
  
 */

void limProcessWdsInfo(tpAniSirGlobal pMac,
                       tSirPropIEStruct propIEInfo)
{
}



/* 
                         
  
           
                                           
                                                     
                              
  
        
  
              
  
       
  
                                                    
               
 */

void
limInitWdsInfoParams(tpAniSirGlobal pMac)
{
    pMac->lim.gLimWdsInfo.wdsLength = 0;
    pMac->lim.gLimNumWdsInfoInd     = 0;
    pMac->lim.gLimNumWdsInfoSet     = 0;
} /*                            */


/*                                                               
                            
                                                     
                                  
                             
                                         
                 
                                                               */
void
limUpdateOverlapStaParam(tpAniSirGlobal pMac, tSirMacAddr bssId, tpLimProtStaParams pStaParams)
{
    int i;
    if (!pStaParams->numSta)
    {
        vos_mem_copy(pMac->lim.protStaOverlapCache[0].addr,
                     bssId,
                     sizeof(tSirMacAddr));
        pMac->lim.protStaOverlapCache[0].active = true;

        pStaParams->numSta = 1;

        return;
    }

    for (i=0; i<LIM_PROT_STA_OVERLAP_CACHE_SIZE; i++)
    {
        if (pMac->lim.protStaOverlapCache[i].active)
        {
            if (vos_mem_compare( pMac->lim.protStaOverlapCache[i].addr,
                          bssId,
                          sizeof(tSirMacAddr))) {
                return; }
        }
        else
            break;
    }

    if (i == LIM_PROT_STA_OVERLAP_CACHE_SIZE)
    {
        PELOG1(limLog(pMac, LOG1, FL("Overlap cache is full"));)
    }
    else
    {
        vos_mem_copy(pMac->lim.protStaOverlapCache[i].addr,
                     bssId,
                     sizeof(tSirMacAddr));
        pMac->lim.protStaOverlapCache[i].active = true;

        pStaParams->numSta++;
    }
}


/* 
                            
  
           
                                                               
                                
  
        
  
              
  
       
  
                                                    
                                                  
                                                              
  
                                                                    
 */

tSirRetStatus
limHandleIBSScoalescing(
    tpAniSirGlobal      pMac,
    tpSchBeaconStruct   pBeacon,
    tANI_U8            *pRxPacketInfo,tpPESession psessionEntry)
{
    tpSirMacMgmtHdr pHdr;
    tSirRetStatus   retCode;

    pHdr = WDA_GET_RX_MAC_HEADER(pRxPacketInfo);
    if ( (!pBeacon->capabilityInfo.ibss) || (limCmpSSid(pMac, &pBeacon->ssId,psessionEntry) != true) )
        /*                                                               */
        retCode =  eSIR_LIM_IGNORE_BEACON;
    else
    {
        tANI_U32 ieLen;
        tANI_U16 tsfLater;
        tANI_U8 *pIEs;
        ieLen    = WDA_GET_RX_PAYLOAD_LEN(pRxPacketInfo);
        tsfLater = WDA_GET_RX_TSF_LATER(pRxPacketInfo);
        pIEs = WDA_GET_RX_MPDU_DATA(pRxPacketInfo);
        PELOG3(limLog(pMac, LOG3, FL("BEFORE Coalescing tsfLater val :%d"), tsfLater);)
        retCode  = limIbssCoalesce(pMac, pHdr, pBeacon, pIEs, ieLen, tsfLater,psessionEntry);
    }
    return retCode;
} /*                                   */



/* 
                                    
  
           
                                                  
                                               
                                                   
                                                  
                                        
  
        
  
              
  
       
                                                     
                                      
  
                                                    
                                                       
               
 */

void
limDetectChangeInApCapabilities(tpAniSirGlobal pMac,
                                tpSirProbeRespBeacon pBeacon,
                                tpPESession psessionEntry)
{
    tANI_U8                 len;
    tSirSmeApNewCaps   apNewCaps;
    tANI_U8            newChannel;
    tSirRetStatus status = eSIR_SUCCESS;
    apNewCaps.capabilityInfo = limGetU16((tANI_U8 *) &pBeacon->capabilityInfo);
    newChannel = (tANI_U8) pBeacon->channelNumber;

    if ( ( false == psessionEntry->limSentCapsChangeNtf ) &&
        ( ( ( limIsNullSsid(&pBeacon->ssId) ) ||
          ( ( !limIsNullSsid(&pBeacon->ssId) ) &&
             ( false == limCmpSSid(pMac, &pBeacon->ssId, psessionEntry) ) ) ) ||
          ( (SIR_MAC_GET_ESS(apNewCaps.capabilityInfo) !=
             SIR_MAC_GET_ESS(psessionEntry->limCurrentBssCaps) ) ||
          ( SIR_MAC_GET_PRIVACY(apNewCaps.capabilityInfo) !=
            SIR_MAC_GET_PRIVACY(psessionEntry->limCurrentBssCaps) ) ||
          ( SIR_MAC_GET_SHORT_PREAMBLE(apNewCaps.capabilityInfo) !=
            SIR_MAC_GET_SHORT_PREAMBLE(psessionEntry->limCurrentBssCaps) ) ||
          ( SIR_MAC_GET_QOS(apNewCaps.capabilityInfo) !=
            SIR_MAC_GET_QOS(psessionEntry->limCurrentBssCaps) ) ||
          ( newChannel !=  psessionEntry->currentOperChannel )
          ) ) )
    {
        if( false == psessionEntry->fWaitForProbeRsp )
        {
            /*                                                                    
                                                                            
                                        */
            if ( true == psessionEntry->fIgnoreCapsChange )
            {
                limLog(pMac, LOGW, FL("Ignoring the Capability change as it is false alarm"));
                return;
            }
            psessionEntry->fWaitForProbeRsp = true;
            limLog(pMac, LOGW, FL("AP capabilities are not matching,"
                   "sending directed probe request.. "));
            status = limSendProbeReqMgmtFrame(pMac, &psessionEntry->ssId, psessionEntry->bssId,
                    psessionEntry->currentOperChannel,psessionEntry->selfMacAddr,
                    psessionEntry->dot11mode, 0, NULL);

            if ( eSIR_SUCCESS != status )
            {
               limLog(pMac, LOGE, FL("send ProbeReq failed"));
               psessionEntry->fWaitForProbeRsp = false;
            }
            return;
        }
        /* 
                                         
                          
         */
        len = sizeof(tSirMacCapabilityInfo) +
              sizeof(tSirMacAddr) + sizeof(tANI_U8) +
              3 * sizeof(tANI_U8) + //                
              pBeacon->ssId.length + 1;

        vos_mem_copy(apNewCaps.bssId,
                     psessionEntry->bssId,
                     sizeof(tSirMacAddr));
        if (newChannel != psessionEntry->currentOperChannel)
        {
            PELOGE(limLog(pMac, LOGE, FL("Channel Change from %d --> %d  - "
                                         "Ignoring beacon!"),
                          psessionEntry->currentOperChannel, newChannel);)
            return;
        }

       /* 
                                                                          
                                                                            
                                                                             
                                                                    
                                                    
                                                                        
                                                                            
                             
        */
        else if ((SIR_MAC_GET_PRIVACY(apNewCaps.capabilityInfo) == 0) &&
                (pBeacon->rsnPresent || pBeacon->wpaPresent))
        {
            PELOGE(limLog(pMac, LOGE, FL("BSS Caps (Privacy) bit 0 in beacon,"
                                         " but WPA or RSN IE present, Ignore Beacon!"));)
            return;
        }
        else
            apNewCaps.channelId = psessionEntry->currentOperChannel;
        vos_mem_copy((tANI_U8 *) &apNewCaps.ssId,
                     (tANI_U8 *) &pBeacon->ssId,
                      pBeacon->ssId.length + 1);

        psessionEntry->fIgnoreCapsChange = false;
        psessionEntry->fWaitForProbeRsp = false;
        psessionEntry->limSentCapsChangeNtf = true;
        limSendSmeWmStatusChangeNtf(pMac, eSIR_SME_AP_CAPS_CHANGED,
                                    (tANI_U32 *) &apNewCaps,
                                    len, psessionEntry->smeSessionId);
    }
    else if ( true == psessionEntry->fWaitForProbeRsp )
    {
        /*                                                                     
                                                                                 
                                                        */

        limLog(pMac, LOG1, FL("capabilities in probe response are"
                    "matching with the current setting,"
                    "Ignoring subsequent capability"
                    "mismatch"));
        psessionEntry->fIgnoreCapsChange = true;
        psessionEntry->fWaitForProbeRsp = false;
     }

} /*                                       */




//                                                                      
/* 
                     
  
            
                            
  
         
  
               
  
        
  
                                                         
               
 */

tSirRetStatus limUpdateShortSlot(tpAniSirGlobal pMac, tpSirProbeRespBeacon pBeacon, tpUpdateBeaconParams pBeaconParams,tpPESession psessionEntry)
{

    tSirSmeApNewCaps   apNewCaps;
    tANI_U32           nShortSlot;
    tANI_U32 val = 0;
    tANI_U32 phyMode;

    //                                                      
    if (wlan_cfgGetInt(pMac, WNI_CFG_11G_SHORT_SLOT_TIME_ENABLED, &val)
                   != eSIR_SUCCESS)
    {
        limLog(pMac, LOGP,
               FL("cfg get WNI_CFG_11G_SHORT_SLOT_TIME failed"));
        return eSIR_FAILURE;
    }
    if (val == false)
        return eSIR_SUCCESS;

    //                                                                                                                        
    limGetPhyMode(pMac, &phyMode, psessionEntry);
    if ((phyMode == WNI_CFG_PHY_MODE_11A) || (phyMode == WNI_CFG_PHY_MODE_11B))
        return eSIR_SUCCESS;

    apNewCaps.capabilityInfo = limGetU16((tANI_U8 *) &pBeacon->capabilityInfo);

    //                                                                                                
    //                                                                               
    //                                                           

    //                                                                                                                 

    //                                                                                       
    //                                                                                 
    /*
                                                                                                 
                                                                                                                                                                
                                                                                                         
                                                                                                         
                                                                                                         
                                                                                                         
                                                                                                         
                                                                                                         
                                                                                                         
    */
    nShortSlot = SIR_MAC_GET_SHORT_SLOT_TIME(apNewCaps.capabilityInfo);

    if (nShortSlot != psessionEntry->shortSlotTimeSupported)
    {
        //                                                           
        PELOG1(limLog(pMac, LOG1, FL("Shortslot capability of AP changed: %d"),  nShortSlot);)
        ((tpSirMacCapabilityInfo)&psessionEntry->limCurrentBssCaps)->shortSlotTime = (tANI_U16)nShortSlot;
        psessionEntry->shortSlotTimeSupported = nShortSlot;
        pBeaconParams->fShortSlotTime = (tANI_U8) nShortSlot;
        pBeaconParams->paramChangeBitmap |= PARAM_SHORT_SLOT_TIME_CHANGED;
    }
    return eSIR_SUCCESS;
}







/*                                                                   
                                                            
 
                                                             
                                                

                                    

           

     
                                                                    */
void limHandleLowRssiInd(tpAniSirGlobal pMac)
{
#if 0  //                                                     
    if ( (pMac->pmm.gPmmState == ePMM_STATE_BMPS_SLEEP) ||
         (pMac->pmm.gPmmState == ePMM_STATE_UAPSD_SLEEP)||
         (pMac->pmm.gPmmState == ePMM_STATE_WOWLAN) )
    {
        PELOG1(limLog(pMac, LOG1, FL("Sending LOW_RSSI_IND to SME "));)
        limSendSmeRsp(pMac, eWNI_SME_LOW_RSSI_IND, eSIR_SME_SUCCESS, 0, 0);
    }
    else
    {
        limLog(pMac, LOGE,
            FL("Received SIR_HAL_LOW_RSSI_IND while in incorrect state: %d"),
            pMac->pmm.gPmmState);
    }
    return;
#endif
}


/*                                                                   
                                                                  
 
                                                                      
                                                                      
                                                     
  
                                                                       
                                                               
                                                                          
                                                                         
                                                                 
   
                                    
                
     
                                                                    */
void limHandleBmpsStatusInd(tpAniSirGlobal pMac)
{
    switch(pMac->pmm.gPmmState)
    {
        case ePMM_STATE_BMPS_SLEEP:
        case ePMM_STATE_UAPSD_WT_SLEEP_RSP:
        case ePMM_STATE_UAPSD_SLEEP:
        case ePMM_STATE_UAPSD_WT_WAKEUP_RSP:
        case ePMM_STATE_WOWLAN:
            PELOG1(limLog(pMac, LOG1, FL("Sending EXIT_BMPS_IND to SME "));)
            limSendExitBmpsInd(pMac, eSME_BMPS_STATUS_IND_RCVD);
            break;

        default:
            limLog(pMac, LOGE,
                FL("Received SIR_HAL_BMPS_STATUS_IND while in incorrect state: %d"),
                pMac->pmm.gPmmState);
            break;
    }
    return;
}


/*                                                                   
                                                                      
 
                                                                       
                                                                      
                                                        
  
                                    
                 
     
                                                                    */
void limHandleMissedBeaconInd(tpAniSirGlobal pMac, tpSirMsgQ pMsg)
{
#ifdef WLAN_ACTIVEMODE_OFFLOAD_FEATURE
    tpSirSmeMissedBeaconInd  pSirMissedBeaconInd =
                           (tpSirSmeMissedBeaconInd)pMsg->bodyptr;
    tpPESession psessionEntry = peFindSessionByBssIdx(pMac,pSirMissedBeaconInd->bssIdx);

#ifdef CUSTOMER_LGE_DEBUG_LOG
    limLog(pMac, LOGE, FL("Beacon Miss Event:: PMM Status = %d, BSS Index:%d"), pMac->pmm.gPmmState, pSirMissedBeaconInd->bssIdx);
#endif

    if (psessionEntry == NULL)
    {
         limLog(pMac, LOGE,
               FL("session does not exist for given BSSIdx:%d"),
               pSirMissedBeaconInd->bssIdx);
         return;
    }
#endif
    if ( (pMac->pmm.gPmmState == ePMM_STATE_BMPS_SLEEP) ||
         (pMac->pmm.gPmmState == ePMM_STATE_UAPSD_SLEEP)||
         (pMac->pmm.gPmmState == ePMM_STATE_WOWLAN) )
    {
        pMac->pmm.inMissedBeaconScenario = TRUE;
        PELOGE(limLog(pMac, LOGE,
              FL("Sending EXIT_BMPS_IND to SME due to Missed beacon from FW"));)
        limSendExitBmpsInd(pMac, eSME_MISSED_BEACON_IND_RCVD);
    }
/*                        */
#ifdef WLAN_ACTIVEMODE_OFFLOAD_FEATURE
    else if(((pMac->pmm.gPmmState == ePMM_STATE_READY) ||
                     (pMac->pmm.gPmmState == ePMM_STATE_BMPS_WAKEUP)) &&
                     (IS_ACTIVEMODE_OFFLOAD_FEATURE_ENABLE))
    {
        pMac->pmm.inMissedBeaconScenario = TRUE;
        PELOGE(limLog(pMac, LOGE, FL("Received Heart Beat Failure"));)
        limMissedBeaconInActiveMode(pMac, psessionEntry);
    }
#endif
    else
    {
        limLog(pMac, LOGE,
            FL("Received SIR_HAL_MISSED_BEACON_IND while in incorrect state: %d"),
            pMac->pmm.gPmmState);
    }
    return;
}

/*                                                                   
                                                             
 
                                                                     

                                    
                 
     
                                                                    */
void limMicFailureInd(tpAniSirGlobal pMac, tpSirMsgQ pMsg)
{
    tpSirSmeMicFailureInd pSirSmeMicFailureInd;
    tpSirSmeMicFailureInd pSirMicFailureInd = (tpSirSmeMicFailureInd)pMsg->bodyptr;
    tSirMsgQ            mmhMsg;
    tpPESession psessionEntry ;
    tANI_U8     sessionId;

    if((psessionEntry = peFindSessionByBssid(pMac,pSirMicFailureInd->bssId,&sessionId))== NULL)
    {
         limLog(pMac, LOGE,
               FL("session does not exist for given BSSId"));
         return;
    }

    pSirSmeMicFailureInd = vos_mem_malloc(sizeof(tSirSmeMicFailureInd));
    if (NULL == pSirSmeMicFailureInd)
    {
        //          
       limLog(pMac, LOGP,
               FL("memory allocate failed for eWNI_SME_MIC_FAILURE_IND"));
       return;
    }

    pSirSmeMicFailureInd->messageType = eWNI_SME_MIC_FAILURE_IND;
    pSirSmeMicFailureInd->length = sizeof(pSirSmeMicFailureInd);
    pSirSmeMicFailureInd->sessionId = psessionEntry->smeSessionId;

    vos_mem_copy(pSirSmeMicFailureInd->bssId,
                 pSirMicFailureInd->bssId,
                 sizeof(tSirMacAddr));

    vos_mem_copy(pSirSmeMicFailureInd->info.srcMacAddr,
                 pSirMicFailureInd->info.srcMacAddr,
                 sizeof(tSirMacAddr));

    vos_mem_copy(pSirSmeMicFailureInd->info.taMacAddr,
                 pSirMicFailureInd->info.taMacAddr,
                 sizeof(tSirMacAddr));

    vos_mem_copy(pSirSmeMicFailureInd->info.dstMacAddr,
                 pSirMicFailureInd->info.dstMacAddr,
                 sizeof(tSirMacAddr));

    vos_mem_copy(pSirSmeMicFailureInd->info.rxMacAddr,
                 pSirMicFailureInd->info.rxMacAddr,
                 sizeof(tSirMacAddr));

    pSirSmeMicFailureInd->info.multicast = 
                                   pSirMicFailureInd->info.multicast;

    pSirSmeMicFailureInd->info.keyId= 
                                  pSirMicFailureInd->info.keyId;

    pSirSmeMicFailureInd->info.IV1= 
                                  pSirMicFailureInd->info.IV1;

    vos_mem_copy(pSirSmeMicFailureInd->info.TSC,
                 pSirMicFailureInd->info.TSC,SIR_CIPHER_SEQ_CTR_SIZE);

    mmhMsg.type = eWNI_SME_MIC_FAILURE_IND;
    mmhMsg.bodyptr = pSirSmeMicFailureInd;
    mmhMsg.bodyval = 0;
    MTRACE(macTraceMsgTx(pMac, sessionId, mmhMsg.type));
    limSysProcessMmhMsgApi(pMac, &mmhMsg, ePROT);
    return;
}


/*                                                                   
                                                                              

                                                                                        
                                                                                   
                                                   
  
                                                                       
                                                                        
                                                        
                                                                                 
  
                                    
                 
     
                                                                    */

tMgmtFrmDropReason limIsPktCandidateForDrop(tpAniSirGlobal pMac, tANI_U8 *pRxPacketInfo, tANI_U32 subType)
{
    tANI_U32                     framelen;
    tANI_U8                      *pBody;
    tSirMacCapabilityInfo     capabilityInfo;

    /*
      
                                                                                        
                                                                                         
                                                                              
                                                          
    */
    if( (subType == SIR_MAC_MGMT_BEACON) ||
        (subType == SIR_MAC_MGMT_PROBE_RSP))
    {
        if(pMac->pmm.inMissedBeaconScenario)
        {
            MTRACE(macTrace(pMac, TRACE_CODE_INFO_LOG, 0, eLOG_NODROP_MISSED_BEACON_SCENARIO));
            return eMGMT_DROP_NO_DROP;
        }
        if (limIsSystemInScanState(pMac))
        {
            return eMGMT_DROP_NO_DROP;
        }
#ifdef WLAN_FEATURE_ROAM_SCAN_OFFLOAD
        else if (WDA_GET_OFFLOADSCANLEARN(pRxPacketInfo) || WDA_GET_ROAMCANDIDATEIND(pRxPacketInfo))
        {
            return eMGMT_DROP_NO_DROP;
        }
#endif
        else if (WDA_IS_RX_IN_SCAN(pRxPacketInfo))
        {
            return eMGMT_DROP_SCAN_MODE_FRAME;
        }
    }

    framelen = WDA_GET_RX_PAYLOAD_LEN(pRxPacketInfo);
    pBody    = WDA_GET_RX_MPDU_DATA(pRxPacketInfo);

   /*                                                                                          
                                               
    */
        *((tANI_U16*) &capabilityInfo) = sirReadU16(pBody+ LIM_BCN_PR_CAPABILITY_OFFSET);
    if(!capabilityInfo.ibss)
        return eMGMT_DROP_NO_DROP;
#if 0
    //                                                           
    if (pMac->lim.gLimSystemRole != eLIM_STA_IN_IBSS_ROLE)
        return eMGMT_DROP_NO_DROP;
#endif

    //                                                   
    if( (subType == SIR_MAC_MGMT_BEACON) ||
        (subType == SIR_MAC_MGMT_PROBE_RSP))
    {
        //                                        
        if(framelen < LIM_MIN_BCN_PR_LENGTH)
            return eMGMT_DROP_INVALID_SIZE;
        
        *((tANI_U16*) &capabilityInfo) = sirReadU16(pBody+ LIM_BCN_PR_CAPABILITY_OFFSET);

        //                                                                               
        if(capabilityInfo.ess)
            return eMGMT_DROP_INFRA_BCN_IN_IBSS;
    }
    else if( (subType == SIR_MAC_MGMT_PROBE_REQ) &&
                (!WDA_GET_RX_BEACON_SENT(pRxPacketInfo)))
    {
        //                                                                            
        //                                                                                
        return eMGMT_DROP_NOT_LAST_IBSS_BCN;
    }

    return eMGMT_DROP_NO_DROP;
}

eHalStatus pe_AcquireGlobalLock( tAniSirLim *psPe)
{
    eHalStatus status = eHAL_STATUS_INVALID_PARAMETER;

    if(psPe)
    {
        if( VOS_IS_STATUS_SUCCESS( vos_lock_acquire( &psPe->lkPeGlobalLock) ) )
        {
            status = eHAL_STATUS_SUCCESS;
        }
    }
    return (status);
}
eHalStatus pe_ReleaseGlobalLock( tAniSirLim *psPe)
{
    eHalStatus status = eHAL_STATUS_INVALID_PARAMETER;
    if(psPe)
    {
        if( VOS_IS_STATUS_SUCCESS( vos_lock_release( &psPe->lkPeGlobalLock) ) )
        {
            status = eHAL_STATUS_SUCCESS;
        }
    }
    return (status);
}

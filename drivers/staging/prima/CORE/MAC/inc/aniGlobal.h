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

#ifndef _ANIGLOBAL_H
#define _ANIGLOBAL_H

//                                                       
//                                 
#if !defined(_HALMAC_WMM_API_H)
typedef struct sAniSirGlobal *tpAniSirGlobal;
#endif

#include "halTypes.h"
#include "sirCommon.h"
#include "aniSystemDefs.h"
#include "sysDef.h"
#include "dphGlobal.h"
#include "limGlobal.h"
#include "pmmGlobal.h"
#include "schGlobal.h"
#include "sysGlobal.h"
#include "cfgGlobal.h"
#include "utilsGlobal.h"
#include "sirApi.h"


#include "wlan_qct_hal.h"

#include "pmc.h"

#include "csrApi.h"
#ifdef WLAN_FEATURE_VOWIFI_11R
#include "sme_FTApi.h"
#endif
#include "csrSupport.h"
#include "smeInternal.h"
#include "ccmApi.h"
#include "btcApi.h"
#include "csrInternal.h"

#ifdef FEATURE_OEM_DATA_SUPPORT
#include "oemDataInternal.h" 
#endif

#if defined WLAN_FEATURE_VOWIFI
#include "smeRrmInternal.h"
#include "rrmGlobal.h"
#endif
#if defined(FEATURE_WLAN_CCX) && !defined(FEATURE_WLAN_CCX_UPLOAD)
#include "ccxApi.h"
#include "ccxGlobal.h"
#endif
#include "p2p_Api.h"

#if defined WLAN_FEATURE_VOWIFI_11R
#include <limFTDefs.h>
#endif


#ifdef ANI_DVT_DEBUG
#include "dvtModule.h"
#endif

//                            
#include "logDump.h"

//                                                                                                
//                                  
#define PMAC_STRUCT( _hHal )  (  (tpAniSirGlobal)_hHal )

#define ANI_DRIVER_TYPE(pMac)     (((tpAniSirGlobal)(pMac))->gDriverType)
//                                                                    
//                                
#define LIM_BSS_CAPS_OFFSET_HCF 0
#define LIM_BSS_CAPS_OFFSET_WME 1
#define LIM_BSS_CAPS_OFFSET_WSM 2

#define LIM_BSS_CAPS_HCF (1 << LIM_BSS_CAPS_OFFSET_HCF)
#define LIM_BSS_CAPS_WME (1 << LIM_BSS_CAPS_OFFSET_WME)
#define LIM_BSS_CAPS_WSM (1 << LIM_BSS_CAPS_OFFSET_WSM)

//                                 
#define LIM_BSS_CAPS_GET(cap, val) (((val) & (LIM_BSS_CAPS_ ## cap)) >> LIM_BSS_CAPS_OFFSET_ ## cap)
#define LIM_BSS_CAPS_SET(cap, val) ((val) |= (LIM_BSS_CAPS_ ## cap ))
#define LIM_BSS_CAPS_CLR(cap, val) ((val) &= (~ (LIM_BSS_CAPS_ ## cap)))

//                                                                        
#define MAX_NO_BEACONS_PER_HEART_BEAT_INTERVAL 41

/*                                                                    */
#define MAX_NUM_LEGACY_BSSID_PER_CHANNEL    10

#define P2P_WILDCARD_SSID "DIRECT-" //                            
#define P2P_WILDCARD_SSID_LEN 7

#ifdef WLAN_FEATURE_CONCURRENT_P2P
#define MAX_NO_OF_P2P_SESSIONS  5
#endif //                           

#define SPACE_ASCII_VALUE  32

#ifdef FEATURE_WLAN_BATCH_SCAN
#define EQUALS_TO_ASCII_VALUE (61)
#endif

//                                                                    
//                              
typedef void (*CHANGE_CHANNEL_CALLBACK)(tpAniSirGlobal pMac, eHalStatus status, tANI_U32 *data,
    tpPESession psessionEntry);

//                        
typedef struct sAniSirLimIbss
{
    void *pHdr;
    void *pBeacon;
} tAniSirLimIbss;

typedef struct sDialogueToken
{
    //         
    tANI_U16 assocId;
    tANI_U8 token;
    tANI_U8 rsvd1;
    //         
    tANI_U16 tid;
    tANI_U8 rsvd2[2];

    struct sDialogueToken* next;
}tDialogueToken, *tpDialogueToken;

typedef struct sLimTimers
{
    //                                                             
    //                                         

//                   
    //                               
    TX_TIMER   gLimBackgroundScanTimer;

    TX_TIMER    gLimPreAuthClnupTimer;
    //                                               

    //                           
    TX_TIMER    gLimAssocFailureTimer;
    TX_TIMER    gLimReassocFailureTimer;


    //                        
    TX_TIMER    gLimHeartBeatTimer;

    //                                                     
    TX_TIMER    gLimProbeAfterHBTimer;


    //                              
    TX_TIMER            gLimAuthFailureTimer;

    //                            
    TX_TIMER              gLimJoinFailureTimer;

    //                
    TX_TIMER    gLimKeepaliveTimer;

    //                    
    TX_TIMER    gLimMinChannelTimer;
    TX_TIMER    gLimMaxChannelTimer;
    TX_TIMER    gLimPeriodicProbeReqTimer;

    //               
    TX_TIMER            *gpLimCnfWaitTimer;

    //                                             
    TX_TIMER            gLimSendDisassocFrameThresholdTimer;

    TX_TIMER       gLimAddtsRspTimer;   //                        

    //                        
    TX_TIMER    gLimUpdateOlbcCacheTimer;

    TX_TIMER           gLimChannelSwitchTimer;
    //                                                      
    //                                                   
    TX_TIMER           gLimQuietTimer;
    //                                                       
    //                
    //                                                    
    //               
    TX_TIMER           gLimQuietBssTimer;

#ifdef WLAN_FEATURE_VOWIFI_11R
    TX_TIMER           gLimFTPreAuthRspTimer;
#endif

#ifdef FEATURE_WLAN_CCX
    TX_TIMER           gLimCcxTsmTimer;
#endif
    TX_TIMER           gLimRemainOnChannelTimer;
#ifdef FEATURE_WLAN_TDLS_INTERNAL
    TX_TIMER           gLimTdlsDisRspWaitTimer;
    TX_TIMER           gLimTdlsLinkSetupRspTimeouTimer;
    TX_TIMER           gLimTdlsLinkSetupCnfTimeoutTimer;
#endif

    TX_TIMER           gLimPeriodicJoinProbeReqTimer;
    TX_TIMER           gLimDisassocAckTimer;
    TX_TIMER           gLimDeauthAckTimer;
    //                                                                                            
    TX_TIMER           gLimP2pSingleShotNoaInsertTimer;
    /*                                                
                                              
                                                       
     */
    TX_TIMER           gLimActiveToPassiveChannelTimer;
//                                                                                        
//                                                             
//                                                                                        

}tLimTimers;

typedef struct {
    void *pMlmDisassocReq;
    void *pMlmDeauthReq;
}tLimDisassocDeauthCnfReq;

typedef struct sAniSirLim
{
    //                                                                                                        

    tLimTimers limTimers;
    //                                                
    tANI_U32   gLimTimersCreated;


    //                                                                                                      

    //                                                                                                             
    /* 
                                                        
                                                       
                                                        
                
     */
    tANI_U32     gLimUseScanModeForLearnMode;

    /* 
                                                
                                                    
     */
    tANI_U32    gLimSystemInScanLearnMode;

    //                            
    tANI_U8               gLimReturnAfterFirstMatch;
    tANI_U8               gLim24Band11dScanDone;
    tANI_U8               gLim50Band11dScanDone;
    tANI_U8               gLimReturnUniqueResults;

    //                                       
    tANI_U32               gLimNumOfBackgroundScanSuccess;
    tANI_U32               gLimNumOfConsecutiveBkgndScanFailure;
    tANI_U32               gLimNumOfForcedBkgndScan;
    tANI_U8                gLimBackgroundScanDisable;      //                 
    tANI_U8                gLimForceBackgroundScanDisable; //                  
    tANI_U8                gLimBackgroundScanTerminate;    //                 
    tANI_U8                gLimReportBackgroundScanResults;//                     

    //                                     
    //               
    tANI_U32   gLimCurrentScanChannelId;

    //                        
    /*                                                                          
                                                                           
     */
    tANI_U16 gDeferMsgTypeForNOA;
    tANI_U32 *gpDefdSmeMsgForNOA;

    tLimMlmScanReq *gpLimMlmScanReq;

    //                                                       
    tANI_U16   gLimMlmScanResultLength;

    //                                                      
    tANI_U16   gLimSmeScanResultLength;

    /* 
                                                     
                                                          
     */
    tLimScanResultNode
           *gLimCachedScanHashTable[LIM_MAX_NUM_OF_SCAN_RESULTS];

    //                                                       
    tANI_U16   gLimMlmLfrScanResultLength;

    //                                                      
    tANI_U16   gLimSmeLfrScanResultLength;

    /* 
                                                         
                                                                    
            
     */
    tLimScanResultNode
        *gLimCachedLfrScanHashTable[LIM_MAX_NUM_OF_SCAN_RESULTS];

    //                                     
    //                                          
    tANI_U32   gLimBackgroundScanChannelId;
    //                                                             
    tANI_U8    gLimBackgroundScanStarted;

    /*                                                                              */
    tANI_U16    gLimRestoreCBNumScanInterval;
    tANI_U16    gLimRestoreCBCount;
    tSirMacAddr gLimLegacyBssidList[MAX_NUM_LEGACY_BSSID_PER_CHANNEL];

    //
    //                   
    //                                                
    //                                           
    //
    //                   
    //                                                   
    //                           
    //                                                   
    //                                 
    //
    tANI_U32 gLimTriggerBackgroundScanDuringQuietBss;


    //                                                  
    tANI_U32 gTotalScanDuration;
    tANI_U32 p2pRemOnChanTimeStamp;

    //                                             
    tANI_U8 abortScan;
    tLimScanChnInfo scanChnInfo;

    //                                                                                                             
    tSirMacAddr         gSelfMacAddr;   //                         

    //                                                                                                        
    //                                     
    //                              

    tANI_U8             gLimCurrentBssUapsd;

    /*                                                        */
    tANI_U8     gLimForceNoPropIE;

    //
    //                                           
    //                      
    //
    //         
    //                                               
    //                     
    //       
    //                                                  
    //

    //
    //                                                
    //                                               
    //                              
    //                                                        
    //                                                   
    //
    void* gpLimMlmSetKeysReq;
    void* gpLimMlmRemoveKeyReq;

    //                                                                              
    //                                                                                           
  //                                                      

    //                                                                                                        

    //                                                                                                           
    //                                       
    //                                       
    //        
    //                                       

    //                                                                                    
    tANI_U8    gLimIbssCoalescingHappened;

    //                                                   
    tLimIbssPeerNode      *gLimIbssPeerList;
    tANI_U32               gLimNumIbssPeers;

    //                                                           
    tAniSirLimIbss      ibssInfo;

    //                                                                                                         

    //                                                                                                                    

    tANI_U16   maxStation;
    tANI_U16   maxBssId;

    tANI_U32    gLimNumBeaconsRcvd;
    tANI_U32    gLimNumBeaconsIgnored;

    tANI_U32    gLimNumDeferredMsgs;

    //                                                               
    tANI_U16  gLimNumOfAniSTAs;      //                   
    tANI_U16  gLimAssocStaLimit;

    //                                                        
   //                                      

    //                          
    tANI_U32   gLimHeartBeatCount;

    //                                                                    
    tANI_U16            gLimHeartBeatBeaconStats[MAX_NO_BEACONS_PER_HEART_BEAT_INTERVAL];

#ifdef WLAN_DEBUG
    //               
    tANI_U32     numTot, numBbt, numProtErr, numLearn, numLearnIgnore;
    tANI_U32     numSme, numMAC[4][16];

    //                                                       
    //                                                             
    tANI_U32    gLimNumAssocReqDropInvldState;
    //                                                                  
    tANI_U32    gLimNumAssocReqDropACRejectTS;
    tANI_U32    gLimNumAssocReqDropACRejectSta;
    //                                                         
    //                                                             
    tANI_U32    gLimNumReassocReqDropInvldState;
    //                                                      
    //                                                       
    tANI_U32    gLimNumHashMissIgnored;

    //                                               
    //                             
    tANI_U32    gLimUnexpBcnCnt;

    //                                               
    //                                                     
    tANI_U32    gLimBcnSSIDMismatchCnt;

    //                                                               
    tANI_U32    gLimNumLinkEsts;

    //                                            
    tANI_U32    gLimNumRxCleanup;

    //                                               
    tANI_U32    gLim11bStaAssocRejectCount;

#endif    

    //                                                                              
    tANI_U64 gLastBeaconTimeStamp;
    //                                                              
    tANI_U32 gCurrentBssBeaconCnt;
    tANI_U8 gLastBeaconDtimCount;
    tANI_U8 gLastBeaconDtimPeriod;


    //                                                                                                                  


    //                                                                                                             
    //                          
    tANI_U8    gLimHBfailureCntInLinkEstState;
    tANI_U8    gLimProbeFailureAfterHBfailedCnt;
    tANI_U8    gLimHBfailureCntInOtherStates;

    /* 
                                                         
                                                                
                                                           
     */
    tANI_U8             gLimRspReqd;

    //                    
    tLimSmeStates       gLimPrevSmeState;

    //                                             
    tLimMlmStates       gLimMlmState;

    //                    
    tLimMlmStates       gLimPrevMlmState;

#ifdef GEN4_SCAN
    //                                                    
    tLimLimHalScanState gLimHalScanState;
//                         
    SUSPEND_RESUME_LINK_CALLBACK gpLimSuspendCallback; 
    tANI_U32 *gpLimSuspendData;
    SUSPEND_RESUME_LINK_CALLBACK gpLimResumeCallback; 
    tANI_U32 *gpLimResumeData;
//                             
    tANI_U8    fScanDisabled;
    //                                                    
    //                                                    
    //                                                              
    tANI_U16    gResumeChannel;
    ePhyChanBondState    gResumePhyCbState;
#endif //          

    //                              
    CHANGE_CHANNEL_CALLBACK gpchangeChannelCallback;
    tANI_U32 *gpchangeChannelData;

    //                                             
    tLimSmeStates         gLimSmeState;
    //                                                     
    tLimSystemRole        gLimSystemRole;

    //                                                  
    tLimNoShortParams         gLimNoShortParams;

    //                                                   
    tLimNoShortSlotParams   gLimNoShortSlotParams;


    //                
    tLimProtStaParams  gLimOverlap11gParams;

    tLimProtStaParams  gLimOverlap11aParams;
    tLimProtStaParams gLimOverlapHt20Params;
    tLimProtStaParams gLimOverlapNonGfParams;

    //
    //                                             
    //                                                                     
    tANI_U32           gLimPhyMode;
    tANI_U32           propRateAdjustPeriod;
    tANI_U32           scanStartTime;    //                          

    //                                
    tANI_U8            gLimMyMacAddr[6];
    tANI_U8            ackPolicy;

    tANI_U8            gLimQosEnabled:1; //   
    tANI_U8            gLimWmeEnabled:1; //   
    tANI_U8            gLimWsmEnabled:1; //   
    tANI_U8            gLimHcfEnabled:1;
    tANI_U8            gLim11dEnabled:1;
    tANI_U8            gLimProbeRespDisableFlag:1; //                            
    //                                             

    //                                                                                                           

    //                                                                                                           

    //         
    tANI_U32            gLimNumWdsInfoInd;
    tANI_U32            gLimNumWdsInfoSet;
    tSirWdsInfo         gLimWdsInfo;

    //                         
    tLimDeferredMsgQParams    gLimDeferredMsgQ;

    //                                                               
    tSirAddtsReq       gLimAddtsReq;
    tANI_U8            gLimAddtsSent;
    tANI_U8            gLimAddtsRspTimerCount;

    //                               
    tCfgProtection    cfgProtection;

    tANI_U8 gLimProtectionControl;
    //                              

    //                                 
    tSirAlternateRadioInfo  gLimAlternateRadio;

    //                                                                                          
    //                                                                                          
    //                                  
    tANI_U8   gLimProcessDefdMsgs;

    //                      
    tANI_U8  gUapsdEnable;          

    /*                                                   
                                                            
                                                          
                                             
     */
    tANI_U8  gUapsdPerAcBitmask;   

    /*                                                   
                                                    
                                                       
                
     */
    tANI_U8  gUapsdPerAcTriggerEnableMask;  

    /*                                        
                                                  
                                                  
                         
     */ 
    tANI_U8  gUapsdPerAcDeliveryEnableMask; 
    
    /*                                                     
                                                        
                                                            
                                                         
     */
    tANI_U8  gAcAdmitMask[SIR_MAC_DIRECTION_DIRECT];

    //                                                             
    tpDialogueToken pDialogueTokenHead;
    tpDialogueToken pDialogueTokenTail;

    tLimTspecInfo tspecInfo[LIM_NUM_TSPEC_MAX];

    //                                     
    tLimAdmitPolicyInfo admitPolicyInfo;
    vos_lock_t lkPeGlobalLock;
    tANI_U8 disableLDPCWithTxbfAP;
#ifdef FEATURE_WLAN_TDLS
    tANI_U8 gLimTDLSBufStaEnabled;
    tANI_U8 gLimTDLSUapsdMask;
#endif



    //                                                                                                         

    //                                                                                                            
    //                                 
    //                              
   // tpSirSmeJoinReq       gpLimJoinReq;

    //                                    
    //                              
    //tpSirSmeReassocReq    gpLimReassocReq;  sep23 review

    //                                        
    //                                        

    //                                                
    //              
    tLimMlmAuthReq     *gpLimMlmAuthReq;

    //                                         
    //                     
    //tLimMlmJoinReq       *gpLimMlmJoinReq;

    //                                                                   
    //                                          
    tANI_U32 channelChangeReasonCode;
    
    //                                              
    tSirMacChanNum        gLimPreAuthChannelNumber;
    tAniAuthType          gLimPreAuthType;
    tSirMacAddr           gLimPreAuthPeerAddr;
    tANI_U32              gLimNumPreAuthContexts;
    tLimPreAuthTable      gLimPreAuthTimerTable;

    //                               
    tANI_U16 gLimDeauthReasonCode;

    //                                              
    struct tLimPreAuthNode *  pLimPreAuthList;

    //                                             
    tANI_U16            gLimDisassocFrameThreshold;
    tANI_U16            gLimDisassocFrameCredit;

    //                                     
    void                *gLimAssocResponseData;

    //                                               
    tCacheParams    protStaOverlapCache[LIM_PROT_STA_OVERLAP_CACHE_SIZE];
    tCacheParams    protStaCache[LIM_PROT_STA_CACHE_SIZE];

    //                                                                                                          



    //
    //                   
    //                                        
    //                             
    //
    tANI_U32 gLimScanOverride;
    //                                                    
    tSirScanType gLimScanOverrideSaved;

    //
    //                                               
    //                                                    
    //                           
    //                                                      
    //                                       
    //
    tANI_U32 gLimCBStateProtection;

    //                                          
    tANI_U16 gLimTitanStaCount;

    //
    //                   
    //                                 
    //                                 
        //                   
    //
    tANI_U32 gLimBlockNonTitanSta;
    //                                                                                                


    //                                                                                               
    //
    //                                                   
    //                                              

    //                                                       
    tANI_U8 htCapabilityPresentInBeacon;

    //                                          
    tANI_U8 htCapability;


    tANI_U8 gHTGreenfield;

    tANI_U8 gHTShortGI40Mhz;
    tANI_U8 gHTShortGI20Mhz;

    //                        
    //                        
    tANI_U8 gHTMaxAmsduLength;


    //                                          
    tANI_U8 gHTDsssCckRate40MHzSupport;

    //                                      
    tANI_U8 gHTPSMPSupport;

    //                                                          
    tANI_U8 gHTLsigTXOPProtection;

    //                
    tSirMacHTMIMOPowerSaveState gHTMIMOPSState;

    //                   
    tANI_U8 gScanInPowersave;

    //
    //               
    //                     
    //               
    //               
    //               
    //             
    //             
    //             
    //             
    //
    tANI_U8 gHTAMpduDensity;

    tANI_BOOLEAN gMaxAmsduSizeEnabled;
    //                            
    tANI_U8 gHTMaxRxAMpduFactor;

    //
    //                                                      
    //           
    //            
    //            
    //            
    //            
    //            
    //            
    //            
    //
    tANI_U8 gHTServiceIntervalGranularity;

    //                                                                 
    tANI_U8 gHTControlledAccessOnly;

    //                                                    
    tANI_U8 gHTRifsMode;
   //                                                                             
    tANI_U8  gHTObssMode;

    //                                      
    tSirMacHTOperatingMode gHTOperMode;

    //                                         
    tANI_U8 gHTPCOActive;

    //
    //                                                   
    //                           
    //                           
    //
    tANI_U8 gHTPCOPhase;

    //
    //                                               
    //                   
    //                     
    //
    tANI_U8 gHTSecondaryBeacon;

    //
    //                    
    //                
    //                                
    //
    tANI_U8 gHTDualCTSProtection;

    //
    //                                                    
    //                                     
    //
    tANI_U8 gHTSTBCBasicMCS;

    tANI_U8 gHTNonGFDevicesPresent;

    tANI_U8   gAddBA_Declined;               //                                                                 

    //                                                                                               

#ifdef FEATURE_WLAN_TDLS_INTERNAL
    //                                                                                               
    
    tSirTdlsDisReq gLimTdlsDisReq ; 
    //                                       
    tLimDisResultList *gLimTdlsDisResultList ;
    tANI_U8 gLimTdlsDisStaCount ;
    tANI_U8 gAddStaDisRspWait ;

    tLimTdlsLinkSetupInfo  gLimTdlsLinkSetupInfo;
    
    /*                                                                      */
#ifdef FEATURE_WLAN_TDLS_NEGATIVE
    tANI_U32 gLimTdlsNegativeBehavior;  
#endif
#endif
#ifdef FEATURE_WLAN_TDLS
    tANI_U8 gLimAddStaTdls ;
    tANI_U8 gLimTdlsLinkMode ;
    //                                                                                               
#endif

    //                                     
    tLimWscIeInfo wscIeInfo;
    tpPESession gpSession ;   //                            
    /*
                                                                                       
                                                                    
     */
    tANI_U8   gSmeSessionId;
    tANI_U16 gTransactionId;

#ifdef FEATURE_OEM_DATA_SUPPORT
tLimMlmOemDataReq       *gpLimMlmOemDataReq;
tLimMlmOemDataRsp       *gpLimMlmOemDataRsp;
#endif

    tSirRemainOnChnReq  *gpLimRemainOnChanReq; //                                       
    vos_list_t  gLimMgmtFrameRegistratinQueue;
    tANI_U32    mgmtFrameSessionId;
    tSirBackgroundScanMode gLimBackgroundScanMode;

#if  defined (WLAN_FEATURE_VOWIFI_11R) || defined (FEATURE_WLAN_CCX) || defined(FEATURE_WLAN_LFR)
    tpPESession  pSessionEntry;
    tANI_U8 reAssocRetryAttempt;
#endif
    tLimDisassocDeauthCnfReq limDisassocDeauthCnfReq;
    tANI_U8 deferredMsgCnt;
    tSirDFSChannelList    dfschannelList;
    tANI_U8 deauthMsgCnt;
    tANI_U8 gLimIbssStaLimit;
} tAniSirLim, *tpAniSirLim;

typedef struct sLimMgmtFrameRegistration
{
    vos_list_node_t node;     //                      
    tANI_U16        frameType;
    tANI_U16        matchLen;
    tANI_U16        sessionId;
    tANI_U8         matchData[1];
} tLimMgmtFrameRegistration, *tpLimMgmtFrameRegistration;

#if defined WLAN_FEATURE_VOWIFI
typedef struct sRrmContext
{
  tRrmSMEContext rrmSmeContext;
  tRrmPEContext  rrmPEContext; 
}tRrmContext, *tpRrmContext;
#endif

#if defined WLAN_FEATURE_VOWIFI_11R
typedef struct sFTContext
{
  tftSMEContext ftSmeContext;
  tftPEContext  ftPEContext; 
} tftContext, *tpFTContext;
#endif

//                                                                             
//                                  
/*                                                                              

                        
    
                                                                      
                                        
    
                                                                               */

typedef enum
{
    eDRIVER_TYPE_PRODUCTION  = 0,
    eDRIVER_TYPE_MFG         = 1,
    eDRIVER_TYPE_DVT         = 2
} tDriverType;

/*                                                                              

                               
    
                                                                                              
                        
    
                                                                               */

typedef struct sMacOpenParameters
{
    tANI_U16 maxStation;
    tANI_U16 maxBssId;
    tANI_U32 frameTransRequired;
    tDriverType  driverType;
} tMacOpenParameters;

typedef struct sHalMacStartParameters
{
    //                           
    //                              
    tDriverType  driverType;

} tHalMacStartParameters;

//                                                                    
//                                
typedef struct sAniSirGlobal

{
    tDriverType  gDriverType;

    //                                                               
    //                                                             
    //                                                                
    //                          
//                                                  
    //                  
    void        *pAdapter;   //                                              
                             //                                                  
                             //                                    
    tSirMbMsg*   pResetMsg;
    tAniSirCfg   cfg;
    tAniSirLim   lim;
    tAniSirPmm   pmm;
    tAniSirSch   sch;
    tAniSirSys   sys;
    tAniSirUtils utils;
    //               
    tHddHandle hHdd;

#ifdef ANI_DVT_DEBUG
    tAniSirDvt   dvt;
#endif

    tSmeStruct sme;
    tCsrScanStruct scan;
    tCsrRoamStruct roam;

#ifdef FEATURE_OEM_DATA_SUPPORT
    tOemDataStruct oemData;
#endif
#ifdef FEATURE_WLAN_TDLS_INTERNAL
    tCsrTdlsCtxStruct tdlsCtx ;
#endif
    tPmcInfo     pmc;
    tSmeBtcInfo  btc;

    tCcm ccm;

#if defined WLAN_FEATURE_VOWIFI
    tRrmContext rrm;
#endif
#ifdef WLAN_FEATURE_CONCURRENT_P2P
    tp2pContext p2pContext[MAX_NO_OF_P2P_SESSIONS];
#else
    tp2pContext p2pContext;
#endif

#if defined WLAN_FEATURE_VOWIFI_11R
    tftContext   ft;
#endif

    tANI_U32     gCurrentLogSize;
    tANI_U32     menuCurrent;
    /*                  */
    tANI_U32 dumpTablecurrentId;
    /*                                                                                    
                                      */ 
    tDumpModuleEntry *dumpTableEntry[MAX_DUMP_TABLE_ENTRY];
#ifdef FEATURE_WLAN_TDLS
    v_BOOL_t isTdlsPowerSaveProhibited;
#endif
    tANI_U8 fScanOffload;
} tAniSirGlobal;

#ifdef FEATURE_WLAN_TDLS

#define RFC1042_HDR_LENGTH      (6)
#define GET_BE16(x)             ((tANI_U16) (((x)[0] << 8) | (x)[1]))
#define ETH_TYPE_89_0d          (0x890d)
#define ETH_TYPE_LEN            (2)
#define PAYLOAD_TYPE_TDLS_SIZE  (1)
#define PAYLOAD_TYPE_TDLS       (2)

#endif

#endif /*              */


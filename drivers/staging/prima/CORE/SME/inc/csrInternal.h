/*
 * Copyright (c) 2012-2014, The Linux Foundation. All rights reserved.
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

/** ------------------------------------------------------------------------- *
    ------------------------------------------------------------------------- *


    \file csrInternal.h

    Define internal data structure for MAC.

    Copyright (C) 2006 Airgo Networks, Incorporated
   ========================================================================== */
#ifndef CSRINTERNAL_H__
#define CSRINTERNAL_H__

#include "vos_status.h"
#include "vos_lock.h"

#include "palTimer.h"
#include "csrSupport.h"
#include "vos_nvitem.h"
#include "wlan_qct_tl.h"

#ifdef WLAN_FEATURE_NEIGHBOR_ROAMING
#include "csrNeighborRoam.h"
#endif

#define CSR_MAX_STA (HAL_NUM_STA)

#define CSR_SME_SCAN_FLAGS_DELETE_CACHE     0x80

#define CSR_TITAN_MAX_RATE_MIMO_CB 240
#define CSR_TITAN_MAX_RATE_MIMO    126

//                                                                
#define CSR_SCAN_RETURN_AFTER_ALL_CHANNELS          (    0 )
#define CSR_SCAN_RETURN_AFTER_FIRST_MATCH           ( 0x01 )
#define CSR_SCAN_RETURN_AFTER_5_BAND_11d_FOUND      ( 0x80 )
#define CSR_SCAN_RETURN_AFTER_24_BAND_11d_FOUND     ( 0x40 )
#define CSR_SCAN_RETURN_AFTER_EITHER_BAND_11d_FOUND ( CSR_SCAN_RETURN_AFTER_5_BAND_11d_FOUND | CSR_SCAN_RETURN_AFTER_24_BAND_11d_FOUND )
#define CSR_NUM_RSSI_CAT        15
#define CSR_MAX_STATISTICS_REQ        10
#define CSR_ROAM_SCAN_CHANNEL_SWITCH_TIME        3

//                            
#define CSR_SESSION_ID_INVALID    0xFF   //                   
#define CSR_ROAM_SESSION_MAX      5   //                                      
                                      //                                     

#define CSR_IS_SESSION_VALID( pMac, sessionId ) ( ( (sessionId) < CSR_ROAM_SESSION_MAX ) \
                                                  && ( (pMac)->roam.roamSession[(sessionId)].sessionActive ) )
#define CSR_GET_SESSION( pMac, sessionId ) \
( \
    (sessionId < CSR_ROAM_SESSION_MAX) ? \
     (&(pMac)->roam.roamSession[(sessionId)]) :\
     NULL \
)

#define CSR_IS_SELECT_5GHZ_MARGIN( pMac ) \
( \
   (((pMac)->roam.configParam.nSelect5GHzMargin)?eANI_BOOLEAN_TRUE:eANI_BOOLEAN_FALSE) \
)

#if  defined (WLAN_FEATURE_VOWIFI_11R) || defined (FEATURE_WLAN_CCX) || defined(FEATURE_WLAN_LFR)
#define CSR_IS_ROAM_PREFER_5GHZ( pMac ) \
( \
   (((pMac)->roam.configParam.nRoamPrefer5GHz)?eANI_BOOLEAN_TRUE:eANI_BOOLEAN_FALSE) \
)
#define CSR_IS_ROAM_INTRA_BAND_ENABLED( pMac ) \
( \
   (((pMac)->roam.configParam.nRoamIntraBand)?eANI_BOOLEAN_TRUE:eANI_BOOLEAN_FALSE) \
)
#endif

#ifdef WLAN_FEATURE_ROAM_SCAN_OFFLOAD
#define CSR_IS_ROAM_SCAN_OFFLOAD_ENABLED( pMac ) \
( \
   (((pMac)->roam.configParam.isRoamOffloadScanEnabled)?eANI_BOOLEAN_TRUE:eANI_BOOLEAN_FALSE) \
)

#define CSR_IS_FASTROAM_IN_CONCURRENCY_INI_FEATURE_ENABLED( pMac ) \
( \
   (((pMac)->roam.configParam.bFastRoamInConIniFeatureEnabled)?eANI_BOOLEAN_TRUE:eANI_BOOLEAN_FALSE) \
)
#endif

//                                                        
#define CSR_BG_SCAN_OCCUPIED_CHANNEL_LIST_LEN 15

typedef enum
{
    //                             
    eCSR_CFG_DOT11_MODE_TAURUS = 0,
    eCSR_CFG_DOT11_MODE_ABG,
    eCSR_CFG_DOT11_MODE_11A,
    eCSR_CFG_DOT11_MODE_11B,
    eCSR_CFG_DOT11_MODE_11G,
    eCSR_CFG_DOT11_MODE_11N,
    eCSR_CFG_DOT11_MODE_POLARIS,
    eCSR_CFG_DOT11_MODE_TITAN,
#ifdef WLAN_FEATURE_11AC
    eCSR_CFG_DOT11_MODE_11AC,
#endif
    eCSR_CFG_DOT11_MODE_11G_ONLY,
    eCSR_CFG_DOT11_MODE_11N_ONLY,
#ifdef WLAN_FEATURE_11AC
    eCSR_CFG_DOT11_MODE_11AC_ONLY,
#endif
    //                                                             
    eCSR_CFG_DOT11_MODE_AUTO,
}eCsrCfgDot11Mode;  //                                                       

typedef enum etCsrRoamCommands
{
    eCsrRoamNoCommand,
    eCsrRoamCommandScan,
    eCsrRoamCommandRoam,
    eCsrRoamCommandWmStatusChange,
    eCsrRoamCommandSetKey,
    eCsrRoamCommandRemoveKey,

} eCsrRoamCommands;

typedef enum
{
    eCsrScanOther = 1,
    eCsrScanLostLink1,
    eCsrScanLostLink2,
    eCsrScanLostLink3,
    eCsrScanLostLink4,
    eCsrScan11d1,  //              
    eCsrScan11d2,  //                         
    eCsrScan11dDone,  //                                               
    eCsrScanUserRequest,
    eCsrScanGetResult,
    eCsrScanSetBGScanParam, //                                                         
    eCsrScanForSsid,
    eCsrScanForCapsChange,
    eCsrScanBGScanAbort,
    eCsrScanBGScanEnable,
    eCsrScanIdleScan,
    eCsrScanGetScanChnInfo,     //                                   

    eCsrScanBgScan, //                                               
    eCsrScanProbeBss, //                                                        
    eCsrScanAbortBgScan,    //                                                               
    eCsrScanAbortNormalScan, //                                                                 
    eCsrScanP2PFindPeer,
    eCsrScanGetLfrResult, //                                          
}eCsrScanReason;

typedef enum
{
    eCsrNoConnection,          //                                                                
    eCsrCapsChange,            //                                                                       
    eCsrForcedDisassoc,        //                                                                     
    eCsrHddIssued,             //                                                            
    eCsrLostLink1,             //                                                 
    eCsrLostLink2,
    eCsrLostLink3,
    eCsrForcedDisassocMICFailure, //                                                               
    eCsrHddIssuedReassocToSameAP,
    eCsrSmeIssuedReassocToSameAP,
    eCsrSmeIssuedReassocToDiffAP,
    eCsrForcedDeauth,        //                                                                   
    eCsrSmeIssuedDisassocForHandoff, //                                                             
    eCsrSmeIssuedAssocToSimilarAP, //                                                                   
    eCsrSmeIssuedIbssJoinFailure, //                                                                          
    eCsrForcedIbssLeave,
    eCsrStopBss,
    eCsrSmeIssuedFTReassoc,
    eCsrForcedDisassocSta,
    eCsrForcedDeauthSta,
    eCsrPerformPreauth,
    eCsrLostLink1Abort,
    eCsrLostLink2Abort,
    eCsrLostLink3Abort,

}eCsrRoamReason;

typedef enum
{
    eCSR_ROAM_SUBSTATE_NONE = 0,
    eCSR_ROAM_SUBSTATE_START_BSS_REQ,
    eCSR_ROAM_SUBSTATE_JOIN_REQ,
    eCSR_ROAM_SUBSTATE_REASSOC_REQ,
    eCSR_ROAM_SUBSTATE_DISASSOC_REQ,
    eCSR_ROAM_SUBSTATE_STOP_BSS_REQ,
    eCSR_ROAM_SUBSTATE_DISCONNECT_CONTINUE_ROAMING,   //                                                  
    eCSR_ROAM_SUBSTATE_AUTH_REQ,
    eCSR_ROAM_SUBSTATE_CONFIG,
    eCSR_ROAM_SUBSTATE_DEAUTH_REQ,
    eCSR_ROAM_SUBSTATE_DISASSOC_NOTHING_TO_JOIN,
    eCSR_ROAM_SUBSTATE_DISASSOC_REASSOC_FAILURE,
    eCSR_ROAM_SUBSTATE_DISASSOC_FORCED,
    eCSR_ROAM_SUBSTATE_WAIT_FOR_KEY,
    eCSR_ROAM_SUBSTATE_DISASSOC_HANDOFF,
    eCSR_ROAM_SUBSTATE_JOINED_NO_TRAFFIC,
    eCSR_ROAM_SUBSTATE_JOINED_NON_REALTIME_TRAFFIC,
    eCSR_ROAM_SUBSTATE_JOINED_REALTIME_TRAFFIC,
//                                              
} eCsrRoamSubState;


typedef enum
{
  eCSR_ROAMING_STATE_STOP = 0,
  eCSR_ROAMING_STATE_IDLE,
  eCSR_ROAMING_STATE_SCANNING,
  eCSR_ROAMING_STATE_JOINING,
  eCSR_ROAMING_STATE_JOINED,
}eCsrRoamState;


typedef enum
{
    eCsrContinueRoaming,
    eCsrStopRoaming,
    eCsrStartIbss,
    eCsrStartIbssSameIbss,
    eCsrReassocToSelfNoCapChange,
    eCsrStopRoamingDueToConcurrency,

}eCsrJoinState;

typedef enum
{
    eCsrNotRoaming,
    eCsrLostlinkRoamingDisassoc,
    eCsrLostlinkRoamingDeauth,
    eCsrDynamicRoaming,
   eCsrReassocRoaming,
}eCsrRoamingReason;

typedef enum
{
    eCsrDisassociated,
    eCsrDeauthenticated

}eCsrRoamWmStatusChangeTypes;

typedef enum
{
   eCsrSummaryStats = 0,
   eCsrGlobalClassAStats,
   eCsrGlobalClassBStats,
   eCsrGlobalClassCStats,
   eCsrGlobalClassDStats,
   eCsrPerStaStats,
   eCsrMaxStats
}eCsrRoamStatsClassTypes;

#ifdef FEATURE_WLAN_DIAG_SUPPORT
typedef enum
{
   eCSR_WLAN_STATUS_CONNECT =0,
   eCSR_WLAN_STATUS_DISCONNECT

}eCsrDiagWlanStatusEventSubtype;

typedef enum
{
   eCSR_REASON_UNSPECIFIED = 0,
   eCSR_REASON_USER_REQUESTED,
   eCSR_REASON_MIC_ERROR,
   eCSR_REASON_DISASSOC,
   eCSR_REASON_DEAUTH,
   eCSR_REASON_HANDOFF,

}eCsrDiagWlanStatusEventReason;

typedef enum
{
   eCSR_WLAN_HANDOFF_EVENT =0,

}eCsrDiagWlanHandoffEventSubtype;

typedef enum
{
   eCSR_WLAN_VCC_EVENT =0,

}eCsrDiagWlanVccEventSubtype;

#endif //                         

typedef struct tagCsrChannel
{
    tANI_U8 numChannels;
    tANI_U8 channelList[WNI_CFG_VALID_CHANNEL_LIST_LEN];
}tCsrChannel;

typedef struct tagScanProfile
{
    tANI_U32 minChnTime;
    tANI_U32 maxChnTime;
    tANI_U32 restTime;  //                                 
    tANI_U32 numOfChannels;
    tANI_U8 *pChannelList;
    tSirScanType scanType;  //                 
    eCsrRoamBssType bssType;    //           
    tANI_U8 ssid[WNI_CFG_SSID_LEN];
    tANI_U8 bReturnAfter1stMatch;
    tANI_U8 fUniqueResult;
    tANI_U8 freshScan;
    tCsrBssid bssid;
}tScanProfile;

typedef struct tagBssConfigParam
{
    eCsrMediaAccessType qosType;
    tSirMacSSid SSID;
    tANI_U32 uRTSThresh;
    tANI_U32 uDeferThresh;  //
    eCsrCfgDot11Mode uCfgDot11Mode;
    eCsrBand eBand;
    tANI_U8 standardRate[CSR_DOT11_SUPPORTED_RATES_MAX];
    tANI_U8 extendedRate[CSR_DOT11_EXTENDED_SUPPORTED_RATES_MAX];
    eCsrExposedTxRate txRate;
    tAniAuthType authType;
    eCsrEncryptionType encType;
    tANI_U32 uShortSlotTime;
    tANI_U32 uHTSupport;    //               
    tANI_U32 uPowerLimit;
    tANI_U32 uHeartBeatThresh;
    tANI_U32 uJoinTimeOut;
    tSirMacCapabilityInfo BssCap;
    tANI_BOOLEAN f11hSupport;
    ePhyChanBondState cbMode;
}tBssConfigParam;


typedef struct tagCsrRoamStartBssParams
{
    tSirMacSSid         ssId;
    tCsrBssid           bssid;    //                                                                          
    tSirNwType          sirNwType;
    ePhyChanBondState   cbMode;
    tSirMacRateSet      operationalRateSet;
    tSirMacRateSet      extendedRateSet;
    tANI_U8             operationChn;
    eCsrCfgDot11Mode    uCfgDot11Mode;
    tANI_U8             privacy;
    tANI_BOOLEAN        fwdWPSPBCProbeReq;
    tANI_BOOLEAN        protEnabled;
    tANI_BOOLEAN        obssProtEnabled;
    tAniAuthType        authType;
    tANI_U16            beaconInterval;    //                                          
    tANI_U16            ht_protection;
    tANI_U32            dtimPeriod;
    tANI_U8             ApUapsdEnable;
    tANI_U8             ssidHidden;
    tANI_U8             wps_state;
    tVOS_CON_MODE       bssPersona;
    tANI_U16            nRSNIELength;  //                                                      
    tANI_U8             *pRSNIE;     //                                              
    tANI_BOOLEAN        updatebeaconInterval; //                            
                                             //               
}tCsrRoamStartBssParams;


typedef struct tagScanCmd
{
    tANI_U32                scanID;
    csrScanCompleteCallback callback;
    void                    *pContext;
    eCsrScanReason          reason;
    eCsrRoamState           lastRoamState[CSR_ROAM_SESSION_MAX];
    tCsrRoamProfile         *pToRoamProfile;
    tANI_U32                roamId;    //                                            
    union
    {
        tCsrScanRequest   scanRequest;
        tCsrBGScanRequest bgScanRequest;
    }u;
}tScanCmd;

typedef struct tagRoamCmd
{
    tANI_U32 roamId;
    eCsrRoamReason roamReason;
    tCsrRoamProfile roamProfile;
    tScanResultHandle hBSSList;     //                         
    tListElem *pRoamBssEntry;  //                                                                                 
    tSirBssDescription *pLastRoamBss;   //                              
    tANI_BOOLEAN fReleaseBssList;  //                        
    tANI_BOOLEAN fReleaseProfile;  //                           
    tANI_BOOLEAN fReassoc;  //                                         
    tANI_BOOLEAN fUpdateCurRoamProfile;     //                                                      
    //                                                                                           
    //                                                
    tANI_BOOLEAN fReassocToSelfNoCapChange;

    tANI_BOOLEAN fStopWds;
    tSirMacAddr peerMac;
    tSirMacReasonCodes reason;
}tRoamCmd;

typedef struct tagSetKeyCmd
{
    tANI_U32 roamId;
    eCsrEncryptionType encType;
    eCsrAuthType authType;
    tAniKeyDirection keyDirection;    //                   
    tSirMacAddr peerMac;   //                                         
    tANI_U8 paeRole;      //                
    tANI_U8 keyId;  //          
    tANI_U8 keyLength;  //                                          
    tANI_U8 Key[CSR_MAX_KEY_LEN];
    tANI_U8 keyRsc[CSR_MAX_RSC_LEN];
} tSetKeyCmd;

typedef struct tahRemoveKeyCmd
{
    tANI_U32 roamId;
    eCsrEncryptionType encType;
    eCsrAuthType authType;
    tSirMacAddr peerMac;   //                                         
    tANI_U8 keyId;  //         
} tRemoveKeyCmd;

typedef struct tagWmStatusChangeCmd
{
    eCsrRoamWmStatusChangeTypes Type;
    union
    {
        tSirSmeDeauthInd   DeauthIndMsg;
        tSirSmeDisassocInd DisassocIndMsg;
    }u;

}tWmStatusChangeCmd;


typedef struct tagAddStaForSessionCmd
{
   //                     
   tSirMacAddr selfMacAddr;
   tVOS_CON_MODE currDeviceMode;
}tAddStaForSessionCmd;

typedef struct tagDelStaForSessionCmd
{
   //                     
   tSirMacAddr selfMacAddr;
   csrRoamSessionCloseCallback callback;
   void *pContext;
}tDelStaForSessionCmd;

//                                          
typedef struct tagCsrCmd
{
    tListElem Link;
    eCsrRoamCommands command;
    tANI_U8 sessionId;              //                            
    union
    {
        tScanCmd scanCmd;
        tRoamCmd roamCmd;
        tWmStatusChangeCmd wmStatusChangeCmd;
        tSetKeyCmd setKeyCmd;
        tRemoveKeyCmd removeKeyCmd;
        tAddStaForSessionCmd addStaSessionCmd;
        tDelStaForSessionCmd delStaSessionCmd;
    }u;
}tCsrCmd;

#ifdef WLAN_FEATURE_VOWIFI_11R
typedef struct tagCsr11rConfig
{
    tANI_BOOLEAN   IsFTResourceReqSupported;
} tCsr11rConfig;
#endif

#ifdef WLAN_FEATURE_NEIGHBOR_ROAMING
typedef struct tagCsrNeighborRoamConfig
{
    tANI_U32       nNeighborScanTimerPeriod;
    tANI_U8        nNeighborLookupRssiThreshold;
    tANI_U8        nNeighborReassocRssiThreshold;
    tANI_U16       nNeighborScanMinChanTime;
    tANI_U16       nNeighborScanMaxChanTime;
    sCsrChannel    neighborScanChanList;
    tANI_U8        nMaxNeighborRetries;
    tANI_U16       nNeighborResultsRefreshPeriod;
    tANI_U16       nEmptyScanRefreshPeriod;
}tCsrNeighborRoamConfig;
#endif

typedef struct tagCsrConfig
{
    tANI_U32 agingCount;
    tANI_U32 FragmentationThreshold;
    tANI_U32 channelBondingMode24GHz;
    tANI_U32 channelBondingMode5GHz;
    tANI_U32 RTSThreshold;
    eCsrPhyMode phyMode;
    eCsrCfgDot11Mode uCfgDot11Mode;
    eCsrBand eBand;
    tANI_U32 HeartbeatThresh50;
    tANI_U32 HeartbeatThresh24;
    tANI_U32 bgScanInterval;
    eCsrCBChoice cbChoice;
    eCsrBand bandCapability;     //                      
    eCsrRoamWmmUserModeType WMMSupportMode;
    tANI_BOOLEAN Is11eSupportEnabled;
    tANI_BOOLEAN Is11dSupportEnabled;
    tANI_BOOLEAN Is11dSupportEnabledOriginal;
    tANI_BOOLEAN Is11hSupportEnabled;
    tANI_BOOLEAN shortSlotTime;
    tANI_BOOLEAN ProprietaryRatesEnabled;
    tANI_BOOLEAN  fenableMCCMode;
    tANI_U16 TxRate;
    tANI_U8  fAllowMCCGODiffBI;
    tANI_U8 AdHocChannel24;
    tANI_U8 AdHocChannel5G;
    tANI_U32 impsSleepTime;     //                        
    tANI_U32 scanAgeTimeNCNPS;  //                                                                           
    tANI_U32 scanAgeTimeNCPS;   //                                                                        
    tANI_U32 scanAgeTimeCNPS;   //                                                                        
    tANI_U32 scanAgeTimeCPS;   //                                                                  
    tANI_U32 BssPreferValue[CSR_NUM_RSSI_CAT];  //                                
    int RSSICat[CSR_NUM_RSSI_CAT];
    tANI_U8 bCatRssiOffset;     //                                            
    tANI_U32 nRoamingTime;  //                                                                      
    //                                                                                     
    //                                                                                      
    //                                                    
    tANI_BOOLEAN fEnforce11dChannels;
    //                     
    //                                     
    //                                     
    tANI_BOOLEAN fSupplicantCountryCodeHasPriority;
    //                                                     
    //                                                                       
    //                                                 
    tANI_BOOLEAN fEnforceCountryCodeMatch;
    //                                                                                     
    //                                                                                    
    //               
    tANI_BOOLEAN fEnforceDefaultDomain;

    tANI_U16 vccRssiThreshold;
    tANI_U32 vccUlMacLossThreshold;

    tANI_U32  nPassiveMinChnTime;    //                        
    tANI_U32  nPassiveMaxChnTime;    //                        
    tANI_U32  nActiveMinChnTime;     //                        
    tANI_U32  nActiveMaxChnTime;     //                        

    tANI_U32  nActiveMinChnTimeBtc;     //                        
    tANI_U32  nActiveMaxChnTimeBtc;     //                        
    tANI_U8   disableAggWithBtc;
#ifdef WLAN_AP_STA_CONCURRENCY
    tANI_U32  nPassiveMinChnTimeConc;    //                        
    tANI_U32  nPassiveMaxChnTimeConc;    //                        
    tANI_U32  nActiveMinChnTimeConc;     //                        
    tANI_U32  nActiveMaxChnTimeConc;     //                        
    tANI_U32  nRestTimeConc;             //                        
    tANI_U8   nNumStaChanCombinedConc;   //                               
                                         //                                
    tANI_U8   nNumP2PChanCombinedConc;   //                               
                                         //                                
#endif

    tANI_BOOLEAN IsIdleScanEnabled;
    //                            
    //                                                        
    //                                                          
    tANI_U8 nTxPowerCap;
    tANI_U32  statsReqPeriodicity;  //                                                   
    tANI_U32  statsReqPeriodicityInPS;//                                                   
    tANI_U32 dtimPeriod;
    tANI_BOOLEAN    ssidHidden;

#ifdef WLAN_FEATURE_VOWIFI_11R
    tCsr11rConfig csr11rConfig;
#endif

#ifdef FEATURE_WLAN_LFR
    tANI_U8      isFastRoamIniFeatureEnabled;
    tANI_U8      MAWCEnabled;
#ifdef WLAN_FEATURE_ROAM_SCAN_OFFLOAD
    tANI_U8      isRoamOffloadScanEnabled;
    tANI_BOOLEAN bFastRoamInConIniFeatureEnabled;
#endif
#endif

#ifdef FEATURE_WLAN_CCX
    tANI_U8   isCcxIniFeatureEnabled;
#endif

#if  defined (WLAN_FEATURE_VOWIFI_11R) || defined (FEATURE_WLAN_CCX) || defined(FEATURE_WLAN_LFR)
    tANI_U8       isFastTransitionEnabled;
    tANI_U8       RoamRssiDiff;
    tANI_U8       nImmediateRoamRssiDiff;
    tANI_BOOLEAN  nRoamPrefer5GHz;
    tANI_BOOLEAN  nRoamIntraBand;
    tANI_BOOLEAN  isWESModeEnabled;
    tANI_BOOLEAN  nRoamScanControl;
    tANI_U8       nProbes;
    tANI_U16      nRoamScanHomeAwayTime;
#endif

#ifdef WLAN_FEATURE_NEIGHBOR_ROAMING
    tCsrNeighborRoamConfig neighborRoamConfig;
#endif

    /*                                                                      
                                                */
    tANI_BOOLEAN addTSWhenACMIsOff;

    tANI_BOOLEAN fValidateList;
    //                                                     
    //                          
    tANI_BOOLEAN doBMPSWorkaround;

    //                                                                                  
    tANI_BOOLEAN fScanTwice;
#ifdef WLAN_FEATURE_11AC
    tANI_U32  nVhtChannelWidth;
    tANI_U8   txBFEnable;
    tANI_U8   txBFCsnValue;
    tANI_BOOLEAN enableVhtFor24GHz;
#endif
    tANI_U8   txLdpcEnable;
    tANI_BOOLEAN  enableOxygenNwk;

    /*
                                       
     */
    tANI_BOOLEAN enableHeartBeatOffload;
    tANI_U8 isAmsduSupportInAMPDU;
    tANI_U8 nSelect5GHzMargin;
}tCsrConfig;

typedef struct tagCsrChannelPowerInfo
{
    tListElem link;
    tANI_U8 firstChannel;
    tANI_U8 numChannels;
    tANI_U8 txPower;
    tANI_U8 interChannelOffset;
}tCsrChannelPowerInfo;

typedef struct tagRoamJoinStatus
{
    tSirResultCodes statusCode;
    //                                                                                                                          
    tANI_U32 reasonCode;
    tSirMacAddr bssId;
}tCsrRoamJoinStatus;

typedef struct tagCsrOsChannelMask
{
    tANI_U8 numChannels;
    tANI_BOOLEAN scanEnabled[WNI_CFG_VALID_CHANNEL_LIST_LEN];
    tANI_U8 channelList[WNI_CFG_VALID_CHANNEL_LIST_LEN];
}tCsrOsChannelMask;


typedef struct tagCsrScanStruct
{
    tScanProfile scanProfile;
    tANI_U32 nextScanID;
    tDblLinkList scanResultList;
    tDblLinkList tempScanResults;
    tANI_BOOLEAN fScanEnable;
    tANI_BOOLEAN fFullScanIssued;
    vos_timer_t hTimerGetResult;
#ifdef WLAN_AP_STA_CONCURRENCY
    vos_timer_t hTimerStaApConcTimer;
#endif
    vos_timer_t hTimerIdleScan;
    vos_timer_t hTimerResultAging;
    vos_timer_t hTimerResultCfgAging;
    tPalTimerHandle hTimerBgScan;
    //                                                                                       
    tANI_U8 channelOf11dInfo;
    tANI_U8 scanResultCfgAgingTime;
    //                                                                                 
    tANI_BOOLEAN fAmbiguous11dInfoFound;
    //                                                                                                           
    tANI_BOOLEAN fCurrent11dInfoMatch;
    tANI_BOOLEAN f11dInfoReset;     //                                                           
    tSirScanType curScanType;
    tCsrChannel baseChannels;   //                                                               
    tCsrChannel channels11d;
    tChannelListWithPower defaultPowerTable[WNI_CFG_VALID_CHANNEL_LIST_LEN]; //       
    tChannelListWithPower defaultPowerTable40MHz[WNI_CFG_VALID_CHANNEL_LIST_LEN]; //       
    tANI_U32 numChannelsDefault; //                    
    tCsrChannel base20MHzChannels;   //                           
    tCsrChannel base40MHzChannels;   //                                  
    tDblLinkList channelPowerInfoList24;
    tDblLinkList channelPowerInfoList5G;
    tANI_U32 nLastAgeTimeOut;
    tANI_U32 nAgingCountDown;
    tANI_U8 countryCodeDefault[WNI_CFG_COUNTRY_CODE_LEN];     //                        
    tANI_U8 countryCodeCurrent[WNI_CFG_COUNTRY_CODE_LEN];
    tANI_U8 countryCode11d[WNI_CFG_COUNTRY_CODE_LEN];
    v_REGDOMAIN_t domainIdDefault;  //                         
    v_REGDOMAIN_t domainIdCurrent;  //                         
    tCsrBssid currentCountryBssid;  //                               
    tANI_BOOLEAN f11dInfoApplied;
    tANI_BOOLEAN fCancelIdleScan;
#ifdef FEATURE_WLAN_WAPI
//                              
//                                                                                                       
#endif /*                   */
    tANI_U8 numBGScanChannel;   //                                                 
    tANI_U8 bgScanChannelList[WNI_CFG_BG_SCAN_CHANNEL_LIST_LEN];
    //                                                     
    //                                                                            
    tCsrBGScanRequest bgScanParams;
    tANI_BOOLEAN fRestartIdleScan;
    tANI_U32 nIdleScanTimeGap;  //                                               
    tCsrOsChannelMask osScanChannelMask;//                                                                
    tANI_U16 nBssLimit; //                                       
    /*                                                                               
                                                                                         
                                     */
    tANI_BOOLEAN fValidateList;
    /*                                                                              
                                                                          
                                                                               
                                             */
    tANI_BOOLEAN fEnableBypass11d;

    /*                                                                        
                                                                   
                      */
    tANI_BOOLEAN fEnableDFSChnlScan;

    /*
                                                                   
    */
    tANI_BOOLEAN fFirstScanOnly2GChnl;

    tANI_BOOLEAN fDropScanCmd; //                                        

#ifdef WLAN_AP_STA_CONCURRENCY
    tDblLinkList scanCmdPendingList;
#endif
    tCsrChannel occupiedChannels;   //                                                           
    tANI_S8     inScanResultBestAPRssi;

    csrScanCompleteCallback callback11dScanDone;
}tCsrScanStruct;

#ifdef FEATURE_WLAN_TDLS_INTERNAL
/*
                                        
 */
typedef struct sCsrTdlsContext
{
    tDblLinkList tdlsPotentialPeerList ;
    tANI_U16 tdlsCommonFlag ;
    tANI_U16 tdlsCommonState ;
    tANI_U16 tdlsPeerCount ;
}tCsrTdlsCtxStruct;

typedef struct sCsrTdlsPeerLinkInfo
{
    tListElem tdlsPeerStaLink ;
    tSirTdlsPeerInfo tdlsDisPeerInfo ;
}tCsrTdlsPeerLinkinfo ;
#endif




//                                                                 
//                                                   
typedef struct tagRoamCsrConnectedInfo
{
    tANI_U32 nBeaconLength; //                                                   
    tANI_U32 nAssocReqLength;   //                                                      
    tANI_U32 nAssocRspLength;   //                                                      
#ifdef WLAN_FEATURE_VOWIFI_11R
    tANI_U32 nRICRspLength; //                                                                  
#endif
#ifdef FEATURE_WLAN_CCX
    tANI_U32 nTspecIeLength;
#endif
    tANI_U8 *pbFrames;  //                                                                               
                        //                                                                                 
                        //                           
    tANI_U8 staId;
}tCsrRoamConnectedInfo;


typedef struct tagCsrLinkQualityIndInfo
{
   csrRoamLinkQualityIndCallback  callback;
   void                          *context;
}tCsrLinkQualityIndInfo;

typedef struct tagCsrPeStatsReqInfo
{
   tListElem              link;  /*            */
   tANI_U32               statsMask;
   tANI_U32               periodicity;
   tANI_BOOLEAN           rspPending;
   vos_timer_t            hPeStatsTimer;
   tANI_BOOLEAN           timerRunning;
   tANI_U8                staId;
   tANI_U8                numClient;
   tpAniSirGlobal         pMac;
   /*                                                                 */
   tANI_BOOLEAN           timerStopFailed;

}tCsrPeStatsReqInfo;

typedef struct tagCsrStatsClientReqInfo
{
   tListElem              link;  /*            */
   eCsrStatsRequesterType requesterId;
   tCsrStatsCallback      callback;
   tANI_U32               periodicity;
   void                  *pContext;
   tANI_U32               statsMask;
   tCsrPeStatsReqInfo    *pPeStaEntry;
   tANI_U8                staId;
   vos_timer_t            timer;
   tANI_BOOLEAN           timerExpired;
   tpAniSirGlobal         pMac; //                                
}tCsrStatsClientReqInfo;

typedef struct tagCsrTlStatsReqInfo
{
   tANI_U32               periodicity;
   tANI_BOOLEAN           timerRunning;
   vos_timer_t            hTlStatsTimer;
   tANI_U8                numClient;
}tCsrTlStatsReqInfo;

typedef struct tagCsrRoamSession
{
    tANI_U8 sessionId;             //           
    tANI_BOOLEAN sessionActive;      //                   
    tCsrBssid selfMacAddr;           //                                                      
    csrRoamCompleteCallback callback;
    void *pContext;
    eCsrConnectState connectState;
    tCsrRoamConnectedProfile connectedProfile;
    tCsrRoamConnectedInfo connectedInfo;
    tCsrRoamProfile *pCurRoamProfile;
    tSirBssDescription *pConnectBssDesc;
    tANI_U16 NumPmkidCache;
    tPmkidCacheInfo PmkidCacheInfo[CSR_MAX_PMKID_ALLOWED];
    tANI_U8 cJoinAttemps;
    //                                                          
    //                                                                                 
    tSirMacChanNum validChannelList[WNI_CFG_VALID_CHANNEL_LIST_LEN];
    tANI_S32 sPendingCommands;    //                              
#ifdef FEATURE_WLAN_WAPI
    tANI_U16 NumBkidCache;
    tBkidCacheInfo BkidCacheInfo[CSR_MAX_BKID_ALLOWED];
#endif /*                   */
    tANI_BOOLEAN fRoaming;  //                                                                        
    //                                                 
    //                                                                           
    tCsrRoamStartBssParams bssParams;
    tANI_U32 nWpaRsnReqIeLength;   //                            
    tANI_U8 *pWpaRsnReqIE; //                                                                             
    tANI_U32 nWpaRsnRspIeLength;    //                               
    tANI_U8 *pWpaRsnRspIE;  //                                               
#ifdef FEATURE_WLAN_WAPI
    tANI_U32 nWapiReqIeLength;   //                             
    tANI_U8 *pWapiReqIE; //                                                                          
    tANI_U32 nWapiRspIeLength;    //                             
    tANI_U8 *pWapiRspIE;  //                                            
#endif /*                   */
    tANI_U32 nAddIEScanLength;  //                               
    tANI_U8 *pAddIEScan; //                                                                              
    tANI_U32 nAddIEAssocLength;      //                                
    tANI_U8 *pAddIEAssoc; //                                                     

    tANI_TIMESTAMP roamingStartTime;    //                
    tCsrTimerInfo roamingTimerInfo;
    eCsrRoamingReason roamingReason;
    tANI_BOOLEAN fCancelRoaming;
    vos_timer_t hTimerRoaming;
    vos_timer_t hTimerIbssJoining;
    tCsrTimerInfo ibssJoinTimerInfo;
    tANI_BOOLEAN ibss_join_pending;
    eCsrRoamResult roamResult;  //                                                        
    tCsrRoamJoinStatus joinFailStatusCode;    //                                               
    //                                                                                                         
    tANI_U32 roamingStatusCode;
    tANI_U16 NumPmkidCandidate;
    tPmkidCandidateInfo PmkidCandidateInfo[CSR_MAX_PMKID_ALLOWED];
 #ifdef FEATURE_WLAN_WAPI
    tANI_U16 NumBkidCandidate;
    tBkidCandidateInfo BkidCandidateInfo[CSR_MAX_BKID_ALLOWED];
#endif
    tANI_BOOLEAN fWMMConnection;
    tANI_BOOLEAN fQOSConnection;

#ifdef FEATURE_WLAN_BTAMP_UT_RF
    //                                                 
    vos_timer_t hTimerJoinRetry;
    tCsrTimerInfo joinRetryTimerInfo;
    tANI_U32 maxRetryCount;
#endif
#ifdef FEATURE_WLAN_CCX
    tCsrCcxCckmInfo ccxCckmInfo;
    tANI_BOOLEAN isPrevApInfoValid;
    tSirMacSSid prevApSSID;
    tCsrBssid prevApBssid;
    tANI_U8 prevOpChannel;
    tANI_U16 clientDissSecs;
    tANI_U32 roamTS1;
#if defined(FEATURE_WLAN_CCX_UPLOAD)
    tCsrCcxCckmIe suppCckmIeInfo;
#endif
#endif
    tANI_U8 bRefAssocStartCnt;   //                               
   /*                                                                                        
                                                                               
                                                                               */
    tANI_BOOLEAN fIgnorePMKIDCache;
} tCsrRoamSession;

typedef struct tagCsrRoamStruct
{
    tANI_U32 nextRoamId;
    tDblLinkList roamCmdPendingList;
    tDblLinkList channelList5G;
    tDblLinkList channelList24;
    tCsrConfig configParam;
    tANI_U32 numChannelsEeprom; //                        
    tCsrChannel base20MHzChannels;   //                           
    tCsrChannel base40MHzChannels;   //                                  
    eCsrRoamState curState[CSR_ROAM_SESSION_MAX];
    eCsrRoamSubState curSubState[CSR_ROAM_SESSION_MAX];
    //                                                          
    //                                                                                 
    tSirMacChanNum validChannelList[WNI_CFG_VALID_CHANNEL_LIST_LEN];
    tANI_U32 numValidChannels; //                               

    tANI_S32 sPendingCommands;
    vos_timer_t hTimerWaitForKey;  //                                       
    tCsrSummaryStatsInfo       summaryStatsInfo;
    tCsrGlobalClassAStatsInfo  classAStatsInfo;
    tCsrGlobalClassBStatsInfo  classBStatsInfo;
    tCsrGlobalClassCStatsInfo  classCStatsInfo;
    tCsrGlobalClassDStatsInfo  classDStatsInfo;
    tCsrPerStaStatsInfo        perStaStatsInfo[CSR_MAX_STA];
    tDblLinkList  statsClientReqList;
    tDblLinkList  peStatsReqList;
    tCsrTlStatsReqInfo  tlStatsReqInfo;
    eCsrRoamLinkQualityInd vccLinkQuality;
    tCsrLinkQualityIndInfo linkQualityIndInfo;
    v_CONTEXT_t gVosContext; //                            
    //                                                          
    //                                                                                 
    //                                                                           
    v_U8_t ucACWeights[WLANTL_MAX_AC];
    /*                  */
    tCsrTimerInfo WaitForKeyTimerInfo;
    tCsrRoamSession   *roamSession;
    tANI_U32 transactionId;        //                                         
#ifdef WLAN_FEATURE_NEIGHBOR_ROAMING
    tCsrNeighborRoamControlInfo neighborRoamInfo;
#endif
#ifdef FEATURE_WLAN_LFR
    tANI_U8   isFastRoamIniFeatureEnabled;
#endif
#ifdef FEATURE_WLAN_CCX
    tANI_U8   isCcxIniFeatureEnabled;
#endif
#if  defined (WLAN_FEATURE_VOWIFI_11R) || defined (FEATURE_WLAN_CCX) || defined(FEATURE_WLAN_LFR)
    tANI_U8        RoamRssiDiff;
    tANI_BOOLEAN   isWESModeEnabled;
#endif
    tANI_U32 deauthRspStatus;
}tCsrRoamStruct;


#define GET_NEXT_ROAM_ID(pRoamStruct)  (((pRoamStruct)->nextRoamId + 1 == 0) ? 1 : (pRoamStruct)->nextRoamId)
#define CSR_IS_ROAM_STATE(pMac, state, sessionId)  ( (state) == (pMac)->roam.curState[sessionId] )

#define CSR_IS_ROAM_STOP(pMac, sessionId) CSR_IS_ROAM_STATE( (pMac), eCSR_ROAMING_STATE_STOP, sessionId )
#define CSR_IS_ROAM_INIT(pMac, sessionId)  CSR_IS_ROAM_STATE( (pMac), eCSR_ROAMING_STATE_INIT, sessionId )
#define CSR_IS_ROAM_SCANNING(pMac, sessionId)  CSR_IS_ROAM_STATE( pMac, eCSR_ROAMING_STATE_SCANNING, sessionId )
#define CSR_IS_ROAM_JOINING(pMac, sessionId)   CSR_IS_ROAM_STATE( pMac, eCSR_ROAMING_STATE_JOINING, sessionId )
#define CSR_IS_ROAM_IDLE(pMac, sessionId) CSR_IS_ROAM_STATE( pMac, eCSR_ROAMING_STATE_IDLE, sessionId )
#define CSR_IS_ROAM_JOINED(pMac, sessionId)    CSR_IS_ROAM_STATE( pMac, eCSR_ROAMING_STATE_JOINED, sessionId )

#define CSR_IS_ROAM_SUBSTATE(pMac, subState, sessionId)   ((subState) == (pMac)->roam.curSubState[sessionId])
#define CSR_IS_ROAM_SUBSTATE_JOIN_REQ(pMac, sessionId)    CSR_IS_ROAM_SUBSTATE((pMac), eCSR_ROAM_SUBSTATE_JOIN_REQ, sessionId)
#define CSR_IS_ROAM_SUBSTATE_AUTH_REQ(pMac, sessionId)    CSR_IS_ROAM_SUBSTATE((pMac), eCSR_ROAM_SUBSTATE_AUTH_REQ, sessionId)
#define CSR_IS_ROAM_SUBSTATE_REASSOC_REQ(pMac, sessionId)    CSR_IS_ROAM_SUBSTATE((pMac), eCSR_ROAM_SUBSTATE_REASSOC_REQ, sessionId)
#define CSR_IS_ROAM_SUBSTATE_DISASSOC_REQ(pMac, sessionId)    CSR_IS_ROAM_SUBSTATE((pMac), eCSR_ROAM_SUBSTATE_DISASSOC_REQ, sessionId)
#define CSR_IS_ROAM_SUBSTATE_DISASSOC_NO_JOIN(pMac, sessionId)    CSR_IS_ROAM_SUBSTATE((pMac), eCSR_ROAM_SUBSTATE_DISASSOC_NOTHING_TO_JOIN, sessionId)
#define CSR_IS_ROAM_SUBSTATE_REASSOC_FAIL(pMac, sessionId)    CSR_IS_ROAM_SUBSTATE((pMac), eCSR_ROAM_SUBSTATE_DISASSOC_REASSOC_FAILURE, sessionId)
#define CSR_IS_ROAM_SUBSTATE_DISASSOC_FORCED(pMac, sessionId)    CSR_IS_ROAM_SUBSTATE((pMac), eCSR_ROAM_SUBSTATE_DISASSOC_FORCED, sessionId)
#define CSR_IS_ROAM_SUBSTATE_DEAUTH_REQ(pMac, sessionId)    CSR_IS_ROAM_SUBSTATE((pMac), eCSR_ROAM_SUBSTATE_DEAUTH_REQ, sessionId)
#define CSR_IS_ROAM_SUBSTATE_START_BSS_REQ(pMac, sessionId)    CSR_IS_ROAM_SUBSTATE((pMac), eCSR_ROAM_SUBSTATE_START_BSS_REQ, sessionId)
#define CSR_IS_ROAM_SUBSTATE_STOP_BSS_REQ(pMac, sessionId)    CSR_IS_ROAM_SUBSTATE((pMac), eCSR_ROAM_SUBSTATE_STOP_BSS_REQ, sessionId)
#define CSR_IS_ROAM_SUBSTATE_DISCONNECT_CONTINUE(pMac, sessionId)    CSR_IS_ROAM_SUBSTATE((pMac), eCSR_ROAM_SUBSTATE_DISCONNECT_CONTINUE_ROAMING, sessionId)
#define CSR_IS_ROAM_SUBSTATE_CONFIG(pMac, sessionId)    CSR_IS_ROAM_SUBSTATE((pMac), eCSR_ROAM_SUBSTATE_CONFIG, sessionId)
#define CSR_IS_ROAM_SUBSTATE_WAITFORKEY(pMac, sessionId)    CSR_IS_ROAM_SUBSTATE((pMac), eCSR_ROAM_SUBSTATE_WAIT_FOR_KEY, sessionId)
#define CSR_IS_ROAM_SUBSTATE_DISASSOC_HO(pMac, sessionId)    CSR_IS_ROAM_SUBSTATE((pMac), eCSR_ROAM_SUBSTATE_DISASSOC_HANDOFF, sessionId)
#define CSR_IS_ROAM_SUBSTATE_HO_NT(pMac, sessionId)    CSR_IS_ROAM_SUBSTATE((pMac), eCSR_ROAM_SUBSTATE_JOINED_NO_TRAFFIC, sessionId)
#define CSR_IS_ROAM_SUBSTATE_HO_NRT(pMac, sessionId)    CSR_IS_ROAM_SUBSTATE((pMac), eCSR_ROAM_SUBSTATE_JOINED_NON_REALTIME_TRAFFIC, sessionId)
#define CSR_IS_ROAM_SUBSTATE_HO_RT(pMac, sessionId)    CSR_IS_ROAM_SUBSTATE((pMac), eCSR_ROAM_SUBSTATE_JOINED_REALTIME_TRAFFIC, sessionId)

#define CSR_IS_PHY_MODE_B_ONLY(pMac) \
        ((eCSR_DOT11_MODE_11b == (pMac)->roam.configParam.phyMode) ||\
        (eCSR_DOT11_MODE_11b_ONLY == (pMac)->roam.configParam.phyMode))

#define CSR_IS_PHY_MODE_G_ONLY(pMac) \
        (eCSR_DOT11_MODE_11g == (pMac)->roam.configParam.phyMode || eCSR_DOT11_MODE_11g_ONLY == (pMac)->roam.configParam.phyMode)

#define CSR_IS_PHY_MODE_A_ONLY(pMac) \
        ((eCSR_DOT11_MODE_11a == (pMac)->roam.configParam.phyMode) ||\
        (eCSR_DOT11_MODE_11a_ONLY == (pMac)->roam.configParam.phyMode))

#ifdef WLAN_FEATURE_11AC
#define CSR_IS_PHY_MODE_DUAL_BAND(phyMode) \
        ((eCSR_DOT11_MODE_abg & (phyMode)) || (eCSR_DOT11_MODE_11n & (phyMode)) || \
        (eCSR_DOT11_MODE_11ac & (phyMode)) || \
        (eCSR_DOT11_MODE_TAURUS & (phyMode)) || \
        (eCSR_DOT11_MODE_AUTO & (phyMode)))
#else
#define CSR_IS_PHY_MODE_DUAL_BAND(phyMode) \
        ((eCSR_DOT11_MODE_abg & (phyMode)) || (eCSR_DOT11_MODE_11n & (phyMode)) || \
        (eCSR_DOT11_MODE_TAURUS & (phyMode)) || \
        (eCSR_DOT11_MODE_AUTO & (phyMode)))
#endif


//                                                                                            
//                                         
#define CSR_IS_24_BAND_ONLY(pMac) \
        (eCSR_BAND_24 == (pMac)->roam.configParam.eBand)

#define CSR_IS_5G_BAND_ONLY(pMac) \
        (eCSR_BAND_5G == (pMac)->roam.configParam.eBand)

#define CSR_IS_RADIO_DUAL_BAND(pMac) \
        (eCSR_BAND_ALL == (pMac)->roam.configParam.bandCapability)

#define CSR_IS_RADIO_BG_ONLY(pMac) \
        (eCSR_BAND_24 == (pMac)->roam.configParam.bandCapability)

//                                                                                            
//                                        
#define CSR_IS_RADIO_A_ONLY(pMac) \
        (eCSR_BAND_5G == (pMac)->roam.configParam.bandCapability)

//                                                                  
#define CSR_IS_OPEARTING_DUAL_BAND(pMac) \
        ((eCSR_BAND_ALL == (pMac)->roam.configParam.bandCapability) && (eCSR_BAND_ALL == (pMac)->roam.configParam.eBand))

//                                                                                            
//                    
#define CSR_IS_OPERATING_A_BAND(pMac) \
        (CSR_IS_OPEARTING_DUAL_BAND((pMac)) || CSR_IS_RADIO_A_ONLY((pMac)) || CSR_IS_5G_BAND_ONLY((pMac)))

//                                                                                            
//                    
#define CSR_IS_OPERATING_BG_BAND(pMac) \
        (CSR_IS_OPEARTING_DUAL_BAND((pMac)) || CSR_IS_RADIO_BG_ONLY((pMac)) || CSR_IS_24_BAND_ONLY((pMac)))

#define CSR_IS_CHANNEL_5GHZ(chnNum) \
        (((chnNum) >= CSR_MIN_5GHz_CHANNEL_NUMBER) && ((chnNum) <= CSR_MAX_5GHz_CHANNEL_NUMBER))

#define CSR_IS_CHANNEL_DFS(chnNum) \
         (NV_CHANNEL_ENABLE != vos_nv_getChannelEnabledState(chnNum))

#define CSR_IS_CHANNEL_24GHZ(chnNum) \
        (((chnNum) > 0) && ((chnNum) <= CSR_MAX_24GHz_CHANNEL_NUMBER))

#define CSR_IS_SAME_BAND_CHANNELS(ch1, ch2) (CSR_IS_CHANNEL_5GHZ(ch1) == CSR_IS_CHANNEL_5GHZ(ch2))


#define CSR_IS_11D_INFO_FOUND(pMac) \
        (0 != (pMac)->scan.channelOf11dInfo)
//          
#define CSR_IS_ROAMING(pSession) ((CSR_IS_LOSTLINK_ROAMING((pSession)->roamingReason)) || \
                              (eCsrDynamicRoaming == (pSession)->roamingReason)  || \
                              (eCsrReassocRoaming == (pSession)->roamingReason))


#define CSR_IS_SET_KEY_COMMAND( pCommand )    ( eSmeCommandSetKey == (pCommand)->command )

#define CSR_IS_ADDTS_WHEN_ACMOFF_SUPPORTED(pMac) (pMac->roam.configParam.addTSWhenACMIsOff)
//          
#define CSR_IS_LOSTLINK_ROAMING(reason)  ((eCsrLostlinkRoamingDisassoc == (reason)) || (eCsrLostlinkRoamingDeauth == (reason)))

#define CSR_IS_ROAMING_COMMAND(pCommand) ((eCsrLostLink1 == (pCommand)->u.roamCmd.roamReason) ||\
                                          (eCsrLostLink2 == (pCommand)->u.roamCmd.roamReason) ||\
                                          (eCsrLostLink3 == (pCommand)->u.roamCmd.roamReason) )


//                                                                          
void csrScanSuspendIMPS( tpAniSirGlobal pMac );
//                                                                                          
//                            
void csrScanResumeIMPS( tpAniSirGlobal pMac );

eHalStatus csrInitGetChannels(tpAniSirGlobal pMac);
eHalStatus csrScanFilter11dResult(tpAniSirGlobal pMac);

eHalStatus csrSetModifyProfileFields(tpAniSirGlobal pMac, tANI_U32 sessionId,
                                     tCsrRoamModifyProfileFields *pModifyProfileFields);
/*                                                                            
                                 
                                                                            
                                                              
                                                                                
          
                                                                         
                                    

                      
                                                                                 */
eHalStatus csrGetModifyProfileFields(tpAniSirGlobal pMac, tANI_U32 sessionId,
                                     tCsrRoamModifyProfileFields * pModifyProfileFields);
void csrSetGlobalCfgs( tpAniSirGlobal pMac );
void csrSetDefaultDot11Mode( tpAniSirGlobal pMac );
void csrScanSetChannelMask(tpAniSirGlobal pMac, tCsrChannelInfo *pChannelInfo);
tANI_BOOLEAN csrIsConnStateDisconnected(tpAniSirGlobal pMac, tANI_U32 sessionId);
tANI_BOOLEAN csrIsConnStateConnectedIbss( tpAniSirGlobal pMac, tANI_U32 sessionId );
tANI_BOOLEAN csrIsConnStateDisconnectedIbss( tpAniSirGlobal pMac, tANI_U32 sessionId );
tANI_BOOLEAN csrIsConnStateConnectedInfra( tpAniSirGlobal pMac, tANI_U32 sessionId );
tANI_BOOLEAN csrIsConnStateConnected( tpAniSirGlobal pMac, tANI_U32 sessionId );
tANI_BOOLEAN csrIsConnStateInfra( tpAniSirGlobal pMac, tANI_U32 sessionId );
tANI_BOOLEAN csrIsConnStateIbss( tpAniSirGlobal pMac, tANI_U32 sessionId );
tANI_BOOLEAN csrIsConnStateWds( tpAniSirGlobal pMac, tANI_U32 sessionId );
tANI_BOOLEAN csrIsConnStateConnectedWds( tpAniSirGlobal pMac, tANI_U32 sessionId );
tANI_BOOLEAN csrIsConnStateDisconnectedWds( tpAniSirGlobal pMac, tANI_U32 sessionId );
tANI_BOOLEAN csrIsAnySessionInConnectState( tpAniSirGlobal pMac );
tANI_BOOLEAN csrIsAllSessionDisconnected( tpAniSirGlobal pMac );
tANI_BOOLEAN csrIsStaSessionConnected( tpAniSirGlobal pMac );
tANI_BOOLEAN csrIsP2pSessionConnected( tpAniSirGlobal pMac );
tANI_BOOLEAN csrIsAnySessionConnected( tpAniSirGlobal pMac );
tANI_BOOLEAN csrIsInfraConnected( tpAniSirGlobal pMac );
tANI_BOOLEAN csrIsConcurrentInfraConnected( tpAniSirGlobal pMac );
tANI_BOOLEAN csrIsConcurrentSessionRunning( tpAniSirGlobal pMac );
tANI_BOOLEAN csrIsInfraApStarted( tpAniSirGlobal pMac );
tANI_BOOLEAN csrIsIBSSStarted( tpAniSirGlobal pMac );
tANI_BOOLEAN csrIsBTAMPStarted( tpAniSirGlobal pMac );
tANI_BOOLEAN csrIsBTAMP( tpAniSirGlobal pMac, tANI_U32 sessionId );
eHalStatus csrIsBTAMPAllowed( tpAniSirGlobal pMac, tANI_U32 chnId );
tANI_BOOLEAN csrIsValidMcConcurrentSession(tpAniSirGlobal pMac, tANI_U32 sessionId,
                                                  tSirBssDescription *pBssDesc);
tANI_BOOLEAN csrIsConnStateConnectedInfraAp( tpAniSirGlobal pMac, tANI_U32 sessionId );
/*                                                                            
                                           

        
                                                                         
                           

                                                  
                                                                    

                                                         

                                              
                             

                              

                    
                                                                             */
eHalStatus csrRoamRegisterLinkQualityIndCallback(tpAniSirGlobal pMac,
                                                 csrRoamLinkQualityIndCallback   callback,
                                                 void                           *pContext);
/*                                                                            
                        
                                                                       
                                            

                                                                                     
                                                                                          
                                                                           
                                                                                
                                     
                                                          
                                                                      
                                                                            

                      
                                                                             */
eHalStatus csrGetStatistics(tpAniSirGlobal pMac, eCsrStatsRequesterType requesterId,
                            tANI_U32 statsMask,
                            tCsrStatsCallback callback,
                            tANI_U32 periodicity, tANI_BOOLEAN cache,
                            tANI_U8 staId, void *pContext);

/*                                                                            
                        
                                                                             

                                                   
                                            
                                 
                                                                             */
tANI_U16 csrGetTLSTAState(tpAniSirGlobal pMac, tANI_U8 staId);

eHalStatus csrGetRssi(tpAniSirGlobal pMac,tCsrRssiCallback callback,tANI_U8 staId,tCsrBssid bssId,void * pContext,void * pVosContext);

/*                                                                            
                 
                                                                       
                     

                                                                           
                                                                      
                                                      
                                                                            

                      
                                                                             */
eHalStatus csrGetSnr(tpAniSirGlobal pMac, tCsrSnrCallback callback,
                     tANI_U8 staId, tCsrBssid bssId, void *pContext);

#if defined WLAN_FEATURE_VOWIFI_11R || defined FEATURE_WLAN_CCX || defined(FEATURE_WLAN_LFR)
eHalStatus csrGetRoamRssi(tpAniSirGlobal pMac,
                          tCsrRssiCallback callback,
                          tANI_U8 staId,
                          tCsrBssid bssId,
                          void * pContext,
                          void * pVosContext);
#endif

#if defined(FEATURE_WLAN_CCX) && defined(FEATURE_WLAN_CCX_UPLOAD)
eHalStatus csrGetTsmStats(tpAniSirGlobal pMac, tCsrTsmStatsCallback callback, tANI_U8 staId,
                              tCsrBssid bssId, void *pContext, void* pVosContext,
                              tANI_U8 tid);
#endif  /*                                             */

eHalStatus csrRoamRegisterCallback(tpAniSirGlobal pMac, csrRoamCompleteCallback callback, void *pContext);
/*                                                                            
                         
                                                                                          
                                           
                      
                                                                                 */
eHalStatus csrGetConfigParam(tpAniSirGlobal pMac, tCsrConfigParam *pParam);

/*                                                                            
                       
                                                                  
                                                                                   
                                           
                      
                                                                                 */
eHalStatus csrChangeDefaultConfigParam(tpAniSirGlobal pMac, tCsrConfigParam *pParam);


/*                                                                            
                       
                                                                            
                                                                                                    
                                                             
                      
                                                                                 */
eHalStatus csrMsgProcessor( tpAniSirGlobal pMac,  void *pMsgBuf );

/*                                                                            
               
                                                                   
                      
                                                                                 */
eHalStatus csrOpen(tpAniSirGlobal pMac);

/*                                                                            
                       
                                                                       
                      
                                                                                 */
eHalStatus csrInitChannels(tpAniSirGlobal pMac);

/*                                                                            
                
                                                                                                           
                      
                                                                                 */
eHalStatus csrClose(tpAniSirGlobal pMac);
/*                                                                            
                
                        
                      
                                                                                 */
eHalStatus csrStart(tpAniSirGlobal pMac);
/*                                                                            
               
                                                            
                      
                                                                                 */
eHalStatus csrStop(tpAniSirGlobal pMac, tHalStopType stopType);
/*                                                                            
                
                                         
                      
                                                                                 */
eHalStatus csrReady(tpAniSirGlobal pMac);

#ifdef FEATURE_WLAN_WAPI
eHalStatus csrRoamGetBKIDCache(tpAniSirGlobal pMac, tANI_U32 sessionId, tANI_U32 *pNum,
                                tBkidCacheInfo *pBkidCache);


eHalStatus csrScanGetBKIDCandidateList(tpAniSirGlobal pMac, tANI_U32 sessionId,
                                       tBkidCandidateInfo *pBkidList, tANI_U32 *pNumItems );
tANI_U32 csrRoamGetNumBKIDCache(tpAniSirGlobal pMac, tANI_U32 sessionId);
eHalStatus csrRoamSetBKIDCache( tpAniSirGlobal pMac, tANI_U32 sessionId, tBkidCacheInfo *pBKIDCache,
                                 tANI_U32 numItems );
/*                                                                            
                           
                                                                                   
                                                                                                            
                                
                                                                                        
                                                                                           
                                                                                 */
eHalStatus csrRoamGetWapiReqIE(tpAniSirGlobal pMac, tANI_U32 sessionId, tANI_U32 *pLen, tANI_U8 *pBuf);

/*                                                                            
                           
                                                                       
                                                                                                            
                                
                                                                                        
                                                                                           
                                                                                 */
eHalStatus csrRoamGetWapiRspIE(tpAniSirGlobal pMac, tANI_U32 sessionId, tANI_U32 *pLen, tANI_U8 *pBuf);
tANI_U8 csrConstructWapiIe( tpAniSirGlobal pMac, tANI_U32 sessionId, tCsrRoamProfile *pProfile,
                            tSirBssDescription *pSirBssDesc, tDot11fBeaconIEs *pIes, tCsrWapiIe *pWapiIe );
#endif /*                   */

eHalStatus csrRoamUpdateAPWPSIE( tpAniSirGlobal pMac, tANI_U32 sessionId, tSirAPWPSIEs *pAPWPSIES );
eHalStatus csrRoamUpdateWPARSNIEs( tpAniSirGlobal pMac, tANI_U32 sessionId, tSirRSNie * pAPSirRSNie);
void csrSetCfgPrivacy( tpAniSirGlobal pMac, tCsrRoamProfile *pProfile, tANI_BOOLEAN fPrivacy );
tANI_S8 csrGetInfraSessionId( tpAniSirGlobal pMac );
tANI_U8 csrGetInfraOperationChannel( tpAniSirGlobal pMac, tANI_U8 sessionId);
tANI_U8 csrGetConcurrentOperationChannel( tpAniSirGlobal pMac );

eHalStatus csrRoamCopyConnectProfile(tpAniSirGlobal pMac, tANI_U32 sessionId,
                               tCsrRoamConnectedProfile *pProfile);
tANI_BOOLEAN csrIsSetKeyAllowed(tpAniSirGlobal pMac, tANI_U32 sessionId);

void csrSetOppositeBandChannelInfo( tpAniSirGlobal pMac );
void csrConstructCurrentValidChannelList( tpAniSirGlobal pMac, tDblLinkList *pChannelSetList,
                                            tANI_U8 *pChannelList, tANI_U8 bSize, tANI_U8 *pNumChannels );

#ifdef FEATURE_WLAN_SCAN_PNO
eHalStatus csrScanSavePreferredNetworkFound(tpAniSirGlobal pMac,
            tSirPrefNetworkFoundInd *pPrefNetworkFoundInd);
#endif //                     
#endif

#ifdef WLAN_FEATURE_VOWIFI_11R
//                                                             
tANI_BOOLEAN csrRoamIs11rAssoc(tpAniSirGlobal pMac);
#endif

#ifdef FEATURE_WLAN_CCX
//                                                             
tANI_BOOLEAN csrRoamIsCCXAssoc(tpAniSirGlobal pMac);
tANI_BOOLEAN csrRoamIsCcxIniFeatureEnabled(tpAniSirGlobal pMac);
tANI_BOOLEAN csrNeighborRoamIsCCXAssoc(tpAniSirGlobal pMac);
#endif

//                                                     
//                          
void csrDisconnectAllActiveSessions(tpAniSirGlobal pMac);

#ifdef FEATURE_WLAN_LFR
//                                                         
tANI_BOOLEAN csrRoamIsFastRoamEnabled(tpAniSirGlobal pMac, tANI_U32 sessionId);
#ifdef WLAN_FEATURE_ROAM_SCAN_OFFLOAD
tANI_BOOLEAN csrRoamIsRoamOffloadScanEnabled(tpAniSirGlobal pMac);
#endif
tANI_BOOLEAN csrIsChannelPresentInList( tANI_U8 *pChannelList, int  numChannels, tANI_U8   channel );
VOS_STATUS csrAddToChannelListFront( tANI_U8 *pChannelList, int  numChannels, tANI_U8   channel );
#ifdef WLAN_FEATURE_ROAM_SCAN_OFFLOAD
eHalStatus csrScanRequestLfrResult(tpAniSirGlobal pMac, tANI_U32 sessionId,
                                   csrScanCompleteCallback callback, void *pContext);
eHalStatus csrRoamOffloadScanRspHdlr(tpAniSirGlobal pMac, tANI_U8 reason);
eHalStatus csrHandoffRequest(tpAniSirGlobal pMac, tCsrHandoffRequest *pHandoffInfo);
#endif
tANI_BOOLEAN csrRoamIsStaMode(tpAniSirGlobal pMac, tANI_U32 sessionId);
#endif


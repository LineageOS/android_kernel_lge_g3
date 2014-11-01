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

/** ------------------------------------------------------------------------- * 
    ------------------------------------------------------------------------- *  

  
    \file csrInsideApi.h
  
    Define interface only used by CSR.
  
  
   Copyright (C) 2006 Airgo Networks, Incorporated 
   ========================================================================== */
#ifndef CSR_INSIDE_API_H__
#define CSR_INSIDE_API_H__


#include "csrSupport.h"
#include "smeInside.h"
#include "vos_nvitem.h"

#define CSR_PASSIVE_MAX_CHANNEL_TIME   110
#define CSR_PASSIVE_MIN_CHANNEL_TIME   60

#define CSR_ACTIVE_MAX_CHANNEL_TIME    40
#define CSR_ACTIVE_MIN_CHANNEL_TIME    20

#define CSR_ACTIVE_MAX_CHANNEL_TIME_BTC    120
#define CSR_ACTIVE_MIN_CHANNEL_TIME_BTC    60

#ifdef WLAN_AP_STA_CONCURRENCY
#define CSR_PASSIVE_MAX_CHANNEL_TIME_CONC   110
#define CSR_PASSIVE_MIN_CHANNEL_TIME_CONC   60 

#define CSR_ACTIVE_MAX_CHANNEL_TIME_CONC    27
#define CSR_ACTIVE_MIN_CHANNEL_TIME_CONC    20

#define CSR_REST_TIME_CONC                  100

#define CSR_NUM_STA_CHAN_COMBINED_CONC      3
#define CSR_NUM_P2P_CHAN_COMBINED_CONC      1
#endif

#define CSR_MAX_NUM_SUPPORTED_CHANNELS 55

#define CSR_MAX_2_4_GHZ_SUPPORTED_CHANNELS 14

#define CSR_MAX_BSS_SUPPORT            250
#define SYSTEM_TIME_MSEC_TO_USEC      1000

//                                                                                               
//                 
#define CSR_AGING_COUNT     3   
//                                          
//                                                    
#define CSR_IDLE_SCAN_WAIT_TIME     (1 * PAL_TIMER_TO_SEC_UNIT)     //         
//                                                  
//                                                                                                     
#define CSR_IDLE_SCAN_NO_PS_INTERVAL     (10 * PAL_TIMER_TO_SEC_UNIT)     //          
#define CSR_IDLE_SCAN_NO_PS_INTERVAL_MIN (5 * PAL_TIMER_TO_SEC_UNIT)
#define CSR_SCAN_GET_RESULT_INTERVAL    (5 * PAL_TIMER_TO_SEC_UNIT)     //         
#define CSR_MIC_ERROR_TIMEOUT  (60 * PAL_TIMER_TO_SEC_UNIT)     //          
#define CSR_TKIP_COUNTER_MEASURE_TIMEOUT  (60 * PAL_TIMER_TO_SEC_UNIT)     //          
#define CSR_SCAN_RESULT_AGING_INTERVAL    (5 * PAL_TIMER_TO_SEC_UNIT)     //         
#define CSR_SCAN_RESULT_CFG_AGING_INTERVAL    (PAL_TIMER_TO_SEC_UNIT)     //          
//                                              
#define CSR_SCAN_AGING_TIME_NOT_CONNECT_NO_PS 50     //          
#define CSR_SCAN_AGING_TIME_NOT_CONNECT_W_PS 300     //           
#define CSR_SCAN_AGING_TIME_CONNECT_NO_PS 150        //           
#define CSR_SCAN_AGING_TIME_CONNECT_W_PS 600         //           
#define CSR_JOIN_FAILURE_TIMEOUT_DEFAULT ( 3000 )
#define CSR_JOIN_FAILURE_TIMEOUT_MIN   (1000)  //             
//                                                                        
#define CSR_BEST_RSSI_VALUE         (-30)   //                       
#define CSR_DEFAULT_RSSI_DB_GAP     30 //                             
#define CSR_BSS_CAP_VALUE_NONE  0    //              
#define CSR_BSS_CAP_VALUE_HT    2    
#define CSR_BSS_CAP_VALUE_WMM   1
#define CSR_BSS_CAP_VALUE_UAPSD 1
#if  defined (WLAN_FEATURE_VOWIFI_11R) || defined (FEATURE_WLAN_CCX) || defined(FEATURE_WLAN_LFR)
#define CSR_BSS_CAP_VALUE_5GHZ  1
#endif
#define CSR_DEFAULT_ROAMING_TIME 10   //          
#define CSR_ROAM_MIN(X, Y)  ((X) < (Y) ? (X) : (Y))
#define CSR_ROAM_MAX(X, Y)  ((X) > (Y) ? (X) : (Y))

#ifdef FEATURE_WLAN_BTAMP_UT_RF
#define CSR_JOIN_MAX_RETRY_COUNT             10
#define CSR_JOIN_RETRY_TIMEOUT_PERIOD        ( 1 *  PAL_TIMER_TO_SEC_UNIT )  //         
#endif

typedef enum 
{
    eCsrNextScanNothing,
    eCsrNextLostLinkScan1Success,
    eCsrNextLostLinkScan1Failed,
    eCsrNextLostLinkScan2Success,
    eCsrNextLostLinkScan2Failed,
    eCsrNextLostLinkScan3Success,
    eCsrNexteScanForSsidSuccess,
    eCsrNextLostLinkScan3Failed,
    eCsrNext11dScan1Failure,
    eCsrNext11dScan1Success,
    eCsrNext11dScan2Failure, 
    eCsrNext11dScan2Success,
    eCsrNext11dScanComplete,
    eCsrNexteScanForSsidFailure,
    eCsrNextIdleScanComplete,
    eCsrNextCapChangeScanComplete,

}eCsrScanCompleteNextCommand;

typedef enum  
{
    eCsrJoinSuccess, 
    eCsrJoinFailure,
    eCsrReassocSuccess,
    eCsrReassocFailure, 
    eCsrNothingToJoin, 
    eCsrStartBssSuccess,
    eCsrStartBssFailure,
    eCsrSilentlyStopRoaming,
    eCsrSilentlyStopRoamingSaveState,
    eCsrJoinWdsFailure,
    eCsrJoinFailureDueToConcurrency,
    
}eCsrRoamCompleteResult;

typedef struct tagScanReqParam
{
    tANI_U8 bReturnAfter1stMatch;
    tANI_U8 fUniqueResult;
    tANI_U8 freshScan;
    tANI_U8 hiddenSsid;
    tANI_U8 reserved;
}tScanReqParam;

typedef struct tagCsrScanResult
{
    tListElem Link;
    tANI_S32 AgingCount;    //                                             
    tANI_U32 preferValue;   //                                                                       
    tANI_U32 capValue;  //                                                                              
    //                                                                                                  
    //                                         
    
    eCsrEncryptionType ucEncryptionType; //                                                    
    eCsrEncryptionType mcEncryptionType; 
    eCsrAuthType authType; //                                                  

    tCsrScanResultInfo Result;
}tCsrScanResult;

typedef struct
{
    tDblLinkList List;
    tListElem *pCurEntry;
}tScanResultList;




#define CSR_IS_ROAM_REASON( pCmd, reason ) ( (reason) == (pCmd)->roamCmd.roamReason )
#define CSR_IS_BETTER_PREFER_VALUE(v1, v2)   ((v1) > (v2))
#define CSR_IS_EQUAL_PREFER_VALUE(v1, v2)   ((v1) == (v2))
#define CSR_IS_BETTER_CAP_VALUE(v1, v2)     ((v1) > (v2))
#define CSR_IS_ENC_TYPE_STATIC( encType ) ( ( eCSR_ENCRYPT_TYPE_NONE == (encType) ) || \
                                            ( eCSR_ENCRYPT_TYPE_WEP40_STATICKEY == (encType) ) || \
                                            ( eCSR_ENCRYPT_TYPE_WEP104_STATICKEY == (encType) ) )
#define CSR_IS_WAIT_FOR_KEY( pMac, sessionId ) ( CSR_IS_ROAM_JOINED( pMac, sessionId ) && CSR_IS_ROAM_SUBSTATE_WAITFORKEY( pMac, sessionId ) )
//                                                                   
//                                              

#define CSR_IS_11n_ALLOWED( encType ) (( eCSR_ENCRYPT_TYPE_TKIP != (encType) ) && \
                                      ( eCSR_ENCRYPT_TYPE_WEP40_STATICKEY != (encType) ) && \
                                      ( eCSR_ENCRYPT_TYPE_WEP104_STATICKEY != (encType) ) && \
                                      ( eCSR_ENCRYPT_TYPE_WEP40 != (encType) ) && \
                                      ( eCSR_ENCRYPT_TYPE_WEP104 != (encType) ) )

eCsrRoamState csrRoamStateChange( tpAniSirGlobal pMac, eCsrRoamState NewRoamState, tANI_U8 sessionId);
eHalStatus csrScanningStateMsgProcessor( tpAniSirGlobal pMac, void *pMsgBuf );
void csrRoamingStateMsgProcessor( tpAniSirGlobal pMac, void *pMsgBuf );
void csrRoamJoinedStateMsgProcessor( tpAniSirGlobal pMac, void *pMsgBuf );
tANI_BOOLEAN csrScanComplete( tpAniSirGlobal pMac, tSirSmeScanRsp *pScanRsp );
void csrReleaseCommandRoam(tpAniSirGlobal pMac, tSmeCmd *pCommand);
void csrReleaseCommandScan(tpAniSirGlobal pMac, tSmeCmd *pCommand);
void csrReleaseCommandWmStatusChange(tpAniSirGlobal pMac, tSmeCmd *pCommand);
//                 
tANI_BOOLEAN csrIsDuplicateBssDescription( tpAniSirGlobal pMac, tSirBssDescription *pSirBssDesc1, 
                                           tSirBssDescription *pSirBssDesc2, tDot11fBeaconIEs *pIes2, tANI_BOOLEAN fForced );
eHalStatus csrRoamSaveConnectedBssDesc( tpAniSirGlobal pMac, tANI_U32 sessionId, tSirBssDescription *pBssDesc );
tANI_BOOLEAN csrIsNetworkTypeEqual( tSirBssDescription *pSirBssDesc1, tSirBssDescription *pSirBssDesc2 );
eHalStatus csrScanSmeScanResponse( tpAniSirGlobal pMac, void *pMsgBuf );
/*
                                                                   
                                                                 
                                               
*/
eHalStatus csrRoamPrepareFilterFromProfile(tpAniSirGlobal pMac, tCsrRoamProfile *pProfile, tCsrScanResultFilter *pScanFilter);
eHalStatus csrRoamCopyProfile(tpAniSirGlobal pMac, tCsrRoamProfile *pDstProfile, tCsrRoamProfile *pSrcProfile);
eHalStatus csrRoamStart(tpAniSirGlobal pMac);
void csrRoamStop(tpAniSirGlobal pMac, tANI_U32 sessionId);
void csrRoamStartMICFailureTimer(tpAniSirGlobal pMac);
void csrRoamStopMICFailureTimer(tpAniSirGlobal pMac);
void csrRoamStartTKIPCounterMeasureTimer(tpAniSirGlobal pMac);
void csrRoamStopTKIPCounterMeasureTimer(tpAniSirGlobal pMac);

eHalStatus csrScanOpen(tpAniSirGlobal pMac);
eHalStatus csrScanClose(tpAniSirGlobal pMac);
eHalStatus csrScanRequestLostLink1( tpAniSirGlobal pMac, tANI_U32 sessionId );
eHalStatus csrScanRequestLostLink2( tpAniSirGlobal pMac, tANI_U32 sessionId );
eHalStatus csrScanRequestLostLink3( tpAniSirGlobal pMac, tANI_U32 sessionId );
eHalStatus csrScanHandleFailedLostlink1(tpAniSirGlobal pMac, tANI_U32 sessionId);
eHalStatus csrScanHandleFailedLostlink2(tpAniSirGlobal pMac, tANI_U32 sessionId);
eHalStatus csrScanHandleFailedLostlink3(tpAniSirGlobal pMac, tANI_U32 sessionId);
tCsrScanResult *csrScanAppendBssDescription( tpAniSirGlobal pMac, 
                                             tSirBssDescription *pSirBssDescription,
                                             tDot11fBeaconIEs *pIes, tANI_BOOLEAN fForced);
void csrScanCallCallback(tpAniSirGlobal pMac, tSmeCmd *pCommand, eCsrScanStatus scanStatus);
eHalStatus csrScanCopyRequest(tpAniSirGlobal pMac, tCsrScanRequest *pDstReq, tCsrScanRequest *pSrcReq);
eHalStatus csrScanFreeRequest(tpAniSirGlobal pMac, tCsrScanRequest *pReq);
eHalStatus csrScanCopyResultList(tpAniSirGlobal pMac, tScanResultHandle hIn, tScanResultHandle *phResult);
void csrInitBGScanChannelList(tpAniSirGlobal pMac);
eHalStatus csrScanForSSID(tpAniSirGlobal pMac, tANI_U32 sessionId, tCsrRoamProfile *pProfile, tANI_U32 roamId, tANI_BOOLEAN notify);
eHalStatus csrScanForCapabilityChange(tpAniSirGlobal pMac, tSirSmeApNewCaps *pNewCaps);
eHalStatus csrScanStartGetResultTimer(tpAniSirGlobal pMac);
eHalStatus csrScanStopGetResultTimer(tpAniSirGlobal pMac);
eHalStatus csrScanStartResultAgingTimer(tpAniSirGlobal pMac);
eHalStatus csrScanStopResultAgingTimer(tpAniSirGlobal pMac);
eHalStatus csrScanStartResultCfgAgingTimer(tpAniSirGlobal pMac);
eHalStatus csrScanStopResultCfgAgingTimer(tpAniSirGlobal pMac);
eHalStatus csrScanBGScanEnable(tpAniSirGlobal pMac);
eHalStatus csrScanStartIdleScanTimer(tpAniSirGlobal pMac, tANI_U32 interval);
eHalStatus csrScanStopIdleScanTimer(tpAniSirGlobal pMac);
eHalStatus csrScanStartIdleScan(tpAniSirGlobal pMac);
//                                                                                                           
//                        
//                                                                                                
eHalStatus csrScanTriggerIdleScan(tpAniSirGlobal pMac, tANI_U32 *pTimeInterval);
void csrScanCancelIdleScan(tpAniSirGlobal pMac);
void csrScanStopTimers(tpAniSirGlobal pMac);
//                                                                                   
tANI_BOOLEAN csrScanRemoveNotRoamingScanCommand(tpAniSirGlobal pMac);
//                                                    
tANI_BOOLEAN csrScanRemoveFreshScanCommand(tpAniSirGlobal pMac, tANI_U8 sessionId);
eHalStatus csrScanAbortMacScan(tpAniSirGlobal pMac);
void csrRemoveCmdFromPendingList(tpAniSirGlobal pMac, tDblLinkList *pList, 
                                              eSmeCommandType commandType );
eHalStatus csrScanAbortMacScanNotForConnect(tpAniSirGlobal pMac);
eHalStatus csrScanGetScanChannelInfo(tpAniSirGlobal pMac);
//                                                                                 
//                                               
//                                                                                
//                                                                                   
//                                         
eHalStatus csrScanAgeResults(tpAniSirGlobal pMac, tSmeGetScanChnRsp *pScanChnInfo);

//                                                              
//                                                                 
tANI_BOOLEAN csrLearnCountryInformation( tpAniSirGlobal pMac, tSirBssDescription *pSirBssDesc,
                                         tDot11fBeaconIEs *pIes, tANI_BOOLEAN fForce );
void csrApplyCountryInformation( tpAniSirGlobal pMac, tANI_BOOLEAN fForce );
void csrSetCfgScanControlList( tpAniSirGlobal pMac, tANI_U8 *countryCode, tCsrChannel *pChannelList  );
void csrReinitScanCmd(tpAniSirGlobal pMac, tSmeCmd *pCommand);
void csrFreeScanResultEntry( tpAniSirGlobal pMac, tCsrScanResult *pResult );

eHalStatus csrRoamCallCallback(tpAniSirGlobal pMac, tANI_U32 sessionId, tCsrRoamInfo *pRoamInfo, 
                               tANI_U32 roamId, eRoamCmdStatus u1, eCsrRoamResult u2);
eHalStatus csrRoamIssueConnect(tpAniSirGlobal pMac, tANI_U32 sessionId, tCsrRoamProfile *pProfile, 
                               tScanResultHandle hBSSList, 
                               eCsrRoamReason reason, tANI_U32 roamId, 
                               tANI_BOOLEAN fImediate, tANI_BOOLEAN fClearScan);
eHalStatus csrRoamIssueReassoc(tpAniSirGlobal pMac, tANI_U32 sessionId, tCsrRoamProfile *pProfile,
                               tCsrRoamModifyProfileFields *pModProfileFields,
                               eCsrRoamReason reason, tANI_U32 roamId, tANI_BOOLEAN fImediate);
void csrRoamComplete( tpAniSirGlobal pMac, eCsrRoamCompleteResult Result, void *Context );
eHalStatus csrRoamIssueSetContextReq( tpAniSirGlobal pMac, tANI_U32 sessionId, eCsrEncryptionType EncryptType, 
                                     tSirBssDescription *pBssDescription,
                                tSirMacAddr *bssId, tANI_BOOLEAN addKey,
                                 tANI_BOOLEAN fUnicast, tAniKeyDirection aniKeyDirection, 
                                 tANI_U8 keyId, tANI_U16 keyLength, 
                                 tANI_U8 *pKey, tANI_U8 paeRole );
eHalStatus csrRoamProcessDisassocDeauth( tpAniSirGlobal pMac, tSmeCmd *pCommand, 
                                         tANI_BOOLEAN fDisassoc, tANI_BOOLEAN fMICFailure );
eHalStatus csrRoamSaveConnectedInfomation(tpAniSirGlobal pMac, tANI_U32 sessionId, tCsrRoamProfile *pProfile, 
                                          tSirBssDescription *pSirBssDesc, tDot11fBeaconIEs *pIes);
void csrRoamCheckForLinkStatusChange( tpAniSirGlobal pMac, tSirSmeRsp *pSirMsg );
void csrRoamStatsRspProcessor(tpAniSirGlobal pMac, tSirSmeRsp *pSirMsg);
eHalStatus csrRoamIssueStartBss( tpAniSirGlobal pMac, tANI_U32 sessionId, tCsrRoamStartBssParams *pParam, 
                                 tCsrRoamProfile *pProfile, tSirBssDescription *pBssDesc, tANI_U32 roamId );
eHalStatus csrRoamIssueStopBss( tpAniSirGlobal pMac, tANI_U32 sessionId, eCsrRoamSubState NewSubstate );
tANI_BOOLEAN csrIsSameProfile(tpAniSirGlobal pMac, tCsrRoamConnectedProfile *pProfile1, tCsrRoamProfile *pProfile2);
tANI_BOOLEAN csrIsRoamCommandWaiting(tpAniSirGlobal pMac);
tANI_BOOLEAN csrIsRoamCommandWaitingForSession(tpAniSirGlobal pMac, tANI_U32 sessionId);
tANI_BOOLEAN csrIsScanForRoamCommandActive( tpAniSirGlobal pMac );
eRoamCmdStatus csrGetRoamCompleteStatus(tpAniSirGlobal pMac, tANI_U32 sessionId);
//                                                  
//                                                                                                                                  
eHalStatus csrRoamIssueDisassociateCmd( tpAniSirGlobal pMac, tANI_U32 sessionId, eCsrRoamDisconnectReason reason );
eHalStatus csrRoamDisconnectInternal(tpAniSirGlobal pMac, tANI_U32 sessionId, eCsrRoamDisconnectReason reason);
//                    
void csrRoamRemoveDuplicateCommand(tpAniSirGlobal pMac, tANI_U32 sessionId, tSmeCmd *pCommand, eCsrRoamReason eRoamReason);
                                 
eHalStatus csrSendJoinReqMsg( tpAniSirGlobal pMac, tANI_U32 sessionId, tSirBssDescription *pBssDescription, 
                              tCsrRoamProfile *pProfile, tDot11fBeaconIEs *pIes, tANI_U16 messageType );
eHalStatus csrSendMBDisassocReqMsg( tpAniSirGlobal pMac, tANI_U32 sessionId, tSirMacAddr bssId, tANI_U16 reasonCode );
eHalStatus csrSendMBDeauthReqMsg( tpAniSirGlobal pMac, tANI_U32 sessionId, tSirMacAddr bssId, tANI_U16 reasonCode );
eHalStatus csrSendMBDisassocCnfMsg( tpAniSirGlobal pMac, tpSirSmeDisassocInd pDisassocInd );
eHalStatus csrSendMBDeauthCnfMsg( tpAniSirGlobal pMac, tpSirSmeDeauthInd pDeauthInd );
eHalStatus csrSendAssocCnfMsg( tpAniSirGlobal pMac, tpSirSmeAssocInd pAssocInd, eHalStatus status );
eHalStatus csrSendAssocIndToUpperLayerCnfMsg( tpAniSirGlobal pMac, tpSirSmeAssocInd pAssocInd, eHalStatus Halstatus, tANI_U8 sessionId );
eHalStatus csrSendMBStartBssReqMsg( tpAniSirGlobal pMac, tANI_U32 sessionId, eCsrRoamBssType bssType, 
                                    tCsrRoamStartBssParams *pParam, tSirBssDescription *pBssDesc );
eHalStatus csrSendMBStopBssReqMsg( tpAniSirGlobal pMac, tANI_U32 sessionId );

tANI_BOOLEAN csrIsMacAddressEqual( tpAniSirGlobal pMac, tCsrBssid *pMacAddr1, tCsrBssid *pMacAddr2 );
//                                                                               
tANI_BOOLEAN csrIsSsidMatch( tpAniSirGlobal pMac, tANI_U8 *ssid1, tANI_U8 ssid1Len, tANI_U8 *bssSsid, 
                            tANI_U8 bssSsidLen, tANI_BOOLEAN fSsidRequired );
tANI_BOOLEAN csrIsPhyModeMatch( tpAniSirGlobal pMac, tANI_U32 phyMode,
                                    tSirBssDescription *pSirBssDesc, tCsrRoamProfile *pProfile,
                                    eCsrCfgDot11Mode *pReturnCfgDot11Mode,
                                    tDot11fBeaconIEs *pIes);
tANI_BOOLEAN csrRoamIsChannelValid( tpAniSirGlobal pMac, tANI_U8 channel );

//                                                              
eHalStatus csrGetCfgValidChannels(tpAniSirGlobal pMac, tANI_U8 *pChannels, tANI_U32 *pNumChan);
void csrRoamCcmCfgSetCallback(tHalHandle hHal, tANI_S32 result);
void csrScanCcmCfgSetCallback(tHalHandle hHal, tANI_S32 result);

tPowerdBm csrGetCfgMaxTxPower (tpAniSirGlobal pMac, tANI_U8 channel);

//                                
void csrFreeRoamProfile(tpAniSirGlobal pMac, tANI_U32 sessionId);
void csrFreeConnectBssDesc(tpAniSirGlobal pMac, tANI_U32 sessionId);
eHalStatus csrMoveBssToHeadFromBSSID(tpAniSirGlobal pMac, tCsrBssid *bssid, tScanResultHandle hScanResult);
tANI_BOOLEAN csrCheckPSReady(void *pv);
void csrFullPowerCallback(void *pv, eHalStatus status);
//                                                     
void csrReleaseProfile(tpAniSirGlobal pMac, tCsrRoamProfile *pProfile);
//                                          
void csrFreeScanFilter(tpAniSirGlobal pMac, tCsrScanResultFilter *pScanFilter);
eCsrCfgDot11Mode csrGetCfgDot11ModeFromCsrPhyMode(tCsrRoamProfile *pProfile, eCsrPhyMode phyMode, tANI_BOOLEAN fProprietary);
tANI_U32 csrTranslateToWNICfgDot11Mode(tpAniSirGlobal pMac, eCsrCfgDot11Mode csrDot11Mode);
void csrSaveChannelPowerForBand( tpAniSirGlobal pMac, tANI_BOOLEAN fPopulate5GBand );
void csrApplyChannelPowerCountryInfo( tpAniSirGlobal pMac, tCsrChannel *pChannelList, tANI_U8 *countryCode, tANI_BOOLEAN updateRiva);
void csrApplyPower2Current( tpAniSirGlobal pMac );
void csrAssignRssiForCategory(tpAniSirGlobal pMac, tANI_S8 bestApRssi, tANI_U8 catOffset);
tANI_BOOLEAN csrIsMacAddressZero( tpAniSirGlobal pMac, tCsrBssid *pMacAddr );
tANI_BOOLEAN csrIsMacAddressBroadcast( tpAniSirGlobal pMac, tCsrBssid *pMacAddr );
eHalStatus csrRoamRemoveConnectedBssFromScanCache(tpAniSirGlobal pMac, tCsrRoamConnectedProfile *pConnProfile);
eHalStatus csrRoamStartRoaming(tpAniSirGlobal pMac, tANI_U32 sessionId, eCsrRoamingReason roamingReason);
//                                                                   
tANI_BOOLEAN csrRoamCompleteRoaming(tpAniSirGlobal pMac, tANI_U32 sessionId,
                                    tANI_BOOLEAN fForce, eCsrRoamResult roamResult);
void csrRoamCompletion(tpAniSirGlobal pMac, tANI_U32 sessionId, tCsrRoamInfo *pRoamInfo, tSmeCmd *pCommand, eCsrRoamResult roamResult, tANI_BOOLEAN fSuccess);
void csrRoamCancelRoaming(tpAniSirGlobal pMac, tANI_U32 sessionId);
void csrResetCountryInformation( tpAniSirGlobal pMac, tANI_BOOLEAN fForce, tANI_BOOLEAN updateRiva );
void csrResetPMKIDCandidateList( tpAniSirGlobal pMac, tANI_U32 sessionId );
#ifdef FEATURE_WLAN_WAPI
void csrResetBKIDCandidateList( tpAniSirGlobal pMac, tANI_U32 sessionId );
#endif /*                   */
eHalStatus csrSaveToChannelPower2G_5G( tpAniSirGlobal pMac, tANI_U32 tableSize, tSirMacChanInfo *channelTable );
//                                                                
eHalStatus csrScanGetSupportedChannels( tpAniSirGlobal pMac );
//                                                         
//                                                        
//                                                                                     
//                                
//                                           
//                                                                 
//                   
tANI_BOOLEAN csrMatchCountryCode( tpAniSirGlobal pMac, tANI_U8 *pCountry, tDot11fBeaconIEs *pIes );
eHalStatus csrRoamSetKey( tpAniSirGlobal pMac, tANI_U32 sessionId, tCsrRoamSetKey *pSetKey, tANI_U32 roamId );
eHalStatus csrRoamOpenSession(tpAniSirGlobal pMac,
                              csrRoamCompleteCallback callback,
                              void *pContext, tANI_U8 *pSelfMacAddr,
                              tANI_U8 *pbSessionId);
//                                                       
eHalStatus csrRoamCloseSession( tpAniSirGlobal pMac, tANI_U32 sessionId,
                                tANI_BOOLEAN fSync, 
                                csrRoamSessionCloseCallback callback,
                                void *pContext );
void csrCleanupSession(tpAniSirGlobal pMac, tANI_U32 sessionId);
eHalStatus csrRoamGetSessionIdFromBSSID( tpAniSirGlobal pMac, tCsrBssid *bssid, tANI_U32 *pSessionId );
eCsrCfgDot11Mode csrFindBestPhyMode( tpAniSirGlobal pMac, tANI_U32 phyMode );

/*                                                                            
                     
                                                                                                          
                                          
                           
                                                                                 */
eHalStatus csrScanEnable(tpAniSirGlobal);

/*                                                                            
                      
                                                                                                                  
                              
                                          
                           
                                                                                 */
eHalStatus csrScanDisable(tpAniSirGlobal);
/*                                                                            
                      
                                      
                                                                           
                                                                                                                         
                                                          
                           
                                                                                 */
eHalStatus csrScanRequest(tpAniSirGlobal, tANI_U16, tCsrScanRequest *,
                   tANI_U32 *pScanRequestID, csrScanCompleteCallback callback,
                   void *pContext);

/*                                                                            
                    
                                                                                                                   
                                                                       
                           
                                                                                 */
eHalStatus csrScanAbort(tpAniSirGlobal, tANI_U32 scanRequestID);

eHalStatus csrScanSetBGScanparams(tpAniSirGlobal, tCsrBGScanRequest *);
eHalStatus csrScanBGScanAbort(tpAniSirGlobal);

/*                                                                            
                        
                               
                                                                        
                                               
                           
                                                                                 */
eHalStatus csrScanGetResult(tpAniSirGlobal, tCsrScanResultFilter *pFilter, tScanResultHandle *phResult);

/*                                                                            
                          
                              
                           
                                                                                 */
eHalStatus csrScanFlushResult(tpAniSirGlobal);
eHalStatus csrScanFlushSelectiveResult(tpAniSirGlobal, v_BOOL_t flushP2P);
/*                                                                            
                             
                                                        
                           
                                                                                 */
eHalStatus csrScanBGScanGetParam(tpAniSirGlobal, tCsrBGScanRequest *);

/*                                                                            
                             
                                                    
                                                       
                                                         
                                                                                 */
tCsrScanResultInfo *csrScanResultGetFirst(tpAniSirGlobal, tScanResultHandle hScanResult);
/*                                                                            
                            
                                                                                                                
                                                       
                                                   
                                                                                 */
tCsrScanResultInfo *csrScanResultGetNext(tpAniSirGlobal, tScanResultHandle hScanResult);

/*                                                                            
                         
                                                                      
                                                                                                               
                                                                                                    
                                                
                           
                                                                                 */
eHalStatus csrGetCountryCode(tpAniSirGlobal pMac, tANI_U8 *pBuf, tANI_U8 *pbLen);

/*                                                                            
                         
                                                                                                        
                             
                                                                                               
                                                                                                               
                                           
                           
                                                                                 */
eHalStatus csrSetCountryCode(tpAniSirGlobal pMac, tANI_U8 *pCountry, tANI_BOOLEAN *pfRestartNeeded);

/*                                                                            
                                      
                                                                                               
                                                 
                                                                                                               
                                           
                           
                                                                                 */
eHalStatus csrResetCountryCodeInformation(tpAniSirGlobal pMac, tANI_BOOLEAN *pfRestartNeeded);

/*                                                                            
                                  
                                                                                     
                                                                                      
                                                                                      
                                          
                                                                                                    
                                                
                           
                                                                                 */
eHalStatus csrGetSupportedCountryCode(tpAniSirGlobal pMac, tANI_U8 *pBuf, tANI_U32 *pbLen);

/*                                                                            
                              
                                                                 
                                                                                                          
        
                                                                                    
                                   
                                                                                                               
                                           
                           
                                                                                 */
eHalStatus csrSetRegulatoryDomain(tpAniSirGlobal pMac, v_REGDOMAIN_t domainId, tANI_BOOLEAN *pfRestartNeeded);

/*                                                                            
                                     
                                                                 
                                                                                                          
        
                             
                                                                                 */
v_REGDOMAIN_t csrGetCurrentRegulatoryDomain(tpAniSirGlobal pMac);

/*                                                                            
                                        
                                                                       
                                                                                                          
        
                                                                                               
                                                                                                            
                                                      
                           
                                                                                 */
eHalStatus csrGetRegulatoryDomainForCountry(tpAniSirGlobal pMac,
                                            tANI_U8 *pCountry,
                                            v_REGDOMAIN_t *pDomainId,
                                            v_CountryInfoSource_t source);


tANI_BOOLEAN csrSave11dCountryString( tpAniSirGlobal pMac, tANI_U8 *pCountryCode, tANI_BOOLEAN fForce );

//                      
tANI_BOOLEAN csrIs11dSupported(tpAniSirGlobal pMac);
tANI_BOOLEAN csrIs11hSupported(tpAniSirGlobal pMac);
tANI_BOOLEAN csrIs11eSupported(tpAniSirGlobal pMac);
tANI_BOOLEAN csrIsWmmSupported(tpAniSirGlobal pMac);
tANI_BOOLEAN csrIsMCCSupported(tpAniSirGlobal pMac);

//                                                                                        
eHalStatus csrScanGetBaseChannels( tpAniSirGlobal pMac, tCsrChannelInfo * pChannelInfo );
//                                               
eHalStatus csrQueueSmeCommand( tpAniSirGlobal pMac, tSmeCmd *pCommand, tANI_BOOLEAN fHighPriority );
tSmeCmd *csrGetCommandBuffer( tpAniSirGlobal pMac );
void csrReleaseCommand(tpAniSirGlobal pMac, tSmeCmd *pCommand);
#ifdef FEATURE_WLAN_WAPI
tANI_BOOLEAN csrIsProfileWapi( tCsrRoamProfile *pProfile );
#endif /*                   */

#ifdef FEATURE_WLAN_DIAG_SUPPORT_CSR

//        
#define WLAN_SECURITY_EVENT_SET_PTK_REQ     1
#define WLAN_SECURITY_EVENT_SET_PTK_RSP     2
#define WLAN_SECURITY_EVENT_SET_GTK_REQ     3
#define WLAN_SECURITY_EVENT_SET_GTK_RSP     4
#define WLAN_SECURITY_EVENT_REMOVE_KEY_REQ  5
#define WLAN_SECURITY_EVENT_REMOVE_KEY_RSP  6
#define WLAN_SECURITY_EVENT_PMKID_CANDIDATE_FOUND  7
#define WLAN_SECURITY_EVENT_PMKID_UPDATE    8
#define WLAN_SECURITY_EVENT_MIC_ERROR       9   

#define AUTH_OPEN       0
#define AUTH_SHARED     1
#define AUTH_WPA_EAP    2
#define AUTH_WPA_PSK    3
#define AUTH_WPA2_EAP   4
#define AUTH_WPA2_PSK   5
#ifdef FEATURE_WLAN_WAPI
#define AUTH_WAPI_CERT  6
#define AUTH_WAPI_PSK   7
#endif /*                   */

#define ENC_MODE_OPEN   0
#define ENC_MODE_WEP40  1
#define ENC_MODE_WEP104 2
#define ENC_MODE_TKIP   3
#define ENC_MODE_AES    4
#ifdef FEATURE_WLAN_WAPI
#define ENC_MODE_SMS4   5 //    
#endif /*                   */

#define NO_MATCH    0
#define MATCH       1

#define WLAN_SECURITY_STATUS_SUCCESS        0
#define WLAN_SECURITY_STATUS_FAILURE        1

//    
#define WLAN_SCAN_EVENT_ACTIVE_SCAN_REQ     1
#define WLAN_SCAN_EVENT_ACTIVE_SCAN_RSP     2
#define WLAN_SCAN_EVENT_PASSIVE_SCAN_REQ    3
#define WLAN_SCAN_EVENT_PASSIVE_SCAN_RSP    4
#define WLAN_SCAN_EVENT_HO_SCAN_REQ         5
#define WLAN_SCAN_EVENT_HO_SCAN_RSP         6

#define WLAN_SCAN_STATUS_SUCCESS        0
#define WLAN_SCAN_STATUS_FAILURE        1
#define WLAN_SCAN_STATUS_ABORT          2

//    
#define WLAN_IBSS_EVENT_START_IBSS_REQ      0
#define WLAN_IBSS_EVENT_START_IBSS_RSP      1
#define WLAN_IBSS_EVENT_JOIN_IBSS_REQ       2
#define WLAN_IBSS_EVENT_JOIN_IBSS_RSP       3
#define WLAN_IBSS_EVENT_COALESCING          4
#define WLAN_IBSS_EVENT_PEER_JOIN           5
#define WLAN_IBSS_EVENT_PEER_LEAVE          6
#define WLAN_IBSS_EVENT_STOP_REQ            7
#define WLAN_IBSS_EVENT_STOP_RSP            8

#define AUTO_PICK       0
#define SPECIFIED       1

#define WLAN_IBSS_STATUS_SUCCESS        0
#define WLAN_IBSS_STATUS_FAILURE        1

//   
#define WLAN_80211D_EVENT_COUNTRY_SET   0
#define WLAN_80211D_EVENT_RESET         1

#define WLAN_80211D_DISABLED         0
#define WLAN_80211D_SUPPORT_MULTI_DOMAIN     1
#define WLAN_80211D_NOT_SUPPORT_MULTI_DOMAIN     2

int diagAuthTypeFromCSRType(eCsrAuthType authType);
int diagEncTypeFromCSRType(eCsrEncryptionType encType);
#endif //                                    
/*                                                                            
                          
                                                                                     
                                                                                           
                                                                
                           
                                                                                 */
eHalStatus csrScanResultPurge(tpAniSirGlobal pMac, tScanResultHandle hScanResult);


//                                                         

/*                                                                            
                      
                                       
                                                          
                                                                                                 
                                               
                           
                                                                                 */
eHalStatus csrRoamConnect(tpAniSirGlobal pMac, tANI_U32 sessionId, tCsrRoamProfile *pProfile, 
                          tScanResultHandle hBssListIn, tANI_U32 *pRoamId);

/*                                                                            
                      
                                         
                                                                              
                                                                                
             
                                                                                
                                                                               
                           
                                               
                           
                                                                                 */
eHalStatus csrRoamReassoc(tpAniSirGlobal pMac, tANI_U32 sessionId, tCsrRoamProfile *pProfile,
                          tCsrRoamModifyProfileFields modProfileFields,
                          tANI_U32 *pRoamId);


/*                                                                            
                        
                                                            
                                                                       
                                                                                 */
eHalStatus csrRoamReconnect(tpAniSirGlobal pMac, tANI_U32 sessionId);

/*                                                                            
                            
                                          
                                                                                     
                                                                                 
                                                                                            
                                                                                                         
                                      
                                                                                            
                                                                                 */
eHalStatus csrRoamSetPMKIDCache( tpAniSirGlobal pMac, tANI_U32 sessionId, tPmkidCacheInfo *pPMKIDCache, tANI_U32 numItems );

/*                                                                            
                             
                                                                                         
                                                                                                             
                                
                                                                                        
                                                                                           
                                                                                 */
eHalStatus csrRoamGetWpaRsnReqIE(tpAniSirGlobal pMac, tANI_U32 sessionId, tANI_U32 *pLen, tANI_U8 *pBuf);

/*                                                                            
                             
                                                                             
                                                                                                             
                                
                                                                                        
                                                                                           
                                                                                 */
eHalStatus csrRoamGetWpaRsnRspIE(tpAniSirGlobal pMac, tANI_U32 sessionId, tANI_U32 *pLen, tANI_U8 *pBuf);


/*                                                                            
                               
                                               
                                                        
                                                                                 */
tANI_U32 csrRoamGetNumPMKIDCache(tpAniSirGlobal pMac, tANI_U32 sessionId);

/*                                                                            
                            
                                      
                                                                                                                                          
                                                 
                                                                                               
                                                                                           
                                                                                 */
eHalStatus csrRoamGetPMKIDCache(tpAniSirGlobal pMac, tANI_U32 sessionId, 
                                tANI_U32 *pNum, tPmkidCacheInfo *pPmkidCache);

/*                                                                            
                                
                                                                                            
                                                                                       
                                                                                      
                                                     
                                                                                 */
eHalStatus csrRoamGetConnectProfile(tpAniSirGlobal pMac, tANI_U32 sessionId, 
                                    tCsrRoamConnectedProfile *pProfile);

/*                                                                            
                              
                                                         
                           
                                                                                 */
eHalStatus csrRoamGetConnectState(tpAniSirGlobal pMac, tANI_U32 sessionId, eCsrConnectState *pState);

/*                                                                            
                                 
                                                                                            
                                                                                      
                             
                                                                                 */
eHalStatus csrRoamFreeConnectProfile(tpAniSirGlobal pMac, tCsrRoamConnectedProfile *pProfile);

/*                                                                            
                          
                                                                                                        
                                                                                                          
        
                           
                                                                                 */
eHalStatus csrInitChannelList( tHalHandle hHal );

/*                                                                            
                             
                                                                              
                                
                                                                          
                                                                              
                                                                            
                                                                         
           
                                                                                
                                       
                                                                               
                                                                              
                                                                               
                                                                         

                           
                                                                             */
eHalStatus csrChangeConfigParams(tpAniSirGlobal pMac, 
                                 tCsrUpdateConfigParam *pUpdateConfigParam);

/*                                                                            
                                   
                                                            
                                                                   
                                                                                 */
eHalStatus csrRoamConnectToLastProfile(tpAniSirGlobal pMac, tANI_U32 sessionId);

/*                                                                            
                         
                                       
                                                                                                                           
                           
                                                                                 */
eHalStatus csrRoamDisconnect(tpAniSirGlobal pMac, tANI_U32 sessionId, eCsrRoamDisconnectReason reason);

/*                                                                            
                                    
                                          
                                                                                        
                                                                                                 
                                                                                           
                                                                                                         
                                          
                                                                                                    
                                                                                 */
eHalStatus csrScanGetPMKIDCandidateList(tpAniSirGlobal pMac, tANI_U32 sessionId,
                                        tPmkidCandidateInfo *pPmkidList, tANI_U32 *pNumItems );

//                                                                                              
//                                                            
eHalStatus csrRoamIssueStopBssCmd( tpAniSirGlobal pMac, tANI_U32 sessionId, tANI_BOOLEAN fHighPriority );

void csrCallRoamingCompletionCallback(tpAniSirGlobal pMac, tCsrRoamSession *pSession, 
                                      tCsrRoamInfo *pRoamInfo, tANI_U32 roamId, eCsrRoamResult roamResult);

/*                                                                            
                                      
                                                                           
                                                
                                                             
                                                                 
                      
                                                                             */
eHalStatus csrRoamIssueDisassociateStaCmd( tpAniSirGlobal pMac, 
                                           tANI_U32 sessionId, 
                                           tANI_U8 *pPeerMacAddr,
                                           tANI_U32 reason);

/*                                                                            
                             
                                                                     
                                                
                                                             
                                                                 
                      
                                                                             */
eHalStatus csrRoamIssueDeauthStaCmd( tpAniSirGlobal pMac, 
                                     tANI_U32 sessionId, 
                                     tANI_U8 *pPeerMacAddr,
                                     tANI_U32 reason);

/*                                                                            
                                       
                                                                             
                                             
                                                         
                      
                                                                             */
eHalStatus csrRoamIssueTkipCounterMeasures( tpAniSirGlobal pMac, tANI_U32 sessionId, tANI_BOOLEAN bEnable);

eHalStatus csrSendMBTkipCounterMeasuresReqMsg( tpAniSirGlobal pMac, tANI_U32 sessinId, tANI_BOOLEAN bEnable, tSirMacAddr bssId );

/*                                                                            
                                
                                                                                            
                                             
                                        
                                           
                                                          
                                                                                            
                      
                                                                             */
eHalStatus csrRoamGetAssociatedStas( tpAniSirGlobal pMac, tANI_U32 sessionId, VOS_MODULE_ID modId,
                                     void *pUsrContext, void *pfnSapEventCallback, tANI_U8 *pAssocStasBuf );

eHalStatus csrSendMBGetAssociatedStasReqMsg( tpAniSirGlobal pMac, tANI_U32 sessionId, VOS_MODULE_ID modId,  tSirMacAddr bssId,
                                             void *pUsrContext, void *pfnSapEventCallback, tANI_U8 *pAssocStasBuf );

/*                                                                            
                                   
                                                                                 
                                             
                                           
                                                          
                                                                       
                      
                                                                             */
eHalStatus csrRoamGetWpsSessionOverlap( tpAniSirGlobal pMac, tANI_U32 sessionId,
                             void *pUsrContext, void *pfnSapEventCallback,v_MACADDR_t pRemoveMac );
                                        
eHalStatus csrSendMBGetWPSPBCSessions( tpAniSirGlobal pMac, tANI_U32 sessionId,
                            tSirMacAddr bssId, void *pUsrContext, void *pfnSapEventCallback,v_MACADDR_t pRemoveMac);

/*                                                                            
                                    
                                                                     
                                             
                      
                                                                             */
eHalStatus
csrSendChngMCCBeaconInterval(tpAniSirGlobal pMac, tANI_U32 sessionId);

#ifdef FEATURE_WLAN_BTAMP_UT_RF
eHalStatus csrRoamStartJoinRetryTimer(tpAniSirGlobal pMac, tANI_U32 sessionId, tANI_U32 interval);
eHalStatus csrRoamStopJoinRetryTimer(tpAniSirGlobal pMac, tANI_U32 sessionId);
#endif
#ifdef WLAN_FEATURE_VOWIFI_11R
/*                                                                            
                                    
                                                                
                                                                             */
void csrRoamFTPreAuthRspProcessor( tHalHandle hHal, tpSirFTPreAuthRsp pFTPreAuthRsp );
#endif

#if defined(FEATURE_WLAN_CCX) && !defined(FEATURE_WLAN_CCX_UPLOAD)
void csrCcxSendAdjacentApRepMsg(tpAniSirGlobal pMac, tCsrRoamSession *pSession);
#endif

#if defined(FEATURE_WLAN_CCX)
void UpdateCCKMTSF(tANI_U32 *timeStamp0, tANI_U32 *timeStamp1, tANI_U32 *incr);
#endif

eHalStatus csrGetDefaultCountryCodeFrmNv(tpAniSirGlobal pMac, tANI_U8 *pCountry);
eHalStatus csrGetCurrentCountryCode(tpAniSirGlobal pMac, tANI_U8 *pCountry);


eHalStatus csrRoamEnqueuePreauth(tpAniSirGlobal pMac, tANI_U32 sessionId, tpSirBssDescription pBssDescription,
                                eCsrRoamReason reason, tANI_BOOLEAN fImmediate);
eHalStatus csrRoamDequeuePreauth(tpAniSirGlobal pMac);
#ifdef FEATURE_WLAN_LFR
void csrInitOccupiedChannelsList(tpAniSirGlobal pMac);
tANI_BOOLEAN csrNeighborRoamIsNewConnectedProfile(tpAniSirGlobal pMac);
tANI_BOOLEAN csrNeighborRoamConnectedProfileMatch(tpAniSirGlobal pMac, tCsrScanResult *pResult,
                                                  tDot11fBeaconIEs *pIes);
#endif
eHalStatus csrSetTxPower(tpAniSirGlobal pMac, v_U8_t sessionId, v_U8_t mW);

eHalStatus csrScanCreateEntryInScanCache(tpAniSirGlobal pMac, tANI_U32 sessionId,
                                         tCsrBssid bssid, tANI_U8 channel);

eHalStatus csrUpdateChannelList(tpAniSirGlobal pMac);
eHalStatus csrRoamDelPMKIDfromCache( tpAniSirGlobal pMac, tANI_U32 sessionId,
                                 tANI_U8 *pBSSId );
#endif


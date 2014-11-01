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

#if !defined( __SME_API_H )
#define __SME_API_H


/**=========================================================================
  
  \file  smeApi.h
  
  \brief prototype for SME APIs
  
   Copyright 2008 (c) Qualcomm, Incorporated.  All Rights Reserved.
   
   Qualcomm Confidential and Proprietary.
  
  ========================================================================*/

/*          */

/*                                                                          
               
                                                                          */
#include "ccmApi.h"
#include "csrApi.h"
#include "pmcApi.h"
#include "vos_mq.h"
#include "vos_lock.h"
#include "halTypes.h"
#include "sirApi.h"
#include "btcApi.h"
#include "vos_nvitem.h"
#include "p2p_Api.h"

#ifdef FEATURE_OEM_DATA_SUPPORT
#include "oemDataApi.h"
#endif

#if defined WLAN_FEATURE_VOWIFI
#include "smeRrmInternal.h"
#endif

/*                                                                           
                                        
                                                                          */

#define SME_SUMMARY_STATS         1
#define SME_GLOBAL_CLASSA_STATS   2
#define SME_GLOBAL_CLASSB_STATS   4
#define SME_GLOBAL_CLASSC_STATS   8
#define SME_GLOBAL_CLASSD_STATS  16
#define SME_PER_STA_STATS        32

#define SME_INVALID_COUNTRY_CODE "XX"

//                           
#define SME_DISABLE_SPLIT_SCAN   255
/*                                                                           
                   
                                                                          */
typedef struct _smeConfigParams
{
   tCsrConfigParam  csrConfig;
#if defined WLAN_FEATURE_VOWIFI
   tRrmConfigParam  rrmConfig;
#endif
#if defined FEATURE_WLAN_LFR
    tANI_U8   isFastRoamIniFeatureEnabled;
    tANI_U8   MAWCEnabled;
#endif
#if defined FEATURE_WLAN_CCX
    tANI_U8   isCcxIniFeatureEnabled;
#endif
#if defined WLAN_FEATURE_P2P_INTERNAL
   tP2PConfigParam  p2pConfig;
#endif
#if  defined (WLAN_FEATURE_VOWIFI_11R) || defined (FEATURE_WLAN_CCX) || defined(FEATURE_WLAN_LFR)
    tANI_U8       isFastTransitionEnabled;
    tANI_U8       RoamRssiDiff;
    tANI_BOOLEAN  isWESModeEnabled;
#endif
    tANI_BOOLEAN  fScanOffload;
    tANI_U8  isAmsduSupportInAMPDU;
} tSmeConfigParams, *tpSmeConfigParams;

typedef enum
{
    eSME_ROAM_TRIGGER_NONE = 0,
    eSME_ROAM_TRIGGER_SCAN = 1,
    eSME_ROAM_TRIGGER_FAST_ROAM = 2,
    eSME_ROAM_TRIGGER_MAX
} tSmeFastRoamTrigger;

/*                                                                          
                                        
                                                                          */

/*                                                                          
  
                                                                          
  
                                                                              
                                                                    

                                                                    
                                       
  
                                               
  
                                                                
  
                                                                 
     
  
                                                                            */
eHalStatus sme_Open(tHalHandle hHal);

/*                                                                          
  
                                                                   
  
                                                                      
                                          

                                                               
                                           
  
                                               
  
                                                          
  
                                                                               
                                                      
     
  
                                                                            */
eHalStatus sme_Close(tHalHandle hHal);

/*                                                                          
  
                                                          
  
                                                                     
                                                    

                                               
  
                                             
  
                                                         
     
  
                                                                            */
eHalStatus sme_Start(tHalHandle hHal);

/*                                                                          
  
                                                                     
  
                                                                    
                                                       

  
                                               

                                           
  
                                               
  
                                                                                    
                         
     
  
                                                                            */
eHalStatus sme_Stop(tHalHandle hHal, tHalStopType stopType);


/*                                                                          
  
                                                                     
  
                            

  
                                               
                                                                                                 
                                                    
                                                                                      
                                                                                   
  
                                                                      
  
                                                                 
                                                            
     
  
                                                                            */
eHalStatus sme_OpenSession(tHalHandle hHal, csrRoamCompleteCallback callback,
                           void *pContext, tANI_U8 *pSelfMacAddr,
                           tANI_U8 *pbSessionId);

/*                                                                          

                                                                          
                                               
                                                        
                                                                            */

void sme_SetCurrDeviceMode (tHalHandle hHal, tVOS_CON_MODE currDeviceMode);

/*                                                                          
  
                                                                      
  
                            

  
                                               

                                                    
  
                                                   
  
                                                                 
                                                                 
     
  
                                                                            */
eHalStatus sme_CloseSession(tHalHandle hHal, tANI_U8 sessionId,
                         csrRoamSessionCloseCallback callback, void *pContext);



/*                                                                          
  
                                                                       
  
                                                                           
                                     
   
                                                                         

  
                                               
                                                                      
                                  
  
                                                                              
  
                                                                           
     
  
                                                                            */
eHalStatus sme_UpdateConfig(tHalHandle hHal, tpSmeConfigParams pSmeConfigParams);

#ifdef FEATURE_WLAN_SCAN_PNO
/*                                                                          

                                                                          
 
                                                                      
                 

                             

                                               
  
                                                                           

                                                                        
     

                                                                            */
eHalStatus sme_UpdateChannelConfig(tHalHandle hHal);

#endif //                       
/*                                                                          
  
                                                                        
                                                
                                       

                             

                                               
                                                                     
                                  

                                                                              

                                                                           
     
                                                                          */

eHalStatus sme_set11dinfo(tHalHandle hHal,  tpSmeConfigParams pSmeConfigParams);

/*                                                                          

                                                                             

                             

                                                      
                                                                            

                                                                       

                                                                          
     
                                                                          */

eHalStatus sme_getSoftApDomain(tHalHandle hHal,  v_REGDOMAIN_t *domainIdSoftAp);

eHalStatus sme_setRegInfo(tHalHandle hHal,  tANI_U8 *apCntryCode);


/*                                                                            
                              
                                                                              
                                 
    
                                                                          
                                                                              
                                                                            
                                                                         

                               
    
                                                 

          
                                                                               
                                                                              
                                                                               
                                                                         

                           
                                                                             */
eHalStatus sme_ChangeConfigParams(tHalHandle hHal, 
                                 tCsrUpdateConfigParam *pUpdateConfigParam);

/*                                                                          
  
                                                                                          
                   
  
                                                                                                
                    
  
  
                                               
  
                                                                                  
  
                                                                  
     
  
                                                                            */
eHalStatus sme_HDDReadyInd(tHalHandle hHal);


/*                                                                          
  
                                                               
  
                                                                           
                    
  
  
                                               
                                                                   
  
                                                                     
  
                                                               
     
  
                                                                            */
eHalStatus sme_ProcessMsg(tHalHandle hHal, vos_msg_t* pMsg);

v_VOID_t sme_FreeMsg( tHalHandle hHal, vos_msg_t* pMsg );

/*                                                                            
                       
                                                                     
                                                                           
                                                                                
                                                               
                                                          
                           
                                                                             */
eHalStatus sme_ScanRequest(tHalHandle hHal, tANI_U8 sessionId, tCsrScanRequest *, 
                           tANI_U32 *pScanRequestID, 
                           csrScanCompleteCallback callback, void *pContext);


/*                                                                            
                               
                                                                        
                                               
                           
                                                                             */
eHalStatus sme_ScanSetBGScanparams(tHalHandle hHal, tANI_U8 sessionId, tCsrBGScanRequest *pScanReq);


/*                                                                            
                         
                                                               
                                                                        
                                               
                           
                                                                             */
eHalStatus sme_ScanGetResult(tHalHandle hHal, tANI_U8 sessionId, tCsrScanResultFilter *pFilter, 
                            tScanResultHandle *phResult);


/*                                                                            
                           
                                                                   
                           
                                                                             */
eHalStatus sme_ScanFlushResult(tHalHandle hHal, tANI_U8 sessionId);
eHalStatus sme_ScanFlushP2PResult(tHalHandle hHal, tANI_U8 sessionId);

/*                                                                            
                              
                                                                             
                       
                                                       
                                                         
                                                                             */
tCsrScanResultInfo *sme_ScanResultGetFirst(tHalHandle, 
                                          tScanResultHandle hScanResult);

/*                                                                            
                             
                                                                            
                                                                               
                
                                                       
                                                   
                                                                             */
tCsrScanResultInfo *sme_ScanResultGetNext(tHalHandle, 
                                          tScanResultHandle hScanResult);

/*                                                                            
                           
                                                                                 
                                                    
                                                                        
                                            
                                                                
                           
                                                                             */
eHalStatus sme_ScanResultPurge(tHalHandle hHal, tScanResultHandle hScanResult);

/*                                                                            
                                     
                                                                
                                                                     
                                           
                                                                    
                                                                             
                                                                            
                                 
                                                                                 
                                                 
                                          
                                                         
                                                      
                                                                             */
eHalStatus sme_ScanGetPMKIDCandidateList(tHalHandle hHal, tANI_U8 sessionId,
                                        tPmkidCandidateInfo *pPmkidList, 
                                        tANI_U32 *pNumItems );


/*                                                                            
                                            

        
                                                                              
                            

                                                  
                                                                    

                                                         

                                              
                             
  
                              

                      
                                                                             */
eHalStatus sme_RoamRegisterLinkQualityIndCallback(tHalHandle hHal, tANI_U8 sessionId,
                                                  csrRoamLinkQualityIndCallback   callback,  
                                                  void                           *pContext);


/*                                                                            
                       
                                                                         
                                                                 
                                                          
                                               
                           
                                                                             */
eHalStatus sme_RoamConnect(tHalHandle hHal, tANI_U8 sessionId, tCsrRoamProfile *pProfile, 
                           tANI_U32 *pRoamId);

/*                                                                            
                       
                                                                           
                                                                              
                                                                                
             
                                                                                
                                                                               
                           
                                               
                           
                                                                                 */
eHalStatus sme_RoamReassoc(tHalHandle hHal, tANI_U8 sessionId, tCsrRoamProfile *pProfile,
                          tCsrRoamModifyProfileFields modProfileFields,
                          tANI_U32 *pRoamId, v_BOOL_t fForce);

/*                                                                            
                                    
                                                                              
                           
                                                                   
                                                                             */
eHalStatus sme_RoamConnectToLastProfile(tHalHandle hHal, tANI_U8 sessionId);

/*                                                                            
                          
                                                                         
                                                                               
                                                                 
                           
                                                                             */
eHalStatus sme_RoamDisconnect(tHalHandle hHal, tANI_U8 sessionId, eCsrRoamDisconnectReason reason);

/*                                                                            
                       
                                                        
                                             
                           
                                                                             */
eHalStatus sme_RoamStopBss(tHalHandle hHal, tANI_U8 sessionId);

/*                                                                            
                                 
                                                                                       
                                 
                                             
                                                                                        
                                                                                                        
                                            
                                                           
                                                                                           
                      
                                                                                 */
eHalStatus sme_RoamGetAssociatedStas(tHalHandle hHal, tANI_U8 sessionId,
                                        VOS_MODULE_ID modId, void *pUsrContext,
                                        void *pfnSapEventCallback, tANI_U8 *pAssocStasBuf);

/*                                                                            
                             
                                                                  
                                                                                        
                                                                                        
                                                                                 */
eHalStatus sme_RoamDisconnectSta(tHalHandle hHal, tANI_U8 sessionId, tANI_U8 *pPeerMacAddr);

/*                                                                            
                         
                                                                  
                                  
                                          
                                                                                        
                                                                                            
                                                                                 */
eHalStatus sme_RoamDeauthSta(tHalHandle hHal, tANI_U8 sessionId,
                                tANI_U8 *pPeerMacAddr);

/*                                                                            
                                   
                                                                               
                                          
                                                            
                                                                                            
                                                                                 */
eHalStatus sme_RoamTKIPCounterMeasures(tHalHandle hHal, tANI_U8 sessionId, tANI_BOOLEAN bEnable);

/*                                                                            
                                    
                                                          
                                 
                                             
                                            
                                                           
                      
                                                                                 */
eHalStatus sme_RoamGetWpsSessionOverlap(tHalHandle hHal, tANI_U8 sessionId,
                                        void *pUsrContext, void *pfnSapEventCallback,
                                        v_MACADDR_t pRemoveMac);

/*                                                                            
                               
                                                                                 
                     
                           
                                                                             */
eHalStatus sme_RoamGetConnectState(tHalHandle hHal, tANI_U8 sessionId, eCsrConnectState *pState);

/*                                                                            
                                 
                                                                           
                                                                                
                                                                      
                                                              
                                              
                                                     
                                                                             */
eHalStatus sme_RoamGetConnectProfile(tHalHandle hHal, tANI_U8 sessionId,
                                     tCsrRoamConnectedProfile *pProfile);

/*                                                                            
                                  
                                                                          
                                                                   
                                                              
                                              
                             
                                                                             */
eHalStatus sme_RoamFreeConnectProfile(tHalHandle hHal, 
                                      tCsrRoamConnectedProfile *pProfile);

/*                                                                            
                             
                                                                               
                                                                      
                                        
                                                                        
                                                                                 
                                                             
                                                                                 
                                                                    
                                         
                                                     
                                                  
                                                                             */
eHalStatus sme_RoamSetPMKIDCache( tHalHandle hHal, tANI_U8 sessionId, tPmkidCacheInfo *pPMKIDCache, 
                                  tANI_U32 numItems );

/*                                                                            
                                
                                                                                    
                                                            
                                
                                                   
                                                                                
                                                                           
                                                                             
                             
                                                                                     
                                                                                 
                                   
                                                                             */
eHalStatus sme_RoamGetSecurityReqIE(tHalHandle hHal, tANI_U8 sessionId, tANI_U32 *pLen,
                                  tANI_U8 *pBuf, eCsrSecurityType secType);

/*                                                                            
                                
                                                                                      
                                               
                                                   
                                                                                
                                                                           
                                                                             
                             
                                                                                                          
                                                                                 
                                   
                                                                             */
eHalStatus sme_RoamGetSecurityRspIE(tHalHandle hHal, tANI_U8 sessionId, tANI_U32 *pLen,
                                  tANI_U8 *pBuf, eCsrSecurityType secType);


/*                                                                            
                                
                                                                             
                  
                                                        
                                                                             */
tANI_U32 sme_RoamGetNumPMKIDCache(tHalHandle hHal, tANI_U8 sessionId);

/*                                                                            
                             
                                                                           
                                                                              
                                                                              
                                                               
                                                                               
                                         
                                                                                 
                                   
                                                                             */
eHalStatus sme_RoamGetPMKIDCache(tHalHandle hHal, tANI_U8 sessionId, tANI_U32 *pNum, 
                                 tPmkidCacheInfo *pPmkidCache);

/*                                                                            
                          
                                                                        
                                        
                                           
                           
                                                                             */
eHalStatus sme_GetConfigParam(tHalHandle hHal, tSmeConfigParams *pParam);

/*                                                                            
                         
                                                                              
                                             
    
                                                                                     
                                                                                          
                                                                        
                               

                              
                                   
                                   
                                   
                                   
                             

                                                                           
                                                                                 
                                     
                                                          
                                                                      
                                                                            
                           
                                                                             */
eHalStatus sme_GetStatistics(tHalHandle hHal, eCsrStatsRequesterType requesterId, 
                             tANI_U32 statsMask, 
                             tCsrStatsCallback callback, 
                             tANI_U32 periodicity, tANI_BOOLEAN cache, 
                             tANI_U8 staId, void *pContext);

/*                                                                            
                        
                                                                             

                                                   
                                            
                                 
                                                                             */
tANI_U16 smeGetTLSTAState(tHalHandle hHal, tANI_U8 staId);

eHalStatus sme_GetRssi(tHalHandle hHal, 
                             tCsrRssiCallback callback, 
                             tANI_U8 staId, tCsrBssid bssId, void *pContext, void* pVosContext);

/*                                                                            
                  
                                                                             
               

                                                                           
                                                                      
                                                     
                                                                            
                                                                             */
eHalStatus sme_GetSnr(tHalHandle hHal,
                       tCsrSnrCallback callback,
                       tANI_U8 staId, tCsrBssid bssId,
                       void *pContext);
#if defined WLAN_FEATURE_VOWIFI_11R || defined FEATURE_WLAN_CCX || defined(FEATURE_WLAN_LFR)
eHalStatus sme_GetRoamRssi(tHalHandle hHal,
                           tCsrRssiCallback callback,
                           tANI_U8 staId,
                           tCsrBssid bssId,
                           void *pContext,
                           void* pVosContext);
#endif

#if defined(FEATURE_WLAN_CCX) && defined(FEATURE_WLAN_CCX_UPLOAD)
/*                                                                            
                       
                                                                                       

                                                                           
                                                                      
                                                                            
                                    
                      
                                                                             */
eHalStatus sme_GetTsmStats(tHalHandle hHal,
                             tCsrTsmStatsCallback callback,
                             tANI_U8 staId, tCsrBssid bssId,
                             void *pContext, void* pVosContext, tANI_U8 tid);

/*                                                                            
                     
                                                                                         
                         
                                        
                                             
                                              
                                
                                                                             */
eHalStatus sme_SetCCKMIe(tHalHandle hHal, tANI_U8 sessionId, tANI_U8 *pCckmIe, tANI_U8 cckmIeLen);


/*                                                                            
                               
                                                         
                                        
                                                      
                                
                                                                             */
eHalStatus sme_SetCcxBeaconRequest(tHalHandle hHal, const tANI_U8 sessionId,
                                   const tCsrCcxBeaconReq* pCcxBcnReq);


#endif /*                                            */
/*                                                                            
                     
                                                                       
                                                              
                                                                           
                                                    
                                                                               
                                                                                
                                                                   
                                                               
                           
                                                                             */
eHalStatus sme_CfgSetInt(tHalHandle hHal, tANI_U32 cfgId, tANI_U32 ccmValue, 
                         tCcmCfgSetCallback callback, eAniBoolean toBeSaved) ;

/*                                                                            
                     
                                                                       
                                                              
                                                                                
                                    
                                                                    
                                                                               
                                                                                
                                                                   
                                                               
                           
                                                                             */
eHalStatus sme_CfgSetStr(tHalHandle hHal, tANI_U32 cfgId, tANI_U8 *pStr, 
                         tANI_U32 length, tCcmCfgSetCallback callback, 
                         eAniBoolean toBeSaved) ;


/*                                                                            
                                  
                                                                             
                                                               
                                                                                
                                                                           
                                                                             
                                                                             
            
                                                                          
                                    

                           
                                                                                 */
eHalStatus sme_GetModifyProfileFields(tHalHandle hHal, tANI_U8 sessionId, 
                                     tCsrRoamModifyProfileFields * pModifyProfileFields);


/*                                                                          
                              
                                                                              
                                                                              
                                                                              
                                                                             
                                          
                                                  
                                                     
                                                                          
                                                                                 
                         
                                                                            */
eHalStatus sme_SetConfigPowerSave(tHalHandle hHal, tPmcPowerSavingMode psMode,
                                  void *pConfigParams);

/*                                                                          
                              
                                                                               
                                                  
                                      
                                                                          
                                                                                 
                         
                                                                            */
eHalStatus sme_GetConfigPowerSave(tHalHandle hHal, tPmcPowerSavingMode psMode,
                                  void *pConfigParams);

/*                                                                            
                            
                                                                            
                                          
                                                  
                                               
                           
                                                                             */
extern eHalStatus sme_SignalPowerEvent (
   tHalHandle hHal,
   tPmcPowerEvent event);

/*                                                                            
                           
                                                                            
                                                                    
                                                  
                                                     
                           
                                                                             */
extern eHalStatus sme_EnablePowerSave (
   tHalHandle hHal,
   tPmcPowerSavingMode psMode);

/*                                                                            
                            
                                                                            
                                                                             
                                        
                                                  
                                                       
                           
                                                                             */
extern eHalStatus sme_DisablePowerSave (
   tHalHandle hHal,
   tPmcPowerSavingMode psMode);

 /*                                                                            
                           
                                                                
                                                  
                                                     
                      
                                                                             */
extern eHalStatus sme_SetHostPowerSave (
   tHalHandle hHal,
   v_BOOL_t psMode);

/*                                                                            
                              
                                                                     
                                                                                  
                                                             
                                                  
                           
                                                                             */
extern eHalStatus sme_StartAutoBmpsTimer ( tHalHandle hHal);

/*                                                                            
                             
                                                                                   
                                                                                   
                                                                                    
                                                  
                           
                                                                             */
extern eHalStatus sme_StopAutoBmpsTimer ( tHalHandle hHal);

/*                                                                            
                           
                                                          
                                                  
                                                                  
                                                                              
                           
                                                                             */
extern eHalStatus sme_QueryPowerState (
   tHalHandle hHal,
   tPmcPowerState *pPowerState,
   tPmcSwitchState *pSwWlanSwitchState);

/*                                                                            
                              
                                                                              
                                                                          
                                                  
                                         
                           
                                                                             */
extern tANI_BOOLEAN sme_IsPowerSaveEnabled(
   tHalHandle hHal,
   tPmcPowerSavingMode psMode);

/*                                                                            
                            
                                                                   
                                                                          
                                                                                    
                                                                                        
                                                                                        
                                                                                        
                                                                                        
                                
                                                                                         
                                                                                         
                     
                                                                                     
                                               
                                       
                                      
                                               
                                               
                                                                                         
                                                           
                                                                                
                                                  
                                                  
                                                                                 
                                                                         
                                                                                  
                                                      
                                
                                                             
                                                                       
                                                                           
                                                                             */
extern eHalStatus sme_RequestFullPower (
   tHalHandle hHal,
   void (*callbackRoutine) (void *callbackContext, eHalStatus status),
   void *callbackContext,
   tRequestFullPowerReason fullPowerReason);

/*                                                                            
                       
                                                                          
                                                                              
                                                    
                                                  
                                                                                 
                                                                         
                      
                                                   
                                                                  
                                                                     
                                                                             */
extern eHalStatus sme_RequestBmps (
   tHalHandle hHal,
   void (*callbackRoutine) (void *callbackContext, eHalStatus status),
   void *callbackContext);

/*                                                                            
                                       
                                                                             
                        
                                                  
                                                                             */
void  sme_SetDHCPTillPowerActiveFlag(tHalHandle hHal, tANI_U8 flag);


/*                                                                            
                      
                                                                              
                                                                            
                 
                                                  
                                                                                 
                                                                         
                                                    
                                                                   
                                                                      
                                                                           
                           
                                                                             */
extern eHalStatus sme_StartUapsd (
   tHalHandle hHal,
   void (*callbackRoutine) (void *callbackContext, eHalStatus status),
   void *callbackContext);

/*                                                                            
                     
                                                                             
                                                                                  
                                             
                                                  
                        
                                                                             
                                                                       
                                                                             */
extern eHalStatus sme_StopUapsd (tHalHandle hHal);

/*                                                                            
                          
                                                                                 
                                                                               
                                                                               
                     
                                                  
                                                                                 
                                                                         
                        
                                                     
                                                                
                                                                   
                                                                             */
extern eHalStatus sme_RequestStandby (
   tHalHandle hHal,
   void (*callbackRoutine) (void *callbackContext, eHalStatus status),
   void *callbackContext);

/*                                                                            
                                  
                                                                       
                                                                     
                                                  
                                                                     
                                                                       
                      
                                                         
                                                               
                                                                             */
extern eHalStatus sme_RegisterPowerSaveCheck (
   tHalHandle hHal, 
   tANI_BOOLEAN (*checkRoutine) (void *checkContext), void *checkContext);

/*                                                                            
                                       
                                                                       
                                               
                                                  
                                                                  
                      
                                                                             */
extern eHalStatus sme_Register11dScanDoneCallback (
   tHalHandle hHal,
   csrScanCompleteCallback);

/*                                                                            
                                    
                                                 
                                                  
                                                                       
                      
                                                           
                                                                 
                                                                             */
extern eHalStatus sme_DeregisterPowerSaveCheck (
   tHalHandle hHal, 
   tANI_BOOLEAN (*checkRoutine) (void *checkContext));

/*                                                                            
                                        
                                                               
                                                                          
                                                  
                                                                
                                                                       
                      
                                                         
                                                               
                                                                             */
extern eHalStatus sme_RegisterDeviceStateUpdateInd (
   tHalHandle hHal, 
   void (*callbackRoutine) (void *callbackContext, tPmcState pmcState),
   void *callbackContext);

/*                                                                            
                                          
                                                                             
                                                  
                                                                  
                      
                                                           
                                                                 
                                                                             */
extern eHalStatus sme_DeregisterDeviceStateUpdateInd (
   tHalHandle hHal, 
   void (*callbackRoutine) (void *callbackContext, tPmcState pmcState));

/*                                                                            
                               
                                                                               
                                                                                 
                 
                                                  
                                          
                      
                                                   
                                                   
                                                                             */
extern eHalStatus sme_WowlAddBcastPattern (
   tHalHandle hHal, 
   tpSirWowlAddBcastPtrn pattern,
   tANI_U8 sessionId);

/*                                                                            
                               
                                                                      
                                                  
                                            
                      
                                                      
                                                   
                                                                             */
extern eHalStatus sme_WowlDelBcastPattern (
   tHalHandle hHal, 
   tpSirWowlDelBcastPtrn pattern,
   tANI_U8   sessionId);

/*                                                                            
                     
                                                             
                                                                                 
                                                                                
                                                                                 
                                
                                                                                     
                                                                                         
                                                                        
                                                                                             
                                                                                      
                                                                                    
                                                                                    
                                                                                        
                                      
                                                                                     
                                                                     
            
                                                  
                                                                         
                                                                           
                                                                                         
                                                      
                                                                
                                                                     
                                                                                       
                                                      
                      
                                                                 
                                                                 
                                                                                     
                                                      
                                                                             */
extern eHalStatus sme_EnterWowl (
    tHalHandle hHal,
    void (*enterWowlCallbackRoutine) (void *callbackContext, eHalStatus status),
    void *enterWowlCallbackContext,
#ifdef WLAN_WAKEUP_EVENTS
    void (*wakeReasonIndCB) (void *callbackContext, tpSirWakeReasonInd pWakeReasonInd),
    void *wakeReasonIndCBContext,
#endif //                   
    tpSirSmeWowlEnterParams wowlEnterParams, tANI_U8 sessionId);

/*                                                                            
                    
                                                                                 
                                                                                   
                                                                               
                                                  
                      
                                                                                
                                                                                   
                                                     
                                                                       
                                                                             */
extern eHalStatus sme_ExitWowl (tHalHandle hHal);

/*                                                                            

                      

                                                                                    
                                

                                                                                   

                                                                                                              

                                                                                       

                                                                           

                                                                                 */
eHalStatus sme_RoamSetKey(tHalHandle, tANI_U8 sessionId, tCsrRoamSetKey *pSetKey, tANI_U32 *pRoamId);

/*                                                                            

                         

                                                              

                                                                                 

                                                                                                              

                                                                                       

                                                                           

                                                                                 */
eHalStatus sme_RoamRemoveKey(tHalHandle, tANI_U8 sessionId, tCsrRoamRemoveKey *pRemoveKey, tANI_U32 *pRoamId);


/*                                                                            

                          

                                                                                                      
                            
                                                                                                         
                              

                                                                                 

                                                                            
                                                                                           
                                                                   
                                                                                     

                                

                                                                           

                                                                                 */
eHalStatus sme_GetCountryCode(tHalHandle hHal, tANI_U8 *pBuf, tANI_U8 *pbLen);

/*                                                                            

                          

                                                       
                                                       
                              

                                                                                

                                                                                                      
                                

                                

                                                                           

                                                                                 */
eHalStatus sme_SetCountryCode(tHalHandle hHal, tANI_U8 *pCountry, tANI_BOOLEAN *pfRestartNeeded);

/*                                                                            

                        

                                                                    

                                       

                                

                                                                           

                                                                                */
eHalStatus sme_InitChannels(tHalHandle hHal);

/*                                                                            
                                       
                                                                                               
                                                                            
                                                                                                               
                                           
                           
                                                                                 */
eHalStatus sme_ResetCountryCodeInformation(tHalHandle hHal, tANI_BOOLEAN *pfRestartNeeded);

/*                                                                            
                                   
                                                                                     
                                                                                      
                                                                                      
                                               
                                                                                                    
                                                                                                
                           
                                                                                 */
eHalStatus sme_GetSupportedCountryCode(tHalHandle hHal, tANI_U8 *pBuf, tANI_U32 *pbLen);

/*                                                                            
                                      
                                                                                            
                                                                                                          
                                                         
                                                                          
                                

                                                                                
                                                                                 */
eHalStatus sme_GetCurrentRegulatoryDomain(tHalHandle hHal, v_REGDOMAIN_t *pDomain);

/*                                                                            
                               
                                                                 
                                                                                                          
                                   
                                                                                    
                                    
                                                                                                               
                                           
                           
                                                                                 */
eHalStatus sme_SetRegulatoryDomain(tHalHandle hHal, v_REGDOMAIN_t domainId, tANI_BOOLEAN *pfRestartNeeded);

/*                                                                            

                                         

                                                                                           

                                                                                  

                                                                                       
                                                                          

                                

                                                                           

                                                                                 */
eHalStatus sme_GetRegulatoryDomainForCountry(tHalHandle hHal, tANI_U8 *pCountry, v_REGDOMAIN_t *pDomainId);



/*                                                                            

                                         

                                                                                       

                                                                                           

                                                                                                
                                                                                                      
                                                                                                       
                                                                                           

                                

                                                                           

                                                                                 */
eHalStatus sme_GetSupportedRegulatoryDomains(tHalHandle hHal, v_REGDOMAIN_t *pDomains, tANI_U32 *pNumDomains);

//                      
tANI_BOOLEAN sme_Is11dSupported(tHalHandle hHal);
tANI_BOOLEAN sme_Is11hSupported(tHalHandle hHal);
tANI_BOOLEAN sme_IsWmmSupported(tHalHandle hHal); 
//                                                                                        
eHalStatus sme_ScanGetBaseChannels( tHalHandle hHal, tCsrChannelInfo * pChannelInfo );

typedef void ( *tSmeChangeCountryCallback)(void *pContext);
/*                                                                            

                             

                                                                            
                                     

                                                 

                                           

                                                             

                                

                                                                           

                                                                                 */
eHalStatus sme_ChangeCountryCode( tHalHandle hHal,
                                  tSmeChangeCountryCallback callback,
                                  tANI_U8 *pCountry,
                                  void *pContext,
                                  void* pVosContext,
                                  tAniBool countryFromUserSpace,
                                  tAniBool sendRegHint);

/*                                                                            

                                    

                                             

                                                 

                                           

                                                                

                                

                                                                          

                                                                                 */
eHalStatus sme_GenericChangeCountryCode( tHalHandle hHal,
                                         tANI_U8 *pCountry,
                                         v_REGDOMAIN_t reg_domain);

/*                                                                            

                        

                                              

                                                 

                                             

                                                

                                

                                                                           

                                                                                 */

eHalStatus sme_DHCPStartInd( tHalHandle hHal,
                             tANI_U8 device_mode,
                             tANI_U8 *macAddr );

/*                                                                            

                       

                                             

                                                 

                                             

                                                

                                

                                                                           

                                                                                */
eHalStatus sme_DHCPStopInd( tHalHandle hHal,
                            tANI_U8 device_mode,
                            tANI_U8 *macAddr );


/*                                                                            
                            
                                                                               
                                                                              
                                                                              
                              
                                                  
                                                                                   
                                                                                    
                      
                                                                             
                                                                                    
                                                                  
                                                         
                                                                             */
VOS_STATUS sme_BtcSignalBtEvent (tHalHandle hHal, tpSmeBtEvent pBtcBtEvent);

/*                                                                            
                        
                                                                               
                                                                                  
                                                            
                                                  
                                                                        
                                                                                    
                                           
                      
                                                            
                                                     
                                                                             */
VOS_STATUS sme_BtcSetConfig (tHalHandle hHal, tpSmeBtcConfig pSmeBtcConfig);

/*                                                                            
                        
                                                                                 
                                                  
                                                                                       
                                                                                     
                      
                                          
                                       
                                                                             */
VOS_STATUS sme_BtcGetConfig (tHalHandle hHal, tpSmeBtcConfig pSmeBtcConfig);

/*                                                                            
                         
                                                   
                                                  
                                                
                                                                  
                            
                
                                                                             */
void sme_SetCfgPrivacy(tHalHandle hHal, tCsrRoamProfile *pProfile, tANI_BOOLEAN fPrivacy);

#if defined WLAN_FEATURE_VOWIFI
/*                                                                            
                                 
                                           
                                                  
                                                                          
                                                                                      
                                           
                      
                                          
                                       
                                                                             */
VOS_STATUS sme_NeighborReportRequest (tHalHandle hHal, tANI_U8 sessionId, 
                tpRrmNeighborReq pRrmNeighborReq, tpRrmNeighborRspCallbackInfo callbackInfo);
#endif

//                                                                         
//                                                                          
//                                                                   

//                   
//                                         
VOS_STATUS sme_DbgReadRegister(tHalHandle hHal, v_U32_t regAddr, v_U32_t *pRegValue);

//                    
//                                         
VOS_STATUS sme_DbgWriteRegister(tHalHandle hHal, v_U32_t regAddr, v_U32_t regValue);

//                 
//                                         
//                                                   
VOS_STATUS sme_DbgReadMemory(tHalHandle hHal, v_U32_t memAddr, v_U8_t *pBuf, v_U32_t nLen);

//                  
//                                         
VOS_STATUS sme_DbgWriteMemory(tHalHandle hHal, v_U32_t memAddr, v_U8_t *pBuf, v_U32_t nLen);

VOS_STATUS sme_GetWcnssWlanCompiledVersion(tHalHandle hHal,
                                           tSirVersionType *pVersion);
VOS_STATUS sme_GetWcnssWlanReportedVersion(tHalHandle hHal,
                                           tSirVersionType *pVersion);
VOS_STATUS sme_GetWcnssSoftwareVersion(tHalHandle hHal,
                                       tANI_U8 *pVersion,
                                       tANI_U32 versionBufferSize);
VOS_STATUS sme_GetWcnssHardwareVersion(tHalHandle hHal,
                                       tANI_U8 *pVersion,
                                       tANI_U32 versionBufferSize);
eHalStatus sme_RoamRegisterCallback(tHalHandle hHal,
                                    csrRoamCompleteCallback callback,
                                    void *pContext);

#ifdef FEATURE_WLAN_WAPI
/*                                                                            
                            
                                                                          
                   
                                                                                 
                                      
                                                                                   
                                                                                 
                                                                   
                                                                                 
                                                              
                                                                             */
eHalStatus sme_RoamSetBKIDCache( tHalHandle hHal, tANI_U32 sessionId, tBkidCacheInfo *pBKIDCache,
                                 tANI_U32 numItems );

/*                                                                            
                            
                                                                                
               
                                                                                 
                                      
                                                                              
                                                                                   
                 
                                                                                  
               
                                                                                 
               
                                                                             */
eHalStatus sme_RoamGetBKIDCache(tHalHandle hHal, tANI_U32 *pNum,
                                tBkidCacheInfo *pBkidCache);

/*                                                                            
                               
                                                                                
                                 
                                                                                 
                                      
                                                        
                                                                             */
tANI_U32 sme_RoamGetNumBKIDCache(tHalHandle hHal, tANI_U32 sessionId);

/*                                                                            
                                    
                                                               
                                                                    
                                          
                                                                    
                                                                            
                                                                            
                                 
                                                                                 
                                                 
                                         
                                                        
                                                     
                                                                             */
eHalStatus sme_ScanGetBKIDCandidateList(tHalHandle hHal, tANI_U32 sessionId,
                                        tBkidCandidateInfo *pBkidList, 
                                        tANI_U32 *pNumItems );
#endif /*                   */

#ifdef FEATURE_OEM_DATA_SUPPORT
/*                                                                                           
                                
                                                                                            */
/*                                                                            
                      
                                                                         
                                                                          
                                                                    
                                                          
                           
                                                                             */
eHalStatus sme_OemDataReq(tHalHandle hHal, 
                                       tANI_U8 sessionId,
                                       tOemDataReqConfig *, 
                                       tANI_U32 *pOemDataReqID, 
                                       oemData_OemDataReqCompleteCallback callback, 
                                       void *pContext);

/*                                                                            
                         
                                                         
                                                                          
                           
                                                                             */
eHalStatus sme_getOemDataRsp(tHalHandle hHal, 
                                         tOemDataRsp **pOemDataRsp);

#endif /*                        */



/*                                                                            

                             

                                                                                                
                                

                                                                               

                                                                                       

                                                                           

                                                                                 */

eHalStatus sme_RoamUpdateAPWPSIE(tHalHandle, tANI_U8 sessionId, tSirAPWPSIEs *pAPWPSIES);
/*                                                                            

                                 

                                                                                                     
                                

                                                                                           

                                 

                                                                           

                                                                                 */
eHalStatus sme_RoamUpdateAPWPARSNIEs(tHalHandle hHal, tANI_U8 sessionId, tSirRSNie * pAPSirRSNie);

/*                                                                            

                               

                                               

                                
                                             
                                                    
                                                                                 */
eHalStatus sme_ChangeMCCBeaconInterval(tHalHandle hHal, tANI_U8 sessionId);



/*                                                                            
                        
                                         
                                                
                                   
                              
                                                                           

                                                                            */

eHalStatus sme_sendBTAmpEvent(tHalHandle hHal, tSmeBtAmpEvent btAmpEvent);



/*                                                                            
                          
                                                
                                                  
                                                       
                      
                                                                              */
eHalStatus sme_SetHostOffload (tHalHandle hHal, tANI_U8 sessionId,
                                    tpSirHostOffloadReq pRequest);

/*                                                                            
                        
                                              
                                                  
                                                          
                      
                                                                             */
eHalStatus sme_SetKeepAlive (tHalHandle hHal, tANI_U8 sessionId,
                                  tpSirKeepAliveReq pRequest);


/*                                                                            
                        
                                   
                                                  
                      
                                          
                                       
                                                                             */
eHalStatus sme_AbortMacScan(tHalHandle hHal);

/*                                                                             
                              
                                                           
                                                                                 
                                                         
                               
                               
                                                                               */
eHalStatus sme_GetOperationChannel(tHalHandle hHal, tANI_U32 *pChannel, tANI_U8 sessionId);

/*                                                                            

                            

                                                                      
                                                                        
                                                                           
                               
                                                 
                       
                                                                                 */
eHalStatus sme_RegisterMgmtFrame(tHalHandle hHal, tANI_U8 sessionId, 
                     tANI_U16 frameType, tANI_U8* matchData, tANI_U16 matchLen);

/*                                                                            

                              

                                                                         
                                                                        
                                                                           
                               
                                                 
                       
                                                                                 */
eHalStatus sme_DeregisterMgmtFrame(tHalHandle hHal, tANI_U8 sessionId, 
                     tANI_U16 frameType, tANI_U8* matchData, tANI_U16 matchLen);

/*                                                                            

                               

         
                                                                              
                                  

         

                                           
 
                                                                          
                                                                               
                                                                              
                               
                         
                         
                                   

   
                        
  
  
                                                                            */
eHalStatus sme_ConfigureRxpFilter( tHalHandle hHal, 
                              tpSirWlanSetRxpFilters  wlanRxpFilterParam);

/*                                                                            

                                        

         
                                                                             
                                                                     
                                     

         

                                           
 
                                                  

   
                        
  
  
                                                                            */
eHalStatus sme_ConfigureAppsCpuWakeupState( tHalHandle hHal, tANI_BOOLEAN  isAppsAwake);

/*                                                                            

                                

         
                                                                               
                

         

                                           
 
                                                     

   
                        
  
  
                                                                            */
eHalStatus sme_ConfigureSuspendInd( tHalHandle hHal, 
                             tpSirWlanSuspendParam  wlanSuspendParam);

/*                                                                            

                               

         
                                                                               
              

         

                                           
 
                                                   

   
                        
  
  
                                                                            */
eHalStatus sme_ConfigureResumeReq( tHalHandle hHal, 
                             tpSirWlanResumeParam  wlanResumeParam);


/*                                                                            

                             

                                                                
                              

                                                 

                                                           

                                                                                 */
tANI_S8 sme_GetInfraSessionId(tHalHandle hHal);

/*                                                                            

                                    

                                                                       
                              

                                                 
                                                                 

                                                                  

                                                                                 */
tANI_U8 sme_GetInfraOperationChannel( tHalHandle hHal, tANI_U8 sessionId);
/*                                                                            

                                         

                                                                                   
                              

                                                 
                                                              

                                                                  

                                                                                 */
tANI_U8 sme_GetConcurrentOperationChannel( tHalHandle hHal );

/*                                                                            
                        
                                   
                                                  
                      
                                          
                                       
                                                                             */
eHalStatus sme_AbortMacScan(tHalHandle hHal);

/*                                                                            
                               
                                         
                                                  
                                                               
                                            
                      
                                                                             */
eHalStatus sme_GetCfgValidChannels(tHalHandle hHal, tANI_U8 *aValidChannels, tANI_U32 *len);

#ifdef FEATURE_WLAN_SCAN_PNO

/*                                                                            
                                   
                                                                  
                                                  
                                                       
                      
                                                                             */
eHalStatus sme_SetPreferredNetworkList (tHalHandle hHal, tpSirPNOScanReq pRequest, tANI_U8 sessionId, preferredNetworkFoundIndCallback callbackRoutine, void *callbackContext );

/*                                                                            
                         
                                           
                                                  
                                                       
                      
                                                                             */
eHalStatus sme_SetRSSIFilter(tHalHandle hHal, v_U8_t rssiThreshold);

/*                                                                             
 
                                    
 
              
                                               
 
             
                                 
                                     
  
                     
 
                                                                             */
eHalStatus sme_PreferredNetworkFoundInd (tHalHandle hHal, void* pMsg);
#endif //                      

/*                                                                            
                          
                                        
                                                  
                                                                  
                                                                               
                                                                               
                                                        
                      
                                                                             */
eHalStatus sme_SetPowerParams(tHalHandle hHal, tSirSetPowerParamsReq* pwParams, tANI_BOOLEAN forced);

/*                                                                            
                            
                                                        
                                                  
                                                                 
                           
                                                                             */
eHalStatus sme_SetTxPerTracking (
   tHalHandle hHal,
   void (*pCallbackfn) (void *pCallbackContext),
   void *pCallbackContext,
   tpSirTxPerTrackingParam pTxPerTrackingParam);

#ifdef WLAN_FEATURE_PACKET_FILTERING
/*                                                                            
                                  
                                                  
                                                  
                                                                   
                         
                                                                             */
eHalStatus sme_8023MulticastList(tHalHandle hHal, tANI_U8 sessionId, tpSirRcvFltMcAddrList pMulticastAddrs);

/*                                                                            
                                  
                                            
                                                  
                                                              
                         
                                                                             */
eHalStatus sme_ReceiveFilterSetFilter(tHalHandle hHal, tpSirRcvPktFilterCfgType pRcvPktFilterCfg,
                                           tANI_U8 sessionId);

/*                                                                            
                               
                                               
                                                  
                                                                                                      
                                                                       
                         
                                                                             */
eHalStatus sme_GetFilterMatchCount(tHalHandle hHal, 
                                   FilterMatchCountCallback callbackRoutine, 
                                   void *callbackContext,
                                   tANI_U8 sessionId);

/*                                                                            
                                    
                                              
                                                  
                                                                        
                         
                                                                             */
eHalStatus sme_ReceiveFilterClearFilter(tHalHandle hHal,
                                        tpSirRcvFltPktClearParam pRcvFltPktClearParam,
                                        tANI_U8  sessionId);
#endif //                              
/*                                                                            

                          
                                                                            
                              

                                                 
                                      

                                                

                                                                                 */
tANI_BOOLEAN sme_IsChannelValid(tHalHandle hHal, tANI_U8 channel);

/*                                                                            
                       
                                       
                
                                       
                        
                                                                             */
eHalStatus sme_SetFreqBand(tHalHandle hHal, eCsrBand eBand);

/*                                                                            
                       
                                                  
                
                                                  
                        
                                                                             */
eHalStatus sme_GetFreqBand(tHalHandle hHal, eCsrBand *pBand);

/*                                                                            

                            
                                                        
                                                  
                                                                 
                           
                                                                             */
eHalStatus sme_SetTxPerTracking (
   tHalHandle hHal,
   void (*pCallbackfn) (void *pCallbackContext),
   void *pCallbackContext,
   tpSirTxPerTrackingParam pTxPerTrackingParam);

#ifdef WLAN_FEATURE_GTK_OFFLOAD
/*                                                                            
                         
                                           
                                                  
                                                           
                      
                                                                             */
eHalStatus sme_SetGTKOffload (tHalHandle hHal, tpSirGtkOffloadParams pRequest, tANI_U8 sessionId);

/*                                                                            
                         
                                               
                                                  
                                                            
                      
                                                                             */
eHalStatus sme_GetGTKOffload (tHalHandle hHal, GTKOffloadGetInfoCallback callbackRoutine, 
                                    void *callbackContext, tANI_U8 sessionId);
#endif //                         

#ifdef WLAN_WAKEUP_EVENTS
eHalStatus sme_WakeReasonIndCallback (tHalHandle hHal, void* pMsg);
#endif //                   

/*                                                                            
                            
                                                        
                                                  
                                                                 
                           
                                                                             */
eHalStatus sme_SetTxPerTracking (
   tHalHandle hHal,
   void (*pCallbackfn) (void *pCallbackContext),
   void *pCallbackContext,
   tpSirTxPerTrackingParam pTxPerTrackingParam);


//                                         
tANI_U16 sme_ChnToFreq(tANI_U8 chanNum);

tANI_BOOLEAN sme_IsChannelValid(tHalHandle hHal, tANI_U8 channel);

#if defined WLAN_FEATURE_P2P_INTERNAL

eHalStatus sme_p2pResetSession(tHalHandle hHal, tANI_U8 HDDSessionId);

/*                                                                            
                              
                                                      
                                                  
                                                             
                           
                                                                             */
eHalStatus sme_p2pFlushDeviceList(tHalHandle hHal, tANI_U8 HDDSessionId);

eHalStatus sme_p2pGetResultFilter(tHalHandle hHal, tANI_U8 HDDSessionId,
                              tCsrScanResultFilter *pFilter);

#endif //                                     
   
/*                                                                            
                         
                                                                          
                                         
                
                                                 
                                                   
                                     
                        
                                                                             */
eHalStatus sme_SetMaxTxPower(tHalHandle hHal, tSirMacAddr pBssid, 
                             tSirMacAddr pSelfMacAddress, v_S7_t dB);

/*                                                                            
                                
                                                      
                                                                               
               
                             
                        
                                                                             */
eHalStatus sme_SetMaxTxPowerPerBand(eCsrBand band, v_S7_t db);

/*                                                                            

                      

                                                                  
                             

                
                                       
                                 
                        

                                                                                 */
eHalStatus sme_SetTxPower(tHalHandle hHal, v_U8_t sessionId, v_U8_t mW);

/*                                                                            

                    

                                                                           
                             

               
                     
                                                                    
                        

                                                                                 */
eHalStatus sme_HideSSID(tHalHandle hHal, v_U8_t sessionId, v_U8_t ssidHidden);

/*                                                                            

                      
                                                
                                                  
                                                     
                                                              
                      
                                                                             */
eHalStatus sme_SetTmLevel(tHalHandle hHal, v_U16_t newTMLevel, v_U16_t tmMode);

/*                                                                           

                                                                                   
              

                                      

              

                                                                           */
void sme_featureCapsExchange(tHalHandle hHal);

/*                                                                           

                                                                                                 
                  

                                      

              

                                                                           */
void sme_disableFeatureCapablity(tANI_U8 feature_index);

/*                                                                           

                                                                             
                     
              

                                      
                                            

                            

                                                                             */
eHalStatus sme_GetDefaultCountryCodeFrmNv(tHalHandle hHal, tANI_U8 *pCountry);

/*                                                                           

                                                                                 
                       

                                      
                                            

                            

                                                                             */
eHalStatus sme_GetCurrentCountryCode(tHalHandle hHal, tANI_U8 *pCountry);

/*                                                                            
                          
                                                     
                                                                               
                                  
                                                                     
                                                         
                                                                               
                                                                   
                  
                                                                             */
void sme_transportDebug(tHalHandle hHal, v_BOOL_t displaySnapshot, v_BOOL_t toggleStallDetect);

/*                                                                            
                                     
                                                                    
                                        
                  
                                                                             */
void sme_ResetPowerValuesFor5G (tHalHandle hHal);

#if  defined (WLAN_FEATURE_VOWIFI_11R) || defined (FEATURE_WLAN_CCX) || defined(FEATURE_WLAN_LFR)
/*                                                                            
                                
                                                        
                                                                               
                                                           
                                        
                                                                        
                                
                                                                             */
eHalStatus sme_UpdateRoamPrefer5GHz(tHalHandle hHal, v_BOOL_t nRoamPrefer5GHz);

/*                                                                            
                            
                                             
                                                                               
                                               
                                        
                                                            
                                
                                                                             */
eHalStatus sme_setRoamIntraBand(tHalHandle hHal, const v_BOOL_t nRoamIntraBand);

/*                                                                            
                                 
                                                 
                                                                               
                                        
                                        
                                                           
                                
                                                                             */
eHalStatus sme_UpdateRoamScanNProbes(tHalHandle hHal, const v_U8_t nProbes);

/*                                                                            
                                      
                                                       
                                                                               
                                              
                                        
                                                 
                                
                                                                             */
eHalStatus sme_UpdateRoamScanHomeAwayTime(tHalHandle hHal,
                                          const v_U16_t nRoamScanHomeAwayTime,
                                          const eAniBoolean bSendOffloadCmd);

/*                                                                            
                            
                                  
                                        
                                
                                                                             */
v_BOOL_t sme_getRoamIntraBand(tHalHandle hHal);

/*                                                                            
                              
                        
                                        
                                
                                                                             */
v_U8_t sme_getRoamScanNProbes(tHalHandle hHal);

/*                                                                            
                                   
                                        
                                        
                                
                                                                             */
v_U16_t sme_getRoamScanHomeAwayTime(tHalHandle hHal);

/*                                                                            
                                       
                                         
                                                                               
                                               
                                                                                     
                                        
                                                                               
                                     
                                
                                                                             */

eHalStatus sme_UpdateImmediateRoamRssiDiff(tHalHandle hHal, v_U8_t nImmediateRoamRssiDiff);

/*                                                                            
                              
                               
                                                                               
                                     
                                                                           
                                        
                                                                    
                                     
                                
                                                                             */

eHalStatus sme_UpdateRoamRssiDiff(tHalHandle hHal, v_U8_t RoamRssiDiff);

/*                                                                          
                                                                                              
                                                               
                          
                             
                                               
                                                                                       
                                                                             
     
                                                                            */

eHalStatus sme_UpdateFastTransitionEnabled(tHalHandle hHal,
        v_BOOL_t isFastTransitionEnabled);

/*                                                                            
                         
                          
                                                                               
                                         
                                        
                                                      
                                
                                                                             */
eHalStatus sme_UpdateWESMode(tHalHandle hHal, v_BOOL_t isWESModeEnabled);

/*                                                                            
                              
                                 
                                                            
                                                                           
                                                              
                                        
                                                                 
                                                  
                                                                             */
eHalStatus sme_SetRoamScanControl(tHalHandle hHal, v_BOOL_t roamScanControl);
#endif /*                                                                       */

#ifdef FEATURE_WLAN_LFR
/*                                                                          
                                                                                        
                                                               
                              
                             
                                               
                                                                                           
                                                                                 
     
                                                                            */

eHalStatus sme_UpdateIsFastRoamIniFeatureEnabled(tHalHandle hHal,
        const v_BOOL_t isFastRoamIniFeatureEnabled);

/*                                                                          
                                              
                                            
                                                               
                          
                            
                                               
                                                                           
                                                                 
     
                                                                            */
eHalStatus sme_UpdateIsMAWCIniFeatureEnabled(tHalHandle hHal,
        const v_BOOL_t MAWCEnabled);


#ifdef WLAN_FEATURE_ROAM_SCAN_OFFLOAD
/*                                                                          
                                                                                                  
                             
                                               
                             
                                          
     
                                                                            */

eHalStatus sme_UpdateEnableFastRoamInConcurrency(tHalHandle hHal,
         v_BOOL_t bFastRoamInConIniFeatureEnabled);
#endif
#endif /*                  */

#ifdef FEATURE_WLAN_CCX
/*                                                                          
                                                                                
                                                               
                         
                             
                                               
                                                                                      
                                                                            
     
                                                                            */

eHalStatus sme_UpdateIsCcxFeatureEnabled(tHalHandle hHal,
        const v_BOOL_t isCcxIniFeatureEnabled);

#endif /*                  */

/*                                                                          
                                                                                                
                                                               
                          
                             
                                               
                                                                                       
                                                    
     
                                                                            */

eHalStatus sme_UpdateConfigFwRssiMonitoring(tHalHandle hHal,
        v_BOOL_t fEnableFwRssiMonitoring);

#ifdef WLAN_FEATURE_NEIGHBOR_ROAMING
/*                                                                          
                                                                                     
                             
                                               
                                                             
                                                    
     
                                                                            */
eHalStatus sme_setNeighborLookupRssiThreshold(tHalHandle hHal,
        v_U8_t neighborLookupRssiThreshold);

/*                                                                          
                                                                                       
                             
                                               
                                                             
                                                    
     
                                                                            */
eHalStatus sme_setNeighborReassocRssiThreshold(tHalHandle hHal,
        v_U8_t neighborReassocRssiThreshold);

/*                                                                          
                                                                                  
                             
                                               
                                                             
                                                    
     
                                                                            */
v_U8_t sme_getNeighborLookupRssiThreshold(tHalHandle hHal);

/*                                                                          
                                                                                      
                             
                                               
                                                             
                                                    
     
                                                                            */
eHalStatus sme_setNeighborScanRefreshPeriod(tHalHandle hHal,
        v_U16_t neighborScanResultsRefreshPeriod);

/*                                                                          
                                                                                      
                             
                                               
                                                             
                                                    
     
                                                                            */
v_U16_t sme_getNeighborScanRefreshPeriod(tHalHandle hHal);

/*                                                                          
                                                                        
                             
                                               
                                                             
                                                    
     
                                                                            */
v_U16_t sme_getEmptyScanRefreshPeriod(tHalHandle hHal);

/*                                                                            
                                        
                                          
                                                                               
                                                
                                                                                     
                                        
                                                                               
                                
                                                                             */
eHalStatus sme_UpdateEmptyScanRefreshPeriod(tHalHandle hHal, v_U16_t nEmptyScanRefreshPeriod);

/*                                                                            
                                      
                                           
                                                                               
                                                    
                                                                                         
                                        
                                                                 
                                
                                                                             */
eHalStatus sme_setNeighborScanMinChanTime(tHalHandle hHal, const v_U16_t nNeighborScanMinChanTime);

/*                                                                            
                                      
                                           
                                                                               
                                                    
                                                                                         
                                        
                                                                 
                                
                                                                             */
eHalStatus sme_setNeighborScanMaxChanTime(tHalHandle hHal, const v_U16_t nNeighborScanMaxChanTime);

/*                                                                            
                                      
                                              
                                                 
                                            
                                                                             */
v_U16_t sme_getNeighborScanMinChanTime(tHalHandle hHal);

/*                                                                            
                                      
                                              
                                                 
                                            
                                                                             */
v_U16_t sme_getNeighborScanMaxChanTime(tHalHandle hHal);

/*                                                                            
                                 
                                      
                                                                               
                                            
                                                                                 
                                        
                                                      
                                
                                                                             */
eHalStatus sme_setNeighborScanPeriod(tHalHandle hHal, const v_U16_t nNeighborScanPeriod);

/*                                                                            
                                 
                                    
                                                 
                                          
                                                                             */
v_U16_t sme_getNeighborScanPeriod(tHalHandle hHal);

#endif

#if  defined (WLAN_FEATURE_VOWIFI_11R) || defined (FEATURE_WLAN_CCX) || defined(FEATURE_WLAN_LFR)
/*                                                                          
                                                   
                             
                                               
                                                             
                                                    
     
                                                                            */
v_U8_t sme_getRoamRssiDiff(tHalHandle hHal);

/*                                                                          
                                                                        
                             
                                               
                                                             
                                                    
     
                                                                            */
eHalStatus sme_ChangeRoamScanChannelList(tHalHandle hHal, tANI_U8 *pChannelList,
                                         tANI_U8 numChannels);

#ifdef FEATURE_WLAN_CCX_UPLOAD
/*                                                                          
                                                                         
                             
                                               
                                                             
                                                    
     
                                                                            */
eHalStatus sme_SetCcxRoamScanChannelList(tHalHandle hHal,
                                                   tANI_U8 *pChannelList,
                                                   tANI_U8 numChannels);
#endif

/*                                                                          
                                                                  
                             
                                               
                                                             
                                                    
     
                                                                            */
eHalStatus sme_getRoamScanChannelList(tHalHandle hHal, tANI_U8 *pChannelList,
                                         tANI_U8 *pNumChannels);

/*                                                                          
                                                                      
                             
                                               
                                                  
                                                                 
     
                                                                            */
tANI_BOOLEAN sme_getIsCcxFeatureEnabled(tHalHandle hHal);

/*                                                                          
                                       
                            
                                               
                                                  
     
                                                                            */
v_BOOL_t sme_GetWESMode(tHalHandle hHal);

/*                                                                          
                                                    
                            
                                               
                                           
     
                                                                            */
v_BOOL_t sme_GetRoamScanControl(tHalHandle hHal);

/*                                                                            
                                        
                                           
                                                                               
                                                 
                                                                                     
                                        
                                                                               
                                
                                                                             */

/*                                                                          
                                                                      
                             
                                               
                                              
                                                                 
     
                                                                            */
tANI_BOOLEAN sme_getIsLfrFeatureEnabled(tHalHandle hHal);

/*                                                                          
                                                                    
                             
                                               
                                              
                                                                 
     
                                                                            */
tANI_BOOLEAN sme_getIsFtFeatureEnabled(tHalHandle hHal);

#endif

#ifdef WLAN_FEATURE_ROAM_SCAN_OFFLOAD
/*                                                                          
                                                                                     
                                                               
                          
                            
                                               
                                                               
                                                     
     
                                                                            */

eHalStatus sme_UpdateRoamScanOffloadEnabled(tHalHandle hHal, v_BOOL_t nRoamScanOffloadEnabled);
#endif


/*                                                                            
                                  
                                               

                                                                           
                                                        
                              
                               
                                                                             */
tANI_U8 sme_IsFeatureSupportedByFW(tANI_U8 featEnumValue);

/*                                                                            
                                      
                                                   

                                                                           
                                                        

                              
                                                                             */
tANI_U8 sme_IsFeatureSupportedByDriver(tANI_U8 featEnumValue);

#ifdef FEATURE_WLAN_TDLS

/*                                                                            
                                       
                                                           

                                        
                                                                             
                               
                                                                             */

VOS_STATUS sme_SendTdlsLinkEstablishParams(tHalHandle hHal,
                                                   tANI_U8 sessionId,
                                                   tSirMacAddr peerMac,
                                                   tCsrTdlsLinkEstablishParams *tdlsLinkEstablishParams);

/*                                                                            
                             
                                               
            
                                        
                                                           
                                                   
                                                      
                                              
                                         
                                        
                               
                                                                             */
VOS_STATUS sme_SendTdlsMgmtFrame(tHalHandle hHal, tANI_U8 sessionId, tSirMacAddr peerMac,
      tANI_U8 frame_type, tANI_U8 dialog, tANI_U16 status, tANI_U8 *buf, tANI_U8 len, tANI_U8 responder);
/*                                                                            
                             
                                                   

                                        
                                                
                               
                                                                             */
VOS_STATUS sme_ChangeTdlsPeerSta(tHalHandle hHal, tANI_U8 sessionId, tSirMacAddr peerMac,
                                 tCsrStaParams *pstaParams);
/*                                                                            
                          
                                           
            
                                        
                               
                                                                             */
VOS_STATUS sme_AddTdlsPeerSta(tHalHandle hHal, tANI_U8 sessionId, tSirMacAddr peerMac);
/*                                                                            
                             
                                              
            
                                        
                               
                                                                             */
VOS_STATUS sme_DeleteTdlsPeerSta(tHalHandle hHal, tANI_U8 sessionId, tSirMacAddr peerMac);
/*                                                                            
                                      
                                                    

                  
                                                                             */
void sme_SetTdlsPowerSaveProhibited(tHalHandle hHal, v_BOOL_t val);
#endif
/*                                                                            
                     
                                              

                      
                                                                             */
v_BOOL_t sme_IsPmcBmps(tHalHandle hHal);

#ifdef FEATURE_WLAN_TDLS_INTERNAL
typedef struct smeTdlsDisResult
{
      tSirMacAddr tdlsPeerMac;
          v_S7_t tdlsPeerRssi;
} tSmeTdlsDisResult;

VOS_STATUS sme_StartTdlsDiscoveryReq(tHalHandle hHal, tANI_U8 sessionId, tSirMacAddr peerMac);
v_U8_t sme_GetTdlsDiscoveryResult(tHalHandle hHal,
                                 tSmeTdlsDisResult *disResult, v_U8_t listType);
VOS_STATUS sme_StartTdlsLinkSetupReq(tHalHandle hHal, tANI_U8 sessionId, tSirMacAddr peerMac);
VOS_STATUS sme_StartTdlsLinkTeardownReq(tHalHandle hHal, tANI_U8 sessionId, tSirMacAddr peerMac);
#endif /*                   */
eHalStatus sme_UpdateDfsSetting(tHalHandle hHal, tANI_U8 fUpdateEnableDFSChnlScan);

/*
                                                      
 */
void sme_UpdateEnableSSR(tHalHandle hHal, tANI_BOOLEAN enableSSR);

/*                                                                            

                      

                               

                                                 

                                              

                                

                                                                                 */
eHalStatus sme_SetPhyMode(tHalHandle hHal, eCsrPhyMode phyMode);

/*                                                                            

                      

                                

                                                 

                              

                                                                                 */
eCsrPhyMode sme_GetPhyMode(tHalHandle hHal);

/*
                                                
 */
VOS_STATUS sme_SelectCBMode(tHalHandle hHal, eCsrPhyMode eCsrPhyMode, tANI_U8 channel);

#ifdef WLAN_FEATURE_ROAM_SCAN_OFFLOAD
/*                                                                          
                                                                       
           
                            
                                              
                                                                              
                      
                                                                           
                                                               
     
                                                                            */

eHalStatus sme_HandoffRequest(tHalHandle hHal, tCsrHandoffRequest *pHandoffInfo);
#endif
/*                                                                          
                                                                               
                                                                     
                            
                                              
                                                                  
                                                               
     
                                                                            */
VOS_STATUS sme_isSta_p2p_clientConnected(tHalHandle hHal);

#ifdef FEATURE_WLAN_LPHB
/*                                                                            
                         
                                              
                                                
                                                    
                                                                            
                                                                   
                                                                             */
eHalStatus sme_LPHBConfigReq(
                      tHalHandle hHal,
                      tSirLPHBReq *lphdReq,
                      void (*pCallbackfn)(void *pAdapter, void *indParam));
#endif /*                   */

/*                                                                            
                             
                                                      
                                                 
                                                                           
                      
                                                                             */
eHalStatus sme_AddPeriodicTxPtrn(tHalHandle hHal, tSirAddPeriodicTxPtrn
                                 *addPeriodicTxPtrnParams);

/*                                                                            
                             
                                                      
                                                 
                                                                                
                      
                                                                             */
eHalStatus sme_DelPeriodicTxPtrn(tHalHandle hHal, tSirDelPeriodicTxPtrn
                                 *delPeriodicTxPtrnParams);
/*                                                                          
                                                                              
                                                         
                            
                                              
               
     
                                                                            */
void sme_enable_disable_split_scan (tHalHandle hHal, tANI_U8 nNumStaChan,
                                    tANI_U8 nNumP2PChan);

/*
                                                                        
           
                
*/
eHalStatus smeIssueFastRoamNeighborAPEvent (tHalHandle hHal,
                                            tANI_U8 *bssid,
                                            tSmeFastRoamTrigger fastRoamTrig);

void smeGetCommandQStatus( tHalHandle hHal );

eHalStatus sme_RoamDelPMKIDfromCache( tHalHandle hHal, tANI_U8 sessionId, tANI_U8 *pBSSId );
#ifdef FEATURE_WLAN_BATCH_SCAN
/*                                                                            
                           
                                               
                                                  
                                                     
                                  
                                                                          
                                                   
                                              
                      
                                                                             */
eHalStatus
sme_SetBatchScanReq
(
    tHalHandle hHal, tSirSetBatchScanReq *pRequest, tANI_U8 sessionId,
    void (*callbackRoutine) (void *callbackCtx, tSirSetBatchScanRsp *pRsp),
    void *callbackContext
);

/*                                                                            
                                     
                                                                     
                                                  
                                                     
                                  
                                                                          
                                                   
                                              
                      
                                                                             */
eHalStatus
sme_TriggerBatchScanResultInd
(
    tHalHandle hHal, tSirTriggerBatchScanResultInd *pRequest, tANI_U8 sessionId,
    void (*callbackRoutine) (void *callbackCtx, void *pRsp),
    void *callbackContext
);

/*                                                                            
                            
                                                
                                                  
                                                        
                      
                                                                             */
eHalStatus
sme_StopBatchScanInd
(
    tHalHandle hHal, tSirStopBatchScanInd *pInd, tANI_U8 sessionId
);

#endif

eHalStatus sme_RoamDelPMKIDfromCache( tHalHandle hHal, tANI_U8 sessionId, tANI_U8 *pBSSId );

#ifdef FEATURE_WLAN_CH_AVOID
/*                                                                            
                              
                                             
                                                                        
                                      
                
                                                                        
                        
                                                                             */
eHalStatus sme_AddChAvoidCallback
(
   tHalHandle hHal,
   void (*pCallbackfn)(void *pAdapter, void *indParam)
);
#endif /*                       */
#endif //                           

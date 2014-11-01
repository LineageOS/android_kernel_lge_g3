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

/******************************************************************************

*

* Name:  pmcApi.h

*

* Description: Power Management Control (PMC) API definitions.

* Copyright 2008 (c) Qualcomm, Incorporated.  

* All Rights Reserved.

* Qualcomm Confidential and Proprietary.

*

******************************************************************************/


#ifndef __PMC_API_H__

#define __PMC_API_H__

//                                                                                     
//                                                                               
#define BMPS_TRAFFIC_TIMER_DEFAULT 5000  //         
#define DHCP_REMAIN_POWER_ACTIVE_THRESHOLD 12 //                                  

//                                                                                    
//                                                                    
#define BMPS_TRAFFIC_TIMER_ALLOW_SECURITY_DHCP 8000  //         

#define PMC_IS_CHIP_ACCESSIBLE(pmcState) ( (IMPS != (pmcState)) && (REQUEST_IMPS != (pmcState)) && \
       (STANDBY != (pmcState)) && (REQUEST_STANDBY != (pmcState)) )



/*                                        */

typedef enum ePmcPowerEvent

{

    ePMC_SYSTEM_HIBERNATE,  /*                              */

    ePMC_SYSTEM_RESUME,  /*                                    */

    ePMC_HW_WLAN_SWITCH_OFF,  /*                                          */

    ePMC_HW_WLAN_SWITCH_ON,  /*                                         */

    ePMC_SW_WLAN_SWITCH_OFF,  /*                                          */

    ePMC_SW_WLAN_SWITCH_ON,  /*                                         */

    ePMC_BATTERY_OPERATION,  /*                                        */

    ePMC_AC_OPERATION  /*                                   */

} tPmcPowerEvent;




/*                     */

typedef enum ePmcPowerSavingMode

{

    ePMC_IDLE_MODE_POWER_SAVE,  /*                             */

    ePMC_BEACON_MODE_POWER_SAVE,  /*                               */

    ePMC_SPATIAL_MULTIPLEX_POWER_SAVE,  /*                                        */

    ePMC_UAPSD_MODE_POWER_SAVE,  /*                                                */

    ePMC_STANDBY_MODE_POWER_SAVE,  /*                         */

    ePMC_WOWL_MODE_POWER_SAVE  /*                                      */

} tPmcPowerSavingMode;




/*                */

typedef enum ePmcSwitchState

{

    ePMC_SWITCH_OFF,  /*            */

    ePMC_SWITCH_ON  /*           */

} tPmcSwitchState;




/*                      */

typedef enum ePmcPowerState

{

    ePMC_FULL_POWER,  /*            */

    ePMC_LOW_POWER,  /*           */

} tPmcPowerState;

 

/*             */

typedef enum ePmcState

{

    STOPPED, /*                */

    FULL_POWER, /*            */

    LOW_POWER, /*           */

    REQUEST_IMPS,  /*                 */

    IMPS,  /*         */

    REQUEST_BMPS,  /*                 */

    BMPS,  /*         */

    REQUEST_FULL_POWER,  /*                       */

    REQUEST_START_UAPSD,  /*                        */

    REQUEST_STOP_UAPSD,  /*                       */

    UAPSD,           /*          */

    REQUEST_STANDBY,  /*                         */

    STANDBY,  /*                 */

    REQUEST_ENTER_WOWL, /*                       */

    REQUEST_EXIT_WOWL,  /*                      */

    WOWL                /*                   */

} tPmcState;


/*                                                */

typedef enum ePmcBeaconsToForward

{

    ePMC_NO_BEACONS,  /*      */

    ePMC_BEACONS_WITH_TIM_SET,  /*              */

    ePMC_BEACONS_WITH_DTIM_SET,  /*               */

    ePMC_NTH_BEACON,  /*                  */

    ePMC_ALL_BEACONS  /*             */

} tPmcBeaconsToForward;




/*                                            */

typedef enum ePmcSmpsMode

{

    ePMC_DYNAMIC_SMPS,  /*              */

    ePMC_STATIC_SMPS  /*             */

} tPmcSmpsMode;




/*                                                           */

typedef struct sPmcImpsConfigParams

{

    tANI_BOOLEAN enterOnAc;  /*                                                           

                                                                                     */

} tPmcImpsConfigParams, *tpPmcImpsConfigParams;




/*                                                             */

typedef struct sPmcBmpsConfigParams

{

    tANI_BOOLEAN enterOnAc;  /*                                                              

                                                                                  */

    tANI_U32 txThreshold;  /*                                                                                        */

    tANI_U32 rxThreshold;  /*                                                                                       */

    tANI_U32 trafficMeasurePeriod; /*                                                    */

    tANI_U32 bmpsPeriod;  /*                                                */

    tPmcBeaconsToForward forwardBeacons;  /*                                               */

    tANI_U32 valueOfN;  /*                                                              */

    tANI_BOOLEAN usePsPoll;  /*                                                                      

                                                                                     */

    tANI_BOOLEAN setPmOnLastFrame; /*                                                                              

                                                                                                 

                                                                                                                    

                                                                  */

    tANI_BOOLEAN enableBeaconEarlyTermination; /*                              
                                                                                   
                                                                              
                                                                              
                                                                 */
    tANI_U8      bcnEarlyTermWakeInterval; /*                                   
                                                                                 
                                            */

} tPmcBmpsConfigParams, *tpPmcBmpsConfigParams;




/*                                                                      */

typedef struct sPmcSmpsConfigParams

{

    tPmcSmpsMode mode;  /*             */

    tANI_BOOLEAN enterOnAc;  /*                                                              

                                                                                  */

} tPmcSmpsConfigParams, *tpPmcSmpsConfigParams;


/*                      */

extern eHalStatus pmcOpen (tHalHandle hHal);

extern eHalStatus pmcStart (tHalHandle hHal);

extern eHalStatus pmcStop (tHalHandle hHal);

extern eHalStatus pmcClose (tHalHandle hHal );

extern eHalStatus pmcSignalPowerEvent (tHalHandle hHal, tPmcPowerEvent event);

extern eHalStatus pmcSetConfigPowerSave (tHalHandle hHal, tPmcPowerSavingMode psMode, void *pConfigParams);

extern eHalStatus pmcGetConfigPowerSave (tHalHandle hHal, tPmcPowerSavingMode psMode, void *pConfigParams);

extern eHalStatus pmcEnablePowerSave (tHalHandle hHal, tPmcPowerSavingMode psMode);

extern eHalStatus pmcStartAutoBmpsTimer (tHalHandle hHal);

extern eHalStatus pmcStopAutoBmpsTimer (tHalHandle hHal);

extern eHalStatus pmcDisablePowerSave (tHalHandle hHal, tPmcPowerSavingMode psMode);

extern eHalStatus pmcQueryPowerState (tHalHandle hHal, tPmcPowerState *pPowerState, tPmcSwitchState *pHwWlanSwitchState,

                                      tPmcSwitchState *pSwWlanSwitchState);

extern tANI_BOOLEAN pmcIsPowerSaveEnabled (tHalHandle hHal, tPmcPowerSavingMode psMode);

extern eHalStatus pmcRequestFullPower (tHalHandle hHal, void (*callbackRoutine) (void *callbackContext, eHalStatus status),

                                       void *callbackContext, tRequestFullPowerReason fullPowerReason);

extern eHalStatus pmcRequestImps (tHalHandle hHal, tANI_U32 impsPeriod,

                                  void (*callbackRoutine) (void *callbackContext, eHalStatus status),

                                  void *callbackContext);

extern eHalStatus pmcRegisterPowerSaveCheck (tHalHandle hHal, tANI_BOOLEAN (*checkRoutine) (void *checkContext),

                                             void *checkContext);

extern eHalStatus pmcDeregisterPowerSaveCheck (tHalHandle hHal, tANI_BOOLEAN (*checkRoutine) (void *checkContext));

extern void pmcMessageProcessor (tHalHandle hHal, tSirSmeRsp *pMsg);
extern void pmcResetImpsFailStatus (tHalHandle hHal);
extern v_BOOL_t IsPmcImpsReqFailed (tHalHandle hHal);

extern eHalStatus pmcRequestBmps (

   tHalHandle hHal,

   void (*callbackRoutine) (void *callbackContext, eHalStatus status),

   void *callbackContext);


extern eHalStatus pmcStartUapsd (

   tHalHandle hHal,

   void (*callbackRoutine) (void *callbackContext, eHalStatus status),

   void *callbackContext);


extern eHalStatus pmcStopUapsd (tHalHandle hHal);


extern eHalStatus pmcRequestStandby (

   tHalHandle hHal,

   void (*callbackRoutine) (void *callbackContext, eHalStatus status),

   void *callbackContext);


extern eHalStatus pmcRegisterDeviceStateUpdateInd (tHalHandle hHal, 

   void (*callbackRoutine) (void *callbackContext, tPmcState pmcState),

   void *callbackContext);


extern eHalStatus pmcDeregisterDeviceStateUpdateInd (tHalHandle hHal, 

   void (*callbackRoutine) (void *callbackContext, tPmcState pmcState));


extern eHalStatus pmcReady(tHalHandle hHal);


void pmcDumpInit(tHalHandle hHal);


extern eHalStatus pmcWowlAddBcastPattern (
   tHalHandle hHal, 
   tpSirWowlAddBcastPtrn pattern, 
   tANI_U8  sessionId);


extern eHalStatus pmcWowlDelBcastPattern (
   tHalHandle hHal, 
   tpSirWowlDelBcastPtrn pattern,
   tANI_U8 sessionId);


extern eHalStatus pmcEnterWowl ( 

    tHalHandle hHal, 

    void (*enterWowlCallbackRoutine) (void *callbackContext, eHalStatus status),

    void *enterWowlCallbackContext,
#ifdef WLAN_WAKEUP_EVENTS
    void (*wakeReasonIndCB) (void *callbackContext, tpSirWakeReasonInd pWakeReasonInd),

    void *wakeReasonIndCBContext,
#endif //                   
    tpSirSmeWowlEnterParams wowlEnterParams, tANI_U8 sessionId);

extern eHalStatus pmcExitWowl (tHalHandle hHal);


extern eHalStatus pmcSetHostOffload (tHalHandle hHal, tpSirHostOffloadReq pRequest,
                                          tANI_U8 sessionId);

/*                                                                            
                       
                                       
                                                  
                                                 
                      
                                                          
                                                   
                                                                             */
extern eHalStatus pmcSetKeepAlive (tHalHandle hHal, tpSirKeepAliveReq pRequest, tANI_U8 sessionId);

extern tANI_BOOLEAN pmcValidateConnectState( tHalHandle hHal );

extern tANI_BOOLEAN pmcAllowImps( tHalHandle hHal );


#ifdef FEATURE_WLAN_SCAN_PNO
/*                              */
typedef void(*preferredNetworkFoundIndCallback)(void *callbackContext, tpSirPrefNetworkFoundInd pPrefNetworkFoundInd);

extern eHalStatus pmcSetPreferredNetworkList(tHalHandle hHal, tpSirPNOScanReq pRequest, tANI_U8 sessionId, preferredNetworkFoundIndCallback callbackRoutine, void *callbackContext);
extern eHalStatus pmcSetRssiFilter(tHalHandle hHal, v_U8_t rssiThreshold);
#endif //                      

#ifdef WLAN_FEATURE_PACKET_FILTERING
//                                                          
typedef void(*FilterMatchCountCallback)(void *callbackContext,
                                        tpSirRcvFltPktMatchRsp pRcvFltPktMatchRsp);
extern eHalStatus pmcGetFilterMatchCount(tHalHandle hHal, FilterMatchCountCallback callbackRoutine, 
                                                void *callbackContext, tANI_U8 sessionId);
#endif //                              

#ifdef WLAN_FEATURE_GTK_OFFLOAD
//                                             
typedef void(*GTKOffloadGetInfoCallback)(void *callbackContext, tpSirGtkOffloadGetInfoRspParams pGtkOffloadGetInfoRsp);

/*                                                                            
                        
                                    
                                                  
                                                             
                      
                                                        
                                                   
                                                                             */
extern eHalStatus pmcSetGTKOffload (tHalHandle hHal, tpSirGtkOffloadParams pGtkOffload, tANI_U8 sessionId);

/*                                                                            
                        
                                        
                                                  
                                                                                            
                      
                                                        
                                                   
                                                                             */
extern eHalStatus pmcGetGTKOffload(tHalHandle hHal,
                                   GTKOffloadGetInfoCallback callbackRoutine,
                                   void *callbackContext, tANI_U8 sessionId);
#endif //                         

#ifdef FEATURE_WLAN_BATCH_SCAN
/*                                     */
typedef void(*hddSetBatchScanReqCallback)(void *callbackContext,
     tSirSetBatchScanRsp *pRsp);

/*                                                   */
typedef void(*hddTriggerBatchScanResultIndCallback)(void *callbackContext,
     void *pRsp);

/*                                                                              
                          
                                            
                                                  
                                  
                                                                                
                                             
                      
                                                               
                                                   
                                                                              */
extern eHalStatus pmcSetBatchScanReq(tHalHandle hHal, tSirSetBatchScanReq
       *pRequest, tANI_U8 sessionId, hddSetBatchScanReqCallback callbackRoutine,
       void *callbackContext);

/*                                                                              
                                    
                                                 
                                                  
                                  
                                                                                
                                             
                      
                                                               
                                                   
                                                                              */
extern eHalStatus pmcTriggerBatchScanResultInd
(
    tHalHandle hHal, tSirTriggerBatchScanResultInd *pRequest, tANI_U8 sessionId,
    hddTriggerBatchScanResultIndCallback callbackRoutine, void *callbackContext
);


/*                                                                              
                           
                                            
                                                  
                                                        
                      
                                                               
                                                   
                                                                              */

extern eHalStatus pmcStopBatchScanInd
(
    tHalHandle hHal,
    tSirStopBatchScanInd *pInd,
    tANI_U8 sessionId
);

#endif //                        


#endif


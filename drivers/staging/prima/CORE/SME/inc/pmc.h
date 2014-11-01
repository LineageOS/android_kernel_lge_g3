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
* Name:  pmc.h
*
* Description: Power Management Control (PMC) internal definitions.
*
* Copyright 2008 (c) Qualcomm, Incorporated.  
  All Rights Reserved.
* Qualcomm Confidential and Proprietary.
*
******************************************************************************/

#ifndef __PMC_H__
#define __PMC_H__


#include "palTimer.h"
#include "csrLinkList.h"
#include "pmcApi.h"
#include "smeInternal.h"


//                                                                                 
//                  
#define PMC_ABORT  

/*                     */
typedef enum ePowerSource
{
    AC_POWER,  /*                                 */
    BATTERY_POWER  /*                                      */
} tPowerSource;


/*                                      */
typedef struct sPowerSaveCheckEntry
{
    tListElem link;  /*            */
    tANI_BOOLEAN (*checkRoutine) (void *checkContext);  /*                          */
    void *checkContext;  /*                                                            */
} tPowerSaveCheckEntry, *tpPowerSaveCheckEntry;


/*                                                  */
typedef struct sDeviceStateUpdateIndEntry
{
    tListElem link;  /*            */
    void (*callbackRoutine) (void *callbackContext, tPmcState pmcState); /*                                                              */
    void *callbackContext;  /*                                                            */
} tDeviceStateUpdateIndEntry, *tpDeviceStateUpdateIndEntry;

/*                                                 */
typedef struct sRequestFullPowerEntry
{
    tListElem link;  /*            */
    void (*callbackRoutine) (void *callbackContext, eHalStatus status);  /*                                             */
    void *callbackContext;  /*                                                            */
} tRequestFullPowerEntry, *tpRequestFullPowerEntry;


/*                                           */
typedef struct sRequestBmpsEntry
{
   tListElem link;  /*            */

   /*                                                    */
   void (*callbackRoutine) (void *callbackContext, eHalStatus status);

   /*                                                            */
   void *callbackContext;  

} tRequestBmpsEntry, *tpRequestBmpsEntry;


/*                                           */
typedef struct sStartUapsdEntry
{
   tListElem link;  /*            */

   /*                                                  */
   void (*callbackRoutine) (void *callbackContext, eHalStatus status);

   /*                                                            */
   void *callbackContext;  

} tStartUapsdEntry, *tpStartUapsdEntry;

typedef struct sPmcDeferredMsg
{
    tListElem link;
    tpAniSirGlobal pMac;
    tANI_U16 messageType;
    tANI_U16 size;  //                         
    union
    {
        tSirPowerSaveCfg powerSaveConfig;
        tSirWowlAddBcastPtrn wowlAddPattern;
        tSirWowlDelBcastPtrn wowlDelPattern;
        tANI_U8 data[1];    //              
    }u;
} tPmcDeferredMsg;



/*                                                  */
typedef struct sPmcInfo
{
    tPowerSource powerSource;  /*                   */
    tPmcSwitchState hwWlanSwitchState;  /*                            */
    tPmcSwitchState swWlanSwitchState;  /*                            */
    tPmcState pmcState;  /*           */
    tANI_BOOLEAN requestFullPowerPending;  /*                                             */
    tRequestFullPowerReason requestFullPowerReason; /*                                  */
    tPmcImpsConfigParams impsConfig;  /*                    */
    tPmcBmpsConfigParams bmpsConfig;  /*                    */
    tPmcSmpsConfigParams smpsConfig;  /*                    */
    tANI_BOOLEAN impsEnabled;  /*                         */
    tANI_BOOLEAN bmpsEnabled;  /*                         */
    tANI_BOOLEAN autoBmpsEntryEnabled;  /*                                                             
                                                                                                        */
    tANI_BOOLEAN bmpsRequestedByHdd; /*                                            */
    tANI_BOOLEAN bmpsRequestQueued; /*                                 */
    tANI_BOOLEAN smpsEnabled;  /*                         */
    tANI_BOOLEAN remainInPowerActiveTillDHCP;  /*                                            */
    tANI_U32 remainInPowerActiveThreshold;  /*                                          */
    tANI_U32 impsPeriod;  /*                                  */
    void (*impsCallbackRoutine) (void *callbackContext, eHalStatus status);  /*                                 
                                                                                             */ 
    void *impsCallbackContext;  /*                                                            */
    vos_timer_t hImpsTimer;  /*                        */
    vos_timer_t hTrafficTimer;  /*                                   */
#ifdef FEATURE_WLAN_DIAG_SUPPORT    
    vos_timer_t hDiagEvtTimer;  /*                                              */
#endif
    vos_timer_t hExitPowerSaveTimer;  /*                                               */
    tDblLinkList powerSaveCheckList; /*                               */
    tDblLinkList requestFullPowerList; /*                                          */
    tANI_U32 cLastTxUnicastFrames;  /*                                                                    */
    tANI_U32 cLastRxUnicastFrames;  /*                                                                   */


    tANI_BOOLEAN uapsdEnabled;  /*                          */
    tANI_BOOLEAN uapsdSessionRequired; /*                                                   */
    tDblLinkList requestBmpsList; /*                                    */
    tDblLinkList requestStartUapsdList; /*                                           */
    tANI_BOOLEAN standbyEnabled;  /*                            */
    void (*standbyCallbackRoutine) (void *callbackContext, eHalStatus status); /*                                     */ 
    void *standbyCallbackContext;/*                                                            */
    tDblLinkList deviceStateUpdateIndList; /*                                    */
    tANI_BOOLEAN pmcReady; /*                                                          */
    tANI_BOOLEAN wowlEnabled;  /*                         */
    tANI_BOOLEAN wowlModeRequired; /*                                                   */
    void (*enterWowlCallbackRoutine) (void *callbackContext, eHalStatus status); /*                                  */ 
    void *enterWowlCallbackContext;/*                                                            */
    tSirSmeWowlEnterParams wowlEnterParams; /*                         */
    tDblLinkList deferredMsgList;   //                                                                  
    tANI_BOOLEAN rfSuppliesVotedOff;  //                                         
#ifdef FEATURE_WLAN_SCAN_PNO
    preferredNetworkFoundIndCallback  prefNetwFoundCB; /*                                                        */ 
    void *preferredNetworkFoundIndCallbackContext;/*                                                            */
#endif //                      
#ifdef WLAN_FEATURE_PACKET_FILTERING
    FilterMatchCountCallback  FilterMatchCountCB; /*                                                          */ 
    void *FilterMatchCountCBContext;/*                                                            */
#endif //                              
#ifdef WLAN_FEATURE_GTK_OFFLOAD
    GTKOffloadGetInfoCallback  GtkOffloadGetInfoCB; /*                                             */ 
    void *GtkOffloadGetInfoCBContext;        /*                                                            */
#endif //                         

#ifdef WLAN_WAKEUP_EVENTS
    void (*wakeReasonIndCB) (void *callbackContext, tpSirWakeReasonInd pWakeReasonInd);  /*                                            */ 
    void *wakeReasonIndCBContext;  /*                                                            */
#endif //                   

/*                                                                                  
                                                                                     */
    v_BOOL_t    isHostPsEn;
    v_BOOL_t    ImpsReqFailed;
    v_BOOL_t    ImpsReqTimerFailed;
    tANI_U8     ImpsReqFailCnt;
    tANI_U8     ImpsReqTimerfailCnt;

#ifdef FEATURE_WLAN_BATCH_SCAN
   /*                                                                    */
   hddSetBatchScanReqCallback setBatchScanReqCallback;
   void * setBatchScanReqCallbackContext;
   /*                                                                        */
   hddTriggerBatchScanResultIndCallback batchScanResultCallback;
   void * batchScanResultCallbackContext;
#endif


} tPmcInfo, *tpPmcInfo;


//     
#define PMC_IS_READY(pMac)  ( ((pMac)->pmc.pmcReady) && (STOPPED != (pMac)->pmc.pmcState) )


/*                      */
extern eHalStatus pmcEnterLowPowerState (tHalHandle hHal);
extern eHalStatus pmcExitLowPowerState (tHalHandle hHal);
extern eHalStatus pmcEnterFullPowerState (tHalHandle hHal);
extern eHalStatus pmcEnterRequestFullPowerState (tHalHandle hHal, tRequestFullPowerReason fullPowerReason);
extern eHalStatus pmcEnterRequestImpsState (tHalHandle hHal);
extern eHalStatus pmcEnterImpsState (tHalHandle hHal);
extern eHalStatus pmcEnterRequestBmpsState (tHalHandle hHal);
extern eHalStatus pmcEnterBmpsState (tHalHandle hHal);
extern eHalStatus pmcEnterRequestStartUapsdState (tHalHandle hHal);
extern eHalStatus pmcEnterUapsdState (tHalHandle hHal);
extern eHalStatus pmcEnterRequestStopUapsdState (tHalHandle hHal);
extern eHalStatus pmcEnterRequestStandbyState (tHalHandle hHal);
extern eHalStatus pmcEnterStandbyState (tHalHandle hHal);
extern tANI_BOOLEAN pmcPowerSaveCheck (tHalHandle hHal);
extern eHalStatus pmcSendPowerSaveConfigMessage (tHalHandle hHal);
extern eHalStatus pmcSendMessage (tpAniSirGlobal pMac, tANI_U16 messageType, void *pMessageData, tANI_U32 messageSize);
extern void pmcDoCallbacks (tHalHandle hHal, eHalStatus callbackStatus);
extern void pmcDoBmpsCallbacks (tHalHandle hHal, eHalStatus callbackStatus);
extern void pmcDoStartUapsdCallbacks (tHalHandle hHal, eHalStatus callbackStatus);
extern void pmcDoStandbyCallbacks (tHalHandle hHal, eHalStatus callbackStatus);
extern eHalStatus pmcStartTrafficTimer (tHalHandle hHal, tANI_U32 expirationTime);
extern void pmcStopTrafficTimer (tHalHandle hHal);
extern void pmcImpsTimerExpired (tHalHandle hHal);
extern void pmcTrafficTimerExpired (tHalHandle hHal);

#ifdef FEATURE_WLAN_DIAG_SUPPORT    
extern eHalStatus pmcStartDiagEvtTimer (tHalHandle hHal);
extern void pmcStopDiagEvtTimer (tHalHandle hHal);
extern void pmcDiagEvtTimerExpired (tHalHandle hHal);
#endif

extern void pmcExitPowerSaveTimerExpired (tHalHandle hHal);
extern tPmcState pmcGetPmcState (tHalHandle hHal);
extern const char* pmcGetPmcStateStr(tPmcState state);
extern void pmcDoDeviceStateUpdateCallbacks (tHalHandle hHal, tPmcState state);
extern eHalStatus pmcRequestEnterWowlState(tHalHandle hHal, tpSirSmeWowlEnterParams wowlEnterParams);
extern eHalStatus pmcEnterWowlState (tHalHandle hHal);
extern eHalStatus pmcRequestExitWowlState(tHalHandle hHal);
extern void pmcDoEnterWowlCallbacks (tHalHandle hHal, eHalStatus callbackStatus);
//                                                                                 
extern eHalStatus pmcDeferMsg( tpAniSirGlobal pMac, tANI_U16 messageType, 
                                               void *pData, tANI_U32 size);
extern eHalStatus pmcIssueCommand( tpAniSirGlobal pMac, eSmeCommandType cmdType, void *pvParam, 
                                   tANI_U32 size, tANI_BOOLEAN fPutToListHead );
extern eHalStatus pmcEnterImpsCheck( tpAniSirGlobal pMac );
extern eHalStatus pmcEnterBmpsCheck( tpAniSirGlobal pMac );
extern tANI_BOOLEAN pmcShouldBmpsTimerRun( tpAniSirGlobal pMac );
#endif

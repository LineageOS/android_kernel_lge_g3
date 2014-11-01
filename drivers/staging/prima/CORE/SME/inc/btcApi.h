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
* Name:  btcApi.h
*
* Description: BTC Events Layer API definitions.
*
* Copyright 2008 (c) Qualcomm, Incorporated.  All Rights Reserved.
* Qualcomm Confidential and Proprietary.
*
******************************************************************************/

#ifndef __BTC_API_H__
#define __BTC_API_H__

#include "vos_types.h"
#include "vos_timer.h"
#include "vos_nvitem.h"

#define BT_INVALID_CONN_HANDLE (0xFFFF)  /*                             */

/*                                         */
#define BT_CONN_STATUS_FAIL      (0)         /*                     */
#define BT_CONN_STATUS_SUCCESS   (1)         /*                         */
#define BT_CONN_STATUS_MAX       (2)         /*                                      */

/*                         
                                      
*/
#define BT_SCO                  (0)   /*            */
#define BT_ACL                  (1)   /*            */
#define BT_eSCO                 (2)   /*             */
#define BT_LINK_TYPE_MAX        (3)   /*                                     */

/*                
                                        
*/
#define BT_ACL_ACTIVE           (0)   /*               */
#define BT_ACL_HOLD             (1)   /*             */
#define BT_ACL_SNIFF            (2)   /*              */
#define BT_ACL_PARK             (3)   /*             */
#define BT_ACL_MODE_MAX         (4)   /*                                     */

/* 
                                      
  
    */
#define BTC_MAX_NUM_ACL_BT_SUB_INTS (7)

/*                                                
*/
#define BTC_SMART_COEXISTENCE   (0) /*                                       */
#define BTC_WLAN_ONLY           (1) /*                      */
#define BTC_PTA_ONLY            (2) /*                                    */
#define BTC_SMART_MAX_WLAN      (3) /*                                                      */
#define BTC_SMART_MAX_BT        (4) /*                                                    */
#define BTC_SMART_BT_A2DP       (5) /*                                                                  */
#define BT_EXEC_MODE_MAX        (6) /*                                     */

/*                                                               
                                          
*/
#define BTC_RESTART_CURRENT     (0) /*                                               */
#define BTC_START_NEXT          (1) /*                                                                                   */
#define BTC_ACTION_TYPE_MAX     (2) /*                                     */

#define BTC_BT_INTERVAL_MODE1_DEFAULT       (120) /*                        */
#define BTC_WLAN_INTERVAL_MODE1_DEFAULT     (30)  /*                          */

/*                                                              
                                                                  
                                                            
*/
#define BT_INQUIRY_STARTED                  (1<<0)
#define BT_PAGE_STARTED                     (1<<1)
#define BT_CREATE_ACL_CONNECTION_STARTED    (1<<2)
#define BT_CREATE_SYNC_CONNECTION_STARTED   (1<<3)

/*                                                                                 
                                                                                           
*/
#define BT_MAX_EVENT_DONE_TIMEOUT   45000


/*
                                                         
*/
#define BT_MAX_SCO_SUPPORT  3
#define BT_MAX_ACL_SUPPORT  3
#define BT_MAX_DISCONN_SUPPORT (BT_MAX_SCO_SUPPORT+BT_MAX_ACL_SUPPORT)
#define BT_MAX_NUM_EVENT_ACL_DEFERRED  4  //                                                 
#define BT_MAX_NUM_EVENT_SCO_DEFERRED  4  //                                                  

/*                                                
*/
#define BTC_STATIC_BT_LEN_INQ_DEF     (120000)  //         
#define BTC_STATIC_BT_LEN_PAGE_DEF     (10000)  //                     
#define BTC_STATIC_BT_LEN_CONN_DEF     (10000)  //                     
#define BTC_STATIC_BT_LEN_LE_DEF       (10000)  //                     
#define BTC_STATIC_WLAN_LEN_INQ_DEF    (30000)  //        
#define BTC_STATIC_WLAN_LEN_PAGE_DEF       (0)  //                      
#define BTC_STATIC_WLAN_LEN_CONN_DEF       (0)  //                      
#define BTC_STATIC_WLAN_LEN_LE_DEF         (0)  //                      
#define BTC_DYNAMIC_BT_LEN_MAX_DEF    (250000)  //         
#define BTC_DYNAMIC_WLAN_LEN_MAX_DEF   (45000)  //        
#define BTC_SCO_BLOCK_PERC_DEF             (1)  //          
#define BTC_DHCP_ON_A2DP_DEF               (1)  //   
#define BTC_DHCP_ON_SCO_DEF                (0)  //    

/*
                                                      
 */
#define MWS_COEX_MAX_VICTIM_TABLE             10
#define MWS_COEX_MAX_CONFIG                   6

/*                                                     
*/
typedef enum eSmeBtEventType
{
  BT_EVENT_DEVICE_SWITCHED_ON,
  BT_EVENT_DEVICE_SWITCHED_OFF,
  BT_EVENT_INQUIRY_STARTED,
  BT_EVENT_INQUIRY_STOPPED,
  BT_EVENT_INQUIRY_SCAN_STARTED,
  BT_EVENT_INQUIRY_SCAN_STOPPED,
  BT_EVENT_PAGE_STARTED,
  BT_EVENT_PAGE_STOPPED,
  BT_EVENT_PAGE_SCAN_STARTED,
  BT_EVENT_PAGE_SCAN_STOPPED,
  BT_EVENT_CREATE_ACL_CONNECTION,
  BT_EVENT_ACL_CONNECTION_COMPLETE,
  BT_EVENT_CREATE_SYNC_CONNECTION,
  BT_EVENT_SYNC_CONNECTION_COMPLETE,
  BT_EVENT_SYNC_CONNECTION_UPDATED,
  BT_EVENT_DISCONNECTION_COMPLETE,
  BT_EVENT_MODE_CHANGED,
  BT_EVENT_A2DP_STREAM_START,
  BT_EVENT_A2DP_STREAM_STOP,
  BT_EVENT_TYPE_MAX,    //                                  
} tSmeBtEventType;

/*                    
*/
typedef enum eSmeBtAmpEventType
{
  BTAMP_EVENT_CONNECTION_START,
  BTAMP_EVENT_CONNECTION_STOP,
  BTAMP_EVENT_CONNECTION_TERMINATED,
  BTAMP_EVENT_TYPE_MAX, //                                  
} tSmeBtAmpEventType;


/*                                                              
                                                                       
*/
typedef struct sSmeBtAclConnectionParam
{
   v_U8_t       bdAddr[6];
   v_U16_t      connectionHandle;
   v_U8_t       status;
} tSmeBtAclConnectionParam, *tpSmeBtAclConnectionParam;

/*                                                               
                                                                      
                                        
*/
typedef struct sSmeBtSyncConnectionParam
{
   v_U8_t       bdAddr[6];
   v_U16_t      connectionHandle;
   v_U8_t       status;
   v_U8_t       linkType;
   v_U8_t       scoInterval; //                              
   v_U8_t       scoWindow;   //                              
   v_U8_t       retransmisisonWindow; //                              
} tSmeBtSyncConnectionParam, *tpSmeBtSyncConnectionParam;

typedef struct sSmeBtSyncUpdateHist
{
    tSmeBtSyncConnectionParam btSyncConnection;
    v_BOOL_t fValid;
} tSmeBtSyncUpdateHist, *tpSmeBtSyncUpdateHist;

/*                                                              
                         
*/
typedef struct sSmeBtAclModeChangeParam
{
    v_U16_t     connectionHandle;
    v_U8_t      mode;
} tSmeBtAclModeChangeParam, *tpSmeBtAclModeChangeParam;

/*                                                             
                                   
*/
typedef struct sSmeBtDisconnectParam
{
   v_U16_t connectionHandle;
} tSmeBtDisconnectParam, *tpSmeBtDisconnectParam;

/*                                                             
                              
                             
*/
typedef struct sSmeBtA2DPParam
{
   v_U8_t       bdAddr[6];
} tSmeBtA2DPParam, *tpSmeBtA2DPParam;


/*                                           
*/
typedef struct sSmeBtcBtEvent
{
   tSmeBtEventType btEventType;
   union
   {
      v_U8_t                    bdAddr[6];    /*                                                */
      tSmeBtAclConnectionParam  btAclConnection;
      tSmeBtSyncConnectionParam btSyncConnection;
      tSmeBtDisconnectParam     btDisconnect;
      tSmeBtAclModeChangeParam  btAclModeChange;
   }uEventParam;
} tSmeBtEvent, *tpSmeBtEvent;


/* 
                          
*/
typedef struct sSmeBtAmpEvent
{
  tSmeBtAmpEventType btAmpEventType;

} tSmeBtAmpEvent, *tpSmeBtAmpEvent;


/*                                                                
*/
typedef struct sSmeBtcConfig
{
   v_U8_t       btcExecutionMode;
   v_U8_t       btcConsBtSlotsToBlockDuringDhcp;
   v_U8_t       btcA2DPBtSubIntervalsDuringDhcp;
   v_U8_t       btcActionOnPmFail;
   v_U8_t       btcBtIntervalMode1;
   v_U8_t       btcWlanIntervalMode1;

   v_U32_t      btcStaticLenInqBt;
   v_U32_t      btcStaticLenPageBt;
   v_U32_t      btcStaticLenConnBt;
   v_U32_t      btcStaticLenLeBt;
   v_U32_t      btcStaticLenInqWlan;
   v_U32_t      btcStaticLenPageWlan;
   v_U32_t      btcStaticLenConnWlan;
   v_U32_t      btcStaticLenLeWlan;
   v_U32_t      btcDynMaxLenBt;
   v_U32_t      btcDynMaxLenWlan;
   v_U32_t      btcMaxScoBlockPerc;
   v_U32_t      btcDhcpProtOnA2dp;
   v_U32_t      btcDhcpProtOnSco;

   v_U32_t      mwsCoexVictimWANFreq[MWS_COEX_MAX_VICTIM_TABLE];
   v_U32_t      mwsCoexVictimWLANFreq[MWS_COEX_MAX_VICTIM_TABLE];
   v_U32_t      mwsCoexVictimConfig[MWS_COEX_MAX_VICTIM_TABLE];
   v_U32_t      mwsCoexVictimConfig2[MWS_COEX_MAX_VICTIM_TABLE];
   v_U32_t      mwsCoexModemBackoff;
   v_U32_t      mwsCoexConfig[MWS_COEX_MAX_CONFIG];
   v_U32_t      SARPowerBackoff;
} tSmeBtcConfig, *tpSmeBtcConfig;


typedef struct sSmeBtAclModeChangeEventHist
{
    tSmeBtAclModeChangeParam  btAclModeChange;
    v_BOOL_t fValid;
} tSmeBtAclModeChangeEventHist, *tpSmeBtAclModeChangeEventHist;

typedef struct sSmeBtAclEventHist
{
    //                                                                       
    tSmeBtEventType btEventType[BT_MAX_NUM_EVENT_ACL_DEFERRED];
    tSmeBtAclConnectionParam  btAclConnection[BT_MAX_NUM_EVENT_ACL_DEFERRED];
    //                                               
    tANI_U8 bNextEventIdx;
} tSmeBtAclEventHist, *tpSmeBtAclEventHist;

typedef struct sSmeBtSyncEventHist
{
    //                                                                       
    tSmeBtEventType btEventType[BT_MAX_NUM_EVENT_SCO_DEFERRED];
    tSmeBtSyncConnectionParam  btSyncConnection[BT_MAX_NUM_EVENT_SCO_DEFERRED];
    //                                               
    tANI_U8 bNextEventIdx;
} tSmeBtSyncEventHist, *tpSmeBtSyncEventHist;

typedef struct sSmeBtDisconnectEventHist
{
    tSmeBtDisconnectParam btDisconnect;
    v_BOOL_t fValid;
} tSmeBtDisconnectEventHist, *tpSmeBtDisconnectEventHist;


/*
                                             
*/
typedef struct sSmeBtcEventHist
{
   tSmeBtSyncEventHist btSyncConnectionEvent[BT_MAX_SCO_SUPPORT];
   tSmeBtAclEventHist btAclConnectionEvent[BT_MAX_ACL_SUPPORT];
   tSmeBtAclModeChangeEventHist btAclModeChangeEvent[BT_MAX_ACL_SUPPORT];
   tSmeBtDisconnectEventHist btDisconnectEvent[BT_MAX_DISCONN_SUPPORT];
   tSmeBtSyncUpdateHist btSyncUpdateEvent[BT_MAX_SCO_SUPPORT];
   int nInquiryEvent;    //                                       
                         //                                     
                         //                     
   int nPageEvent;  //                                   
                    //                                  
                    //                  
   v_BOOL_t fA2DPStarted;
   v_BOOL_t fA2DPStopped;
} tSmeBtcEventHist, *tpSmeBtcEventHist;

typedef struct sSmeBtcEventReplay
{
   tSmeBtcEventHist btcEventHist;
   v_BOOL_t fBTSwitchOn;
   v_BOOL_t fBTSwitchOff;
   //                                                                                 
   v_BOOL_t fRestoreHBMonitor;
} tSmeBtcEventReplay, *tpSmeBtcEventReplay;

typedef struct sSmeBtcInfo
{
   tSmeBtcConfig btcConfig;
   v_BOOL_t      btcReady;
   v_U8_t        btcEventState;
   v_U8_t        btcHBActive;    /*                        */
   v_U8_t        btcHBCount;     /*                  */
   vos_timer_t   restoreHBTimer; /*                             */
   tSmeBtcEventReplay btcEventReplay;
   v_BOOL_t      fReplayBTEvents;
   v_BOOL_t      btcUapsdOk;  /*                                       */
   v_BOOL_t      fA2DPTrafStop;/*                                                          */
   v_U16_t       btcScoHandles[BT_MAX_SCO_SUPPORT];  /*                        */
   v_BOOL_t      fA2DPUp;        /*                                   */
   v_BOOL_t      btcScanCompromise;
   v_U8_t        btcBssfordisableaggr[VOS_MAC_ADDRESS_LEN];
} tSmeBtcInfo, *tpSmeBtcInfo;


/*                     
*/

#ifndef WLAN_MDM_CODE_REDUCTION_OPT
VOS_STATUS btcOpen (tHalHandle hHal);
VOS_STATUS btcClose (tHalHandle hHal);
VOS_STATUS btcReady (tHalHandle hHal);
VOS_STATUS btcSendCfgMsg(tHalHandle hHal, tpSmeBtcConfig pSmeBtcConfig);
VOS_STATUS btcSignalBTEvent (tHalHandle hHal, tpSmeBtEvent pBtEvent);
VOS_STATUS btcSetConfig (tHalHandle hHal, tpSmeBtcConfig pSmeBtcConfig);
VOS_STATUS btcGetConfig (tHalHandle hHal, tpSmeBtcConfig pSmeBtcConfig);
/*
                                                                                 
        
                                              
                                                                       
*/
v_BOOL_t btcIsReadyForUapsd( tHalHandle hHal );
eHalStatus btcHandleCoexInd(tHalHandle hHal, void* pMsg);
#endif /*                                    */

#endif

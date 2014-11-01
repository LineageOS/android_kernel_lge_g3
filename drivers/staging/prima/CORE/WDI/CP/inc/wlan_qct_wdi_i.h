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


#ifndef WLAN_QCT_WDI_I_H
#define WLAN_QCT_WDI_I_H

/*===========================================================================

         W L A N   D E V I C E   A B S T R A C T I O N   L A Y E R 
              I N T E R N A L     A P I       F O R    T H E
                                 D A T A   P A T H 
                
                   
DESCRIPTION
  This file contains the internal API exposed by the DAL Control Path Core 
  module to be used by the DAL Data Path Core. 
  
      
  Copyright (c) 2010 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary
===========================================================================*/


/*                                                                           

                                           


                                                                       
                                                                


                                   


                                   
                                                                             
                                                 
                                   

                                                                           */

#include "wlan_qct_pal_type.h"
#include "wlan_qct_pal_api.h"
#include "wlan_qct_pal_list.h"
#include "wlan_qct_pal_sync.h"
#include "wlan_qct_pal_timer.h"
#include "wlan_qct_wdi_cts.h" 
#include "wlan_qct_wdi_bd.h" 

#include "wlan_hal_msg.h"
#include "wlan_status_code.h"
#include "wlan_qct_dev_defs.h"
/*                                                                            
                                         
                                                                            */

/*                                                                    */
#define WDI_ASSERT(_cond) WPAL_ASSERT(_cond)

/*                                                                      
                                                                           
                                                       */
#define WDI_ERR_BASIC_OP_FAILURE           0 
#define WDI_ERR_TRANSPORT_FAILURE          1 
#define WDI_ERR_INVALID_RSP_FMT            2 
#define WDI_ERR_RSP_TIMEOUT                3 
#define WDI_ERR_DEV_INTERNAL_FAILURE       4

/*                                                                  
                                                                         
                                                         */
 
#ifdef WLAN_SOFTAP_VSTA_FEATURE
#define WDI_MAX_SUPPORTED_STAS   41
#else
#define WDI_MAX_SUPPORTED_STAS   12
#endif
#define WDI_MAX_SUPPORTED_BSS     5 

/*                               */
#define WDI_CT_CHANNEL_SIZE 4096

/*                                                             */
#define WDI_BSS_INVALID_IDX 0xFF

#define WDI_FTM_MAX_RECEIVE_BUFFER   6500

/*                                                                           
                              
                                                                           */      
typedef enum
{
  /*                                                                         
             */
  WDI_INIT_ST = 0,      

  /*                                                                      
                                          */
  WDI_STARTED_ST,     

  /*                                                     */
  WDI_STOPPED_ST, 

  /*                                                                       
                             */
  WDI_BUSY_ST,  

  /*                                                            */
  WDI_SHUTDOWN_ST,

  WDI_MAX_ST
}WDI_MainStateType;


/*                                                                           
                              
                                                                           */      
typedef enum
{
  /*                                                                            
                                                                          
                 */
  WDI_SCAN_INITIALIZED_ST = 0, 

  /*                                                                        
                                                                    */
  WDI_SCAN_STARTED_ST     = 1, 

  /*                                                                           
                                                                           
            */
  WDI_SCAN_ENDED_ST       = 2, 

  /*                                                                            
                                                                           
                                                   */
  WDI_SCAN_FINISHED_ST    = 3,

  WDI_SCAN_MAX_ST
}WDI_ScanStateType;

/*                                                                            
                                                                     
                                             
                                                                            */
#define WDI_MAX_BSS_SESSIONS  10

typedef enum
{
  /*          */
  WDI_ASSOC_INIT_ST, 

  /*             */
  WDI_ASSOC_JOINING_ST, 

  /*                */
  WDI_ASSOC_POST_ST,

  WDI_ASSOC_MAX_ST
}WDI_AssocStateType;

/*                                                                            
                                   
                                                                            */
typedef enum
{
  /*                      */
  WDI_START_REQ  = 0, 

  /*                     */
  WDI_STOP_REQ   = 1, 

  /*                     */
  WDI_CLOSE_REQ  = 2, 


  /*    */
  /*                          */
  WDI_INIT_SCAN_REQ    = 3, 

  /*                           */
  WDI_START_SCAN_REQ   = 4, 

  /*                         */
  WDI_END_SCAN_REQ     = 5, 

  /*                            */
  WDI_FINISH_SCAN_REQ  = 6, 


  /*           */
  /*                     */
  WDI_JOIN_REQ          = 7, 

  /*                           */
  WDI_CONFIG_BSS_REQ    = 8, 

  /*                        */
  WDI_DEL_BSS_REQ       = 9, 

  /*                           */
  WDI_POST_ASSOC_REQ    = 10, 

  /*                        */
  WDI_DEL_STA_REQ       = 11, 

  /*        */
  /*                            */
  WDI_SET_BSS_KEY_REQ   = 12,
  
  /*                               */ 
  WDI_RMV_BSS_KEY_REQ   = 13,
  
  /*                            */ 
  WDI_SET_STA_KEY_REQ   = 14,
  
  /*                               */ 
  WDI_RMV_STA_KEY_REQ   = 15, 

  /*          */
  /*                          */
  WDI_ADD_TS_REQ        = 16,
  
  /*                             */ 
  WDI_DEL_TS_REQ        = 17,
  
  /*                                   */ 
  WDI_UPD_EDCA_PRMS_REQ = 18,

  /*                               */
  WDI_ADD_BA_SESSION_REQ = 19,

  /*                          */ 
  WDI_DEL_BA_REQ        = 20,

   /*                       */
  /*                               */ 
  WDI_CH_SWITCH_REQ     = 21,
  
  /*                           */ 
  WDI_CONFIG_STA_REQ    = 22, 

  /*                               */ 
  WDI_SET_LINK_ST_REQ   = 23,
  
  /*                          */ 
  WDI_GET_STATS_REQ     = 24, 

 /*                              */ 
  WDI_UPDATE_CFG_REQ    = 25, 

  /*                    */
  WDI_ADD_BA_REQ        = 26,

  /*                        */
  WDI_TRIGGER_BA_REQ    = 27,

  /*                                     */ 
  WDI_UPD_BCON_PRMS_REQ = 28,

  /*                                     */ 
  WDI_SND_BCON_REQ      = 29,

  /*                                             */ 
  WDI_UPD_PROBE_RSP_TEMPLATE_REQ = 30,

  /*                                  */ 
  WDI_SET_STA_BCAST_KEY_REQ      = 31,
  
  /*                                     */ 
  WDI_RMV_STA_BCAST_KEY_REQ      = 32, 

  /*                                 */
  WDI_SET_MAX_TX_POWER_REQ       = 33,
  
  /*                                           */
  WDI_P2P_GO_NOTICE_OF_ABSENCE_REQ    = 34,

  /*                           */ 
  WDI_ENTER_IMPS_REQ    = 35, 

  /*                          */ 
  WDI_EXIT_IMPS_REQ     = 36, 

  /*                           */ 
  WDI_ENTER_BMPS_REQ    = 37, 

  /*                          */ 
  WDI_EXIT_BMPS_REQ     = 38, 

  /*                            */ 
  WDI_ENTER_UAPSD_REQ   = 39, 

  /*                           */ 
  WDI_EXIT_UAPSD_REQ    = 40, 

  /*                                */  
  WDI_SET_UAPSD_PARAM_REQ = 41, 

  /*                                                 */
  WDI_UPDATE_UAPSD_PARAM_REQ = 42, 

  /*                                     */
  WDI_CONFIGURE_RXP_FILTER_REQ = 43, 

  /*                                        */
  WDI_SET_BEACON_FILTER_REQ = 44, 

  /*                                     */
  WDI_REM_BEACON_FILTER_REQ = 45, 

  /*                                    */
  WDI_SET_RSSI_THRESHOLDS_REQ = 46, 

  /*                             */
  WDI_HOST_OFFLOAD_REQ = 47, 

  /*                                 */
  WDI_WOWL_ADD_BC_PTRN_REQ = 48, 

  /*                                    */
  WDI_WOWL_DEL_BC_PTRN_REQ = 49, 

  /*                           */
  WDI_WOWL_ENTER_REQ = 50, 

  /*                          */
  WDI_WOWL_EXIT_REQ = 51, 

  /*                                                */
  WDI_CONFIGURE_APPS_CPU_WAKEUP_STATE_REQ = 52, 

  /*                           */
  WDI_NV_DOWNLOAD_REQ   = 53,
  /*                         */ 
  WDI_FLUSH_AC_REQ      = 54, 

  /*                             */ 
  WDI_BTAMP_EVENT_REQ   = 55, 
  /*                                     */
  WDI_AGGR_ADD_TS_REQ   = 56,

  WDI_ADD_STA_SELF_REQ       = 57,

  WDI_DEL_STA_SELF_REQ       = 58,

  /*                          */
  WDI_FTM_CMD_REQ            = 59,

  /*                                */
  WDI_START_OEM_DATA_REQ   = 60,
  /*                          */
  WDI_HOST_RESUME_REQ      = 61,
  
  WDI_KEEP_ALIVE_REQ       = 62,  

  /*         */
  WDI_SET_PREF_NETWORK_REQ     = 63,

  /*                   */
  WDI_SET_RSSI_FILTER_REQ      = 64,

  /*                       */
  WDI_UPDATE_SCAN_PARAMS_REQ   = 65,

  WDI_SET_TX_PER_TRACKING_REQ = 66,

  WDI_8023_MULTICAST_LIST_REQ                   = 67,
  WDI_RECEIVE_FILTER_SET_FILTER_REQ             = 68,
  WDI_PACKET_COALESCING_FILTER_MATCH_COUNT_REQ  = 69,
  WDI_RECEIVE_FILTER_CLEAR_FILTER_REQ           = 70,
  
  /*                                         
                                  */
  WDI_INIT_SCAN_CON_REQ                         = 71, 

  /*                               */
  WDI_HAL_DUMP_CMD_REQ                          = 72,

  /*                           */
  WDI_SHUTDOWN_REQ                              = 73,

  /*                                  */
  WDI_SET_POWER_PARAMS_REQ                      = 74,

  /*                                          */
  WDI_TSM_STATS_REQ                             = 75,
  /*                   */
  WDI_GTK_OFFLOAD_REQ             = 76, 
  WDI_GTK_OFFLOAD_GETINFO_REQ   = 77, 

  /*                                   */
  WDI_SET_TM_LEVEL_REQ                          = 78, 

  /*                                           */
  WDI_FEATURE_CAPS_EXCHANGE_REQ                 = 79,

#ifdef WLAN_FEATURE_11AC
  /*                                           */
  WDI_UPDATE_VHT_OP_MODE_REQ                    = 80,
#endif

  /*                              */
  WDI_GET_ROAM_RSSI_REQ                         = 81,

  /*                             */
  WDI_SET_TX_POWER_REQ                          = 82,
  WDI_ROAM_SCAN_OFFLOAD_REQ                     = 83,

  WDI_TDLS_LINK_ESTABLISH_REQ                   = 84,

  /*                             */
  WDI_LPHB_CFG_REQ                              = 85,

  /*                                */
  WDI_SET_BATCH_SCAN_REQ                        = 86,

  /*                                          */
  WDI_SET_MAX_TX_POWER_PER_BAND_REQ             = 87,

  WDI_UPDATE_CHAN_REQ                           = 88,

  WDI_MAX_REQ,

  /*                                     */
  WDI_HOST_SUSPEND_IND          = WDI_MAX_REQ ,

  /*                                        */
  WDI_TRAFFIC_STATS_IND,

  /*                       */
  WDI_DHCP_START_IND,

  /*                      */
  WDI_DHCP_STOP_IND,

  /*                                                   */
  WDI_EXCLUDE_UNENCRYPTED_IND,

  /*                                                   */
  WDI_ADD_PERIODIC_TX_PATTERN_IND,

  /*                                                    */
  WDI_DEL_PERIODIC_TX_PATTERN_IND,

  /*                                     */
  WDI_STOP_BATCH_SCAN_IND,

  /*                                     */
  WDI_TRIGGER_BATCH_SCAN_RESULT_IND,

  /*                                              
                                     */

  WDI_MAX_UMAC_IND
}WDI_RequestEnumType; 

/*                                                                            
                                    
                                                                            */
typedef enum
{
  /*                       */
  WDI_START_RESP  = 0, 

  /*                      */
  WDI_STOP_RESP   = 1, 

  /*                      */
  WDI_CLOSE_RESP  = 2, 

  /*    */
  /*                           */
  WDI_INIT_SCAN_RESP    = 3, 

  /*                            */
  WDI_START_SCAN_RESP   = 4, 

  /*                          */
  WDI_END_SCAN_RESP     = 5, 

  /*                             */
  WDI_FINISH_SCAN_RESP  = 6, 


  /*           */
  /*                      */
  WDI_JOIN_RESP          = 7, 

  /*                            */
  WDI_CONFIG_BSS_RESP    = 8, 

  /*                         */
  WDI_DEL_BSS_RESP       = 9, 

  /*                            */
  WDI_POST_ASSOC_RESP    = 10, 

  /*                         */
  WDI_DEL_STA_RESP       = 11, 

  /*                             */
  WDI_SET_BSS_KEY_RESP   = 12,
  
  /*                                */ 
  WDI_RMV_BSS_KEY_RESP   = 13,
  
  /*                             */ 
  WDI_SET_STA_KEY_RESP   = 14,
  
  /*                                */ 
  WDI_RMV_STA_KEY_RESP   = 15, 

  /*                           */
  WDI_ADD_TS_RESP        = 16,
  
  /*                              */ 
  WDI_DEL_TS_RESP        = 17,
  
  /*                                    */ 
  WDI_UPD_EDCA_PRMS_RESP = 18,

  /*                                */
  WDI_ADD_BA_SESSION_RESP = 19,

  /*                           */ 
  WDI_DEL_BA_RESP        = 20,

  /*                                */ 
  WDI_CH_SWITCH_RESP     = 21,
  
  /*                            */ 
  WDI_CONFIG_STA_RESP    = 22, 

  /*                                */ 
  WDI_SET_LINK_ST_RESP   = 23,
  
  /*                           */ 
  WDI_GET_STATS_RESP     = 24, 

  /*                               */ 
  WDI_UPDATE_CFG_RESP    = 25, 

  /*                     */
  WDI_ADD_BA_RESP        = 26,

  /*                         */
  WDI_TRIGGER_BA_RESP    = 27,

 /*                                       */
  WDI_UPD_BCON_PRMS_RESP = 28,
  
 /*                                      */
  WDI_SND_BCON_RESP = 29,

  /*                                                */
  WDI_UPD_PROBE_RSP_TEMPLATE_RESP = 30,

  /*                             */ 
  WDI_SET_STA_BCAST_KEY_RESP      = 31,
  
  /*                                */ 
  WDI_RMV_STA_BCAST_KEY_RESP      = 32, 

  /*                                  */
  WDI_SET_MAX_TX_POWER_RESP       = 33,

  /*                            */ 
  WDI_ENTER_IMPS_RESP     = 34, 

  /*                           */ 
  WDI_EXIT_IMPS_RESP      = 35, 

  /*                            */ 
  WDI_ENTER_BMPS_RESP    = 36, 

  /*                           */ 
  WDI_EXIT_BMPS_RESP      = 37, 

  /*                             */ 
  WDI_ENTER_UAPSD_RESP    = 38, 

  /*                            */ 
  WDI_EXIT_UAPSD_RESP     = 39, 

  /*                                 */  
  WDI_SET_UAPSD_PARAM_RESP = 40, 

  /*                                                  */
  WDI_UPDATE_UAPSD_PARAM_RESP = 41, 

  /*                                      */
  WDI_CONFIGURE_RXP_FILTER_RESP = 42, 

  /*                                   */
  WDI_SET_BEACON_FILTER_RESP = 43, 

  /*                                      */
  WDI_REM_BEACON_FILTER_RESP = 44, 

  /*                                     */
  WDI_SET_RSSI_THRESHOLDS_RESP = 45, 

  /*                                     */
  WDI_HOST_OFFLOAD_RESP = 46, 

  /*                                  */
  WDI_WOWL_ADD_BC_PTRN_RESP = 47, 

  /*                                     */
  WDI_WOWL_DEL_BC_PTRN_RESP = 48, 

  /*                            */
  WDI_WOWL_ENTER_RESP = 49, 

  /*                           */
  WDI_WOWL_EXIT_RESP = 50, 

  /*                                                 */
  WDI_CONFIGURE_APPS_CPU_WAKEUP_STATE_RESP = 51,
  
  /*                           */
  WDI_NV_DOWNLOAD_RESP = 52,

  /*                          */ 
  WDI_FLUSH_AC_RESP      = 53,

  /*                          */ 
  WDI_BTAMP_EVENT_RESP   = 54,

  /*                                      */
  WDI_AGGR_ADD_TS_RESP  = 55,

  /*                     */
  WDI_ADD_STA_SELF_RESP = 56,
  
  /*                        */
  WDI_DEL_STA_SELF_RESP       = 57,

  /*                            */
  WDI_START_OEM_DATA_RESP   = 58,

  /*                          */
  WDI_HOST_RESUME_RESP        = 59,

  /*                                            */
  WDI_P2P_GO_NOTICE_OF_ABSENCE_RESP    = 60,

  /*                       */
  WDI_FTM_CMD_RESP                     = 61,

  /*                    */
  WDI_KEEP_ALIVE_RESP                  = 62,

  /*                  */
  WDI_SET_PREF_NETWORK_RESP            = 63,

  /*                          */
  WDI_SET_RSSI_FILTER_RESP             = 64,

  /*                             */
  WDI_UPDATE_SCAN_PARAMS_RESP          = 65,

  //               
  WDI_SET_TX_PER_TRACKING_RESP       = 66,


  
  /*                           */
  WDI_8023_MULTICAST_LIST_RESP                  = 67,

  WDI_RECEIVE_FILTER_SET_FILTER_RESP            = 68,

  WDI_PACKET_COALESCING_FILTER_MATCH_COUNT_RESP = 69,

  WDI_RECEIVE_FILTER_CLEAR_FILTER_RESP          = 70,

  
  /*                                */
  WDI_HAL_DUMP_CMD_RESP                         = 71,

  /*                        */
  WDI_SHUTDOWN_RESP                             = 72,

  /*                              */
  WDI_SET_POWER_PARAMS_RESP                     = 73,

  WDI_TSM_STATS_RESP                            = 74,
  /*                   */
  WDI_GTK_OFFLOAD_RESP                          = 75, 
  WDI_GTK_OFFLOAD_GETINFO_RESP                  = 76, 

  WDI_SET_TM_LEVEL_RESP                         = 77,

  /*                                              */
  WDI_FEATURE_CAPS_EXCHANGE_RESP                = 78,

#ifdef WLAN_FEATURE_11AC
  WDI_UPDATE_VHT_OP_MODE_RESP                   = 79,
#endif

  /*                                */
  WDI_GET_ROAM_RSSI_RESP                        = 80,

  WDI_SET_TX_POWER_RESP                         = 81,
  WDI_ROAM_SCAN_OFFLOAD_RESP                    = 82,

  WDI_TDLS_LINK_ESTABLISH_REQ_RESP              = 83,

  /*                              */
  WDI_LPHB_CFG_RESP                             = 84,

  WDI_SET_BATCH_SCAN_RESP                       = 85,

  WDI_SET_MAX_TX_POWER_PER_BAND_RSP             = 86,

  WDI_UPDATE_CHAN_RESP                          = 87,
  /*                                                                         
               
                                               
                                                                             */
  WDI_HAL_IND_MIN                     , 
  /*                                                                         
            */
  WDI_HAL_RSSI_NOTIFICATION_IND       = WDI_HAL_IND_MIN, 

  /*                         */
  WDI_HAL_MISSED_BEACON_IND           = WDI_HAL_IND_MIN + 1,

  /*                                                                       
                                                                              
                */
  WDI_HAL_UNKNOWN_ADDR2_FRAME_RX_IND  = WDI_HAL_IND_MIN + 2,

  /*                          */
  WDI_HAL_MIC_FAILURE_IND             = WDI_HAL_IND_MIN + 3,

  /*               */
  WDI_HAL_FATAL_ERROR_IND             = WDI_HAL_IND_MIN + 4, 

  /*                                                                     
                                               */
  WDI_HAL_DEL_STA_IND                 = WDI_HAL_IND_MIN + 5,

  /*               */
  WDI_HAL_COEX_IND                    = WDI_HAL_IND_MIN + 6,

  /*                        */
  WDI_HAL_TX_COMPLETE_IND             = WDI_HAL_IND_MIN + 7,

  WDI_HAL_P2P_NOA_ATTR_IND            = WDI_HAL_IND_MIN + 8,

  /*                                    */
  WDI_HAL_PREF_NETWORK_FOUND_IND      = WDI_HAL_IND_MIN + 9,

  /*                          */
  WDI_HAL_WAKE_REASON_IND              = WDI_HAL_IND_MIN + 10,

  /*                       */
  WDI_HAL_TX_PER_HIT_IND              = WDI_HAL_IND_MIN + 11,

  /*                                      */
  WDI_HAL_P2P_NOA_START_IND            = WDI_HAL_IND_MIN + 12,

  /*                                 */
  WDI_HAL_TDLS_IND                     = WDI_HAL_IND_MIN + 13,

  /*                         */
  WDI_HAL_LPHB_WAIT_TIMEOUT_IND        = WDI_HAL_IND_MIN + 14,

  /*                                                 */
  WDI_HAL_IBSS_PEER_INACTIVITY_IND     = WDI_HAL_IND_MIN + 15,

  /*                                                */
  WDI_HAL_PERIODIC_TX_PTRN_FW_IND     = WDI_HAL_IND_MIN + 16,


  WDI_BATCHSCAN_RESULT_IND           =  WDI_HAL_IND_MIN + 17,

  WDI_HAL_CH_AVOID_IND                 = WDI_HAL_IND_MIN + 18,
  WDI_MAX_RESP
}WDI_ResponseEnumType; 

typedef struct 
{
  /*                                         */
  wpt_boolean         bInUse; 

  /*                                                                    
                                               
                                                                        
                                                         */
  wpt_boolean         bAssocReqQueued;

  /*                    */
  wpt_macAddr         macBSSID; 

  /*                                    */
  wpt_uint8           ucBSSIdx; 

  /*                                   */
  WDI_AssocStateType  wdiAssocState;

  /*                         */
  wpt_list            wptPendingQueue;

  /*                            */
  wpt_uint8           bcastDpuIndex;
  wpt_uint8           bcastDpuSignature;
  wpt_uint8           bcastMgmtDpuIndex;
  wpt_uint8           bcastMgmtDpuSignature;

  /*                    */
  wpt_uint8           ucRmfEnabled;

  /*                                              */
  wpt_uint8           bcastStaIdx;

  /*                                   */
  WDI_BssType         wdiBssType;
}WDI_BSSSessionType;

/*                                                                           
                          
                                                                           */
typedef WPT_PACK_PRE struct 
{
  /*                          */
  wpt_uint8    ucBSSIdx;

  /*                */
  wpt_macAddr  macBSSID; 

  /*                                                            
                              */
  wpt_uint8    ucBcastDpuDescIndx;

  /*                                                        */
  wpt_uint8    ucBcastDpuSignature;
  
  /*                                                                      
         */
  wpt_uint8    ucMgmtDpuDescIndx;

  /*                                                           */
  wpt_uint8    ucMgmtDpuSignature;

  /*                                        */
  eHalStatus   halStatus;
}WPT_PACK_POST WDI_ConfigBSSRspInfoType;


/*                                                                           
                          
                                                                           */
typedef WPT_PACK_PRE struct
{
  /*                           */
  wpt_uint16   usSTAIdx;
  
  /*                  */ 
  wpt_macAddr  macSTA;
  
  /*                     */
  wpt_uint8    ucUcastSig;

  /*                       */
  wpt_uint8    ucBcastSig;

  /*                */
  wpt_macAddr  macBSSID; 

  /*           */
  eHalStatus   halStatus;
}WPT_PACK_POST WDI_PostAssocRspInfoType;

/*                                                                            
                                
                                                                            */
typedef struct
{
  /*                                                                         
                           */
  wpt_list_node          wptListNode; 

  /*                 */
  WDI_RequestEnumType    wdiRequest;

  /*                  */
  WDI_ResponseEnumType   wdiResponse;

  /*                                 */
  void*                  pEventData;

  /*         */
  wpt_uint32             uEventDataSize;

  /*                                                         */
  void*                  pCBfnc;

  /*                                                    */
  void*                  pUserData;
}WDI_EventInfoType; 

/*                                                                            
                              
                                                                            */
typedef struct
{
  /*                                                                         
                           */
  wpt_list_node          wptListNode; 

  /*                                                  */
  wpt_uint8              ucIndex; 

}WDI_NextSessionIdType; 

#define WDI_CONTROL_BLOCK_MAGIC  0x67736887  /*                       */
/*                                                                            
                               
                                                                            */
typedef struct
{
  /*                                                             */
  wpt_uint32                  magic;

  /*                                            */
  void*                       pOSContext;

  /*                                        */
  void*                       pPALContext; 

  /*                                             */
  void*                       pDPContext; 

  /*                                                              */
  void*                       pDTDriverContext; 

  /*                                       */
  WCTS_HandleType             wctsHandle;

  /*                                            */
  wpt_boolean                 bCTOpened;

  /*                                        */
  WDI_MainStateType           uGlobalState; 

  /*                                                                    
                  */
  WDI_MainStateType           ucExpectedStateTransition;

  /*                                          */
  wpt_mutex                   wptMutex;

  /*                  */
  wpt_timer                   wptResponseTimer;

  /*                         */
  wpt_list                    wptPendingQueue;
#if 0
  /*                                                */
  WDI_ScanStateType           uScanState;  

  /*                                                        */
  wpt_boolean                 bScanInProgress;
#endif
  /*                                                                */
  wpt_boolean                 bAssociationInProgress; 

  /*                                  */
  WDI_BSSSessionType          aBSSSessions[WDI_MAX_BSS_SESSIONS];

  /*                                                                
                                                */
  wpt_list                    wptPendingAssocSessionIdQueue;

  /*                                                                      
            */

  /*                                                                  
                                                                     
                                                                       */
  WDI_PostAssocReqParamsType  wdiCachedPostAssocReq; 

  /*                                                                  
                                                                     
                                                                       */
  WDI_ConfigSTAReqParamsType  wdiCachedConfigStaReq; 

  /*                                                                  
                                                                     
                                                                   */
  WDI_ConfigBSSReqParamsType  wdiCachedConfigBssReq; 

  /*                                                                      
                                                                     
                                                                     */
  WDI_SetLinkReqParamsType     wdiCacheSetLinkStReq; 

  /*                                                                    
                                                                     
                                                                             */
  WDI_AddSTASelfReqParamsType  wdiCacheAddSTASelfReq;

  /*                             */
  wpt_uint8                   ucCurrentBSSSesIdx;

  /*                                                 */
  void*                       pfncRspCB;

  /*                                                              */
  void*                       pRspCBUserData; 

  /*                              */
  WDI_ResponseEnumType        wdiExpectedResponse;

  /*                                                                     
                                                                         
                             */
  WDI_ReqStatusCb             wdiReqStatusCB; 

  /*                                                                    
                                    */
  void*                       pReqStatusUserData;

   /*                                                                      
                                                          */
  WDI_LowLevelIndCBType       wdiLowLevelIndCB; 

  /*                                                                         
                                    */
  void*                       pIndUserData;

  /*                                */
  WDI_StartRspParamsType      wdiCachedStartRspParams;
  
  /*                                */
  WDI_NvBlobInfoParams        wdiNvBlobInfo;

  /*                     */
  /*                                         */
  wpt_uint8                   ucMaxStations;

  /*                                      */
  wpt_uint8                   ucMaxBssids;

  /*                                                             */
  void*                       staTable;

#ifndef HAL_SELF_STA_PER_BSS
  /*                      */
  wpt_uint8                   ucSelfStaId;

  /*                    */
  wpt_uint16                  usSelfStaDpuId;

  /*            */
  wpt_macAddr                 macSelfSta;
#endif

  /*                             */
  wpt_uint8                   bFrameTransEnabled;

  /*                                                             */
  WDI_RxBdType                wdiRxAmsduBdFixMask;

  /*                                                          */
  WDI_RxBdType                wdiRxAmsduFirstBdCache;

  /*                                       */
  wpt_uint32                  uBdSigSerialNum;

  /*                 
                                                            */  
  wpt_uint8                   ucDpuRF;    
  /*                                                          */
  wpt_event                   wctsActionEvent;
  /*                                                             */
  wpt_event                   setPowerStateEvent;
  /*                                                                             
                                       */
  unsigned int                dxePhyAddr;

  wpt_boolean                 dxeRingsEmpty;

  /*                                */
  WDI_NvDownloadReqParamsType   wdiCachedNvDownloadReq;

  /*             */
  tDriverType                 driverMode;  

  /*                                          */
  wpt_uint8                   ucFTMCommandRspBuffer[WDI_FTM_MAX_RECEIVE_BUFFER];

  /*                    */
  wpt_boolean                 bInBmps;

  /*                                         */
  wpt_uint8                   wdiPNOVersion;

  /*         */
  wpt_timer                   ssrTimer;

  /*                                                  */
  WDI_WlanVersionType wlanVersion;

  /*                                      */
  wpt_uint32                  uTimeStampRspTmrStart;

  /*                                          */
  wpt_uint32                  uTimeStampRspTmrExp;

  /*                                   */
  wpt_boolean                 bEnableSSR;
}WDI_ControlBlockType; 




/*                                                                           

              
                                                     
    
             

     
                                                    
                                                               
 
   
              
                                                              

                                                                           */
typedef WDI_Status (*WDI_ReqProcFuncType)( WDI_ControlBlockType*  pWDICtx,
                                           WDI_EventInfoType*     pEventData);


/*                                                                           

              
                                                      
    
             

     
                                                    
                                                               
 
   
              
                                                              

                                                                           */
typedef WDI_Status (*WDI_RspProcFuncType)( WDI_ControlBlockType*  pWDICtx,
                                           WDI_EventInfoType*     pEventData);




/*                                                                          
                                                        
                                                                          */

/*                                                                            
                              
                                                                            */
#define WDI_STATE_TRANSITION(_pctx, _st)   (_pctx->uGlobalState = _st)



/*                                                                           
                     
                                                                           */      
typedef enum
{
  /*                                  */
  WDI_START_EVENT          = 0,

  /*                                 */
  WDI_STOP_EVENT           = 1,

  /*                               */
  WDI_REQUEST_EVENT        = 2,

  /*                                   */
  WDI_RESPONSE_EVENT       = 3,

  /*                                  */
  WDI_CLOSE_EVENT          = 4,

  /*                                     */
  WDI_SHUTDOWN_EVENT       = 5,

  WDI_MAX_EVENT

}WDI_MainEventType;

/*                                                                           

              
                                                
    
             

     
                                                    
                                                               
 
   
              
                                                              

                                                                           */
typedef WDI_Status (*WDI_MainFuncType)( WDI_ControlBlockType*  pWDICtx,
                                        WDI_EventInfoType*     pEventData);

/*                                                                           
                         
                                                                           */      
typedef struct
{
  WDI_MainFuncType  pfnMainTbl[WDI_MAX_EVENT];
} WDI_MainFsmEntryType;

/*                                   */
#define WDI_VALID_SESSION_IDX(_idx)  ( _idx < WDI_MAX_BSS_SESSIONS ) 

/*                                                                           
 
                                                       
 
                                                                          */ 

/* 
                                                                 
                                                            
 
             
  
     
                                
*/
void* WDI_GET_PAL_CTX( void );

/*                                                                           
                                                      
                                                                           */      
/* 
                                                          

 
                                                          
                                                          
                                                          
                   
  
     
                                    
*/
WDI_Status
WDI_PostMainEvent
(
  WDI_ControlBlockType*  pWDICtx, 
  WDI_MainEventType      wdiEV, 
  WDI_EventInfoType*     pEventData
  
);

/*                                                                          
                       
                                                                          */
/* 
                                                          

 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_MainStart
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                 

 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_MainRspInit
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                          

 
                                                          
                                                                     
        
  
     
                                    
*/
WDI_Status
WDI_MainClose
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/*                                                                          
                          
                                                                          */
/* 
                                                 

 
                                                          
                                                                     
      
     
                                    
*/
WDI_Status
WDI_MainStartStarted
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                

 
                                                          
                                                                     
                                                        
                                                        
                                    
  
     
                                    
*/
WDI_Status
WDI_MainStopStarted
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                   

 
                                                          
                                                                     
        
  
     
                                    
*/
WDI_Status
WDI_MainReqStarted
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                             

 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_MainRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/*                                                                          
                          
                                                                          */
/* 
                                                

 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_MainStopStopped
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
  );

/*                                                                          
                       
                                                                          */
/* 
                                              

 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_MainStartBusy
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                             

 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_MainStopBusy
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                

 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_MainReqBusy
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                              

 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_MainCloseBusy
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                            


                                                         
                                                                    

     
                                    
*/
WDI_Status
WDI_MainShutdown
(
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                 


                                                         
                                                                    

     
                                    
*/
WDI_Status
WDI_MainShutdownBusy
(
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/*                                                                        
                                                       
                                                                        */

/* 
                                                             
                  
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessStartReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);


/* 
                                                            
                  
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessStopReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);


/* 
                                                             
                  
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessCloseReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                               
                  

                                                         
                                                                    

     
                                    
*/
WDI_Status
WDI_ProcessShutdownReq
(
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);


/* 
                                                                
                  

                                                         
                                                                    

     
                                    
*/
WDI_Status
WDI_ProcessInitScanReq
(
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);


/* 
                                                              
                      
 
                                                          
                                                                    

     
                                    
*/
WDI_Status
WDI_ProcessStartScanReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);


/* 
                                                                
                  
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessEndScanReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);


/* 
                                                               
                      
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessFinishScanReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);


/* 
                                                            
                  
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessJoinReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);


/* 
                                                              
                      
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessConfigBSSReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);


/* 
                                                               
                  
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessDelBSSReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                              
                      
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessPostAssocReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                               
                  
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessDelSTAReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                                  
                  
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessSetBssKeyReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                                     
                      
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessRemoveBssKeyReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                                  
                  
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessSetStaKeyReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                             
                           
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessRemoveStaKeyReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                                  
                  
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessSetStaBcastKeyReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                             
                           
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessRemoveStaBcastKeyReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                                
                  
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessAddTSpecReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);


/* 
                                                                
                  
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessDelTSpecReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                                
                           
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessUpdateEDCAParamsReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                              
                  
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessAddBASessionReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                              
                  
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessDelBAReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

#ifdef FEATURE_WLAN_CCX
/* 
                                                                
                  

                                                         
                                                                    

     
                                    
*/
WDI_Status
WDI_ProcessTSMStatsReq
(
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);
#endif

/* 
                                                             
                           
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessChannelSwitchReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                                  
                  
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessConfigStaReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);


/* 
                                                             
                           
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessSetLinkStateReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);


/* 
                                                                
                  
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessGetStatsReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

#if defined WLAN_FEATURE_VOWIFI_11R || defined FEATURE_WLAN_CCX || defined(FEATURE_WLAN_LFR)
/* 
                                                                    
                  

                                                         
                                                                    

     
                                    
*/
WDI_Status
WDI_ProcessGetRoamRssiReq
(
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);


/* 
                                                                     
                                             

                                                         
                                                                    

     
                                    
*/
WDI_Status
WDI_ProcessGetRoamRssiRsp
(
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

#endif


/* 
                                                              
                      
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessUpdateCfgReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                              
                  
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessAddBAReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                                  
                  
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessTriggerBAReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                                        
                      
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessUpdateBeaconParamsReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                                          
                      
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessSendBeaconParamsReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                                            
                  
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessUpdateProbeRspTemplateReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);
/* 
                                                                
                  
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessNvDownloadReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                                       
                      
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status WDI_ProcessSetMaxTxPowerReq
(
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                                            
                      

                                                         
                                                                    

     
                                    
*/
WDI_Status WDI_ProcessSetMaxTxPowerPerBandReq
(
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                               
                      

                                                         
                                                                    

     
                                    
*/
WDI_Status WDI_ProcessSetTxPowerReq
(
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                                            
                  
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessP2PGONOAReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                                          
                  

                                                         
                                                                    

     
                                    
*/
WDI_Status
WDI_ProcessTdlsLinkEstablishReq
(
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                         
                           
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessEnterImpsReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                        
                           
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessExitImpsReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                              
                      
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessEnterBmpsReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                                
                  
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessExitBmpsReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                               
                      
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessEnterUapsdReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                         
                           
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessExitUapsdReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                               
                           
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessSetUapsdAcParamsReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                             
                                
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessUpdateUapsdParamsReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                              
                                
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessConfigureRxpFilterReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                           
                                
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessSetBeaconFilterReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                              
                                
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessRemBeaconFilterReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                             
                                
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessSetRSSIThresholdsReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                             
                                
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessHostOffloadReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                    
                                
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessKeepAliveReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);


/* 
                                                          
                                
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessWowlAddBcPtrnReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                             
                                
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessWowlDelBcPtrnReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                    
                                
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessWowlEnterReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                                
                  
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessWowlExitReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                                
                                        
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessConfigureAppsCpuWakeupStateReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                                
                  
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessFlushAcReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

#ifdef FEATURE_OEM_DATA_SUPPORT
/* 
                                                                  
                      
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessStartOemDataReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);
#endif

/* 
                                                               
                      
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessHostResumeReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                                
                      
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessBtAmpEventReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                                   
                  
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessAddSTASelfReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                                
                      
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessDelSTASelfReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                                            
                                
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessSetTxPerTrackingReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                 
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessSetPowerParamsReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                            
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessSetTmLevelReq
(
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

#ifdef FEATURE_WLAN_LPHB
/* 
                                
                                    

                              
                                     

     
                                   
*/
WDI_Status WDI_ProcessLPHBConfReq
(
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);
#endif /*                   */

#ifdef FEATURE_WLAN_BATCH_SCAN
/* 
                                    
                                                   

                              
                                     

     
                           
*/
WDI_Status WDI_ProcessSetBatchScanReq
(
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                    
                                     

                              
                                     

     
                           
*/
WDI_Status WDI_ProcessGetBatchScanReq
(
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);
#endif /*                         */

/* 
                                           
                                     

                              
                                     

     
                           
*/
WDI_Status WDI_ProcessUpdateChannelParamsReq
(
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/*                                                                         
                                        
                                                                         */

/* 
                                                                             
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessHostSuspendInd
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                   

                                                         
                                                                    

     
                                    
*/
WDI_Status
WDI_ProcessDHCPStartInd
(
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                  

                                                         
                                                                    

     
                                    
*/
WDI_Status
WDI_ProcessDHCPStopInd
(
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);


/* 
                                                                                   

                                                         
                                                                    

     
                                    
*/
WDI_Status
WDI_ProcessTrafficStatsInd
(
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

#ifdef WLAN_FEATURE_11W
/* 
                                                                
                                

                                                         
                                                                         

     
                                    
*/
WDI_Status
WDI_ProcessExcludeUnencryptInd
(
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);
#endif

/* 
                                                                        
                              

                                                        
                                                                   

     
                                    
*/
WDI_Status
WDI_ProcessAddPeriodicTxPtrnInd
(
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                                           
                              

                                                        
                                                                   

     
                                    
*/
WDI_Status
WDI_ProcessDelPeriodicTxPtrnInd
(
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

#ifdef FEATURE_WLAN_BATCH_SCAN
/* 
                                                     
                                             

                                                          
                                                                     

      
                                     
 */
 WDI_Status
 WDI_ProcessStopBatchScanInd
 (
   WDI_ControlBlockType*  pWDICtx,
   WDI_EventInfoType*     pEventData
 );

/* 
                                                                 
                                             

                                                          
                                                                     

      
                                     
 */
 WDI_Status
 WDI_ProcessTriggerBatchScanResultInd
 (
   WDI_ControlBlockType*  pWDICtx,
   WDI_EventInfoType*     pEventData
 );

#endif

/*                                                                        
                                                        
                                                                        */


/* 
                                                               
                                                
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessStartRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);


/* 
                                                              
                                                
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessStopRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);


/* 
                                                          
                                                
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessCloseRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                     
                                                      
                                         

                                                         
                                                                    

     
                                    
*/
WDI_Status
WDI_ProcessShutdownRsp
(
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                              
                                                
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessInitScanRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);


/* 
                                                               
                                                
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessStartScanRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);


/* 
                                                          
                                                         
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessEndScanRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);


/* 
                                                             
                                                         
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessFinishScanRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);


/* 
                                                              
                                                
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessJoinRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);


/* 
                                                            
                                                         
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessConfigBSSRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);


/* 
                                                                 
                                                
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessDelBSSRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                               
                                                
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessPostAssocRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                                
                                                
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessDelSTARsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                                
                                                
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessSetBssKeyRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                                   
                                                
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessRemoveBssKeyRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);


/* 
                                                                
                                                
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessSetStaKeyRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                           
                                                         
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessRemoveStaKeyRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);


/* 
                                                              
                                                         
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessSetStaBcastKeyRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                                
                                                         
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessRemoveStaBcastKeyRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                              
                                                
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessAddTSpecRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);


/* 
                                                              
                                                
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessDelTSpecRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                                    
                                                         
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessUpdateEDCAParamsRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);


/* 
                                                           
                                                
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessAddBASessionRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);


/* 
                                                           
                                                
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessDelBARsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

#ifdef FEATURE_WLAN_CCX
/* 
                                                              
                                                

                                                         
                                                                    

     
                                    
*/
WDI_Status
WDI_ProcessTsmStatsRsp
(
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

#endif


/* 
                                                                   
                                                
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessChannelSwitchRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);


/* 
                                                               
                                                
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessConfigStaRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);


/* 
                                                           
                                                         
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessSetLinkStateRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                                      
                                             

                                                         
                                                                    

     
                                    
*/
WDI_Status
WDI_ProcessUpdateChanRsp
(
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                                    
                                             
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessGetStatsRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);


/* 
                                                                    
                                             
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessUpdateCfgRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                           
                                                
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessAddBARsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                           
                                                
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessTriggerBARsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                                              
                                             
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessUpdateBeaconParamsRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                                              
                                             
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessSendBeaconParamsRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                                
                                                                
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessUpdateProbeRspTemplateRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);
  /* 
                                                                     
                                                
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessSetMaxTxPowerRsp
( 
  WDI_ControlBlockType*          pWDICtx,
  WDI_EventInfoType*             pEventData
);

/* 
                                                                              
                                                

                                                         
                                                                    

     
                                    
*/
WDI_Status
WDI_ProcessSetMaxTxPowerPerBandRsp
(
  WDI_ControlBlockType*          pWDICtx,
  WDI_EventInfoType*             pEventData
);

  /* 
                                                                 
                                                

                                                         
                                                                    

     
                                    
*/
WDI_Status
WDI_ProcessSetTxPowerRsp
(
  WDI_ControlBlockType*          pWDICtx,
  WDI_EventInfoType*             pEventData
);

  /* 
                                                                            
                                                

                                                         
                                                                    

     
                                    
*/
WDI_Status
WDI_ProcessLinkEstablishReqRsp
(
  WDI_ControlBlockType*          pWDICtx,
  WDI_EventInfoType*             pEventData
);

/* 
                                                  
                                                
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessNvDownloadRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                                       
                                                                
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessP2PGONOARsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                                
                                                
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessEnterImpsRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                               
                                                
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessExitImpsRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                                
                                                
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessEnterBmpsRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                               
                                                
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessExitBmpsRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                                
                                                
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessEnterUapsdRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                                
                                                
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessExitUapsdRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                             
                                                         
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessSetUapsdAcParamsRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                                
                                                         
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessUpdateUapsdParamsRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                                
                                                         
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessConfigureRxpFilterRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                             
                                                         
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessSetBeaconFilterRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                                
                                                         
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessRemBeaconFilterRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                               
                                                         
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessSetRSSIThresoldsRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                        
                                                         
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessHostOffloadRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                      
                                                         
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessKeepAliveRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);


/* 
                                                         
                                                         
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessWowlAddBcPtrnRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                             
                                                         
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessWowlDelBcPtrnRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                                
                                                
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessWowlEnterRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                               
                                                
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessWowlExitRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                             
                                                              
                 
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessConfigureAppsCpuWakeupStateRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);
/* 
                                                             
                                                
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessFlushAcRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                         
                                                         
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessBtAmpEventRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                  
                                                                
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessAddSTASelfRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

 /* 
                                                         
                                                         
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessDelSTASelfRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

#ifdef FEATURE_OEM_DATA_SUPPORT
/* 
                                                   
                                                         
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessStartOemDataRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);
#endif

 /* 
                                                         
                                                         
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessHostResumeRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                               
                                                         
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessSetTxPerTrackingRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                        
                                                         
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessSetPowerParamsRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                        
                                                         
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessSetTmLevelRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/*                                                                          
                                            
                                                                           */
/* 
                                                             
                                                              
                 
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessLowRSSIInd
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                               
                                                             
                     
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessMissedBeaconInd
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);


/* 
                                                                
                                                             
                     
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessUnkAddrFrameInd
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);


/* 
                                                                
                                                              
                 
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessMicFailureInd
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                               
                                                             
                     
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessFatalErrorInd
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                            
                                                             
                     
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessDelSTAInd
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                     
                                                             
                     
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessCoexInd
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                            
                                                             
                     

                                                         
                                                                    

     
                                    
*/
WDI_Status
WDI_ProcessTxCompleteInd
(
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                     
                                                             
                     

                                                         
                                                                    

     
                                    
*/
WDI_Status
WDI_ProcessTdlsInd
(
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                          
                                                             
                     

                                                         
                                                                    

     
                                    
*/
WDI_Status
WDI_ProcessP2pNoaStartInd
(
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                         
                                                             
                     

                                                         
                                                                    

     
                                    
*/
WDI_Status
WDI_ProcessP2pNoaAttrInd
(
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                           
                                                             
                     
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessTxPerHitInd
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

#ifdef FEATURE_WLAN_LPHB
/* 
                                       
                                                           
                      

                              
                                     

     
                                    
*/
WDI_Status
WDI_ProcessLphbWaitTimeoutInd
(
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);
#endif /*                   */

/* 
                                                          

                                                        
                                                                   

     
                                    
*/
WDI_Status
WDI_ProcessPeriodicTxPtrnFwInd
(
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

#ifdef WLAN_FEATURE_VOWIFI_11R
/* 
                                                                          
                  
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessAggrAddTSpecReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                              
                                                
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessAggrAddTSpecRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

#endif /*                         */

/* 
                                
                                                
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessFTMCommandReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                
                                                                      
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessFTMCommandRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);
/* 
                                
                                                     
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessHALDumpCmdReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                
                                                                           
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessHALDumpCmdRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                        
                                                           

                                                          
                                                                     

      
                                     
*/
WDI_Status
WDI_ProcessIbssPeerInactivityInd

(
 WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);


/*                                                                        
                                  
                                                                        */

/* 
                                                                
                              
 
                                              

                                    
*/
WDI_Status
WDI_CleanCB
(
  WDI_ControlBlockType*  pWDICtx
);

/* 
                                                          

 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessRequest
(
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                                
                                                             
                                               
 
                                                          
                                                        
                                            
                                                    
                                                               
                                           
                                                                
             
  
     
                                    
*/
WDI_Status
WDI_GetMessageBuffer
( 
  WDI_ControlBlockType*  pWDICtx, 
  WDI_RequestEnumType    wdiReqType, 
  wpt_uint16             usBufferLen,
  wpt_uint8**            pMsgBuffer, 
  wpt_uint16*            pusDataOffset, 
  wpt_uint16*            pusBufferSize
);

/* 
                                                                
                                                
 
                                                         
                                                               
                                 
  
     
              
*/
void
WDI_DetectedDeviceError
(
  WDI_ControlBlockType*  pWDICtx,
  wpt_uint16             usErrorCode
);

/*                                                                         
                                                   
                                                                         */

/* 
                                                                 
                                                        
        
    
                                               
                                                           
           
    
      
                                  
*/
WDI_Status
WDI_QueuePendingReq
(
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                              
                                                            
                                                 
        
    
                                               
    
      
                                  
*/
WDI_Status
WDI_ClearPendingRequests
( 
  WDI_ControlBlockType*  pWDICtx
);

/* 
                                                                 
                                                              
                                                             
                                                
        
    
                                                            
    
      
              
*/
void 
WDI_ResponseTimerCB
(
  void *pUserData
);

/*                                                                          
                                                 
 
                                                                           
                              
                                                                          */
/* 
                                                             
                                                         
                   
 
       
    
                                                              
                                              
                                                    
    
                         
  
              
*/
void 
WDI_NotifyMsgCTSCB
(
  WCTS_HandleType        wctsHandle, 
  WCTS_NotifyEventType   wctsEvent,
  void*                  wctsNotifyCBData
);

/* 
                                                             
                                                               
               
 
       
    
                                                         
                           
                                  
                                                   
    
                         
  
              
*/
void 
WDI_RXMsgCTSCB 
(
  WCTS_HandleType       wctsHandle, 
  void*                 pMsg,
  wpt_uint32            uLen,
  void*                 wctsRxMsgCBData
);

/* 
                                         

 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessResponse
(
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                                
                                                             
               
 
                                                          
                                           
  
                                                         
                                                               
           
                                                         
                 
                                                              
                                              
  
     
                                    
*/
WDI_Status 
WDI_SendMsg
( 
  WDI_ControlBlockType*  pWDICtx,  
  wpt_uint8*             pSendBuffer, 
  wpt_uint32             uSendSize, 
  void*                  pRspCb, 
  void*                  pUserData,
  WDI_ResponseEnumType   wdiExpectedResponse
);


/* 
                                                               
                                                               
                 
 
                                                          
                                           
                                                  
  
     
                                    
*/
WDI_Status 
WDI_SendIndication
( 
  WDI_ControlBlockType*  pWDICtx,  
  wpt_uint8*             pSendBuffer, 
  wpt_uint32             usSendSize
);

/* 
                                                                
                                                        
        
    
                                               
                                                           
           
    
      
                                  
*/
WDI_Status
WDI_DequeuePendingReq
(
  WDI_ControlBlockType*  pWDICtx
);

/* 
                                                               
                                                                
                                                             
        
    
                                               
                                                                
                   
    
      
                                  
*/
WDI_Status
WDI_QueueNewAssocRequest
(
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData,
  wpt_macAddr            macBSSID
); 

/* 
                                                               
                                                                
                                                             
        
    
                                               
                                         
                                                          
          
    
      
                                  
*/
WDI_Status
WDI_QueueAssocRequest
(
  WDI_ControlBlockType*  pWDICtx,
  WDI_BSSSessionType*    pSession,
  WDI_EventInfoType*     pEventData
);

/* 
                                                                
                                                  
                                                             
                                                 
        
    
                                               
  
    
      
                                  
*/
WDI_Status
WDI_DequeueAssocRequest
(
  WDI_ControlBlockType*  pWDICtx
);

/* 
                                                                              
  
 
                                                        
  
     
*/
void
WDI_ResetAssocSessions
( 
  WDI_ControlBlockType*   pWDICtx
);

/* 
                                                                
          
  
 
                                                        
                                                          
  
     
                                           
*/
wpt_uint8
WDI_FindEmptySession
( 
  WDI_ControlBlockType*   pWDICtx,
  WDI_BSSSessionType**    ppSession
);

/* 
                                                            
                


                                                       
                                                                       
                                                                                                            
                                                                                                             
                                                                     
     
                                  
*/
wpt_uint8
WDI_GetActiveSessionsCount
(
  WDI_ControlBlockType*   pWDICtx,
  wpt_macAddr             macBSSID,
  wpt_boolean             skipBSSID
);

/* 
                                                         
          
  
 
                                                        
                                                          
  
     
                                           
*/
void 
WDI_DeleteSession
( 
  WDI_ControlBlockType*   pWDICtx,
  WDI_BSSSessionType*     ppSession
);

/* 
                                                                 
  
 
                                                    
                                        
                                                         
  
     
                                           
*/
wpt_uint8
WDI_FindAssocSession
( 
  WDI_ControlBlockType*   pWDICtx,
  wpt_macAddr             macBSSID,
  WDI_BSSSessionType**    ppSession
);


/* 
                                                                 
  
 
                                                    
                                            
                                                         
  
     
                                           
*/
wpt_uint8
WDI_FindAssocSessionByBSSIdx
( 
  WDI_ControlBlockType*   pWDICtx,
  wpt_uint16              usBssIdx,
  WDI_BSSSessionType**    ppSession
);

/* 
                                                                 
  
 
                                                    
                                            
                                                         
  
     
                                           
*/
wpt_uint8
WDI_FindAssocSessionByIdx
( 
  WDI_ControlBlockType*   pWDICtx,
  wpt_uint16              usBssIdx,
  WDI_BSSSessionType**    ppSession
);

/* 
                                                                 
                                                     
                                                       
  
     
         
*/
void
WDI_DS_AssignDatapathContext 
(
  void *pContext, 
  void *pDPContext
);

/* 
                                                                 
  
 
                                                     
  
     
                                    
*/
void *
WDI_DS_GetDatapathContext 
(
  void *pContext
);

/* 
                                                                 
  
 
                                                     
                                                                     
  
     
             
*/
void
WDT_AssignTransportDriverContext 
(
  void *pContext, 
  void *pDTDriverContext
);

/* 
                                                                 
  
 
                                                    
  
     
                                     
*/
void *
WDT_GetTransportDriverContext 
(
  void *pContext
);

#ifdef FEATURE_WLAN_SCAN_PNO
/* 
                                                           
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessSetPreferredNetworkReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessSetRssiFilterReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                           
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessUpdateScanParamsReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                           
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessPrefNetworkFoundInd
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                               
                                                         
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessSetPreferredNetworkRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                       
                                                         
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessSetRssiFilterRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                              
                                                         
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessUpdateScanParamsRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);
#endif //                      


#ifdef WLAN_FEATURE_ROAM_SCAN_OFFLOAD
/* 
                                                            

                                                         
                                                                    

     
                                    
*/
WDI_Status
WDI_ProcessRoamScanOffloadReq
(
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);
/* 
                                                                            
                                                         

                                                         
                                                                    

     
                                    
*/
WDI_Status
WDI_ProcessRoamScanOffloadRsp
(
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);
#endif


#ifdef WLAN_FEATURE_PACKET_FILTERING
/* 
                                                    
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_Process8023MulticastListReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                          
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessReceiveFilterSetFilterReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                         
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessFilterMatchCountReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                            
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessReceiveFilterClearFilterReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                     
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_Process8023MulticastListRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                           
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessReceiveFilterSetFilterRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                          
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessFilterMatchCountRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                             
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessReceiveFilterClearFilterRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);
#endif //                              

#ifdef WLAN_FEATURE_GTK_OFFLOAD
/* 
                                                 
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessGTKOffloadReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                            
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessGTKOffloadGetInfoReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                        
                                                         
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessGtkOffloadRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                             
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessGTKOffloadGetInfoRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);
#endif //                         

#ifdef WLAN_WAKEUP_EVENTS
WDI_Status
WDI_ProcessWakeReasonInd
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);
#endif //                   

/* 
                                                            
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessFeatureCapsExchangeReq
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                                             
 
                                                          
                                                                     
  
     
                                    
*/
WDI_Status
WDI_ProcessFeatureCapsExchangeRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

#ifdef WLAN_FEATURE_11AC
WDI_Status
WDI_ProcessUpdateVHTOpModeReq
(
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

WDI_Status
WDI_ProcessUpdateVHTOpModeRsp
( 
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);
#endif

#ifdef WLAN_FEATURE_ROAM_SCAN_OFFLOAD
/* 
                                        
                                                             
                                                              
                                                                             
                                                              
        
                
    */
void
WDI_wdiEdTypeEncToEdTypeEnc
(
 tEdType *EdType,
 WDI_EdType wdiEdType
);
#endif

#ifdef FEATURE_WLAN_LPHB
/* 
                               
                                       

                              
                                     

     
                                    
*/
WDI_Status WDI_ProcessLphbCfgRsp
(
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);
#endif /*                   */

#ifdef FEATURE_WLAN_BATCH_SCAN
/* 
                                    
                                            

                              
                                     

     
                                    
*/
WDI_Status WDI_ProcessSetBatchScanRsp
(
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

/* 
                                           

                                                        
                                                                   

     
                                    
*/
WDI_Status
WDI_ProcessBatchScanResultInd
(
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);

#endif /*                         */

#ifdef FEATURE_WLAN_CH_AVOID
/* 
           


                              
                                     
     
                                    
*/
WDI_Status
WDI_ProcessChAvoidInd
(
  WDI_ControlBlockType*  pWDICtx,
  WDI_EventInfoType*     pEventData
);
#endif /*                       */

#endif /*                */


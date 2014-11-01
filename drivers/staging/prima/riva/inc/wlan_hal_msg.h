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

/*==========================================================================
 *
 *  @file:     wlan_hal_msg.h
 *
 *  @brief:    Exports and types for messages sent to HAL from WDI
 *
 *  @author:   Kumar Anand
 *
 *             Copyright (C) 2010, Qualcomm Technologies, Inc. 
 *             All rights reserved.
 *
 *=========================================================================*/

#ifndef _WLAN_HAL_MSG_H_
#define _WLAN_HAL_MSG_H_

#include "halLegacyPalTypes.h"
#include "halCompiler.h"
#include "wlan_qct_dev_defs.h"
#include "wlan_nv.h"

/*                                                                           
                            

                                                           
                                                                 
                                                  
                                                                 
                                            
                                                               
                                   
                                                                   
                              
                                                               
                                                                            */
#define WLAN_HAL_VER_MAJOR 1
#define WLAN_HAL_VER_MINOR 5
#define WLAN_HAL_VER_VERSION 1
#define WLAN_HAL_VER_REVISION 2

/*                                                                           
                        
                                                                            */

//                                                                  
#define WLAN_HAL_MAX_ENUM_SIZE    0x7FFFFFFF
#define WLAN_HAL_MSG_TYPE_MAX_ENUM_SIZE    0x7FFF

//                              
#define STACFG_MAX_TC    8

//                                    
#define WLAN_HAL_MAX_AC  4

typedef tANI_U8 tSirMacAddr[6];
typedef tANI_U8 tHalIpv4Addr[4];

#define HAL_MAC_ADDR_LEN        6
#define HAL_IPV4_ADDR_LEN       4

#define WALN_HAL_STA_INVALID_IDX 0xFF
#define WLAN_HAL_BSS_INVALID_IDX 0xFF

//                            
#define BEACON_TEMPLATE_SIZE 0x180 


//                        
#define MAX_TX_RATE_SAMPLES     10
//                        
#define MAX_BCN_RSSI_SAMPLES    10

//                                
#define PARAM_BCN_INTERVAL_CHANGED                      (1 << 0)
#define PARAM_SHORT_PREAMBLE_CHANGED                 (1 << 1)
#define PARAM_SHORT_SLOT_TIME_CHANGED                 (1 << 2)
#define PARAM_llACOEXIST_CHANGED                            (1 << 3)
#define PARAM_llBCOEXIST_CHANGED                            (1 << 4)
#define PARAM_llGCOEXIST_CHANGED                            (1 << 5)
#define PARAM_HT20MHZCOEXIST_CHANGED                  (1<<6)
#define PARAM_NON_GF_DEVICES_PRESENT_CHANGED (1<<7)
#define PARAM_RIFS_MODE_CHANGED                            (1<<8)
#define PARAM_LSIG_TXOP_FULL_SUPPORT_CHANGED   (1<<9)
#define PARAM_OBSS_MODE_CHANGED                               (1<<10)
#define PARAM_BEACON_UPDATE_MASK                (PARAM_BCN_INTERVAL_CHANGED|PARAM_SHORT_PREAMBLE_CHANGED|PARAM_SHORT_SLOT_TIME_CHANGED|PARAM_llACOEXIST_CHANGED |PARAM_llBCOEXIST_CHANGED|\
    PARAM_llGCOEXIST_CHANGED|PARAM_HT20MHZCOEXIST_CHANGED|PARAM_NON_GF_DEVICES_PRESENT_CHANGED|PARAM_RIFS_MODE_CHANGED|PARAM_LSIG_TXOP_FULL_SUPPORT_CHANGED| PARAM_OBSS_MODE_CHANGED)

/*                                 */
#define DUMPCMD_RSP_BUFFER 500

/*                                          */
#define WLAN_HAL_VERSION_LENGTH  64

#define WLAN_HAL_ROAM_SCAN_MAX_PROBE_SIZE     450
/*                                                  
                                     */
#define WLAN_HAL_ROAM_SCAN_MAX_CHANNELS       80
#define WLAN_HAL_ROAM_SCAN_RESERVED_BYTES     56

/*                                                          */
typedef enum 
{
   //            
   WLAN_HAL_START_REQ = 0,
   WLAN_HAL_START_RSP = 1,
   WLAN_HAL_STOP_REQ  = 2,
   WLAN_HAL_STOP_RSP  = 3,

   //    
   WLAN_HAL_INIT_SCAN_REQ    = 4,
   WLAN_HAL_INIT_SCAN_RSP    = 5,
   WLAN_HAL_START_SCAN_REQ   = 6,
   WLAN_HAL_START_SCAN_RSP   = 7 ,
   WLAN_HAL_END_SCAN_REQ     = 8,
   WLAN_HAL_END_SCAN_RSP     = 9,
   WLAN_HAL_FINISH_SCAN_REQ  = 10,
   WLAN_HAL_FINISH_SCAN_RSP  = 11,

   //                                     
   WLAN_HAL_CONFIG_STA_REQ   = 12,
   WLAN_HAL_CONFIG_STA_RSP   = 13,
   WLAN_HAL_DELETE_STA_REQ   = 14,
   WLAN_HAL_DELETE_STA_RSP   = 15,
   WLAN_HAL_CONFIG_BSS_REQ   = 16,
   WLAN_HAL_CONFIG_BSS_RSP   = 17,
   WLAN_HAL_DELETE_BSS_REQ   = 18,
   WLAN_HAL_DELETE_BSS_RSP   = 19,

   //                     
   WLAN_HAL_JOIN_REQ         = 20,
   WLAN_HAL_JOIN_RSP         = 21,
   WLAN_HAL_POST_ASSOC_REQ   = 22,
   WLAN_HAL_POST_ASSOC_RSP   = 23,

   //        
   WLAN_HAL_SET_BSSKEY_REQ   = 24,
   WLAN_HAL_SET_BSSKEY_RSP   = 25,
   WLAN_HAL_SET_STAKEY_REQ   = 26,
   WLAN_HAL_SET_STAKEY_RSP   = 27,
   WLAN_HAL_RMV_BSSKEY_REQ   = 28,
   WLAN_HAL_RMV_BSSKEY_RSP   = 29,
   WLAN_HAL_RMV_STAKEY_REQ   = 30,
   WLAN_HAL_RMV_STAKEY_RSP   = 31,

   //           
   WLAN_HAL_ADD_TS_REQ          = 32,
   WLAN_HAL_ADD_TS_RSP          = 33,
   WLAN_HAL_DEL_TS_REQ          = 34,
   WLAN_HAL_DEL_TS_RSP          = 35,
   WLAN_HAL_UPD_EDCA_PARAMS_REQ = 36,
   WLAN_HAL_UPD_EDCA_PARAMS_RSP = 37,
   WLAN_HAL_ADD_BA_REQ          = 38,
   WLAN_HAL_ADD_BA_RSP          = 39,
   WLAN_HAL_DEL_BA_REQ          = 40,
   WLAN_HAL_DEL_BA_RSP          = 41,

   WLAN_HAL_CH_SWITCH_REQ       = 42,
   WLAN_HAL_CH_SWITCH_RSP       = 43,
   WLAN_HAL_SET_LINK_ST_REQ     = 44,
   WLAN_HAL_SET_LINK_ST_RSP     = 45,
   WLAN_HAL_GET_STATS_REQ       = 46,
   WLAN_HAL_GET_STATS_RSP       = 47,
   WLAN_HAL_UPDATE_CFG_REQ      = 48,
   WLAN_HAL_UPDATE_CFG_RSP      = 49,

   WLAN_HAL_MISSED_BEACON_IND           = 50,
   WLAN_HAL_UNKNOWN_ADDR2_FRAME_RX_IND  = 51,
   WLAN_HAL_MIC_FAILURE_IND             = 52,
   WLAN_HAL_FATAL_ERROR_IND             = 53,
   WLAN_HAL_SET_KEYDONE_MSG             = 54,
   
   //            
   WLAN_HAL_DOWNLOAD_NV_REQ             = 55,
   WLAN_HAL_DOWNLOAD_NV_RSP             = 56,

   WLAN_HAL_ADD_BA_SESSION_REQ          = 57,
   WLAN_HAL_ADD_BA_SESSION_RSP          = 58,
   WLAN_HAL_TRIGGER_BA_REQ              = 59,
   WLAN_HAL_TRIGGER_BA_RSP              = 60,
   WLAN_HAL_UPDATE_BEACON_REQ           = 61,
   WLAN_HAL_UPDATE_BEACON_RSP           = 62,
   WLAN_HAL_SEND_BEACON_REQ             = 63,
   WLAN_HAL_SEND_BEACON_RSP             = 64,

   WLAN_HAL_SET_BCASTKEY_REQ               = 65,
   WLAN_HAL_SET_BCASTKEY_RSP               = 66,
   WLAN_HAL_DELETE_STA_CONTEXT_IND         = 67,
   WLAN_HAL_UPDATE_PROBE_RSP_TEMPLATE_REQ  = 68,
   WLAN_HAL_UPDATE_PROBE_RSP_TEMPLATE_RSP  = 69,
   
  //                      
   WLAN_HAL_PROCESS_PTT_REQ   = 70,
   WLAN_HAL_PROCESS_PTT_RSP   = 71,
   
   //                     
   WLAN_HAL_SIGNAL_BTAMP_EVENT_REQ  = 72,
   WLAN_HAL_SIGNAL_BTAMP_EVENT_RSP  = 73,
   WLAN_HAL_TL_HAL_FLUSH_AC_REQ     = 74,
   WLAN_HAL_TL_HAL_FLUSH_AC_RSP     = 75,

   WLAN_HAL_ENTER_IMPS_REQ           = 76,
   WLAN_HAL_EXIT_IMPS_REQ            = 77,
   WLAN_HAL_ENTER_BMPS_REQ           = 78,
   WLAN_HAL_EXIT_BMPS_REQ            = 79,
   WLAN_HAL_ENTER_UAPSD_REQ          = 80,
   WLAN_HAL_EXIT_UAPSD_REQ           = 81,
   WLAN_HAL_UPDATE_UAPSD_PARAM_REQ   = 82,
   WLAN_HAL_CONFIGURE_RXP_FILTER_REQ = 83,
   WLAN_HAL_ADD_BCN_FILTER_REQ       = 84,
   WLAN_HAL_REM_BCN_FILTER_REQ       = 85,
   WLAN_HAL_ADD_WOWL_BCAST_PTRN      = 86,
   WLAN_HAL_DEL_WOWL_BCAST_PTRN      = 87,
   WLAN_HAL_ENTER_WOWL_REQ           = 88,
   WLAN_HAL_EXIT_WOWL_REQ            = 89,
   WLAN_HAL_HOST_OFFLOAD_REQ         = 90,
   WLAN_HAL_SET_RSSI_THRESH_REQ      = 91,
   WLAN_HAL_GET_RSSI_REQ             = 92,
   WLAN_HAL_SET_UAPSD_AC_PARAMS_REQ  = 93,
   WLAN_HAL_CONFIGURE_APPS_CPU_WAKEUP_STATE_REQ = 94,

   WLAN_HAL_ENTER_IMPS_RSP           = 95,
   WLAN_HAL_EXIT_IMPS_RSP            = 96,
   WLAN_HAL_ENTER_BMPS_RSP           = 97,
   WLAN_HAL_EXIT_BMPS_RSP            = 98,
   WLAN_HAL_ENTER_UAPSD_RSP          = 99,
   WLAN_HAL_EXIT_UAPSD_RSP           = 100,
   WLAN_HAL_SET_UAPSD_AC_PARAMS_RSP  = 101,
   WLAN_HAL_UPDATE_UAPSD_PARAM_RSP   = 102,
   WLAN_HAL_CONFIGURE_RXP_FILTER_RSP = 103,
   WLAN_HAL_ADD_BCN_FILTER_RSP       = 104,
   WLAN_HAL_REM_BCN_FILTER_RSP       = 105,
   WLAN_HAL_SET_RSSI_THRESH_RSP      = 106,
   WLAN_HAL_HOST_OFFLOAD_RSP         = 107,
   WLAN_HAL_ADD_WOWL_BCAST_PTRN_RSP  = 108,
   WLAN_HAL_DEL_WOWL_BCAST_PTRN_RSP  = 109,
   WLAN_HAL_ENTER_WOWL_RSP           = 110,
   WLAN_HAL_EXIT_WOWL_RSP            = 111,
   WLAN_HAL_RSSI_NOTIFICATION_IND    = 112,
   WLAN_HAL_GET_RSSI_RSP             = 113,
   WLAN_HAL_CONFIGURE_APPS_CPU_WAKEUP_STATE_RSP = 114,

   //                  
   WLAN_HAL_SET_MAX_TX_POWER_REQ   = 115,
   WLAN_HAL_SET_MAX_TX_POWER_RSP   = 116,

   //                
   WLAN_HAL_AGGR_ADD_TS_REQ        = 117,
   WLAN_HAL_AGGR_ADD_TS_RSP        = 118,

   //                     
   WLAN_HAL_SET_P2P_GONOA_REQ      = 119,
   WLAN_HAL_SET_P2P_GONOA_RSP      = 120,
   
   //                  
   WLAN_HAL_DUMP_COMMAND_REQ       = 121,
   WLAN_HAL_DUMP_COMMAND_RSP       = 122,

   //                        
   WLAN_HAL_START_OEM_DATA_REQ   = 123,
   WLAN_HAL_START_OEM_DATA_RSP   = 124,

   //                        
   WLAN_HAL_ADD_STA_SELF_REQ       = 125,
   WLAN_HAL_ADD_STA_SELF_RSP       = 126,

   //                    
   WLAN_HAL_DEL_STA_SELF_REQ       = 127,
   WLAN_HAL_DEL_STA_SELF_RSP       = 128,

   //                
   WLAN_HAL_COEX_IND               = 129,

   //                        
   WLAN_HAL_OTA_TX_COMPL_IND       = 130,

   //                            
   WLAN_HAL_HOST_SUSPEND_IND       = 131,
   WLAN_HAL_HOST_RESUME_REQ        = 132,
   WLAN_HAL_HOST_RESUME_RSP        = 133,

   WLAN_HAL_SET_TX_POWER_REQ       = 134,
   WLAN_HAL_SET_TX_POWER_RSP       = 135,
   WLAN_HAL_GET_TX_POWER_REQ       = 136,
   WLAN_HAL_GET_TX_POWER_RSP       = 137,

   WLAN_HAL_P2P_NOA_ATTR_IND       = 138,
   
   WLAN_HAL_ENABLE_RADAR_DETECT_REQ  = 139,
   WLAN_HAL_ENABLE_RADAR_DETECT_RSP  = 140,
   WLAN_HAL_GET_TPC_REPORT_REQ       = 141,
   WLAN_HAL_GET_TPC_REPORT_RSP       = 142,
   WLAN_HAL_RADAR_DETECT_IND         = 143,
   WLAN_HAL_RADAR_DETECT_INTR_IND    = 144,
   WLAN_HAL_KEEP_ALIVE_REQ           = 145,
   WLAN_HAL_KEEP_ALIVE_RSP           = 146,      

   /*            */
   WLAN_HAL_SET_PREF_NETWORK_REQ     = 147,
   WLAN_HAL_SET_PREF_NETWORK_RSP     = 148,
   WLAN_HAL_SET_RSSI_FILTER_REQ      = 149,
   WLAN_HAL_SET_RSSI_FILTER_RSP      = 150,
   WLAN_HAL_UPDATE_SCAN_PARAM_REQ    = 151,
   WLAN_HAL_UPDATE_SCAN_PARAM_RSP    = 152,
   WLAN_HAL_PREF_NETW_FOUND_IND      = 153, 

   WLAN_HAL_SET_TX_PER_TRACKING_REQ  = 154,
   WLAN_HAL_SET_TX_PER_TRACKING_RSP  = 155,
   WLAN_HAL_TX_PER_HIT_IND           = 156,
   
   WLAN_HAL_8023_MULTICAST_LIST_REQ   = 157,
   WLAN_HAL_8023_MULTICAST_LIST_RSP   = 158,   

   WLAN_HAL_SET_PACKET_FILTER_REQ     = 159,
   WLAN_HAL_SET_PACKET_FILTER_RSP     = 160,   
   WLAN_HAL_PACKET_FILTER_MATCH_COUNT_REQ   = 161,
   WLAN_HAL_PACKET_FILTER_MATCH_COUNT_RSP   = 162,   
   WLAN_HAL_CLEAR_PACKET_FILTER_REQ         = 163,
   WLAN_HAL_CLEAR_PACKET_FILTER_RSP         = 164,  
   /*                                         
                                   */
   WLAN_HAL_INIT_SCAN_CON_REQ               = 165,
    
   WLAN_HAL_SET_POWER_PARAMS_REQ            = 166,
   WLAN_HAL_SET_POWER_PARAMS_RSP            = 167,

   WLAN_HAL_TSM_STATS_REQ                   = 168,
   WLAN_HAL_TSM_STATS_RSP                   = 169,

   //                             
   WLAN_HAL_WAKE_REASON_IND                 = 170,
   //                     
   WLAN_HAL_GTK_OFFLOAD_REQ                 = 171,
   WLAN_HAL_GTK_OFFLOAD_RSP                 = 172,
   WLAN_HAL_GTK_OFFLOAD_GETINFO_REQ         = 173,
   WLAN_HAL_GTK_OFFLOAD_GETINFO_RSP         = 174,

   WLAN_HAL_FEATURE_CAPS_EXCHANGE_REQ       = 175,
   WLAN_HAL_FEATURE_CAPS_EXCHANGE_RSP       = 176,
   WLAN_HAL_EXCLUDE_UNENCRYPTED_IND         = 177,

   WLAN_HAL_SET_THERMAL_MITIGATION_REQ      = 178,
   WLAN_HAL_SET_THERMAL_MITIGATION_RSP      = 179,

   WLAN_HAL_UPDATE_VHT_OP_MODE_REQ          = 182,
   WLAN_HAL_UPDATE_VHT_OP_MODE_RSP          = 183,

   WLAN_HAL_P2P_NOA_START_IND               = 184,

   WLAN_HAL_GET_ROAM_RSSI_REQ               = 185,
   WLAN_HAL_GET_ROAM_RSSI_RSP               = 186,
   
   WLAN_HAL_CLASS_B_STATS_IND               = 187,
   WLAN_HAL_DEL_BA_IND                      = 188,
   WLAN_HAL_DHCP_START_IND                  = 189,
   WLAN_HAL_DHCP_STOP_IND                   = 190,
   WLAN_ROAM_SCAN_OFFLOAD_REQ               = 191,
   WLAN_ROAM_SCAN_OFFLOAD_RSP               = 192,
   WLAN_HAL_WIFI_PROXIMITY_REQ              = 193,
   WLAN_HAL_WIFI_PROXIMITY_RSP              = 194,

   WLAN_HAL_START_SPECULATIVE_PS_POLLS_REQ  = 195,
   WLAN_HAL_START_SPECULATIVE_PS_POLLS_RSP  = 196,
   WLAN_HAL_STOP_SPECULATIVE_PS_POLLS_IND   = 197,

   WLAN_HAL_TDLS_LINK_ESTABLISHED_REQ       = 198,
   WLAN_HAL_TDLS_LINK_ESTABLISHED_RSP       = 199,
   WLAN_HAL_TDLS_LINK_TEARDOWN_REQ          = 200,
   WLAN_HAL_TDLS_LINK_TEARDOWN_RSP          = 201,
   WLAN_HAL_TDLS_IND                        = 202,
   WLAN_HAL_IBSS_PEER_INACTIVITY_IND        = 203,

   /*                   */
   WLAN_HAL_START_SCAN_OFFLOAD_REQ          = 204,
   WLAN_HAL_START_SCAN_OFFLOAD_RSP          = 205,
   WLAN_HAL_STOP_SCAN_OFFLOAD_REQ           = 206,
   WLAN_HAL_STOP_SCAN_OFFLOAD_RSP           = 207,
   WLAN_HAL_UPDATE_CHANNEL_LIST_REQ         = 208,
   WLAN_HAL_UPDATE_CHANNEL_LIST_RSP         = 209,
   WLAN_HAL_OFFLOAD_SCAN_EVENT_IND          = 210,

   /*                                              */
   WLAN_HAL_LPHB_CFG_REQ                    = 211,
   WLAN_HAL_LPHB_CFG_RSP                    = 212,
   WLAN_HAL_LPHB_IND                        = 213,

   WLAN_HAL_ADD_PERIODIC_TX_PTRN_IND        = 214,
   WLAN_HAL_DEL_PERIODIC_TX_PTRN_IND        = 215,
   WLAN_HAL_PERIODIC_TX_PTRN_FW_IND         = 216,

   //                                      
   WLAN_HAL_SET_MAX_TX_POWER_PER_BAND_REQ   = 217,
   WLAN_HAL_SET_MAX_TX_POWER_PER_BAND_RSP   = 218,

   /*                                                */
   WLAN_HAL_LBP_LEADER_REQ                  = 219,
   WLAN_HAL_LBP_LEADER_RSP                  = 220,
   WLAN_HAL_LBP_UPDATE_IND                  = 221,

   /*           */
   WLAN_HAL_BATCHSCAN_SET_REQ               = 222,
   WLAN_HAL_BATCHSCAN_SET_RSP               = 223,
   WLAN_HAL_BATCHSCAN_TRIGGER_RESULT_IND    = 224,
   WLAN_HAL_BATCHSCAN_RESULT_IND            = 225,
   WLAN_HAL_BATCHSCAN_STOP_IND              = 226,

   WLAN_HAL_GET_IBSS_PEER_INFO_REQ          = 227,
   WLAN_HAL_GET_IBSS_PEER_INFO_RSP          = 228,

   WLAN_HAL_RATE_UPDATE_IND                 = 229,

   /*                                    */
   WLAN_HAL_TX_FAIL_MONITOR_IND             = 230,
   WLAN_HAL_TX_FAIL_IND                     = 231,

   /*                              */
   WLAN_HAL_IP_FORWARD_TABLE_UPDATE_IND     = 232,

   WLAN_HAL_AVOID_FREQ_RANGE_IND            = 233,
  WLAN_HAL_MSG_MAX = WLAN_HAL_MSG_TYPE_MAX_ENUM_SIZE
}tHalHostMsgType;

/*                         */
typedef enum
{
   WLAN_HAL_MSG_VERSION0 = 0,
   WLAN_HAL_MSG_VERSION1 = 1,
   WLAN_HAL_MSG_WCNSS_CTRL_VERSION = 0x7FFF, /*                      */
   WLAN_HAL_MSG_VERSION_MAX_FIELD  = WLAN_HAL_MSG_WCNSS_CTRL_VERSION
}tHalHostMsgVersion;

/*                                              */
typedef enum tagAniBoolean 
{
    eANI_BOOLEAN_FALSE = 0,
    eANI_BOOLEAN_TRUE,
    eANI_BOOLEAN_OFF = 0,
    eANI_BOOLEAN_ON = 1,
    eANI_BOOLEAN_MAX_FIELD = 0x7FFFFFFF  /*                        */
} eAniBoolean;

typedef enum
{
   eDRIVER_TYPE_PRODUCTION  = 0,
   eDRIVER_TYPE_MFG         = 1,
   eDRIVER_TYPE_DVT         = 2,
   eDRIVER_TYPE_MAX         = WLAN_HAL_MAX_ENUM_SIZE
} tDriverType;

typedef enum
{
   HAL_STOP_TYPE_SYS_RESET,
   HAL_STOP_TYPE_SYS_DEEP_SLEEP,
   HAL_STOP_TYPE_RF_KILL,
   HAL_STOP_TYPE_MAX = WLAN_HAL_MAX_ENUM_SIZE
}tHalStopType;

typedef enum
{
   eHAL_SYS_MODE_NORMAL,
   eHAL_SYS_MODE_LEARN,
   eHAL_SYS_MODE_SCAN,
   eHAL_SYS_MODE_PROMISC,
   eHAL_SYS_MODE_SUSPEND_LINK,
   eHAL_SYS_MODE_ROAM_SCAN,
   eHAL_SYS_MODE_ROAM_SUSPEND_LINK,
   eHAL_SYS_MODE_OEM_DATA,
   eHAL_SYS_MODE_MAX = WLAN_HAL_MAX_ENUM_SIZE
} eHalSysMode;

typedef enum
{
    PHY_SINGLE_CHANNEL_CENTERED = 0,     //                                          
    PHY_DOUBLE_CHANNEL_LOW_PRIMARY = 1,  //                                                                   
    PHY_DOUBLE_CHANNEL_CENTERED = 2,     //                                          
    PHY_DOUBLE_CHANNEL_HIGH_PRIMARY = 3, //                                                                    
#ifdef WLAN_FEATURE_11AC
    PHY_QUADRUPLE_CHANNEL_20MHZ_LOW_40MHZ_CENTERED = 4, //                                            
    PHY_QUADRUPLE_CHANNEL_20MHZ_CENTERED_40MHZ_CENTERED = 5, //                                                 
    PHY_QUADRUPLE_CHANNEL_20MHZ_HIGH_40MHZ_CENTERED = 6, //                                             
    PHY_QUADRUPLE_CHANNEL_20MHZ_LOW_40MHZ_LOW = 7,//                                       
    PHY_QUADRUPLE_CHANNEL_20MHZ_HIGH_40MHZ_LOW = 8, //                                        
    PHY_QUADRUPLE_CHANNEL_20MHZ_LOW_40MHZ_HIGH = 9, //                                        
    PHY_QUADRUPLE_CHANNEL_20MHZ_HIGH_40MHZ_HIGH = 10,//                                         
#endif
    PHY_CHANNEL_BONDING_STATE_MAX = WLAN_HAL_MAX_ENUM_SIZE
}ePhyChanBondState;

//                                         
typedef enum eSirMacHTMIMOPowerSaveState
{
  eSIR_HT_MIMO_PS_STATIC = 0,    //                          
  eSIR_HT_MIMO_PS_DYNAMIC = 1,   //                           
  eSIR_HT_MIMO_PS_NA = 2,        //         
  eSIR_HT_MIMO_PS_NO_LIMIT = 3,  //                       
  eSIR_HT_MIMO_PS_MAX = WLAN_HAL_MAX_ENUM_SIZE
} tSirMacHTMIMOPowerSaveState;

/*                                                                       */
typedef enum eStaRateMode {
    eSTA_TAURUS = 0,
    eSTA_TITAN,
    eSTA_POLARIS,
    eSTA_11b,
    eSTA_11bg,
    eSTA_11a,
    eSTA_11n,
#ifdef WLAN_FEATURE_11AC
    eSTA_11ac,
#endif
    eSTA_INVALID_RATE_MODE = WLAN_HAL_MAX_ENUM_SIZE
} tStaRateMode, *tpStaRateMode;

#define SIR_NUM_11B_RATES           4  //          
#define SIR_NUM_11A_RATES           8  //                     
#define SIR_NUM_POLARIS_RATES       3  //         

#define SIR_MAC_MAX_SUPPORTED_MCS_SET    16


typedef enum eSirBssType
{
    eSIR_INFRASTRUCTURE_MODE,
    eSIR_INFRA_AP_MODE,                    //                        
    eSIR_IBSS_MODE,
    eSIR_BTAMP_STA_MODE,                   //                        
    eSIR_BTAMP_AP_MODE,                    //                        
    eSIR_AUTO_MODE,
    eSIR_DONOT_USE_BSS_TYPE = WLAN_HAL_MAX_ENUM_SIZE
} tSirBssType;

typedef enum eSirNwType
{
    eSIR_11A_NW_TYPE,
    eSIR_11B_NW_TYPE,
    eSIR_11G_NW_TYPE,
    eSIR_11N_NW_TYPE,
    eSIR_DONOT_USE_NW_TYPE = WLAN_HAL_MAX_ENUM_SIZE
} tSirNwType;

typedef tANI_U16 tSirMacBeaconInterval;

#define SIR_MAC_RATESET_EID_MAX            12

typedef enum eSirMacHTOperatingMode
{
  eSIR_HT_OP_MODE_PURE,                //              
  eSIR_HT_OP_MODE_OVERLAP_LEGACY,      //                                                      
  eSIR_HT_OP_MODE_NO_LEGACY_20MHZ_HT,  //                                        
  eSIR_HT_OP_MODE_MIXED,               //                       
  eSIR_HT_OP_MODE_MAX = WLAN_HAL_MAX_ENUM_SIZE
} tSirMacHTOperatingMode;

//                                     
typedef enum eAniEdType
{
    eSIR_ED_NONE,
    eSIR_ED_WEP40,
    eSIR_ED_WEP104,
    eSIR_ED_TKIP,
    eSIR_ED_CCMP,
    eSIR_ED_WPI,
    eSIR_ED_AES_128_CMAC,
    eSIR_ED_NOT_IMPLEMENTED = WLAN_HAL_MAX_ENUM_SIZE
} tAniEdType;

#define WLAN_MAX_KEY_RSC_LEN                16
#define WLAN_WAPI_KEY_RSC_LEN               16

//                                 
#define SIR_MAC_MAX_KEY_LENGTH              32
#define SIR_MAC_MAX_NUM_OF_DEFAULT_KEYS     4

//                                     
//                              
typedef enum eAniKeyDirection
{
    eSIR_TX_ONLY,
    eSIR_RX_ONLY,
    eSIR_TX_RX,
    eSIR_TX_DEFAULT,
    eSIR_DONOT_USE_KEY_DIRECTION = WLAN_HAL_MAX_ENUM_SIZE
} tAniKeyDirection;

typedef enum eAniWepType
{
    eSIR_WEP_STATIC,
    eSIR_WEP_DYNAMIC,
    eSIR_WEP_MAX = WLAN_HAL_MAX_ENUM_SIZE
} tAniWepType;

typedef enum eSriLinkState {

    eSIR_LINK_IDLE_STATE        = 0,
    eSIR_LINK_PREASSOC_STATE    = 1,
    eSIR_LINK_POSTASSOC_STATE   = 2,
    eSIR_LINK_AP_STATE          = 3,
    eSIR_LINK_IBSS_STATE        = 4,

    /*             */
    eSIR_LINK_BTAMP_PREASSOC_STATE  = 5,
    eSIR_LINK_BTAMP_POSTASSOC_STATE  = 6,
    eSIR_LINK_BTAMP_AP_STATE  = 7,
    eSIR_LINK_BTAMP_STA_STATE  = 8,
    
    /*                               */
    eSIR_LINK_LEARN_STATE       = 9,
    eSIR_LINK_SCAN_STATE        = 10,
    eSIR_LINK_FINISH_SCAN_STATE = 11,
    eSIR_LINK_INIT_CAL_STATE    = 12,
    eSIR_LINK_FINISH_CAL_STATE  = 13,
#ifdef WLAN_FEATURE_P2P
    eSIR_LINK_LISTEN_STATE      = 14,
    eSIR_LINK_SEND_ACTION_STATE = 15,
#endif
    eSIR_LINK_MAX = WLAN_HAL_MAX_ENUM_SIZE
} tSirLinkState;

typedef enum
{
    HAL_SUMMARY_STATS_INFO           = 0x00000001,
    HAL_GLOBAL_CLASS_A_STATS_INFO    = 0x00000002,
    HAL_GLOBAL_CLASS_B_STATS_INFO    = 0x00000004,
    HAL_GLOBAL_CLASS_C_STATS_INFO    = 0x00000008,
    HAL_GLOBAL_CLASS_D_STATS_INFO    = 0x00000010,
    HAL_PER_STA_STATS_INFO           = 0x00000020
}eHalStatsMask;

/*                    */
typedef enum 
{
    BTAMP_EVENT_CONNECTION_START,
    BTAMP_EVENT_CONNECTION_STOP,
    BTAMP_EVENT_CONNECTION_TERMINATED,
    BTAMP_EVENT_TYPE_MAX = WLAN_HAL_MAX_ENUM_SIZE, //                                  
} tBtAmpEventType;

//                                                               


/*                                                       */
typedef enum
{
    PE_SUMMARY_STATS_INFO           = 0x00000001,
    PE_GLOBAL_CLASS_A_STATS_INFO    = 0x00000002,
    PE_GLOBAL_CLASS_B_STATS_INFO    = 0x00000004,
    PE_GLOBAL_CLASS_C_STATS_INFO    = 0x00000008,
    PE_GLOBAL_CLASS_D_STATS_INFO    = 0x00000010,
    PE_PER_STA_STATS_INFO           = 0x00000020,
    PE_STATS_TYPE_MAX = WLAN_HAL_MAX_ENUM_SIZE //                                  
}ePEStatsMask;

/*                                                                           
                                                               
                                                                            */

#if defined(__ANI_COMPILER_PRAGMA_PACK_STACK)
#pragma pack(push, 1)
#elif defined(__ANI_COMPILER_PRAGMA_PACK)
#pragma pack(1)
#else
#endif

//                                 
typedef PACKED_PRE struct PACKED_POST
{
    tANI_U8                  revision;
    tANI_U8                  version;
    tANI_U8                  minor;
    tANI_U8                  major;
} tWcnssWlanVersion, *tpWcnssWlanVersion;

//                                
typedef PACKED_PRE struct PACKED_POST
{
    tANI_U8                  keyId;
    tANI_U8                  unicast;  //                
    tAniKeyDirection         keyDirection;
    tANI_U8                  keyRsc[WLAN_MAX_KEY_RSC_LEN];  //                 
    tANI_U8                  paeRole;  //                                       
    tANI_U16                 keyLength;
    tANI_U8                  key[SIR_MAC_MAX_KEY_LENGTH];
} tSirKeys, *tpSirKeys;


//                                                                          
typedef PACKED_PRE struct PACKED_POST
{
    /*         */
    tANI_U16        staIdx;

    /*                              */
    tAniEdType      encType;

    /*                                   */
    tAniWepType     wepType; 

    /*                                                                 */
    tANI_U8         defWEPIdx;

    /*                                           */
    tSirKeys        key[SIR_MAC_MAX_NUM_OF_DEFAULT_KEYS];            
  
    /*                                                                
                                           */
    tANI_U8         singleTidRc;

} tSetStaKeyParams, *tpSetStaKeyParams;



/*                                          */
typedef PACKED_PRE struct PACKED_POST
{
   tHalHostMsgType  msgType:16;
   tHalHostMsgVersion msgVersion:16;
   tANI_U32         msgLen;
} tHalMsgHeader, *tpHalMsgHeader;

/*                                                */
typedef PACKED_PRE struct PACKED_POST
{
   /*                                                  
                                                 */
   tANI_U16   uCfgId;

   /*                                                             
                        */
   tANI_U16   uCfgLen;

   /*                                       */
   tANI_U16   uCfgPadBytes;

   /*                                                  */
   tANI_U16   uCfgReserve;

   /*                                                              
                                                            */
} tHalCfg, *tpHalCfg;

/*                                                                           
                    
                                                                           */

typedef PACKED_PRE struct PACKED_POST sHalMacStartParameters
{
    /*                                    */
    tDriverType  driverType;

    /*                           */
    tANI_U32  uConfigBufferLen;

    /*                                                          
                                                              
                                                     
                                                                          
                                                                     
     */
} tHalMacStartParameters, *tpHalMacStartParameters;

typedef PACKED_PRE struct PACKED_POST
{
   /*                                                                    
                                                                    */
   tHalMsgHeader header;
   tHalMacStartParameters startReqParams;
}  tHalMacStartReqMsg, *tpHalMacStartReqMsg;

/*                                                                           
                    
                                                                           */

typedef PACKED_PRE struct PACKED_POST sHalMacStartRspParameters
{
   /*                   */
   tANI_U16  status;

   /*                                         */
   tANI_U8     ucMaxStations;

   /*                                         */
   tANI_U8     ucMaxBssids;

   /*            */
   tWcnssWlanVersion wcnssWlanVersion;

   /*                      */
   tANI_U8     wcnssCrmVersionString[WLAN_HAL_VERSION_LENGTH];

   /*                                          */
   tANI_U8     wcnssWlanVersionString[WLAN_HAL_VERSION_LENGTH];

} tHalMacStartRspParams, *tpHalMacStartRspParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalMacStartRspParams startRspParams;
}  tHalMacStartRspMsg, *tpHalMacStartRspMsg;

/*                                                                           
                   
                                                                           */

typedef PACKED_PRE struct PACKED_POST
{
  /*                                                */
  tHalStopType   reason;

}tHalMacStopReqParams, *tpHalMacStopReqParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalMacStopReqParams stopReqParams;
}  tHalMacStopReqMsg, *tpHalMacStopReqMsg;

/*                                                                           
                   
                                                                           */

typedef PACKED_PRE struct PACKED_POST
{
  /*                   */
  tANI_U32   status;

}tHalMacStopRspParams, *tpHalMacStopRspParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalMacStopRspParams stopRspParams;
}  tHalMacStopRspMsg, *tpHalMacStopRspMsg;

/*                                                                           
                         
                                                                           */

typedef PACKED_PRE struct PACKED_POST
{
    /*                                                                  */
    tANI_U32  uConfigBufferLen;

    /*                                                          
                                                              
                                                     
                                                                          
                                                                     
     */
} tHalUpdateCfgReqParams, *tpHalUpdateCfgReqParams;

typedef PACKED_PRE struct PACKED_POST
{
   /*                                                                     
                                                                     */
   tHalMsgHeader header;
   tHalUpdateCfgReqParams updateCfgReqParams;
}  tHalUpdateCfgReqMsg, *tpHalUpdateCfgReqMsg;

/*                                                                           
                         
                                                                           */

typedef PACKED_PRE struct PACKED_POST
{
  /*                    */
  tANI_U32   status;

}tHalUpdateCfgRspParams, *tpHalUpdateCfgRspParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalUpdateCfgRspParams updateCfgRspParams;
}  tHalUpdateCfgRspMsg, *tpHalUpdateCfgRspMsg;

/*                                                                           
                        
                                                                           */

//                                      
typedef  __ani_attr_pre_packed struct sSirMacFrameCtl
{

#ifndef ANI_LITTLE_BIT_ENDIAN

    tANI_U8 subType :4;
    tANI_U8 type :2;
    tANI_U8 protVer :2;

    tANI_U8 order :1;
    tANI_U8 wep :1;
    tANI_U8 moreData :1;
    tANI_U8 powerMgmt :1;
    tANI_U8 retry :1;
    tANI_U8 moreFrag :1;
    tANI_U8 fromDS :1;
    tANI_U8 toDS :1;

#else

    tANI_U8 protVer :2;
    tANI_U8 type :2;
    tANI_U8 subType :4;

    tANI_U8 toDS :1;
    tANI_U8 fromDS :1;
    tANI_U8 moreFrag :1;
    tANI_U8 retry :1;
    tANI_U8 powerMgmt :1;
    tANI_U8 moreData :1;
    tANI_U8 wep :1;
    tANI_U8 order :1;

#endif

} __ani_attr_packed  tSirMacFrameCtl, *tpSirMacFrameCtl;

//                        
typedef __ani_attr_pre_packed struct sSirMacSeqCtl
{
    tANI_U8 fragNum : 4;
    tANI_U8 seqNumLo : 4;
    tANI_U8 seqNumHi : 8;
} __ani_attr_packed tSirMacSeqCtl, *tpSirMacSeqCtl;

//                          
typedef __ani_attr_pre_packed struct sSirMacMgmtHdr
{
    tSirMacFrameCtl fc;
    tANI_U8           durationLo;
    tANI_U8           durationHi;
    tANI_U8              da[6];
    tANI_U8              sa[6];
    tANI_U8              bssId[6];
    tSirMacSeqCtl   seqControl;
} __ani_attr_packed tSirMacMgmtHdr, *tpSirMacMgmtHdr;

//                                     
typedef __ani_attr_pre_packed struct sSirScanEntry
{
    tANI_U8 bssIdx[HAL_NUM_BSSID];
    tANI_U8 activeBSScnt;
}__ani_attr_packed tSirScanEntry, *ptSirScanEntry;

typedef PACKED_PRE struct PACKED_POST {

    /*               
                     */
    eHalSysMode scanMode;

    /*                */
    tSirMacAddr bssid;

    /*                                */
    tANI_U8 notifyBss;

    /*                                                                       
                                                      */
    tANI_U8 frameType;

    /*                                                                     
                                                                           
                                                                               
                */
    tANI_U8 frameLength;

    /*                                                                      
                    */
    tSirMacMgmtHdr macMgmtHdr;

    /*                                        */
    tSirScanEntry scanEntry;

} tInitScanParams, * tpInitScanParams;

typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tInitScanParams initScanParams;
}  tHalInitScanReqMsg, *tpHalInitScanReqMsg;

typedef PACKED_PRE struct PACKED_POST {

    /*               
                     */
    eHalSysMode scanMode;

    /*                */
    tSirMacAddr bssid;

    /*                                */
    tANI_U8 notifyBss;

    /*                                                                       
                                                      */
    tANI_U8 frameType;

    /*                                                                     
                                                                           
                                                                               
                */
    tANI_U8 frameLength;

    /*                                                                      
                    */
    tSirMacMgmtHdr macMgmtHdr;

    /*                                        */
    tSirScanEntry scanEntry;

    /*                              */
    tANI_U8 useNoA;

    /*                                     */
    tANI_U16 scanDuration;

} tInitScanConParams, * tpInitScanConParams;

typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tInitScanConParams initScanParams;
}  tHalInitScanConReqMsg, *tpHalInitScanConReqMsg;


/*                                                                           
                        
                                                                           */

typedef PACKED_PRE struct PACKED_POST
{
  /*                   */
  tANI_U32   status;

}tHalInitScanRspParams, *tpHalInitScanRspParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalInitScanRspParams initScanRspParams;
}  tHalInitScanRspMsg, *tpHalInitScanRspMsg;

/*                                                                           
                         
                                                                           */

typedef PACKED_PRE struct PACKED_POST 
{
   /*                             */
   tANI_U8 scanChannel;

 } tStartScanParams, * tpStartScanParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tStartScanParams startScanParams;
}  tHalStartScanReqMsg, *tpHalStartScanReqMsg;

/*                                                                           
                         
                                                                           */

typedef PACKED_PRE struct PACKED_POST
{
  /*                   */
  tANI_U32   status;

  tANI_U32 startTSF[2];
  tPowerdBm txMgmtPower;

}tHalStartScanRspParams, *tpHalStartScanRspParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalStartScanRspParams startScanRspParams;
}  tHalStartScanRspMsg, *tpHalStartScanRspMsg;

/*                                                                           
                       
                                                                           */

typedef PACKED_PRE struct PACKED_POST
{
   /*                                                                      
                                                                     
                    */
    tANI_U8 scanChannel;

} tEndScanParams, *tpEndScanParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tEndScanParams endScanParams;
}  tHalEndScanReqMsg, *tpHalEndScanReqMsg;

/*                                                                           
                       
                                                                           */

typedef PACKED_PRE struct PACKED_POST
{
  /*                   */
  tANI_U32   status;

}tHalEndScanRspParams, *tpHalEndScanRspParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalEndScanRspParams endScanRspParams;
}  tHalEndScanRspMsg, *tpHalEndScanRspMsg;

/*                                                                           
                          
                                                                           */

typedef PACKED_PRE struct PACKED_POST
{
    /*                                               
                                       */
    eHalSysMode scanMode;

    /*                             */
    tANI_U8 currentOperChannel;

    /*                                                                         
                                                                      */
    ePhyChanBondState cbState;

    /*                */
    tSirMacAddr bssid;

    /*                                */
    tANI_U8 notifyBss;

    /*                                                                       
                                                     */
    tANI_U8 frameType;

    /*                                                                     
                                                                           
                                                                               
                */
    tANI_U8 frameLength;
    
    /*                                                                     
                  */    
    tSirMacMgmtHdr macMgmtHdr;

    /*                                        */
    tSirScanEntry scanEntry;

} tFinishScanParams, *tpFinishScanParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tFinishScanParams finishScanParams;
}  tHalFinishScanReqMsg, *tpHalFinishScanReqMsg;

/*                                                                           
                          
                                                                           */

typedef PACKED_PRE struct PACKED_POST
{
  /*                   */
  tANI_U32   status;

}tHalFinishScanRspParams, *tpHalFinishScanRspParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalFinishScanRspParams finishScanRspParams;
}  tHalFinishScanRspMsg, *tpHalFinishScanRspMsg;

/*                                                                           
                         
                                                                           */

typedef PACKED_PRE struct PACKED_POST {
    /*
                                                    
                                                              
                 
                                                         
                                                                                                              
            
                                                                                        
                                              
                                                                                            
    */

    tStaRateMode        opRateMode;
    //                                                                             
    tANI_U16             llbRates[SIR_NUM_11B_RATES];
    tANI_U16             llaRates[SIR_NUM_11A_RATES];
    tANI_U16             aniLegacyRates[SIR_NUM_POLARIS_RATES];
    tANI_U16             reserved;

    //                                                                          
    //                                                    
    //                                                                    
    tANI_U32             aniEnhancedRateBitmap; //                      

    /*
                                        
                                     
    */
    tANI_U8 supportedMCSSet[SIR_MAC_MAX_SUPPORTED_MCS_SET];

    /*
                                                              
                                                                
                                                                  
                                 
     */
    tANI_U16 rxHighestDataRate;

} tSirSupportedRates, *tpSirSupportedRates;

typedef PACKED_PRE struct PACKED_POST
{
    /*            */
    tSirMacAddr bssId;

    /*                             */
    tANI_U16 assocId;

    /*                                                                */
    tANI_U8 staType;

    /*                         */
    tANI_U8 shortPreambleSupported;

    /*                  */
    tSirMacAddr staMac;

    /*                          */
    tANI_U16 listenInterval;

    /*                   */
    tANI_U8 wmmEnabled;

    /*                  */
    tANI_U8 htCapable;

    /*                                            */
    tANI_U8 txChannelWidthSet;

    /*                              */
    tANI_U8 rifsMode;

    /*                                
                                   
                                 */
    tANI_U8 lsigTxopProtection;

    /*                                                 
                                         */
    tANI_U8 maxAmpduSize;

    /*                                                                */
    tANI_U8 maxAmpduDensity;

    /*                                             */
    tANI_U8 maxAmsduSize;

    /*                                  */
    tANI_U8 fShortGI40Mhz;

    /*                                  */
    tANI_U8 fShortGI20Mhz;

    /*                                              */
    tANI_U8 rmfEnabled;

    /*                                                */
    tANI_U32 encryptType;

    /*                                                                   
                                                                            
                                 */
    tANI_U8 action;

    /*                                             
                                
                               */
    tANI_U8 uAPSD;

    /*             */
    tANI_U8 maxSPLen;

    /*                                
                                       */
    tANI_U8 greenFieldCapable;

    /*                    */
    tSirMacHTMIMOPowerSaveState mimoPS;

    /*                  */
    tANI_U8 delayedBASupport;

    /*                          */
    tANI_U8 us32MaxAmpduDuration;

    /*                                                                    
                                                                           
                                                              */
    tANI_U8 fDsssCckMode40Mhz;

    /*                                                            
                                                               */
    tANI_U8 staIdx;

    /*                                                                       
                                                               */
    tANI_U8 bssIdx;

    tANI_U8  p2pCapableSta;

    /*                                                */
    tANI_U8  reserved;

    /*                                                               */
    tSirSupportedRates supportedRates;

} tConfigStaParams, *tpConfigStaParams;

/*                                                                        
                          
                                                                         */

typedef PACKED_PRE struct PACKED_POST {
    /*
                                                    
                                                              
                 
                                                         
                                                                                                              
            
                                                                                        
                                              
                                                                                            
    */

    tStaRateMode        opRateMode;
    //                                                                             
    tANI_U16             llbRates[SIR_NUM_11B_RATES];
    tANI_U16             llaRates[SIR_NUM_11A_RATES];
    tANI_U16             aniLegacyRates[SIR_NUM_POLARIS_RATES];
    tANI_U16             reserved;

    //                                                                          
    //                                                    
    //                                                                    
    tANI_U32             aniEnhancedRateBitmap; //                      

    /*
                                        
                                     
    */
    tANI_U8 supportedMCSSet[SIR_MAC_MAX_SUPPORTED_MCS_SET];

    /*
                                                              
                                                                
                                                                  
                                 
     */
    tANI_U16 rxHighestDataRate;

    /*                                                               
                             */
    tANI_U16 vhtRxMCSMap;

    /*                                                                  */
    tANI_U16 vhtRxHighestDataRate;

    /*                                                                   
                              */
    tANI_U16 vhtTxMCSMap;

    /*                                                                   */
    tANI_U16 vhtTxHighestDataRate;

} tSirSupportedRates_V1, *tpSirSupportedRates_V1;

typedef PACKED_PRE struct PACKED_POST
{
    /*            */
    tSirMacAddr bssId;

    /*                             */
    tANI_U16 assocId;

    /*                                                                */
    tANI_U8 staType;

    /*                         */
    tANI_U8 shortPreambleSupported;

    /*                  */
    tSirMacAddr staMac;

    /*                          */
    tANI_U16 listenInterval;

    /*                   */
    tANI_U8 wmmEnabled;

    /*                  */
    tANI_U8 htCapable;

    /*                                            */
    tANI_U8 txChannelWidthSet;

    /*                              */
    tANI_U8 rifsMode;

    /*                               
                                   
                                 */
    tANI_U8 lsigTxopProtection;

    /*                                                 
                                         */
    tANI_U8 maxAmpduSize;

    /*                                                                */
    tANI_U8 maxAmpduDensity;

    /*                                             */
    tANI_U8 maxAmsduSize;

    /*                                  */
    tANI_U8 fShortGI40Mhz;

    /*                                  */
    tANI_U8 fShortGI20Mhz;

    /*                                              */
    tANI_U8 rmfEnabled;

    /*                                                */
    tANI_U32 encryptType;
    
    /*                                                                    
                                                                            
                                 */
    tANI_U8 action;

    /*                                             
                                
                               */
    tANI_U8 uAPSD;

    /*             */
    tANI_U8 maxSPLen;

    /*                                
                                       */
    tANI_U8 greenFieldCapable;

    /*                    */
    tSirMacHTMIMOPowerSaveState mimoPS;

    /*                  */
    tANI_U8 delayedBASupport;
    
    /*                          */
    tANI_U8 us32MaxAmpduDuration;
    
    /*                                                                    
                                                                           
                                                              */
    tANI_U8 fDsssCckMode40Mhz;

    /*                                                            
                                                               */
    tANI_U8 staIdx;

    /*                                                                       
                                                               */
    tANI_U8 bssIdx;

    tANI_U8  p2pCapableSta;

    /*                                                */
    tANI_U8 htLdpcEnabled:1;
    tANI_U8 vhtLdpcEnabled:1;
    tANI_U8 vhtTxBFEnabled:1;
    tANI_U8 reserved:5;

        /*                                                               */
    tSirSupportedRates_V1 supportedRates;

    tANI_U8  vhtCapable;
    tANI_U8  vhtTxChannelWidthSet;

} tConfigStaParams_V1, *tpConfigStaParams_V1;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   PACKED_PRE union PACKED_POST {
   tConfigStaParams configStaParams;
    tConfigStaParams_V1 configStaParams_V1;
   } uStaParams;
}  tConfigStaReqMsg, *tpConfigStaReqMsg;

/*                                                                           
                         
                                                                           */

typedef PACKED_PRE struct PACKED_POST
{
  /*                   */
  tANI_U32   status;

  /*                                                             */
  tANI_U8 staIdx;

  /*                                                       */
  tANI_U8 bssIdx;

  /*                   */
  tANI_U8 dpuIndex;

  /*                   */  
  tANI_U8 bcastDpuIndex;

  /*                    */
  tANI_U8 bcastMgmtDpuIdx;

  /*                 */
  tANI_U8 ucUcastSig;

  /*                  */
  tANI_U8 ucBcastSig;

  /*                   */
  tANI_U8 ucMgmtSig;

  tANI_U8  p2pCapableSta;

}tConfigStaRspParams, *tpConfigStaRspParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tConfigStaRspParams configStaRspParams;
}tConfigStaRspMsg, *tpConfigStaRspMsg;

/*                                                                           
                         
                                                                           */

/*                           */
typedef PACKED_PRE struct PACKED_POST 
{
   /*                        */
   tANI_U8    staIdx;
} tDeleteStaParams, *tpDeleteStaParams;

/*                           */
typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tDeleteStaParams delStaParams;
}  tDeleteStaReqMsg, *tpDeleteStaReqMsg;

/*                                                                           
                         
                                                                           */

/*                            */
typedef PACKED_PRE struct PACKED_POST 
{
   /*                   */
   tANI_U32   status;

   /*                      */
   tANI_U8    staId;
} tDeleteStaRspParams, *tpDeleteStaRspParams;

/*                            */
typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tDeleteStaRspParams delStaRspParams;
}  tDeleteStaRspMsg, *tpDeleteStaRspMsg;

/*                                                                           
                         
                                                                           */

//                                                                  
//                                                                   
typedef __ani_attr_pre_packed struct sSirMacRateSet
{
    tANI_U8  numRates;
    tANI_U8  rate[SIR_MAC_RATESET_EID_MAX];
} __ani_attr_packed tSirMacRateSet;

//                       
typedef __ani_attr_pre_packed struct sSirMacAciAifsn
{
#ifndef ANI_LITTLE_BIT_ENDIAN
    tANI_U8  rsvd  : 1;
    tANI_U8  aci   : 2;
    tANI_U8  acm   : 1;
    tANI_U8  aifsn : 4;
#else
    tANI_U8  aifsn : 4;
    tANI_U8  acm   : 1;
    tANI_U8  aci   : 2;
    tANI_U8  rsvd  : 1;
#endif
} __ani_attr_packed tSirMacAciAifsn;

//                       
typedef __ani_attr_pre_packed struct sSirMacCW
{
#ifndef ANI_LITTLE_BIT_ENDIAN
    tANI_U8  max : 4;
    tANI_U8  min : 4;
#else
    tANI_U8  min : 4;
    tANI_U8  max : 4;
#endif
} __ani_attr_packed tSirMacCW;

typedef __ani_attr_pre_packed struct sSirMacEdcaParamRecord
{
    tSirMacAciAifsn  aci;
    tSirMacCW        cw;
    tANI_U16         txoplimit;
} __ani_attr_packed tSirMacEdcaParamRecord;

typedef __ani_attr_pre_packed struct sSirMacSSid
{
    tANI_U8        length;
    tANI_U8        ssId[32];
} __ani_attr_packed tSirMacSSid;

//                                                                         
//                        
typedef enum {
    HAL_STA_MODE=0, 
    HAL_STA_SAP_MODE=1, //                                                                     
    HAL_P2P_CLIENT_MODE,
    HAL_P2P_GO_MODE,
    HAL_MONITOR_MODE,
} tHalConMode;

//                                             
//                
//               
//                       
//                   
typedef enum
{
    HAL_STA=1, 
    HAL_SAP=2,
    HAL_STA_SAP=3, //                                                     
    HAL_P2P_CLIENT=4,
    HAL_P2P_GO=8,
    HAL_MAX_CONCURRENCY_PERSONA=4
} tHalConcurrencyMode;

//                                            
typedef enum
{
    HAL_IFACE_UNKNOWN,
    HAL_IFACE_STA_MODE,
    HAL_IFACE_P2P_MODE,
    HAL_IFACE_MAX
} tHalIfacePersona;

typedef PACKED_PRE struct PACKED_POST
{
    /*       */
    tSirMacAddr bssId;

    /*                  */
    tSirMacAddr  selfMacAddr;

    /*          */
    tSirBssType bssType;

    /*                                */
    tANI_U8 operMode;

    /*            */
    tSirNwType nwType;

    /*                                                  */
    tANI_U8 shortSlotTimeSupported;

    /*                     */
    tANI_U8 llaCoexist;

    /*                     */
    tANI_U8 llbCoexist;

    /*                     */
    tANI_U8 llgCoexist;

    /*                        */
    tANI_U8 ht20Coexist;

    /*                   */
    tANI_U8 llnNonGFCoexist;

    /*                       */
    tANI_U8 fLsigTXOPProtectionFullSupport;

    /*         */
    tANI_U8 fRIFSMode;

    /*                     */
    tSirMacBeaconInterval beaconInterval;

    /*           */
    tANI_U8 dtimPeriod;

    /*                                            */
    tANI_U8 txChannelWidthSet;

    /*                 */
    tANI_U8 currentOperChannel;

    /*                                     */
    tANI_U8 currentExtChannel;

    /*                                                */
    tANI_U8 reserved;

    /*               */
    tSirMacSSid ssId;

    /*                                                              
                                                                            
                                                                */
    tANI_U8 action;

    /*              */
    tSirMacRateSet rateSet;

    /*                                         */
    tANI_U8 htCapable;

    //                               
    tANI_U8 obssProtEnabled;

    /*                    */
    tANI_U8 rmfEnabled;

    /*                                                   */
    tSirMacHTOperatingMode htOperMode;

    /*                                         */
    tANI_U8 dualCTSProtection;

    /*                            */
    tANI_U8   ucMaxProbeRespRetryLimit;

    /*                       */
    tANI_U8   bHiddenSSIDEn;

    /*                                       */
    tANI_U8   bProxyProbeRespEn;

    /*                                                                        
                                                                              
                                                   */
    tANI_U8   edcaParamsValid;

    /*                                               */
    tSirMacEdcaParamRecord acbe;

    /*                                             */
    tSirMacEdcaParamRecord acbk;

    /*                                         */
    tSirMacEdcaParamRecord acvi;

    /*                                         */
    tSirMacEdcaParamRecord acvo;

#ifdef WLAN_FEATURE_VOWIFI_11R
    tANI_U8 extSetStaKeyParamValid; //                         
    tSetStaKeyParams extSetStaKeyParam;  //                               
#endif

    /*                                                                       */
    tANI_U8   halPersona;

    tANI_U8 bSpectrumMgtEnable;

    /*                                                             */
    tANI_S8     txMgmtPower;
    /*                                                                               */
    tANI_S8     maxTxPower;
    /*                                        
                                    
                                                    
                                                                           */
    tConfigStaParams staContext;
} tConfigBssParams, * tpConfigBssParams;


/*                                                                          
                          
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
    /*       */
    tSirMacAddr bssId;

    /*                  */
    tSirMacAddr  selfMacAddr;

    /*          */
    tSirBssType bssType;

    /*                                */
    tANI_U8 operMode;

    /*            */
    tSirNwType nwType;

    /*                                                  */
    tANI_U8 shortSlotTimeSupported;

    /*                     */
    tANI_U8 llaCoexist;

    /*                     */
    tANI_U8 llbCoexist;

    /*                     */
    tANI_U8 llgCoexist;

    /*                        */
    tANI_U8 ht20Coexist;

    /*                   */
    tANI_U8 llnNonGFCoexist;

    /*                       */
    tANI_U8 fLsigTXOPProtectionFullSupport;
    /*         */
    tANI_U8 fRIFSMode;

    /*                     */
    tSirMacBeaconInterval beaconInterval;

    /*           */
    tANI_U8 dtimPeriod;

    /*                                            */
    tANI_U8 txChannelWidthSet;

    /*                 */
    tANI_U8 currentOperChannel;

    /*                                     */
    tANI_U8 currentExtChannel;

    /*                                                */
    tANI_U8 reserved;

    /*               */
    tSirMacSSid ssId;

    /*                                                              
                                                                            
                                                                */
    tANI_U8 action;

    /*              */
    tSirMacRateSet rateSet;

    /*                                         */
    tANI_U8 htCapable;

    //                               
    tANI_U8 obssProtEnabled;

    /*                    */
    tANI_U8 rmfEnabled;

    /*                                                   */
    tSirMacHTOperatingMode htOperMode;

    /*                                         */
    tANI_U8 dualCTSProtection;

    /*                            */
    tANI_U8   ucMaxProbeRespRetryLimit;

    /*                       */
    tANI_U8   bHiddenSSIDEn;

    /*                                       */
    tANI_U8   bProxyProbeRespEn;

    /*                                                                         
                                                                               
                                                   */
    tANI_U8   edcaParamsValid;

    /*                                               */
    tSirMacEdcaParamRecord acbe;
    
    /*                                             */
    tSirMacEdcaParamRecord acbk;

    /*                                         */
    tSirMacEdcaParamRecord acvi;

    /*                                         */
    tSirMacEdcaParamRecord acvo;

#ifdef WLAN_FEATURE_VOWIFI_11R
    tANI_U8 extSetStaKeyParamValid; //                         
    tSetStaKeyParams extSetStaKeyParam;  //                               
#endif

    /*                                                                       */    
    tANI_U8   halPersona;
   
    tANI_U8 bSpectrumMgtEnable;

    /*                                                             */
    tANI_S8     txMgmtPower;
    /*                                                                               */
    tANI_S8     maxTxPower;
    /*                                        
                                    
                                                    
                                                                           */
    tConfigStaParams_V1 staContext;
  
    tANI_U8   vhtCapable;
    tANI_U8   vhtTxChannelWidthSet;
} tConfigBssParams_V1, * tpConfigBssParams_V1;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   PACKED_PRE union PACKED_POST {
   tConfigBssParams configBssParams;
    tConfigBssParams_V1 configBssParams_V1;
   }uBssParams;
}  tConfigBssReqMsg, *tpConfigBssReqMsg;

/*                                                                           
                         
                                                                           */

typedef PACKED_PRE struct PACKED_POST
{
    /*                    */
    tANI_U32   status;

    /*                            */
    tANI_U8    bssIdx;

    /*                              */
    tANI_U8    dpuDescIndx;

    /*                   */
    tANI_U8    ucastDpuSignature;

    /*                             */
    tANI_U8    bcastDpuDescIndx;

    /*                   */
    tANI_U8    bcastDpuSignature;

    /*                       */
    tANI_U8    mgmtDpuDescIndx;

    /*                    */
    tANI_U8    mgmtDpuSignature;

    /*                            */
    tANI_U8     bssStaIdx;

    /*                                 */
    tANI_U8     bssSelfStaIdx;

    /*                                                     */
    tANI_U8     bssBcastStaIdx;

    /*                                                           */
    tSirMacAddr   staMac;

    /*                                                              */
    tANI_S8     txMgmtPower;

} tConfigBssRspParams, * tpConfigBssRspParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tConfigBssRspParams configBssRspParams;
}  tConfigBssRspMsg, *tpConfigBssRspMsg;

/*                                                                           
                         
                                                                           */

typedef PACKED_PRE struct PACKED_POST
{
    /*                         */
    tANI_U8 bssIdx;

} tDeleteBssParams, *tpDeleteBssParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tDeleteBssParams deleteBssParams;
}  tDeleteBssReqMsg, *tpDeleteBssReqMsg;

/*                                                                           
                         
                                                                           */

typedef PACKED_PRE struct PACKED_POST
{
    /*                    */
    tANI_U32   status;

    /*                                 */
    tANI_U8 bssIdx;

} tDeleteBssRspParams, *tpDeleteBssRspParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tDeleteBssRspParams deleteBssRspParams;
}  tDeleteBssRspMsg, *tpDeleteBssRspMsg;

/*                                                                           
                   
                                                                           */

typedef PACKED_PRE struct PACKED_POST
{
  /*                                                      */
  tSirMacAddr     bssId; 

  /*                                   */
  tANI_U8         ucChannel;

  /*              */
  tSirMacAddr selfStaMacAddr;
     
  /*                      */
  tANI_U8         ucLocalPowerConstraint;

  /*                         */
  ePhyChanBondState  secondaryChannelOffset;

  /*          */
  tSirLinkState   linkState;

  /*              */
  tANI_S8 maxTxPower;

} tHalJoinReqParams, *tpHalJoinReqParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalJoinReqParams joinReqParams;
}  tHalJoinReqMsg, *tpHalJoinReqMsg;

/*                                                                           
                   
                                                                           */

typedef PACKED_PRE struct PACKED_POST
{
  /*                   */
  tANI_U32   status;

  /*                                                              */
  tPowerdBm txMgmtPower;

}tHalJoinRspParams, *tpHalJoinRspParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalJoinRspParams joinRspParams;
}tHalJoinRspMsg, *tpHalJoinRspMsg;

/*                                                                           
                         
                                                                           */

typedef PACKED_PRE struct PACKED_POST
{
   tConfigStaParams configStaParams;
   tConfigBssParams configBssParams;
} tPostAssocReqParams, *tpPostAssocReqParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tPostAssocReqParams postAssocReqParams;
}  tPostAssocReqMsg, *tpPostAssocReqMsg;

/*                                                                           
                         
                                                                           */

typedef PACKED_PRE struct PACKED_POST
{
   tConfigStaRspParams configStaRspParams;
   tConfigBssRspParams configBssRspParams;
} tPostAssocRspParams, *tpPostAssocRspParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tPostAssocRspParams postAssocRspParams;
}  tPostAssocRspMsg, *tpPostAssocRspMsg;

/*                                                                           
                         
                                                                           */

/*
                                                                  
 */
typedef PACKED_PRE struct PACKED_POST
{
    /*                    */
    tANI_U8         bssIdx;

    /*                              */
    tAniEdType      encType;

    /*              */
    tANI_U8         numKeys;

    /*              */
    tSirKeys        key[SIR_MAC_MAX_NUM_OF_DEFAULT_KEYS];
  
    /*                                                                
                                         */
    tANI_U8         singleTidRc;
} tSetBssKeyParams, *tpSetBssKeyParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tSetBssKeyParams setBssKeyParams;
} tSetBssKeyReqMsg, *tpSetBssKeyReqMsg;

/*                               */
typedef PACKED_PRE struct PACKED_POST
{
   tSetBssKeyReqMsg  Msg;
   uint32            Tag;
} tSetBssKeyReqMsgTagged;

/*                                                                           
                         
                                                                           */
typedef PACKED_PRE struct PACKED_POST
{
  /*                   */
  tANI_U32   status;

} tSetBssKeyRspParams, *tpSetBssKeyRspParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tSetBssKeyRspParams setBssKeyRspParams;
}  tSetBssKeyRspMsg, *tpSetBssKeyRspMsg;

/*                                                                           
                           
                                                                           */

/*
                                                                          
                                                                       
                                                                        
                                                          
 */

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tSetStaKeyParams setStaKeyParams;
} tSetStaKeyReqMsg, *tpSetStaKeyReqMsg;

/*                                                                           
                           
                                                                           */
typedef PACKED_PRE struct PACKED_POST
{
  /*                   */
  tANI_U32   status;

} tSetStaKeyRspParams, *tpSetStaKeyRspParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tSetStaKeyRspParams setStaKeyRspParams;
} tSetStaKeyRspMsg, *tpSetStaKeyRspMsg;

/*                                                                           
                           
                                                                           */
/*
                                                     
 */
typedef PACKED_PRE struct PACKED_POST

{
    /*                    */
    tANI_U8         bssIdx;
    
    /*                              */
    tAniEdType      encType;

    /*      */
    tANI_U8         keyId;

    /*                                                                             */
    tAniWepType    wepType;

} tRemoveBssKeyParams, *tpRemoveBssKeyParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tRemoveBssKeyParams removeBssKeyParams;
}  tRemoveBssKeyReqMsg, *tpRemoveBssKeyReqMsg;

/*                                                                           
                           
                                                                           */
typedef PACKED_PRE struct PACKED_POST
{
  /*                   */
  tANI_U32   status;

} tRemoveBssKeyRspParams, *tpRemoveBssKeyRspParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tRemoveBssKeyRspParams removeBssKeyRspParams;
}  tRemoveBssKeyRspMsg, *tpRemoveBssKeyRspMsg;

/*                                                                           
                           
                                                                           */
/*
                                                                       
 */
typedef PACKED_PRE struct PACKED_POST
{
    /*         */
    tANI_U16         staIdx;

    /*                              */
    tAniEdType      encType;

    /*      */
    tANI_U8           keyId;

    /*                                                                       
                                                          */
    tANI_BOOLEAN    unicast;

} tRemoveStaKeyParams, *tpRemoveStaKeyParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tRemoveStaKeyParams removeStaKeyParams;
}  tRemoveStaKeyReqMsg, *tpRemoveStaKeyReqMsg;

/*                                                                           
                           
                                                                           */
typedef PACKED_PRE struct PACKED_POST
{
  /*                   */
  tANI_U32   status;
} tRemoveStaKeyRspParams, *tpRemoveStaKeyRspParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tRemoveStaKeyRspParams removeStaKeyRspParams;
}  tRemoveStaKeyRspMsg, *tpRemoveStaKeyRspMsg;

#ifdef FEATURE_OEM_DATA_SUPPORT

#ifndef OEM_DATA_REQ_SIZE
#define OEM_DATA_REQ_SIZE 134
#endif

#ifndef OEM_DATA_RSP_SIZE
#define OEM_DATA_RSP_SIZE 1968
#endif

/*                                                                         
                           
                                                                          */
typedef PACKED_PRE struct PACKED_POST
{
    tANI_U32                 status;
    tSirMacAddr              selfMacAddr;
    tANI_U8                 oemDataReq[OEM_DATA_REQ_SIZE];
} tStartOemDataReqParams, *tpStartOemDataReqParams;

typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader                header;
    tStartOemDataReqParams  startOemDataReqParams;
} tStartOemDataReqMsg, *tpStartOemDataReqMsg;

/*                                                                         
                           
                                                                          */

typedef PACKED_PRE struct PACKED_POST
{
   tANI_U8                   oemDataRsp[OEM_DATA_RSP_SIZE];
} tStartOemDataRspParams, *tpStartOemDataRspParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader             header;
   tStartOemDataRspParams    startOemDataRspParams;
} tStartOemDataRspMsg, *tpStartOemDataRspMsg;

#endif



/*                                                                           
                      
                                                                           */

typedef PACKED_PRE struct PACKED_POST
{
    /*                */
    tANI_U8 channelNumber;

    /*                        */
    tANI_U8 localPowerConstraint;

    /*                         */
    ePhyChanBondState secondaryChannelOffset;

    //                                                             
    tPowerdBm txMgmtPower;

    /*              */
    tPowerdBm maxTxPower;
    
    /*              */
    tSirMacAddr selfStaMacAddr;

    /*                                                                                        
                                                  */
    /*                                                                                   
                                                        */
    /*                                                                                            
                                                                                                           
     */
    tSirMacAddr bssId;
   
}tSwitchChannelParams, *tpSwitchChannelParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tSwitchChannelParams switchChannelParams;
}  tSwitchChannelReqMsg, *tpSwitchChannelReqMsg;

/*                                                                           
                      
                                                                           */

typedef PACKED_PRE struct PACKED_POST
{
    /*        */
    tANI_U32 status;

    /*                                    */
    tANI_U8 channelNumber;

    /*                                                              */
    tPowerdBm txMgmtPower;

    /*                                                      */
    tSirMacAddr bssId;
    
}tSwitchChannelRspParams, *tpSwitchChannelRspParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tSwitchChannelRspParams switchChannelRspParams;
}  tSwitchChannelRspMsg, *tpSwitchChannelRspMsg;

/*                                                                           
                            
                                                                           */

typedef PACKED_PRE struct PACKED_POST
{
   /*         */
   tANI_U16 bssIdx;

   /*             */
   tSirMacEdcaParamRecord acbe; 

   /*            */
   tSirMacEdcaParamRecord acbk;
   
   /*       */
   tSirMacEdcaParamRecord acvi;

   /*       */
   tSirMacEdcaParamRecord acvo;

} tEdcaParams, *tpEdcaParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tEdcaParams edcaParams;
}  tUpdateEdcaParamsReqMsg, *tpUpdateEdcaParamsReqMsg;

/*                                                                           
                            
                                                                           */
typedef PACKED_PRE struct PACKED_POST
{
  /*                   */
  tANI_U32   status;
} tEdcaRspParams, *tpEdcaRspParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tEdcaRspParams edcaRspParams;
}  tUpdateEdcaParamsRspMsg, *tpUpdateEdcaParamsRspMsg;



/*                                                                           
                         
                                                                            */
typedef PACKED_PRE struct PACKED_POST

{
    /*                                      */
    tANI_U16 staIdx;

    /*                 */
    tANI_U8 encMode;
    
    /*        */
    tANI_U32  status;
    
    /*            */
    tANI_U32  sendBlocks;
    tANI_U32  recvBlocks;
    tANI_U32  replays;
    tANI_U8   micErrorCnt;
    tANI_U32  protExclCnt;
    tANI_U16  formatErrCnt;
    tANI_U16  unDecryptableCnt;
    tANI_U32  decryptErrCnt;
    tANI_U32  decryptOkCnt;
} tDpuStatsParams, * tpDpuStatsParams;

typedef PACKED_PRE struct PACKED_POST
{
   /*                                         */
   tANI_U32    staId;

   /*                                                            */
   tANI_U32    statsMask;
}tHalStatsReqParams, *tpHalStatsReqParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader        header;
   tHalStatsReqParams   statsReqParams;
} tHalStatsReqMsg, *tpHalStatsReqMsg;

/*                                                                           
                         
                                                                            */

typedef PACKED_PRE struct PACKED_POST
{
    tANI_U32 retry_cnt[4];          //                                                                               
    tANI_U32 multiple_retry_cnt[4]; //                                                                  
                                    //                                                                            

    tANI_U32 tx_frm_cnt[4];         //                                                                   
                                    //                                                                
    tANI_U32 rx_frm_cnt;            //                                                        
                                    //                                                                    
    tANI_U32 frm_dup_cnt;           //                                                      
    tANI_U32 fail_cnt[4];           //                                               
    tANI_U32 rts_fail_cnt;          //                                                                      
    tANI_U32 ack_fail_cnt;          //                                                                             
    tANI_U32 rts_succ_cnt;          //                                                                      
    tANI_U32 rx_discard_cnt;        //                                                                           
                                    //                                                                       
    tANI_U32 rx_error_cnt;          //                                                                           
    tANI_U32 tx_byte_cnt;           //                                                                           
                                    //                                                                                
                                    //                
}tAniSummaryStatsInfo, *tpAniSummaryStatsInfo;


//                      
typedef enum eTxRateInfo
{
   eHAL_TX_RATE_LEGACY = 0x1,    /*              */
   eHAL_TX_RATE_HT20   = 0x2,    /*            */
   eHAL_TX_RATE_HT40   = 0x4,    /*            */
   eHAL_TX_RATE_SGI    = 0x8,    /*                                */
   eHAL_TX_RATE_LGI    = 0x10,   /*                               */
   eHAL_TX_RATE_VHT20  = 0x20,   /*              */
   eHAL_TX_RATE_VHT40  = 0x40,   /*              */
   eHAL_TX_RATE_VHT80  = 0x80,   /*              */
   eHAL_TX_RATE_VIRT   = 0x100,  /*              */
   eHAL_TX_RATE_MAX    = WLAN_HAL_MAX_ENUM_SIZE
} tTxrateinfoflags, tTxRateInfoFlags;


typedef PACKED_PRE struct PACKED_POST
{
    tANI_U32 rx_frag_cnt;              //                                                                          
                                       //               
    tANI_U32 promiscuous_rx_frag_cnt;  //                                                                          
                                       //                                                            
    tANI_U32 rx_input_sensitivity;     //                                                                           
                                       //                                                                                   
                                       //                                                                                   
    tANI_U32 max_pwr;                  //                                                    
                                       //                                                 
    tANI_U32 sync_fail_cnt;            //                                                                            
                                       //                                                                                     

    tANI_U32 tx_rate;                  //                                                             
                                       //                              
    tANI_U32  mcs_index;               //                                 
    tANI_U32  tx_rate_flags;           //                                  
                                       //                                            
}tAniGlobalClassAStatsInfo, *tpAniGlobalClassAStatsInfo;

typedef PACKED_PRE struct PACKED_POST
{
    tANI_U32 rx_wep_unencrypted_frm_cnt;  //                                                                                 
                                          //                                                                                 
                                          //          
    tANI_U32 rx_mic_fail_cnt;             //                                                                           
                                          //                       
    tANI_U32 tkip_icv_err;                //                                                                              
                                          //                           
    tANI_U32 aes_ccmp_format_err;         //                                                                           
                                          //                       
    tANI_U32 aes_ccmp_replay_cnt;         //                                                                                
                                          //                                        
    tANI_U32 aes_ccmp_decrpt_err;         //                                                                                
                                          //                                                    
    tANI_U32 wep_undecryptable_cnt;       //                                                                                
                                          //                                   
    tANI_U32 wep_icv_err;                 //                                                                              
                                          //                          
    tANI_U32 rx_decrypt_succ_cnt;         //                                                                              
                                          //         
    tANI_U32 rx_decrypt_fail_cnt;         //                                                                         

}tAniGlobalSecurityStats, *tpAniGlobalSecurityStats;
   
typedef PACKED_PRE struct PACKED_POST
{
    tAniGlobalSecurityStats ucStats;
    tAniGlobalSecurityStats mcbcStats;
}tAniGlobalClassBStatsInfo, *tpAniGlobalClassBStatsInfo;

typedef PACKED_PRE struct PACKED_POST
{
    tANI_U32 rx_amsdu_cnt;             //                                                                                
                                       //                                                                                  
                                       //               
    tANI_U32 rx_ampdu_cnt;             //                                                                             
    tANI_U32 tx_20_frm_cnt;            //                                                                          
                                       //               
    tANI_U32 rx_20_frm_cnt;            //                                                                                      
    tANI_U32 rx_mpdu_in_ampdu_cnt;     //                                                                                
                                       //                          
    tANI_U32 ampdu_delimiter_crc_err;  //                                                                                   
                                       //                                                                                     
                                       //                 
}tAniGlobalClassCStatsInfo, *tpAniGlobalClassCStatsInfo;

typedef PACKED_PRE struct PACKED_POST
{
    tANI_U32 tx_frag_cnt[4];        //                                                                               
                                    //                                   
    tANI_U32 tx_ampdu_cnt;          //                                                                
    tANI_U32 tx_mpdu_in_ampdu_cnt;  //                                                                                
                                    //              
}tAniPerStaStatsInfo, *tpAniPerStaStatsInfo;

typedef PACKED_PRE struct PACKED_POST
{
   /*                    */
   tANI_U32 status;

   /*         */
   tANI_U32 staId;

   /*                                                        */
   tANI_U32 statsMask;

   /*                                          */
   tANI_U16 msgType;

   /*                                                                 */
   tANI_U16 msgLen;  

} tHalStatsRspParams, *tpHalStatsRspParams;



typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader  header;
   tHalStatsRspParams statsRspParams;
} tHalStatsRspMsg, *tpHalStatsRspMsg;

/*                                                                           
                           
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
    tSirMacAddr bssid;
    tSirLinkState state;
    tSirMacAddr selfMacAddr;
} tLinkStateParams, *tpLinkStateParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tLinkStateParams linkStateParams;
}  tSetLinkStateReqMsg, *tpSetLinkStateReqMsg;

/*                                                                           
                           
                                                                            */

typedef PACKED_PRE struct PACKED_POST
{
  /*                   */
  tANI_U32   status;
} tLinkStateRspParams, *tpLinkStateRspParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tLinkStateRspParams linkStateRspParams;
}  tSetLinkStateRspMsg, *tpSetLinkStateRspMsg;

/*                                                                           
                      
                                                                            */

/*              */
typedef __ani_attr_pre_packed struct sSirMacTSInfoTfc
{
    tANI_U16       trafficType : 1;
    tANI_U16       tsid : 4;
    tANI_U16       direction : 2;
    tANI_U16       accessPolicy : 2;
    tANI_U16       aggregation : 1;
    tANI_U16       psb : 1;
    tANI_U16       userPrio : 3;
    tANI_U16       ackPolicy : 2;
} __ani_attr_packed tSirMacTSInfoTfc;

/*                                   */
typedef __ani_attr_pre_packed struct sSirMacTSInfoSch
{
    tANI_U8        schedule : 1;
    tANI_U8        rsvd : 7;
} __ani_attr_packed tSirMacTSInfoSch;

/*                             */
typedef __ani_attr_pre_packed struct sSirMacTSInfo
{
    tSirMacTSInfoTfc traffic;
    tSirMacTSInfoSch schedule;
} __ani_attr_packed tSirMacTSInfo;

/*                      */
typedef __ani_attr_pre_packed struct sSirMacTspecIE
{
    tANI_U8             type;
    tANI_U8             length;
    tSirMacTSInfo       tsinfo;
    tANI_U16            nomMsduSz;
    tANI_U16            maxMsduSz;
    tANI_U32            minSvcInterval;
    tANI_U32            maxSvcInterval;
    tANI_U32            inactInterval;
    tANI_U32            suspendInterval;
    tANI_U32            svcStartTime;
    tANI_U32            minDataRate;
    tANI_U32            meanDataRate;
    tANI_U32            peakDataRate;
    tANI_U32            maxBurstSz;
    tANI_U32            delayBound;
    tANI_U32            minPhyRate;
    tANI_U16            surplusBw;
    tANI_U16            mediumTime;
}__ani_attr_packed tSirMacTspecIE;

typedef PACKED_PRE struct PACKED_POST
{
    /*               */
    tANI_U16 staIdx;

    /*                                                               */
    tANI_U16 tspecIdx;

    /*                                         */
    tSirMacTspecIE   tspec;

    /*                                              
                              
                             */
    tANI_U8 uAPSD;

    /*                                                    */
    tANI_U32 srvInterval[WLAN_HAL_MAX_AC];   //                 
    tANI_U32 susInterval[WLAN_HAL_MAX_AC];   //                 
    tANI_U32 delayInterval[WLAN_HAL_MAX_AC]; //               
           
} tAddTsParams, *tpAddTsParams;

typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tAddTsParams  addTsParams;
}  tAddTsReqMsg, *tpAddTsReqMsg;

/*                                                                           
                      
                                                                            */

typedef PACKED_PRE struct PACKED_POST
{
    /*                   */
    tANI_U32   status;
} tAddTsRspParams, *tpAddTsRspParams;

typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tAddTsRspParams addTsRspParams;
}  tAddTsRspMsg, *tpAddTsRspMsg;


/*                                                                           
                      
                                                                            */

typedef PACKED_PRE struct PACKED_POST
{
    /*               */
    tANI_U16 staIdx;

    /*                                                                  */
    tANI_U16 tspecIdx;

    /*                                            */
    tSirMacAddr bssId; 

} tDelTsParams, *tpDelTsParams;

typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tDelTsParams  delTsParams; 
}  tDelTsReqMsg, *tpDelTsReqMsg;

/*                                                                           
                      
                                                                            */

typedef PACKED_PRE struct PACKED_POST
{
    /*                   */
    tANI_U32   status;
} tDelTsRspParams, *tpDelTsRspParams;

typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tDelTsRspParams delTsRspParams;
}  tDelTsRspMsg, *tpDelTsRspMsg;

/*                         */

/*                                       */

/*                                                                           
                              
                                                                            */

typedef PACKED_PRE struct PACKED_POST
{
    /*               */
    tANI_U16 staIdx;

    /*                  */
    tSirMacAddr peerMacAddr;

    /*                                
                                          */
    tANI_U8 baDialogToken;

    /*                                    
                                                */
    tANI_U8 baTID;

    /*                               
                        */
    tANI_U8 baPolicy;

    /*                                                     
                                                          
                                                         
                                                         
                                                          
                                                      */
    tANI_U16 baBufferSize;

    /*                                                  */
    tANI_U16 baTimeout;

    /*                                           
                                                       
                                  */
    tANI_U16 baSSN;

    /*                
                     
                     */
    tANI_U8 baDirection;
} tAddBASessionParams, *tpAddBASessionParams;

typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tAddBASessionParams  addBASessionParams; 
}tAddBASessionReqMsg, *tpAddBASessionReqMsg;

/*                                                                           
                              
                                                                            */

typedef PACKED_PRE struct PACKED_POST
{
    /*                   */
    tANI_U32   status;

    /*              */
    tANI_U8 baDialogToken;

    /*                                             */
    tANI_U8 baTID;

    /*                                                     */
    tANI_U8 baBufferSize;

    tANI_U8 baSessionID;

    /*                          */
    tANI_U8 winSize;
    
    /*                            */
    tANI_U8 STAID;
    
    /*                          */
    tANI_U16 SSN;
} tAddBASessionRspParams, *tpAddBASessionRspParams;

typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tAddBASessionRspParams addBASessionRspParams;
}  tAddBASessionRspMsg, *tpAddBASessionRspMsg;

/*                                                                           
                      
                                                                            */

typedef PACKED_PRE struct PACKED_POST
{
    /*            */
    tANI_U8 baSessionID;

    /*                     */
    tANI_U8 winSize;

#ifdef FEATURE_ON_CHIP_REORDERING
    tANI_BOOLEAN isReorderingDoneOnChip;
#endif
} tAddBAParams, *tpAddBAParams;

typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tAddBAParams  addBAParams; 
}  tAddBAReqMsg, *tpAddBAReqMsg;


/*                                                                           
                      
                                                                            */

typedef PACKED_PRE struct PACKED_POST
{
    /*                   */
    tANI_U32   status;

    /*              */
    tANI_U8 baDialogToken;
 
} tAddBARspParams, *tpAddBARspParams;

typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tAddBARspParams addBARspParams;
}  tAddBARspMsg, *tpAddBARspMsg;


/*                                                                           
                          
                                                                            */


typedef struct sAddBaInfo
{
    tANI_U16 fBaEnable : 1;
    tANI_U16 startingSeqNum: 12;
    tANI_U16 reserved : 3;
}tAddBaInfo, *tpAddBaInfo;

typedef struct sTriggerBaRspCandidate
{
    tSirMacAddr staAddr;
    tAddBaInfo baInfo[STACFG_MAX_TC];
}tTriggerBaRspCandidate, *tpTriggerBaRspCandidate;

typedef struct sTriggerBaCandidate
{
    tANI_U8  staIdx;
    tANI_U8 tidBitmap;
}tTriggerBaReqCandidate, *tptTriggerBaReqCandidate;

typedef PACKED_PRE struct PACKED_POST
{
    /*            */
    tANI_U8 baSessionID;

    /*                                          
                                          
     */
    tANI_U16 baCandidateCnt;
    
} tTriggerBAParams, *tpTriggerBAParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tTriggerBAParams  triggerBAParams; 
}  tTriggerBAReqMsg, *tpTriggerBAReqMsg;


/*                                                                           
                          
                                                                            */

typedef PACKED_PRE struct PACKED_POST
{
   
    /*                   */
    tSirMacAddr  bssId; 

    /*                    */
    tANI_U32   status;

    /*                                          
                                                 
     */
    tANI_U16 baCandidateCnt;
    

} tTriggerBARspParams, *tpTriggerBARspParams;

typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tTriggerBARspParams triggerBARspParams;
}  tTriggerBARspMsg, *tpTriggerBARspMsg;

/*                                                                           
                      
                                                                            */

typedef PACKED_PRE struct PACKED_POST
{
    /*               */
    tANI_U16 staIdx;

    /*                                               */
    tANI_U8 baTID;

    /*                
                     
                     */
    tANI_U8 baDirection;  
} tDelBAParams, *tpDelBAParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tDelBAParams  delBAParams; 
}  tDelBAReqMsg, *tpDelBAReqMsg;

/*                                                                           
                      
                                                                            */

typedef PACKED_PRE struct PACKED_POST
{
    /*                    */
    tANI_U32   status;
} tDelBARspParams, *tpDelBARspParams;

typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tDelBARspParams delBARspParams;
}  tDelBARspMsg, *tpDelBARspMsg;


#ifdef FEATURE_WLAN_CCX

/*                                                                           
                         
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
    /*            */
    tANI_U8 tsmTID;

    tSirMacAddr bssId;
} tTsmStatsParams, *tpTsmStatsParams;

typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tTsmStatsParams  tsmStatsParams; 
}  tTsmStatsReqMsg, *tpTsmStatsReqMsg;


/*                                                                           
                         
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
    /*                   */
    tANI_U32   status;

    /*                           */ 
    tANI_U16      UplinkPktQueueDly;

    /*                                     */ 
    tANI_U16      UplinkPktQueueDlyHist[4];

    /*                              */ 
    tANI_U32      UplinkPktTxDly;

    /*                    */ 
    tANI_U16      UplinkPktLoss;

    /*                     */ 
    tANI_U16      UplinkPktCount;

    /*               */ 
    tANI_U8       RoamingCount;

    /*               */ 
    tANI_U16      RoamingDly;
} tTsmStatsRspParams, *tpTsmStatsRspParams;

typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tTsmStatsRspParams tsmStatsRspParams;
}  tTsmStatsRspMsg, *tpTsmStatsRspMsg;


#endif

/*                                                                           
                           
                                                                            */

typedef PACKED_PRE struct PACKED_POST
{
  /*                  */
  tANI_U8   bssidx;
  tANI_U8   encType;
} tSetKeyDoneParams, *tpSetKeyDoneParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tSetKeyDoneParams setKeyDoneParams;
}  tSetKeyDoneMsg, *tpSetKeyDoneMsg;

/*                                                                           
                           
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
    /*                                                                       
                                                                                
                                                                                  
                                                                           
                                                                                 
                                                                         */
    tANI_U16 fragNumber;

    /*                                                                             
                                                                                    
                                                                             
                                                                */
    tANI_U16 isLastFragment;

    /*                                 */
    tANI_U32 nvImgBufferSize;

    /*                                                                     
                                                       */
} tHalNvImgDownloadReqParams, *tpHalNvImgDownloadReqParams;

typedef PACKED_PRE struct PACKED_POST
{
    /*                                                                         
                                                                         */
    tHalMsgHeader header;
    tHalNvImgDownloadReqParams nvImageReqParams;
} tHalNvImgDownloadReqMsg, *tpHalNvImgDownloadReqMsg;

/*                                                                           
                           
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
    /*                                                                  
                           */
    tANI_U32   status;
} tHalNvImgDownloadRspParams, *tpHalNvImgDownloadRspParams;

typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tHalNvImgDownloadRspParams nvImageRspParams;
}  tHalNvImgDownloadRspMsg, *tpHalNvImgDownloadRspMsg;

/*                                                                           
                        
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
    /*         */
    eNvTable tableID;

    /*                 */
    tANI_U32 nvBlobSize;

    /*                                                                 
                                      */
} tHalNvStoreParams, *tpHalNvStoreParams;

typedef PACKED_PRE struct PACKED_POST
{
    /*                                                                
                                                           */
    tHalMsgHeader header;
    tHalNvStoreParams nvStoreParams;
}  tHalNvStoreInd, *tpHalNvStoreInd;

/*                                     */

/*                                                                           
                           
                                                                            */

#define SIR_CIPHER_SEQ_CTR_SIZE 6

typedef PACKED_PRE struct PACKED_POST
{
    tSirMacAddr  srcMacAddr;     //                            
    tSirMacAddr  taMacAddr;      //                   
    tSirMacAddr  dstMacAddr;
    tANI_U8      multicast;             
    tANI_U8      IV1;            //                 
    tANI_U8      keyId;          //                  
    tANI_U8      TSC[SIR_CIPHER_SEQ_CTR_SIZE]; //                
    tSirMacAddr  rxMacAddr;      //                
} tSirMicFailureInfo, *tpSirMicFailureInfo;

/*                                      
                                                                     
 */
typedef PACKED_PRE struct PACKED_POST
{
    tSirMacAddr         bssId;   //      
    tSirMicFailureInfo  info;
} tSirMicFailureInd, *tpSirMicFailureInd;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tSirMicFailureInd micFailureInd;
}  tMicFailureIndMsg, *tpMicFailureIndMsg;

typedef PACKED_PRE struct PACKED_POST
{
   tANI_U16  opMode;
   tANI_U16  staId;
}tUpdateVHTOpMode, *tpUpdateVHTOpMode; 

typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tUpdateVHTOpMode updateVhtOpMode;
}  tUpdateVhtOpModeReqMsg, *tpUpdateVhtOpModeReqMsg;

typedef PACKED_PRE struct PACKED_POST
{
    tANI_U32   status;
} tUpdateVhtOpModeParamsRsp, *tpUpdateVhtOpModeParamsRsp;

typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tUpdateVhtOpModeParamsRsp updateVhtOpModeRspParam;
}  tUpdateVhtOpModeParamsRspMsg,  *tpUpdateVhtOpModeParamsRspMsg;

/*                                                                           
                             
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{

    tANI_U8  bssIdx;

    //                                                               
    //                     
    tANI_U8 fShortPreamble;
    //                
    tANI_U8 fShortSlotTime;
    //               
    tANI_U16 beaconInterval;
    //                  
    tANI_U8 llaCoexist;
    tANI_U8 llbCoexist;
    tANI_U8 llgCoexist;
    tANI_U8 ht20MhzCoexist;
    tANI_U8 llnNonGFCoexist;
    tANI_U8 fLsigTXOPProtectionFullSupport;
    tANI_U8 fRIFSMode;

    tANI_U16 paramChangeBitmap;
}tUpdateBeaconParams, *tpUpdateBeaconParams;


typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tUpdateBeaconParams updateBeaconParam;
}  tUpdateBeaconReqMsg, *tpUpdateBeaconReqMsg;

/*                                                                           
                             
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
    tANI_U32   status;
} tUpdateBeaconRspParams, *tpUpdateBeaconRspParams;

typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tUpdateBeaconRspParams updateBeaconRspParam;
}  tUpdateBeaconRspMsg, *tpUpdateBeaconRspMsg;

/*                                                                           
                           
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
    tANI_U32 beaconLength; //                       
    tANI_U8 beacon[BEACON_TEMPLATE_SIZE];     //             
    tSirMacAddr bssId;
    tANI_U32 timIeOffset; //                                                 
    tANI_U16 p2pIeOffset; //                                               
}tSendBeaconParams, *tpSendBeaconParams;


typedef PACKED_PRE struct PACKED_POST
{
  tHalMsgHeader header;
  tSendBeaconParams sendBeaconParam;
}tSendBeaconReqMsg, *tpSendBeaconReqMsg;

/*                                                                           
                           
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
    tANI_U32   status;
} tSendBeaconRspParams, *tpSendBeaconRspParams;

typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tSendBeaconRspParams sendBeaconRspParam;
}  tSendBeaconRspMsg, *tpSendBeaconRspMsg;

#ifdef FEATURE_5GHZ_BAND

/*                                                                           
                                   
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
    tSirMacAddr BSSID;
    tANI_U8   channel; 
}tSirEnableRadarInfoType, *tptSirEnableRadarInfoType;


typedef PACKED_PRE struct PACKED_POST
{
    /*                 */
    tSirEnableRadarInfoType EnableRadarInfo;
}tEnableRadarReqParams, *tpEnableRadarReqParams;

typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tEnableRadarReqParams  enableRadarReqParams; 
}tEnableRadarReqMsg, *tpEnableRadarReqMsg;

/*                                                                           
                                   
                                                                            */

typedef PACKED_PRE struct PACKED_POST
{
    /*                 */
    tSirMacAddr BSSID;
    /*                    */
    tANI_U32   status;
}tEnableRadarRspParams, *tpEnableRadarRspParams;

typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tEnableRadarRspParams  enableRadarRspParams; 
}tEnableRadarRspMsg, *tpEnableRadarRspMsg;

/*                                                                           
                                
                                                                            */

typedef PACKED_PRE struct PACKED_POST
{
    tANI_U8 radarDetChannel;
}tRadarDetectIntrIndParams, *tpRadarDetectIntrIndParams;

typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tRadarDetectIntrIndParams  radarDetectIntrIndParams; 
}tRadarDetectIntrIndMsg, *tptRadarDetectIntrIndMsg;

/*                                                                           
                           
                                                                           */
typedef PACKED_PRE struct PACKED_POST
{
    /*                                          */
    tANI_U8          channelNumber;

    /*                 */
    tANI_U16         radarPulseWidth; //           

    /*                       */
    tANI_U16         numRadarPulse;
}tRadarDetectIndParams,*tpRadarDetectIndParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tRadarDetectIndParams  radarDetectIndParams; 
}tRadarDetectIndMsg, *tptRadarDetectIndMsg;


/*                                                                           
                             
                                                                           */
typedef PACKED_PRE struct PACKED_POST
{
   tSirMacAddr sta;
   tANI_U8     dialogToken;
   tANI_U8     txpower;
}tSirGetTpcReportReqParams, *tpSirGetTpcReportReqParams;


typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tSirGetTpcReportReqParams  getTpcReportReqParams; 
}tSirGetTpcReportReqMsg, *tpSirGetTpcReportReqMsg;

/*                                                                           
                              
                                                                            */

typedef PACKED_PRE struct PACKED_POST
{
    /*                    */
    tANI_U32   status;
}tSirGetTpcReportRspParams, *tpSirGetTpcReportRspParams;

typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tSirGetTpcReportRspParams  getTpcReportRspParams; 
}tSirGetTpcReportRspMsg, *tpSirGetTpcReportRspMsg;

#endif

/*                                                                           
                                        
                                                                           */
typedef PACKED_PRE struct PACKED_POST
{
    tANI_U8      pProbeRespTemplate[BEACON_TEMPLATE_SIZE];
    tANI_U32     probeRespTemplateLen;
    tANI_U32     ucProxyProbeReqValidIEBmap[8];
    tSirMacAddr  bssId;

}tSendProbeRespReqParams, *tpSendProbeRespReqParams;

typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tSendProbeRespReqParams sendProbeRespReqParams ;
}tSendProbeRespReqMsg, *tpSendProbeRespReqMsg;

/*                                                                           
                                         
                                                                            */

typedef PACKED_PRE struct PACKED_POST
{
    /*                    */
    tANI_U32   status;
}tSendProbeRespRspParams, *tpSendProbeRespRspParams;

typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tSendProbeRespRspParams sendProbeRespRspParams;
}tSendProbeRespRspMsg, *tpSendProbeRespRspMsg;


/*                                                                           
                                      
                                                                            */

typedef PACKED_PRE struct PACKED_POST
{
    /*                    */
    tANI_U32   status;
}tSendUnkownFrameRxIndParams, *tpSendUnkownFrameRxIndParams;

typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tSendUnkownFrameRxIndParams sendUnkownFrameRxIndParams;
}tSendUnkownFrameRxIndMsg, *tpSendUnkownFrameRxIndMsg;

/*                                                                           
                                 
                                                                            */

typedef PACKED_PRE struct PACKED_POST
{
    tANI_U16    assocId;
    tANI_U16    staId;
    tSirMacAddr bssId; //                  
                       //                                     
    tSirMacAddr addr2;        //
    tANI_U16    reasonCode;   //                                                                                                                             

}tDeleteStaContextParams, *tpDeleteStaContextParams;


typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tDeleteStaContextParams deleteStaContextParams;
}tDeleteStaContextIndMsg, *tpDeleteStaContextIndMsg;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tANI_U8  assocId;
   tANI_U8  staIdx;
   tANI_U8  bssIdx;
   tANI_U8  uReasonCode;
   tANI_U32  uStatus;
} tIndicateDelSta, *tpIndicateDelSta;

/*                                                                           
                                 
                                                                            */

typedef PACKED_PRE struct PACKED_POST
{
    tBtAmpEventType btAmpEventType;

}tBtAmpEventParams, *tpBtAmpEventParams;



typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tBtAmpEventParams btAmpEventParams;
}tBtAmpEventMsg, *tpBtAmpEventMsg;

/*                                                                           
                                
                                                                           */

typedef PACKED_PRE struct PACKED_POST
{
    /*                    */
    tANI_U32   status;
}tBtAmpEventRspParams, *tpBtAmpEventRspParams;

typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tBtAmpEventRspParams btAmpEventRspParams;
}tBtAmpEventRsp, *tpBtAmpEventRsp;


/*                                                                           
                              
                                                                            */

typedef PACKED_PRE struct PACKED_POST
{
   //                                   
    tANI_U8  ucSTAId;

    //                                                  
    tANI_U8   ucTid;

}tTlHalFlushAcParams, *tpTlHalFlushAcParams;


typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tTlHalFlushAcParams tlHalFlushAcParam;
}tTlHalFlushAcReq, *tpTlHalFlushAcReq;

/*                                                                           
                             
                                                                           */

typedef PACKED_PRE struct PACKED_POST
{
    //                                   
    tANI_U8  ucSTAId;

    //                                                  
    tANI_U8   ucTid;

    /*                    */
    tANI_U32   status;
}tTlHalFlushAcRspParams, *tpTlHalFlushAcRspParams;

typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tTlHalFlushAcRspParams tlHalFlushAcRspParam;
}tTlHalFlushAcRspMsg, *tpTlHalFlushAcRspMsg;

/*                                                                           
                          
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
}  tHalEnterImpsReqMsg, *tpHalEnterImpsReqMsg;

/*                                                                           
                         
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
}  tHalExitImpsReqMsg, *tpHalExitImpsReqMsg;

/*                                                                           
                          
                                                                            */

typedef PACKED_PRE struct PACKED_POST
{
   tANI_U8         bssIdx;
   //                                       
#ifndef BUILD_QWPTTSTATIC
   tANI_U64 tbtt;
#endif
   tANI_U8 dtimCount;
   //                                                             
   //                                                      
   tANI_U8 dtimPeriod;

   //                        
   tANI_U32 rssiFilterPeriod;
   tANI_U32 numBeaconPerRssiAverage;
   tANI_U8  bRssiFilterEnable;

} tHalEnterBmpsReqParams, *tpHalEnterBmpsReqParams;


typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalEnterBmpsReqParams enterBmpsReq;
}  tHalEnterBmpsReqMsg, *tpHalEnterBmpsReqMsg;

/*                                                                           
                         
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
   tANI_U8     sendDataNull;
   tANI_U8     bssIdx;
} tHalExitBmpsReqParams, *tpHalExitBmpsReqParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalExitBmpsReqParams exitBmpsReqParams;
}  tHalExitBmpsReqMsg, *tpHalExitBmpsReqMsg;

/*                                                                           
                             
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
   tANI_U8     bssIdx;
} tHalMissedBeaconIndParams, *tpHalMissedBeaconIndParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalMissedBeaconIndParams missedBeaconIndParams;
}  tHalMissedBeaconIndMsg, *tpHalMissedBeaconIndMsg;

/*                                                                           
                              
                                                                            */
/*                                  */
typedef PACKED_PRE struct PACKED_POST
{
    tANI_U8     offset;
    tANI_U8     value;
    tANI_U8     bitMask;
    tANI_U8     ref;
} tEidByteInfo, *tpEidByteInfo;

typedef PACKED_PRE struct PACKED_POST 
{
    tANI_U16    capabilityInfo;
    tANI_U16    capabilityMask;
    tANI_U16    beaconInterval;
    tANI_U16    ieNum;
    tANI_U8     bssIdx;
    tANI_U8     reserved;
} tBeaconFilterMsg, *tpBeaconFilterMsg;

/*                                                                                */
typedef PACKED_PRE struct PACKED_POST
{
    tANI_U8         elementId;
    tANI_U8         checkIePresence;
    tEidByteInfo    byte;
} tBeaconFilterIe, *tpBeaconFilterIe;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tBeaconFilterMsg addBcnFilterParams;
}  tHalAddBcnFilterReqMsg, *tpHalAddBcnFilterReqMsg;

/*                                                                           
                              
                                                                            */
typedef PACKED_PRE struct PACKED_POST 
{
    tANI_U8  ucIeCount;
    tANI_U8  ucRemIeId[1];
} tRemBeaconFilterMsg, *tpRemBeaconFilterMsg;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tRemBeaconFilterMsg remBcnFilterParams;
}  tHalRemBcnFilterReqMsg, *tpHalRemBcnFilterReqMsg;

/*                                                                           
                            
                                                                            */
#define HAL_IPV4_ARP_REPLY_OFFLOAD                  0
#define HAL_IPV6_NEIGHBOR_DISCOVERY_OFFLOAD         1
#define HAL_IPV6_NS_OFFLOAD                         2
#define HAL_IPV6_ADDR_LEN                           16
#define HAL_MAC_ADDR_LEN                            6
#define HAL_OFFLOAD_DISABLE                         0
#define HAL_OFFLOAD_ENABLE                          1
#define HAL_OFFLOAD_BCAST_FILTER_ENABLE             0x2
#define HAL_OFFLOAD_MCAST_FILTER_ENABLE             0x4
#define HAL_OFFLOAD_ARP_AND_BCAST_FILTER_ENABLE     (HAL_OFFLOAD_ENABLE|HAL_OFFLOAD_BCAST_FILTER_ENABLE)
#define HAL_OFFLOAD_IPV6NS_AND_MCAST_FILTER_ENABLE  (HAL_OFFLOAD_ENABLE|HAL_OFFLOAD_MCAST_FILTER_ENABLE)

typedef PACKED_PRE struct PACKED_POST _tHalNSOffloadParams
{
   tANI_U8 srcIPv6Addr[HAL_IPV6_ADDR_LEN];
   tANI_U8 selfIPv6Addr[HAL_IPV6_ADDR_LEN];
   //                                                          
   tANI_U8 targetIPv6Addr1[HAL_IPV6_ADDR_LEN];
   tANI_U8 targetIPv6Addr2[HAL_IPV6_ADDR_LEN];
   tANI_U8 selfMacAddr[HAL_MAC_ADDR_LEN];
   tANI_U8 srcIPv6AddrValid : 1;
   tANI_U8 targetIPv6Addr1Valid : 1;
   tANI_U8 targetIPv6Addr2Valid : 1;
   tANI_U8 reserved1 : 5;
   tANI_U8 reserved2;   //                     
   tANI_U8 bssIdx;
   tANI_U32 slotIndex; //                            
} tHalNSOffloadParams;

typedef PACKED_PRE struct PACKED_POST
{
    tANI_U8 offloadType;
    tANI_U8 enableOrDisable;
    PACKED_PRE union PACKED_POST
    {
        tANI_U8 hostIpv4Addr [4];
        tANI_U8 hostIpv6Addr [HAL_IPV6_ADDR_LEN];
    } params;
} tHalHostOffloadReq, *tpHalHostOffloadReq;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalHostOffloadReq hostOffloadParams;
   tHalNSOffloadParams nsOffloadParams;
}  tHalHostOffloadReqMsg, *tpHalHostOffloadReqMsg;


#ifdef FEATURE_WLAN_LPHB
typedef enum
{
   WIFI_HB_SET_ENABLE         = 0x0001,
   WIFI_HB_SET_TCP_PARAMS     = 0x0002,
   WIFI_HB_SET_TCP_PKT_FILTER = 0x0003,
   WIFI_HB_SET_UDP_PARAMS     = 0x0004,
   WIFI_HB_SET_UDP_PKT_FILTER = 0x0005,
   WIFI_HB_SET_NETWORK_INFO   = 0x0006,
}tLowPowerHeartBeatCmdType ;

#define MAX_FLITER_SIZE 64
/*                                                                           
                       
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
   uint32 hostIpv4Addr;
   uint32 destIpv4Addr;
   uint16 hostPort;
   uint16 destPort;
   uint16 timeOutSec;  //           
   tSirMacAddr gatewayMacAddr;
   uint16 timePeriodSec; //           
   uint32 tcpSn;
} tlowPowerHeartBeatParamsTcpStruct;

typedef PACKED_PRE struct PACKED_POST
{
   uint32 hostIpv4Addr;
   uint32 destIpv4Addr;
   uint16 hostPort;
   uint16 destPort;
   uint16 timePeriodSec;//           
   uint16 timeOutSec;   //           
   tSirMacAddr gatewayMacAddr;
} tlowPowerHeartBeatParamsUdpStruct;

typedef PACKED_PRE struct PACKED_POST
{
   uint32 offset;
   uint32 filterLength;
   uint8  filter[MAX_FLITER_SIZE];
} tlowPowerHeartBeatFilterStruct;

typedef PACKED_PRE struct PACKED_POST
{
   uint8 heartBeatEnable;
   uint8 heartBeatType; //          
} tlowPowerHeartBeatEnableStruct;

typedef PACKED_PRE struct PACKED_POST
{
  uint8 dummy;
} tlowPowerHeartBeatNetworkInfoStruct;


typedef PACKED_PRE struct PACKED_POST
{
   uint8 sessionIdx;
   uint16 lowPowerHeartBeatCmdType;
   PACKED_PRE union PACKED_PRO
   {
      tlowPowerHeartBeatEnableStruct control;
      tlowPowerHeartBeatFilterStruct tcpUdpFilter;
      tlowPowerHeartBeatParamsTcpStruct tcpParams;
      tlowPowerHeartBeatParamsUdpStruct udpParams;
      tlowPowerHeartBeatNetworkInfoStruct info;
    }options;
} tHalLowPowerHeartBeatReq, *tpHalLowPowerHeartBeatReq;


typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalLowPowerHeartBeatReq lowPowerHeartBeatParams;
}  tHalLowPowerHeartBeatReqMsg, *tpHalLowPowerHeartBeatReqMsg;

/*                                                                           
                        
                                                                            */

typedef PACKED_PRE struct PACKED_POST
{
   /*                    */
   uint8  sessionIdx;
   uint32 status;
   uint16 lowPowerHeartBeatCmdType;
}tHalLowPowerHeartBeatRspParams, *tpHalLowPowerHeartBeatRspParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalLowPowerHeartBeatRspParams lowPowerHeartBeatRspParams;
}tHalLowPowerHeartBeatRspMsg, *tpHalLowPowerHeartBeatRspMsg;


/*                                                                           
                        
                                                                            */
#define WIFI_HB_EVENT_TCP_RX_TIMEOUT 0x0001
#define WIFI_HB_EVENT_UDP_RX_TIMEOUT 0x0002

#define WIFI_LPHB_EVENT_REASON_TIMEOUT 0x01
#define WIFI_LPHB_EVENT_REASON_FW_ON_MONITOR 0x02
#define WIFI_LPHB_EVENT_REASON_FW_OFF_MONITOR 0x03


#define WIFI_LPHB_PROTO_UDP 0x01
#define WIFI_LPHB_PROTO_TCP 0x02

typedef PACKED_PRE struct PACKED_POST
{
   uint8 bssIdx;
   uint8 sessionIdx;
   uint8 protocolType; /*          */
   uint8 eventReason;

}tHalLowPowerHeartBeatIndParam,*tpHalLowPowerHeartBeatIndParam;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalLowPowerHeartBeatIndParam lowPowerHeartBeatIndParams;
}tHalLowPowerHeartBeatIndMsg, *tpHalLowPowerHeartBeatIndMsg;

#endif

#ifdef FEATURE_WLAN_BATCH_SCAN

/*                                                                           
                             
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
    /*                                   */
    tANI_U32   scanInterval;
    tANI_U32   numScan2Batch;
    tANI_U32   bestNetworks;
    tANI_U8    rfBand;
    tANI_U8    rtt;
} tHalBatchScanSetParams, *tpHalBatchScanSetParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalBatchScanSetParams batchScanParams;
}  tHalBatchScanSetReqMsg, *tpHalBatchScanSetReqMsg;

/*                                                                           
                             
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
   tANI_U32 supportedMscan;
}  tHalBatchScanSetRspParam, *tpHalBatchScanSetRspParam;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalBatchScanSetRspParam setBatchScanRspParam;
}  tHalBatchScanSetRspMsg, *tpHalBatchScanSetRspMsg;

/*                                                                           
                             
                                                                           */
typedef PACKED_PRE struct PACKED_POST
{
    tANI_U32 param;
} tHalBatchScanStopIndParam, *tpHalBatchScanStopIndParam;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader  header;
   tHalBatchScanStopIndParam param;
} tHalBatchScanStopIndMsg, *tpHalBatchScanStopIndMsg;

/*                                                                           
                                       
                                                                           */
typedef PACKED_PRE struct PACKED_POST
{
    tANI_U32 param;
} tHalBatchScanTriggerResultParam, *tpHalBatchScanTriggerResultParam;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader  header;
   tHalBatchScanTriggerResultParam param;
} tHalBatchScanTriggerResultIndMsg, *tpHalBatchScanTriggerResultIndMsg;

/*                                                                           
                             
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
    tANI_U8   bssid[6];     /*       */
    tANI_U8   ssid[33];     /*      */
    tANI_U8   ch;           /*         */
    tANI_U8   rssi;         /*               */
    /*                                                                                                  */
    tANI_U32  timestamp;
} tHalBatchScanNetworkInfo, *tpHalBatchScanNetworkInfo;

typedef PACKED_PRE struct PACKED_POST
{
    tANI_U32   scanId;                         /*               */
    /*                                                                         */
    tANI_U32   numNetworksInScanList;
    /*                                              */
    /*                                                                        
                                                                  */
    tANI_U8    scanList[1];
} tHalBatchScanList, *tpHalBatchScanList;

typedef PACKED_PRE struct PACKED_POST
{
    tANI_U32      timestamp;
    tANI_U32      numScanLists;
    boolean       isLastResult;
    /*                                        */
    /*                                                         
                                                  */
    tANI_U8       scanResults[1];
}  tHalBatchScanResultIndParam, *tpHalBatchScanResultIndParam;

typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tHalBatchScanResultIndParam resultIndMsgParam;
}  tHalBatchScanResultIndMsg, *tpHalBatchScanResultIndMsg;

#endif

/*                                                                           
                          
                                                                            */
/*               */
#define HAL_KEEP_ALIVE_NULL_PKT              1
#define HAL_KEEP_ALIVE_UNSOLICIT_ARP_RSP     2

/*                              */
#define HAL_KEEP_ALIVE_DISABLE   0
#define HAL_KEEP_ALIVE_ENABLE    1

/*                     */
typedef PACKED_PRE struct PACKED_POST
{
    tANI_U8          packetType;
    tANI_U32         timePeriod;
    tHalIpv4Addr     hostIpv4Addr; 
    tHalIpv4Addr     destIpv4Addr;
    tSirMacAddr      destMacAddr;
    tANI_U8          bssIdx;
} tHalKeepAliveReq, *tpHalKeepAliveReq;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalKeepAliveReq KeepAliveParams;
}  tHalKeepAliveReqMsg, *tpHalKeepAliveReqMsg;

/*                                                                           
                               
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
    tANI_S8   ucRssiThreshold1     : 8;
    tANI_S8   ucRssiThreshold2     : 8;
    tANI_S8   ucRssiThreshold3     : 8;
    tANI_U8   bRssiThres1PosNotify : 1;
    tANI_U8   bRssiThres1NegNotify : 1;
    tANI_U8   bRssiThres2PosNotify : 1;
    tANI_U8   bRssiThres2NegNotify : 1;
    tANI_U8   bRssiThres3PosNotify : 1;
    tANI_U8   bRssiThres3NegNotify : 1;
    tANI_U8   bReserved10          : 2;
} tHalRSSIThresholds, *tpHalRSSIThresholds;
    
typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalRSSIThresholds rssiThreshParams;
}  tHalRSSIThresholdReqMsg, *tpHalRSSIThresholdReqMsg;

/*                                                                           
                           
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
    tANI_U8     bkDeliveryEnabled:1;
    tANI_U8     beDeliveryEnabled:1;
    tANI_U8     viDeliveryEnabled:1;
    tANI_U8     voDeliveryEnabled:1;
    tANI_U8     bkTriggerEnabled:1;
    tANI_U8     beTriggerEnabled:1;
    tANI_U8     viTriggerEnabled:1;
    tANI_U8     voTriggerEnabled:1;
    tANI_U8     bssIdx;
} tUapsdReqParams, *tpUapsdReqParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tUapsdReqParams enterUapsdParams;
}  tHalEnterUapsdReqMsg, *tpHalEnterUapsdReqMsg;

/*                                                                           
                          
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tANI_U8       bssIdx;
}  tHalExitUapsdReqMsg, *tpHalExitUapsdReqMsg;

#define HAL_PERIODIC_TX_PTRN_MAX_SIZE 1536
#define HAL_MAXNUM_PERIODIC_TX_PTRNS 6
/*                                                                           
                                    
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
    tANI_U32 selfStaIdx:8;
    tANI_U32 ucPtrnId:8;         //           
    tANI_U32 usPtrnSize:16;      //                      
    tANI_U32 uPtrnIntervalMs;    //        
    tANI_U8  ucPattern[HAL_PERIODIC_TX_PTRN_MAX_SIZE]; //               
} tHalAddPeriodicTxPtrn, *tpHalAddPeriodicTxPtrn;

typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tHalAddPeriodicTxPtrn ptrnParams;
}  tHalAddPeriodicTxPtrnIndMsg, *tpHalAddPeriodicTxPtrnIndMsg;

/*                                                                           
                                    
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
    tANI_U32 selfStaIdx:8;
    tANI_U32 rsvd:24;
    /*                                                */
    tANI_U32 uPatternIdBitmap;
} tHalDelPeriodicTxPtrn, *tpHalDelPeriodicTxPtrn;

typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tHalDelPeriodicTxPtrn ptrnParams;
}   tHalDelPeriodicTxPtrnIndMsg, *tpHalDelPeriodicTxPtrnIndMsg;

/*                                                                           
                                   
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
    /*                            */
    tANI_U32 bssIdx:8;
    tANI_U32 selfStaIdx:8;
    tANI_U32 rsvd:16;
    tANI_U32 status;
    tANI_U32 patternIdBitmap;
} tHalPeriodicTxPtrnFwInd, *tpHalPeriodicTxPtrnFwInd;

typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tHalPeriodicTxPtrnFwInd fwIndParams;
}   tHalPeriodicTxPtrnFwIndMsg, *tpHalPeriodicTxPtrnFwIndMsg;

/*                                                                           
                               
                                                                            */
#define HAL_WOWL_BCAST_PATTERN_MAX_SIZE 128
#define HAL_WOWL_BCAST_MAX_NUM_PATTERNS 16

typedef PACKED_PRE struct PACKED_POST
{
    tANI_U8  ucPatternId;           //           
    //                                                                        
    //                
    tANI_U8  ucPatternByteOffset;   
    tANI_U8  ucPatternSize;         //                      
    tANI_U8  ucPattern[HAL_WOWL_BCAST_PATTERN_MAX_SIZE]; //        
    tANI_U8  ucPatternMaskSize;     //                           
    tANI_U8  ucPatternMask[HAL_WOWL_BCAST_PATTERN_MAX_SIZE]; //             
    tANI_U8  ucPatternExt[HAL_WOWL_BCAST_PATTERN_MAX_SIZE]; //              
    tANI_U8  ucPatternMaskExt[HAL_WOWL_BCAST_PATTERN_MAX_SIZE]; //                   
    tANI_U8  bssIdx;
} tHalWowlAddBcastPtrn, *tpHalWowlAddBcastPtrn;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalWowlAddBcastPtrn ptrnParams;
}  tHalWowlAddBcastPtrnReqMsg, *tpHalWowlAddBcastPtrnReqMsg;
                                


/*                                                                           
                               
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
    /*                                                */
    tANI_U8  ucPatternId;
    tANI_U8  bssIdx;
} tHalWowlDelBcastPtrn, *tpHalWowlDelBcastPtrn;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalWowlDelBcastPtrn ptrnParams;
}  tHalWowlDelBcastPtrnReqMsg, *tpHalWowlDelBcastPtrnReqMsg;

/*                                                                           
                          
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
    /*                                         */
    tANI_U8   ucMagicPktEnable; 

    /*               */
    tSirMacAddr magicPtrn;

    /*                                                        
                                                             
                                                                  
                                                               
               
    */
    tANI_U8   ucPatternFilteringEnable;

    /*                                                    
                                                               
                                                                  
                                                               
                                              
    */
    tANI_U8   ucUcastPatternFilteringEnable;                     

    /*                                                         
                                                            
                                   
     */
    tANI_U8   ucWowChnlSwitchRcv;

    /*                                                         
                                                            
                               
     */
    tANI_U8   ucWowDeauthRcv;

    /*                                                         
                                                            
                             
     */
    tANI_U8   ucWowDisassocRcv;

    /*                                                         
                                                         
                                                       
                                                     
     */
    tANI_U8   ucWowMaxMissedBeacons;

    /*                                                         
                                                                
                                                              
                                                                         
     */
    tANI_U8   ucWowMaxSleepUsec;
    
    /*                                                                       
                                                                       
     */
    tANI_U8   ucWoWEAPIDRequestEnable;

    /*                                                                           
                                              
     */
    tANI_U8   ucWoWEAPOL4WayEnable;

    /*                                                                          
     */
    tANI_U8   ucWowNetScanOffloadMatch;

    /*                                                                   
     */
    tANI_U8   ucWowGTKRekeyError;

    /*                                                                
     */
    tANI_U8   ucWoWBSSConnLoss;

    tANI_U8   bssIdx;

} tHalWowlEnterParams, *tpHalWowlEnterParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalWowlEnterParams enterWowlParams;
}  tHalWowlEnterReqMsg, *tpHalWowlEnterReqMsg;

/*                                                                           
                         
                                                                            */

typedef PACKED_PRE struct PACKED_POST
{
    tANI_U8   bssIdx;

} tHalWowlExitParams, *tpHalWowlExitParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader     header;
   tHalWowlExitParams exitWowlParams;
}  tHalWowlExitReqMsg, *tpHalWowlExitReqMsg;

/*                                                                           
                        
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
}  tHalGetRssiReqMsg, *tpHalGetRssiReqMsg;

typedef PACKED_PRE struct PACKED_POST
{
   /*                                         */
   tANI_U32    staId;

}tHalRoamRssiReqParams, *tpHalRoamRssiReqParams;


/*                                                                           
                             
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalRoamRssiReqParams roamRssiReqParams;
}  tHalGetRoamRssiReqMsg, *tpHalGetRoamRssiReqMsg;


/*                                                                           
                                   
                                                                            */
typedef PACKED_PRE struct PACKED_POST {
    tANI_U8  staidx;        //          
    tANI_U8  ac;            //                
    tANI_U8  up;            //              
    tANI_U32 srvInterval;   //                 
    tANI_U32 susInterval;   //                 
    tANI_U32 delayInterval; //               
} tUapsdInfo, tpUapsdInfo;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tUapsdInfo    enableUapsdAcParams;
}  tHalSetUapsdAcParamsReqMsg, *tpHalSetUapsdAcParamsReqMsg;

/*                                                                           
                                    
                                                                            */
typedef PACKED_PRE struct PACKED_POST {
    tANI_U8 setMcstBcstFilterSetting;
    tANI_U8 setMcstBcstFilter;
} tHalConfigureRxpFilterReqParams, tpHalConfigureRxpFilterReqParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalConfigureRxpFilterReqParams    configureRxpFilterReqParams;
}  tHalConfigureRxpFilterReqMsg, *tpHalConfigureRxpFilterReqMsg;

/*                                                                           
                          
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
    /*                    */
    tANI_U32   status;
} tHalEnterImpsRspParams, *tpHalEnterImpsRspParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalEnterImpsRspParams enterImpsRspParams;
}  tHalEnterImpsRspMsg, *tpHalEnterImpsRspMsg;

/*                                                                           
                         
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
    /*                    */
    tANI_U32   status;
} tHalExitImpsRspParams, *tpHalExitImpsRspParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalExitImpsRspParams exitImpsRspParams;
}  tHalExitImpsRspMsg, *tpHalExitImpsRspMsg;

/*                                                                           
                          
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
    /*                    */
    tANI_U32   status;
    tANI_U8    bssIdx;
} tHalEnterBmpsRspParams, *tpHalEnterBmpsRspParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalEnterBmpsRspParams enterBmpsRspParams;
}  tHalEnterBmpsRspMsg, *tpHalEnterBmpsRspMsg;

/*                                                                           
                         
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
    /*                    */
    tANI_U32   status;
    tANI_U8    bssIdx;
} tHalExitBmpsRspParams, *tpHalExitBmpsRspParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalExitBmpsRspParams exitBmpsRspParams;
}  tHalExitBmpsRspMsg, *tpHalExitBmpsRspMsg;

/*                                                                           
                           
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
    /*                    */
    tANI_U32    status;
    tANI_U8     bssIdx;
}tUapsdRspParams, *tpUapsdRspParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tUapsdRspParams enterUapsdRspParams;
}  tHalEnterUapsdRspMsg, *tpHalEnterUapsdRspMsg;

/*                                                                           
                          
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
    /*                    */
    tANI_U32   status;
    tANI_U8    bssIdx;
} tHalExitUapsdRspParams, *tpHalExitUapsdRspParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalExitUapsdRspParams exitUapsdRspParams;
}  tHalExitUapsdRspMsg, *tpHalExitUapsdRspMsg;

/*                                                                           
                                 
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
    tANI_U32             bRssiThres1PosCross : 1;
    tANI_U32             bRssiThres1NegCross : 1;
    tANI_U32             bRssiThres2PosCross : 1;
    tANI_U32             bRssiThres2NegCross : 1;
    tANI_U32             bRssiThres3PosCross : 1;
    tANI_U32             bRssiThres3NegCross : 1;
    tANI_U32             avgRssi             : 8;
    tANI_U32             uBssIdx             : 8;
    tANI_U32             isBTCoexCompromise  : 1;
    tANI_U32             bReserved           : 9;
} tHalRSSINotification, *tpHalRSSINotification;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalRSSINotification rssiNotificationParams;
}  tHalRSSINotificationIndMsg, *tpHalRSSINotificationIndMsg;

/*                                                                           
                        
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
    /*                    */
    tANI_U32   status;
    tANI_S8    rssi;
} tHalGetRssiParams, *tpHalGetRspParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalGetRssiParams rssiRspParams;
}  tHalGetRssiRspMsg, *tpHalGetRssiRspMsg;

/*                                                                           
                             
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
    /*                    */
    tANI_U32   status;

    tANI_U8    staId;
    tANI_S8    rssi;
} tHalGetRoamRssiParams, *tpHalGetRoamRspParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalGetRoamRssiParams roamRssiRspParams;
}  tHalGetRoamRssiRspMsg, *tpHalGetRoamRssiRspMsg;

/*                                                                           
                          
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
    /*                    */
   tANI_U32   status;
   tANI_U8    bssIdx;
} tHalEnterWowlRspParams, *tpHalEnterWowlRspParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalEnterWowlRspParams enterWowlRspParams;
}  tHalWowlEnterRspMsg, *tpHalWowlEnterRspMsg;

/*                                                                           
                         
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
    /*                    */
   tANI_U32   status;
   tANI_U8    bssIdx;
} tHalExitWowlRspParams, *tpHalExitWowlRspParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalExitWowlRspParams exitWowlRspParams;
}  tHalWowlExitRspMsg, *tpHalWowlExitRspMsg;

/*                                                                           
                              
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
    /*                    */
    tANI_U32   status;
} tHalAddBcnFilterRspParams, *tpHalAddBcnFilterRspParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalAddBcnFilterRspParams addBcnFilterRspParams;
}  tHalAddBcnFilterRspMsg, *tpHalAddBcnFilterRspMsg;

/*                                                                           
                              
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
    /*                    */
    tANI_U32   status;
} tHalRemBcnFilterRspParams, *tpHalRemBcnFilterRspParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalRemBcnFilterRspParams remBcnFilterRspParams;
}  tHalRemBcnFilterRspMsg, *tpHalRemBcnFilterRspMsg;

/*                                                                           
                                   
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
    /*                    */
   tANI_U32   status;
   tANI_U8    bssIdx;
} tHalAddWowlBcastPtrnRspParams, *tpHalAddWowlBcastPtrnRspParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalAddWowlBcastPtrnRspParams addWowlBcastPtrnRspParams;
}  tHalAddWowlBcastPtrnRspMsg, *tpHalAddWowlBcastPtrnRspMsg;

/*                                                                           
                                   
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
    /*                    */
   tANI_U32   status;
   tANI_U8    bssIdx;
} tHalDelWowlBcastPtrnRspParams, *tpHalDelWowlBcastPtrnRspParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalDelWowlBcastPtrnRspParams delWowlBcastRspParams;
}  tHalDelWowlBcastPtrnRspMsg, *tpHalDelWowlBcastPtrnRspMsg;

/*                                                                           
                            
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
    /*                    */
    tANI_U32   status;
} tHalHostOffloadRspParams, *tpHalHostOffloadRspParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalHostOffloadRspParams hostOffloadRspParams;
}  tHalHostOffloadRspMsg, *tpHalHostOffloadRspMsg;

/*                                                                           
                          
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
    /*                    */
    tANI_U32   status;
} tHalKeepAliveRspParams, *tpHalKeepAliveRspParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalKeepAliveRspParams keepAliveRspParams;
}  tHalKeepAliveRspMsg, *tpHalKeepAliveRspMsg;

/*                                                                           
                               
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
    /*                    */
    tANI_U32   status;
} tHalSetRssiThreshRspParams, *tpHalSetRssiThreshRspParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalSetRssiThreshRspParams setRssiThreshRspParams;
}  tHalSetRssiThreshRspMsg, *tpHalSetRssiThreshRspMsg;

/*                                                                           
                                   
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
    /*                    */
    tANI_U32   status;
} tHalSetUapsdAcParamsRspParams, *tpHalSetUapsdAcParamsRspParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalSetUapsdAcParamsRspParams setUapsdAcParamsRspParams;
}  tHalSetUapsdAcParamsRspMsg, *tpHalSetUapsdAcParamsRspMsg;

/*                                                                           
                                    
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
    /*                    */
    tANI_U32   status;
} tHalConfigureRxpFilterRspParams, *tpHalConfigureRxpFilterRspParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalConfigureRxpFilterRspParams configureRxpFilterRspParams;
}  tHalConfigureRxpFilterRspMsg, *tpHalConfigureRxpFilterRspMsg;

/*                                                                           
                               
                                                                            */

typedef PACKED_PRE struct PACKED_POST
{
    tSirMacAddr bssId;  //                                                                  
                        //                                                                              
    tSirMacAddr selfStaMacAddr;
    //           
    //                                
    tPowerdBm  power;

}tSetMaxTxPwrParams, *tpSetMaxTxPwrParams;


typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tSetMaxTxPwrParams setMaxTxPwrParams;
}tSetMaxTxPwrReq, *tpSetMaxTxPwrReq;

/*                                                                           
                              
                                                                           */

typedef PACKED_PRE struct PACKED_POST
{
    //                                             
    tPowerdBm  power;

    /*                    */
    tANI_U32   status;
}tSetMaxTxPwrRspParams, *tpSetMaxTxPwrRspParams;

typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tSetMaxTxPwrRspParams setMaxTxPwrRspParams;
}tSetMaxTxPwrRspMsg, *tpSetMaxTxPwrRspMsg;


/*                                                                           
                                        
                                                                            */

/*                                                                           */
typedef enum
{
   WLAN_HAL_SET_MAX_TX_POWER_BAND_ALL = 0,
   //                         
   WLAN_HAL_SET_MAX_TX_POWER_BAND_2_4_GHZ,
   WLAN_HAL_SET_MAX_TX_POWER_BAND_5_0_GHZ,
   //                   
   WLAN_HAL_SET_MAX_TX_POWER_BAND_MAX = WLAN_HAL_MAX_ENUM_SIZE,
}tHalSetMaxTxPwrBandInfo;

typedef PACKED_PRE struct PACKED_POST
{
    tHalSetMaxTxPwrBandInfo bandInfo;  //                   
    tPowerdBm   power;  //                                             
}tSetMaxTxPwrPerBandParams, *tpSetMaxTxPwrPerBandParams;


typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tSetMaxTxPwrPerBandParams setMaxTxPwrPerBandParams;
}tSetMaxTxPwrPerBandReq, *tpSetMaxTxPwrPerBandReq;

/*                                                                           
                                       
                                                                           */

typedef PACKED_PRE struct PACKED_POST
{
    //                                             
    tPowerdBm  power;

    /*                    */
    tANI_U32   status;
}tSetMaxTxPwrPerBandRspParams, *tpSetMaxTxPwrPerBandRspParams;

typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tSetMaxTxPwrPerBandRspParams setMaxTxPwrPerBandRspParams;
}tSetMaxTxPwrPerBandRspMsg, *tpSetMaxTxPwrPerBandRspMsg;

/*                                                                           
                           
                                                                            */

typedef PACKED_PRE struct PACKED_POST
{
    /*                         */
    tANI_U32  txPower;
    tANI_U8   bssIdx;
}tSetTxPwrReqParams, *tpSetTxPwrReqParams;


typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tSetTxPwrReqParams setTxPwrReqParams;
}tSetTxPwrReqMsg, *tpSetTxPwrReqMsg;

/*                                                                           
                          
                                                                           */

typedef PACKED_PRE struct PACKED_POST
{
    /*                    */
    tANI_U32   status;
}tSetTxPwrRspParams, *tpSetTxPwrRspParams;

typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tSetTxPwrRspParams setTxPwrRspParams;
}tSetTxPwrRspMsg, *tpSetTxPwrRspMsg;

/*                                                                           
                           
                                                                            */

typedef PACKED_PRE struct PACKED_POST
{
    tANI_U8  staId;
}tGetTxPwrReqParams, *tpGetTxPwrReqParams;

typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tGetTxPwrReqParams getTxPwrReqParams;
}tGetTxPwrReqMsg, *tpGetTxPwrReqMsg;

/*                                                                           
                          
                                                                           */

typedef PACKED_PRE struct PACKED_POST
{
    /*                    */
    tANI_U32   status;

    /*                         */
    tANI_U32   txPower;
}tGetTxPwrRspParams, *tpGetTxPwrRspParams;

typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tGetTxPwrRspParams getTxPwrRspParams;
}tGetTxPwrRspMsg, *tpGetTxPwrRspMsg;

#ifdef WLAN_FEATURE_P2P
/*                                                                           
                            
                                                                            */

typedef PACKED_PRE struct PACKED_POST
{
  tANI_U8   opp_ps;
  tANI_U32  ctWindow;
  tANI_U8   count; 
  tANI_U32  duration;
  tANI_U32  interval;
  tANI_U32  single_noa_duration;
  tANI_U8   psSelection;
}tSetP2PGONOAParams, *tpSetP2PGONOAParams;


typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tSetP2PGONOAParams setP2PGONOAParams;
}tSetP2PGONOAReq, *tpSetP2PGONOAReq;

/*                                                                           
                           
                                                                           */

typedef PACKED_PRE struct PACKED_POST
{
    /*                    */
    tANI_U32   status;
}tSetP2PGONOARspParams, *tpSetP2PGONOARspParams;

typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tSetP2PGONOARspParams setP2PGONOARspParams;
}tSetP2PGONOARspMsg, *tpSetP2PGONOARspMsg;
#endif

/*                                                                           
                           
                                                                            */

typedef PACKED_PRE struct PACKED_POST
{
  tSirMacAddr selfMacAddr;
  tANI_U32    status;
}tAddStaSelfParams, *tpAddStaSelfParams;

typedef PACKED_PRE struct PACKED_POST
{
  tSirMacAddr selfMacAddr;
  tANI_U32    status;
  tHalIfacePersona iface_persona;
}tAddStaSelfParams_V1, *tpAddStaSelfParams_V1;

typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    PACKED_PRE union PACKED_POST {
    tAddStaSelfParams addStaSelfParams;
    tAddStaSelfParams_V1 addStaSelfParams_V1;
    }uAddStaSelfParams;
}tAddStaSelfReq, *tpAddStaSelfReq;

/*                                                                           
                          
                                                                           */

typedef PACKED_PRE struct PACKED_POST
{
    /*                    */
    tANI_U32   status;

   /*               */
   tANI_U8    selfStaIdx;

   /*                                     */
   tANI_U8 dpuIdx;

   /*               */
   tANI_U8 dpuSignature;
   
}tAddStaSelfRspParams, *tpAddStaSelfRspParams;

typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tAddStaSelfRspParams addStaSelfRspParams;
}tAddStaSelfRspMsg, *tpAddStaSelfRspMsg;


/*                                                                           
                           
                                                                           */

typedef PACKED_PRE struct PACKED_POST
{
   tSirMacAddr selfMacAddr;

}tDelStaSelfParams, *tpDelStaSelfParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tDelStaSelfParams delStaSelfParams;
}  tDelStaSelfReqMsg, *tpDelStaSelfReqMsg;


/*                                                                           
                           
                                                                           */

typedef PACKED_PRE struct PACKED_POST
{
  /*                   */
  tANI_U32   status;

  tSirMacAddr selfMacAddr;
}tDelStaSelfRspParams, *tpDelStaSelfRspParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tDelStaSelfRspParams delStaSelfRspParams;
}  tDelStaSelfRspMsg, *tpDelStaSelfRspMsg;


#ifdef WLAN_FEATURE_VOWIFI_11R

/*                                                                           
                          
                                                                            */

typedef PACKED_PRE struct PACKED_POST
{
    /*               */
    tANI_U16 staIdx;

    /*                                                               */
    /*                                                                              */
    tANI_U16 tspecIdx;

    /*                                                            */
    tSirMacTspecIE   tspec[WLAN_HAL_MAX_AC];

    /*                                              
                              
                             */
    tANI_U8 uAPSD;

    /*                                                    */
    tANI_U32 srvInterval[WLAN_HAL_MAX_AC];   //                 
    tANI_U32 susInterval[WLAN_HAL_MAX_AC];   //                 
    tANI_U32 delayInterval[WLAN_HAL_MAX_AC]; //               

}tAggrAddTsParams, *tpAggrAddTsParams;


typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tAggrAddTsParams aggrAddTsParam;
}tAggrAddTsReq, *tpAggrAddTsReq;

/*                                                                           
                         
                                                                           */

typedef PACKED_PRE struct PACKED_POST
{
    /*                    */
    tANI_U32   status0;
    /*                                    */
    tANI_U32   status1;
}tAggrAddTsRspParams, *tpAggrAddTsRspParams;

typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tAggrAddTsRspParams aggrAddTsRspParam;
}tAggrAddTsRspMsg, *tpAggrAddTsRspMsg;

#endif

/*                                                                           
                                               
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
   tANI_U8   isAppsCpuAwake;
} tHalConfigureAppsCpuWakeupStateReqParams, *tpHalConfigureAppsCpuWakeupStatReqParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalConfigureAppsCpuWakeupStateReqParams appsStateReqParams;
}  tHalConfigureAppsCpuWakeupStateReqMsg, *tpHalConfigureAppsCpuWakeupStateReqMsg;

/*                                                                           
                                               
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
    /*                    */
    tANI_U32   status;
} tHalConfigureAppsCpuWakeupStateRspParams, *tpHalConfigureAppsCpuWakeupStateRspParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalConfigureAppsCpuWakeupStateRspParams appsStateRspParams;
}  tHalConfigureAppsCpuWakeupStateRspMsg, *tpHalConfigureAppsCpuWakeupStateRspMsg;
/*                                                                           
                            
                                                                            */

typedef PACKED_PRE struct PACKED_POST
{
   tANI_U32    argument1;
   tANI_U32    argument2;
   tANI_U32    argument3;
   tANI_U32    argument4;
   tANI_U32    argument5;

}tHalDumpCmdReqParams,*tpHalDumpCmdReqParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader          header;
   tHalDumpCmdReqParams   dumpCmdReqParams;
} tHalDumpCmdReqMsg, *tpHalDumpCmdReqMsg;

/*                                                                           
                            
                                                                            */

typedef PACKED_PRE struct PACKED_POST
{
    /*                    */
    tANI_U32   status;
    /*                              */
    tANI_U32   rspLength;
    /*                                                                          */
    tANI_U8    rspBuffer[DUMPCMD_RSP_BUFFER];
    
} tHalDumpCmdRspParams, *tpHalDumpCmdRspParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalDumpCmdRspParams dumpCmdRspParams;
}  tHalDumpCmdRspMsg, *tpHalDumpCmdRspMsg;

/*                                                                           
                   
                                                                           */
#define WLAN_COEX_IND_DATA_SIZE (4)
#define WLAN_COEX_IND_TYPE_DISABLE_HB_MONITOR (0)
#define WLAN_COEX_IND_TYPE_ENABLE_HB_MONITOR (1)
#define WLAN_COEX_IND_TYPE_SCANS_ARE_COMPROMISED_BY_COEX (2)
#define WLAN_COEX_IND_TYPE_SCANS_ARE_NOT_COMPROMISED_BY_COEX (3)
#define WLAN_COEX_IND_TYPE_DISABLE_AGGREGATION_IN_2p4 (4)
#define WLAN_COEX_IND_TYPE_ENABLE_AGGREGATION_IN_2p4 (5)

typedef PACKED_PRE struct PACKED_POST
{
    /*                    */
    tANI_U32   coexIndType;

    /*                    */
    tANI_U32   coexIndData[WLAN_COEX_IND_DATA_SIZE];
}tCoexIndParams,*tpCoexIndParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader   header;
   tCoexIndParams  coexIndParams; 
}tCoexIndMsg, *tpCoexIndMsg;

/*                                                                           
                           
                                                                           */

typedef PACKED_PRE struct PACKED_POST
{
   /*                                         */
   tANI_U32   status;
}tTxComplParams,*tpTxComplParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader   header;
   tTxComplParams  txComplParams; 
}tTxComplIndMsg, *tpTxComplIndMsg;

/*                                                                           
                            
                                                                           */

typedef PACKED_PRE struct PACKED_POST
{
    tANI_U32 configuredMcstBcstFilterSetting;
    tANI_U32 activeSessionCount; 
}tHalWlanHostSuspendIndParam,*tpHalWlanHostSuspendIndParam;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalWlanHostSuspendIndParam suspendIndParams;
}tHalWlanHostSuspendIndMsg, *tpHalWlanHostSuspendIndMsg;

/*                                                                           
                                  
                                                                           */

typedef PACKED_PRE struct PACKED_POST
{
    tANI_BOOLEAN bDot11ExcludeUnencrypted;
    tSirMacAddr bssId; 
}tHalWlanExcludeUnEncryptedIndParam,*tpHalWlanExcludeUnEncryptedIndParam;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalWlanExcludeUnEncryptedIndParam excludeUnEncryptedIndParams;
}tHalWlanExcludeUnEncrptedIndMsg, *tpHalWlanExcludeUnEncrptedIndMsg;

#ifdef WLAN_FEATURE_P2P
/*                                                                           
                       
                                                                           */

typedef PACKED_PRE struct PACKED_POST
{
   tANI_U8      index ;
   tANI_U8      oppPsFlag ;
   tANI_U16     ctWin  ;

   tANI_U16      uNoa1IntervalCnt;
   tANI_U16      bssIdx;
   tANI_U32      uNoa1Duration;
   tANI_U32      uNoa1Interval;
   tANI_U32      uNoa1StartTime;

   tANI_U16      uNoa2IntervalCnt;
   tANI_U16      rsvd2;
   tANI_U32      uNoa2Duration;
   tANI_U32      uNoa2Interval;
   tANI_U32      uNoa2StartTime;

   tANI_U32   status;
}tNoaAttrIndParams, *tpNoaAttrIndParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader      header;
   tNoaAttrIndParams  noaAttrIndParams; 
}tNoaAttrIndMsg, *tpNoaAttrIndMsg;

/*                                                                           
                        
                                                                           */

typedef PACKED_PRE struct PACKED_POST
{
    tANI_U32             status;
    tANI_U32             bssIdx;
}tNoaStartIndParams, *tpNoaStartIndParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader      header;
   tNoaStartIndParams noaStartIndParams;
}tNoaStartIndMsg, tpNoaStartIndMsg;
#endif

/*                                                                           
                           
                                                                           */

typedef PACKED_PRE struct PACKED_POST
{
    tANI_U8 configuredMcstBcstFilterSetting;
}tHalWlanHostResumeReqParam,*tpHalWlanHostResumeReqParam;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalWlanHostResumeReqParam resumeReqParams;
}tHalWlanHostResumeReqMsg, *tpHalWlanHostResumeReqMsg;

/*                                                                           
                           
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
    /*                    */
    tANI_U32   status;
} tHalHostResumeRspParams, *tpHalHostResumeRspParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalHostResumeRspParams hostResumeRspParams;
}  tHalHostResumeRspMsg, *tpHalHostResumeRspMsg;

typedef PACKED_PRE struct PACKED_POST
{
	tANI_U16 staIdx;
	//                                                 
	tSirMacAddr peerMacAddr;
	//                                                      
	tANI_U8 baTID;
	//                
	//               
	//              
	tANI_U8 baDirection;
	tANI_U32 reasonCode;
	tSirMacAddr  bssId; //                  
} tHalWlanDelBaIndMsg, *tpHalWlanDelBaIndMsg;

typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader   header;
	tHalWlanDelBaIndMsg hostdelBaParam;
} tHalDelBAIndMsg, *tpHalDelBAIndMsg;

/*                                                                           
              
                                                                           */
/*                                                      */
/*                                                                            
                          
 */
#define WLAN_HAL_PNO_MAX_NETW_CHANNELS  60

/*                                                     */
#define WLAN_HAL_PNO_MAX_NETW_CHANNELS_EX  60

/*                                            */
#define WLAN_HAL_PNO_MAX_SUPP_NETWORKS  16

/*                                                                 */
#define WLAN_HAL_PNO_MAX_SCAN_TIMERS    10

/*                                  */
#define WLAN_HAL_PNO_MAX_PROBE_SIZE     450

/*                     
                                                                    
                            
                                          
                                               
                                                  
  */
typedef enum
{
   ePNO_MODE_IMMEDIATE,
   ePNO_MODE_ON_SUSPEND,
   ePNO_MODE_ON_RESUME,
   ePNO_MODE_DELAY,
   ePNO_MODE_PROXIMITY,  //                       
   ePNO_MODE_MAX = WLAN_HAL_MAX_ENUM_SIZE
} ePNOMode;

/*                   */
typedef enum 
{
    eAUTH_TYPE_ANY                   = 0,    
    eAUTH_TYPE_OPEN_SYSTEM           = 1,
    
    //                                 
    eAUTH_TYPE_WPA                   = 2,
    eAUTH_TYPE_WPA_PSK               = 3,
    
    eAUTH_TYPE_RSN                   = 4,
    eAUTH_TYPE_RSN_PSK               = 5,
    eAUTH_TYPE_FT_RSN                = 6,
    eAUTH_TYPE_FT_RSN_PSK            = 7,
    eAUTH_TYPE_WAPI_WAI_CERTIFICATE  = 8,
    eAUTH_TYPE_WAPI_WAI_PSK          = 9,
    eAUTH_TYPE_CCKM_WPA              = 10,
    eAUTH_TYPE_CCKM_RSN              = 11,
    
    eAUTH_TYPE_MAX = WLAN_HAL_MAX_ENUM_SIZE

}tAuthType;

/*                 */
typedef enum eEdType
{
    eED_ANY           = 0,
    eED_NONE          = 1,
    eED_WEP           = 2,
    eED_TKIP          = 3,
    eED_CCMP          = 4,
    eED_WPI           = 5,
        
    eED_TYPE_MAX = WLAN_HAL_MAX_ENUM_SIZE
} tEdType;

/*                      */
typedef enum eSSIDBcastType
{
  eBCAST_UNKNOWN      = 0,
  eBCAST_NORMAL       = 1,
  eBCAST_HIDDEN       = 2,

  eBCAST_TYPE_MAX     = WLAN_HAL_MAX_ENUM_SIZE
} tSSIDBcastType;

/* 
                                                             
*/
typedef PACKED_PRE struct PACKED_POST
{
  /*               */
  tSirMacSSid ssId;

  /*                                   */
  tAuthType   authentication; 

  /*                               */
  tEdType     encryption; 

  /*                                                       
                        */
  tANI_U8     ucChannelCount;
  tANI_U8     aChannels[WLAN_HAL_PNO_MAX_NETW_CHANNELS];

  /*                                                             */
  tANI_U8     rssiThreshold;
}tNetworkType; 

typedef PACKED_PRE struct PACKED_POST
{
  /*                        */
  tANI_U32    uTimerValue; 

  /*                                                
                                                   */
  tANI_U32    uTimerRepeat; 

  /*           
               
                                                           
                                                                         */
}tScanTimer; 

/* 
                                                        
*/
typedef PACKED_PRE struct PACKED_POST
{
  /*                                                                */
  tANI_U8     ucScanTimersCount; 

  /*                                                                     
                                   
                                                                             
                                            
                                                                            
                                                                     
                                                              */
  tScanTimer  aTimerValues[WLAN_HAL_PNO_MAX_SCAN_TIMERS]; 

}tScanTimersType;

typedef PACKED_PRE struct PACKED_POST {

    /*          */
    tANI_U32          enable;

    /*                                   */
    ePNOMode         modePNO;
    
    /*                               */
    tANI_U32          ucNetworksCount;

    /*                                       */
    tNetworkType     aNetworks[WLAN_HAL_PNO_MAX_SUPP_NETWORKS];

    /*                                */
    tScanTimersType  scanTimers; 

    /*                              */
    tANI_U16         us24GProbeSize; 
    tANI_U8          a24GProbeTemplate[WLAN_HAL_PNO_MAX_PROBE_SIZE];

    /*                            */
    tANI_U16         us5GProbeSize; 
    tANI_U8          a5GProbeTemplate[WLAN_HAL_PNO_MAX_PROBE_SIZE];

} tPrefNetwListParams, * tpPrefNetwListParams;

/*
                                 
*/
typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tPrefNetwListParams   prefNetwListParams;
}  tSetPrefNetwListReq, *tpSetPrefNetwListReq;


/* 
                                                             
*/
typedef PACKED_PRE struct PACKED_POST
{
  /*               */
  tSirMacSSid ssId;

  /*                                   */
  tAuthType   authentication; 

  /*                               */
  tEdType     encryption; 

  /*                                              */
  tSSIDBcastType bcastNetworkType;

  /*                                                       
                        */
  tANI_U8     ucChannelCount;
  tANI_U8     aChannels[WLAN_HAL_PNO_MAX_NETW_CHANNELS];

  /*                                                             */
  tANI_U8     rssiThreshold;
}tNetworkTypeNew; 

typedef PACKED_PRE struct PACKED_POST {

    /*          */
    tANI_U32          enable;

    /*                                   */
    ePNOMode         modePNO;
    
    /*                               */
    tANI_U32         ucNetworksCount; 

    /*                                       */
    tNetworkTypeNew  aNetworks[WLAN_HAL_PNO_MAX_SUPP_NETWORKS];

    /*                                */
    tScanTimersType  scanTimers; 

    /*                              */
    tANI_U16         us24GProbeSize; 
    tANI_U8          a24GProbeTemplate[WLAN_HAL_PNO_MAX_PROBE_SIZE];

    /*                            */
    tANI_U16         us5GProbeSize; 
    tANI_U8          a5GProbeTemplate[WLAN_HAL_PNO_MAX_PROBE_SIZE];

} tPrefNetwListParamsNew, * tpPrefNetwListParamsNew;

/*
                                    
*/
typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tPrefNetwListParamsNew   prefNetwListParams;
}  tSetPrefNetwListReqNew, *tpSetPrefNetwListReqNew;

#ifdef WLAN_FEATURE_ROAM_SCAN_OFFLOAD
typedef PACKED_PRE struct PACKED_POST
{
   tSirMacSSid ssId;
   tANI_U8     currAPbssid[HAL_MAC_ADDR_LEN];
   tANI_U32    authentication;
   tEdType     encryption;
   tEdType     mcencryption;
   tANI_U8     ChannelCount;
   tANI_U8     ChannelCache[WLAN_HAL_ROAM_SCAN_MAX_CHANNELS];
}tRoamNetworkType;

typedef PACKED_PRE struct PACKED_POST
{
   tANI_U8 mdiePresent;
   tANI_U16 mobilityDomain;
}tMobilityDomainInfo;

typedef PACKED_PRE struct PACKED_POST {
   eAniBoolean       RoamScanOffloadEnabled;
   tANI_S8           LookupThreshold;
   tANI_U8           RoamRssiDiff;
   tANI_U8           ChannelCacheType;
   tANI_U8           Command;
   tANI_U8           StartScanReason;
   tANI_U16          NeighborScanTimerPeriod;
   tANI_U16          NeighborRoamScanRefreshPeriod;
   tANI_U16          NeighborScanChannelMinTime;
   tANI_U16          NeighborScanChannelMaxTime;
   tANI_U16          EmptyRefreshScanPeriod;
   tANI_U8           ValidChannelCount;
   tANI_U8           ValidChannelList[WLAN_HAL_ROAM_SCAN_MAX_CHANNELS];
   eAniBoolean       IsCCXEnabled;

   tANI_U16          us24GProbeSize;
   tANI_U8           a24GProbeTemplate[WLAN_HAL_ROAM_SCAN_MAX_PROBE_SIZE];
   tANI_U16          us5GProbeSize;
   tANI_U8           a5GProbeTemplate[WLAN_HAL_ROAM_SCAN_MAX_PROBE_SIZE];
   /*                    */
   tANI_U8           nProbes;
   tANI_U16          HomeAwayTime;
   eAniBoolean       MAWCEnabled;
   tANI_S8           RxSensitivityThreshold;
   tANI_U8           ReservedBytes[WLAN_HAL_ROAM_SCAN_RESERVED_BYTES];
   tRoamNetworkType  ConnectedNetwork;
   tMobilityDomainInfo MDID;
} tRoamCandidateListParams, * tpRoamCandidateListParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tRoamCandidateListParams RoamScanOffloadNetwListParams;
}  tSetRoamScanOffloadReq, *tpRoamScanOffloadReq;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;

  /*                                                                   
                                         */
   tANI_U32   status;
}  tSetRoamOffloadScanResp, *tpSetRoamOffloadScanResp;
#endif

/*
                                 
*/
typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;

   /*                                                                   
                                         */
   tANI_U32   status;
}  tSetPrefNetwListResp, *tpSetPrefNetwListResp;

/*
                                         
*/
typedef PACKED_PRE struct PACKED_POST {

  /*                                            */
  tSirMacSSid ssId;

  /*                   */
  tANI_U8     rssi;

  //                                                                              
  tANI_U16    frameLength;

} tPrefNetwFoundParams, * tpPrefNetwFoundParams;

/*
                                    
*/
typedef PACKED_PRE struct PACKED_POST {

   tHalMsgHeader header;
   tPrefNetwFoundParams   prefNetwFoundParams;
}  tPrefNetwFoundInd, *tpPrefNetwFoundInd;


typedef PACKED_PRE struct PACKED_POST {

  /*              */
  tANI_U8          ucRssiThreshold;

} tRssiFilterParams, * tpRssiFilterParams;

/*
                      
*/
typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tRssiFilterParams   prefRSSIFilterParams;
}  tSetRssiFilterReq, *tpSetRssiFilterReq;

/*
                     
*/
typedef PACKED_PRE struct PACKED_POST{
   tHalMsgHeader header;
   /*                      */
   tANI_U32   status;
}  tSetRssiFilterResp, *tpSetRssiFilterResp;
/*
                     
*/
typedef PACKED_PRE struct PACKED_POST  
{

  /*                    */
  tANI_U8   b11dEnabled; 

  /*                                                            */
  tANI_U8   b11dResolved;

  /*                                        */
  tANI_U8   ucChannelCount; 
  tANI_U8   aChannels[WLAN_HAL_PNO_MAX_NETW_CHANNELS]; 

  /*                    */
  tANI_U16  usActiveMinChTime; 

  /*                    */
  tANI_U16  usActiveMaxChTime; 

  /*                    */
  tANI_U16  usPassiveMinChTime; 

  /*                    */
  tANI_U16  usPassiveMaxChTime; 

  /*        */
  ePhyChanBondState cbState;

} tUpdateScanParams, * tpUpdateScanParams;

/*
                     
*/
typedef PACKED_PRE struct PACKED_POST  
{

  /*                    */
  tANI_U8   b11dEnabled; 

  /*                                                            */
  tANI_U8   b11dResolved;

  /*                                        */
  tANI_U8   ucChannelCount; 
  tANI_U8   aChannels[WLAN_HAL_PNO_MAX_NETW_CHANNELS_EX]; 

  /*                    */
  tANI_U16  usActiveMinChTime; 

  /*                    */
  tANI_U16  usActiveMaxChTime; 

  /*                    */
  tANI_U16  usPassiveMinChTime; 

  /*                    */
  tANI_U16  usPassiveMaxChTime; 

  /*        */
  ePhyChanBondState cbState;

} tUpdateScanParamsEx, * tpUpdateScanParamsEx;

/*
                                            
                                 
*/
typedef PACKED_PRE struct PACKED_POST{

   tHalMsgHeader header;
   tUpdateScanParams   scanParams;
}  tUpdateScanParamsReq, *tpUpdateScanParamsReq;

/*
                                            
                                 
*/
typedef PACKED_PRE struct PACKED_POST{

   tHalMsgHeader header;
   tUpdateScanParamsEx   scanParams;
}  tUpdateScanParamsReqEx, *tpUpdateScanParamsReqEx;

/*
                                            
                                 
*/
typedef PACKED_PRE struct PACKED_POST{

   tHalMsgHeader header;

   /*                      */
   tANI_U32   status;

}  tUpdateScanParamsResp, *tpUpdateScanParamsResp;

/*                                                                           
                                   
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
    tANI_U8  ucTxPerTrackingEnable;           /*                      */
    tANI_U8  ucTxPerTrackingPeriod;           /*                            */
    tANI_U8  ucTxPerTrackingRatio;            /*                                                            */
    tANI_U32 uTxPerTrackingWatermark;         /*                                                                                                   */
} tHalTxPerTrackingReqParam, *tpHalTxPerTrackingReqParam;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalTxPerTrackingReqParam txPerTrackingParams;
}  tHalSetTxPerTrackingReqMsg, *tpHalSetTxPerTrackingReqMsg;

/*                                                                           
                                   
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
    /*                    */
    tANI_U32   status;
} tHalTxPerTrackingRspParams, *tpHalTxPerTrackingRspParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalTxPerTrackingRspParams txPerTrackingRspParams;
}  tHalSetTxPerTrackingRspMsg, *tpHalSetTxPerTrackingRspMsg;

/*                                                                           
                          
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader   header;
}tTxPerHitIndMsg, *tpTxPerHitIndMsg;

/*                                                                           
                                                                         
                                                                            */
#define    HAL_PROTOCOL_DATA_LEN                  8
#define    HAL_MAX_NUM_MULTICAST_ADDRESS        240
#define    HAL_MAX_NUM_FILTERS                   20
#define    HAL_MAX_CMP_PER_FILTER                10

typedef enum
{
  HAL_RCV_FILTER_TYPE_INVALID,
  HAL_RCV_FILTER_TYPE_FILTER_PKT,
  HAL_RCV_FILTER_TYPE_BUFFER_PKT,
  HAL_RCV_FILTER_TYPE_MAX_ENUM_SIZE
}tHalReceivePacketFilterType;

typedef enum 
{
  HAL_FILTER_PROTO_TYPE_INVALID,
  HAL_FILTER_PROTO_TYPE_MAC,
  HAL_FILTER_PROTO_TYPE_ARP,
  HAL_FILTER_PROTO_TYPE_IPV4,
  HAL_FILTER_PROTO_TYPE_IPV6,
  HAL_FILTER_PROTO_TYPE_UDP,
  HAL_FILTER_PROTO_TYPE_MAX
}tHalRcvPktFltProtocolType;

typedef enum 
{
  HAL_FILTER_CMP_TYPE_INVALID,
  HAL_FILTER_CMP_TYPE_EQUAL,
  HAL_FILTER_CMP_TYPE_MASK_EQUAL,
  HAL_FILTER_CMP_TYPE_NOT_EQUAL,
  HAL_FILTER_CMP_TYPE_MAX
}tHalRcvPktFltCmpFlagType;

typedef PACKED_PRE struct PACKED_POST 
{
    tANI_U8                          protocolLayer;
    tANI_U8                          cmpFlag;
    tANI_U16                         dataLength; /*                               */
    tANI_U8                          dataOffset; /*                                           */
    tANI_U8                          reserved; /*                */
    tANI_U8                          compareData[HAL_PROTOCOL_DATA_LEN];  /*                 */
    tANI_U8                          dataMask[HAL_PROTOCOL_DATA_LEN];   /*                                                               */
}tHalRcvPktFilterParams, *tpHalRcvPktFilterParams;

typedef PACKED_PRE struct PACKED_POST
{
    tANI_U8                         filterId;
    tANI_U8                         filterType;
    tANI_U8                         numParams; 
    tANI_U32                        coalesceTime;
    tHalRcvPktFilterParams          paramsData[1];
}tHalRcvPktFilterCfgType, *tpHalRcvPktFilterCfgType;

typedef PACKED_PRE struct PACKED_POST
{
    tANI_U8                         filterId;
    tANI_U8                         filterType;
    tANI_U8                         numParams; 
    tANI_U32                        coleasceTime; 
    tANI_U8                         bssIdx;
    tHalRcvPktFilterParams          paramsData[1];
}tHalSessionizedRcvPktFilterCfgType, *tpHalSessionizedRcvPktFilterCfgType;

typedef PACKED_PRE struct PACKED_POST
{
  tHalMsgHeader                 header;
  tHalRcvPktFilterCfgType       pktFilterCfg;
} tHalSetRcvPktFilterReqMsg, *tpHalSetRcvPktFilterReqMsg;

typedef PACKED_PRE struct PACKED_POST
{
    tANI_U8         dataOffset; /*                                           */
    tANI_U32       cMulticastAddr;
    tSirMacAddr    multicastAddr[HAL_MAX_NUM_MULTICAST_ADDRESS];
    tANI_U8        bssIdx;
} tHalRcvFltMcAddrListType, *tpHalRcvFltMcAddrListType;

typedef PACKED_PRE struct PACKED_POST
{
    /*                    */
    tANI_U32   status;
    tANI_U8    bssIdx;
} tHalSetPktFilterRspParams, *tpHalSetPktFilterRspParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader               header;
   tHalSetPktFilterRspParams   pktFilterRspParams;
}  tHalSetPktFilterRspMsg, *tpHalSetPktFilterRspMsg;

typedef PACKED_PRE struct PACKED_POST
{
   tANI_U8    bssIdx;
} tHalRcvFltPktMatchCntReqParams, *tpHalRcvFltPktMatchCntReqParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader        header;
   tHalRcvFltPktMatchCntReqParams   pktMatchCntReqParams;
} tHalRcvFltPktMatchCntReqMsg, *tpHalRcvFltPktMatchCntReqMsg;


typedef PACKED_PRE struct PACKED_POST
{
   tANI_U8    filterId;
   tANI_U32   matchCnt;
} tHalRcvFltPktMatchCnt;
typedef PACKED_PRE struct PACKED_POST
{
   /*                    */
   tANI_U32                 status;
   tANI_U32                 matchCnt;   
   tHalRcvFltPktMatchCnt    filterMatchCnt[HAL_MAX_NUM_FILTERS]; 
   tANI_U8                  bssIdx;
} tHalRcvFltPktMatchRspParams, *tptHalRcvFltPktMatchRspParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader  header;
   tHalRcvFltPktMatchRspParams fltPktMatchRspParams;
} tHalRcvFltPktMatchCntRspMsg, *tpHalRcvFltPktMatchCntRspMsg;

typedef PACKED_PRE struct PACKED_POST
{
    tANI_U32   status;  /*                                 */
    tANI_U8    filterId;
    tANI_U8    bssIdx;
}tHalRcvFltPktClearParam, *tpHalRcvFltPktClearParam;

typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader               header;
    tHalRcvFltPktClearParam     filterClearParam;
} tHalRcvFltPktClearReqMsg, *tpHalRcvFltPktClearReqMsg;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader              header;
   tHalRcvFltPktClearParam    filterClearParam;
} tHalRcvFltPktClearRspMsg, *tpHalRcvFltPktClearRspMsg;

typedef PACKED_PRE struct PACKED_POST
{
    tANI_U32   status;  
    tANI_U8    bssIdx;
}tHalRcvFltPktSetMcListRspType, *tpHalRcvFltPktSetMcListRspType;

typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader                    header;
    tHalRcvFltMcAddrListType         mcAddrList;
} tHalRcvFltPktSetMcListReqMsg, *tpHalRcvFltPktSetMcListReqMsg;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader                    header;
   tHalRcvFltPktSetMcListRspType    rspParam;
} tHalRcvFltPktSetMcListRspMsg, *tpHalRcvFltPktSetMcListRspMsg;


/*                                                                           
                                                                       
                                                                            */

/*
                                         
                                   
                                                           
 */
typedef PACKED_PRE struct PACKED_POST
{
   /*              */
  tANI_U32 uIgnoreDTIM;

  /*           */
  tANI_U32 uDTIMPeriod;

  /*                 */
  tANI_U32 uListenInterval;

  /*                             */
  tANI_U32 uBcastMcastFilter;

  /*                          */
  tANI_U32 uEnableBET;

  /*                                   */
  tANI_U32 uBETInterval;
}tSetPowerParamsVer1Type, *tpSetPowerParamsVer1Type;

typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader               header;
    tSetPowerParamsVer1Type     powerParams;
} tSetPowerParamsVer1ReqMsg, *tpSetPowerParamsVer1ReqMsg;

typedef PACKED_PRE struct PACKED_POST
{
   /*              */
  tANI_U32 uIgnoreDTIM;

  /*           */
  tANI_U32 uDTIMPeriod; 

  /*                 */
  tANI_U32 uListenInterval;

  /*                             */
  tANI_U32 uBcastMcastFilter;

  /*                          */
  tANI_U32 uEnableBET;

  /*                                   */
  tANI_U32 uBETInterval; 

  /*                           */
  tANI_U32 uMaxLIModulatedDTIM;
}tSetPowerParamsType, *tpSetPowerParamsType;

typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader               header;
    tSetPowerParamsType         powerParams;
} tSetPowerParamsReqMsg, *tpSetPowerParamsReqMsg;

typedef PACKED_PRE struct PACKED_POST{

   tHalMsgHeader header;

   /*                      */
   tANI_U32   status;

}  tSetPowerParamsResp, *tpSetPowerParamsResp;

/*                                                                           
                                                                                      
                                                                            */

typedef enum {
    MCC                    = 0,
    P2P                    = 1,
    DOT11AC                = 2,
    SLM_SESSIONIZATION     = 3,
    DOT11AC_OPMODE         = 4,
    SAP32STA               = 5,
    TDLS                   = 6,
    P2P_GO_NOA_DECOUPLE_INIT_SCAN = 7,
    WLANACTIVE_OFFLOAD     = 8,
    BEACON_OFFLOAD         = 9,
    SCAN_OFFLOAD           = 10,
    ROAM_OFFLOAD           = 11,
    BCN_MISS_OFFLOAD       = 12,
    STA_POWERSAVE          = 13,
    STA_ADVANCED_PWRSAVE   = 14,
    AP_UAPSD               = 15,
    AP_DFS                 = 16,
    BLOCKACK               = 17,
    PHY_ERR                = 18,
    BCN_FILTER             = 19,
    RTT                    = 20,
    RATECTRL               = 21,
    WOW                    = 22,
    WLAN_ROAM_SCAN_OFFLOAD = 23,
    SPECULATIVE_PS_POLL    = 24,
    SCAN_SCH               = 25,
    IBSS_HEARTBEAT_OFFLOAD = 26,
    WLAN_SCAN_OFFLOAD      = 27,
    WLAN_PERIODIC_TX_PTRN  = 28,
    ADVANCE_TDLS           = 29,
    BATCH_SCAN             = 30,
    EXTENDED_NSOFFLOAD_SLOT = 32,
    UPDATE_CHANNEL_LIST    = 35,
    MAX_FEATURE_SUPPORTED  = 128,
} placeHolderInCapBitmap;

typedef PACKED_PRE struct PACKED_POST{

   tANI_U32 featCaps[4];
}  tWlanFeatCaps, *tpWlanFeatCaps;

typedef PACKED_PRE struct PACKED_POST{

   tHalMsgHeader header;    
   tWlanFeatCaps wlanFeatCaps;

}  tWlanFeatCapsMsg, *tpWlanFeatCapsMsg;

#define IS_MCC_SUPPORTED_BY_HOST (!!(halMsg_GetHostWlanFeatCaps(MCC)))
#define IS_SLM_SESSIONIZATION_SUPPORTED_BY_HOST (!!(halMsg_GetHostWlanFeatCaps(SLM_SESSIONIZATION)))
#define IS_FEATURE_SUPPORTED_BY_HOST(featEnumValue) (!!halMsg_GetHostWlanFeatCaps(featEnumValue))
#define IS_WLANACTIVE_OFFLOAD_SUPPORTED_BY_HOST (!!(halMsg_GetHostWlanFeatCaps(WLANACTIVE_OFFLOAD)))
#define IS_WLAN_ROAM_SCAN_OFFLOAD_SUPPORTED_BY_HOST (!!(halMsg_GetHostWlanFeatCaps(WLAN_ROAM_SCAN_OFFLOAD)))
#define IS_IBSS_HEARTBEAT_OFFLOAD_SUPPORTED_BY_HOST (!!(halMsg_GetHostWlanFeatCaps(IBSS_HEARTBEAT_OFFLOAD)))
#define IS_SCAN_OFFLOAD_SUPPORTED_BY_HOST (!!(halMsg_GetHostWlanFeatCaps(WLAN_SCAN_OFFLOAD)))

tANI_U8 halMsg_GetHostWlanFeatCaps(tANI_U8 feat_enum_value);

#define setFeatCaps(a,b)   {  tANI_U32 arr_index, bit_index; \
                              if ((b)<=127) { \
                                arr_index = (b)/32; \
                                bit_index = (b)%32; \
                                if(arr_index < 4) \
                                (a)->featCaps[arr_index] |= (1<<bit_index); \
                              } \
                           }
#define getFeatCaps(a,b,c) {  tANI_U32 arr_index, bit_index; \
                              if ((b)<=127) { \
                                arr_index = (b)/32; \
                                bit_index = (b)%32; \
                                (c) = ((a)->featCaps[arr_index] & (1<<bit_index))?1:0; \
                              } \
                           }
#define clearFeatCaps(a,b) {  tANI_U32 arr_index, bit_index; \
                              if ((b)<=127) { \
                                arr_index = (b)/32; \
                                bit_index = (b)%32; \
                                (a)->featCaps[arr_index] &= ~(1<<bit_index); \
                              } \
                           }

/*                                                                           
                           
                                                                            */

/*                                           */
typedef enum
{
    WLAN_HAL_GTK_REKEY_STATUS_SUCCESS            = 0,
    WLAN_HAL_GTK_REKEY_STATUS_NOT_HANDLED        = 1, /*                                 */
    WLAN_HAL_GTK_REKEY_STATUS_MIC_ERROR          = 2, /*                       */
    WLAN_HAL_GTK_REKEY_STATUS_DECRYPT_ERROR      = 3, /*                         */
    WLAN_HAL_GTK_REKEY_STATUS_REPLAY_ERROR       = 4, /*                    */
    WLAN_HAL_GTK_REKEY_STATUS_MISSING_KDE        = 5, /*                                  */
    WLAN_HAL_GTK_REKEY_STATUS_MISSING_IGTK_KDE   = 6, /*                                   */
    WLAN_HAL_GTK_REKEY_STATUS_INSTALL_ERROR      = 7, /*                        */
    WLAN_HAL_GTK_REKEY_STATUS_IGTK_INSTALL_ERROR = 8, /*                             */
    WLAN_HAL_GTK_REKEY_STATUS_RESP_TX_ERROR      = 9, /*                                */

    WLAN_HAL_GTK_REKEY_STATUS_GEN_ERROR          = 255 /*                            */
} tGTKRekeyStatus;

/*                   */
typedef enum
{
    WLAN_HAL_WAKE_REASON_NONE             = 0,
    WLAN_HAL_WAKE_REASON_MAGIC_PACKET     = 1,  /*                    */
    WLAN_HAL_WAKE_REASON_PATTERN_MATCH    = 2,  /*                            */
    WLAN_HAL_WAKE_REASON_EAPID_PACKET     = 3,  /*                       */
    WLAN_HAL_WAKE_REASON_EAPOL4WAY_PACKET = 4,  /*                                         */
    WLAN_HAL_WAKE_REASON_NETSCAN_OFFL_MATCH = 5, /*                            */
    WLAN_HAL_WAKE_REASON_GTK_REKEY_STATUS = 6,  /*                                      */
    WLAN_HAL_WAKE_REASON_BSS_CONN_LOST    = 7,  /*                     */
} tWakeReasonType;

/*
                                                           
                                                                             

                                        
                                         
                                        
                                            
                                            

                                                                         

*/

/*
                                   
*/
typedef PACKED_PRE struct PACKED_POST
{
    uint32  ulReason;        /*                     */
    uint32  ulReasonArg;     /*                                      */
    uint32  ulStoredDataLen; /*                                                        
                                                                                    
                                                                  */
    uint32  ulActualDataLen; /*                       */
    uint8   aDataStart[1];   /*                                                        
                                                    */
} tWakeReasonParams, *tpWakeReasonParams;

/*
                        
*/
typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader       header;
    tWakeReasonParams   wakeReasonParams;
    tANI_U32            uBssIdx : 8;
    tANI_U32            bReserved : 24;
} tHalWakeReasonInd, *tpHalWakeReasonInd;

/*                                                                           
                          
                                                                           */

#define HAL_GTK_KEK_BYTES 16
#define HAL_GTK_KCK_BYTES 16

#define WLAN_HAL_GTK_OFFLOAD_FLAGS_DISABLE (1 << 0)

#define GTK_SET_BSS_KEY_TAG  0x1234AA55

typedef PACKED_PRE struct PACKED_POST
{
  tANI_U32     ulFlags;             /*                */
  tANI_U8      aKCK[HAL_GTK_KCK_BYTES];  /*                      */ 
  tANI_U8      aKEK[HAL_GTK_KEK_BYTES];  /*                    */
  tANI_U64     ullKeyReplayCounter; /*                */
  tANI_U8      bssIdx;
} tHalGtkOffloadReqParams, *tpHalGtkOffloadReqParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalGtkOffloadReqParams gtkOffloadReqParams;
}  tHalGtkOffloadReqMsg, *tpHalGtkOffloadReqMsg;

/*                                                                           
                          
                                                                           */
typedef PACKED_PRE struct PACKED_POST
{
    tANI_U32   ulStatus;   /*                    */
    tANI_U8    bssIdx;
} tHalGtkOffloadRspParams, *tpHalGtkOffloadRspParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalGtkOffloadRspParams gtkOffloadRspParams;
}  tHalGtkOffloadRspMsg, *tpHalGtkOffloadRspMsg;


/*                                                                           
                                  
                                                                           */
typedef PACKED_PRE struct PACKED_POST
{
   tANI_U8    bssIdx;

} tHalGtkOffloadGetInfoReqParams, *tptHalGtkOffloadGetInfoReqParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalGtkOffloadGetInfoReqParams gtkOffloadGetInfoReqParams;
}  tHalGtkOffloadGetInfoReqMsg, *tpHalGtkOffloadGetInfoReqMsg;

/*                                                                           
                                  
                                                                           */
typedef PACKED_PRE struct PACKED_POST
{
   tANI_U32   ulStatus;             /*                    */
   tANI_U32   ulLastRekeyStatus;    /*                                                */
   tANI_U64   ullKeyReplayCounter;  /*                              */
   tANI_U32   ulTotalRekeyCount;    /*                      */
   tANI_U32   ulGTKRekeyCount;      /*                       */
   tANI_U32   ulIGTKRekeyCount;     /*                        */
   tANI_U8    bssIdx;
} tHalGtkOffloadGetInfoRspParams, *tptHalGtkOffloadGetInfoRspParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalGtkOffloadGetInfoRspParams gtkOffloadGetInfoRspParams;
}  tHalGtkOffloadGetInfoRspMsg, *tpHalGtkOffloadGetInfoRspMsg;

/*                                                                           
                   
                                                                           */
typedef PACKED_PRE struct PACKED_POST
{
   /*                                                                  */
    tANI_U8 device_mode;
    tSirMacAddr macAddr;
} tDHCPInfo, *tpDHCPInfo;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader  header;
   tANI_U32       status;  /*                    */
} tDHCPIndStatus, *tpDHCPIndstatus;

/*
                                        
                                                                       
                                                                           
                          
                                                
*/
typedef enum
{
  HAL_THERMAL_MITIGATION_MODE_INVALID = -1,
  HAL_THERMAL_MITIGATION_MODE_0,
  HAL_THERMAL_MITIGATION_MODE_1,
  HAL_THERMAL_MITIGATION_MODE_2,
  HAL_THERMAL_MITIGATION_MODE_MAX = WLAN_HAL_MAX_ENUM_SIZE,
}tHalThermalMitigationModeType;
//                                               

/*
                            
                                                                       
                                                                  

                                                                            
                                           
                                                                   
                                                                   
                                                                   
                                                                   
*/
typedef enum 
{
  HAL_THERMAL_MITIGATION_LEVEL_INVALID = -1,
  HAL_THERMAL_MITIGATION_LEVEL_0,
  HAL_THERMAL_MITIGATION_LEVEL_1,
  HAL_THERMAL_MITIGATION_LEVEL_2,
  HAL_THERMAL_MITIGATION_LEVEL_3,
  HAL_THERMAL_MITIGATION_LEVEL_4,
  HAL_THERMAL_MITIGATION_LEVEL_MAX = WLAN_HAL_MAX_ENUM_SIZE,
}tHalThermalMitigationLevelType;
//                                                

typedef PACKED_PRE struct PACKED_POST
{
   /*                                   */
   tHalThermalMitigationModeType thermalMitMode;

   /*                          */
   tHalThermalMitigationLevelType thermalMitLevel;
    
}tSetThermalMitgationType, *tpSetThermalMitgationType;

/*                                     */
typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader               header;
    tSetThermalMitgationType    thermalMitParams;
} tSetThermalMitigationReqMsg, *tpSetThermalMitigationReqMsg;

typedef PACKED_PRE struct PACKED_POST{

   tHalMsgHeader header;

   /*                      */
   tANI_U32   status;

}  tSetThermalMitigationResp, *tpSetThermalMitigationResp;

/*                                                                     
                                               */
typedef PACKED_PRE struct PACKED_POST {
   /*          */
   uint32 txBytesPushed;
   uint32 txPacketsPushed;

   /*          */
   uint32 rxBytesRcvd;
   uint32 rxPacketsRcvd;
   uint32 rxTimeTotal;
} tStaStatsClassB, *tpStaStatsClassB;

typedef PACKED_PRE struct PACKED_POST {

   /*                                              */
   tANI_U32 duration;

   /*               */
   tStaStatsClassB staStatsClassB[HAL_NUM_STA];
} tStatsClassBIndParams, *tpStatsClassBIndParams;

typedef PACKED_PRE struct PACKED_POST {

   tHalMsgHeader header;

   /*               */
   tStatsClassBIndParams statsClassBIndParams;
} tStatsClassBInd, *tpStatsClassBInd;

/*                                   */
#ifdef FEATURE_WIFI_PROXIMITY

typedef PACKED_PRE struct PACKED_POST{

   tANI_U8  wifiProximityChannel;
   tANI_U32 wifiProximityDuration;
   tANI_U32 wifiProximityInterval;
   tANI_U32 wifiProximityMode;
   tANI_U32 wifiProximityStatus;
   tSirMacAddr bssId;
   tSirMacSSid ssId;

} tSetWifiProximityReqParam, *tpSetWifiProximityReqParam;

typedef PACKED_PRE struct PACKED_POST
{
  tHalMsgHeader header;

  tSetWifiProximityReqParam wifiProximityReqParams;

}tSetWifiProximityReqMsg, *tpSetWifiProximityReqMsg;

/*                           */
typedef PACKED_PRE struct PACKED_POST{

   tHalMsgHeader header;

   /*                      */
   tANI_U32   status;

}  tSetWifiProximityRspMsg, *tpSetWifiProxmityRspMsg;

#endif

#ifdef FEATURE_SPECULATIVE_PS_POLL
/*                                                                           
                                          
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
   tANI_U8         bssIdx;
   tANI_U16 serviceInterval;
   tANI_U16 suspendInterval;
   tANI_U8 acMask;
} tHalStartSpecPsPollReqParams, *tpHalStartSpecPsPollReqParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalStartSpecPsPollReqParams specPsPollReq;
}  tHalStartSpecPsPollReqMsg, *tpHalStartSpecPsPollReqMsg;

/*                                                                           
                                          
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
    /*                    */
    tANI_U32   status;
    tANI_U8    bssIdx;
} tHalStartSpecPsPollRspParams, *tpHalStartSpecPsPollRspParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalStartSpecPsPollRspParams startSpecPsPollRspParams;
}  tHalStartSpecPsPollRspMsg, *tpHalStartSpecPsPollRspMsg;

/*                                                                           
                                         
                                                                            */
typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tANI_U8     bssIdx;
}  tHalStopSpecPsPollsIndMsg, *tpHalStopSpecPsPollsIndMsg;
#endif

#ifdef FEATURE_WLAN_TDLS
#define HAL_MAX_SUPP_CHANNELS 128
#define HAL_MAX_SUPP_OPER_CLASSES 32
/*                                                                           
                                     
                                                                           */
typedef PACKED_PRE struct PACKED_POST
{
    /*         */
    tANI_U16        staIdx;

    /*                                                      */
    tANI_U8                bIsResponder;

    /*          */
    tANI_U8        acVOUAPSDFlag:1;
    tANI_U8        acVIUAPSDFlag:1;
    tANI_U8        acBKUAPSDFlag:1;
    tANI_U8        acBEUAPSDFlag:1;
    tANI_U8        aAck:1;
    tANI_U8        maxServicePeriodLength:2;
    tANI_U8        moreDataAck:1;

    /*                                   */
    tANI_U8        TPUBufferStaSupport;

    /*                                */
    tANI_U8        tdlsOffChannelSupport;
    tANI_U8        peerCurrOperClass;
    tANI_U8        selfCurrOperClass;
    tANI_U8        validChannelsLen;
    tANI_U8        validChannels[HAL_MAX_SUPP_CHANNELS];
    tANI_U8        validOperClassesLen;
    tANI_U8        validOperClasses[HAL_MAX_SUPP_OPER_CLASSES];
}tTDLSLinkEstablishedType, *tpTDLSLinkEstablishedType;

typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader               header;
    tTDLSLinkEstablishedType    tdlsLinkEstablishedParams;
} tTDLSLinkEstablishedReqMsg, *tpTDLSLinkEstablishedReqMsg;

/*                                                                           
                                     
                                                                           */

typedef PACKED_PRE struct PACKED_POST
{
    tANI_U32   status;

    /*         */
    tANI_U16        staIdx;
} tTDLSLinkEstablishedResp, *tpTDLSLinkEstablishedResp;

typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tTDLSLinkEstablishedResp TDLSLinkEstablishedRespParams;
}  tTDLSLinkEstablishedRespMsg,  *tpTDLSLinkEstablishedRespMsg;

/*                                                                           
                                  
                                                                           */
typedef PACKED_PRE struct PACKED_POST
{
    /*         */
    tANI_U16        staIdx;
}tTDLSLinkTeardownType, *tpTDLSLinkTeardownType;

typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader               header;
    tTDLSLinkTeardownType    tdlsLinkTeardownParams;
} tTDLSLinkTeardownReqMsg, *tpTDLSLinkTeardownReqMsg;

/*                                                                           
                                  
                                                                           */

typedef PACKED_PRE struct PACKED_POST
{
    tANI_U32   status;

    /*         */
    tANI_U16        staIdx;
} tTDLSLinkTeardownResp, *tpTDLSLinkTeardownResp;

typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tTDLSLinkTeardownResp TDLSLinkTeardownRespParams;
}  tTDLSLinkTeardownRespMsg,  *tpTDLSLinkTeardownRespMsg;

/*                                                                           
                   
                                                                            */

typedef PACKED_PRE struct PACKED_POST
{
    tANI_U16    assocId;
    tANI_U16    staIdx;
    tANI_U16    status;
    tANI_U16    reasonCode;
}tTdlsIndParams, *tpTdlsIndParams;


typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tTdlsIndParams tdlsIndParams;
}tTdlsIndMsg, *tpTdlsIndMsg;

#endif

/*                                                                           
                                   
                                                                            */

typedef PACKED_PRE struct PACKED_POST
{
    tANI_U8     bssIdx;
    tANI_U8     staIdx;
    tSirMacAddr staAddr;
}tIbssPeerInactivityIndParams, *tpIbssPeerInactivityIndParams;


typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tIbssPeerInactivityIndParams ibssPeerInactivityIndParams;
}tIbssPeerInactivityIndMsg, *tpIbssPeerInactivityIndMsg;


/*                                                       */
#define HAL_NUM_SCAN_SSID           10
#define HAL_NUM_SCAN_BSSID          4

/*
                                                     
 */
typedef enum
{
    eSIR_PASSIVE_SCAN,
    eSIR_ACTIVE_SCAN = WLAN_HAL_MAX_ENUM_SIZE,
} tSirScanType;

typedef PACKED_PRE struct PACKED_POST
{
  tANI_U8      numBssid;
  tSirMacAddr  bssid[HAL_NUM_SCAN_BSSID];
  tANI_U8      numSsid;
  tSirMacSSid  ssid[HAL_NUM_SCAN_SSID];
  tANI_BOOLEAN hiddenSsid;
  tSirMacAddr  selfMacAddr;
  tSirBssType  bssType;
  tSirScanType scanType;
  tANI_U32     minChannelTime;
  tANI_U32     maxChannelTime;
  tANI_BOOLEAN p2pSearch;
  tANI_U8      channelCount;
  tANI_U8      channels[WLAN_HAL_ROAM_SCAN_MAX_CHANNELS];
  tANI_U16     ieFieldLen;
  tANI_U8      ieField[1];
}tScanOffloadReqType, *tpScanOffloadReqType;

/*                                                                           
                                  
                                                                           */
typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tScanOffloadReqType scanOffloadParams;
}  tHalStartScanOffloadReqMsg, *tpHalStartScanOffloadReqMsg;

/*                                                                           
                                  
                                                                           */
typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;

  /*                                                            
                                                              */
   tANI_U32   status;
}  tHalStartScanOffloadRspMsg, *tpHalStartScanOffloadRspMsg;

/*                                                                           
                                 
                                                                           */
typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
}  tHalStopScanOffloadReqMsg, *tpHalStopScanOffloadReqMsg;

/*                                                                           
                                 
                                                                           */
typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;

  /*                                                            
                                       */
   tANI_U32   status;
}  tHalStopScanOffloadRspMsg, *tpHalStopScanOffloadRspMsg;

/*
                                                               
 */
typedef enum
{
    WLAN_HAL_SCAN_EVENT_STARTED = 0x1,        /*                             */
    WLAN_HAL_SCAN_EVENT_COMPLETED = 0x2,      /*                               */
    WLAN_HAL_SCAN_EVENT_BSS_CHANNEL = 0x4,    /*                                     */
    WLAN_HAL_SCAN_EVENT_FOREIGN_CHANNEL = 0x8,/*                                        */
    WLAN_HAL_SCAN_EVENT_DEQUEUED = 0x10,      /*                           */
    WLAN_HAL_SCAN_EVENT_PREEMPTED = 0x20,     /*                                       */
    WLAN_HAL_SCAN_EVENT_START_FAILED = 0x40,  /*                   */
    WLAN_HAL_SCAN_EVENT_RESTARTED = 0x80,     /*              */
    WLAN_HAL_SCAN_EVENT_MAX = WLAN_HAL_MAX_ENUM_SIZE
} tScanEventType;

typedef PACKED_PRE struct PACKED_POST
{
    tScanEventType event;
    tANI_U32 channel;
    tANI_U32 scanId;
} tScanOffloadEventInfo;

/*                                                                           
                                  
                                                                           */
typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tScanOffloadEventInfo scanOffloadInd;
}  tHalScanOffloadIndMsg, *tpHalScanOffloadIndMsg;

typedef PACKED_PRE struct PACKED_POST {
    /*                                          */
    tANI_U32 mhz;
    /*                           */
    tANI_U32 band_center_freq1;
    /*                                                                  */
    tANI_U32 band_center_freq2;
    /*                                                                
                                
                                                                  */
    tANI_U32 channel_info;
    /*                                                             */
    tANI_U32 reg_info_1;
    /*                      */
    tANI_U32 reg_info_2;
} tUpdateChannelParam;


typedef enum {
    WLAN_HAL_MODE_11A            = 0,   /*          */
    WLAN_HAL_MODE_11G            = 1,   /*            */
    WLAN_HAL_MODE_11B            = 2,   /*          */
    WLAN_HAL_MODE_11GONLY        = 3,   /*               */
    WLAN_HAL_MODE_11NA_HT20      = 4,  /*               */
    WLAN_HAL_MODE_11NG_HT20      = 5,  /*               */
    WLAN_HAL_MODE_11NA_HT40      = 6,  /*               */
    WLAN_HAL_MODE_11NG_HT40      = 7,  /*               */
    WLAN_HAL_MODE_11AC_VHT20     = 8,
    WLAN_HAL_MODE_11AC_VHT40     = 9,
    WLAN_HAL_MODE_11AC_VHT80     = 10,
    WLAN_HAL_MODE_11AC_VHT20_2G  = 11,
    WLAN_HAL_MODE_11AC_VHT40_2G  = 12,
    WLAN_HAL_MODE_11AC_VHT80_2G  = 13,
    WLAN_HAL_MODE_UNKNOWN        = 14,

} tChannelPhyModeType;

#define WLAN_HAL_CHAN_FLAG_HT40_PLUS   6
#define WLAN_HAL_CHAN_FLAG_PASSIVE     7
#define WLAN_HAL_CHAN_ADHOC_ALLOWED    8
#define WLAN_HAL_CHAN_AP_DISABLED      9
#define WLAN_HAL_CHAN_FLAG_DFS         10
#define WLAN_HAL_CHAN_FLAG_ALLOW_HT    11  /*                               */
#define WLAN_HAL_CHAN_FLAG_ALLOW_VHT   12  /*                                */
#define WLAN_HAL_CHAN_CHANGE_CAUSE_CSA 13  /*                                    */

#define WLAN_HAL_SET_CHANNEL_FLAG(pwlan_hal_update_channel,flag) do { \
        (pwlan_hal_update_channel)->channel_info |=  (1 << flag);      \
     } while(0)

#define WLAN_HAL_GET_CHANNEL_FLAG(pwlan_hal_update_channel,flag)   \
        (((pwlan_hal_update_channel)->channel_info & (1 << flag)) >> flag)

#define WLAN_HAL_SET_CHANNEL_MIN_POWER(pwlan_hal_update_channel,val) do { \
     (pwlan_hal_update_channel)->reg_info_1 &= 0xffffff00;           \
     (pwlan_hal_update_channel)->reg_info_1 |= (val&0xff);           \
     } while(0)
#define WLAN_HAL_GET_CHANNEL_MIN_POWER(pwlan_hal_update_channel) ((pwlan_hal_update_channel)->reg_info_1 & 0xff )

#define WLAN_HAL_SET_CHANNEL_MAX_POWER(pwlan_hal_update_channel,val) do { \
     (pwlan_hal_update_channel)->reg_info_1 &= 0xffff00ff;           \
     (pwlan_hal_update_channel)->reg_info_1 |= ((val&0xff) << 8);    \
     } while(0)
#define WLAN_HAL_GET_CHANNEL_MAX_POWER(pwlan_hal_update_channel) ( (((pwlan_hal_update_channel)->reg_info_1) >> 8) & 0xff )

#define WLAN_HAL_SET_CHANNEL_REG_POWER(pwlan_hal_update_channel,val) do { \
     (pwlan_hal_update_channel)->reg_info_1 &= 0xff00ffff;           \
     (pwlan_hal_update_channel)->reg_info_1 |= ((val&0xff) << 16);   \
     } while(0)
#define WLAN_HAL_GET_CHANNEL_REG_POWER(pwlan_hal_update_channel) ( (((pwlan_hal_update_channel)->reg_info_1) >> 16) & 0xff )
#define WLAN_HAL_SET_CHANNEL_REG_CLASSID(pwlan_hal_update_channel,val) do { \
     (pwlan_hal_update_channel)->reg_info_1 &= 0x00ffffff;             \
     (pwlan_hal_update_channel)->reg_info_1 |= ((val&0xff) << 24);     \
     } while(0)
#define WLAN_HAL_GET_CHANNEL_REG_CLASSID(pwlan_hal_update_channel) ( (((pwlan_hal_update_channel)->reg_info_1) >> 24) & 0xff )

#define WLAN_HAL_SET_CHANNEL_ANTENNA_MAX(pwlan_hal_update_channel,val) do { \
     (pwlan_hal_update_channel)->reg_info_2 &= 0xffffff00;             \
     (pwlan_hal_update_channel)->reg_info_2 |= (val&0xff);             \
     } while(0)
#define WLAN_HAL_GET_CHANNEL_ANTENNA_MAX(pwlan_hal_update_channel) ((pwlan_hal_update_channel)->reg_info_2 & 0xff )

#define WLAN_HAL_SET_CHANNEL_MAX_TX_POWER(pwlan_hal_update_channel,val) do { \
     (pwlan_hal_update_channel)->reg_info_2 &= 0xffff00ff;              \
     (pwlan_hal_update_channel)->reg_info_2 |= ((val&0xff)<<8);         \
     } while(0)
#define WLAN_HAL_GET_CHANNEL_MAX_TX_POWER(pwlan_hal_update_channel) ( (((pwlan_hal_update_channel)->reg_info_2)>>8) & 0xff )

typedef PACKED_PRE struct PACKED_POST
{
    tANI_U8 numChan;
    tUpdateChannelParam chanParam[WLAN_HAL_ROAM_SCAN_MAX_CHANNELS];
} tUpdateChannelReqType;

/*                                                                           
                                   
                                                                           */
typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tUpdateChannelReqType updateChannelParams;
}  tHalUpdateChannelReqMsg, *tpHalUpdateChannelReqMsg;

/*                                                                           
                                   
                                                                           */
typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;

  /*                                                            
                                                              */
   tANI_U32   status;
}  tHalUpdateChannelRspMsg, *tpHalUpdateChannelRspMsg;


/*                                                                           
                      
                                                                           */
//                                                             
typedef PACKED_PRE struct PACKED_POST
{
   //                                                          
   //                                                          
   //                            
    tANI_U16 seqNo;
    tANI_U16 staId;
    tANI_U8 macAddr[HAL_MAC_ADDR_LEN];
} tHalTXFailIndParams, *tpHalTXFailIndParams;

typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tHalTXFailIndParams txFailIndParams;
}  tHalTXFailIndMsg, *tpHalTXFailIndMsg;

/*                                                                           
                              
                                                                           */
//                                                        
typedef PACKED_PRE struct PACKED_POST
{
    //                                                                                       
    tANI_U8 tx_fail_count;
} tTXFailMonitorInfo, *tpTXFailMonitorInfo;

typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader  header;
    tTXFailMonitorInfo txFailMonitor;
} tTXFailMonitorInd, *tpTXFailMonitorInd;

/*                                                                           
                                      
                                                                           */
typedef PACKED_PRE struct PACKED_POST
{
   tANI_U8 destIpv4Addr[HAL_IPV4_ADDR_LEN];
   tANI_U8 nextHopMacAddr[HAL_MAC_ADDR_LEN];
} tDestIpNextHopMacPair;

typedef PACKED_PRE struct PACKED_POST
{
   tANI_U8 numEntries;
   tDestIpNextHopMacPair destIpMacPair[1];
} tWlanIpForwardTableUpdateIndParam;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tWlanIpForwardTableUpdateIndParam ipForwardTableParams;
} tWlanIpForwardTableUpdateInd;

/*                                                                           
                         
                                                                           */

typedef PACKED_PRE struct PACKED_POST
{
    /*                                                                                     */
    tANI_S32 ucastDataRate; //            

    /*                                                 */
    tTxRateInfoFlags ucastDataRateTxFlag;

    /*                                                                     */
    tSirMacAddr bssid;

    /*                                                                          */
    tANI_S32 reliableMcastDataRate; //            

    /*                                                 */
    tTxRateInfoFlags reliableMcastDataRateTxFlag;

    /*                                                                                 */
    tANI_U32 mcastDataRate24GHz; //            

    /*                                                 */
    tTxRateInfoFlags mcastDataRate24GHzTxFlag;

    /*                                                                               */
    tANI_U32 mcastDataRate5GHz; //            

    /*                                                 */
    tTxRateInfoFlags mcastDataRate5GHzTxFlag;

} tHalRateUpdateParams, *tpHalRateUpdateParams;

typedef PACKED_PRE struct PACKED_POST
{
    tHalMsgHeader header;
    tHalRateUpdateParams halRateUpdateParams;
}  tHalRateUpdateInd, * tpHalRateUpdateInd;

/*                                                                           
                                
                                                                           */

#define WLAN_HAL_MAX_AVOID_FREQ_RANGE           4

typedef PACKED_PRE struct PACKED_POST
{
   tANI_U32     startFreq;
   tANI_U32     endFreq;
}  tHalFreqRange, *tpHalFreqRange;

typedef PACKED_PRE struct PACKED_POST
{
   tANI_U32         avoidCnt;
   tHalFreqRange    avoidRange[WLAN_HAL_MAX_AVOID_FREQ_RANGE];
}  tHalAvoidFreqRangeIndParams, *tpHalAvoidFreqRangeIndParams;

typedef PACKED_PRE struct PACKED_POST
{
   tHalMsgHeader header;
   tHalAvoidFreqRangeIndParams freqRangeIndParams;
}  tHalAvoidFreqRangeInd, *tpHalAvoidFreqRangeInd;

/*                                                                           
                                                                           */

#if defined(__ANI_COMPILER_PRAGMA_PACK_STACK)
#pragma pack(pop)
#elif defined(__ANI_COMPILER_PRAGMA_PACK)
#else
#endif

#endif /*                  */


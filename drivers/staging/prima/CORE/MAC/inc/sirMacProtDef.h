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
#ifndef __MAC_PROT_DEFS_H
#define __MAC_PROT_DEFS_H

#include "palTypes.h"
#include "sirTypes.h"
#include "wniCfgSta.h"
#include "aniCompiler.h"


//                               
#define CAPABILITY_INFO_DELAYED_BA_BIT 14
#define CAPABILITY_INFO_IMMEDIATE_BA_BIT 15

//                  
#define SIR_11B_CHANNEL_ID_BEGIN           1
#define SIR_TOTAL_NUM_11B_CHANNELS         14
#define SIR_11B_DEFAULT_MAX_TRANSMIT_POWER 20
#define SIR_11A_CHANNEL_ID_BEGIN           240
#define SIR_TOTAL_NUM_11A_CHANNELS         34
#define SIR_11A_DEFAULT_MAX_TRANSMIT_POWER 16

//                  
#define SIR_11A_CHANNEL_BEGIN           34
#define SIR_11A_CHANNEL_END             165
#define SIR_11B_CHANNEL_BEGIN           1
#define SIR_11B_CHANNEL_END             14
#define SIR_11A_FREQUENCY_OFFSET        4
#define SIR_11B_FREQUENCY_OFFSET        1


#define PRIM_DEVICE_LEN                 (8)

//                       
#define SIR_MAC_MAX_FRAG_SIZE  2398

//                        
#define SIR_MAC_MAX_PDU_SIZE   2346

//                                                                   
#define SIR_MAC_FCS_LENGTH 4

//                        
#define SIR_MAC_ACK_SIZE        14
#define SIR_MAC_CTS_SIZE        14
#define SIR_MAC_RTS_SIZE        20
#define SIR_MAC_BRQ_SIZE        24
#define SIR_MAC_BACK_SIZE       26

//                           
#define SIR_MAC_PROTOCOL_VERSION 0

//                       

#define SIR_MAC_MGMT_FRAME    0x0
#define SIR_MAC_CTRL_FRAME    0x1
#define SIR_MAC_DATA_FRAME    0x2

#define SIR_MAC_FRAME_TYPE_START   0x0
#define SIR_MAC_FRAME_TYPE_END     0x3

//                                  

#define SIR_MAC_CTRL_RR         4
#define SIR_MAC_CTRL_BAR        8
#define SIR_MAC_CTRL_BA         9
#define SIR_MAC_CTRL_PS_POLL    10
#define SIR_MAC_CTRL_RTS        11
#define SIR_MAC_CTRL_CTS        12
#define SIR_MAC_CTRL_ACK        13
#define SIR_MAC_CTRL_CF_END     14
#define SIR_MAC_CTRL_CF_END_ACK 15

#define GEN4_SCAN         1
#ifdef GEN4_SCAN
#define SIR_MAC_MAX_DURATION_MICRO_SECONDS       32767
#endif //          

//                               
#define SIR_MAC_DATA_DATA                 0
#define SIR_MAC_DATA_DATA_ACK             1
#define SIR_MAC_DATA_DATA_POLL            2
#define SIR_MAC_DATA_DATA_ACK_POLL        3
#define SIR_MAC_DATA_NULL                 4
#define SIR_MAC_DATA_NULL_ACK             5
#define SIR_MAC_DATA_NULL_POLL            6
#define SIR_MAC_DATA_NULL_ACK_POLL        7
#define SIR_MAC_DATA_QOS_DATA             8
#define SIR_MAC_DATA_QOS_DATA_ACK         9
#define SIR_MAC_DATA_QOS_DATA_POLL        10
#define SIR_MAC_DATA_QOS_DATA_ACK_POLL    11
#define SIR_MAC_DATA_QOS_NULL             12
#define SIR_MAC_DATA_QOS_NULL_ACK         13
#define SIR_MAC_DATA_QOS_NULL_POLL        14
#define SIR_MAC_DATA_QOS_NULL_ACK_POLL    15

#define SIR_MAC_FRAME_SUBTYPE_START       0
#define SIR_MAC_FRAME_SUBTYPE_END         16

#define SIR_MAC_DATA_QOS_MASK             8
#define SIR_MAC_DATA_NULL_MASK            4
#define SIR_MAC_DATA_POLL_MASK            2
#define SIR_MAC_DATA_ACK_MASK             1

//                                     

#define SIR_MAC_MGMT_ASSOC_REQ    0x0
#define SIR_MAC_MGMT_ASSOC_RSP    0x1
#define SIR_MAC_MGMT_REASSOC_REQ  0x2
#define SIR_MAC_MGMT_REASSOC_RSP  0x3
#define SIR_MAC_MGMT_PROBE_REQ    0x4
#define SIR_MAC_MGMT_PROBE_RSP    0x5
#define SIR_MAC_MGMT_BEACON       0x8
#define SIR_MAC_MGMT_ATIM         0x9
#define SIR_MAC_MGMT_DISASSOC     0xA
#define SIR_MAC_MGMT_AUTH         0xB
#define SIR_MAC_MGMT_DEAUTH       0xC
#define SIR_MAC_MGMT_ACTION       0xD
#define SIR_MAC_MGMT_RESERVED15   0xF

//                        

#define SIR_MAC_ACTION_SPECTRUM_MGMT   0
#define SIR_MAC_ACTION_QOS_MGMT        1
#define SIR_MAC_ACTION_DLP             2
#define SIR_MAC_ACTION_BLKACK          3
#define SIR_MAC_ACTION_PUBLIC_USAGE    4
#if defined WLAN_FEATURE_VOWIFI
#define SIR_MAC_ACTION_RRM             5
#endif
#define SIR_MAC_ACTION_HT              7
#ifdef FEATURE_WLAN_TDLS
#define SIR_MAC_ACTION_TDLS           12
#endif
#define SIR_MAC_ACTION_WME            17
#ifdef WLAN_FEATURE_11AC
#define SIR_MAC_ACTION_VHT            21
#endif

//                            

#define SIR_MAC_QOS_ADD_TS_REQ      0
#define SIR_MAC_QOS_ADD_TS_RSP      1
#define SIR_MAC_QOS_DEL_TS_REQ      2
#define SIR_MAC_QOS_SCHEDULE        3
//                          
#define SIR_MAC_QOS_DEF_BA_REQ      4
#define SIR_MAC_QOS_DEF_BA_RSP      5
#define SIR_MAC_QOS_DEL_BA_REQ      6
#define SIR_MAC_QOS_DEL_BA_RSP      7

#ifdef ANI_SUPPORT_11H
//                                 
#define SIR_MAC_ACTION_MEASURE_REQUEST_ID      0
#define SIR_MAC_ACTION_MEASURE_REPORT_ID       1
#define SIR_MAC_ACTION_TPC_REQUEST_ID          2
#define SIR_MAC_ACTION_TPC_REPORT_ID           3
#endif //               
#define SIR_MAC_ACTION_CHANNEL_SWITCH_ID       4


#ifdef ANI_SUPPORT_11H
//                                
#define SIR_MAC_BASIC_MEASUREMENT_TYPE         0
#define SIR_MAC_CCA_MEASUREMENT_TYPE           1
#define SIR_MAC_RPI_MEASUREMENT_TYPE           2
#endif //               

//            
//                                                         
#if defined WLAN_FEATURE_VOWIFI

#define SIR_MAC_RRM_CHANNEL_LOAD_TYPE          3
#define SIR_MAC_RRM_NOISE_HISTOGRAM_BEACON     4
#define SIR_MAC_RRM_BEACON_TYPE                5
#define SIR_MAC_RRM_FRAME_TYPE                 6
#define SIR_MAC_RRM_STA_STATISTICS_TYPE        7
#define SIR_MAC_RRM_LCI_TYPE                   8
#define SIR_MAC_RRM_TSM_TYPE                   9

//                
#define SIR_MAC_RRM_RADIO_MEASURE_REQ          0
#define SIR_MAC_RRM_RADIO_MEASURE_RPT          1
#define SIR_MAC_RRM_LINK_MEASUREMENT_REQ       2
#define SIR_MAC_RRM_LINK_MEASUREMENT_RPT       3
#define SIR_MAC_RRM_NEIGHBOR_REQ               4
#define SIR_MAC_RRM_NEIGHBOR_RPT               5

#endif

//                 
#ifdef WLAN_FEATURE_11AC
#define SIR_MAC_VHT_OPMODE_NOTIFICATION        2
#endif

//                      
#define SIR_MAC_SM_POWER_SAVE       1

//                       
#define SIR_MAC_DLP_REQ             0
#define SIR_MAC_DLP_RSP             1
#define SIR_MAC_DLP_TEARDOWN        2

//                                         
#define SIR_MAC_BLKACK_ADD_REQ      0
#define SIR_MAC_BLKACK_ADD_RSP      1
#define SIR_MAC_BLKACK_DEL          2
#define SIR_MAC_ACTION_VENDOR_SPECIFIC 9
#define SIR_MAC_ACTION_VENDOR_SPECIFIC_CATEGORY     0x7F
#define SIR_MAC_ACTION_P2P_SUBTYPE_PRESENCE_RSP     2

#ifdef WLAN_FEATURE_11W
//                                                        
#define SIR_MAC_ACTION_SA_QUERY          8
#define SIR_MAC_SA_QUERY_REQ             0
#define SIR_MAC_SA_QUERY_RSP             1
#endif

#ifdef FEATURE_WLAN_TDLS
#define SIR_MAC_TDLS_SETUP_REQ           0
#define SIR_MAC_TDLS_SETUP_RSP           1
#define SIR_MAC_TDLS_SETUP_CNF           2
#define SIR_MAC_TDLS_TEARDOWN            3
#define SIR_MAC_TDLS_PEER_TRAFFIC_IND    4
#define SIR_MAC_TDLS_CH_SWITCH_REQ       5
#define SIR_MAC_TDLS_CH_SWITCH_RSP       6
#define SIR_MAC_TDLS_PEER_TRAFFIC_RSP    9
#define SIR_MAC_TDLS_DIS_REQ             10
#define SIR_MAC_TDLS_DIS_RSP             14
#endif

#define SIR_MAC_MAX_RANDOM_LENGTH   2306

//                                                                             
//                             
//                          
//                                                                             

#define SIR_MAC_SSID_EID               0
#define SIR_MAC_SSID_EID_MIN               0
#define SIR_MAC_SSID_EID_MAX               32
#define SIR_MAC_RATESET_EID            1
#define SIR_MAC_RATESET_EID_MIN            1
#define SIR_MAC_RATESET_EID_MAX            12
#define SIR_MAC_FH_PARAM_SET_EID       2
#define SIR_MAC_FH_PARAM_SET_EID_MIN       5
#define SIR_MAC_FH_PARAM_SET_EID_MAX       5
#define SIR_MAC_DS_PARAM_SET_EID       3
#define SIR_MAC_DS_PARAM_SET_EID_MIN       1
#define SIR_MAC_DS_PARAM_SET_EID_MAX       1
#define SIR_MAC_CF_PARAM_SET_EID       4
#define SIR_MAC_CF_PARAM_SET_EID_MIN       6
#define SIR_MAC_CF_PARAM_SET_EID_MAX       6
#define SIR_MAC_TIM_EID                5
#define SIR_MAC_TIM_EID_MIN                3
#define SIR_MAC_TIM_EID_MAX                254
#define SIR_MAC_IBSS_PARAM_SET_EID     6
#define SIR_MAC_IBSS_PARAM_SET_EID_MIN     2
#define SIR_MAC_IBSS_PARAM_SET_EID_MAX     2
#define SIR_MAC_COUNTRY_EID            7
#define SIR_MAC_COUNTRY_EID_MIN            6
#define SIR_MAC_COUNTRY_EID_MAX            254
#define SIR_MAC_FH_PARAMS_EID          8
#define SIR_MAC_FH_PARAMS_EID_MIN          4
#define SIR_MAC_FH_PARAMS_EID_MAX          4
#define SIR_MAC_FH_PATTERN_EID         9
#define SIR_MAC_FH_PATTERN_EID_MIN         4
#define SIR_MAC_FH_PATTERN_EID_MAX         254
#define SIR_MAC_REQUEST_EID            10
#define SIR_MAC_REQUEST_EID_MIN            1
#define SIR_MAC_REQUEST_EID_MAX            255
#define SIR_MAC_QBSS_LOAD_EID          11
#define SIR_MAC_QBSS_LOAD_EID_MIN          5
#define SIR_MAC_QBSS_LOAD_EID_MAX          5
#define SIR_MAC_EDCA_PARAM_SET_EID     12 //                   
#define SIR_MAC_EDCA_PARAM_SET_EID_MIN     18
#define SIR_MAC_EDCA_PARAM_SET_EID_MAX     20 //                            
#define SIR_MAC_TSPEC_EID              13
#define SIR_MAC_TSPEC_EID_MIN              55
#define SIR_MAC_TSPEC_EID_MAX              55
#define SIR_MAC_TCLAS_EID              14
#define SIR_MAC_TCLAS_EID_MIN              4
#define SIR_MAC_TCLAS_EID_MAX              255
#define SIR_MAC_QOS_SCHEDULE_EID       15
#define SIR_MAC_QOS_SCHEDULE_EID_MIN       14
#define SIR_MAC_QOS_SCHEDULE_EID_MAX       14
#define SIR_MAC_CHALLENGE_TEXT_EID     16
#define SIR_MAC_CHALLENGE_TEXT_EID_MIN     1
#define SIR_MAC_CHALLENGE_TEXT_EID_MAX     253
//                     
#define SIR_MAC_PWR_CONSTRAINT_EID     32
#define SIR_MAC_PWR_CONSTRAINT_EID_MIN     1
#define SIR_MAC_PWR_CONSTRAINT_EID_MAX     1
#define SIR_MAC_PWR_CAPABILITY_EID     33
#define SIR_MAC_PWR_CAPABILITY_EID_MIN     2
#define SIR_MAC_PWR_CAPABILITY_EID_MAX     2
#define SIR_MAC_TPC_REQ_EID            34
#define SIR_MAC_TPC_REQ_EID_MIN            0
#define SIR_MAC_TPC_REQ_EID_MAX            255
//                               
#define SIR_MAC_TPC_RPT_EID            35
#define SIR_MAC_TPC_RPT_EID_MIN            2
#define SIR_MAC_TPC_RPT_EID_MAX            2
#define SIR_MAC_SPRTD_CHNLS_EID        36
#define SIR_MAC_SPRTD_CHNLS_EID_MIN        2
#define SIR_MAC_SPRTD_CHNLS_EID_MAX        254
#define SIR_MAC_CHNL_SWITCH_ANN_EID    37
#define SIR_MAC_CHNL_SWITCH_ANN_EID_MIN    3
#define SIR_MAC_CHNL_SWITCH_ANN_EID_MAX    3
#define SIR_MAC_MEAS_REQ_EID           38
#define SIR_MAC_MEAS_REQ_EID_MIN           3
#define SIR_MAC_MEAS_REQ_EID_MAX           255
#define SIR_MAC_MEAS_RPT_EID           39
#define SIR_MAC_MEAS_RPT_EID_MIN           3
#define SIR_MAC_MEAS_RPT_EID_MAX           255
#define SIR_MAC_QUIET_EID              40
#define SIR_MAC_QUIET_EID_MIN              6
#define SIR_MAC_QUIET_EID_MAX              6
#define SIR_MAC_IBSS_DFS_EID           41
#define SIR_MAC_IBSS_DFS_EID_MIN           7
#define SIR_MAC_IBSS_DFS_EID_MAX           255
#define SIR_MAC_ERP_INFO_EID           42
#define SIR_MAC_ERP_INFO_EID_MIN           0
#define SIR_MAC_ERP_INFO_EID_MAX           255
#define SIR_MAC_TS_DELAY_EID           43
#define SIR_MAC_TS_DELAY_EID_MIN           4
#define SIR_MAC_TS_DELAY_EID_MAX           4
#define SIR_MAC_TCLAS_PROC_EID         44
#define SIR_MAC_TCLAS_PROC_EID_MIN         1
#define SIR_MAC_TCLAS_PROC_EID_MAX         1
#define SIR_MAC_QOS_CAPABILITY_EID     46
#define SIR_MAC_QOS_CAPABILITY_EID_MIN     1
#define SIR_MAC_QOS_CAPABILITY_EID_MAX     1
#define SIR_MAC_RSN_EID                48
#define SIR_MAC_RSN_EID_MIN                4
#define SIR_MAC_RSN_EID_MAX                254

//                                             
//                                         
#define SIR_MAC_QOS_ACTION_EID         49
#define SIR_MAC_QOS_ACTION_EID_MIN         4
#define SIR_MAC_QOS_ACTION_EID_MAX         255
#define SIR_MAC_EXTENDED_RATE_EID      50
#define SIR_MAC_EXTENDED_RATE_EID_MIN      0
#define SIR_MAC_EXTENDED_RATE_EID_MAX      255
//                     
#define SIR_MAC_RM_ENABLED_CAPABILITY_EID      70
#define SIR_MAC_RM_ENABLED_CAPABILITY_EID_MIN  5
#define SIR_MAC_RM_ENABLED_CAPABILITY_EID_MAX  5
//                      
#define SIR_MAC_WPA_EID                221
#define SIR_MAC_WPA_EID_MIN                0
#define SIR_MAC_WPA_EID_MAX                255

#define SIR_MAC_EID_VENDOR                221

//                                            
#define SIR_MAC_HT_CAPABILITIES_EID    45
#define SIR_MAC_HT_CAPABILITIES_EID_MIN    0
#define SIR_MAC_HT_CAPABILITIES_EID_MAX    255
#define SIR_MAC_HT_INFO_EID      61
#define SIR_MAC_HT_INFO_EID_MIN    0
#define SIR_MAC_HT_INFO_EID_MAX    255

#ifdef WLAN_FEATURE_11AC
#define SIR_MAC_VHT_CAPABILITIES_EID   191
#define SIR_MAC_VHT_OPERATION_EID      192
#define SIR_MAC_VHT_EXT_BSS_LOAD_EID   193
#define SIR_MAC_VHT_OPMODE_EID         199
#endif
#define SIR_MAC_MAX_SUPPORTED_MCS_SET    16

//                                                        
#define SIR_MAC_ANI_WORKAROUND_EID     255
#define SIR_MAC_ANI_WORKAROUND_EID_MIN     0
#define SIR_MAC_ANI_WORKAROUND_EID_MAX     255

//                           
#define SIR_MAC_MAX_IE_LENGTH       255

//                           
#define SIR_MAC_RSN_IE_MAX_LENGTH   255
#define SIR_MAC_WPA_IE_MAX_LENGTH   255
//                           
#define SIR_MAC_RSN_IE_MIN_LENGTH   2
#define SIR_MAC_WPA_IE_MIN_LENGTH   6

#ifdef FEATURE_WLAN_CCX
#define CCX_VERSION_4               4
#define CCX_VERSION_SUPPORTED       CCX_VERSION_4

//                                         
//                         
//                        
#define RM_STATE_NORMAL             1 
#endif

#define SIR_MAC_OUI_VERSION_1         1

//                                                         
#define SIR_MAC_WPA_OUI             0x01F25000
#define SIR_MAC_WME_OUI             0x02F25000
#define SIR_MAC_WSM_OUI             SIR_MAC_WME_OUI
#define SIR_MAC_WSC_OUI             "\x00\x50\xf2\x04"
#define SIR_MAC_WSC_OUI_SIZE        4
#define SIR_MAC_P2P_OUI             "\x50\x6f\x9a\x09"
#define SIR_MAC_P2P_OUI_SIZE        4
#define SIR_P2P_NOA_ATTR            12
#define SIR_MAX_NOA_ATTR_LEN        31
#define SIR_MAX_NOA_DESCR           2
#define SIR_P2P_IE_HEADER_LEN       6

#define SIR_MAC_CISCO_OUI "\x00\x40\x96"
#define SIR_MAC_CISCO_OUI_SIZE 3

//                                                              
#define SIR_MAC_OUI_WME_HDR_MIN       6

//                              
#define SIR_MAC_OUI_SUBTYPE_WME_INFO  0
#define SIR_MAC_OUI_WME_INFO_MIN      7
#define SIR_MAC_OUI_WME_INFO_MAX      7

#define SIR_MAC_OUI_SUBTYPE_WME_PARAM 1
#define SIR_MAC_OUI_WME_PARAM_MIN     24
#define SIR_MAC_OUI_WME_PARAM_MAX     24

#define SIR_MAC_OUI_SUBTYPE_WME_TSPEC 2
#define SIR_MAC_OUI_WME_TSPEC_MIN     61
#define SIR_MAC_OUI_WME_TSPEC_MAX     61

#define SIR_MAC_OUI_SUBTYPE_WSM_TSPEC 2   //                  
#define SIR_MAC_OUI_WSM_TSPEC_MIN     61
#define SIR_MAC_OUI_WSM_TSPEC_MAX     61

//                                             
//               
#define SIR_MAC_OUI_SUBTYPE_WSM_CAPABLE     5
#define SIR_MAC_OUI_WSM_CAPABLE_MIN         7
#define SIR_MAC_OUI_WSM_CAPABLE_MAX         7
//               
#define SIR_MAC_OUI_SUBTYPE_WSM_TCLAS       6
#define SIR_MAC_OUI_WSM_TCLAS_MIN           10
#define SIR_MAC_OUI_WSM_TCLAS_MAX           255
//                              
#define SIR_MAC_OUI_SUBTYPE_WSM_TCLASPROC   7
#define SIR_MAC_OUI_WSM_TCLASPROC_MIN       7
#define SIR_MAC_OUI_WSM_TCLASPROC_MAX       7
//                    
#define SIR_MAC_OUI_SUBTYPE_WSM_TSDELAY     8
#define SIR_MAC_OUI_WSM_TSDELAY_MIN         10
#define SIR_MAC_OUI_WSM_TSDELAY_MAX         10
//                 
#define SIR_MAC_OUI_SUBTYPE_WSM_SCHEDULE    9
#define SIR_MAC_OUI_WSM_SCHEDULE_MIN        20
#define SIR_MAC_OUI_WSM_SCHEDULE_MAX        20

#ifdef WLAN_NS_OFFLOAD
#define SIR_MAC_NS_OFFLOAD_SIZE             1  //                             
#define SIR_MAC_NUM_TARGET_IPV6_NS_OFFLOAD_NA   2 //                                                       
#define SIR_MAC_IPV6_ADDR_LEN               16
#define SIR_IPV6_ADDR_VALID                 1
#endif //               
#define SIR_MAC_ARP_OFFLOAD_SIZE        1

//                                                     
#define EID_LEN(eid) (2 + (eid))

//                                         
#define CHANNEL_SWITCH_MAX_FRAME_SIZE               256


//                                         
#define SIR_SME_CHANNEL_SWITCH_SIZE        (sizeof(tANI_U8) + 2 *sizeof(tANI_U16) + sizeof(tANI_U32) + sizeof(ePhyChanBondState))
#define SIR_CHANNEL_SWITCH_IE_SIZE         EID_LEN(SIR_MAC_CHNL_SWITCH_ANN_EID_MIN)

//                                   
#define SIR_MEAS_REQ_FIELD_SIZE                11
#define SIR_MEAS_REQ_IE_SIZE                   (5 + SIR_MEAS_REQ_FIELD_SIZE)
#define SIR_MEAS_REQ_ACTION_FRAME_SIZE         (3 + SIR_MEAS_REQ_IE_SIZE)
#define SIR_MEAS_MAX_FRAME_SIZE                256
#define SIR_MEAS_REPORT_MIN_FRAME_SIZE         (3 + EID_LEN(SIR_MAC_MEAS_RPT_EID_MIN))

#define SIR_MAC_SET_MEAS_REQ_ENABLE(x)         (((tANI_U8) x) | 2)
#define SIR_MAC_SET_MEAS_REQ_REQUEST(x)        (((tANI_U8) x) | 4)
#define SIR_MAC_SET_MEAS_REQ_REPORT(x)         (((tANI_U8) x) | 8)

#define SIR_MAC_SET_MEAS_REPORT_LATE(x)        (((tANI_U8) x) | 1)
#define SIR_MAC_SET_MEAS_REPORT_INCAPABLE(x)   (((tANI_U8) x) | 2)
#define SIR_MAC_SET_MEAS_REPORT_REFUSE(x)      (((tANI_U8) x) | 4)

//                                   
#define SIR_TPC_REQ_ACTION_FRAME_SIZE          (3 + EID_LEN(SIR_MAC_TPC_REQ_EID_MIN))
#define SIR_TPC_REPORT_ACTION_FRAME_SIZE       (3 + EID_LEN(SIR_MAC_TPC_RPT_EID_MIN))
#define SIR_TPC_MAX_FRAME_SIZE                 256
//                                                                             

//                                                         

//                              
#define SIR_MAC_TS_OFFSET                    0
#define SIR_MAC_BEACON_INT_OFFSET            8    //                       
#define SIR_MAC_B_PR_CAPAB_OFFSET            10
#define SIR_MAC_B_PR_SSID_OFFSET             12

//                                  
#define SIR_MAC_ASSOC_CAPAB_OFFSET           0
#define SIR_MAC_LISTEN_INT_OFFSET            2    //                       
#define SIR_MAC_ASSOC_SSID_OFFSET            4
#define SIR_MAC_CURRENT_AP_OFFSET            4
#define SIR_MAC_REASSOC_SSID_OFFSET          10
#define SIR_MAC_ASSOC_STATUS_CODE_OFFSET     2
#define SIR_MAC_ASSOC_AID_OFFSET             4
#define SIR_MAC_ASSOC_RSP_RATE_OFFSET        6

//                                        
#define SIR_MAC_REASON_CODE_OFFSET           0

//                     
#define SIR_MAC_PROBE_REQ_SSID_OFFSET        0

//                       
#define SIR_MAC_AUTH_ALGO_OFFSET             0
#define SIR_MAC_AUTH_XACT_SEQNUM_OFFSET      2
#define SIR_MAC_AUTH_STATUS_CODE_OFFSET      4
#define SIR_MAC_AUTH_CHALLENGE_OFFSET        6

//                                                                         
#define    SIR_MAC_AUTH_FRAME_1        1
#define    SIR_MAC_AUTH_FRAME_2        2
#define    SIR_MAC_AUTH_FRAME_3        3
#define    SIR_MAC_AUTH_FRAME_4        4

//                                  
#define SIR_MAC_ADDR_LENGTH                  6
#define SIR_MAC_MAX_SSID_LENGTH              32
#define SIR_MAC_MAX_NUMBER_OF_RATES          12
#define SIR_MAC_MAX_NUM_OF_DEFAULT_KEYS      4
#define SIR_MAC_KEY_LENGTH                   13   //                            
#define SIR_MAC_AUTH_CHALLENGE_LENGTH        128
#define SIR_MAC_WEP_IV_LENGTH                4
#define SIR_MAC_WEP_ICV_LENGTH               4

//                                 
#define SIR_MAC_MAX_KEY_LENGTH               32

//                                            
#define SIR_MAC_GET_PROT_VERSION(x)      ((((tANI_U16) x) & 0x0300) >> 8)
#define SIR_MAC_GET_FRAME_TYPE(x)        ((((tANI_U16) x) & 0x0C00) >> 8)
#define SIR_MAC_GET_FRAME_SUB_TYPE(x)    ((((tANI_U16) x) & 0xF000) >> 12)
#define SIR_MAC_GET_WEP_BIT_IN_FC(x)     (((tANI_U16) x) & 0x0040)
#define SIR_MAC_SET_PROT_VERSION(x)      ((tANI_U16) x)
#define SIR_MAC_SET_FRAME_TYPE(x)        (((tANI_U16) x) << 2)
#define SIR_MAC_SET_FRAME_SUB_TYPE(x)    (((tANI_U16) x) << 4)
#define SIR_MAC_SET_WEP_BIT_IN_FC(x)     (((tANI_U16) x) << 14)

//                                                      
#define SIR_MAC_GET_ESS(x)               (((tANI_U16) x) & 0x0001)
#define SIR_MAC_GET_IBSS(x)              ((((tANI_U16) x) & 0x0002) >> 1)
#define SIR_MAC_GET_CF_POLLABLE(x)       ((((tANI_U16) x) & 0x0004) >> 2)
#define SIR_MAC_GET_CF_POLL_REQ(x)       ((((tANI_U16) x) & 0x0008) >> 3)
#define SIR_MAC_GET_PRIVACY(x)           ((((tANI_U16) x) & 0x0010) >> 4)
#define SIR_MAC_GET_SHORT_PREAMBLE(x)    ((((tANI_U16) x) & 0x0020) >> 5)
#define SIR_MAC_GET_SPECTRUM_MGMT(x)     ((((tANI_U16) x) & 0x0100) >> 8)
#define SIR_MAC_GET_QOS(x)               ((((tANI_U16) x) & 0x0200) >> 9)
#define SIR_MAC_GET_SHORT_SLOT_TIME(x)   ((((tANI_U16) x) & 0x0400) >> 10)
#define SIR_MAC_GET_APSD(x)              ((((tANI_U16) x) & 0x0800) >> 11)
#if defined WLAN_FEATURE_VOWIFI
#define SIR_MAC_GET_RRM(x)               ((((tANI_U16) x) & 0x1000) >> 12)
#endif
#define SIR_MAC_GET_BLOCK_ACK(x)         ((((tANI_U16) x) & 0xc000) >> CAPABILITY_INFO_DELAYED_BA_BIT)
#define SIR_MAC_SET_ESS(x)               (((tANI_U16) x) | 0x0001)
#define SIR_MAC_SET_IBSS(x)              (((tANI_U16) x) | 0x0002)
#define SIR_MAC_SET_CF_POLLABLE(x)       (((tANI_U16) x) | 0x0004)
#define SIR_MAC_SET_CF_POLL_REQ(x)       (((tANI_U16) x) | 0x0008)
#define SIR_MAC_SET_PRIVACY(x)           (((tANI_U16) x) | 0x0010)
#define SIR_MAC_SET_SHORT_PREAMBLE(x)    (((tANI_U16) x) | 0x0020)
#define SIR_MAC_SET_SPECTRUM_MGMT(x)     (((tANI_U16) x) | 0x0100)
#define SIR_MAC_SET_QOS(x)               (((tANI_U16) x) | 0x0200)
#define SIR_MAC_SET_SHORT_SLOT_TIME(x)   (((tANI_U16) x) | 0x0400)
#define SIR_MAC_SET_APSD(x)              (((tANI_U16) x) | 0x0800)
#if defined WLAN_FEATURE_VOWIFI
#define SIR_MAC_SET_RRM(x)               (((tANI_U16) x) | 0x1000) 
#endif
#define SIR_MAC_SET_GROUP_ACK(x)         (((tANI_U16) x) | 0x4000)

//                                                            
#define SIR_MAC_CLEAR_CAPABILITY(u16value, bitname) \
  ((u16value) &= (~(SIR_MAC_SET_##bitname(0))))

#define IS_WES_MODE_ENABLED(x) \
                    ((x)->roam.configParam.isWESModeEnabled)

#define BA_RECIPIENT       1
#define BA_INITIATOR       2
#define BA_BOTH_DIRECTIONS 3

//                                                          

typedef enum eSirMacStatusCodes
{
    eSIR_MAC_SUCCESS_STATUS                       = 0, //        
    eSIR_MAC_UNSPEC_FAILURE_STATUS                = 1, //                  
    //                                                 
    /*
                                          
                                         
                     
                          
                          
                
               
                    
    */
    eSIR_MAC_WME_INVALID_PARAMS_STATUS            = 1, //   
    eSIR_MAC_WME_REFUSED_STATUS                   = 3, //   
    eSIR_MAC_CAPABILITIES_NOT_SUPPORTED_STATUS    = 10, //                                                                             
    eSIR_MAC_INABLITY_TO_CONFIRM_ASSOC_STATUS     = 11, //                                                                        
    eSIR_MAC_OUTSIDE_SCOPE_OF_SPEC_STATUS         = 12, //                                                                   
    eSIR_MAC_AUTH_ALGO_NOT_SUPPORTED_STATUS       = 13, //                                                                          
    eSIR_MAC_AUTH_FRAME_OUT_OF_SEQ_STATUS         = 14, //                                                                                
                                                        //                        
    eSIR_MAC_CHALLENGE_FAILURE_STATUS             = 15, //                                                    
    eSIR_MAC_AUTH_RSP_TIMEOUT_STATUS              = 16, //                                                                          
    eSIR_MAC_MAX_ASSOC_STA_REACHED_STATUS         = 17, //                                                                                
    eSIR_MAC_BASIC_RATES_NOT_SUPPORTED_STATUS     = 18, //                                                                                        
                                                        //                         
    eSIR_MAC_SHORT_PREAMBLE_NOT_SUPPORTED_STATUS  = 19, //                                                                              
                                                        //      
    eSIR_MAC_PBCC_NOT_SUPPORTED_STATUS            = 20, //                                                                               
                                                        //      
    eSIR_MAC_CHANNEL_AGILITY_NOT_SUPPORTED_STATUS = 21, //                                                                               
                                                        //      
    eSIR_MAC_SPECTRUM_MGMT_REQD_STATUS            = 22, //                                                                               
    eSIR_MAC_PWR_CAPABILITY_BAD_STATUS            = 23, //                                                                            
                                                        //                       
    eSIR_MAC_SPRTD_CHANNELS_BAD_STATUS            = 24, //                                                                              
                                                        //                       
    eSIR_MAC_SHORT_SLOT_NOT_SUPORTED_STATUS       = 25, //                                                                               
                                                        //      
    eSIR_MAC_DSSS_OFDM_NOT_SUPPORTED_STATUS       = 26, //                                                                                
    //                                                   
    eSIR_MAC_QOS_UNSPECIFIED_FAILURE_STATUS       = 32, //                                
    eSIR_MAC_QAP_NO_BANDWIDTH_STATUS              = 33, //                                                                              
                                                        //       
    eSIR_MAC_XS_FRAME_LOSS_STATUS                 = 34, //                                                                                   
                                                        //                      
    eSIR_MAC_STA_QOS_NOT_SUPPORTED_STATUS         = 35, //                                                                                 
                                                        //            
    eSIR_MAC_STA_BLK_ACK_NOT_SUPPORTED_STATUS     = 36, //        
    eSIR_MAC_REQ_DECLINED_STATUS                  = 37, //                             
    eSIR_MAC_INVALID_PARAM_STATUS                 = 38, //                                                                                 
    eSIR_MAC_TS_NOT_HONOURED_STATUS               = 39, //                                                                                       
                                                        //                                                                          
                                                        //                                       
    eSIR_MAC_INVALID_INFORMATION_ELEMENT_STATUS   = 40, //                                                                                      
                                                        //                                                              
    eSIR_MAC_INVALID_GROUP_CIPHER_STATUS          = 41, //                    
    eSIR_MAC_INVALID_PAIRWISE_CIPHER_STATUS       = 42, //                       
    eSIR_MAC_INVALID_AKMP_STATUS                  = 43, //            
    eSIR_MAC_UNSUPPORTED_RSN_IE_VERSION_STATUS    = 44, //                                           
    eSIR_MAC_INVALID_RSN_IE_CAPABILITIES_STATUS   = 45, //                                            
    eSIR_MAC_CIPHER_SUITE_REJECTED_STATUS         = 46, //                                                
    eSIR_MAC_TS_NOT_CREATED_STATUS                = 47, //                                                                                
                                                        //                                                                       
    eSIR_MAC_DL_NOT_ALLOWED_STATUS                = 48, //                                               
    eSIR_MAC_DEST_STA_NOT_KNOWN_STATUS            = 49, //                                                  
    eSIR_MAC_DEST_STA_NOT_QSTA_STATUS             = 50, //                                    
    eSIR_MAC_INVALID_LISTEN_INTERVAL_STATUS       = 51, //                                                          

    eSIR_MAC_DSSS_CCK_RATE_MUST_SUPPORT_STATUS    = 52, //       
    eSIR_MAC_DSSS_CCK_RATE_NOT_SUPPORT_STATUS     = 53,
    eSIR_MAC_PSMP_CONTROLLED_ACCESS_ONLY_STATUS   = 54,
#ifdef FEATURE_WLAN_CCX    
    eSIR_MAC_CCX_UNSPECIFIED_QOS_FAILURE_STATUS   = 200, //                                                                 
    eSIR_MAC_CCX_TSPEC_REQ_REFUSED_STATUS         = 201, //                                                                                       
    eSIR_MAC_CCX_ASSOC_DENIED_INSUFF_BW_STATUS    = 202, //                                                                                 
    eSIR_MAC_CCX_INVALID_PARAMETERS_STATUS        = 203, //                                                                                
                                                         //               
#endif

} tSirMacStatusCodes;

/* 
                                                          
                          
 */
typedef enum eSirMacReasonCodes
{
    eSIR_MAC_UNSPEC_FAILURE_REASON                   = 1, //                  
    eSIR_MAC_PREV_AUTH_NOT_VALID_REASON              = 2, //                                       
    eSIR_MAC_DEAUTH_LEAVING_BSS_REASON               = 3, //                                                                            
    eSIR_MAC_DISASSOC_DUE_TO_INACTIVITY_REASON       = 4, //                               
    eSIR_MAC_DISASSOC_DUE_TO_DISABILITY_REASON       = 5, //                                                                              
    eSIR_MAC_CLASS2_FRAME_FROM_NON_AUTH_STA_REASON   = 6, //                                                    
    eSIR_MAC_CLASS3_FRAME_FROM_NON_ASSOC_STA_REASON  = 7, //                                                 
    eSIR_MAC_DISASSOC_LEAVING_BSS_REASON             = 8, //                                                                  
    eSIR_MAC_STA_NOT_PRE_AUTHENTICATED_REASON        = 9, //                                                                               
    eSIR_MAC_PWR_CAPABILITY_BAD_REASON               = 10, //                                                                                     
    eSIR_MAC_SPRTD_CHANNELS_BAD_REASON               = 11, //                                                                                       
    //                                                   
    eSIR_MAC_INVALID_IE_REASON                       = 13, //                                                                                      
                                                           //                                                              
    eSIR_MAC_MIC_FAILURE_REASON                      = 14, //                                    
    eSIR_MAC_4WAY_HANDSHAKE_TIMEOUT_REASON           = 15, //                       
    eSIR_MAC_GR_KEY_UPDATE_TIMEOUT_REASON            = 16, //                           
    eSIR_MAC_RSN_IE_MISMATCH_REASON                  = 17, //                                                                                   
                                                           //                     
    eSIR_MAC_INVALID_MC_CIPHER_REASON                = 18, //                    
    eSIR_MAC_INVALID_UC_CIPHER_REASON                = 19, //                       
    eSIR_MAC_INVALID_AKMP_REASON                     = 20, //            
    eSIR_MAC_UNSUPPORTED_RSN_IE_VER_REASON           = 21, //                                           
    eSIR_MAC_INVALID_RSN_CAPABILITIES_REASON         = 22, //                                            
    eSIR_MAC_1X_AUTH_FAILURE_REASON                  = 23, //                                 
    eSIR_MAC_CIPHER_SUITE_REJECTED_REASON            = 24, //                                                    
#ifdef FEATURE_WLAN_TDLS
    eSIR_MAC_TDLS_TEARDOWN_PEER_UNREACHABLE          = 25, //                                                                                   
    eSIR_MAC_TDLS_TEARDOWN_UNSPEC_REASON             = 26, //                                                
#endif
    //                                                        
#ifdef WLAN_FEATURE_11W
    eSIR_MAC_ROBUST_MGMT_FRAMES_POLICY_VIOLATION     = 31, //                                         
#endif
    eSIR_MAC_QOS_UNSPECIFIED_REASON                  = 32, //                                                 
    eSIR_MAC_QAP_NO_BANDWIDTH_REASON                 = 33, //                                                                        
    eSIR_MAC_XS_UNACKED_FRAMES_REASON                = 34, //                                                                                     
                                                           //                                                                   
    eSIR_MAC_BAD_TXOP_USE_REASON                     = 35, //                                                                         
    eSIR_MAC_PEER_STA_REQ_LEAVING_BSS_REASON         = 36, //                                                                    
    eSIR_MAC_PEER_REJECT_MECHANISIM_REASON           = 37, //                                                                
    eSIR_MAC_MECHANISM_NOT_SETUP_REASON              = 38, //                                                                                  
                                                           //                 
    eSIR_MAC_PEER_TIMEDOUT_REASON                    = 39, //                                      
    eSIR_MAC_CIPHER_NOT_SUPPORTED_REASON             = 45,  //                                                    
    eSIR_MAC_DISASSOC_DUE_TO_FTHANDOFF_REASON        = 46, //         
    //                                                            
    eSIR_BEACON_MISSED                               = 65534, //                                           
} tSirMacReasonCodes;


typedef enum eSirMacChannelType
{
    eSIR_MAC_11A_BAND,
    eSIR_MAC_11B_BAND,
    eSIR_MAC_UNKNOWN_BAND
} tSirMacChannelType;

//                           
typedef enum eBADirection
{
  eBA_RECIPIENT,
  eBA_INITIATOR
} tBADirection;

//                                            
typedef enum eBAEnable
{
  eBA_DISABLE,
  eBA_ENABLE
} tBAEnable;

//                 
typedef enum eBAPolicy
{
  eBA_UNCOMPRESSED,
  eBA_COMPRESSED
} tBAPolicy;

//                 
typedef enum eBAPolicyType
{
  eBA_POLICY_DELAYED,
  eBA_POLICY_IMMEDIATE
} tBAPolicyType;

#ifdef WLAN_FEATURE_VOWIFI
/*                                        */
typedef enum eRrmNeighborReachability
{
    eREACHABILITY_RESERVED,
    eREACHABILITY_NOT_REACHABLE,
    eREACHABILITY_UNKNOWN,
    eREACHABILITY_REACHABLE,
} tRrmNeighborReachability;
#endif /*                     */

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

#ifndef ANI_LITTLE_BIT_ENDIAN

    tANI_U8 seqNumLo : 4;
    tANI_U8 fragNum : 4;

    tANI_U8 seqNumHi : 8;

#else

    tANI_U8 fragNum : 4;
    tANI_U8 seqNumLo : 4;
    tANI_U8 seqNumHi : 8;

#endif
} __ani_attr_packed tSirMacSeqCtl, *tpSirMacSeqCtl;

//             

typedef enum eSirMacAckPolicy
{
    eSIR_MAC_IMMEDIATE_ACK=0 ,
    eSIR_MAC_NO_ACK,
    eSIR_MAC_NO_EXPLICIT_ACK,
    eSIR_MAC_BURST_ACK
} tSirMacAckPolicy;

//                   
typedef __ani_attr_pre_packed struct sSirMacQosCtl
{

#ifndef ANI_LITTLE_BIT_ENDIAN

    tANI_U8 rsvd : 1;
    tANI_U8 ackPolicy : 2;
    tANI_U8 esop_txopUnit : 1;
    tANI_U8 tid : 4;

    tANI_U8 txop : 8;

#else

    tANI_U8 tid : 4;
    tANI_U8 esop_txopUnit : 1;
    tANI_U8 ackPolicy : 2;
    tANI_U8 rsvd : 1;

    tANI_U8 txop : 8;

#endif
} __ani_attr_packed tSirMacQosCtl, *tpSirMacQosCtl;

//                                                     
#define SIR_MAC_HDR_LEN_3A    24

//                                                     
#define SIR_MAC_HDR_LEN_4A    30

//                                                              
#define SIR_MAC_QOS_CTL_LEN    2

//                                                
typedef __ani_attr_pre_packed struct sSirMacDot3Hdr
{
    tANI_U8           da[6];
    tANI_U8           sa[6];
    tANI_U16          length;
} __ani_attr_packed tSirMacDot3Hdr, *tpSirMacDot3Hdr;


//                                                
typedef __ani_attr_pre_packed struct sSirMacDataHdr3a
{
    tSirMacFrameCtl fc;
    tANI_U8           durationLo;
    tANI_U8           durationHi;
    tANI_U8           addr1[6];
    tANI_U8           addr2[6];
    tANI_U8           addr3[6];
    tSirMacSeqCtl   seqControl;
    tSirMacQosCtl   qosControl;
} __ani_attr_packed tSirMacDataHdr3a, *tpSirMacDataHdr3a;

//                                                
typedef __ani_attr_pre_packed struct sSirMacDataHdr4a
{
    tSirMacFrameCtl fc;
    tANI_U8           durationLo;
    tANI_U8           durationHi;
    tANI_U8           addr1[6];
    tANI_U8           addr2[6];
    tANI_U8           addr3[6];
    tSirMacSeqCtl   seqControl;
    tANI_U8           addr4[6];
    tSirMacQosCtl   qosControl;
} __ani_attr_packed tSirMacDataHdr4a, *tpSirMacDataHdr4a;

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
typedef __ani_attr_pre_packed struct sSirMacPSpoll
{
    tSirMacFrameCtl fc;
    tANI_U8              aidLo;
    tANI_U8              aidHi;
    tANI_U8              bssId[6];
    tANI_U8              ta[6];
    tANI_U8              fcs[4];
} __ani_attr_packed tSirMacPSpoll, *tpSirMacPSpoll;

//                       
typedef __ani_attr_pre_packed struct sSirMacErpInfo
{
#ifndef ANI_LITTLE_BIT_ENDIAN
    tANI_U8    reserved:5;
    tANI_U8    barkerPreambleMode:1;
    tANI_U8    useProtection:1;
    tANI_U8    nonErpPresent:1;
#else
    tANI_U8    nonErpPresent:1;
    tANI_U8    useProtection:1;
    tANI_U8    barkerPreambleMode:1;
    tANI_U8    reserved:5;
#endif
} __ani_attr_packed tSirMacErpInfo, *tpSirMacErpInfo;

//                              
typedef __ani_attr_pre_packed struct sSirMacCapabilityInfo
{
#ifndef ANI_LITTLE_BIT_ENDIAN
    tANI_U16  immediateBA:1;
    tANI_U16  delayedBA:1;
    tANI_U16  dsssOfdm:1;
    tANI_U16  rrm:1;
    tANI_U16  apsd:1;
    tANI_U16  shortSlotTime:1;
    tANI_U16  qos:1;
    tANI_U16  spectrumMgt:1;
    tANI_U16  channelAgility:1;
    tANI_U16  pbcc:1;
    tANI_U16  shortPreamble:1;
    tANI_U16  privacy:1;
    tANI_U16  cfPollReq:1;
    tANI_U16  cfPollable:1;
    tANI_U16  ibss:1;
    tANI_U16  ess:1;
#else
    tANI_U16  ess:1;
    tANI_U16  ibss:1;
    tANI_U16  cfPollable:1;
    tANI_U16  cfPollReq:1;
    tANI_U16  privacy:1;
    tANI_U16  shortPreamble:1;
    tANI_U16  pbcc:1;
    tANI_U16  channelAgility:1;
    tANI_U16  spectrumMgt:1;
    tANI_U16  qos:1;
    tANI_U16  shortSlotTime:1;
    tANI_U16  apsd:1;
    tANI_U16  rrm:1;
    tANI_U16  dsssOfdm:1;
    tANI_U16  delayedBA:1;
    tANI_U16  immediateBA:1;
#endif
} __ani_attr_packed tSirMacCapabilityInfo, *tpSirMacCapabilityInfo;

typedef __ani_attr_pre_packed struct sSirMacCfParamSet
{
    tANI_U8    cfpCount;
    tANI_U8    cfpPeriod;
    tANI_U16   cfpMaxDuration;
    tANI_U16   cfpDurRemaining;
} __ani_attr_packed tSirMacCfParamSet;

typedef __ani_attr_pre_packed struct sSirMacTim
{
    tANI_U8    dtimCount;
    tANI_U8    dtimPeriod;
    tANI_U8    bitmapControl;
    tANI_U8    bitmapLength;
    tANI_U8    bitmap[251];
} __ani_attr_packed tSirMacTim;

//                                                                  
//                         
//                                        
typedef __ani_attr_pre_packed struct sSirMacRateSet
{
    tANI_U8  numRates;
    tANI_U8  rate[SIR_MAC_RATESET_EID_MAX];
} __ani_attr_packed tSirMacRateSet;


typedef __ani_attr_pre_packed struct sSirMacSSid
{
    tANI_U8        length;
    tANI_U8        ssId[32];
} __ani_attr_packed tSirMacSSid;

typedef __ani_attr_pre_packed struct sSirMacWpaInfo
{
    tANI_U8        length;
    tANI_U8        info[SIR_MAC_MAX_IE_LENGTH];
} __ani_attr_packed tSirMacWpaInfo, *tpSirMacWpaInfo, tSirMacRsnInfo, *tpSirMacRsnInfo;

typedef __ani_attr_pre_packed struct sSirMacFHParamSet
{
    tANI_U16     dwellTime;
    tANI_U8      hopSet;
    tANI_U8      hopPattern;
    tANI_U8      hopIndex;
} tSirMacFHParamSet, *tpSirMacFHParamSet;

typedef __ani_attr_pre_packed struct sSirMacIBSSParams
{
    tANI_U16     atim;
} tSirMacIBSSParams, *tpSirMacIBSSParams;

typedef __ani_attr_pre_packed struct sSirMacRRMEnabledCap
{
#ifndef ANI_LITTLE_BIT_ENDIAN
    tANI_U8                reserved: 6;
    tANI_U8      AntennaInformation: 1;
    tANI_U8       BSSAvailAdmission: 1;
    tANI_U8       BssAvgAccessDelay: 1;
    tANI_U8         RSNIMeasurement: 1;
    tANI_U8         RCPIMeasurement: 1;
    tANI_U8       NeighborTSFOffset: 1;
    tANI_U8 MeasurementPilotEnabled: 1;
    tANI_U8        MeasurementPilot: 3;
    tANI_U8      nonOperatinChanMax: 3;
    tANI_U8        operatingChanMax: 3;
    tANI_U8           RRMMIBEnabled: 1;
    tANI_U8            APChanReport: 1;
    tANI_U8            triggeredTCM: 1;
    tANI_U8           TCMCapability: 1;
    tANI_U8              LCIAzimuth: 1;
    tANI_U8          LCIMeasurement: 1;
    tANI_U8              statistics: 1;
    tANI_U8          NoiseHistogram: 1;
    tANI_U8             ChannelLoad: 1;
    tANI_U8        FrameMeasurement: 1;
    tANI_U8           BeaconRepCond: 1;
    tANI_U8             BeaconTable: 1;
    tANI_U8            BeaconActive: 1;
    tANI_U8           BeaconPassive: 1;
    tANI_U8                repeated: 1;
    tANI_U8                parallel: 1;
    tANI_U8             NeighborRpt: 1;
    tANI_U8         LinkMeasurement: 1;
    tANI_U8                    present;
#else
    tANI_U8                    present;
    tANI_U8         LinkMeasurement: 1;
    tANI_U8             NeighborRpt: 1;
    tANI_U8                parallel: 1;
    tANI_U8                repeated: 1;
    tANI_U8           BeaconPassive: 1;
    tANI_U8            BeaconActive: 1;
    tANI_U8             BeaconTable: 1;
    tANI_U8           BeaconRepCond: 1;
    tANI_U8        FrameMeasurement: 1;
    tANI_U8             ChannelLoad: 1;
    tANI_U8          NoiseHistogram: 1;
    tANI_U8              statistics: 1;
    tANI_U8          LCIMeasurement: 1;
    tANI_U8              LCIAzimuth: 1;
    tANI_U8           TCMCapability: 1;
    tANI_U8            triggeredTCM: 1;
    tANI_U8            APChanReport: 1;
    tANI_U8           RRMMIBEnabled: 1;
    tANI_U8        operatingChanMax: 3;
    tANI_U8      nonOperatinChanMax: 3;
    tANI_U8        MeasurementPilot: 3;
    tANI_U8 MeasurementPilotEnabled: 1;
    tANI_U8       NeighborTSFOffset: 1;
    tANI_U8         RCPIMeasurement: 1;
    tANI_U8         RSNIMeasurement: 1;
    tANI_U8       BssAvgAccessDelay: 1;
    tANI_U8       BSSAvailAdmission: 1;
    tANI_U8      AntennaInformation: 1;
    tANI_U8                reserved: 6;
#endif
} tSirMacRRMEnabledCap, *tpSirMacRRMEnabledCap;


/*                 
                 
                  
 */

#define EDCA_AC_BE 0
#define EDCA_AC_BK 1
#define EDCA_AC_VI 2
#define EDCA_AC_VO 3
#define AC_MGMT_LO 4
#define AC_MGMT_HI 5
#define MAX_NUM_AC 4
#define TOT_NUM_AC (MAX_NUM_AC + 2)   /*                   */

/*
                                        
                                        
                                        
                                        
                                        
                                        
                                        
                                        
*/
//                  
#define SIR_MAC_EDCAACI_BESTEFFORT  (EDCA_AC_BE)
#define SIR_MAC_EDCAACI_BACKGROUND  (EDCA_AC_BK)
#define SIR_MAC_EDCAACI_VIDEO       (EDCA_AC_VI)
#define SIR_MAC_EDCAACI_VOICE       (EDCA_AC_VO)

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
    tSirMacAciAifsn aci;
    tSirMacCW       cw;
    tANI_U16             txoplimit;
} __ani_attr_packed tSirMacEdcaParamRecord;

typedef __ani_attr_pre_packed struct sSirMacQosInfo
{
#ifndef ANI_LITTLE_BIT_ENDIAN
    tANI_U8  uapsd   : 1;
    tANI_U8  txopreq : 1;
    tANI_U8  qreq    : 1;
    tANI_U8  qack    : 1;
    tANI_U8  count   : 4;
#else
    tANI_U8  count   : 4;
    tANI_U8  qack    : 1;
    tANI_U8  qreq    : 1;
    tANI_U8  txopreq : 1;
    tANI_U8  uapsd   : 1;
#endif
} __ani_attr_packed tSirMacQosInfo;


typedef __ani_attr_pre_packed struct sSirMacQosInfoStation
{
#ifdef ANI_LITTLE_BIT_ENDIAN
    tANI_U8 acvo_uapsd:1;
    tANI_U8 acvi_uapsd:1;
    tANI_U8 acbk_uapsd:1;
    tANI_U8 acbe_uapsd:1;
    tANI_U8 qack:1;
    tANI_U8 maxSpLen:2;
    tANI_U8 moreDataAck:1;
#else
    tANI_U8 moreDataAck:1;
    tANI_U8 maxSpLen:2;
    tANI_U8 qack:1;
    tANI_U8 acbe_uapsd:1;
    tANI_U8 acbk_uapsd:1;
    tANI_U8 acvi_uapsd:1;
    tANI_U8 acvo_uapsd:1;
#endif
} __ani_attr_packed  tSirMacQosInfoStation, *tpSirMacQosInfoStation;



typedef __ani_attr_pre_packed struct sSirMacEdcaParamSetIE
{
    tANI_U8                     type;
    tANI_U8                     length;
    tSirMacQosInfo         qosInfo;
    tANI_U8                     rsvd;
    tSirMacEdcaParamRecord acbe; //            
    tSirMacEdcaParamRecord acbk; //           
    tSirMacEdcaParamRecord acvi; //      
    tSirMacEdcaParamRecord acvo; //      
} __ani_attr_packed tSirMacEdcaParamSetIE;

#if 1
typedef __ani_attr_pre_packed struct sSirMacQoSParams
{
    tANI_U8        count;
    tANI_U16       limit;
    tANI_U8        CWmin[8];
    tANI_U8        AIFS[8];
} __ani_attr_packed tSirMacQoSParams;
#endif

typedef __ani_attr_pre_packed struct sSirMacQbssLoadIE
{
    tANI_U8        type;
    tANI_U8        length;
    tANI_U16       staCount;
    tANI_U8        chnlUtil;
    tANI_U16       admitCapacity;
} __ani_attr_packed tSirMacQbssLoadIE;

//                                                     
#define SIR_MAC_DIRECTION_UPLINK    0
#define SIR_MAC_DIRECTION_DNLINK    1
#define SIR_MAC_DIRECTION_DIRECT    2
#define SIR_MAC_DIRECTION_BIDIR     3

//              
//                                   
#define SIR_MAC_ACCESSPOLICY_EDCA   1
#define SIR_MAC_ACCESSPOLICY_HCCA   2
#define SIR_MAC_ACCESSPOLICY_BOTH   3

#define SIR_MAC_HCCA_TSID_MIN       8 //                                  
#define SIR_MAC_TID_MAX            15
#define MAC_BA_TID_MAX              8

typedef __ani_attr_pre_packed struct sSirMacTSInfoTfc
{
#ifndef ANI_LITTLE_BIT_ENDIAN
    tANI_U8        burstSizeDefn : 1;
    tANI_U8        reserved :7;
#else
    tANI_U8        reserved :7;
    tANI_U8        burstSizeDefn : 1;
#endif

#ifndef ANI_LITTLE_BIT_ENDIAN
    tANI_U16       ackPolicy : 2;
    tANI_U16       userPrio : 3;
    tANI_U16       psb : 1;
    tANI_U16       aggregation : 1;
    tANI_U16       accessPolicy : 2;
    tANI_U16       direction : 2;
    tANI_U16       tsid : 4;
    tANI_U16       trafficType : 1;
#else
    tANI_U16       trafficType : 1;
    tANI_U16       tsid : 4;
    tANI_U16       direction : 2;
    tANI_U16       accessPolicy : 2;
    tANI_U16       aggregation : 1;
    tANI_U16       psb : 1;
    tANI_U16       userPrio : 3;
    tANI_U16       ackPolicy : 2;
#endif
} __ani_attr_packed tSirMacTSInfoTfc;

typedef __ani_attr_pre_packed struct sSirMacTSInfoSch
{
#ifndef ANI_LITTLE_BIT_ENDIAN
    tANI_U8        rsvd : 7;
    tANI_U8        schedule : 1;
#else
    tANI_U8        schedule : 1;
    tANI_U8        rsvd : 7;
#endif
} __ani_attr_packed tSirMacTSInfoSch;

typedef __ani_attr_pre_packed struct sSirMacTSInfo
{
    tSirMacTSInfoTfc traffic;
    tSirMacTSInfoSch schedule;
} __ani_attr_packed tSirMacTSInfo;

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
}
__ani_attr_packed tSirMacTspecIE;

//                                                  
#define SIR_MAC_CLSPARAM_LEN 253

//                       
#define SIR_MAC_TCLASTYPE_ETHERNET 0
#define SIR_MAC_TCLASTYPE_TCPUDPIP 1
#define SIR_MAC_TCLASTYPE_8021DQ   2
//                                      

#define SIR_MAC_TCLAS_PARAM_ETHERNET_MIN  14
typedef __ani_attr_pre_packed struct sSirMacTclasParamEthernet
{
    tANI_U8             srcAddr[6];
    tANI_U8             dstAddr[6];
    tANI_U16            type;
}__ani_attr_packed tSirMacTclasParamEthernet;

#define SIR_MAC_TCLAS_PARAM_IPV4_MIN  16
typedef __ani_attr_pre_packed struct sSirMacTclasParamIPv4
{
    tANI_U8             version;
    tANI_U32            srcIpAddr;
    tANI_U32            dstIpAddr;
    tANI_U16            srcPort;
    tANI_U16            dstPort;
    tANI_U8             dscp;
    tANI_U8             protocol;
    tANI_U8             rsvd;
} __ani_attr_packed tSirMacTclasParamIPv4;

#define SIR_MAC_TCLAS_IPV4  4
#define SIR_MAC_TCLAS_IPV6  6

#define SIR_MAC_TCLAS_PARAM_IPV6_MIN  40
typedef __ani_attr_pre_packed struct sSirMacTclasParamIPv6
{
    tANI_U8             version;
    tANI_U8             srcIpAddr[16];
    tANI_U8             dstIpAddr[16];
    tANI_U16            srcPort;
    tANI_U16            dstPort;
    tANI_U8             flowLabel[3];
} __ani_attr_packed tSirMacTclasParamIPv6;

#define SIR_MAC_TCLAS_PARAM_8021DQ_MIN  2
typedef  __ani_attr_pre_packed struct sSirMacTclasParam8021dq
{
    tANI_U16            tag;
} __ani_attr_packed tSirMacTclasParam8021dq;

typedef __ani_attr_pre_packed struct sSirMacTclasIE
{
    tANI_U8             type;
    tANI_U8             length;
    tANI_U8             userPrio;
    tANI_U8             classifierType;
    tANI_U8             classifierMask;
} __ani_attr_packed tSirMacTclasIE;

typedef __ani_attr_pre_packed struct sSirMacTsDelayIE
{
    tANI_U8             type;
    tANI_U8             length;
    tANI_U32            delay;
} __ani_attr_packed tSirMacTsDelayIE;

typedef __ani_attr_pre_packed struct sSirMacTclasProcIE
{
    tANI_U8             type;
    tANI_U8             length;
    tANI_U8             processing;
} __ani_attr_packed tSirMacTclasProcIE;

typedef __ani_attr_pre_packed struct sSirMacScheduleInfo
{
#ifndef ANI_LITTLE_BIT_ENDIAN
    tANI_U16            rsvd : 9;
    tANI_U16            direction : 2;
    tANI_U16            tsid : 4;
    tANI_U16            aggregation : 1;
#else
    tANI_U16            aggregation : 1;
    tANI_U16            tsid : 4;
    tANI_U16            direction : 2;
    tANI_U16            rsvd : 9;
#endif
} __ani_attr_packed tSirMacScheduleInfo;

typedef __ani_attr_pre_packed struct sSirMacScheduleIE
{
    tANI_U8                  type;
    tANI_U8                  length;
    tSirMacScheduleInfo info;
    tANI_U32                 svcStartTime;
    tANI_U32                 svcInterval;
    tANI_U16                 maxSvcDuration;
    tANI_U16                 specInterval;
} __ani_attr_packed tSirMacScheduleIE;

typedef __ani_attr_pre_packed struct sSirMacQosCapabilityIE
{
    tANI_U8                  type;
    tANI_U8                  length;
    tSirMacQosInfo      qosInfo;
} __ani_attr_packed tSirMacQosCapabilityIE;

typedef __ani_attr_pre_packed struct sSirMacQosCapabilityStaIE
{
    tANI_U8                  type;
    tANI_U8                  length;
    tSirMacQosInfoStation    qosInfo;
} __ani_attr_packed tSirMacQosCapabilityStaIE;


#define  ADDTS
typedef __ani_attr_pre_packed struct sSirMacQosActionIE
{
    tANI_U8                  type;
    tANI_U8                  length;
    tANI_U8                  qosAction;
    tANI_U8                  qosBody[1];
} __ani_attr_packed tSirMacQosActionIE;

typedef tANI_U32 tSirMacTimeStamp[2];

typedef tANI_U16 tSirMacBeaconInterval;

typedef tANI_U16 tSirMacListenInterval;

typedef tANI_U8 tSirMacChanNum;

typedef tANI_U8 tSirMacAddr[6];


//               
typedef __ani_attr_pre_packed struct sSirMacIE
{
    tANI_U8    elementID;
    tANI_U8    length;
    tANI_U8    info[1];
} __ani_attr_packed tSirMacIE;

typedef __ani_attr_pre_packed struct sSirMacSSidIE
{
    tANI_U8              type;
    tSirMacSSid     ssId;
} __ani_attr_packed tSirMacSSidIE;

typedef __ani_attr_pre_packed struct sSirMacRateSetIE
{
    tANI_U8              type;
    tSirMacRateSet  supportedRateSet;
} __ani_attr_packed tSirMacRateSetIE;

typedef __ani_attr_pre_packed struct sSirMacDsParamSetIE
{
    tANI_U8             type;
    tANI_U8             length;
    tSirMacChanNum channelNumber;
} __ani_attr_packed tSirMacDsParamSetIE;

typedef __ani_attr_pre_packed struct sSirMacCfParamSetIE
{
    tANI_U8                  type;
    tANI_U8                  length;
    tSirMacCfParamSet   cfParams;
} __ani_attr_packed tSirMacCfParamSetIE;

typedef __ani_attr_pre_packed struct sSirMacChanInfo
{
    tSirMacChanNum firstChanNum;
    tANI_U8             numChannels;
    tANI_S8             maxTxPower;
} __ani_attr_packed tSirMacChanInfo;

typedef __ani_attr_pre_packed struct sSirMacCountry
{
    tANI_U8                 countryString[3];    //                             
    tANI_U8                 numChanInfo;
    tSirMacChanInfo    chanInfo[1];
} __ani_attr_packed tSirMacCountry;

typedef __ani_attr_pre_packed struct sSirMacCountryIE
{
    tANI_U8                type;
    tANI_U8                length;
    tSirMacCountry    country;
} __ani_attr_packed tSirMacCountryIE;

typedef __ani_attr_pre_packed struct sSirMacNonErpPresentIE
{
    tANI_U8                type;
    tANI_U8                length;
    tANI_U8                erp;
} __ani_attr_packed tSirMacNonErpPresentIE;

typedef __ani_attr_pre_packed struct sSirMacWpaIE
{
    tANI_U8                type;
    tSirMacWpaInfo    wpa;
} __ani_attr_packed tSirMacWpaIE;

typedef  struct sSirMacPowerCapabilityIE
{
    tANI_U8        type;
    tANI_U8        length;
    tANI_U8        minTxPower;
    tANI_U8        maxTxPower;
} tSirMacPowerCapabilityIE;

typedef  struct sSirMacSupportedChannelIE
{
    tANI_U8        type;
    tANI_U8        length;
    tANI_U8        supportedChannels[96];
} tSirMacSupportedChannelIE;

typedef  struct sSirMacMeasReqField
{
    tANI_U8        channelNumber;
    tANI_U8        measStartTime[8];
    tANI_U16       measDuration;
} tSirMacMeasReqField, *tpSirMacMeasReqField;

typedef  struct sSirMacMeasReqIE
{
    tANI_U8                     type;
    tANI_U8                     length;
    tANI_U8                     measToken;
    tANI_U8                     measReqMode;
    tANI_U8                     measType;
    tSirMacMeasReqField    measReqField;
} tSirMacMeasReqIE, *tpSirMacMeasReqIE;

//                         
typedef  struct sSirMacBasicReport
{
    tANI_U8        channelNumber;
    tANI_U8        measStartTime[8];
    tANI_U16       measDuration;
    tANI_U8        mapField;
} tSirMacBasicReport, *tpSirMacBasicReport;

typedef  struct sSirMacBasicReportIE
{
    tANI_U8                       type;
    tANI_U8                       length;
    tANI_U8                       measToken;
    tANI_U8                       measReportMode;
    tANI_U8                       measType;
    tSirMacBasicReport       basicReport;
} tSirMacBasicReportIE, *tpSirMacBasicReportIE;

//                       
typedef  struct sSirMacCcaReport
{
    tANI_U8        channelNumber;
    tANI_U8        measStartTime[8];
    tANI_U16       measDuration;
    tANI_U8        ccaBusyFraction;
} tSirMacCcaReport, *tpSirMacCcaReport;

typedef  struct sSirMacCcaReportIE
{
    tANI_U8                       type;
    tANI_U8                       length;
    tANI_U8                       measToken;
    tANI_U8                       measReportMode;
    tANI_U8                       measType;
    tSirMacCcaReport         ccaReport;
} tSirMacCcaReportIE, *tpSirMacCcaReportIE;

//                                 
typedef  struct sSirMacRpiReport
{
    tANI_U8        channelNumber;
    tANI_U8        measStartTime[8];
    tANI_U16       measDuration;
    tANI_U8        rpiDensity[8];
} tSirMacRpiReport, *tpSirMacRpiReport;

typedef  struct sSirMacRpiReportIE
{
    tANI_U8                       type;
    tANI_U8                       length;
    tANI_U8                       measToken;
    tANI_U8                       measReportMode;
    tANI_U8                       measType;
    tSirMacRpiReport     rpiReport;
} tSirMacRpiReportIE, *tpSirMacRpiReportIE;

#define SIR_MAC_MAX_SUPP_RATES            32

#define SIR_MAC_MAX_EXTN_CAP               8

//                      
typedef __ani_attr_pre_packed struct sSirMacVHTCapabilityInfo
{
#ifndef ANI_LITTLE_BIT_ENDIAN
    tANI_U32        reserved1: 2;
    tANI_U32     txAntPattern: 1;
    tANI_U32     rxAntPattern: 1;
    tANI_U32  vhtLinkAdaptCap: 2;
    tANI_U32   maxAMPDULenExp: 3;
    tANI_U32        htcVHTCap: 1;
    tANI_U32        vhtTXOPPS: 1;
    tANI_U32  muBeamformeeCap: 1;
    tANI_U32  muBeamformerCap: 1;
    tANI_U32   numSoundingDim: 3;
    tANI_U32 csnofBeamformerAntSup: 3;
    tANI_U32  suBeamformeeCap: 1;
    tANI_U32  suBeamFormerCap: 1;
    tANI_U32           rxSTBC: 3;
    tANI_U32           txSTBC: 1;
    tANI_U32 shortGI160and80plus80MHz: 1;
    tANI_U32     shortGI80MHz: 1;
    tANI_U32    ldpcCodingCap: 1;
    tANI_U32 supportedChannelWidthSet: 2;
    tANI_U32       maxMPDULen: 2;
#else
    tANI_U32       maxMPDULen: 2;
    tANI_U32 supportedChannelWidthSet: 2;
    tANI_U32    ldpcCodingCap: 1;
    tANI_U32     shortGI80MHz: 1;
    tANI_U32 shortGI160and80plus80MHz: 1;
    tANI_U32           txSTBC: 1;
    tANI_U32           rxSTBC: 3;
    tANI_U32  suBeamFormerCap: 1;
    tANI_U32  suBeamformeeCap: 1;
    tANI_U32 csnofBeamformerAntSup: 3;
    tANI_U32   numSoundingDim: 3;
    tANI_U32  muBeamformerCap: 1;
    tANI_U32  muBeamformeeCap: 1;
    tANI_U32        vhtTXOPPS: 1;
    tANI_U32        htcVHTCap: 1;
    tANI_U32   maxAMPDULenExp: 3;
    tANI_U32  vhtLinkAdaptCap: 2;
    tANI_U32     rxAntPattern: 1;
    tANI_U32     txAntPattern: 1;
    tANI_U32        reserved1: 2;
#endif
} __ani_attr_packed tSirMacVHTCapabilityInfo;

typedef __ani_attr_pre_packed struct sSirMacVHTTxSupDataRateInfo
{
#ifndef ANI_LITTLE_BIT_ENDIAN
    tANI_U16 reserved: 3;
    tANI_U16 txSupDataRate: 13;
#else
    tANI_U16 txSupDataRate: 13;
    tANI_U16 reserved: 3;
#endif
}__ani_attr_packed tSirMacVHTTxSupDataRateInfo;

typedef __ani_attr_pre_packed struct sSirMacVHTRxSupDataRateInfo
{
#ifndef ANI_LITTLE_BIT_ENDIAN
    tANI_U16 reserved: 3;
    tANI_U16 rxSupDataRate: 13;
#else
    tANI_U16 rxSupDataRate: 13;
    tANI_U16 reserved: 3;
#endif
}__ani_attr_packed tSirMacVHTRxSupDataRateInfo;

/* 
                                              
                                                                  
                                                            
                                                           
                                                           
                                                    
                                                                  
                                                            
                                                            
                                                           
                                                    
 */
typedef struct sSirVhtMcsInfo {
    tANI_U16 rxMcsMap;
    tANI_U16 rxHighest;
    tANI_U16 txMcsMap;
    tANI_U16 txHighest;
}tSirVhtMcsInfo;

/* 
                                       
  
                                                      
                                       
                                     
                                     
 */
typedef struct sSirVHtCap {
    tANI_U32       vhtCapInfo;
    tSirVhtMcsInfo suppMcs;
}tSirVHTCap;

/* 
                                     
  
                                                        
                                              
 */


typedef struct sSirHtCap {
    tANI_U16 capInfo;
    tANI_U8  ampduParamsInfo;
    tANI_U8  suppMcsSet[16];
    tANI_U16 extendedHtCapInfo;
    tANI_U32 txBFCapInfo;
    tANI_U8  antennaSelectionInfo;
}tSirHTCap;

//                              
#define HT_CAPABILITY_IE_SIZE                       28
#define HT_INFO_IE_SIZE                                          24

//
//                                                         
//
typedef enum eSirMacHTOperatingMode
{
  eSIR_HT_OP_MODE_PURE, //              
  eSIR_HT_OP_MODE_OVERLAP_LEGACY, //                                                      
  eSIR_HT_OP_MODE_NO_LEGACY_20MHZ_HT, //                                        
  eSIR_HT_OP_MODE_MIXED //                      
} tSirMacHTOperatingMode;


//                                         
typedef enum eSirMacHTMIMOPowerSaveState
{
  eSIR_HT_MIMO_PS_STATIC = 0, //                          
  eSIR_HT_MIMO_PS_DYNAMIC = 1, //                           
  eSIR_HT_MIMO_PS_NA = 2, //         
  eSIR_HT_MIMO_PS_NO_LIMIT = 3 //                       
} tSirMacHTMIMOPowerSaveState;


typedef enum eSirMacHTChannelWidth
{
    eHT_CHANNEL_WIDTH_20MHZ = 0,
    eHT_CHANNEL_WIDTH_40MHZ = 1,
#ifdef WLAN_FEATURE_11AC
    eHT_CHANNEL_WIDTH_80MHZ = 2,
#endif
    eHT_MAX_CHANNEL_WIDTH
} tSirMacHTChannelWidth;

//                               
typedef __ani_attr_pre_packed struct sHtCaps {
    tANI_U16     advCodingCap: 1;
    tANI_U16 supportedChannelWidthSet: 1;
    tANI_U16    mimoPowerSave: 2;
    tANI_U16       greenField: 1;
    tANI_U16     shortGI20MHz: 1;
    tANI_U16     shortGI40MHz: 1;
    tANI_U16           txSTBC: 1;
    tANI_U16           rxSTBC: 2;
    tANI_U16        delayedBA: 1;
    tANI_U16 maximalAMSDUsize: 1;
    tANI_U16 dsssCckMode40MHz: 1;
    tANI_U16             psmp: 1;
    tANI_U16 stbcControlFrame: 1;
    tANI_U16 lsigTXOPProtection: 1;
    tANI_U8 maxRxAMPDUFactor: 2;
    tANI_U8      mpduDensity: 3;
    tANI_U8        reserved1: 3;
    tANI_U8      supportedMCSSet[16];
    tANI_U16              pco: 1;
    tANI_U16   transitionTime: 2;
    tANI_U16        reserved2: 5;
    tANI_U16      mcsFeedback: 2;
    tANI_U16        reserved3: 6;
    tANI_U32             txBF: 1;
    tANI_U32 rxStaggeredSounding: 1;
    tANI_U32 txStaggeredSounding: 1;
    tANI_U32            rxZLF: 1;
    tANI_U32            txZLF: 1;
    tANI_U32     implicitTxBF: 1;
    tANI_U32      calibration: 2;
    tANI_U32  explicitCSITxBF: 1;
    tANI_U32 explicitUncompressedSteeringMatrix: 1;
    tANI_U32 explicitBFCSIFeedback: 3;
    tANI_U32 explicitUncompressedSteeringMatrixFeedback: 3;
    tANI_U32 explicitCompressedSteeringMatrixFeedback: 3;
    tANI_U32 csiNumBFAntennae: 2;
    tANI_U32 uncompressedSteeringMatrixBFAntennae: 2;
    tANI_U32 compressedSteeringMatrixBFAntennae: 2;
    tANI_U32        reserved4: 7;
    tANI_U8 antennaSelection: 1;
    tANI_U8 explicitCSIFeedbackTx: 1;
    tANI_U8 antennaIndicesFeedbackTx: 1;
    tANI_U8 explicitCSIFeedback: 1;
    tANI_U8 antennaIndicesFeedback: 1;
    tANI_U8             rxAS: 1;
    tANI_U8  txSoundingPPDUs: 1;
    tANI_U8        reserved5: 1;

} __ani_attr_packed tHtCaps;

/*                                                      
                                                            
                  
                                                          
                                                         
 */
typedef enum eSirMacChanSwMode 
{
    eSIR_CHANSW_MODE_NORMAL = 0,
    eSIR_CHANSW_MODE_SILENT = 1
} tSirMacChanSwitchMode;


typedef __ani_attr_pre_packed struct _BarControl {

#ifndef ANI_BIG_BYTE_ENDIAN

    tANI_U16    barAckPolicy:1;
    tANI_U16    multiTID:1;
    tANI_U16    bitMap:1;
    tANI_U16    rsvd:9;
    tANI_U16    numTID:4;

#else
    tANI_U16    numTID:4;
    tANI_U16    rsvd:9;
    tANI_U16    bitMap:1;
    tANI_U16    multiTID:1;
    tANI_U16    barAckPolicy:1;

#endif

}__ani_attr_packed barCtrlType;

typedef __ani_attr_pre_packed struct _BARFrmStruct {
    tSirMacFrameCtl   fc;
    tANI_U16          duration;
    tSirMacAddr       rxAddr;
    tSirMacAddr       txAddr;
    barCtrlType       barControl;
    tSirMacSeqCtl     ssnCtrl;
}__ani_attr_packed BARFrmType;


#if 0
//                             
//                                     
#define SUPPORT_ADVANCE_CODING_1                            1
#define NOT_SUPPORT_ADVANCE_CODING_0                        0
#define SUPPORT_CHANNEL_WIDTH_SET_20_40_MHZ                 1
#define NOT_SUPPORT_CHANNEL_WIDTH_SET_20_MHZ_ONLY           0
#define DO_NOT_SEND_MIMO_SEQ_STATIC_POWER_SAVE              0
#define RTS_PRECEDE_MIMO_SEQ_DYN_POWER_SAVE                 1
#define MIMO_POWER_SAVE_NA                                  2
#define NO_LIMIT_ON_MIMO_SEQ                                3
#define DEVICE_IS_ABLE_TO_RCVE_PPDU_WITH_GF_PREAMBLE        1
#define DEVICE_NOT_ABLE_TO_RCVE_PPDU_WITH_GF_PREAMBLE       0
#define SUPPORT_SHORT_GI_FOR_20_MHZ                         1
#define NOT_SUPPORT_SHORT_GI_FOR_20_MHZ                     0
#define SUPPORT_SHORT_GI_FOR_40_MHZ                         1
#define NOT_SUPPORT_SHORT_GI_FOR_40_MHZ                     0
#define TRANSMITTER_SUPPORT_STBC                            1
#define TRANSMITTER_NOT_SUPPORT_STBC                        0
#define RX_NOT_SUPPORT_STBC                                 0
#define RX_SUPPORT_STBC_ONE_SPATIAL_STREAM                  1
#define RX_SUPPORT_STBC_ONE_AND_TWO_SPATIAL_STREAM          2
#define RX_SUPPORT_STBC_ONE_TWO_THREE_SPATIAL_STREAM        3
#define SUPPORT_DELAYED_BA                                  1
#define NOT_SUPPORT_DELAYED_BA                              0
#define RECEIVER_MAX_A_MSDU_SIZE_7935                       1
#define RECEIVER_MAX_A_MSDU_SIZE_3839                       0
#define BSS_OR_DEVICE_ALLOW_USE_DSSS_CCK                    1
#define BSS_OR_DEVICE_NOT_ALLOW_USE_DSSS_CCK                0
#define BSS_OR_DEVICE_SUPPORT_USE_PSMP                      1
#define BSS_OR_DEVICE_NOT_SUPPORT_USE_PSMP                  0
#define BSS_OR_DEVICE_SUPPORT_USE_STBC_CONTROL_FRAME        1
#define BSS_OR_DEVICE_NOT_SUPPORT_USE_STBC_CONTROL_FRAME    0
#define SUPPORT_L_SIG_TXOP_PROTECTION                       1
#define NOT_SUPPORT_L_SIG_TXOP_PROTECTION                   0

//                                     
#define MAX_RX_AMPDU_FACTOR_0                               0
#define MAX_RX_AMPDU_FACTOR_1                               1
#define MAX_RX_AMPDU_FACTOR_2                               2
#define MAX_RX_AMPDU_FACTOR_3                               3
#define MPDU_DENSITY_NO_RESTRICTION                         0
#define MPDU_DENSITY_ONE_EIGHTH_USECOND                     1
#define MPDU_DENSITY_ONE_FOURTH_USECOND                     2
#define MPDU_DENSITY_ONE_HALF_USECOND                       3
#define MPDU_DENSITY_ONE_USECOND                            4
#define MPDU_DENSITY_TWO_USECOND                            5
#define MPDU_DENSITY_FOUR_USECOND                           6
#define MPDU_DENSITY_EIGHT_USECOND                          7
#endif

//                  
#define SIZE_OF_SUPPORTED_MCS_SET                          16
#define SIZE_OF_BASIC_MCS_SET                              16
#define VALID_MCS_SIZE                                     77 //    
#define MCS_RX_HIGHEST_SUPPORTED_RATE_BYTE_OFFSET          10
//                                                                 
#define VALID_MAX_MCS_INDEX                                8

#if 0
//                                          
#define SUPPORT_PCO                                             1
#define NOT_SUPPORT_PCO                                         0
#define TRANSITION_TIME_0                                       0
#define STA_DOES_NOT_PROVIDE_MCS_FEEDBACK                       0
#define RESERVE_VALUE_FOR_STA_MCS_FEEDBACK                      1
#define STA_PROVIDE_UNSOLICITED_MCS_FEEDBACK                    2
#define STA_PROVIDE_ON_MRQ_AND_UNSOLICITED_MCS_FEEDBACK         3

//                                        
#define TX_BEAMFORMING_CAPABLE                                  1
#define TX_BEAMFORMING_INCAPABLE                                0
#define CAPABLE_RECEIVE_STAGGERED_SOUNDING                      1
#define NOT_CAPABLE_RECEIVE_STAGGERED_SOUNDING                  0
#define CAPABLE_TRANSMIT_STAGGERED_SOUNDING                     1
#define NOT_CAPABLE_TRANSMIT_STAGGERED_SOUNDING                 0
#define CAPABLE_RECEIVE_ZLF                                     1
#define NOT_CAPABLE_RECEIVE_ZLF                                 0
#define CAPABLE_TRANSMIT_ZLF                                    1
#define NOT_CAPABLE_TRANSMIT_ZLF                                0
#define CAPABLE_IMPLICIT_TXBF                                   1
#define NOT_CAPABLE_IMPLICIT_TXBF                               0
#define INCAPABLE_CALIBRATION                                   0
#define LIMITED_CALIBRATION_CANNOT_INITIATE                     1
#define LIMITED_CALIBRATION_CAN_INITIATE                        2
#define FULLT_CAPABLE_CALIBRATION                               3
#define CAPABLE_EXPLICIT_CSI_TXBF                               1
#define INCAPABLE_EXPLICIT_CSI_TXBF                             0
#define CAPABLE_EXPLICIT_UNCOMPRESSED_STEERING_MATRIX           1
#define INCAPABLE_EXPLICIT_UNCOMPRESSED_STEERING_MATRIX         0
#define INCAPABLE_EXPLICIT_BFCI_FEEDBACK                        0
#define CAPABLE_UNSOLICITED_BFCI_FEEDBACK                       1
#define CAPABLE_IMMEDIATE_BFCI_FEEDBACK                         2
#define CAPABLE_AGGREGATE_BFCI_FEEDBACK                         4
#define INCAPABLE_EXPLICIT_UNCOMPRESSED_STEERING_MATRIX_FEEDBAC 0
#define CAPABLE_UNSOLICITED_UNCOMPRESSED_STEERING_MATRIX        1
#define CAPABLE_IMMEDIATE_UNCOMPRESSED_STEERING_MATRIX          2
#define CAPABLE_AGGREGATED_UNCOMPRESSED_STEERING_MATRIX         4
#define INCAPABLE_EXPLICIT_COMPRESSED_STEERING_MATRIX_FEEDBAC   0
#define CAPABLE_UNSOLICITED_COMPRESSED_STEERING_MATRIX          1
#define CAPABLE_IMMEDIATE_COMPRESSED_STEERING_MATRIX            2
#define CAPABLE_AGGREGATED_COMPRESSED_STEERING_MATRIX           4
#define SUPPORT_CSI_BEAMFORMING_SINGLE_TX_ANTENNA_SOUNDING      0
#define SUPPORT_CSI_BEAMFORMING_2_TX_ANTENNA_SOUNDING           1
#define SUPPORT_CSI_BEAMFORMING_3_TX_ANTENNA_SOUNDING           2
#define SUPPORT_CSI_BEAMFORMING_4_TX_ANTENNA_SOUNDING           3
#define SUPPORT_UNCOMPRESSED_STEERING_MATRIX_1_TX_ANTENNA_SOUNDING  0
#define SUPPORT_UNCOMPRESSED_STEERING_MATRIX_2_TX_ANTENNA_SOUNDING  1
#define SUPPORT_UNCOMPRESSED_STEERING_MATRIX_3_TX_ANTENNA_SOUNDING  2
#define SUPPORT_UNCOMPRESSED_STEERING_MATRIX_4_TX_ANTENNA_SOUNDING  3
#define SUPPORT_COMPRESSED_STEERING_MATRIX_1_TX_ANTENNA_SOUNDING  0
#define SUPPORT_COMPRESSED_STEERING_MATRIX_2_TX_ANTENNA_SOUNDING  1
#define SUPPORT_COMPRESSED_STEERING_MATRIX_3_TX_ANTENNA_SOUNDING  2
#define SUPPORT_COMPRESSED_STEERING_MATRIX_4_TX_ANTENNA_SOUNDING  3

//                                     
#define CAPABLE_ANTENNA_SELECTION                               1
#define INCAPABLE_ANTENNA_SELECTION                             0
#define CAPABLE_EXPLICIT_CSI_FEEDBACK_BASED_TX_AS               1
#define INCAPABLE_EXPLICIT_CSI_FEEDBACK_BASED_TX_AS             0
#define CAPABLE_ANTENNA_INDICES_FEEDBACK_BASED_TX_AS            1
#define INCAPABLE_ANTENNA_INDICES_FEEDBACK_BASED_TX_AS          0
#define CAPABLE_EXPLICIT_CSI_FEEDBACK                           1
#define INCAPABLE_EXPLICIT_CSI_FEEDBACK                         0
#define CAPABLE_ANTENNA_INDICES_FEEDBACK_BASED_TX_AS            1
#define INCAPABLE_ANTENNA_INDICES_FEEDBACK_BASED_TX_AS          0
#define CAPABLE_RX_AS                                           1
#define INCAPABLE_RX_AS                                         0
#define CAPABLE_TRANSMIT_SOUNDING_PPDU                          1
#define INCAPABLE_TRANSMIT_SOUNDING_PPDU                        0

//                                          
#define CONTROL_CHANNEL_1                                       1

#define OFFSET_NO_EXTENSION_CHANNEL_IS_PRESENT                  0
#define OFFSET_EXTENSION_CHANNEL_ABOVE_CONTROL_CHANNEL          1
#define OFFSET_EXTENSION_CHANNEL_BELOW_CONTROL_CHANNEL          3
#define USE_20_MHZ_CONTROL_CHANNEL                              0
#define USE_ANY_SUPPORTED_CHANNEL_WIDTH                         1
#define USE_RIFS_MODE_PROHIBITED                                0
#define USE_RIFS_MODE_PERMITTED                                 1
#define CONTROLLED_ACCESS_NOT_ONLY_PSMP                         0
#define CONTROLLED_ACCESS_ONLY_PSMP                             1
#define SERVICE_INTERVAL_GRANULARITY_5MS                        0
#define SERVICE_INTERVAL_GRANULARITY_10MS                       1
#define SERVICE_INTERVAL_GRANULARITY_15MS                       2
#define SERVICE_INTERVAL_GRANULARITY_20MS                       3
#define SERVICE_INTERVAL_GRANULARITY_25MS                       4
#define SERVICE_INTERVAL_GRANULARITY_30MS                       5
#define SERVICE_INTERVAL_GRANULARITY_35MS                       6
#define SERVICE_INTERVAL_GRANULARITY_40MS                       7

#define OPERATION_MODE_PURE_NO_PROTECTION                       0
#define OPERATION_MODE_OPTIONAL_PROTECTION                      1
#define OPERATION_MODE_NO_LEGACY_STA_40MHZ_PROTECTED            2
#define OPERATION_MODE_MIXED_PROTECTION                         3

#define DUAL_CTS_PROTECTION_IS_USED                             1
#define REGULAR_RTS_CTS_IS_USED                                 0
#define SECONDARY_BEACON                                        1
#define PRIMARY_BEACON                                          0
#define LSIG_TXOP_PROTECTION_FULL_SUPPORT                       1
#define LSIG_TXOP_PROTECTION_NOT_FULL_SUPPORT                   0
#define PCO_IS_ACTIVATED_IN_BSS                                 1
#define PCO_IS_NOT_ACTIVATED_IN_BSS                             0
#define PCO_PHASE_REQUEST_SWITCH_KEEP_TO_20MHZ                  0
#define PCO_PHASE_REQUEST_SWITCH_KEEP_TO_40MHZ                  1
#endif

//
//                                                                         
//                                                                        
//                                                         
//
typedef enum eHTCapability
{
  eHT_LSIG_TXOP_PROTECTION,
  eHT_STBC_CONTROL_FRAME,
  eHT_PSMP,
  eHT_DSSS_CCK_MODE_40MHZ,
  eHT_MAX_AMSDU_LENGTH,
  eHT_DELAYED_BA,
  eHT_RX_STBC,
  eHT_TX_STBC,
  eHT_SHORT_GI_40MHZ,
  eHT_SHORT_GI_20MHZ,
  eHT_GREENFIELD,
  eHT_MIMO_POWER_SAVE,
  eHT_SUPPORTED_CHANNEL_WIDTH_SET,
  eHT_ADVANCED_CODING,
  eHT_MAX_RX_AMPDU_FACTOR,
  eHT_MPDU_DENSITY,
  eHT_PCO,
  eHT_TRANSITION_TIME,
  eHT_MCS_FEEDBACK,
  eHT_TX_BEAMFORMING,
  eHT_ANTENNA_SELECTION,
  //                                                    
  eHT_SI_GRANULARITY,
  eHT_CONTROLLED_ACCESS,
  eHT_RIFS_MODE,
  eHT_RECOMMENDED_TX_WIDTH_SET,
  eHT_EXTENSION_CHANNEL_OFFSET,
  eHT_OP_MODE,
  eHT_BASIC_STBC_MCS,
  eHT_DUAL_CTS_PROTECTION,
  eHT_LSIG_TXOP_PROTECTION_FULL_SUPPORT,
  eHT_PCO_ACTIVE,
  eHT_PCO_PHASE
} tHTCapability;

//                     
typedef __ani_attr_pre_packed struct sSirMacHTCapabilityInfo
{
#ifndef ANI_LITTLE_BIT_ENDIAN
  tANI_U16  lsigTXOPProtection:1; //              
  tANI_U16  stbcControlFrame:1; //               
  tANI_U16  psmp:1; //               
  tANI_U16  dsssCckMode40MHz:1; //               
  tANI_U16  maximalAMSDUsize:1; //               
  tANI_U16  delayedBA:1; //               
  tANI_U16  rxSTBC:2; //               
  tANI_U16  txSTBC:1; //               
  tANI_U16  shortGI40MHz:1; //               
  tANI_U16  shortGI20MHz:1; //               
  tANI_U16  greenField:1; //               
  tANI_U16  mimoPowerSave:2; //              
  tANI_U16  supportedChannelWidthSet:1; //               
  tANI_U16  advCodingCap:1; //               
#else
  tANI_U16  advCodingCap:1;
  tANI_U16  supportedChannelWidthSet:1;
  tANI_U16  mimoPowerSave:2;
  tANI_U16  greenField:1;
  tANI_U16  shortGI20MHz:1;
  tANI_U16  shortGI40MHz:1;
  tANI_U16  txSTBC:1;
  tANI_U16  rxSTBC:2;
  tANI_U16  delayedBA:1;
  tANI_U16  maximalAMSDUsize:1;
  tANI_U16  dsssCckMode40MHz:1;
  tANI_U16  psmp:1;
  tANI_U16  stbcControlFrame:1;
  tANI_U16  lsigTXOPProtection:1;
#endif
} __ani_attr_packed tSirMacHTCapabilityInfo;

//                   
typedef __ani_attr_pre_packed struct sSirMacHTParametersInfo
{
#ifndef ANI_LITTLE_BIT_ENDIAN
  tANI_U8  reserved:3;
  tANI_U8  mpduDensity:3; //              
  tANI_U8  maxRxAMPDUFactor:2; //              
#else
  tANI_U8  maxRxAMPDUFactor:2;
  tANI_U8  mpduDensity:3;
  tANI_U8  reserved:3;
#endif
} __ani_attr_packed tSirMacHTParametersInfo;

//                              
typedef __ani_attr_pre_packed struct sSirMacExtendedHTCapabilityInfo
{
#ifndef ANI_LITTLE_BIT_ENDIAN
  tANI_U16  reserved2:6;
  tANI_U16  mcsFeedback:2; //               
  tANI_U16  reserved1:5;
  tANI_U16  transitionTime:2; //               
  tANI_U16  pco:1; //               
#else
  tANI_U16  pco:1;
  tANI_U16  transitionTime:2;
  tANI_U16  reserved1:5;
  tANI_U16  mcsFeedback:2;
  tANI_U16  reserved2:6;
#endif
} __ani_attr_packed tSirMacExtendedHTCapabilityInfo;

//                              
//                                     
typedef __ani_attr_pre_packed struct sSirMacRxHighestSupportRate
{
#ifndef ANI_LITTLE_BIT_ENDIAN
    tANI_U16 reserved : 6;
    tANI_U16 rate : 10;
#else
    tANI_U16 rate : 10;
    tANI_U16 reserved : 6;
#endif
} __ani_attr_packed tSirMacRxHighestSupportRate, *tpSirMacRxHighestSupportRate;


//                                        
typedef __ani_attr_pre_packed struct sSirMacTxBFCapabilityInfo
{
#ifndef ANI_LITTLE_BIT_ENDIAN
  tANI_U32  reserved:7;
  tANI_U32  compressedSteeringMatrixBFAntennae:2; //               
  tANI_U32  uncompressedSteeringMatrixBFAntennae:2; //               
  tANI_U32  csiNumBFAntennae:2; //               
  tANI_U32  explicitCompressedSteeringMatrixFeedback:3; //               
  tANI_U32  explicitUncompressedSteeringMatrixFeedback:3; //               
  tANI_U32  explicitBFCSIFeedback:3; //               
  tANI_U32  explicitUncompressedSteeringMatrix:1; //               
  tANI_U32  explicitCSITxBF:1; //               
  tANI_U32  calibration:2; //               
  tANI_U32  implicitTxBF:1; //               
  tANI_U32  txZLF:1; //               
  tANI_U32  rxZLF:1; //               
  tANI_U32  txStaggeredSounding:1; //               
  tANI_U32  rxStaggeredSounding:1; //               
  tANI_U32  txBF:1; //               
#else
  tANI_U32  txBF:1;
  tANI_U32  rxStaggeredSounding:1;
  tANI_U32  txStaggeredSounding:1;
  tANI_U32  rxZLF:1;
  tANI_U32  txZLF:1;
  tANI_U32  implicitTxBF:1;
  tANI_U32  calibration:2;
  tANI_U32  explicitCSITxBF:1;
  tANI_U32  explicitUncompressedSteeringMatrix:1;
  tANI_U32  explicitBFCSIFeedback:3;
  tANI_U32  explicitUncompressedSteeringMatrixFeedback:3;
  tANI_U32  explicitCompressedSteeringMatrixFeedback:3;
  tANI_U32  csiNumBFAntennae:2;
  tANI_U32  uncompressedSteeringMatrixBFAntennae:2;
  tANI_U32  compressedSteeringMatrixBFAntennae:2;
  tANI_U32  reserved:7;
#endif
} __ani_attr_packed tSirMacTxBFCapabilityInfo;

//                                  
typedef __ani_attr_pre_packed struct sSirMacASCapabilityInfo
{
#ifndef ANI_LITTLE_BIT_ENDIAN
  tANI_U8  reserved2:1;
  tANI_U8  txSoundingPPDUs:1; //               
  tANI_U8  rxAS:1; //               
  tANI_U8  antennaIndicesFeedback:1; //               
  tANI_U8  explicitCSIFeedback:1; //               
  tANI_U8  antennaIndicesFeedbackTx:1; //               
  tANI_U8  explicitCSIFeedbackTx:1; //               
  tANI_U8  antennaSelection:1; //               
#else
  tANI_U8  antennaSelection:1;
  tANI_U8  explicitCSIFeedbackTx:1;
  tANI_U8  antennaIndicesFeedbackTx:1;
  tANI_U8  explicitCSIFeedback:1;
  tANI_U8  antennaIndicesFeedback:1;
  tANI_U8  rxAS:1;
  tANI_U8  txSoundingPPDUs:1;
  tANI_U8  reserved2:1;
#endif
} __ani_attr_packed tSirMacASCapabilityInfo;

//                        
typedef __ani_attr_pre_packed struct sSirMacHTInfoField1
{
#ifndef ANI_LITTLE_BIT_ENDIAN
  tANI_U8  serviceIntervalGranularity:3; //              
  tANI_U8  controlledAccessOnly:1; //               
  tANI_U8  rifsMode:1; //              
  tANI_U8  recommendedTxWidthSet:1; //              
  tANI_U8  secondaryChannelOffset:2; //              
#else
  tANI_U8  secondaryChannelOffset:2;
  tANI_U8  recommendedTxWidthSet:1;
  tANI_U8  rifsMode:1;
  tANI_U8  controlledAccessOnly:1;
  tANI_U8  serviceIntervalGranularity:3;
#endif
} __ani_attr_packed tSirMacHTInfoField1;

//                        
typedef __ani_attr_pre_packed struct sSirMacHTInfoField2
{
#ifndef ANI_LITTLE_BIT_ENDIAN
  tANI_U16  reserved:11;
  tANI_U16  obssNonHTStaPresent:1;  /*                */
  tANI_U16  transmitBurstLimit: 1;
  tANI_U16  nonGFDevicesPresent:1;
  tANI_U16  opMode:2; //              
#else
  tANI_U16  opMode:2;
  tANI_U16  nonGFDevicesPresent:1;
  tANI_U16  transmitBurstLimit: 1;
  tANI_U16  obssNonHTStaPresent:1;  /*                */
  tANI_U16  reserved:11;
#endif
} __ani_attr_packed tSirMacHTInfoField2;

//                        
typedef __ani_attr_pre_packed struct sSirMacHTInfoField3
{
#ifndef ANI_LITTLE_BIT_ENDIAN
  tANI_U16  reserved:4;
  tANI_U16  pcoPhase:1; //              
  tANI_U16  pcoActive:1; //              
  tANI_U16  lsigTXOPProtectionFullSupport:1; //              
  tANI_U16  secondaryBeacon:1; //              
  tANI_U16  dualCTSProtection:1; //              
  tANI_U16  basicSTBCMCS:7; //              
#else
  tANI_U16  basicSTBCMCS:7;
  tANI_U16  dualCTSProtection:1;
  tANI_U16  secondaryBeacon:1;
  tANI_U16  lsigTXOPProtectionFullSupport:1;
  tANI_U16  pcoActive:1;
  tANI_U16  pcoPhase:1;
  tANI_U16  reserved:4;
#endif
} __ani_attr_packed tSirMacHTInfoField3;

typedef __ani_attr_pre_packed struct sSirMacProbeReqFrame
{
    tSirMacSSidIE      ssIdIE;
    tSirMacRateSetIE   rateSetIE;
    tSirMacRateSetIE         extendedRateSetIE;
} __ani_attr_packed tSirMacProbeReqFrame, *tpSirMacProbeReqFrame;

typedef __ani_attr_pre_packed struct sSirMacProbeRspFrame
{
    tSirMacTimeStamp         ts;
    tSirMacBeaconInterval    beaconInterval;
    tSirMacCapabilityInfo    capabilityInfo;
    tSirMacSSidIE            ssIdIE;
    tSirMacRateSetIE         rateSetIE;
    tSirMacRateSetIE         extendedRateSetIE;
    tSirMacNonErpPresentIE   nonErpPresent;
    tSirMacDsParamSetIE      dsParamsIE;
    tSirMacCfParamSetIE      cfParamsIE;
} __ani_attr_packed tSirMacProbeRspFrame, *tpSirMacProbeRspFrame;

typedef __ani_attr_pre_packed struct sSirMacAuthFrameBody
{
    tANI_U16     authAlgoNumber;
    tANI_U16     authTransactionSeqNumber;
    tANI_U16     authStatusCode;
    tANI_U8      type;   //                             
    tANI_U8      length; //                                
    tANI_U8      challengeText[SIR_MAC_AUTH_CHALLENGE_LENGTH];
} __ani_attr_packed tSirMacAuthFrameBody, *tpSirMacAuthFrameBody;

typedef __ani_attr_pre_packed struct sSirMacAuthenticationFrame
{
    tSirMacAuthFrameBody  authFrameBody;
} __ani_attr_packed tSirMacAuthFrame, *tpSirMacAuthFrame;

typedef __ani_attr_pre_packed struct sSirMacAssocReqFrame
{
    tSirMacCapabilityInfo    capabilityInfo;
    tANI_U16                      listenInterval;
    tSirMacSSidIE            ssIdIE;
    tSirMacRateSetIE         rateSetIE;
    tSirMacRateSetIE         extendedRateSetIE;
} __ani_attr_packed tSirMacAssocReqFrame, *tpSirMacAssocReqFrame;

typedef __ani_attr_pre_packed struct sSirMacAssocRspFrame
{
    tSirMacCapabilityInfo    capabilityInfo;
    tANI_U16                      statusCode;
    tANI_U16                      aid;
    tSirMacRateSetIE         supportedRates;
    tSirMacRateSetIE         extendedRateSetIE;
} __ani_attr_packed tSirMacAssocRspFrame, *tpSirMacAssocRspFrame;

typedef __ani_attr_pre_packed struct sSirMacDisassocFrame
{
    tANI_U16                reasonCode;
} __ani_attr_packed tSirMacDisassocFrame, *tpSirMacDisassocFrame;

typedef __ani_attr_pre_packed struct sDSirMacDeauthFrame
{
    tANI_U16                reasonCode;
} __ani_attr_packed tSirMacDeauthFrame, *tpSirMacDeauthFrame;

//                                     
typedef __ani_attr_pre_packed struct sSirMacActionFrameHdr
{
    tANI_U8    category;
    tANI_U8    actionID;
} __ani_attr_packed tSirMacActionFrameHdr, *tpSirMacActionFrameHdr;

#if  defined (WLAN_FEATURE_VOWIFI_11R) || defined (FEATURE_WLAN_CCX) || defined(FEATURE_WLAN_LFR)
typedef __ani_attr_pre_packed struct sSirMacVendorSpecificFrameHdr
{
    tANI_U8    category;
    tANI_U8    Oui[4];
} __ani_attr_packed tSirMacVendorSpecificFrameHdr, *tpSirMacVendorSpecificFrameHdr;
#endif

typedef __ani_attr_pre_packed struct sSirMacVendorSpecificPublicActionFrameHdr
{
    tANI_U8    category;
    tANI_U8    actionID;
    tANI_U8    Oui[4];
    tANI_U8    OuiSubType;
    tANI_U8    dialogToken;
} __ani_attr_packed tSirMacVendorSpecificPublicActionFrameHdr, *tpSirMacVendorSpecificPublicActionFrameHdr;

typedef __ani_attr_pre_packed struct sSirMacP2PActionFrameHdr
{
    tANI_U8    category;
    tANI_U8    Oui[4];
    tANI_U8    OuiSubType;
    tANI_U8    dialogToken;
} __ani_attr_packed tSirMacP2PActionFrameHdr, *tpSirMacP2PActionFrameHdr;



typedef  struct sSirMacMeasActionFrameHdr
{
    tANI_U8    category;
    tANI_U8    actionID;
    tANI_U8    dialogToken;
} tSirMacMeasActionFrameHdr, *tpSirMacMeasActionFrameHdr;


#ifdef ANI_SUPPORT_11H
typedef  struct sSirMacTpcReqActionFrame
{
    tSirMacMeasActionFrameHdr   actionHeader;
    tANI_U8                          type;
    tANI_U8                          length;
} tSirMacTpcReqActionFrame, *tpSirMacTpcReqActionFrame;

typedef  struct sSirMacMeasReqActionFrame
{
    tSirMacMeasActionFrameHdr   actionHeader;
    tSirMacMeasReqIE            measReqIE;
} tSirMacMeasReqActionFrame, *tpSirMacMeasReqActionFrame;
#endif

typedef  struct sSirMacBasicMeasReportActionFrame
{
    tSirMacMeasActionFrameHdr   actionHeader;
    tSirMacBasicReportIE        measReportIE;
} tSirMacBasicMeasReportActionFrame, *tpSirMacBasicMeasReportActionFrame;

typedef  struct sSirMacCcaMeasReportActionFrame
{
    tSirMacMeasActionFrameHdr   actionHeader;
    tSirMacCcaReportIE          measReportIE;
} tSirMacCcaMeasReportActionFrame, *tpSirMacCcaMeasReportActionFrame;

typedef  struct sSirMacRpiMeasReportActionFrame
{
    tSirMacMeasActionFrameHdr   actionHeader;
    tSirMacRpiReportIE          measReportIE;
} tSirMacRpiMeasReportActionFrame, *tpSirMacRpiMeasReportActionFrame;

#if defined WLAN_FEATURE_VOWIFI

typedef struct sSirMacNeighborReportReq
{
   tANI_U8 dialogToken;
   tANI_U8 ssid_present;
   tSirMacSSid ssid;  
} tSirMacNeighborReportReq, *tpSirMacNeighborReportReq;

typedef struct sSirMacLinkReport
{
   tANI_U8 dialogToken;
   tANI_U8 txPower;
   tANI_U8 rxAntenna;
   tANI_U8 txAntenna;
   tANI_U8 rcpi;
   tANI_U8 rsni;
} tSirMacLinkReport, *tpSirMacLinkReport;

#define BEACON_REPORT_MAX_IES 224 //                                    
typedef struct sSirMacBeaconReport
{
   tANI_U8 regClass;
   tANI_U8 channel;
   tANI_U8 measStartTime[8];
   tANI_U8 measDuration;
   tANI_U8 phyType;
   tANI_U8 bcnProbeRsp;
   tANI_U8 rsni;
   tANI_U8 rcpi;
   tSirMacAddr bssid;
   tANI_U8 antennaId;
   tANI_U32 parentTSF;
   tANI_U8 numIes;
   tANI_U8 Ies[BEACON_REPORT_MAX_IES];  

} tSirMacBeaconReport, *tpSirMacBeaconReport;

#define RADIO_REPORTS_MAX_IN_A_FRAME 4
typedef struct sSirMacRadioMeasureReport
{
   tANI_U8     token;
   tANI_U8     refused;
   tANI_U8     incapable;
   tANI_U8     type;  
   union 
   {
     tSirMacBeaconReport beaconReport;
   }report;  

}tSirMacRadioMeasureReport, *tpSirMacRadioMeasureReport;

#endif

//                             

//                                                   
#define SIR_MAC_TCLASIE_MAXNUM  2

//              
typedef __ani_attr_pre_packed struct sSirMacQosAddtsReqAF
{
    tSirMacActionFrameHdr afHdr;
    tANI_U8                    dlgToken;
    tSirMacTspecIE        tspec;
    tANI_U8                    tclas[1]; //                        
} __ani_attr_packed tSirMacQosAddtsReqAF;

//               
typedef __ani_attr_pre_packed struct sSirMacQosAddtsRspAF
{
    tSirMacActionFrameHdr afHdr;
    tANI_U8                    dlgToken;
    tSirMacStatusCodes    status;
    tSirMacTsDelayIE      delay;
    tSirMacTspecIE        tspec;
    tANI_U8                    tclas[1];
} __ani_attr_packed tSirMacQosAddtsRspAF;

//            
typedef __ani_attr_pre_packed struct sSirMacQosDeltsAF
{
    tSirMacActionFrameHdr afHdr;
    tSirMacTSInfo         tsinfo;
} __ani_attr_packed tSirMacQosDeltsAF;

//               
typedef __ani_attr_pre_packed struct sSirMacQosScheduleAF
{
    tSirMacActionFrameHdr afHdr;
    tSirMacScheduleIE     schedule;
} __ani_attr_packed tSirMacQosScheduleAF;

//                             

//            
typedef __ani_attr_pre_packed struct sSirMacQosDlpReqAF
{
    tSirMacActionFrameHdr afHdr;
    tANI_U8                    dstMAC[6];
    tANI_U8                    srcMAC[6];
    tSirMacQosCapabilityIE qosCapability;
    tANI_U16                   dlpTimeout;
    tSirMacRateSetIE      supportedRates;
} __ani_attr_packed tSirMacQosDlpReqAF;

//             
typedef __ani_attr_pre_packed struct sSirMacQosDlpRspAF
{
    tSirMacActionFrameHdr afHdr;
    tANI_U8                    dstMAC[6];
    tANI_U8                    srcMAC[6];
    tSirMacStatusCodes    status;
    tSirMacQosCapabilityIE qosCapability;
    tSirMacRateSetIE      supportedRates;
} __ani_attr_packed tSirMacQosDlpRspAF;

//             
typedef __ani_attr_pre_packed struct sSirMacQosDlpTdnAF
{
    tSirMacActionFrameHdr afHdr;
    tANI_U8                    dstMAC[6];
    tANI_U8                    srcMAC[6];
} __ani_attr_packed tSirMacQosDlpTdnAF;




//
//                                                     
typedef __ani_attr_pre_packed struct sSirMacAniActionFrame
{
    tSirMacActionFrameHdr afHdr;
    tANI_U8                    aniOui[3]; //         
    tANI_U8                    type;      //                                 
} __ani_attr_packed tSirMacAniActionFrame, *tpSirMacAniActionFrame;


typedef __ani_attr_pre_packed struct sSirMacLinkTestReqData
{
    tSirMacAniActionFrame hdr;
    tANI_U8                    frameStatus; //                   
                                       //                   
                                       //                       
                                       //                   
                                       //                  
    tANI_U8                    rate;
    tANI_U8                    randomData[SIR_MAC_MAX_RANDOM_LENGTH];
} __ani_attr_packed tSirMacLinkTestReqData, *tpSirMacLinkTestReqData;

//                            
typedef __ani_attr_pre_packed struct sSirMacSnrReport
{
    tSirMacAniActionFrame hdr;
    tANI_U32                   numSamples;
    tANI_U32                   snr;
    tANI_U32                   snrComp;
    tANI_U32                   sq;
    tANI_U32                   rssi;
} __ani_attr_packed tSirMacSnrReport, *tpSirMacSnrReport;

typedef __ani_attr_pre_packed struct sSirMacStaCbLegacyBssDetect
{
    tSirMacAniActionFrame hdr;
    tANI_U8               channelNum;
    tANI_U8               numLegacyBssid;
    /*                                                                                                                      */
    tANI_U8               LegacyBssidList[1]; /*                                                            */
} __ani_attr_packed tSirMacStaCbLegacyBssDetect, *tpSirMacStaCbLegacyBssDetect;

typedef __ani_attr_pre_packed struct sSirMacQoSDefBAReq
{
    tSirMacActionFrameHdr hdr;

    tANI_U8  rsvd1;

#ifdef ANI_LITTLE_BIT_ENDIAN
    tANI_U8  tid: 4;
    tANI_U8  rsvd2: 4;
#else
    tANI_U8  rsvd2: 4;
    tANI_U8  tid: 4;
#endif
} __ani_attr_packed tSirMacQoSDefBAReq, *tpSirMacQoSDefBAReq;

typedef __ani_attr_pre_packed struct sSirMacQoSDefBARsp
{
    tSirMacActionFrameHdr hdr;

#ifdef ANI_LITTLE_BIT_ENDIAN
    tANI_U8 tid: 4;
    tANI_U8 policy: 1;
    tANI_U8 reject: 1;
    tANI_U8 rsvd1: 2;
#else
    tANI_U8 rsvd1: 2;
    tANI_U8 reject: 1;
    tANI_U8 policy: 1;
    tANI_U8 tid: 4;
#endif

    tANI_U8 bufferSize;
} __ani_attr_packed tSirMacQoSDefBARsp, *tpSirMacQoSDefBARsp;

typedef __ani_attr_pre_packed struct sSirMacQoSDelBAReq
{
    tSirMacActionFrameHdr hdr;
    tANI_U8 rsvd1;

#ifdef ANI_LITTLE_BIT_ENDIAN
    tANI_U8 rsvd2: 3;
    tANI_U8 direction: 1;
    tANI_U8 tid: 4;
#else
    tANI_U8 tid: 4;
    tANI_U8 direction: 1;
    tANI_U8 rsvd2: 3;
#endif
} __ani_attr_packed tSirMacQoSDelBAReq, *tpSirMacQoSDelBAReq;

typedef __ani_attr_pre_packed struct sSirMacQoSDelBARsp
{
    tSirMacActionFrameHdr hdr;

#ifdef ANI_LITTLE_BIT_ENDIAN
    tANI_U8 tid: 4;
    tANI_U8 policy: 1;
    tANI_U8 reject: 1;
    tANI_U8 rsvd1: 2;
#else
    tANI_U8 rsvd1: 2;
    tANI_U8 reject: 1;
    tANI_U8 policy: 1;
    tANI_U8 tid: 4;
#endif

} __ani_attr_packed tSirMacQoSDelBARsp, *tpSirMacQoSDelBARsp;



//                                

#define SIR_MAC_RATE_1   0x02
#define SIR_MAC_RATE_2   0x04
#define SIR_MAC_RATE_5_5 0x0B
#define SIR_MAC_RATE_11  0x16

//                                  

#define SIR_MAC_RATE_6   0x0C
#define SIR_MAC_RATE_9   0x12
#define SIR_MAC_RATE_12  0x18
#define SIR_MAC_RATE_18  0x24
#define SIR_MAC_RATE_24  0x30
#define SIR_MAC_RATE_36  0x48
#define SIR_MAC_RATE_48  0x60
#define SIR_MAC_RATE_54  0x6C

//                           
#define SIR_MAC_RATE_72  0x01
#define SIR_MAC_RATE_96  0x03
#define SIR_MAC_RATE_108 0x05

//                   
#define SIR_MAC_RATE_42  1000
#define SIR_MAC_RATE_84  1001
#define SIR_MAC_RATE_126 1002
#define SIR_MAC_RATE_144 1003
#define SIR_MAC_RATE_168 1004
#define SIR_MAC_RATE_192 1005
#define SIR_MAC_RATE_216 1006
#define SIR_MAC_RATE_240 1007


#define sirIsArate(x) ((((tANI_U8)x)==SIR_MAC_RATE_6) || \
                       (((tANI_U8)x)==SIR_MAC_RATE_9) || \
                       (((tANI_U8)x)==SIR_MAC_RATE_12)|| \
                       (((tANI_U8)x)==SIR_MAC_RATE_18)|| \
                       (((tANI_U8)x)==SIR_MAC_RATE_24)|| \
                       (((tANI_U8)x)==SIR_MAC_RATE_36)|| \
                       (((tANI_U8)x)==SIR_MAC_RATE_48)|| \
                       (((tANI_U8)x)==SIR_MAC_RATE_54))

#define sirIsBrate(x) ((((tANI_U8)x)==SIR_MAC_RATE_1)  || \
                       (((tANI_U8)x)==SIR_MAC_RATE_2)  || \
                       (((tANI_U8)x)==SIR_MAC_RATE_5_5)|| \
                       (((tANI_U8)x)==SIR_MAC_RATE_11))

#define sirIsGrate(x) ((((tANI_U8)x)==SIR_MAC_RATE_1)  || \
                       (((tANI_U8)x)==SIR_MAC_RATE_2)  || \
                       (((tANI_U8)x)==SIR_MAC_RATE_5_5)|| \
                       (((tANI_U8)x)==SIR_MAC_RATE_11) || \
                       (((tANI_U8)x)==SIR_MAC_RATE_6)  || \
                       (((tANI_U8)x)==SIR_MAC_RATE_9)  || \
                       (((tANI_U8)x)==SIR_MAC_RATE_12) || \
                       (((tANI_U8)x)==SIR_MAC_RATE_18) || \
                       (((tANI_U8)x)==SIR_MAC_RATE_24) || \
                       (((tANI_U8)x)==SIR_MAC_RATE_36) || \
                       (((tANI_U8)x)==SIR_MAC_RATE_48) || \
                       (((tANI_U8)x)==SIR_MAC_RATE_54))

#define sirIsProprate(x) ((((tANI_U8)x)==SIR_MAC_RATE_72) || \
                          (((tANI_U8)x)==SIR_MAC_RATE_96) || \
                          (((tANI_U8)x)==SIR_MAC_RATE_108))

#define sirIsEnhancedRate(x) \
                         (((x)==SIR_MAC_RATE_42)  || \
                          ((x)==SIR_MAC_RATE_84)  || \
                          ((x)==SIR_MAC_RATE_126) || \
                          ((x)==SIR_MAC_RATE_144) || \
                          ((x)==SIR_MAC_RATE_168) || \
                          ((x)==SIR_MAC_RATE_192) || \
                          ((x)==SIR_MAC_RATE_216) || \
                          ((x)==SIR_MAC_RATE_240))

//                                          
typedef __ani_attr_pre_packed struct sSirMacPhyRates
{
    tANI_U8    rateId;
    tANI_U8    phyRate;        //                            
    tANI_U16   macRate;         //                            
} __ani_attr_packed tSirMacPhyRates, *tpSirMacPhyRates;


typedef __ani_attr_pre_packed struct sSirPhy11aHdr
{

#ifndef ANI_LITTLE_BIT_ENDIAN

    tANI_U8 lengthLo : 3;
    tANI_U8 reserved : 1;
    tANI_U8 rate : 4;

    tANI_U8 lengthMid : 8;

    tANI_U8 tail: 6;
    tANI_U8 parity : 1;
    tANI_U8 lengthHi : 1;

    tANI_U8 serviceLo;
    tANI_U8 serviceHi;
#else
    tANI_U8 rate : 4;
    tANI_U8 reserved : 1;
    tANI_U8 lengthLo : 3;

    tANI_U8 lengthMid : 8;

    tANI_U8 lengthHi : 1;
    tANI_U8 parity : 1;
    tANI_U8 tail: 6;

    tANI_U8 serviceLo;
    tANI_U8 serviceHi;
#endif
} __ani_attr_packed tSirPhy11aHdr, *tpSirPhy11aHdr;

#define SIR_MAC_MIN_IE_LEN 2 //                                    
#endif /*                   */

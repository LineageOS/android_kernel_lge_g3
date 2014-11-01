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

#ifndef EVENT_DEFS_H
#define EVENT_DEFS_H


typedef enum
{
  EVENT_DROP_ID = 0,

  EVENT_BAND_CLASS_CHANGE = 0x0100,   /*                                */
  EVENT_CDMA_CH_CHANGE,               /*                                  */
  EVENT_BS_P_REV_CHANGE,              /*                              */
  EVENT_P_REV_IN_USE_CHANGE,          /*                                  */
  EVENT_SID_CHANGE,                   /*                         */
  EVENT_NID_CHANGE,                   /*                         */
  EVENT_PZID_CHANGE,                  /*                          */
  EVENT_PDE_SESSION_END,              /*            */
  EVENT_OP_MODE_CHANGE,               /*                                    */
  EVENT_MESSAGE_RECEIVED,             /*                                    
                                                 */
  EVENT_MESSAGE_TRANSMITTED,          /*                                    
                                                 */
  EVENT_TIMER_EXPIRED,                /*                              */
  EVENT_COUNTER_THRESHOLD,            /*                                */
  EVENT_CALL_PROCESSING_STATE_CHANGE, /*                                    
                                                 */ 
  EVENT_CALL_CONTROL_INSTANTIATED,    /*                             */
  EVENT_CALL_CONTROL_STATE_CHANGE,    /*                                   
                                                                 */
  EVENT_CALL_CONTROL_TERMINATED,      /*                             */
  EVENT_REG_ZONE_CHANGE,              /*                              */ 
  EVENT_SLOTTED_MODE_OPERATION,       /*                                    */
  EVENT_QPCH_IN_USE,                  /*                                        */
  EVENT_IDLE_HANDOFF,                 /*                               */
  EVENT_ACCESS_HANDOFF,               /*                               */
  EVENT_ACCESS_PROBE_HANDOFF,         /*                               */
  EVENT_SOFT_HANDOFF,   
    /*                                                                         
           */ 
  EVENT_HARD_HANDOFF_FREQ_CHANGE, 
    /*                                                                         
           */ 
  EVENT_HARD_HANDOFF_FRAME_OFFSET_CHANGE, 
    /*                                                                         
           */ 
  EVENT_HARD_HANDOFF_DISJOINT_ASET,
    /*                                                                         
           */ 
  EVENT_UNSUCCESSFUL_HARD_HANDOFF,    /*            */
  EVENT_TMSI_ASSIGNED,                /*                          */
  EVENT_SERVICE_NEGOTIATION_COMPLETED,/*            */
  EVENT_SO_NEGOTIATION_COMPLETED,     /*            */
  EVENT_ENTER_CONTROL_HOLD_MODE,      /*            */
  EVENT_EXIT_CONTROL_HOLD_MODE,       /*            */
  EVENT_START_FWD_SUPP_BURST_ASSGN,   /*                              */
  EVENT_END_FWD_SUPP_BURST_ASSGN,     /*            */
  EVENT_START_REV_SUPP_BURST_ASSGN,   /*                              */
  EVENT_END_REV_SUPP_BURST_ASSGN,     /*            */
  EVENT_DTX,                          /*            */
  EVENT_T_ADD_ABORT,                  /*            */
  EVENT_CH_IND_CHANGE,                /*                           */
  EVENT_TRANSMITTER_DISABLED,         /*            */
  EVENT_TRANSMITTER_ENABLED,          /*            */
  EVENT_SMS_RECEIVED,                 /*            */
  EVENT_SMS_SENT,                     /*            */
  EVENT_INACTIVITY_TIMER_EXPIRED,     /*            */
  EVENT_DORMANT_TIMER_EXPIRED,        /*            */
  EVENT_ACCESS_ATTEMPT_FAIL_MAX_PROBES_SENT, /*            */
  EVENT_ACCESS_ATTEMPT_FAIL_LOSS_OF_PC_OR_FCCC, /*            */
  EVENT_PCH_ACQUIRED,                 /*                              
                                                    */
  EVENT_BCCH_ACQUIRED,                /*                                  
                                                              */
  EVENT_FFCH_ACQUIRED,                /*                   */
  EVENT_FDCCH_ACQUIRED,               /*                   */
  EVENT_FFCH_PLUS_DCCH_ACQUIRED,      /*            */
  EVENT_REGISTRATION_PERFORMED,       /*                              */ 
  EVENT_NEW_SYSTEM_IDLE_HANDOFF,      /*            */
  EVENT_SYSTEM_RESELECTION,           /*                                 */
  EVENT_RESCAN,                       /*            */
  EVENT_PROTOCOL_MISMATCH,            /*            */
  EVENT_LOCK,                         /*            */
  EVENT_UNLOCK,                       /*            */
  EVENT_ACCESS_DENIED,                /*            */
  EVENT_NDSS_OFF,                     /*            */
  EVENT_RELEASE,                      /*                 */
  EVENT_ERROR,                        /*            */
  EVENT_REDIRECTION,                  /*            */
  EVENT_REGISTRATION_REJECTED,        /*            */
  EVENT_WRONG_SYSTEM,                 /*            */
  EVENT_WRONG_NETWORK,                /*            */
  EVENT_LOSS_OF_ACQ_AFTER_SLEEP,      /*            */
  EVENT_POWER_DOWN,                   /*            */
  EVENT_CALL_RELEASE_REQUEST,         /*            */
  EVENT_SERVICE_INACTIVE,             /*            */
  EVENT_EXTENDED_RELEASE,             /*            */

  EVENT_HDR_MSG_RX,                   /*                        */
  EVENT_HDR_RXMSG_IGNORED_STATE,      /*                        */
  EVENT_HDR_RXMSG_IGNORED_SEQ,        /*                        */
  EVENT_HDR_TXMSG_ACKED,              /*                        */
  EVENT_HDR_TXMSG_DROPPED,            /*                        */      
  EVENT_HDR_STATE_CHANGE,             /*                              */
  EVENT_HDR_ALMP_OBEYING_REDIRECTION, /*            */
  EVENT_HDR_ALMP_CONNECTION_CLOSED,   /*            */
  EVENT_HDR_ALMP_T_SD_RESELECT,       /*            */
  EVENT_HDR_ALMP_CONNECTION_OPENED,   /*            */
  EVENT_HDR_HMP_QUEUED_MSG,           /*                        */
  EVENT_HDR_HMP_SENT_MSG,             /*                                            */
  EVENT_HDR_HMP_ABORTING_ACMAC_ACTIVATION, /*            */
  EVENT_HDR_IDLE_T_CONFIG_RSP,        /*            */
  EVENT_HDR_IDLE_T_AT_SETUP,          /*            */
  EVENT_HDR_IDLE_T_SUSPEND,           /*            */
  EVENT_HDR_IDLE_CONNECTION_DENIED,   /*            */
  EVENT_HDR_INIT_T_SYNC_ACQ,          /*            */
  EVENT_HDR_INIT_PROTOCOL_MISMATCH,   /*            */
  EVENT_HDR_OVHD_INFO_CURRENT,        /*            */
  EVENT_HDR_OVHD_T_QC_SUPERVISION,    /*            */
  EVENT_HDR_OVHD_T_SP_SUPERVISION,    /*            */
  EVENT_HDR_OVHD_T_AP_SUPERVISION,    /*            */
  EVENT_HDR_OVHD_IGNORED_MSG_UNEXPECTED_LINK, /*                        
                                                                               
                                                                       */
  EVENT_HDR_OVHD_IGNORED_SP_MSG_DIFF_SEC_SIG, /*                           */
  EVENT_HDR_OVHD_IGNORED_AP_MSG_DIFF_ACC_SIG, /*                           */
  EVENT_HDR_OVHD_IGNORED_SP_MSG_DIFF_SEC_ID,  /*            */
  EVENT_HDR_OVHD_SP_MSG_RX,           /*            */
  EVENT_HDR_OVHD_AP_MSG_RX,           /*            */
  EVENT_HDR_RUP_T_CONNECTION_SETUP,   /*            */
  EVENT_HDR_SLP_MAX_RETRIES,          /*               */
  EVENT_HDR_LMAC_ACQ_FAIL_PILOT,      /*            */
  EVENT_HDR_LMAC_ACQ_SUCCESS,         /*            */
  EVENT_HDR_LMAC_NETWORK_LOST,        /*            */
  EVENT_HDR_LMAC_IDLE_HO,             /*                     */
  EVENT_HDR_LMAC_CHAN_CHANGE_COMPLETE, /*            */
  EVENT_HDR_LMAC_ACCESS_HO_NEEDED,    /*                           */
  EVENT_HDR_LMAC_ACCESS_HO_COMPLETE,  /*                     */
  EVENT_HDR_LMAC_ACQUIRE,             /*                 */
  EVENT_HDR_LMAC_CHANGING_CC_HASH,    /*                  */
  EVENT_HDR_LMAC_IDLE_CHAN_CHANGE,    /*                   */
  EVENT_HDR_CMAC_T_SUPERVISION,       /*            */
  EVENT_HDR_AMAC_START_ACCESS,        /*            */
  EVENT_HDR_AMAC_PROBING_STOPPED,     /*            */
  EVENT_HDR_AMAC_ACCESS_COMPLETE,     /*            */
  EVENT_HDR_AMAC_ACCESS_ABORTED,      /*            */
  EVENT_HDR_AMAC_MAX_PROBES,          /*            */
  EVENT_HDR_FMAC_DROP_PKT,            /*            */
  EVENT_HDR_RMAC_T_RATE_LIMIT,        /*            */
  EVENT_HDR_RMAC_TX_STARTED,          /*            */
  EVENT_HDR_RMAC_TX_STOPPED,          /*            */
  EVENT_HDR_SMP_T_KEEP_ALIVE,         /*            */
  EVENT_HDR_AMP_ASSIGN_MSG_IGNORED_FRESH, /*            */
  EVENT_HDR_AMP_T_AT_RESPONSE,        /*            */
  EVENT_HDR_AMP_T_DUAL_ADDRESS,       /*            */
  EVENT_HDR_SCP_BEGIN_CONFIGURATION,  /*            */
  EVENT_HDR_SCP_T_CONFIG_RSP,         /*            */
  EVENT_HDR_SCP_T_AN_INIT_STATE,      /*            */

  EVENT_WCDMA_L1_STATE,               /*                   */
  EVENT_WCDMA_IMSI,                   /*                */
  EVENT_GSM_L1_STATE,                 /*                       */
  EVENT_RANDOM_ACCESS_REQUEST,        /*                                     */
  EVENT_HIGH_LEVEL_CALL_PROCESSING_STATE_CHANGE, /*                      */
                                                 /*                                                */
  EVENT_ENCRYPTION_FAILURE,                  /*                        */
  EVENT_ACCT_BLOCKED,                        /*                        */
  EVENT_COMMON_CHANNEL_MONITORED,            /*                            */
  EVENT_SOFT_HANDOFF_V2,                     /*                             */
  EVENT_HARD_HANDOFF_FREQ_CHANGE_V2,         /*                             */
  EVENT_HARD_HANDOFF_FRAME_OFFSET_CHANGE_V2, /*                             */
  EVENT_HARD_HANDOFF_DISJOINT_ASET_V2,       /*                             */
  EVENT_WCDMA_NEW_REFERENCE_CELL,
  EVENT_CALL_CONTROL_CONREF_CHANGE,          /*                            */

  EVENT_GPS_SESSION_BEGIN,
  EVENT_GPS_SESSION_END,
  EVENT_GPS_WAITING_ON_SA,
  EVENT_GPS_PPM_START,
  EVENT_GPS_PPM_RESULTS,
  EVENT_GPS_PPM_END,
  EVENT_GPS_VISIT_BEGIN,
  EVENT_GPS_VISIT_END,
  EVENT_GPS_CDMA_RESUMED_AFTER_GPS_VISIT,
  EVENT_GPS_PD_SESSION_BEGIN,
  EVENT_GPS_PD_SESSION_END,                  /*                               */
  EVENT_GPS_IS801_RX,                        /*                          */
  EVENT_GPS_IS801_TX,                        /*                          */
  EVENT_POWERUP,
  EVENT_WCDMA_ASET,
  EVENT_CM_CALL_STATE,                       /*                                      */
  EVENT_CM_OPERATIONAL_MODE,                 /*                           */
  EVENT_CM_SYSTEM_MODE,                      /*                            */

  EVENT_DEEP_SLEEP,                          /*            */
  EVENT_WAKEUP,                              /*                                 */
  EVENT_ACQUISITION_MODE,                    /*                                */
  EVENT_ACQUISITION_TYPE,                    /*                                */
  EVENT_ACP_EXIT,                            /*                                */
  EVENT_CDMA_EXIT,                           /*                                */

  EVENT_HDR_HYBRID_POWER_SAVE,               /*            */
  EVENT_HDR_DEEP_SLEEP,                      /*            */
  EVENT_HDR_RESELECTION,                     /*            */
  EVENT_SAM_LOCK_GRANTED,                    /*                                        */
  EVENT_SAM_LOCK_RELEASED,                   /*                    */

  EVENT_GSM_HANDOVER_START,
  EVENT_GSM_HANDOVER_END,
  EVENT_GSM_LINK_FAILURE,
  EVENT_GSM_RESELECT_START,
  EVENT_GSM_RESELECT_END,
  EVENT_GSM_CAMP_ATTEMPT_START,
  EVENT_GSM_RR_IN_SERVICE,
  EVENT_GSM_RR_OUT_OF_SERVICE,
  EVENT_GSM_PAGE_RECEIVED,
  EVENT_GSM_CAMP_ATTEMPT_END,
  EVENT_GPS_IS801_TIMEOUT,
  EVENT_GPS_IS801_DISCARD,                   /*                          */
  EVENT_GSM_CELL_SELECTION_START,            /*                                       */
  EVENT_GSM_CELL_SELECTION_END,              /*                                                   */
  EVENT_GSM_POWER_SCAN_STATUS,               /*                                               */
  EVENT_GSM_PLMN_LIST_START,                 /*                 */
  EVENT_GSM_PLMN_LIST_END,
  EVENT_WCDMA_INTER_RAT_HANDOVER_START,      /*                                                  */
  EVENT_WCDMA_INTER_RAT_HANDOVER_END,        /*                                          */

  EVENT_GSM_MESSAGE_SENT,                    /*                                                     */
  EVENT_GSM_MESSAGE_RECEIVED,                /*                                                     */
  EVENT_GSM_TIMER_EXPIRED,                   /*                                                     */
  EVENT_GSM_COUNTER_EXPIRED,                 /*                                                     */

  EVENT_NAS_MESSAGE_SENT,
  EVENT_NAS_MESSAGE_RECEIVED,
  EVENT_RRC_MESSAGE_SENT,
  EVENT_RRC_MESSAGE_RECEIVED,
                                    /*                           */
  EVENT_CAMERA_CANNOT_CAPTURE,      /*                                     */
  EVENT_CAMERA_CANNOT_CONFIG_JPEG,  /*                                         */
  EVENT_CAMERA_CANNOT_CONFIG_VFE,   /*                                        */
  EVENT_CAMERA_CANNOT_ENCODE,       /*                                    */
  EVENT_CAMERA_CANNOT_IDLE_DSP,     /*                                      */
  EVENT_CAMERA_CANNOT_LOAD_DSP,     /*                                      */
  EVENT_CAMERA_DSP_FATAL,           /*                                      */
  EVENT_CAMERA_DSP_REQ_ILLEGAL,     /*                                          */
  EVENT_CAMERA_EFS_FAILED,          /*                                 */
  EVENT_CAMERA_EXIT,                /*                           */
  EVENT_CAMERA_FORMAT_NOT_SUPPORTED,/*                      */
  EVENT_CAMERA_FUNCTION_REJECTED,   /*                                        */
  EVENT_CAMERA_IMAGE_CORRUPT,       /*                           */
  EVENT_CAMERA_INVALID_CONFIG_PARM, /*                               */
  EVENT_CAMERA_INVALID_SET_ID,      /*                           */
  EVENT_CAMERA_INVALID_STATE,       /*               */
  EVENT_CAMERA_JPEG_ENCODED,        /*              */
  EVENT_CAMERA_NO_MEMORY,           /*                                */
  EVENT_CAMERA_NO_PICTURE,          /*                   */
  EVENT_CAMERA_PICTURE_SAVED,       /*                                    */
  EVENT_CAMERA_PICTURE_TAKEN,       /*                                    */
  EVENT_CAMERA_PREVIEW,             /*               */
  EVENT_CAMERA_RECORD,              /*                 */
  EVENT_CAMERA_SAVE_PICTURE,        /*                    */
  EVENT_CAMERA_SET_FAILED,          /*            */
  EVENT_CAMERA_SET_SUCCEEDED,       /*               */
  EVENT_CAMERA_START,               /*                            */
  EVENT_CAMERA_STOP,                /*                          */
  EVENT_CAMERA_TAKE_PICTURE,        /*                    */

  EVENT_DIAG_STRESS_TEST_NO_PAYLOAD,
  EVENT_DIAG_STRESS_TEST_WITH_PAYLOAD,

  EVENT_CM_CALL_ORIG_START_P1,      /*                         */
  EVENT_CM_CALL_ORIG_START_P2,      /*                         */
  EVENT_CM_CALL_ORIG_START_P3,      /*                         */
  EVENT_CM_CALL_ORIG_SWITCH_TO_HDR, /*            */
  EVENT_CM_CALL_ORIG_REDIAL,        /*                  */
  EVENT_CM_CALL_ORIG_SEND_HDR_ORIG, /*            */
  EVENT_CM_CALL_ORIG_SEND_MC_ORIG,  /*            */
  EVENT_CM_CALL_ORIG_END,           /*                  */
  EVENT_CM_CALL_ORIG_CONNECTED,     /*                  */

  EVENT_MT_SMS_NOTIFY,              /*                                            */
  EVENT_SMS_SLOT_WAKEUP,            /*                           */
  EVENT_MO_SMS_STATUS,              /*                                                 
                                                                                        
                                                                 */
  EVENT_GPRS_SURROUND_SEARCH_START,
  EVENT_GPRS_SURROUND_SEARCH_END,
  EVENT_GPRS_MAC_RESELECT_IND,
  EVENT_GPRS_PAGE_RECEIVED,
  EVENT_GPRS_LINK_FAILURE,
  EVENT_GPRS_CELL_UPDATE_START,
  EVENT_GPRS_CELL_UPDATE_END,
  EVENT_GPRS_EARLY_CAMPING,
  EVENT_PACKET_RANDOM_ACCESS_REQ,
  EVENT_GPRS_MAC_MSG_SENT,          /*                                                      */
  EVENT_GPRS_MAC_MSG_RECEIVED,      /*                                                      */
  EVENT_GPRS_SMGMM_MSG_SENT,        /*                                                      */
  EVENT_GPRS_SMGMM_MSG_RECEIVED,    /*                                                      */

  EVENT_CP_MATCHED_MSG,             /*                 */
  EVENT_PREF_SYS_RESEL,             /*               */ 

  EVENT_WCDMA_LAYER1_PRACH,         /*                  */
  EVENT_WCDMA_LAYER1_MEASUREMENT,   /*                      */

  EVENT_MOBILITY_MANAGEMENT_STATE_CHANGE, /*                                             */

  EVENT_LSM_STATE_CHANGE,           /*                 */
  EVENT_RLP,                        /*                  */

  EVENT_CM_MODE_PREF,               /*                  */
  EVENT_CM_BAND_PREF,               /*                  */
  EVENT_CM_ROAM_PREF,               /*                  */
  EVENT_CM_SRV_DOMAIN_PREF,         /*                  */
  EVENT_CM_GW_ACQ_ORDER_PREF,       /*                  */
  EVENT_CM_HYBRID_PREF,             /*                  */
  EVENT_CM_NETWORK_SEL_MODE_PREF,   /*                  */

  EVENT_WCDMA_L1_SUSPEND,                 /*                  */
  EVENT_WCDMA_L1_RESUME,                  /*                  */
  EVENT_WCDMA_L1_STOPPED,                 /*                  */
  EVENT_WCDMA_TO_WCDMA_RESELECTION_START, /*                  */
  EVENT_WCDMA_TO_GSM_RESELECTION_START,   /*                  */
  EVENT_WCDMA_TO_GSM_RESELECTION_END,     /*                  */
  EVENT_WCDMA_TO_WCDMA_RESELECTION_END,   /*                  */
  EVENT_WCDMA_RACH_ATTEMPT,               /*                  */

  EVENT_START_FWD_SUPP_BURST_ASSIGN,      /*                  */
  EVENT_START_REV_SUPP_BURST_ASSIGN,      /*                  */
  EVENT_REV_FCH_GATING_IN_USE,            /*                  */
  EVENT_PPP,                              /*                  */
  EVENT_MIP,                              /*                  */
  EVENT_TCP,                              /*                  */
  EVENT_CAMERA_EXIF_FAILED,               /*                      */
  EVENT_CAMERA_VIDEO_FAILED,              /*                       */
  EVENT_CAMERA_NO_SENSOR,                 /*           */
  EVENT_CAMERA_ABORT,                     /*                   */

  EVENT_CM_BLOCK_HDR_ORIG_DURING_GPS,     /*            */
  EVENT_CM_ALLOW_HDR_ORIG_DURING_GPS,     /*            */

  EVENT_GSM_AMR_STATE_CHANGE,             /*                 */
  EVENT_GSM_RATSCCH_IN_DTX,               /*            */
  EVENT_GSM_FACCH_IN_DTX,                 /*            */
  EVENT_GSM_FACCH_AND_RATSCCH_COLLISION,  /*            */
  EVENT_GSM_FACCH_AND_SID_UPDATE_COLLISION,  /*            */
  EVENT_GSM_RATSCCH_AND_SID_UPDATE_COLLISION,/*            */
  EVENT_GSM_RATSCCH_CMI_PHASE_CHANGE,     /*                 */
  EVENT_GSM_RATSCCH_REQ_ACT_TIMER_EXPIRY, /*                 */
  EVENT_GSM_RATSCCH_ACK_ACT_TIMER_EXPIRY, /*                 */
  EVENT_GSM_AMR_CMC_TURNAROUND_TIME,      /*                 */
  EVENT_CM_PLMN_FOUND,                    /*          */
  EVENT_CM_SERVICE_CONFIRMED,             /*          */

  EVENT_GPRS_MAC_CAMPED_ON_CELL,          /*                            */
  EVENT_GPRS_LLC_READY_TIMER_START,       /*                            */
  EVENT_GPRS_LLC_READY_TIMER_END,         /*                            */

  EVENT_WCDMA_PHYCHAN_ESTABLISHED,        /*             */
  EVENT_HS_DISPLAY_BMP_CAPTURE_STATUS,    /*                 */

  EVENT_WCDMA_CELL_SELECTED,              /*                */
  EVENT_WCDMA_PAGE_RECEIVED,              /*                */
  EVENT_WCDMA_SEND_KEY,                   /*             */
  EVENT_WCDMA_RL_FAILURE,                 /*            */
  EVENT_WCDMA_MAX_RESET,                  /*                */
  EVENT_WCDMA_CALL_SETUP,                 /*             */
  EVENT_WCDMA_CALL_DROPPED,               /*             */
  EVENT_WCDMA_RRC_STATE,                  /*                */
  EVENT_GPS_PD_CONNECTION_TIMEOUT,        /*            */
  EVENT_GPS_PD_DISCONNECTION_COMPLETE,    /*            */

  EVENT_MEDIA_PLAYER_START,               /*                                                */
  EVENT_MEDIA_PLAYER_STOP,                /*                                                 */
  EVENT_MEDIA_PLAYER_SEEK,                /*                                              */

  EVENT_GPS_SRCH_START,             /*                                */

  EVENT_GPS_SRCH_END,               /*            */
  EVENT_GPS_PPM_PAUSE,              /*                                */
  EVENT_GPS_PPM_RESUME,             /*            */
  EVENT_GPS_SA_RECEIVED,            /*                                 
                                                                    
                                    */
  EVENT_GPS_CLK_ON,                 /*            */
  EVENT_GPS_CLK_OFF,                /*            */
  EVENT_GPS_VISIT_REQUEST,          /*            */ 
  EVENT_GPS_VISIT_RESPONSE,         /*                                  
                                    */ 
  EVENT_GPS_TA_START,               /*            */
  EVENT_GPS_DSP_READY,              /*            */
  EVENT_GPS_DSP_CHANNEL_START,      /*                                  
                                                                        
                                                                       
                                                                      
                                    */
  EVENT_GPS_DSP_CHANNEL_DONE,       /*                               */
  EVENT_GPS_DSP_STOP,               /*            */
  EVENT_GPS_DSP_DONE,               /*            */
  EVENT_GPS_TB_END,                 /*            */
  EVENT_GPS_SRCH_LARGE_DOPP_WIN,    /*                              
                                                                   
                                                                  
                                    */ 
  EVENT_GPS_SRCH_EXCEPTION,         /*                               
                                                                       
                                    */
  EVENT_GPS_SRCH_HW_POLLING1,       /*                           
                                                                 
                                                                 
                                                                
                                                                
                                    */

  EVENT_GPS_SRCH_HW_POLLING2,       /*                         
                                    */
  EVENT_GPS_PGI_ACTION_PROCESS,     /*                                
                                                                
                                    */
  EVENT_GPS_GSC_ACTION_PROCESS,     /*                                
                                                                
                                    */
  EVENT_GPS_PGI_ABORT,              /*                               */
  EVENT_GPS_GSC_ABORT,              /*                               */



  EVENT_GPS_PD_FIX_START,                /*                                   
                                                                       
                                         */
  EVENT_GPS_PD_FIX_END,                  /*                                   */
  EVENT_GPS_DATA_DOWNLOAD_START,         /*                               
                                                                              */
  EVENT_GPS_DATA_DOWNLOAD_END,           /*                                   */
  EVENT_GPS_PD_SESSION_START,            /*                                 
                                                                            
                                                                            
                                                                            
                                                                             
                                                                             
                                                                            
                                                                             
                                                                             
                                                                              */
  EVENT_GPS_DORMANCY_BEGIN,               /*            */
  EVENT_GPS_DORMANCY_END,                 /*            */
  EVENT_GPS_PRQ_TIMEOUT,                  /*            */
  EVENT_GPS_PD_CONNECTION_START,          /*            */
  EVENT_GPS_PD_CONNECTION_ESTABLISHED,    /*            */
  EVENT_GPS_PD_DISCONNECTION_START,       /*            */
  EVENT_GPS_FTEST_FIX_START,              /*                            */
  EVENT_GPS_FTEST_FIX_END,                /*                            */
  EVENT_GPS_PD_POSITION,                  /*            */
  EVENT_GPS_E911_START,                   /*            */
  EVENT_GPS_E911_END,                     /*            */
  EVENT_GPS_DBM_SEND_FAILURE,             /*            */
  EVENT_GPS_UAPDMS_STATE_CHANGE,          /*                            
                                                                         */
  EVENT_WCDMA_OUT_OF_SERVICE,             /*            */
  EVENT_GSM_L1_SUBSTATE,                  /*                 */
  EVENT_SD_EVENT_ACTION,                  /*                */
  EVENT_SD_EVENT_ACTION_HYBR,             /*                */

  EVENT_UMTS_CALLS_STATISTICS,            /*                */
  EVENT_PZID_HAT_STARTED,                 /*            */
  EVENT_WCDMA_DRX_CYCLE,                  /*                */
  EVENT_WCDMA_RE_ACQUISITION_FAIL,        /*            */
  EVENT_WCDMA_RRC_RB0_SETUP_FAILURE,      /*            */
  EVENT_WCDMA_RRC_PHYCHAN_EST_FAILURE,    /*            */
  EVENT_CM_CALL_EVENT_ORIG,               /*                */
  EVENT_CM_CALL_EVENT_CONNECT,            /*                */
  EVENT_CM_CALL_EVENT_END,                /*                */
  EVENT_CM_ENTER_EMERGENCY_CB,            /*            */
  EVENT_CM_EXIT_EMERGENCY_CB,             /*            */
  EVENT_PZID_HAT_EXPIRED,                 /*            */
  EVENT_HDR_SMP_SESSION_CLOSED,           /*                */
  EVENT_WCDMA_MEMORY_LEAK,                /*            */
  EVENT_PZID_HT_STARTED,                  /*                */
  EVENT_PZID_HT_EXPIRED,                  /*                */
  EVENT_ACCESS_ENTRY_HANDOFF,             /*                */

  EVENT_BREW_APP_START,                   /*                */
  EVENT_BREW_APP_STOP,                    /*                */
  EVENT_BREW_APP_PAUSE,                   /*                */
  EVENT_BREW_APP_RESUME,                  /*                */
  EVENT_BREW_EXT_MODULE_START,            /*                */
  EVENT_BREW_EXT_MODULE_STOP,             /*                */
  EVENT_BREW_ERROR,                       /*                */
  EVENT_BREW_RESERVED_647,                /*                        */
  EVENT_BREW_RESERVED_648,                /*                        */
  EVENT_BREW_RESERVED_649,                /*                        */
  EVENT_BREW_RESERVED_650,                /*                        */
  EVENT_BREW_RESERVED_651,                /*                        */
  EVENT_BREW_RESERVED_652,                /*                        */
  EVENT_BREW_RESERVED_653,                /*                        */
  EVENT_BREW_RESERVED_654,                /*                        */
  EVENT_BREW_RESERVED_655,                /*                        */
  EVENT_BREW_USER_656,                    /*                           */
  EVENT_BREW_GENERIC,                     /*                              */
  EVENT_BREW_MEDIAPLAYER_SELECT_FILE,     /*            */
  EVENT_BREW_MEDIAPLAYER_CONTROL,         /*            */
  EVENT_BREW_APP_FORMITEM_STACK_CHANGE,   /*                */
  EVENT_BREW_CATAPP_RECV_PROACTIVE_CMD,   /*            */
  EVENT_BREW_CATAPP_TERMINAL_RSP,         /*            */
  EVENT_BREW_CATAPP_NO_DISPLAY,           /*            */
  EVENT_BREW_SIRIUS_EMAIL_DELETE,                  /*            */
  EVENT_BREW_SIRIUS_EMAIL_OPERATION_COMPLETE,      /*                */
  EVENT_BREW_SIRIUS_EMAIL_NEW_EMAIL_NOTIFICATION,  /*            */
  EVENT_BREW_UNDEFINED_667,
  EVENT_BREW_UNDEFINED_668,
  EVENT_BREW_UNDEFINED_669,
  EVENT_BREW_UNDEFINED_670,
  EVENT_BREW_UNDEFINED_671,
  EVENT_BREW_UNDEFINED_672,
  EVENT_BREW_UNDEFINED_673,
  EVENT_BREW_UNDEFINED_674,
  EVENT_BREW_UNDEFINED_675,
  EVENT_BREW_UNDEFINED_676,
  EVENT_BREW_UNDEFINED_677,
  EVENT_BREW_UNDEFINED_678,
  EVENT_BREW_UNDEFINED_679,
  EVENT_BREW_UNDEFINED_680,
  EVENT_BREW_UNDEFINED_681,
  EVENT_BREW_UNDEFINED_682,
  EVENT_BREW_UNDEFINED_683,
  EVENT_BREW_UNDEFINED_684,
  EVENT_BREW_UNDEFINED_685,
  EVENT_BREW_UNDEFINED_686,
  EVENT_BREW_UNDEFINED_687,
  EVENT_BREW_UNDEFINED_688,
  EVENT_BREW_UNDEFINED_689,
  EVENT_BREW_UNDEFINED_690,
  EVENT_BREW_UNDEFINED_691,
  EVENT_BREW_UNDEFINED_692,
  EVENT_BREW_UNDEFINED_693,
  EVENT_BREW_UNDEFINED_694,
  EVENT_BREW_UNDEFINED_695,
  EVENT_BREW_UNDEFINED_696,
  EVENT_BREW_UNDEFINED_697,
  EVENT_BREW_UNDEFINED_698,
  EVENT_BREW_UNDEFINED_699,
  EVENT_BREW_UNDEFINED_700,
  EVENT_BREW_UNDEFINED_701,
  EVENT_BREW_UNDEFINED_702,
  EVENT_BREW_UNDEFINED_703,
  EVENT_BREW_UNDEFINED_704,
  EVENT_BREW_UNDEFINED_705,
  EVENT_BREW_UNDEFINED_706,
  EVENT_BREW_UNDEFINED_707,
  EVENT_BREW_UNDEFINED_708,
  EVENT_BREW_UNDEFINED_709,
  EVENT_BREW_UNDEFINED_710,
  EVENT_BREW_UNDEFINED_711,
  EVENT_BREW_UNDEFINED_712,
  EVENT_BREW_UNDEFINED_713,
  EVENT_BREW_UNDEFINED_714,
  EVENT_BREW_UNDEFINED_715,
  EVENT_BREW_UNDEFINED_716,
  EVENT_BREW_UNDEFINED_717,
  EVENT_BREW_UNDEFINED_718,
  EVENT_BREW_UNDEFINED_719,
  EVENT_BREW_UNDEFINED_720,
  EVENT_BREW_UNDEFINED_721,
  EVENT_BREW_UNDEFINED_722,
  EVENT_BREW_UNDEFINED_723,
  EVENT_BREW_UNDEFINED_724,
  EVENT_BREW_UNDEFINED_725,
  EVENT_BREW_UNDEFINED_726,
  EVENT_BREW_UNDEFINED_727,
  EVENT_BREW_UNDEFINED_728,
  EVENT_BREW_UNDEFINED_729,
  EVENT_BREW_UNDEFINED_730,
  EVENT_BREW_UNDEFINED_731,
  EVENT_BREW_UNDEFINED_732,
  EVENT_BREW_UNDEFINED_733,
  EVENT_BREW_UNDEFINED_734,
  EVENT_BREW_UNDEFINED_735,
  EVENT_BREW_UNDEFINED_736,
  EVENT_BREW_UNDEFINED_737,
  EVENT_BREW_UNDEFINED_738,
  EVENT_BREW_UNDEFINED_739,
  EVENT_BREW_UNDEFINED_740,
  EVENT_BREW_UNDEFINED_741,
  EVENT_BREW_UNDEFINED_742,
  EVENT_BREW_UNDEFINED_743,
  EVENT_BREW_UNDEFINED_744,
  EVENT_BREW_UNDEFINED_745,
  EVENT_BREW_UNDEFINED_746,
  EVENT_BREW_UNDEFINED_747,
  EVENT_BREW_UNDEFINED_748,
  EVENT_BREW_UNDEFINED_749,
  EVENT_BREW_UNDEFINED_750,
  EVENT_BREW_UNDEFINED_751,
  EVENT_BREW_UNDEFINED_752,
  EVENT_BREW_UNDEFINED_753,
  EVENT_BREW_UNDEFINED_754,
  EVENT_BREW_UNDEFINED_755,
  EVENT_BREW_UNDEFINED_756,
  EVENT_BREW_UNDEFINED_757,
  EVENT_BREW_UNDEFINED_758,
  EVENT_BREW_UNDEFINED_759,
  EVENT_BREW_UNDEFINED_760,
  EVENT_BREW_UNDEFINED_761,
  EVENT_BREW_UNDEFINED_762,
  EVENT_BREW_UNDEFINED_763,
  EVENT_BREW_UNDEFINED_764,
  EVENT_BREW_UNDEFINED_765,
  EVENT_BREW_UNDEFINED_766,
  EVENT_BREW_UNDEFINED_767,
  EVENT_BREW_UNDEFINED_768,
  EVENT_BREW_UNDEFINED_769,
  EVENT_BREW_UNDEFINED_770,
  EVENT_BREW_UNDEFINED_771,
  EVENT_BREW_UNDEFINED_772,
  EVENT_BREW_UNDEFINED_773,
  EVENT_BREW_UNDEFINED_774,
  EVENT_BREW_UNDEFINED_775,
  EVENT_BREW_UNDEFINED_776,
  EVENT_BREW_UNDEFINED_777,
  EVENT_BREW_UNDEFINED_778,
  EVENT_BREW_UNDEFINED_779,
  EVENT_BREW_UNDEFINED_780,
  EVENT_BREW_UNDEFINED_781,
  EVENT_BREW_UNDEFINED_782,
  EVENT_BREW_UNDEFINED_783,
  EVENT_BREW_UNDEFINED_784,
  EVENT_BREW_UNDEFINED_785,
  EVENT_BREW_UNDEFINED_786,
  EVENT_BREW_UNDEFINED_787,
  EVENT_BREW_UNDEFINED_788,
  EVENT_BREW_UNDEFINED_789,
  EVENT_BREW_UNDEFINED_790,
  EVENT_BREW_UNDEFINED_791,
  EVENT_BREW_UNDEFINED_792,
  EVENT_BREW_UNDEFINED_793,
  EVENT_BREW_UNDEFINED_794,
  EVENT_BREW_UNDEFINED_795,
  EVENT_BREW_UNDEFINED_796,
  EVENT_BREW_UNDEFINED_797,
  EVENT_BREW_UNDEFINED_798,
  EVENT_BREW_UNDEFINED_799,
  EVENT_BREW_UNDEFINED_800,
  EVENT_BREW_UNDEFINED_801,
  EVENT_BREW_UNDEFINED_802,
  EVENT_BREW_UNDEFINED_803,
  EVENT_BREW_UNDEFINED_804,
  EVENT_BREW_UNDEFINED_805,
  EVENT_BREW_UNDEFINED_806,
  EVENT_BREW_UNDEFINED_807,
  EVENT_BREW_UNDEFINED_808,
  EVENT_BREW_UNDEFINED_809,
  EVENT_BREW_UNDEFINED_810,
  EVENT_BREW_UNDEFINED_811,
  EVENT_BREW_UNDEFINED_812,
  EVENT_BREW_UNDEFINED_813,
  EVENT_BREW_UNDEFINED_814,
  EVENT_BREW_UNDEFINED_815,
  EVENT_BREW_UNDEFINED_816,
  EVENT_BREW_UNDEFINED_817,
  EVENT_BREW_UNDEFINED_818,
  EVENT_BREW_UNDEFINED_819,
  EVENT_BREW_UNDEFINED_820,
  EVENT_BREW_UNDEFINED_821,
  EVENT_BREW_UNDEFINED_822,
  EVENT_BREW_UNDEFINED_823,
  EVENT_BREW_UNDEFINED_824,
  EVENT_BREW_UNDEFINED_825,
  EVENT_BREW_UNDEFINED_826,
  EVENT_BREW_UNDEFINED_827,
  EVENT_BREW_UNDEFINED_828,
  EVENT_BREW_UNDEFINED_829,
  EVENT_BREW_UNDEFINED_830,
  EVENT_BREW_UNDEFINED_831,
  EVENT_BREW_UNDEFINED_832,
  EVENT_BREW_UNDEFINED_833,
  EVENT_BREW_UNDEFINED_834,
  EVENT_BREW_UNDEFINED_835,
  EVENT_BREW_UNDEFINED_836,
  EVENT_BREW_UNDEFINED_837,
  EVENT_BREW_UNDEFINED_838,
  EVENT_BREW_UNDEFINED_839,
  EVENT_BREW_UNDEFINED_840,
  EVENT_BREW_UNDEFINED_841,
  EVENT_BREW_UNDEFINED_842,
  EVENT_BREW_UNDEFINED_843,
  EVENT_BREW_UNDEFINED_844,
  EVENT_BREW_UNDEFINED_845,
  EVENT_BREW_UNDEFINED_846,
  EVENT_BREW_UNDEFINED_847,
  EVENT_BREW_UNDEFINED_848,
  EVENT_BREW_UNDEFINED_849,
  EVENT_BREW_UNDEFINED_850,
  EVENT_BREW_UNDEFINED_851,
  EVENT_BREW_UNDEFINED_852,
  EVENT_BREW_UNDEFINED_853,
  EVENT_BREW_UNDEFINED_854,
  EVENT_BREW_UNDEFINED_855,
  EVENT_BREW_UNDEFINED_856,
  EVENT_BREW_UNDEFINED_857,
  EVENT_BREW_UNDEFINED_858,
  EVENT_BREW_UNDEFINED_859,
  EVENT_BREW_UNDEFINED_860,
  EVENT_BREW_UNDEFINED_861,
  EVENT_BREW_UNDEFINED_862,
  EVENT_BREW_UNDEFINED_863,
  EVENT_BREW_UNDEFINED_864,
  EVENT_BREW_UNDEFINED_865,
  EVENT_BREW_UNDEFINED_866,
  EVENT_BREW_UNDEFINED_867,
  EVENT_BREW_UNDEFINED_868,
  EVENT_BREW_UNDEFINED_869,
  EVENT_BREW_UNDEFINED_870,
  EVENT_BREW_UNDEFINED_871,
  EVENT_BREW_UNDEFINED_872,
  EVENT_BREW_UNDEFINED_873,
  EVENT_BREW_UNDEFINED_874,
  EVENT_BREW_UNDEFINED_875,
  EVENT_BREW_UNDEFINED_876,
  EVENT_BREW_UNDEFINED_877,
  EVENT_BREW_UNDEFINED_878,
  EVENT_BREW_UNDEFINED_879,
  EVENT_BREW_UNDEFINED_880,
  EVENT_BREW_UNDEFINED_881,
  EVENT_BREW_UNDEFINED_882,
  EVENT_BREW_UNDEFINED_883,
  EVENT_BREW_UNDEFINED_884,
  EVENT_BREW_UNDEFINED_885,
  EVENT_BREW_UNDEFINED_886,
  EVENT_BREW_UNDEFINED_887,
  EVENT_BREW_UNDEFINED_888,
  EVENT_BREW_UNDEFINED_889,
  EVENT_BREW_UNDEFINED_890,
  EVENT_BREW_UNDEFINED_891,
  EVENT_BREW_UNDEFINED_892,
  EVENT_BREW_UNDEFINED_893,
  EVENT_BREW_UNDEFINED_894,
  EVENT_BREW_UNDEFINED_895,

  EVENT_WCDMA_PS_DATA_RATE,                  /*                */
  EVENT_GSM_TO_WCDMA_RESELECT_END,           /*                */
  EVENT_PZID_HAI_ENABLED,                    /*           */
  EVENT_PZID_HAI_DISABLED,                   /*           */
  EVENT_GSM_TO_WCDMA_HANDOVER_START,         /*                */
  EVENT_WCDMA_RRC_MODE,                      /*                */
  EVENT_WCDMA_L1_ACQ_SUBSTATE,               /*                */
  EVENT_WCDMA_PHYCHAN_CFG_CHANGED,           /*                */

  EVENT_QTV_CLIP_STARTED,                    /*                */
  EVENT_QTV_CLIP_ENDED,                      /*                */
  EVENT_QTV_SDP_PARSER_REJECT,               /*            */
  EVENT_QTV_CLIP_PAUSE,                      /*                */
  EVENT_QTV_CLIP_REPOSITIONING,              /*                */
  EVENT_QTV_CLIP_ZOOM_IN,                    /*            */
  EVENT_QTV_CLIP_ZOOM_OUT,                   /*            */
  EVENT_QTV_CLIP_ROTATE,                     /*                */
  EVENT_QTV_CLIP_PAUSE_RESUME,               /*                */
  EVENT_QTV_CLIP_REPOSITION_RESUME,          /*                */
  EVENT_QTV_DSP_INIT,                        /*            */
  EVENT_QTV_STREAMING_SERVER_URL,            /*                 */
  EVENT_QTV_SERVER_PORTS_USED,               /*                */
  EVENT_QTV_USING_PROXY_SERVER,              /*                */
  EVENT_QTV_STREAMER_STATE_IDLE,             /*            */
  EVENT_QTV_STREAMER_STATE_CONNECTING,       /*            */
  EVENT_QTV_STREAMER_STATE_SETTING_TRACKS,   /*            */
  EVENT_QTV_STREAMER_STATE_STREAMING,        /*            */
  EVENT_QTV_STREAMER_STATE_PAUSED,           /*            */
  EVENT_QTV_STREAMER_STATE_SUSPENDED,        /*            */
  EVENT_QTV_STREAMER_CONNECTED,              /*            */
  EVENT_QTV_STREAMER_INITSTREAM_FAIL,        /*            */
  EVENT_QTV_BUFFERING_STARTED,               /*                */
  EVENT_QTV_BUFFERING_ENDED,                 /*                */
  EVENT_QTV_CLIP_FULLSCREEN,                 /*            */
  EVENT_QTV_PS_DOWNLOAD_STARTED,             /*                */
  EVENT_QTV_PSEUDO_STREAM_STARTED,           /*            */
  EVENT_QTV_PS_PLAYER_STATE_PSEUDO_PAUSE,    /*            */
  EVENT_QTV_PS_PLAYER_STATE_PSEUDO_RESUME,   /*                */
  EVENT_QTV_PARSER_STATE_READY,              /*                 */
  EVENT_QTV_FRAGMENT_PLAYBACK_BEGIN,         /*                */
  EVENT_QTV_FRAGMENT_PLAYBACK_COMPLETE,      /*                */
  EVENT_QTV_PARSER_STATE_PSEUDO_PAUSE,       /*            */
  EVENT_QTV_PLAYER_STATE_PSEUDO_PAUSE,       /*            */
  EVENT_QTV_PARSER_STATE_PSEUDO_RESUME,      /*                */
  EVENT_QTV_PLAYER_STATE_PSEUDO_RESUME,      /*                */
  EVENT_QTV_FRAGMENTED_FILE_DECODE_START,    /*                */
  EVENT_QTV_FRAGMENTED_FILE_END_SUCCESS,     /*                */
  EVENT_QTV_DOWNLOAD_DATA_REPORT,            /*                */
  EVENT_QTV_VDEC_DIAG_DECODE_CALLBACK,       /*                */
  EVENT_QTV_URL_PLAYED_IS_MULTICAST,         /*            */
  EVENT_QTV_VDEC_DIAG_STATUS,                /*                */
  EVENT_QTV_STREAMING_URL_OPEN,              /*                */
  EVENT_QTV_STREAMING_URL_OPENING,           /*            */
  EVENT_QTV_CLIP_ENDED_VER2,                 /*                 */
  EVENT_QTV_SILENCE_INSERTION_STARTED,       /*            */
  EVENT_QTV_SILENCE_INSERTION_ENDED,         /*                */
  EVENT_QTV_AUDIO_CHANNEL_SWITCH_FRAME,      /*                */
  EVENT_QTV_FIRST_VIDEO_FRAME_RENDERED,      /*            */
  EVENT_QTV_FIRST_VIDEO_I_FRAME_RENDERED,    /*            */
  EVENT_QTV_SDP_SELECTED,                    /*            */
  EVENT_QTV_DIAG_PLAYER_STATUS,              /*                 */
  EVENT_QTV_SILENCE_INSERTION_DURATION,      /*                */
  EVENT_QTV_UNDEFINED_957,
  EVENT_QTV_UNDEFINED_958,
  EVENT_QTV_UNDEFINED_959,
  EVENT_QTV_UNDEFINED_960,
  EVENT_QTV_UNDEFINED_961,
  EVENT_QTV_UNDEFINED_962,
  EVENT_QTV_UNDEFINED_963,
  EVENT_QTV_UNDEFINED_964,
  EVENT_QTV_UNDEFINED_965,
  EVENT_QTV_UNDEFINED_966,
  EVENT_QTV_UNDEFINED_967,

  EVENT_DS_SETS_ARM_CLOCK_FASTER,      /*            */
  EVENT_DS_SETS_ARM_CLOCK_SLOWER,      /*            */

  EVENT_SMS_STATISTICS,                /*                */
  EVENT_SM_PDP_STATE,                  /*                */
  EVENT_MVS_STATE,                     /*                */

  EVENT_SECSSL,                        /*                 */
  EVENT_SECTEST,                       /*                 */
  EVENT_SECVPN,                        /*                 */
  EVENT_SECCRYPT,                      /*                 */
  EVENT_SECCRYPT_CMD,                  /*                 */

  EVENT_SEC_RESERVED_978,              /*                 */
  EVENT_SEC_RESERVED_979,              /*                 */
  EVENT_SEC_RESERVED_980,              /*                 */
  EVENT_SEC_RESERVED_981,              /*                 */

  EVENT_ARM_CLK_FREQUENCY_CHANGE,      /*                 */
  EVENT_ADSP_CLK_FREQUENCY_CHANGE,     /*                */
  EVENT_MDSP_CLK_FREQUENCY_CHANGE,     /*                */

  EVENT_CELL_CHANGE_INDICATION,        /*                */
  EVENT_CB_STATE_CHANGE,               /*                */
  EVENT_SMSCB_L1_STATE_CHANGE,         /*                */
  EVENT_SMSCB_L1_COLLISION,            /*                */
  EVENT_WMS_SEARCH_REQUEST,            /*                */
  EVENT_CM_GET_PASSWORD_IND,           /*                */
  EVENT_CM_PASSWORD_AUTHENTICATION_STATUS, /*                */
  EVENT_CM_USS_RESPONSE_NOTIFY_IND,    /*                */
  EVENT_CM_USS_CONF,                   /*                */
  EVENT_CM_RELEASE_USS_IND,            /*                */
  EVENT_CM_FWD_AOC_IND,                /*                */
  EVENT_PZID_ID,                       /*                */
  EVENT_PZID_HT_VALUE,                 /*                */
  EVENT_PZID_EXISTS_IN_LIST,           /*                */
  EVENT_GSDI_GET_FILE_ATTRIBUTES,      /*                */
  EVENT_GSDI_SIM_READ,                 /*                */
  EVENT_GSDI_SIM_WRITE,                /*                */
  EVENT_GSDI_GET_PIN_STATUS,           /*                */
  EVENT_GSDI_VERIFY_PIN,               /*                */
  EVENT_GSDI_UNBLOCK_PIN,              /*                */
  EVENT_GSDI_DISABLE_PIN,              /*                */
  EVENT_GSDI_ENABLE_PIN,               /*                */
  EVENT_GSDI_SIM_INCREASE,             /*                */
  EVENT_GSDI_EXECUTE_APDU_REQ,         /*                */
  EVENT_SEG_UPM_ADDR_MISMATCH,         /*                */
  EVENT_WCDMA_PRACH,                   /*                */
  EVENT_GSDI_SELECT,                   /*                */
  EVENT_WCDMA_RAB_RATE_RECONFIG,       /*                */
  EVENT_WCDMA_RLC_RESETS,              /*                */
  EVENT_WCDMA_RLC_OPEN_CLOSE,          /*                */
  EVENT_WCDMA_RLC_MRW,                 /*                */
  EVENT_QVP_APP_PROCESS_EVENT,         /*                */
  EVENT_QVP_APP_STATE_CHANGED_EVENT,   /*                */
  EVENT_QVP_APP_CALL_CONNECTED_EVENT,  /*                */
  EVENT_GSDI_CARD_EVENT_NOTIFICATION,  /*                */
  EVENT_CM_DATA_AVAILABLE,             /*                */
  EVENT_CM_DS_INTERRAT_STATE,          /*                */
  EVENT_MM_STATE,                      /*                */
  EVENT_GMM_STATE,                     /*                */
  EVENT_PLMN_INFORMATION,              /*                */
  EVENT_COREAPP_SET_VOICE_PRIVACY,     /*                */
  EVENT_COREAPP_GET_VOICE_PRIVACY,     /*                */
  EVENT_HARD_HANDOFF_LONG_CODE_MASK_CHANGE, /*                 */
  EVENT_VCTCXO_FREEZE,                 /*         */
  EVENT_VCTCXO_UNFREEZE,               /*         */
  EVENT_SMS_SLOT_WAKEUP_V2,            /*                */
  EVENT_QVP_RCVD_FIRST_VIDEO_FRAME,    /*            */
  EVENT_QVP_CALL_RELEASED,             /*                */
  EVENT_CB_SMS_NOTIFY,                 /*                 */
  EVENT_GPS_PDSM_EVENT_REPORT,         /*                */
  EVENT_LONG_CODE_MASK_CHANGED,        /*                */
  EVENT_DS707,                         /*                */

  EVENT_GSDI_ACTIVATE_FEATURE_IND,     /*                */
  EVENT_GSDI_DEACTIVATE_FEATURE_IND,   /*                */
  EVENT_GSDI_GET_FEATURE_IND,          /*                 */
  EVENT_GSDI_SET_FEATURE_DATA,         /*                */
  EVENT_GSDI_UNBLOCK_FEATURE_IND,      /*                */
  EVENT_GSDI_GET_CONTROL_KEY,          /*                */
  EVENT_GSDI_OTA_DEPERSO,              /*                 */
  EVENT_GSDI_GET_PERM_FEATURE_IND,     /*                 */
  EVENT_GSDI_PERM_DISBALE_FEATURE_IND, /*                */
  EVENT_GSM_L1_VOCODER_INITIALIZE,     /*     */
  EVENT_GSM_L1_ALIGN_VFR,              /*     */
  EVENT_GSM_L1_VOCODER_ENABLED,        /*     */
  EVENT_HDR_AMAC_PERSISTENCE_FAILED,   /*            */
  EVENT_HDR_AMAC_PERSISTENCE_PASSED,   /*            */

  /*                                 */
  EVENT_MFLO_STREAM_STATE,                /*                 */
  EVENT_MFLO_CONTROL_CHANNEL_STATE_CHANGE,/*                 */
  EVENT_MFLO_SLEEP_STATE_CHANGE,          /*                 */
  EVENT_MFLO_NETWORK_STATE_CHANGE,        /*                 */
  EVENT_MFLO_TRANS_STATE,                 /*                 */
  EVENT_MFLO_OIS_STATE,                   /*                 */
  EVENT_MFLO_RXD_STATE,                   /*                 */
  EVENT_MFLO_HIPRI_STATE_CHANGE,          /*                 */
  EVENT_MFLO_CAS_STATE,                   /*                 */
  EVENT_MFLO_ACQ_STATE,                   /*                 */
  EVENT_MFLO_OSCAR_FRAME_DECODED,         /*                 */
  EVENT_MFLO_CHAN_SWITCH_RENDERED,        /*                 */
  EVENT_MFLO_OSCAR_DEC_EXCEPTION_DETECTED,/*                 */
  EVENT_MFLO_MFN_SUBSTATE,                /*                 */
  EVENT_MFLO_MFN_STATE,                   /*                  */
  EVENT_MFLO_MFN_VERTICAL_HANDOFF,        /*                  */
  EVENT_MFLO_MFN_ACQ_STATE,               /*                  */
  EVENT_MFLO_FLOW_STATUS,                 /*                  */
  EVENT_MFLO_NETWORK_STATUS,              /*                  */
  EVENT_MFLO_UNDEFINED_1070,

  EVENT_CM_LCS_MOLR_CONF,                 /*                */
  EVENT_PPP_NETMODEL,                     /*                */
  EVENT_CAMERA_PROFILING,                 /*                */
  EVENT_MAC_HS_T1_EXPIRY,                 /*                */
  EVENT_ASYNC_DS707,                      /*                */
  EVENT_PKT_DS707,                        /*                */
  EVENT_GPRS_TIMER_EXPIRY,                /*                */
  EVENT_GPRS_MAC_IDLE_IND,                /*            */
  EVENT_GPRS_PACKET_CHANNEL_REQUEST,      /*                */
  EVENT_GPRS_ACCESS_REJECT,               /*                */
  EVENT_GPRS_PACKET_RESOURCE_REQUEST,     /*                */
  EVENT_GPRS_PACKET_UPLINK_ASSIGNMENT,    /*                */
  EVENT_GPRS_PACKET_DOWNLINK_ASSIGNMENT,  /*                */
  EVENT_PACKET_TIMESLOT_RECONFIGURE,      /*                */
  EVENT_GPRS_TBF_RELEASE,                 /*                */
  EVENT_GPRS_CELL_CHANGE_ORDER,           /*                */
  EVENT_GPRS_CELL_CHANGE_FAILURE,         /*                */
  EVENT_GSM_AMR_RATSCCH_REQ,              /*                */
  EVENT_GSM_AMR_RATSCCH_RSP,              /*                */
  EVENT_SD_SRV_IND_HYBR_WLAN,
  EVENT_SD_EVENT_ACTION_HYBR_WLAN,
  EVENT_GPS_PD_DEMOD_SESS_START,          /*                */
  EVENT_GPS_PD_DEMOD_SESS_END,            /*                */
  EVENT_GPS_SV_ACQUIRED,                  /*                */
  EVENT_GPS_SV_BIT_EDGE_FOUND,            /*                */
  EVENT_GPS_DEMOD_STARTED,                /*                */
  EVENT_GPS_DEMOD_OUT_OF_LOCK,            /*                */
  EVENT_GPS_DEMOD_STOPPED,                /*                */
  EVENT_GPS_DEMOD_PREAMBLE_FOUND,         /*                */
  EVENT_GPS_DEMOD_FRAME_SYNC_STATUS,      /*                */
  EVENT_GPS_DEMOD_SUBFRAME,               /*                */
  EVENT_GPS_DEMOD_EPHEMERIS_COMPLETE,     /*                */
  EVENT_GPS_DEMOD_ALMANAC_COMPLETE,       /*                */
  EVENT_GPS_DEMOD_BIT_EDGE_STATUS,        /*                */
  EVENT_RAT_CHANGE,                       /*                */
  EVENT_REGISTRATION_SUPPRESSED,          /*                */
  EVENT_HDR_RUP_DIST_BASED_REG,           /*                */
  EVENT_GPS_DIAG_APP_TRACKING_START,      /*                */
  EVENT_GPS_DIAG_APP_TRACKING_END,        /*                 */
  EVENT_GPS_DIAG_APP_POSITION_SUCCESS,    /*                 */
  EVENT_GPS_DIAG_APP_POSITION_FAILURE,    /*                */
  EVENT_GSM_AMR_MULTIRATE_IE,             /*                */
  EVENT_EPZID_HYSTERESIS_ENABLED,         /*            */
  EVENT_EPZID_HYSTERESIS_DISABLED,        /*            */
  EVENT_EPZID_HT_STARTED,                 /*                 */
  EVENT_EPZID_HT_EXPIRED,                 /*                 */
  EVENT_HDR_BCMCS_FLOW_STATE_CHANGE,      /*                */
  EVENT_HDR_LMAC_UPDATE_BC_STATUS,        /*                */
  EVENT_DS_CAM_TIMER,                     /*                */
  EVENT_DS_RDUD_TIMER,                    /*                */
  EVENT_DS_CTA_TIMER,                     /*                 */
  EVENT_DS_FALLBACK,                      /*                */
  EVENT_DS3G_CAM_FLOW_CTRL_TIMER,         /*                */
  EVENT_GPS_JAMMER_DETECTION_TEST_PASS,   /*            */
  EVENT_GPS_JAMMER_DETECTION_TEST_FAILURE,/*                */
  EVENT_JAMMER_DETECT_NOISE_STATS,        /*                */
  EVENT_GPS_GET_PARAM,                    /*                */
  EVENT_GPS_GET_PARAM_BS_INFO,            /*                 */
  EVENT_HS_SERVING_CELL_CHANGE,           /*                */
  EVENT_HS_DSCH_STATUS,                   /*                */
  EVENT_SMGMM_REQUEST_SENT,               /*                */
  EVENT_SMGMM_REJECT_RECEIVED,            /*                */
  EVENT_LINUX_APP_STOP,                   /*                */
  EVENT_GPS_PD_CME_SESSION_START,         /*                */
  EVENT_GPS_PD_CME_SESSION_END,           /*                */

  /*                                    */
  EVENT_SIP_REGISTER_START,               /*                */
  EVENT_SIP_REGISTER_DONE,                /*                */
  EVENT_SIP_CALL_SETUP_START,             /*                */
  EVENT_SIP_CALL_SETUP_DONE,              /*            */
  EVENT_SIP_CALL_RELEASE_START,           /*                */
  EVENT_SIP_CALL_RELEASE_DONE,            /*            */
  EVENT_AUDIO_FRAME_SENT_TO_DECODER,      /*                 */
  EVENT_VIDEO_FRAME_SENT_TO_DECODER,      /*                 */
  EVENT_DEC_RENDER_FRAME,                 /*                */
  EVENT_DEC_RENDER_DONE,                  /*            */
  EVENT_DEC_START_DECODING,               /*                */
  EVENT_DEC_FRAME_DECODED,                /*                */
  EVENT_V_ENCODED,                        /*                 */
  EVENT_DEC_START_DECODING_EXT,           /*                */
  EVENT_DEC_FRAME_DECODED_EXT,            /*                */
  EVENT_QVIDEOPHONE_UNDEFINED_1151,
  EVENT_QVIDEOPHONE_UNDEFINED_1152,
  EVENT_QVIDEOPHONE_UNDEFINED_1153,
  EVENT_QVIDEOPHONE_UNDEFINED_1154,
  EVENT_QVIDEOPHONE_UNDEFINED_1155,

  EVENT_GPS_CME_POS_REQ,                  /*            */
  EVENT_GPS_CME_FIX_START,                /*            */
  EVENT_GPS_CME_FIX_END,                  /*            */
  EVENT_GPS_SEED_CLM,                     /*                 */
  EVENT_GPS_SEED_SID,                     /*                 */
  EVENT_GPS_SEED_SL,                      /*                 */
  EVENT_GPS_SEED_GET,                     /*                 */

  EVENT_HDR_OVHD_BC_MSG_RX,               /*            */
  EVENT_HDR_OVHD_T_BC_SUPERVISION,        /*            */
  EVENT_HDR_LMAC_SET_BCMCS_PAGE_CYCLE,    /*                */
  EVENT_HDR_HMP_SESSION_CLOSED,           /*                */

  EVENT_WLAN_CP,                          /*                 */
  EVENT_ARP,                              /*                 */
  EVENT_DHCP,                             /*                 */
  EVENT_WLAN_WPA,                         /*                */
  EVENT_EAP,                              /*                */
  EVENT_LAN_1X,                           /*                */

  EVENT_CAMERA_SVCS_START,                /*            */
  EVENT_CAMERA_SVCS_STOP,                 /*            */

  EVENT_BCMCS_SRVC_AVAILABLE,             /*                */
  EVENT_BCMCS_SRVC_LOST,                  /*                */
  EVENT_BCMCS_FLOW_REGISTERED,            /*                 */
  EVENT_BCMCS_FLOW_DEREGISTERED,          /*                 */
  EVENT_BCMCS_FLOW_STATUS_CHANGED,        /*                 */

  EVENT_CAMERA_SVCS_X,                    /*                */
  EVENT_CM_CALL_EVENT_ORIG_THR,           /*                */

  EVENT_VFE_MSG_CONFIG_COMPLETE,          /*            */
  EVENT_VFE_MSG_IDLE_COMPLETE,            /*            */
  EVENT_VFE_MSG_UPDATE_COMPLETE,          /*            */
  EVENT_VFE_MSG_AE_AWB_STATS,             /*            */
  EVENT_DSP_VIDEO_ENC_DOWNLOAD_DONE,      /*            */
  EVENT_DSP_VIDEO_ENC_SELECTION_DONE,     /*            */
  EVENT_DSP_VIDEO_ENC_CONFIG_DONE,        /*            */
  EVENT_DSP_VIDEO_ENC_FRAME_DONE,         /*            */

  EVENT_HDR_OVHD_BCMCS_CHAN_CHANGE,       /*                */

  EVENT_QVS_REGISTER_START,               /*                */
  EVENT_QVS_REGISTER_DONE,                /*                */
  EVENT_QVS_REGISTER_FAILED,              /*            */
  EVENT_QVS_CALL_SETUP_START,             /*                */
  EVENT_QVS_CALL_SETUP_DONE,              /*            */
  EVENT_QVS_CALL_SETUP_FAILED,            /*            */
  EVENT_QVS_CALL_RELEASE_START,           /*                */
  EVENT_QVS_CALL_RELEASE_DONE,            /*            */
  EVENT_QVS_CALL_RELEASE_FAILED,          /*            */

  EVENT_CAMCORDER_START_RECORD,           /*                */
  EVENT_CAMCORDER_START_TRANSCODE,        /*                */
  EVENT_CAMCORDER_FRAME_DROP,             /*            */
  EVENT_CAMCORDER_AUDIODUB,               /*                */

  EVENT_PSMM_SENT,                        /*                 */
  EVENT_GPS_PD_FALLBACK_MODE,             /*                */

  EVENT_PEAP,                             /*                */
  EVENT_TTLS,                             /*                */
  EVENT_TLS,                              /*                */

  EVENT_WCDMA_TO_WCDMA_RESELECTION_VER2_START, /*                */

  EVENT_EUL_RECONFIG_OR_ASU,              /*                 */
  EVENT_EUL_SERVING_CELL_CHANGE,          /*                */
  EVENT_EUL_PHYSICAL_LAYER_RECONFIG,      /*                 */

  EVENT_DRM_ROAP_TRIGGER_RECIEVED,        /*                */
  EVENT_DRM_ROAP_PROTOCOL_START,          /*                */
  EVENT_DRM_ROAP_REQUEST,                 /*                */
  EVENT_DRM_ROAP_REQUEST_EXTENSION,       /*                */
  EVENT_DRM_ROAP_RESPONSE,                /*                */
  EVENT_DRM_ROAP_RESPONSE_EXTENSION,      /*                */
  EVENT_DRM_ROAP_RI_CONTEXT,              /*                */
  EVENT_DRM_ROAP_ERROR,                   /*                */
  EVENT_DRM_ROAP_RSP_VALIDATION,          /*                */
  EVENT_DRM_ROAP_PROTOCOL_END,            /*                */

  EVENT_DS_WMK_ALLOCATED,                 /*                 */
  EVENT_DS_WMK_DEALLOCATED,               /*                 */
  EVENT_DS_WMK_FLUSHED,                   /*                 */
  EVENT_DS_WMK_FLOW_ENABLED,              /*                 */
  EVENT_DS_WMK_FLOW_DISABLED,             /*                 */

  EVENT_HDR_IDLE_SET_SLEEP_DURATION,      /*                */
  EVENT_HDR_SCM_SESSION_CHANGED,          /*                */

  EVENT_UMTS_TO_CDMA_DATA_HANDOVER,       /*            */ 
  EVENT_UMTS_TO_CDMA_VOICE_HANDOVER,      /*                 */ 

  EVENT_MO_SMS_RETRY_ATTEMPT,             /*                 */ 

  EVENT_HDR_LMAC_UPDATE_QSM_STATUS,       /*                */ 

  EVENT_CM_CELL_SRV_IND,                  /*                */ 

  EVENT_RLP_NAK_ABORT,                    /*                */ 

  EVENT_DRM_RIGHTS_OPERATION,             /*                */

  EVENT_DS_RESV_MSG_SENT_REV_FLOWS,       /*                 */
  EVENT_DS_RESV_MSG_SENT_FWD_FLOWS,       /*                 */
  EVENT_DS_RESV_RESP_SUCCESS_RECD,        /*                */
  EVENT_DS_RESV_RESP_FAILURE_RECD,        /*                */

  EVENT_GPS_PD_COMM_FAILURE,              /*                */
  EVENT_GPS_PD_COMM_DONE,                 /*            */
  EVENT_GPS_PD_EVENT_END,                 /*                */
  EVENT_GPS_PA_EVENT_CALLBACK,            /*                */
  EVENT_GPS_PD_CMD_ERR_CALLBACK,          /*                */
  EVENT_GPS_PA_CMD_ERR_CALLBACK,          /*                */

  EVENT_GPS_LM_ENTER_SA_RF_VERIF,         /*                */
  EVENT_GPS_LM_EXIT_SA_RF_VERIF,          /*                */
  EVENT_GPS_LM_ERROR_SA_RF_VERIF,         /*                */
  EVENT_GPS_LM_PD_COMPLETE,               /*            */
  EVENT_GPS_LM_IQ_TEST_COMPLETE,          /*            */

  EVENT_PM_APP_OTG_INIT,                  /*            */
  EVENT_PM_APP_OTG_RESET,                 /*            */
  EVENT_PM_APP_OTG_ACQUIRE_BUS_REQ,                         /*                */
  EVENT_PM_APP_OTG_RELINQUISH_BUS_REQ,                      /*            */
  EVENT_PM_APP_OTG_SUSPEND,                           /*            */
  EVENT_PM_APP_OTG_RESUME,                              /*            */
  EVENT_PM_APP_OTG_DEVICE_ATTACHED,                            /*                */
  EVENT_PM_APP_OTG_DEVICE_DETACHED,                        /*            */
  EVENT_PM_APP_OTG_HOST_MODE_REM_PERI_DIS,                     /*            */
  EVENT_PM_APP_OTG_PERI_MODE_PREPARE_FOR_REM_HOST_WAKEUP_SIG,  /*            */
  EVENT_PM_APP_OTG_PERI_MODE_REM_HOST_WAKEUP_SIG_DONE,   /*            */
  EVENT_PM_APP_OTG_SET_REM_WAKEUP_CAPABILITY,              /*                */
  EVENT_PM_APP_OTG_OPERATIONAL_ERROR,                          /*                */
  EVENT_PM_APP_OTG_CONFIGURE_USB_POWER_CONSUMER,         /*            */
  EVENT_PM_APP_OTG_SET_USB_POWER_CONSUMPTION_REQUIREMENT,      /*                */
  EVENT_PM_APP_OTG_PERI_MODE_PROCESS_USB_POWER_LINE_CONT_REQ,  /*                */
  EVENT_PM_APP_OTG_PERI_MODE_SET_REM_A_DEV_INFO,               /*                */
  EVENT_PM_APP_OTG_STATE_TRANSITION,                           /*                */

  EVENT_DTV_TABLE_ACQ_SUCCESS,                                 /*                 */  
  EVENT_DTV_TABLE_ACQ_FAIL,                                    /*                */  
  EVENT_DTV_DVBH_SEL_PLTFM_REQ_RCVD,                           /*                */
  EVENT_DTV_DVBH_PLTFM_ACQ_SUCCESS,                            /*                */
  EVENT_DTV_DVBH_PLTFM_ACQ_FAIL,                               /*                */  
  EVENT_DTV_DVBH_TBL_MGR_STATE_CHANGED,                        /*                */
  EVENT_DTV_DVBH_CE_STATE_CHANGED,                             /*                */
  EVENT_DTV_DVBH_MCAST_JOIN_REQ_RCVD,                          /*                 */
  EVENT_DTV_DVBH_MCAST_LEAVE_REQ_RCVD,                         /*                 */
  EVENT_DTV_DVBH_INIT_REQ_RCVD,                                /*            */
  EVENT_DTV_DVBH_MCAST_JOIN_SUCCESS,                           /*                 */
  EVENT_DTV_DVBH_MCAST_JOIN_FAILURE,                           /*                 */
  EVENT_DTV_DVBH_MCAST_LEAVE_SUCCESS,                          /*                 */
  EVENT_DTV_DVBH_MCAST_LEAVE_FAILURE,                          /*                 */
  EVENT_DTV_DVBH_INIT_SUCCESS,                                 /*            */
  EVENT_DTV_DVBH_INIT_FAILURE,                                 /*            */

  EVENT_GPS_LM_SESSION_START,             /*                */
  EVENT_GPS_LM_SESSION_END,               /*            */
  EVENT_GPS_LM_FIX_REQUEST_START,         /*            */
  EVENT_GPS_LM_FIX_REQUEST_END,           /*            */
  EVENT_GPS_LM_PRM_REQUEST_START,         /*            */
  EVENT_GPS_LM_PRM_REQUEST_END,           /*            */
  EVENT_GPS_LM_SESSION_CONTINUE,          /*                */
  EVENT_GPS_LM_FIX_REQUEST_CONTINUE,      /*            */
  EVENT_GPS_LM_PRM_REQUEST_CONTINUE,      /*            */
  EVENT_GPS_LM_PPM_REQUEST_CONTINUE,      /*            */
  EVENT_GPS_LM_AIDING_DATA_RECEIVED,      /*                */
  EVENT_GPS_LM_RC_ON_TIMER_TIMEOUT,       /*            */
  EVENT_GPS_LM_SHUT_OFF_TIMER_TIMEOUT,    /*            */
  EVENT_GPS_LM_MGP_ON,                    /*            */
  EVENT_GPS_LM_MGP_IDLE,                  /*            */
  EVENT_GPS_LM_MGP_OFF,                   /*            */

  EVENT_DRM_RO_CONSUMPTION_VALIDATION,    /*                */
  EVENT_DRM_RO_INSTALLATION_VALIDATION,   /*                */

  EVENT_FLUTE_FDT_INST_RCVD,              /*                */
  EVENT_FLUTE_FDT_INST_RCV_FAIL,          /*                */
  EVENT_FLUTE_FDT_INST_EXPIRED,           /*                */
  EVENT_FLUTE_JOIN_SESSION_REQ_RCVD,      /*                 */
  EVENT_FLUTE_LEAVE_SESSION_REQ_RCVD,     /*                */
  EVENT_FLUTE_SESSION_CLOSED,             /*                */ 
  EVENT_FLUTE_SESSION_CLOSED_BY_APP,      /*                */ 
  EVENT_FLUTE_B_FLAG_RCVD,                /*                */ 
  EVENT_FLUTE_GET_FILE_REQUEST_RCVD,      /*                */ 
  EVENT_FLUTE_JOIN_SESSION_RSP,           /*                */ 
  EVENT_FLUTE_FILE_STATUS_RSP,            /*                 */ 
  EVENT_FLUTE_CANCEL_FILE_REQ_RCVD,       /*                */

  EVENT_DTV_DVBH_DEINIT_REQ_RCVD,         /*            */
  EVENT_DTV_DVBH_DEINIT_SUCCESS,          /*            */
  EVENT_DTV_DVBH_DEINIT_FAILURE,          /*            */

  EVENT_CONTENT_INSTALL_BEGIN,            /*            */
  EVENT_CONTENT_INSTALL_COMPLETE,         /*                */
  EVENT_CONTENT_RETRIEVAL_BEGIN ,         /*            */
  EVENT_CONTENT_RETRIEVAL_COMPLETE,       /*                */
  EVENT_CONTENT_BACKUP_BEGIN,             /*            */
  EVENT_CONTENT_BACKUP_COMPLETE,          /*                */
  EVENT_CONTENT_FWD_BEGIN,                /*            */
  EVENT_CONTENT_FWD_COMPLETE,             /*                */

  EVENT_HARD_HANDOFF_VOIP_TO_CDMA,        /*                 */

  EVENT_EAP_SIM_AKA,                      /*                 */
  EVENT_WLAN_CP_MEAS,                     /*                 */
  EVENT_WLAN_CP_HO,                       /*                 */
  EVENT_WLAN_CP_11D,                      /*                 */
  EVENT_WLAN_MC,                          /*                 */

  EVENT_SVG_CONTENT_SET,                  /*                 */
  EVENT_SVG_CONTENT_PLAY,                 /*                 */
  EVENT_SVG_CONTENT_RESUME,               /*                 */
  EVENT_SVG_CONTENT_PAUSE,                /*                 */
  EVENT_SVG_CONTENT_STOP,                 /*                 */
  EVENT_SVG_CONTENT_USEREVENT,            /*                 */
  EVENT_SVG_CONTENT_GETURIDATA,           /*                 */
  EVENT_SVG_CONTENT_TRANSFORM,            /*                 */
  EVENT_SVG_GET_PARAM,                    /*                 */
  EVENT_SVG_SET_PARAM,                    /*                 */

  EVENT_WLAN_WPA2,                        /*                */

  EVENT_WCDMA_PSC_SCANNER_STOP,           /*                */

  EVENT_MEDIA_PLAYER_KEYPRESS,            /*                */

  EVENT_WLAN_MC_QOS,                      /*                */

  EVENT_WCDMA_PSC_SCANNER_STATE,          /*                */

  EVENT_WLAN_CP_ADHOC,                    /*                 */

  EVENT_DMB_STACK_SHUTDOWN,               /*                */
  EVENT_DMB_TUNE_DONE_SUCCESS,            /*                */
  EVENT_DMB_TUNE_DONE_FAILURE,            /*                */
  EVENT_DMB_SEARCH_DONE,                  /*                */
  EVENT_DMB_SCAN_DONE,                    /*                */
  EVENT_DMB_RECEPTION_INFO_CHANGED,       /*                */
  EVENT_DMB_DMB_GUIDE_CHANGED,            /*                */
  EVENT_DMB_LOCATION_INFO_CHANGED,        /*                */
  EVENT_DMB_LOST_ENSEMBLE,                /*                */
  EVENT_DMB_STREAM_TERMINATED,            /*                */
  EVENT_DMB_STREAM_DATA_AVAILABLE,        /*                */
  EVENT_DMB_RESERVED1,                    /*                */
  EVENT_DMB_RESERVED2,                    /*                */
  EVENT_DMB_RESERVED3,                    /*                */
  EVENT_DMB_RESERVED4,                    /*                */
  EVENT_DMB_RESERVED5,                    /*                */
  EVENT_DMB_RESERVED6,                    /*                */
  EVENT_DMB_RESERVED7,                    /*                */
  EVENT_DMB_RESERVED8,                    /*                */
  EVENT_DMB_RESERVED9,                    /*                */
  EVENT_DMB_RESERVED10,                   /*                */

  EVENT_MOBILEVIEW_RESERVED1,             /*     */
  EVENT_MOBILEVIEW_RESERVED2,             /*     */
  EVENT_MOBILEVIEW_RESERVED3,             /*     */
  EVENT_MOBILEVIEW_RESERVED4,             /*     */
  EVENT_MOBILEVIEW_RESERVED5,             /*     */

  EVENT_HDR_DOS_MO_DOS_STATUS,            /*                */ 

  EVENT_GPSONEXTRA_START_DOWNLOAD,        /*                */
  EVENT_GPSONEXTRA_END_DOWNLOAD,          /*                */

  EVENT_SNSD_GENERIC,                     /*                */
  EVENT_SNSD_DEVICE_INIT,                 /*                */
  EVENT_SNSD_DEVICE_CONFIGURED,           /*                */
  EVENT_SNSD_EVENT_DATA_READY,            /*                */
  EVENT_SNSD_EVENT_COND_MET,              /*                */
  EVENT_SNSD_DEVICE_DOWN,                 /*                */
  EVENT_SNSD_ERROR,                       /*                */
  
  EVENT_CM_COUNTRY_SELECTED,              /*                */
  EVENT_CM_SELECT_COUNTRY,                /*                */

  EVENT_GPS_DCME_NEW_SV_ADDED_IN_AA,      /*                */
  EVENT_GPS_DCME_SV_REMOVED_FROM_AA,      /*                */

  EVENT_ESG_GET_PROV_LIST_REQ_RCVD,       /*            */
  EVENT_ESG_GET_PROV_LIST_REQ_FAIL,       /*            */
  EVENT_ESG_PROV_LIST_AVAILABLE,          /*            */
  EVENT_ESG_ACQ_REQ_RCVD,                 /*                */
  EVENT_ESG_ACQ_REQ_FAIL,                 /*                */
  EVENT_ESG_STOP_REQ_RCVD,                /*            */
  EVENT_ESG_STOP_REQ_FAIL,                /*            */
  EVENT_ESG_STOP_COMPLETE,                /*                */

  EVENT_ADC_ONDIE_THERM_READ,             /*                */

  EVENT_CONTENT_NO_VALID_OR_EXPIRED_RIGHTS, /*            */

  EVENT_MOBILEVIEW_RESERVED30,            /*                      */
  EVENT_MOBILEVIEW_RESERVED31,            /*                      */
  EVENT_MOBILEVIEW_RESERVED32,            /*                      */

  EVENT_GPS_DCME_MEAS_CYCLE_START,        /*            */ 
  EVENT_GPS_DCME_MEAS_CYCLE_END,          /*            */ 
  EVENT_GPS_CME_ENGAGED,                  /*            */ 
  EVENT_GPS_CME_NOT_ENGAGED,              /*            */ 
  EVENT_GPS_DCME_ENGAGED,                 /*            */ 
  EVENT_GPS_DCME_NOT_ENGAGED,             /*            */ 

  EVENT_HS_USB_DEVICE_ATTACHED,           /*            */
  EVENT_HS_USB_HID_DISCONECT,             /*            */
  EVENT_HS_USB_HID_CONNECT,               /*                */
  EVENT_HS_USB_MSD_CONNECT,               /*            */
  EVENT_HS_USB_MSD_DISCONECT,             /*            */
  EVENT_HS_USB_STACK_SUSPENDED,           /*                */
  EVENT_HS_USB_STACK_RESUMED,             /*                */
  EVENT_HS_USB_ENTER_HOST_MODE,           /*            */
  EVENT_HS_USB_OPERATIONAL_ERROR,         /*                */

  EVENT_DTV_L1_ACQ_DONE,                  /*                */
  EVENT_DTV_L1_SCAN,                      /*                */
  EVENT_DTV_L1_ONLINE,                    /*                */
  EVENT_DTV_L1_SNOOZE,                    /*            */
  EVENT_DTV_L1_SLEEP,                     /*            */
  EVENT_DTV_L1_HANDOFF,                   /*                */
  EVENT_DTV_L1_SIGNAL_LOST,               /*            */

  EVENT_IMS_SIP_REGISTRATION_START,       /*                */
  EVENT_IMS_SIP_REGISTER_END,             /*                */
  EVENT_IMS_SIP_DEREGISTER_START,         /*                */
  EVENT_IMS_SIP_DEREGISTER_END,           /*                */
  EVENT_IMS_SIP_SESSION_START,            /*                */
  EVENT_IMS_SIP_SESSION_RINGING,          /*                */
  EVENT_IMS_SIP_SESSION_ESTABLISHED,      /*                */
  EVENT_IMS_SIP_SESSION_TERMINATED,       /*                */
  EVENT_IMS_SIP_SESSION_CANCEL,           /*                */
  EVENT_IMS_SIP_SESSION_FAILURE,          /*                */
  EVENT_IMS_SIP_RESPONSE_RECV,            /*                */
  EVENT_IMS_SIP_REQUEST_RECV,             /*                */
  EVENT_IMS_SIP_RESPONSE_SEND,            /*                */
  EVENT_IMS_SIP_REQUEST_SEND,             /*                */

  EVENT_WLAN_TKIP_COUNTER_MEAS,           /*                */

  EVENT_GPS_BLANKING_OFF,                 /*            */
  EVENT_GPS_BLANKING_ON,                  /*            */

  EVENT_MMGSDI_EVENT,                     /*                 */

  EVENT_WLAN_CP_SYS_MGR_STATE_TRANS,      /*                */

  EVENT_GPS_OPTIMISTIC_PUNC_START,        /*                */
  EVENT_GPS_OPTIMISTIC_PUNC_END,          /*                */

  EVENT_QVP_SEND_RTP_PACKET,              /*                */
  EVENT_QVP_RECV_RTP_PACKET,              /*                */

  EVENT_HDR_IDLE_REACQ_FAIL_DDARF,        /*                */

  EVENT_BCAST_SEC_STKM_PARSE_STATUS,      /*                */
  EVENT_BCAST_SEC_STKM_RECEIVED,          /*            */
  EVENT_BCAST_SEC_SDP_PARSE_STATUS,       /*                */

  EVENT_CGPS_ME_DPO_STATUS,               /*                */
  EVENT_GPS_SV_SEARCH_STATE,              /*                */
  EVENT_GPS_TM_ON_DEMAND_MODE_CHANGE,     /*                */
  EVENT_GPS_TM_ON_DEMAND_BEGIN,           /*                */
  EVENT_GPS_TM_ON_DEMAND_DONE,            /*                */

  EVENT_RMAC_CARRIER_STATE_CHANGED,       /*                */

  EVENT_GPS_SBAS_DEMOD_REPORT,            /*                */
  EVENT_GPS_EXTERN_COARSE_POS_INJ_START,  /*            */
  EVENT_GPS_EXTERN_COARSE_POS_INJ_END,    /*                */
  EVENT_GPS_EPH_REREQUEST_TIME,           /*                */

  EVENT_WLAN_QOS_PSTREAM,                 /*                */
  EVENT_WLAN_CP_VCC,                      /*                */

  EVENT_CGPS_DIAG_FIRST_SUCCESSFUL_FIX,   /*            */

  EVENT_EUL_RECONFIG_OR_ASU_OR_TTI_RECFG, /*                 */

  EVENT_DS707_PKT_LN_UPDATE,              /*                */
  EVENT_DS707_PKT_IDM_CHANGE,             /*                */

  EVENT_RLP_QN_ADD,                       /*                */
  EVENT_RLP_QN_DROP,                      /*                */
  EVENT_RLP_MULTILINK_NAK,                /*                */
  EVENT_RLP_REV_LINK_NAK,                 /*                */
  EVENT_GSTK_EVENT,                       /*                 */

  EVENT_GAN_REGISTRATION_REQUEST,         /*                */
  EVENT_GAN_REGISTER_ACCEPT,              /*                */
  EVENT_CALL_RINGING_ALERT,               /*                */
  EVENT_GAN_PAGING_RECEIVED,              /*                */
  EVENT_GAN_CALL_DISCONNECT,              /*                */
  EVENT_GAN_CALL_RELEASE_COMPLETE,        /*                */
  EVENT_GAN_HANDIN_COMMAND,               /*                */
  EVENT_GAN_HANDIN_COMPLETE,              /*                */
  EVENT_GAN_HANDOUT_COMMAND,              /*                */
  EVENT_GAN_HANDOUT_COMPLETE,             /*                */
  EVENT_GAN_SMS_START,                    /*                */
  EVENT_GAN_SMS_ACK,                      /*                */
  EVENT_GAN_QDJ_ENQUEUE,                  /*            */
  EVENT_GAN_QDJ_DEQUEUE,                  /*            */
  EVENT_GAN_ACTIVATE_DATA_CHANNEL,        /*                */
  EVENT_GAN_DATA_CHANNEL_CONNECTED,       /*                */
  EVENT_GAN_RLP_SUSPEND,                  /*                */
  EVENT_GAN_RLP_RESUME,                   /*                */
  EVENT_GAN_WAKEUP_REQ,                   /*            */
  EVENT_GAN_WAKEUP_CNF,                   /*            */
  EVENT_GAN_HIBERNATION_REQ,              /*            */
  EVENT_GAN_HIBERNATION_CNF,              /*            */

  EVENT_WCDMA_UL_AMR_RATE,                /*                 */
  EVENT_EUL_TTI_RECONFIG,                 /*                */
  EVENT_WCDMA_CONN_REL_CAUSE,             /*                */
  EVENT_WCDMA_CONN_REQ_CAUSE,             /*                */

  EVENT_LTE_TIMING_ADVANCE,               /*                */
  EVENT_LTE_UL_OUT_OF_SYNC,               /*            */
  EVENT_LTE_SPS_DEACTIVATED,              /*                */
  EVENT_LTE_RACH_ACCESS_START,            /*                */
  EVENT_LTE_RACH_RAID_MATCH,              /*                */
  EVENT_LTE_RACH_ACCESS_RESULT,           /*                */

  EVENT_DTV_L1_POWERUP,                   /*                */
  EVENT_DTV_L1_POWERDOWN,                 /*                */
  EVENT_DTV_L1_SOFT_RESET,                /*                */
  EVENT_DTV_L1_STATE_CHANGE,              /*                */
  EVENT_DTV_L1_ACQ_TUNE_STATUS,           /*                */
  EVENT_DTV_L1_ACQ_DONE_STATUS,           /*                */
  EVENT_DTV_L1_ACQ_FAIL,                  /*                */
  EVENT_DTV_L1_TRAFFIC_STARTED,           /*                */
  EVENT_DTV_L1_BAD_FRAME_RECEIVED,        /*                */
  EVENT_DTV_L1_TMCC_FAILURE,              /*                */
  EVENT_DTV_L1_RECOVERY_STATUS,           /*                */
  EVENT_DTV_L1_INTERRUPT_LOG_RECEIVED,    /*                */
  EVENT_DTV_L1_L3_API_COMMAND,            /*                */
  EVENT_DTV_L1_MODEM_FAILURE,             /*                */

  EVENT_GSM_CALL_DROP,                    /*                */
  EVENT_GSM_ACCESS_FAILURE,               /*                */

  EVENT_DTV_ISDB_ACTIVATE,                /*                */
  EVENT_DTV_ISDB_DEACTIVATE,              /*                */
  EVENT_DTV_ISDB_TUNE,                    /*                 */
  EVENT_DTV_ISDB_UNTUNE,                  /*                */
  EVENT_DTV_ISDB_SELECT_SERVICE,          /*                 */
  EVENT_DTV_ISDB_SERVICE_AVAILABLE,       /*                */
  EVENT_DTV_ISDB_TRAFFIC_LOST,            /*                */
  EVENT_DTV_ISDB_TABLE_UPDATE,            /*                */
  EVENT_DTV_ISDB_TRACKS_SELECTED,         /*                 */
  EVENT_DTV_ISDB_PES_BUFFER_OVERFLOW,     /*                */
  EVENT_DTV_ISDB_PES_BUFFER_UNDERFLOW,    /*                */
  EVENT_DTV_ISDB_ACQUIRE_DATA_COMPONENT,  /*                 */
  EVENT_DTV_ISDB_STOP_COMPONENT_ACQUISITION, /*                 */
  EVENT_DTV_ISDB_DII_CHANGED,             /*                */
  EVENT_DTV_ISDB_DATA_EVENT_MESSAGE,      /*                */
  EVENT_DTV_ISDB_MODULE_CONSTRUCTION,     /*                 */
  EVENT_DTV_ISDB_PARSING_ERROR,           /*                 */

  EVENT_HDR_SLP_SLPQH_TIMER_STARTED,      /*            */
  EVENT_HDR_SLP_SLPQH_TIMER_STOPPED,      /*                */
  EVENT_HDR_SLP_SLPQH_NUM_PENDING_MSGS,   /*                */
  EVENT_HDR_OVHD_FIND_CACHED_MSG,         /*                */

  EVENT_WCDMA_RRC_TIMER_EXPIRED,          /*                */
  EVENT_WCDMA_UOOS_TIMER_USED,            /*                */
  EVENT_WCDMA_UOOS_TIMER_START,           /*                */
  EVENT_WCDMA_UOOS_TIMER_STOP,            /*                */
  EVENT_WCDMA_UOOS_TIME_REMAINING,        /*                */
  EVENT_WCDMA_RRCCSP_SCAN_START,          /*                */
  EVENT_WCDMA_ACQUISITON_SUCCESS,         /*                */
  EVENT_WCDMA_CELL_SELECTION_FAIL,        /*                */
  EVENT_WCDMA_BPLMN_START,                /*                */
  EVENT_WCDMA_BPLMN_END,                  /*                */
  EVENT_WCDMA_BPLMN_SCAN_START,           /*                */
  EVENT_WCDMA_BPLMN_SCAN_END,             /*                */

  EVENT_MSG_HIGH,                         /*                 */
  EVENT_MSG_MED,                          /*                 */
  EVENT_MSG_LOW,                          /*                 */
  EVENT_MSG_ERROR,                        /*                 */
  EVENT_MSG_FATAL,                        /*                 */

  EVENT_GAN_START_TU3910,                 /*            */
  EVENT_GAN_STOP_TU3910,                  /*            */ 
  EVENT_GAN_EXPIRY_TU3910,                /*            */
  EVENT_GAN_START_TU3920,                 /*            */
  EVENT_GAN_STOP_TU3920,                  /*            */
  EVENT_GAN_EXPIRY_TU3920,                /*            */
  EVENT_GAN_START_TU3906,                 /*            */ 
  EVENT_GAN_STOP_TU3906,                  /*            */ 
  EVENT_GAN_EXPIRY_TU3906,                /*            */
  EVENT_GAN_URR_REGISTER_UPDATE,          /*                */

  EVENT_IPSEC_IKE_SA_INIT_SENT,            /*                 */
  EVENT_IPSEC_IKE_SA_INIT_RECV,            /*                  */
  EVENT_IPSEC_IKE_SA_ESTABLISHED,          /*                  */
  EVENT_IPSEC_IKE_AUTH_SENT,               /*                  */
  EVENT_IPSEC_IKE_AUTH_RECV,               /*                  */
  EVENT_IPSEC_IKE_EAP_START,               /*            */
  EVENT_IPSEC_IKE_EAP_FINISH,              /*                */
  EVENT_IPSEC_CHILD_SA_ESTABLISHED,        /*                 */
  EVENT_IPSEC_IKE_INFO_MSG_SENT,           /*                  */ 
  EVENT_IPSEC_IKE_INFO_MSG_RECV,           /*                  */
  EVENT_IPSEC_CREATE_CHILD_SA_SENT,        /*                  */
  EVENT_IPSEC_CREATE_CHILD_SA_RECV,        /*                  */
  EVENT_IPSEC_IKE_SA_DELETE_START,         /*                  */
  EVENT_IPSEC_IKE_SA_DELETE_DONE,          /*                  */
  EVENT_IPSEC_CHILD_SA_DELETE_START,       /*                 */
  EVENT_IPSEC_CHILD_SA_DELETE_DONE,        /*                 */
  EVENT_IPSEC_IKE_SA_REKEY_START,          /*                  */
  EVENT_IPSEC_IKE_SA_REKEY_DONE,           /*                  */
  EVENT_IPSEC_CHILD_SA_REKEY_START,        /*                 */
  EVENT_IPSEC_CHILD_SA_REKEY_DONE,         /*                 */
  EVENT_IPSEC_IKE_MESG_RETRANSMIT,         /*                 */
  EVENT_IPSEC_IKE_NAT_DETECTED,            /*            */
  EVENT_IPSEC_IKE_NAT_KEEPALIVE_SENT,      /*                  */
  EVENT_IPSEC_IKE_DPD_SENT,                /*                  */
  EVENT_IPSEC_IKE_ERR_NOTIFY_SENT,         /*                 */
  EVENT_IPSEC_IKE_ERR_NOTIFY_RECV,         /*                 */

  EVENT_GAN_ROVEIN_CNF,                    /*            */
  EVENT_GAN_ROVEOUT_CNF,                   /*            */
  EVENT_GAN_RRC_ROVEIN_CNF,                /*            */
  EVENT_GAN_RRC_ROVEIN_REJ,                /*                */
  EVENT_GAN_RRC_ROVEOUT_CNF,               /*            */
  EVENT_GAN_RRC_ROVEOUT_REJ,               /*                */

  EVENT_GPSXTRA_T_SESS_BEGIN,              /*                */
  EVENT_GPSXTRA_T_SESS_DATA,               /*                */
  EVENT_GPSXTRA_T_SESS_DONE,               /*                */
  EVENT_GPSXTRA_T_SESS_END,                /*                */

  EVENT_DS_GO_NULL_TIMER,                  /*                */

  EVENT_LTE_RRC_TIMER_STATUS,              /*                */
  EVENT_LTE_RRC_STATE_CHANGE,              /*                */
  EVENT_LTE_RRC_OUT_OF_SERVICE,            /*                */
  EVENT_LTE_RRC_RADIO_LINK_FAILURE,        /*                */
  EVENT_LTE_RRC_DL_MSG,                    /*                */ 
  EVENT_LTE_RRC_UL_MSG,                    /*                */
  EVENT_LTE_RRC_NEW_CELL_IND,              /*                */
  EVENT_LTE_RRC_CELL_RESEL_FAILURE,        /*                */
  EVENT_LTE_RRC_HO_FAILURE,                /*                */
  EVENT_LTE_RRC_PAGING_DRX_CYCLE,          /*                */
  EVENT_LTE_RRC_IRAT_HO_FROM_EUTRAN,       /*                */
  EVENT_LTE_RRC_IRAT_HO_FROM_EUTRAN_FAILURE, /*                */
  EVENT_LTE_RRC_IRAT_RESEL_FROM_EUTRAN,            /*                */
  EVENT_LTE_RRC_IRAT_RESEL_FROM_EUTRAN_FAILURE, /*                */
  EVENT_LTE_RRC_SIB_READ_FAILURE,          /*                */

  EVENT_GAN_ROVEIN_REQ,                    /*            */
  EVENT_GAN_ROVEOUT_REQ,                   /*            */

  EVENT_MBP_RF_ANALOG_JD_MODE_CHANGE,      /*                */
  EVENT_MBP_RF_ANALOG_JD_INT,              /*                */

  EVENT_CGPS_QWIP_SYSD_TRANSITION,         /*                */
  
  EVENT_HPLMN_TIMER_EXPIRED,               /*            */

  EVENT_GSDI_GET_FEATURE_INDICATOR_DATA,   /*                */

  EVENT_LTE_CM_INCOMING_MSG,               /*                */
  EVENT_LTE_CM_OUTGOING_MSG,               /*                */
  EVENT_LTE_EMM_INCOMING_MSG,              /*                */
  EVENT_LTE_EMM_OUTGOING_MSG,              /*                */
  EVENT_LTE_EMM_TIMER_START,               /*                */
  EVENT_LTE_EMM_TIMER_EXPIRY,              /*                */

  EVENT_LTE_REG_INCOMING_MSG,              /*                */
  EVENT_LTE_REG_OUTGOING_MSG,              /*                */
  EVENT_LTE_ESM_INCOMING_MSG,              /*                */
  EVENT_LTE_ESM_OUTGOING_MSG,              /*                */
  EVENT_LTE_ESM_TIMER_START,               /*                */
  EVENT_LTE_ESM_TIMER_EXPIRY,              /*                */

  EVENT_SNS_CONTEXT_OPEN,                  /*                */
  EVENT_SNS_CONTEXT_CLOSE,                 /*                */
  EVENT_SNS_COND_SET,                      /*                */
  EVENT_SNS_COND_CANCEL,                   /*                */
  EVENT_SNS_COND_MET,                      /*                */
  EVENT_SNS_DATA_START,                    /*                 */
  EVENT_SNS_DATA_STOP,                     /*                */

  EVENT_WCDMA_RLC_CONFIG,                  /*                */

  EVENT_HSPA_PLUS_CFG,                     /*                */

  EVENT_SNS_DRIVER_STATE_CHANGE,           /*                */

  EVENT_WCDMA_TIMER_DISCARD_EXPIRY,        /*                */  

  EVENT_NAS_CB_PAGE_RECEIVED,              /*                */  

  EVENT_WCDMA_RLC_RESET,                   /*                */  

  EVENT_HDR_MRLP_EHRPD_PERSONALITY_IS_ACTIVE, /*                */  
  
  EVENT_WLAN_SECURITY,                     /*                 */
  EVENT_WLAN_STATUS,                       /*                 */
  EVENT_WLAN_HANDOFF,                      /*                 */
  EVENT_WLAN_VCC,                          /*                */
  EVENT_WLAN_QOS,                          /*                */
  EVENT_WLAN_PE,                           /*                 */
  EVENT_WLAN_ADD_BLOCK_ACK_SUCCESS,        /*                 */
  EVENT_WLAN_ADD_BLOCK_ACK_FAILED,         /*                */
  EVENT_WLAN_DELETE_BLOCK_ACK_SUCCESS,     /*                */
  EVENT_WLAN_DELETE_BLOCK_ACK_FAILED,      /*                */
  EVENT_WLAN_BSS_PROTECTION,               /*                */
  EVENT_WLAN_BRINGUP_STATUS,               /*                 */
  EVENT_WLAN_POWERSAVE_GENERIC,            /*                 */
  EVENT_WLAN_POWERSAVE_WOW,                /*                 */
  EVENT_WLAN_WCM,                          /*                 */
  EVENT_WLAN_WPS_SCAN_START,               /*                 */
  EVENT_WLAN_WPS_SCAN_COMPLETE,            /*                */
  EVENT_WLAN_WPS_CONNECT_REQUEST,          /*                */
  EVENT_WLAN_WPS_CONNECT_RESPONSE,         /*                */
  EVENT_WLAN_WPS_PBC_SESSION_OVERLAP,      /*                 */
  EVENT_WLAN_WPS_PBC_WALK_TIMER_START,     /*            */
  EVENT_WLAN_WPS_PBC_WALK_TIMER_STOP,      /*            */
  EVENT_WLAN_WPS_PBC_AP_DETECTED,          /*                 */
  EVENT_WLAN_WPS_REGISTRATION_START,       /*                */
  EVENT_WLAN_WPS_WSC_MESSAGE,              /*                */
  EVENT_WLAN_WPS_DISCOVERY,                /*                */
  EVENT_WLAN_WPS_REGISTRATION_COMPLETE,    /*                */
  EVENT_WLAN_WPS_DISCONNECT,               /*            */
  EVENT_WLAN_BTC,                          /*                 */

  EVENT_IPV6_SM_EVENT,                     /*                */
  EVENT_IPV6_SM_TRANSITION,                /*                */
  EVENT_IPV6_PREFIX_UPDATE,                /*                 */

  EVENT_LTE_ML1_STATE_CHANGE,              /*                */

  EVENT_AUTH_PROTO,                        /*                */
  EVENT_VSNCP,                             /*                */
  EVENT_IID,                               /*                */

  EVENT_IMS_VIDEOSHARE_REGISTRATION_SUCCESS,   /*                */
  EVENT_IMS_VIDEOSHARE_INVITE_SENT,            /*                */
  EVENT_IMS_VIDEOSHARE_INCOMING_INVITE,        /*                */
  EVENT_IMS_VIDEOSHARE_ACCEPT_REJECT_INVITE,   /*                */
  EVENT_IMS_VIDEOSHARE_ACCEPTING_SESSION,      /*                */
  EVENT_IMS_VIDEOSHARE_SESSION_ESTABLISHED,    /*                */
  EVENT_IMS_VIDEOSHARE_END_SESSION,            /*                */
  EVENT_IMS_VIDEOSHARE_PREVIEW_VIDEO_FRAME,    /*                */
  EVENT_IMS_VIDEOSHARE_DECODED_VIDEO_FRAME,    /*                */
  EVENT_IMS_VIDEOSHARE_RECEIVING,              /*                */
  EVENT_IMS_VIDEOSHARE_START_APPLICATION,      /*                */
  EVENT_IMS_VIDEOSHARE_END_APPLICATION,        /*                */
  EVENT_IMS_VIDEOSHARE_CAPABILITY_SUCCESS,     /*                */
  EVENT_IMS_VIDEOSHARE_MEDIA_RECORDING_RESOURCE_ACQUIRED,   /*                */
  EVENT_IMS_VIDEOSHARE_MEDIA_RECORDING_RESOURCE_RELEASED,   /*                */
  EVENT_IMS_VIDEOSHARE_SENDING,                             /*                */
  EVENT_IMS_VIDEOSHARE_INCOMING_OPTION_RECEIVED,            /*                */
  EVENT_IMS_VIDEOSHARE_INCOMING_OPTION_RESPONDED,           /*                */
  EVENT_IMS_VIDEOSHARE_ERR_CALL_FAILED,                     /*                */
  EVENT_IMS_VIDEOSHARE_ERR_REGISTRATION_FAILED,             /*                */
  EVENT_IMS_VIDEOSHARE_ERR_RECORDER_ERROR,                  /*                */
  EVENT_IMS_VIDEOSHARE_ERR_PLAYER_ERROR,                    /*                */
  EVENT_IMS_VIDEOSHARE_ERR_MEDIA_SESSION_FAILURE,           /*                */
  EVENT_IMS_VIDEOSHARE_ERR_CAPABILITY_FAILURE,              /*                */
  EVENT_IMS_VIDEOSHARE_ERR_MEDIA_RECORDING_FAILED,          /*                */

  EVENT_WLAN_PE_FRAME,                     /*                 */

  EVENT_SNS_VCPS_HEADING_COMPUTED,         /*                 */ 
  EVENT_SNS_VCPS_TRACKED_CAL_SET_SAVED,    /*                 */ 

  EVENT_GNSS_PRESC_DWELL_COMPLETE,         /*                */ 

  EVENT_LTE_MAC_RESET,                     /*                */ 
  EVENT_LTE_BSR_SR_REQUEST,                /*                */ 
  EVENT_LTE_MAC_TIMER,                     /*                */ 

  EVENT_CM_DS_OPERATIONAL_MODE,            /*                */
  EVENT_CM_DS_MODE_PREF,                   /*                */
  EVENT_CM_DS_GW_ACQ_ORDER_PREF,           /*                */
  EVENT_CM_DS_SRV_DOMAIN_PREF,             /*                */
  EVENT_CM_DS_BAND_PREF,                   /*                */
  EVENT_CM_DS_ROAM_PREF,                   /*                */
  EVENT_CM_DS_HYBRID_PREF,                 /*                */
  EVENT_CM_DS_NETWORK_SEL_MODE_PREF,       /*                */
  EVENT_CM_DS_CALL_EVENT_ORIG,             /*                */
  EVENT_CM_DS_CALL_EVENT_CONNECT,          /*                */
  EVENT_CM_DS_CALL_EVENT_END,              /*                */
  EVENT_CM_DS_ENTER_EMERGENCY_CB,          /*                */
  EVENT_CM_DS_EXIT_EMERGENCY_CB,           /*                */
  EVENT_CM_DS_CALL_STATE,                  /*                */
  EVENT_CM_DS_DS_INTERRAT_STATE,           /*                */
  EVENT_CM_DS_CELL_SRV_IND,                /*                */
  EVENT_CM_DS_COUNTRY_SELECTED,            /*                */
  EVENT_CM_DS_DATA_AVAILABLE,              /*                */
  EVENT_CM_DS_SELECT_COUNTRY,              /*                */
  EVENT_CM_DS_CALL_EVENT_ORIG_THR,         /*                */
  EVENT_CM_DS_PLMN_FOUND,                  /*                 */
  EVENT_CM_DS_SERVICE_CONFIRMED,           /*                 */
  EVENT_CM_DS_GET_PASSWORD_IND,            /*                */
  EVENT_CM_DS_PASSWORD_AUTHENTICATION_STATUS, /*                */
  EVENT_CM_DS_USS_RESPONSE_NOTIFY_IND,     /*                */
  EVENT_CM_DS_LCS_MOLR_CONF,               /*                */

  EVENT_DS_NAS_MESSAGE_SENT,               /*                */ 
  EVENT_DS_NAS_MESSAGE_RECEIVED,           /*                */  
  EVENT_DS_MM_STATE,                       /*                */ 
  EVENT_DS_GMM_STATE,                      /*                */ 
  EVENT_DS_PLMN_INFORMATION,               /*                 */ 

  EVENT_DIAG_STRESS_TEST_COMPLETED,        /*                */ 

  EVENT_GNSS_CC_STATUS,                    /*                */ 

  EVENT_SNS_USER_STATE_CHANGE,             /*                */ 

  EVENT_DS_HPLMN_TIMER_EXPIRED,            /*                */  
  EVENT_DS_RAT_CHANGE,                     /*                */ 

  EVENT_DTV_CMMB_API_CALL_ACTIVATE,               /*              */ /*       */ 
  EVENT_DTV_CMMB_API_CALL_DEACTIVATE,           /*              */
  EVENT_DTV_CMMB_API_CALL_TUNE,                     /*               */
  EVENT_DTV_CMMB_API_CALL_SELECT_SERVICE,     /*               */
  EVENT_DTV_CMMB_API_CALL_DESELECT_SERVICE,  /*               */
  EVENT_DTV_CMMB_API_CALL_GET_SIGNAL_PARAMETERS,      /*              */
  EVENT_DTV_CMMB_API_CALL_GET_NIT,                /*              */
  EVENT_DTV_CMMB_API_CALL_GET_CMCT,             /*              */
  EVENT_DTV_CMMB_API_CALL_GET_SMCT,             /*              */
  EVENT_DTV_CMMB_API_CALL_GET_CSCT,             /*              */
  EVENT_DTV_CMMB_API_CALL_GET_SSCT,             /*              */
  EVENT_DTV_CMMB_API_CALL_GET_EADT,             /*              */
  EVENT_DTV_CMMB_API_CALL_REQUEST_CA_CARD_NUMBER,   /*              */
  EVENT_DTV_CMMB_API_CALL_REQUEST_CAS_ID,                 /*              */
  EVENT_DTV_CMMB_API_CALL_REGISTER_FOR_CONTROL_NOTIFICATIONS,         /*              */
  EVENT_DTV_CMMB_API_CALL_DEREGISTER_FROM_CONTROL_NOTIFICATIONS,   /*              */
  EVENT_DTV_CMMB_API_NOTIFICATION_ACTIVATE,               /*              */
  EVENT_DTV_CMMB_API_NOTIFICATION_DEACTIVATE,           /*              */
  EVENT_DTV_CMMB_API_NOTIFICATION_TUNE,      /*               */ 
  EVENT_DTV_CMMB_API_NOTIFICATION_SELECT_SERVICE,     /*               */
  EVENT_DTV_CMMB_API_NOTIFICATION_DESELECT_SERVICE,  /*               */
  EVENT_DTV_CMMB_API_NOTIFICATION_TABLE_UPDATE,        /*              */
  EVENT_DTV_CMMB_API_NOTIFICATION_SIGNAL_PARAMETERS,/*               */
  EVENT_DTV_CMMB_API_NOTIFICATION_AUTHORIZATION_FAILURE,                 /*               */
  EVENT_DTV_CMMB_API_NOTIFICATION_REGISTER_FOR_CONTROL_NOTIFICATIONS_COMPLETE,      /*              */
  EVENT_DTV_CMMB_API_NOTIFICATION_DEREGISTER_FROM_CONTROL_NOTIFICATIONS_COMPLETE, /*              */
  EVENT_DTV_CMMB_API_NOTIFICATION_CA_CARD_NUMBER,   /*              */
  EVENT_DTV_CMMB_API_NOTIFICATION_CAS_ID,                 /*              */
  EVENT_DTV_CMMB_API_NOTIFICATION_EMERGENCY_BROADCASTING_TRIGGER, /*              */
  EVENT_DTV_CMMB_API_NOTIFICATION_EMERGENCY_BROADCASTING_MESSAGE,/*              */
  EVENT_DTV_CMMB_API_CALL_REGISTER_FOR_ESG_NOTIFICATIONS,                /*              */
  EVENT_DTV_CMMB_API_CALL_DEREGISTER_FROM_ESG_NOTIFICATIONS,          /*              */
  EVENT_DTV_CMMB_API_CALL_GET_BASIC_DESCRIPTION_INFORMATION,          /*              */
  EVENT_DTV_CMMB_API_CALL_SET_OUTPUT_PATH,/*              */
  EVENT_DTV_CMMB_API_NOTIFICATION_ESG_DATA_INFORMATION,                  /*              */
  EVENT_DTV_CMMB_API_NOTIFICATION_ESG_DATA_INFORMATION_DOWNLOAD_COMPLETE,         /*              */
  EVENT_DTV_CMMB_API_NOTIFICATION_ESG_PROGRAM_INDICATION_INFORMATION,                   /*              */
  EVENT_DTV_CMMB_API_NOTIFICATION_REGISTER_FOR_ESG_NOTIFICATIONS_COMPLETE,           /*              */
  EVENT_DTV_CMMB_API_NOTIFICATION_DEREGISTER_FROM_ESG_NOTIFICATIONS_COMPLETE,     /*              */
  EVENT_DTV_CMMB_CAS_INITIALIZED,                 /*              */
  EVENT_DTV_CMMB_CAS_EMM_RECEIVED_AND_PROCESSED,    /*              */
  EVENT_DTV_CMMB_CAS_ECM_RECEIVED_AND_PROCESSED,    /*               */ /*       */
  
  EVENT_ECALL_START,                                      /*              */ 
  EVENT_ECALL_STOP,                                       /*              */ 
  EVENT_ECALL_SESSION_START,                        /*              */
  EVENT_ECALL_SESSION_FAILURE,                      /*              */
  EVENT_ECALL_SESSION_COMPLETE,                   /*              */
  EVENT_ECALL_SESSION_RESET,                        /*              */
  EVENT_ECALL_PSAP_MSD_DECODE_SUCCESS,     /*              */
  EVENT_ECALL_PSAP_LOST_SYNC,                     /*              */ /*         */

  EVENT_LTE_RRC_IRAT_REDIR_FROM_EUTRAN_START, /*               */
  EVENT_LTE_RRC_IRAT_REDIR_FROM_EUTRAN_END,    /*                */ /*         */

  EVENT_GPRS_DS_CELL_CHANGE_ORDER,            /*                */ /*    */
  EVENT_GSM_DS_CELL_SELECTION_END,            /*                */ /*    */
  EVENT_GSM_DS_L1_STATE,                      /*                */ /*    */
  EVENT_GSM_DS_PLMN_LIST_START,               /*                */ /*    */
  EVENT_GSM_DS_PLMN_LIST_END,                 /*                */ /*    */
  EVENT_GSM_DS_POWER_SCAN_STATUS,             /*                */ /*    */
  EVENT_GSM_DS_RESELECT_START,                /*                */ /*    */
  EVENT_GSM_DS_RR_IN_SERVICE,                 /*                */ /*    */
  EVENT_GSM_DS_RR_OUT_OF_SERVICE,             /*                */ /*    */
  EVENT_GSM_DS_TIMER_EXPIRED,                 /*                */ /*    */
  EVENT_GSM_DS_TO_WCDMA_RESELECT_END,         /*                */ /*    */

  EVENT_CM_DS_SYSTEM_MODE,                    /*              */ /*    */
  EVENT_SD_DS_EVENT_ACTION,                   /*              */ /*    */
  EVENT_SMGMM_DS_REQUEST_SENT,                /*              */ /*    */

  EVENT_IFACE,                                /*              */ /*    */
  EVENTS_DS_GSM_L1_ALIGN_VFR = 0x720,
  EVENTS_DS_GSM_L1_STATE = 0x721,
  EVENTS_DS_GSM_RATSCCH_IN_DTX = 0x722,
  EVENTS_DS_GSM_FACCH_IN_DTX = 0x723,
  EVENTS_DS_GSM_FACCH_AND_RATSCCH_COLLISION = 0x724,
  EVENTS_DS_GSM_FACCH_AND_SID_UPDATE_COLLISION = 0x725,
  EVENTS_DS_GSM_RATSCCH_AND_SID_UPDATE_COLLISION = 0x726,
  EVENTS_DS_GSM_AMR_STATE_CHANGE = 0x727,
  EVENTS_DS_GSM_RATSCCH_CMI_PHASE_CHANGE = 0x728,
  EVENTS_DS_GSM_RATSCCH_REQ_ACT_TIMER_EXPIRY = 0x729,
  EVENTS_DS_GSM_RATSCCH_ACK_ACT_TIMER_EXPIRY = 0x72a,
  EVENTS_DS_GSM_AMR_RATSCCH_REQ = 0x72b,
  EVENTS_DS_GSM_AMR_RATSCCH_RSP = 0x72c,
  EVENTS_DS_GSM_AMR_CMC_TURNAROUND_TIME = 0x72d,
  EVENTS_DS_GPRS_SMGMM_MSG_RECEIVED = 0x72e,
  EVENTS_DS_GPRS_SMGMM_MSG_SENT = 0x72f,
  EVENTS_DS_GPRS_LLC_READY_TIMER_START = 0x730,
  EVENTS_DS_GPRS_LLC_READY_TIMER_END = 0x731,
  EVENTS_DS_PACKET_TIMESLOT_RECONFIGURE = 0x732,
  EVENTS_DS_GPRS_MAC_MSG_RECEIVED = 0x733,
  EVENTS_DS_GPRS_MAC_MSG_SENT = 0x734,
  EVENTS_DS_GPRS_MAC_CAMPED_ON_CELL = 0x735,
  EVENTS_DS_GPRS_CELL_CHANGE_FAILURE = 0x736,
  EVENTS_DS_GPRS_PACKET_CHANNEL_REQUEST = 0x737,
  EVENTS_DS_GPRS_PACKET_UPLINK_ASSIGNMENT = 0x738,
  EVENTS_DS_GPRS_PACKET_DOWNLINK_ASSIGNMENT = 0x739,
  EVENTS_DS_GPRS_TBF_RELEASE = 0x73a,
  EVENTS_DS_GPRS_TIMER_EXPIRY = 0x73b,
  EVENTS_DS_GPRS_PACKET_RESOURCE_REQUEST = 0x73c,
  EVENTS_DS_RANDOM_ACCESS_REQUEST = 0x73d,
  EVENTS_DS_GSM_HANDOVER_START = 0x73e,
  EVENTS_DS_GSM_HANDOVER_END = 0x73f,
  EVENTS_DS_GSM_RESELECT_START = 0x740,
  EVENTS_DS_GSM_RESELECT_END = 0x741,
  EVENTS_DS_GSM_TO_WCDMA_RESELECT_END = 0x742,
  EVENTS_DS_GSM_MESSAGE_RECEIVED = 0x743,
  EVENTS_DS_GSM_RR_IN_SERVICE = 0x744,
  EVENTS_DS_GSM_RR_OUT_OF_SERVICE = 0x745,
  EVENTS_DS_GSM_PAGE_RECEIVED = 0x746,
  EVENTS_DS_GSM_CAMP_ATTEMPT_START = 0x747,
  EVENTS_DS_GSM_CAMP_ATTEMPT_END = 0x748,
  EVENTS_DS_GSM_CALL_DROP = 0x749,
  EVENTS_DS_GSM_ACCESS_FAILURE = 0x74a,
  EVENTS_DS_GSM_CELL_SELECTION_START = 0x74b,
  EVENTS_DS_GSM_CELL_SELECTION_END = 0x74c,
  EVENTS_DS_GSM_POWER_SCAN_STATUS = 0x74d,
  EVENTS_DS_GSM_PLMN_LIST_START = 0x74e,
  EVENTS_DS_GSM_PLMN_LIST_END = 0x74f,
  EVENTS_DS_GSM_AMR_MULTIRATE_IE = 0x750,
  EVENTS_DS_GPRS_LINK_FAILURE = 0x751,
  EVENTS_DS_GPRS_PAGE_RECEIVED = 0x752,
  EVENTS_DS_GPRS_SURROUND_SEARCH_START = 0x753,
  EVENTS_DS_GPRS_SURROUND_SEARCH_END = 0x754,
  EVENTS_DS_GPRS_EARLY_CAMPING = 0x755,
  EVENTS_DS_GSM_LINK_FAILURE = 0x756,


  EVENT_MTP_FILE_DELETED = 0x757,
  EVENT_MTP_PLAYLIST_REMOVED_OBJECT = 0x758,
  EVENT_MTP_SYNC_STARTED = 0x759,
  EVENT_MTP_SYNC_FINISHED = 0x75a,
  EVENT_MTP_SAVE_ALBUMART_STARTED = 0x75b,
  EVENT_MTP_SAVE_ALBUMART_FINISHED = 0x75c,
  EVENT_MTP_FORMAT_STORE_STARTED = 0x75d,
  EVENT_MTP_FORMAT_STORE_DONE = 0x75e,
  EVENT_MTP_FORMAT_STORE_ERROR = 0x75f,
  EVENT_LTE_RRC_SECURITY_CONFIG = 0x760,
  EVENT_LTE_RRC_IRAT_RESEL_FROM_EUTRAN_START = 0x761,
  EVENT_LTE_RRC_IRAT_RESEL_FROM_EUTRAN_END = 0x762,
  EVENT_SNS_REST_DETECT_ACCEL_ACTIVE_TS = 0x763,
  EVENT_SNS_REST_DETECT_ACCEL_STOP_TS = 0x764,
  EVENT_CPC_CONFIG_ACTION = 0x765,
  EVENT_FDPCH_CONFIG_ACTION = 0x766,
  EVENT_SNS_DRV_MOTION_DETECT_SIG = 0x767,
  EVENT_SNS_DRV_OPMODE_CHANGE = 0x768,

  EVENT_NEXT_UNUSED_EVENT,
  EVENT_RSVD_START = 0x0800,
  EVENT_RSVD_END   = 0x083F,
  EVENT_LAST_ID    = 0x083F,

  EVENT_MAX_ID     = 0x0FFF
} event_id_enum_type;


#endif /*              */

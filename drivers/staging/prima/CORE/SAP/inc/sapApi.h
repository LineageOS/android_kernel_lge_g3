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

#ifndef WLAN_QCT_WLANSAP_H
#define WLAN_QCT_WLANSAP_H

/*===========================================================================

               W L A N   S O F T A P  P A L   L A Y E R 
                       E X T E R N A L  A P I
                
                   
DESCRIPTION
  This file contains the external API exposed by the wlan SAP PAL layer 
  module.
  
      
  Copyright (c) 2010 QUALCOMM Incorporated. All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/


/*                                                                           

                                           


                                                                       
                                                                


                                                                                                                                                                                                                          


                                                  
                                                                                          
                                           

                                                                           */



/*                                                                           

                                                  

                                                                           */

/*                                                                            
                
                                                                            */
#include "vos_api.h" 
#include "vos_packet.h" 
#include "vos_types.h"

#include "p2p_Api.h"

/*                                                                            
                                         
                                                                            */
 #ifdef __cplusplus
 extern "C" {
 #endif 
 


/*                                                                            
           
                                                                            */
 
/*                                                                          
                  
                                                                          */

#define       MAX_SSID_LEN                 32
#define       MAX_ACL_MAC_ADDRESS          16
#define       AUTO_CHANNEL_SELECT          0
#define       MAX_ASSOC_IND_IE_LEN         255

/*                               */
#define       SAP_WPS_DISABLED             0
#define       SAP_WPS_ENABLED_UNCONFIGURED 1
#define       SAP_WPS_ENABLED_CONFIGURED   2

#define       MAX_NAME_SIZE                64
#define       MAX_TEXT_SIZE                32


/*                                                                          
                                                                                          
                                                                          */

typedef enum{  
    eSAP_RC_RESERVED0,               /* */  
    eSAP_RC_UNSPECIFIED,             /* */  
    eSAP_RC_PREV_AUTH_INVALID,       /* */  
    eSAP_RC_STA_LEFT_DEAUTH,         /* */  
    eSAP_RC_INACTIVITY_DISASSOC,     /* */ 
    eSAP_RC_AP_CAPACITY_FULL,        /* */
    eSAP_RC_CLS2_FROM_NON_AUTH_STA,  /* */
    eSAP_RC_CLS3_FROM_NON_AUTH_STA,  /* */
    eSAP_RC_STA_LEFT_DISASSOC,       /* */
    eSAP_RC_STA_NOT_AUTH,            /* */
    eSAP_RC_PC_UNACCEPTABLE,         /*  */
    eSAP_RC_SC_UNACCEPTABLE,         /*  */
    eSAP_RC_RESERVED1,               /*  */
    eSAP_RC_INVALID_IE,              /*  */
    eSAP_RC_MIC_FAIL,                /*  */
    eSAP_RC_4_WAY_HANDSHAKE_TO,      /*  */
    eSAP_RC_GO_KEY_HANDSHAKE_TO,     /*  */
    eSAP_RC_IE_MISMATCH,             /*  */
    eSAP_RC_INVALID_GRP_CHIPHER,     /*  */
    eSAP_RC_INVALID_PAIR_CHIPHER,    /*  */
    eSAP_RC_INVALID_AKMP,            /*  */
    eSAP_RC_UNSUPPORTED_RSN,         /*  */
    eSAP_RC_INVALID_RSN,             /*  */
    eSAP_RC_1X_AUTH_FAILED,          /*  */
    eSAP_RC_CHIPER_SUITE_REJECTED,   /*  */
}eSapReasonCode;    

typedef enum {
    eSAP_DOT11_MODE_abg = 0x0001,
    eSAP_DOT11_MODE_11a = 0x0002,
    eSAP_DOT11_MODE_11b = 0x0004,     
    eSAP_DOT11_MODE_11g = 0x0008,     
    eSAP_DOT11_MODE_11n = 0x0010,     
    eSAP_DOT11_MODE_11g_ONLY = 0x0080,
    eSAP_DOT11_MODE_11n_ONLY = 0x0100,
    eSAP_DOT11_MODE_11b_ONLY = 0x0400,
#ifdef WLAN_FEATURE_11AC
    eSAP_DOT11_MODE_11ac     = 0x1000,
    eSAP_DOT11_MODE_11ac_ONLY = 0x2000
#endif
} eSapPhyMode;

typedef enum {
    eSAP_ACCEPT_UNLESS_DENIED = 0,
    eSAP_DENY_UNLESS_ACCEPTED = 1,
    eSAP_SUPPORT_ACCEPT_AND_DENY = 2, /*                                                                           */
    eSAP_ALLOW_ALL = 3, /*                                                     */
} eSapMacAddrACL;

typedef enum {
    eSAP_BLACK_LIST = 0, /*                                            */
    eSAP_WHITE_LIST = 1, /*                                        */
} eSapACLType;

typedef enum {
    ADD_STA_TO_ACL      = 0, /*                                       */
    DELETE_STA_FROM_ACL = 1, /*                                            */
} eSapACLCmdType;

typedef enum {
    eSAP_START_BSS_EVENT = 0, /*                              */
    eSAP_STOP_BSS_EVENT,      /*                              */
    eSAP_STA_ASSOC_IND,       /*                                                  */    
    eSAP_STA_ASSOC_EVENT,     /*                                                              
                                                                        */
    eSAP_STA_REASSOC_EVENT,   /*                                                                
                                                                        */
    eSAP_STA_DISASSOC_EVENT,  /*                                                                                         */
    eSAP_STA_SET_KEY_EVENT,   /*                                              */
    eSAP_STA_DEL_KEY_EVENT,   /*                                              */
    eSAP_STA_MIC_FAILURE_EVENT, /*                                                  */
    eSAP_ASSOC_STA_CALLBACK_EVENT,  /*                                                     */
    eSAP_GET_WPSPBC_SESSION_EVENT,  /*                                                         */  
    eSAP_WPS_PBC_PROBE_REQ_EVENT, /*                                                 */
    eSAP_INDICATE_MGMT_FRAME,
    eSAP_REMAIN_CHAN_READY,
    eSAP_SEND_ACTION_CNF,
    eSAP_DISCONNECT_ALL_P2P_CLIENT,
    eSAP_MAC_TRIG_STOP_BSS_EVENT,
    eSAP_UNKNOWN_STA_JOIN, /*                                                                                             */
    eSAP_MAX_ASSOC_EXCEEDED, /*                                                                                            */
} eSapHddEvent;

typedef enum {
    eSAP_OPEN_SYSTEM,
    eSAP_SHARED_KEY,
    eSAP_AUTO_SWITCH
 } eSapAuthType; 

typedef enum {
    eSAP_MAC_INITATED_DISASSOC = 0x10000, /*                                                      */
    eSAP_USR_INITATED_DISASSOC /*                                                                                     */
 } eSapDisassocReason; 

/*                        */
typedef enum {
    eSAP_FALSE,
    eSAP_TRUE,
}eSapBool;

/*                                                                           
                                                 
                                                                            */    
typedef enum  {
    eSAP_STATUS_SUCCESS,                 /*           */
    eSAP_STATUS_FAILURE,                 /*                   */
    eSAP_START_BSS_CHANNEL_NOT_SELECTED, /*                                           */
    eSAP_ERROR_MAC_START_FAIL,           /*                           */
}eSapStatus;

/*                                                                           
                                                 
                                                                            */    
typedef enum  {
    eSAP_WPSPBC_OVERLAP_IN120S,                 /*         */
    eSAP_WPSPBC_NO_WPSPBC_PROBE_REQ_IN120S,     /*                                    */
    eSAP_WPSPBC_ONE_WPSPBC_PROBE_REQ_IN120S,    /*                                      */
}eWPSPBCOverlap;

/*                                                                            
            
                                                                            */
typedef struct sap_StartBssCompleteEvent_s {
    v_U8_t  status;
    v_U8_t  operatingChannel;
    v_U16_t staId; //          
} tSap_StartBssCompleteEvent;

typedef struct sap_StopBssCompleteEvent_s {
    v_U8_t status;
} tSap_StopBssCompleteEvent;

typedef struct sap_StationAssocIndication_s {
    v_MACADDR_t  staMac;
    v_U8_t       assoId;
    v_U8_t       staId;
    v_U8_t       status;
    //                                                  
    tANI_U32     beaconLength;
    tANI_U8*     beaconPtr;
    tANI_U32     assocReqLength;
    tANI_U8*     assocReqPtr;
    tANI_BOOLEAN fWmmEnabled;
    eCsrAuthType negotiatedAuthType;
    eCsrEncryptionType negotiatedUCEncryptionType;
    eCsrEncryptionType negotiatedMCEncryptionType;
    tANI_BOOLEAN fAuthRequired;
} tSap_StationAssocIndication;

typedef struct sap_StationAssocReassocCompleteEvent_s {
    v_MACADDR_t  staMac;
    v_U8_t       staId;
    v_U8_t       status;
    v_U8_t       ies[MAX_ASSOC_IND_IE_LEN];
    v_U16_t      iesLen;
    v_U32_t      statusCode; 
    eSapAuthType SapAuthType;
    v_BOOL_t     wmmEnabled;
    //                                                  
    tANI_U32     beaconLength;
    tANI_U8*     beaconPtr;
    tANI_U32     assocReqLength;
    tANI_U8*     assocReqPtr;
    tANI_U32     assocRespLength;
    tANI_U8*     assocRespPtr;    
} tSap_StationAssocReassocCompleteEvent;

typedef struct sap_StationDisassocCompleteEvent_s {
    v_MACADDR_t        staMac;
    v_U8_t             staId;    //                        
    v_U8_t             status;
    v_U32_t            statusCode;
    eSapDisassocReason reason;
} tSap_StationDisassocCompleteEvent;

typedef struct sap_StationSetKeyCompleteEvent_s {
    v_U8_t        status;
    v_MACADDR_t   peerMacAddr;
} tSap_StationSetKeyCompleteEvent;

/*                                              */
typedef struct sap_StationDeleteKeyCompleteEvent_s {
    v_U8_t status;
    v_U8_t  keyId; /*           */
} tSap_StationDeleteKeyCompleteEvent;

/*                                                  */
typedef struct sap_StationMICFailureEvent_s {
    v_MACADDR_t   srcMacAddr; //                            
    v_MACADDR_t   staMac; //                             
    v_MACADDR_t   dstMacAddr;
    eSapBool   multicast;
    v_U8_t     IV1;            //                 
    v_U8_t     keyId;          //                  
    v_U8_t     TSC[SIR_CIPHER_SEQ_CTR_SIZE]; //                

} tSap_StationMICFailureEvent;
/*                                                       */
typedef struct sap_AssocMacAddr_s {
    v_MACADDR_t staMac;     /*                                         */
    v_U8_t      assocId;        /*                                                 */
    v_U8_t      staId;            /*                                           */
    v_U8_t      ShortGI40Mhz;
    v_U8_t      ShortGI20Mhz;
    v_U8_t      Support40Mhz;
    v_U32_t     requestedMCRate;
    tSirSupportedRates supportedRates;
} tSap_AssocMacAddr, *tpSap_AssocMacAddr;

/*                                                     */
typedef struct sap_AssocStaListEvent_s {
    VOS_MODULE_ID      module; /*                                                          */
    v_U8_t             noOfAssocSta;  /*                              */
    tpSap_AssocMacAddr pAssocStas; /*                                                             
                                                                                   */
} tSap_AssocStaListEvent;
 
typedef struct sap_GetWPSPBCSessionEvent_s {
    v_U8_t         status;
    VOS_MODULE_ID  module; /*                                                          */       
    v_U8_t         UUID_E[16];         //                             
    v_MACADDR_t    addr;  
    eWPSPBCOverlap wpsPBCOverlap;
} tSap_GetWPSPBCSessionEvent; 

typedef struct sap_WPSPBCProbeReqEvent_s {
    v_U8_t             status;
    VOS_MODULE_ID      module; /*                                                          */   
    tSirWPSPBCProbeReq WPSPBCProbeReq;
} tSap_WPSPBCProbeReqEvent; 

typedef struct sap_ManagementFrameInfo_s {
    tANI_U32 nFrameLength;
    tANI_U8  frameType;
    tANI_U32 rxChan;            //                                    
    tANI_U8 *pbFrames;         //                                                                               
                             //                                                                                 
                            //                           
} tSap_ManagementFrameInfo;

typedef struct sap_SendActionCnf_s {
    eSapStatus actionSendSuccess; 
} tSap_SendActionCnf;

typedef struct sap_UnknownSTAJoinEvent_s {
    v_MACADDR_t    macaddr;  
} tSap_UnknownSTAJoinEvent;

typedef struct sap_MaxAssocExceededEvent_s {
    v_MACADDR_t    macaddr;  
} tSap_MaxAssocExceededEvent;


/* 
                                                                                                                 
                                                                                                    
*/

typedef struct sap_Event_s {
    eSapHddEvent sapHddEventCode;
    union {
        tSap_StartBssCompleteEvent                sapStartBssCompleteEvent; /*                   */
        tSap_StopBssCompleteEvent                 sapStopBssCompleteEvent;  /*                  */
        tSap_StationAssocIndication               sapAssocIndication;       /*                     */         
        tSap_StationAssocReassocCompleteEvent     sapStationAssocReassocCompleteEvent; /*                                          */
        tSap_StationDisassocCompleteEvent         sapStationDisassocCompleteEvent;/*                      */
        tSap_StationSetKeyCompleteEvent           sapStationSetKeyCompleteEvent;/*                     */
        tSap_StationDeleteKeyCompleteEvent        sapStationDeleteKeyCompleteEvent;/*                     */
        tSap_StationMICFailureEvent               sapStationMICFailureEvent; /*                          */
        tSap_AssocStaListEvent                    sapAssocStaListEvent; /*                             */
        tSap_GetWPSPBCSessionEvent                sapGetWPSPBCSessionEvent; /*                             */
        tSap_WPSPBCProbeReqEvent                  sapPBCProbeReqEvent; /*                             */
        tSap_ManagementFrameInfo                  sapManagementFrameInfo; /*                        */
        tSap_SendActionCnf                        sapActionCnf;  /*                      */ 
        tSap_UnknownSTAJoinEvent                  sapUnknownSTAJoin; /*                       */
        tSap_MaxAssocExceededEvent                sapMaxAssocExceeded; /*                         */
    } sapevt;
} tSap_Event, *tpSap_Event;


typedef __ani_attr_pre_packed struct sap_SSID {
    v_U8_t       length;
    v_U8_t       ssId[MAX_SSID_LEN];
} __ani_attr_packed tSap_SSID_t;

typedef __ani_attr_pre_packed struct sap_SSIDInfo {
    tSap_SSID_t  ssid;       /*              */
    v_U8_t       ssidHidden; /*                                                          */
} __ani_attr_packed tSap_SSIDInfo_t;

typedef struct sap_Config {
    tSap_SSIDInfo_t SSIDinfo;
    eSapPhyMode     SapHw_mode; /*               */
    eSapMacAddrACL  SapMacaddr_acl;
    v_MACADDR_t     accept_mac[MAX_ACL_MAC_ADDRESS]; /*               */
    v_BOOL_t        ieee80211d;      /*                                     */
    v_BOOL_t        protEnabled;     /*                                            */
    v_BOOL_t        obssProtEnabled; /*                                                 */
    v_MACADDR_t     deny_mac[MAX_ACL_MAC_ADDRESS]; /*               */
    v_MACADDR_t     self_macaddr; //                        
   
    v_U8_t          channel;         /*                   */
    v_U8_t          max_num_sta;     /*                                         */
    v_U8_t          dtim_period;     /*               */
    v_U8_t          num_accept_mac;
    v_U8_t          num_deny_mac;
    v_U8_t          *pRSNWPAReqIE;   //                                                   

    v_U8_t          countryCode[WNI_CFG_COUNTRY_CODE_LEN];  //                          
    v_U8_t          RSNAuthType;
    v_U8_t          RSNEncryptType;
    v_U8_t          mcRSNEncryptType;
    eSapAuthType    authType;
    v_BOOL_t        privacy;
    v_BOOL_t        UapsdEnable;
    v_BOOL_t        fwdWPSPBCProbeReq;
    v_U8_t          wps_state; //                                                  

    v_U16_t         ht_capab;
    v_U16_t         RSNWPAReqIELength;   //                               

    v_U32_t         beacon_int;     /*                 */
    v_U32_t         ap_table_max_size;
    v_U32_t         ap_table_expiration_time;
    v_U32_t         ht_op_mode_fixed;
    tVOS_CON_MODE   persona; /*                                             */

} tsap_Config_t;

typedef enum {
    eSAP_WPS_PROBE_RSP_IE,
    eSAP_WPS_BEACON_IE,
    eSAP_WPS_ASSOC_RSP_IE
} eSapWPSIE_CODE;

typedef struct sSapName {
    v_U8_t num_name;
    v_U8_t name[MAX_NAME_SIZE];
} tSapName;

typedef struct sSapText {
    v_U8_t num_text;
    v_U8_t text[MAX_TEXT_SIZE];
} tSapText;

#define WPS_PROBRSP_VER_PRESENT                          0x00000001
#define WPS_PROBRSP_STATE_PRESENT                        0x00000002
#define WPS_PROBRSP_APSETUPLOCK_PRESENT                  0x00000004
#define WPS_PROBRSP_SELECTEDREGISTRA_PRESENT             0x00000008
#define WPS_PROBRSP_DEVICEPASSWORDID_PRESENT             0x00000010
#define WPS_PROBRSP_SELECTEDREGISTRACFGMETHOD_PRESENT    0x00000020
#define WPS_PROBRSP_RESPONSETYPE_PRESENT                 0x00000040
#define WPS_PROBRSP_UUIDE_PRESENT                        0x00000080
#define WPS_PROBRSP_MANUFACTURE_PRESENT                  0x00000100
#define WPS_PROBRSP_MODELNAME_PRESENT                    0x00000200
#define WPS_PROBRSP_MODELNUMBER_PRESENT                  0x00000400
#define WPS_PROBRSP_SERIALNUMBER_PRESENT                 0x00000800
#define WPS_PROBRSP_PRIMARYDEVICETYPE_PRESENT            0x00001000
#define WPS_PROBRSP_DEVICENAME_PRESENT                   0x00002000
#define WPS_PROBRSP_CONFIGMETHODS_PRESENT                0x00004000
#define WPS_PROBRSP_RF_BANDS_PRESENT                     0x00008000

typedef struct sap_WPSProbeRspIE_s {
    v_U32_t     FieldPresent;
    v_U32_t     Version;           //                                         
    v_U32_t     wpsState;          //                                      
    v_BOOL_t    APSetupLocked;     //                                  
    v_BOOL_t    SelectedRegistra;  //                                                                                   
    v_U16_t     DevicePasswordID;  //                   
    v_U16_t     SelectedRegistraCfgMethod; //                                 
    v_U8_t      ResponseType;      //              
    v_U8_t      UUID_E[16];         //                             
    tSapName    Manufacture;
    tSapText    ModelName;
    tSapText    ModelNumber;
    tSapText    SerialNumber;
    v_U32_t     PrimaryDeviceCategory ; //                                                  
    v_U8_t      PrimaryDeviceOUI[4] ; //                                            
    v_U32_t     DeviceSubCategory ; //                                                                         
    tSapText    DeviceName;
    v_U16_t    ConfigMethod;     //                      
    v_U8_t    RFBand;           //                             
} tSap_WPSProbeRspIE;

#define WPS_BEACON_VER_PRESENT                         0x00000001
#define WPS_BEACON_STATE_PRESENT                       0x00000002
#define WPS_BEACON_APSETUPLOCK_PRESENT                 0x00000004
#define WPS_BEACON_SELECTEDREGISTRA_PRESENT            0x00000008
#define WPS_BEACON_DEVICEPASSWORDID_PRESENT            0x00000010
#define WPS_BEACON_SELECTEDREGISTRACFGMETHOD_PRESENT   0x00000020
#define WPS_BEACON_UUIDE_PRESENT                       0x00000080
#define WPS_BEACON_RF_BANDS_PRESENT                    0x00000100

typedef struct sap_WPSBeaconIE_s {
    v_U32_t  FieldPresent;
    v_U32_t  Version;           //                                         
    v_U32_t  wpsState;          //                                      
    v_BOOL_t APSetupLocked;     //                                  
    v_BOOL_t SelectedRegistra;  //                                                                                   
    v_U16_t  DevicePasswordID;  //                   
    v_U16_t  SelectedRegistraCfgMethod; //                                 
    v_U8_t   UUID_E[16];        //                             
    v_U8_t   RFBand;           //                             
} tSap_WPSBeaconIE;

#define WPS_ASSOCRSP_VER_PRESENT             0x00000001
#define WPS_ASSOCRSP_RESPONSETYPE_PRESENT    0x00000002

typedef struct sap_WPSAssocRspIE_s {
   v_U32_t FieldPresent;
   v_U32_t Version;
   v_U8_t ResposeType;
} tSap_WPSAssocRspIE;

typedef struct sap_WPSIE_s {
    eSapWPSIE_CODE sapWPSIECode;
    union {
               tSap_WPSProbeRspIE  sapWPSProbeRspIE;    /*                        */
               tSap_WPSBeaconIE    sapWPSBeaconIE;      /*                 */
               tSap_WPSAssocRspIE  sapWPSAssocRspIE;    /*                         */
    } sapwpsie;
} tSap_WPSIE, *tpSap_WPSIE;

#ifdef WLANTL_DEBUG
#define MAX_RATE_INDEX      136
#define MAX_NUM_RSSI        100
#define MAX_RSSI_INTERVAL     5
#endif

typedef struct sap_SoftapStats_s {
   v_U32_t txUCFcnt;
   v_U32_t txMCFcnt;
   v_U32_t txBCFcnt;
   v_U32_t txUCBcnt;
   v_U32_t txMCBcnt;
   v_U32_t txBCBcnt;
   v_U32_t rxUCFcnt;
   v_U32_t rxMCFcnt;
   v_U32_t rxBCFcnt;
   v_U32_t rxUCBcnt;
   v_U32_t rxMCBcnt;
   v_U32_t rxBCBcnt;
   v_U32_t rxBcnt;
   v_U32_t rxBcntCRCok;
   v_U32_t rxRate;
#ifdef WLANTL_DEBUG
   v_U32_t pktCounterRateIdx[MAX_RATE_INDEX];
   v_U32_t pktCounterRssi[MAX_NUM_RSSI];
#endif
} tSap_SoftapStats, *tpSap_SoftapStats;

#ifdef FEATURE_WLAN_CH_AVOID
/*                                 */
typedef struct
{
   v_U16_t   channelNumber;
   v_BOOL_t  isSafe;
} safeChannelType;
#endif /*                       */

int sapSetPreferredChannel(tANI_U8* ptr);
void sapCleanupChannelList(void);

/*                                                                          
                               

              
                                                            

               
        

            

      
                                                   
                                               
   
              
                                                                  

                                
  
                 
                                                                            */
VOS_STATUS
WLANSAP_Set_WpsIe
(
    v_PVOID_t pvosGCtx, tSap_WPSIE *pWPSIe
);

/*                                                                          
                                 

              
                                                                   

              
        

            

      
                                                 
   
              
                                                                  

                                
  
                 
                                                                            */
VOS_STATUS
WLANSAP_Update_WpsIe
(
    v_PVOID_t pvosGCtx
);

/*                                                                          
                              

              
                                                                  

               
        

            

      
                                                 
   
              
                                                                  

                                
  
                 
                                                                            */
VOS_STATUS
WLANSAP_Stop_Wps
(
    v_PVOID_t pvosGCtx
);

/*                                                                          
                                   

              
                                                               

               
        

            

      
                                                 

       
                                                                              
 
              
                                                                  

                                
  
                 
                                                                            */
VOS_STATUS
WLANSAP_Get_WPS_State
(
    v_PVOID_t pvosGCtx, v_BOOL_t * pbWPSState
); 

/*                                                                            
                                       
                                                                            */

typedef v_PVOID_t tSapHandle, *ptSapHandle;

/*                                                                            
                                          
                                                                            */

/*                                                                          
                          

              
                                                                    
                                                                      
                                     
    
               
    
             

      
                                                                       
                                                                  
   
              
                                                              

                                                                               
                                
                                               

                 
                                                                            */
VOS_STATUS 
WLANSAP_Open
( 
    v_PVOID_t  pvosGCtx 
);

/*                                                                          
                           

              
                                                               
    
               
    
             

      
                                                                         
                                                                    
   
              
                                                              

                                                                               
                                
                                               

                                                             
    
                 
                                                                            */
VOS_STATUS 
WLANSAP_Start
( 
    v_PVOID_t  pvosGCtx 
);

/*                                                                          
                          

              
                                                               
    
               
    
             

      
                                                                         
                                                                    
   
              
                                                              

                                                                               
                                
                                               

                 
                                                                            */
VOS_STATUS 
WLANSAP_Stop
( 
    v_PVOID_t  pvosGCtx 
);

/*                                                                          
                           

              
                                                                                 
                                
    
               
    
             

      
                                                                         
                                                                    
   
              
                                                              

                                                                               
                                
                                               

                 
                                                                            */
VOS_STATUS 
WLANSAP_Close
( 
    v_PVOID_t  pvosGCtx 
);

/*                                                                          
                                  

              
                                                         
                                    
                 
                   

               
        

             

      
                                                                                                 
                                                     
                                                                        
              
                                                              

                                                     
                                
  
                 
                                                                            */
typedef VOS_STATUS (*tpWLAN_SAPEventCB)( tpSap_Event pSapEvent, v_PVOID_t  pUsrContext);



/*                                                                          
                              

              
                                                         

               

             

      
                                                          

              
                                
                                                                            */

v_U8_t WLANSAP_getState ( v_PVOID_t  pvosGCtx);

/*                                                                          
                              

              
                                                                                   
                

               
        

             

      
                                                 
                                                                                     
                                                                                            
                                                                                      

   
              
                                                                  

                                
  
                 
                                                                            */
VOS_STATUS 
WLANSAP_StartBss
(
    v_PVOID_t  pvosGCtx, 
    tpWLAN_SAPEventCB pSapEventCallback, 
    tsap_Config_t *pConfig, v_PVOID_t  pUsrContext
);

/*                                                                          
                               

             
                                                                             
              

              
       

            

      
                                                 
                                                             
                                  


              
                                                                

                                

              
                                                                            */
VOS_STATUS
WLANSAP_SetMacACL
(
    v_PVOID_t  pvosGCtx,
    tsap_Config_t *pConfig
);

/*                                                                          
                          

              
                                                                                       
             

               
        

              

      
                                                     
   
              
                                                                  

                                
  
                 
                                                                            */
VOS_STATUS 
WLANSAP_StopBss
(
    v_PVOID_t  pvosGCtx
);

/*                                                                          
                                 

              
                                                                                 

               
        

            

      
                                                                 
                                                                    
   
              
                                                                  

                                
  
                 
                                                                            */
VOS_STATUS 
WLANSAP_DisassocSta
(
    v_PVOID_t  pvosGCtx, v_U8_t *pPeerStaMac
);

/*                                                                          
                               

              
                                                                                   

               
        

            

      
                                                                 
                                                                      
   
              
                                                                  

                                
  
                 
                                                                            */
VOS_STATUS 
WLANSAP_DeauthSta
(
    v_PVOID_t  pvosGCtx, v_U8_t *pPeerStaMac
);

/*                                                                          
                                     

              
                                                              

               
        

            

      
                                        
                                      
                                                       
   
              
                                                                  

                                
  
                 
                                                                            */
VOS_STATUS
WLANSAP_SetChannelRange(tHalHandle hHal,v_U8_t startChannel, v_U8_t endChannel, 
                              v_U8_t operatingBand);

/*                                                                          
                               

              
                                                                          

               
        

            

      
                                                 
                                                     
   
              
                                                                  

                                
  
                 
                                                                            */
VOS_STATUS
WLANSAP_SetKeySta
(
    v_PVOID_t pvosGCtx, tCsrRoamSetKey *pSetKeyInfo
); 

/*                                                                          
                               

              
                                                                          

               
        

            

      
                                                 
                                                     
   
              
                                                                  

                                
  
                 
                                                                            */
VOS_STATUS
WLANSAP_DelKeySta
(
    v_PVOID_t pvosGCtx, tCsrRoamRemoveKey *pDelKeyInfo
); 



/*                                                                          
                                      

              
                                                                                                         

               
        

            

      
                                                 
                                                                                                            
                                               
          
                                                                                                       
                                                                                                           
                                                                           
   
              
                                                                  

                                
  
                 
                                                                            */
VOS_STATUS
WLANSAP_GetAssocStations
(
    v_PVOID_t pvosGCtx, VOS_MODULE_ID module, 
    tpSap_AssocMacAddr pAssocStas
); 
/*                                                                          
                                             

              
                                                                                                   

               
        

            

      
                                                     
                                                                                                        
   
              
                                                                  

                                
                                                                                       
  
                 
                                                                            */
VOS_STATUS
WLANSAP_RemoveWpsSessionOverlap

(
    v_PVOID_t pvosGCtx,
    v_MACADDR_t pRemoveMac
);

/*                                                                          
                                          

              
                                                                              

               
        

            

      
                                                 
                                                           
                                                   
   
              
                                                                  

                                
                                                                                   
  
                 
                                                                            */
VOS_STATUS
WLANSAP_getWpsSessionOverlap
(
    v_PVOID_t pvosGCtx
);

/*                                                                          
                                       

              
                                                                           
                                                                                     
                                                                               
                                                                                 
                                                                                           
                                                                                       
                                                                                            
                                                                                                

               
        

            

      
                                                 
                                                                                                                     
                            
   
              
                                                                  

                                
  
                 
                                                                            */
VOS_STATUS
WLANSAP_SetCounterMeasure
(
    v_PVOID_t pvosGCtx, v_BOOL_t bEnable
);

/*                                                                          
                                              

              
                                                                                        

               
        

            

      
                                                         
                                                                  
                                        
   
              
                                                                  

                                
  
                 
                                                                            */
VOS_STATUS
WLANSap_getstationIE_information(v_PVOID_t pvosGCtx, 
                                 v_U32_t   *pLen,
                                 v_U8_t    *pBuf);


VOS_STATUS
WLANSAP_getWpsSessionOverlap
(
    v_PVOID_t pvosGCtx
);
/*                                                                          
                              

              
                                                                            

               
        

            

      
                                                         
   
              
                                                                  

                                
  
                 
                                                                            */
VOS_STATUS 
WLANSAP_ClearACL
( 
    v_PVOID_t  pvosGCtx
);

/*                                                                          
                             

              
                                        

               
        

            

      
                                                         
   
              
                                                                  

                                
  
                 
                                                                            */
VOS_STATUS 
WLANSAP_SetMode
( 
    v_PVOID_t  pvosGCtx,
    v_U32_t mode
);

/*                                                                          
                               

              
                                                                                                        

               
        

            

      
                                                                 
                                                             
                                                                      
                                                                               
              
                                                                  

                                
  
                 
                                                                            */
VOS_STATUS 
WLANSAP_ModifyACL
(
    v_PVOID_t  pvosGCtx,
    v_U8_t *pPeerStaMac,
    eSapACLType listType,
    eSapACLCmdType cmd
);

/*                                                                          
                                   

              
                                                                                                        

               
        

            

      
                                                         
                                              
                                           
   
              
                                                                  

                                
  
                 
                                                                            */
VOS_STATUS WLANSAP_Set_WPARSNIes(v_PVOID_t pvosGCtx, v_U8_t *pWPARSNIEs, v_U32_t WPARSNIEsLen);

/*                                                                          
                                   

              
                                                                                               

               
        

            

      
                                                         
                                                                  
       
                                             

              
                                                                  

                                

                 
                                                                            */
VOS_STATUS WLANSAP_GetStatistics(v_PVOID_t pvosGCtx, tSap_SoftapStats *statBuf, v_BOOL_t bReset);

/*                                                                          

                                

              
                                                                        

               
        

            

    
                                                     
                                                       
                                   
   
              
                                                                  

                                
  
                 
                                                                            */
VOS_STATUS WLANSAP_SendAction( v_PVOID_t pvosGCtx, const tANI_U8 *pBuf, 
                               tANI_U32 len, tANI_U16 wait );

/*                                                                          

                                     

              
                                                                            
                           

               
        

            

    
                                                     
                                                   
                                                                          
                                                                 
                                                               
   
              
                                                                  

                                
  
                 
                                                                            */
VOS_STATUS WLANSAP_RemainOnChannel( v_PVOID_t pvosGCtx,
                                    tANI_U8 channel, tANI_U32 duration,
                                    remainOnChanCallback callback, 
                                    void *pContext );

/*                                                                          

                                           

              
                                                       

               
        

            

    
                                                     
   
              
                                                                  

                                
  
                 
                                                                            */
VOS_STATUS WLANSAP_CancelRemainOnChannel( v_PVOID_t pvosGCtx );


/*                                                                          

                                       

              
                                                                         
                                                                            

               
        

            

    
                                                     
                                                             
                                                                                
                                     
   
              
                                                                  

                                
  
                 
                                                                            */
VOS_STATUS WLANSAP_RegisterMgmtFrame( v_PVOID_t pvosGCtx, tANI_U16 frameType, 
                                      tANI_U8* matchData, tANI_U16 matchLen );

/*                                                                          

                                         

              
                                                               

               
        

            

    
                                                     
                                                                
                                                                                
                                     
   
              
                                                                  

                                
  
                 
                                                                            */
VOS_STATUS WLANSAP_DeRegisterMgmtFrame( v_PVOID_t pvosGCtx, tANI_U16 frameType, 
                                      tANI_U8* matchData, tANI_U16 matchLen );


#ifdef __cplusplus
 }
#endif 


#endif /*                            */


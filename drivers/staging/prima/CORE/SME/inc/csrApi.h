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
    \file csrApi.h

    Exports and types for the Common Scan and Roaming Module interfaces.

    Copyright (C) 2006 Airgo Networks, Incorporated
   ========================================================================== */
#ifndef CSRAPI_H__
#define CSRAPI_H__

#include "sirApi.h"
#include "sirMacProtDef.h"
#include "csrLinkList.h"

typedef enum
{
    eCSR_AUTH_TYPE_NONE,    //          
    //                               
    eCSR_AUTH_TYPE_OPEN_SYSTEM,
    eCSR_AUTH_TYPE_SHARED_KEY,
    eCSR_AUTH_TYPE_AUTOSWITCH,

    //                                 
    eCSR_AUTH_TYPE_WPA,
    eCSR_AUTH_TYPE_WPA_PSK,
    eCSR_AUTH_TYPE_WPA_NONE,

    eCSR_AUTH_TYPE_RSN,
    eCSR_AUTH_TYPE_RSN_PSK,
#if defined WLAN_FEATURE_VOWIFI_11R
    eCSR_AUTH_TYPE_FT_RSN,
    eCSR_AUTH_TYPE_FT_RSN_PSK,
#endif
#ifdef FEATURE_WLAN_WAPI
    eCSR_AUTH_TYPE_WAPI_WAI_CERTIFICATE,
    eCSR_AUTH_TYPE_WAPI_WAI_PSK,
#endif /*                   */
#ifdef FEATURE_WLAN_CCX
    eCSR_AUTH_TYPE_CCKM_WPA,
    eCSR_AUTH_TYPE_CCKM_RSN,
#endif /*                  */
#ifdef WLAN_FEATURE_11W
    eCSR_AUTH_TYPE_RSN_PSK_SHA256,
#endif
    eCSR_NUM_OF_SUPPORT_AUTH_TYPE,
    eCSR_AUTH_TYPE_FAILED = 0xff,
    eCSR_AUTH_TYPE_UNKNOWN = eCSR_AUTH_TYPE_FAILED,

}eCsrAuthType;


typedef enum
{
    eCSR_ENCRYPT_TYPE_NONE,
    eCSR_ENCRYPT_TYPE_WEP40_STATICKEY,
    eCSR_ENCRYPT_TYPE_WEP104_STATICKEY,

    eCSR_ENCRYPT_TYPE_WEP40,
    eCSR_ENCRYPT_TYPE_WEP104,
    eCSR_ENCRYPT_TYPE_TKIP,
    eCSR_ENCRYPT_TYPE_AES,
#ifdef FEATURE_WLAN_WAPI
    eCSR_ENCRYPT_TYPE_WPI, //    
#endif /*                   */
#ifdef FEATURE_WLAN_CCX
    eCSR_ENCRYPT_TYPE_KRK,
#endif /*                  */
#ifdef WLAN_FEATURE_11W
    //       
    eCSR_ENCRYPT_TYPE_AES_CMAC,
#endif
    eCSR_ENCRYPT_TYPE_ANY,
    eCSR_NUM_OF_ENCRYPT_TYPE = eCSR_ENCRYPT_TYPE_ANY,

    eCSR_ENCRYPT_TYPE_FAILED = 0xff,
    eCSR_ENCRYPT_TYPE_UNKNOWN = eCSR_ENCRYPT_TYPE_FAILED,

}eCsrEncryptionType;

/*                                                                           
                                            
                                                                           */
typedef enum
{
    eCSR_SECURITY_TYPE_WPA,
    eCSR_SECURITY_TYPE_RSN,
#ifdef FEATURE_WLAN_WAPI
    eCSR_SECURITY_TYPE_WAPI,
#endif /*                   */
    eCSR_SECURITY_TYPE_UNKNOWN,

}eCsrSecurityType;

typedef enum
{
    eCSR_DOT11_MODE_TAURUS = 0, //                                                           
    eCSR_DOT11_MODE_abg = 0x0001,    //                                   
    eCSR_DOT11_MODE_11a = 0x0002,
    eCSR_DOT11_MODE_11b = 0x0004,
    eCSR_DOT11_MODE_11g = 0x0008,
    eCSR_DOT11_MODE_11n = 0x0010,
    eCSR_DOT11_MODE_POLARIS = 0x0020,
    eCSR_DOT11_MODE_TITAN = 0x0040,
    eCSR_DOT11_MODE_11g_ONLY = 0x0080,
    eCSR_DOT11_MODE_11n_ONLY = 0x0100,
    eCSR_DOT11_MODE_TAURUS_ONLY = 0x0200,
    eCSR_DOT11_MODE_11b_ONLY = 0x0400,
    eCSR_DOT11_MODE_11a_ONLY = 0x0800,
#ifdef WLAN_FEATURE_11AC
    eCSR_DOT11_MODE_11ac     = 0x1000,
    eCSR_DOT11_MODE_11ac_ONLY = 0x2000,
#endif
    //                                                                                                         
    //                          
    eCSR_DOT11_MODE_AUTO = 0x4000,

    eCSR_NUM_PHY_MODE = 16,     //                                              
}eCsrPhyMode;


typedef tANI_U8 tCsrBssid[WNI_CFG_BSSID_LEN];

typedef enum
{
    eCSR_BSS_TYPE_NONE,
    eCSR_BSS_TYPE_INFRASTRUCTURE,
    eCSR_BSS_TYPE_INFRA_AP,       //          
    eCSR_BSS_TYPE_IBSS,           //                                  
    eCSR_BSS_TYPE_START_IBSS,     //                                                       
    eCSR_BSS_TYPE_WDS_AP,         //          
    eCSR_BSS_TYPE_WDS_STA,        //               
    eCSR_BSS_TYPE_ANY,            //                                       
}eCsrRoamBssType;



typedef enum {
    eCSR_SCAN_REQUEST_11D_SCAN = 1,
    eCSR_SCAN_REQUEST_FULL_SCAN,
    eCSR_SCAN_IDLE_MODE_SCAN,
    eCSR_SCAN_HO_BG_SCAN, //                                               
    eCSR_SCAN_HO_PROBE_SCAN, //                                                   
    eCSR_SCAN_HO_NT_BG_SCAN, //                                 
    eCSR_SCAN_P2P_DISCOVERY,

    eCSR_SCAN_SOFTAP_CHANNEL_RANGE,
    eCSR_SCAN_P2P_FIND_PEER,
}eCsrRequestType;

typedef enum {
    eCSR_SCAN_RESULT_GET = 0,
    eCSR_SCAN_RESULT_FLUSH = 1,     //                                 
}eCsrScanResultCmd;

typedef enum
{
    eCSR_SCAN_SUCCESS,
    eCSR_SCAN_FAILURE,
    eCSR_SCAN_ABORT,
   eCSR_SCAN_FOUND_PEER,
}eCsrScanStatus;

#define CSR_SCAN_TIME_DEFAULT       0
#define CSR_VALUE_IGNORED           0xFFFFFFFF
#define CSR_RSN_PMKID_SIZE          16
#define CSR_MAX_PMKID_ALLOWED       16
#define CSR_WEP40_KEY_LEN       5
#define CSR_WEP104_KEY_LEN      13
#define CSR_TKIP_KEY_LEN        32
#define CSR_AES_KEY_LEN         16
#define CSR_MAX_TX_POWER        ( WNI_CFG_CURRENT_TX_POWER_LEVEL_STAMAX )
#define CSR_MAX_RSC_LEN          16
#ifdef FEATURE_WLAN_WAPI
#define CSR_WAPI_BKID_SIZE          16
#define CSR_MAX_BKID_ALLOWED        16
#define CSR_WAPI_KEY_LEN        32
#define CSR_MAX_KEY_LEN         ( CSR_WAPI_KEY_LEN )  //                       
#else
#define CSR_MAX_KEY_LEN         ( CSR_TKIP_KEY_LEN )  //                       
#endif /*                   */
#ifdef FEATURE_WLAN_CCX
#define CSR_KRK_KEY_LEN 16
#endif



typedef struct tagCsrChannelInfo
{
    tANI_U8 numOfChannels;
    tANI_U8 *ChannelList;   //                               
}tCsrChannelInfo, *tpCsrChannelInfo;

typedef struct tagCsrSSIDInfo
{
   tSirMacSSid     SSID;
   tANI_BOOLEAN    handoffPermitted;
   tANI_BOOLEAN    ssidHidden;
}tCsrSSIDInfo;

typedef struct tagCsrSSIDs
{
    tANI_U32 numOfSSIDs;
    tCsrSSIDInfo *SSIDList;   //                                  
}tCsrSSIDs;

typedef struct tagCsrBSSIDs
{
    tANI_U32 numOfBSSIDs;
    tCsrBssid *bssid;
}tCsrBSSIDs;

typedef struct tagCsrStaParams
{
    tANI_U16   capability;
    tANI_U8    extn_capability[SIR_MAC_MAX_EXTN_CAP];
    tANI_U8    supported_rates_len;
    tANI_U8    supported_rates[SIR_MAC_MAX_SUPP_RATES];
    tANI_U8    htcap_present;
    tSirHTCap  HTCap;
    tANI_U8    vhtcap_present;
    tSirVHTCap VHTCap;
    tANI_U8    uapsd_queues;
    tANI_U8    max_sp;
}tCsrStaParams;

typedef struct tagCsrScanRequest
{
    tSirScanType scanType;
    tCsrBssid bssid;
    eCsrRoamBssType BSSType;
    tCsrSSIDs SSIDs;
    tCsrChannelInfo ChannelInfo;
    tANI_U32 minChnTime;    //                        
    tANI_U32 maxChnTime;    //                        
    tANI_U32 minChnTimeBtc;    //                        
    tANI_U32 maxChnTimeBtc;    //                        
    tANI_U32 restTime;      //                                                      
    tANI_U32 uIEFieldLen;
    tANI_U8 *pIEField;
    eCsrRequestType requestType;    //                     
    tANI_BOOLEAN p2pSearch;
    tANI_BOOLEAN skipDfsChnlInP2pSearch;
    tANI_BOOLEAN bcnRptReqScan;     //                                       
}tCsrScanRequest;

typedef struct tagCsrBGScanRequest
{
    tSirScanType scanType;
    tSirMacSSid SSID;
    tCsrChannelInfo ChannelInfo;
    tANI_U32 scanInterval;  //                        
    tANI_U32 minChnTime;    //                        
    tANI_U32 maxChnTime;    //                        
    tANI_U32 minChnTimeBtc;    //                        
    tANI_U32 maxChnTimeBtc;    //                        
    tANI_U32 restTime;      //                                                      
    tANI_U32 throughputImpact;      //                                                                                      
    tCsrBssid bssid;    //                     
}tCsrBGScanRequest, *tpCsrBGScanRequest;


typedef struct tagCsrScanResultInfo
{
    //                                                                                                      
    void *pvIes;
    tAniSSID ssId;
    v_TIME_t timer; //                                                              
    //                                                                                         
    //                                         
    tSirBssDescription BssDescriptor;
}tCsrScanResultInfo;

typedef struct tagCsrEncryptionList
{

    tANI_U32 numEntries;
    eCsrEncryptionType encryptionType[eCSR_NUM_OF_ENCRYPT_TYPE];

}tCsrEncryptionList, *tpCsrEncryptionList;

typedef struct tagCsrAuthList
{
    tANI_U32 numEntries;
    eCsrAuthType authType[eCSR_NUM_OF_SUPPORT_AUTH_TYPE];
}tCsrAuthList, *tpCsrAuthList;

#ifdef WLAN_FEATURE_VOWIFI_11R
typedef struct tagCsrMobilityDomainInfo
{
    tANI_U8 mdiePresent;
    tANI_U16 mobilityDomain;
} tCsrMobilityDomainInfo;
#endif

#ifdef FEATURE_WLAN_CCX
typedef struct tagCsrCcxCckmInfo
{
    tANI_U32       reassoc_req_num;
    tANI_BOOLEAN   krk_plumbed;
    tANI_U8        krk[CSR_KRK_KEY_LEN];
} tCsrCcxCckmInfo;
#endif

#if defined(FEATURE_WLAN_CCX) && defined(FEATURE_WLAN_CCX_UPLOAD)
#define CSR_DOT11F_IE_RSN_MAX_LEN   (114)  /*                                */

typedef struct tagCsrCcxCckmIe
{
    tANI_U8 cckmIe[CSR_DOT11F_IE_RSN_MAX_LEN];
    tANI_U8 cckmIeLen;
} tCsrCcxCckmIe;
#endif /*                                             */

typedef struct tagCsrScanResultFilter
{
    tCsrBSSIDs BSSIDs;    //                                                
    tCsrSSIDs SSIDs;
    tCsrChannelInfo ChannelInfo;
    tCsrAuthList authType;
    tCsrEncryptionList EncryptionType;
    //                                                                                         
    //                                          
    tCsrEncryptionList mcEncryptionType;
    eCsrRoamBssType BSSType;
    //                                                                    
    tANI_U32 phyMode;
    //                                                                                          
    tANI_U8 countryCode[WNI_CFG_COUNTRY_CODE_LEN];
    tANI_U8 uapsd_mask;
    /*                                                                 */
    tANI_BOOLEAN bWPSAssociation;
#if defined WLAN_FEATURE_VOWIFI
    /*                                                                                             */
    tANI_BOOLEAN fMeasurement;
#endif
#ifdef WLAN_FEATURE_VOWIFI_11R
    tCsrMobilityDomainInfo MDID;
#endif
    tANI_BOOLEAN p2pResult;
}tCsrScanResultFilter;


typedef struct sCsrChnPower_
{
  tANI_U8 firstChannel;
  tANI_U8 numChannels;
  tANI_U8 maxtxPower;
}sCsrChnPower;


typedef struct sCsrChannel_
{
    tANI_U8 numChannels;
    tANI_U8 channelList[WNI_CFG_VALID_CHANNEL_LIST_LEN];
}sCsrChannel;


typedef struct tagCsr11dinfo
{
  sCsrChannel     Channels;
  tANI_U8         countryCode[WNI_CFG_COUNTRY_CODE_LEN+1];
  //                      
  sCsrChnPower    ChnPower[WNI_CFG_VALID_CHANNEL_LIST_LEN];
}tCsr11dinfo;


typedef enum
{
    eCSR_ROAM_CANCELLED = 1,
    //                                                                            
    eCSR_ROAM_FAILED,
    //                                                                                          
    eCSR_ROAM_ROAMING_START,
    //                                            
    eCSR_ROAM_ROAMING_COMPLETION,
    //                            
    eCSR_ROAM_CONNECT_COMPLETION,
    //                                              
    //                                                                                 
    eCSR_ROAM_ASSOCIATION_START,
    //                                                     
    //                         
    eCSR_ROAM_ASSOCIATION_COMPLETION,
    eCSR_ROAM_DISASSOCIATED,
    eCSR_ROAM_ASSOCIATION_FAILURE,
    //                                                                      
    eCSR_ROAM_SHOULD_ROAM,
    //                                  
    eCSR_ROAM_SCAN_FOUND_NEW_BSS,
    //                                                       
    eCSR_ROAM_LOSTLINK,
    //                                            
    eCSR_ROAM_LOSTLINK_DETECTED,
    //                                                                         
    eCSR_ROAM_MIC_ERROR_IND,
    eCSR_ROAM_IBSS_IND, //                 
    //                                                                                     
    eCSR_ROAM_CONNECT_STATUS_UPDATE,
    eCSR_ROAM_GEN_INFO,
    eCSR_ROAM_SET_KEY_COMPLETE,
    eCSR_ROAM_REMOVE_KEY_COMPLETE,
    eCSR_ROAM_IBSS_LEAVE, //                 
    //                                 
    eCSR_ROAM_WDS_IND,
    //                                    
    eCSR_ROAM_INFRA_IND,
    eCSR_ROAM_WPS_PBC_PROBE_REQ_IND,
#ifdef WLAN_FEATURE_VOWIFI_11R
    eCSR_ROAM_FT_RESPONSE,
#endif
    eCSR_ROAM_FT_START,
    eCSR_ROAM_INDICATE_MGMT_FRAME,
    eCSR_ROAM_REMAIN_CHAN_READY,
    eCSR_ROAM_SEND_ACTION_CNF,
    //                                                                            
    eCSR_ROAM_SESSION_OPENED,
    eCSR_ROAM_FT_REASSOC_FAILED,
#ifdef FEATURE_WLAN_LFR
    eCSR_ROAM_PMK_NOTIFY,
#endif
#ifdef FEATURE_WLAN_TDLS
    eCSR_ROAM_TDLS_STATUS_UPDATE,
    eCSR_ROAM_RESULT_MGMT_TX_COMPLETE_IND,
#endif
    eCSR_ROAM_DISCONNECT_ALL_P2P_CLIENTS, //                           
    eCSR_ROAM_SEND_P2P_STOP_BSS, //                                           
                                 //                
#ifdef WLAN_FEATURE_11W
    eCSR_ROAM_UNPROT_MGMT_FRAME_IND,
#endif

#if defined(FEATURE_WLAN_CCX) && defined(FEATURE_WLAN_CCX_UPLOAD)
    eCSR_ROAM_TSM_IE_IND,
    eCSR_ROAM_CCKM_PREAUTH_NOTIFY,
    eCSR_ROAM_CCX_ADJ_AP_REPORT_IND,
    eCSR_ROAM_CCX_BCN_REPORT_IND,
#endif /*                                             */
}eRoamCmdStatus;


//                                                   
typedef enum
{
    eCSR_ROAM_RESULT_NONE,
    //                                
    //                                                                                        
    eCSR_ROAM_RESULT_FAILURE,
    //                                 
    eCSR_ROAM_RESULT_ASSOCIATED,
    eCSR_ROAM_RESULT_NOT_ASSOCIATED,
    eCSR_ROAM_RESULT_MIC_FAILURE,
    eCSR_ROAM_RESULT_FORCED,
    eCSR_ROAM_RESULT_DISASSOC_IND,
    eCSR_ROAM_RESULT_DEAUTH_IND,
    eCSR_ROAM_RESULT_CAP_CHANGED,
    //                            
    //                                     
    eCSR_ROAM_RESULT_IBSS_STARTED,
    //                
    //                                     
    eCSR_ROAM_RESULT_IBSS_START_FAILED,
    eCSR_ROAM_RESULT_IBSS_JOIN_SUCCESS,
    eCSR_ROAM_RESULT_IBSS_JOIN_FAILED,
    eCSR_ROAM_RESULT_IBSS_CONNECT,
    eCSR_ROAM_RESULT_IBSS_INACTIVE,
    //                                                 
    //                                                                                   
    //                                    
    //                                                                                   
    eCSR_ROAM_RESULT_IBSS_NEW_PEER,
    //                                                                                   
    eCSR_ROAM_RESULT_IBSS_PEER_DEPARTED,
    //                                                       
    //                                       
    eCSR_ROAM_RESULT_IBSS_COALESCED,
    //                                                                                                             
    eCSR_ROAM_RESULT_IBSS_STOP,
    eCSR_ROAM_RESULT_LOSTLINK,
    eCSR_ROAM_RESULT_MIC_ERROR_UNICAST,
    eCSR_ROAM_RESULT_MIC_ERROR_GROUP,
    eCSR_ROAM_RESULT_AUTHENTICATED,
    eCSR_ROAM_RESULT_NEW_RSN_BSS,
#ifdef FEATURE_WLAN_WAPI
    eCSR_ROAM_RESULT_NEW_WAPI_BSS,
#endif /*                   */
    //                         
    eCSR_ROAM_RESULT_WDS_STARTED,
    //                 
    eCSR_ROAM_RESULT_WDS_START_FAILED,
    //            
    eCSR_ROAM_RESULT_WDS_STOPPED,
    //                                    
    eCSR_ROAM_RESULT_WDS_ASSOCIATED,
    //                        
    eCSR_ROAM_RESULT_WDS_ASSOCIATION_IND,
    //                            
    eCSR_ROAM_RESULT_WDS_NOT_ASSOCIATED,
    //                  
    eCSR_ROAM_RESULT_WDS_DISASSOCIATED,
    //                           
    eCSR_ROAM_RESULT_INFRA_STARTED,
    //                   
    eCSR_ROAM_RESULT_INFRA_START_FAILED,
    //              
    eCSR_ROAM_RESULT_INFRA_STOPPED,
    //                           
    eCSR_ROAM_RESULT_INFRA_ASSOCIATION_IND,
    //                          
    eCSR_ROAM_RESULT_INFRA_ASSOCIATION_CNF,
    //                    
    eCSR_ROAM_RESULT_INFRA_DISASSOCIATED,
    eCSR_ROAM_RESULT_WPS_PBC_PROBE_REQ_IND,
    eCSR_ROAM_RESULT_SEND_ACTION_FAIL,
    //                                                                                   
    eCSR_ROAM_RESULT_MAX_ASSOC_EXCEEDED,
    //                                                                       
    eCSR_ROAM_RESULT_ASSOC_FAIL_CON_CHANNEL,
#ifdef FEATURE_WLAN_TDLS
    eCSR_ROAM_RESULT_ADD_TDLS_PEER,
    eCSR_ROAM_RESULT_UPDATE_TDLS_PEER,
    eCSR_ROAM_RESULT_DELETE_TDLS_PEER,
    eCSR_ROAM_RESULT_TEARDOWN_TDLS_PEER_IND,
    eCSR_ROAM_RESULT_DELETE_ALL_TDLS_PEER_IND,
    eCSR_ROAM_RESULT_LINK_ESTABLISH_REQ_RSP,
#ifdef FEATURE_WLAN_TDLS_OXYGEN_DISAPPEAR_AP
    eCSR_ROAM_RESULT_TDLS_DISAPPEAR_AP_IND,
#endif
#endif

}eCsrRoamResult;



/*                                                                            
                                                           
                                                                             */
typedef enum
{
 eCSR_ROAM_LINK_QUAL_MIN_IND     = -1,

 eCSR_ROAM_LINK_QUAL_POOR_IND            =  0,   /*                         */
 eCSR_ROAM_LINK_QUAL_GOOD_IND            =  1,   /*                         */
 eCSR_ROAM_LINK_QUAL_VERY_GOOD_IND       =  2,   /*                         */
 eCSR_ROAM_LINK_QUAL_EXCELLENT_IND       =  3,   /*                         */

 eCSR_ROAM_LINK_QUAL_MAX_IND  /*               */

} eCsrRoamLinkQualityInd;

typedef enum
{
    eCSR_DISCONNECT_REASON_UNSPECIFIED = 0,
    eCSR_DISCONNECT_REASON_MIC_ERROR,
    eCSR_DISCONNECT_REASON_DISASSOC,
    eCSR_DISCONNECT_REASON_DEAUTH,
    eCSR_DISCONNECT_REASON_HANDOFF,
    eCSR_DISCONNECT_REASON_IBSS_JOIN_FAILURE,
    eCSR_DISCONNECT_REASON_IBSS_LEAVE,
}eCsrRoamDisconnectReason;

typedef enum
{
    //                                                                      
    eCSR_ASSOC_STATE_TYPE_NOT_CONNECTED,
    //                                         
    eCSR_ASSOC_STATE_TYPE_INFRA_ASSOCIATED,
    //                                                                          
    //              
    eCSR_ASSOC_STATE_TYPE_IBSS_DISCONNECTED,
    //                                                                    
    eCSR_ASSOC_STATE_TYPE_IBSS_CONNECTED,
    //                                                                       
    eCSR_ASSOC_STATE_TYPE_WDS_DISCONNECTED,
    //                                                          
    eCSR_ASSOC_STATE_TYPE_WDS_CONNECTED,
    //                                                                  
    eCSR_ASSOC_STATE_TYPE_INFRA_DISCONNECTED,
    //                                                         
    eCSR_ASSOC_STATE_TYPE_INFRA_CONNECTED,

}eCsrConnectState;


//                                                                                                 
//                 
typedef enum eCSR_MEDIUM_ACCESS
{
    eCSR_MEDIUM_ACCESS_AUTO = 0,
    eCSR_MEDIUM_ACCESS_DCF,
    eCSR_MEDIUM_ACCESS_eDCF,
    eCSR_MEDIUM_ACCESS_HCF,

    eCSR_MEDIUM_ACCESS_WMM_eDCF_802dot1p,
    eCSR_MEDIUM_ACCESS_WMM_eDCF_DSCP,
    eCSR_MEDIUM_ACCESS_WMM_eDCF_NoClassify,
    eCSR_MEDIUM_ACCESS_11e_eDCF = eCSR_MEDIUM_ACCESS_eDCF,
    eCSR_MEDIUM_ACCESS_11e_HCF  = eCSR_MEDIUM_ACCESS_HCF,
}eCsrMediaAccessType;

typedef enum
{
    eCSR_TX_RATE_AUTO = 0,   //                                        

    eCSR_TX_RATE_1Mbps   = 0x00000001,
    eCSR_TX_RATE_2Mbps   = 0x00000002,
    eCSR_TX_RATE_5_5Mbps = 0x00000004,
    eCSR_TX_RATE_6Mbps   = 0x00000008,
    eCSR_TX_RATE_9Mbps   = 0x00000010,
    eCSR_TX_RATE_11Mbps  = 0x00000020,
    eCSR_TX_RATE_12Mbps  = 0x00000040,
    eCSR_TX_RATE_18Mbps  = 0x00000080,
    eCSR_TX_RATE_24Mbps  = 0x00000100,
    eCSR_TX_RATE_36Mbps  = 0x00000200,
    eCSR_TX_RATE_42Mbps  = 0x00000400,
    eCSR_TX_RATE_48Mbps  = 0x00000800,
    eCSR_TX_RATE_54Mbps  = 0x00001000,
    eCSR_TX_RATE_72Mbps  = 0x00002000,
    eCSR_TX_RATE_84Mbps  = 0x00004000,
    eCSR_TX_RATE_96Mbps  = 0x00008000,
    eCSR_TX_RATE_108Mbps = 0x00010000,
    eCSR_TX_RATE_126Mbps = 0x00020000,
    eCSR_TX_RATE_144Mbps = 0x00040000,
    eCSR_TX_RATE_168Mbps = 0x00080000,
    eCSR_TX_RATE_192Mbps = 0x00100000,
    eCSR_TX_RATE_216Mbps = 0x00200000,
    eCSR_TX_RATE_240Mbps = 0x00400000,

}eCsrExposedTxRate;

typedef enum
{
    eCSR_OPERATING_CHANNEL_ALL  = 0,
    eCSR_OPERATING_CHANNEL_AUTO = eCSR_OPERATING_CHANNEL_ALL,
    eCSR_OPERATING_CHANNEL_ANY  = eCSR_OPERATING_CHANNEL_ALL,
}eOperationChannel;

typedef enum
{
    eCSR_DOT11_FRAG_THRESH_AUTO            = -1,
    eCSR_DOT11_FRAG_THRESH_MIN             = 256,
    eCSR_DOT11_FRAG_THRESH_MAX             = 2346,
    eCSR_DOT11_FRAG_THRESH_DEFAULT         = 2000
}eCsrDot11FragThresh;


//                            
typedef enum
{
    eCSR_CB_OFF = 0,
    eCSR_CB_AUTO = 1,
    eCSR_CB_DOWN = 2,
    eCSR_CB_UP = 3,
}eCsrCBChoice;

//                                                                                               
#define CSR_CB_CHANNEL_GAP 4
#define CSR_CB_CENTER_CHANNEL_OFFSET    2
#define CSR_MAX_24GHz_CHANNEL_NUMBER ( SIR_11B_CHANNEL_END )
#define CSR_MIN_5GHz_CHANNEL_NUMBER  ( SIR_11A_CHANNEL_BEGIN )
#define CSR_MAX_5GHz_CHANNEL_NUMBER  ( SIR_11A_CHANNEL_END )

//                         
typedef enum
{
    eCSR_SECURITY_WEP_KEYSIZE_40  =  40,   //                                  
    eCSR_SECURITY_WEP_KEYSIZE_104 = 104,   //                                   

    eCSR_SECURITY_WEP_KEYSIZE_MIN = eCSR_SECURITY_WEP_KEYSIZE_40,
    eCSR_SECURITY_WEP_KEYSIZE_MAX = eCSR_SECURITY_WEP_KEYSIZE_104,
    eCSR_SECURITY_WEP_KEYSIZE_MAX_BYTES = ( eCSR_SECURITY_WEP_KEYSIZE_MAX / 8 ),
}eCsrWEPKeySize;


//                                             
typedef enum
{

    eCSR_SECURITY_WEP_STATIC_KEY_ID_MIN       =  0,
    eCSR_SECURITY_WEP_STATIC_KEY_ID_MAX       =  3,
    eCSR_SECURITY_WEP_STATIC_KEY_ID_DEFAULT   =  0,

    eCSR_SECURITY_WEP_STATIC_KEY_ID_INVALID   = -1,

}eCsrWEPStaticKeyID;

//                                                                    
#define CSR_MAX_NUM_KEY     (eCSR_SECURITY_WEP_STATIC_KEY_ID_MAX + 2 + 1)

typedef enum
{
    eCSR_SECURITY_SET_KEY_ACTION_NO_CHANGE,
    eCSR_SECURITY_SET_KEY_ACTION_SET_KEY,
    eCSR_SECURITY_SET_KEY_ACTION_DELETE_KEY,
}eCsrSetKeyAction;

typedef enum
{
    eCSR_BAND_ALL,
    eCSR_BAND_24,
    eCSR_BAND_5G,
    eCSR_BAND_MAX,
}eCsrBand;


typedef enum
{
   //                                                                           
   //                                
   //                                                                           
   //                             
   eCsrRoamReasonStaCapabilityChanged,
   //                                                                        
   //                  
   eCsrRoamReasonBetterAP,
   //                                                                         
   //                                      
   eCsrRoamReasonSmeIssuedForLostLink,

}eCsrRoamReasonCodes;

typedef enum
{
   eCsrRoamWmmAuto = 0,
   eCsrRoamWmmQbssOnly = 1,
   eCsrRoamWmmNoQos = 2,

} eCsrRoamWmmUserModeType;

typedef enum
{
   eCSR_REQUESTER_MIN = 0,
   eCSR_DIAG,
   eCSR_UMA_GAN,
   eCSR_HDD
} eCsrStatsRequesterType;

typedef struct tagPmkidCandidateInfo
{
    tCsrBssid BSSID;
    tANI_BOOLEAN preAuthSupported;
}tPmkidCandidateInfo;

typedef struct tagPmkidCacheInfo
{
    tCsrBssid BSSID;
    tANI_U8 PMKID[CSR_RSN_PMKID_SIZE];
}tPmkidCacheInfo;

#ifdef FEATURE_WLAN_WAPI
typedef struct tagBkidCandidateInfo
{
    tCsrBssid BSSID;
    tANI_BOOLEAN preAuthSupported;
}tBkidCandidateInfo;

typedef struct tagBkidCacheInfo
{
    tCsrBssid BSSID;
    tANI_U8 BKID[CSR_WAPI_BKID_SIZE];
}tBkidCacheInfo;
#endif /*                   */

typedef struct tagCsrKeys
{
    tANI_U8 KeyLength[ CSR_MAX_NUM_KEY ];   //                                                 
    tANI_U8 KeyMaterial[ CSR_MAX_NUM_KEY ][ CSR_MAX_KEY_LEN ];
    tANI_U8 defaultIndex;
}tCsrKeys;

/*                                                                           
                                                                           
           */
typedef struct tagCsrRoamModifyProfileFields
{
   //                                                        
   //                                                                       
   //                                                                        
   //                                                                       
   //                  
   tANI_U8     uapsd_mask;
   //                                   
   tANI_U16    listen_interval;
}tCsrRoamModifyProfileFields;

typedef struct tagCsrRoamProfile
{
    //                                                          
    //                                                                                     
    //                                                       
    tCsrSSIDs SSIDs;
    tCsrBSSIDs BSSIDs;
    tANI_U32 phyMode;   //                                                                    
    eCsrRoamBssType BSSType;

    tCsrAuthList AuthType;
    eCsrAuthType negotiatedAuthType;

    tCsrEncryptionList EncryptionType;
    //                                            
    eCsrEncryptionType negotiatedUCEncryptionType;

    //                                                                                         
    //                                          
    tCsrEncryptionList mcEncryptionType;
    //                                            
    eCsrEncryptionType negotiatedMCEncryptionType;

#ifdef WLAN_FEATURE_11W
    //                            
    tANI_BOOLEAN MFPEnabled;
    tANI_U8 MFPRequired;
    tANI_U8 MFPCapable;
#endif

    tCsrKeys Keys;
    eCsrCBChoice CBMode; //                
    tCsrChannelInfo ChannelInfo;
    tANI_U8 operationChannel;
    tANI_U16 beaconInterval;    //                                          
    //                                                        
    //                                                                       
    //                                                                                      
    tANI_U8 uapsd_mask;
    tANI_U32 nWPAReqIELength;   //                               
    tANI_U8 *pWPAReqIE;   //                                              
    tANI_U32 nRSNReqIELength;  //                               
    tANI_U8 *pRSNReqIE;     //                                              
#ifdef FEATURE_WLAN_WAPI
    tANI_U32 nWAPIReqIELength;   //                                
    tANI_U8 *pWAPIReqIE;   //                                               
#endif /*                   */

    tANI_U32 nAddIEScanLength;   //                                                           
    tANI_U8 *pAddIEScan;       //                                                                                           
    tANI_U32 nAddIEAssocLength;   //                                      
    tANI_U8 *pAddIEAssoc;       //                                                                                           

    tANI_U8 countryCode[WNI_CFG_COUNTRY_CODE_LEN];  //                          
    /*                                                               */
    tANI_BOOLEAN bWPSAssociation;
    tANI_U32 nWSCReqIELength;   //                               
    tANI_U8 *pWSCReqIE;   //                                              

    tANI_U8 ieee80211d;
    tANI_U8 privacy;
    tANI_BOOLEAN fwdWPSPBCProbeReq;
    tAniAuthType csr80211AuthType;
    tANI_U32 dtimPeriod;
    tANI_BOOLEAN ApUapsdEnable;
    tANI_BOOLEAN protEnabled;
    tANI_BOOLEAN obssProtEnabled;
    tANI_U16 cfg_protection;
    tANI_U8 wps_state;

#ifdef WLAN_FEATURE_VOWIFI_11R
    tCsrMobilityDomainInfo MDID;
#endif
    tVOS_CON_MODE csrPersona;

}tCsrRoamProfile;


typedef struct tagCsrRoamConnectedProfile
{
    tSirMacSSid SSID;
    tANI_BOOLEAN    handoffPermitted;
    tANI_BOOLEAN    ssidHidden;
    tCsrBssid bssid;
    eCsrRoamBssType BSSType;
    eCsrAuthType AuthType;
    tCsrAuthList AuthInfo;
    eCsrEncryptionType EncryptionType;
    tCsrEncryptionList EncryptionInfo;
    eCsrEncryptionType mcEncryptionType;
    tCsrEncryptionList mcEncryptionInfo;
    eCsrCBChoice CBMode; //                
    tANI_U8 operationChannel;
    tANI_U16   beaconInterval;
    tCsrKeys Keys;
    //                                                                   
    //                                                         
    //                         
    //                                                                
    tANI_U8  acm_mask;
    tCsrRoamModifyProfileFields modifyProfileFields;
    tANI_U32 nAddIEAssocLength;   //                                      
    tANI_U8 *pAddIEAssoc;       //                                                                                           

    tSirBssDescription *pBssDesc;
    tANI_BOOLEAN   qap; //               
    tANI_BOOLEAN   qosConnection; //                           
#ifdef WLAN_FEATURE_VOWIFI_11R
    tCsrMobilityDomainInfo MDID;
#endif

#ifdef FEATURE_WLAN_CCX
    tCsrCcxCckmInfo ccxCckmInfo;
    tANI_BOOLEAN    isCCXAssoc;
#endif
    tANI_U32 dot11Mode;
}tCsrRoamConnectedProfile;


#ifdef WLAN_FEATURE_VOWIFI_11R
typedef struct tagCsr11rConfigParams
{
    tANI_BOOLEAN   IsFTResourceReqSupported;
} tCsr11rConfigParams;
#endif

#ifdef WLAN_FEATURE_NEIGHBOR_ROAMING
typedef struct tagCsrNeighborRoamConfigParams
{

    tANI_U32       nNeighborScanTimerPeriod;
    tANI_U8        nNeighborLookupRssiThreshold;
    tANI_U8        nNeighborReassocRssiThreshold;
    tANI_U16       nNeighborScanMinChanTime;
    tANI_U16       nNeighborScanMaxChanTime;
    sCsrChannel    neighborScanChanList;
    tANI_U8        nMaxNeighborRetries;
    tANI_U16       nNeighborResultsRefreshPeriod;
    tANI_U16       nEmptyScanRefreshPeriod;
}tCsrNeighborRoamConfigParams;
#endif

typedef struct tagCsrConfigParam
{
    tANI_U32 FragmentationThreshold;
    tANI_U32 channelBondingMode24GHz;   //                                                                
    tANI_U32 channelBondingMode5GHz;    //                                                                   
    eCsrPhyMode phyMode;
    eCsrBand eBand;
    tANI_U32 RTSThreshold;
    tANI_U32 HeartbeatThresh50;
    tANI_U32 HeartbeatThresh24;
    eCsrCBChoice cbChoice;
    eCsrBand bandCapability;     //                      
    tANI_U32 bgScanInterval;
    tANI_U16 TxRate;
    eCsrRoamWmmUserModeType WMMSupportMode;
    tANI_BOOLEAN Is11eSupportEnabled;
    tANI_BOOLEAN Is11dSupportEnabled;
    tANI_BOOLEAN Is11dSupportEnabledOriginal;
    tANI_BOOLEAN Is11hSupportEnabled;
    tANI_BOOLEAN shortSlotTime;
    tANI_BOOLEAN ProprietaryRatesEnabled;
    tANI_U8 AdHocChannel24;
    tANI_U8 AdHocChannel5G;
    tANI_U32 impsSleepTime;     //                   
    tANI_U32 nScanResultAgeCount;   //                                                                                        
    tANI_U32 scanAgeTimeNCNPS;  //                                                                           
    tANI_U32 scanAgeTimeNCPS;   //                                                                        
    tANI_U32 scanAgeTimeCNPS;   //                                                                        
    tANI_U32 scanAgeTimeCPS;   //                                                                  
    tANI_U32 nRoamingTime;  //                                                                      
    tANI_U8 bCatRssiOffset;     //                                            
    tANI_U8 fEnableMCCMode; //                              
    tANI_U8 fAllowMCCGODiffBI; //                                                                                                                           
                               //                                                                    
    tCsr11dinfo  Csr11dinfo;
    //                                                                                     
    //                                                                                      
    //                                                    
    tANI_BOOLEAN fEnforce11dChannels;
    //                     
    //                                
    //                                
    tANI_BOOLEAN fSupplicantCountryCodeHasPriority;
    //                                                     
    //                                                                       
    //                                                 
    tANI_BOOLEAN fEnforceCountryCodeMatch;
    //                                                                                     
    //                                                                                    
    //               
    tANI_BOOLEAN fEnforceDefaultDomain;

    tANI_U16 vccRssiThreshold;
    tANI_U32 vccUlMacLossThreshold;

    tANI_U32  nPassiveMinChnTime;    //                        
    tANI_U32  nPassiveMaxChnTime;    //                        
    tANI_U32  nActiveMinChnTime;     //                        
    tANI_U32  nActiveMaxChnTime;     //                        

    tANI_U32  nActiveMinChnTimeBtc;     //                        
    tANI_U32  nActiveMaxChnTimeBtc;     //                        
    tANI_U32  disableAggWithBtc;
#ifdef WLAN_AP_STA_CONCURRENCY
    tANI_U32  nPassiveMinChnTimeConc;    //                        
    tANI_U32  nPassiveMaxChnTimeConc;    //                        
    tANI_U32  nActiveMinChnTimeConc;     //                        
    tANI_U32  nActiveMaxChnTimeConc;     //                        
    tANI_U32  nRestTimeConc;             //                        
    tANI_U8   nNumStaChanCombinedConc;   //                               
                                         //                                
    tANI_U8   nNumP2PChanCombinedConc;   //                               
                                         //                                
#endif

    tANI_BOOLEAN IsIdleScanEnabled;
    //                            
    //                                                        
    //                                                          
    tANI_U8 nTxPowerCap;
    tANI_U32  statsReqPeriodicity;  //                                                   
    tANI_U32  statsReqPeriodicityInPS;//                                                   
#ifdef WLAN_FEATURE_VOWIFI_11R
    tCsr11rConfigParams  csr11rConfig;
#endif
#ifdef FEATURE_WLAN_CCX
    tANI_U8   isCcxIniFeatureEnabled;
#endif
#ifdef FEATURE_WLAN_LFR
    tANI_U8   isFastRoamIniFeatureEnabled;
    tANI_U8   MAWCEnabled;
#endif

#if  defined (WLAN_FEATURE_VOWIFI_11R) || defined (FEATURE_WLAN_CCX) || defined(FEATURE_WLAN_LFR)
    tANI_U8        isFastTransitionEnabled;
    tANI_U8        RoamRssiDiff;
    tANI_U8        nImmediateRoamRssiDiff;
    tANI_BOOLEAN   isWESModeEnabled;
#endif

#ifdef WLAN_FEATURE_NEIGHBOR_ROAMING
    tCsrNeighborRoamConfigParams    neighborRoamConfig;
#endif

    /*                                                                      
                                                */
    tANI_BOOLEAN addTSWhenACMIsOff;


    /*                                                                               
                                                                                         
                                     */
    tANI_BOOLEAN fValidateList;

    /*                                                                              
                                                                          
                                                                               
                                             */
    tANI_BOOLEAN fEnableBypass11d;

    /*                                                                        
                                                                   
                      */
    tANI_BOOLEAN fEnableDFSChnlScan;

    //                                                                                  
    tANI_BOOLEAN fScanTwice;
#ifdef WLAN_FEATURE_11AC
    tANI_U32        nVhtChannelWidth;
    tANI_U8         enableTxBF;
    tANI_U8         txBFCsnValue;
    tANI_BOOLEAN    enableVhtFor24GHz;
#endif

    /*
                                                                   
    */
    tANI_BOOLEAN fFirstScanOnly2GChnl;

#if  defined (WLAN_FEATURE_VOWIFI_11R) || defined (FEATURE_WLAN_CCX) || defined(FEATURE_WLAN_LFR)
    tANI_BOOLEAN nRoamPrefer5GHz;
    tANI_BOOLEAN nRoamIntraBand;
    tANI_U8      nProbes;
    tANI_U16     nRoamScanHomeAwayTime;

#ifdef WLAN_FEATURE_ROAM_SCAN_OFFLOAD
    tANI_BOOLEAN isRoamOffloadScanEnabled;
    tANI_BOOLEAN bFastRoamInConIniFeatureEnabled;
#endif
#endif


    tANI_U8 scanCfgAgingTime;

    tANI_U8   enableTxLdpc;

    tANI_BOOLEAN  enableOxygenNwk;

    tANI_U8 isAmsduSupportInAMPDU;
    tANI_U8 nSelect5GHzMargin;

}tCsrConfigParam;

//    
typedef struct tagCsrUpdateConfigParam
{
   tCsr11dinfo  Csr11dinfo;
}tCsrUpdateConfigParam;

typedef struct tagCsrRoamInfo
{
    tCsrRoamProfile *pProfile;  //           
    tSirBssDescription *pBssDesc;  //           
    tANI_U32 nBeaconLength; //                                                   
    tANI_U32 nAssocReqLength;   //                                                      
    tANI_U32 nAssocRspLength;   //                                                      
    tANI_U32 nFrameLength;
    tANI_U8  frameType;
    tANI_U8 *pbFrames;  //                                                                               
                        //                                                                                 
                        //                           
    tANI_BOOLEAN fReassocReq;   //                                 
    tANI_BOOLEAN fReassocRsp;   //                                 
    tCsrBssid bssid;
    //                  
    //                                                                                 
    tCsrBssid peerMac;
    tSirResultCodes statusCode;
    tANI_U32 reasonCode;    //                                                                         
    tANI_U8  staId;         //                              
    /*                                                                         
                                              */
    /*                     */
    tANI_U8            ucastSig;

    /*                       */
    tANI_U8            bcastSig;

    tANI_BOOLEAN fAuthRequired;   //                                                                                   
    tANI_U8 sessionId;
    tANI_U8 rsnIELen;
    tANI_U8 *prsnIE;

    tANI_U8 addIELen;
    tANI_U8 *paddIE;

    union
    {
        tSirMicFailureInfo *pMICFailureInfo;
        tCsrRoamConnectedProfile *pConnectedProfile;
        tSirWPSPBCProbeReq *pWPSPBCProbeReq;
    } u;

    tANI_BOOLEAN wmmEnabledSta;   //                              
    tANI_U32 dtimPeriod;

#ifdef FEATURE_WLAN_CCX
    tANI_BOOLEAN isCCXAssoc;
#ifdef FEATURE_WLAN_CCX_UPLOAD
    tSirTsmIE tsmIe;
    tANI_U32 timestamp[2];
    tANI_U16 tsmRoamDelay;
    tSirCcxBcnReportRsp *pCcxBcnReportRsp;
#endif /*                         */
#endif
    void* pRemainCtx;
    tANI_U32 rxChan;

#ifdef FEATURE_WLAN_TDLS
    tANI_U8 staType;
#endif

    //                                                  
    tANI_U32 beaconLength;
    tANI_U8* beaconPtr;
    tANI_U32 assocReqLength;
    tANI_U8* assocReqPtr;

    tANI_S8 rxRssi;
}tCsrRoamInfo;





typedef struct tagCsrFreqScanInfo
{
    tANI_U32 nStartFreq;    //              
    tANI_U32 nEndFreq;      //              
    tSirScanType scanType;
}tCsrFreqScanInfo;


typedef struct sSirSmeAssocIndToUpperLayerCnf
{
    tANI_U16             messageType; //                   
    tANI_U16             length;
    tANI_U8              sessionId;
    tSirResultCodes      statusCode;
    tSirMacAddr          bssId;      //           
    tSirMacAddr          peerMacAddr;
    tANI_U16             aid;
    tSirMacAddr          alternateBssId;
    tANI_U8              alternateChannelId;
    tANI_U8              wmmEnabledSta;   //                              
    tSirRSNie            rsnIE;           //                          
    tSirAddie            addIE;           //                                                                 
    tANI_U8              reassocReq;      //                      
} tSirSmeAssocIndToUpperLayerCnf, *tpSirSmeAssocIndToUpperLayerCnf;

typedef struct tagCsrSummaryStatsInfo
{
   tANI_U32 retry_cnt[4];
   tANI_U32 multiple_retry_cnt[4];
   tANI_U32 tx_frm_cnt[4];
   //                                                 
   //                                              
   tANI_U32 rx_frm_cnt;
   tANI_U32 frm_dup_cnt;
   tANI_U32 fail_cnt[4];
   tANI_U32 rts_fail_cnt;
   tANI_U32 ack_fail_cnt;
   tANI_U32 rts_succ_cnt;
   tANI_U32 rx_discard_cnt;
   tANI_U32 rx_error_cnt;
   tANI_U32 tx_byte_cnt;

}tCsrSummaryStatsInfo;

typedef struct tagCsrGlobalClassAStatsInfo
{
   tANI_U32 rx_frag_cnt;
   tANI_U32 promiscuous_rx_frag_cnt;
   //                    
   tANI_U32 rx_input_sensitivity;
   tANI_U32 max_pwr;
   //                     
   tANI_U32 sync_fail_cnt;
   tANI_U32 tx_rate;
   //                                 
   tANI_U32  mcs_index;
   //                                                                          
   tANI_U32  tx_rate_flags;

}tCsrGlobalClassAStatsInfo;

typedef struct tagCsrGlobalClassBStatsInfo
{
   tANI_U32 uc_rx_wep_unencrypted_frm_cnt;
   tANI_U32 uc_rx_mic_fail_cnt;
   tANI_U32 uc_tkip_icv_err;
   tANI_U32 uc_aes_ccmp_format_err;
   tANI_U32 uc_aes_ccmp_replay_cnt;
   tANI_U32 uc_aes_ccmp_decrpt_err;
   tANI_U32 uc_wep_undecryptable_cnt;
   tANI_U32 uc_wep_icv_err;
   tANI_U32 uc_rx_decrypt_succ_cnt;
   tANI_U32 uc_rx_decrypt_fail_cnt;
   tANI_U32 mcbc_rx_wep_unencrypted_frm_cnt;
   tANI_U32 mcbc_rx_mic_fail_cnt;
   tANI_U32 mcbc_tkip_icv_err;
   tANI_U32 mcbc_aes_ccmp_format_err;
   tANI_U32 mcbc_aes_ccmp_replay_cnt;
   tANI_U32 mcbc_aes_ccmp_decrpt_err;
   tANI_U32 mcbc_wep_undecryptable_cnt;
   tANI_U32 mcbc_wep_icv_err;
   tANI_U32 mcbc_rx_decrypt_succ_cnt;
   tANI_U32 mcbc_rx_decrypt_fail_cnt;

}tCsrGlobalClassBStatsInfo;

typedef struct tagCsrGlobalClassCStatsInfo
{
   tANI_U32 rx_amsdu_cnt;
   tANI_U32 rx_ampdu_cnt;
   tANI_U32 tx_20_frm_cnt;
   tANI_U32 rx_20_frm_cnt;
   tANI_U32 rx_mpdu_in_ampdu_cnt;
   tANI_U32 ampdu_delimiter_crc_err;

}tCsrGlobalClassCStatsInfo;

typedef struct tagCsrGlobalClassDStatsInfo
{
   tANI_U32 tx_uc_frm_cnt;
   tANI_U32 tx_mc_frm_cnt;
   tANI_U32 tx_bc_frm_cnt;
   tANI_U32 rx_uc_frm_cnt;
   tANI_U32 rx_mc_frm_cnt;
   tANI_U32 rx_bc_frm_cnt;
   tANI_U32 tx_uc_byte_cnt[4];
   tANI_U32 tx_mc_byte_cnt;
   tANI_U32 tx_bc_byte_cnt;
   tANI_U32 rx_uc_byte_cnt[4];
   tANI_U32 rx_mc_byte_cnt;
   tANI_U32 rx_bc_byte_cnt;
   tANI_U32 rx_byte_cnt;
   tANI_U32 num_rx_bytes_crc_ok;
   tANI_U32 rx_rate;

}tCsrGlobalClassDStatsInfo;

typedef struct tagCsrPerStaStatsInfo
{
   tANI_U32 tx_frag_cnt[4];
   tANI_U32 tx_ampdu_cnt;
   tANI_U32 tx_mpdu_in_ampdu_cnt;
} tCsrPerStaStatsInfo;

typedef struct tagCsrRoamSetKey
{
    eCsrEncryptionType encType;
    tAniKeyDirection keyDirection;    //                   
    tCsrBssid peerMac;   //                                        
    tANI_U8 paeRole;      //                
    tANI_U8 keyId;  //          
    tANI_U16 keyLength;  //                                          
    tANI_U8 Key[CSR_MAX_KEY_LEN];
    tANI_U8 keyRsc[CSR_MAX_RSC_LEN];
} tCsrRoamSetKey;

typedef struct tagCsrRoamRemoveKey
{
    eCsrEncryptionType encType;
    tCsrBssid peerMac;   //                                        
    tANI_U8 keyId;  //         
} tCsrRoamRemoveKey;

#ifdef FEATURE_WLAN_TDLS

typedef struct tagCsrLinkEstablishParams
{
    tSirMacAddr peerMac;
    tANI_U8 uapsdQueues;
    tANI_U8 maxSp;
    tANI_U8 isBufSta;
    tANI_U8 isResponder;
}tCsrTdlsLinkEstablishParams;

typedef struct tagCsrTdlsSendMgmt
{
        tSirMacAddr peerMac;
        tANI_U8 frameType;
        tANI_U8 dialog;
        tANI_U16 statusCode;
        tANI_U8 responder;
        tANI_U8 *buf;
        tANI_U8 len;

}tCsrTdlsSendMgmt;

#ifdef FEATURE_WLAN_TDLS_INTERNAL
typedef struct tagCsrTdlsDisRequest
{
        tSirMacAddr peerMac;
            tANI_U8 disType;
}tCsrTdlsDisRequest;

typedef struct tagCsrTdlsSetupRequest
{
        tSirMacAddr peerMac;
            tANI_U8 linkIndex;
}tCsrTdlsSetupRequest;

typedef struct tagCsrTdlsTeardownRequest
{
        tSirMacAddr peerMac;
            tANI_U8 linkIndex;
}tCsrTdlsTeardownRequest ;
#endif
#endif

typedef void * tScanResultHandle;

#define CSR_INVALID_SCANRESULT_HANDLE       (NULL)

#ifdef WLAN_FEATURE_ROAM_SCAN_OFFLOAD
typedef struct tagCsrHandoffRequest
{
    tCsrBssid bssid;
    tANI_U8 channel;
}tCsrHandoffRequest;
#endif

#if defined(FEATURE_WLAN_CCX) && defined(FEATURE_WLAN_CCX_UPLOAD)
typedef struct tagCsrCcxBeaconReqParams
{
    tANI_U16   measurementToken;
    tANI_U8    channel;
    tANI_U8    scanMode;
    tANI_U16   measurementDuration;
} tCsrCcxBeaconReqParams, *tpCsrCcxBeaconReqParams;

typedef struct tagCsrCcxBeaconReq
{
    tANI_U8                numBcnReqIe;
    tCsrCcxBeaconReqParams bcnReq[SIR_CCX_MAX_MEAS_IE_REQS];
} tCsrCcxBeaconReq, *tpCsrCcxBeaconReq;
#endif /*                                             */

//                                                            

//                                                     
//                                                                
typedef eHalStatus (*csrScanCompleteCallback)(tHalHandle, void *p2, tANI_U32 scanID, eCsrScanStatus status);



//                                                           

//                                                        
//                                                                           
//                                                          
//                                                                          
//                                                          
//                        
typedef eHalStatus (*csrRoamCompleteCallback)(void *pContext, tCsrRoamInfo *pParam, tANI_U32 roamId,
                                              eRoamCmdStatus roamStatus, eCsrRoamResult roamResult);

typedef eHalStatus (*csrRoamSessionCloseCallback)(void *pContext);

/*                                                                            
                               
                                               
                                                        
                                                                                 */
//                                                  

/*                                                                            
                            
                                      
                                                                                                                                         
                                                 
                                                                                               
                                                                                           
                                                                                 */
//                                                                                               

//                                     
#define CSR_IS_START_IBSS(pProfile) (eCSR_BSS_TYPE_START_IBSS == (pProfile)->BSSType)
#define CSR_IS_JOIN_TO_IBSS(pProfile) (eCSR_BSS_TYPE_IBSS == (pProfile)->BSSType)
#define CSR_IS_IBSS(pProfile) ( CSR_IS_START_IBSS(pProfile) || CSR_IS_JOIN_TO_IBSS(pProfile) )
#define CSR_IS_INFRASTRUCTURE(pProfile) (eCSR_BSS_TYPE_INFRASTRUCTURE == (pProfile)->BSSType)
#define CSR_IS_ANY_BSS_TYPE(pProfile) (eCSR_BSS_TYPE_ANY == (pProfile)->BSSType)
#define CSR_IS_WDS_AP( pProfile )  ( eCSR_BSS_TYPE_WDS_AP == (pProfile)->BSSType )
#define CSR_IS_WDS_STA( pProfile ) ( eCSR_BSS_TYPE_WDS_STA == (pProfile)->BSSType )
#define CSR_IS_WDS( pProfile )  ( CSR_IS_WDS_AP( pProfile ) || CSR_IS_WDS_STA( pProfile ) )
#define CSR_IS_INFRA_AP( pProfile )  ( eCSR_BSS_TYPE_INFRA_AP == (pProfile)->BSSType )

//                                              
#define CSR_IS_CONN_INFRA_AP( pProfile )  ( eCSR_BSS_TYPE_INFRA_AP == (pProfile)->BSSType )
#define CSR_IS_CONN_WDS_AP( pProfile )  ( eCSR_BSS_TYPE_WDS_AP == (pProfile)->BSSType )
#define CSR_IS_CONN_WDS_STA( pProfile ) ( eCSR_BSS_TYPE_WDS_STA == (pProfile)->BSSType )
#define CSR_IS_CONN_WDS( pProfile )  ( CSR_IS_WDS_AP( pProfile ) || CSR_IS_WDS_STA( pProfile ) )



//                                                         



/*                                                                            
                      
                                                                  
                                                                                   
                                           
                      
                                                                                 */

eHalStatus csrSetChannels(tHalHandle hHal,  tCsrConfigParam *pParam  );

eHalStatus csrSetRegInfo(tHalHandle hHal,  tANI_U8 *apCntryCode);


//                                          
const char * get_eRoamCmdStatus_str(eRoamCmdStatus val);
const char * get_eCsrRoamResult_str(eCsrRoamResult val);
/*                                                                            
                     
                                                      
                                                                                                          
        
                                                                                                          
                                  
                                                                
                                                                                                              
                                           
                      
                                                                                 */
eHalStatus csrSetPhyMode(tHalHandle hHal, tANI_U32 phyMode, eCsrBand eBand, tANI_BOOLEAN *pfRestartNeeded);

void csrDumpInit(tHalHandle hHal);


/*                                                                           
                                                                        
                 
                                                                              
                                                     
                                                                    

                                                                      
                       

                                      
                                                                 
              

                                                                           */
typedef void (* csrRoamLinkQualityIndCallback)
             (eCsrRoamLinkQualityInd  ind, void *pContext);


/*                                                                           
                                                                      
                     

                                                                            
                                                                    
                                                                         
                                                                               

                                                           
                                                                 
              

                                                                           */
typedef void ( *tCsrStatsCallback) (void * stats, void *pContext);

/*                                                                           
                                                                
                  

                    
                                                                 
              

                                                                           */

typedef void ( *tCsrRssiCallback) (v_S7_t rssi, tANI_U32 staId, void *pContext);


#if defined(FEATURE_WLAN_CCX) && defined(FEATURE_WLAN_CCX_UPLOAD)
/*                                                                           
                                                                     
                       

                                
                                                                 
              

                                                                           */

typedef void ( *tCsrTsmStatsCallback) (tAniTrafStrmMetrics tsmMetrics, tANI_U32 staId, void *pContext);
#endif /*                                             */

/*                                                                           
                                                               
                 

            
                                                                 
              

                                                                           */
typedef void (*tCsrSnrCallback) (v_S7_t snr, tANI_U32 staId, void *pContext);

#ifdef WLAN_FEATURE_VOWIFI_11R
eHalStatus csrRoamIssueFTPreauthReq(tHalHandle hHal, tANI_U32 sessionId, tpSirBssDescription pBssDescription);
#endif

/*                                                                           
                                                                           

                                     
                           
                     

                                                                           */
eHalStatus csrSetBand(tHalHandle hHal, eCsrBand eBand);

/*                                                                           
                                                              
                                    
                               

                                                                           */
eCsrBand csrGetCurrentBand (tHalHandle hHal);

#endif


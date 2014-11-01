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

#ifndef __SIR_API_H
#define __SIR_API_H

#include "sirTypes.h"
#include "sirMacProtDef.h"
#include "aniSystemDefs.h"
#include "sirParams.h"

#if defined(FEATURE_WLAN_CCX) && !defined(FEATURE_WLAN_CCX_UPLOAD)
#include "ccxGlobal.h"
#endif

//                                           
#define SIR_MAX_NUM_STA                256

//                                                            
#define SIR_MAX_NUM_NEIGHBOR_BSS       3

//                                                            
#define SIR_MAX_NUM_ALTERNATE_RADIOS   5

//                                  
#define SIR_MAX_SCAN_RSP_MSG_LENGTH    2600

//                                                    
#define SIR_HAL_HOST_MSG_START         0x1000

//                              
#define SIR_MAX_POWER_SAVE          3
#define SIR_INTERMEDIATE_POWER_SAVE 4
#define SIR_NO_POWER_SAVE           5

//                            
#define SIR_MAX_SUPPORTED_CHANNEL_LIST      96

//                     
#define SIR_MAX_DTIM_FACTOR         32

#define SIR_MDIE_SIZE               3

/*                                                                      
                           
 */
#define SIR_MAX_24G_5G_CHANNEL_RANGE      166
#define SIR_BCN_REPORT_MAX_BSS_DESC       4


#ifdef FEATURE_WLAN_BATCH_SCAN
#define SIR_MAX_SSID_SIZE (32)
#endif


#define SIR_NUM_11B_RATES 4   //          
#define SIR_NUM_11A_RATES 8  //                     
#define SIR_NUM_POLARIS_RATES 3 //         
#define SIR_NUM_TITAN_RATES 26
#define SIR_NUM_TAURUS_RATES 4 //                      
#define SIR_NUM_PROP_RATES  (SIR_NUM_TITAN_RATES + SIR_NUM_TAURUS_RATES)

#define SIR_11N_PROP_RATE_136_5 (1<<28)
#define SIR_11N_PROP_RATE_151_7 (1<<29)
#define SIR_11N_PROP_RATE_283_5 (1<<30)
#define SIR_11N_PROP_RATE_315     (1<<31)
#define SIR_11N_PROP_RATE_BITMAP 0x80000000 //                                    
//                                                                                
//                                                                                                
//                                             
//                       
#define SIR_TITAN_PROP_RATE_BITMAP 0
#define SIR_CONVERT_2_U32_BITMAP(nRates) ((nRates + 31)/32)

/*                                                                   */
#define SIR_NUM_U32_MAP_RATES    SIR_CONVERT_2_U32_BITMAP(SIR_NUM_PROP_RATES)


#define SIR_PM_SLEEP_MODE   0
#define SIR_PM_ACTIVE_MODE        1

//                                          
#define ANI_IGNORE_CFG_ID 0xFFFF

//                   
#define SIR_SCAN_NO_HIDDEN_SSID                      0
#define SIR_SCAN_HIDDEN_SSID_PE_DECISION             1
#define SIR_SCAN_HIDDEN_SSID                         2

#define SIR_MAC_ADDR_LEN        6
#define SIR_IPV4_ADDR_LEN       4

typedef tANI_U8 tSirIpv4Addr[SIR_IPV4_ADDR_LEN];

#define SIR_VERSION_STRING_LEN 64
typedef tANI_U8 tSirVersionString[SIR_VERSION_STRING_LEN];

/*                                     */
#define PERIODIC_TX_PTRN_MAX_SIZE 1536
#define MAXNUM_PERIODIC_TX_PTRNS 6

enum eSirHostMsgTypes
{
    SIR_HAL_APP_SETUP_NTF = SIR_HAL_HOST_MSG_START,
    SIR_HAL_INITIAL_CAL_FAILED_NTF,
    SIR_HAL_NIC_OPER_NTF,
    SIR_HAL_INIT_START_REQ,
    SIR_HAL_SHUTDOWN_REQ,
    SIR_HAL_SHUTDOWN_CNF,
    SIR_HAL_RESET_REQ,
    SIR_HAL_RADIO_ON_OFF_IND,    
    SIR_HAL_RESET_CNF,
    SIR_WRITE_TO_TD,
    SIR_HAL_HDD_ADDBA_REQ, //           
    SIR_HAL_HDD_ADDBA_RSP, //                   
    SIR_HAL_DELETEBA_IND, //           
    SIR_HAL_BA_FAIL_IND, //           
    SIR_TL_HAL_FLUSH_AC_REQ, 
    SIR_HAL_TL_FLUSH_AC_RSP
};



/* 
                         
 */
enum {
    SIR_BOOT_MODULE_ID = 1,
    SIR_HAL_MODULE_ID  = 0x10,
    SIR_CFG_MODULE_ID = 0x12,
    SIR_LIM_MODULE_ID,
    SIR_ARQ_MODULE_ID,
    SIR_SCH_MODULE_ID,
    SIR_PMM_MODULE_ID,
    SIR_MNT_MODULE_ID,
    SIR_DBG_MODULE_ID,
    SIR_DPH_MODULE_ID,
    SIR_SYS_MODULE_ID,
    SIR_SMS_MODULE_ID,

    SIR_PHY_MODULE_ID = 0x20,


    //                                
    SIR_DVT_MODULE_ID
};

#define SIR_WDA_MODULE_ID SIR_HAL_MODULE_ID

/* 
                                                       
  
                                                         
                                    
 */
#define SIR_FIRST_MODULE_ID     SIR_HAL_MODULE_ID
#define SIR_LAST_MODULE_ID      SIR_DVT_MODULE_ID


//                                                     

//                                    
typedef enum eSirScanType
{
    eSIR_PASSIVE_SCAN,
    eSIR_ACTIVE_SCAN,
    eSIR_BEACON_TABLE,
} tSirScanType;

//                                         
typedef enum eSirResultCodes
{
    eSIR_SME_SUCCESS,

    eSIR_EOF_SOF_EXCEPTION,
    eSIR_BMU_EXCEPTION,
    eSIR_LOW_PDU_EXCEPTION,
    eSIR_USER_TRIG_RESET,
    eSIR_LOGP_EXCEPTION,
    eSIR_CP_EXCEPTION,
    eSIR_STOP_BSS,
    eSIR_AHB_HANG_EXCEPTION,
    eSIR_DPU_EXCEPTION,
    eSIR_RPE_EXCEPTION,
    eSIR_TPE_EXCEPTION,
    eSIR_DXE_EXCEPTION,
    eSIR_RXP_EXCEPTION,
    eSIR_MCPU_EXCEPTION,
    eSIR_MCU_EXCEPTION,
    eSIR_MTU_EXCEPTION,
    eSIR_MIF_EXCEPTION,
    eSIR_FW_EXCEPTION,
    eSIR_PS_MUTEX_READ_EXCEPTION,
    eSIR_PHY_HANG_EXCEPTION,
    eSIR_MAILBOX_SANITY_CHK_FAILED,
    eSIR_RADIO_HW_SWITCH_STATUS_IS_OFF, //                                  
    eSIR_CFB_FLAG_STUCK_EXCEPTION,

    eSIR_SME_BASIC_RATES_NOT_SUPPORTED_STATUS=30,

    eSIR_SME_INVALID_PARAMETERS=500,
    eSIR_SME_UNEXPECTED_REQ_RESULT_CODE,
    eSIR_SME_RESOURCES_UNAVAILABLE,
    eSIR_SME_SCAN_FAILED,   //                                
                            //                                 
    eSIR_SME_BSS_ALREADY_STARTED_OR_JOINED,
    eSIR_SME_LOST_LINK_WITH_PEER_RESULT_CODE,
    eSIR_SME_REFUSED,
    eSIR_SME_JOIN_TIMEOUT_RESULT_CODE,
    eSIR_SME_AUTH_TIMEOUT_RESULT_CODE,
    eSIR_SME_ASSOC_TIMEOUT_RESULT_CODE,
    eSIR_SME_REASSOC_TIMEOUT_RESULT_CODE,
    eSIR_SME_MAX_NUM_OF_PRE_AUTH_REACHED,
    eSIR_SME_AUTH_REFUSED,
    eSIR_SME_INVALID_WEP_DEFAULT_KEY,
    eSIR_SME_NO_KEY_MAPPING_KEY_FOR_PEER,
    eSIR_SME_ASSOC_REFUSED,
    eSIR_SME_REASSOC_REFUSED,
    eSIR_SME_DEAUTH_WHILE_JOIN, //                                                    
    eSIR_SME_DISASSOC_WHILE_JOIN, //                                      
    eSIR_SME_DEAUTH_WHILE_REASSOC, //                                  
    eSIR_SME_DISASSOC_WHILE_REASSOC, //                                         
    eSIR_SME_STA_NOT_AUTHENTICATED,
    eSIR_SME_STA_NOT_ASSOCIATED,
    eSIR_SME_STA_DISASSOCIATED,
    eSIR_SME_ALREADY_JOINED_A_BSS,
    eSIR_ULA_COMPLETED,
    eSIR_ULA_FAILURE,
    eSIR_SME_LINK_ESTABLISHED,
    eSIR_SME_UNABLE_TO_PERFORM_MEASUREMENTS,
    eSIR_SME_UNABLE_TO_PERFORM_DFS,
    eSIR_SME_DFS_FAILED,
    eSIR_SME_TRANSFER_STA, //                                     
    eSIR_SME_INVALID_LINK_TEST_PARAMETERS,//                             
    eSIR_SME_LINK_TEST_MAX_EXCEEDED,    //                             
    eSIR_SME_UNSUPPORTED_RATE,          //                                    
                                        //                          
                                        //              
    eSIR_SME_LINK_TEST_TIMEOUT,         //                                    
                                        //                                    
                                        //        
    eSIR_SME_LINK_TEST_COMPLETE,        //                                  
                                        //             
    eSIR_SME_LINK_TEST_INVALID_STATE,   //                             
    eSIR_SME_LINK_TEST_TERMINATE,       //                             
    eSIR_SME_LINK_TEST_INVALID_ADDRESS, //                            
    eSIR_SME_POLARIS_RESET,             //                          
    eSIR_SME_SETCONTEXT_FAILED,         //                                 
                                        //                          
    eSIR_SME_BSS_RESTART,               //                          

    eSIR_SME_MORE_SCAN_RESULTS_FOLLOW,  //                              
                                        //                       
                                        //                        
                                        //                          
                                        //                        
                                        //                      
    eSIR_SME_INVALID_ASSOC_RSP_RXED,    //                             
                                        //                        
                                        //                            
    eSIR_SME_MIC_COUNTER_MEASURES,      //                                                                          
                                        //                                                                
    eSIR_SME_ADDTS_RSP_TIMEOUT,         //                                     
                                        //                 
    eSIR_SME_ADDTS_RSP_FAILED,          //                                     
    eSIR_SME_RECEIVED,
    //                                 

    eSIR_SME_CHANNEL_SWITCH_FAIL,        //                                               
    eSIR_SME_INVALID_STA_ROLE,
    eSIR_SME_INVALID_STATE,
#ifdef GEN4_SCAN
    eSIR_SME_CHANNEL_SWITCH_DISABLED,    //                                                            
    eSIR_SME_HAL_SCAN_INIT_FAILED,       //                                                     
    eSIR_SME_HAL_SCAN_START_FAILED,      //                                              
    eSIR_SME_HAL_SCAN_END_FAILED,        //                                            
    eSIR_SME_HAL_SCAN_FINISH_FAILED,     //                                               
    eSIR_SME_HAL_SEND_MESSAGE_FAIL,      //                                
#else //          
    eSIR_SME_CHANNEL_SWITCH_DISABLED,    //                                                            
    eSIR_SME_HAL_SEND_MESSAGE_FAIL,      //                                
#endif //          
#ifdef FEATURE_OEM_DATA_SUPPORT
    eSIR_SME_HAL_OEM_DATA_REQ_START_FAILED,
#endif
    eSIR_SME_STOP_BSS_FAILURE,           //                       
    eSIR_SME_STA_ASSOCIATED,
    eSIR_SME_INVALID_PMM_STATE,
    eSIR_SME_CANNOT_ENTER_IMPS,
    eSIR_SME_IMPS_REQ_FAILED,
    eSIR_SME_BMPS_REQ_FAILED,
    eSIR_SME_BMPS_REQ_REJECT,
    eSIR_SME_UAPSD_REQ_FAILED,
    eSIR_SME_WOWL_ENTER_REQ_FAILED,
    eSIR_SME_WOWL_EXIT_REQ_FAILED,
#if defined WLAN_FEATURE_VOWIFI_11R
    eSIR_SME_FT_REASSOC_TIMEOUT_FAILURE,
    eSIR_SME_FT_REASSOC_FAILURE,
#endif
    eSIR_SME_SEND_ACTION_FAIL,
#ifdef WLAN_FEATURE_PACKET_FILTERING
    eSIR_SME_PC_FILTER_MATCH_COUNT_REQ_FAILED,
#endif //                              
    
#ifdef WLAN_FEATURE_GTK_OFFLOAD
    eSIR_SME_GTK_OFFLOAD_GETINFO_REQ_FAILED,
#endif //                         
    eSIR_SME_DEAUTH_STATUS,
    eSIR_DONOT_USE_RESULT_CODE = SIR_MAX_ENUM_SIZE    
} tSirResultCodes;

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
    eSTA_INVALID_RATE_MODE
} tStaRateMode, *tpStaRateMode;

//                                                                                                            
//                                                                                           
#define IERATE_BASICRATE_MASK     0x80
#define IERATE_RATE_MASK          0x7f
#define IERATE_IS_BASICRATE(x)   ((x) & IERATE_BASICRATE_MASK)
#define ANIENHANCED_TAURUS_RATEMAP_BITOFFSET_START  28

typedef struct sSirSupportedRates {
    /*
                                                    
                                                              
                 
                                                         
                                                                                                              
            
                                                                                        
                                              
                                                                                            
    */

    tStaRateMode        opRateMode;
    //                                                                             
    tANI_U16             llbRates[SIR_NUM_11B_RATES];
    tANI_U16             llaRates[SIR_NUM_11A_RATES];
    tANI_U16             aniLegacyRates[SIR_NUM_POLARIS_RATES];

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

#ifdef WLAN_FEATURE_11AC
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
#endif
} tSirSupportedRates, *tpSirSupportedRates;


typedef enum eSirRFBand
{
    SIR_BAND_UNKNOWN,
    SIR_BAND_2_4_GHZ,
    SIR_BAND_5_GHZ,
} tSirRFBand;


/*
                                                                       
                                
*/
typedef enum eBeaconForwarding
{
    ePM_BEACON_FWD_NTH,
    ePM_BEACON_FWD_TIM,
    ePM_BEACON_FWD_DTIM,
    ePM_BEACON_FWD_NONE
} tBeaconForwarding;


typedef struct sSirRemainOnChnReq
{
    tANI_U16 messageType;
    tANI_U16 length;
    tANI_U8 sessionId;
    tSirMacAddr selfMacAddr;
    tANI_U8  chnNum;
    tANI_U8  phyMode;
    tANI_U32 duration;
    tANI_U8  isProbeRequestAllowed;
    tANI_U8  probeRspIe[1];
}tSirRemainOnChnReq, *tpSirRemainOnChnReq;

typedef struct sSirRegisterMgmtFrame
{
    tANI_U16 messageType;
    tANI_U16 length;
    tANI_U8 sessionId;
    tANI_BOOLEAN registerFrame;
    tANI_U16 frameType;
    tANI_U16 matchLen;
    tANI_U8  matchData[1];
}tSirRegisterMgmtFrame, *tpSirRegisterMgmtFrame;

//
//                                                     
//                                 
//
typedef struct sAniTitanCBNeighborInfo
{
  //                               
  tANI_U8 cbBssFoundPri;

  //                                                
  tANI_U8 cbBssFoundSecUp;

  //                                                
  tANI_U8 cbBssFoundSecDown;

} tAniTitanCBNeighborInfo, *tpAniTitanCBNeighborInfo;

//                                             
//                                   
typedef struct sSirSmeRsp
{
    tANI_U16             messageType; //               
    tANI_U16             length;
    tANI_U8              sessionId;  //                  
    tANI_U16             transactionId;   //                  
    tSirResultCodes statusCode;
} tSirSmeRsp, *tpSirSmeRsp;

//                                              
typedef struct sSirSmeStartReq
{
    tANI_U16   messageType;      //                   
    tANI_U16   length;    
    tANI_U8      sessionId;      //                        
    tANI_U16     transcationId;  //                        
    tSirMacAddr  bssId;          //                           
    tANI_U32   roamingAtPolaris;
    tANI_U32   sendNewBssInd;
} tSirSmeStartReq, *tpSirSmeStartReq;

//                                                    
typedef struct sSirSmeReadyReq
{
    tANI_U16   messageType; //                       
    tANI_U16   length;
    tANI_U16   transactionId;     
} tSirSmeReadyReq, *tpSirSmeReadyReq;

//                                                                    
typedef struct sSirSmeStartRsp
{
    tANI_U16             messageType; //                   
    tANI_U16             length;
    tSirResultCodes statusCode;
    tANI_U16             transactionId;     
} tSirSmeStartRsp, *tpSirSmeStartRsp;


//                               
typedef struct sSirLoad
{
    tANI_U16             numStas;
    tANI_U16             channelUtilization;
} tSirLoad, *tpSirLoad;

//                                                  
typedef enum eSirBssType
{
    eSIR_INFRASTRUCTURE_MODE,
    eSIR_INFRA_AP_MODE,                    //                        
    eSIR_IBSS_MODE,
    eSIR_BTAMP_STA_MODE,                     //                        
    eSIR_BTAMP_AP_MODE,                     //                        
    eSIR_AUTO_MODE,
    eSIR_DONOT_USE_BSS_TYPE = SIR_MAX_ENUM_SIZE
} tSirBssType;

//                                
typedef struct sSirWdsInfo
{
    tANI_U16                wdsLength;
    tANI_U8                 wdsBytes[ANI_WDS_INFO_MAX_LENGTH];
} tSirWdsInfo, *tpSirWdsInfo;

//                                   
typedef struct sSirMacPowerCapInfo
{
    tANI_U8              minTxPower;
    tANI_U8              maxTxPower;
} tSirMacPowerCapInfo, *tpSirMacPowerCapInfo;

//                                    
typedef struct sSirSupChnl
{
    tANI_U8              numChnl;
    tANI_U8              channelList[SIR_MAX_SUPPORTED_CHANNEL_LIST];
} tSirSupChnl, *tpSirSupChnl;

typedef enum eSirNwType
{
    eSIR_11A_NW_TYPE,
    eSIR_11B_NW_TYPE,
    eSIR_11G_NW_TYPE,
    eSIR_11N_NW_TYPE,
#ifdef WLAN_FEATURE_11AC
    eSIR_11AC_NW_TYPE,
#endif
    eSIR_DONOT_USE_NW_TYPE = SIR_MAX_ENUM_SIZE
} tSirNwType;

//                                   
typedef struct sSirNewIbssPeerInfo
{
    tSirMacAddr    peerAddr;
    tANI_U16            aid;
} tSirNewIbssPeerInfo, *tpSirNewIbssPeerInfo;

//                                   
typedef struct sSirAlternateRadioInfo
{
    tSirMacAddr    bssId;
    tANI_U8             channelId;
} tSirAlternateRadioInfo, *tpSirAlternateRadioInfo;

//                                   
typedef struct sSirAlternateRadioList
{
    tANI_U8                       numBss;
    tSirAlternateRadioInfo   alternateRadio[1];
} tSirAlternateRadioList, *tpSirAlternateRadioList;

//                                  
//          
/* 
                                     
                                    
                                                                
                         
                                                             
                                                          
                                                                
                                                         
                                                                
                                                              
                                               
 */
//                                                                                                    
typedef struct sSirSmeStartBssReq
{
    tANI_U16                messageType;       //                       
    tANI_U16                length;
    tANI_U8                 sessionId;       //                        
    tANI_U16                transactionId;   //                        
    tSirMacAddr             bssId;           //                        
    tSirMacAddr             selfMacAddr;     //                        
    tANI_U16                beaconInterval;  //                        
    tANI_U8                 dot11mode;
    tSirBssType             bssType;
    tSirMacSSid             ssId;
    tANI_U8                 channelId;
    ePhyChanBondState       cbMode;
    
    tANI_U8                 privacy;
    tANI_U8                 apUapsdEnable;
    tANI_U8                 ssidHidden;
    tANI_BOOLEAN            fwdWPSPBCProbeReq;
    tANI_BOOLEAN            protEnabled;
    tANI_BOOLEAN            obssProtEnabled;
    tANI_U16                ht_capab;
    tAniAuthType            authType;
    tANI_U32                dtimPeriod;
    tANI_U8                 wps_state;
    tVOS_CON_MODE           bssPersona;

    tANI_U8                 txLdpcIniFeatureEnabled;

    tANI_U8                 oxygenNwkIniFeatureEnabled;

    tSirRSNie               rsnIE;             //                     
                                               //                 
                                               //                
    tSirNwType              nwType;            //                  
    tSirMacRateSet          operationalRateSet;//                     
    tSirMacRateSet          extendedRateSet;    //              

} tSirSmeStartBssReq, *tpSirSmeStartBssReq;

#define GET_IE_LEN_IN_BSS(lenInBss) ( lenInBss + sizeof(lenInBss) - \
              ((int) OFFSET_OF( tSirBssDescription, ieFields)))

#define WSCIE_PROBE_RSP_LEN (317 + 2)

typedef struct sSirBssDescription
{
    //                                  
    tANI_U16             length;
    tSirMacAddr          bssId;
    v_TIME_t             scanSysTimeMsec;
    tANI_U32             timeStamp[2];
    tANI_U16             beaconInterval;
    tANI_U16             capabilityInfo;
    tSirNwType           nwType; //                  
    tANI_U8              aniIndicator;
    tANI_S8              rssi;
    tANI_S8              sinr;
    //                                            
    tANI_U8              channelId;
    //                                    
    //                       
    tANI_U8              channelIdSelf;
    tANI_U8              sSirBssDescriptionRsvd[3];
    tANI_TIMESTAMP nReceivedTime;     //                                                              
#if defined WLAN_FEATURE_VOWIFI
    tANI_U32       parentTSF;
    tANI_U32       startTSF[2];
#endif
#ifdef WLAN_FEATURE_VOWIFI_11R
    tANI_U8              mdiePresent;
    tANI_U8              mdie[SIR_MDIE_SIZE];                //                                      
#endif
#ifdef FEATURE_WLAN_CCX
    tANI_U16             QBSSLoad_present;
    tANI_U16             QBSSLoad_avail; 
#endif
    //                                                             

    tANI_U8              fProbeRsp; //                              
    tANI_U8              reservedPadding1;
    tANI_U8              reservedPadding2;
    tANI_U8              reservedPadding3;
    tANI_U32             WscIeLen;
    tANI_U8              WscIeProbeRsp[WSCIE_PROBE_RSP_LEN];
    tANI_U8              reservedPadding4;
    
    tANI_U32             ieFields[1];
} tSirBssDescription, *tpSirBssDescription;

//                                               
//                          
//          
typedef struct sSirSmeStartBssRsp
{
    tANI_U16            messageType; //                       
    tANI_U16            length;
    tANI_U8             sessionId;
    tANI_U16            transactionId;//                      
    tSirResultCodes     statusCode;
    tSirBssType         bssType;//                         
    tANI_U16            beaconInterval;//                             
    tANI_U32            staId;//                    
    tSirBssDescription  bssDescription;//                    
} tSirSmeStartBssRsp, *tpSirSmeStartBssRsp;


typedef struct sSirChannelList
{
    tANI_U8          numChannels;
    tANI_U8          channelNumber[SIR_CCX_MAX_MEAS_IE_REQS];
} tSirChannelList, *tpSirChannelList;

typedef struct sSirDFSChannelList
{
    tANI_U32         timeStamp[SIR_MAX_24G_5G_CHANNEL_RANGE];

} tSirDFSChannelList, *tpSirDFSChannelList;

#ifdef FEATURE_WLAN_CCX
typedef struct sTspecInfo {
    tANI_U8         valid;
    tSirMacTspecIE  tspec;
} tTspecInfo;

#define SIR_CCX_MAX_TSPEC_IES   4
typedef struct sCCXTspecTspecInfo {
    tANI_U8 numTspecs;
    tTspecInfo tspec[SIR_CCX_MAX_TSPEC_IES];
} tCCXTspecInfo;
#endif


//                           
typedef struct sSirRadarInfo
{
    tANI_U8          channelNumber;
    tANI_U16         radarPulseWidth; //           
    tANI_U16         numRadarPulse;
} tSirRadarInfo, *tpSirRadarInfo;

#define SIR_RADAR_INFO_SIZE                (sizeof(tANI_U8) + 2 *sizeof(tANI_U16))

//                          
typedef enum eSirBackgroundScanMode
{
    eSIR_AGGRESSIVE_BACKGROUND_SCAN = 0,
    eSIR_NORMAL_BACKGROUND_SCAN = 1,
    eSIR_ROAMING_SCAN = 2,
} tSirBackgroundScanMode;

//                            
typedef enum eSirLinkTrafficCheck
{
    eSIR_DONT_CHECK_LINK_TRAFFIC_BEFORE_SCAN = 0,
    eSIR_CHECK_LINK_TRAFFIC_BEFORE_SCAN = 1,
    eSIR_CHECK_ROAMING_SCAN = 2,
} tSirLinkTrafficCheck;

#define SIR_BG_SCAN_RETURN_CACHED_RESULTS              0x0
#define SIR_BG_SCAN_PURGE_RESUTLS                      0x80
#define SIR_BG_SCAN_RETURN_FRESH_RESULTS               0x01
#define SIR_SCAN_MAX_NUM_SSID                          0x09 
#define SIR_BG_SCAN_RETURN_LFR_CACHED_RESULTS          0x02
#define SIR_BG_SCAN_PURGE_LFR_RESULTS                  0x40

//                             
typedef struct sSirSmeScanReq
{
    tANI_U16        messageType; //                  
    tANI_U16        length;
    tANI_U8         sessionId;         //           
    tANI_U16        transactionId;     //                       
    tSirMacAddr     bssId;
    tSirMacSSid     ssId[SIR_SCAN_MAX_NUM_SSID];
    tSirMacAddr     selfMacAddr; //                        
    tSirBssType     bssType;
    tANI_U8         dot11mode;
    tSirScanType    scanType;
    /* 
                                                       
                                                                            
                                                                                     
                                                                    
     */
    tANI_U32        minChannelTime;
    /* 
                      
                                             
                                                                                                    
      
     */
    tANI_U32        maxChannelTime;
    /* 
                                                       
                                                                 
                                                                       
                                         
                                                                 
                                 
                                                                  
                      
                                                               
                           
      
                                                    
                                               
     */
    tANI_U32 minChannelTimeBtc;    //                        
    tANI_U32 maxChannelTimeBtc;    //                        
    tANI_U8              returnAfterFirstMatch;

    /* 
                                                     
                                                                        
                                                                 
     */
    tANI_U8              returnUniqueResults;

    /* 
                                                    
                                             
                                                    
                                                                     
                      
                                                                     
                                                        
     */
    tANI_U8              returnFreshResults;

    /*                                               
                            
                                                                
                                               
     */
    tSirBackgroundScanMode   backgroundScanMode;

    tANI_U8              hiddenSsid;

    /*                         */
    tANI_U8             numSsid;
    
    //                                                                                              
    /*                                              */
    
 
    tANI_BOOLEAN         p2pSearch;
    tANI_U16             uIEFieldLen;
    tANI_U16             uIEFieldOffset;

    //                                                    
    tSirChannelList channelList;
    /*                             
                        
                                   
                  
                                   
                                        
                                        
                                        
                                        
                                        
                                        
                                        
                                        
                                        
                                 
                                  
                                   */
} tSirSmeScanReq, *tpSirSmeScanReq;

#ifdef FEATURE_OEM_DATA_SUPPORT

#ifndef OEM_DATA_REQ_SIZE
#define OEM_DATA_REQ_SIZE 134
#endif
#ifndef OEM_DATA_RSP_SIZE
#define OEM_DATA_RSP_SIZE 1968
#endif

typedef struct sSirOemDataReq
{
    tANI_U16              messageType; //                     
    tSirMacAddr           selfMacAddr;
    tANI_U8               oemDataReq[OEM_DATA_REQ_SIZE];
} tSirOemDataReq, *tpSirOemDataReq;

typedef struct sSirOemDataRsp
{
    tANI_U16             messageType;
    tANI_U16             length;
    tANI_U8              oemDataRsp[OEM_DATA_RSP_SIZE];
} tSirOemDataRsp, *tpSirOemDataRsp;
    
#endif //                        

//                                                                   
typedef struct sSirSmeScanRsp
{
    tANI_U16           messageType; //                  
    tANI_U16           length;
    tANI_U8            sessionId;     
    tSirResultCodes    statusCode;
    tANI_U16           transcationId; 
    tSirBssDescription bssDescription[1];
} tSirSmeScanRsp, *tpSirSmeScanRsp;

//                                                 
typedef struct sSirSmeBackgroundScanModeReq
{
    tANI_U16                      messageType; //                                  
    tANI_U16                      length;
    tSirBackgroundScanMode   mode;
} tSirSmeBackgroundScanModeReq, *tpSirSmeBackgroundScanModeReq;

//                           
typedef struct sSirBackgroundScanInfo {
    tANI_U32        numOfScanSuccess;
    tANI_U32        numOfScanFailure;
    tANI_U32        reserved;
} tSirBackgroundScanInfo, *tpSirBackgroundScanInfo;

#define SIR_BACKGROUND_SCAN_INFO_SIZE        (3 * sizeof(tANI_U32))

//                                       
typedef struct sSirSmeAuthReq
{
    tANI_U16           messageType; //                  
    tANI_U16           length;
    tANI_U8            sessionId;        //           
    tANI_U16           transactionId;    //                       
    tSirMacAddr        bssId;            //           
    tSirMacAddr        peerMacAddr;
    tAniAuthType       authType;
    tANI_U8            channelNumber;
} tSirSmeAuthReq, *tpSirSmeAuthReq;

//                                                                  
typedef struct sSirSmeAuthRsp
{
    tANI_U16           messageType; //                  
    tANI_U16           length;
    tANI_U8            sessionId;      //           
    tANI_U16           transactionId;  //                       
    tSirMacAddr        peerMacAddr;
    tAniAuthType       authType;
    tSirResultCodes    statusCode;
    tANI_U16           protStatusCode; //                                                            
                                       //                               
} tSirSmeAuthRsp, *tpSirSmeAuthRsp;



//                                                                                                       
typedef struct sJoinReassocInfo
{
    tAniTitanCBNeighborInfo cbNeighbors;
    tAniBool            spectrumMgtIndicator;
    tSirMacPowerCapInfo powerCap;
    tSirSupChnl         supportedChannels;
} tJoinReassocInfo, *tpJoinReassocInfo;

//                             
//          
//                                           
//                                             
//                                          
//                                           
//                                                                
typedef struct sSirSmeJoinReq
{
    tANI_U16            messageType;            //                  
    tANI_U16            length;
    tANI_U8             sessionId;
    tANI_U16            transactionId;  
    tSirMacSSid         ssId;
    tSirMacAddr         selfMacAddr;            //                 
    tSirBssType         bsstype;                //                                            
    tANI_U8             dot11mode;              //                       
    tVOS_CON_MODE       staPersona;             //       
    ePhyChanBondState   cbMode;                 //                            

    /*                                         
                           
                           
                           
                           
     */
    tANI_U8                 uapsdPerAcBitmask;

    tSirMacRateSet      operationalRateSet;//                     
    tSirMacRateSet      extendedRateSet;    //              
    tSirRSNie           rsnIE;                  //                     
                                                //                         
#ifdef FEATURE_WLAN_CCX
    tSirCCKMie          cckmIE;             //                                                           
                                            //                                                             
                                            //    
#endif

    tSirAddie           addIEScan;              //                            
                                                //                                            

    tSirAddie           addIEAssoc;             //                             
                                                //                         

    tAniEdType          UCEncryptionType;

    tAniEdType          MCEncryptionType;

#ifdef WLAN_FEATURE_11W
    tAniEdType          MgmtEncryptionType;
#endif

#ifdef WLAN_FEATURE_VOWIFI_11R
    tAniBool            is11Rconnection;
#endif
#ifdef FEATURE_WLAN_CCX
    tAniBool            isCCXFeatureIniEnabled;
    tAniBool            isCCXconnection;
    tCCXTspecInfo       ccxTspecInfo;
#endif
    
#if defined WLAN_FEATURE_VOWIFI_11R || defined FEATURE_WLAN_CCX || defined(FEATURE_WLAN_LFR)
    tAniBool            isFastTransitionEnabled;
#endif
#ifdef FEATURE_WLAN_LFR
    tAniBool            isFastRoamIniFeatureEnabled;
#endif

    tANI_U8             txLdpcIniFeatureEnabled;
#ifdef WLAN_FEATURE_11AC
    tANI_U8             txBFIniFeatureEnabled;
    tANI_U8             txBFCsnValue;
#endif
    tANI_U8             isAmsduSupportInAMPDU;
    tAniBool            isWMEenabled;
    tAniBool            isQosEnabled;
    tAniTitanCBNeighborInfo cbNeighbors;
    tAniBool            spectrumMgtIndicator;
    tSirMacPowerCapInfo powerCap;
    tSirSupChnl         supportedChannels;
    tSirBssDescription  bssDescription;

} tSirSmeJoinReq, *tpSirSmeJoinReq;

//                                                                  
//          
typedef struct sSirSmeJoinRsp
{
    tANI_U16                messageType; //                  
    tANI_U16                length;
    tANI_U8                 sessionId;         //           
    tANI_U16                transactionId;     //                       
    tSirResultCodes    statusCode;
    tAniAuthType       authType;
    tANI_U16        protStatusCode; //                                                                 
                                    //                               
    tANI_U16        aid;
    tANI_U32        beaconLength;
    tANI_U32        assocReqLength;
    tANI_U32        assocRspLength;
#ifdef WLAN_FEATURE_VOWIFI_11R
    tANI_U32        parsedRicRspLen;
#endif
#ifdef FEATURE_WLAN_CCX
    tANI_U32        tspecIeLen;
#endif
    tANI_U32        staId;//                   

    /*                                                                          
                                              */
    /*                     */
    tANI_U8            ucastSig;

    /*                       */
    tANI_U8            bcastSig;

    tANI_U8         frames[ 1 ];
} tSirSmeJoinRsp, *tpSirSmeJoinRsp;

//                                                    
typedef struct sSirSmeAuthInd
{
    tANI_U16           messageType; //                  
    tANI_U16           length;         
    tANI_U8            sessionId;
    tSirMacAddr        bssId;             //           
    tSirMacAddr        peerMacAddr;
    tAniAuthType       authType;
} tSirSmeAuthInd, *tpSirSmeAuthInd;

//                                         
typedef struct sSirSmeProbereq
{
    tANI_U16           messageType; //                   
    tANI_U16           length;
    tANI_U8            sessionId;
    tSirMacAddr        peerMacAddr;
    tANI_U16           devicePasswdId;
} tSirSmeProbeReq, *tpSirSmeProbeReq;

//                                                 
//          
typedef struct sSirSmeAssocInd
{
    tANI_U16             messageType; //                   
    tANI_U16             length;
    tANI_U8              sessionId;
    tSirMacAddr          peerMacAddr;
    tANI_U16             aid;
    tSirMacAddr          bssId; //           
    tANI_U16             staId; //                    
    tANI_U8              uniSig;  //                                  
    tANI_U8              bcastSig; //                                    
    tAniAuthType         authType;    
    tAniSSID             ssId; //                              
    tSirRSNie            rsnIE;//                          
    tSirAddie            addIE;//                                                                              

    //                                                   
    //                                 
    tAniBool                spectrumMgtIndicator;
    tSirMacPowerCapInfo     powerCap;
    tSirSupChnl             supportedChannels;
    tAniBool             wmmEnabledSta; /*                                 */
    tAniBool             reassocReq;
    //                                                  
    tANI_U32             beaconLength;
    tANI_U8*             beaconPtr;
    tANI_U32             assocReqLength;
    tANI_U8*             assocReqPtr;
} tSirSmeAssocInd, *tpSirSmeAssocInd;


//                                    
//          
typedef struct sSirSmeAssocCnf
{
    tANI_U16             messageType; //                   
    tANI_U16             length;
    tSirResultCodes      statusCode;
    tSirMacAddr          bssId;      //           
    tSirMacAddr          peerMacAddr;
    tANI_U16             aid;
    tSirMacAddr          alternateBssId;
    tANI_U8              alternateChannelId;
} tSirSmeAssocCnf, *tpSirSmeAssocCnf;

//                                                   
typedef struct sSirSmeReassocInd
{
    tANI_U16            messageType; //                     
    tANI_U16            length;
    tANI_U8             sessionId;         //           
    tSirMacAddr         peerMacAddr;
    tSirMacAddr         oldMacAddr;
    tANI_U16            aid;
    tSirMacAddr         bssId;           //           
    tANI_U16            staId;           //                    
    tAniAuthType        authType;
    tAniSSID            ssId;   //                                
    tSirRSNie           rsnIE;  //                          

    tSirAddie           addIE;  //                                 
    
    //                                                   
    //                                 
    tAniBool                spectrumMgtIndicator;
    tSirMacPowerCapInfo     powerCap;
    tSirSupChnl             supportedChannels;
    //                                                  
    //                                                                                                                        
    tANI_U32             beaconLength;
    tANI_U8*             beaconPtr;
    tANI_U32             assocReqLength;
    tANI_U8*             assocReqPtr;
} tSirSmeReassocInd, *tpSirSmeReassocInd;

//                                      
//          
typedef struct sSirSmeReassocCnf
{
    tANI_U16                  messageType; //                     
    tANI_U16                  length;
    tSirResultCodes      statusCode;
    tSirMacAddr          bssId;             //           
    tSirMacAddr          peerMacAddr;
    tANI_U16                  aid;
    tSirMacAddr          alternateBssId;
    tANI_U8                   alternateChannelId;
} tSirSmeReassocCnf, *tpSirSmeReassocCnf;


//                                                          
typedef enum eSirSmeStatusChangeCode
{
    eSIR_SME_DEAUTH_FROM_PEER,
    eSIR_SME_DISASSOC_FROM_PEER,
    eSIR_SME_LOST_LINK_WITH_PEER,
    eSIR_SME_CHANNEL_SWITCH,
    eSIR_SME_JOINED_NEW_BSS,
    eSIR_SME_LEAVING_BSS,
    eSIR_SME_IBSS_ACTIVE,
    eSIR_SME_IBSS_INACTIVE,
    eSIR_SME_IBSS_PEER_DEPARTED,
    eSIR_SME_RADAR_DETECTED,
    eSIR_SME_IBSS_NEW_PEER,
    eSIR_SME_AP_CAPS_CHANGED,
    eSIR_SME_BACKGROUND_SCAN_FAIL,
    eSIR_SME_CB_LEGACY_BSS_FOUND_BY_AP,
    eSIR_SME_CB_LEGACY_BSS_FOUND_BY_STA
} tSirSmeStatusChangeCode;

typedef struct sSirSmeNewBssInfo
{
    tSirMacAddr   bssId;
    tANI_U8            channelNumber;
    tANI_U8            reserved;
    tSirMacSSid   ssId;
} tSirSmeNewBssInfo, *tpSirSmeNewBssInfo;

typedef struct sSirSmeApNewCaps
{
    tANI_U16           capabilityInfo;
    tSirMacAddr   bssId;
    tANI_U8            channelId;
    tANI_U8            reserved[3];
    tSirMacSSid   ssId;
} tSirSmeApNewCaps, *tpSirSmeApNewCaps;

/* 
                                                               
                                                  
  
                                                  
                                                                         
                                                                        
                                                                          
                                        
                                                      
                                                                  
                                        
                                                                     
                                                                 
                                                
                                                                        
                                           
                                                                    
                                                                         
                                                                         
                                                                         
                                                 
                                                                     
 */

//                                                           
typedef struct sSirSmeWmStatusChangeNtf
{
    tANI_U16                     messageType; //                              
    tANI_U16                     length;
    tANI_U8                      sessionId;         //           
    tSirSmeStatusChangeCode statusChangeCode;
    tSirMacAddr             bssId;             //           
    union
    {
        tANI_U16                 deAuthReasonCode; //                          
        tANI_U16                 disassocReasonCode; //                            
        //                                      
        tANI_U8                  newChannelId;   //                        
        tSirSmeNewBssInfo   newBssInfo;     //                        
        //                              
        //                              
        //                                
        tSirNewIbssPeerInfo     newIbssPeerInfo;  //                       
        tSirSmeApNewCaps        apNewCaps;        //                         
        tSirBackgroundScanInfo  bkgndScanInfo;    //                              
        tAniTitanCBNeighborInfo cbNeighbors;      //                                    
    } statusChangeInfo;
} tSirSmeWmStatusChangeNtf, *tpSirSmeWmStatusChangeNtf;

//                                       
typedef
__ani_attr_pre_packed
struct sSirSmeDisassocReq
{
    tANI_U16            messageType; //                      
    tANI_U16            length;
    tANI_U8             sessionId;         //           
    tANI_U16            transactionId;     //                       
    tSirMacAddr         bssId;             //           
    tSirMacAddr peerMacAddr;
    tANI_U16         reasonCode;
    tANI_U8          doNotSendOverTheAir;  //                                                           
                                           //                                                          
}
__ani_attr_packed
tSirSmeDisassocReq, *tpSirSmeDisassocReq;

//                                             
typedef __ani_attr_pre_packed struct sSirSmeTkipCntrMeasReq
{
    tANI_U16            messageType;    //                      
    tANI_U16            length;
    tANI_U8             sessionId;      //           
    tANI_U16            transactionId;  //                       
    tSirMacAddr         bssId;          //           
    tANI_BOOLEAN        bEnable;        //                           
} __ani_attr_packed tSirSmeTkipCntrMeasReq, *tpSirSmeTkipCntrMeasReq;

typedef struct sAni64BitCounters
{
    tANI_U32 Hi;
    tANI_U32 Lo;
}tAni64BitCounters, *tpAni64BitCounters;

typedef struct sAniSecurityStat
{
    tAni64BitCounters txBlks;
    tAni64BitCounters rxBlks;
    tAni64BitCounters formatErrorCnt;
    tAni64BitCounters decryptErr;
    tAni64BitCounters protExclCnt;
    tAni64BitCounters unDecryptableCnt;
    tAni64BitCounters decryptOkCnt;

}tAniSecurityStat, *tpAniSecurityStat;

typedef struct sAniTxRxCounters
{
    tANI_U32 txFrames; //                                
    tANI_U32 rxFrames;    
    tANI_U32 nRcvBytes;
    tANI_U32 nXmitBytes;
}tAniTxRxCounters, *tpAniTxRxCounters;

typedef struct sAniTxRxStats
{
    tAni64BitCounters txFrames;
    tAni64BitCounters rxFrames;
    tAni64BitCounters nRcvBytes;
    tAni64BitCounters nXmitBytes;

}tAniTxRxStats,*tpAniTxRxStats;

typedef struct sAniSecStats
{
    tAniSecurityStat aes;
    tAni64BitCounters aesReplays;
    tAniSecurityStat tkip;
    tAni64BitCounters tkipReplays;
    tAni64BitCounters tkipMicError;

    tAniSecurityStat wep;
#if defined(FEATURE_WLAN_WAPI) && !defined(LIBRA_WAPI_SUPPORT)
    tAniSecurityStat wpi;
    tAni64BitCounters wpiReplays;
    tAni64BitCounters wpiMicError;
#endif
}tAniSecStats, *tpAniSecStats;    

#define SIR_MAX_RX_CHAINS 3

typedef struct sAniStaStatStruct
{
    /*                                                                                   
                                                                  
                                                             
                                                 
     */
    tANI_U32 sentAesBlksUcastHi;
    tANI_U32 sentAesBlksUcastLo;
    tANI_U32 recvAesBlksUcastHi;
    tANI_U32 recvAesBlksUcastLo;
    tANI_U32 aesFormatErrorUcastCnts;
    tANI_U32 aesReplaysUcast;
    tANI_U32 aesDecryptErrUcast;
    tANI_U32 singleRetryPkts;
    tANI_U32 failedTxPkts;
    tANI_U32 ackTimeouts;
    tANI_U32 multiRetryPkts;
    tANI_U32 fragTxCntsHi;
    tANI_U32 fragTxCntsLo;
    tANI_U32 transmittedPktsHi;
    tANI_U32 transmittedPktsLo;
    tANI_U32 phyStatHi; //                                       
    tANI_U32 phyStatLo; //                             

    tANI_U32 uplinkRssi;
    tANI_U32 uplinkSinr;
    tANI_U32 uplinkRate;
    tANI_U32 downlinkRssi;
    tANI_U32 downlinkSinr;
    tANI_U32 downlinkRate;
    tANI_U32 nRcvBytes;
    tANI_U32 nXmitBytes;

    //               
    tANI_U32 chunksTxCntHi;          //                             
    tANI_U32 chunksTxCntLo;
    tANI_U32 compPktRxCntHi;         //                                                         
    tANI_U32 compPktRxCntLo;
    tANI_U32 expanPktRxCntHi;        //                                             
    tANI_U32 expanPktRxCntLo;


    /*                                                                              
     */

    //                         
    tAniTxRxStats ucStats;

    //                           
    tAniTxRxStats bcStats;

    //                           
    tAniTxRxStats mcStats;

    tANI_U32      currentTxRate; 
    tANI_U32      currentRxRate; //               

    tANI_U32      maxTxRate;
    tANI_U32      maxRxRate;

    tANI_S8       rssi[SIR_MAX_RX_CHAINS]; 


    tAniSecStats   securityStats;

    tANI_U8       currentRxRateIdx; //                            
    tANI_U8       currentTxRateIdx;

} tAniStaStatStruct, *tpAniStaStatStruct;

//                                                
typedef struct sAniGlobalStatStruct
{
  tAni64BitCounters txError;
  tAni64BitCounters rxError;
  tAni64BitCounters rxDropNoBuffer;
  tAni64BitCounters rxDropDup;
  tAni64BitCounters rxCRCError;

  tAni64BitCounters singleRetryPkts;
  tAni64BitCounters failedTxPkts;
  tAni64BitCounters ackTimeouts;
  tAni64BitCounters multiRetryPkts;
  tAni64BitCounters fragTxCnts;
  tAni64BitCounters fragRxCnts;

  tAni64BitCounters txRTSSuccess;
  tAni64BitCounters txCTSSuccess;
  tAni64BitCounters rxRTSSuccess;
  tAni64BitCounters rxCTSSuccess;

  tAniSecStats      securityStats;

  tAniTxRxStats     mcStats;
  tAniTxRxStats     bcStats;
    
}tAniGlobalStatStruct,*tpAniGlobalStatStruct;

typedef enum sPacketType
{
    ePACKET_TYPE_UNKNOWN,
    ePACKET_TYPE_11A,
    ePACKET_TYPE_11G,
    ePACKET_TYPE_11B,
    ePACKET_TYPE_11N

}tPacketType, *tpPacketType;

typedef struct sAniStatSummaryStruct
{
    tAniTxRxStats uc; //                 
    tAniTxRxStats bc; //                   
    tAniTxRxStats mc; //                   
    tAni64BitCounters txError;
    tAni64BitCounters rxError;
    tANI_S8     rssi[SIR_MAX_RX_CHAINS]; //               
    tANI_U32    rxRate; //                                     
    tANI_U32    txRate;
    tANI_U16    rxMCSId; //                                                            
    tANI_U16    txMCSId;
    tPacketType rxPacketType;
    tPacketType txPacketType;
    tSirMacAddr macAddr; //                                                                  
}tAniStatSummaryStruct,*tpAniStatSummaryStruct;

//                                                                      
//                                                                          
typedef struct _sPermStaStats
{
    //                            
    //                            
    //                            
    //                            
    tANI_U32 aesFormatErrorUcastCnts;
    tANI_U32 aesReplaysUcast;
    tANI_U32 aesDecryptErrUcast;
    tANI_U32 singleRetryPkts;
    tANI_U32 failedTxPkts;
    tANI_U32 ackTimeouts;
    tANI_U32 multiRetryPkts;
    tANI_U32 fragTxCntsHi;
    tANI_U32 fragTxCntsLo;
    tANI_U32 transmittedPktsHi;
    tANI_U32 transmittedPktsLo;

    //               
    tANI_U32 chunksTxCntHi;          //                             
    tANI_U32 chunksTxCntLo;
    tANI_U32 compPktRxCntHi;         //                                                         
    tANI_U32 compPktRxCntLo;
    tANI_U32 expanPktRxCntHi;        //                                             
    tANI_U32 expanPktRxCntLo;
}tPermanentStaStats;




//                                        
typedef struct sSirSmeDisassocRsp
{
    tANI_U16           messageType; //                      
    tANI_U16           length;
    tANI_U8            sessionId;         //           
    tANI_U16           transactionId;     //                       
    tSirResultCodes    statusCode;
    tSirMacAddr        peerMacAddr;
    tAniStaStatStruct  perStaStats; //          
    tANI_U16           staId;
}
__ani_attr_packed
 tSirSmeDisassocRsp, *tpSirSmeDisassocRsp;

//                                                    
typedef struct sSirSmeDisassocInd
{
    tANI_U16            messageType; //                      
    tANI_U16            length;
    tANI_U8             sessionId;  //                   
    tANI_U16            transactionId;   //                               
    tSirResultCodes     statusCode;
    tSirMacAddr         bssId;            
    tSirMacAddr         peerMacAddr;
    tAniStaStatStruct  perStaStats; //          
    tANI_U16            staId;
    tANI_U32            reasonCode;
} tSirSmeDisassocInd, *tpSirSmeDisassocInd;

//                                       
//          
typedef struct sSirSmeDisassocCnf
{
    tANI_U16            messageType; //                      
    tANI_U16            length;
    tSirResultCodes     statusCode;
    tSirMacAddr         bssId;            
    tSirMacAddr         peerMacAddr;
} tSirSmeDisassocCnf, *tpSirSmeDisassocCnf;

//                                        
typedef struct sSirSmeDeauthReq
{
    tANI_U16            messageType;   //                    
    tANI_U16            length;
    tANI_U8             sessionId;     //           
    tANI_U16            transactionId; //                       
    tSirMacAddr         bssId;         //         
    tSirMacAddr         peerMacAddr;
    tANI_U16            reasonCode;
} tSirSmeDeauthReq, *tpSirSmeDeauthReq;

//                                         
typedef struct sSirSmeDeauthRsp
{
    tANI_U16                messageType; //                    
    tANI_U16                length;
    tANI_U8             sessionId;         //           
    tANI_U16            transactionId;     //                       
    tSirResultCodes     statusCode;
    tSirMacAddr        peerMacAddr;
} tSirSmeDeauthRsp, *tpSirSmeDeauthRsp;

//                                                     
typedef struct sSirSmeDeauthInd
{
    tANI_U16            messageType; //                    
    tANI_U16            length;
    tANI_U8            sessionId;       //                
    tANI_U16            transactionId;  //                
    tSirResultCodes     statusCode;
    tSirMacAddr         bssId;//         
    tSirMacAddr         peerMacAddr;

    tANI_U16            staId;
    tANI_U32            reasonCode;
} tSirSmeDeauthInd, *tpSirSmeDeauthInd;

//                                         
//          
typedef struct sSirSmeDeauthCnf
{
    tANI_U16                messageType; //                    
    tANI_U16                length;
    tSirResultCodes     statusCode;
    tSirMacAddr         bssId;             //         
    tSirMacAddr        peerMacAddr;
} tSirSmeDeauthCnf, *tpSirSmeDeauthCnf;

//                                         
typedef struct sSirSmeStopBssReq
{
    tANI_U16                messageType;    //                      
    tANI_U16                length;
    tANI_U8             sessionId;      //          
    tANI_U16            transactionId;  //                      
    tSirResultCodes         reasonCode;
    tSirMacAddr             bssId;          //          
} tSirSmeStopBssReq, *tpSirSmeStopBssReq;

//                                          
typedef struct sSirSmeStopBssRsp
{
    tANI_U16             messageType; //                      
    tANI_U16             length;
    tSirResultCodes statusCode;
    tANI_U8             sessionId;         //           
    tANI_U16            transactionId;     //                       
} tSirSmeStopBssRsp, *tpSirSmeStopBssRsp;



//                                                      
//          
typedef struct sSirSmeSwitchChannelInd
{
    tANI_U16                messageType; //                        
    tANI_U16                length;
    tANI_U8                 sessionId;
    tANI_U16    newChannelId;
    tSirMacAddr        bssId;      //      
} tSirSmeSwitchChannelInd, *tpSirSmeSwitchChannelInd;

//                                                
typedef struct sirUlaCompleteInd
{
    tANI_U16                messageType; //                      
    tANI_U16                length;
    tSirResultCodes    statusCode;
    tSirMacAddr        peerMacAddr;
} tSirUlaCompleteInd, *tpSirUlaCompleteInd;

//                                                  
typedef struct sirUlaCompleteCnf
{
    tANI_U16                messageType; //                      
    tANI_U16                length;
    tSirResultCodes    statusCode;
    tSirMacAddr        peerMacAddr;
} tSirUlaCompleteCnf, *tpSirUlaCompleteCnf;

//                                        
//          
//                                                    
typedef struct sSirSmeNeighborBssInd
{
    tANI_U16                    messageType; //                          
    tANI_U16                    length;
    tANI_U8                     sessionId;
    tSirBssDescription     bssDescription[1];
} tSirSmeNeighborBssInd, *tpSirSmeNeighborBssInd;

//                                       
//          
//                                                                    
typedef struct sSirSmeMicFailureInd
{
    tANI_U16                    messageType; //                         
    tANI_U16                    length;
    tANI_U8                     sessionId;
    tSirMacAddr         bssId;             //      
    tSirMicFailureInfo     info;
} tSirSmeMicFailureInd, *tpSirSmeMicFailureInd;

typedef struct sSirSmeMissedBeaconInd
{
    tANI_U16                    messageType; //                           
    tANI_U16                    length;
    tANI_U8                     bssIdx;
} tSirSmeMissedBeaconInd, *tpSirSmeMissedBeaconInd;

//                                    
//          
typedef struct sSirSmeSetContextReq
{
    tANI_U16           messageType; //                         
    tANI_U16          length;
    tANI_U8            sessionId;  //          
    tANI_U16           transactionId; //                      
    tSirMacAddr        peerMacAddr;
    tSirMacAddr        bssId;      //      
    //                                           
    //                                  
    //                       
    tSirKeyMaterial    keyMaterial;
} tSirSmeSetContextReq, *tpSirSmeSetContextReq;

//                                     
//          
typedef struct sSirSmeSetContextRsp
{
    tANI_U16                messageType; //                         
    tANI_U16                length;
    tANI_U8             sessionId;         //           
    tANI_U16            transactionId;     //                       
    tSirResultCodes     statusCode;
    tSirMacAddr             peerMacAddr;
} tSirSmeSetContextRsp, *tpSirSmeSetContextRsp;

//                                           
//          
typedef struct sSirSmeRemoveKeyReq
{
    tANI_U16                messageType;    //                        
    tANI_U16                length;
    tANI_U8             sessionId;         //           
    tANI_U16            transactionId;     //                       
    tSirMacAddr         bssId;             //      
    tSirMacAddr             peerMacAddr;
    tANI_U8    edType;
    tANI_U8    wepType;
    tANI_U8    keyId;
    tANI_BOOLEAN unicast;
} tSirSmeRemoveKeyReq, *tpSirSmeRemoveKeyReq;

//                                            
//          
typedef struct sSirSmeRemoveKeyRsp
{
    tANI_U16                messageType; //                        
    tANI_U16                length;
    tANI_U8             sessionId;         //           
    tANI_U16            transactionId;     //                       
    tSirResultCodes     statusCode;
    tSirMacAddr             peerMacAddr;
} tSirSmeRemoveKeyRsp, *tpSirSmeRemoveKeyRsp;

//                                  
//          
typedef struct sSirSmeSetPowerReq
{
    tANI_U16                messageType; //                       
    tANI_U16                length;
    tANI_U16            transactionId;     //                       
    tANI_S8                 powerLevel;
} tSirSmeSetPowerReq, *tpSirSmeSetPowerReq;

//                                   
//          
typedef struct sSirSmeSetPowerRsp
{
    tANI_U16                messageType; //                       
    tANI_U16                length;
    tSirResultCodes    statusCode;
    tANI_U16            transactionId;     //                       
} tSirSmeSetPowerRsp, *tpSirSmeSetPowerRsp;


//                                         
//          
typedef struct sSirSmeLinkTestStartRsp
{
    tANI_U16                messageType; //                             
    tANI_U16                length;
    tSirMacAddr        peerMacAddr;
    tSirResultCodes    statusCode;
} tSirSmeLinkTestStartRsp, *tpSirSmeLinkTestStartRsp;

//                                        
//              
typedef struct sSirSmeLinkTestStopRsp
{
    tANI_U16                messageType; //                            
    tANI_U16                length;
    tSirMacAddr        peerMacAddr;
    tSirResultCodes    statusCode;
} tSirSmeLinkTestStopRsp, *tpSirSmeLinkTestStopRsp;

//                                               
typedef struct sSirSmeDFSreq
{
    tANI_U16             messageType; //                 
    tANI_U16             length;
    tANI_U16            transactionId;     //                       
} tSirSmeDFSrequest, *tpSirSmeDFSrequest;

//                                               
//                    
typedef struct sSirSmeDFSrsp
{
    tANI_U16             messageType; //                 
    tANI_U16             length;
    tSirResultCodes statusCode;
    tANI_U16            transactionId;     //                       
    tANI_U32             dfsReport[1];
} tSirSmeDFSrsp, *tpSirSmeDFSrsp;

//                       
//                                                             
//                                                                         
//                   

//                                          
typedef enum
{
    eANI_CLEAR_ALL_STATS, //                 
    eANI_CLEAR_RX_STATS,  //                                            
    eANI_CLEAR_TX_STATS,  //                                            
    eANI_CLEAR_RADIO_STATS,   //                           
    eANI_CLEAR_PER_STA_STATS, //                     
    eANI_CLEAR_AGGR_PER_STA_STATS, //                       

    //                                                       
    //                                                                    
    eANI_LINK_STATS,     //                  
    eANI_SECURITY_STATS, //                           

    eANI_CLEAR_STAT_TYPES_END
} tAniStatSubTypes;

typedef struct sAniTxCtrs
{
    //                           
    tANI_U32 tx1Mbps;
    tANI_U32 tx2Mbps;
    tANI_U32 tx5_5Mbps;
    tANI_U32 tx6Mbps;
    tANI_U32 tx9Mbps;
    tANI_U32 tx11Mbps;
    tANI_U32 tx12Mbps;
    tANI_U32 tx18Mbps;
    tANI_U32 tx24Mbps;
    tANI_U32 tx36Mbps;
    tANI_U32 tx48Mbps;
    tANI_U32 tx54Mbps;
    tANI_U32 tx72Mbps;
    tANI_U32 tx96Mbps;
    tANI_U32 tx108Mbps;

    //                     
    tANI_U32 txFragHi;
    tANI_U32 txFragLo;
    tANI_U32 txFrameHi;
    tANI_U32 txFrameLo;
    tANI_U32 txMulticastFrameHi;
    tANI_U32 txMulticastFrameLo;
    tANI_U32 txFailedHi;
    tANI_U32 txFailedLo;
    tANI_U32 multipleRetryHi;
    tANI_U32 multipleRetryLo;
    tANI_U32 singleRetryHi;
    tANI_U32 singleRetryLo;
    tANI_U32 ackFailureHi;
    tANI_U32 ackFailureLo;
    tANI_U32 xmitBeacons;

    //               
    tANI_U32 txCbEscPktCntHi;            //                                                         
    tANI_U32 txCbEscPktCntLo;
    tANI_U32 txChunksCntHi;              //                                   
    tANI_U32 txChunksCntLo;
    tANI_U32 txCompPktCntHi;             //                                               
    tANI_U32 txCompPktCntLo;
    tANI_U32 tx50PerCompPktCntHi;        //                                                     
    tANI_U32 tx50PerCompPktCntLo;
    tANI_U32 txExpanPktCntHi;            //                                                      
    tANI_U32 txExpanPktCntLo;
} tAniTxCtrs, *tpAniTxCtrs;

typedef struct sAniRxCtrs
{
    //                            
    tANI_U32 rx1Mbps;
    tANI_U32 rx2Mbps;
    tANI_U32 rx5_5Mbps;
    tANI_U32 rx6Mbps;
    tANI_U32 rx9Mbps;
    tANI_U32 rx11Mbps;
    tANI_U32 rx12Mbps;
    tANI_U32 rx18Mbps;
    tANI_U32 rx24Mbps;
    tANI_U32 rx36Mbps;
    tANI_U32 rx48Mbps;
    tANI_U32 rx54Mbps;
    tANI_U32 rx72Mbps;
    tANI_U32 rx96Mbps;
    tANI_U32 rx108Mbps;

    //                                                     
    tANI_U32 rxLte64;
    tANI_U32 rxLte128Gt64;
    tANI_U32 rxLte256Gt128;
    tANI_U32 rxLte512Gt256;
    tANI_U32 rxLte1kGt512;
    tANI_U32 rxLte1518Gt1k;
    tANI_U32 rxLte2kGt1518;
    tANI_U32 rxLte4kGt2k;

    //               
    tANI_U32 rxFrag;
    tANI_U32 rxFrame;
    tANI_U32 fcsError;
    tANI_U32 rxMulticast;
    tANI_U32 duplicate;
    tANI_U32 rtsSuccess;
    tANI_U32 rtsFailed;
    tANI_U32 wepUndecryptables;
    tANI_U32 drops;
    tANI_U32 aesFormatErrorUcastCnts;
    tANI_U32 aesReplaysUcast;
    tANI_U32 aesDecryptErrUcast;

    //               
    tANI_U32 rxDecompPktCntHi;           //                                              
    tANI_U32 rxDecompPktCntLo;
    tANI_U32 rxCompPktCntHi;             //                                                               
    tANI_U32 rxCompPktCntLo;
    tANI_U32 rxExpanPktCntHi;            //                                                   
    tANI_U32 rxExpanPktCntLo;
} tAniRxCtrs, *tpAniRxCtrs;

//            
typedef struct sAniRadioStats
{
    tAniTxCtrs tx;
    tAniRxCtrs rx;
} tAniRadioStats, *tpAniRadioStats;

//                                  
//                                                                      
//                                                                         
//                                                             
typedef struct sAniGetStatsReq
{
    //                                  
    tANI_U16                msgType;    //                                         
    tANI_U16                msgLen;     //                             
    tANI_U8                 sessionId;  //          
    tANI_U16                transactionId;
    tSirMacAddr             bssId;      //     
    //                                                  
    tAniStatSubTypes        stat;   //                                         
    tANI_U32                staId;  //                                         
                               //       
    tANI_U8                 macAddr[6];
} tAniGetStatsReq, *tpAniGetStatsReq;

//                                
typedef struct sAniGetRadioStatsRsp
{
    tANI_U16            type;   //                                         
    tANI_U16            msgLen; //                             
    tANI_U32            rc;
    tANI_U16            transactionId;
    tAniRadioStats radio;
} tAniGetRadioStatsRsp, *tpAniGetRadioStatsRsp;

//                              
typedef struct sAniGetPerStaStatsRsp
{
    tANI_U16               type;   //                                         
    tANI_U16               msgLen; //                             
    tANI_U32               rc;
    tANI_U16               transactionId;
    tAniStatSubTypes  stat;   //                                              
    tAniStaStatStruct sta;
    tANI_U32               staId;
    tANI_U8                macAddr[6];
} tAniGetPerStaStatsRsp, *tpAniGetPerStaStatsRsp;

//                    
typedef struct sAniGetAggrStaStatsRsp
{
    tANI_U16               type;   //                                         
    tANI_U16               msgLen; //                             
    tANI_U32               rc;
    tANI_U16               transactionId;
    tAniStaStatStruct sta;
} tAniGetAggrStaStatsRsp, *tpAniGetAggrStaStatsRsp;

//                                                                    
//                                                  
typedef struct sAniClearStatsRsp
{
    tANI_U16                type;   //                                         
    tANI_U16                msgLen; //                             
    tANI_U32                rc;     //                                      
                               //          
                       //                                               
    tANI_U16                transactionId;
    tAniStatSubTypes   stat;       //                                         
    tANI_U32                staId;      //                                          
    tANI_U8                 macAddr[6]; //                                          
} tAniClearStatsRsp, *tpAniClearStatsRsp;

typedef struct sAniGetGlobalStatsRsp
{
    tANI_U16            type;   //                                         
    tANI_U16            msgLen; //                             
    tANI_U32            rc;
    tANI_U16            transactionId;
    tAniGlobalStatStruct global;
} tAniGetGlobalStatsRsp, *tpAniGetGlobalStatsRsp;

typedef struct sAniGetStatSummaryRsp
{
    tANI_U16               type;   //                                         
    tANI_U16               msgLen; //                                    
    tANI_U32               rc;
    tANI_U16               transactionId;
    tAniStatSummaryStruct stat;
} tAniGetStatSummaryRsp, *tpAniGetStatSummaryRsp;

//                                                               


/*                                                       */
typedef enum
{
    PE_SUMMARY_STATS_INFO           = 0x00000001,
    PE_GLOBAL_CLASS_A_STATS_INFO    = 0x00000002,
    PE_GLOBAL_CLASS_B_STATS_INFO    = 0x00000004,
    PE_GLOBAL_CLASS_C_STATS_INFO    = 0x00000008,
    PE_GLOBAL_CLASS_D_STATS_INFO    = 0x00000010,
    PE_PER_STA_STATS_INFO           = 0x00000020
}ePEStatsMask;

/*
                                 
                                                         
                                                  
 */
typedef struct sAniGetPEStatsReq
{
    //                                  
    tANI_U16                msgType;    //                                         
    tANI_U16                msgLen;  //                             
    tANI_U32                staId;  //                                         
    tANI_U32                statsMask;  //                                                    
} tAniGetPEStatsReq, *tpAniGetPEStatsReq;

/*
                                 
                                                         
                                                  
 */
typedef struct sAniGetPEStatsRsp
{
    //                                   
    tANI_U16                msgType;    //                                         
    tANI_U16                msgLen;  //                                                                
    tANI_U8                  sessionId;
    tANI_U32                rc;         //               
    tANI_U32                staId;  //                                         
    tANI_U32                statsMask;  //                                                    
/*                                                                                             
                                       
                                                                                     
                                                                                   
                                                                                           
                                                                                             */                                        
} tAniGetPEStatsRsp, *tpAniGetPEStatsRsp;

typedef struct sAniGetRssiReq
{
    //                                  
    tANI_U16                msgType;    //                                         
    tANI_U16                msgLen;  //                             
    tANI_U8                 sessionId;
    tANI_U8                 staId;  
    void                    *rssiCallback;
    void                    *pDevContext; //              
    void                    *pVosContext; //            
    
} tAniGetRssiReq, *tpAniGetRssiReq;

typedef struct sAniGetSnrReq
{
    //                                  
    tANI_U16                msgType;    //                                         
    tANI_U16                msgLen;  //                             
    tANI_U8                 sessionId;
    tANI_U8                 staId;
    void                    *snrCallback;
    void                    *pDevContext; //              
} tAniGetSnrReq, *tpAniGetSnrReq;

#if defined WLAN_FEATURE_VOWIFI_11R || defined FEATURE_WLAN_CCX || defined(FEATURE_WLAN_LFR)
typedef struct sAniGetRoamRssiRsp
{
    //                                   
    tANI_U16                msgType;    //                                         
    tANI_U16                msgLen;  //                                                                
    tANI_U8                 sessionId;
    tANI_U32                rc;         //               
    tANI_U32                staId;  //                                         
    tANI_S8                 rssi;
    void                    *rssiReq;  //                   

} tAniGetRoamRssiRsp, *tpAniGetRoamRssiRsp;

#endif

#if defined(FEATURE_WLAN_CCX) || defined(FEATURE_WLAN_CCX_UPLOAD)

typedef struct sSirTsmIE
{
    tANI_U8      tsid;
    tANI_U8      state;
    tANI_U16     msmt_interval;
} tSirTsmIE, *tpSirTsmIE;

typedef struct sSirSmeTsmIEInd
{
    tSirTsmIE tsmIe;
    tANI_U8   sessionId;
} tSirSmeTsmIEInd, *tpSirSmeTsmIEInd;


typedef struct sAniTrafStrmMetrics
{
    tANI_U16      UplinkPktQueueDly;
    tANI_U16      UplinkPktQueueDlyHist[4];
    tANI_U32      UplinkPktTxDly;
    tANI_U16      UplinkPktLoss;
    tANI_U16      UplinkPktCount;
    tANI_U8       RoamingCount;
    tANI_U16      RoamingDly;
} tAniTrafStrmMetrics, *tpAniTrafStrmMetrics;

typedef struct sAniGetTsmStatsReq
{
    //                                  
    tANI_U16       msgType;            //                                         
    tANI_U16       msgLen;             //                             
    tANI_U8        staId;
    tANI_U8        tid;                //           
    tSirMacAddr    bssId;
    void           *tsmStatsCallback;
    void           *pDevContext;       //              
    void           *pVosContext;       //            
} tAniGetTsmStatsReq, *tpAniGetTsmStatsReq;

typedef struct sAniGetTsmStatsRsp
{
    //                                   
    tANI_U16            msgType;      //                                         
    tANI_U16            msgLen;       //                                                                
    tANI_U8             sessionId;
    tANI_U32            rc;           //               
    tANI_U32            staId;        //                                         
    tAniTrafStrmMetrics tsmMetrics;
    void               *tsmStatsReq; //                        
} tAniGetTsmStatsRsp, *tpAniGetTsmStatsRsp;

typedef struct sSirCcxBcnReportBssInfo
{
    tBcnReportFields  bcnReportFields;
    tANI_U8           ieLen;
    tANI_U8           *pBuf;
} tSirCcxBcnReportBssInfo, *tpSirCcxBcnReportBssInfo;

typedef struct sSirCcxBcnReportRsp
{
    tANI_U16    measurementToken;
    tANI_U8     flag;     /*                                               */
    tANI_U8     numBss;
    tSirCcxBcnReportBssInfo bcnRepBssInfo[SIR_BCN_REPORT_MAX_BSS_DESC];
} tSirCcxBcnReportRsp, *tpSirCcxBcnReportRsp;

#endif /*                                             */

/*                                           */
typedef struct sAniChangeCountryCodeReq
{
    //                                  
    tANI_U16                msgType;    //                                         
    tANI_U16                msgLen;     //                             
    tANI_U8                 countryCode[WNI_CFG_COUNTRY_CODE_LEN];   //                   
    tAniBool                countryFromUserSpace;
    tAniBool                sendRegHint;  //                                       
    void                    *changeCCCallback;
    void                    *pDevContext; //              
    void                    *pVosContext; //            

} tAniChangeCountryCodeReq, *tpAniChangeCountryCodeReq;

/*                                                   */
typedef struct sAniGenericChangeCountryCodeReq
{
    //                                  
    tANI_U16                msgType;    //                                         
    tANI_U16                msgLen;     //                             
    tANI_U8                 countryCode[WNI_CFG_COUNTRY_CODE_LEN];   //                   
    tANI_U16                domain_index;
} tAniGenericChangeCountryCodeReq, *tpAniGenericChangeCountryCodeReq;

typedef struct sAniDHCPStopInd
{
    tANI_U16                msgType;      //                                         
    tANI_U16                msgLen;       //                             
    tANI_U8                 device_mode;  //                               
    tSirMacAddr             macAddr;

} tAniDHCPInd, *tpAniDHCPInd;

typedef struct sAniSummaryStatsInfo
{
    tANI_U32 retry_cnt[4];         //                                                                               
    tANI_U32 multiple_retry_cnt[4];//                                                                   
    //                                                                            

    tANI_U32 tx_frm_cnt[4];        //                                                                   
                                   //                                                                
    //                     
    //                                                                               
    //                                                                                                
    tANI_U32 rx_frm_cnt;           //                                                        
                                   //                                                                    
    tANI_U32 frm_dup_cnt;          //                                                      
    tANI_U32 fail_cnt[4];          //                                               
    tANI_U32 rts_fail_cnt;         //                                                                      
    tANI_U32 ack_fail_cnt;         //                                                                             
    tANI_U32 rts_succ_cnt;         //                                                                      
    tANI_U32 rx_discard_cnt;       //                                                                           
                                   //                                                                       
    tANI_U32 rx_error_cnt;         //                                                                           
    tANI_U32 tx_byte_cnt;          //                                                                           
                                   //                                                                                
                                   //                
#if 0                                   
    //                                                                                                        
    tANI_U32 tx_unicast_lower_byte_cnt;
    tANI_U32 tx_unicast_upper_byte_cnt;
    tANI_U32 tx_multicast_lower_byte_cnt;
    tANI_U32 tx_multicast_upper_byte_cnt;
    tANI_U32 tx_broadcast_lower_byte_cnt;
    tANI_U32 tx_broadcast_upper_byte_cnt;
#endif

}tAniSummaryStatsInfo, *tpAniSummaryStatsInfo;

typedef enum eTxRateInfo
{
   eHAL_TX_RATE_LEGACY = 0x1,    /*              */
   eHAL_TX_RATE_HT20   = 0x2,    /*            */
   eHAL_TX_RATE_HT40   = 0x4,    /*            */
   eHAL_TX_RATE_SGI    = 0x8,    /*                                */
   eHAL_TX_RATE_LGI    = 0x10,   /*                               */
   eHAL_TX_RATE_VHT20  = 0x20,   /*              */
   eHAL_TX_RATE_VHT40  = 0x40,   /*              */
   eHAL_TX_RATE_VHT80  = 0x80    /*              */
} tTxrateinfoflags;

typedef struct sAniGlobalClassAStatsInfo
{
    tANI_U32 rx_frag_cnt;             //                                                                          
                                     //               
    tANI_U32 promiscuous_rx_frag_cnt; //                                                                          
                                     //                                                            
    //                                                                                                              
    tANI_U32 rx_input_sensitivity;    //                                                                           
                                     //                                                                                   
                                     //                                                                                   
    tANI_U32 max_pwr;                 //                                                    
                                      //                                                 
    //                                                                                                  
    tANI_U32 sync_fail_cnt;           //                                                                            
                                     //                                                                                     
    tANI_U32 tx_rate;                //                                  
                                     //                           
                                     //                           
    tANI_U32  mcs_index;             //                                 
    tANI_U32  tx_rate_flags;         //                                  
                                     //                                          

}tAniGlobalClassAStatsInfo, *tpAniGlobalClassAStatsInfo;


typedef struct sAniGlobalSecurityStats
{
    tANI_U32 rx_wep_unencrypted_frm_cnt; //                                                                                 
                                        //                                                                                 
                                        //          
    tANI_U32 rx_mic_fail_cnt;            //                                                                           
                                        //                       
    tANI_U32 tkip_icv_err;               //                                                                              
                                        //                           
    tANI_U32 aes_ccmp_format_err;        //                                                                           
                                        //                       
    tANI_U32 aes_ccmp_replay_cnt;        //                                                                                
                                        //                                        
    tANI_U32 aes_ccmp_decrpt_err;        //                                                                                
                                        //                                                    
    tANI_U32 wep_undecryptable_cnt;      //                                                                                
                                        //                                   
    tANI_U32 wep_icv_err;                //                                                                              
                                        //                          
    tANI_U32 rx_decrypt_succ_cnt;        //                                                                              
                                        //         
    tANI_U32 rx_decrypt_fail_cnt;        //                                                                         

}tAniGlobalSecurityStats, *tpAniGlobalSecurityStats;
   
typedef struct sAniGlobalClassBStatsInfo
{
    tAniGlobalSecurityStats ucStats;
    tAniGlobalSecurityStats mcbcStats;
}tAniGlobalClassBStatsInfo, *tpAniGlobalClassBStatsInfo;

typedef struct sAniGlobalClassCStatsInfo
{
    tANI_U32 rx_amsdu_cnt;           //                                                                                
                                    //                                                                                  
                                    //               
    tANI_U32 rx_ampdu_cnt;           //                                                                             
    tANI_U32 tx_20_frm_cnt;          //                                                                          
                                    //               
    tANI_U32 rx_20_frm_cnt;          //                                                                                      
    tANI_U32 rx_mpdu_in_ampdu_cnt;   //                                                                                
                                    //                          
    tANI_U32 ampdu_delimiter_crc_err;//                                                                                   
                                    //                                                                                     
                                    //                 

}tAniGlobalClassCStatsInfo, *tpAniGlobalClassCStatsInfo;

typedef struct sAniPerStaStatsInfo
{
    tANI_U32 tx_frag_cnt[4];       //                                                                               
                                  //                                   
    tANI_U32 tx_ampdu_cnt;         //                                                                
    tANI_U32 tx_mpdu_in_ampdu_cnt; //                                                                                
                                  //              

}tAniPerStaStatsInfo, *tpAniPerStaStatsInfo;

/*                                                              */



typedef struct sSirRSSIThresholds
{
#ifdef ANI_BIG_BYTE_ENDIAN
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
#else
    tANI_U8   bReserved10          : 2;
    tANI_U8   bRssiThres3NegNotify : 1;
    tANI_U8   bRssiThres3PosNotify : 1;
    tANI_U8   bRssiThres2NegNotify : 1;
    tANI_U8   bRssiThres2PosNotify : 1;
    tANI_U8   bRssiThres1NegNotify : 1;
    tANI_U8   bRssiThres1PosNotify : 1;
    tANI_S8   ucRssiThreshold3     : 8;
    tANI_S8   ucRssiThreshold2     : 8;
    tANI_S8   ucRssiThreshold1     : 8;
#endif

}tSirRSSIThresholds, *tpSirRSSIThresholds;

typedef struct sSirRSSINotification
{
#ifdef ANI_BIG_BYTE_ENDIAN
    tANI_U32             bRssiThres1PosCross : 1;
    tANI_U32             bRssiThres1NegCross : 1;
    tANI_U32             bRssiThres2PosCross : 1;
    tANI_U32             bRssiThres2NegCross : 1;
    tANI_U32             bRssiThres3PosCross : 1;
    tANI_U32             bRssiThres3NegCross : 1;
    v_S7_t               avgRssi             : 8;
    tANI_U32             bReserved           : 18;
#else
    tANI_U32             bReserved           : 18;
    v_S7_t               avgRssi             : 8;
    tANI_U32             bRssiThres3NegCross : 1;
    tANI_U32             bRssiThres3PosCross : 1;
    tANI_U32             bRssiThres2NegCross : 1;
    tANI_U32             bRssiThres2PosCross : 1;
    tANI_U32             bRssiThres1NegCross : 1;
    tANI_U32             bRssiThres1PosCross : 1;
#endif
    
}tSirRSSINotification, *tpSirRSSINotification;


typedef struct sSirP2PNoaStart
{
   tANI_U32      status;
   tANI_U32      bssIdx;
} tSirP2PNoaStart, *tpSirP2PNoaStart;

typedef struct sSirTdlsInd
{
   tANI_U16      status;
   tANI_U16      assocId;
   tANI_U16      staIdx;
   tANI_U16      reasonCode;
} tSirTdlsInd, *tpSirTdlsInd;

typedef struct sSirP2PNoaAttr
{
#ifdef ANI_BIG_BYTE_ENDIAN
   tANI_U32      index :8;
   tANI_U32      oppPsFlag :1;
   tANI_U32      ctWin     :7;
   tANI_U32      rsvd1: 16;
#else
   tANI_U32      rsvd1: 16;
   tANI_U32      ctWin     :7;
   tANI_U32      oppPsFlag :1;
   tANI_U32      index :8;
#endif

#ifdef ANI_BIG_BYTE_ENDIAN
   tANI_U32       uNoa1IntervalCnt:8;
   tANI_U32       rsvd2:24;
#else
   tANI_U32       rsvd2:24;
   tANI_U32       uNoa1IntervalCnt:8;
#endif
   tANI_U32       uNoa1Duration;
   tANI_U32       uNoa1Interval;
   tANI_U32       uNoa1StartTime;

#ifdef ANI_BIG_BYTE_ENDIAN
   tANI_U32       uNoa2IntervalCnt:8;
   tANI_U32       rsvd3:24;
#else
   tANI_U32       rsvd3:24;
   tANI_U32       uNoa2IntervalCnt:8;
#endif
   tANI_U32       uNoa2Duration;
   tANI_U32       uNoa2Interval;
   tANI_U32       uNoa2StartTime;
} tSirP2PNoaAttr, *tpSirP2PNoaAttr;

typedef __ani_attr_pre_packed struct sSirTclasInfo
{
    tSirMacTclasIE   tclas;
    tANI_U8               version; //                                    
    __ani_attr_pre_packed union {
        tSirMacTclasParamEthernet eth;
        tSirMacTclasParamIPv4     ipv4;
        tSirMacTclasParamIPv6     ipv6;
        tSirMacTclasParam8021dq   t8021dq;
    }__ani_attr_packed tclasParams;
} __ani_attr_packed tSirTclasInfo;


#if defined(FEATURE_WLAN_CCX) || defined(FEATURE_WLAN_CCX_UPLOAD)
#define TSRS_11AG_RATE_6MBPS   0xC
#define TSRS_11B_RATE_5_5MBPS  0xB

typedef struct sSirMacCCXTSRSIE
{
    tANI_U8      tsid;
    tANI_U8      rates[8];
} tSirMacCCXTSRSIE;

typedef struct sSirMacCCXTSMIE
{
    tANI_U8      tsid;
    tANI_U8      state;
    tANI_U16     msmt_interval;
} tSirMacCCXTSMIE;

typedef struct sTSMStats
{
    tANI_U8           tid;
    tSirMacAddr       bssId;
    tTrafStrmMetrics  tsmMetrics;
} tTSMStats, *tpTSMStats;

typedef struct sCcxTSMContext
{
   tANI_U8           tid;
   tSirMacCCXTSMIE   tsmInfo;
   tTrafStrmMetrics  tsmMetrics;
} tCcxTSMContext, *tpCcxTSMContext;

typedef struct sCcxPEContext
{
#if defined(FEATURE_WLAN_CCX) && !defined(FEATURE_WLAN_CCX_UPLOAD)
   tCcxMeasReq       curMeasReq;
#endif
   tCcxTSMContext    tsm;
} tCcxPEContext, *tpCcxPEContext;


#endif /*                                             */


typedef struct sSirAddtsReqInfo
{
    tANI_U8               dialogToken;
    tSirMacTspecIE   tspec;

    tANI_U8               numTclas; //                         
    tSirTclasInfo    tclasInfo[SIR_MAC_TCLASIE_MAXNUM];
    tANI_U8               tclasProc;
#if defined(FEATURE_WLAN_CCX)
    tSirMacCCXTSRSIE      tsrsIE;
    tANI_U8               tsrsPresent:1;
#endif
    tANI_U8               wmeTspecPresent:1;
    tANI_U8               wsmTspecPresent:1;
    tANI_U8               lleTspecPresent:1;
    tANI_U8               tclasProcPresent:1;
} tSirAddtsReqInfo, *tpSirAddtsReqInfo;

typedef struct sSirAddtsRspInfo
{
    tANI_U8                 dialogToken;
    tSirMacStatusCodes status;
    tSirMacTsDelayIE   delay;

    tSirMacTspecIE     tspec;
    tANI_U8                 numTclas; //                         
    tSirTclasInfo      tclasInfo[SIR_MAC_TCLASIE_MAXNUM];
    tANI_U8                 tclasProc;
    tSirMacScheduleIE  schedule;
#if defined(FEATURE_WLAN_CCX) || defined(FEATURE_WLAN_CCX_UPLOAD)
    tSirMacCCXTSMIE    tsmIE;
    tANI_U8                 tsmPresent:1;
#endif
    tANI_U8                 wmeTspecPresent:1;
    tANI_U8                 wsmTspecPresent:1;
    tANI_U8                 lleTspecPresent:1;
    tANI_U8                 tclasProcPresent:1;
    tANI_U8                 schedulePresent:1;
} tSirAddtsRspInfo, *tpSirAddtsRspInfo;

typedef struct sSirDeltsReqInfo
{
    tSirMacTSInfo      tsinfo;
    tSirMacTspecIE     tspec;
    tANI_U8                 wmeTspecPresent:1;
    tANI_U8                 wsmTspecPresent:1;
    tANI_U8                 lleTspecPresent:1;
} tSirDeltsReqInfo, *tpSirDeltsReqInfo;

//                        
typedef struct sSirAddtsReq
{
    tANI_U16                messageType; //                   
    tANI_U16                length;
    tANI_U8                 sessionId;  //          
    tANI_U16                transactionId;
    tSirMacAddr             bssId;      //     
    tANI_U32                timeout; //      
    tANI_U8                 rspReqd;
    tSirAddtsReqInfo        req;
} tSirAddtsReq, *tpSirAddtsReq;

typedef struct sSirAddtsRsp
{
    tANI_U16                messageType; //                   
    tANI_U16                length;
    tANI_U8                 sessionId;  //                                         
    tANI_U16                transactionId; //                                            
    tANI_U32                rc;          //            
    tSirAddtsRspInfo        rsp;
} tSirAddtsRsp, *tpSirAddtsRsp;

typedef struct sSirDeltsReq
{
    tANI_U16                messageType; //                   
    tANI_U16                length;
    tANI_U8                 sessionId;//          
    tANI_U16                transactionId;
    tSirMacAddr             bssId;  //     
    tANI_U16                aid;  //                                    
    tANI_U8                 macAddr[6]; //                              
    tANI_U8                 rspReqd;
    tSirDeltsReqInfo        req;
} tSirDeltsReq, *tpSirDeltsReq;

typedef struct sSirDeltsRsp
{
    tANI_U16                messageType; //                   
    tANI_U16                length;
    tANI_U8                 sessionId;  //                                         
    tANI_U16                transactionId; //                                            
    tANI_U32                rc;
    tANI_U16                aid;  //                                    
    tANI_U8                 macAddr[6]; //                              
    tSirDeltsReqInfo        rsp;
} tSirDeltsRsp, *tpSirDeltsRsp;

#if defined WLAN_FEATURE_VOWIFI_11R || defined FEATURE_WLAN_CCX || defined(FEATURE_WLAN_LFR)

#define SIR_QOS_NUM_TSPEC_MAX 2
#define SIR_QOS_NUM_AC_MAX 4

typedef struct sSirAggrQosReqInfo
{
    tANI_U16 tspecIdx;
    tSirAddtsReqInfo aggrAddTsInfo[SIR_QOS_NUM_AC_MAX];
}tSirAggrQosReqInfo, *tpSirAggrQosReqInfo;

typedef struct sSirAggrQosReq
{
    tANI_U16                messageType; //                   
    tANI_U16                length;
    tANI_U8                 sessionId;  //          
    tANI_U16                transactionId;
    tSirMacAddr             bssId;      //     
    tANI_U32                timeout; //      
    tANI_U8                 rspReqd;
    tSirAggrQosReqInfo      aggrInfo;
}tSirAggrQosReq, *tpSirAggrQosReq;

typedef struct sSirAggrQosRspInfo
{
    tANI_U16                tspecIdx;
    tSirAddtsRspInfo        aggrRsp[SIR_QOS_NUM_AC_MAX];
} tSirAggrQosRspInfo, *tpSirAggrQosRspInfo;

typedef struct sSirAggrQosRsp
{
    tANI_U16                messageType;
    tANI_U16                length;
    tANI_U8                 sessionId;
    tSirAggrQosRspInfo      aggrInfo;
} tSirAggrQosRsp, *tpSirAggrQosRsp;

#endif/*                                           */

typedef struct sSirSetTxPowerReq
{
    tANI_U16       messageType;
    tANI_U16       length;
    tSirMacAddr    bssId;
    tANI_U8        mwPower;
    tANI_U8        bssIdx;
} tSirSetTxPowerReq, *tpSirSetTxPowerReq;

typedef struct sSirSetTxPowerRsp
{
    tANI_U16            messageType;
    tANI_U16        length;
    tANI_U32        status;
} tSirSetTxPowerRsp, *tpSirSetTxPowerRsp;

typedef struct sSirGetTxPowerReq
{
    tANI_U16    messageType;
    tANI_U16    length;
    tANI_U16    staid;
} tSirGetTxPowerReq, *tpSirGetTxPowerReq;

typedef struct sSirGetTxPowerRsp
{
    tANI_U16            messageType;
    tANI_U16            length; //                             
    tANI_U32            power;  //                    
    tANI_U32            status;
} tSirGetTxPowerRsp, *tpSirGetTxPowerRsp;


typedef tANI_U32 tSirMacNoise[3];

typedef struct sSirGetNoiseRsp 
{
    tANI_U16            messageType;
    tANI_U16            length; 
    tSirMacNoise        noise;
} tSirGetNoiseRsp, *tpSirGetNoiseRsp;


//
//                   
//                                    
//
typedef struct sSirPowerSaveCfg
{
    tANI_U16    listenInterval;
   
    /*                                             
                                                  
                                             
     */
    tANI_U32 HeartBeatCount;

    /*                                            
                                                
                                   
     */
    tANI_U32    nthBeaconFilter;

    /*                                       
                                                 
     */
    tANI_U32    maxPsPoll;                                                 

    /*                                           
                                              
                                      
     */
    tANI_U32    minRssiThreshold;                                       

    /*                                           
                                                       
     */
    tANI_U8     numBeaconPerRssiAverage;                        

    /*                                           
                      
     */
    tANI_U8     rssiFilterPeriod;

    //                                                  
    tANI_U8     broadcastFrameFilter;    

    //                                           
    tANI_U8     ignoreDtim;

    /*                                                
                                                    
                                                 
     */
    tBeaconForwarding beaconFwd;
    tANI_U16 nthBeaconFwd;
    tANI_U8 fEnablePwrSaveImmediately;
    tANI_U8 fPSPoll;

    //                                                    
    tANI_U8     fEnableBeaconEarlyTermination;    
    tANI_U8     bcnEarlyTermWakeInterval;    

}tSirPowerSaveCfg, *tpSirPowerSaveCfg;

/*                                                                    
                                                                   
                                          */
typedef enum eRequestFullPowerReason
{
   eSME_MISSED_BEACON_IND_RCVD,    /*                                     */
   eSME_BMPS_STATUS_IND_RCVD,      /*                                       */
   eSME_BMPS_MODE_DISABLED,        /*                                      */
   eSME_LINK_DISCONNECTED_BY_HDD,  /*                                             */
   eSME_LINK_DISCONNECTED_BY_OTHER,/*                                                 */
   eSME_FULL_PWR_NEEDED_BY_HDD,    /*                                        */
   eSME_FULL_PWR_NEEDED_BY_BAP,    /*                                   */
   eSME_FULL_PWR_NEEDED_BY_CSR,    /*                         */
   eSME_FULL_PWR_NEEDED_BY_QOS,    /*                         */
   eSME_FULL_PWR_NEEDED_BY_CHANNEL_SWITCH, /*                                  */
#ifdef FEATURE_WLAN_TDLS
   eSME_FULL_PWR_NEEDED_BY_TDLS_PEER_SETUP, /*                */
#endif
   eSME_REASON_OTHER               /*                                         */ 
} tRequestFullPowerReason, tExitBmpsReason;



//                                                     
typedef struct sExitBmpsInfo
{
   tExitBmpsReason exitBmpsReason;  /*                        */
}tExitBmpsInfo, *tpExitBmpsInfo;


//               
//                                                  
typedef struct sSirSmeExitBmpsInd
{
    tANI_U16  mesgType;               /*                        */
    tANI_U16  mesgLen;
    tSirResultCodes  statusCode;
    tExitBmpsReason  exitBmpsReason;  /*                        */

} tSirSmeExitBmpsInd, *tpSirSmeExitBmpsInd;


//
//           
//                                         
//                      
//                                         
//
typedef struct sSmeDelBAPeerInd
{
    //             
    tANI_U16 mesgType;

    tSirMacAddr bssId;//      

    //               
    tANI_U16 mesgLen;

    //              
    tANI_U16 staIdx;

    //                                              
    tANI_U8 baTID;

    //                
    //                           
    //                           
    tANI_U8 baDirection;
} tSmeDelBAPeerInd, *tpSmeDelBAPeerInd;

typedef struct sSmeIbssPeerInd
{
    tANI_U16    mesgType;
    tANI_U16    mesgLen;
    tANI_U8     sessionId;

    tSirMacAddr peerAddr;
    tANI_U16    staId;

    /*                                                                          
                                              */
    /*                     */
    tANI_U8            ucastSig;

    /*                       */
    tANI_U8            bcastSig;

    //                                                
}tSmeIbssPeerInd, *tpSmeIbssPeerInd;

typedef struct sSirIbssPeerInactivityInd
{
   tANI_U8       bssIdx;
   tANI_U8       staIdx;
   tSirMacAddr   peerAddr;
}tSirIbssPeerInactivityInd, *tpSirIbssPeerInactivityInd;


typedef struct sLimScanChn
{
    tANI_U16 numTimeScan;   //                                  
    tANI_U8 channelId;
}tLimScanChn;

typedef struct sSmeGetScanChnRsp
{
    //             
    tANI_U16 mesgType;
    //               
    tANI_U16 mesgLen;
    tANI_U8   sessionId;
    tANI_U8 numChn;
    tLimScanChn scanChn[1];
} tSmeGetScanChnRsp, *tpSmeGetScanChnRsp;

typedef struct sLimScanChnInfo
{
    tANI_U8 numChnInfo;     //                             
    tLimScanChn scanChn[SIR_MAX_SUPPORTED_CHANNEL_LIST];
}tLimScanChnInfo;

typedef struct sSirSmeGetAssocSTAsReq
{
    tANI_U16    messageType;    //                            
    tANI_U16    length;    
    tSirMacAddr bssId;          //      
    tANI_U16    modId;
    void        *pUsrContext;
    void        *pSapEventCallback;
    void        *pAssocStasArray;//                                                                   
} tSirSmeGetAssocSTAsReq, *tpSirSmeGetAssocSTAsReq;

typedef struct sSmeMaxAssocInd
{
    tANI_U16    mesgType;    //                            
    tANI_U16    mesgLen;    
    tANI_U8     sessionId;    
    tSirMacAddr peerMac;     //                                                                     
} tSmeMaxAssocInd, *tpSmeMaxAssocInd;

/*                                                                    */
/*                                                                    */
/*                                                                    */

/*                                                                    */
/*                                                                    */
/*                                                                    */
#define SIR_FW_IMAGE_SIZE            146332


#define SIR_BOOT_MODULE_ID           1

#define SIR_BOOT_SETUP_IND           ((SIR_BOOT_MODULE_ID << 8) | 0x11)
#define SIR_BOOT_POST_RESULT_IND     ((SIR_BOOT_MODULE_ID << 8) | 0x12)
#define SIR_BOOT_DNLD_RESULT_IND     ((SIR_BOOT_MODULE_ID << 8) | 0x13)
#define SIR_BOOT_DNLD_DEV_REQ        ((SIR_BOOT_MODULE_ID << 8) | 0x41)
#define SIR_BOOT_DNLD_DEV_RSP        ((SIR_BOOT_MODULE_ID << 8) | 0x81)
#define SIR_BOOT_DNLD_REQ            ((SIR_BOOT_MODULE_ID << 8) | 0x42)
#define SIR_BOOT_DNLD_RSP            ((SIR_BOOT_MODULE_ID << 8) | 0x82)

/*                                                                    */
/*                                                                    */
/*                                                                    */

//               
#define SIR_BOOT_MB_HEADER                 0
#define SIR_BOOT_MB_HEADER2                1

#define SIR_BOOT_MSG_HDR_MASK              0xffff0000
#define SIR_BOOT_MSG_LEN_MASK              0x0000ffff

//                                 
#define SIR_BOOT_SETUP_IND_MBADDR          2
#define SIR_BOOT_SETUP_IND_MBSIZE          3
#define SIR_BOOT_SETUP_IND_MEMOPT          4
#define SIR_BOOT_SETUP_IND_LEN             \
                                      ((SIR_BOOT_SETUP_IND_MEMOPT+1)<<2)

//                                       
#define SIR_BOOT_POST_RESULT_IND_RES       2
#define SIR_BOOT_POST_RESULT_IND_LEN       \
                                  ((SIR_BOOT_POST_RESULT_IND_RES+1)<<2)

#define SIR_BOOT_POST_RESULT_IND_SUCCESS       1
#define SIR_BOOT_POST_RESULT_IND_MB_FAILED     2
#define SIR_BOOT_POST_RESULT_IND_SDRAM_FAILED  3
#define SIR_BOOT_POST_RESULT_IND_ESRAM_FAILED  4


//                                       
#define SIR_BOOT_DNLD_RESULT_IND_RES       2
#define SIR_BOOT_DNLD_RESULT_IND_LEN       \
                                   ((SIR_BOOT_DNLD_RESULT_IND_RES+1)<<2)

#define SIR_BOOT_DNLD_RESULT_IND_SUCCESS   1
#define SIR_BOOT_DNLD_RESULT_IND_HDR_ERR   2
#define SIR_BOOT_DNLD_RESULT_IND_ERR       3

//                  
#define SIR_BOOT_DNLD_DEV_REQ_SDRAMSIZE    2
#define SIR_BOOT_DNLD_DEV_REQ_FLASHSIZE    3
#define SIR_BOOT_DNLD_DEV_REQ_LEN          \
                                 ((SIR_BOOT_DNLD_DEV_REQ_FLASHSIZE+1)<<2)

//                  
#define SIR_BOOT_DNLD_DEV_RSP_DEVTYPE      2
#define SIR_BOOT_DNLD_DEV_RSP_LEN          \
                                   ((SIR_BOOT_DNLD_DEV_RSP_DEVTYPE+1)<<2)

#define SIR_BOOT_DNLD_DEV_RSP_SRAM         1
#define SIR_BOOT_DNLD_DEV_RSP_FLASH        2

//              
#define SIR_BOOT_DNLD_REQ_OFFSET           2
#define SIR_BOOT_DNLD_REQ_WRADDR           3
#define SIR_BOOT_DNLD_REQ_SIZE             4
#define SIR_BOOT_DNLD_REQ_LEN              ((SIR_BOOT_DNLD_REQ_SIZE+1)<<2)

//              
#define SIR_BOOT_DNLD_RSP_SIZE             2
#define SIR_BOOT_DNLD_RSP_LEN              ((SIR_BOOT_DNLD_RSP_SIZE+1)<<2)


//                                            
typedef __ani_attr_pre_packed struct sSirBoardCapabilities
{
#ifndef ANI_LITTLE_BIT_ENDIAN
    tANI_U32 concat:1;        //                                                     
    tANI_U32 compression:1;   //                                                               
    tANI_U32 chnlBonding:1;   //                                                                       
    tANI_U32 reverseFCS:1;    //                                                               
    tANI_U32 rsvd1:2;
    //                                                                 
    tANI_U32 cbFamily:1;      //                               
    tANI_U32 apFamily:1;      //                          
    tANI_U32 mpciFamily:1;    //                              
    tANI_U32 bgOnly:1;        //                                
    tANI_U32 bbChipVer:4;     //                      
    tANI_U32 loType:2;        //                                 
    tANI_U32 radioOn:2;       //                                            
    tANI_U32 nReceivers:2;    //         
    tANI_U32 nTransmitters:1; //                                      
    tANI_U32 sdram:1;         //                        
    tANI_U32 rsvd:1;
    tANI_U32 extVsIntAnt:1;   //                                      
#else

    tANI_U32 extVsIntAnt:1;   //                                      
    tANI_U32 rsvd:1;
    tANI_U32 sdram:1;         //                        
    tANI_U32 nTransmitters:1; //                                      
    tANI_U32 nReceivers:2;    //         
    tANI_U32 radioOn:2;       //                                            
    tANI_U32 loType:2;        //                                 
    tANI_U32 bbChipVer:4;     //                      
    tANI_U32 bgOnly:1;        //                                
    //                                                                 
    tANI_U32 mpciFamily:1;    //                              
    tANI_U32 apFamily:1;      //                          
    tANI_U32 cbFamily:1;      //                               
    tANI_U32 rsvd1:2;
    tANI_U32 reverseFCS:1;    //                                                               
    tANI_U32 chnlBonding:1;   //                                                                       
    tANI_U32 compression:1;   //                                                               
    tANI_U32 concat:1;        //                                                     
#endif
} __ani_attr_packed  tSirBoardCapabilities, *tpSirBoardCapabilities;

# define ANI_BCAP_EXT_VS_INT_ANT_MASK   0x1
# define ANI_BCAP_EXT_VS_INT_ANT_OFFSET 0

# define ANI_BCAP_GAL_ON_BOARD_MASK     0x2
# define ANI_BCAP_GAL_ON_BOARD_OFFSET   1

# define ANI_BCAP_SDRAM_MASK            0x4
# define ANI_BCAP_SDRAM_OFFSET          2

# define ANI_BCAP_NUM_TRANSMITTERS_MASK   0x8
# define ANI_BCAP_NUM_TRANSMITTERS_OFFSET 3

# define ANI_BCAP_NUM_RECEIVERS_MASK    0x30
# define ANI_BCAP_NUM_RECEIVERS_OFFSET  4

# define ANI_BCAP_RADIO_ON_MASK         0xC0
# define ANI_BCAP_RADIO_ON_OFFSET       6

# define ANI_BCAP_LO_TYPE_MASK          0x300
# define ANI_BCAP_LO_TYPE_OFFSET        8

# define ANI_BCAP_BB_CHIP_VER_MASK      0xC00
# define ANI_BCAP_BB_CHIP_VER_OFFSET    10

# define ANI_BCAP_CYG_DATE_CODE_MASK    0xFF000
# define ANI_BCAP_CYG_DATE_CODE_OFFSET  12

# define ANI_BCAP_RADIO_OFF              0
# define ANI_BCAP_RADIO_ON               1
# define ANI_BCAP_RADIO_ON_NOT_SUPPORTED 3


//                        
//                  
#define SIR_WOWL_BCAST_PATTERN_MAX_SIZE 128
#define SIR_WOWL_BCAST_MAX_NUM_PATTERNS 16

//                                                               
//                                                          
//                                      
typedef struct sSirWowlAddBcastPtrn
{
    tANI_U8  ucPatternId;           //           
    //                                                                        
    //                
    tANI_U8  ucPatternByteOffset;   
    tANI_U8  ucPatternSize;         //                      
    tANI_U8  ucPattern[SIR_WOWL_BCAST_PATTERN_MAX_SIZE]; //        
    tANI_U8  ucPatternMaskSize;     //                           
    tANI_U8  ucPatternMask[SIR_WOWL_BCAST_PATTERN_MAX_SIZE]; //             
    //                                    
    tANI_U8  ucPatternExt[SIR_WOWL_BCAST_PATTERN_MAX_SIZE]; //              
    tANI_U8  ucPatternMaskExt[SIR_WOWL_BCAST_PATTERN_MAX_SIZE]; //                   
    tSirMacAddr    bssId;           //      
} tSirWowlAddBcastPtrn, *tpSirWowlAddBcastPtrn;


//                                                                  
//                                                          
//                                      
typedef struct sSirWowlDelBcastPtrn
{
    /*                                                */
    tANI_U8  ucPatternId;
    tSirMacAddr    bssId;           //      
}tSirWowlDelBcastPtrn, *tpSirWowlDelBcastPtrn;


//                                  
typedef struct sSirSmeWowlEnterParams
{
    /*                                         */
    tANI_U8   ucMagicPktEnable; 

    /*               */
    tSirMacAddr magicPtrn;

    /*                                           */
    tANI_U8   ucPatternFilteringEnable; 

#ifdef WLAN_WAKEUP_EVENTS
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
#endif //                   

    tSirMacAddr bssId;
} tSirSmeWowlEnterParams, *tpSirSmeWowlEnterParams;


//                                   
typedef struct sSirHalWowlEnterParams
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

#ifdef WLAN_WAKEUP_EVENTS
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
#endif //                   

    /*                                              
                                     
     */  
    eHalStatus  status;

   /*                                 
      */
    tANI_U8  bssIdx;
} tSirHalWowlEnterParams, *tpSirHalWowlEnterParams;

//                                  
typedef struct sSirHalWowlExitParams
{
    /*                                              
                                    
     */  
    eHalStatus  status;

   /*                                  
      */
    tANI_U8  bssIdx;
} tSirHalWowlExitParams, *tpSirHalWowlExitParams;


#define SIR_MAX_NAME_SIZE 64
#define SIR_MAX_TEXT_SIZE 32

typedef struct sSirName {
    v_U8_t num_name;
    v_U8_t name[SIR_MAX_NAME_SIZE];
} tSirName;

typedef struct sSirText {
    v_U8_t num_text;
    v_U8_t text[SIR_MAX_TEXT_SIZE];
} tSirText;


#define SIR_WPS_PROBRSP_VER_PRESENT    0x00000001
#define SIR_WPS_PROBRSP_STATE_PRESENT    0x00000002
#define SIR_WPS_PROBRSP_APSETUPLOCK_PRESENT    0x00000004
#define SIR_WPS_PROBRSP_SELECTEDREGISTRA_PRESENT    0x00000008
#define SIR_WPS_PROBRSP_DEVICEPASSWORDID_PRESENT    0x00000010
#define SIR_WPS_PROBRSP_SELECTEDREGISTRACFGMETHOD_PRESENT    0x00000020
#define SIR_WPS_PROBRSP_RESPONSETYPE_PRESENT    0x00000040
#define SIR_WPS_PROBRSP_UUIDE_PRESENT    0x00000080
#define SIR_WPS_PROBRSP_MANUFACTURE_PRESENT    0x00000100
#define SIR_WPS_PROBRSP_MODELNAME_PRESENT    0x00000200
#define SIR_WPS_PROBRSP_MODELNUMBER_PRESENT    0x00000400
#define SIR_WPS_PROBRSP_SERIALNUMBER_PRESENT    0x00000800
#define SIR_WPS_PROBRSP_PRIMARYDEVICETYPE_PRESENT    0x00001000
#define SIR_WPS_PROBRSP_DEVICENAME_PRESENT    0x00002000
#define SIR_WPS_PROBRSP_CONFIGMETHODS_PRESENT    0x00004000
#define SIR_WPS_PROBRSP_RF_BANDS_PRESENT    0x00008000


typedef struct sSirWPSProbeRspIE {
   v_U32_t  FieldPresent;
   v_U32_t  Version;           //                                         
   v_U32_t  wpsState;          //                                      
   v_BOOL_t APSetupLocked;     //                                  
   v_BOOL_t SelectedRegistra;  //                                                                                   
   v_U16_t  DevicePasswordID;  //                   
   v_U16_t  SelectedRegistraCfgMethod; //                                 
   v_U8_t   ResponseType;      //              
   v_U8_t   UUID_E[16];         //                             
   tSirName   Manufacture;
   tSirText   ModelName;
   tSirText   ModelNumber;
   tSirText  SerialNumber;
   v_U32_t  PrimaryDeviceCategory ; //                                                  
   v_U8_t   PrimaryDeviceOUI[4] ; //                                            
   v_U32_t  DeviceSubCategory ; //                                                                         
   tSirText DeviceName;
   v_U16_t  ConfigMethod;     //                      
   v_U8_t   RFBand;           //                             
} tSirWPSProbeRspIE;

#define SIR_WPS_BEACON_VER_PRESENT    0x00000001
#define SIR_WPS_BEACON_STATE_PRESENT    0x00000002
#define SIR_WPS_BEACON_APSETUPLOCK_PRESENT    0x00000004
#define SIR_WPS_BEACON_SELECTEDREGISTRA_PRESENT    0x00000008
#define SIR_WPS_BEACON_DEVICEPASSWORDID_PRESENT    0x00000010
#define SIR_WPS_BEACON_SELECTEDREGISTRACFGMETHOD_PRESENT    0x00000020
#define SIR_WPS_BEACON_UUIDE_PRESENT    0x00000080
#define SIR_WPS_BEACON_RF_BANDS_PRESENT    0x00000100

typedef struct sSirWPSBeaconIE {
   v_U32_t  FieldPresent;
   v_U32_t  Version;           //                                         
   v_U32_t  wpsState;          //                                      
   v_BOOL_t APSetupLocked;     //                                  
   v_BOOL_t SelectedRegistra;  //                                                                                   
   v_U16_t  DevicePasswordID;  //                   
   v_U16_t  SelectedRegistraCfgMethod; //                                 
   v_U8_t   UUID_E[16];        //                             
   v_U8_t   RFBand;           //                             
} tSirWPSBeaconIE;

#define SIR_WPS_ASSOCRSP_VER_PRESENT    0x00000001
#define SIR_WPS_ASSOCRSP_RESPONSETYPE_PRESENT    0x00000002

typedef struct sSirWPSAssocRspIE {
   v_U32_t FieldPresent;
   v_U32_t Version;
   v_U8_t ResposeType;
} tSirWPSAssocRspIE;

typedef struct sSirAPWPSIEs {
   tSirWPSProbeRspIE  SirWPSProbeRspIE;    /*                        */
   tSirWPSBeaconIE    SirWPSBeaconIE;      /*                 */
   tSirWPSAssocRspIE  SirWPSAssocRspIE;    /*                         */
} tSirAPWPSIEs, *tpSiriAPWPSIEs;

typedef struct sSirUpdateAPWPSIEsReq
{
    tANI_U16       messageType;     //                            
    tANI_U16       length;
    tANI_U16       transactionId;   //                      
    tSirMacAddr    bssId;           //      
    tANI_U8        sessionId;       //          
    tSirAPWPSIEs   APWPSIEs;
} tSirUpdateAPWPSIEsReq, *tpSirUpdateAPWPSIEsReq;

typedef struct sSirUpdateParams
{
    tANI_U16       messageType;     
    tANI_U16       length;
    tANI_U8        sessionId;      //           
    tANI_U8        ssidHidden;     //          
} tSirUpdateParams, *tpSirUpdateParams;

//               
typedef struct sSirChangeBIParams
{
    tANI_U16       messageType;     
    tANI_U16       length;
    tANI_U16       beaconInterval; //                
    tSirMacAddr    bssId;
    tANI_U8        sessionId;      //           
} tSirChangeBIParams, *tpSirChangeBIParams;

#define SIR_WPS_UUID_LEN 16
#define SIR_WPS_PBC_WALK_TIME   120  //           

typedef struct sSirWPSPBCSession {
    struct sSirWPSPBCSession *next;
    tSirMacAddr              addr;
    tANI_U8                  uuid_e[SIR_WPS_UUID_LEN];
    tANI_TIMESTAMP           timestamp;
} tSirWPSPBCSession;

typedef struct sSirSmeGetWPSPBCSessionsReq
{
    tANI_U16        messageType;    //                                
    tANI_U16        length;
    void            *pUsrContext;
    void            *pSapEventCallback;
    tSirMacAddr     bssId;          //      
    tSirMacAddr     pRemoveMac;      //                                                
}  tSirSmeGetWPSPBCSessionsReq, *tpSirSmeGetWPSPBCSessionsReq;

typedef struct sSirWPSPBCProbeReq
{
    tSirMacAddr        peerMacAddr;
    tANI_U16           probeReqIELen;
    tANI_U8            probeReqIE[512];
} tSirWPSPBCProbeReq, *tpSirWPSPBCProbeReq;

//                                        
typedef struct sSirSmeProbeReqInd
{
    tANI_U16           messageType; //                                
    tANI_U16           length;
    tANI_U8            sessionId;
    tSirMacAddr        bssId;
    tSirWPSPBCProbeReq WPSPBCProbeReq;
} tSirSmeProbeReqInd, *tpSirSmeProbeReqInd;

typedef struct sSirUpdateAPWPARSNIEsReq
{
    tANI_U16       messageType;      //                             
    tANI_U16       length;    
    tANI_U16       transactionId; //                      
    tSirMacAddr    bssId;      //      
    tANI_U8        sessionId;  //              
    tSirRSNie      APWPARSNIEs;
} tSirUpdateAPWPARSNIEsReq, *tpSirUpdateAPWPARSNIEsReq;

#ifdef WLAN_FEATURE_ROAM_SCAN_OFFLOAD
#define SIR_ROAM_MAX_CHANNELS            NUM_RF_CHANNELS
#define SIR_ROAM_SCAN_MAX_PB_REQ_SIZE    450
#define CHANNEL_LIST_STATIC                   1 /*                                      */
#define CHANNEL_LIST_DYNAMIC_INIT             2 /*                                                */
#define CHANNEL_LIST_DYNAMIC_FLUSH            3 /*                                                 */
#define CHANNEL_LIST_DYNAMIC_UPDATE           4 /*                                                  */
#define SIR_ROAM_SCAN_24G_DEFAULT_CH     1
#define SIR_ROAM_SCAN_5G_DEFAULT_CH      36

/*                                                    */
#define LFR_SENSITIVITY_THR_1MBPS             -89
#define LFR_LOOKUP_THR_1MBPS                  -78
#define LFR_SENSITIVITY_THR_2MBPS             -87
#define LFR_LOOKUP_THR_2MBPS                  -78
#define LFR_SENSITIVITY_THR_5_5MBPS           -86
#define LFR_LOOKUP_THR_5_5MBPS                -77
#define LFR_SENSITIVITY_THR_11MBPS            -85
#define LFR_LOOKUP_THR_11MBPS                 -76
#define LFR_SENSITIVITY_THR_6MBPS_2G          -83
#define LFR_LOOKUP_THR_6MBPS_2G               -78
#define LFR_SENSITIVITY_THR_6MBPS_5G          -83
#define LFR_LOOKUP_THR_6MBPS_5G               -78
#define LFR_SENSITIVITY_THR_12MBPS_2G         -83
#define LFR_LOOKUP_THR_12MBPS_2G              -78
#define LFR_SENSITIVITY_THR_12MBPS_5G         -81
#define LFR_LOOKUP_THR_12MBPS_5G              -76
#define LFR_SENSITIVITY_THR_24MBPS_2G         -81
#define LFR_LOOKUP_THR_24MBPS_2G              -76
#define LFR_SENSITIVITY_THR_24MBPS_5G         -79
#define LFR_LOOKUP_THR_24MBPS_5G              -74
#define LFR_SENSITIVITY_THR_DEFAULT             0
#define LFR_LOOKUP_THR_DEFAULT                -78
#endif //                              

//                                                
#define SIR_IPV4_ARP_REPLY_OFFLOAD                  0
#define SIR_IPV6_NEIGHBOR_DISCOVERY_OFFLOAD         1
#define SIR_IPV6_NS_OFFLOAD                         2
#define SIR_OFFLOAD_DISABLE                         0
#define SIR_OFFLOAD_ENABLE                          1
#define SIR_OFFLOAD_BCAST_FILTER_ENABLE             0x2
#define SIR_OFFLOAD_MCAST_FILTER_ENABLE             0x4
#define SIR_OFFLOAD_ARP_AND_BCAST_FILTER_ENABLE     (SIR_OFFLOAD_ENABLE|SIR_OFFLOAD_BCAST_FILTER_ENABLE)
#define SIR_OFFLOAD_NS_AND_MCAST_FILTER_ENABLE      (SIR_OFFLOAD_ENABLE|SIR_OFFLOAD_MCAST_FILTER_ENABLE)

#ifdef WLAN_NS_OFFLOAD
typedef struct sSirNsOffloadReq
{
    tANI_U8 srcIPv6Addr[16];
    tANI_U8 selfIPv6Addr[16];
    //                                                          
    tANI_U8 targetIPv6Addr[SIR_MAC_NUM_TARGET_IPV6_NS_OFFLOAD_NA][16];
    tANI_U8 selfMacAddr[6];
    tANI_U8 srcIPv6AddrValid;
    tANI_U8 targetIPv6AddrValid[SIR_MAC_NUM_TARGET_IPV6_NS_OFFLOAD_NA];
    tANI_U8 slotIdx;
} tSirNsOffloadReq, *tpSirNsOffloadReq;
#endif //               

typedef struct sSirHostOffloadReq
{
    tANI_U8 offloadType;
    tANI_U8 enableOrDisable;
    union
    {
        tANI_U8 hostIpv4Addr [4];
        tANI_U8 hostIpv6Addr [16];
    } params;
#ifdef WLAN_NS_OFFLOAD
    tSirNsOffloadReq nsOffloadInfo;
#endif //               
    tSirMacAddr  bssId;
} tSirHostOffloadReq, *tpSirHostOffloadReq;

/*               */
#define SIR_KEEP_ALIVE_NULL_PKT              1
#define SIR_KEEP_ALIVE_UNSOLICIT_ARP_RSP     2

/*                            */
#define SIR_KEEP_ALIVE_DISABLE   0
#define SIR_KEEP_ALIVE_ENABLE    1

/*                     */
typedef struct sSirKeepAliveReq
{
    v_U8_t          packetType;
    v_U32_t         timePeriod;
    tSirIpv4Addr    hostIpv4Addr; 
    tSirIpv4Addr    destIpv4Addr;
    tSirMacAddr     destMacAddr;
    tSirMacAddr     bssId;
} tSirKeepAliveReq, *tpSirKeepAliveReq;

typedef struct sSirSmeAddStaSelfReq
{
    tANI_U16        mesgType;
    tANI_U16        mesgLen;
    tSirMacAddr     selfMacAddr;
    tVOS_CON_MODE   currDeviceMode;
}tSirSmeAddStaSelfReq, *tpSirSmeAddStaSelfReq;

typedef struct sSirSmeDelStaSelfReq
{
    tANI_U16        mesgType;
    tANI_U16        mesgLen;
    tSirMacAddr     selfMacAddr;
}tSirSmeDelStaSelfReq, *tpSirSmeDelStaSelfReq;

typedef struct sSirSmeAddStaSelfRsp
{
    tANI_U16        mesgType;
    tANI_U16        mesgLen;
    tANI_U16        status;
    tSirMacAddr     selfMacAddr;
}tSirSmeAddStaSelfRsp, *tpSirSmeAddStaSelfRsp;

typedef struct sSirSmeDelStaSelfRsp
{
    tANI_U16        mesgType;
    tANI_U16        mesgLen;
    tANI_U16        status;
    tSirMacAddr     selfMacAddr;
}tSirSmeDelStaSelfRsp, *tpSirSmeDelStaSelfRsp;

/*                           
                                        
                                                      
                                                     */
#define SIR_COEX_IND_DATA_SIZE (4)
#define SIR_COEX_IND_TYPE_DISABLE_HB_MONITOR (0)
#define SIR_COEX_IND_TYPE_ENABLE_HB_MONITOR (1)
#define SIR_COEX_IND_TYPE_SCAN_COMPROMISED (2)
#define SIR_COEX_IND_TYPE_SCAN_NOT_COMPROMISED (3)
#define SIR_COEX_IND_TYPE_DISABLE_AGGREGATION_IN_2p4 (4)
#define SIR_COEX_IND_TYPE_ENABLE_AGGREGATION_IN_2p4 (5)

typedef struct sSirSmeCoexInd
{
    tANI_U16        mesgType;
    tANI_U16        mesgLen;
    tANI_U32        coexIndType;
    tANI_U32        coexIndData[SIR_COEX_IND_DATA_SIZE];
}tSirSmeCoexInd, *tpSirSmeCoexInd;

typedef struct sSirSmeMgmtFrameInd
{
    tANI_U16        mesgType;
    tANI_U16        mesgLen;
    tANI_U32        rxChan;
    tANI_U8        sessionId;
    tANI_U8         frameType;
    tANI_S8         rxRssi;
    tANI_U8  frameBuf[1]; //        
}tSirSmeMgmtFrameInd, *tpSirSmeMgmtFrameInd;

#ifdef WLAN_FEATURE_11W
typedef struct sSirSmeUnprotMgmtFrameInd
{
    tANI_U8         sessionId;
    tANI_U8         frameType;
    tANI_U8         frameLen;
    tANI_U8         frameBuf[1]; //        
}tSirSmeUnprotMgmtFrameInd, *tpSirSmeUnprotMgmtFrameInd;
#endif

#define SIR_IS_FULL_POWER_REASON_DISCONNECTED(eReason) \
    ( ( eSME_LINK_DISCONNECTED_BY_HDD == (eReason) ) || \
      ( eSME_LINK_DISCONNECTED_BY_OTHER == (eReason) ) || \
      (eSME_FULL_PWR_NEEDED_BY_CHANNEL_SWITCH == (eReason)))
#define SIR_IS_FULL_POWER_NEEDED_BY_HDD(eReason) \
    ( ( eSME_LINK_DISCONNECTED_BY_HDD == (eReason) ) || ( eSME_FULL_PWR_NEEDED_BY_HDD == (eReason) ) )

/*                        */
typedef struct sSirNoAParam
{
    tANI_U8 ctWindow:7;
    tANI_U8 OppPS:1;
    tANI_U8 count;
    tANI_U32 duration;
    tANI_U32 interval;
    tANI_U32 singleNoADuration;
    tANI_U8   psSelection;
}tSirNoAParam, *tpSirNoAParam;

typedef struct sSirWlanSuspendParam
{
    tANI_U8 configuredMcstBcstFilterSetting;
}tSirWlanSuspendParam,*tpSirWlanSuspendParam;

typedef struct sSirWlanResumeParam
{
    tANI_U8 configuredMcstBcstFilterSetting;
}tSirWlanResumeParam,*tpSirWlanResumeParam;

typedef struct sSirWlanSetRxpFilters
{
    tANI_U8 configuredMcstBcstFilterSetting;
    tANI_U8 setMcstBcstFilter;
}tSirWlanSetRxpFilters,*tpSirWlanSetRxpFilters;


#ifdef FEATURE_WLAN_SCAN_PNO
//
//             
//

//         
#define SIR_PNO_MAX_NETW_CHANNELS  26
#define SIR_PNO_MAX_NETW_CHANNELS_EX  60
#define SIR_PNO_MAX_SUPP_NETWORKS  16
#define SIR_PNO_MAX_SCAN_TIMERS    10

/*                                                                                    */
#define SIR_PNO_MAX_PB_REQ_SIZE    450 

#define SIR_PNO_24G_DEFAULT_CH     1
#define SIR_PNO_5G_DEFAULT_CH      36

typedef enum
{
   SIR_PNO_MODE_IMMEDIATE,
   SIR_PNO_MODE_ON_SUSPEND,
   SIR_PNO_MODE_ON_RESUME,
   SIR_PNO_MODE_MAX 
} eSirPNOMode;

typedef struct 
{
  tSirMacSSid ssId;
  tANI_U32    authentication; 
  tANI_U32    encryption; 
  tANI_U32    bcastNetwType; 
  tANI_U8     ucChannelCount;
  tANI_U8     aChannels[SIR_PNO_MAX_NETW_CHANNELS_EX];
  tANI_U8     rssiThreshold;
} tSirNetworkType; 

typedef struct 
{
  tANI_U32    uTimerValue; 
  tANI_U32    uTimerRepeat; 
}tSirScanTimer; 

typedef struct
{
  tANI_U8        ucScanTimersCount; 
  tSirScanTimer  aTimerValues[SIR_PNO_MAX_SCAN_TIMERS]; 
} tSirScanTimersType;

/*                    */
typedef void(*PNOReqStatusCb)(void *callbackContext, VOS_STATUS status);


typedef struct sSirPNOScanReq
{
  tANI_U8             enable;
  PNOReqStatusCb      statusCallback;
  void                *callbackContext;
  eSirPNOMode         modePNO;
  tANI_U8             ucNetworksCount; 
  tSirNetworkType     aNetworks[SIR_PNO_MAX_SUPP_NETWORKS];
  tSirScanTimersType  scanTimers;
  
  /*            */
  tANI_U16  us24GProbeTemplateLen; 
  tANI_U8   p24GProbeTemplate[SIR_PNO_MAX_PB_REQ_SIZE];
  tANI_U16  us5GProbeTemplateLen; 
  tANI_U8   p5GProbeTemplate[SIR_PNO_MAX_PB_REQ_SIZE]; 
} tSirPNOScanReq, *tpSirPNOScanReq;

typedef struct sSirSetRSSIFilterReq
{
  tANI_U8     rssiThreshold;
} tSirSetRSSIFilterReq, *tpSirSetRSSIFilterReq;


//                   
typedef struct {
  tANI_U8   b11dEnabled;
  tANI_U8   b11dResolved;
  tANI_U8   ucChannelCount;
  tANI_U8   aChannels[SIR_PNO_MAX_NETW_CHANNELS_EX];
  tANI_U16  usPassiveMinChTime;
  tANI_U16  usPassiveMaxChTime;
  tANI_U16  usActiveMinChTime;
  tANI_U16  usActiveMaxChTime;
  tANI_U8   ucCBState;
} tSirUpdateScanParams, * tpSirUpdateScanParams;

//                                   
typedef struct
{
  tANI_U16      mesgType;
  tANI_U16      mesgLen;
  /*                                             */
  tSirMacSSid   ssId;
  /*                    */
  tANI_U8       rssi;
  /*                                       
                                                 */
  tANI_U32      frameLength;
  /*                                               
                                                   */
  tANI_U8       data[1];
} tSirPrefNetworkFoundInd, *tpSirPrefNetworkFoundInd;
#endif //                     

#ifdef WLAN_FEATURE_ROAM_SCAN_OFFLOAD
typedef struct
{
  tSirMacSSid ssId;
  tANI_U8     currAPbssid[WNI_CFG_BSSID_LEN];
  tANI_U32    authentication;
  tANI_U8     encryption;
  tANI_U8     mcencryption;
  tANI_U8     ChannelCount;
  tANI_U8     ChannelCache[SIR_ROAM_MAX_CHANNELS];

} tSirRoamNetworkType;

typedef struct SirMobilityDomainInfo
{
  tANI_U8 mdiePresent;
  tANI_U16 mobilityDomain;
} tSirMobilityDomainInfo;

typedef struct sSirRoamOffloadScanReq
{
  eAniBoolean RoamScanOffloadEnabled;
  eAniBoolean MAWCEnabled;
  tANI_S8     LookupThreshold;
  tANI_S8     RxSensitivityThreshold;
  tANI_U8     RoamRssiDiff;
  tANI_U8     ChannelCacheType;
  tANI_U8     Command;
  tANI_U8     StartScanReason;
  tANI_U16    NeighborScanTimerPeriod;
  tANI_U16    NeighborRoamScanRefreshPeriod;
  tANI_U16    NeighborScanChannelMinTime;
  tANI_U16    NeighborScanChannelMaxTime;
  tANI_U16    EmptyRefreshScanPeriod;
  tANI_U8     ValidChannelCount;
  tANI_U8     ValidChannelList[SIR_ROAM_MAX_CHANNELS];
  eAniBoolean IsCCXEnabled;
  tANI_U16  us24GProbeTemplateLen;
  tANI_U8   p24GProbeTemplate[SIR_ROAM_SCAN_MAX_PB_REQ_SIZE];
  tANI_U16  us5GProbeTemplateLen;
  tANI_U8   p5GProbeTemplate[SIR_ROAM_SCAN_MAX_PB_REQ_SIZE];
  tANI_U8   nProbes;
  tANI_U16  HomeAwayTime;
  tSirRoamNetworkType ConnectedNetwork;
  tSirMobilityDomainInfo MDID;
} tSirRoamOffloadScanReq, *tpSirRoamOffloadScanReq;
#endif //                              

#define SIR_NOCHANGE_POWER_VALUE  0xFFFFFFFF

//                     
typedef enum
{
   eSIR_IGNORE_DTIM        = 1,
   eSIR_LISTEN_INTERVAL    = 2, 
   eSIR_MCAST_BCAST_FILTER = 3, 
   eSIR_ENABLE_BET         = 4, 
   eSIR_BET_INTERVAL       = 5 
}tPowerParamType;

//                        
typedef struct 
{
  /*              */
  tANI_U32 uIgnoreDTIM;

  /*             */
  tANI_U32 uDTIMPeriod; 

  /*                 */
  tANI_U32 uListenInterval;

  /*                            */
  tANI_U32 uBcastMcastFilter;

  /*                          */
  tANI_U32 uEnableBET;

  /*                                   */
  tANI_U32 uBETInterval; 

  /*                           */
  tANI_U32 uMaxLIModulatedDTIM;

}tSirSetPowerParamsReq, *tpSirSetPowerParamsReq;

typedef struct sSirTxPerTrackingParam
{
    tANI_U8  ucTxPerTrackingEnable;           /*                      */
    tANI_U8  ucTxPerTrackingPeriod;              /*                                                                                                         */
    tANI_U8  ucTxPerTrackingRatio;            /*                                                                                                                                                                                                                                  */
    tANI_U32 uTxPerTrackingWatermark;               /*                                                                                                   */
}tSirTxPerTrackingParam, *tpSirTxPerTrackingParam;

#ifdef WLAN_FEATURE_PACKET_FILTERING
/*                                                                           
                             
                                                                           */
#define    SIR_IPV4_ADDR_LEN                 4
#define    SIR_MAC_ADDR_LEN                  6
#define    SIR_MAX_FILTER_TEST_DATA_LEN       8
#define    SIR_MAX_NUM_MULTICAST_ADDRESS    240
#define    SIR_MAX_NUM_FILTERS               20 
#define    SIR_MAX_NUM_TESTS_PER_FILTER      10 

//
//                          
//
typedef enum
{
  SIR_RCV_FILTER_TYPE_INVALID,
  SIR_RCV_FILTER_TYPE_FILTER_PKT,
  SIR_RCV_FILTER_TYPE_BUFFER_PKT,
  SIR_RCV_FILTER_TYPE_MAX_ENUM_SIZE
}eSirReceivePacketFilterType;

typedef enum 
{
  SIR_FILTER_HDR_TYPE_INVALID,
  SIR_FILTER_HDR_TYPE_MAC,
  SIR_FILTER_HDR_TYPE_ARP,
  SIR_FILTER_HDR_TYPE_IPV4,
  SIR_FILTER_HDR_TYPE_IPV6,
  SIR_FILTER_HDR_TYPE_UDP,
  SIR_FILTER_HDR_TYPE_MAX
}eSirRcvPktFltProtocolType;

typedef enum 
{
  SIR_FILTER_CMP_TYPE_INVALID,
  SIR_FILTER_CMP_TYPE_EQUAL,
  SIR_FILTER_CMP_TYPE_MASK_EQUAL,
  SIR_FILTER_CMP_TYPE_NOT_EQUAL,
  SIR_FILTER_CMP_TYPE_MASK_NOT_EQUAL,
  SIR_FILTER_CMP_TYPE_MAX
}eSirRcvPktFltCmpFlagType;

typedef struct sSirRcvPktFilterFieldParams
{
  eSirRcvPktFltProtocolType        protocolLayer;
  eSirRcvPktFltCmpFlagType         cmpFlag;
  /*                               */
  tANI_U16                         dataLength; 
  /*                                           */
  tANI_U8                          dataOffset; 
  /*                */
  tANI_U8                          reserved; 
  /*                 */
  tANI_U8                          compareData[SIR_MAX_FILTER_TEST_DATA_LEN];
  /*                                                               */
  tANI_U8                          dataMask[SIR_MAX_FILTER_TEST_DATA_LEN];   
}tSirRcvPktFilterFieldParams, *tpSirRcvPktFilterFieldParams;

typedef struct sSirRcvPktFilterCfg
{
  tANI_U8                         filterId; 
  eSirReceivePacketFilterType     filterType;
  tANI_U32                        numFieldParams;
  tANI_U32                        coalesceTime;
  tSirMacAddr                     selfMacAddr;
  tSirMacAddr                     bssId; //                         
  tSirRcvPktFilterFieldParams     paramsData[SIR_MAX_NUM_TESTS_PER_FILTER];
}tSirRcvPktFilterCfgType, *tpSirRcvPktFilterCfgType;

//
//                                     
//
typedef struct sSirRcvFltPktMatchCnt
{
  tANI_U8    filterId;
  tANI_U32   matchCnt;
} tSirRcvFltPktMatchCnt, tpSirRcvFltPktMatchCnt;

typedef struct sSirRcvFltPktMatchRsp
{
  tANI_U16        mesgType;
  tANI_U16        mesgLen;
    
  /*                    */
  tANI_U32                 status;
  tSirRcvFltPktMatchCnt    filterMatchCnt[SIR_MAX_NUM_FILTERS];
  tSirMacAddr      bssId;
} tSirRcvFltPktMatchRsp, *tpSirRcvFltPktMatchRsp;

//
//                                
//
typedef struct sSirRcvFltPktClearParam
{
  tANI_U32   status;  /*                                 */
  tANI_U8    filterId;
  tSirMacAddr selfMacAddr;
  tSirMacAddr bssId;
}tSirRcvFltPktClearParam, *tpSirRcvFltPktClearParam;

//
//                                  
//
typedef struct sSirRcvFltMcAddrList
{
  tANI_U32       ulMulticastAddrCnt;
  tSirMacAddr    multicastAddr[SIR_MAX_NUM_MULTICAST_ADDRESS];
  tSirMacAddr    selfMacAddr;
  tSirMacAddr    bssId;
} tSirRcvFltMcAddrList, *tpSirRcvFltMcAddrList;
#endif //                              

//
//                            
//
typedef struct
{
  tANI_U8    revision;
  tANI_U8    version;
  tANI_U8    minor;
  tANI_U8    major;
} tSirVersionType;

typedef struct sAniBtAmpLogLinkReq
{
    //                                  
    tANI_U16                msgType;    //                                         
    tANI_U16                msgLen;  //                             
    tANI_U8                 sessionId; //              
    void                   *btampHandle; //           
    
} tAniBtAmpLogLinkReq, *tpAniBtAmpLogLinkReq;

#ifdef WLAN_FEATURE_GTK_OFFLOAD
/*                                                                           
                     
                                                                           */
typedef struct
{
  tANI_U32     ulFlags;             /*                */
  tANI_U8      aKCK[16];            /*                      */ 
  tANI_U8      aKEK[16];            /*                    */
  tANI_U64     ullKeyReplayCounter; /*                */
  tSirMacAddr  bssId;
} tSirGtkOffloadParams, *tpSirGtkOffloadParams;

/*                                                                           
                             
                                                                           */
typedef struct
{
   tANI_U16   mesgType;
   tANI_U16   mesgLen;

   tANI_U32   ulStatus;             /*                    */
   tANI_U64   ullKeyReplayCounter;  /*                              */
   tANI_U32   ulTotalRekeyCount;    /*                      */
   tANI_U32   ulGTKRekeyCount;      /*                       */
   tANI_U32   ulIGTKRekeyCount;     /*                        */
   tSirMacAddr bssId;
} tSirGtkOffloadGetInfoRspParams, *tpSirGtkOffloadGetInfoRspParams;
#endif //                         

#ifdef WLAN_WAKEUP_EVENTS
/*                                                                           
                       
                                                                           */
typedef struct
{  
    tANI_U16      mesgType;
    tANI_U16      mesgLen;
    tANI_U32      ulReason;        /*                     */
    tANI_U32      ulReasonArg;     /*                                      */
    tANI_U32      ulStoredDataLen; /*                                                        
                                                                                    
                                                                  */
    tANI_U32      ulActualDataLen; /*                       */
    tANI_U8       aDataStart[1];  /*                                                        
                                                    */ 
} tSirWakeReasonInd, *tpSirWakeReasonInd;
#endif //                   

/*                                                                           
                       
                                                                           */
typedef struct sAniSetTmLevelReq
{
    tANI_U16                tmMode;
    tANI_U16                newTmLevel;
} tAniSetTmLevelReq, *tpAniSetTmLevelReq;

#ifdef FEATURE_WLAN_TDLS
/*                              */
typedef struct sSirTdlsSendMgmtReq
{
    tANI_U16            messageType;   //                                  
    tANI_U16            length;
    tANI_U8             sessionId;     //           
    tANI_U16            transactionId; //                       
    tANI_U8             reqType;
    tANI_U8             dialog;
    tANI_U16            statusCode;
    tANI_U8             responder;
    tSirMacAddr         bssid;         //                                                 
    tSirMacAddr         peerMac;
    tANI_U8             addIe[1];      //                                               
} tSirTdlsSendMgmtReq, *tpSirSmeTdlsSendMgmtReq ;

typedef enum TdlsAddOper
{
    TDLS_OPER_NONE,
    TDLS_OPER_ADD,
    TDLS_OPER_UPDATE
} eTdlsAddOper;

/*                              */
typedef struct sSirTdlsAddStaReq
{
    tANI_U16            messageType;   //                                  
    tANI_U16            length;
    tANI_U8             sessionId;     //           
    tANI_U16            transactionId; //                       
    tSirMacAddr         bssid;         //                                                 
    eTdlsAddOper        tdlsAddOper;
    tSirMacAddr         peerMac;
    tANI_U16            capability;
    tANI_U8             extn_capability[SIR_MAC_MAX_EXTN_CAP];
    tANI_U8             supported_rates_length;
    tANI_U8             supported_rates[SIR_MAC_MAX_SUPP_RATES];
    tANI_U8             htcap_present;
    tSirHTCap           htCap;
    tANI_U8             vhtcap_present;
    tSirVHTCap          vhtCap;
    tANI_U8             uapsd_queues;
    tANI_U8             max_sp;
} tSirTdlsAddStaReq, *tpSirSmeTdlsAddStaReq ;

/*                               */
typedef struct sSirTdlsAddStaRsp
{
    tANI_U16               messageType;
    tANI_U16               length;
    tSirResultCodes        statusCode;
    tSirMacAddr            peerMac;
    tANI_U8                sessionId;     //           
    tANI_U16               staId ;
    tANI_U16               staType ;
    tANI_U8                ucastSig;
    tANI_U8                bcastSig;
    eTdlsAddOper           tdlsAddOper;
} tSirTdlsAddStaRsp ;

/*                              */
typedef struct
{
    tANI_U16            messageType;   //                                 
    tANI_U16            length;
    tANI_U8             sessionId;     //           
    tANI_U16            transactionId; //                       
    tANI_U8             uapsdQueues;   //                                
    tANI_U8             maxSp;         //                                        
    tANI_U8             isBufSta;      //                                     
    tANI_U8             isResponder;   //                     
    tSirMacAddr         bssid;         //                                                 
    tSirMacAddr         peerMac;
}tSirTdlsLinkEstablishReq, *tpSirTdlsLinkEstablishReq;

/*                              */
typedef struct
{
    tANI_U16            messageType;   //                                 
    tANI_U16            length;
    tANI_U8             sessionId;     //           
    tANI_U16            transactionId; //                       
    tSirResultCodes        statusCode;
    tSirMacAddr            peerMac;
}tSirTdlsLinkEstablishReqRsp, *tpSirTdlsLinkEstablishReqRsp;

/*                              */
typedef struct sSirTdlsDelStaReq
{
    tANI_U16            messageType;   //                                  
    tANI_U16            length;
    tANI_U8             sessionId;     //           
    tANI_U16            transactionId; //                       
    tSirMacAddr         bssid;         //                                                 
    tSirMacAddr         peerMac;
} tSirTdlsDelStaReq, *tpSirSmeTdlsDelStaReq ;
/*                               */
typedef struct sSirTdlsDelStaRsp
{
   tANI_U16               messageType;
   tANI_U16               length;
   tANI_U8                sessionId;     //           
   tSirResultCodes        statusCode;
   tSirMacAddr            peerMac;
   tANI_U16               staId;
} tSirTdlsDelStaRsp, *tpSirTdlsDelStaRsp;
/*                                        */
typedef struct sSirTdlsDelStaInd
{
   tANI_U16               messageType;
   tANI_U16               length;
   tANI_U8                sessionId;     //           
   tSirMacAddr            peerMac;
   tANI_U16               staId;
   tANI_U16               reasonCode;
} tSirTdlsDelStaInd, *tpSirTdlsDelStaInd;
typedef struct sSirTdlsDelAllPeerInd
{
   tANI_U16               messageType;
   tANI_U16               length;
   tANI_U8                sessionId;     //           
} tSirTdlsDelAllPeerInd, *tpSirTdlsDelAllPeerInd;
#ifdef FEATURE_WLAN_TDLS_OXYGEN_DISAPPEAR_AP
typedef struct sSirTdlsDisappearAPInd
{
   tANI_U16               messageType;
   tANI_U16               length;
   tANI_U8                sessionId;     //           
   tANI_U16               staId;
   tSirMacAddr            staAddr;
} tSirTdlsDisappearAPInd, *tpSirTdlsDisappearAPInd;
#endif
typedef struct sSirMgmtTxCompletionInd
{
   tANI_U16               messageType;
   tANI_U16               length;
   tANI_U8                sessionId;     //           
   tANI_U32               txCompleteStatus;
} tSirMgmtTxCompletionInd, *tpSirMgmtTxCompletionInd;
#endif /*                   */

#ifdef FEATURE_WLAN_TDLS_INTERNAL
typedef enum tdlsListType
{
    TDLS_DIS_LIST,
    TDLS_SETUP_LIST
}eTdlsListType ;

typedef enum tdlsStates
{
    TDLS_LINK_IDLE_STATE,
    TDLS_LINK_DIS_INIT_STATE,
    TDLS_LINK_DIS_RSP_WAIT_STATE,
    TDLS_DIS_REQ_PROCESS_STATE,
    TDLS_DIS_RSP_SENT_WAIT_STATE,
    TDLS_DIS_RSP_SENT_DONE_STATE,
    TDLS_LINK_DIS_DONE_STATE,
    TDLS_LINK_SETUP_START_STATE,
    TDLS_LINK_SETUP_WAIT_STATE,
    TDLS_LINK_SETUP_RSP_WAIT_STATE,
    TDLS_LINK_SETUP_DONE_STATE,
    TDLS_LINK_TEARDOWN_START_STATE,
    TDLS_LINK_TEARDOWN_DONE_STATE,
    TDLS_LINK_SETUP_STATE
}eSirTdlsStates ;

typedef struct sSirTdlsPeerInfo
{
    tSirMacAddr peerMac;
    tANI_U8     sessionId;
    tANI_U8     dialog ;
    tSirMacCapabilityInfo capabilityInfo ;
    tSirMacRateSet  tdlsPeerSuppRates ;
    tSirMacRateSet  tdlsPeerExtRates ;
    //                                
    tSirMacHTCapabilityInfo tdlsPeerHtCaps ;
    tSirMacHTParametersInfo tdlsPeerHtParams ;
    tSirMacExtendedHTCapabilityInfo tdlsPeerHtExtCaps ;
    tANI_U8  supportedMCSSet[SIZE_OF_SUPPORTED_MCS_SET];

    //                                          
    tSirMacRsnInfo tdlsPeerRsn ;
    tANI_U16 tdlsPeerFtIe ;
    tANI_U16 tdlsPeerTimeoutIntvl ;
    tANI_U16 tdlsPeerSuppChan ;
    tANI_U16 tdlsPeerSuppReguClass ;
    tANI_S8  tdlsPeerRssi ;
    tANI_U16 tdlsPeerState ;
    /*                                        */
    tANI_U8  ExtRatesPresent ;
    tANI_U8  rsnIePresent ;
    tANI_U8  htCapPresent ;
    tANI_U8  delStaNeeded ;

} tSirTdlsPeerInfo, *tpSirSmeTdlsPeerInfo ;

/*                              */
typedef struct sSirTdlsDiscoveryReq
{
    tANI_U16            messageType;   //                                  
    tANI_U16            length;
    tANI_U8             sessionId;     //           
    tANI_U16            transactionId; //                       
    tANI_U8             reqType;
    tANI_U8             dialog;
    tSirMacAddr         bssid;         //                                                 
    tSirMacAddr         peerMac;
} tSirTdlsDisReq, *tpSirSmeTdlsDisReq ;

typedef struct sSirTdlsLinkSetupReq
{
    tANI_U16            messageType;   //                             
    tANI_U16            length;
    tANI_U8             sessionId;     //           
    tANI_U16            transactionId; //                       
    tANI_U8             dialog;
    tSirMacAddr         bssid;         //                                                 
    tSirMacAddr         peerMac;
} tSirTdlsSetupReq, *tpSirSmeTdlsSetupReq ;

typedef struct sSirTdlsTeardownReq
{
    tANI_U16            messageType;   //                           
    tANI_U16            length;
    tANI_U8             sessionId;     //           
    tANI_U16            transactionId; //                       
    tSirMacAddr         bssid;         //                                                 
    tSirMacAddr         peerMac;
} tSirTdlsTeardownReq, *tpSirSmeTdlsTeardownReq ;


/*                                */
typedef struct sSirTdlsDiscoveryRsp
{
    tANI_U16               messageType;
    tANI_U16               length;
    tSirResultCodes        statusCode;
    tANI_U16               numDisSta ;
    tSirTdlsPeerInfo       tdlsDisPeerInfo[0];
} tSirTdlsDisRsp, *tpSirSmeTdlsDiscoveryRsp;

typedef struct sSirTdlsLinkSetupRsp
{
    tANI_U16               messageType;
    tANI_U16               length;
    tSirResultCodes        statusCode;
    tSirMacAddr            peerMac;
} tSirTdlsLinksetupRsp ;

typedef struct sSirTdlsLinkSetupInd
{
    tANI_U16               messageType;
    tANI_U16               length;
    tSirResultCodes        statusCode;
    tSirMacAddr            peerMac;
} tSirTdlsLinkSetupInd ;


typedef struct sSirTdlsTeardownRsp
{
    tANI_U16               messageType;
    tANI_U16               length;
    tSirResultCodes        statusCode;
    tSirMacAddr            peerMac;
} tSirTdlsTeardownRsp ;

typedef struct sSirTdlsPeerInd
{
    tANI_U16               messageType;
    tANI_U16               length;
    tSirMacAddr            peerMac;
    tANI_U8                sessionId;     //           
    tANI_U16               staId ;
    tANI_U16               staType ;
    tANI_U8                ucastSig;
    tANI_U8                bcastSig;
} tSirTdlsPeerInd ;

typedef struct sSirTdlsLinkEstablishInd
{
    tANI_U16               messageType;
    tANI_U16               length;
    tANI_U8                bIsResponder;  /*                                                      */
    tANI_U8                linkIdenOffset;  /*                                                         */
    tANI_U8                ptiBufStatusOffset; /*                                            */
    tANI_U8                ptiTemplateLen;
    tANI_U8                ptiTemplateBuf[64];
    tANI_U8                extCapability[8];
/*                                                     
                                              
*/    
} tSirTdlsLinkEstablishInd, *tpSirTdlsLinkEstablishInd;

typedef struct sSirTdlsLinkTeardownInd
{
   tANI_U16               messageType;
   tANI_U16               length;
   tANI_U16               staId;
} tSirTdlsLinkTeardownInd, *tpSirTdlsLinkTeardownInd;

#endif  /*                            */

typedef struct sSirActiveModeSetBcnFilterReq
{
   tANI_U16               messageType;
   tANI_U16               length;
   tANI_U8                seesionId;
} tSirSetActiveModeSetBncFilterReq, *tpSirSetActiveModeSetBncFilterReq;

//                     
typedef struct sSirResetAPCapsChange
{
    tANI_U16       messageType;
    tANI_U16       length;
    tSirMacAddr    bssId;
} tSirResetAPCapsChange, *tpSirResetAPCapsChange;
//                                                   
typedef struct sSirSmeCandidateFoundInd
{
    tANI_U16            messageType; //                             
    tANI_U16            length;
    tANI_U8             sessionId;  //                   
} tSirSmeCandidateFoundInd, *tpSirSmeCandidateFoundInd;

#ifdef WLAN_FEATURE_11W
typedef struct sSirWlanExcludeUnencryptParam
{
    tANI_BOOLEAN    excludeUnencrypt;
    tSirMacAddr     bssId;
}tSirWlanExcludeUnencryptParam,*tpSirWlanExcludeUnencryptParam;
#endif

typedef struct sAniHandoffReq
{
    //                                  
    tANI_U16  msgType; //                                         
    tANI_U16  msgLen;  //                             
    tANI_U8   sessionId;
    tANI_U8   bssid[WNI_CFG_BSSID_LEN];
    tANI_U8   channel;
} tAniHandoffReq, *tpAniHandoffReq;

typedef struct sSirScanOffloadReq {
    tANI_U8 sessionId;
    tSirMacAddr bssId;
    tANI_U8 numSsid;
    tSirMacSSid ssId[SIR_SCAN_MAX_NUM_SSID];
    tANI_U8 hiddenSsid;
    tSirMacAddr selfMacAddr;
    tSirBssType bssType;
    tANI_U8 dot11mode;
    tSirScanType scanType;
    tANI_U32 minChannelTime;
    tANI_U32 maxChannelTime;
    tANI_BOOLEAN p2pSearch;
    tANI_U16 uIEFieldLen;
    tANI_U16 uIEFieldOffset;
    tSirChannelList channelList;
    /*                             
                            
                                   
                 
                                   
                                        
                                        
                                        
                                        
                                        
                                        
                                        
                                        
                                        
                                
                                  
                                   */
} tSirScanOffloadReq, *tpSirScanOffloadReq;

typedef enum sSirScanEventType {
    SCAN_EVENT_STARTED=0x1,          /*                             */
    SCAN_EVENT_COMPLETED=0x2,        /*                               */
    SCAN_EVENT_BSS_CHANNEL=0x4,      /*                                     */
    SCAN_EVENT_FOREIGN_CHANNEL = 0x8,/*                                        */
    SCAN_EVENT_DEQUEUED=0x10,       /*                           */
    SCAN_EVENT_PREEMPTED=0x20,      /*                                       */
    SCAN_EVENT_START_FAILED=0x40,   /*                   */
    SCAN_EVENT_RESTARTED=0x80,      /*              */
    SCAN_EVENT_MAX=0x8000
} tSirScanEventType;

typedef struct sSirScanOffloadEvent{
    tSirScanEventType event;
    tSirResultCodes reasonCode;
    tANI_U32 chanFreq;
    tANI_U32 requestor;
    tANI_U32 scanId;
} tSirScanOffloadEvent, *tpSirScanOffloadEvent;

typedef struct sSirUpdateChanParam
{
    tANI_U8 chanId;
    tANI_U8 pwr;
    tANI_BOOLEAN dfsSet;
} tSirUpdateChanParam, *tpSirUpdateChanParam;

typedef struct sSirUpdateChan
{
    tANI_U8 numChan;
    tSirUpdateChanParam chanParam[1];
} tSirUpdateChanList, *tpSirUpdateChanList;

#ifdef FEATURE_WLAN_LPHB
#define SIR_LPHB_FILTER_LEN   64

typedef enum
{
   LPHB_SET_EN_PARAMS_INDID,
   LPHB_SET_TCP_PARAMS_INDID,
   LPHB_SET_TCP_PKT_FILTER_INDID,
   LPHB_SET_UDP_PARAMS_INDID,
   LPHB_SET_UDP_PKT_FILTER_INDID,
   LPHB_SET_NETWORK_INFO_INDID,
} LPHBIndType;

typedef struct sSirLPHBEnableStruct
{
   v_U8_t enable;
   v_U8_t item;
   v_U8_t session;
} tSirLPHBEnableStruct;

typedef struct sSirLPHBTcpParamStruct
{
   v_U32_t      srv_ip;
   v_U32_t      dev_ip;
   v_U16_t      src_port;
   v_U16_t      dst_port;
   v_U16_t      timeout;
   v_U8_t       session;
   tSirMacAddr  gateway_mac;
} tSirLPHBTcpParamStruct;

typedef struct sSirLPHBTcpFilterStruct
{
   v_U16_t length;
   v_U8_t  offset;
   v_U8_t  session;
   v_U8_t  filter[SIR_LPHB_FILTER_LEN];
} tSirLPHBTcpFilterStruct;

typedef struct sSirLPHBUdpParamStruct
{
   v_U32_t      srv_ip;
   v_U32_t      dev_ip;
   v_U16_t      src_port;
   v_U16_t      dst_port;
   v_U16_t      interval;
   v_U16_t      timeout;
   v_U8_t       session;
   tSirMacAddr  gateway_mac;
} tSirLPHBUdpParamStruct;

typedef struct sSirLPHBUdpFilterStruct
{
   v_U16_t length;
   v_U8_t  offset;
   v_U8_t  session;
   v_U8_t  filter[SIR_LPHB_FILTER_LEN];
} tSirLPHBUdpFilterStruct;

typedef struct sSirLPHBReq
{
   v_U16_t cmd;
   v_U16_t dummy;
   union
   {
      tSirLPHBEnableStruct     lphbEnableReq;
      tSirLPHBTcpParamStruct   lphbTcpParamReq;
      tSirLPHBTcpFilterStruct  lphbTcpFilterReq;
      tSirLPHBUdpParamStruct   lphbUdpParamReq;
      tSirLPHBUdpFilterStruct  lphbUdpFilterReq;
   } params;
} tSirLPHBReq;

typedef struct sSirLPHBTimeoutInd
{
   v_U8_t sessionIdx;
   v_U8_t protocolType; /*          */
   v_U8_t eventReason;
} tSirLPHBTimeoutInd;
#endif /*                   */

typedef struct sSirAddPeriodicTxPtrn
{
    /*                             */
    tSirMacAddr macAddress;

    tANI_U8  ucPtrnId;           //           
    tANI_U16 ucPtrnSize;         //             
    tANI_U32 usPtrnIntervalMs;   //        
    tANI_U8  ucPattern[PERIODIC_TX_PTRN_MAX_SIZE]; //               
} tSirAddPeriodicTxPtrn, *tpSirAddPeriodicTxPtrn;

typedef struct sSirDelPeriodicTxPtrn
{
    /*                             */
    tSirMacAddr macAddress;

    /*                                               */
    tANI_U32 ucPatternIdBitmap;
} tSirDelPeriodicTxPtrn, *tpSirDelPeriodicTxPtrn;

#ifdef FEATURE_WLAN_BATCH_SCAN
//                                
typedef struct
{
  /*                                          */
  tANI_U32 nScansToBatch;
} tSirSetBatchScanRsp, *tpSirSetBatchScanRsp;

//                             
typedef struct
{
    tANI_U32 scanFrequency;        /*                                        */
    tANI_U32 numberOfScansToBatch; /*                          */
    tANI_U32 bestNetwork;          /*                                */
    tANI_U8  rfBand;               /*               
                                                                   
                                                        */
    tANI_U32 rtt;                  /*                                    
                                                                   */
} tSirSetBatchScanReq, *tpSirSetBatchScanReq;


//                              
typedef struct
{
    tANI_U32 param;
} tSirStopBatchScanInd, *tpSirStopBatchScanInd;

//                                           
typedef struct
{
    tANI_U32 param;
} tSirTriggerBatchScanResultInd, *tpSirTriggerBatchScanResultInd;

//                                     
typedef PACKED_PRE struct PACKED_POST
{
    tANI_U8   bssid[6];     /*       */
    tANI_U8   ssid[33];     /*      */
    tANI_U8   ch;           /*         */
    tANI_U8   rssi;         /*               */
    /*                                                                          
                            */
    tANI_U32  timestamp;
} tSirBatchScanNetworkInfo, *tpSirBatchScanNetworkInfo;

typedef PACKED_PRE struct PACKED_POST
{
    tANI_U32   scanId; /*               */
    /*                                                                       */
    tANI_U32   numNetworksInScanList;
    /*                                            */
    /*                                                                       
                                                                 */
    tANI_U8    scanList[1];
} tSirBatchScanList, *tpSirBatchScanList;

typedef PACKED_PRE struct PACKED_POST
{
    tANI_U32      timestamp;
    tANI_U32      numScanLists;
    boolean       isLastResult;
    /*                                        */
    /*                                                         
                                                  */
    tANI_U8       scanResults[1];
}  tSirBatchScanResultIndParam, *tpSirBatchScanResultIndParam;

#endif //                        

#ifdef FEATURE_WLAN_CH_AVOID
#define SIR_CH_AVOID_MAX_RANGE   4

typedef struct sSirChAvoidFreqType
{
   tANI_U32 startFreq;
   tANI_U32 endFreq;
} tSirChAvoidFreqType;

typedef struct sSirChAvoidIndType
{
   tANI_U32            avoidRangeCount;
   tSirChAvoidFreqType avoidFreqRange[SIR_CH_AVOID_MAX_RANGE];
} tSirChAvoidIndType;
#endif /*                       */

#endif /*             */

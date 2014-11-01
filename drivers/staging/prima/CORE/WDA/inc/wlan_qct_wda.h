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

#ifndef WLAN_QCT_WDA_H
#define WLAN_QCT_WDA_H

/*                                                                           

                                                      
                                             


           
                                                                                    
             

                                                                               
                                                                                          
                                                                                      
                               
  
                                                                           */


/*                                                                           

                                           


                                                                       
                                                                


                                   


                                         
                                                                      
                                                             
                                                      
                                                               
                                                                          
                                                     
                                                                            */

#include "aniGlobal.h"


#  include "wlan_qct_wdi_ds.h"


/*             */

typedef enum
{
   WDA_INIT_STATE,
   WDA_START_STATE,
   WDA_READY_STATE,
   WDA_PRE_ASSOC_STATE,
   WDA_BA_UPDATE_TL_STATE,
   WDA_BA_UPDATE_LIM_STATE,
   WDA_STOP_STATE,
   WDA_CLOSE_STATE
}WDA_state;

typedef enum
{
   WDA_PROCESS_SET_LINK_STATE,
   WDA_IGNORE_SET_LINK_STATE
}WDA_processSetLinkStateStatus;

typedef enum
{
   WDA_DISABLE_BA,
   WDA_ENABLE_BA
}WDA_BaEnableFlags;

typedef enum
{
   WDA_INVALID_STA_INDEX,
   WDA_VALID_STA_INDEX
}WDA_ValidStaIndex;
typedef enum
{
  eWDA_AUTH_TYPE_NONE,    //          
  //                               
  eWDA_AUTH_TYPE_OPEN_SYSTEM,
  //                                 
  eWDA_AUTH_TYPE_WPA,
  eWDA_AUTH_TYPE_WPA_PSK,

  eWDA_AUTH_TYPE_RSN,
  eWDA_AUTH_TYPE_RSN_PSK,
  eWDA_AUTH_TYPE_FT_RSN,
  eWDA_AUTH_TYPE_FT_RSN_PSK,
  eWDA_AUTH_TYPE_WAPI_WAI_CERTIFICATE,
  eWDA_AUTH_TYPE_WAPI_WAI_PSK,
  eWDA_AUTH_TYPE_CCKM_WPA,
  eWDA_AUTH_TYPE_CCKM_RSN,
  eWDA_AUTH_TYPE_WPA_NONE,
  eWDA_AUTH_TYPE_AUTOSWITCH,
  eWDA_AUTH_TYPE_SHARED_KEY,
  eWDA_NUM_OF_SUPPORT_AUTH_TYPE,
  eWDA_AUTH_TYPE_FAILED = 0xff,
  eWDA_AUTH_TYPE_UNKNOWN = eCSR_AUTH_TYPE_FAILED,
}WDA_AuthType;

/*                                                                          
           
                                                                           */

#define WDA_TLI_CEIL( _a, _b)  (( 0 != (_a)%(_b))? (_a)/(_b) + 1: (_a)/(_b))

/*
                                                                       
 */
#define IS_MCC_SUPPORTED (WDA_IsWcnssWlanReportedVersionGreaterThanOrEqual( 0, 1, 1, 0))
#define IS_FEATURE_SUPPORTED_BY_FW(featEnumValue) (!!WDA_getFwWlanFeatCaps(featEnumValue))
#define IS_FEATURE_SUPPORTED_BY_DRIVER(featEnumValue) (!!WDA_getHostWlanFeatCaps(featEnumValue))

#ifdef WLAN_ACTIVEMODE_OFFLOAD_FEATURE
#define IS_ACTIVEMODE_OFFLOAD_FEATURE_ENABLE ((WDA_getFwWlanFeatCaps(WLANACTIVE_OFFLOAD)) & (WDI_getHostWlanFeatCaps(WLANACTIVE_OFFLOAD)))
#else
#define IS_ACTIVEMODE_OFFLOAD_FEATURE_ENABLE 0
#endif

#ifdef WLAN_FEATURE_ROAM_SCAN_OFFLOAD
#define IS_ROAM_SCAN_OFFLOAD_FEATURE_ENABLE ((WDI_getHostWlanFeatCaps(WLAN_ROAM_SCAN_OFFLOAD)) & (WDA_getFwWlanFeatCaps(WLAN_ROAM_SCAN_OFFLOAD)))
#else
#define IS_ROAM_SCAN_OFFLOAD_FEATURE_ENABLE 0
#endif

/*                                       */
#define IS_IBSS_HEARTBEAT_OFFLOAD_FEATURE_ENABLE ((WDI_getHostWlanFeatCaps(IBSS_HEARTBEAT_OFFLOAD)) & (WDA_getFwWlanFeatCaps(IBSS_HEARTBEAT_OFFLOAD)))

#ifdef FEATURE_WLAN_TDLS
#define IS_ADVANCE_TDLS_ENABLE ((WDI_getHostWlanFeatCaps(ADVANCE_TDLS)) & (WDA_getFwWlanFeatCaps(ADVANCE_TDLS)))
#else
#define IS_ADVANCE_TDLS_ENABLE 0
#endif
typedef enum {
    MODE_11A        = 0,   /*          */
    MODE_11G        = 1,   /*            */
    MODE_11B        = 2,   /*          */
    MODE_11GONLY    = 3,   /*               */
    MODE_11NA_HT20   = 4,  /*               */
    MODE_11NG_HT20   = 5,  /*               */
    MODE_11NA_HT40   = 6,  /*               */
    MODE_11NG_HT40   = 7,  /*               */
    MODE_11AC_VHT20 = 8,
    MODE_11AC_VHT40 = 9,
    MODE_11AC_VHT80 = 10,
//                          
    MODE_11AC_VHT20_2G = 11,
    MODE_11AC_VHT40_2G = 12,
    MODE_11AC_VHT80_2G = 13,
    MODE_UNKNOWN    = 14,
    MODE_MAX        = 14
} WLAN_PHY_MODE;

#define WLAN_HAL_CHAN_FLAG_HT40_PLUS   6
#define WLAN_HAL_CHAN_FLAG_PASSIVE     7
#define WLAN_HAL_CHAN_ADHOC_ALLOWED    8
#define WLAN_HAL_CHAN_AP_DISABLED      9
#define WLAN_HAL_CHAN_FLAG_DFS         10
#define WLAN_HAL_CHAN_FLAG_ALLOW_HT    11  /*                               */
#define WLAN_HAL_CHAN_FLAG_ALLOW_VHT   12  /*                                */

#define WDA_SET_CHANNEL_FLAG(pwda_channel,flag) do { \
        (pwda_channel)->channel_info |=  (1 << flag);      \
     } while(0)

#define WDA_SET_CHANNEL_MODE(pwda_channel,val) do { \
     (pwda_channel)->channel_info &= 0xffffffc0;            \
     (pwda_channel)->channel_info |= (val);                 \
     } while(0)

#define WDA_SET_CHANNEL_MAX_POWER(pwda_channel,val) do { \
     (pwda_channel)->reg_info_1 &= 0xffff00ff;           \
     (pwda_channel)->reg_info_1 |= ((val&0xff) << 8);    \
     } while(0)

#define WDA_SET_CHANNEL_REG_POWER(pwda_channel,val) do { \
     (pwda_channel)->reg_info_1 &= 0xff00ffff;           \
     (pwda_channel)->reg_info_1 |= ((val&0xff) << 16);   \
     } while(0)
#define WDA_SET_CHANNEL_MIN_POWER(pwlan_hal_update_channel,val) do { \
     (pwlan_hal_update_channel)->reg_info_1 &= 0xffffff00;           \
     (pwlan_hal_update_channel)->reg_info_1 |= (val&0xff);           \
     } while(0)
#define WDA_SET_CHANNEL_ANTENNA_MAX(pwlan_hal_update_channel,val) do { \
     (pwlan_hal_update_channel)->reg_info_2 &= 0xffffff00;             \
     (pwlan_hal_update_channel)->reg_info_2 |= (val&0xff);             \
     } while(0)
#define WDA_SET_CHANNEL_REG_CLASSID(pwlan_hal_update_channel,val) do { \
     (pwlan_hal_update_channel)->reg_info_1 &= 0x00ffffff;             \
     (pwlan_hal_update_channel)->reg_info_1 |= ((val&0xff) << 24);     \
     } while(0)

/*                                                                          
                                
                                                                           */

/*                   */
#define WDA_TLI_MGMT_FRAME_TYPE       0x00
#define WDA_TLI_CTRL_FRAME_TYPE       0x10
#define WDA_TLI_DATA_FRAME_TYPE       0x20

/*                        */
#define  WDA_TLI_802_3_HEADER_LEN             14
/*                                                             */
#define  WDA_TLI_802_11_HEADER_LEN            24

/*                                                        */
#define WDA_TLI_MAC_HEADER_LEN( _dxtl)                \
      ( ( 0 == _dxtl )?                               \
         WDA_TLI_802_3_HEADER_LEN:WDA_TLI_802_11_HEADER_LEN )

/*                      
      
                                                                          
                                                                           
                                                                   
                                               
*/
typedef enum
{
   WDA_TXFLOW_AC_BK = 0,
   WDA_TXFLOW_AC_BE = 1,
   WDA_TXFLOW_AC_VI = 2,
   WDA_TXFLOW_AC_VO = 3,
   WDA_TXFLOW_MGMT  = 4,
   WDA_TXFLOW_BAP   = 1, /*                   */
   WDA_TXFLOW_FC    = 1, /*                   */
   WDA_TXFLOW_MAX
} WDA_TXFlowEnumType;

#define WDA_TXFLOWMASK  0x1F /*                                     */

/*                                                                      
                             

                                                                      
                                    
                                                                       */

/*                                                                    
                                              */
#define WDA_DXE_HEADER_SIZE   0


/*                                    */

/*        */
#define WDA_WLAN_LIBRA_HEADER_LEN              (20 + 8)

#define WDA_TLI_BD_PDU_RESERVE_THRESHOLD    10


#  define WDA_TLI_MIN_RES_MF   1
#  define WDA_TLI_MIN_RES_BAP  2
#  define WDA_TLI_MIN_RES_DATA 3

#  define WDA_NUM_STA 8

/*                                      
 
                                                                      
                          

                                             
                                              
                                                         
                                                                    

                                                                       
                                                                
 */

#ifdef WINDOWS_DT
#define WDA_TLI_PROCESS_FRAME_LEN( _vosBuff, _usPktLen,              \
                                            _uResLen, _uTotalPktLen) \
  do                                                                 \
  {                                                                  \
    _usPktLen = wpalPacketGetFragCount((wpt_packet*)_vosBuff) + 1/*  */;\
    _uResLen  = _usPktLen;                                           \
    _uTotalPktLen = _usPktLen;                                       \
  }                                                                  \
  while ( 0 )
#else /*            */
#define WDA_TLI_PROCESS_FRAME_LEN( _vosBuff, _usPktLen,              \
                                            _uResLen, _uTotalPktLen) \
  do                                                                 \
  {                                                                  \
    _usPktLen = 2;  /*                                        */     \
    _uResLen  = 2;  /*                                          */   \
    _uTotalPktLen = _usPktLen;                                       \
  }                                                                  \
  while ( 0 )
#endif /*            */



/*                                                                          
                     
                                                                           */

/*                                                                   
                                  
*/
#define WDA_DS_TX_START_XMIT  WLANTL_TX_START_XMIT
#define WDA_DS_FINISH_ULA     WLANTL_FINISH_ULA

#define VOS_TO_WPAL_PKT(_vos_pkt) ((wpt_packet*)_vos_pkt)

#define WDA_TX_PACKET_FREED      0X0

/*                                                                                    */
#define WDA_STOP_TIMEOUT ( (WDI_RESPONSE_TIMEOUT * 2) + WDI_SET_POWER_STATE_TIMEOUT + 5)
/*                                                                          
           
                                                                           */

/*                 */
typedef VOS_STATUS (*WDA_DS_TxCompleteCallback) ( v_PVOID_t pContext, vos_pkt_t *pFrameDataBuff, VOS_STATUS txStatus );
typedef VOS_STATUS (*WDA_DS_RxPacketCallback)   ( v_PVOID_t pContext, vos_pkt_t *pFrameDataBuff );
typedef v_U32_t   (*WDA_DS_TxPacketCallback)   ( v_PVOID_t pContext, 
                                                  vos_pkt_t **ppFrameDataBuff, 
                                                  v_U32_t uSize, 
                                                  v_U8_t uFlowMask, 
                                                  v_BOOL_t *pbUrgent );
typedef VOS_STATUS (*WDA_DS_ResourceCB)      ( v_PVOID_t pContext, v_U32_t uCount );


/*                       */
typedef VOS_STATUS (*WDA_DS_TxCompleteCb)( v_PVOID_t     pContext, wpt_packet *pFrame );
typedef VOS_STATUS (*WDA_DS_RxCompleteCb)( v_PVOID_t pContext, wpt_packet *pFrame );
typedef VOS_STATUS (*WDA_DS_TxFlowControlCb)( v_PVOID_t pContext, v_U8_t acMask );
typedef void (*pWDATxRxCompFunc)( v_PVOID_t pContext, void *pData );

//                                 
//                                 
//                                                          
typedef eHalStatus (*pWDAAckFnTxComp)(tpAniSirGlobal, tANI_U32);

typedef struct
{
   tANI_U16 ucValidStaIndex ;
   /* 
                                                                            
                                         
    */
   tANI_U8    ucUseBaBitmap ;
   tANI_U8    bssIdx;
   tANI_U32   framesTxed[STACFG_MAX_TC];
}tWdaStaInfo, *tpWdaStaInfo ;

/*                                              */
typedef struct
{
   /*                         */
   TX_TIMER baActivityChkTmr ;

   /*                           */
   TX_TIMER TxCompleteTimer ;

   /*                     */
   TX_TIMER trafficStatsTimer ;
}tWdaTimers ;
#ifdef WLAN_SOFTAP_VSTA_FEATURE
#define WDA_MAX_STA    (41)
#else
#define WDA_MAX_STA    (16)
#endif
typedef struct
{
   v_PVOID_t            pVosContext;             /*                    */
   v_PVOID_t            pWdiContext;             /*             */
   WDA_state            wdaState ;               /*                    */ 
   v_PVOID_t            wdaWdiCfgApiMsgParam ;   /*                           */
   vos_event_t          wdaWdiEvent;             /*                    */

   /*                                 */
   vos_event_t          txFrameEvent;

   /*                                   */
   pWDATxRxCompFunc     pTxCbFunc;
   /*                                      */
   pWDAAckFnTxComp      pAckTxCbFunc;   
   tANI_U32             frameTransRequired;
   tSirMacAddr          macBSSID;             /*                     */
   tSirMacAddr          macSTASelf;     /*            */

   /*                                  */
   v_U8_t               uTxFlowMask;
   /*                                  */
   WDA_DS_ResourceCB    pfnTxResourceCB;
   /*                               */
   WDA_DS_TxCompleteCallback pfnTxCompleteCallback; 
   
   tWdaStaInfo          wdaStaInfo[WDA_MAX_STA];

   tANI_U8              wdaMaxSta;
   tWdaTimers           wdaTimers;

   /*                              */
   tBssSystemRole       wdaGlobalSystemRole; 

   /*                                */
   tDriverType          driverMode;

   /*                              */
   v_PVOID_t            wdaFTMCmdReq;

   /*                                  */
   vos_event_t          suspendDataTxEvent;
   /*                                       */
   tANI_U8    txStatus;
   /*                                           */
   tANI_U8    txSuspendTimedOut;   

   vos_event_t          waitOnWdiIndicationCallBack;

   /*                     */
   tSirVersionType      wcnssWlanCompiledVersion;
   tSirVersionType      wcnssWlanReportedVersion;
   tSirVersionString    wcnssSoftwareVersionString;
   tSirVersionString    wcnssHardwareVersionString;

   
   tSirLinkState        linkState;
   /*                                      */
   v_BOOL_t             wdaAmpSessionOn;
   v_U32_t              VosPacketToFree;
   v_BOOL_t             needShutdown;
   v_BOOL_t             wdiFailed;
   v_BOOL_t             wdaTimersCreated;

   /*                                        */
   vos_event_t          ftmStopDoneEvent;
} tWDA_CbContext ; 

typedef struct
{
   v_PVOID_t            pWdaContext;             /*                       */
   v_PVOID_t            wdaMsgParam;            /*                       */
   v_PVOID_t            wdaWdiApiMsgParam;      /*                           */
} tWDA_ReqParams; 

/*
                     
                   
 */ 

VOS_STATUS WDA_open(v_PVOID_t pVosContext, v_PVOID_t pOSContext, 
                                              tMacOpenParameters *pMacParams ) ;

/*
                         
                                        
 */ 
VOS_STATUS WDA_start(v_PVOID_t pVosContext) ;

VOS_STATUS WDA_NVDownload_Start(v_PVOID_t pVosContext);

/*
                         
                                     
 */ 
VOS_STATUS WDA_preStart(v_PVOID_t pVosContext) ;
/*
                     
           
 */
VOS_STATUS WDA_stop(v_PVOID_t pVosContext,tANI_U8 reason);

/*
                      
                    
 */
VOS_STATUS WDA_close(v_PVOID_t pVosContext);
/*
                         
                                                              
 */
VOS_STATUS WDA_shutdown(v_PVOID_t pVosContext, wpt_boolean closeTransport);

/*
                                
                                            
 */
void WDA_setNeedShutdown(v_PVOID_t pVosContext);
/*
                             
                                              
 */
v_BOOL_t WDA_needShutdown(v_PVOID_t pVosContext);

/*
                             
                                            
 */ 

VOS_STATUS WDA_McProcessMsg( v_CONTEXT_t pVosContext, vos_msg_t *pMsg ) ;

/*                                                                  
                      
                                                                   */
/*
                               
                          
 */ 

VOS_STATUS WDA_MgmtDSRegister(tWDA_CbContext *pWDA, 
                              WDA_DS_TxCompleteCb WDA_TxCompleteCallback,
                              WDA_DS_RxCompleteCb WDA_RxCompleteCallback,  
                              WDA_DS_TxFlowControlCb WDA_TxFlowCtrlCallback 
                             ) ;
/*
                               
                                     
 */ 

VOS_STATUS WDA_TxPacket(tWDA_CbContext *pWDA, 
                                    void *pFrmBuf,
                                    tANI_U16 frmLen,
                                    eFrameType frmType,
                                    eFrameTxDir txDir,
                                    tANI_U8 tid,
                                    pWDATxRxCompFunc pCompFunc,
                                    void *pData,
                                    pWDAAckFnTxComp pAckTxComp, 
                                    tANI_U8 txFlag);

/*
                           
                                    
 */
VOS_STATUS WDA_PostMsgApi(tpAniSirGlobal pMac, tSirMsgQ *pMsg) ;

/*                                                          
                                      
  
                                   
                                                            
 */
tBssSystemRole wdaGetGlobalSystemRole(tpAniSirGlobal pMac);

/*                                                                            
                                    */
#define WDA_WDI_COMPLETION_TIME_OUT 30000 /*       */

#define WDA_TL_TX_FRAME_TIMEOUT  10000  /*                                               */
#define WDA_TL_SUSPEND_TIMEOUT   2000  /*            */

/*                                         */
#define WDA_TL_TX_MGMT_TIMED_OUT   0xDEAD 

#define WDA_TL_TX_SUSPEND_SUCCESS   0
#define WDA_TL_TX_SUSPEND_FAILURE   1

//                                            
//                                                                            

#define DPU_FEEDBACK_UNPROTECTED_ERROR 0x0F


/*                                                                            
 
                                         
                                             

                                                            

                                                                             */

/*                                                                           */
#  define WDA_GET_RX_MAC_HEADER(pRxMeta)  \
      (tpSirMacMgmtHdr)( ((WDI_DS_RxMetaInfoType *)(pRxMeta))->mpduHeaderPtr )

/*                                                                            */
#  define WDA_GET_RX_MPDUHEADER3A(pRxMeta) \
   (tpSirMacDataHdr3a)( ((WDI_DS_RxMetaInfoType *)(pRxMeta))->mpduHeaderPtr )

/*                                                                            */
#  define WDA_GET_RX_MPDU_HEADER_LEN(pRxMeta)   \
                    ( ((WDI_DS_RxMetaInfoType *)(pRxMeta))->mpduHeaderLength )

/*                                                                            */
#  define WDA_GET_RX_MPDU_LEN(pRxMeta)  \
               ( ((WDI_DS_RxMetaInfoType *)(pRxMeta))->mpduLength )

/*                                                                           */
#  define WDA_GET_RX_PAYLOAD_LEN(pRxMeta)   \
       ( WDA_GET_RX_MPDU_LEN(pRxMeta) - WDA_GET_RX_MPDU_HEADER_LEN(pRxMeta) )

/*                                                                           */
#  define WDA_GET_RX_MAC_RATE_IDX(pRxMeta)  \
                          ( ((WDI_DS_RxMetaInfoType *)(pRxMeta))->rateIndex )

/*                                                                           */
#  define WDA_GET_RX_MPDU_DATA(pRxMeta)  \
                   ( ((WDI_DS_RxMetaInfoType *)(pRxMeta))->mpduDataPtr )

/*                                                                           */
//                                                                              
//                                                          
#  define WDA_GET_RX_MPDU_DATA_OFFSET(pRxMeta)  WDA_GET_RX_MPDU_HEADER_LEN(pRxMeta)

/*                                                                           */
//                                                
//                                                          
#  define WDA_GET_RX_MPDU_HEADER_OFFSET(pRxMeta)   0

/*                                                                           */
#  define WDA_GET_RX_UNKNOWN_UCAST(pRxMeta)   \
                     ( ((WDI_DS_RxMetaInfoType *)(pRxMeta))->unknownUcastPkt )

/*                                                                           */
#  define WDA_GET_RX_TID(pRxMeta) ( ((WDI_DS_RxMetaInfoType *)(pRxMeta))->tid )

/*                                                                           */
#  define WDA_GET_RX_STAID(pRxMeta) (((WDI_DS_RxMetaInfoType*)(pRxMeta))->staId)

/*                                                                           */
#  define WDA_GET_RX_ADDR3_IDX(pRxMeta) (((WDI_DS_RxMetaInfoType*)(pRxMeta))->addr3Idx)

/*                                                                           */
#  define WDA_GET_RX_CH(pRxMeta) (((WDI_DS_RxMetaInfoType*)(pRxMeta))->rxChannel)

/*                                                                           */
#  define WDA_GET_RX_RFBAND(pRxMeta) (((WDI_DS_RxMetaInfoType*)(pRxMeta))->rfBand)

/*                                                                           */
#  define WDA_GET_RX_DPUSIG(pRxMeta)  (((WDI_DS_RxMetaInfoType*)(pRxMeta))->dpuSig)

/*                                                                           */
#  define WDA_IS_RX_BCAST(pRxMeta)   \
      ( (1 == ((WDI_DS_RxMetaInfoType*)(pRxMeta))->bcast) ? VOS_TRUE : VOS_FALSE )
    
/*                                                                           */
#  define WDA_GET_RX_FT_DONE(pRxMeta) (((WDI_DS_RxMetaInfoType*)(pRxMeta))->ft)

/*                                                                          */
#  define WDA_GET_RX_DPU_FEEDBACK(pRxMeta) \
                      (((WDI_DS_RxMetaInfoType*)(pRxMeta))->dpuFeedback)

/*                                                                           */
#  define WDA_GET_RX_ASF(pRxMeta) (((WDI_DS_RxMetaInfoType*)(pRxMeta))->amsdu_asf)

/*                                                                           */
#  define WDA_GET_RX_AEF(pRxMeta) (((WDI_DS_RxMetaInfoType*)(pRxMeta))->amsdu_aef)

/*                                                                           */
#  define WDA_GET_RX_ESF(pRxMeta)  (((WDI_DS_RxMetaInfoType*)(pRxMeta))->amsdu_esf)

/*                                                                           */
#  define WDA_GET_RX_BEACON_SENT(pRxMeta) \
                     (((WDI_DS_RxMetaInfoType*)(pRxMeta))->bsf)

/*                                                                          */
#  define WDA_GET_RX_TSF_LATER(pRxMeta) (((WDI_DS_RxMetaInfoType*)(pRxMeta))->rtsf)

/*                                                                           */
#  define WDA_GET_RX_TYPE(pRxMeta) (((WDI_DS_RxMetaInfoType*)(pRxMeta))->type)

/*                                                                           */
#  define WDA_GET_RX_SUBTYPE(pRxMeta) (((WDI_DS_RxMetaInfoType*)(pRxMeta))->subtype)

/*                                                                            */
#  define WDA_GET_RX_TYPE_SUBTYPE(pRxMeta)  \
                 ((WDA_GET_RX_TYPE(pRxMeta)<<4)|WDA_GET_RX_SUBTYPE(pRxMeta))

/*                                                                            */
#  define WDA_GET_RX_REORDER_OPCODE(pRxMeta) \
           (((WDI_DS_RxMetaInfoType*)(pRxMeta))->ampdu_reorderOpcode)

/*                                                                           */
#  define WDA_GET_RX_REORDER_SLOT_IDX(pRxMeta) \
                (((WDI_DS_RxMetaInfoType*)(pRxMeta))->ampdu_reorderSlotIdx)

/*                                                                           */
#  define WDA_GET_RX_REORDER_FWD_IDX(pRxMeta)  \
         (((WDI_DS_RxMetaInfoType*)(pRxMeta))->ampdu_reorderFwdIdx)

/*                                                                           */
#  define WDA_GET_RX_REORDER_CUR_PKT_SEQ_NO(pRxMeta)  \
         (((WDI_DS_RxMetaInfoType*)(pRxMeta))->currentPktSeqNo)

/*                                                                           */
#  define WDA_IS_RX_LLC_PRESENT(pRxMeta)    \
      ( (0 == ((WDI_DS_RxMetaInfoType*)(pRxMeta))->llcr) ? VOS_TRUE : VOS_FALSE )

#define WLANWDA_HO_IS_AN_AMPDU                    0x4000
#define WLANWDA_HO_LAST_MPDU_OF_AMPDU             0x400

/*                                                                           */
#  define WDA_IS_RX_AN_AMPDU(pRxMeta)       \
   ( ((WDI_DS_RxMetaInfoType*)(pRxMeta))->rxpFlags & WLANWDA_HO_IS_AN_AMPDU )

/*                                                                           */
#  define WDA_IS_RX_LAST_MPDU(pRxMeta)      \
   ( ((WDI_DS_RxMetaInfoType*)(pRxMeta))->rxpFlags & WLANWDA_HO_LAST_MPDU_OF_AMPDU ) 

/*                                                                          */
#  define WDA_GET_RX_TIMESTAMP(pRxMeta) (((WDI_DS_RxMetaInfoType*)(pRxMeta))->mclkRxTimestamp)

/*                                                                           */
#  define WDA_IS_RX_IN_SCAN(pRxMeta)  (((WDI_DS_RxMetaInfoType*)(pRxMeta))->scan)
#ifdef WLAN_FEATURE_ROAM_SCAN_OFFLOAD
/*                                                                           */
#  define WDA_GET_OFFLOADSCANLEARN(pRxMeta) (((WDI_DS_RxMetaInfoType*)(pRxMeta))->offloadScanLearn)
/*                                                                           */
#  define WDA_GET_ROAMCANDIDATEIND(pRxMeta) (((WDI_DS_RxMetaInfoType*)(pRxMeta))->roamCandidateInd)
#endif
/*                                                                           */
//          
#  define WDA_RSSI_OFFSET             100
#  define WDA_GET_RSSI0_DB(rssi0)     (rssi0 - WDA_RSSI_OFFSET)
#  define WDA_GET_RSSI1_DB(rssi0)     (0 - WDA_RSSI_OFFSET)
#  define WDA_MAX_OF_TWO(val1, val2)  ( ((val1) > (val2)) ? (val1) : (val2))
#  define WDA_GET_RSSI_DB(rssi0)  \
                WDA_MAX_OF_TWO(WDA_GET_RSSI0_DB(rssi0), WDA_GET_RSSI1_DB(rssi0))
#  define WDA_GET_RX_RSSI_DB(pRxMeta) \
                       WDA_GET_RSSI_DB((((WDI_DS_RxMetaInfoType*)(pRxMeta))->rssi0))

/*                                                                           */
#  define WDA_GET_RX_SNR(pRxMeta)  (((WDI_DS_RxMetaInfoType*)(pRxMeta))->snr)

/*                                                                           */
//                    
/*                                                                 
                                                                              */
#  define WDA_IS_RX_FC(pRxMeta)    (((WDI_DS_RxMetaInfoType*)(pRxMeta))->fc)

/*                                                                           */
#  define WDA_GET_RX_FC_VALID_STA_MASK(pRxMeta) \
                       (((WDI_DS_RxMetaInfoType*)(pRxMeta))->fcSTAValidMask)

/*                                                                           */
#  define WDA_GET_RX_FC_PWRSAVE_STA_MASK(pRxMeta) \
                 (((WDI_DS_RxMetaInfoType*)(pRxMeta))->fcSTAPwrSaveStateMask)

/*                                                                            */
#  define WDA_GET_RX_FC_STA_THRD_IND_MASK(pRxMeta) \
                     (((WDI_DS_RxMetaInfoType*)(pRxMeta))->fcSTAThreshIndMask)

/*                                                                                        */
#  define WDA_GET_RX_FC_STA_TX_DISABLED_BITMAP(pRxMeta) \
                     (((WDI_DS_RxMetaInfoType*)(pRxMeta))->fcStaTxDisabledBitmap)

/*                                                                           */
#  define WDA_GET_RX_FC_STA_TXQ_LEN(pRxMeta, staId) \
                  (((WDI_DS_RxMetaInfoType*)(pRxMeta))->fcSTATxQLen[(staId)])

/*                                                                           */
#  define WDA_GET_RX_FC_STA_CUR_TXRATE(pRxMeta, staId) \
                (((WDI_DS_RxMetaInfoType*)(pRxMeta))->fcSTACurTxRate[(staId)])

/*                                                                           */
#  define WDA_GET_RX_REPLAY_COUNT(pRxMeta) \
                            (((WDI_DS_RxMetaInfoType*)(pRxMeta))->replayCount)

/*                                                                            */
#  define WDA_GETRSSI0(pRxMeta) (((WDI_DS_RxMetaInfoType*)(pRxMeta))->rssi0)

/*                                                                            */
#  define WDA_GETRSSI1(pRxMeta) (((WDI_DS_RxMetaInfoType*)(pRxMeta))->rssi1)

/*                                                                    */
#ifdef WLAN_FEATURE_11W
#  define WDA_GET_RX_RMF(pRxMeta) (((WDI_DS_RxMetaInfoType*)(pRxMeta))->rmf)
#endif

/*                                                                     */

uint8 WDA_IsWcnssWlanCompiledVersionGreaterThanOrEqual(uint8 major, uint8 minor, uint8 version, uint8 revision);
uint8 WDA_IsWcnssWlanReportedVersionGreaterThanOrEqual(uint8 major, uint8 minor, uint8 version, uint8 revision);


VOS_STATUS WDA_GetWcnssWlanCompiledVersion(v_PVOID_t pvosGCtx,
                                           tSirVersionType *pVersion);
VOS_STATUS WDA_GetWcnssWlanReportedVersion(v_PVOID_t pvosGCtx,
                                           tSirVersionType *pVersion);
VOS_STATUS WDA_GetWcnssSoftwareVersion(v_PVOID_t pvosGCtx,
                                       tANI_U8 *pVersion,
                                       tANI_U32 versionBufferSize);
VOS_STATUS WDA_GetWcnssHardwareVersion(v_PVOID_t pvosGCtx,
                                       tANI_U8 *pVersion,
                                       tANI_U32 versionBufferSize);

VOS_STATUS WDA_SetUapsdAcParamsReq(v_PVOID_t , v_U8_t , tUapsdInfo *);
VOS_STATUS WDA_ClearUapsdAcParamsReq(v_PVOID_t , v_U8_t , wpt_uint8 );
VOS_STATUS WDA_SetRSSIThresholdsReq(tpAniSirGlobal , tSirRSSIThresholds *);
//                                                          
extern tSirRetStatus halMmhForwardMBmsg(void*, tSirMbMsg*);
tSirRetStatus uMacPostCtrlMsg(void* pSirGlobal, tSirMbMsg* pMb);


#define WDA_MAX_TXPOWER_INVALID HAL_MAX_TXPOWER_INVALID

//                                    
#if 0
//               
//                                                                                    
//                                           

//              
#define WDA_HOST_MSG_START SIR_HAL_HOST_MSG_START 
#define WDA_INITIAL_CAL_FAILED_NTF SIR_HAL_INITIAL_CAL_FAILED_NTF
#define WDA_SHUTDOWN_REQ SIR_HAL_SHUTDOWN_REQ
#define WDA_SHUTDOWN_CNF SIR_HAL_SHUTDOWN_CNF
#define WDA_RADIO_ON_OFF_IND SIR_HAL_RADIO_ON_OFF_IND
#define WDA_RESET_CNF SIR_HAL_RESET_CNF
#define WDA_SetRegDomain \
    (eHalStatus halPhySetRegDomain(tHalHandle hHal, eRegDomainId regDomain))
#endif

#define WDA_APP_SETUP_NTF  SIR_HAL_APP_SETUP_NTF 
#define WDA_NIC_OPER_NTF   SIR_HAL_NIC_OPER_NTF
#define WDA_INIT_START_REQ SIR_HAL_INIT_START_REQ
#define WDA_RESET_REQ      SIR_HAL_RESET_REQ
#define WDA_HDD_ADDBA_REQ  SIR_HAL_HDD_ADDBA_REQ
#define WDA_HDD_ADDBA_RSP  SIR_HAL_HDD_ADDBA_RSP
#define WDA_DELETEBA_IND   SIR_HAL_DELETEBA_IND
#define WDA_BA_FAIL_IND    SIR_HAL_BA_FAIL_IND
#define WDA_TL_FLUSH_AC_REQ SIR_TL_HAL_FLUSH_AC_REQ
#define WDA_TL_FLUSH_AC_RSP SIR_HAL_TL_FLUSH_AC_RSP

#define WDA_MSG_TYPES_BEGIN            SIR_HAL_MSG_TYPES_BEGIN
#define WDA_ITC_MSG_TYPES_BEGIN        SIR_HAL_ITC_MSG_TYPES_BEGIN
#define WDA_RADAR_DETECTED_IND         SIR_HAL_RADAR_DETECTED_IND
#define WDA_WDT_KAM_RSP                SIR_HAL_WDT_KAM_RSP 
#define WDA_TIMER_TEMP_MEAS_REQ        SIR_HAL_TIMER_TEMP_MEAS_REQ
#define WDA_TIMER_PERIODIC_STATS_COLLECT_REQ   SIR_HAL_TIMER_PERIODIC_STATS_COLLECT_REQ
#define WDA_CAL_REQ_NTF                SIR_HAL_CAL_REQ_NTF
#define WDA_MNT_OPEN_TPC_TEMP_MEAS_REQ SIR_HAL_MNT_OPEN_TPC_TEMP_MEAS_REQ
#define WDA_CCA_MONITOR_INTERVAL_TO    SIR_HAL_CCA_MONITOR_INTERVAL_TO
#define WDA_CCA_MONITOR_DURATION_TO    SIR_HAL_CCA_MONITOR_DURATION_TO
#define WDA_CCA_MONITOR_START          SIR_HAL_CCA_MONITOR_START 
#define WDA_CCA_MONITOR_STOP           SIR_HAL_CCA_MONITOR_STOP
#define WDA_CCA_CHANGE_MODE            SIR_HAL_CCA_CHANGE_MODE
#define WDA_TIMER_WRAP_AROUND_STATS_COLLECT_REQ   SIR_HAL_TIMER_WRAP_AROUND_STATS_COLLECT_REQ

/*
                              
 */
#define WDA_ADD_STA_REQ                SIR_HAL_ADD_STA_REQ
#define WDA_ADD_STA_RSP                SIR_HAL_ADD_STA_RSP
#define WDA_ADD_STA_SELF_RSP           SIR_HAL_ADD_STA_SELF_RSP
#define WDA_DEL_STA_SELF_RSP           SIR_HAL_DEL_STA_SELF_RSP
#define WDA_DELETE_STA_REQ             SIR_HAL_DELETE_STA_REQ 
#define WDA_DELETE_STA_RSP             SIR_HAL_DELETE_STA_RSP
#define WDA_ADD_BSS_REQ                SIR_HAL_ADD_BSS_REQ
#define WDA_ADD_BSS_RSP                SIR_HAL_ADD_BSS_RSP
#define WDA_DELETE_BSS_REQ             SIR_HAL_DELETE_BSS_REQ
#define WDA_DELETE_BSS_RSP             SIR_HAL_DELETE_BSS_RSP
#define WDA_INIT_SCAN_REQ              SIR_HAL_INIT_SCAN_REQ
#define WDA_INIT_SCAN_RSP              SIR_HAL_INIT_SCAN_RSP
#define WDA_START_SCAN_REQ             SIR_HAL_START_SCAN_REQ
#define WDA_START_SCAN_RSP             SIR_HAL_START_SCAN_RSP
#define WDA_END_SCAN_REQ               SIR_HAL_END_SCAN_REQ
#define WDA_END_SCAN_RSP               SIR_HAL_END_SCAN_RSP
#define WDA_FINISH_SCAN_REQ            SIR_HAL_FINISH_SCAN_REQ
#define WDA_FINISH_SCAN_RSP            SIR_HAL_FINISH_SCAN_RSP
#define WDA_SEND_BEACON_REQ            SIR_HAL_SEND_BEACON_REQ
#define WDA_SEND_BEACON_RSP            SIR_HAL_SEND_BEACON_RSP

#define WDA_INIT_CFG_REQ               SIR_HAL_INIT_CFG_REQ
#define WDA_INIT_CFG_RSP               SIR_HAL_INIT_CFG_RSP

#define WDA_INIT_WM_CFG_REQ            SIR_HAL_INIT_WM_CFG_REQ
#define WDA_INIT_WM_CFG_RSP            SIR_HAL_INIT_WM_CFG_RSP

#define WDA_SET_BSSKEY_REQ             SIR_HAL_SET_BSSKEY_REQ
#define WDA_SET_BSSKEY_RSP             SIR_HAL_SET_BSSKEY_RSP
#define WDA_SET_STAKEY_REQ             SIR_HAL_SET_STAKEY_REQ
#define WDA_SET_STAKEY_RSP             SIR_HAL_SET_STAKEY_RSP
#define WDA_DPU_STATS_REQ              SIR_HAL_DPU_STATS_REQ 
#define WDA_DPU_STATS_RSP              SIR_HAL_DPU_STATS_RSP
#define WDA_GET_DPUINFO_REQ            SIR_HAL_GET_DPUINFO_REQ
#define WDA_GET_DPUINFO_RSP            SIR_HAL_GET_DPUINFO_RSP

#define WDA_UPDATE_EDCA_PROFILE_IND    SIR_HAL_UPDATE_EDCA_PROFILE_IND

#define WDA_UPDATE_STARATEINFO_REQ     SIR_HAL_UPDATE_STARATEINFO_REQ
#define WDA_UPDATE_STARATEINFO_RSP     SIR_HAL_UPDATE_STARATEINFO_RSP

#define WDA_UPDATE_BEACON_IND          SIR_HAL_UPDATE_BEACON_IND
#define WDA_UPDATE_CF_IND              SIR_HAL_UPDATE_CF_IND
#define WDA_CHNL_SWITCH_REQ            SIR_HAL_CHNL_SWITCH_REQ
#define WDA_ADD_TS_REQ                 SIR_HAL_ADD_TS_REQ
#define WDA_DEL_TS_REQ                 SIR_HAL_DEL_TS_REQ
#define WDA_SOFTMAC_TXSTAT_REPORT      SIR_HAL_SOFTMAC_TXSTAT_REPORT

#define WDA_MBOX_SENDMSG_COMPLETE_IND  SIR_HAL_MBOX_SENDMSG_COMPLETE_IND
#define WDA_EXIT_BMPS_REQ              SIR_HAL_EXIT_BMPS_REQ
#define WDA_EXIT_BMPS_RSP              SIR_HAL_EXIT_BMPS_RSP
#define WDA_EXIT_BMPS_IND              SIR_HAL_EXIT_BMPS_IND 
#define WDA_ENTER_BMPS_REQ             SIR_HAL_ENTER_BMPS_REQ
#define WDA_ENTER_BMPS_RSP             SIR_HAL_ENTER_BMPS_RSP
#define WDA_BMPS_STATUS_IND            SIR_HAL_BMPS_STATUS_IND
#define WDA_MISSED_BEACON_IND          SIR_HAL_MISSED_BEACON_IND

#define WDA_CFG_RXP_FILTER_REQ         SIR_HAL_CFG_RXP_FILTER_REQ
#define WDA_CFG_RXP_FILTER_RSP         SIR_HAL_CFG_RXP_FILTER_RSP

#define WDA_SWITCH_CHANNEL_RSP         SIR_HAL_SWITCH_CHANNEL_RSP
#define WDA_P2P_NOA_ATTR_IND           SIR_HAL_P2P_NOA_ATTR_IND
#define WDA_P2P_NOA_START_IND          SIR_HAL_P2P_NOA_START_IND
#define WDA_PWR_SAVE_CFG               SIR_HAL_PWR_SAVE_CFG

#define WDA_REGISTER_PE_CALLBACK       SIR_HAL_REGISTER_PE_CALLBACK
#define WDA_SOFTMAC_MEM_READREQUEST    SIR_HAL_SOFTMAC_MEM_READREQUEST
#define WDA_SOFTMAC_MEM_WRITEREQUEST   SIR_HAL_SOFTMAC_MEM_WRITEREQUEST

#define WDA_SOFTMAC_MEM_READRESPONSE   SIR_HAL_SOFTMAC_MEM_READRESPONSE
#define WDA_SOFTMAC_BULKREGWRITE_CONFIRM      SIR_HAL_SOFTMAC_BULKREGWRITE_CONFIRM
#define WDA_SOFTMAC_BULKREGREAD_RESPONSE      SIR_HAL_SOFTMAC_BULKREGREAD_RESPONSE
#define WDA_SOFTMAC_HOSTMESG_MSGPROCESSRESULT SIR_HAL_SOFTMAC_HOSTMESG_MSGPROCESSRESULT

#define WDA_ADDBA_REQ                  SIR_HAL_ADDBA_REQ 
#define WDA_ADDBA_RSP                  SIR_HAL_ADDBA_RSP
#define WDA_DELBA_IND                  SIR_HAL_DELBA_IND
#define WDA_DEL_BA_IND                 SIR_HAL_DEL_BA_IND
#define WDA_MIC_FAILURE_IND            SIR_HAL_MIC_FAILURE_IND

//                                                      
#define WDA_DELBA_REQ                  SIR_HAL_DELBA_REQ
#define WDA_IBSS_STA_ADD               SIR_HAL_IBSS_STA_ADD
#define WDA_TIMER_ADJUST_ADAPTIVE_THRESHOLD_IND   SIR_HAL_TIMER_ADJUST_ADAPTIVE_THRESHOLD_IND
#define WDA_SET_LINK_STATE             SIR_HAL_SET_LINK_STATE
#define WDA_SET_LINK_STATE_RSP         SIR_HAL_SET_LINK_STATE_RSP
#define WDA_ENTER_IMPS_REQ             SIR_HAL_ENTER_IMPS_REQ
#define WDA_ENTER_IMPS_RSP             SIR_HAL_ENTER_IMPS_RSP
#define WDA_EXIT_IMPS_RSP              SIR_HAL_EXIT_IMPS_RSP
#define WDA_EXIT_IMPS_REQ              SIR_HAL_EXIT_IMPS_REQ
#define WDA_SOFTMAC_HOSTMESG_PS_STATUS_IND  SIR_HAL_SOFTMAC_HOSTMESG_PS_STATUS_IND  
#define WDA_POSTPONE_ENTER_IMPS_RSP    SIR_HAL_POSTPONE_ENTER_IMPS_RSP
#define WDA_STA_STAT_REQ               SIR_HAL_STA_STAT_REQ 
#define WDA_GLOBAL_STAT_REQ            SIR_HAL_GLOBAL_STAT_REQ
#define WDA_AGGR_STAT_REQ              SIR_HAL_AGGR_STAT_REQ 
#define WDA_STA_STAT_RSP               SIR_HAL_STA_STAT_RSP
#define WDA_GLOBAL_STAT_RSP            SIR_HAL_GLOBAL_STAT_RSP
#define WDA_AGGR_STAT_RSP              SIR_HAL_AGGR_STAT_RSP
#define WDA_STAT_SUMM_REQ              SIR_HAL_STAT_SUMM_REQ
#define WDA_STAT_SUMM_RSP              SIR_HAL_STAT_SUMM_RSP
#define WDA_REMOVE_BSSKEY_REQ          SIR_HAL_REMOVE_BSSKEY_REQ
#define WDA_REMOVE_BSSKEY_RSP          SIR_HAL_REMOVE_BSSKEY_RSP
#define WDA_REMOVE_STAKEY_REQ          SIR_HAL_REMOVE_STAKEY_REQ
#define WDA_REMOVE_STAKEY_RSP          SIR_HAL_REMOVE_STAKEY_RSP
#define WDA_SET_STA_BCASTKEY_REQ       SIR_HAL_SET_STA_BCASTKEY_REQ 
#define WDA_SET_STA_BCASTKEY_RSP       SIR_HAL_SET_STA_BCASTKEY_RSP
#define WDA_REMOVE_STA_BCASTKEY_REQ    SIR_HAL_REMOVE_STA_BCASTKEY_REQ
#define WDA_REMOVE_STA_BCASTKEY_RSP    SIR_HAL_REMOVE_STA_BCASTKEY_RSP
#define WDA_ADD_TS_RSP                 SIR_HAL_ADD_TS_RSP
#define WDA_DPU_MIC_ERROR              SIR_HAL_DPU_MIC_ERROR
#define WDA_TIMER_BA_ACTIVITY_REQ      SIR_HAL_TIMER_BA_ACTIVITY_REQ
#define WDA_TIMER_CHIP_MONITOR_TIMEOUT SIR_HAL_TIMER_CHIP_MONITOR_TIMEOUT
#define WDA_TIMER_TRAFFIC_ACTIVITY_REQ SIR_HAL_TIMER_TRAFFIC_ACTIVITY_REQ
#define WDA_TIMER_ADC_RSSI_STATS       SIR_HAL_TIMER_ADC_RSSI_STATS
#define WDA_TIMER_TRAFFIC_STATS_IND    SIR_HAL_TRAFFIC_STATS_IND

#ifdef WLAN_FEATURE_11W
#define WDA_EXCLUDE_UNENCRYPTED_IND    SIR_HAL_EXCLUDE_UNENCRYPTED_IND
#endif

#ifdef FEATURE_WLAN_CCX
#define WDA_TSM_STATS_REQ              SIR_HAL_TSM_STATS_REQ
#define WDA_TSM_STATS_RSP              SIR_HAL_TSM_STATS_RSP
#endif
#define WDA_UPDATE_PROBE_RSP_IE_BITMAP_IND SIR_HAL_UPDATE_PROBE_RSP_IE_BITMAP_IND
#define WDA_UPDATE_UAPSD_IND           SIR_HAL_UPDATE_UAPSD_IND

#define WDA_SET_MIMOPS_REQ                      SIR_HAL_SET_MIMOPS_REQ 
#define WDA_SET_MIMOPS_RSP                      SIR_HAL_SET_MIMOPS_RSP
#define WDA_SYS_READY_IND                       SIR_HAL_SYS_READY_IND
#define WDA_SET_TX_POWER_REQ                    SIR_HAL_SET_TX_POWER_REQ
#define WDA_SET_TX_POWER_RSP                    SIR_HAL_SET_TX_POWER_RSP
#define WDA_GET_TX_POWER_REQ                    SIR_HAL_GET_TX_POWER_REQ
#define WDA_GET_TX_POWER_RSP                    SIR_HAL_GET_TX_POWER_RSP
#define WDA_GET_NOISE_REQ                       SIR_HAL_GET_NOISE_REQ 
#define WDA_GET_NOISE_RSP                       SIR_HAL_GET_NOISE_RSP
#define WDA_SET_TX_PER_TRACKING_REQ    SIR_HAL_SET_TX_PER_TRACKING_REQ

/*                                                       */
#define WDA_TRANSMISSION_CONTROL_IND            SIR_HAL_TRANSMISSION_CONTROL_IND
/*                                                          */
#define WDA_INIT_RADAR_IND                      SIR_HAL_INIT_RADAR_IND
/*                                                       */


#define WDA_BEACON_PRE_IND             SIR_HAL_BEACON_PRE_IND
#define WDA_ENTER_UAPSD_REQ            SIR_HAL_ENTER_UAPSD_REQ
#define WDA_ENTER_UAPSD_RSP            SIR_HAL_ENTER_UAPSD_RSP
#define WDA_EXIT_UAPSD_REQ             SIR_HAL_EXIT_UAPSD_REQ 
#define WDA_EXIT_UAPSD_RSP             SIR_HAL_EXIT_UAPSD_RSP
#define WDA_LOW_RSSI_IND               SIR_HAL_LOW_RSSI_IND 
#define WDA_BEACON_FILTER_IND          SIR_HAL_BEACON_FILTER_IND
//                          
#define WDA_WOWL_ADD_BCAST_PTRN        SIR_HAL_WOWL_ADD_BCAST_PTRN
#define WDA_WOWL_DEL_BCAST_PTRN        SIR_HAL_WOWL_DEL_BCAST_PTRN
#define WDA_WOWL_ENTER_REQ             SIR_HAL_WOWL_ENTER_REQ
#define WDA_WOWL_ENTER_RSP             SIR_HAL_WOWL_ENTER_RSP
#define WDA_WOWL_EXIT_REQ              SIR_HAL_WOWL_EXIT_REQ
#define WDA_WOWL_EXIT_RSP              SIR_HAL_WOWL_EXIT_RSP
#define WDA_TX_COMPLETE_IND            SIR_HAL_TX_COMPLETE_IND
#define WDA_TIMER_RA_COLLECT_AND_ADAPT SIR_HAL_TIMER_RA_COLLECT_AND_ADAPT
//                                
#define WDA_GET_STATISTICS_REQ         SIR_HAL_GET_STATISTICS_REQ
#define WDA_GET_STATISTICS_RSP         SIR_HAL_GET_STATISTICS_RSP
#define WDA_SET_KEY_DONE               SIR_HAL_SET_KEY_DONE

//                         
#define WDA_BTC_SET_CFG                SIR_HAL_BTC_SET_CFG
#define WDA_SIGNAL_BT_EVENT            SIR_HAL_SIGNAL_BT_EVENT
#define WDA_HANDLE_FW_MBOX_RSP         SIR_HAL_HANDLE_FW_MBOX_RSP
#define WDA_UPDATE_PROBE_RSP_TEMPLATE_IND     SIR_HAL_UPDATE_PROBE_RSP_TEMPLATE_IND
#define WDA_SIGNAL_BTAMP_EVENT         SIR_HAL_SIGNAL_BTAMP_EVENT

#ifdef FEATURE_OEM_DATA_SUPPORT
/*                                      */
#define WDA_START_OEM_DATA_REQ         SIR_HAL_START_OEM_DATA_REQ 
#define WDA_START_OEM_DATA_RSP         SIR_HAL_START_OEM_DATA_RSP
#define WDA_FINISH_OEM_DATA_REQ        SIR_HAL_FINISH_OEM_DATA_REQ
#endif

#define WDA_SET_MAX_TX_POWER_REQ       SIR_HAL_SET_MAX_TX_POWER_REQ
#define WDA_SET_MAX_TX_POWER_RSP       SIR_HAL_SET_MAX_TX_POWER_RSP

#define WDA_SET_MAX_TX_POWER_PER_BAND_REQ \
        SIR_HAL_SET_MAX_TX_POWER_PER_BAND_REQ
#define WDA_SET_MAX_TX_POWER_PER_BAND_RSP \
        SIR_HAL_SET_MAX_TX_POWER_PER_BAND_RSP

#define WDA_SEND_MSG_COMPLETE          SIR_HAL_SEND_MSG_COMPLETE 

//                                 
#define WDA_SET_HOST_OFFLOAD           SIR_HAL_SET_HOST_OFFLOAD

//                               
#define WDA_SET_KEEP_ALIVE             SIR_HAL_SET_KEEP_ALIVE

#ifdef WLAN_NS_OFFLOAD
#define WDA_SET_NS_OFFLOAD             SIR_HAL_SET_NS_OFFLOAD
#endif //               
#define WDA_ADD_STA_SELF_REQ           SIR_HAL_ADD_STA_SELF_REQ
#define WDA_DEL_STA_SELF_REQ           SIR_HAL_DEL_STA_SELF_REQ

#define WDA_SET_P2P_GO_NOA_REQ         SIR_HAL_SET_P2P_GO_NOA_REQ
#define WDA_SET_TDLS_LINK_ESTABLISH_REQ SIR_HAL_TDLS_LINK_ESTABLISH_REQ
#define WDA_SET_TDLS_LINK_ESTABLISH_REQ_RSP SIR_HAL_TDLS_LINK_ESTABLISH_REQ_RSP

#define WDA_TX_COMPLETE_TIMEOUT_IND  (WDA_MSG_TYPES_END - 1)
#define WDA_WLAN_SUSPEND_IND           SIR_HAL_WLAN_SUSPEND_IND
#define WDA_WLAN_RESUME_REQ           SIR_HAL_WLAN_RESUME_REQ
#define WDA_MSG_TYPES_END    SIR_HAL_MSG_TYPES_END

#define WDA_MMH_TXMB_READY_EVT SIR_HAL_MMH_TXMB_READY_EVT     
#define WDA_MMH_RXMB_DONE_EVT  SIR_HAL_MMH_RXMB_DONE_EVT    
#define WDA_MMH_MSGQ_NE_EVT    SIR_HAL_MMH_MSGQ_NE_EVT

#ifdef WLAN_FEATURE_VOWIFI_11R
#define WDA_AGGR_QOS_REQ               SIR_HAL_AGGR_QOS_REQ
#define WDA_AGGR_QOS_RSP               SIR_HAL_AGGR_QOS_RSP
#endif /*                         */

/*             */
#define WDA_FTM_CMD_REQ        SIR_PTT_MSG_TYPES_BEGIN
#define WDA_FTM_CMD_RSP        SIR_PTT_MSG_TYPES_END

#ifdef FEATURE_WLAN_SCAN_PNO
/*                             */
#define WDA_SET_PNO_REQ             SIR_HAL_SET_PNO_REQ
#define WDA_SET_RSSI_FILTER_REQ     SIR_HAL_SET_RSSI_FILTER_REQ
#define WDA_UPDATE_SCAN_PARAMS_REQ  SIR_HAL_UPDATE_SCAN_PARAMS

/*                                    */
#define WDA_SET_PNO_CHANGED_IND     SIR_HAL_SET_PNO_CHANGED_IND
#endif //                      

#ifdef WLAN_FEATURE_ROAM_SCAN_OFFLOAD
#define WDA_ROAM_SCAN_OFFLOAD_REQ   SIR_HAL_ROAM_SCAN_OFFLOAD_REQ
#define WDA_ROAM_SCAN_OFFLOAD_RSP   SIR_HAL_ROAM_SCAN_OFFLOAD_RSP
#endif

#ifdef WLAN_WAKEUP_EVENTS
#define WDA_WAKE_REASON_IND    SIR_HAL_WAKE_REASON_IND  
#endif //                   

#ifdef WLAN_FEATURE_PACKET_FILTERING
#define WDA_8023_MULTICAST_LIST_REQ                     SIR_HAL_8023_MULTICAST_LIST_REQ
#define WDA_RECEIVE_FILTER_SET_FILTER_REQ               SIR_HAL_RECEIVE_FILTER_SET_FILTER_REQ
#define WDA_PACKET_COALESCING_FILTER_MATCH_COUNT_REQ    SIR_HAL_PACKET_COALESCING_FILTER_MATCH_COUNT_REQ
#define WDA_PACKET_COALESCING_FILTER_MATCH_COUNT_RSP    SIR_HAL_PACKET_COALESCING_FILTER_MATCH_COUNT_RSP
#define WDA_RECEIVE_FILTER_CLEAR_FILTER_REQ             SIR_HAL_RECEIVE_FILTER_CLEAR_FILTER_REQ   
#endif //                              

#define WDA_SET_POWER_PARAMS_REQ   SIR_HAL_SET_POWER_PARAMS_REQ
#define WDA_DHCP_START_IND              SIR_HAL_DHCP_START_IND
#define WDA_DHCP_STOP_IND               SIR_HAL_DHCP_STOP_IND


#ifdef WLAN_FEATURE_GTK_OFFLOAD
#define WDA_GTK_OFFLOAD_REQ             SIR_HAL_GTK_OFFLOAD_REQ
#define WDA_GTK_OFFLOAD_GETINFO_REQ     SIR_HAL_GTK_OFFLOAD_GETINFO_REQ
#define WDA_GTK_OFFLOAD_GETINFO_RSP     SIR_HAL_GTK_OFFLOAD_GETINFO_RSP
#endif //                        

#define WDA_SET_TM_LEVEL_REQ       SIR_HAL_SET_TM_LEVEL_REQ

#ifdef WLAN_FEATURE_11AC
#define WDA_UPDATE_OP_MODE         SIR_HAL_UPDATE_OP_MODE
#endif

#define WDA_GET_ROAM_RSSI_REQ      SIR_HAL_GET_ROAM_RSSI_REQ
#define WDA_GET_ROAM_RSSI_RSP      SIR_HAL_GET_ROAM_RSSI_RSP

#define WDA_START_SCAN_OFFLOAD_REQ  SIR_HAL_START_SCAN_OFFLOAD_REQ
#define WDA_START_SCAN_OFFLOAD_RSP  SIR_HAL_START_SCAN_OFFLOAD_RSP
#define WDA_STOP_SCAN_OFFLOAD_REQ  SIR_HAL_STOP_SCAN_OFFLOAD_REQ
#define WDA_STOP_SCAN_OFFLOAD_RSP  SIR_HAL_STOP_SCAN_OFFLOAD_RSP
#define WDA_UPDATE_CHAN_LIST_REQ    SIR_HAL_UPDATE_CHAN_LIST_REQ
#define WDA_UPDATE_CHAN_LIST_RSP    SIR_HAL_UPDATE_CHAN_LIST_RSP
#define WDA_RX_SCAN_EVENT           SIR_HAL_RX_SCAN_EVENT
#define WDA_IBSS_PEER_INACTIVITY_IND SIR_HAL_IBSS_PEER_INACTIVITY_IND

#ifdef FEATURE_WLAN_LPHB
#define WDA_LPHB_CONF_REQ          SIR_HAL_LPHB_CONF_IND
#define WDA_LPHB_WAIT_EXPIRE_IND   SIR_HAL_LPHB_WAIT_EXPIRE_IND
#endif /*                   */

#define WDA_ADD_PERIODIC_TX_PTRN_IND    SIR_HAL_ADD_PERIODIC_TX_PTRN_IND
#define WDA_DEL_PERIODIC_TX_PTRN_IND    SIR_HAL_DEL_PERIODIC_TX_PTRN_IND

#ifdef FEATURE_WLAN_BATCH_SCAN
#define WDA_SET_BATCH_SCAN_REQ            SIR_HAL_SET_BATCH_SCAN_REQ
#define WDA_SET_BATCH_SCAN_RSP            SIR_HAL_SET_BATCH_SCAN_RSP
#define WDA_STOP_BATCH_SCAN_IND           SIR_HAL_STOP_BATCH_SCAN_IND
#define WDA_TRIGGER_BATCH_SCAN_RESULT_IND SIR_HAL_TRIGGER_BATCH_SCAN_RESULT_IND
#endif

tSirRetStatus wdaPostCtrlMsg(tpAniSirGlobal pMac, tSirMsgQ *pMsg);

eHalStatus WDA_SetRegDomain(void * clientCtxt, v_REGDOMAIN_t regId,
                                               tAniBool sendRegHint);

#define HAL_USE_BD_RATE2_FOR_MANAGEMENT_FRAME 0x40 //                                                            

#define halTxFrame(hHal, pFrmBuf, frmLen, frmType, txDir, tid, pCompFunc, pData, txFlag) \
   (eHalStatus)( WDA_TxPacket(\
         vos_get_context(VOS_MODULE_ID_WDA, vos_get_global_context(VOS_MODULE_ID_WDA, (hHal))),\
         (pFrmBuf),\
         (frmLen),\
         (frmType),\
         (txDir),\
         (tid),\
         (pCompFunc),\
         (pData),\
         (NULL), \
         (txFlag)) )

#define halTxFrameWithTxComplete(hHal, pFrmBuf, frmLen, frmType, txDir, tid, pCompFunc, pData, pCBackFnTxComp, txFlag) \
   (eHalStatus)( WDA_TxPacket(\
         vos_get_context(VOS_MODULE_ID_WDA, vos_get_global_context(VOS_MODULE_ID_WDA, (hHal))),\
         (pFrmBuf),\
         (frmLen),\
         (frmType),\
         (txDir),\
         (tid),\
         (pCompFunc),\
         (pData),\
         (pCBackFnTxComp), \
         (txFlag)) )

/*                                                                  
                            
                                                                    */

v_BOOL_t WDA_IsHwFrameTxTranslationCapable(v_PVOID_t pVosGCtx, 
                                                      tANI_U8 staIdx);

#  define WDA_EnableUapsdAcParams(vosGCtx, staId, uapsdInfo) \
         WDA_SetUapsdAcParamsReq(vosGCtx, staId, uapsdInfo)

#  define WDA_DisableUapsdAcParams(vosGCtx, staId, ac) \
          WDA_ClearUapsdAcParamsReq(vosGCtx, staId, ac)

#  define WDA_SetRSSIThresholds(pMac, pThresholds) \
         WDA_SetRSSIThresholdsReq(pMac, pThresholds)

#define WDA_UpdateRssiBmps(pvosGCtx,  staId, rssi) \
        WLANTL_UpdateRssiBmps(pvosGCtx, staId, rssi)

#define WDA_UpdateSnrBmps(pvosGCtx,  staId, rssi) \
        WLANTL_UpdateSnrBmps(pvosGCtx, staId, snr)

#define WDA_GetSnr(staId, snr) \
        WLANTL_GetSnr(staId, snr)

#define WDA_UpdateLinkCapacity(pvosGCtx,  staId, linkCapacity) \
        WLANTL_UpdateLinkCapacity(pvosGCtx, staId, linkCapacity)

#ifdef WLAN_PERF 
/*                                                                          
                                        

              
                                                          

                                                                            
                                                                              
                                                                            
                                                                           
                                                                             
                                                         

                                                                         
                                                    

               
     
             

     
                               
                                      
                                           
                                         
    
      
                                   
                                                                          
   
              
                 

               
  
                                                                            */
void WDA_TLI_FastHwFwdDataFrame
(
  v_PVOID_t     pvosGCtx,
  vos_pkt_t*    vosDataBuff,
  VOS_STATUS*   pvosStatus,
  v_U32_t*       puFastFwdOK,
  WLANTL_MetaInfoType*  pMetaInfo,
  WLAN_STADescType*  pStaInfo
);
#endif /*           */

/*                                                                          
                             

              
                                                                         
                                      


                                                                        
                                           

       
                                                                        
                                                                      
                                    

               
     
             

     
                               
                                                                           
                                               
                                                                           
                                                                    
                                                                       
                                                              
      
                                                        
                                                                   
   
              
                                                          
                                              

               
  
                                                                            */
VOS_STATUS 
WDA_DS_Register 
( 
  v_PVOID_t                 pvosGCtx, 
  WDA_DS_TxCompleteCallback pfnTxCompleteCallback,
  WDA_DS_RxPacketCallback   pfnRxPacketCallback, 
  WDA_DS_TxPacketCallback   pfnTxPacketCallback,
  WDA_DS_ResourceCB         pfnResourceCB,
  v_U32_t                   uResTheshold,
  v_PVOID_t                 pCallbackContext,
  v_U32_t                   *uAvailableTxBuf
);

/*                                                                          
                              

              
                                               

                                                                      
                     

                                                                 

               
     
             

     
                               
   
              
                                                          
                                              

               
  
                                                                            */
VOS_STATUS
WDA_DS_StartXmit
(
  v_PVOID_t pvosGCtx
);

/*                                                                          
                              

              
                                                                     

               
     
             

     
                                                            
                                                           
   
              
                                  

               
  
                                                                            */
VOS_STATUS
WDA_DS_FinishULA
(
 void (*callbackRoutine) (void *callbackContext),
 void  *callbackContext
);

/*                                                                          
                                       

             
                                          
    
                                                                           
                            
    
              

            

     
                                
                                    
                                                  
                                           
                                    
                                    
                                                                         
                                  
                                                                  
                              
                        
          
             
             
        

      
                                  

              
                                                          
                                              

              

                                                                            */
VOS_STATUS
WDA_DS_BuildTxPacketInfo
(
  v_PVOID_t       pvosGCtx,
  vos_pkt_t*      vosDataBuff,
  v_MACADDR_t*    pvDestMacAddr,
  v_U8_t          ucDisableFrmXtl,
  v_U16_t*        pusPktLen,
  v_U8_t          ucQosEnabled,
  v_U8_t          ucWDSEnabled,
  v_U8_t          extraHeadSpace,
  v_U8_t          typeSubtype,
  v_PVOID_t       pAddr2,
  v_U8_t          uTid,
  v_U8_t          txFlag,
  v_U32_t         timeStamp,
  v_U8_t          ucIsEapol,
  v_U8_t          ucUP
);

/*                                                                          
                                      

             
                                                      
    
                                                
    
              

            

     
                                    

                                                   
                                                                       
                                                       
      
                                        

              
                                                          
                                              

              

                                                                            */
VOS_STATUS
WDA_DS_PeekRxPacketInfo
(
  vos_pkt_t *vosDataBuff,
  v_PVOID_t *ppRxHeader,
  v_BOOL_t  bSwap
);

/*                                                                          
                                      

             
                                                    
                                       
    
              

            

     
                                    

              
                                                          
                                              

              

                                                                            */
VOS_STATUS
WDA_DS_TrimRxPacketInfo
( 
  vos_pkt_t *vosDataBuff
);

/*                                                                          
                                    

             
                                                                             
                                                                

                                                                 
    
              

            

     
                                    
   
      
                                                 

              
                                                          
                                              

              

                                                                            */
VOS_STATUS
WDA_DS_GetTxResources
( 
  v_PVOID_t pvosGCtx,
  v_U32_t*  puResCount
);

/*                                                                          
                             

             
             

                                                                            
                                

              

            

     
                                    

      
                         

              
                                                          
                                              

              

                                                                            */
VOS_STATUS
WDA_DS_GetRssi
(
  v_PVOID_t pvosGCtx,
  v_S7_t*   puRssi
);

/*                                                                          
                                  

             
                                                                           
                     

                                       

              

            

     
                                
                                      

      

              
                                                          
                                              

              

                                                                            */
VOS_STATUS
WDA_DS_RxAmsduBdFix
(
  v_PVOID_t pvosGCtx,
  v_PVOID_t pvBDHeader
);

/*                                                                          
                                         

             
                                                                    
                                                                        
                                                                    
             

                                                   
                                                                    

              

            

     
                                
                                    
                                                      

      
                                

              
                                                          
                                              

              

                                                                            */
VOS_STATUS
WDA_DS_GetFrameTypeSubType
(
  v_PVOID_t pvosGCtx,
  vos_pkt_t *vosDataBuff,
  v_PVOID_t pRxHeader,
  v_U8_t    *ucTypeSubtype
);

/*                                                                          
                                      

             
                                                        

              

            

     
                                                      

      

              
                  

              

                                                                            */
v_U64_t
WDA_DS_GetReplayCounter
(
  v_PVOID_t pRxHeader
);

/*                                                                          
                                      

             
                                                                          

      
                                                          

              

            

     
                                       
                                                           

      

              
                                                          
                                              

              

                                                                            */
VOS_STATUS
WDA_DS_SetRSSIThresholds
(
  tpAniSirGlobal      pMac,
  tpSirRSSIThresholds pThresholds
);

/*                                                                          
                              

             
                                                                   
                         

                                                                    

      
                                                         

              

            

     
                                

      

              
                                                          
                                              

              

                                                                            */
VOS_STATUS
WDA_DS_TxFrames
( 
  v_PVOID_t pvosGCtx 
);

/*                                                                          
                                           

             
                                      

              

            

     
                                
                                                     
                                                  

      

              

              

                                                                            */
v_VOID_t
WDA_DS_TxFlowControlCallback
(
 v_PVOID_t pvosGCtx,
 v_U8_t    uFlowMask
);

/*                                                                          
                                   

             
                                     

              

            

     
                                

      
                                                     
                                                  

              
                                                          
                                              

              

                                                                            */
VOS_STATUS
WDA_DS_GetTxFlowMask
(
 v_PVOID_t pvosGCtx,
 v_U8_t*   puFlowMask
);

/*                                                                          
                                

             
                            
    
              

            

     
                                       
                                      
                                             
                                             
                                             
                                             

      
                                                    

              
                                                          
                                              

              

                                                                            */
VOS_STATUS WDA_HALDumpCmdReq(tpAniSirGlobal   pMac,tANI_U32 cmd, 
                 tANI_U32   arg1, tANI_U32   arg2, tANI_U32   arg3,
                 tANI_U32   arg4, tANI_U8   *pBuffer);

/*                                                                          
                                      

             
                                                             

              

            

     
                                   

      
        

              
        
    
              
                                                                            */
void WDA_featureCapsExchange(v_PVOID_t pVosContext);

void WDA_disableCapablityFeature(tANI_U8 feature_index);
/*                                                                          
                                      

             
                                                                            
                                           

              

            

     
                                                                                                

      
        

              
                                        
                                                     
    
              
                                                                            */
tANI_U8 WDA_getHostWlanFeatCaps(tANI_U8 featEnumValue);

/*                                                                          
                                    

             
                                                                            
                                         

              

            

     
                                                                                                

      
        

              
                                        
                                                     
    
              
                                                                            */
tANI_U8 WDA_getFwWlanFeatCaps(tANI_U8 featEnumValue);

/*                                                                          
                                      

             
                                                   
                                                    
                                                            

            
                                    
                                                 
                                                  
                                                      
                                               
                                       

              
        

                                                                           */
void WDA_TransportChannelDebug
(
  tpAniSirGlobal pMac,
  v_BOOL_t       displaySnapshot,
  v_U8_t         debugFlags
);

/*                                                                          
                                          

             
                                                                                                 
            
                              

              
        

                                                                           */
void WDA_TrafficStatsTimerActivate(wpt_boolean activate);

/*                                                                          
                             

             
                                            

            
                                  

              
        

                                                                           */
void WDA_SetEnableSSR(v_BOOL_t enableSSR);

#endif

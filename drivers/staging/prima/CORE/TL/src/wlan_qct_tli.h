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


#ifndef WLAN_QCT_TLI_H
#define WLAN_QCT_TLI_H

/*                                                                           

                                                      
                                            


           
                                                                         
               

                                                                           */


/*                                                                           

                                           


                                                                       
                                                                


                                   


                                   
                                                                             
                                                             
                                                                 
                                         
                                        
                                       
                                              
                                                   
                                        
                                                   
                                                    
                                     
                                               
                                           
                                                
                                   

                                                                           */



/*                                                                           

                                                  

                                                                           */

/*                                                                            
                
                                                                            */
#include "vos_packet.h"
#include "vos_api.h"
#include "vos_timer.h"
#include "vos_mq.h"
#include "vos_list.h"
#include "wlan_qct_tl.h"
#include "pmcApi.h"
#include "wlan_qct_hal.h"


#define STATIC  static
/*                                                                            
                                         
                                                                            */

/*                       */
#define WLAN_MAX_TID                          8

/*                                                  */
#define WLANTL_LLC_OUI_OFFSET                 3

/*                                                     */
#define WLANTL_LLC_OUI_SIZE                   3

/*                                                            */
#define WLANTL_LLC_PROTO_TYPE_OFFSET  (WLANTL_LLC_OUI_OFFSET +  WLANTL_LLC_OUI_SIZE)

/*                                                          */
#define WLANTL_LLC_PROTO_TYPE_SIZE            2

/*                     */
#define WLANTL_LLC_8021X_TYPE            0x888E

/*                   */
#define WLANTL_LLC_WAI_TYPE              0x88b4

#ifdef FEATURE_WLAN_TDLS
#define WLANTL_LLC_TDLS_TYPE             0x890d
#endif

/*                                               */
#define WLANTL_AMSDU_SUBFRAME_LEN_OFFSET     12

/*                        */
#define  WLANTL_802_3_HEADER_LEN             14

/*                                     */
#define  WLANTL_802_3_HEADER_DA_OFFSET        0

/*                                                             */
#define  WLANTL_802_11_HEADER_LEN            24

/*                                     */
#define  WLANTL_MPDU_HEADER_LEN              32

/*                         */
#define  WLANTL_802_11_MAX_HEADER_LEN        40

/*                                              */
#define  WLANTL_802_11_HEADER_QOS_CTL         2

/*                                             */
#define  WLANTL_802_11_HEADER_HT_CTL          4

/*                                         */
#define  WLANTL_802_11_HEADER_ADDR1_OFFSET    4

/*                                */
#define  WLANTL_802_11_HEADER_ADDR4_LEN       VOS_MAC_ADDR_SIZE

/*                              */
#define TL_AMSDU_SUBFRM_HEADER_LEN           14

/*                         */
#define WLANTL_LLC_HEADER_LEN   8

/*                   */
#define WLANTL_MGMT_FRAME_TYPE       0x00
#define WLANTL_CTRL_FRAME_TYPE       0x10
#define WLANTL_DATA_FRAME_TYPE       0x20

/*                                                 */
#define WLANTL_80211_DATA_TYPE         0x02
#define WLANTL_80211_DATA_QOS_SUBTYPE  0x08
#define WLANTL_80211_NULL_QOS_SUBTYPE  0x0C

/*                                       */
#define WLANTL_FRAME_TYPE_BCAST 0xff
#define WLANTL_FRAME_TYPE_MCAST 0x01
#define WLANTL_FRAME_TYPE_UCAST 0x00


/*                                                                         
                                                                           
                                                                           */

/*                                                                         
               
                                                                           */
 /*                         */
#define WLANTL_STA_ID_INVALID( _staid )( _staid >= WLAN_MAX_STA_COUNT )

/*                      */
#define WLANTL_STA_ID_BCAST     0xFF

/*                   */
#define WLANTL_TID_INVALID( _tid )     ( _tid >= WLAN_MAX_TID )

/*                  */
#define WLANTL_AC_INVALID( _tid )     ( _tid >= WLANTL_MAX_AC )

/*                                                           */
#define WLANTL_MAC_ADDR_ALIGN( _dxtl )                                    \
      ( ( 0 == _dxtl ) ?                              \
        WLANTL_802_3_HEADER_DA_OFFSET: WLANTL_802_11_HEADER_ADDR1_OFFSET )

/*                                                        */
#define WLANTL_MAC_HEADER_LEN( _dxtl)                                     \
      ( ( 0 == _dxtl )?                               \
         WLANTL_802_3_HEADER_LEN:WLANTL_802_11_HEADER_LEN )

/*                                                         
                                                                         
                                             */
#define WLANTL_BD_HEADER_LEN( _dxtl )                                    \
      ( ( 0 == _dxtl )?                               \
         (WLANHAL_TX_BD_HEADER_SIZE+WLANTL_802_11_MAX_HEADER_LEN): WLANHAL_TX_BD_HEADER_SIZE )


#define WLAN_TL_CEIL( _a, _b)  (( 0 != (_a)%(_b))? (_a)/(_b) + 1: (_a)/(_b))

/*                                             */
#define VOS_GET_TL_CB(_pvosGCtx) \
  ((WLANTL_CbType*)vos_get_context( VOS_MODULE_ID_TL, _pvosGCtx))

/*                                                                */
#define WLANTL_BAP_IS_NON_DATA_PKT_TYPE(usType) \
  ((WLANTL_BT_AMP_TYPE_AR == usType) || (WLANTL_BT_AMP_TYPE_SEC == usType) || \
   (WLANTL_BT_AMP_TYPE_LS_REQ == usType) || (WLANTL_BT_AMP_TYPE_LS_REP == usType))

/*                      */
/*                                                */
#define WLAN_TL_RSSI_CORRECTION 100
#define WLANTL_GETRSSI0(pBD)    (WDA_GETRSSI0(pBD) - WLAN_TL_RSSI_CORRECTION)

/*                      */
#define WLANTL_GETRSSI1(pBD)    (WDA_GETRSSI1(pBD) - WLAN_TL_RSSI_CORRECTION)

#define WLANTL_GETSNR(pBD)      WDA_GET_RX_SNR(pBD)

/*                                                                */
#define WLANTL_BAP_IS_NON_DATA_PKT_TYPE(usType) \
  ((WLANTL_BT_AMP_TYPE_AR == usType) || (WLANTL_BT_AMP_TYPE_SEC == usType) || \
   (WLANTL_BT_AMP_TYPE_LS_REQ == usType) || (WLANTL_BT_AMP_TYPE_LS_REP == usType))

/*                                                                           
                          
                                                                           */
typedef enum
{
  /*                                                                 */
  WLANTL_TX_SIG_SUSPEND = 0,

  /*                                                                      
                             */
  WLANTL_TX_RES_NEEDED  = 1,

  /*                             */
  WLANTL_TX_FWD_CACHED  = 2,

  /*                                */
  WLANTL_TX_STAID_AC_IND = 3,

  /*                               */
  WLANTL_TX_START_XMIT = 4,

  /*                                            */
  WLANTL_FINISH_ULA   = 5,

  /*                                        */
  WLANTL_TX_SNAPSHOT = 6,

  WLANTL_TX_MAX
}WLANTL_TxSignalsType;

/*                                                                           
                
                                                                           */
typedef enum
{
  /*                      */
  WLANTL_TX_EVENT = 0,

  /*                     */
  WLANTL_RX_EVENT = 1,

  WLANTL_MAX_EVENT
}WLANTL_STAEventType;

/*                                                                           

             
                                                                       
            

            

     
                                                                           
                                                                   
                                                             
                                                  

              
                                                            

                                                                           */
typedef VOS_STATUS (*WLANTL_STAFuncType)( v_PVOID_t     pAdapter,
                                          v_U8_t        ucSTAId,
                                          vos_pkt_t**   pvosDataBuff,
                                          v_BOOL_t      bForwardIAPPwithLLC);

/*                                                                           
                    
                                                                           */
typedef struct
{
  WLANTL_STAFuncType  pfnSTATbl[WLANTL_MAX_EVENT];
} WLANTL_STAFsmEntryType;

/*                                               */
VOS_STATUS WLANTL_STARxConn( v_PVOID_t     pAdapter,
                             v_U8_t        ucSTAId,
                             vos_pkt_t**   pvosDataBuff,
                             v_BOOL_t      bForwardIAPPwithLLC);

/*                                                */
VOS_STATUS WLANTL_STATxConn( v_PVOID_t     pAdapter,
                             v_U8_t        ucSTAId,
                             vos_pkt_t**   pvosDataBuff,
                             v_BOOL_t      bForwardIAPPwithLLC);

/*                                                  */
VOS_STATUS WLANTL_STARxAuth( v_PVOID_t     pAdapter,
                             v_U8_t        ucSTAId,
                             vos_pkt_t**   pvosDataBuff,
                             v_BOOL_t      bForwardIAPPwithLLC);

/*                                                   */
VOS_STATUS WLANTL_STATxAuth( v_PVOID_t     pAdapter,
                             v_U8_t        ucSTAId,
                             vos_pkt_t**   pvosDataBuff,
                             v_BOOL_t      bForwardIAPPwithLLC);

/*                                                */
VOS_STATUS WLANTL_STARxDisc( v_PVOID_t     pAdapter,
                             v_U8_t        ucSTAId,
                             vos_pkt_t**   pvosDataBuff,
                             v_BOOL_t      bForwardIAPPwithLLC);

/*                                                 */
VOS_STATUS WLANTL_STATxDisc( v_PVOID_t     pAdapter,
                             v_U8_t        ucSTAId,
                             vos_pkt_t**   pvosDataBuff,
                             v_BOOL_t      bForwardIAPPwithLLC);

/*                  */
STATIC const WLANTL_STAFsmEntryType tlSTAFsm[WLANTL_STA_MAX_STATE] =
{
  /*                 */
  { {
    NULL,      /*                                                    */
    NULL,      /*                                                       */
  } },

  /*                      */
  { {
    WLANTL_STATxConn,      /*                                                       */
    WLANTL_STARxConn,      /*                                                     */
  } },

  /*                          */
  { {
    WLANTL_STATxAuth,      /*                                          */
    WLANTL_STARxAuth,      /*                                             */
  } },

  /*                         */
  { {
    WLANTL_STATxDisc,      /*                              */
    WLANTL_STARxDisc,      /*                                              */
  } }
};

/*                                                                           
                        
                                                                           */

#define WLANTL_MAX_WINSIZE      64
#define WLANTL_MAX_BA_SESSION   40

typedef struct
{
   v_BOOL_t     isAvailable;
   v_U64_t      ullReplayCounter[WLANTL_MAX_WINSIZE];
   v_PVOID_t    arrayBuffer[WLANTL_MAX_WINSIZE];
} WLANTL_REORDER_BUFFER_T;


/*                                         
                                                                   
                                                                   
 */
typedef struct
{
   /*                                    */
   v_PVOID_t          pAdapter;

   /*                   */
   v_PVOID_t          pTLHandle;

   /*                                    */
   v_U32_t            STAID;

   v_U8_t             TID;
} WLANTL_TIMER_EXPIER_UDATA_T;

typedef struct
{
  /*                                    */
  v_U8_t             ucExists;

  /*               */
  v_U32_t             ucCIndex;

  /*                                   */
  v_U16_t            usCount;

  /*                                       */
  vos_timer_t        agingTimer;

  /*               */
  v_U32_t            winSize;

  /*                                */
  v_U32_t            bufferSize;

  /*                       */
  v_U32_t            SSN;

  /*                                */
  v_U32_t            sessionID;

  v_U32_t            currentESN;

  v_U32_t            pendingFramesCount;

  vos_lock_t         reorderLock;

  /*                                */
  WLANTL_TIMER_EXPIER_UDATA_T timerUdata;

  WLANTL_REORDER_BUFFER_T     *reorderBuffer;

  v_U16_t            LastSN;
}WLANTL_BAReorderType;


/*                                                                           
                   
                                                                           */
typedef struct
{
  /*                                                                         */
  v_U8_t              ucSet;
}WLANTL_UAPSDInfoType;

/*                                                                           
                 
                                                                           */
typedef struct
{
  /*                                                                
                */
  v_U8_t                        ucExists;

  /*                                                                    
                                                                            
                                                                     
                                                                             
    */
  v_U8_t                        ucRxBlocked;

  /*                                                         */
  WLANTL_STARxCBType            pfnSTARx;

  /*                                                               
             */
  WLANTL_TxCompCBType           pfnSTATxComp;

  /*                                                         */
  WLANTL_STAFetchPktCBType      pfnSTAFetchPkt;

  /*                                    */
  WLANTL_BAReorderType          atlBAReorderInfo[WLAN_MAX_TID];

  /*                                                                   */
  WLAN_STADescType              wSTADesc;

  /*                                       */
  WLANTL_STAStateType           tlState;

  /*                  */
  WLANTL_STAPriorityType        tlPri;

  /*                                             */
  v_S7_t                        rssiAvg;

  /*                                                     */
  v_S7_t                        rssiAvgBmps;

  /*                                              */
  v_S7_t                        rssiAlpha;

  /*                                             */
  v_U32_t                       uLinkQualityAvg;

  /*                                                    */
  v_U32_t                       snrSum;

  /*                                                              */
  v_S7_t                        snrIdx;

  /*                                   */
  v_S7_t                        prevSnrAvg;

  /*                            */
  v_S7_t                        snrAvgBmps;

  /*                                     */
  v_U32_t                       auTxCount[WLAN_MAX_TID];

  /*                                     */
  v_U32_t                       auRxCount[WLAN_MAX_TID];

  /*              */
  v_U8_t                        ucTxSuspended;

  /*                                                                  
                           */
  vos_pkt_t*                    vosAMSDUChainRoot;

  /*                                  */
  vos_pkt_t*                    vosAMSDUChain;

  /*                                                                      */
  v_U8_t                        aucMPDUHeader[WLANTL_MPDU_HEADER_LEN];

  /*                      */
  v_U8_t                        ucMPDUHeaderLen;

  /*                                                             */
  v_U8_t                        aucACMask[WLANTL_MAX_AC];

  /*                            */
  WLANTL_ACEnumType             ucCurrentAC;

  /*                                                          */
  v_U8_t                        ucPktPending;

  /*                                                                          */
  v_U8_t                        ucEapolPktPending;

  /*                                                                    
                                         */
  v_U8_t                    ucNoMoreData;

  /*                                  */
  WLANTL_ACEnumType             ucServicedAC;

   /*                           */
  v_U8_t                        ucCurrentWeight;

  /*                                               */
  WLANTL_UAPSDInfoType          wUAPSDInfo[WLANTL_MAX_AC];

  /*                                               */
  v_U8_t                        ucPendingTrigFrm;

  WLANTL_TRANSFER_STA_TYPE      trafficStatistics;

  /*                                       */
  /*                                    */
  vos_pkt_t*                 vosBegCachedFrame;

  /*                                    */
  vos_pkt_t*                 vosEndCachedFrame;


  /*                    */

  //                                             
  v_BOOL_t ucLwmModeEnabled;
  //                                                                     
  //                                          
  v_BOOL_t ucLwmEventReported;

  //                         

  /*                     */
  //                             
  v_U8_t bmuMemConsumed;

  //                                         
  v_U32_t uIngress_length;

  //                                                                                   
  v_U32_t uBuffThresholdMax;


  //                        

  //                         

  //                           

  //                            

  /*                                                                            
                                                                                     
                     */
  v_U32_t uLwmThreshold;

  //                                  
  v_U16_t fcStaTxDisabled;

  /*                                                   
                                                         
                                                          
                                                            
                      */
  //                                          

  /*                                                */
  vos_list_t pStaManageQ;

 /*                                                
                                                      */
  v_BOOL_t      ucIsReplayCheckValid;

 /*                                          */
  v_U64_t       ullReplayCounter[WLANTL_MAX_TID];

 /*                                                
                                     */
  v_U32_t       ulTotalReplayPacketsDetected;

 /*                                                       
                                                         
                                    */
  v_U8_t ptkInstalled;

  v_U32_t       linkCapacity;
}WLANTL_STAClientType;

/*                                                                           
                 
                                                                           */
typedef struct
{
  /*                                                                       
             */
  v_U8_t                    ucExists;

  /*                                                                      */
  WLANTL_BAPRxCBType        pfnTlBAPRx;

  /*                                                  */
  WLANTL_FlushOpCompCBType    pfnFlushOpCompleteCb;

  /*                                                           */
  vos_pkt_t*                vosPendingDataBuff;

  /*                */
  v_U8_t                    ucBAPSTAId;
}WLANTL_BAPClientType;


/*                                                                           
                              
                                                                           */
typedef struct
{
  /*                                                                 
                    */
  v_U8_t                       ucExists;

  /*                                                                 */
  WLANTL_MgmtFrmRxCBType       pfnTlMgmtFrmRx;

  /*                                                      */
  vos_pkt_t*                   vosPendingDataBuff;
}WLANTL_MgmtFrmClientType;

typedef struct
{
   WLANTL_TrafficStatusChangedCBType  trafficCB;
   WLANTL_HO_TRAFFIC_STATUS_TYPE      trafficStatus;
   v_U32_t                            idleThreshold;
   v_U32_t                            measurePeriod;
   v_U32_t                            rtRXFrameCount;
   v_U32_t                            rtTXFrameCount;
   v_U32_t                            nrtRXFrameCount;
   v_U32_t                            nrtTXFrameCount;
   vos_timer_t                        trafficTimer;
   v_PVOID_t                          usrCtxt;
} WLANTL_HO_TRAFFIC_STATUS_HANDLE_TYPE;

typedef struct
{
   v_S7_t                          rssiValue;
   v_U8_t                          triggerEvent[WLANTL_HS_NUM_CLIENT];
   v_PVOID_t                       usrCtxt[WLANTL_HS_NUM_CLIENT];
   v_U8_t                          whoIsClient[WLANTL_HS_NUM_CLIENT];
   WLANTL_RSSICrossThresholdCBType crossCBFunction[WLANTL_HS_NUM_CLIENT];
   v_U8_t                          numClient;
} WLANTL_HO_RSSI_INDICATION_TYPE;

typedef struct
{
   v_U8_t                             numThreshold;
   v_U8_t                             regionNumber;
   v_S7_t                             historyRSSI;
   v_U8_t                             alpha;
   v_U32_t                            sampleTime;
   v_U32_t                            fwNotification;
} WLANTL_CURRENT_HO_STATE_TYPE;

typedef struct
{
   WLANTL_HO_RSSI_INDICATION_TYPE       registeredInd[WLANTL_MAX_AVAIL_THRESHOLD];
   WLANTL_CURRENT_HO_STATE_TYPE         currentHOState;
   WLANTL_HO_TRAFFIC_STATUS_HANDLE_TYPE currentTraffic;
   v_PVOID_t                            macCtxt;
   vos_lock_t                           hosLock;
} WLANTL_HO_SUPPORT_TYPE;

/*                                                                           
                       
                                                                           */
typedef struct
{
  /*                              */
  WLANTL_ConfigInfoType     tlConfigInfo;

  /*                             */
  WLANTL_STAClientType*      atlSTAClients[WLAN_MAX_STA_COUNT];


  /*                                            */
  WLANTL_MgmtFrmClientType  tlMgmtFrmClient;

  /*                                  */
  WLANTL_BAPClientType      tlBAPClient;

  /*                                                                    */
  v_U16_t                   usPendingTxCompleteCount;

  /*                     */
  v_U8_t                    ucTxSuspended;

  /*               */
  v_U32_t                   uResCount;

  /*                                    */
  vos_pkt_t*                vosDummyBuf;

  /*                                                             */
  vos_pkt_t*                vosTempBuf;

  /*                                                          */
  v_U8_t                    ucCachedSTAId;
  v_U8_t                    ucCachedAC;

  /*                                                                    
                           */
  v_U8_t                    ucRegisteredStaId;

  /*                          */
  v_U8_t                    ucCurrentSTA;

  WLANTL_REORDER_BUFFER_T   *reorderBufferPool; /*                                                      */

  WLANTL_HO_SUPPORT_TYPE    hoSupport;

  v_BOOL_t                  bUrgent;


  /*               */
  v_U32_t bd_pduResCount;

  /*                                   */
  //                                    

#if 0
  //                                   
  tFcTxParams_type tlFCInfo;
#endif

  vos_pkt_t*                vosTxFCBuf;

  /*                                                                                     */
  //                      

  /*                              */
  //                               

  /*                                                      
                                                                                           
                                                                                         
                                                                                          
             

                                                                                            
                                                                                                  
                                                                                                  
                                           
      */

  /*                                                        */
  //                 

  /*                                                                          */
  WLANTL_ACEnumType uCurServedAC;

  /*                                                      */
  v_U8_t ucCurLeftWeight;

  /*                                                                                     
                         */
  vos_list_t pMCBCManageQ;

  v_U32_t sendFCFrame;

  v_U8_t done_once;
  v_U8_t uFramesProcThres;
#ifdef FEATURE_WLAN_TDLS
  /*                                           
                                                                     
                                                                   */
  v_U8_t        ucTdlsPeerCount;
#endif
  /*                                                                                */
  v_BOOL_t                  isBMPS;
  /*                                                    */
  v_BOOL_t   isTxTranmitMsgPending;
}WLANTL_CbType;

/*                                                                          

                              

             

                                                                      
                                                                         
                                                                        

              

                                                                          
                  

            

      
                                                                           
                                                                            
                                                            
                                                                
                                      

       
                                                                         
                                                                        
                                                        

              

                                                            

                                                                              
                                                                                     
                                       

              

                                                                            */
v_U32_t
WLANTL_GetFrames
(
  v_PVOID_t       pAdapter,
  vos_pkt_t     **ppFrameDataBuff,
  v_U32_t         uSize,
  v_U8_t          uFlowMask,
  v_BOOL_t*       pbUrgent
);

/*                                                                          

                           

             
                                                                          
                                        

              
                                                             

            

      
                                                                           
                                                                            
                                                                          
                                                                           
                                                                   
                                                                           
                                                                    

              
                                                            

              

                                                                            */
VOS_STATUS
WLANTL_TxComp
(
  v_PVOID_t      pAdapter,
  vos_pkt_t     *pFrameDataBuff,
  VOS_STATUS     wTxStatus
);

/*                                                                          

                             

             
                                                                         
                                                                           
                                                                         
                                          

              
                                                             
                                                                         
                                                             

            

      
                                                                           
                                                                            

                                                                          
                                                                         
                                       

              
                                                            

              

                                                                            */
VOS_STATUS
WLANTL_RxFrames
(
  v_PVOID_t      pAdapter,
  vos_pkt_t     *pFrameDataBuff
);

/*                                                                          

                                   

             
                                                                            
            

              
                                                             
                                                                         
                                                             

            

      
                                 

                                                                

                                                                       
                                                                          
                                              

              
                                                            

                                                      
                                                                         
                                    
                                               

              

                                                                            */
VOS_STATUS
WLANTL_RxCachedFrames
(
  WLANTL_CbType*  pTLCb,
  v_U8_t          ucSTAId,
  vos_pkt_t*      vosDataBuff
);

/*                                                                          
                               

             
                                                                    

              
                                                                          

            

      
                                                                           
                                                                            
                                                         

              
                                                            

              

                                                                            */
VOS_STATUS
WLANTL_ResourceCB
(
  v_PVOID_t       pAdapter,
  v_U32_t         uCount
);


/*                                                                          
                                       

             
                                                                   
                     

              
                                                                  

            

      
                                                                           
                                                                   
                                                   


              
                                                            

              

                                                                            */
VOS_STATUS
WLANTL_ProcessMainMessage
(
  v_PVOID_t        pAdapter,
  vos_msg_t*       message
);

/*                                                                          
                                     

             
                                                                   
                   

              
                                                                  

            

      
                                                                           
                                                                   
                                                   


              

                                                            
                                              


              

                                                                            */
VOS_STATUS
WLANTL_ProcessTxMessage
(
  v_PVOID_t        pAdapter,
  vos_msg_t*       message
);

/*                                                                          
                                 

             
                                                 

              

            

      
                          

              
                                                            

              

                                                                            */
VOS_STATUS
WLAN_TLGetNextTxIds
(
  v_PVOID_t    pAdapter,
  v_U8_t*      pucSTAId
);

/*                                                                          

                            

             
                           

              

            

      
                                              
                                                                              

              
                                                            

              

                                                                            */
VOS_STATUS
WLANTL_CleanCB
(
  WLANTL_CbType*  pTLCb,
  v_U8_t          ucEmpty
);

/*                                                                          

                             

             
                                   

              

            

      
                                                                           
                                                                   
                                                                      

              
                                                            

              

                                                                            */
VOS_STATUS
WLANTL_CleanSTA
(
  WLANTL_STAClientType*  ptlSTAClient,
  v_U8_t                 ucEmpty
);

/*                                                                          
                                     

             
                                                                            
                                   

              
                                                                  

            

      
                                                                       
                                                                   


              
                                                            

                                                                         
                                    
                                               

                                                                           
                                  
              

                                                                            */
VOS_STATUS
WLANTL_GetTxResourcesCB
(
  v_PVOID_t        pvosGCtx
);

/*                                                                          
                                    

             
                                                                      

              
                                                   

            

     
                                                                          
                                                                  
                                                   

              
                                                            

              

                                                                            */
void
WLANTL_PrepareBDHeader
(
  vos_pkt_t*      vosDataBuff,
  v_PVOID_t*      ppvBDHeader,
  v_MACADDR_t*    pvDestMacAdddr,
  v_U8_t          ucDisableFrmXtl,
  VOS_STATUS*     pvosSTAtus,
  v_U16_t*        usPktLen,
  v_U8_t          ucQosEnabled,
  v_U8_t          ucWDSEnabled,
  v_U8_t          extraHeadSpace
);

/*                                                                          
                                               

             
                                                                           

              


            

     
                                      

                                                                         
                                                     

         
                                                                            

      
                                             

              
              

              

                                                                            */
VOS_STATUS
WLANTL_Translate8023To80211Header
(
  vos_pkt_t*      vosDataBuff,
  VOS_STATUS*     pvosStatus,
  WLANTL_CbType*  pTLCb,
  v_U8_t          *pucStaId,
  WLANTL_MetaInfoType* pTlMetaInfo,
  v_U8_t          *ucWDSEnabled,
  v_U8_t          *extraHeadSpace
);
/*                                                                          
                                               

             
                                                                           

              


            

     
                                      
                                
                                                  
                                                                              

         
                                                                            

      
                                             

              
                           

              

                                                                            */
VOS_STATUS
WLANTL_Translate80211To8023Header
(
  vos_pkt_t*      vosDataBuff,
  VOS_STATUS*     pvosStatus,
  v_U16_t         usActualHLen,
  v_U8_t          ucHeaderLen,
  WLANTL_CbType*  pTLCb,
  v_U8_t          ucSTAId,
  v_BOOL_t	  bForwardIAPPwithLLC
);

/*                                                                          
                                        

             
                                                                           
               

              
                                                                          

            

     
                                                    
                                                            
                                            

         
                                                                         

              
                                                            

                                                  
                                                                 
                                 

              
         
                                                                            */
VOS_STATUS
WLANTL_FindFrameTypeBcMcUc
(
  WLANTL_CbType *pTLCb,
  v_U8_t        ucSTAId,
  vos_pkt_t     *vosDataBuff,
  v_U8_t        *pucBcMcUc
);

/*                                                                          

                                       

             
                                                                             
                                                                       

              

            

             

              
                  

                                                                            */
VOS_STATUS
WLANTL_MgmtFrmRxDefaultCb
(
  v_PVOID_t  pAdapter,
  v_PVOID_t  vosBuff
);

/*                                                                          

                                   

             
                                                                        
                                                                  

              

            

             

              
                  

                                                                            */
VOS_STATUS
WLANTL_BAPRxDefaultCb
(
  v_PVOID_t    pAdapter,
  vos_pkt_t*   vosDataBuff,
  WLANTL_BAPFrameEnumType frameType
);

/*                                                                          

                                   

             
                                                                        
                                                                      
                                                        

              

            

             

              
                  

                                                                            */
VOS_STATUS
WLANTL_STARxDefaultCb
(
  v_PVOID_t               pAdapter,
  vos_pkt_t*              vosDataBuff,
  v_U8_t                  ucSTAId,
  WLANTL_RxMetaInfoType*  pRxMetaInfo
);

/*                                                                          

                                         

             
                                                                       
                                                                         
                                                        

              

            

             

              
                  

                                                                            */
VOS_STATUS
WLANTL_STAFetchPktDefaultCb
(
  v_PVOID_t              pAdapter,
  v_U8_t*                pucSTAId,
  WLANTL_ACEnumType      ucAC,
  vos_pkt_t**            vosDataBuff,
  WLANTL_MetaInfoType*   tlMetaInfo
);

/*                                                                          

                                    

             
                                                                     
                         

            

      
                                                                      
                                                                         
                               
                                                                       
                                              


              
                                                                    
                                                      

                                                                            */
VOS_STATUS
WLANTL_TxCompDefaultCb
(
 v_PVOID_t      pAdapter,
 vos_pkt_t*     vosDataBuff,
 VOS_STATUS     wTxSTAtus
);

/*                                                                          

                                       

             
                                                                            
       

              

            

    
                                                  
                                                                   
                             
                                  

              
        

              

                                                                            */
VOS_STATUS
WLANTL_PackUpTriggerFrame
(
  WLANTL_CbType*            pTLCb,
  WLANTL_TxCompCBType       pfnSTATxComp,
  v_U8_t                    ucSTAId,
  WLANTL_ACEnumType         ucAC
);

/*                                                                          

                                       

             
                                                               
                                 

            

     
                                                                  
                                                                         
                               
                                                                       
                                              


              
                                                            

                                                                             */
VOS_STATUS
WLANTL_TxCompTriggFrameSI
(
  v_PVOID_t      pvosGCtx,
  vos_pkt_t*     vosDataBuff,
  VOS_STATUS     wTxSTAtus
);

/*                                                                          

                                       

             
                                                               
                                 

            

     
                                                                  
                                                                         
                               
                                                                       
                                              


              
                                                            

                                                                            */
VOS_STATUS
WLANTL_TxCompTriggFrameDI
(
 v_PVOID_t      pvosGCtx,
 vos_pkt_t*     vosDataBuff,
 VOS_STATUS     wTxSTAtus
);

/*                                                                          

           

                                               

                                                  

               

                                                                            */
VOS_STATUS WLANTL_ReadRSSI
(
   v_PVOID_t        pAdapter,
   v_PVOID_t        pBDHeader,
   v_U8_t           STAid
);

/*                                                                          

           

                                              

                                                  

               

                                                                            */
VOS_STATUS WLANTL_ReadSNR
(
   v_PVOID_t        pAdapter,
   v_PVOID_t        pBDHeader,
   v_U8_t           STAid
);


void WLANTL_PowerStateChangedCB
(
   v_PVOID_t pAdapter,
   tPmcState newState
);

/*                                                                          
                               

             
                                                                         

              

            

     
                                                                      
                                                                  
                                                            
                                              

              
                                                            

                                                  
                                                                         
                                    
                                               

              

                                                                            */

VOS_STATUS
WLANTL_FwdPktToHDD
(
  v_PVOID_t       pvosGCtx,
  vos_pkt_t*     pvosDataBuff,
  v_U8_t          ucSTAId
);

#endif /*                        */

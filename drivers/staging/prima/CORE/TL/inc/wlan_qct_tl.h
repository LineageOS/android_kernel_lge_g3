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

#ifndef WLAN_QCT_WLANTL_H
#define WLAN_QCT_WLANTL_H

/*===========================================================================

               W L A N   T R A N S P O R T   L A Y E R
                       E X T E R N A L  A P I


DESCRIPTION
  This file contains the external API exposed by the wlan transport layer
  module.
<<<<<<< HEAD:CORE/TL/inc/wlan_qct_tl.h
  
      
  Copyright (c) 2008 QUALCOMM Incorporated. All Rights Reserved.
  Qualcomm Confidential and Proprietary
=======


  Copyright (c) 2008 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary
>>>>>>> 326d6cf... wlan: remove obsolete ANI_CHIPSET_VOLANS featurization:prima/CORE/TL/inc/wlan_qct_tl.h
===========================================================================*/


/*                                                                           

                                           


                                                                       
                                                                


                                   


                                   
                                                                             
                                          
                                                
                                            
                                                                
                                       
                                               
                                                 
                                      
                                   

                                                                           */



/*                                                                           

                                                  

                                                                           */

/*                                                                            
                
                                                                            */
#include "vos_api.h" 
#include "vos_packet.h" 
#include "sirApi.h"
#include "csrApi.h"
#include "sapApi.h"
/*                                                                            
                                         
                                                                            */
 #ifdef __cplusplus
 extern "C" {
 #endif 
 
/*                                                  */
#define WLANTL_LLC_OUI_OFFSET                 3

/*                                                     */
#define WLANTL_LLC_OUI_SIZE                   3

/*                             */
#define WLANTL_LLC_SNAP_OFFSET                0

/*                           */
#define WLANTL_LLC_SNAP_SIZE                   8

/*                                                                            
                                                
                                     
                                                                             */
/*                      */
#define WLANTL_MAX_AC                         4

/*                                                          */
#define WLAN_MAX_STA_COUNT  (HAL_NUM_STA)
#define WLAN_NON32_STA_COUNT   14
/*                                                                      */
#define WLAN_RX_BCMC_STA_ID (WLAN_MAX_STA_COUNT + 1)

/*                                                                                  */
#define WLAN_RX_SAP_SELF_STA_ID (WLAN_MAX_STA_COUNT + 2)

/*                                                                           
                                                                            */
#define WLANTL_RX_WDS_STAID WLAN_MAX_STA_COUNT

/*                                                                                   
                                                                               */
//                              


#define WLANTL_MAX_TID                        15
/*                            */
#define WLANTL_HO_DEFAULT_ALPHA               5
#define WLANTL_HO_TDLS_ALPHA                  7

//                                                                          
//          
#define SNR_HACK_BMPS                         (127)
/*                                                                          
                                 
                                                                    
                                                                           */
typedef enum
{
  WLANTL_AC_BK = 0,
  WLANTL_AC_BE = 1,
  WLANTL_AC_VI = 2,
  WLANTL_AC_VO = 3
}WLANTL_ACEnumType; 

/*                                                                           
          
                                                                           */
typedef enum
{
  /*                          */
  WLAN_STA_INFRA  = 0,  

  /*            */
  WLAN_STA_IBSS,   

  /*            */
  WLAN_STA_BT_AMP,

  /*                */
  WLAN_STA_SOFTAP,

#ifdef FEATURE_WLAN_TDLS
  /*                  */
  WLAN_STA_TDLS,    /*   */
#endif


  /*             */
  WLAN_STA_MAX

}WLAN_STAType;

/*                                                                           
                           
                                                                           */
typedef enum
{
    /*                            */
    WLANTL_BT_AMP_TYPE_DATA = 0x0001,

    /*                                       */
    WLANTL_BT_AMP_TYPE_AR = 0x0002,

    /*                                      */
    WLANTL_BT_AMP_TYPE_SEC = 0x0003,

    /*                                                      */
    WLANTL_BT_AMP_TYPE_LS_REQ = 0x0004,

    /*                                                    */
    WLANTL_BT_AMP_TYPE_LS_REP = 0x0005,

    /*               */
    WLANTL_BAP_INVALID_FRAME

} WLANTL_BAPFrameEnumType;

/*                                         */
typedef enum  {
    WLAN_LWM_THRESHOLD_BYTE = 0,

    WLAN_LWM_THRESHOLD_PACKET
} WLAN_LWM_Threshold_Type;

/*                                                                           
           
                                                                           */
typedef enum
{
  /*                                            */
  WLANTL_STA_INIT = 0,

  /*                                                                      
              */
  WLANTL_STA_CONNECTED,

  /*                                                                     
                        */
  WLANTL_STA_AUTHENTICATED,

  /*                                             */
  WLANTL_STA_DISCONNECTED,

  WLANTL_STA_MAX_STATE
}WLANTL_STAStateType;


/*                                                                           
                     
                                                                           */
typedef struct
{
  /*                                           */
  v_U8_t         ucSTAId; 

  /*               */
  v_MACADDR_t    vSTAMACAddress; 

  /*              */
  v_MACADDR_t    vBSSIDforIBSS; 

  /*                */
  v_MACADDR_t    vSelfMACAddress;

  /*               */
  WLAN_STAType   wSTAType; 

  /*                                                  */
  v_U8_t         ucQosEnabled;

  /*                */
  v_U8_t         ucSwFrameTXXlation; 
  v_U8_t         ucSwFrameRXXlation;

  /*                                                                   
           */
  v_U8_t         ucAddRmvLLC;

 /*                                                     */
  v_U8_t         ucProtectedFrame;

 /*                                                           */
  v_U8_t              ucUcastSig;
 /*                                     */
  v_U8_t         ucIsWapiSta;

#ifdef FEATURE_WLAN_CCX
 /*                                    */
  v_U8_t         ucIsCcxSta;
#endif

  /*                                                             */
  v_U8_t              ucBcastSig;

  /*                                                      */
  WLANTL_STAStateType ucInitState;
 /*                                                
                                                      */ 
  v_BOOL_t      ucIsReplayCheckValid; 
}WLAN_STADescType;

/*                                                                           
                  
                                                                           */      
typedef struct
{
  /*                 */
  v_U8_t   ucAcWeights[WLANTL_MAX_AC]; 

  /*                                                                       
                                                           */
  v_U32_t  uDelayedTriggerFrmInt;  

  /*                                           */
  v_U8_t   uMinFramesProcThres;

  /*                     */
  v_U16_t  ucReorderAgingTime[WLANTL_MAX_AC];
}WLANTL_ConfigInfoType;

/*                                                                           
                           
                                                                           */
typedef enum
{
  /*        */
  WLANTL_TX_DIR = 0,

  /*          */
  WLANTL_RX_DIR = 1,

  /*             */
  WLANTL_BI_DIR = 2,
}WLANTL_TSDirType;

/*                                                                            
                               
                                                                             */



/*                                                                            
                     
                                                                            */

/*                                                                           
                
                                                                           */      
typedef enum
{
  /*               */
  WLANTL_ERROR = 0,      

  /*                                         */
  WLANTL_NO_RX_DATA_CB,  

  /*                                              */
  WLANTL_NO_RX_MGMT_CB,  

  /*                     */
  WLANTL_MEM_ERROR,      

  /*                           */
  WLANTL_BUS_ERROR       

}WLANTL_ErrorType;

/*                                                                           
                   
                                                                           */      
typedef enum 
{
  /*                                  */
  WLANTL_STA_PRI_VERY_LOW  = -2, 

  /*                                 */
  WLANTL_STA_PRI_LOW       = -1, 

  /*                          */
  WLANTL_STA_PRI_NORMAL    =  0, 

  /*                               */ 
  WLANTL_STA_PRI_HIGH      =  1, 

  /*                                     */
  WLANTL_STA_PRI_VERY_HIGH =  2  

}WLANTL_STAPriorityType;

/*                                                                           
                                            
                                                                           */      
typedef struct
{
  /*                              */
  v_U8_t    ucTID;

  /*                             */
  v_U8_t    ucUP;

  /*                                               */
  v_U8_t    ucIsEapol;
#ifdef FEATURE_WLAN_WAPI
  /*                                            */
  v_U8_t    ucIsWai;
#endif
  /*                                                  */
  v_U8_t    ucDisableFrmXtl;

  /*                    */
  v_U8_t    ucBcast;

  /*                    */
  v_U8_t    ucMcast;

  /*            */
  v_U8_t    ucType;

  /*           */
  v_U16_t   usTimeStamp;

  /*                              */
  v_BOOL_t  bMorePackets;
}WLANTL_MetaInfoType;

/*                                                                           
                                                     
                                                                           */      
typedef struct
{
  /*                             */
  v_U8_t    ucUP;
  /*                                        */
  v_U16_t   ucDesSTAId;
 /*                                  */
  v_S7_t    rssiAvg;
 #ifdef FEATURE_WLAN_TDLS
 /*                                        */
  v_U8_t    isStaTdls;
 #endif
}WLANTL_RxMetaInfoType;


/*                                                                           
                                                       
                                                                           */
/*                                          */
#define WLANTL_HO_THRESHOLD_NA    0x00
#define WLANTL_HO_THRESHOLD_DOWN  0x01
#define WLANTL_HO_THRESHOLD_UP    0x02
#define WLANTL_HO_THRESHOLD_CROSS 0x04

/*                         */
typedef enum
{
   WLANTL_HO_RT_TRAFFIC_STATUS_OFF,
   WLANTL_HO_RT_TRAFFIC_STATUS_ON
} WLANTL_HO_RT_TRAFFIC_STATUS_TYPE;

/*                             */
typedef enum
{
   WLANTL_HO_NRT_TRAFFIC_STATUS_OFF,
   WLANTL_HO_NRT_TRAFFIC_STATUS_ON
} WLANTL_HO_NRT_TRAFFIC_STATUS_TYPE;

/*                              */
typedef enum
{
   WLANTL_STATIC_TX_UC_FCNT,
   WLANTL_STATIC_TX_MC_FCNT,
   WLANTL_STATIC_TX_BC_FCNT,
   WLANTL_STATIC_TX_UC_BCNT,
   WLANTL_STATIC_TX_MC_BCNT,
   WLANTL_STATIC_TX_BC_BCNT,
   WLANTL_STATIC_RX_UC_FCNT,
   WLANTL_STATIC_RX_MC_FCNT,
   WLANTL_STATIC_RX_BC_FCNT,
   WLANTL_STATIC_RX_UC_BCNT,
   WLANTL_STATIC_RX_MC_BCNT,
   WLANTL_STATIC_RX_BC_BCNT,
   WLANTL_STATIC_RX_BCNT,
   WLANTL_STATIC_RX_BCNT_CRC_OK,
   WLANTL_STATIC_RX_RATE
} WLANTL_TRANSFER_STATIC_TYPE;

/*                                                                           
                                           
                                                                           */
typedef struct
{
   WLANTL_HO_RT_TRAFFIC_STATUS_TYPE   rtTrafficStatus;
   WLANTL_HO_NRT_TRAFFIC_STATUS_TYPE  nrtTrafficStatus;
} WLANTL_HO_TRAFFIC_STATUS_TYPE;

typedef tSap_SoftapStats WLANTL_TRANSFER_STA_TYPE;

/*                                                    */
/*             */
#define WLANTL_MAX_AVAIL_THRESHOLD   5
#define WLANTL_HS_NUM_CLIENT         2
#define WLANTL_SINGLE_CLNT_THRESHOLD 4

/*                                                                            
                      
                                                                            */

/*                                                                            

                
                                                         
    
                                                                     
                       

             

      
                                                                   
                                                                          
                                
                                                                        
                                               

  
               
                                                            

                                                                            */
typedef VOS_STATUS (*WLANTL_TxCompCBType)( v_PVOID_t      pvosGCtx,
                                           vos_pkt_t*     pFrameDataBuff,
                                           VOS_STATUS     wTxSTAtus );


/*                                                                            
                        
                                                                            */
/*                                                                            

                
                                                          
    
                                                                     
                                                  
                                                                      
                                                


             

      
                                                                
                                                                    
                                     

          
                                                                       
                                                                      
                                                                    
                                                       
                                                                          
                                                                           
                                                  

       
                                                                        
                                                      


  
               
                                                            

                                                                            */
typedef VOS_STATUS (*WLANTL_STAFetchPktCBType)( 
                                            v_PVOID_t             pvosGCtx,
                                            v_U8_t*               pucSTAId,
                                            WLANTL_ACEnumType     ucAC,
                                            vos_pkt_t**           vosDataBuff,
                                            WLANTL_MetaInfoType*  tlMetaInfo);

/*                                                                            

                
                                                     
    
                                                                      
                         

             

      
                                                                   
                                                                      
                                
                                                                    
                                              
                              
                                                        
  
               
                                                            

                                                                            */
typedef VOS_STATUS (*WLANTL_STARxCBType)( v_PVOID_t              pvosGCtx,
                                          vos_pkt_t*             vosDataBuff,
                                          v_U8_t                 ucSTAId,
                                          WLANTL_RxMetaInfoType* pRxMetaInfo);


/*                                                                            
                        
                                                                            */

/*                                                                            

                
                                                             
    
                                                                          
                                                                        

             

      
                                                                       
                                                                            
                                                                              
                                                        
                                                            
  
               
                                                            

                                                                            */
typedef VOS_STATUS (*WLANTL_BAPRxCBType)( v_PVOID_t               pvosGCtx,
                                          vos_pkt_t*              vosDataBuff,
                                          WLANTL_BAPFrameEnumType frameType);

/*                                                                            

                
                                                                     
                                                                             
    
                                                                          
                                                             

             

      
                                                                        
                                                                             
                                                                              
                                                          
  
               
                                                            

                                                                            */
typedef VOS_STATUS (*WLANTL_FlushOpCompCBType)( v_PVOID_t     pvosGCtx,
                                                v_U8_t        ucStaId,
                                                v_U8_t        ucTID, 
                                                v_U8_t        status);
/*                                                                            
                       
                                                                            */

/*                                                                            

                
                                                            
    
                                                                           
                                                                              

             

      
                                                                        
                                                                    
                                                                            
                            
  
               
                                                            

                                                                            */
typedef VOS_STATUS (*WLANTL_MgmtFrmRxCBType)( v_PVOID_t  pvosGCtx, 
                                              v_PVOID_t  vosBuff);


/*                                                                            
                        
                                                                            */

/*                                                                            

              
                                                          
    
                                                                    
                   
    
             

      
                                                                        
                                                                    
                                                                             
                                                                      
                                                                           
                                        
   
              
                                                              
  
                                                                            */
typedef VOS_STATUS (*WLANTL_SuspendCBType)( v_PVOID_t      pvosGCtx,
                                            v_U8_t*        ucSTAId,
                                            VOS_STATUS     vosStatus);


/*                                                                          

              
                                            
                                                                          
                                  

             
                                        
                                                    
                                                
   
              
              

               
        
 
                                                                            */
/*                                                        */
typedef VOS_STATUS (*WLANTL_TrafficStatusChangedCBType)
(
   v_PVOID_t                     pAdapter,
   WLANTL_HO_TRAFFIC_STATUS_TYPE trafficStatus,
   v_PVOID_t                     pUserCtxt
);

/*                                                                          

              
                                                         
                                  

             
                                           
                                             
                                                   

              

               
  
                                                                            */
/*                                                                  */
typedef VOS_STATUS (*WLANTL_RSSICrossThresholdCBType)
(
   v_PVOID_t                       pAdapter,
   v_U8_t                          rssiNotification,
   v_PVOID_t                       pUserCtxt,
   v_S7_t                          avgRssi
);

typedef struct
{
    //                                  
    v_U16_t                         msgType;    //                                         
    v_U16_t                         msgLen;  //                             
    v_U8_t                          sessionId; //              
    v_U8_t                          rssiNotification;    
    v_U8_t                          avgRssi;
    v_PVOID_t                       tlCallback;
    v_PVOID_t                       pAdapter;
    v_PVOID_t                       pUserCtxt;
} WLANTL_TlIndicationReq;

/*                                                                            
                                          
                                                                            */

/*                                                                          

                         

              
                                                                       
                                                                       
                            
    
               
    
             

      
                                                                        
                                                                    
                                     
   
              
                                                              

                                                                              
                                
                                               

               
  
                                                                            */
VOS_STATUS 
WLANTL_Open
( 
  v_PVOID_t               pvosGCtx,
  WLANTL_ConfigInfoType*  pTLConfig
);

/*                                                                          

                          

              
                                                                           
                                                           
    
               
    
             

      
                                                                        
                                                                    
   
              
                                                              

                                                                              
                                
                                               

                                                                          
                 
    
               
  
                                                                            */
VOS_STATUS 
WLANTL_Start
( 
  v_PVOID_t  pvosGCtx 
);

/*                                                                          

                         

              
                                                                             
                                                                             
                   
    
               
    
             

      
                                                                        
                                                                    
   
              
                                                              

                                                                              
                                
                                               

               
  
                                                                            */
VOS_STATUS 
WLANTL_Stop
( 
  v_PVOID_t  pvosGCtx 
);

/*                                                                          

                          

              
                                                                          
                                
    
               
    
             

      
                                                                        
                                                                    
   
              
                                                              

                                                                              
                                
                                               

               
  
                                                                            */
VOS_STATUS 
WLANTL_Close
( 
  v_PVOID_t  pvosGCtx 
);


/*                                                                            
                        
                                                                            */
/*                                                                          

                                                    

             
                                                                               

              

            
     
                                    
                                                                         
     

              

        

                                                                            */
void
WLANTL_ConfigureSwFrameTXXlationForAll
(
  v_PVOID_t pvosGCtx, 
  v_BOOL_t enableFrameXlation
);

/*                                                                           

                                      

              

                                                                           
                                                                       
                                                                           
                     

               

                                                          
     
                 
                                                                        
                                                                       
                                                         

             

                                                                        
                                                                    
                                                                           
                                                                           
                                     
                                                                            
                                                                        
                                 
   
              

                                                              

                                                     
                                                                             
                                                                         
                                                       
                                               
    
               
  
                                                                            */
VOS_STATUS  
WLANTL_RegisterSTAClient 
( 
  v_PVOID_t                 pvosGCtx,
  WLANTL_STARxCBType        pfnSTARx,  
  WLANTL_TxCompCBType       pfnSTATxComp,  
  WLANTL_STAFetchPktCBType  pfnSTAFetchPkt,
  WLAN_STADescType*         wSTADescType ,
  v_S7_t                    rssi
);

/*                                                                           

                                   

              

                                                                         
                        

               

                                                                         
            

             

                                                                        
                                                                    
                                                        
   
              

                                                              
    
                                                                             
                                                                         
                                                    
                                               
    
               
  
                                                                            */
VOS_STATUS  
WLANTL_ClearSTAClient 
( 
  v_PVOID_t        pvosGCtx,
  v_U8_t           ucSTAId 
);

/*                                                                           

                                   

              

                                                                    
                                            

               

                                                                      
           

                                                                         
                                                                           
                                                                         

                                                                             
                   
                                                                             
                 

             

                                                                        
                                                                    
                                                                       
                                                                       

   
              

                                                              
    
                                                      
                                                                              
                                                                          
                                                    
                                               

               
  
                                                                            */
VOS_STATUS  
WLANTL_ChangeSTAState 
( 
  v_PVOID_t             pvosGCtx,
  v_U8_t                ucSTAId,
  WLANTL_STAStateType   tlSTAState 
);

/*                                                                           

                                    

             

                                                                         

              

                                                                      
           

            

                                                                       
                                                                   
                                                                    
                             

              

                                                            

                                                                             
                                                                        
                                                   
                                              

              

                                                                            */
VOS_STATUS
WLANTL_STAPtkInstalled
(
  v_PVOID_t             pvosGCtx,
  v_U8_t                ucSTAId
);
/*                                                                           

                                

             

                                                   

              

                                                                          


            

      
                                                                       
                                                                   
                                             

       
                                                                            


              

                                                            

                                                     
                                                                             
                                                                        
                                                   
                                              

              

                                                                            */
VOS_STATUS
WLANTL_GetSTAState
(
  v_PVOID_t             pvosGCtx,
  v_U8_t                ucSTAId,
  WLANTL_STAStateType   *ptlSTAState
);

/*                                                                           

                                  

              

                                                                        
            

               

                                                                  
                             

                                                                     
                                                                    
                                                                     
                                                                 
                                            

             

                                                                     
                                                                 
                                                                    
                                                       
   
              

                                                              

                                                      
                                                                           
                                                                             
                                                    
                                               

               
  
                                                                            */
VOS_STATUS
WLANTL_STAPktPending 
( 
  v_PVOID_t            pvosGCtx,
  v_U8_t               ucSTAId,
  WLANTL_ACEnumType    ucAc
);

/*                                                                          

                                   

              

                                                                       
                                                                          
                

               

                                                                             
           

             

                                                                        
                                                                    
                                                                      
   
              

                                                              
    
                                                      
                                                                           
                                                                             
                                                    
                                               

               
  
                                                                            */
VOS_STATUS
WLANTL_SetSTAPriority 
( 
  v_PVOID_t                pvosGCtx,
  v_U8_t                   ucSTAId,
  WLANTL_STAPriorityType   tlSTAPri
);

/*                                                                            
                        
                                                                            */

/*                                                                          

                                      

              
                                                                            

               
                                                               
    
             

      
                                                                        
                                                                             
                                                                           
                                  
                          
                                                                          
                                                
   
              
  
                                                              
    
                                                      
                                                                           
                                                                             
                                                          
                                               

               
  
                                                                            */
VOS_STATUS  
WLANTL_RegisterBAPClient 
( 
  v_PVOID_t                   pvosGCtx,
  WLANTL_BAPRxCBType          pfnTlBAPRx,
  WLANTL_FlushOpCompCBType    pfnFlushOpCompleteCb
);


/*                                                                          

                             

              
                                                              

               
                                                                       

                                                                            
                                                                       
                                                                 
                                  

                                                                             
                                                                   

             

      
                                                                        
                                                                             
                                                                          
                               
                                                      
                                                                         
                                                            
   
              
                                                              

                                                                         
                                     
                                                          
                                                                           
                                               

                                                                       
                                                         
    
               
  
                                                                            */
VOS_STATUS  
WLANTL_TxBAPFrm 
( 
  v_PVOID_t               pvosGCtx,
  vos_pkt_t*              vosDataBuff,  
  WLANTL_MetaInfoType*    pMetaInfo,   
  WLANTL_TxCompCBType     pfnTlBAPTxComp
);


/*                                                                            
                        
                                                                            */

/*                                                                          

                            

              
                                                                         
                                                                         
                 

               

                                                                    
                                                                       
                                              
    
             

      
                                                                        
                                                                             
                                                              

       
                                                 

   
              
                                                              

                                                      
                                                                           
                                                                             
                                                   
                                               
    
               
  
                                                                            */
VOS_STATUS  
WLANTL_GetRssi 
( 
  v_PVOID_t             pvosGCtx,
  v_U8_t                ucSTAId,
  v_S7_t*               puRssi
);

/*                                                                          

                           

             
                                                                       
                                                                        
                 

              

                                                                   
                                                                      
                                            

            

      
                                                                       
                                                                            
                                                              

       
                                               


              
                                                            

                                                     
                                                                          
                                                                           
                                                   
                                              

              

                                                                            */
VOS_STATUS
WLANTL_GetSnr
(
  tANI_U8           ucSTAId,
  tANI_S8*          pSnr
);

/*                                                                          

                                   

              
                                                                                
                                                                         
                 

               

                                                                    
                                                                       
                                              
    
             

      
                                                                        
                                                                             
                                                              

       
                                                               

   
              
                                                              

                                                      
                                                                           
                                                                             
                                                   
                                               
    
               
  
                                                                            */
VOS_STATUS  
WLANTL_GetLinkQuality 
( 
  v_PVOID_t             pvosGCtx,
  v_U8_t                ucSTAId,
  v_U32_t*              puLinkQuality
);

/*                                                                          

                                

              
                                                                               
                                                                             
              

               

             

      
                                                                        
                                                                             
                                                              
                                                   

       
                                                                              
                            
   
              
                                               
    
               
                                                                            */
VOS_STATUS  
WLANTL_FlushStaTID 
( 
  v_PVOID_t             pvosGCtx,
  v_U8_t                ucSTAId,
  v_U8_t                ucTid
);

/*                                                                            
                       
                                                                            */

/*                                                                          

                                          

              
                                                                         

               
                                                          
    
             

      
                                                                       
                                                                             
                                                                      
                                         
      
              
                                                              
    
                                                      
                                                                         
                                     
                                                                 
                                               
    
               
  
                                                                            */
VOS_STATUS  
WLANTL_RegisterMgmtFrmClient
( 
  v_PVOID_t               pvosGCtx,
  WLANTL_MgmtFrmRxCBType  pfnTlMgmtFrmRx
);

/*                                                                          

                                            

             
                                                                          

              
                                                         

            

      
                                                                      
                                                                            
              
                                                            

                                                                        
                                   
                                                               
                                              

              

                                                                            */
VOS_STATUS
WLANTL_DeRegisterMgmtFrmClient
(
  v_PVOID_t               pvosGCtx
);

/*                                                                          

                              

              
                                                              
                                                                             
                                                                        
                                       
                                                                 

               
                                                          

                                                                            
                                                                   

                                                                          
                                                                    
    
             

      
                                                                       
                                                                    
                                                                        
                                           
                                                                           
                                                          
                                                           
                                                   
                                                                     
                                                                          
                                                                            
                                                                     
                                                         
                                                                     
                                                                        
                                                                          
                                                                        
                              

   
              
                                                              

                                                      
                                                                         
                                     
                                                                 
                                                                         
                                               

                                                                       
                                                         
    
               
  
                                                                            */
VOS_STATUS 
WLANTL_TxMgmtFrm
( 
  v_PVOID_t            pvosGCtx,  
  vos_pkt_t*           vosFrmBuf,
  v_U16_t              usFrmLen,
  v_U8_t               ucFrmType, 
  v_U8_t               tid,
  WLANTL_TxCompCBType  pfnCompTxFunc,
  v_PVOID_t            voosBDHeader,
  v_U8_t               ucAckResponse
);


/*                                                                            
                        
                                                                            */

/*                                                                          

                                      

              
                                                   
                       
    
               
    
             

      
                                                                        
                                                                    

   
              
                                                              

                                                                         
                                     
                                               
    
               
  
                                                                            */
VOS_STATUS
WLANTL_ResetNotification
( 
  v_PVOID_t   pvosGCtx 
);

/*                                                                          

                                  

              
                                                                    
                   
    
               
                                                                        
               

                                                                            
                                                                      
                                                                         
                                                                

             

      
                                                                        
                                                                    
                                                                             
                                                                         
                                                                           
                                        

   
              
                                                              

                                                                            
                                                                              
                                                     
                                               
    
               
  
                                                                            */
VOS_STATUS
WLANTL_SuspendDataTx
( 
  v_PVOID_t               pvosGCtx,
  v_U8_t*                 ucSTAId,
  WLANTL_SuspendCBType    pfnSuspendTx
);

/*                                                                          

                                 

              
                                                               

                                                                          
                                    
             
               
    
             

      
                                                                        
                                                                    
                                                                          
                                                 
   
              
                                                              

                                                                            
                                                                              
                                                     
                                               
    
               
  
                                                                            */
VOS_STATUS
WLANTL_ResumeDataTx
( 
  v_PVOID_t      pvosGCtx,
  v_U8_t*        pucSTAId 
);


/*                                                                            
                                
                                                                            */

/*                                                                          

                                  

              
                                                                  
                 
    
               
    
             

      
                                                                        
                                                                    
                                              
                                            

       
                                                                        
   
              
                                                              
    
                                                       
                                                                            
                                                                              
                                                     
                                               
    
               
  
                                                                            */
VOS_STATUS
WLANTL_GetTxPktCount
( 
  v_PVOID_t      pvosGCtx,
  v_U8_t         ucSTAId,
  v_U8_t         ucTid,
  v_U32_t*       puTxPktCount
);

/*                                                                          

                                  

              
                                                               
                 
    
               
    
             

      
                                                                        
                                                                    
                                              
                                            
   
      
                                                                        
   
              
                                                              

                                                       
                                                                            
                                                                              
                                                     
                                               
    
               
  
                                                                            */
VOS_STATUS
WLANTL_GetRxPktCount
( 
  v_PVOID_t      pvosGCtx,
  v_U8_t         ucSTAId,
  v_U8_t         ucTid,
  v_U32_t*       puRxPktCount
);

/*                                                                          
                              
                                                                            */

/*                                                                          
                                 

              
                                                                   
                      

               
                                                                   
    
             

      
                                                                        
                                                                    
                                                    
                    
   
              
                                                              

                                                  
                                                                          
                                      
                                               
   
               
  
                                                                            */
VOS_STATUS
WLANTL_McProcessMsg
(
  v_PVOID_t        pvosGCtx,
  vos_msg_t*       message
);

/*                                                                          
                              

              
                                                                            
                                                                           
                                                                            
               

               
                                                                   
    
             

      
                                                                        
                                                                    
                                                    
                    
   
              
                                                              

                                               

               
  
                                                                            */
VOS_STATUS
WLANTL_McFreeMsg
(
  v_PVOID_t        pvosGCtx,
  vos_msg_t*       message
);

/*                                                                          
                                 

              
                                                                   
                    

               
                                                                   
    
             

      
                                                                        
                                                                    
                                                    
                    
   
              
                                                              

                                                  
                                                                          
                                      
                                               

                                                                            
                                   
               
  
                                                                            */
VOS_STATUS
WLANTL_TxProcessMsg
(
  v_PVOID_t        pvosGCtx,
  vos_msg_t*       message
);

/*                                                                          
                              

              
                                                                            
                                                                           
                                                                            
               

               
                                                                   
    
             

      
                                                                        
                                                                    
                                                    
                    
   
              
                                                              

                                               

               
  
                                                                            */
VOS_STATUS
WLANTL_TxFreeMsg
(
  v_PVOID_t        pvosGCtx,
  vos_msg_t*       message
);


/*                                                                          
                                     

              
                                                                            
           

               
                                                                   
    
             

      
                                                                        
                                                                    
                               
                                                          
                                 
                                                                      
                                                                     
                                                                              
                                       
   
              
                                                              

                                               

               
  
                                                                            */
VOS_STATUS
WLANTL_EnableUAPSDForAC
(
  v_PVOID_t          pvosGCtx,
  v_U8_t             ucSTAId,
  WLANTL_ACEnumType  ucACId,
  v_U8_t             ucTid,
  v_U8_t             ucUP,
  v_U32_t            uServiceInt,
  v_U32_t            uSuspendInt,
  WLANTL_TSDirType   wTSDir
);


/*                                                                          
                                      

              
                                                                      
           

               
                                                                   
    
             

      
                                                                        
                                                                    
                               
                                                               
   
   
              
                                                              

                                               

               
  
                                                                            */
VOS_STATUS
WLANTL_DisableUAPSDForAC
(
  v_PVOID_t          pvosGCtx,
  v_U8_t             ucSTAId,
  WLANTL_ACEnumType  ucACId
);

#if defined WLAN_FEATURE_NEIGHBOR_ROAMING
/*                                                                          
                                         

                                                                      
                         
                                                                            
                                        

                   
    
                                          
                                                  
                                                                   
                                                    
                                                            
                                        

                         

                   
  
                                                                            */
VOS_STATUS WLANTL_RegRSSIIndicationCB
(
   v_PVOID_t                       pAdapter,
   v_S7_t                          rssiValue,
   v_U8_t                          triggerEvent,
   WLANTL_RSSICrossThresholdCBType crossCBFunction,
   VOS_MODULE_ID                   moduleID,
   v_PVOID_t                       usrCtxt
);

/*                                                                          
                                           

                                                  

                   
    
                                          
                                                  
                                                                   
                                                    
   
                         

                   
  
                                                                            */
VOS_STATUS WLANTL_DeregRSSIIndicationCB
(
   v_PVOID_t                       pAdapter,
   v_S7_t                          rssiValue,
   v_U8_t                          triggerEvent,
   WLANTL_RSSICrossThresholdCBType crossCBFunction,
   VOS_MODULE_ID                   moduleID
);

/*                                                                          

           

               
    
              

               

                                                                            */
VOS_STATUS WLANTL_BMPSRSSIRegionChangedNotification
(
   v_PVOID_t             pAdapter,
   tpSirRSSINotification pRSSINotification
);

/*                                                                          
                              

                                                          
                                                                         
                                                                   
                                              
                                                        
                           

                   
    
                                          
                                    
   
                         

                   
  
                                                                            */
VOS_STATUS WLANTL_SetAlpha
(
   v_PVOID_t pAdapter,
   v_U8_t    valueAlpha
);

/*                                                                          
                                         

                                                                  
                                                       
                                                                             
                      
                                                                       
                                           

                   
    
                                          
                                                             
                                                            
                                                                      
                                              
                                        
   
                         

                   
  
                                                                            */
VOS_STATUS WLANTL_RegGetTrafficStatus
(
   v_PVOID_t                          pAdapter,
   v_U32_t                            idleThreshold,
   v_U32_t                            measurePeriod,
   WLANTL_TrafficStatusChangedCBType  trfficStatusCB,
   v_PVOID_t                          usrCtxt
);
#endif
/*                                                                          
                                    

                                                              

                    
    
                                           
                                                                
                                                          
   
                          

                    
  
                                                                            */
VOS_STATUS WLANTL_GetStatistics
(
   v_PVOID_t                  pAdapter,
   WLANTL_TRANSFER_STA_TYPE  *statBuffer,
   v_U8_t                     STAid
);

/*                                                                          
                                      

                                                                    
                                           

                    
    
                                           
                                                                
   
                          

                    
  
                                                                            */
VOS_STATUS WLANTL_ResetStatistics
(
   v_PVOID_t                  pAdapter,
   v_U8_t                     STAid
);

/*                                                                          
                                       

                                                                  
                                      

                    

                                           
                                                                
                                        
                                             
   
                          

                    
  
                                                                            */
VOS_STATUS WLANTL_GetSpecStatistic
(
   v_PVOID_t                    pAdapter,
   WLANTL_TRANSFER_STATIC_TYPE  statType,
   v_U32_t                     *buffer,
   v_U8_t                       STAid
);

/*                                                                          
                                         

                                                                    
                                     
                                    

                    
    
                                           
                                                                
                                        

                          

                    
  
                                                                            */
VOS_STATUS WLANTL_ResetSpecStatistic
(
   v_PVOID_t                    pAdapter,
   WLANTL_TRANSFER_STATIC_TYPE  statType,
   v_U8_t                       STAid
);
/*                                                                               
                                     
   
                                                                  

                                                                          
                         
                           
                                                                                
                                                                                
                                          
                                                     
                                                         

                     
                                                                                */
v_BOOL_t WLANTL_IsReplayPacket
(
    v_U64_t    currentReplayCounter,
    v_U64_t    previousReplayCounter
);

/*                                                                               
                                               
     
                                                                              
 
                                                                          
                         
                          
                                                   
                                       
                                                             

                     
                                                                                */
v_U64_t
WLANTL_GetReplayCounterFromRxBD
(
   v_U8_t *pucRxBDHeader
);



/*
             
                                                     
   
                                                                        
                                                                             

    
                                                                  

                         
*/
VOS_STATUS  
WLANTL_GetACWeights 
( 
  v_PVOID_t             pvosGCtx,
  v_U8_t*               pACWeights
);


/*
             
                                                 
   
                                                                        
                                                                             
                                                                      


                         
*/
VOS_STATUS  
WLANTL_SetACWeights 
( 
  v_PVOID_t             pvosGCtx,
  v_U8_t*               pACWeights
);

/*                                                                          
                                          

                                                                         

                    
    
                                                                
                                                                                 
                                                              

                                                                                 

                    

                                                                            */
VOS_STATUS WLANTL_GetSoftAPStatistics(v_PVOID_t pAdapter, WLANTL_TRANSFER_STA_TYPE *statsSum, v_BOOL_t bReset);

#ifdef __cplusplus
 }
#endif 


 /*                                                                           

                                

             

                                                                        
                                                   

                                                                           
                             

   
              

                                                          

   
            

                         

              

       
   
              
                                                                                 
                                                                                

                                                                             
                                          
   
                                                                            */
void WLANTL_AssocFailed(v_U8_t staId);


/*                                                                               
                                    
     
                                                                          
 
                    
                          
                        
                                       
                    

                     
                                                                                */

void WLANTL_PostResNeeded(v_PVOID_t pvosGCtx);

/*                                                                           

                               

             
                                                                                   
                                                              
                                                                               
                                                                               
   
              

                                                          

   
            

                                            
                                          

              

                                        
   
              
   
                                                                            */

VOS_STATUS WLANTL_Finish_ULA( void (*callbackRoutine) (void *callbackContext),
                              void *callbackContext);

/*                                                                               
                                      

                                                                   

                     

            

                                                            
                                                    
                                                           

                     

                     
                                                                                */

void WLANTL_UpdateRssiBmps(v_PVOID_t pvosGCtx, v_U8_t staId, v_S7_t rssi);

/*                                                                               
                                     

                                                                  

                     

            

                                                            
                                                    
                                                        

                     

                     
                                                                                */

void WLANTL_UpdateSnrBmps(v_PVOID_t pvosGCtx, v_U8_t staId, v_S7_t snr);

/*                                                                          
                                    

             
                                                                             
                               

              
                                                                          

            

      
                                                                       
                                                                            

                        

              

                                                                            */

v_VOID_t
WLANTL_SetTxXmitPending
(
  v_PVOID_t       pvosGCtx
);

/*                                                                          
                                   

             
                                                                            
                               

              
                                                                          

            

      
                                                                       
                                                                            

              
                                                            

                                             
                                                                  

              

                                                                            */

v_BOOL_t
WLANTL_IsTxXmitPending
(
  v_PVOID_t       pvosGCtx
);

/*                                                                          
                                      

             
                                                                                
                               

              
                                                                          

            

      
                                                                       
                                                                            

                        

              

                                                                            */

v_VOID_t
WLANTL_ClearTxXmitPending
(
  v_PVOID_t       pvosGCtx
);

/*                                                                          
                                         

             
                                                                

              
                                                                             

            

      
                                                                    
                                                                            
      
                                                        
      
                                       

              
                                                              

                                                       
                                                                               
                                                                          
                                                     
                                                

              
                                                                            */

VOS_STATUS
WLANTL_UpdateSTABssIdforIBSS
(
  v_PVOID_t             pvosGCtx,
  v_U8_t                ucSTAId,
  v_U8_t               *pBssid
);



/*                                                                               
                                          

                                                                    

                     

            

                                                            
                                                    
                                                       

                     

                     
                                                                                */

void
WLANTL_UpdateLinkCapacity
(
  v_PVOID_t pvosGCtx,
  v_U8_t staId,
  v_U32_t linkCapacity);

/*                                                                           

                                       

             

                                              

              

                                                                          

            

      
                                                                       
                                                                   
                                             

       
                                                               
                                     


              

                                                            

                                                     
                                                                             
                                                                        
                                                   
                                              

              

                                                                            */

VOS_STATUS
WLANTL_GetSTALinkCapacity
(
  v_PVOID_t             pvosGCtx,
  v_U8_t                ucSTAId,
  v_U32_t               *plinkCapacity
);

/*                                                                           
                                        

             
                                                                          
                                                                             
                                                        

              
                                                       

            

      
                                                                    
                                                                            

                     

              
                                                                            */

v_VOID_t
WLANTL_TxThreadDebugHandler
(
  v_PVOID_t       *pvosGCtx
);

/*                                                                          
                                  

             
                                                          

              
                                                       

            

      
                                                                        

                        

              

                                                                            */

v_VOID_t
WLANTL_TLDebugMessage
(
  v_BOOL_t displaySnapshot
);

#endif /*                           */

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

/*===========================================================================


                       W L A N _ Q C T _ WDA _ DS . C

  OVERVIEW:

  This software unit holds the implementation of WLAN Data Abtraction APIs
  for the WLAN Transport Layer.

  DEPENDENCIES:

  Are listed for each API below.


  Copyright (c) 2010-2011 QUALCOMM Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*                                                                           

                                           


                                                                       
                                                                


                                   


                                         
                                                                      
                                                                        
                                                                          
                                                                            */

#include "wlan_qct_wda.h"
#include "wlan_qct_tl.h"
#include "wlan_qct_tli.h"
#include "tlDebug.h"

#define WDA_DS_DXE_RES_COUNT   (WDA_TLI_MIN_RES_DATA + 20)

/*                                             */
#define WDA_TL_SET_TX_XMIT_PENDING(a) WLANTL_SetTxXmitPending(a)
#define WDA_TL_IS_TX_XMIT_PENDING(a) WLANTL_IsTxXmitPending(a)
#define WDA_TL_CLEAR_TX_XMIT_PENDING(a) WLANTL_ClearTxXmitPending(a)

#define WDA_HI_FLOW_MASK 0xF0
#define WDA_LO_FLOW_MASK 0x0F

static v_VOID_t 
WDA_DS_TxCompleteCB
(
 v_PVOID_t pvosGCtx, 
 v_PVOID_t pFrameDataBuff
);


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
)
{
  /*                                           
                         
   */
}
#endif /*         */

/*                                                                          
                             

              
                                                                         
                                      


                                                                        
                                           

       
                                                                        
                                                                      
                                    

               
     
             

     
                               
                                                                           
                                               
                                               
                                                                           
                                                                    
                                                                       
                                                              
      
                                                        
                                                                   
   
              
                                                          
                                              

               
  
                                                                            */
VOS_STATUS WDA_DS_Register 
( 
  v_PVOID_t                 pvosGCtx, 
  WDA_DS_TxCompleteCallback pfnTxCompleteCallback,
  WDA_DS_RxPacketCallback   pfnRxPacketCallback, 
  WDA_DS_TxPacketCallback   pfnTxPacketCallback,
  WDA_DS_ResourceCB         pfnResourceCB,
  v_U32_t                   uResTheshold,
  v_PVOID_t                 pCallbackContext,
  v_U32_t                   *uAvailableTxBuf
)
{
  tWDA_CbContext      *wdaContext = NULL;
  WDI_Status          wdiStatus;

  VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
             "WLAN WDA: WDA_DS_Register" );

  /*                                                                        
                
                                                                           */
  if ( ( NULL == pvosGCtx ) ||
       ( NULL == pfnTxPacketCallback ) ||
       ( NULL == pfnTxCompleteCallback ) ||
       ( NULL == pfnRxPacketCallback ) ||
       ( NULL == pfnResourceCB) )
  {
    VOS_TRACE( VOS_MODULE_ID_WDA, VOS_TRACE_LEVEL_ERROR,
               "WLAN WDA:Invalid pointers on WDA_DS_Register" );
    return VOS_STATUS_E_FAULT;
  }

  /*                                                                        
                       
                                                                           */
  wdaContext = (tWDA_CbContext *)vos_get_context( VOS_MODULE_ID_WDA, pvosGCtx );
  if ( NULL == wdaContext )
  {
    VOS_TRACE( VOS_MODULE_ID_WDA, VOS_TRACE_LEVEL_ERROR,
               "WLAN WDA:Invalid wda context pointer "
               "from pvosGCtx on WDA_DS_Register" );
    return VOS_STATUS_E_FAULT;
  }

  /*                                                                        
                                               
                                                                          */
  VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH, "WDA: WDA_DS_Register");

  wdaContext->pfnTxResourceCB       = pfnResourceCB;
  wdaContext->pfnTxCompleteCallback = pfnTxCompleteCallback;

  wdiStatus = WDI_DS_Register( wdaContext->pWdiContext,
                               (WDI_DS_TxCompleteCallback)WDA_DS_TxCompleteCB,
                               (WDI_DS_RxPacketCallback)pfnRxPacketCallback,
                               WDA_DS_TxFlowControlCallback,
                               pvosGCtx );

  if ( WDI_STATUS_SUCCESS != wdiStatus )
  {
    VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
               "WLAN TL:TL failed to register with DAL, Err: %d", wdiStatus );
    return VOS_STATUS_E_FAILURE;
  }
   
  /*                                                                           
                                                            */
  *uAvailableTxBuf = WDA_DS_DXE_RES_COUNT; 

  return VOS_STATUS_SUCCESS;
}

/*                                                                          
                              

              
                                               

                                                                      
                     

                                                                 

               
     
             

     
                               
   
              
                                                          
                                              

               
  
                                                                            */
VOS_STATUS
WDA_DS_StartXmit
(
  v_PVOID_t pvosGCtx
)
{
  vos_msg_t                    sMessage;
  VOS_STATUS                   status;
/*                                                                         */

  if ( NULL == pvosGCtx )
  {
    VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
               "WLAN WDA:Invalid pointers on WDA_DS_StartXmit" );
    return VOS_STATUS_E_FAULT;
  }

  if(WDA_TL_IS_TX_XMIT_PENDING( pvosGCtx ))
  {  
    /*                                                            */
    return VOS_STATUS_SUCCESS;
  }
  /*                      */
  VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
             "Serializing WDA TX Start Xmit event" );

  vos_mem_zero( &sMessage, sizeof(vos_msg_t) );

  sMessage.bodyptr = NULL;
  sMessage.type    = WDA_DS_TX_START_XMIT;

  WDA_TL_SET_TX_XMIT_PENDING(pvosGCtx);

  status = vos_tx_mq_serialize(VOS_MQ_ID_TL, &sMessage);

  if(status != VOS_STATUS_SUCCESS)
  {
    VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_FATAL,
             "Serializing WDA TX Start Xmit event FAILED" );
    WDA_TL_CLEAR_TX_XMIT_PENDING(pvosGCtx);
  }
  return status;
}


/*                                                                          
                              

              
                                                                     

               
     
             

     
                                                            
                                                           
   
              
                                  

               
  
                                                                            */
VOS_STATUS
WDA_DS_FinishULA
(
 void (*callbackRoutine) (void *callbackContext),
 void  *callbackContext
)
{
  vos_msg_t                    sMessage;
/*                                                                         */

  /*                      */
  VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
             "Serializing WDA_DS_FinishULA event" );

  vos_mem_zero( &sMessage, sizeof(vos_msg_t) );

  sMessage.bodyval  = (v_U32_t)callbackContext;
  sMessage.bodyptr  = callbackRoutine;
  sMessage.type     = WDA_DS_FINISH_ULA;

  return vos_tx_mq_serialize(VOS_MQ_ID_TL, &sMessage);
}/*                */

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
)
{
  VOS_STATUS             vosStatus;
  WDI_DS_TxMetaInfoType* pTxMetaInfo = NULL;
  v_SIZE_t               usMacAddrSize;
  wpt_FrameCtrl          *pFrameControl;
  /*                                                                       */

  /*                                                                        
                
                            
                                                                           */
  if ( ( NULL == pvosGCtx ) || ( NULL == vosDataBuff ) || ( NULL == pvDestMacAddr ) )
  {
    VOS_TRACE( VOS_MODULE_ID_WDA, VOS_TRACE_LEVEL_ERROR,
               "WDA:Invalid parameter sent on WDA_DS_BuildTxPacketInfo" );
    return VOS_STATUS_E_FAULT;
  }

  /*                                                                        
                                                
                                                                           */
  pTxMetaInfo = WDI_DS_ExtractTxMetaData( VOS_TO_WPAL_PKT(vosDataBuff)  );
  if ( NULL == pTxMetaInfo )
  {
    VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
        "WLAN TL:Invalid RxMetaInfo pointer from PAL packet on WLANTL_RxFrames");
    VOS_ASSERT( 0 );
    return VOS_STATUS_E_FAULT;
  }

  /*                  */
  vos_mem_zero( pTxMetaInfo, sizeof( WDI_DS_TxMetaInfoType ) );

  /*                       */
  pTxMetaInfo->txFlags    = txFlag;
  pTxMetaInfo->qosEnabled = ucQosEnabled;
  pTxMetaInfo->fenableWDS = ucWDSEnabled;
  pTxMetaInfo->ac         = ucUP;
  pTxMetaInfo->fUP        = uTid;
  pTxMetaInfo->isEapol    = ucIsEapol;
  pTxMetaInfo->fdisableFrmXlt = ucDisableFrmXtl;
  pTxMetaInfo->frmType     = ( ( typeSubtype & 0x30 ) >> 4 );
  pTxMetaInfo->typeSubtype = typeSubtype;

  /*                               */
  vos_pkt_get_packet_length( vosDataBuff, pusPktLen);
  pTxMetaInfo->fPktlen = *pusPktLen;

  /*                                               
                                               */
  pTxMetaInfo->fProtMgmtFrame = 0;
  if ( WDA_TLI_MGMT_FRAME_TYPE == pTxMetaInfo->frmType )
  {
    if ( 1 == ucDisableFrmXtl )  /*                             */
    {
      vosStatus = vos_pkt_peek_data( vosDataBuff, 0, (v_PVOID_t)&pFrameControl,
                                     sizeof( wpt_FrameCtrl ));
      if ( VOS_STATUS_SUCCESS != vosStatus )
      {
        VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                   "WDA: Failed while attempting to extract Protect Bit in "
                   "Frame Control, status %d", vosStatus );
        VOS_ASSERT( 0 );
        return VOS_STATUS_E_FAULT;
      }
      pTxMetaInfo->fProtMgmtFrame = pFrameControl->wep;
      VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_LOW,
                 "WLAN TL: fProtMgmtFrame:%d", pTxMetaInfo->fProtMgmtFrame );
    }
  }

  //            
  usMacAddrSize = VOS_MAC_ADDR_SIZE;
  vosStatus = vos_pkt_extract_data( vosDataBuff,
                    WLANTL_MAC_ADDR_ALIGN( ucDisableFrmXtl ),
                    (v_PVOID_t)pvDestMacAddr,
                    &usMacAddrSize );
  if ((VOS_STATUS_SUCCESS != vosStatus) ||
          (usMacAddrSize != VOS_MAC_ADDR_SIZE))
  {
    VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
               "WDA:Failed while attempting to extract MAC Addr %d",
                vosStatus );
    VOS_ASSERT( 0 );
    return VOS_STATUS_E_FAULT;
  }

  vos_copy_macaddr( (v_MACADDR_t*)pTxMetaInfo->fSTAMACAddress, pvDestMacAddr );

  //      
  vos_copy_macaddr( (v_MACADDR_t*)pTxMetaInfo->addr2MACAddress, pAddr2 );

  /*                                  */
  VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_LOW,
             "WLAN TL: Dump TX meta info: "
             "txFlags:%d, qosEnabled:%d, ac:%d, "
             "isEapol:%d, fdisableFrmXlt:%d, frmType:%d",
             pTxMetaInfo->txFlags, ucQosEnabled, pTxMetaInfo->ac,
             pTxMetaInfo->isEapol, pTxMetaInfo->fdisableFrmXlt,
             pTxMetaInfo->frmType );

  return VOS_STATUS_SUCCESS;
}


/*                                                                          
                                      

             
                                                    
                                       
    
              

            

     
                                    

              
                                                          
                                              

              

                                                                            */
VOS_STATUS
WDA_DS_TrimRxPacketInfo
( 
  vos_pkt_t *vosDataBuff
)
{
  /*                
                */

  return VOS_STATUS_SUCCESS;
}



/*                                                                          
                                      

             
                                                      
    
                                                
    
              

            

     
                                    

                                                  
                                                                       
                                                       
      
                                        

              
                                                          
                                              

              

                                                                            */
VOS_STATUS
WDA_DS_PeekRxPacketInfo
(
  vos_pkt_t *vosDataBuff,
  v_PVOID_t *ppRxHeader,
  v_BOOL_t  bSwap
)
{
  /*                                                                        
                
                                                                           */
  if (  NULL == vosDataBuff )
  {
    VOS_TRACE( VOS_MODULE_ID_WDA, VOS_TRACE_LEVEL_ERROR,
               "WDA:Invalid parameter sent on WDA_DS_PeekRxPacketInfo" );
    return VOS_STATUS_E_FAULT;
  }

  *ppRxHeader = WDI_DS_ExtractRxMetaData( (wpt_packet*)vosDataBuff );

  if ( NULL == *ppRxHeader )
  {
    VOS_TRACE( VOS_MODULE_ID_WDA, VOS_TRACE_LEVEL_ERROR,
               "WDA:Failed to get RX meta data from WDI" );
     return VOS_STATUS_E_FAILURE;
  }
     
  return VOS_STATUS_SUCCESS;
}



/*                                                                          
                                         

             
                                                                    
                                                                        
                                                                    
             

                                                   
                                                                    

              

            

     
                                
                                    
                                                      

      
                                

              
                                                          
                                              

              

                                                                            */
VOS_STATUS
WDA_DS_GetFrameTypeSubType
(
  v_PVOID_t pvosGCtx,
  vos_pkt_t *vosDataBuff,
  v_PVOID_t pRxHeader,
  v_U8_t    *ucTypeSubtype
)
{
  /*                                                                        
                
                                                                           */
  if ( NULL == pRxHeader )
  {
    VOS_TRACE( VOS_MODULE_ID_WDA, VOS_TRACE_LEVEL_ERROR,
               "WDA:Invalid parameter sent on WDA_DS_GetFrameTypeSubType" );
    return VOS_STATUS_E_FAULT;
  }

  *ucTypeSubtype = ( WDA_GET_RX_TYPE( pRxHeader ) << 4 ) | WDA_GET_RX_SUBTYPE( pRxHeader );

  return VOS_STATUS_SUCCESS;
}


/*                                                                          
                                  

             
                                                                           
                     

                                       

              

            

     
                                
                                      

      

              
                                                          
                                              

              

                                                                            */
VOS_STATUS
WDA_DS_RxAmsduBdFix
(
  v_PVOID_t pvosGCtx,
  v_PVOID_t pvBDHeader
)
{
  /*                      */
  return VOS_STATUS_SUCCESS;
}

/*                                                                          
                             

             
             

                                                                            
                                

              

            

     
                                    

      
                         

              
                                                          
                                              

              

                                                                            */
VOS_STATUS
WDA_DS_GetRssi
(
  v_PVOID_t pvosGCtx,
  v_S7_t*   puRssi
)
{
  VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
         "WDA:halPS_GetRssi no longer supported. Need replacement");

  *puRssi = -30;

  return VOS_STATUS_SUCCESS;
}

/*                                                                          
                                    

             
                                                                   
                                                                

                                                                 
    
              

            

     
                                    
   
      
                                                 

              
                                                          
                                              

              

                                                                            */
VOS_STATUS
WDA_DS_GetTxResources
( 
  v_PVOID_t pvosGCtx,
  v_U32_t*  puResCount
)
{
  /*                                                                    */
  /*                                   */
  *puResCount = WDA_TLI_BD_PDU_RESERVE_THRESHOLD + 50;
  return VOS_STATUS_SUCCESS;
}



/*                                                                          
                                      

             
                                                        

              

            

     
                                                      

      

              
                  

              

                                                                            */
v_U64_t
WDA_DS_GetReplayCounter
(
  v_PVOID_t pRxHeader
)
{
  return WDA_GET_RX_REPLAY_COUNT( pRxHeader );
}

/*                                                                          
                              

             
                                                                   
                         

                                                                    

      
                                                         

              

            

     
                                

      

              
                                                          
                                              

              

                                                                            */

VOS_STATUS
WDA_DS_TxFrames
( 
  v_PVOID_t pvosGCtx 
)
{
  VOS_STATUS vosStatus;
  vos_pkt_t  *pTxMgmtChain = NULL;
  vos_pkt_t  *pTxDataChain = NULL;
  vos_pkt_t  *pTxPacket = NULL;
  v_BOOL_t   bUrgent;
  wpt_uint32  ucTxResReq;
  WDI_Status wdiStatus;
  tWDA_CbContext *wdaContext = NULL;
  v_U32_t     uMgmtAvailRes;
  v_U32_t     uDataAvailRes;
  WLANTL_TxCompCBType  pfnTxComp = NULL;
  v_U32_t     uTxFailCount = 0;

  wdaContext = (tWDA_CbContext *)vos_get_context(VOS_MODULE_ID_WDA, pvosGCtx);
  if ( NULL == wdaContext )
  {
    VOS_TRACE( VOS_MODULE_ID_WDA, VOS_TRACE_LEVEL_ERROR,
               "WDA:Invalid wda context pointer from pvosGCtx on WDA_DS_TxFrames" );
    return VOS_STATUS_E_FAULT;
  }

  /*                                                                         
                                                                         
                                                               
                                                                           */
  /*       */
  uMgmtAvailRes = WDI_GetAvailableResCount(wdaContext->pWdiContext, 
                                           WDI_MGMT_POOL_ID);
  
  ucTxResReq = WLANTL_GetFrames( pvosGCtx, 
                              &pTxMgmtChain, 
                               uMgmtAvailRes, 
                              (wdaContext->uTxFlowMask & WDA_HI_FLOW_MASK),
                              &bUrgent );

  //                                                                   
  //                                                                       
  //                                                                  
  //                                                    
  vosStatus = VOS_STATUS_SUCCESS;
      
  while ( NULL != pTxMgmtChain )
  {
    /*                                       */
    pTxPacket = pTxMgmtChain;
    vosStatus = vos_pkt_walk_packet_chain( pTxMgmtChain, &pTxMgmtChain, VOS_TRUE );

    if( (VOS_STATUS_SUCCESS != vosStatus) &&
        (VOS_STATUS_E_EMPTY != vosStatus) )
    {
      VOS_TRACE( VOS_MODULE_ID_WDA, VOS_TRACE_LEVEL_ERROR,
                 "WDA Walking packet chain returned status : %d", vosStatus );
      VOS_ASSERT( 0 );
      vosStatus = VOS_STATUS_E_FAILURE;
      break;
    }

    if ( VOS_STATUS_E_EMPTY == vosStatus )
    {
       vosStatus = VOS_STATUS_SUCCESS;
    }

    wdiStatus = WDI_DS_TxPacket( wdaContext->pWdiContext, 
                                 (wpt_packet*)pTxPacket, 
                                 0 /*      */ );
    if ( WDI_STATUS_SUCCESS != wdiStatus )
    {
      VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_WARN,
                  "WDA : Pushing a packet to WDI failed." );
      if ( WDI_STATUS_E_NOT_ALLOWED != wdiStatus )
      {
        uTxFailCount++;
      }
      VOS_ASSERT( wdiStatus == WDI_STATUS_E_NOT_ALLOWED );
      //                               
      vos_pkt_get_user_data_ptr(pTxPacket, VOS_PKT_USER_DATA_ID_TL, (void **)&pfnTxComp);
      if(pfnTxComp)
      {
         pfnTxComp(pvosGCtx, pTxPacket, VOS_STATUS_E_FAILURE);
      }
    }

  };

  if ( uTxFailCount )
  {
    VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                 "WDA : Tx fail count for mgmt pkts: %d.", uTxFailCount);
    uTxFailCount = 0;
  }

  /*       */
  uDataAvailRes = WDI_GetAvailableResCount(wdaContext->pWdiContext, 
                                           WDI_DATA_POOL_ID);

  ucTxResReq = WLANTL_GetFrames( pvosGCtx, 
                              &pTxDataChain, 
                              /*                    */ uDataAvailRes, 
                              (wdaContext->uTxFlowMask & WDA_LO_FLOW_MASK),
                              &bUrgent );

  //                                                                   
  //                                                                       
  //                                                                  
  //                                                    
  vosStatus = VOS_STATUS_SUCCESS;

  while ( NULL != pTxDataChain )
  {
    /*                                       */
    pTxPacket = pTxDataChain;
    vosStatus = vos_pkt_walk_packet_chain( pTxDataChain, &pTxDataChain, VOS_TRUE );

    if( (VOS_STATUS_SUCCESS != vosStatus) &&
        (VOS_STATUS_E_EMPTY != vosStatus) )
    {
      VOS_TRACE( VOS_MODULE_ID_WDA, VOS_TRACE_LEVEL_ERROR,
                 "WDA Walking packet chain returned status : %d", vosStatus );
      VOS_ASSERT( 0 );
      vosStatus = VOS_STATUS_E_FAILURE;
      break;
    }

    if ( VOS_STATUS_E_EMPTY == vosStatus )
    {
       vosStatus = VOS_STATUS_SUCCESS;
    }

    wdiStatus = WDI_DS_TxPacket( wdaContext->pWdiContext, 
                                 (wpt_packet*)pTxPacket, 
                                 0 /*      */ );
    if ( WDI_STATUS_SUCCESS != wdiStatus )
    {
      VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_WARN,
                  "WDA : Pushing a packet to WDI failed." );
      if ( WDI_STATUS_E_NOT_ALLOWED != wdiStatus )
      {
        uTxFailCount++;
      }
      VOS_ASSERT( wdiStatus == WDI_STATUS_E_NOT_ALLOWED );
      //                               
      vos_pkt_get_user_data_ptr(pTxPacket, VOS_PKT_USER_DATA_ID_TL, (void **)&pfnTxComp);
      if(pfnTxComp)
      {
         pfnTxComp(pvosGCtx, pTxPacket, VOS_STATUS_E_FAILURE);
      }
    }

  };

  if ( uTxFailCount )
  {
    VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                 "WDA : Tx fail count for data pkts: %d.", uTxFailCount);
  }


  WDI_DS_TxComplete(wdaContext->pWdiContext, ucTxResReq);

  return vosStatus;
}

/*                                                                          
                                           

             
                                      

              

            

     
                                
                                                     
                                                  

      

              

              

                                                                            */
v_VOID_t
WDA_DS_TxFlowControlCallback
(
   v_PVOID_t pvosGCtx,
   v_U8_t    ucFlowMask
)
{
   tWDA_CbContext* wdaContext = NULL;
    v_U8_t          ucOldFlowMask;
   /*                                                                       */

   /*                                                                        
                 
                                                                            */
   if ( NULL == pvosGCtx )
   {
      VOS_TRACE( VOS_MODULE_ID_WDA, VOS_TRACE_LEVEL_ERROR,
                 "WDA: Invalid parameter sent on WDA_DS_TxFlowControlCallback" );
      return;
   }

   wdaContext = (tWDA_CbContext *)vos_get_context( VOS_MODULE_ID_WDA, pvosGCtx );
   if ( NULL == wdaContext )
   {
      VOS_TRACE( VOS_MODULE_ID_WDA, VOS_TRACE_LEVEL_ERROR,
                 "WDA: Invalid context on WDA_DS_TxFlowControlCallback" );
      return;
   }

   /*                          
                                                                                      
                                                              
   */


   /*               */
   ucOldFlowMask           = wdaContext->uTxFlowMask; 
   wdaContext->uTxFlowMask = ucFlowMask;

   /*                                              
    
                                                      
    
                         
                          
                          
                          
                          
    
                                                    
    
                                                        
                                                        
                                                              
   */

   if ( ucFlowMask > ucOldFlowMask  )
   {
     WDA_DS_StartXmit(pvosGCtx);
   }

}

/*                                                                          
                                   

             
                                     

              

            

     
                                

      
                                                     
                                                  

              
                                                          
                                              

              

                                                                            */
VOS_STATUS
WDA_DS_GetTxFlowMask
(
 v_PVOID_t pvosGCtx,
 v_U8_t*   puFlowMask
)
{
   tWDA_CbContext* wdaContext = NULL;
   /*                                                                       */

   /*                                                                        
                 
                                                                            */
   if ( ( NULL == pvosGCtx ) || ( NULL == puFlowMask ) )
   {
      VOS_TRACE( VOS_MODULE_ID_WDA, VOS_TRACE_LEVEL_ERROR,
                 "WDA: Invalid parameter sent on WDA_DS_GetTxFlowMask" );
      return VOS_STATUS_E_INVAL;
   }

   wdaContext = (tWDA_CbContext *)vos_get_context( VOS_MODULE_ID_WDA, pvosGCtx );
   if ( NULL == wdaContext )
   {
      VOS_TRACE( VOS_MODULE_ID_WDA, VOS_TRACE_LEVEL_ERROR,
                 "WDA: Invalid context on WDA_DS_GetTxFlowMask" );
      return VOS_STATUS_E_INVAL;
   }

   *puFlowMask = wdaContext->uTxFlowMask;

   return VOS_STATUS_SUCCESS;
}

v_VOID_t 
WDA_DS_TxCompleteCB
(
 v_PVOID_t pvosGCtx, 
 v_PVOID_t pFrameDataBuff
)
{
  tWDA_CbContext*        wdaContext = NULL;
  WDI_DS_TxMetaInfoType* pTxMetadata;
  VOS_STATUS             vosStatus;
  /*                                                                       */

  /*                                                                        
                
                                                                            */

  if ( ( NULL == pvosGCtx ) || ( NULL == pFrameDataBuff ) )
  {
    VOS_TRACE( VOS_MODULE_ID_WDA, VOS_TRACE_LEVEL_ERROR,
               "WDA: Invalid parameter sent on WDA_DS_TxCompleteCB" );
    return;
  }

  wdaContext = (tWDA_CbContext *)vos_get_context( VOS_MODULE_ID_WDA, pvosGCtx );
  if ( NULL == wdaContext )
  {
    VOS_TRACE( VOS_MODULE_ID_WDA, VOS_TRACE_LEVEL_ERROR,
               "WDA: Invalid context on WDA_DS_TxCompleteCB" );
    return;
  }

  //                                 
  pTxMetadata = WDI_DS_ExtractTxMetaData( (wpt_packet*)pFrameDataBuff );
  
  if ( eWLAN_PAL_STATUS_SUCCESS == pTxMetadata->txCompleteStatus )
    vosStatus = VOS_STATUS_SUCCESS;
  else 
    vosStatus = VOS_STATUS_E_FAILURE;

  wdaContext->pfnTxCompleteCallback( pvosGCtx, pFrameDataBuff, vosStatus );
}

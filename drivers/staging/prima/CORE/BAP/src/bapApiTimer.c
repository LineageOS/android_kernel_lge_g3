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

                      b a p A p i T i m e r . C
                                               
  OVERVIEW:
  
  This software unit holds the implementation of the timer routines
  required by the WLAN BAP module.  
  
  The functions provide by this module are called by the rest of 
  the BT-AMP PAL module.

  DEPENDENCIES: 

  Are listed for each API below. 
  
  
  Copyright (c) 2008 QUALCOMM Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*                                                                           

                                           


                                                                       
                                                                


                                                                                                                                             


                                      
                                                                             
                                    

                                                                           */

/*                                                                            
                
                                                                            */
//                                 
#include "vos_types.h"
#include "bapApiTimer.h"

//                 

/*                                                                            
                                         
                                                                            */
#define WLAN_BAP_TX_PKT_MONITOR_TIME 100

/*                                                                            
                    
                                                                            */

/*                                                                            
                          
                                                                            */

/*                                                                            
                                             
                                                                            */
#if 1
//                              
static const v_U8_t WLANBAP_BT_AMP_OUI[] =  {0x00, 0x19, 0x58 };

#endif

/*                                                                            
                              
                                                                            */

/*                                                                            
                                               
                                                                            */

/*                                                                            
                                    
                                                                           */

/*                                                                            
                                          
                                                                            */

/*                                                                            
                                    
                                                                            */

/*                                                                          

                                               

              
                                           
    
               
    
             

      
                                                 
   
              
                                                              

                                                          
                                               

               
  
                                                                            */
VOS_STATUS 
WLANBAP_InitConnectionAcceptTimer
( 
  ptBtampContext  pBtampCtx
)
{
  VOS_STATUS  vosStatus = VOS_STATUS_SUCCESS;

  /*                                                                       */


  /*                                                                        
                                   
                                                                           */

  if ( NULL == pBtampCtx ) 
  {
    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                 "Invalid BAP pointer in %s", __func__);
    return VOS_STATUS_E_FAULT;
  }

  /*                     */
  vosStatus = vos_timer_init( 
          &pBtampCtx->bapConnectionAcceptTimer,
          VOS_TIMER_TYPE_SW, /*               */
          WLANBAP_ConnectionAcceptTimerHandler,
          pBtampCtx);
   
  return VOS_STATUS_SUCCESS;
}/*                                   */

/*                                                                          

                                                 

              
                                        
    
               
    
             

      
                                                 
   
              
                                                              

                                                          
                                               

               
  
                                                                            */
VOS_STATUS 
WLANBAP_DeinitConnectionAcceptTimer
( 
  ptBtampContext  pBtampCtx
)
{
  VOS_STATUS  vosStatus = VOS_STATUS_SUCCESS;

  /*                                                                       */


  /*                                                                        
                                   
                                                                           */

  if ( NULL == pBtampCtx ) 
  {
    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                 "Invalid BAP pointer in %s", __func__);
    return VOS_STATUS_E_FAULT;
  }

  /*                                    */
  vosStatus = vos_timer_destroy ( 
          &pBtampCtx->bapConnectionAcceptTimer );
   
  return VOS_STATUS_SUCCESS;
}/*                                     */

/*                                                                          

                                                

              
                                      
    
               
    
             

      
                                                 
                               
   
              
                                                              

                                                          
                                               

               
  
                                                                            */
VOS_STATUS 
WLANBAP_StartConnectionAcceptTimer
( 
  ptBtampContext  pBtampCtx,
  v_U32_t interval
)
{
  /*                                                                        
                                   
                                                                           */

  if ( NULL == pBtampCtx ) 
  {
    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                 "Invalid BAP pointer in %s", __func__);
    return VOS_STATUS_E_FAULT;
  }

  /*                                 */
  vos_timer_start( 
          &pBtampCtx->bapConnectionAcceptTimer,
          interval);

  return VOS_STATUS_SUCCESS;
}/*                                    */


/*                                                                          

                                                

              
                                     
    
               
    
             

      
                                                 
   
              
                                                              

                                                          
                                               

               
  
                                                                            */
VOS_STATUS 
WLANBAP_StopConnectionAcceptTimer 
( 
  ptBtampContext  pBtampCtx
)
{
  VOS_STATUS  vosStatus = VOS_STATUS_SUCCESS;
  /*                                                                       */


  /*                                                                        
                                   
                                                                           */

  if ( NULL == pBtampCtx ) 
  {
    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                 "Invalid BAP pointer in %s", __func__);
    return VOS_STATUS_E_FAULT;
  }

  /*               */
  vosStatus =  vos_timer_stop( 
           &pBtampCtx->bapConnectionAcceptTimer);
 
   
  return VOS_STATUS_SUCCESS;
}/*                                   */ 



/*                                                                          

                                                  

              
                                                                  
                 
    
               
    
             

      
                                                                     
   
              
        
         
               
  
                                                                            */
v_VOID_t 
WLANBAP_ConnectionAcceptTimerHandler
( 
  v_PVOID_t userData 
)
{
  ptBtampContext  pBtampCtx = (ptBtampContext)userData;
  tWLAN_BAPEvent bapEvent; /*                     */
  VOS_STATUS  vosStatus = VOS_STATUS_SUCCESS;
  v_U8_t status;    /*                               */
  /*                                                                       */

  /*                                                                       
                 
                                                                          */
  if ( NULL == pBtampCtx )
  {
     VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                "WLAN BAP: Fatal error in %s", __func__ );
     VOS_ASSERT(0);
     return; 
  }

 /*                                                                     
                                       
                                                                        */
  /*                             */ 
  bapEvent.event = eWLAN_BAP_TIMER_CONNECT_ACCEPT_TIMEOUT;
  bapEvent.params = NULL;

  /*              */ 
  vosStatus = btampFsm(pBtampCtx, &bapEvent, &status);

  /*                                                                    */ 
  bapEvent.event =  eWLAN_BAP_MAC_READY_FOR_CONNECTIONS;
  bapEvent.params = NULL;

  /*              */ 
  vosStatus = btampFsm(pBtampCtx, &bapEvent, &status);


}/*                                    */

/*                                                                          

                                              

              
                                          
    
               
    
             

      
                                                 
   
              
                                                              

                                                          
                                               

               
  
                                                                            */
VOS_STATUS 
WLANBAP_InitLinkSupervisionTimer
( 
  ptBtampContext  pBtampCtx
)
{
  VOS_STATUS  vosStatus = VOS_STATUS_SUCCESS;

  /*                                                                       */


  /*                                                                        
                                   
                                                                           */

  if ( NULL == pBtampCtx ) 
  {
    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                 "Invalid BAP pointer in %s", __func__);
    return VOS_STATUS_E_FAULT;
  }

  /*                     */
  vosStatus = vos_timer_init( 
          &pBtampCtx->bapLinkSupervisionTimer,
          VOS_TIMER_TYPE_SW, /*               */
          WLANBAP_LinkSupervisionTimerHandler,
          pBtampCtx);
   
  return VOS_STATUS_SUCCESS;
}/*                                  */

/*                                                                          

                                                

              
                                       
    
               
    
             

      
                                                 
   
              
                                                              

                                                          
                                               

               
  
                                                                            */
VOS_STATUS 
WLANBAP_DeinitLinkSupervisionTimer
( 
  ptBtampContext  pBtampCtx
)
{
  VOS_STATUS  vosStatus = VOS_STATUS_SUCCESS;

  /*                                                                       */


  /*                                                                        
                                   
                                                                           */

  if ( NULL == pBtampCtx ) 
  {
    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                 "Invalid BAP pointer in %s", __func__);
    return VOS_STATUS_E_FAULT;
  }

  /*                                    */
  vosStatus = vos_timer_destroy ( 
          &pBtampCtx->bapLinkSupervisionTimer );
   
  return VOS_STATUS_SUCCESS;
}/*                                    */

/*                                                                          

                                               

              
                                         
    
               
    
             

      
                                                 
                               
   
              
                                                              

                                                          
                                               

               
  
                                                                            */
VOS_STATUS 
WLANBAP_StartLinkSupervisionTimer
( 
  ptBtampContext  pBtampCtx,
  v_U32_t interval
)
{
  /*                                                                        
                                   
                                                                           */

  if ( NULL == pBtampCtx ) 
  {
    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                 "Invalid BAP pointer in %s", __func__);
    return VOS_STATUS_E_FAULT;
  }

  vos_timer_start( 
          &pBtampCtx->bapLinkSupervisionTimer,
          interval);

  return VOS_STATUS_SUCCESS;
}/*                                   */

/*                                                                          

                                               

              
                                   
    
               
    
             

      
                                                 
   
              
                                                              

                                                          
                                               

               
  
                                                                            */
VOS_STATUS 
WLANBAP_StopLinkSupervisionTimer 
( 
  ptBtampContext  pBtampCtx
)
{
  VOS_STATUS  vosStatus = VOS_STATUS_SUCCESS;
  /*                                                                       */


  /*                                                                        
                                   
                                                                           */

  if ( NULL == pBtampCtx ) 
  {
    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                 "Invalid BAP pointer in %s", __func__);
    return VOS_STATUS_E_FAULT;
  }

  /*               */
  vosStatus =  vos_timer_stop( 
           &pBtampCtx->bapLinkSupervisionTimer);
 
   
  return VOS_STATUS_SUCCESS;
}/*                                  */ 


/*                                                                          

                                                 

              
                                                                              
    
               
    
             

      
                                                                     
   
              
        
         
               
  
                                                                            */
v_VOID_t 
WLANBAP_LinkSupervisionTimerHandler
( 
  v_PVOID_t userData 
)
{
    ptBtampContext           pBtampCtx =      (ptBtampContext)userData;
    VOS_STATUS               vosStatus =      VOS_STATUS_SUCCESS;
    ptBtampHandle            btampHandle =    (ptBtampHandle)userData;
    tWLAN_BAPEvent           bapEvent; /*                     */
    v_U8_t                   phy_link_handle;

  /*                                                                       */

  /*                                                                       
                 
                                                                          */
    if ( NULL == pBtampCtx )
    {
       VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                "WLAN BAP: Fatal error in %s", __func__ );
       VOS_ASSERT(0);
       return; 
    }

    phy_link_handle = pBtampCtx->phy_link_handle;
     VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                "WLAN BAP:In LinkSupervision Timer handler %s", __func__ );

    if(pBtampCtx->dataPktPending == VOS_TRUE)
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                    "%s: Data seen. Do nothing", __func__ );

        pBtampCtx->dataPktPending = VOS_FALSE;
        pBtampCtx->lsReqPktPending = VOS_FALSE;
        pBtampCtx->retries = 0;
        vosStatus = WLANBAP_StopLinkSupervisionTimer(pBtampCtx);
        vosStatus = WLANBAP_StartLinkSupervisionTimer (pBtampCtx,
                    pBtampCtx->bapLinkSupervisionTimerInterval * WLANBAP_BREDR_BASEBAND_SLOT_TIME);

        //                                                                                             
        return;
    }
    else if((pBtampCtx->lsReqPktPending == VOS_TRUE ) 
            && (pBtampCtx->retries == WLANBAP_LINK_SUPERVISION_RETRIES))
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                    "#########WLAN BAP: LinkSupervision Timed OUT######## %s", __func__ );

     /*                                                                     
                                       
                                                                        */
        /*                             */ 
        /*                                  */ 
        //                                                       
        bapEvent.event =eWLAN_BAP_MAC_INDICATES_MEDIA_DISCONNECTION;
        bapEvent.params = NULL;

        /*              */ 
        vosStatus = btampFsm(pBtampCtx, &bapEvent, (v_U8_t *)&vosStatus);
    }
    else
    {    
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                    "%s: Resend the LS packet", __func__ );

        /*                                                                                */
        WLANBAP_StopLinkSupervisionTimer(pBtampCtx);
        pBtampCtx->pPacket = pBtampCtx->lsReqPacket;
        vosStatus = WLANBAP_TxLinkSupervision( btampHandle, 
                                               phy_link_handle, 
                                               pBtampCtx->pPacket ,
                                               WLANTL_BT_AMP_TYPE_LS_REQ);
    }
    
}/*                                   */

/*                                                                          

                                               

              
                                      
    
               
    
             

      
                                                 
                               
   
              
                                                              

                                                          
                                               

               
  
                                                                            */
VOS_STATUS 
WLANBAP_StartTxPacketMonitorTimer
( 
  ptBtampContext  pBtampCtx
)
{
  VOS_STATUS  vosStatus = VOS_STATUS_SUCCESS;
  v_U32_t     uInterval = WLAN_BAP_TX_PKT_MONITOR_TIME; 
  /*                                                                       */


  /*                                                                        
                                   
                                                                           */
  if ( NULL == pBtampCtx ) 
  {
    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                 "Invalid BAP pointer in %s", __func__);
    return VOS_STATUS_E_FAULT;
  }

  /*                */
  vosStatus = vos_timer_start( &pBtampCtx->bapTxPktMonitorTimer,
                                uInterval);

  return vosStatus;
}/*                                   */


/*                                                                          

                                               

              
                                     
    
               
    
             

      
                                                 
   
              
                                                              

                                                          
                                               

               
  
                                                                            */
VOS_STATUS 
WLANBAP_StopTxPacketMonitorTimer 
( 
  ptBtampContext  pBtampCtx
)
{
  VOS_STATUS  vosStatus = VOS_STATUS_SUCCESS;
  /*                                                                       */


  /*                                                                        
                                   
                                                                           */
  if ( NULL == pBtampCtx ) 
  {
    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                 "Invalid BAP pointer in %s", __func__);
    return VOS_STATUS_E_FAULT;
  }

  /*               */
  vosStatus =  vos_timer_stop( &pBtampCtx->bapTxPktMonitorTimer);
 
   
  return vosStatus;
}/*                                  */ 


/*                                                                          

                                            

              
                                                                        
    
               
    
             

      
                                                  
   
              
        
         
               
  
                                                                            */
v_VOID_t 
WLANBAP_SendCompletedPktsEvent
( 
  ptBtampContext     pBtampCtx 
)
{
  v_U8_t             i, j;
  tBtampHCI_Event    bapHCIEvent; /*                                  */
  v_U32_t            uTxCompleted    = 0; 
  tpBtampLogLinkCtx  pLogLinkContext = NULL;
  /*                                                                       */
    /*                                              */ 
  bapHCIEvent.bapHCIEventCode = BTAMP_TLV_HCI_NUM_OF_COMPLETED_PKTS_EVENT;
  bapHCIEvent.u.btampNumOfCompletedPktsEvent.num_handles = 0;

 /*                                                                     
                                                                
                      
                                                                        */
  for (i = 0, j = 0; i < WLANBAP_MAX_LOG_LINKS ; i++) 
  {
     pLogLinkContext = &pBtampCtx->btampLogLinkCtx[i];
     if ( pLogLinkContext->present ) 
     {
       uTxCompleted = pLogLinkContext->uTxPktCompleted;
       bapHCIEvent.u.btampNumOfCompletedPktsEvent.conn_handles[j] =
           pLogLinkContext->log_link_handle;
       bapHCIEvent.u.btampNumOfCompletedPktsEvent.num_completed_pkts[j] =
           uTxCompleted;

       j++;

       vos_atomic_decrement_U32_by_value((v_U32_t *) &pLogLinkContext->uTxPktCompleted,
                                         (v_U32_t) uTxCompleted);

       if (uTxCompleted) { 
          VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR, 
                  "wlan bap: %s Log Link handle - %d No Of Pkts - %d", __func__, 
                  pLogLinkContext->log_link_handle, uTxCompleted);  
       }
     }
  }

  /*                                                                    
                                   */
  if (j && uTxCompleted)
  {
      VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                "WLAN BAP: Indicating Num Completed packets Event");

      /*                                    */
      bapHCIEvent.u.btampNumOfCompletedPktsEvent.num_handles = j;
      (*pBtampCtx->pBapHCIEventCB)
      (
           pBtampCtx->pHddHdl,   /*                                             */
           &bapHCIEvent, /*                                  */
           VOS_TRUE /*                                       */ 
      );
  }

}

/*                                                                          

                                                

              
                                                                               
    
               
    
             

      
                                                  
   
              
        
         
               
  
                                                                            */
v_VOID_t 
WLANBAP_SendCompletedDataBlksEvent
( 
  ptBtampContext     pBtampCtx 
)
{
  v_U8_t             i, j;
  tBtampHCI_Event    bapHCIEvent; /*                                  */
  v_U32_t            uTxCompleted    = 0; 
  tpBtampLogLinkCtx  pLogLinkContext = NULL;
  /*                                                                       */
    /*                                                  */ 
  bapHCIEvent.bapHCIEventCode = BTAMP_TLV_HCI_NUM_OF_COMPLETED_DATA_BLOCKS_EVENT;
  bapHCIEvent.u.btampNumOfCompletedDataBlocksEvent.num_handles = 0;

 /*                                                                     
                                                                        
                      
                                                                        */
  for (i = 0, j = 0; i < WLANBAP_MAX_LOG_LINKS ; i++) 
  {
     pLogLinkContext = &pBtampCtx->btampLogLinkCtx[i];
     if ( pLogLinkContext->present ) 
     {
       uTxCompleted = pLogLinkContext->uTxPktCompleted;
       bapHCIEvent.u.btampNumOfCompletedDataBlocksEvent.conn_handles[j] =
           pLogLinkContext->log_link_handle;
       bapHCIEvent.u.btampNumOfCompletedDataBlocksEvent.num_completed_pkts[j] =
           uTxCompleted;
       bapHCIEvent.u.btampNumOfCompletedDataBlocksEvent.num_completed_blocks[j] =
           uTxCompleted;
       bapHCIEvent.u.btampNumOfCompletedDataBlocksEvent.total_num_data_blocks = 16;

       j++;

       vos_atomic_decrement_U32_by_value((v_U32_t *) &pLogLinkContext->uTxPktCompleted,
                                         (v_U32_t) uTxCompleted);

       if (uTxCompleted) { 
          VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR, 
                  "wlan bap: %s Log Link handle - %d No Of Pkts - %d", __func__, 
                  pLogLinkContext->log_link_handle, uTxCompleted);  
       }
     }
  }

  /*                                                                    
                                       */
  if (j && uTxCompleted)
  {
      VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                "WLAN BAP: Indicating Num Completed Data Blocks Event");

      /*                                            */
      bapHCIEvent.u.btampNumOfCompletedDataBlocksEvent.num_handles = j;
      (*pBtampCtx->pBapHCIEventCB)
      (
           pBtampCtx->pHddHdl,   /*                                             */
           &bapHCIEvent, /*                                  */
           VOS_TRUE /*                                       */ 
      );
  }

}

/*                                                                          

                                            

              
                                                                          
          
    
               
    
             

      
                                                                     
   
              
        
         
               
  
                                                                            */
v_VOID_t 
WLANBAP_TxPacketMonitorHandler
( 
  v_PVOID_t userData 
)
{
  ptBtampContext     pBtampCtx       = (ptBtampContext)userData;
  BTAMPFSM_INSTANCEDATA_T *instanceVar = &pBtampCtx->bapPhysLinkMachine;
  /*                                                                       */

  /*                                                                       
                 
                                                                          */
  if ( NULL == pBtampCtx )
  {
     VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                "WLAN BAP: Fatal error in %s", __func__ );
     VOS_ASSERT(0);
     return; 
  }

#if 0 //         
  /*                                      */
  VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH,
            "WLAN BAP Context Monitor: pBtampCtx value = %x in %s:%d", pBtampCtx, __func__, __LINE__ );
#endif //         

  if(WLANBAP_FLOW_CONTROL_MODE_BLOCK_BASED == pBtampCtx->ucDataTrafficMode)
     {
    WLANBAP_SendCompletedDataBlksEvent(pBtampCtx);
       }
  else
  {
    WLANBAP_SendCompletedPktsEvent(pBtampCtx);
  }

  /*                                                           
                 
                                                              
                                                        
   */
  if (CONNECTED == instanceVar->stateVar)
  {
    WLANBAP_StartTxPacketMonitorTimer(pBtampCtx);
  }
}/*                              */


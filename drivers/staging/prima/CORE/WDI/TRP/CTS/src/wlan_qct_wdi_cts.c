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

                       W L A N _ Q C T _ C T S . C

  OVERVIEW:

  This software unit holds the implementation of the WLAN Control Transport
  Service Layer.

  The functions externalized by this module are to be called by the DAL Core
  that wishes to use a platform agnostic API to communicate with the WLAN SS.

  DEPENDENCIES:

  Are listed for each API below.


  Copyright (c) 2010-2011 QUALCOMM Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*                                                                           

                                           


                                                                       
                                                                


                            


                                       
                                                                               
                                                                      
                                     

                                                                           */



/*                                                                           

                                                  

                                                                           */

/*                                                                            
                
                                                                            */
#include "wlan_qct_wdi_cts.h"
#include "wlan_qct_pal_msg.h"
#include "wlan_qct_pal_api.h"
#include "wlan_qct_pal_trace.h"
#include "wlan_qct_os_list.h"
#include "wlan_qct_wdi.h"
#include "wlan_qct_wdi_i.h"
#ifdef CONFIG_ANDROID
#include <mach/msm_smd.h>
#include <linux/delay.h>
#else
#include "msm_smd.h"
#endif

/*                                                            
                                      */
static WCTS_HandleType gwctsHandle;
/*                                                                            
                                         
                                                                            */

/*                                                                       */
#define WCTS_CB_MAGIC     0x53544357

/*                                                 */
#define WCTS_SMD_OPEN_TIMEOUT 5000

/*                                                  */
#define WCTS_SMD_CLOSE_TIMEOUT 5000

/*                                                                            
                    
                                                                            */
/*                                                                           
                 
                                                                            */
typedef enum
{
   WCTS_STATE_CLOSED,       /*        */
   WCTS_STATE_OPEN_PENDING, /*                                     */
   WCTS_STATE_OPEN,         /*                              */
   WCTS_STATE_DEFERRED,     /*                                    */
   WCTS_STATE_REM_CLOSED,   /*                                   */
   WCTS_STATE_MAX
} WCTS_StateType;

/*                                                                           
                                       
                                                                            */
typedef struct
{
   WCTS_NotifyCBType      wctsNotifyCB;
   void*                  wctsNotifyCBData;
   WCTS_RxMsgCBType       wctsRxMsgCB;
   void*                  wctsRxMsgCBData;
   WCTS_StateType         wctsState;
   smd_channel_t*         wctsChannel;
   wpt_list               wctsPendingQueue;
   wpt_uint32             wctsMagic;
   wpt_msg                wctsOpenMsg;
   wpt_msg                wctsDataMsg;
   wpt_event              wctsEvent;
} WCTS_ControlBlockType;

/*                                                                           
                      
                                                                            */
typedef struct
{
   /*                                                    */
   wpt_list_node          node;

   /*                                   */
   void*                  pBuffer;

   /*           */
   int                    bufferSize;

} WCTS_BufferType;

/*                                                                            
                          
                                                                            */

/*                                                                            
                              
                                                                            */
#ifdef FEATURE_R33D
/*                             
                                                                  
                                                               */
static WCTS_ControlBlockType  *ctsCB;

/*                                                     */
static int                     port_open;
#endif /*              */
/*                                                                            
                                               
                                                                            */



/* 
                                                      
                                            
       

                                 

     
             
*/
static void
WCTS_PALOpenCallback
(
   wpt_msg *pMsg
)
{
   WCTS_ControlBlockType*        pWCTSCb;
   /*                                                                      */

   /*                                      */
   pWCTSCb = pMsg->pContext;

   /*                                                                    
                 
                                                                         */
   if ((NULL == pWCTSCb) || (WCTS_CB_MAGIC != pWCTSCb->wctsMagic)) {
      WPAL_TRACE(eWLAN_MODULE_DAL_CTRL, eWLAN_PAL_TRACE_LEVEL_ERROR,
                 "WCTS_PALOpenCallback: Invalid parameters received.");
      return;
   }

   if (WCTS_STATE_OPEN_PENDING != pWCTSCb->wctsState) {
      WPAL_TRACE(eWLAN_MODULE_DAL_CTRL, eWLAN_PAL_TRACE_LEVEL_ERROR,
                 "WCTS_PALOpenCallback: Invoke from invalid state %d.",
                 pWCTSCb->wctsState);
      return;
   }

   /*                                                     */
   pWCTSCb->wctsState = WCTS_STATE_OPEN;
   pWCTSCb->wctsNotifyCB((WCTS_HandleType)pWCTSCb,
                        WCTS_EVENT_OPEN,
                        pWCTSCb->wctsNotifyCBData);

   /*                                                */
   wpalEventSet(&pWCTSCb->wctsEvent);

}/*                    */



/* 
                                                      
                                            

                                      

     
             
*/
static void
WCTS_PALReadCallback
(
   WCTS_ControlBlockType*  pWCTSCb
)
{
   void* buffer;
   int packet_size;
   int available;
   int bytes_read;

   /*                                                                      */

   /*                                                                    
                 
                                                                         */
   if ((NULL == pWCTSCb) || (WCTS_CB_MAGIC != pWCTSCb->wctsMagic)) {
      WPAL_TRACE(eWLAN_MODULE_DAL_CTRL, eWLAN_PAL_TRACE_LEVEL_ERROR,
                 "WCTS_PALReadCallback: Invalid parameter received.");
      return;
   }

   /*                                             */
   while (1) {
      /*                                     */
      packet_size = smd_cur_packet_size(pWCTSCb->wctsChannel);
      if (0 == packet_size) {
         /*                         */
         return;
      }

      /*                                         */
      available = smd_read_avail(pWCTSCb->wctsChannel);
      if (available < packet_size) {
         /*                                          
                                                                */
         return;
      }

      buffer = wpalMemoryAllocate(packet_size);
      if (NULL ==  buffer) {
         WPAL_TRACE(eWLAN_MODULE_DAL_CTRL, eWLAN_PAL_TRACE_LEVEL_ERROR,
                    "WCTS_PALReadCallback: Memory allocation failure");
         WPAL_ASSERT(0);
         return;
      }

      bytes_read = smd_read(pWCTSCb->wctsChannel,
                            buffer,
                            packet_size);

      if (bytes_read != packet_size) {
         /*                                       */
         WPAL_TRACE(eWLAN_MODULE_DAL_CTRL, eWLAN_PAL_TRACE_LEVEL_ERROR,
                    "WCTS_PALReadCallback: Failed to read data from SMD");
         wpalMemoryFree(buffer);
         WPAL_ASSERT(0);
         return;
      }

      /*                                               */
      pWCTSCb->wctsRxMsgCB((WCTS_HandleType)pWCTSCb,
                           buffer,
                           packet_size,
                           pWCTSCb->wctsRxMsgCBData);

      /*                          */
      wpalMemoryFree(buffer);
   }

} /*                    */



/* 
                                                       
                                            

                                      

     
             
*/
static void
WCTS_PALWriteCallback
(
   WCTS_ControlBlockType*  pWCTSCb
)
{
   wpt_list_node*      pNode;
   WCTS_BufferType*    pBufferQueue;
   void*               pBuffer;
   int                 len;
   int                 available;
   int                 written;

   /*                                                                        */

   /*                                                                    
                 
                                                                         */
   if ((NULL == pWCTSCb) || (WCTS_CB_MAGIC != pWCTSCb->wctsMagic)) {
      WPAL_TRACE(eWLAN_MODULE_DAL_CTRL, eWLAN_PAL_TRACE_LEVEL_ERROR,
                 "WCTS_PALWriteCallback: Invalid parameter received.");
      return;
   }

   /*                                                           */
   if (WCTS_STATE_DEFERRED != pWCTSCb->wctsState) {
      return;
   }

   /*                                                           
                                                                  
                                                                  
                                                                   
                                        */
   while (eWLAN_PAL_STATUS_SUCCESS ==
          wpal_list_peek_front(&pWCTSCb->wctsPendingQueue, &pNode)) {
      pBufferQueue = container_of(pNode, WCTS_BufferType, node);
      pBuffer = pBufferQueue->pBuffer;
      len = pBufferQueue->bufferSize;

      available = smd_write_avail(pWCTSCb->wctsChannel);
      if (available < len) {
         /*                                                        */
         return;
      }

      /*                                                         
                                                        */
      wpal_list_remove_front(&pWCTSCb->wctsPendingQueue, &pNode);

      /*                                                       
                                                   */

      written = smd_write(pWCTSCb->wctsChannel, pBuffer, len);
      if (written != len) {
         /*                      */
         WPAL_TRACE(eWLAN_MODULE_DAL_CTRL, eWLAN_PAL_TRACE_LEVEL_ERROR,
                    "WCTS_PALWriteCallback: channel write failure");

         /*                                                        
                                                                  
                                                                   
                                 
                                                                     
                                        */
      }

      /*                                                       */
      wpalMemoryFree(pBuffer);
      wpalMemoryFree(pBufferQueue);

      /*                                                           
                                                 */
   }

   /*                                                                   
                                                                       
                                */
   pWCTSCb->wctsState = WCTS_STATE_OPEN;
   smd_disable_read_intr(pWCTSCb->wctsChannel);

} /*                     */



/* 
                                                           
                                            
       

                                 

     
             
*/
static void
WCTS_PALDataCallback
(
   wpt_msg *pMsg
)
{
   WCTS_ControlBlockType*        pWCTSCb;
   /*                                                                        */

   /*                                      */
   pWCTSCb = pMsg->pContext;

   /*                               */
   WCTS_PALReadCallback(pWCTSCb);

   /*                            */
   WCTS_PALWriteCallback(pWCTSCb);

} /*                    */

/* 
                                                                
                                          

                                           

     
                        
*/
wpt_uint32
WCTS_ClearPendingQueue
(
   WCTS_HandleType      wctsHandle
)
{
   WCTS_ControlBlockType* pWCTSCb = (WCTS_ControlBlockType*) wctsHandle;
   wpt_list_node*      pNode = NULL;
   WCTS_BufferType*    pBufferQueue = NULL;
   void*               pBuffer = NULL;

   /*                                                                      */

   if ((NULL == pWCTSCb) || (WCTS_CB_MAGIC != pWCTSCb->wctsMagic)) {
      WPAL_TRACE(eWLAN_MODULE_DAL_CTRL, eWLAN_PAL_TRACE_LEVEL_ERROR,
                 "WCTS_ClearPendingQueue: Invalid parameters received.");
      return eWLAN_PAL_STATUS_E_INVAL;
   }

   /*                                      */
   while (eWLAN_PAL_STATUS_SUCCESS ==
          wpal_list_remove_front(&pWCTSCb->wctsPendingQueue, &pNode)) {
      pBufferQueue = container_of(pNode, WCTS_BufferType, node);
      pBuffer = pBufferQueue->pBuffer;
      wpalMemoryFree(pBuffer);
      wpalMemoryFree(pBufferQueue);
   }
   return eWLAN_PAL_STATUS_SUCCESS;

}/*                      */


/* 
                                                                          
              
  
                                                                            
                                              
  
                                                                                
                                            
  
               
 */

static void
WCTS_NotifyCallback
(
   void            *data,
   unsigned        event
)
{
   wpt_msg                       *palMsg;
   WCTS_ControlBlockType*        pWCTSCb = (WCTS_ControlBlockType*) data;

   /*                                                                      */

   /*                                                                    
                 
                                                                         */
   if (WCTS_CB_MAGIC != pWCTSCb->wctsMagic) {
      WPAL_TRACE(eWLAN_MODULE_DAL_CTRL, eWLAN_PAL_TRACE_LEVEL_ERROR,
                 "%s: Received unexpected SMD event %u",
                 __func__, event);

      /*                                                    */
      return;
   }

   /*                                            */
   switch (event) {
   case SMD_EVENT_OPEN:
      WPAL_TRACE(eWLAN_MODULE_DAL_CTRL, eWLAN_PAL_TRACE_LEVEL_INFO,
                 "%s: received SMD_EVENT_OPEN from SMD", __func__);
      /*                                                                   
                                  
       */
      if (WCTS_STATE_REM_CLOSED == pWCTSCb->wctsState)
      {
           WPAL_TRACE(eWLAN_MODULE_DAL_CTRL, eWLAN_PAL_TRACE_LEVEL_INFO,
                 "%s: received SMD_EVENT_OPEN in WCTS_STATE_REM_CLOSED state",
                 __func__);
           /*                                         */
           wpalDriverReInit();
           return;
      }
      palMsg = &pWCTSCb->wctsOpenMsg;
      break;

   case SMD_EVENT_DATA:
      if (WCTS_STATE_REM_CLOSED == pWCTSCb->wctsState)
      {
           WPAL_TRACE(eWLAN_MODULE_DAL_CTRL, eWLAN_PAL_TRACE_LEVEL_ERROR,
                 "%s: received SMD data when the state is remote closed ",
                 __func__);
           /*                                       */
           return;
      }
      WPAL_TRACE(eWLAN_MODULE_DAL_CTRL, eWLAN_PAL_TRACE_LEVEL_INFO,
                 "%s: received SMD_EVENT_DATA from SMD", __func__);
      palMsg = &pWCTSCb->wctsDataMsg;
      break;

   case SMD_EVENT_CLOSE:
      WPAL_TRACE(eWLAN_MODULE_DAL_CTRL, eWLAN_PAL_TRACE_LEVEL_INFO,
                 "%s: received SMD_EVENT_CLOSE from SMD", __func__);
      /*                                             
                                                            
                                               */
      pWCTSCb->wctsState = WCTS_STATE_REM_CLOSED;
      WPAL_TRACE(eWLAN_MODULE_DAL_CTRL, eWLAN_PAL_TRACE_LEVEL_INFO,
                 "%s: received SMD_EVENT_CLOSE WLAN driver going down now",
                 __func__);
      /*                             */
      wpalDriverShutdown();
      return;

   case SMD_EVENT_STATUS:
      WPAL_TRACE(eWLAN_MODULE_DAL_CTRL, eWLAN_PAL_TRACE_LEVEL_INFO,
                 "%s: received SMD_EVENT_STATUS from SMD", __func__);
      return;

   case SMD_EVENT_REOPEN_READY:
      WPAL_TRACE(eWLAN_MODULE_DAL_CTRL, eWLAN_PAL_TRACE_LEVEL_ERROR,
                 "%s: received SMD_EVENT_REOPEN_READY from SMD", __func__);

      /*                                                            
                                                                      
                                                                   */
      wpalEventSet(&pWCTSCb->wctsEvent);
      return;

   default:
      WPAL_TRACE(eWLAN_MODULE_DAL_CTRL, eWLAN_PAL_TRACE_LEVEL_ERROR,
                 "%s: Unexpected event %u received from SMD",
                 __func__, event);

      return;
   }

   /*                      */
   wpalPostCtrlMsg(WDI_GET_PAL_CTX(), palMsg);

} /*                   */



/*                                                                            
                                    
                                                                            */


/* 
                                                                           
                                                                            
                                                


                                                                  
                                                                         
                                                                           
                                                                       
                                                       

     
                                                                            
                                                                          
                                                

*/
WCTS_HandleType
WCTS_OpenTransport
(
   const wpt_uint8*         szName,
   wpt_uint32               uSize,
   WCTS_TransportCBsType*   wctsCBs
)
{
   WCTS_ControlBlockType*    pWCTSCb;
   wpt_status                status;
   int                       smdstatus;

   /*                                                                       */

   /*                                                                     
                 
                                                                          */
   if ((NULL == wctsCBs) || (NULL == szName) ||
       (NULL == wctsCBs->wctsNotifyCB) || (NULL == wctsCBs->wctsRxMsgCB)) {
      WPAL_TRACE(eWLAN_MODULE_DAL_CTRL, eWLAN_PAL_TRACE_LEVEL_ERROR,
                 "WCTS_OpenTransport: Invalid parameters received.");

      return NULL;
   }

   /*                                                              
                                              */
   if (gwctsHandle) {
       WPAL_TRACE(eWLAN_MODULE_DAL_CTRL, eWLAN_PAL_TRACE_LEVEL_INFO,
               "WCTS_OpenTransport port is already open\n");

       pWCTSCb = gwctsHandle;
       if (WCTS_CB_MAGIC != pWCTSCb->wctsMagic) {
           WPAL_TRACE(eWLAN_MODULE_DAL_CTRL, eWLAN_PAL_TRACE_LEVEL_FATAL,
                   "WCTS_OpenTransport: Invalid magic.");
           return NULL;
       }   
       pWCTSCb->wctsState = WCTS_STATE_OPEN;

       pWCTSCb->wctsNotifyCB((WCTS_HandleType)pWCTSCb,
               WCTS_EVENT_OPEN,
               pWCTSCb->wctsNotifyCBData);

       /*                                        
                                                                */
       smd_disable_read_intr(pWCTSCb->wctsChannel);

       return (WCTS_HandleType)pWCTSCb;
   }

#ifdef FEATURE_R33D
   if(port_open)
   {
      /*                                          */
      /*                                                     */
      ctsCB->wctsState = WCTS_STATE_OPEN;
      ctsCB->wctsNotifyCB((WCTS_HandleType)ctsCB,
                           WCTS_EVENT_OPEN,
                           ctsCB->wctsNotifyCBData);
      return (WCTS_HandleType)ctsCB;
   }
#endif /*              */

   /*                                             */
   pWCTSCb = wpalMemoryAllocate(sizeof(*pWCTSCb));
   if (NULL == pWCTSCb) {
      WPAL_TRACE(eWLAN_MODULE_DAL_CTRL, eWLAN_PAL_TRACE_LEVEL_ERROR,
                 "WCTS_OpenTransport: Memory allocation failure.");
      return NULL;
   }

   /*                                                                   
                                                                         
                                                                          
             */
   wpalMemoryZero(pWCTSCb, sizeof(*pWCTSCb));

#ifdef FEATURE_R33D
   smd_init(0);
   port_open = 1;
   ctsCB = pWCTSCb;
#endif /*              */

   /*                    */
   wpalEventInit(&pWCTSCb->wctsEvent);

   /*                                    */
   pWCTSCb->wctsNotifyCB       = wctsCBs->wctsNotifyCB;
   pWCTSCb->wctsNotifyCBData   = wctsCBs->wctsNotifyCBData;
   pWCTSCb->wctsRxMsgCB        = wctsCBs->wctsRxMsgCB;
   pWCTSCb->wctsRxMsgCBData    = wctsCBs->wctsRxMsgCBData;

   /*                                 */
   wpal_list_init(&pWCTSCb->wctsPendingQueue);
   pWCTSCb->wctsMagic   = WCTS_CB_MAGIC;
   pWCTSCb->wctsState   = WCTS_STATE_OPEN_PENDING;
   pWCTSCb->wctsChannel = NULL;

   /*                                                           
                                                               
             
    */
   pWCTSCb->wctsOpenMsg.callback = WCTS_PALOpenCallback;
   pWCTSCb->wctsOpenMsg.pContext = pWCTSCb;
   pWCTSCb->wctsOpenMsg.type= WPAL_MC_MSG_SMD_NOTIF_OPEN_SIG;

   pWCTSCb->wctsDataMsg.callback = WCTS_PALDataCallback;
   pWCTSCb->wctsDataMsg.pContext = pWCTSCb;
   pWCTSCb-> wctsDataMsg.type= WPAL_MC_MSG_SMD_NOTIF_DATA_SIG;

   /*                                                                     
                         
                                                                          */

   wpalEventReset(&pWCTSCb->wctsEvent);
   smdstatus = smd_named_open_on_edge(szName,
                                      SMD_APPS_WCNSS,
                                      &pWCTSCb->wctsChannel,
                                      pWCTSCb,
                                      WCTS_NotifyCallback);
   if (0 != smdstatus) {
      WPAL_TRACE(eWLAN_MODULE_DAL_CTRL, eWLAN_PAL_TRACE_LEVEL_ERROR,
                 "%s: smd_named_open_on_edge failed with status %d",
                 __func__, smdstatus);
      goto fail;
   }

   /*                                                           */
   status = wpalEventWait(&pWCTSCb->wctsEvent, WCTS_SMD_OPEN_TIMEOUT);
   if (eWLAN_PAL_STATUS_SUCCESS != status) {
      WPAL_TRACE(eWLAN_MODULE_DAL_CTRL, eWLAN_PAL_TRACE_LEVEL_ERROR,
                 "%s: failed to receive SMD_EVENT_OPEN",
                 __func__);
      /*                                                  */
      smdstatus = smd_close(pWCTSCb->wctsChannel);
      if (0 != smdstatus) {
         WPAL_TRACE(eWLAN_MODULE_DAL_CTRL, eWLAN_PAL_TRACE_LEVEL_ERROR,
                    "%s: smd_close failed with status %d",
                    __func__, smdstatus);
      }
      goto fail;
   }

   /*                                        
                                                             */
   smd_disable_read_intr(pWCTSCb->wctsChannel);

   /*                                             */
   gwctsHandle = pWCTSCb;
   return (WCTS_HandleType)pWCTSCb;

 fail:
   /*                                        */
   pWCTSCb->wctsMagic = 0;
   wpalMemoryFree(pWCTSCb);
   return NULL;

}/*                  */



/* 
                                                             
                                                            
                                                          
                                                      
                             


                                             

     
                        
*/
wpt_uint32
WCTS_CloseTransport
(
   WCTS_HandleType      wctsHandle
)
{
   WCTS_ControlBlockType* pWCTSCb = (WCTS_ControlBlockType*) wctsHandle;
   wpt_list_node*      pNode = NULL;
   WCTS_BufferType*    pBufferQueue = NULL;
   void*               pBuffer = NULL;
   wpt_status          status;
   int                 smdstatus;

   /*                                                                      */

   if ((NULL == pWCTSCb) || (WCTS_CB_MAGIC != pWCTSCb->wctsMagic)) {
      WPAL_TRACE(eWLAN_MODULE_DAL_CTRL, eWLAN_PAL_TRACE_LEVEL_ERROR,
                 "WCTS_CloseTransport: Invalid parameters received.");
      return eWLAN_PAL_STATUS_E_INVAL;
   }

#ifdef FEATURE_R33D
   /*                                       */
   /*                                                       */
   pWCTSCb->wctsState = WCTS_STATE_CLOSED;
   pWCTSCb->wctsNotifyCB((WCTS_HandleType)pWCTSCb,
                         WCTS_EVENT_CLOSE,
                         pWCTSCb->wctsNotifyCBData);

   printk(KERN_ERR "R33D Not need to close");
   return eWLAN_PAL_STATUS_SUCCESS;
#endif /*              */

   /*                                      */
   while (eWLAN_PAL_STATUS_SUCCESS ==
          wpal_list_remove_front(&pWCTSCb->wctsPendingQueue, &pNode)) {
      pBufferQueue = container_of(pNode, WCTS_BufferType, node);
      pBuffer = pBufferQueue->pBuffer;
      wpalMemoryFree(pBuffer);
      wpalMemoryFree(pBufferQueue);
   }

   /*                 */
   pWCTSCb->wctsState = WCTS_STATE_CLOSED;

   wpalEventReset(&pWCTSCb->wctsEvent);
   smdstatus = smd_close(pWCTSCb->wctsChannel);
   if (0 != smdstatus) {
      WPAL_TRACE(eWLAN_MODULE_DAL_CTRL, eWLAN_PAL_TRACE_LEVEL_ERROR,
                 "%s: smd_close failed with status %d",
                 __func__, smdstatus);
      /*                                                         
                                                                  
                                                     */

   } else {
      /*                                                             */
      status = wpalEventWait(&pWCTSCb->wctsEvent, WCTS_SMD_CLOSE_TIMEOUT);
      if (eWLAN_PAL_STATUS_SUCCESS != status) {
         WPAL_TRACE(eWLAN_MODULE_DAL_CTRL, eWLAN_PAL_TRACE_LEVEL_ERROR,
                    "%s: failed to receive SMD_EVENT_REOPEN_READY",
                    __func__);
      }

      /*                                                             
                                                                     
                                                                
                                                                  
                                                                     
                                                                  
                                                           */
      msleep(50);
   }

   /*                                     */
   pWCTSCb->wctsNotifyCB((WCTS_HandleType)pWCTSCb,
                         WCTS_EVENT_CLOSE,
                         pWCTSCb->wctsNotifyCBData);

   /*                      */
   pWCTSCb->wctsMagic = 0;
   wpalMemoryFree(pWCTSCb);
   gwctsHandle = NULL;

   return eWLAN_PAL_STATUS_SUCCESS;

}/*                   */



/* 
                                                             
                                               

                                                              
                                                             
                                                             
                          

                                                              
                                                         
                                   

                                             
                                     
                                       

     
                        
*/
wpt_uint32
WCTS_SendMessage
(
   WCTS_HandleType      wctsHandle,
   void*                pMsg,
   wpt_uint32           uLen
)
{
   WCTS_ControlBlockType*    pWCTSCb = (WCTS_ControlBlockType*) wctsHandle;
   WCTS_BufferType*          pBufferQueue;
   int                       len;
   int                       written = 0;
   int                       available;

   /*                                                                      */

   /*                                                                    
                 
                                                                         */
   if ((NULL == pWCTSCb) || (WCTS_CB_MAGIC != pWCTSCb->wctsMagic) ||
       (NULL == pMsg) || (0 == uLen) || (0x7fffffff < uLen)) {
      WPAL_TRACE(eWLAN_MODULE_DAL_CTRL, eWLAN_PAL_TRACE_LEVEL_ERROR,
                 "WCTS_SendMessage: Invalid parameters received.");
      WPAL_ASSERT(0);
      if (NULL != pMsg) {
         wpalMemoryFree(pMsg);
      }
      return eWLAN_PAL_STATUS_E_INVAL;
   }

   /*                                                            */
   len = (int)uLen;

   if (WCTS_STATE_OPEN == pWCTSCb->wctsState) {
      available = smd_write_avail(pWCTSCb->wctsChannel);
      if (available >= len) {
         written = smd_write(pWCTSCb->wctsChannel, pMsg, len);
      }
   } else if (WCTS_STATE_DEFERRED == pWCTSCb->wctsState) {
      WPAL_TRACE(eWLAN_MODULE_DAL_CTRL, eWLAN_PAL_TRACE_LEVEL_INFO,
                 "WCTS_SendMessage: FIFO space not available, the packets will be queued");
   } else {
      WPAL_TRACE(eWLAN_MODULE_DAL_CTRL, eWLAN_PAL_TRACE_LEVEL_ERROR,
                 "WCTS_SendMessage: Channel in illegal state [%d].",
                 pWCTSCb->wctsState);
      /*                                             */
      written = -1;
   }

   if (-1 == written) {
      /*               */
      WPAL_TRACE(eWLAN_MODULE_DAL_CTRL, eWLAN_PAL_TRACE_LEVEL_ERROR,
                 "WCTS_SendMessage: Failed to send message over the bus.");
      wpalMemoryFree(pMsg);
      return eWLAN_PAL_STATUS_E_FAILURE;
   } else if (written == len) {
      /*                                                 */
      wpalMemoryFree(pMsg);
   } else {
      /*                                               
                                             */
      pBufferQueue = wpalMemoryAllocate(sizeof(WCTS_BufferType));
      if (NULL == pBufferQueue) {
         WPAL_TRACE(eWLAN_MODULE_DAL_CTRL, eWLAN_PAL_TRACE_LEVEL_ERROR,
                    "WCTS_SendMessage: Cannot allocate memory for queuing the buffer");
         wpalMemoryFree(pMsg);
         WPAL_ASSERT(0);
         return eWLAN_PAL_STATUS_E_NOMEM;
      }

      pBufferQueue->bufferSize = len;
      pBufferQueue->pBuffer = pMsg;
      wpal_list_insert_back(&pWCTSCb->wctsPendingQueue, &pBufferQueue->node);

      /*                                                             
                                                                 
                                                                   
                             */
      if (WCTS_STATE_DEFERRED != pWCTSCb->wctsState) {

         /*                            
                                                            */
         pWCTSCb->wctsState = WCTS_STATE_DEFERRED;

         smd_enable_read_intr(pWCTSCb->wctsChannel);
      }

      /*                                                            */
      return eWLAN_PAL_STATUS_E_RESOURCES;
   }

   return eWLAN_PAL_STATUS_SUCCESS;

}/*                */

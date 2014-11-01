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
  @file vos_sched.c
  @brief VOS Scheduler Implementation

  Copyright (c) 2011 QUALCOMM Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/
/*                                                                           
                                            

                                                                       
                                                                

                                   

                                     
                                                                             
                                                                           */
/*                                                                           
                
                                                                           */
#include <vos_mq.h>
#include <vos_api.h>
#include <aniGlobal.h>
#include <sirTypes.h>
#include <halTypes.h>
#include <limApi.h>
#include <sme_Api.h>
#include <wlan_qct_sys.h>
#include <wlan_qct_tl.h>
#include "vos_sched.h"
#include <wlan_hdd_power.h>
#include "wlan_qct_wda.h"
#include "wlan_qct_pal_msg.h"
#include <linux/spinlock.h>
#include <linux/kthread.h>
/*                                                                           
                                         
                                                                           */
#define VOS_SCHED_THREAD_HEART_BEAT    INFINITE
/*                                                                           
                    
                                                                           */
/*                                                                           
                   
                                                                           */
static pVosSchedContext gpVosSchedContext;
static pVosWatchdogContext gpVosWatchdogContext;

/*                                                                           
                      
                                                                           */
static int VosMCThread(void *Arg);
static int VosWDThread(void *Arg);
static int VosTXThread(void *Arg);
static int VosRXThread(void *Arg);
void vos_sched_flush_rx_mqs(pVosSchedContext SchedContext);
extern v_VOID_t vos_core_return_msg(v_PVOID_t pVContext, pVosMsgWrapper pMsgWrapper);
/*                                                                           
                                   
                                                                           */

/*                                                                           
                                                         
                                                                 
                                 
                                             
                                                                     
                         
                                                                          

                                                          
                                                                         
                                             
                                                                         
                              
                                                                       
                                                    
                                                                       
                       
                                                                             
                  
                                                                     
                      
                                                                           */
VOS_STATUS
vos_sched_open
(
  v_PVOID_t        pVosContext,
  pVosSchedContext pSchedContext,
  v_SIZE_t         SchedCtxSize
)
{
  VOS_STATUS  vStatus = VOS_STATUS_SUCCESS;
/*                                                                         */
  VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO_HIGH,
             "%s: Opening the VOSS Scheduler",__func__);
  //              
  if ((pVosContext == NULL) || (pSchedContext == NULL)) {
     VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
             "%s: Null params being passed",__func__);
     return VOS_STATUS_E_FAILURE;
  }
  if (sizeof(VosSchedContext) != SchedCtxSize)
  {
     VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO_HIGH,
               "%s: Incorrect VOS Sched Context size passed",__func__);
     return VOS_STATUS_E_INVAL;
  }
  vos_mem_zero(pSchedContext, sizeof(VosSchedContext));
  pSchedContext->pVContext = pVosContext;
  vStatus = vos_sched_init_mqs(pSchedContext);
  if (!VOS_IS_STATUS_SUCCESS(vStatus))
  {
     VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
               "%s: Failed to initialize VOS Scheduler MQs",__func__);
     return vStatus;
  }
  //                                              
  init_completion(&pSchedContext->McStartEvent);
  init_completion(&pSchedContext->TxStartEvent);
  init_completion(&pSchedContext->RxStartEvent);
  init_completion(&pSchedContext->McShutdown);
  init_completion(&pSchedContext->TxShutdown);
  init_completion(&pSchedContext->RxShutdown);
  init_completion(&pSchedContext->ResumeMcEvent);
  init_completion(&pSchedContext->ResumeTxEvent);
  init_completion(&pSchedContext->ResumeRxEvent);

  spin_lock_init(&pSchedContext->McThreadLock);
  spin_lock_init(&pSchedContext->TxThreadLock);
  spin_lock_init(&pSchedContext->RxThreadLock);

  init_waitqueue_head(&pSchedContext->mcWaitQueue);
  pSchedContext->mcEventFlag = 0;
  init_waitqueue_head(&pSchedContext->txWaitQueue);
  pSchedContext->txEventFlag= 0;
  init_waitqueue_head(&pSchedContext->rxWaitQueue);
  pSchedContext->rxEventFlag= 0;
  /*
                                                                         
                              
    
                                                                          
                                    
  */
  gpVosSchedContext = pSchedContext;

  //                                      
  pSchedContext->McThread = kthread_create(VosMCThread, pSchedContext,
                                           "VosMCThread");
  if (IS_ERR(pSchedContext->McThread)) 
  {
     VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL,
               "%s: Could not Create VOSS Main Thread Controller",__func__);
     goto MC_THREAD_START_FAILURE;
  }
  wake_up_process(pSchedContext->McThread);
  VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO_HIGH,
            "%s: VOSS Main Controller thread Created",__func__);

  pSchedContext->TxThread = kthread_create(VosTXThread, pSchedContext,
                                           "VosTXThread");
  if (IS_ERR(pSchedContext->TxThread)) 
  {
     VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL,
               "%s: Could not Create VOSS TX Thread",__func__);
     goto TX_THREAD_START_FAILURE;
  }
  wake_up_process(pSchedContext->TxThread);
  VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO_HIGH,
             ("VOSS TX thread Created\n"));

  pSchedContext->RxThread = kthread_create(VosRXThread, pSchedContext,
                                           "VosRXThread");
  if (IS_ERR(pSchedContext->RxThread)) 
  {

     VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL,
               "%s: Could not Create VOSS RX Thread",__func__);
     goto RX_THREAD_START_FAILURE;

  }
  wake_up_process(pSchedContext->RxThread);
  VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO_HIGH,
             ("VOSS RX thread Created\n"));

  /*
                                                           
                                                         
  */
  wait_for_completion_interruptible(&pSchedContext->McStartEvent);
  VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO_HIGH,
               "%s: VOSS MC Thread has started",__func__);
  wait_for_completion_interruptible(&pSchedContext->TxStartEvent);
  VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO_HIGH,
               "%s: VOSS Tx Thread has started",__func__);
  wait_for_completion_interruptible(&pSchedContext->RxStartEvent);
  VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO_HIGH,
               "%s: VOSS Rx Thread has started",__func__);

  /*
                                                  
  */
  VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO_HIGH,
            "%s: VOSS Scheduler successfully Opened",__func__);
  return VOS_STATUS_SUCCESS;


RX_THREAD_START_FAILURE:
    //                                              
    set_bit(MC_SHUTDOWN_EVENT_MASK, &pSchedContext->txEventFlag);
    set_bit(MC_POST_EVENT_MASK, &pSchedContext->txEventFlag);
    wake_up_interruptible(&pSchedContext->txWaitQueue);
     //                   
    wait_for_completion_interruptible(&pSchedContext->TxShutdown);

TX_THREAD_START_FAILURE:
    //                                                
    set_bit(MC_SHUTDOWN_EVENT_MASK, &pSchedContext->mcEventFlag);
    set_bit(MC_POST_EVENT_MASK, &pSchedContext->mcEventFlag);
    wake_up_interruptible(&pSchedContext->mcWaitQueue);
    //                   
    wait_for_completion_interruptible(&pSchedContext->McShutdown);

MC_THREAD_START_FAILURE:
  //                                    
  vos_sched_deinit_mqs(pSchedContext);
  return VOS_STATUS_E_RESOURCES;

} /*                  */

VOS_STATUS vos_watchdog_open
(
  v_PVOID_t           pVosContext,
  pVosWatchdogContext pWdContext,
  v_SIZE_t            wdCtxSize
)
{
/*                                                                         */
  VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO_HIGH,
             "%s: Opening the VOSS Watchdog module",__func__);
  //             
  if ((pVosContext == NULL) || (pWdContext == NULL)) {
     VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
             "%s: Null params being passed",__func__);
     return VOS_STATUS_E_FAILURE;
  }
  if (sizeof(VosWatchdogContext) != wdCtxSize)
  {
     VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO_HIGH,
               "%s: Incorrect VOS Watchdog Context size passed",__func__);
     return VOS_STATUS_E_INVAL;
  }
  vos_mem_zero(pWdContext, sizeof(VosWatchdogContext));
  pWdContext->pVContext = pVosContext;
  gpVosWatchdogContext = pWdContext;

  //                                             
  init_completion(&pWdContext->WdStartEvent);
  init_completion(&pWdContext->WdShutdown);
  init_waitqueue_head(&pWdContext->wdWaitQueue);
  pWdContext->wdEventFlag = 0;

  //                    
  spin_lock_init(&pWdContext->wdLock);

  //                          
  pWdContext->WdThread = kthread_create(VosWDThread, pWdContext,"VosWDThread");
  
  if (IS_ERR(pWdContext->WdThread)) 
  {
     VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL,
               "%s: Could not Create Watchdog thread",__func__);
     return VOS_STATUS_E_RESOURCES;
  }  
  else
  {
     wake_up_process(pWdContext->WdThread);
  }
 /*
                                                     
                                                         
  */
  wait_for_completion_interruptible(&pWdContext->WdStartEvent);
  VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO_HIGH,
               "%s: VOSS Watchdog Thread has started",__func__);
  return VOS_STATUS_SUCCESS;
} /*                     */
/*                                                                           
                                                        
                                                          
                                                        
                          
                   
                                                                           */
static int
VosMCThread
(
  void * Arg
)
{
  pVosSchedContext pSchedContext = (pVosSchedContext)Arg;
  pVosMsgWrapper pMsgWrapper     = NULL;
  tpAniSirGlobal pMacContext     = NULL;
  tSirRetStatus macStatus        = eSIR_SUCCESS;
  VOS_STATUS vStatus             = VOS_STATUS_SUCCESS;
  int retWaitStatus              = 0;
  v_BOOL_t shutdown              = VOS_FALSE;
  hdd_context_t *pHddCtx         = NULL;
  v_CONTEXT_t pVosContext        = NULL;

  if (Arg == NULL)
  {
     VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
        "%s: Bad Args passed", __func__);
     return 0;
  }
  set_user_nice(current, -2);

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,8,0))
  daemonize("MC_Thread");
#endif

  /*
                                                                           
             
  */
  complete(&pSchedContext->McStartEvent);
  VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
      "%s: MC Thread %d (%s) starting up",__func__, current->pid, current->comm);

  /*                             */
  pVosContext = vos_get_global_context(VOS_MODULE_ID_SYS, NULL);
  if(!pVosContext) {
     hddLog(VOS_TRACE_LEVEL_FATAL,"%s: Global VOS context is Null", __func__);
     return 0;
  }

  /*                     */
  pHddCtx = (hdd_context_t *)vos_get_context(VOS_MODULE_ID_HDD, pVosContext );
  if(!pHddCtx) {
     hddLog(VOS_TRACE_LEVEL_FATAL,"%s: HDD context is Null",__func__);
     return 0;
  }

  while(!shutdown)
  {
    //                                              
    retWaitStatus = wait_event_interruptible(pSchedContext->mcWaitQueue,
       test_bit(MC_POST_EVENT_MASK, &pSchedContext->mcEventFlag) || 
       test_bit(MC_SUSPEND_EVENT_MASK, &pSchedContext->mcEventFlag));

    if(retWaitStatus == -ERESTARTSYS)
    {
      VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
         "%s: wait_event_interruptible returned -ERESTARTSYS", __func__);
      break;
    }
    clear_bit(MC_POST_EVENT_MASK, &pSchedContext->mcEventFlag);

    while(1)
    {
      //                              
      if(test_bit(MC_SHUTDOWN_EVENT_MASK, &pSchedContext->mcEventFlag))
      {
        VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
                "%s: MC thread signaled to shutdown", __func__);
        shutdown = VOS_TRUE;
        /*                                  */
        if(test_bit(MC_SUSPEND_EVENT_MASK, &pSchedContext->mcEventFlag))
        {
           clear_bit(MC_SUSPEND_EVENT_MASK, &pSchedContext->mcEventFlag);
        
           /*                                   */
           complete(&pHddCtx->mc_sus_event_var);
        }
        break;
      }
      /*
                            
                                                  
      */
      if (!vos_is_mq_empty(&pSchedContext->wdiMcMq))
      {
        wpt_msg *pWdiMsg;
        /*
                                        
        */
        VOS_TRACE(VOS_MODULE_ID_WDI, VOS_TRACE_LEVEL_INFO,
                  ("Servicing the VOS MC WDI Message queue"));

        pMsgWrapper = vos_mq_get(&pSchedContext->wdiMcMq);

        if (pMsgWrapper == NULL)
        {
           VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
               "%s: pMsgWrapper is NULL", __func__);
           VOS_ASSERT(0);
           break;
        }

        pWdiMsg = (wpt_msg *)pMsgWrapper->pVosMsg->bodyptr;

        if(pWdiMsg == NULL || pWdiMsg->callback == NULL)
        {
           VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
               "%s: WDI Msg or Callback is NULL", __func__);
           VOS_ASSERT(0);
           break;
        }

        pWdiMsg->callback(pWdiMsg);

        /* 
                                     
        */
        vos_core_return_msg(pSchedContext->pVContext, pMsgWrapper);

        continue;
      }

      //                          
      if (!vos_is_mq_empty(&pSchedContext->sysMcMq))
      {
        //                              
        VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
                  "%s: Servicing the VOS SYS MC Message queue",__func__);
        pMsgWrapper = vos_mq_get(&pSchedContext->sysMcMq);
        if (pMsgWrapper == NULL)
        {
           VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
              "%s: pMsgWrapper is NULL", __func__);
           VOS_ASSERT(0);
           break;
        }
        vStatus = sysMcProcessMsg(pSchedContext->pVContext,
           pMsgWrapper->pVosMsg);
        if (!VOS_IS_STATUS_SUCCESS(vStatus))
        {
           VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                     "%s: Issue Processing SYS message",__func__);
        }
        //                          
        vos_core_return_msg(pSchedContext->pVContext, pMsgWrapper);
        continue;
      }
      //                    
      if (!vos_is_mq_empty(&pSchedContext->wdaMcMq))
      {
        //                              
        VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
                 "%s: Servicing the VOS WDA MC Message queue",__func__);
        pMsgWrapper = vos_mq_get(&pSchedContext->wdaMcMq);
        if (pMsgWrapper == NULL)
        {
           VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
              "%s: pMsgWrapper is NULL", __func__);
           VOS_ASSERT(0);
           break;
        }
        vStatus = WDA_McProcessMsg( pSchedContext->pVContext, pMsgWrapper->pVosMsg);
        if (!VOS_IS_STATUS_SUCCESS(vStatus))
        {
           VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                     "%s: Issue Processing WDA message",__func__);
        }
        //                           
        vos_core_return_msg(pSchedContext->pVContext, pMsgWrapper);
        continue;
      }
      //                   
      if (!vos_is_mq_empty(&pSchedContext->peMcMq))
      {
        //                             
        VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
                  "%s: Servicing the VOS PE MC Message queue",__func__);
        pMsgWrapper = vos_mq_get(&pSchedContext->peMcMq);
        if (NULL == pMsgWrapper)
        {
           VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
              "%s: pMsgWrapper is NULL", __func__);
           VOS_ASSERT(0);
           break;
        }

        /*                       */
        pMacContext = vos_get_context(VOS_MODULE_ID_PE, pSchedContext->pVContext);
        if (NULL == pMacContext)
        {
           VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
                     "MAC Context not ready yet");
           vos_core_return_msg(pSchedContext->pVContext, pMsgWrapper);
           continue;
        }

        macStatus = peProcessMessages( pMacContext, (tSirMsgQ*)pMsgWrapper->pVosMsg);
        if (eSIR_SUCCESS != macStatus)
        {
          VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                     "%s: Issue Processing PE message",__func__);
        }
        //                           
        vos_core_return_msg(pSchedContext->pVContext, pMsgWrapper);
        continue;
      }
      /*                       */
      if (!vos_is_mq_empty(&pSchedContext->smeMcMq))
      {
        /*                               */
        VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
                  "%s: Servicing the VOS SME MC Message queue",__func__);
        pMsgWrapper = vos_mq_get(&pSchedContext->smeMcMq);
        if (NULL == pMsgWrapper)
        {
           VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
               "%s: pMsgWrapper is NULL", __func__);
           VOS_ASSERT(0);
           break;
        }

        /*                       */
        pMacContext = vos_get_context(VOS_MODULE_ID_SME, pSchedContext->pVContext);
        if (NULL == pMacContext)
        {
           VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
                     "MAC Context not ready yet");
           vos_core_return_msg(pSchedContext->pVContext, pMsgWrapper);
           continue;
        }

        vStatus = sme_ProcessMsg( (tHalHandle)pMacContext, pMsgWrapper->pVosMsg);
        if (!VOS_IS_STATUS_SUCCESS(vStatus))
        {
          VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                     "%s: Issue Processing SME message",__func__);
        }
        //                           
        vos_core_return_msg(pSchedContext->pVContext, pMsgWrapper);
        continue;
      }
      /*                      */
      if (!vos_is_mq_empty(&pSchedContext->tlMcMq))
      {
        //                             
        VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
                  ("Servicing the VOS TL MC Message queue"));
        pMsgWrapper = vos_mq_get(&pSchedContext->tlMcMq);
        if (pMsgWrapper == NULL)
        {
           VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
              "%s: pMsgWrapper is NULL", __func__);
           VOS_ASSERT(0);
           break;
        }
        vStatus = WLANTL_McProcessMsg( pSchedContext->pVContext,
            pMsgWrapper->pVosMsg);
        if (!VOS_IS_STATUS_SUCCESS(vStatus))
        {
          VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                     "%s: Issue Processing TL message",__func__);
        }
        //                           
        vos_core_return_msg(pSchedContext->pVContext, pMsgWrapper);
        continue;
      }
      /*                                  */
      if(test_bit(MC_SUSPEND_EVENT_MASK, &pSchedContext->mcEventFlag))
      {
        clear_bit(MC_SUSPEND_EVENT_MASK, &pSchedContext->mcEventFlag);
        spin_lock(&pSchedContext->McThreadLock);

        /*                     */
        complete(&pHddCtx->mc_sus_event_var);

        INIT_COMPLETION(pSchedContext->ResumeMcEvent);
        spin_unlock(&pSchedContext->McThreadLock);

        /*                            */
        wait_for_completion_interruptible(&pSchedContext->ResumeMcEvent);
      }
      break; //                        
    } //                              
  } //           
  //                                       
  VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
      "%s: MC Thread exiting!!!!", __func__);
  complete_and_exit(&pSchedContext->McShutdown, 0);
} /*               */

v_BOOL_t isWDresetInProgress(void)
{
   VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
                "%s: Reset is in Progress...",__func__);
   if(gpVosWatchdogContext!=NULL)
   {
      return gpVosWatchdogContext->resetInProgress;
   }
   else
   {
      return FALSE;
   }
}
/*                                                                           
                                                 
                                              
                                                        
                          
                   
                                                                           */
static int
VosWDThread
(
  void * Arg
)
{
  pVosWatchdogContext pWdContext = (pVosWatchdogContext)Arg;
  int retWaitStatus              = 0;
  v_BOOL_t shutdown              = VOS_FALSE;
  VOS_STATUS vosStatus = VOS_STATUS_SUCCESS;
  hdd_context_t *pHddCtx         = NULL;
  v_CONTEXT_t pVosContext        = NULL;
  set_user_nice(current, -3);

  if (Arg == NULL)
  {
     VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
        "%s: Bad Args passed", __func__);
     return 0;
  }

  /*                             */
  pVosContext = vos_get_global_context(VOS_MODULE_ID_SYS, NULL);

  if(!pVosContext)
  {
     hddLog(VOS_TRACE_LEVEL_FATAL,"%s: Global VOS context is Null", __func__);
     return 0;
  }

  /*                     */
  pHddCtx = (hdd_context_t *)vos_get_context(VOS_MODULE_ID_HDD, pVosContext );

  if(!pHddCtx)
  {
     hddLog(VOS_TRACE_LEVEL_FATAL,"%s: HDD context is Null",__func__);
     return 0;
  }

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,8,0))
  daemonize("WD_Thread");
#endif

  /*
                                                                    
             
  */
  complete(&pWdContext->WdStartEvent);
  VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
      "%s: Watchdog Thread %d (%s) starting up",__func__, current->pid, current->comm);

  while(!shutdown)
  {
    //                                                       
    retWaitStatus = wait_event_interruptible(pWdContext->wdWaitQueue,
       test_bit(WD_POST_EVENT_MASK, &pWdContext->wdEventFlag));
    if(retWaitStatus == -ERESTARTSYS)
    {
      VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
         "%s: wait_event_interruptible returned -ERESTARTSYS", __func__);
      break;
    }
    clear_bit(WD_POST_EVENT_MASK, &pWdContext->wdEventFlag);
    while(1)
    {
      //                                    
      if(test_bit(WD_SHUTDOWN_EVENT_MASK, &pWdContext->wdEventFlag))
      {
        VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
                "%s: Watchdog thread signaled to shutdown", __func__);

        clear_bit(WD_SHUTDOWN_EVENT_MASK, &pWdContext->wdEventFlag);
        shutdown = VOS_TRUE;
        break;
      }
      /*                                           */
      else if(test_bit(WD_WLAN_SHUTDOWN_EVENT_MASK, &pWdContext->wdEventFlag))
      {
        VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL,
                "%s: Watchdog thread signaled to perform WLAN shutdown",__func__);
        clear_bit(WD_WLAN_SHUTDOWN_EVENT_MASK, &pWdContext->wdEventFlag);

        //                     
        if(!pWdContext->resetInProgress)
        {
          pWdContext->resetInProgress = true;
          vosStatus = hdd_wlan_shutdown();

          if (! VOS_IS_STATUS_SUCCESS(vosStatus))
          {
             VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL,
                     "%s: Failed to shutdown WLAN",__func__);
             VOS_ASSERT(0);
             goto err_reset;
          }
        }
      }
      /*                                          */
      else if(test_bit(WD_WLAN_REINIT_EVENT_MASK, &pWdContext->wdEventFlag))
      {
        VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL,
                "%s: Watchdog thread signaled to perform WLAN re-init",__func__);
        clear_bit(WD_WLAN_REINIT_EVENT_MASK, &pWdContext->wdEventFlag);

        //                    
        if(!pWdContext->resetInProgress)
        {
          VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL,
          "%s: Trying to do WLAN re-init when it is not shutdown !!",__func__);
        }
        vosStatus = hdd_wlan_re_init();

        if (! VOS_IS_STATUS_SUCCESS(vosStatus))
        {
          VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL,
                  "%s: Failed to re-init WLAN",__func__);
          VOS_ASSERT(0);
          goto err_reset;
        }
        pWdContext->resetInProgress = false;
        complete(&pHddCtx->ssr_comp_var);
      }
      else
      {
        //                                          
        VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL,
        "%s: Watchdog thread woke up unnecessarily",__func__);
      }
      break;
    } //                              
  } //               

  //                                             
  VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
      "%s: Watchdog Thread exiting !!!!", __func__);
  complete_and_exit(&pWdContext->WdShutdown, 0);

err_reset:
    VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL,
      "%s: Watchdog Thread Failed to Reset, Exiting!!!!", __func__);
    return 0;

} /*               */

/*                                                                           
                                                
                                                          
                                                        

                          
                   
                                                                           */
static int VosTXThread ( void * Arg )
{
  pVosSchedContext pSchedContext = (pVosSchedContext)Arg;
  pVosMsgWrapper   pMsgWrapper   = NULL;
  VOS_STATUS       vStatus       = VOS_STATUS_SUCCESS;
  int              retWaitStatus = 0;
  v_BOOL_t shutdown = VOS_FALSE;
  hdd_context_t *pHddCtx         = NULL;
  v_CONTEXT_t pVosContext        = NULL;

  set_user_nice(current, -1);
  
#ifdef WLAN_FEATURE_11AC_HIGH_TP
  set_wake_up_idle(true);
#endif

  if (Arg == NULL)
  {
     VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
         "%s Bad Args passed", __func__);
     return 0;
  }

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,8,0))
  daemonize("TX_Thread");
#endif

  /*
                                                                           
             
  */
  complete(&pSchedContext->TxStartEvent);
  VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
      "%s: TX Thread %d (%s) starting up!",__func__, current->pid, current->comm);

  /*                             */
  pVosContext = vos_get_global_context(VOS_MODULE_ID_SYS, NULL);
  if(!pVosContext) {
     hddLog(VOS_TRACE_LEVEL_FATAL,"%s: Global VOS context is Null", __func__);
     return 0;
  }

  /*                     */
  pHddCtx = (hdd_context_t *)vos_get_context(VOS_MODULE_ID_HDD, pVosContext );
  if(!pHddCtx) {
     hddLog(VOS_TRACE_LEVEL_FATAL,"%s: HDD context is Null",__func__);
     return 0;
  }


  while(!shutdown)
  {
    //                                              
    retWaitStatus = wait_event_interruptible(pSchedContext->txWaitQueue,
        test_bit(TX_POST_EVENT_MASK, &pSchedContext->txEventFlag) || 
        test_bit(TX_SUSPEND_EVENT_MASK, &pSchedContext->txEventFlag));


    if(retWaitStatus == -ERESTARTSYS)
    {
        VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
           "%s: wait_event_interruptible returned -ERESTARTSYS", __func__);
        break;
    }
    clear_bit(TX_POST_EVENT_MASK, &pSchedContext->txEventFlag);

    while(1)
    {
      if(test_bit(TX_SHUTDOWN_EVENT_MASK, &pSchedContext->txEventFlag))
      {
        VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
                 "%s: TX thread signaled to shutdown", __func__);
        shutdown = VOS_TRUE;
        /*                                  */
        if(test_bit(TX_SUSPEND_EVENT_MASK, &pSchedContext->txEventFlag))
        {
           clear_bit(TX_SUSPEND_EVENT_MASK, &pSchedContext->txEventFlag);
        
           /*                                   */
           complete(&pHddCtx->tx_sus_event_var);
        }
        break;
      }
      //                          
      if (!vos_is_mq_empty(&pSchedContext->sysTxMq))
      {
        //                              
        VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
                "%s: Servicing the VOS SYS TX Message queue",__func__);
        pMsgWrapper = vos_mq_get(&pSchedContext->sysTxMq);
        if (pMsgWrapper == NULL)
        {
           VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
               "%s: pMsgWrapper is NULL", __func__);
           VOS_ASSERT(0);
           break;
        }
        vStatus = sysTxProcessMsg( pSchedContext->pVContext,
                                   pMsgWrapper->pVosMsg);
        if (!VOS_IS_STATUS_SUCCESS(vStatus))
        {
          VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                     "%s: Issue Processing TX SYS message",__func__);
        }
        //                           
        vos_core_return_msg(pSchedContext->pVContext, pMsgWrapper);
        continue;
      }
      //                       
      if (!vos_is_mq_empty(&pSchedContext->tlTxMq))
      {
        //                             
        VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
                "%s: Servicing the VOS TL TX Message queue",__func__);
        pMsgWrapper = vos_mq_get(&pSchedContext->tlTxMq);
        if (pMsgWrapper == NULL)
        {
           VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
               "%s: pMsgWrapper is NULL", __func__);
           VOS_ASSERT(0);
           break;
        }
        vStatus = WLANTL_TxProcessMsg( pSchedContext->pVContext,
                                       pMsgWrapper->pVosMsg);
        if (!VOS_IS_STATUS_SUCCESS(vStatus))
        {
          VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                     "%s: Issue Processing TX TL message",__func__);
        }
        //                           
        vos_core_return_msg(pSchedContext->pVContext, pMsgWrapper);
        continue;
      }
      //                    
      if (!vos_is_mq_empty(&pSchedContext->wdiTxMq))
      {
        wpt_msg *pWdiMsg;
        VOS_TRACE(VOS_MODULE_ID_WDI, VOS_TRACE_LEVEL_INFO,
                  "%s: Servicing the VOS TX WDI Message queue",__func__);

        pMsgWrapper = vos_mq_get(&pSchedContext->wdiTxMq);

        if (pMsgWrapper == NULL)
        {
           VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
               "%s: pMsgWrapper is NULL", __func__);
           VOS_ASSERT(0);
           break;
        }

        pWdiMsg = (wpt_msg *)pMsgWrapper->pVosMsg->bodyptr;

        if(pWdiMsg == NULL || pWdiMsg->callback == NULL)
        {
           VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
               "%s: WDI Msg or Callback is NULL", __func__);
           VOS_ASSERT(0);
           break;
        }
        
        pWdiMsg->callback(pWdiMsg);

        //                           
        vos_core_return_msg(pSchedContext->pVContext, pMsgWrapper);

        continue;
      }
      /*                                  */
      if(test_bit(TX_SUSPEND_EVENT_MASK, &pSchedContext->txEventFlag))
      {
        clear_bit(TX_SUSPEND_EVENT_MASK, &pSchedContext->txEventFlag);
        spin_lock(&pSchedContext->TxThreadLock);

        /*                     */
        complete(&pHddCtx->tx_sus_event_var);

        INIT_COMPLETION(pSchedContext->ResumeTxEvent);
        spin_unlock(&pSchedContext->TxThreadLock);

        /*                            */
        wait_for_completion_interruptible(&pSchedContext->ResumeTxEvent);
      }

      break; //                        
    } //                              
  } //           
  //                                       
  VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
      "%s: TX Thread exiting!!!!", __func__);
  complete_and_exit(&pSchedContext->TxShutdown, 0);
} /*               */

/*                                                                           
                                                
                                                        
                                                        

                          
                   
                                                                           */

static int VosRXThread ( void * Arg )
{
  pVosSchedContext pSchedContext = (pVosSchedContext)Arg;
  pVosMsgWrapper   pMsgWrapper   = NULL;
  int              retWaitStatus = 0;
  v_BOOL_t shutdown = VOS_FALSE;
  hdd_context_t *pHddCtx         = NULL;
  v_CONTEXT_t pVosContext        = NULL;
  VOS_STATUS       vStatus       = VOS_STATUS_SUCCESS;

  set_user_nice(current, -1);
  
#ifdef WLAN_FEATURE_11AC_HIGH_TP
  set_wake_up_idle(true);
#endif

  if (Arg == NULL)
  {
     VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
         "%s Bad Args passed", __func__);
     return 0;
  }

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,8,0))
  daemonize("RX_Thread");
#endif

  /*
                                                                           
             
  */
  complete(&pSchedContext->RxStartEvent);
  VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
      "%s: RX Thread %d (%s) starting up!",__func__, current->pid, current->comm);

  /*                             */
  pVosContext = vos_get_global_context(VOS_MODULE_ID_SYS, NULL);
  if(!pVosContext) {
     hddLog(VOS_TRACE_LEVEL_FATAL,"%s: Global VOS context is Null", __func__);
     return 0;
  }

  /*                     */
  pHddCtx = (hdd_context_t *)vos_get_context(VOS_MODULE_ID_HDD, pVosContext );
  if(!pHddCtx) {
     hddLog(VOS_TRACE_LEVEL_FATAL,"%s: HDD context is Null",__func__);
     return 0;
  }

  while(!shutdown)
  {
    //                                              
    retWaitStatus = wait_event_interruptible(pSchedContext->rxWaitQueue,
        test_bit(RX_POST_EVENT_MASK, &pSchedContext->rxEventFlag) || 
        test_bit(RX_SUSPEND_EVENT_MASK, &pSchedContext->rxEventFlag));


    if(retWaitStatus == -ERESTARTSYS)
    {
        VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
           "%s: wait_event_interruptible returned -ERESTARTSYS", __func__);
        break;
    }
    clear_bit(RX_POST_EVENT_MASK, &pSchedContext->rxEventFlag);

    while(1)
    {
      if(test_bit(RX_SHUTDOWN_EVENT_MASK, &pSchedContext->rxEventFlag))
      {
        VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
                 "%s: RX thread signaled to shutdown", __func__);
        shutdown = VOS_TRUE;
        /*                                  */
        if(test_bit(RX_SUSPEND_EVENT_MASK, &pSchedContext->rxEventFlag))
        {
           clear_bit(RX_SUSPEND_EVENT_MASK, &pSchedContext->rxEventFlag);
        
           /*                                   */
           complete(&pHddCtx->rx_sus_event_var);
        }
        break;
      }


      //                          
      if (!vos_is_mq_empty(&pSchedContext->sysRxMq))
      {
        //                              
        VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
                "%s: Servicing the VOS SYS RX Message queue",__func__);
        pMsgWrapper = vos_mq_get(&pSchedContext->sysRxMq);
        if (pMsgWrapper == NULL)
        {
           VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
               "%s: pMsgWrapper is NULL", __func__);
           VOS_ASSERT(0);
           break;
        }
        vStatus = sysRxProcessMsg( pSchedContext->pVContext,
                                   pMsgWrapper->pVosMsg);
        if (!VOS_IS_STATUS_SUCCESS(vStatus))
        {
          VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                     "%s: Issue Processing TX SYS message",__func__);
        }
        //                           
        vos_core_return_msg(pSchedContext->pVContext, pMsgWrapper);
        continue;
      }

      //                    
      if (!vos_is_mq_empty(&pSchedContext->wdiRxMq))
      {
        wpt_msg *pWdiMsg;
        VOS_TRACE(VOS_MODULE_ID_WDI, VOS_TRACE_LEVEL_INFO,
                  "%s: Servicing the VOS RX WDI Message queue",__func__);

        pMsgWrapper = vos_mq_get(&pSchedContext->wdiRxMq);
        if ((NULL == pMsgWrapper) || (NULL == pMsgWrapper->pVosMsg))
        {
          VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                    "%s: wdiRxMq message is NULL", __func__);
          VOS_ASSERT(0);
          //                                                 
        }
        else
        {
          pWdiMsg = (wpt_msg *)pMsgWrapper->pVosMsg->bodyptr;
          if ((NULL == pWdiMsg) || (NULL == pWdiMsg->callback))
          {
            VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                      "%s: WDI Msg or callback is NULL", __func__);
            VOS_ASSERT(0);
          }
          else
          {
            //                           
            pWdiMsg->callback(pWdiMsg);
          }

          //                           
          vos_core_return_msg(pSchedContext->pVContext, pMsgWrapper);
        }
        continue;
      }

      /*                                  */
      if(test_bit(RX_SUSPEND_EVENT_MASK, &pSchedContext->rxEventFlag))
      {
        clear_bit(RX_SUSPEND_EVENT_MASK, &pSchedContext->rxEventFlag);
        spin_lock(&pSchedContext->RxThreadLock);

        /*                     */
        complete(&pHddCtx->rx_sus_event_var);

        INIT_COMPLETION(pSchedContext->ResumeRxEvent);
        spin_unlock(&pSchedContext->RxThreadLock);

        /*                            */
        wait_for_completion_interruptible(&pSchedContext->ResumeRxEvent);
      }

      break; //                        
    } //                              
  } //           
  //                                       
  VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
      "%s: RX Thread exiting!!!!", __func__);
  complete_and_exit(&pSchedContext->RxShutdown, 0);
} /*               */

/*                                                                           
                                                     
                                                              
                          
                                         
                                           
                              

                                                          
                                                                         
                              
                                                                             
                  
                                                                     
                       
                                                                           */
VOS_STATUS vos_sched_close ( v_PVOID_t pVosContext )
{
    VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO_HIGH,
        "%s: invoked", __func__);
    if (gpVosSchedContext == NULL)
    {
       VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
           "%s: gpVosSchedContext == NULL\n",__func__);
       return VOS_STATUS_E_FAILURE;
    }

    //                    
    set_bit(MC_SHUTDOWN_EVENT_MASK, &gpVosSchedContext->mcEventFlag);
    set_bit(MC_POST_EVENT_MASK, &gpVosSchedContext->mcEventFlag);
    wake_up_interruptible(&gpVosSchedContext->mcWaitQueue);
    //                   
    wait_for_completion(&gpVosSchedContext->McShutdown);
    gpVosSchedContext->McThread = 0;

    //                    
    set_bit(TX_SHUTDOWN_EVENT_MASK, &gpVosSchedContext->txEventFlag);
    set_bit(TX_POST_EVENT_MASK, &gpVosSchedContext->txEventFlag);
    wake_up_interruptible(&gpVosSchedContext->txWaitQueue);
    //                   
    wait_for_completion(&gpVosSchedContext->TxShutdown);
    gpVosSchedContext->TxThread = 0;

    //                    
    set_bit(RX_SHUTDOWN_EVENT_MASK, &gpVosSchedContext->rxEventFlag);
    set_bit(RX_POST_EVENT_MASK, &gpVosSchedContext->rxEventFlag);
    wake_up_interruptible(&gpVosSchedContext->rxWaitQueue);
    //                   
    wait_for_completion(&gpVosSchedContext->RxShutdown);
    gpVosSchedContext->RxThread = 0;

    //                                           
    vos_sched_flush_mc_mqs(gpVosSchedContext);
    vos_sched_flush_tx_mqs(gpVosSchedContext);
    vos_sched_flush_rx_mqs(gpVosSchedContext);

    //                     
    vos_sched_deinit_mqs(gpVosSchedContext);

    return VOS_STATUS_SUCCESS;
} /*                   */

VOS_STATUS vos_watchdog_close ( v_PVOID_t pVosContext )
{
    VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO_HIGH,
        "%s: vos_watchdog closing now", __func__);
    if (gpVosWatchdogContext == NULL)
    {
       VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
           "%s: gpVosWatchdogContext is NULL\n",__func__);
       return VOS_STATUS_E_FAILURE;
    }
    set_bit(WD_SHUTDOWN_EVENT_MASK, &gpVosWatchdogContext->wdEventFlag);
    set_bit(WD_POST_EVENT_MASK, &gpVosWatchdogContext->wdEventFlag);
    wake_up_interruptible(&gpVosWatchdogContext->wdWaitQueue);
    //                                
    wait_for_completion(&gpVosWatchdogContext->WdShutdown);
    return VOS_STATUS_SUCCESS;
} /*                      */

VOS_STATUS vos_watchdog_chip_reset ( vos_chip_reset_reason_type  reason )
{
    return VOS_STATUS_SUCCESS;
} /*                           */

/*                                                                           
                                                                         
                                                                     
                 
                                                              
                                                                         
                              
                                                                       
                                                    

                          
                                                                           */
VOS_STATUS vos_sched_init_mqs ( pVosSchedContext pSchedContext )
{
  VOS_STATUS vStatus = VOS_STATUS_SUCCESS;
  //                                     
  VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO_HIGH,
            "%s: Initializing the WDA MC Message queue",__func__);
  vStatus = vos_mq_init(&pSchedContext->wdaMcMq);
  if (! VOS_IS_STATUS_SUCCESS(vStatus))
  {
    VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
            "%s: Failed to init WDA MC Message queue",__func__);
    VOS_ASSERT(0);
    return vStatus;
  }
  VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO_HIGH,
            "%s: Initializing the PE MC Message queue",__func__);
  vStatus = vos_mq_init(&pSchedContext->peMcMq);
  if (! VOS_IS_STATUS_SUCCESS(vStatus))
  {
    VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
            "%s: Failed to init PE MC Message queue",__func__);
    VOS_ASSERT(0);
    return vStatus;
  }
  VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO_HIGH,
            "%s: Initializing the SME MC Message queue", __func__);
  vStatus = vos_mq_init(&pSchedContext->smeMcMq);
  if (! VOS_IS_STATUS_SUCCESS(vStatus))
  {
    VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
            "%s: Failed to init SME MC Message queue",__func__);
    VOS_ASSERT(0);
    return vStatus;
  }
  VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO_HIGH,
            "%s: Initializing the TL MC Message queue",__func__);
  vStatus = vos_mq_init(&pSchedContext->tlMcMq);
  if (! VOS_IS_STATUS_SUCCESS(vStatus))
  {
    VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
            "%s: Failed to init TL MC Message queue",__func__);
    VOS_ASSERT(0);
    return vStatus;
  }
  VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO_HIGH,
            "%s: Initializing the SYS MC Message queue",__func__);
  vStatus = vos_mq_init(&pSchedContext->sysMcMq);
  if (! VOS_IS_STATUS_SUCCESS(vStatus))
  {
    VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
            "%s: Failed to init SYS MC Message queue",__func__);
    VOS_ASSERT(0);
    return vStatus;
  }
  VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO_HIGH,
            "%s: Initializing the WDI MC Message queue",__func__);

  vStatus = vos_mq_init(&pSchedContext->wdiMcMq);
  if (! VOS_IS_STATUS_SUCCESS(vStatus))
  {
    VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
            "%s: Failed to init WDI MC Message queue",__func__);
    VOS_ASSERT(0);
    return vStatus;
  }

  VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO_HIGH,
            "%s: Initializing the TL Tx Message queue",__func__);
  vStatus = vos_mq_init(&pSchedContext->tlTxMq);
  if (! VOS_IS_STATUS_SUCCESS(vStatus))
  {
    VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
            "%s: Failed to init TL TX Message queue",__func__);
    VOS_ASSERT(0);
    return vStatus;
  }
  VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO_HIGH,
            "%s: Initializing the WDI Tx Message queue",__func__);
  vStatus = vos_mq_init(&pSchedContext->wdiTxMq);
  if (! VOS_IS_STATUS_SUCCESS(vStatus))
  {
    VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
            "%s: Failed to init WDI TX Message queue",__func__);
    VOS_ASSERT(0);
    return vStatus;
  }

  VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO_HIGH,
            "%s: Initializing the WDI Rx Message queue",__func__);

  vStatus = vos_mq_init(&pSchedContext->wdiRxMq);
  if (! VOS_IS_STATUS_SUCCESS(vStatus))
  {
    VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
            "%s: Failed to init WDI RX Message queue",__func__);
    VOS_ASSERT(0);
    return vStatus;
  }

  VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO_HIGH,
            "%s: Initializing the SYS Tx Message queue",__func__);
  vStatus = vos_mq_init(&pSchedContext->sysTxMq);
  if (! VOS_IS_STATUS_SUCCESS(vStatus))
  {
    VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
            "%s: Failed to init SYS TX Message queue",__func__);
    VOS_ASSERT(0);
    return vStatus;
  }

  vStatus = vos_mq_init(&pSchedContext->sysRxMq);
  if (! VOS_IS_STATUS_SUCCESS(vStatus))
  {
    VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
            "%s: Failed to init SYS RX Message queue",__func__);
    VOS_ASSERT(0);
    return vStatus;
  }
  return VOS_STATUS_SUCCESS;
} /*                      */

/*                                                                           
                                                                             
                                                                       
                 
                                                              
              
                            
                                                                           */
void vos_sched_deinit_mqs ( pVosSchedContext pSchedContext )
{
  //                                    
 //       
  VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO_HIGH,
            "%s De-Initializing the WDA MC Message queue",__func__);
  vos_mq_deinit(&pSchedContext->wdaMcMq);
  //     
  VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO_HIGH,
            "%s De-Initializing the PE MC Message queue",__func__);
  vos_mq_deinit(&pSchedContext->peMcMq);
  //      
  VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO_HIGH,
            "%s De-Initializing the SME MC Message queue",__func__);
  vos_mq_deinit(&pSchedContext->smeMcMq);
  //     
  VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO_HIGH,
            "%s De-Initializing the TL MC Message queue",__func__);
  vos_mq_deinit(&pSchedContext->tlMcMq);
  //      
  VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO_HIGH,
            "%s De-Initializing the SYS MC Message queue",__func__);
  vos_mq_deinit(&pSchedContext->sysMcMq);
  //       
  VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO_HIGH,
            "%s De-Initializing the WDI MC Message queue",__func__);
  vos_mq_deinit(&pSchedContext->wdiMcMq);

  //     
  VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO_HIGH,
            "%s De-Initializing the TL Tx Message queue",__func__);
  vos_mq_deinit(&pSchedContext->tlTxMq);
  //      
  VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO_HIGH,
            "%s: DeInitializing the WDI Tx Message queue",__func__);
  vos_mq_deinit(&pSchedContext->wdiTxMq);


  //      
  VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO_HIGH,
            "%s: DeInitializing the WDI Rx Message queue",__func__);
  vos_mq_deinit(&pSchedContext->wdiRxMq);

  //      
  VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO_HIGH,
            "%s: DeInitializing the SYS Tx Message queue",__func__);
  vos_mq_deinit(&pSchedContext->sysTxMq);

  //      
  VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO_HIGH,
            "%s: DeInitializing the SYS Rx Message queue",__func__);
  vos_mq_deinit(&pSchedContext->sysRxMq);

} /*                        */

/*                                                                         
                                                       
                                                                          */
void vos_sched_flush_mc_mqs ( pVosSchedContext pSchedContext )
{
  pVosMsgWrapper pMsgWrapper = NULL;
  pVosContextType vosCtx;

  /*
                                                                       
                                                                           
                 
  */
  VOS_TRACE( VOS_MODULE_ID_VOSS,
             VOS_TRACE_LEVEL_INFO,
             ("Flushing the MC Thread message queue\n") );

  if (NULL == pSchedContext)
  {
     VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
         "%s: pSchedContext is NULL", __func__);
     return;
  }

  vosCtx = (pVosContextType)(pSchedContext->pVContext);
  if (NULL == vosCtx)
  {
     VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                "%s: vosCtx is NULL", __func__);
     return;
  }

  /*                  */
  while( NULL != (pMsgWrapper = vos_mq_get(&pSchedContext->sysMcMq) ))
  {
    VOS_TRACE( VOS_MODULE_ID_VOSS,
               VOS_TRACE_LEVEL_ERROR,
               "%s: Freeing MC SYS message type %d ",__func__,
               pMsgWrapper->pVosMsg->type );
    sysMcFreeMsg(pSchedContext->pVContext, pMsgWrapper->pVosMsg);
    vos_core_return_msg(pSchedContext->pVContext, pMsgWrapper);
  }
  /*                  */
  while( NULL != (pMsgWrapper = vos_mq_get(&pSchedContext->wdaMcMq) ))
  {
    if(pMsgWrapper->pVosMsg != NULL) 
    {
        VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                   "%s: Freeing MC WDA MSG message type %d",
                   __func__, pMsgWrapper->pVosMsg->type );
        if (pMsgWrapper->pVosMsg->bodyptr) {
            vos_mem_free((v_VOID_t*)pMsgWrapper->pVosMsg->bodyptr);
        }

        pMsgWrapper->pVosMsg->bodyptr = NULL;
        pMsgWrapper->pVosMsg->bodyval = 0;
        pMsgWrapper->pVosMsg->type = 0;
    }
    vos_core_return_msg(pSchedContext->pVContext, pMsgWrapper);
  }

  /*                  */
  while( NULL != (pMsgWrapper = vos_mq_get(&pSchedContext->wdiMcMq) ))
  {
    if(pMsgWrapper->pVosMsg != NULL)
    {
        VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                   "%s: Freeing MC WDI MSG message type %d",
                   __func__, pMsgWrapper->pVosMsg->type );

        /*                             
                            
                                               
                                    */
        if ((pMsgWrapper->pVosMsg->bodyptr) && (!pMsgWrapper->pVosMsg->type))
        {
            vos_mem_free((v_VOID_t*)pMsgWrapper->pVosMsg->bodyptr);
        }
        /*                             
                                
                                           
                                       */
        else if ((pMsgWrapper->pVosMsg->bodyptr) && pMsgWrapper->pVosMsg->type)
        {
            VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                       "%s: SMD NOTIFY MSG, do not free body",
                       __func__);
        }
        pMsgWrapper->pVosMsg->bodyptr = NULL;
        pMsgWrapper->pVosMsg->bodyval = 0;
        pMsgWrapper->pVosMsg->type = 0;
    }
    vos_core_return_msg(pSchedContext->pVContext, pMsgWrapper);
  }

  /*                 */
  while( NULL != (pMsgWrapper = vos_mq_get(&pSchedContext->peMcMq) ))
  {
    VOS_TRACE( VOS_MODULE_ID_VOSS,
               VOS_TRACE_LEVEL_ERROR,
               "%s: Freeing MC PE MSG message type %d",__func__,
               pMsgWrapper->pVosMsg->type );
    peFreeMsg(vosCtx->pMACContext, (tSirMsgQ*)pMsgWrapper->pVosMsg);
    vos_core_return_msg(pSchedContext->pVContext, pMsgWrapper);
  }
  /*                  */
  while( NULL != (pMsgWrapper = vos_mq_get(&pSchedContext->smeMcMq) ))
  {
    VOS_TRACE( VOS_MODULE_ID_VOSS,
               VOS_TRACE_LEVEL_ERROR,
               "%s: Freeing MC SME MSG message type %d", __func__,
               pMsgWrapper->pVosMsg->type );
    sme_FreeMsg(vosCtx->pMACContext, pMsgWrapper->pVosMsg);
    vos_core_return_msg(pSchedContext->pVContext, pMsgWrapper);
  }
    /*                 */
  while( NULL != (pMsgWrapper = vos_mq_get(&pSchedContext->tlMcMq) ))
  {
    VOS_TRACE( VOS_MODULE_ID_VOSS,
               VOS_TRACE_LEVEL_ERROR,
               "%s: Freeing MC TL message type %d",__func__,
               pMsgWrapper->pVosMsg->type );
    WLANTL_McFreeMsg(pSchedContext->pVContext, pMsgWrapper->pVosMsg);
    vos_core_return_msg(pSchedContext->pVContext, pMsgWrapper);
  }
} /*                          */

/*                                                                         
                                                       
                                                                         */
void vos_sched_flush_tx_mqs ( pVosSchedContext pSchedContext )
{
  pVosMsgWrapper pMsgWrapper = NULL;
  /*
                                                                       
                                                                        
                   
  */
  VOS_TRACE( VOS_MODULE_ID_VOSS,
             VOS_TRACE_LEVEL_INFO,
             "%s: Flushing the TX Thread message queue",__func__);

  if (NULL == pSchedContext)
  {
     VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
         "%s: pSchedContext is NULL", __func__);
     return;
  }

  /*                  */
  while( NULL != (pMsgWrapper = vos_mq_get(&pSchedContext->sysTxMq) ))
  {
    VOS_TRACE( VOS_MODULE_ID_VOSS,
               VOS_TRACE_LEVEL_INFO,
               "%s: Freeing TX SYS message type %d",__func__,
               pMsgWrapper->pVosMsg->type );
    sysTxFreeMsg(pSchedContext->pVContext, pMsgWrapper->pVosMsg);
    vos_core_return_msg(pSchedContext->pVContext, pMsgWrapper);
  }
  /*                 */
  while( NULL != (pMsgWrapper = vos_mq_get(&pSchedContext->tlTxMq) ))
  {
    VOS_TRACE( VOS_MODULE_ID_VOSS,
               VOS_TRACE_LEVEL_INFO,
               "%s: Freeing TX TL MSG message type %d",__func__,
               pMsgWrapper->pVosMsg->type );
    WLANTL_TxFreeMsg(pSchedContext->pVContext, pMsgWrapper->pVosMsg);
    vos_core_return_msg(pSchedContext->pVContext, pMsgWrapper);
  }
  /*                  */
  while( NULL != (pMsgWrapper = vos_mq_get(&pSchedContext->wdiTxMq) ))
  {
    VOS_TRACE( VOS_MODULE_ID_VOSS,
               VOS_TRACE_LEVEL_INFO,
               "%s: Freeing TX WDI MSG message type %d",__func__,
               pMsgWrapper->pVosMsg->type );
    sysTxFreeMsg(pSchedContext->pVContext, pMsgWrapper->pVosMsg);
    vos_core_return_msg(pSchedContext->pVContext, pMsgWrapper);
  }
} /*                          */
/*                                                                         
                                                       
                                                                         */
void vos_sched_flush_rx_mqs ( pVosSchedContext pSchedContext )
{
  pVosMsgWrapper pMsgWrapper = NULL;
  /*
                                                                       
                                                                        
                   
  */
  VOS_TRACE( VOS_MODULE_ID_VOSS,
             VOS_TRACE_LEVEL_INFO,
             "%s: Flushing the RX Thread message queue",__func__);

  if (NULL == pSchedContext)
  {
     VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
         "%s: pSchedContext is NULL", __func__);
     return;
  }

  while( NULL != (pMsgWrapper = vos_mq_get(&pSchedContext->wdiRxMq) ))
  {
    VOS_TRACE( VOS_MODULE_ID_VOSS,
               VOS_TRACE_LEVEL_INFO,
               "%s: Freeing RX WDI MSG message type %d",__func__,
               pMsgWrapper->pVosMsg->type );
    sysTxFreeMsg(pSchedContext->pVContext, pMsgWrapper->pVosMsg);
  }

  while( NULL != (pMsgWrapper = vos_mq_get(&pSchedContext->sysRxMq) ))
  {
    VOS_TRACE( VOS_MODULE_ID_VOSS,
               VOS_TRACE_LEVEL_INFO,
               "%s: Freeing RX SYS MSG message type %d",__func__,
               pMsgWrapper->pVosMsg->type );
    sysTxFreeMsg(pSchedContext->pVContext, pMsgWrapper->pVosMsg);
  }

}/*                          */

/*                                                                         
                                                                  
                                                                         */
int vos_sched_is_tx_thread(int threadID)
{
   //                                                          
   VOS_ASSERT( NULL != gpVosSchedContext);
   if (gpVosSchedContext == NULL)
   {
      VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
          "%s: gpVosSchedContext == NULL",__func__);
      return 0;
   }
   return ((gpVosSchedContext->TxThread) && (threadID == gpVosSchedContext->TxThread->pid));
}
/*                                                                         
                                                                  
                                                                         */
int vos_sched_is_rx_thread(int threadID)
{
   //                                                          
   VOS_ASSERT( NULL != gpVosSchedContext);
   if (gpVosSchedContext == NULL)
   {
      VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
          "%s: gpVosSchedContext == NULL",__func__);
      return 0;
   }
   return ((gpVosSchedContext->RxThread) && (threadID == gpVosSchedContext->RxThread->pid));
}
/*                                                                         
                                             
                                                                         */
pVosSchedContext get_vos_sched_ctxt(void)
{
   //                                                         
   if (gpVosSchedContext == NULL)
   {
      VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
         "%s: gpVosSchedContext == NULL",__func__);
   }
   return (gpVosSchedContext);
}
/*                                                                         
                                            
                                                                         */
pVosWatchdogContext get_vos_watchdog_ctxt(void)
{
   //                                                         
   if (gpVosWatchdogContext == NULL)
   {
      VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
         "%s: gpVosWatchdogContext == NULL",__func__);
   }
   return (gpVosWatchdogContext);
}
/* 
                                     

                                                             
                                                                
                          

        
             
         
                                                                 
                                                 

*/
VOS_STATUS vos_watchdog_wlan_shutdown(void)
{
    v_CONTEXT_t pVosContext = NULL;
    hdd_context_t *pHddCtx = NULL;

    VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL,
        "%s: WLAN driver is shutting down ", __func__);
    if (NULL == gpVosWatchdogContext)
    {
       VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL,
           "%s: Watchdog not enabled. LOGP ignored.", __func__);
       return VOS_STATUS_E_FAILURE;
    }

    pVosContext = vos_get_global_context(VOS_MODULE_ID_HDD, NULL);
    pHddCtx = (hdd_context_t *)vos_get_context(VOS_MODULE_ID_HDD, pVosContext );
    if (NULL == pHddCtx)
    {
       VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL,
           "%s: Invalid HDD Context", __func__);
       return VOS_STATUS_E_FAILURE;
    }

    /*                    */
    spin_lock(&gpVosWatchdogContext->wdLock);

    /*                                        */
    if (gpVosWatchdogContext->resetInProgress)
    {
        VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL,
            "%s: Shutdown already in Progress. Ignoring signaling Watchdog",
                                                           __func__);
        /*                       */
        spin_unlock(&gpVosWatchdogContext->wdLock);
        return VOS_STATUS_E_FAILURE;
    }
    /*                                                            
                        */
    else if (pHddCtx->isLogpInProgress)
    {
        VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL,
            "%s: shutdown/re-init already in Progress. Ignoring signaling Watchdog",
                                                           __func__);
        /*                       */
        spin_unlock(&gpVosWatchdogContext->wdLock);
        return VOS_STATUS_E_FAILURE;
    } 

    /*                                                                                 */
    vos_set_logp_in_progress(VOS_MODULE_ID_VOSS, TRUE);
    vos_set_reinit_in_progress(VOS_MODULE_ID_VOSS, FALSE);
    pHddCtx->isLogpInProgress = TRUE;

    /*                       */
    spin_unlock(&gpVosWatchdogContext->wdLock);

    if (WLAN_HDD_IS_LOAD_UNLOAD_IN_PROGRESS(pHddCtx))
    {
        VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL,
                "%s: Load/unload in Progress. Ignoring signaling Watchdog",
                __func__);
        /*                                                                     
                                           */
        hdd_set_ssr_required( HDD_SSR_DISABLED );
        return VOS_STATUS_E_FAILURE;
    }
    /*                              */
    pHddCtx->hddRivaResetStats++;
#ifdef CONFIG_HAS_EARLYSUSPEND
    if(VOS_STATUS_SUCCESS != hdd_wlan_reset_initialization())
    {
       VOS_ASSERT(0);
    }
#endif

    set_bit(WD_WLAN_SHUTDOWN_EVENT_MASK, &gpVosWatchdogContext->wdEventFlag);
    set_bit(WD_POST_EVENT_MASK, &gpVosWatchdogContext->wdEventFlag);
    wake_up_interruptible(&gpVosWatchdogContext->wdWaitQueue);

    return VOS_STATUS_SUCCESS;
}

/* 
                                    

                                                                   
                              

        
             
         
                                                                 
                                                 

*/
VOS_STATUS vos_watchdog_wlan_re_init(void)
{
    /*                                                              */
    set_bit(WD_WLAN_REINIT_EVENT_MASK, &gpVosWatchdogContext->wdEventFlag);
    set_bit(WD_POST_EVENT_MASK, &gpVosWatchdogContext->wdEventFlag);
    wake_up_interruptible(&gpVosWatchdogContext->wdWaitQueue);

    return VOS_STATUS_SUCCESS;
}

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

/*                                                                           

                                             
   
   
                                                                        
                                                                 
   
   
                                      
   
   
                                      
                                                                             
                                     

                                                                            */

/*                                                                            
                
                                                                            */
#include <wlan_hdd_dev_pwr.h>
#ifdef ANI_BUS_TYPE_PLATFORM
#include <linux/wcnss_wlan.h>
#else
#include <wcnss_wlan.h>
#endif //                     

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
static const hdd_tmLevelAction_t thermalMigrationAction[WLAN_HDD_TM_LEVEL_MAX] =
{
   /*                                              */
   {1, 0, 0, 0, 0xFFFFF},
   /*                              */
   {0, 0, 0, 0, 0xFFFFF},
   /*                           
                                                                          */
   {0, 0, 100, 300, 16},
   /*                           
                                                                          */
   {0, 0, 500, 500, 11},
   /*                                      */
   {0, 1, 1000, 500, 10}
};
#ifdef HAVE_WCNSS_SUSPEND_RESUME_NOTIFY
static bool suspend_notify_sent;
#endif


/*                                                                            

                                              

                               
                          


               

                                                                            */
static int wlan_suspend(hdd_context_t* pHddCtx)
{
   int rc = 0;

   pVosSchedContext vosSchedContext = NULL;

   /*                             */
   vosSchedContext = get_vos_sched_ctxt();

   if(!vosSchedContext) {
      VOS_TRACE(VOS_MODULE_ID_HDD,VOS_TRACE_LEVEL_FATAL,"%s: Global VOS_SCHED context is Null",__func__);
      return 0;
   }
   if(!vos_is_apps_power_collapse_allowed(pHddCtx))
   {
       /*                   */
       VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR, "%s: Fail wlan suspend: not in IMPS/BMPS", __func__);
       return -EPERM;
   }

   /*
                                                                                                   
   */
   VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO, "%s: Suspending Mc, Rx and Tx Threads",__func__);

   INIT_COMPLETION(pHddCtx->tx_sus_event_var);

   /*                               */
   set_bit(TX_SUSPEND_EVENT_MASK, &vosSchedContext->txEventFlag);

   wake_up_interruptible(&vosSchedContext->txWaitQueue);

   /*                                              */
   rc = wait_for_completion_interruptible_timeout(&pHddCtx->tx_sus_event_var, msecs_to_jiffies(200));

   if(!rc)
   {
      VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_FATAL, "%s: Not able to suspend TX thread timeout happened", __func__);
      clear_bit(TX_SUSPEND_EVENT_MASK, &vosSchedContext->txEventFlag);

      return -ETIME;
   }
   /*                                */
   pHddCtx->isTxThreadSuspended = TRUE;

   INIT_COMPLETION(pHddCtx->rx_sus_event_var);

   /*                               */
   set_bit(RX_SUSPEND_EVENT_MASK, &vosSchedContext->rxEventFlag);

   wake_up_interruptible(&vosSchedContext->rxWaitQueue);

   /*                                              */
   rc = wait_for_completion_interruptible_timeout(&pHddCtx->rx_sus_event_var, msecs_to_jiffies(200));

   if(!rc)
   {
       VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_FATAL, "%s: Not able to suspend Rx thread timeout happened", __func__);

       clear_bit(RX_SUSPEND_EVENT_MASK, &vosSchedContext->rxEventFlag);

       /*                              */
       complete(&vosSchedContext->ResumeTxEvent);

       /*                              */
       pHddCtx->isTxThreadSuspended = FALSE;

       return -ETIME;
   }

   /*                                */
   pHddCtx->isRxThreadSuspended = TRUE;

   INIT_COMPLETION(pHddCtx->mc_sus_event_var);

   /*                               */
   set_bit(MC_SUSPEND_EVENT_MASK, &vosSchedContext->mcEventFlag);

   wake_up_interruptible(&vosSchedContext->mcWaitQueue);

   /*                                              */
   rc = wait_for_completion_interruptible_timeout(&pHddCtx->mc_sus_event_var, msecs_to_jiffies(200));

   if(!rc)
   {
       VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_FATAL, "%s: Not able to suspend MC thread timeout happened", __func__);

       clear_bit(MC_SUSPEND_EVENT_MASK, &vosSchedContext->mcEventFlag);

       /*                              */
       complete(&vosSchedContext->ResumeRxEvent);

       /*                              */
       pHddCtx->isRxThreadSuspended = FALSE;

       /*                              */
       complete(&vosSchedContext->ResumeTxEvent);

       /*                              */
       pHddCtx->isTxThreadSuspended = FALSE;

       return -ETIME;
   }

   /*                                */
   pHddCtx->isMcThreadSuspended = TRUE;
   
   /*                                    */
   pHddCtx->isWlanSuspended = TRUE;

   return 0;
}

/*                                                                            

                                             

                               
                          


               

                                                                            */
static void wlan_resume(hdd_context_t* pHddCtx)
{
   pVosSchedContext vosSchedContext = NULL;

   //                            
   vosSchedContext = get_vos_sched_ctxt();

   if(!vosSchedContext) {
      VOS_TRACE(VOS_MODULE_ID_HDD,VOS_TRACE_LEVEL_FATAL,"%s: Global VOS_SCHED context is Null",__func__);
      return;
   }

   /*
                                                                  
   */
   VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO, "%s: Resuming Mc, Rx and Tx Thread",__func__);

   /*                              */
   complete(&vosSchedContext->ResumeMcEvent);

   /*                              */
   pHddCtx->isMcThreadSuspended = FALSE;

   /*                              */
   complete(&vosSchedContext->ResumeRxEvent);

   /*                              */
   pHddCtx->isRxThreadSuspended = FALSE;

   /*                              */
   complete(&vosSchedContext->ResumeTxEvent);

   /*                              */
   pHddCtx->isTxThreadSuspended = FALSE;

   /*                                    */
   pHddCtx->isWlanSuspended = FALSE;
}

/*                                                                            

                                              
                                                                             

                                     


               

                                                                            */
int hddDevSuspendHdlr(struct device *dev)
{
   int ret = 0;
   hdd_context_t* pHddCtx = NULL;

   pHddCtx =  (hdd_context_t*)wcnss_wlan_get_drvdata(dev);

   VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO, "%s: WLAN suspended by platform driver",__func__);

   /*                     */
   if(!pHddCtx) {
      VOS_TRACE(VOS_MODULE_ID_HDD,VOS_TRACE_LEVEL_FATAL,"%s: HDD context is Null",__func__);
      return 0;
   }

   if(pHddCtx->isWlanSuspended == TRUE)
   {
      VOS_TRACE(VOS_MODULE_ID_HDD,VOS_TRACE_LEVEL_FATAL,"%s: WLAN is already in suspended state",__func__);
      return 0;
   }

   /*                         */
   ret = wlan_suspend(pHddCtx);
   if(ret != 0)
   {
      VOS_TRACE(VOS_MODULE_ID_HDD,VOS_TRACE_LEVEL_FATAL,"%s: Not able to suspend wlan",__func__);
      return ret;
   }

#ifdef HAVE_WCNSS_SUSPEND_RESUME_NOTIFY
   if(hdd_is_suspend_notify_allowed(pHddCtx))
   {
      wcnss_suspend_notify();
      suspend_notify_sent = true;
   }
#endif
   return 0;
}

/*                                                                            

                                             
                                                                            

                                     


               

                                                                            */
int hddDevResumeHdlr(struct device *dev)
{
   hdd_context_t* pHddCtx = NULL;

   pHddCtx =  (hdd_context_t*)wcnss_wlan_get_drvdata(dev);

   VOS_TRACE(VOS_MODULE_ID_HDD,VOS_TRACE_LEVEL_INFO, "%s: WLAN being resumed by Android OS",__func__);

   if(pHddCtx->isWlanSuspended != TRUE)
   {
      VOS_TRACE(VOS_MODULE_ID_HDD,VOS_TRACE_LEVEL_FATAL,"%s: WLAN is already in resumed state",__func__);
      return 0;
   }

   /*                        */
   wlan_resume(pHddCtx);
#ifdef HAVE_WCNSS_SUSPEND_RESUME_NOTIFY
   if(suspend_notify_sent == true)
   {
      wcnss_resume_notify();
      suspend_notify_sent = false;
   }
#endif

   return 0;
}

static const struct dev_pm_ops pm_ops = {
   .suspend = hddDevSuspendHdlr,
   .resume = hddDevResumeHdlr,
};

/*                                                                            
  

                                
                                                                          

                               
                          

                              
                                                     
                                                  

                                                                            */
VOS_STATUS hddRegisterPmOps(hdd_context_t *pHddCtx)
{
    wcnss_wlan_set_drvdata(pHddCtx->parent_dev, pHddCtx);
#ifndef FEATURE_R33D
    wcnss_wlan_register_pm_ops(pHddCtx->parent_dev, &pm_ops);
#endif /*              */
    return VOS_STATUS_SUCCESS;
}

/*                                                                            

                                   
                                                                              

                               
                          

                              
                                                        
                                                     

                                                                            */
VOS_STATUS hddDeregisterPmOps(hdd_context_t *pHddCtx)
{
#ifndef FEATURE_R33D
    wcnss_wlan_unregister_pm_ops(pHddCtx->parent_dev, &pm_ops);
#endif /*              */
    return VOS_STATUS_SUCCESS;
}

/*                                                                            

                                        
                                             

                               
                          

               

                                                                            */
void hddDevTmTxBlockTimeoutHandler(void *usrData)
{
   hdd_context_t        *pHddCtx = (hdd_context_t *)usrData;
   hdd_adapter_t        *staAdapater;
   /*                                */
   if(NULL == pHddCtx)
   {
      VOS_TRACE(VOS_MODULE_ID_HDD,VOS_TRACE_LEVEL_ERROR,
                "%s: NULL Context", __func__);
      VOS_ASSERT(0);
      return;
   }

   staAdapater = hdd_get_adapter(pHddCtx, WLAN_HDD_INFRA_STATION);

   if(NULL == staAdapater)
   {
      VOS_TRACE(VOS_MODULE_ID_HDD,VOS_TRACE_LEVEL_ERROR,
                "%s: NULL Adapter", __func__);
      VOS_ASSERT(0);
      return;
   }

   if(mutex_lock_interruptible(&pHddCtx->tmInfo.tmOperationLock))
   {
      VOS_TRACE(VOS_MODULE_ID_HDD,VOS_TRACE_LEVEL_ERROR,
                "%s: Acquire lock fail", __func__);
      return;
   }
   pHddCtx->tmInfo.txFrameCount = 0;

   /*                */
    
   netif_tx_wake_all_queues(staAdapater->dev);
   pHddCtx->tmInfo.qBlocked = VOS_FALSE;
   mutex_unlock(&pHddCtx->tmInfo.tmOperationLock);

   return;
}

/*                                                                            

                                 
                                                

                              
                                               

           

                                                                            */
void hddDevTmLevelChangedHandler(struct device *dev, int changedTmLevel)
{
   hdd_context_t        *pHddCtx = NULL;
   WLAN_TmLevelEnumType  newTmLevel = changedTmLevel;
   hdd_adapter_t        *staAdapater;

   pHddCtx =  (hdd_context_t*)wcnss_wlan_get_drvdata(dev);

   if ((pHddCtx->tmInfo.currentTmLevel == newTmLevel) ||
       (!pHddCtx->cfg_ini->thermalMitigationEnable))
   {
      VOS_TRACE(VOS_MODULE_ID_HDD,VOS_TRACE_LEVEL_WARN,
                "%s: TM Not enabled %d or Level does not changed %d",
                __func__, pHddCtx->cfg_ini->thermalMitigationEnable, newTmLevel);
      /*                           
                                    
                    */
      return;
   }

   /*                             
                                                    */
   if (~VOS_STA & pHddCtx->concurrency_mode)
   {
      VOS_TRACE(VOS_MODULE_ID_HDD,VOS_TRACE_LEVEL_ERROR,
                "%s: CMODE 0x%x, TM disable",
                __func__, pHddCtx->concurrency_mode);
      newTmLevel = WLAN_HDD_TM_LEVEL_0;
   }

   if ((newTmLevel < WLAN_HDD_TM_LEVEL_0) ||
       (newTmLevel >= WLAN_HDD_TM_LEVEL_MAX))
   {
      VOS_TRACE(VOS_MODULE_ID_HDD,VOS_TRACE_LEVEL_ERROR,
                "%s: TM level %d out of range",
                __func__, newTmLevel);
      return;
   }

   if (newTmLevel != WLAN_HDD_TM_LEVEL_4)
      sme_SetTmLevel(pHddCtx->hHal, newTmLevel, 0);

   if (mutex_lock_interruptible(&pHddCtx->tmInfo.tmOperationLock))
   {
      VOS_TRACE(VOS_MODULE_ID_HDD,VOS_TRACE_LEVEL_ERROR,
                "%s: Acquire lock fail", __func__);
      return;
   }

   pHddCtx->tmInfo.currentTmLevel = newTmLevel;
   pHddCtx->tmInfo.txFrameCount = 0;
   vos_mem_copy(&pHddCtx->tmInfo.tmAction,
                &thermalMigrationAction[newTmLevel],
                sizeof(hdd_tmLevelAction_t));


   if (pHddCtx->tmInfo.tmAction.enterImps)
   {
      staAdapater = hdd_get_adapter(pHddCtx, WLAN_HDD_INFRA_STATION);
      if (staAdapater)
      {
         if (hdd_connIsConnected(WLAN_HDD_GET_STATION_CTX_PTR(staAdapater)))
         {
            sme_RoamDisconnect(pHddCtx->hHal,
                               staAdapater->sessionId, 
                               eCSR_DISCONNECT_REASON_UNSPECIFIED);
         }
      }
   }

   mutex_unlock(&pHddCtx->tmInfo.tmOperationLock);

   return;
}

/*                                                                            

                           
                                                                          

                               
                          

                              
                                                     
                                                  

                                                                            */
VOS_STATUS hddDevTmRegisterNotifyCallback(hdd_context_t *pHddCtx)
{
   VOS_TRACE(VOS_MODULE_ID_HDD,VOS_TRACE_LEVEL_INFO,
             "%s: Register TM Handler", __func__);

   wcnss_register_thermal_mitigation(pHddCtx->parent_dev ,hddDevTmLevelChangedHandler);

   /*                                            */
   pHddCtx->tmInfo.currentTmLevel = WLAN_HDD_TM_LEVEL_0;
   vos_mem_zero(&pHddCtx->tmInfo.tmAction, sizeof(hdd_tmLevelAction_t)); 
   vos_timer_init(&pHddCtx->tmInfo.txSleepTimer,
                  VOS_TIMER_TYPE_SW,
                  hddDevTmTxBlockTimeoutHandler,
                  (void *)pHddCtx);
   mutex_init(&pHddCtx->tmInfo.tmOperationLock);
   pHddCtx->tmInfo.txFrameCount = 0;
   pHddCtx->tmInfo.blockedQueue = NULL;
   pHddCtx->tmInfo.qBlocked     = VOS_FALSE;
   return VOS_STATUS_SUCCESS;
}

/*                                                                            

                              
                                                                             

                               
                          

                              
                                                        
                                                     

                                                                            */
VOS_STATUS hddDevTmUnregisterNotifyCallback(hdd_context_t *pHddCtx)
{
   VOS_STATUS vosStatus = VOS_STATUS_SUCCESS;

   wcnss_unregister_thermal_mitigation(hddDevTmLevelChangedHandler);

   if(VOS_TIMER_STATE_RUNNING ==
           vos_timer_getCurrentState(&pHddCtx->tmInfo.txSleepTimer))
   {
       vosStatus = vos_timer_stop(&pHddCtx->tmInfo.txSleepTimer);
       if(!VOS_IS_STATUS_SUCCESS(vosStatus))
       {
           VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                                "%s: Timer stop fail", __func__);
       }
   }

   //                         
   vosStatus = vos_timer_destroy(&pHddCtx->tmInfo.txSleepTimer);
   if (!VOS_IS_STATUS_SUCCESS(vosStatus))
   {
       VOS_TRACE(VOS_MODULE_ID_HDD,VOS_TRACE_LEVEL_ERROR,
                            "%s: Fail to destroy timer", __func__);
   }

   return VOS_STATUS_SUCCESS;
}


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

/*============================================================================
  @file wlan_hdd_wmm.c

  This module (wlan_hdd_wmm.h interface + wlan_hdd_wmm.c implementation)
  houses all the logic for WMM in HDD.

  On the control path, it has the logic to setup QoS, modify QoS and delete
  QoS (QoS here refers to a TSPEC). The setup QoS comes in two flavors: an
  explicit application invoked and an internal HDD invoked.  The implicit QoS
  is for applications that do NOT call the custom QCT WLAN OIDs for QoS but
  which DO mark their traffic for priortization. It also has logic to start,
  update and stop the U-APSD trigger frame generation. It also has logic to
  read WMM related config parameters from the registry.

  On the data path, it has the logic to figure out the WMM AC of an egress
  packet and when to signal TL to serve a particular AC queue. It also has the
  logic to retrieve a packet based on WMM priority in response to a fetch from
  TL.

  The remaining functions are utility functions for information hiding.


               Copyright (c) 2008-9 QUALCOMM Incorporated.
               All Rights Reserved.
               Qualcomm Confidential and Proprietary
============================================================================*/

/*                                                                           
               
                                                                           */
#include <wlan_hdd_tx_rx.h>
#include <wlan_hdd_dp_utils.h>
#include <wlan_hdd_wmm.h>
#include <wlan_hdd_ether.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/etherdevice.h>
#include <linux/if_vlan.h>
#include <linux/ip.h>
#include <linux/semaphore.h>
#include <wlan_hdd_hostapd.h>
#include <wlan_hdd_softap_tx_rx.h>
#include <vos_sched.h>

//                                              
#ifdef HDD_WMM_DEBUG
#define WMM_TRACE_LEVEL_FATAL      VOS_TRACE_LEVEL_FATAL
#define WMM_TRACE_LEVEL_ERROR      VOS_TRACE_LEVEL_FATAL
#define WMM_TRACE_LEVEL_WARN       VOS_TRACE_LEVEL_FATAL
#define WMM_TRACE_LEVEL_INFO       VOS_TRACE_LEVEL_FATAL
#define WMM_TRACE_LEVEL_INFO_HIGH  VOS_TRACE_LEVEL_FATAL
#define WMM_TRACE_LEVEL_INFO_LOW   VOS_TRACE_LEVEL_FATAL
#else
#define WMM_TRACE_LEVEL_FATAL      VOS_TRACE_LEVEL_FATAL
#define WMM_TRACE_LEVEL_ERROR      VOS_TRACE_LEVEL_ERROR
#define WMM_TRACE_LEVEL_WARN       VOS_TRACE_LEVEL_WARN
#define WMM_TRACE_LEVEL_INFO       VOS_TRACE_LEVEL_INFO
#define WMM_TRACE_LEVEL_INFO_HIGH  VOS_TRACE_LEVEL_INFO_HIGH
#define WMM_TRACE_LEVEL_INFO_LOW   VOS_TRACE_LEVEL_INFO_LOW
#endif


#define WLAN_HDD_MAX_DSCP 0x3f

//                 
#define DHCP_SOURCE_PORT 0x4400
#define DHCP_DESTINATION_PORT 0x4300

static sme_QosWmmUpType hddWmmDscpToUpMap[WLAN_HDD_MAX_DSCP+1];

const v_U8_t hddWmmUpToAcMap[] = {
   WLANTL_AC_BE,
   WLANTL_AC_BK,
   WLANTL_AC_BK,
   WLANTL_AC_BE,
   WLANTL_AC_VI,
   WLANTL_AC_VI,
   WLANTL_AC_VO,
   WLANTL_AC_VO
};

//                            
const v_U8_t hddLinuxUpToAcMap[8] = {
   HDD_LINUX_AC_BE,
   HDD_LINUX_AC_BK,
   HDD_LINUX_AC_BK,
   HDD_LINUX_AC_BE,
   HDD_LINUX_AC_VI,
   HDD_LINUX_AC_VI,
   HDD_LINUX_AC_VO,
   HDD_LINUX_AC_VO
};

#ifndef WLAN_MDM_CODE_REDUCTION_OPT
/* 
                                                                       
                                      

                                                                      

         
      
*/
static void hdd_wmm_enable_tl_uapsd (hdd_wmm_qos_context_t* pQosContext)
{
   hdd_adapter_t* pAdapter = pQosContext->pAdapter;
   WLANTL_ACEnumType acType = pQosContext->acType;
   hdd_wmm_ac_status_t *pAc = &pAdapter->hddWmmStatus.wmmAcStatus[acType];
   VOS_STATUS status;
   v_U32_t service_interval;
   v_U32_t suspension_interval;
   sme_QosWmmDirType direction;
   v_BOOL_t psb;


   //                        
   if (pAc->wmmAcTspecValid == VOS_FALSE)
   {
      VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_ERROR,
                "%s: Invoked with invalid TSPEC",
                __func__);
      return;
   }

   //                               
   if (pAc->wmmAcTspecInfo.min_service_interval)
   {
      service_interval = pAc->wmmAcTspecInfo.min_service_interval;
   }
   else if (pAc->wmmAcTspecInfo.max_service_interval)
   {
      service_interval = pAc->wmmAcTspecInfo.max_service_interval;
   }
   else
   {
      //                                            
      //                                       
      VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO,
                "%s: No service interval supplied",
                __func__);
      service_interval = 0;
   }

   //                                              
   suspension_interval = pAc->wmmAcTspecInfo.suspension_interval;
   direction = pAc->wmmAcTspecInfo.ts_info.direction;
   psb = pAc->wmmAcTspecInfo.ts_info.psb;

   //                                                               
   if ((pAc->wmmAcUapsdInfoValid) &&
       (pAc->wmmAcUapsdServiceInterval == service_interval) &&
       (pAc->wmmAcUapsdSuspensionInterval == suspension_interval) &&
       (pAc->wmmAcUapsdDirection == direction) &&
       (pAc->wmmAcIsUapsdEnabled == psb))
   {
      VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO,
                "%s: No change in U-APSD parameters",
                __func__);
      return;
   }

   //                                            
   if (!sme_IsPowerSaveEnabled(WLAN_HDD_GET_HAL_CTX(pAdapter), ePMC_BEACON_MODE_POWER_SAVE))
   {
      VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO,
                "%s: BMPS is not enabled",
                __func__);
      return;
   }

   if (!sme_IsPowerSaveEnabled(WLAN_HDD_GET_HAL_CTX(pAdapter), ePMC_UAPSD_MODE_POWER_SAVE))
   {
      VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO,
                "%s: U-APSD is not enabled",
                __func__);
      return;
   }

   //                                    
   status = WLANTL_EnableUAPSDForAC((WLAN_HDD_GET_CTX(pAdapter))->pvosContext,
                                    (WLAN_HDD_GET_STATION_CTX_PTR(pAdapter))->conn_info.staId[0],
                                    acType,
                                    pAc->wmmAcTspecInfo.ts_info.tid,
                                    pAc->wmmAcTspecInfo.ts_info.up,
                                    service_interval,
                                    suspension_interval,
                                    direction);

   if ( !VOS_IS_STATUS_SUCCESS( status ) )
   {
      VOS_TRACE( VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_ERROR,
                 "%s: Failed to enable U-APSD for AC=%d",
                 __func__, acType );
      return;
   }

   //                                         
   pAc->wmmAcUapsdInfoValid = VOS_TRUE;
   pAc->wmmAcUapsdServiceInterval = service_interval;
   pAc->wmmAcUapsdSuspensionInterval = suspension_interval;
   pAc->wmmAcUapsdDirection = direction;
   pAc->wmmAcIsUapsdEnabled = psb;

   VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO,
             "%s: Enabled UAPSD in TL srv_int=%ld "
             "susp_int=%ld dir=%d AC=%d",
             __func__,
             service_interval,
             suspension_interval,
             direction,
             acType);

}

/* 
                                                                    
                                   

                                                                      

         
      
*/
static void hdd_wmm_disable_tl_uapsd (hdd_wmm_qos_context_t* pQosContext)
{
   hdd_adapter_t* pAdapter = pQosContext->pAdapter;
   WLANTL_ACEnumType acType = pQosContext->acType;
   hdd_wmm_ac_status_t *pAc = &pAdapter->hddWmmStatus.wmmAcStatus[acType];
   VOS_STATUS status;


   //                                  
   if (pAc->wmmAcUapsdInfoValid == VOS_TRUE)
   {
      status = WLANTL_DisableUAPSDForAC((WLAN_HDD_GET_CTX(pAdapter))->pvosContext,
                                        (WLAN_HDD_GET_STATION_CTX_PTR(pAdapter))->conn_info.staId[0],
                                        acType);

      if ( !VOS_IS_STATUS_SUCCESS( status ) )
      {
         VOS_TRACE( VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_ERROR,
                    "%s: Failed to disable U-APSD for AC=%d",
                    __func__, acType );
      }
      else
      {
         //                                  
         pAc->wmmAcUapsdInfoValid = VOS_FALSE;
         VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO,
                   "%s: Disabled UAPSD in TL for AC=%d",
                   __func__,
                   acType);
      }
   }
}

#endif

/* 
                                                                    

                                                                      

         
      
*/
static void hdd_wmm_free_context (hdd_wmm_qos_context_t* pQosContext)
{
   hdd_adapter_t* pAdapter;

   VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO_LOW,
             "%s: Entered, context %p",
             __func__, pQosContext);

   if (unlikely((NULL == pQosContext) ||
                (HDD_WMM_CTX_MAGIC != pQosContext->magic)))
   {
      //                                       
      return;
   }

   //                                   
   pAdapter = pQosContext->pAdapter;

   //                                                           
   mutex_lock(&pAdapter->hddWmmStatus.wmmLock);

   //                                                
   pQosContext->magic = 0;

   //                   
   list_del(&pQosContext->node);

   //                           
   mutex_unlock(&pAdapter->hddWmmStatus.wmmLock);

   //               
   kfree(pQosContext);

}

#ifndef WLAN_MDM_CODE_REDUCTION_OPT
/* 
                                                                      
                                                           

                                                                      

         
      
*/
#define MAX_NOTIFY_LEN 50
static void hdd_wmm_notify_app (hdd_wmm_qos_context_t* pQosContext)
{
   hdd_adapter_t* pAdapter;
   union iwreq_data wrqu;
   char buf[MAX_NOTIFY_LEN+1];

   VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO_LOW,
             "%s: Entered, context %p",
             __func__, pQosContext);

   if (unlikely((NULL == pQosContext) ||
                (HDD_WMM_CTX_MAGIC != pQosContext->magic)))
   {
      VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_ERROR,
                "%s: Invalid QoS Context",
                __func__);
      return;
   }


   //                 
   memset(&wrqu, 0, sizeof(wrqu));
   memset(buf, 0, sizeof(buf));

   snprintf(buf, MAX_NOTIFY_LEN, "QCOM: TS change[%u: %u]",
            (unsigned int)pQosContext->handle,
            (unsigned int)pQosContext->lastStatus);

   wrqu.data.pointer = buf;
   wrqu.data.length = strlen(buf);

   //                           
   pAdapter = pQosContext->pAdapter;

   //               
   VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO,
             "%s: Sending [%s]", __func__, buf);
   wireless_send_event(pAdapter->dev, IWEVCUSTOM, &wrqu, buf);
}


/* 
                                                                          
                                                                        
                                                                         

                                                      
                                                        

                                                   
                                                        
      
*/
static v_BOOL_t hdd_wmm_is_access_allowed(hdd_adapter_t* pAdapter,
                                          hdd_wmm_ac_status_t* pAc)
{
   //                                                   
   //                                    
   //                                               
   //                    
   if (!hdd_wmm_is_active(pAdapter) ||
       !(WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->bImplicitQosEnabled ||
       !pAc->wmmAcAccessRequired)
   {
      return VOS_TRUE;
   }

   //                                                            
   //                       
   if (pAc->wmmAcAccessGranted || pAc->wmmAcAccessFailed)
   {
      return VOS_TRUE;
   }

   //                                                        
   //                                                       
   //        
   //                                                    
   if (!pAc->wmmAcTspecValid)
   {
      return VOS_FALSE;
   }

   //                                                                    
   //                                                                
   if (pAc->wmmAcTspecInfo.ts_info.direction == SME_QOS_WMM_TS_DIR_DOWNLINK)
   {
      return VOS_FALSE;
   }

   //                                       
   return VOS_TRUE;
}

#ifdef FEATURE_WLAN_CCX
/* 
                                                                        
                                                                     
                                                                        
                                                                     
                                                             

                                                  

                           
*/
void hdd_wmm_inactivity_timer_cb( v_PVOID_t pUserData )
{
    hdd_wmm_qos_context_t* pQosContext = (hdd_wmm_qos_context_t*)pUserData;
    hdd_adapter_t* pAdapter;
    hdd_wmm_ac_status_t *pAc;
    hdd_wlan_wmm_status_e status;
    VOS_STATUS vos_status;
    v_U32_t currentTrafficCnt = 0;
    WLANTL_ACEnumType acType = pQosContext->acType;

    pAdapter = pQosContext->pAdapter;
    pAc = &pAdapter->hddWmmStatus.wmmAcStatus[acType];

    //                              
    currentTrafficCnt = pAdapter->hdd_stats.hddTxRxStats.txXmitClassifiedAC[pQosContext->acType];

    VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_WARN,
            FL("WMM inactivity Timer for AC=%d, currentCnt=%d, prevCnt=%d\n"),
            acType, (int)currentTrafficCnt, (int)pAc->wmmPrevTrafficCnt);
    if (pAc->wmmPrevTrafficCnt == currentTrafficCnt)
    {
        //                                                              
        status = hdd_wmm_delts(pAdapter, pQosContext->handle);
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_WARN,
                FL("Deleted TS on AC %d, due to inactivity with status = %d!!!"),
                acType, status);
    }
    else
    {
        pAc->wmmPrevTrafficCnt = currentTrafficCnt;
        if (pAc->wmmInactivityTimer.state == VOS_TIMER_STATE_STOPPED)
        {
            //                  
            vos_status = vos_timer_start(&pAc->wmmInactivityTimer, pAc->wmmInactivityTime);
            if (!VOS_IS_STATUS_SUCCESS(vos_status))
            {
                VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                        FL("Restarting inactivity timer failed on AC %d"), acType);
            }
        }
        else
        {
            VOS_ASSERT(vos_timer_getCurrentState(
                        &pAc->wmmInactivityTimer) == VOS_TIMER_STATE_STOPPED);
        }
    }

    return;
}


/* 
                                                                   
                                                                       
                                               

                                                    
                                                                           

                                             
                                           
*/
VOS_STATUS hdd_wmm_enable_inactivity_timer(hdd_wmm_qos_context_t* pQosContext, v_U32_t inactivityTime)
{
    VOS_STATUS vos_status = VOS_STATUS_E_FAILURE;
    hdd_adapter_t* pAdapter = pQosContext->pAdapter;
    WLANTL_ACEnumType acType = pQosContext->acType;
    hdd_wmm_ac_status_t *pAc;

    pAdapter = pQosContext->pAdapter;
    pAc = &pAdapter->hddWmmStatus.wmmAcStatus[acType];


    //                                                                            
    //                                                                
    vos_status = vos_timer_init(
            &pAc->wmmInactivityTimer,
            VOS_TIMER_TYPE_SW,
            hdd_wmm_inactivity_timer_cb,
            (v_PVOID_t)pQosContext );
    if ( !VOS_IS_STATUS_SUCCESS(vos_status))
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                FL("Initializing inactivity timer failed on AC %d"), acType);
        return vos_status;
    }

    //                           
    vos_status = vos_timer_start(
            &pAc->wmmInactivityTimer,
            inactivityTime);
    if ( !VOS_IS_STATUS_SUCCESS(vos_status))
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                FL("Starting inactivity timer failed on AC %d"), acType);
        return vos_status;
    }
    pAc->wmmInactivityTime = inactivityTime;
    //                                                   
    pAc->wmmPrevTrafficCnt = pAdapter->hdd_stats.hddTxRxStats.txXmitClassifiedAC[pQosContext->acType];

    return vos_status;
}

/* 
                                                                    
                                                                      
                  

                                                    

                                             
                                           
*/
VOS_STATUS hdd_wmm_disable_inactivity_timer(hdd_wmm_qos_context_t* pQosContext)
{
    hdd_adapter_t* pAdapter = pQosContext->pAdapter;
    WLANTL_ACEnumType acType = pQosContext->acType;
    hdd_wmm_ac_status_t *pAc  = &pAdapter->hddWmmStatus.wmmAcStatus[acType];
    VOS_STATUS vos_status = VOS_STATUS_E_FAILURE;

    //                                
    pAc->wmmInactivityTime = 0;
    pAc->wmmPrevTrafficCnt = 0;
    vos_timer_stop(&pAc->wmmInactivityTimer);
    vos_status = vos_timer_destroy(&pAc->wmmInactivityTimer);

    return vos_status;
}
#endif //                 

/* 
                                                                                   
                                                                                
                                                                        
                              

                                   
                                               
                                                
                                                    

         
                                                                
*/
static eHalStatus hdd_wmm_sme_callback (tHalHandle hHal,
                                        void * hddCtx,
                                        sme_QosWmmTspecInfo* pCurrentQosInfo,
                                        sme_QosStatusType smeStatus,
                                        v_U32_t qosFlowId)
{
   hdd_wmm_qos_context_t* pQosContext = hddCtx;
   hdd_adapter_t* pAdapter;
   WLANTL_ACEnumType acType;
   hdd_wmm_ac_status_t *pAc;
   VOS_STATUS status;

   VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO_LOW,
             "%s: Entered, context %p",
             __func__, pQosContext);

   if (unlikely((NULL == pQosContext) ||
                (HDD_WMM_CTX_MAGIC != pQosContext->magic)))
   {
      VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_ERROR,
                "%s: Invalid QoS Context",
                __func__);
      return eHAL_STATUS_FAILURE;
   }

   pAdapter = pQosContext->pAdapter;
   acType = pQosContext->acType;
   pAc = &pAdapter->hddWmmStatus.wmmAcStatus[acType];

   VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO_LOW,
             "%s: status %d flowid %d info %p",
             __func__, smeStatus, qosFlowId, pCurrentQosInfo);

   switch (smeStatus)
   {

   case SME_QOS_STATUS_SETUP_SUCCESS_IND:
      VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO_LOW,
                "%s: Setup is complete",
                __func__);

      //                                                                
      //                             
      if (pCurrentQosInfo)
      {
         pAc->wmmAcTspecValid = VOS_TRUE;
         memcpy(&pAc->wmmAcTspecInfo,
                pCurrentQosInfo,
                sizeof(pAc->wmmAcTspecInfo));
      }

      if (HDD_WMM_HANDLE_IMPLICIT == pQosContext->handle)
      {

         VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO,
                   "%s: Implicit Qos, notifying TL for TL AC %d",
                   __func__, acType);

         //                                                                  
         //             
         pAc->wmmAcAccessAllowed = VOS_TRUE;
         pAc->wmmAcAccessGranted = VOS_TRUE;
         pAc->wmmAcAccessPending = VOS_FALSE;

         //                                   
         status = WLANTL_STAPktPending( (WLAN_HDD_GET_CTX(pAdapter))->pvosContext,
                                        (WLAN_HDD_GET_STATION_CTX_PTR(pAdapter))->conn_info.staId[0],
                                        acType );

         if ( !VOS_IS_STATUS_SUCCESS( status ) )
         {
            VOS_TRACE( VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_ERROR,
                       "%s: Failed to signal TL for AC=%d",
                       __func__, acType );
         }
      }
      else
      {
         VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO,
                   "%s: Explicit Qos, notifying userspace",
                   __func__);

         //                                     
         pQosContext->lastStatus = HDD_WLAN_WMM_STATUS_SETUP_SUCCESS;
         hdd_wmm_notify_app(pQosContext);
      }

#ifdef FEATURE_WLAN_CCX
      //                                              
      if (pCurrentQosInfo && pCurrentQosInfo->inactivity_interval) {
         VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO,
                 "%s: Inactivity timer value = %d for AC=%d\n",
                 __func__, pCurrentQosInfo->inactivity_interval, acType);
         hdd_wmm_enable_inactivity_timer(pQosContext, pCurrentQosInfo->inactivity_interval);
      }
#endif //                 

      //                                                
      hdd_wmm_enable_tl_uapsd(pQosContext);

      break;

   case SME_QOS_STATUS_SETUP_SUCCESS_APSD_SET_ALREADY:
      VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO,
                "%s: Setup is complete (U-APSD set previously)",
                __func__);

      if (HDD_WMM_HANDLE_IMPLICIT == pQosContext->handle)
      {

         VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO,
                   "%s: Implicit Qos, notifying TL",
                   __func__);

         //                                                                  
         //             
         pAc->wmmAcAccessAllowed = VOS_TRUE;
         pAc->wmmAcAccessGranted = VOS_TRUE;
         pAc->wmmAcAccessPending = VOS_FALSE;

         //                                   
         status = WLANTL_STAPktPending( (WLAN_HDD_GET_CTX(pAdapter))->pvosContext,
                                        (WLAN_HDD_GET_STATION_CTX_PTR(pAdapter))->conn_info.staId[0],
                                        acType );

         if ( !VOS_IS_STATUS_SUCCESS( status ) )
         {
            VOS_TRACE( VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_ERROR,
                       "%s: Failed to signal TL for AC=%d",
                       __func__, acType );
         }
      }
      else
      {
         VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO,
                   "%s: Explicit Qos, notifying userspace",
                   __func__);

         //                                     
         pQosContext->lastStatus = HDD_WLAN_WMM_STATUS_SETUP_SUCCESS_NO_ACM_UAPSD_EXISTING;
         hdd_wmm_notify_app(pQosContext);
      }

      break;

   case SME_QOS_STATUS_SETUP_FAILURE_RSP:
      VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_ERROR,
                "%s: Setup failed",
                __func__);
      //                 

      if (HDD_WMM_HANDLE_IMPLICIT == pQosContext->handle)
      {

         VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO,
                   "%s: Implicit Qos, notifying TL",
                   __func__);

         //                                                                
         //                                                                    
         pAc->wmmAcAccessPending = VOS_FALSE;
         pAc->wmmAcAccessFailed = VOS_TRUE;
         pAc->wmmAcAccessAllowed = VOS_TRUE;

         //                                                                  
         status = WLANTL_STAPktPending( (WLAN_HDD_GET_CTX(pAdapter))->pvosContext,
                                        (WLAN_HDD_GET_STATION_CTX_PTR(pAdapter))->conn_info.staId[0],
                                        acType );

         if ( !VOS_IS_STATUS_SUCCESS( status ) )
         {
            VOS_TRACE( VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_ERROR,
                       "%s: Failed to signal TL for AC=%d",
                       __func__, acType );
         }
      }
      else
      {
         VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO,
                   "%s: Explicit Qos, notifying userspace",
                   __func__);

         //                                     
         pQosContext->lastStatus = HDD_WLAN_WMM_STATUS_SETUP_FAILED;
         hdd_wmm_notify_app(pQosContext);
      }

      /*                                                    
                                                                                       
                                                                                    
                                                                                       
                                                          
       */
      hdd_wmm_free_context(pQosContext);
      break;

   case SME_QOS_STATUS_SETUP_INVALID_PARAMS_RSP:
      VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_ERROR,
                "%s: Setup Invalid Params, notify TL",
                __func__);
      //                 

      if (HDD_WMM_HANDLE_IMPLICIT == pQosContext->handle)
      {

         VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO,
                   "%s: Implicit Qos, notifying TL",
                   __func__);

         //                                                                
         //                                                                    
         pAc->wmmAcAccessPending = VOS_FALSE;
         pAc->wmmAcAccessFailed = VOS_TRUE;
         pAc->wmmAcAccessAllowed = VOS_TRUE;

         //                                                                  
         status = WLANTL_STAPktPending( (WLAN_HDD_GET_CTX(pAdapter))->pvosContext,
                                        (WLAN_HDD_GET_STATION_CTX_PTR(pAdapter))->conn_info.staId[0],
                                        acType );

         if ( !VOS_IS_STATUS_SUCCESS( status ) )
         {
            VOS_TRACE( VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_ERROR,
                       "%s: Failed to signal TL for AC=%d",
                       __func__, acType );
         }
      }
      else
      {
         VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO,
                   "%s: Explicit Qos, notifying userspace",
                   __func__);

         //                                     
         pQosContext->lastStatus = HDD_WLAN_WMM_STATUS_SETUP_FAILED_BAD_PARAM;
         hdd_wmm_notify_app(pQosContext);
      }
      break;

   case SME_QOS_STATUS_SETUP_NOT_QOS_AP_RSP:
      VOS_TRACE( VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_ERROR,
                 "%s: Setup failed, not a QoS AP\n",
                 __func__);
      if (!HDD_WMM_HANDLE_IMPLICIT == pQosContext->handle)
      {
         VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO,
                   "%s: Explicit Qos, notifying userspace",
                   __func__);

         //                                     
         pQosContext->lastStatus = HDD_WLAN_WMM_STATUS_SETUP_FAILED_NO_WMM;
         hdd_wmm_notify_app(pQosContext);
      }
      break;

   case SME_QOS_STATUS_SETUP_REQ_PENDING_RSP:
      VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO,
                "%s: Setup pending",
                __func__);
      //                                             
      break;

   case SME_QOS_STATUS_SETUP_MODIFIED_IND:
      VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO,
                "%s: Setup modified",
                __func__);
      if (pCurrentQosInfo)
      {
         //                 
         pAc->wmmAcTspecValid = VOS_TRUE;
         memcpy(&pAc->wmmAcTspecInfo,
                pCurrentQosInfo,
                sizeof(pAc->wmmAcTspecInfo));

         if (HDD_WMM_HANDLE_IMPLICIT != pQosContext->handle)
         {
            VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO,
                      "%s: Explicit Qos, notifying userspace",
                      __func__);

            //                                     
            pQosContext->lastStatus = HDD_WLAN_WMM_STATUS_MODIFIED;
            hdd_wmm_notify_app(pQosContext);
         }

         //                                             
         hdd_wmm_enable_tl_uapsd(pQosContext);
      }
      break;

   case SME_QOS_STATUS_SETUP_SUCCESS_NO_ACM_NO_APSD_RSP:
      if (HDD_WMM_HANDLE_IMPLICIT == pQosContext->handle)
      {

         VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO,
                   "%s: Implicit Qos, notifying TL",
                   __func__);

         //                                                                  
         pAc->wmmAcAccessPending = VOS_FALSE;
         pAc->wmmAcAccessGranted = VOS_TRUE;
         pAc->wmmAcAccessAllowed = VOS_TRUE;

         //                                   
         status = WLANTL_STAPktPending( (WLAN_HDD_GET_CTX(pAdapter))->pvosContext,
                                        (WLAN_HDD_GET_STATION_CTX_PTR(pAdapter))->conn_info.staId[0],
                                        acType );

         if ( !VOS_IS_STATUS_SUCCESS( status ) )
         {
            VOS_TRACE( VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_ERROR,
                       "%s: Failed to signal TL for AC=%d",
                       __func__, acType );
         }
      }
      else
      {
         VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO,
                   "%s: Explicit Qos, notifying userspace",
                   __func__);

         //                                     
         pQosContext->lastStatus = HDD_WLAN_WMM_STATUS_SETUP_SUCCESS_NO_ACM_NO_UAPSD;
         hdd_wmm_notify_app(pQosContext);
      }
      break;

   case SME_QOS_STATUS_SETUP_SUCCESS_IND_APSD_PENDING:
      //                      
      break;

   case SME_QOS_STATUS_SETUP_SUCCESS_IND_APSD_SET_FAILED:
      VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_ERROR,
                "%s: Setup successful but U-APSD failed",
                __func__);

      if (HDD_WMM_HANDLE_IMPLICIT == pQosContext->handle)
      {

         VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO,
                   "%s: Implicit Qos, notifying TL",
                   __func__);

         //                                                   
         //                                                                
         //                   
         //                                                                    
         pAc->wmmAcAccessGranted = VOS_TRUE;
         pAc->wmmAcAccessAllowed = VOS_TRUE;
         pAc->wmmAcAccessFailed = VOS_FALSE;
         pAc->wmmAcAccessPending = VOS_FALSE;

         //                                                                  
         status = WLANTL_STAPktPending( (WLAN_HDD_GET_CTX(pAdapter))->pvosContext,
                                        (WLAN_HDD_GET_STATION_CTX_PTR(pAdapter))->conn_info.staId[0],
                                        acType );

         if ( !VOS_IS_STATUS_SUCCESS( status ) )
         {
            VOS_TRACE( VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_ERROR,
                       "%s: Failed to signal TL for AC=%d",
                       __func__, acType );
         }
      }
      else
      {
         VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO,
                   "%s: Explicit Qos, notifying userspace",
                   __func__);

         //                                     
         pQosContext->lastStatus = HDD_WLAN_WMM_STATUS_SETUP_UAPSD_SET_FAILED;
         hdd_wmm_notify_app(pQosContext);
      }

      //                                                              
      hdd_wmm_disable_tl_uapsd(pQosContext);

      break;

   case SME_QOS_STATUS_RELEASE_SUCCESS_RSP:
      VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO,
                "%s: Release is complete",
                __func__);

      if (pCurrentQosInfo)
      {
         VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO,
                   "%s: flows still active",
                   __func__);

         //                                                    
         //                       
         memcpy(&pAc->wmmAcTspecInfo,
                pCurrentQosInfo,
                sizeof(pAc->wmmAcTspecInfo));

         //                                             
         hdd_wmm_enable_tl_uapsd(pQosContext);
      }
      else
      {
         VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO,
                   "%s: last flow",
                   __func__);

         //                                                                
         pAc->wmmAcTspecValid = VOS_FALSE;

         //                                             
         hdd_wmm_disable_tl_uapsd(pQosContext);
      }

      if (HDD_WMM_HANDLE_IMPLICIT != pQosContext->handle)
      {
         VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO,
                   "%s: Explicit Qos, notifying userspace",
                   __func__);

         //                                     
         pQosContext->lastStatus = HDD_WLAN_WMM_STATUS_RELEASE_SUCCESS;
         hdd_wmm_notify_app(pQosContext);
      }

      //                           
      hdd_wmm_free_context(pQosContext);
      break;

   case SME_QOS_STATUS_RELEASE_FAILURE_RSP:
      VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO,
                "%s: Release failure",
                __func__);

      //                                                                  
      if (HDD_WMM_HANDLE_IMPLICIT != pQosContext->handle)
      {
         VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO,
                   "%s: Explicit Qos, notifying userspace",
                   __func__);

         //                                     
         pQosContext->lastStatus = HDD_WLAN_WMM_STATUS_RELEASE_FAILED;
         hdd_wmm_notify_app(pQosContext);
      }

      break;

   case SME_QOS_STATUS_RELEASE_QOS_LOST_IND:
      VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO,
                "%s: QOS Lost indication received",
                __func__);

      //                                 
      pAc->wmmAcTspecValid = VOS_FALSE;

      //                                             
      hdd_wmm_disable_tl_uapsd(pQosContext);

      if (HDD_WMM_HANDLE_IMPLICIT == pQosContext->handle)
      {
         //                                          
         pAc->wmmAcAccessGranted = VOS_FALSE;
         pAc->wmmAcAccessFailed = VOS_FALSE;
      }
      else
      {
         VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO_LOW,
                   "%s: Explicit Qos, notifying userspace",
                   __func__);

         //                                     
         pQosContext->lastStatus = HDD_WLAN_WMM_STATUS_LOST;
         hdd_wmm_notify_app(pQosContext);
      }

      //                           
      hdd_wmm_free_context(pQosContext);
      break;

   case SME_QOS_STATUS_RELEASE_REQ_PENDING_RSP:
      VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO_LOW,
                "%s: Release pending",
                __func__);
      //                                             
      break;

   case SME_QOS_STATUS_RELEASE_INVALID_PARAMS_RSP:
      VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_ERROR,
                "%s: Release Invalid Params",
                __func__);
      if (HDD_WMM_HANDLE_IMPLICIT != pQosContext->handle)
      {
         //                                     
         pQosContext->lastStatus = HDD_WLAN_WMM_STATUS_RELEASE_FAILED_BAD_PARAM;
         hdd_wmm_notify_app(pQosContext);
      }
      break;

   case SME_QOS_STATUS_MODIFY_SETUP_SUCCESS_IND:
      VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO_LOW,
                "%s: Modification is complete, notify TL",
                __func__);

      //                                                                
      //                             
      if (pCurrentQosInfo)
      {
         pAc->wmmAcTspecValid = VOS_TRUE;
         memcpy(&pAc->wmmAcTspecInfo,
                pCurrentQosInfo,
                sizeof(pAc->wmmAcTspecInfo));
      }

      if (HDD_WMM_HANDLE_IMPLICIT != pQosContext->handle)
      {
         //                                     
         pQosContext->lastStatus = HDD_WLAN_WMM_STATUS_MODIFY_SUCCESS;
         hdd_wmm_notify_app(pQosContext);
      }

      //                                                
      hdd_wmm_enable_tl_uapsd(pQosContext);

      break;

   case SME_QOS_STATUS_MODIFY_SETUP_SUCCESS_APSD_SET_ALREADY:
      if (HDD_WMM_HANDLE_IMPLICIT != pQosContext->handle)
      {
         //                                     
         pQosContext->lastStatus = HDD_WLAN_WMM_STATUS_MODIFY_SUCCESS_NO_ACM_UAPSD_EXISTING;
         hdd_wmm_notify_app(pQosContext);
      }
      break;

   case SME_QOS_STATUS_MODIFY_SETUP_FAILURE_RSP:
      //                                                     
      //                            

      if (HDD_WMM_HANDLE_IMPLICIT != pQosContext->handle)
      {
         //                                     
         pQosContext->lastStatus = HDD_WLAN_WMM_STATUS_MODIFY_FAILED;
         hdd_wmm_notify_app(pQosContext);
      }
      break;

   case SME_QOS_STATUS_MODIFY_SETUP_PENDING_RSP:
      VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO_LOW,
                "%s: modification pending",
                __func__);
      //                                             
      break;

   case SME_QOS_STATUS_MODIFY_SETUP_SUCCESS_NO_ACM_NO_APSD_RSP:
      //                                                                 

      if (HDD_WMM_HANDLE_IMPLICIT != pQosContext->handle)
      {
         //                                     
         pQosContext->lastStatus = HDD_WLAN_WMM_STATUS_MODIFY_SUCCESS_NO_ACM_NO_UAPSD;
         hdd_wmm_notify_app(pQosContext);
      }
      break;

   case SME_QOS_STATUS_MODIFY_SETUP_INVALID_PARAMS_RSP:
      //                                         
      if (HDD_WMM_HANDLE_IMPLICIT != pQosContext->handle)
      {
         //                                     
         pQosContext->lastStatus = HDD_WLAN_WMM_STATUS_MODIFY_FAILED_BAD_PARAM;
         hdd_wmm_notify_app(pQosContext);
      }
      break;

   case SME_QOS_STATUS_MODIFY_SETUP_SUCCESS_IND_APSD_PENDING:
      //                                                                       
      break;

   case SME_QOS_STATUS_MODIFY_SETUP_SUCCESS_IND_APSD_SET_FAILED:
      VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_ERROR,
                "%s: Modify successful but U-APSD failed",
                __func__);

      //                                                           
      //                                                            
      //                                                  
      if (HDD_WMM_HANDLE_IMPLICIT != pQosContext->handle)
      {
         //                                     
         pQosContext->lastStatus = HDD_WLAN_WMM_STATUS_MODIFY_UAPSD_SET_FAILED;
         hdd_wmm_notify_app(pQosContext);
      }

      //                                                              
      hdd_wmm_disable_tl_uapsd(pQosContext);

      break;

   case SME_QOS_STATUS_HANDING_OFF:
      //                                
      break;

   case SME_QOS_STATUS_OUT_OF_APSD_POWER_MODE_IND:
      //                                                 
      hdd_wmm_disable_tl_uapsd(pQosContext);
      break;

   case SME_QOS_STATUS_INTO_APSD_POWER_MODE_IND:
      //                                                      
      hdd_wmm_enable_tl_uapsd(pQosContext);
      break;

   default:
      VOS_TRACE( VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_ERROR,
                 "%s: unexpected SME Status=%d\n",
                 __func__, smeStatus );
      VOS_ASSERT(0);
   }

   //                                                               
   //                                                               
   //                                                           
   //                                                        
   pAc->wmmAcAccessAllowed = hdd_wmm_is_access_allowed(pAdapter, pAc);

   VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO,
             "%s: complete, access for TL AC %d is%sallowed",
             __func__,
             acType,
             pAc->wmmAcAccessAllowed ? " " : " not ");

   return eHAL_STATUS_SUCCESS;
}
#endif

/*                                                                         
                                                                   

                                                         

                                                      

                                                                      
                                                                         */
int hdd_wmmps_helper(hdd_adapter_t *pAdapter, tANI_U8 *ptr)
{
   if (NULL == pAdapter)
   {
       VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_ERROR,
                 "%s: pAdapter is NULL", __func__);
       return -EINVAL;
   }
   if (NULL == ptr)
   {
       VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_ERROR,
                 "%s: ptr is NULL", __func__);
       return -EINVAL;
   }
   /*                          */
   pAdapter->configuredPsb = ptr[9] - '0';
   pAdapter->psbChanged = HDD_PSB_CHANGED;

   return 0;
}

/*                                                                             
                                                                         
                             

                                                   

                        
                                                                             */
static void hdd_wmm_do_implicit_qos(struct work_struct *work)
{
   hdd_wmm_qos_context_t* pQosContext =
      container_of(work, hdd_wmm_qos_context_t, wmmAcSetupImplicitQos);
   hdd_adapter_t* pAdapter;
   WLANTL_ACEnumType acType;
   hdd_wmm_ac_status_t *pAc;
#ifndef WLAN_MDM_CODE_REDUCTION_OPT
   VOS_STATUS status;
   sme_QosStatusType smeStatus;
#endif
   sme_QosWmmTspecInfo qosInfo;

   VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO_LOW,
             "%s: Entered, context %p",
             __func__, pQosContext);

   if (unlikely(HDD_WMM_CTX_MAGIC != pQosContext->magic))
   {
      VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_ERROR,
                "%s: Invalid QoS Context",
                __func__);
      return;
   }

   pAdapter = pQosContext->pAdapter;
   acType = pQosContext->acType;
   pAc = &pAdapter->hddWmmStatus.wmmAcStatus[acType];

   VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO_LOW,
             "%s: pAdapter %p acType %d",
             __func__, pAdapter, acType);

   if (!pAc->wmmAcAccessNeeded)
   {
      VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_ERROR,
                "%s: AC %d doesn't need service",
                __func__, acType);
      pQosContext->magic = 0;
      kfree(pQosContext);
      return;
   }

   pAc->wmmAcAccessPending = VOS_TRUE;
   pAc->wmmAcAccessNeeded = VOS_FALSE;

   memset(&qosInfo, 0, sizeof(qosInfo));

   qosInfo.ts_info.psb = pAdapter->configuredPsb;

   switch (acType)
   {
   case WLANTL_AC_VO:
      qosInfo.ts_info.up = SME_QOS_WMM_UP_VO;
      /*                                                          */
      if (HDD_PSB_CFG_INVALID == pAdapter->configuredPsb)
      {
          qosInfo.ts_info.psb = ((WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->UapsdMask &
                                  SME_QOS_UAPSD_VO) ? 1 : 0;
      }
      qosInfo.ts_info.direction = (WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->InfraDirAcVo;
      qosInfo.ts_info.tid = 255;
      qosInfo.mean_data_rate = (WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->InfraMeanDataRateAcVo;
      qosInfo.min_phy_rate = (WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->InfraMinPhyRateAcVo;
      qosInfo.min_service_interval = (WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->InfraUapsdVoSrvIntv;
      qosInfo.nominal_msdu_size = (WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->InfraNomMsduSizeAcVo;
      qosInfo.surplus_bw_allowance = (WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->InfraSbaAcVo;
      qosInfo.suspension_interval = (WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->InfraUapsdVoSuspIntv;
      break;
   case WLANTL_AC_VI:
      qosInfo.ts_info.up = SME_QOS_WMM_UP_VI;
      /*                                                          */
      if (HDD_PSB_CFG_INVALID == pAdapter->configuredPsb)
      {
          qosInfo.ts_info.psb = ((WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->UapsdMask &
                                  SME_QOS_UAPSD_VI) ? 1 : 0;
      }
      qosInfo.ts_info.direction = (WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->InfraDirAcVi;
      qosInfo.ts_info.tid = 255;
      qosInfo.mean_data_rate = (WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->InfraMeanDataRateAcVi;
      qosInfo.min_phy_rate = (WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->InfraMinPhyRateAcVi;
      qosInfo.min_service_interval = (WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->InfraUapsdViSrvIntv;
      qosInfo.nominal_msdu_size = (WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->InfraNomMsduSizeAcVi;
      qosInfo.surplus_bw_allowance = (WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->InfraSbaAcVi;
      qosInfo.suspension_interval = (WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->InfraUapsdViSuspIntv;
      break;
   default:
   case WLANTL_AC_BE:
      qosInfo.ts_info.up = SME_QOS_WMM_UP_BE;
      /*                                                          */
      if (HDD_PSB_CFG_INVALID == pAdapter->configuredPsb)
      {
          qosInfo.ts_info.psb = ((WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->UapsdMask &
                                  SME_QOS_UAPSD_BE) ? 1 : 0;
      }
      qosInfo.ts_info.direction = (WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->InfraDirAcBe;
      qosInfo.ts_info.tid = 255;
      qosInfo.mean_data_rate = (WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->InfraMeanDataRateAcBe;
      qosInfo.min_phy_rate = (WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->InfraMinPhyRateAcBe;
      qosInfo.min_service_interval = (WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->InfraUapsdBeSrvIntv;
      qosInfo.nominal_msdu_size = (WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->InfraNomMsduSizeAcBe;
      qosInfo.surplus_bw_allowance = (WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->InfraSbaAcBe;
      qosInfo.suspension_interval = (WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->InfraUapsdBeSuspIntv;
      break;
   case WLANTL_AC_BK:
      qosInfo.ts_info.up = SME_QOS_WMM_UP_BK;
      /*                                                          */
      if (HDD_PSB_CFG_INVALID == pAdapter->configuredPsb)
      {
          qosInfo.ts_info.psb = ((WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->UapsdMask &
                                  SME_QOS_UAPSD_BK) ? 1 : 0;
      }
      qosInfo.ts_info.direction = (WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->InfraDirAcBk;
      qosInfo.ts_info.tid = 255;
      qosInfo.mean_data_rate = (WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->InfraMeanDataRateAcBk;
      qosInfo.min_phy_rate = (WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->InfraMinPhyRateAcBk;
      qosInfo.min_service_interval = (WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->InfraUapsdBkSrvIntv;
      qosInfo.nominal_msdu_size = (WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->InfraNomMsduSizeAcBk;
      qosInfo.surplus_bw_allowance = (WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->InfraSbaAcBk;
      qosInfo.suspension_interval = (WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->InfraUapsdBkSuspIntv;
      break;
   }
#ifdef FEATURE_WLAN_CCX
   qosInfo.inactivity_interval = (WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->InfraInactivityInterval;
#endif
   qosInfo.ts_info.burst_size_defn = (WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->burstSizeDefinition;

   switch ((WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->tsInfoAckPolicy)
   {
     case HDD_WLAN_WMM_TS_INFO_ACK_POLICY_NORMAL_ACK:
       qosInfo.ts_info.ack_policy = SME_QOS_WMM_TS_ACK_POLICY_NORMAL_ACK;
       break;

     case HDD_WLAN_WMM_TS_INFO_ACK_POLICY_HT_IMMEDIATE_BLOCK_ACK:
       qosInfo.ts_info.ack_policy = SME_QOS_WMM_TS_ACK_POLICY_HT_IMMEDIATE_BLOCK_ACK;
       break;

     default:
       //        
       qosInfo.ts_info.ack_policy = SME_QOS_WMM_TS_ACK_POLICY_NORMAL_ACK;
   }

   if(qosInfo.ts_info.ack_policy == SME_QOS_WMM_TS_ACK_POLICY_HT_IMMEDIATE_BLOCK_ACK)
   {
     if(!sme_QosIsTSInfoAckPolicyValid((tpAniSirGlobal)WLAN_HDD_GET_HAL_CTX(pAdapter), &qosInfo, pAdapter->sessionId))
     {
       qosInfo.ts_info.ack_policy = SME_QOS_WMM_TS_ACK_POLICY_NORMAL_ACK;
     }
   }

   mutex_lock(&pAdapter->hddWmmStatus.wmmLock);
   list_add(&pQosContext->node, &pAdapter->hddWmmStatus.wmmContextList);
   mutex_unlock(&pAdapter->hddWmmStatus.wmmLock);

#ifndef WLAN_MDM_CODE_REDUCTION_OPT
   smeStatus = sme_QosSetupReq(WLAN_HDD_GET_HAL_CTX(pAdapter),
                               pAdapter->sessionId,
                               &qosInfo,
                               hdd_wmm_sme_callback,
                               pQosContext,
                               qosInfo.ts_info.up,
                               &pQosContext->qosFlowId);

   VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO,
             "%s: sme_QosSetupReq returned %d flowid %d",
             __func__, smeStatus, pQosContext->qosFlowId);

   //                                                      
   switch (smeStatus)
   {
   case SME_QOS_STATUS_SETUP_REQ_PENDING_RSP:
   case SME_QOS_STATUS_SETUP_SUCCESS_IND_APSD_PENDING:
      //                                             
      //                                                        
      VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO_LOW,
                "%s: Setup is pending, no further work",
                __func__);

      break;


   case SME_QOS_STATUS_SETUP_FAILURE_RSP:
      //                                                                  
      //                                                             

      //                                                        
      //                
      hdd_wmm_free_context(pQosContext);

      //                                       
   case SME_QOS_STATUS_SETUP_SUCCESS_NO_ACM_NO_APSD_RSP:
      //                                          
   case SME_QOS_STATUS_SETUP_SUCCESS_APSD_SET_ALREADY:
      //                                                          

      //                                                      
      //                                 
      VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO_LOW,
                "%s: Setup is complete, notify TL",
                __func__);

      pAc->wmmAcAccessAllowed = VOS_TRUE;
      pAc->wmmAcAccessGranted = VOS_TRUE;
      pAc->wmmAcAccessPending = VOS_FALSE;

      status = WLANTL_STAPktPending( (WLAN_HDD_GET_CTX(pAdapter))->pvosContext,
                                     (WLAN_HDD_GET_STATION_CTX_PTR(pAdapter))->conn_info.staId[0],
                                     acType );

      if ( !VOS_IS_STATUS_SUCCESS( status ) )
      {
         VOS_TRACE( VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_ERROR,
                    "%s: Failed to signal TL for AC=%d",
                    __func__, acType );
      }

      break;


   default:
      VOS_TRACE( VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_ERROR,
                 "%s: unexpected SME Status=%d\n",
                 __func__, smeStatus );
      VOS_ASSERT(0);
   }
#endif

}

/*                                                                             
                                                                             
                                                                             
                      

                                               

                                                   
                                           

                                                                             */
VOS_STATUS hdd_wmm_init ( hdd_context_t* pHddCtx )
{
   v_U8_t dscp;

   VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO_LOW,
             "%s: Entered", __func__);

   //                                   
   for (dscp = 0; dscp <= WLAN_HDD_MAX_DSCP; dscp++)
   {
      hddWmmDscpToUpMap[dscp] = SME_QOS_WMM_UP_BE;
   }
   hddWmmDscpToUpMap[8]  = SME_QOS_WMM_UP_BK;
   hddWmmDscpToUpMap[16] = SME_QOS_WMM_UP_RESV;
   hddWmmDscpToUpMap[24] = SME_QOS_WMM_UP_EE;
   hddWmmDscpToUpMap[32] = SME_QOS_WMM_UP_CL;
   hddWmmDscpToUpMap[40] = SME_QOS_WMM_UP_VI;
   hddWmmDscpToUpMap[48] = SME_QOS_WMM_UP_VO;
   hddWmmDscpToUpMap[56] = SME_QOS_WMM_UP_NC;
   return VOS_STATUS_SUCCESS;
}

/*                                                                             
                                                                                     
                                                                             
                      

                                                    

                                                   
                                           

                                                                             */
VOS_STATUS hdd_wmm_adapter_init( hdd_adapter_t *pAdapter )
{
   hdd_wmm_ac_status_t *pAcStatus;
   WLANTL_ACEnumType acType;

   VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO_LOW,
             "%s: Entered", __func__);

   pAdapter->hddWmmStatus.wmmQap = VOS_FALSE;
   INIT_LIST_HEAD(&pAdapter->hddWmmStatus.wmmContextList);
   mutex_init(&pAdapter->hddWmmStatus.wmmLock);

   for (acType = 0; acType < WLANTL_MAX_AC; acType++)
   {
      pAcStatus = &pAdapter->hddWmmStatus.wmmAcStatus[acType];
      pAcStatus->wmmAcAccessRequired = VOS_FALSE;
      pAcStatus->wmmAcAccessNeeded = VOS_FALSE;
      pAcStatus->wmmAcAccessPending = VOS_FALSE;
      pAcStatus->wmmAcAccessFailed = VOS_FALSE;
      pAcStatus->wmmAcAccessGranted = VOS_FALSE;
      pAcStatus->wmmAcAccessAllowed = VOS_FALSE;
      pAcStatus->wmmAcTspecValid = VOS_FALSE;
      pAcStatus->wmmAcUapsdInfoValid = VOS_FALSE;
   }
   //                                                                                
   pAdapter->configuredPsb = HDD_PSB_CFG_INVALID;

   return VOS_STATUS_SUCCESS;
}

/*                                                                             
                                                                           
                 

                                                    

                                                   
                                           

                                                                             */
VOS_STATUS hdd_wmm_adapter_clear( hdd_adapter_t *pAdapter )
{
   hdd_wmm_ac_status_t *pAcStatus;
   WLANTL_ACEnumType acType;
   VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO_LOW,
             "%s: Entered", __func__);
   for (acType = 0; acType < WLANTL_MAX_AC; acType++)
   {
      pAcStatus = &pAdapter->hddWmmStatus.wmmAcStatus[acType];
      pAcStatus->wmmAcAccessRequired = VOS_FALSE;
      pAcStatus->wmmAcAccessNeeded = VOS_FALSE;
      pAcStatus->wmmAcAccessPending = VOS_FALSE;
      pAcStatus->wmmAcAccessFailed = VOS_FALSE;
      pAcStatus->wmmAcAccessGranted = VOS_FALSE;
      pAcStatus->wmmAcAccessAllowed = VOS_FALSE;
      pAcStatus->wmmAcTspecValid = VOS_FALSE;
      pAcStatus->wmmAcUapsdInfoValid = VOS_FALSE;
   }
   return VOS_STATUS_SUCCESS;
}

/*                                                                             
                                                                            
                                                                     

                                                    

                                                   
                                           

                                                                             */
VOS_STATUS hdd_wmm_adapter_close ( hdd_adapter_t* pAdapter )
{
   hdd_wmm_qos_context_t* pQosContext;

   VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO_LOW,
             "%s: Entered", __func__);

   //                                                   
   while (!list_empty(&pAdapter->hddWmmStatus.wmmContextList))
   {
      pQosContext = list_first_entry(&pAdapter->hddWmmStatus.wmmContextList,
                                     hdd_wmm_qos_context_t, node);
#ifdef FEATURE_WLAN_CCX
      hdd_wmm_disable_inactivity_timer(pQosContext);
#endif
#ifdef WLAN_OPEN_SOURCE
      cancel_work_sync(&pQosContext->wmmAcSetupImplicitQos);
#endif
      hdd_wmm_free_context(pQosContext);
   }

   return VOS_STATUS_SUCCESS;
}

/*                                                                             
                                                                   
             

                                                        
                                                            
                                       
                                                                             */
v_BOOL_t is_dhcp_packet(struct sk_buff *skb)
{
   if (*((u16*)((u8*)skb->data+34)) == DHCP_SOURCE_PORT ||
       *((u16*)((u8*)skb->data+34)) == DHCP_DESTINATION_PORT)
      return VOS_TRUE;

   return VOS_FALSE;
}

/*                                                                             
                                                                           
                                              

                                                    
                                                        
                                                             

                        
                                                                             */
v_VOID_t hdd_wmm_classify_pkt ( hdd_adapter_t* pAdapter,
                                struct sk_buff *skb,
                                WLANTL_ACEnumType* pAcType,
                                sme_QosWmmUpType *pUserPri)
{
   unsigned char * pPkt;
   union generic_ethhdr *pHdr;
   struct iphdr *pIpHdr;
   unsigned char tos;
   unsigned char dscp;
   sme_QosWmmUpType userPri;
   WLANTL_ACEnumType acType;

   //                                                 
   //                                   

#ifdef HDD_WMM_DEBUG
   VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO_LOW,
             "%s: Entered", __func__);
#endif //              

   pPkt = skb->data;
   pHdr = (union generic_ethhdr *)pPkt;

#ifdef HDD_WMM_DEBUG
   VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO_LOW,
             "%s: proto/length is 0x%04x",
             __func__, pHdr->eth_II.h_proto);
#endif //              

   if (HDD_WMM_CLASSIFICATION_DSCP == (WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->PktClassificationBasis)
   {
      if (pHdr->eth_II.h_proto == htons(ETH_P_IP))
      {
         //                              
         pIpHdr = (struct iphdr *)&pPkt[sizeof(pHdr->eth_II)];
         tos = pIpHdr->tos;
#ifdef HDD_WMM_DEBUG
         VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO_LOW,
                   "%s: Ethernet II IP Packet, tos is %d",
                   __func__, tos);
#endif //              

      }
      else if ((ntohs(pHdr->eth_II.h_proto) < WLAN_MIN_PROTO) &&
               (pHdr->eth_8023.h_snap.dsap == WLAN_SNAP_DSAP) &&
               (pHdr->eth_8023.h_snap.ssap == WLAN_SNAP_SSAP) &&
               (pHdr->eth_8023.h_snap.ctrl == WLAN_SNAP_CTRL) &&
               (pHdr->eth_8023.h_proto == htons(ETH_P_IP)))
      {
         //                                 
         pIpHdr = (struct iphdr *)&pPkt[sizeof(pHdr->eth_8023)];
         tos = pIpHdr->tos;
#ifdef HDD_WMM_DEBUG
         VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO_LOW,
                   "%s: 802.3 LLC/SNAP IP Packet, tos is %d",
                   __func__, tos);
#endif //              
      }
      else if (pHdr->eth_II.h_proto == htons(ETH_P_8021Q))
      {
         //            

         if (pHdr->eth_IIv.h_vlan_encapsulated_proto == htons(ETH_P_IP))
         {
            //                                          
            pIpHdr = (struct iphdr *)&pPkt[sizeof(pHdr->eth_IIv)];
            tos = pIpHdr->tos;
#ifdef HDD_WMM_DEBUG
            VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO_LOW,
                      "%s: Ethernet II VLAN tagged IP Packet, tos is %d",
                      __func__, tos);
#endif //              
         }
         else if ((ntohs(pHdr->eth_IIv.h_vlan_encapsulated_proto) < WLAN_MIN_PROTO) &&
                  (pHdr->eth_8023v.h_snap.dsap == WLAN_SNAP_DSAP) &&
                  (pHdr->eth_8023v.h_snap.ssap == WLAN_SNAP_SSAP) &&
                  (pHdr->eth_8023v.h_snap.ctrl == WLAN_SNAP_CTRL) &&
                  (pHdr->eth_8023v.h_proto == htons(ETH_P_IP)))
         {
            //                                             
            pIpHdr = (struct iphdr *)&pPkt[sizeof(pHdr->eth_8023v)];
            tos = pIpHdr->tos;
#ifdef HDD_WMM_DEBUG
            VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO_LOW,
                      "%s: 802.3 LLC/SNAP VLAN tagged IP Packet, tos is %d",
                      __func__, tos);
#endif //              
         }
         else
         {
            //        
#ifdef HDD_WMM_DEBUG
            VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_WARN,
                      "%s: VLAN tagged Unhandled Protocol, using default tos",
                      __func__);
#endif //              
            tos = 0;
         }
      }
      else
      {
          //        
#ifdef HDD_WMM_DEBUG
          VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_WARN,
                  "%s: Unhandled Protocol, using default tos",
                  __func__);
#endif //              
          //                                          
          if (pHdr->eth_II.h_proto == htons(HDD_ETHERTYPE_802_1_X))
              tos = 0xC0;
          else
              tos = 0;
      }

      dscp = (tos>>2) & 0x3f;
      userPri = hddWmmDscpToUpMap[dscp];

#ifdef HDD_WMM_DEBUG
      VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO,
                "%s: tos is %d, dscp is %d, up is %d",
                __func__, tos, dscp, userPri);
#endif //              

   }
   else if (HDD_WMM_CLASSIFICATION_802_1Q == (WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->PktClassificationBasis)
   {
      if (pHdr->eth_IIv.h_vlan_proto == htons(ETH_P_8021Q))
      {
         //            
         userPri = (ntohs(pHdr->eth_IIv.h_vlan_TCI)>>13) & 0x7;
#ifdef HDD_WMM_DEBUG
         VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO,
                   "%s: Tagged frame, UP is %d",
                   __func__, userPri);
#endif //              
      }
      else
      {
          //                             
#ifdef HDD_WMM_DEBUG
          VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_WARN,
                  "%s: Untagged frame, using default UP",
                  __func__);
#endif //              
          //                                          
          if (pHdr->eth_II.h_proto == htons(HDD_ETHERTYPE_802_1_X))
              userPri = SME_QOS_WMM_UP_VO;
          else
              userPri = SME_QOS_WMM_UP_BE;
      }
   }
   else
   {
      //        
#ifdef HDD_WMM_DEBUG
      VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_ERROR,
                "%s: Unknown classification scheme, using default UP",
                __func__);
#endif //              
      userPri = SME_QOS_WMM_UP_BE;
   }

   acType = hddWmmUpToAcMap[userPri];

#ifdef HDD_WMM_DEBUG
   VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO,
             "%s: UP is %d, AC is %d",
             __func__, userPri, acType);
#endif //              

   *pUserPri = userPri;
   *pAcType = acType;

   return;
}

/*                                                                             
                                                                              
                                              


                                                         
                                              

                                     
                                                                             */
v_U16_t hdd_hostapd_select_queue(struct net_device * dev, struct sk_buff *skb)
{
   WLANTL_ACEnumType ac;
   sme_QosWmmUpType up = SME_QOS_WMM_UP_BE;
   v_USHORT_t queueIndex;
   v_MACADDR_t *pDestMacAddress = (v_MACADDR_t*)skb->data;
   hdd_adapter_t *pAdapter = (hdd_adapter_t *)netdev_priv(dev);
   hdd_context_t *pHddCtx = WLAN_HDD_GET_CTX(pAdapter);
   v_U8_t STAId;
   v_U8_t *pSTAId = (v_U8_t *)(((v_U8_t *)(skb->data)) - 1);

   /*                  */
   if (VOS_STATUS_SUCCESS != hdd_softap_GetStaId(pAdapter, pDestMacAddress, &STAId))
   {
      VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_INFO,
            "%s: Failed to find right station", __func__);
      *pSTAId = HDD_WLAN_INVALID_STA_ID;
      goto done;
   }

   spin_lock_bh( &pAdapter->staInfo_lock );
   if (FALSE == vos_is_macaddr_equal(&pAdapter->aStaInfo[STAId].macAddrSTA, pDestMacAddress))
   {
      VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_INFO,
                   "%s: Station MAC address does not matching", __func__);

      *pSTAId = HDD_WLAN_INVALID_STA_ID;
      goto release_lock;
   }
   if (pAdapter->aStaInfo[STAId].isUsed && pAdapter->aStaInfo[STAId].isQosEnabled && (HDD_WMM_USER_MODE_NO_QOS != pHddCtx->cfg_ini->WmmMode))
   {
      /*                                                         */
      hdd_wmm_classify_pkt (pAdapter, skb, &ac, &up);
      //                                                                         
      if (pAdapter->aStaInfo[STAId].vosLowResource && is_dhcp_packet(skb))
      {
         VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_WARN,
                    "%s: Making priority of DHCP packet as VOICE", __func__);
         up = SME_QOS_WMM_UP_VO;
         ac = hddWmmUpToAcMap[up];
      }
   }
   *pSTAId = STAId;

release_lock:
    spin_unlock_bh( &pAdapter->staInfo_lock );
done:
   skb->priority = up;
   queueIndex = hddLinuxUpToAcMap[skb->priority];

   return queueIndex;
}

/*                                                                             
                                                                          
                                              


                                                         
                                              

                                     
                                                                             */
v_U16_t hdd_wmm_select_queue(struct net_device * dev, struct sk_buff *skb)
{
   WLANTL_ACEnumType ac;
   sme_QosWmmUpType up = SME_QOS_WMM_UP_BE;
   v_USHORT_t queueIndex;
   hdd_adapter_t *pAdapter =  WLAN_HDD_GET_PRIV_PTR(dev);

   if (isWDresetInProgress()) {
       VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_WARN,
                  FL("called during WDReset"));
       skb->priority = SME_QOS_WMM_UP_BE;
       return HDD_LINUX_AC_BE;
   }

   /*                  */
   if (WLAN_HDD_IBSS == pAdapter->device_mode)
   {
       v_U8_t *pSTAId = (v_U8_t *)(((v_U8_t *)(skb->data)) - 1);
       v_MACADDR_t *pDestMacAddress = (v_MACADDR_t*)skb->data;

       if ( VOS_STATUS_SUCCESS !=
            hdd_Ibss_GetStaId(&pAdapter->sessionCtx.station,
                               pDestMacAddress, pSTAId))
       {
          VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO,
                     "%s: Failed to find right station pDestMacAddress: "
                     MAC_ADDRESS_STR , __func__,
                     MAC_ADDR_ARRAY(pDestMacAddress->bytes));
          *pSTAId = HDD_WLAN_INVALID_STA_ID;
          goto done;
       }
   }
   //                                                   
   //                                                               
   if( hdd_wmm_is_active(pAdapter) ) {
      /*                                                         */
      hdd_wmm_classify_pkt (pAdapter, skb, &ac, &up);
      //                                                                                
      if (pAdapter->isVosLowResource && is_dhcp_packet(skb))
      {
         VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_WARN,
                   "%s: Making priority of DHCP packet as VOICE", __func__);
         up = SME_QOS_WMM_UP_VO;
         ac = hddWmmUpToAcMap[up];
      }
   }
done:
   skb->priority = up;
   queueIndex = hddLinuxUpToAcMap[skb->priority];

   return queueIndex;
}

/*                                                                           
                                                                          
                                                                               
                   

                                                      

                                                 

                       
                                                                             */
void hdd_wmm_acquire_access_required(hdd_adapter_t *pAdapter,
                                     WLANTL_ACEnumType acType)
{
/*                                           
                                                        
                  
 */
   switch(acType)
   {
   case WLANTL_AC_BK:
      pAdapter->psbChanged &= ~SME_QOS_UAPSD_CFG_BK_CHANGED_MASK; /*                 */
      break;
   case WLANTL_AC_BE:
      pAdapter->psbChanged &= ~SME_QOS_UAPSD_CFG_BE_CHANGED_MASK; /*                  */
      break;
   case WLANTL_AC_VI:
      pAdapter->psbChanged &= ~SME_QOS_UAPSD_CFG_VI_CHANGED_MASK; /*                 */
      break;
   case WLANTL_AC_VO:
      pAdapter->psbChanged &= ~SME_QOS_UAPSD_CFG_VO_CHANGED_MASK; /*                  */
      break;
   default:
      VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_ERROR,
               "%s: Invalid AC Type", __func__);
     break;
   }
}

/*                                                                             
                                                                          
                         

                                                    
                                                  
                                                                          
                                                 

                                                   
                                           
                                                                             */
VOS_STATUS hdd_wmm_acquire_access( hdd_adapter_t* pAdapter,
                                   WLANTL_ACEnumType acType,
                                   v_BOOL_t * pGranted )
{
   hdd_wmm_qos_context_t *pQosContext;

   VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO_LOW,
             "%s: Entered for AC %d", __func__, acType);

   if (!hdd_wmm_is_active(pAdapter) || !(WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->bImplicitQosEnabled)
   {
      //                                                       
      //                                    
      VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO_LOW,
                "%s: QoS not configured on both ends ", __func__);

      pAdapter->hddWmmStatus.wmmAcStatus[acType].wmmAcAccessAllowed = VOS_TRUE;
      *pGranted = VOS_TRUE;
      return VOS_STATUS_SUCCESS;
   }

   //                                                                
   if ((pAdapter->hddWmmStatus.wmmAcStatus[acType].wmmAcAccessNeeded) ||
       (pAdapter->hddWmmStatus.wmmAcStatus[acType].wmmAcAccessPending))
   {
      //                                                             
      VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO_LOW,
                "%s: Implicit QoS for TL AC %d already scheduled",
                __func__, acType);

      *pGranted = VOS_FALSE;
      return VOS_STATUS_SUCCESS;
   }

   //                                                           
   //                                                                      
   if (pAdapter->hddWmmStatus.wmmAcStatus[acType].wmmAcAccessFailed)
   {
      //                          
      //                                      
      VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO_LOW,
                "%s: Implicit QoS for TL AC %d previously failed",
                __func__, acType);

      pAdapter->hddWmmStatus.wmmAcStatus[acType].wmmAcAccessAllowed = VOS_TRUE;
      *pGranted = VOS_TRUE;
      return VOS_STATUS_SUCCESS;
   }

   //                                  
   VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO,
             "%s: Need to schedule implicit QoS for TL AC %d, pAdapter is %p",
             __func__, acType, pAdapter);

   pAdapter->hddWmmStatus.wmmAcStatus[acType].wmmAcAccessNeeded = VOS_TRUE;

   pQosContext = kmalloc(sizeof(*pQosContext), GFP_ATOMIC);
   if (NULL == pQosContext)
   {
      //                                                                
      VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_ERROR,
                "%s: Unable to allocate context", __func__);
      pAdapter->hddWmmStatus.wmmAcStatus[acType].wmmAcAccessAllowed = VOS_TRUE;
      *pGranted = VOS_TRUE;
      return VOS_STATUS_SUCCESS;
   }

   pQosContext->acType = acType;
   pQosContext->pAdapter = pAdapter;
   pQosContext->qosFlowId = 0;
   pQosContext->handle = HDD_WMM_HANDLE_IMPLICIT;
   pQosContext->magic = HDD_WMM_CTX_MAGIC;
   INIT_WORK(&pQosContext->wmmAcSetupImplicitQos,
             hdd_wmm_do_implicit_qos);

   VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO,
             "%s: Scheduling work for AC %d, context %p",
             __func__, acType, pQosContext);

   schedule_work(&pQosContext->wmmAcSetupImplicitQos);

   //                                                        
   //                            
   *pGranted = VOS_FALSE;
   return VOS_STATUS_SUCCESS;
}

/*                                                                             
                                                                      
                                              

                                                    
                                                     
                                     

                                                   
                                           
                                                                             */
VOS_STATUS hdd_wmm_assoc( hdd_adapter_t* pAdapter,
                          tCsrRoamInfo *pRoamInfo,
                          eCsrRoamBssType eBssType )
{
   tANI_U8 uapsdMask;
   VOS_STATUS status;

   //                                                             
   //                                

   VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO_LOW,
             "%s: Entered", __func__);

   if (pRoamInfo->fReassocReq)
   {
      //                                                       
      //                                                   
      //                                                      
      //                                                      
      //                                                      
      //                                                  

      VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO_LOW,
                "%s: Reassoc so no work, Exiting", __func__);

      return VOS_STATUS_SUCCESS;
   }

   //                              
   uapsdMask = pRoamInfo->u.pConnectedProfile->modifyProfileFields.uapsd_mask;

   VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO_LOW,
             "%s: U-APSD mask is 0x%02x", __func__, (int) uapsdMask);

   if (uapsdMask & HDD_AC_VO)
   {
      status = WLANTL_EnableUAPSDForAC( (WLAN_HDD_GET_CTX(pAdapter))->pvosContext,
                                        (WLAN_HDD_GET_STATION_CTX_PTR(pAdapter))->conn_info.staId[0],
                                        WLANTL_AC_VO,
                                        7,
                                        7,
                                        (WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->InfraUapsdVoSrvIntv,
                                        (WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->InfraUapsdVoSuspIntv,
                                        WLANTL_BI_DIR );

      VOS_ASSERT( VOS_IS_STATUS_SUCCESS( status ));
   }

   if (uapsdMask & HDD_AC_VI)
   {
      status = WLANTL_EnableUAPSDForAC( (WLAN_HDD_GET_CTX(pAdapter))->pvosContext,
                                        (WLAN_HDD_GET_STATION_CTX_PTR(pAdapter))->conn_info.staId[0],
                                        WLANTL_AC_VI,
                                        5,
                                        5,
                                        (WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->InfraUapsdViSrvIntv,
                                        (WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->InfraUapsdViSuspIntv,
                                        WLANTL_BI_DIR );

      VOS_ASSERT( VOS_IS_STATUS_SUCCESS( status ));
   }

   if (uapsdMask & HDD_AC_BK)
   {
      status = WLANTL_EnableUAPSDForAC( (WLAN_HDD_GET_CTX(pAdapter))->pvosContext,
                                        (WLAN_HDD_GET_STATION_CTX_PTR(pAdapter))->conn_info.staId[0],
                                        WLANTL_AC_BK,
                                        2,
                                        2,
                                        (WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->InfraUapsdBkSrvIntv,
                                        (WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->InfraUapsdBkSuspIntv,
                                        WLANTL_BI_DIR );

      VOS_ASSERT( VOS_IS_STATUS_SUCCESS( status ));
   }

   if (uapsdMask & HDD_AC_BE)
   {
      status = WLANTL_EnableUAPSDForAC( (WLAN_HDD_GET_CTX(pAdapter))->pvosContext,
                                        (WLAN_HDD_GET_STATION_CTX_PTR(pAdapter))->conn_info.staId[0],
                                        WLANTL_AC_BE,
                                        3,
                                        3,
                                        (WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->InfraUapsdBeSrvIntv,
                                        (WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->InfraUapsdBeSuspIntv,
                                        WLANTL_BI_DIR );

      VOS_ASSERT( VOS_IS_STATUS_SUCCESS( status ));
   }

   VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO_LOW,
             "%s: Exiting", __func__);

   return VOS_STATUS_SUCCESS;
}



static const v_U8_t acmMaskBit[WLANTL_MAX_AC] =
   {
      0x4, /*              */
      0x8, /*              */
      0x2, /*              */
      0x1  /*              */
   };

/*                                                                             
                                                                        
                                                  

                                                    
                                                     
                                     

                                                   
                                           
                                                                             */
VOS_STATUS hdd_wmm_connect( hdd_adapter_t* pAdapter,
                            tCsrRoamInfo *pRoamInfo,
                            eCsrRoamBssType eBssType )
{
   int ac;
   v_BOOL_t qap;
   v_BOOL_t qosConnection;
   v_U8_t acmMask;

   VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO_LOW,
             "%s: Entered", __func__);

   if ((eCSR_BSS_TYPE_INFRASTRUCTURE == eBssType) &&
       pRoamInfo &&
       pRoamInfo->u.pConnectedProfile)
   {
      qap = pRoamInfo->u.pConnectedProfile->qap;
      qosConnection = pRoamInfo->u.pConnectedProfile->qosConnection;
      acmMask = pRoamInfo->u.pConnectedProfile->acm_mask;
   }
   else
   {
      qap = VOS_TRUE;
      qosConnection = VOS_TRUE;
      acmMask = 0x0;
   }

   VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO_LOW,
             "%s: qap is %d, qosConnection is %d, acmMask is 0x%x",
             __func__, qap, qosConnection, acmMask);

   pAdapter->hddWmmStatus.wmmQap = qap;
   pAdapter->hddWmmStatus.wmmQosConnection = qosConnection;

   for (ac = 0; ac < WLANTL_MAX_AC; ac++)
   {
      if (qap &&
          qosConnection &&
          (acmMask & acmMaskBit[ac]))
      {
         VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO_LOW,
                   "%s: ac %d on",
                   __func__, ac);

         //                      
         pAdapter->hddWmmStatus.wmmAcStatus[ac].wmmAcAccessRequired = VOS_TRUE;
         pAdapter->hddWmmStatus.wmmAcStatus[ac].wmmAcAccessAllowed = VOS_FALSE;
         pAdapter->hddWmmStatus.wmmAcStatus[ac].wmmAcAccessGranted = VOS_FALSE;
      }
      else
      {
         VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO_LOW,
                   "%s: ac %d off",
                   __func__, ac);
         //                                               
         pAdapter->hddWmmStatus.wmmAcStatus[ac].wmmAcAccessRequired = VOS_FALSE;
         pAdapter->hddWmmStatus.wmmAcStatus[ac].wmmAcAccessAllowed = VOS_TRUE;
      }

   }

   VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO_LOW,
             "%s: Exiting", __func__);

   return VOS_STATUS_SUCCESS;
}

/*                                                                             
                                                                     
                                                                     

                                                     
                                                                          

                                                   
                                           
                                                                             */
VOS_STATUS hdd_wmm_get_uapsd_mask( hdd_adapter_t* pAdapter,
                                   tANI_U8 *pUapsdMask )
{
   tANI_U8 uapsdMask;

   if (HDD_WMM_USER_MODE_NO_QOS == (WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->WmmMode)
   {
      //                     
      uapsdMask = 0;
   }
   else
   {
      //                            
      uapsdMask = (WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->UapsdMask;

      //                                                    
      if( (WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->InfraUapsdVoSrvIntv == 0 )
      {
         uapsdMask &= ~HDD_AC_VO;
      }

      if( (WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->InfraUapsdViSrvIntv == 0 )
      {
         uapsdMask &= ~HDD_AC_VI;
      }

      if( (WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->InfraUapsdBkSrvIntv == 0 )
      {
         uapsdMask &= ~HDD_AC_BK;
      }

      if( (WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->InfraUapsdBeSrvIntv == 0 )
      {
         uapsdMask &= ~HDD_AC_BE;
      }
   }

   //                       
   *pUapsdMask = uapsdMask;
   return VOS_STATUS_SUCCESS;
}


/*                                                                             
                                                                      
                                  

                                                     

                                              
                                                   
                                                                             */
v_BOOL_t hdd_wmm_is_active( hdd_adapter_t* pAdapter )
{
   if ((!pAdapter->hddWmmStatus.wmmQosConnection) ||
       (!pAdapter->hddWmmStatus.wmmQap))
   {
      return VOS_FALSE;
   }
   else
   {
      return VOS_TRUE;
   }
}

/*                                                                             
                                                                        
                           

                                                     
                                                           
                                                      

                                          
                                                                             */
hdd_wlan_wmm_status_e hdd_wmm_addts( hdd_adapter_t* pAdapter,
                                     v_U32_t handle,
                                     sme_QosWmmTspecInfo* pTspec )
{
   hdd_wmm_qos_context_t *pQosContext;
   hdd_wlan_wmm_status_e status = HDD_WLAN_WMM_STATUS_SETUP_SUCCESS ;
#ifndef WLAN_MDM_CODE_REDUCTION_OPT
   sme_QosStatusType smeStatus;
#endif
   v_BOOL_t found = VOS_FALSE;

   VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO_LOW,
             "%s: Entered with handle 0x%x", __func__, handle);

   //                                                      
   mutex_lock(&pAdapter->hddWmmStatus.wmmLock);
   list_for_each_entry(pQosContext,
                       &pAdapter->hddWmmStatus.wmmContextList,
                       node)
   {
      if (pQosContext->handle == handle)
      {
         found = VOS_TRUE;
         break;
      }
   }
   mutex_unlock(&pAdapter->hddWmmStatus.wmmLock);
   if (found)
   {
      //                                       
      VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_ERROR,
                "%s: Record already exists with handle 0x%x",
                __func__, handle);

      /*                                                                    */
      smeStatus = sme_QosModifyReq(WLAN_HDD_GET_HAL_CTX(pAdapter),
                                  pTspec,
                                  pQosContext->qosFlowId);

      //                                                     
      switch (smeStatus)
      {
        case SME_QOS_STATUS_MODIFY_SETUP_PENDING_RSP:
          status = HDD_WLAN_WMM_STATUS_MODIFY_PENDING;
          break;
        case SME_QOS_STATUS_MODIFY_SETUP_SUCCESS_NO_ACM_NO_APSD_RSP:
          status = HDD_WLAN_WMM_STATUS_MODIFY_SUCCESS_NO_ACM_NO_UAPSD;
          break;
        case SME_QOS_STATUS_MODIFY_SETUP_SUCCESS_APSD_SET_ALREADY:
          status = HDD_WLAN_WMM_STATUS_MODIFY_SUCCESS_NO_ACM_UAPSD_EXISTING;
          break;
        case SME_QOS_STATUS_MODIFY_SETUP_INVALID_PARAMS_RSP:
          status = HDD_WLAN_WMM_STATUS_MODIFY_FAILED_BAD_PARAM;
          break;
        case SME_QOS_STATUS_MODIFY_SETUP_FAILURE_RSP:
          status = HDD_WLAN_WMM_STATUS_MODIFY_FAILED;
          break;
        case SME_QOS_STATUS_SETUP_NOT_QOS_AP_RSP:
          status = HDD_WLAN_WMM_STATUS_SETUP_FAILED_NO_WMM;
          break;
        default:
          //                                                                   
          VOS_TRACE( VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_ERROR,
                     "%s: unexpected SME Status=%d\n", __func__, smeStatus );
          VOS_ASSERT(0);
          return HDD_WLAN_WMM_STATUS_MODIFY_FAILED;
      }

      //                                    
      pQosContext->lastStatus = status;
      return status;
   }

   pQosContext = kmalloc(sizeof(*pQosContext), GFP_KERNEL);
   if (NULL == pQosContext)
   {
      //                                              
      VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_ERROR,
                "%s: Unable to allocate QoS context", __func__);
      return HDD_WLAN_WMM_STATUS_INTERNAL_FAILURE;
   }

   //                                                             

   pQosContext->handle = handle;
   pQosContext->acType = hddWmmUpToAcMap[pTspec->ts_info.up];
   pQosContext->pAdapter = pAdapter;
   pQosContext->qosFlowId = 0;
   pQosContext->magic = HDD_WMM_CTX_MAGIC;

   VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO,
             "%s: Setting up QoS, context %p",
             __func__, pQosContext);

   mutex_lock(&pAdapter->hddWmmStatus.wmmLock);
   list_add(&pQosContext->node, &pAdapter->hddWmmStatus.wmmContextList);
   mutex_unlock(&pAdapter->hddWmmStatus.wmmLock);

#ifndef WLAN_MDM_CODE_REDUCTION_OPT
   smeStatus = sme_QosSetupReq(WLAN_HDD_GET_HAL_CTX(pAdapter),
                               pAdapter->sessionId,
                               pTspec,
                               hdd_wmm_sme_callback,
                               pQosContext,
                               pTspec->ts_info.up,
                               &pQosContext->qosFlowId);

   VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO,
             "%s: sme_QosSetupReq returned %d flowid %d",
             __func__, smeStatus, pQosContext->qosFlowId);

   //                                                     
   switch (smeStatus)
   {
   case SME_QOS_STATUS_SETUP_REQ_PENDING_RSP:
      status = HDD_WLAN_WMM_STATUS_SETUP_PENDING;
      break;
   case SME_QOS_STATUS_SETUP_SUCCESS_NO_ACM_NO_APSD_RSP:
      status = HDD_WLAN_WMM_STATUS_SETUP_SUCCESS_NO_ACM_NO_UAPSD;
      break;
   case SME_QOS_STATUS_SETUP_SUCCESS_APSD_SET_ALREADY:
      status = HDD_WLAN_WMM_STATUS_SETUP_SUCCESS_NO_ACM_UAPSD_EXISTING;
      break;
   case SME_QOS_STATUS_SETUP_SUCCESS_IND_APSD_PENDING:
      status = HDD_WLAN_WMM_STATUS_SETUP_PENDING;
      break;
   case SME_QOS_STATUS_SETUP_INVALID_PARAMS_RSP:
      hdd_wmm_free_context(pQosContext);
      return HDD_WLAN_WMM_STATUS_SETUP_FAILED_BAD_PARAM;
   case SME_QOS_STATUS_SETUP_FAILURE_RSP:
      //                                                                  
      //                                                            
      hdd_wmm_free_context(pQosContext);
      return HDD_WLAN_WMM_STATUS_SETUP_FAILED;
   case SME_QOS_STATUS_SETUP_NOT_QOS_AP_RSP:
      hdd_wmm_free_context(pQosContext);
      return HDD_WLAN_WMM_STATUS_SETUP_FAILED_NO_WMM;
   default:
      //                                                                  
      hdd_wmm_free_context(pQosContext);
      VOS_TRACE( VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_ERROR,
                 "%s: unexpected SME Status=%d\n", __func__, smeStatus );
      VOS_ASSERT(0);
      return HDD_WLAN_WMM_STATUS_SETUP_FAILED;
   }
#endif

   //                                    
   pQosContext->lastStatus = status;

   return status;
}

/*                                                                             
                                                                           
                           

                                                     
                                                           

                                          
                                                                             */
hdd_wlan_wmm_status_e hdd_wmm_delts( hdd_adapter_t* pAdapter,
                                     v_U32_t handle )
{
   hdd_wmm_qos_context_t *pQosContext;
   v_BOOL_t found = VOS_FALSE;
   WLANTL_ACEnumType acType = 0;
   v_U32_t qosFlowId = 0;
   hdd_wlan_wmm_status_e status = HDD_WLAN_WMM_STATUS_SETUP_SUCCESS ;
#ifndef WLAN_MDM_CODE_REDUCTION_OPT
   sme_QosStatusType smeStatus;
#endif

   VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO_LOW,
             "%s: Entered with handle 0x%x", __func__, handle);

   //                                         
   mutex_lock(&pAdapter->hddWmmStatus.wmmLock);
   list_for_each_entry(pQosContext,
                       &pAdapter->hddWmmStatus.wmmContextList,
                       node)
   {
      if (pQosContext->handle == handle)
      {
         found = VOS_TRUE;
         acType = pQosContext->acType;
         qosFlowId = pQosContext->qosFlowId;
         break;
      }
   }
   mutex_unlock(&pAdapter->hddWmmStatus.wmmLock);

   if (VOS_FALSE == found)
   {
      //                          
      VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO_LOW,
                "%s: handle 0x%x not found", __func__, handle);
      return HDD_WLAN_WMM_STATUS_RELEASE_FAILED_BAD_PARAM;
   }


   VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO_LOW,
             "%s: found handle 0x%x, flow %d, AC %d, context %p",
             __func__, handle, qosFlowId, acType, pQosContext);

#ifndef WLAN_MDM_CODE_REDUCTION_OPT
   smeStatus = sme_QosReleaseReq( WLAN_HDD_GET_HAL_CTX(pAdapter), qosFlowId );

   VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO_LOW,
             "%s: SME flow %d released, SME status %d",
             __func__, qosFlowId, smeStatus);

   switch(smeStatus)
   {
   case SME_QOS_STATUS_RELEASE_SUCCESS_RSP:
      //                                                             
      //                           
      pAdapter->hddWmmStatus.wmmAcStatus[acType].wmmAcTspecValid = VOS_FALSE;

      //                                           
      hdd_wmm_disable_tl_uapsd(pQosContext);

#ifdef FEATURE_WLAN_CCX
      //                             
      hdd_wmm_disable_inactivity_timer(pQosContext);
#endif
      //                              
      hdd_wmm_free_context(pQosContext);

      //                                                                     
      //                   

      return HDD_WLAN_WMM_STATUS_RELEASE_SUCCESS;

   case SME_QOS_STATUS_RELEASE_REQ_PENDING_RSP:
      //                                              
      status = HDD_WLAN_WMM_STATUS_RELEASE_PENDING;
      break;

   case SME_QOS_STATUS_RELEASE_INVALID_PARAMS_RSP:
      //                                                         
      status = HDD_WLAN_WMM_STATUS_RELEASE_FAILED_BAD_PARAM;
      break;

   case SME_QOS_STATUS_RELEASE_FAILURE_RSP:
      //                                                         
      status = HDD_WLAN_WMM_STATUS_RELEASE_FAILED;

   default:
      //                                                                    
      VOS_TRACE( VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_ERROR,
                 "%s: unexpected SME Status=%d\n", __func__, smeStatus );
      VOS_ASSERT(0);
      status = HDD_WLAN_WMM_STATUS_RELEASE_FAILED;
   }

#endif
   pQosContext->lastStatus = status;
   return status;
}

/*                                                                             
                                                                               
                                       

                                                     
                                                           

                                          
                                                                             */
hdd_wlan_wmm_status_e hdd_wmm_checkts( hdd_adapter_t* pAdapter,
                                       v_U32_t handle )
{
   hdd_wmm_qos_context_t *pQosContext;
   hdd_wlan_wmm_status_e status = HDD_WLAN_WMM_STATUS_LOST;

   VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO_LOW,
             "%s: Entered with handle 0x%x", __func__, handle);

   //                                         
   mutex_lock(&pAdapter->hddWmmStatus.wmmLock);
   list_for_each_entry(pQosContext,
                       &pAdapter->hddWmmStatus.wmmContextList,
                       node)
   {
      if (pQosContext->handle == handle)
      {
         VOS_TRACE(VOS_MODULE_ID_HDD, WMM_TRACE_LEVEL_INFO_LOW,
                   "%s: found handle 0x%x, context %p",
                   __func__, handle, pQosContext);

         status = pQosContext->lastStatus;
         break;
      }
   }
   mutex_unlock(&pAdapter->hddWmmStatus.wmmLock);
   return status;
}

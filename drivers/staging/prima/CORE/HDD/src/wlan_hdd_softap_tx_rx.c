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

/**===========================================================================
  
  \file  wlan_hdd_softap_tx_rx.c
  
  \brief Linux HDD Tx/RX APIs
         Copyright 2008 (c) Qualcomm, Incorporated.
         All Rights Reserved.
         Qualcomm Confidential and Proprietary.
  
  ==========================================================================*/

/*                                                                            
               
                                                                           */ 
#include <linux/semaphore.h>
#include <wlan_hdd_tx_rx.h>
#include <wlan_hdd_softap_tx_rx.h>
#include <wlan_hdd_dp_utils.h>
#include <wlan_qct_tl.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/etherdevice.h>
//                     
#include <vos_types.h>
#include <aniGlobal.h>
#include <halTypes.h>
#include <net/ieee80211_radiotap.h>


/*                                                                            
                                        
                                                                           */ 

/*                                                                            
                   
                                                                           */ 

/*                                                                            
                                       
                                                                           */ 
#if 0
static void hdd_softap_dump_sk_buff(struct sk_buff * skb)
{
  VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_ERROR,"%s: head = %p ", __func__, skb->head);
  //                                                                                                  
  VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_ERROR,"%s: tail = %p ", __func__, skb->tail);
  VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_ERROR,"%s: end = %p ", __func__, skb->end);
  VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_ERROR,"%s: len = %d ", __func__, skb->len);
  VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_ERROR,"%s: data_len = %d ", __func__, skb->data_len);
  VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_ERROR,"%s: mac_len = %d\n", __func__, skb->mac_len);

  VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_ERROR,"0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x ", 
     skb->data[0], skb->data[1], skb->data[2], skb->data[3], skb->data[4], 
     skb->data[5], skb->data[6], skb->data[7]); 
  VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_ERROR,"0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x \n", 
     skb->data[8], skb->data[9], skb->data[10], skb->data[11], skb->data[12],
     skb->data[13], skb->data[14], skb->data[15]); 
}
#endif

extern void hdd_set_wlan_suspend_mode(bool suspend);

/*                                                                             
                                                       
                                                            
                                                                   

                                               
                        
                                                                             */
void hdd_softap_traffic_monitor_timeout_handler( void *pUsrData )
{
   hdd_context_t *pHddCtx = (hdd_context_t *)pUsrData;
   v_TIME_t       currentTS;

   if (NULL == pHddCtx)
   {
      VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_ERROR,
              "%s: Invalid user data, context", __func__);
      return;
   }

   currentTS = vos_timer_get_system_time();
   if (pHddCtx->cfg_ini->trafficIdleTimeout <
       (currentTS - pHddCtx->traffic_monitor.lastFrameTs))
   {
      VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_INFO,
          "%s: No Data Activity calling Wlan Suspend", __func__ );
      hdd_set_wlan_suspend_mode(1);
      atomic_set(&pHddCtx->traffic_monitor.isActiveMode, 0);
   }
   else
   {
      vos_timer_start(&pHddCtx->traffic_monitor.trafficTimer,
                      pHddCtx->cfg_ini->trafficIdleTimeout);
   }

   return;
}

VOS_STATUS hdd_start_trafficMonitor( hdd_adapter_t *pAdapter )
{

    hdd_context_t *pHddCtx = WLAN_HDD_GET_CTX(pAdapter);
    VOS_STATUS status = VOS_STATUS_SUCCESS;

    status = wlan_hdd_validate_context(pHddCtx);

    if (0 != status)
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                   "%s: HDD context is not valid", __func__);
        return status;
    }

    if ((pHddCtx->cfg_ini->enableTrafficMonitor) &&
        (!pHddCtx->traffic_monitor.isInitialized))
    {
        atomic_set(&pHddCtx->traffic_monitor.isActiveMode, 1);
        vos_timer_init(&pHddCtx->traffic_monitor.trafficTimer,
                      VOS_TIMER_TYPE_SW,
                      hdd_softap_traffic_monitor_timeout_handler,
                      pHddCtx);
        vos_lock_init(&pHddCtx->traffic_monitor.trafficLock);
        pHddCtx->traffic_monitor.isInitialized = 1;
        pHddCtx->traffic_monitor.lastFrameTs   = 0;
        /*                                 
                                                      */
        VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_INFO,
                  "%s  Start Traffic Monitor Timer", __func__);
        vos_timer_start(&pHddCtx->traffic_monitor.trafficTimer,
                      pHddCtx->cfg_ini->trafficIdleTimeout);
    }
    else
    {
        VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_INFO,
                  "%s  Traffic Monitor is not Enable in ini file", __func__);
    }
    return status;
}

VOS_STATUS hdd_stop_trafficMonitor( hdd_adapter_t *pAdapter )
{
    hdd_context_t *pHddCtx = WLAN_HDD_GET_CTX(pAdapter);
    VOS_STATUS status = VOS_STATUS_SUCCESS;

    status = wlan_hdd_validate_context(pHddCtx);

    if (-ENODEV == status)
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                   "%s: HDD context is not valid", __func__);
        return status;
    }

    if (pHddCtx->traffic_monitor.isInitialized)
    {
        if (VOS_TIMER_STATE_STOPPED !=
            vos_timer_getCurrentState(&pHddCtx->traffic_monitor.trafficTimer))
        {
            VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_INFO,
                      "%s  Stop Traffic Monitor Timer", __func__);
            vos_timer_stop(&pHddCtx->traffic_monitor.trafficTimer);
        }
        VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_INFO,
                  "%s  Destroy Traffic Monitor Timer", __func__);
        vos_timer_destroy(&pHddCtx->traffic_monitor.trafficTimer);
        vos_lock_destroy(&pHddCtx->traffic_monitor.trafficLock);
        pHddCtx->traffic_monitor.isInitialized = 0;
    }
    return VOS_STATUS_SUCCESS;
}

/*                                                                             
                                                                               

                                                     
                                                                   
                                                
                                                                             */
static VOS_STATUS hdd_softap_flush_tx_queues( hdd_adapter_t *pAdapter )
{
   VOS_STATUS status = VOS_STATUS_SUCCESS;
   v_SINT_t i = -1;
   v_U8_t STAId = 0;
   hdd_list_node_t *anchor = NULL;
   skb_list_node_t *pktNode = NULL;
   struct sk_buff *skb = NULL;

   spin_lock_bh( &pAdapter->staInfo_lock );
   for (STAId = 0; STAId < WLAN_MAX_STA_COUNT; STAId++)
   {
      if (FALSE == pAdapter->aStaInfo[STAId].isUsed)
      {
         continue;
      }

      for (i = 0; i < NUM_TX_QUEUES; i ++)
      {
         spin_lock_bh(&pAdapter->aStaInfo[STAId].wmm_tx_queue[i].lock);
         while (true) 
         {
            status = hdd_list_remove_front ( &pAdapter->aStaInfo[STAId].wmm_tx_queue[i], &anchor);

            if (VOS_STATUS_E_EMPTY != status)
            {
               //                                                  
               pktNode = list_entry(anchor, skb_list_node_t, anchor);
               skb = pktNode->skb;
               ++pAdapter->stats.tx_dropped;
               ++pAdapter->hdd_stats.hddTxRxStats.txFlushed;
               ++pAdapter->hdd_stats.hddTxRxStats.txFlushedAC[i];
               kfree_skb(skb);
               continue;
            }

            //                     
            break;
         }
         pAdapter->aStaInfo[STAId].txSuspended[i] = VOS_FALSE;
         spin_unlock_bh(&pAdapter->aStaInfo[STAId].wmm_tx_queue[i].lock);
      }
      pAdapter->aStaInfo[STAId].vosLowResource = VOS_FALSE;
   }

   spin_unlock_bh( &pAdapter->staInfo_lock );

   return status;
}

/*                                                                             
                                                                                  
                                                                            
                                                                              
                             

                                                        
                                                        
  
                                                        
                                                                      
                                                                             */
int hdd_softap_hard_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
   VOS_STATUS status;
   WLANTL_ACEnumType ac = WLANTL_AC_BE;
   sme_QosWmmUpType up = SME_QOS_WMM_UP_BE;
   skb_list_node_t *pktNode = NULL;
   v_SIZE_t pktListSize = 0;
   v_BOOL_t txSuspended = VOS_FALSE;
   hdd_adapter_t *pAdapter = (hdd_adapter_t *)netdev_priv(dev);
   hdd_ap_ctx_t *pHddApCtx = WLAN_HDD_GET_AP_CTX_PTR(pAdapter);   
   vos_list_node_t *anchor = NULL;
   v_U8_t STAId = WLAN_MAX_STA_COUNT;
   //                                                   
   v_MACADDR_t *pDestMacAddress = (v_MACADDR_t*)skb->data;
   int os_status = NETDEV_TX_OK; 

   pDestMacAddress = (v_MACADDR_t*)skb->data;
   
   ++pAdapter->hdd_stats.hddTxRxStats.txXmitCalled;

   VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_INFO,
              "%s: enter", __func__);

   spin_lock_bh( &pAdapter->staInfo_lock );
   if (vos_is_macaddr_broadcast( pDestMacAddress ) || vos_is_macaddr_group(pDestMacAddress))
   {
      //                                                                                        
      //                                                                      
      STAId = pHddApCtx->uBCStaId;
      
      /*                                                                                  */
      skb->priority = SME_QOS_WMM_UP_BE;
      skb->queue_mapping = HDD_LINUX_AC_BE;

      VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_INFO_LOW,
              "%s: BC/MC packet\n", __func__);
   }
   else
   {
      STAId = *(v_U8_t *)(((v_U8_t *)(skb->data)) - 1);
      if (STAId == HDD_WLAN_INVALID_STA_ID)
      {
         VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_WARN,
                    "%s: Failed to find right station", __func__);
         ++pAdapter->stats.tx_dropped;
         ++pAdapter->hdd_stats.hddTxRxStats.txXmitDropped;
         kfree_skb(skb);
         goto xmit_done;
      }
      else if (FALSE == pAdapter->aStaInfo[STAId].isUsed )
      {
         VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_WARN,"%s: STA %d is unregistered", __func__, STAId);
         ++pAdapter->stats.tx_dropped;
         ++pAdapter->hdd_stats.hddTxRxStats.txXmitDropped;
         kfree_skb(skb);
         goto xmit_done;
      }

      if ( (WLANTL_STA_CONNECTED != pAdapter->aStaInfo[STAId].tlSTAState) && 
           (WLANTL_STA_AUTHENTICATED != pAdapter->aStaInfo[STAId].tlSTAState) )
      {
         VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_WARN,
                    "%s: Station not connected yet", __func__);
         ++pAdapter->stats.tx_dropped;
         ++pAdapter->hdd_stats.hddTxRxStats.txXmitDropped;
         kfree_skb(skb);
         goto xmit_done;
      }
      else if(WLANTL_STA_CONNECTED == pAdapter->aStaInfo[STAId].tlSTAState)
      {
        if(ntohs(skb->protocol) != HDD_ETHERTYPE_802_1_X)
        {
            VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_WARN,
                       "%s: NON-EAPOL packet in non-Authenticated state", __func__);
            ++pAdapter->stats.tx_dropped;
            ++pAdapter->hdd_stats.hddTxRxStats.txXmitDropped;
            kfree_skb(skb);
            goto xmit_done;
        }
      }
   }

   //                                                
   ac = hdd_QdiscAcToTlAC[skb->queue_mapping];
   //                                                                      
   //                               
   up = skb->priority;
   ++pAdapter->hdd_stats.hddTxRxStats.txXmitClassifiedAC[ac];

   VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_INFO,
              "%s: Classified as ac %d up %d", __func__, ac, up);

   //                                                                                       
   //                                             
    spin_lock_bh(&pAdapter->aStaInfo[STAId].wmm_tx_queue[ac].lock);
    hdd_list_size(&pAdapter->aStaInfo[STAId].wmm_tx_queue[ac], &pktListSize);
    if(pktListSize >= pAdapter->aTxQueueLimit[ac])
    {
       VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_WARN,
            "%s: station %d ac %d queue over limit %d \n", __func__, STAId, ac, pktListSize); 
       pAdapter->aStaInfo[STAId].txSuspended[ac] = VOS_TRUE;
       netif_stop_subqueue(dev, skb_get_queue_mapping(skb));
       txSuspended = VOS_TRUE;
    }

    /*                                                             
                                                                       */
   if (WLANTL_AC_BE == ac)
   {
      if (pAdapter->aStaInfo[STAId].wmm_tx_queue[ac].count >= HDD_TX_QUEUE_LOW_WATER_MARK)
      {
          VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_WARN,
                     "%s: TX queue for Best Effort AC is 3/4th full", __func__);
          pAdapter->aStaInfo[STAId].vosLowResource = VOS_TRUE;
      }
      else
      {
          pAdapter->aStaInfo[STAId].vosLowResource = VOS_FALSE;
      }
   }
   spin_unlock_bh(&pAdapter->aStaInfo[STAId].wmm_tx_queue[ac].lock);

   if (VOS_TRUE == txSuspended)
   {
       VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_WARN, 
                  "%s: TX queue full for AC=%d Disable OS TX queue", 
                  __func__, ac );
      os_status = NETDEV_TX_BUSY;   
      goto xmit_done;
   }

   //                                                       
   pktNode = (skb_list_node_t *)&skb->cb;

   //                                     
   pktNode->skb = skb;

   //                                         
   pktNode->userPriority = up;

   INIT_LIST_HEAD(&pktNode->anchor);

   spin_lock_bh(&pAdapter->aStaInfo[STAId].wmm_tx_queue[ac].lock);
   status = hdd_list_insert_back_size(&pAdapter->aStaInfo[STAId].wmm_tx_queue[ac], &pktNode->anchor, &pktListSize );
   spin_unlock_bh(&pAdapter->aStaInfo[STAId].wmm_tx_queue[ac].lock);

   if ( !VOS_IS_STATUS_SUCCESS( status ) )
   {
      VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_WARN,"%s:Insert Tx queue failed. Pkt dropped", __func__);
      ++pAdapter->hdd_stats.hddTxRxStats.txXmitDropped;
      ++pAdapter->hdd_stats.hddTxRxStats.txXmitDroppedAC[ac];
      ++pAdapter->stats.tx_dropped;
      kfree_skb(skb);
      goto xmit_done;
   }

   ++pAdapter->hdd_stats.hddTxRxStats.txXmitQueued;
   ++pAdapter->hdd_stats.hddTxRxStats.txXmitQueuedAC[ac];
   ++pAdapter->hdd_stats.hddTxRxStats.pkt_tx_count;

   if (1 == pktListSize)
   {
      //                                                
      status = WLANTL_STAPktPending( (WLAN_HDD_GET_CTX(pAdapter))->pvosContext, STAId, ac );      

      if ( !VOS_IS_STATUS_SUCCESS( status ) )
      {
         VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_WARN, "%s: Failed to signal TL for AC=%d STAId =%d", 
                    __func__, ac, STAId );

         //                                                                                                               
         //                                                                                        
         spin_lock_bh(&pAdapter->aStaInfo[STAId].wmm_tx_queue[ac].lock);
         status = hdd_list_remove_back( &pAdapter->aStaInfo[STAId].wmm_tx_queue[ac], &anchor);
         spin_unlock_bh(&pAdapter->aStaInfo[STAId].wmm_tx_queue[ac].lock);
         ++pAdapter->stats.tx_dropped;
         ++pAdapter->hdd_stats.hddTxRxStats.txXmitDropped;
         ++pAdapter->hdd_stats.hddTxRxStats.txXmitDroppedAC[ac];
         kfree_skb(skb);
         goto xmit_done;
      }
   }
   dev->trans_start = jiffies;

   VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_INFO_LOW, "%s: exit \n", __func__);

xmit_done:
   spin_unlock_bh( &pAdapter->staInfo_lock );
   return os_status;
}

/*                                                                             
                                                                                                                      

                                                    
                                                        
                                                          
                                       
  
                                                        
                                                                      
                                                                             */
VOS_STATUS hdd_softap_sta_2_sta_xmit(struct sk_buff *skb, 
                                      struct net_device *dev,
                                      v_U8_t STAId, 
                                      v_U8_t up)
{
   VOS_STATUS status = VOS_STATUS_SUCCESS; 
   skb_list_node_t *pktNode = NULL;
   v_SIZE_t pktListSize = 0;
   hdd_adapter_t *pAdapter = (hdd_adapter_t *)netdev_priv(dev);
   v_U8_t ac;
   vos_list_node_t *anchor = NULL;

   ++pAdapter->hdd_stats.hddTxRxStats.txXmitCalled;

   VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_INFO,
              "%s: enter", __func__);

   spin_lock_bh( &pAdapter->staInfo_lock );
   if ( FALSE == pAdapter->aStaInfo[STAId].isUsed )
   {
      VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_WARN,
                 "%s: STA %d is unregistered", __func__, STAId );
      kfree_skb(skb);
      status = VOS_STATUS_E_FAILURE;
      goto xmit_end;
   }

   /*                                                                                   */
   if ( !pAdapter->aStaInfo[STAId].isQosEnabled )
       up = SME_QOS_WMM_UP_BE;

   ac = hddWmmUpToAcMap[up];
   ++pAdapter->hdd_stats.hddTxRxStats.txXmitClassifiedAC[ac];
   VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_INFO,
              "%s: Classified as ac %d up %d", __func__, ac, up);

   skb->queue_mapping = hddLinuxUpToAcMap[up];

   //                                                       
   pktNode = (skb_list_node_t *)&skb->cb;

   //                                     
   pktNode->skb = skb;

   //                                         
   pktNode->userPriority = up;

   INIT_LIST_HEAD(&pktNode->anchor);

   spin_lock_bh(&pAdapter->aStaInfo[STAId].wmm_tx_queue[ac].lock);
   hdd_list_size(&pAdapter->aStaInfo[STAId].wmm_tx_queue[ac], &pktListSize);
   if(pAdapter->aStaInfo[STAId].txSuspended[ac] ||
       pktListSize >= pAdapter->aTxQueueLimit[ac])
   {
       VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_WARN,
            "%s: station %d ac %d queue over limit %d \n", __func__, STAId, ac, pktListSize); 
       /*                                                                     
                                                                     
                                                
        */
       kfree_skb(skb);
       spin_unlock_bh(&pAdapter->aStaInfo[STAId].wmm_tx_queue[ac].lock);
       status = VOS_STATUS_E_FAILURE;
       goto xmit_end;
   }
   status = hdd_list_insert_back_size(&pAdapter->aStaInfo[STAId].wmm_tx_queue[ac], &pktNode->anchor, &pktListSize );
   spin_unlock_bh(&pAdapter->aStaInfo[STAId].wmm_tx_queue[ac].lock);

   if ( !VOS_IS_STATUS_SUCCESS( status ) )
   {
      VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_WARN,"%s:Insert Tx queue failed. Pkt dropped", __func__);
      ++pAdapter->hdd_stats.hddTxRxStats.txXmitDropped;
      ++pAdapter->hdd_stats.hddTxRxStats.txXmitDroppedAC[ac];
      ++pAdapter->stats.tx_dropped;
      kfree_skb(skb);
      status = VOS_STATUS_E_FAILURE;
      goto xmit_end;
   }

   ++pAdapter->hdd_stats.hddTxRxStats.txXmitQueued;
   ++pAdapter->hdd_stats.hddTxRxStats.txXmitQueuedAC[ac];

   if (1 == pktListSize)
   {
      //                                                
      //                                                                                                   
      status = WLANTL_STAPktPending( (WLAN_HDD_GET_CTX(pAdapter))->pvosContext, STAId, ac );      

      if ( !VOS_IS_STATUS_SUCCESS( status ) )
      {
         VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_WARN, "%s: Failed to signal TL for AC=%d STAId =%d", 
                    __func__, ac, STAId );

         //                                                                                                               
         //                                                                                        
         spin_lock_bh(&pAdapter->aStaInfo[STAId].wmm_tx_queue[ac].lock);
         status = hdd_list_remove_back( &pAdapter->aStaInfo[STAId].wmm_tx_queue[ac], &anchor);
         spin_unlock_bh(&pAdapter->aStaInfo[STAId].wmm_tx_queue[ac].lock);
         ++pAdapter->stats.tx_dropped;
         ++pAdapter->hdd_stats.hddTxRxStats.txXmitDropped;
         ++pAdapter->hdd_stats.hddTxRxStats.txXmitDroppedAC[ac];
         kfree_skb(skb);
         status = VOS_STATUS_E_FAILURE;
         goto xmit_end;
      }
   }

   VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_INFO_LOW, "%s: exit \n", __func__);

xmit_end:
   spin_unlock_bh( &pAdapter->staInfo_lock );
   return status;
}

/*                                                                             
                                                                        
                                                               
                                                                   

                                                   
                   
                                                                             */
void hdd_softap_tx_timeout(struct net_device *dev)
{
   VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_ERROR,
      "%s: Transmission timeout occurred", __func__);
   //                                                                        
   //                                                                       
   //                                                                          
   //                         
} 


/*                                                                             
                                                                        
                        

                                                        
  
                                                         
                                                                             */
struct net_device_stats* hdd_softap_stats(struct net_device *dev)
{
   hdd_adapter_t* priv = netdev_priv(dev);
   return &priv->stats;
}


/*                                                                             
                                                                    
                

                                                     
                                                                   
                                                
                                                                             */
VOS_STATUS hdd_softap_init_tx_rx( hdd_adapter_t *pAdapter )
{
   VOS_STATUS status = VOS_STATUS_SUCCESS;
   v_SINT_t i = -1;
   v_SIZE_t size = 0;

   v_U8_t STAId = 0;

   v_U8_t pACWeights[] = {
                           HDD_SOFTAP_BK_WEIGHT_DEFAULT, 
                           HDD_SOFTAP_BE_WEIGHT_DEFAULT, 
                           HDD_SOFTAP_VI_WEIGHT_DEFAULT, 
                           HDD_SOFTAP_VO_WEIGHT_DEFAULT
                         };

   pAdapter->isVosOutOfResource = VOS_FALSE;
   pAdapter->isVosLowResource = VOS_FALSE;

   vos_mem_zero(&pAdapter->stats, sizeof(struct net_device_stats));

   while (++i != NUM_TX_QUEUES) 
      hdd_list_init( &pAdapter->wmm_tx_queue[i], HDD_TX_QUEUE_MAX_LEN);

   /*                                                 */
   vos_pkt_get_available_buffer_pool (VOS_PKT_TYPE_TX_802_3_DATA, &size);

   pAdapter->aTxQueueLimit[WLANTL_AC_BK] = HDD_SOFTAP_TX_BK_QUEUE_MAX_LEN;
   pAdapter->aTxQueueLimit[WLANTL_AC_BE] = HDD_SOFTAP_TX_BE_QUEUE_MAX_LEN;
   pAdapter->aTxQueueLimit[WLANTL_AC_VI] = HDD_SOFTAP_TX_VI_QUEUE_MAX_LEN;
   pAdapter->aTxQueueLimit[WLANTL_AC_VO] = HDD_SOFTAP_TX_VO_QUEUE_MAX_LEN;

   spin_lock_init( &pAdapter->staInfo_lock );

   for (STAId = 0; STAId < WLAN_MAX_STA_COUNT; STAId++)
   {
      vos_mem_zero(&pAdapter->aStaInfo[STAId], sizeof(hdd_station_info_t));
      for (i = 0; i < NUM_TX_QUEUES; i ++)
      {
         hdd_list_init(&pAdapter->aStaInfo[STAId].wmm_tx_queue[i], HDD_TX_QUEUE_MAX_LEN);
      }
   }

   /*                                                             */
   WLANTL_SetACWeights((WLAN_HDD_GET_CTX(pAdapter))->pvosContext, pACWeights);

   if (VOS_STATUS_SUCCESS != hdd_start_trafficMonitor(pAdapter))
   {
       VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_ERROR,
          "%s: failed to start Traffic Monito timer ", __func__ );
       return VOS_STATUS_E_INVAL;
   }
   return status;
}

/*                                                                             
                                                                      
                

                                                     
                                                                   
                                                
                                                                             */
VOS_STATUS hdd_softap_deinit_tx_rx( hdd_adapter_t *pAdapter )
{
   VOS_STATUS status = VOS_STATUS_SUCCESS;

   if (VOS_STATUS_SUCCESS != hdd_stop_trafficMonitor(pAdapter))
   {
       VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_ERROR,
                 "%s: Fail to Stop Traffic Monito timer", __func__ );
       return VOS_STATUS_E_INVAL;
   }

   status = hdd_softap_flush_tx_queues(pAdapter);

   return status;
}

/*                                                                             
                                                                                                

                                                   
                                              
                                                                   
                                                
                                                                             */
static VOS_STATUS hdd_softap_flush_tx_queues_sta( hdd_adapter_t *pAdapter, v_U8_t STAId )
{
   VOS_STATUS status = VOS_STATUS_SUCCESS;
   v_U8_t i = -1;

   hdd_list_node_t *anchor = NULL;

   skb_list_node_t *pktNode = NULL;
   struct sk_buff *skb = NULL;

   if (FALSE == pAdapter->aStaInfo[STAId].isUsed)
   {
      return status;
   }

   for (i = 0; i < NUM_TX_QUEUES; i ++)
   {
      spin_lock_bh(&pAdapter->aStaInfo[STAId].wmm_tx_queue[i].lock);
      while (true) 
      {
         status = hdd_list_remove_front ( &pAdapter->aStaInfo[STAId].wmm_tx_queue[i], &anchor);
         if (VOS_STATUS_E_EMPTY != status)
         {
            //                                                  
            pktNode = list_entry(anchor, skb_list_node_t, anchor);
            skb = pktNode->skb;
            ++pAdapter->stats.tx_dropped;
            ++pAdapter->hdd_stats.hddTxRxStats.txFlushed;
            ++pAdapter->hdd_stats.hddTxRxStats.txFlushedAC[i];
            kfree_skb(skb);
            continue;
         }

         //                     
         break;
      }
      spin_unlock_bh(&pAdapter->aStaInfo[STAId].wmm_tx_queue[i].lock);
   }

   return status;
}

/*                                                                             
                                                                                     
                

                                                   
                                             
                                                                        
                                                                   
                                                
                                                                             */
VOS_STATUS hdd_softap_init_tx_rx_sta( hdd_adapter_t *pAdapter, v_U8_t STAId, v_MACADDR_t *pmacAddrSTA)
{
   v_U8_t i = 0;
   spin_lock_bh( &pAdapter->staInfo_lock );
   if (pAdapter->aStaInfo[STAId].isUsed)
   {
      VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_ERROR, "%s: Reinit station %d", __func__, STAId );
      spin_unlock_bh( &pAdapter->staInfo_lock );
      return VOS_STATUS_E_FAILURE;
   }

   vos_mem_zero(&pAdapter->aStaInfo[STAId], sizeof(hdd_station_info_t));
   for (i = 0; i < NUM_TX_QUEUES; i ++)
   {
      hdd_list_init(&pAdapter->aStaInfo[STAId].wmm_tx_queue[i], HDD_TX_QUEUE_MAX_LEN);
   }

   pAdapter->aStaInfo[STAId].isUsed = TRUE;
   pAdapter->aStaInfo[STAId].isDeauthInProgress = FALSE;
   vos_copy_macaddr( &pAdapter->aStaInfo[STAId].macAddrSTA, pmacAddrSTA);

   spin_unlock_bh( &pAdapter->staInfo_lock );
   return VOS_STATUS_SUCCESS;
}

/*                                                                             
                                                                                        
                

                                                   
                                              
                                                                   
                                                
                                                                             */
VOS_STATUS hdd_softap_deinit_tx_rx_sta ( hdd_adapter_t *pAdapter, v_U8_t STAId )
{
   VOS_STATUS status = VOS_STATUS_SUCCESS;
   v_U8_t ac;
   /*                                             */
   v_BOOL_t txSuspended[NUM_TX_QUEUES];
   v_U8_t tlAC;
   hdd_hostapd_state_t *pHostapdState;
   v_U8_t i;

   pHostapdState = WLAN_HDD_GET_HOSTAP_STATE_PTR(pAdapter);

   spin_lock_bh( &pAdapter->staInfo_lock );
   if (FALSE == pAdapter->aStaInfo[STAId].isUsed)
   {
      VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_ERROR,
                 "%s: Deinit station not inited %d", __func__, STAId );
      spin_unlock_bh( &pAdapter->staInfo_lock );
      return VOS_STATUS_E_FAILURE;
   }

   status = hdd_softap_flush_tx_queues_sta(pAdapter, STAId);

   pAdapter->aStaInfo[STAId].isUsed = FALSE;
   pAdapter->aStaInfo[STAId].isDeauthInProgress = FALSE;

   /*                                                             
                                                                    
                                                                    
                                                                     
                                                                   
                                           */

   if (BSS_START == pHostapdState->bssState)
   {
      for (ac = HDD_LINUX_AC_VO; ac <= HDD_LINUX_AC_BK; ac++)
      {
         tlAC = hdd_QdiscAcToTlAC[ac];
         txSuspended[ac] = pAdapter->aStaInfo[STAId].txSuspended[tlAC];
      }
   }
   vos_mem_zero(&pAdapter->aStaInfo[STAId], sizeof(hdd_station_info_t));

   /*                                                             
                          
    */
   for (i = 0; i < NUM_TX_QUEUES; i ++)
   {
      hdd_list_init(&pAdapter->aStaInfo[STAId].wmm_tx_queue[i],
                    HDD_TX_QUEUE_MAX_LEN);
   }

   if (BSS_START == pHostapdState->bssState)
   {
      for (ac = HDD_LINUX_AC_VO; ac <= HDD_LINUX_AC_BK; ac++)
      {
         if (txSuspended[ac])
         {
            VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_INFO,
                       "%s: TX queue re-enabled", __func__);
            netif_wake_subqueue(pAdapter->dev, ac);
         }
      }
   }

   spin_unlock_bh( &pAdapter->staInfo_lock );
   return status;
}

/*                                                                             
                                                                              
                

                                                     
                                                                   
                                                
                                                                             */
VOS_STATUS hdd_softap_disconnect_tx_rx( hdd_adapter_t *pAdapter )
{
   return hdd_softap_flush_tx_queues(pAdapter);
}

/*                                                                             
                                                                       
                                                               
                                                                  

                                                      
                                                                    
                                                        

                                                                       
                                                    
                                                                             */
VOS_STATUS hdd_softap_tx_complete_cbk( v_VOID_t *vosContext, 
                                vos_pkt_t *pVosPacket, 
                                VOS_STATUS vosStatusIn )
{
   VOS_STATUS status = VOS_STATUS_SUCCESS;
   hdd_adapter_t *pAdapter = NULL;   
   void* pOsPkt = NULL;
   
   if( ( NULL == vosContext ) || ( NULL == pVosPacket )  )
   {
      VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_ERROR,"%s: Null params being passed", __func__);
      return VOS_STATUS_E_FAILURE; 
   }

   //                        
   status = vos_pkt_get_os_packet( pVosPacket, &pOsPkt, VOS_TRUE );
   if(!VOS_IS_STATUS_SUCCESS( status ))
   {
      //                                                              
      VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_ERROR,"%s: Failure extracting skb from vos pkt", __func__);
      vos_pkt_return_packet( pVosPacket );
      return VOS_STATUS_E_FAILURE;
   }

   //                        
   pAdapter = (hdd_adapter_t *)netdev_priv(((struct sk_buff *)pOsPkt)->dev);
   if(pAdapter == NULL)
   {
      VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_ERROR,"%s: HDD adapter context is Null", __func__);
   }
   else
   {
      ++pAdapter->hdd_stats.hddTxRxStats.txCompleted;
   }

   kfree_skb((struct sk_buff *)pOsPkt); 

   //                                
   status = vos_pkt_return_packet( pVosPacket );
   if(!VOS_IS_STATUS_SUCCESS( status ))
   {
      VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_ERROR,"%s: Could not return VOS packet to the pool", __func__);
   }

   return status;
}


/*                                                                             
                                                                               
                                  

                                                     
                                                                     
                                                            
                                                                  
                                                               
  
                                                                    
                                                                       
                                                    
                                                                             */
VOS_STATUS hdd_softap_tx_fetch_packet_cbk( v_VOID_t *vosContext,
                                    v_U8_t *pStaId,
                                    WLANTL_ACEnumType  ac,
                                    vos_pkt_t **ppVosPacket,
                                    WLANTL_MetaInfoType *pPktMetaInfo )
{
   VOS_STATUS status = VOS_STATUS_E_FAILURE;
   hdd_adapter_t *pAdapter = NULL;
   hdd_list_node_t *anchor = NULL;
   skb_list_node_t *pktNode = NULL;
   struct sk_buff *skb = NULL;
   vos_pkt_t *pVosPacket = NULL;
   v_MACADDR_t* pDestMacAddress = NULL;
   v_TIME_t timestamp;
   v_SIZE_t size = 0;
   v_U8_t STAId = WLAN_MAX_STA_COUNT;   
   hdd_context_t *pHddCtx = NULL;

   //                      
   if ( ( NULL == vosContext ) || 
        ( NULL == pStaId ) || 
        ( NULL == ppVosPacket ) ||
        ( NULL == pPktMetaInfo ) )
   {
      VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_ERROR,"%s: Null Params being passed", __func__);
      return VOS_STATUS_E_FAILURE;
   }
    
   //                    
   pHddCtx = (hdd_context_t *)vos_get_context( VOS_MODULE_ID_HDD, vosContext );
   if ( NULL == pHddCtx )
   {
      VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_ERROR,"%s: HDD adapter context is Null", __func__);
      return VOS_STATUS_E_FAILURE;
   }

   STAId = *pStaId;
   if (STAId >= WLAN_MAX_STA_COUNT)
   {
      VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_ERROR,
                 "%s: Invalid STAId %d passed by TL", __func__, STAId);
      return VOS_STATUS_E_FAILURE;
   }

   pAdapter = pHddCtx->sta_to_adapter[STAId];
   if ((NULL == pAdapter) || (WLAN_HDD_ADAPTER_MAGIC != pAdapter->magic))
   {
      VOS_ASSERT(0);
      return VOS_STATUS_E_FAILURE;
   }
 
   if (FALSE == pAdapter->aStaInfo[STAId].isUsed )
   {
      VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_ERROR,
                 "%s: Unregistered STAId %d passed by TL", __func__, STAId);
      return VOS_STATUS_E_FAILURE;
   }

   /*                 */
   if ( pHddCtx->cfg_ini->enableTrafficMonitor )
   {
      pHddCtx->traffic_monitor.lastFrameTs = vos_timer_get_system_time();
      if ( !atomic_read(&pHddCtx->traffic_monitor.isActiveMode) )
      {
         vos_lock_acquire(&pHddCtx->traffic_monitor.trafficLock);
         /*                  
                                                                       */
         if ( !atomic_read(&pHddCtx->traffic_monitor.isActiveMode) )
         {
            hdd_set_wlan_suspend_mode(0);
            vos_timer_start(&pHddCtx->traffic_monitor.trafficTimer,
                            pHddCtx->cfg_ini->trafficIdleTimeout);
            atomic_set(&pHddCtx->traffic_monitor.isActiveMode, 1);
         }
         vos_lock_release(&pHddCtx->traffic_monitor.trafficLock);
      }
   }

   ++pAdapter->hdd_stats.hddTxRxStats.txFetched;

   *ppVosPacket = NULL;

   //                                        
   if( ac > WLANTL_MAX_AC || ac < 0)
   {
      VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_ERROR,
                 "%s: Invalid AC %d passed by TL", __func__, ac);
      return VOS_STATUS_E_FAILURE;
   }

   ++pAdapter->hdd_stats.hddTxRxStats.txFetchedAC[ac];

   VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_INFO,
              "%s: AC %d passed by TL", __func__, ac);

   //                                                                                             
   //                                                     
   status = vos_pkt_wrap_data_packet( &pVosPacket, 
                                      VOS_PKT_TYPE_TX_802_3_DATA, 
                                      NULL, //                          
                                      hdd_softap_tx_low_resource_cbk, 
                                      pAdapter );

   if (status == VOS_STATUS_E_ALREADY || status == VOS_STATUS_E_RESOURCES)
   {
      //                                           
      pAdapter->isVosOutOfResource = VOS_TRUE;
      ++pAdapter->hdd_stats.hddTxRxStats.txFetchLowResources;
      VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_WARN,
                 "%s: VOSS in Low Resource scenario", __func__);
      //                                                                                     
      return VOS_STATUS_E_FAILURE;
   }

   /*                                                                                                    
                               
   */
   spin_lock_bh(&pAdapter->aStaInfo[STAId].wmm_tx_queue[ac].lock);
   hdd_list_size(&pAdapter->aStaInfo[STAId].wmm_tx_queue[ac], &size);

   if (0 == size)
   {
      spin_unlock_bh(&pAdapter->aStaInfo[STAId].wmm_tx_queue[ac].lock);
      vos_pkt_return_packet(pVosPacket);
      return VOS_STATUS_E_EMPTY;
   }

   status = hdd_list_remove_front( &pAdapter->aStaInfo[STAId].wmm_tx_queue[ac], &anchor );
   spin_unlock_bh(&pAdapter->aStaInfo[STAId].wmm_tx_queue[ac].lock);

   VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_INFO,
                       "%s: AC %d has packets pending", __func__, ac);

   if(VOS_STATUS_SUCCESS == status)
   {
      //                                                  
      pktNode = list_entry(anchor, skb_list_node_t, anchor);
      skb = pktNode->skb;
   }
   else
   {
      ++pAdapter->hdd_stats.hddTxRxStats.txFetchDequeueError;
      VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_ERROR,
                 "%s: Error in de-queuing skb from Tx queue status = %d",
                 __func__, status );
      vos_pkt_return_packet(pVosPacket);
      return VOS_STATUS_E_FAILURE;
   }

   //                         
   status = vos_pkt_set_os_packet( pVosPacket, skb );
   if (status != VOS_STATUS_SUCCESS)
   {
      VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_ERROR,
                 "%s: Error attaching skb", __func__);
      vos_pkt_return_packet(pVosPacket);
      ++pAdapter->stats.tx_dropped;
      ++pAdapter->hdd_stats.hddTxRxStats.txFetchDequeueError;
      kfree_skb(skb);
      return VOS_STATUS_E_FAILURE;
   }

   //                                        
   if(pVosPacket == NULL)
   {
      VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_ERROR,
                 "%s: VOS packet returned by VOSS is NULL", __func__);
      ++pAdapter->stats.tx_dropped;
      ++pAdapter->hdd_stats.hddTxRxStats.txFetchDequeueError;
      kfree_skb(skb);
      return VOS_STATUS_E_FAILURE;
   }

   //                        
   *ppVosPacket = pVosPacket;

   //                                          
   //                                                                 
   vos_pkt_get_timestamp( pVosPacket, &timestamp );
   pPktMetaInfo->usTimeStamp = (v_U16_t)timestamp;
   if ( 1 < size )
   {
       pPktMetaInfo->bMorePackets = 1; //                            
   }
   else
   {
       pPktMetaInfo->bMorePackets = 0;
   }

   pPktMetaInfo->ucIsEapol = 0;

   if(pAdapter->aStaInfo[STAId].tlSTAState != WLANTL_STA_AUTHENTICATED) 
   {
      if (TRUE == hdd_IsEAPOLPacket( pVosPacket ))
      {
         VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_INFO_HIGH,
                    "%s: VOS packet is EAPOL packet", __func__);
         pPktMetaInfo->ucIsEapol = 1;
      }
   }
 
//                                                      
   {
      pPktMetaInfo->ucUP = pktNode->userPriority;
      pPktMetaInfo->ucTID = pPktMetaInfo->ucUP;
   }

   pPktMetaInfo->ucType = 0;          //                             
   //                                                   
   pDestMacAddress = (v_MACADDR_t*)skb->data;

   //                                          
   //                                                                
   pPktMetaInfo->ucDisableFrmXtl = 0;
   pPktMetaInfo->ucBcast = vos_is_macaddr_broadcast( pDestMacAddress ) ? 1 : 0;
   pPktMetaInfo->ucMcast = vos_is_macaddr_group( pDestMacAddress ) ? 1 : 0;

   if ( (pAdapter->aStaInfo[STAId].txSuspended[ac]) &&
        (size <= ((pAdapter->aTxQueueLimit[ac]*3)/4) ))
   {
      VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_INFO,
                 "%s: TX queue re-enabled", __func__);
      pAdapter->aStaInfo[STAId].txSuspended[ac] = VOS_FALSE;
      netif_wake_subqueue(pAdapter->dev, skb_get_queue_mapping(skb));
   }    

   //                                                              
   //                                                             
   //                                                             
   //                                                           
   //                                                              
   //                                                          
   //                                                              
   //                                                             
   //                 
   pAdapter->stats.tx_bytes += skb->len;
   ++pAdapter->stats.tx_packets;
   ++pAdapter->hdd_stats.hddTxRxStats.txFetchDequeued;
   ++pAdapter->hdd_stats.hddTxRxStats.txFetchDequeuedAC[ac];

   VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_INFO,
              "%s: Valid VOS PKT returned to TL", __func__);

   return status;
}


/*                                                                             
                                                                             
                                                                          
                                                                     
            

                                                  
                                                                       
  
                                                                      
                                                  
                                                                               */
VOS_STATUS hdd_softap_tx_low_resource_cbk( vos_pkt_t *pVosPacket, 
                                    v_VOID_t *userData )
{
   VOS_STATUS status;
   v_SINT_t i = 0;
   v_SIZE_t size = 0;
   hdd_adapter_t* pAdapter = (hdd_adapter_t *)userData;
   v_U8_t STAId = WLAN_MAX_STA_COUNT;
 
   if(pAdapter == NULL)
   {
      VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_ERROR,"%s: HDD adapter context is Null", __func__);
      return VOS_STATUS_E_FAILURE;
   }

   //                                                                                
   //                                                                               
   //                                                                          
   vos_pkt_return_packet( pVosPacket );

   pAdapter->isVosOutOfResource = VOS_FALSE;
   
   //                                                                   
   //                                                              
   //                 
   for (STAId = 0; STAId < WLAN_MAX_STA_COUNT; STAId++)
   {
      if ((pAdapter->aStaInfo[STAId].tlSTAState == WLANTL_STA_AUTHENTICATED) ||
           (pAdapter->aStaInfo[STAId].tlSTAState == WLANTL_STA_CONNECTED))
      {
         for( i=NUM_TX_QUEUES-1; i>=0; --i )
         {
            size = 0;
            hdd_list_size(&pAdapter->aStaInfo[STAId].wmm_tx_queue[i], &size);
            if ( size > 0 )
            {
               status = WLANTL_STAPktPending( (WLAN_HDD_GET_CTX(pAdapter))->pvosContext, 
                                        STAId, 
                                        (WLANTL_ACEnumType)i );
               if( !VOS_IS_STATUS_SUCCESS( status ) )
               {
                  VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_ERROR,"%s: Failure in indicating pkt to TL for ac=%d", __func__,i); 
               }
            }
         }
      }
   } 
   return VOS_STATUS_SUCCESS;
}


/*                                                                             
                                                                          
                                                                   
                                

                                                        
                                                           
                                                           
                                                                             

                                                                           
                                                       
                                                                             */
VOS_STATUS hdd_softap_rx_packet_cbk( v_VOID_t *vosContext, 
                              vos_pkt_t *pVosPacketChain,
                              v_U8_t staId,
                              WLANTL_RxMetaInfoType* pRxMetaInfo )
{
   hdd_adapter_t *pAdapter = NULL;
   VOS_STATUS status = VOS_STATUS_E_FAILURE;
   int rxstat;
   struct sk_buff *skb = NULL;
   vos_pkt_t* pVosPacket;
   vos_pkt_t* pNextVosPacket;   
   hdd_context_t *pHddCtx = NULL;   

   //                      
   if ( ( NULL == vosContext ) || 
        ( NULL == pVosPacketChain ) ||
        ( NULL == pRxMetaInfo ) )
   {
      VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_ERROR,"%s: Null params being passed", __func__);
      return VOS_STATUS_E_FAILURE;
   }
   
   pHddCtx = (hdd_context_t *)vos_get_context( VOS_MODULE_ID_HDD, vosContext );
   if ( NULL == pHddCtx )
   {
      VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_ERROR,"%s: HDD adapter context is Null", __func__);
      return VOS_STATUS_E_FAILURE;
   }

   pAdapter = pHddCtx->sta_to_adapter[staId];
   if( NULL == pAdapter )
   {
      VOS_ASSERT(0);
      return VOS_STATUS_E_FAILURE;
   }

   /*                 */
   if ( pHddCtx->cfg_ini->enableTrafficMonitor )
   {
      pHddCtx->traffic_monitor.lastFrameTs = vos_timer_get_system_time();
      if ( !atomic_read(&pHddCtx->traffic_monitor.isActiveMode) )
      {
         vos_lock_acquire(&pHddCtx->traffic_monitor.trafficLock);
         /*                  
                                                                       */
         if ( !atomic_read(&pHddCtx->traffic_monitor.isActiveMode) )
         {
            hdd_set_wlan_suspend_mode(0);
            vos_timer_start(&pHddCtx->traffic_monitor.trafficTimer,
                            pHddCtx->cfg_ini->trafficIdleTimeout);
            atomic_set(&pHddCtx->traffic_monitor.isActiveMode, 1);
         }
         vos_lock_release(&pHddCtx->traffic_monitor.trafficLock);
      }
   }

   ++pAdapter->hdd_stats.hddTxRxStats.rxChains;

   //                                       
   pVosPacket = pVosPacketChain;
   do
   {
      //                                                
      //                                                                   
      status = vos_pkt_walk_packet_chain( pVosPacket, &pNextVosPacket, VOS_FALSE);

      //                                                  
      if (!((status == VOS_STATUS_SUCCESS) || (status == VOS_STATUS_E_EMPTY)))
      {
         ++pAdapter->hdd_stats.hddTxRxStats.rxDropped;
         VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_ERROR,"%s: Failure walking packet chain", __func__);
         return VOS_STATUS_E_FAILURE;
      }

      //                             
      //                                                     
      status = vos_pkt_get_os_packet( pVosPacket, (v_VOID_t **)&skb, VOS_TRUE );
      if(!VOS_IS_STATUS_SUCCESS( status ))
      {
         ++pAdapter->hdd_stats.hddTxRxStats.rxDropped;
         VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_ERROR,"%s: Failure extracting skb from vos pkt", __func__);
         return VOS_STATUS_E_FAILURE;
      }
   
      //                             

      skb->dev = pAdapter->dev;
      
      if(skb->dev == NULL) {
  
          hddLog(VOS_TRACE_LEVEL_FATAL, "ERROR!!Invalid netdevice\n");
          return VOS_STATUS_E_FAILURE;
      }
      ++pAdapter->hdd_stats.hddTxRxStats.rxPackets;
      ++pAdapter->stats.rx_packets;
      pAdapter->stats.rx_bytes += skb->len;

      if (WLAN_RX_BCMC_STA_ID == pRxMetaInfo->ucDesSTAId)
      {
         //                                         
         struct sk_buff *pSkbCopy;
         hdd_ap_ctx_t *pHddApCtx;

         pHddApCtx = WLAN_HDD_GET_AP_CTX_PTR(pAdapter);
         if (!(pHddApCtx->apDisableIntraBssFwd))
         {
             pSkbCopy = skb_copy(skb, GFP_ATOMIC);
             if (pSkbCopy)
             {
               hdd_softap_sta_2_sta_xmit(pSkbCopy, pSkbCopy->dev,
                          pHddApCtx->uBCStaId, (pRxMetaInfo->ucUP));
             }
         }
         else
         {
             VOS_TRACE(VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_ERROR,
                      "%s: skb allocation fails", __func__);
         }
 

      } //                              

      if ((WLAN_RX_BCMC_STA_ID == pRxMetaInfo->ucDesSTAId) || 
          (WLAN_RX_SAP_SELF_STA_ID == pRxMetaInfo->ucDesSTAId))
      {
         VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_INFO_LOW,
                          "%s: send one packet to kernel \n", __func__);

         skb->protocol = eth_type_trans(skb, skb->dev);
         skb->ip_summed = CHECKSUM_NONE;
#ifdef WLAN_OPEN_SOURCE
#ifdef WLAN_FEATURE_HOLD_RX_WAKELOCK
         wake_lock_timeout(&pHddCtx->rx_wake_lock, msecs_to_jiffies(HDD_WAKE_LOCK_DURATION));
#endif
#endif
         rxstat = netif_rx_ni(skb);
         if (NET_RX_SUCCESS == rxstat)
         {
            ++pAdapter->hdd_stats.hddTxRxStats.rxDelivered;
            ++pAdapter->hdd_stats.hddTxRxStats.pkt_rx_count;
         }
         else
         {
            ++pAdapter->hdd_stats.hddTxRxStats.rxRefused;
         }
      }
      else if ((WLAN_HDD_GET_AP_CTX_PTR(pAdapter))->apDisableIntraBssFwd)
      {
        kfree_skb(skb);   
      } 
      else
      {
         //                
        status = hdd_softap_sta_2_sta_xmit(skb, skb->dev,
                 pRxMetaInfo->ucDesSTAId, (pRxMetaInfo->ucUP));
      }

      //                                         
      pVosPacket = pNextVosPacket;

   } while (pVosPacket);

   //                                                       
   status = vos_pkt_return_packet( pVosPacketChain );
   if(!VOS_IS_STATUS_SUCCESS( status ))
   {
      VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_ERROR,"%s: Failure returning vos pkt", __func__);
   }
   
   pAdapter->dev->last_rx = jiffies;

   return status;   
}

VOS_STATUS hdd_softap_DeregisterSTA( hdd_adapter_t *pAdapter, tANI_U8 staId )
{
    VOS_STATUS vosStatus = VOS_STATUS_SUCCESS;
    hdd_context_t *pHddCtx;
    if (NULL == pAdapter)
    {
        VOS_TRACE(VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_ERROR,
                    "%s: pAdapter is NULL", __func__);
        return VOS_STATUS_E_INVAL;
    }

    if (WLAN_HDD_ADAPTER_MAGIC != pAdapter->magic)
    {
        VOS_TRACE(VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_ERROR,
                    "%s: Invalid pAdapter magic", __func__);
        return VOS_STATUS_E_INVAL;
    }

    pHddCtx = (hdd_context_t*)(pAdapter->pHddCtx);
    //                                                                    
    //                                                      
    vosStatus = WLANTL_ClearSTAClient( pHddCtx->pvosContext, staId );
    if ( !VOS_IS_STATUS_SUCCESS( vosStatus ) )
    {
        VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_ERROR, 
                    "WLANTL_ClearSTAClient() failed to for staID %d.  "
                    "Status= %d [0x%08lX]",
                    staId, vosStatus, vosStatus );
    }

    vosStatus = hdd_softap_deinit_tx_rx_sta ( pAdapter, staId );
    if( VOS_STATUS_E_FAILURE == vosStatus )
    {
        VOS_TRACE ( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_ERROR,
                    "hdd_softap_deinit_tx_rx_sta() failed for staID %d. "
                    "Status = %d [0x%08lX]",
                    staId, vosStatus, vosStatus );
        return( vosStatus );
    }
    
    pHddCtx->sta_to_adapter[staId] = NULL;

    return( vosStatus );
}

VOS_STATUS hdd_softap_RegisterSTA( hdd_adapter_t *pAdapter,
                                       v_BOOL_t fAuthRequired,
                                       v_BOOL_t fPrivacyBit,
                                       v_U8_t staId,
                                       v_U8_t ucastSig,
                                       v_U8_t bcastSig,
                                       v_MACADDR_t *pPeerMacAddress,
                                       v_BOOL_t fWmmEnabled )
{
   VOS_STATUS vosStatus = VOS_STATUS_E_FAILURE;
   WLAN_STADescType staDesc = {0};   
   hdd_context_t *pHddCtx = pAdapter->pHddCtx;
   hdd_adapter_t *pmonAdapter = NULL;

   //                                       
   //                     
   
   /*
                                                         
   */
   if ( pAdapter->aStaInfo[staId].isUsed )
   {
      VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_INFO,
                 "clean up old entry for STA %d", staId);
      hdd_softap_DeregisterSTA( pAdapter, staId );
   }

   //                                                             
   
   staDesc.ucSTAId = staId;
   

   /*                                        */
   pHddCtx->sta_to_adapter[staId] = pAdapter;

   staDesc.wSTAType = WLAN_STA_SOFTAP;

   vos_mem_copy( staDesc.vSTAMACAddress.bytes, pPeerMacAddress->bytes,sizeof(pPeerMacAddress->bytes) );
   vos_mem_copy( staDesc.vBSSIDforIBSS.bytes, &pAdapter->macAddressCurrent,6 );
   vos_copy_macaddr( &staDesc.vSelfMACAddress, &pAdapter->macAddressCurrent );

   VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_INFO,
              "register station \n");
   VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_INFO,
              "station mac " MAC_ADDRESS_STR,
              MAC_ADDR_ARRAY(staDesc.vSTAMACAddress.bytes));
   VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_INFO,
              "BSSIDforIBSS " MAC_ADDRESS_STR,
              MAC_ADDR_ARRAY(staDesc.vBSSIDforIBSS.bytes));
   VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_INFO,
              "SOFTAP SELFMAC " MAC_ADDRESS_STR,
              MAC_ADDR_ARRAY(staDesc.vSelfMACAddress.bytes));

   vosStatus = hdd_softap_init_tx_rx_sta(pAdapter, staId, &staDesc.vSTAMACAddress);

   staDesc.ucQosEnabled = fWmmEnabled;
   VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_INFO,
              "HDD SOFTAP register TL QoS_enabled=%d",
              staDesc.ucQosEnabled );

   staDesc.ucProtectedFrame = (v_U8_t)fPrivacyBit ;


   //                                                   
   staDesc.ucSwFrameTXXlation = 1;
   staDesc.ucSwFrameRXXlation = 1;
   staDesc.ucAddRmvLLC = 1;

   //                                
   staDesc.ucUcastSig  = ucastSig;
   staDesc.ucBcastSig  = bcastSig;
   staDesc.ucInitState = fAuthRequired ?
      WLANTL_STA_CONNECTED : WLANTL_STA_AUTHENTICATED;

   staDesc.ucIsReplayCheckValid = VOS_FALSE;

   //                                      
   vosStatus = WLANTL_RegisterSTAClient( (WLAN_HDD_GET_CTX(pAdapter))->pvosContext, 
                                         hdd_softap_rx_packet_cbk, 
                                         hdd_softap_tx_complete_cbk, 
                                         hdd_softap_tx_fetch_packet_cbk, &staDesc, 0 );
   
   if ( !VOS_IS_STATUS_SUCCESS( vosStatus ) )
   {
      VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_ERROR, 
                 "SOFTAP WLANTL_RegisterSTAClient() failed to register.  Status= %d [0x%08lX]",
                 vosStatus, vosStatus );
      return vosStatus;      
   }

   //                                     
   if ( pHddCtx->cfg_ini->dynSplitscan &&
      ( VOS_TIMER_STATE_RUNNING !=
                      vos_timer_getCurrentState(&pHddCtx->tx_rx_trafficTmr)))
   {
        vos_timer_start(&pHddCtx->tx_rx_trafficTmr,
                        pHddCtx->cfg_ini->trafficMntrTmrForSplitScan);
   }

   //                                                                             
   //                                                                                
   //                                                                                    
   //       
   
   //                         
   pAdapter->aStaInfo[staId].ucSTAId = staId;
   pAdapter->aStaInfo[staId].isQosEnabled = fWmmEnabled;

   if ( !fAuthRequired )
   {
      VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_INFO,
                 "open/shared auth StaId= %d.  Changing TL state to AUTHENTICATED at Join time", pAdapter->aStaInfo[staId].ucSTAId );
   
      //                                                                      
      //                                
      vosStatus = WLANTL_ChangeSTAState( (WLAN_HDD_GET_CTX(pAdapter))->pvosContext, staDesc.ucSTAId, 
                                         WLANTL_STA_AUTHENTICATED );
  
      pAdapter->aStaInfo[staId].tlSTAState = WLANTL_STA_AUTHENTICATED;
      pAdapter->sessionCtx.ap.uIsAuthenticated = VOS_TRUE;
   }                                            
   else
   {

      VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_INFO,
                 "ULA auth StaId= %d.  Changing TL state to CONNECTED at Join time", pAdapter->aStaInfo[staId].ucSTAId );
   
      vosStatus = WLANTL_ChangeSTAState( (WLAN_HDD_GET_CTX(pAdapter))->pvosContext, staDesc.ucSTAId, 
                                         WLANTL_STA_CONNECTED );
      pAdapter->aStaInfo[staId].tlSTAState = WLANTL_STA_CONNECTED;

      pAdapter->sessionCtx.ap.uIsAuthenticated = VOS_FALSE;

   }      
   pmonAdapter= hdd_get_mon_adapter( pAdapter->pHddCtx);
   if(pmonAdapter) 
   {
       hddLog(VOS_TRACE_LEVEL_INFO_HIGH, "Turn on Monitor the carrier\n");
       netif_carrier_on(pmonAdapter->dev);
           //               
       netif_tx_start_all_queues(pmonAdapter->dev);
    }
   netif_carrier_on(pAdapter->dev);
   //               
   netif_tx_start_all_queues(pAdapter->dev);

   return( vosStatus );
}

VOS_STATUS hdd_softap_Register_BC_STA( hdd_adapter_t *pAdapter, v_BOOL_t fPrivacyBit)
{
   VOS_STATUS vosStatus = VOS_STATUS_E_FAILURE;
   hdd_context_t *pHddCtx = WLAN_HDD_GET_CTX(pAdapter);
   v_MACADDR_t broadcastMacAddr = VOS_MAC_ADDR_BROADCAST_INITIALIZER;
   
   
   pHddCtx->sta_to_adapter[WLAN_RX_BCMC_STA_ID] = pAdapter;
   pHddCtx->sta_to_adapter[WLAN_RX_SAP_SELF_STA_ID] = pAdapter;
   vosStatus = hdd_softap_RegisterSTA( pAdapter, VOS_FALSE, fPrivacyBit, (WLAN_HDD_GET_AP_CTX_PTR(pAdapter))->uBCStaId, 0, 1, &broadcastMacAddr,0);

   return vosStatus;
}

VOS_STATUS hdd_softap_Deregister_BC_STA( hdd_adapter_t *pAdapter)
{
   return hdd_softap_DeregisterSTA( pAdapter, (WLAN_HDD_GET_AP_CTX_PTR(pAdapter))->uBCStaId);
}

VOS_STATUS hdd_softap_stop_bss( hdd_adapter_t *pAdapter)
{
    hdd_context_t *pHddCtx;
    VOS_STATUS vosStatus = VOS_STATUS_E_FAILURE;
    v_U8_t staId = 0;
    pHddCtx = WLAN_HDD_GET_CTX(pAdapter);

    /*                                                                     */
    if (WLAN_HDD_IS_LOAD_UNLOAD_IN_PROGRESS(pHddCtx))
    {
        VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                   "%s:Loading_unloading in Progress. Ignore!!!",__func__);
        return VOS_STATUS_E_PERM;
    }

    vosStatus = hdd_softap_Deregister_BC_STA( pAdapter);

    if (!VOS_IS_STATUS_SUCCESS(vosStatus))
    {
        VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_ERROR,
                   "%s: Failed to deregister BC sta Id %d", __func__, (WLAN_HDD_GET_AP_CTX_PTR(pAdapter))->uBCStaId);
    }

    for (staId = 0; staId < WLAN_MAX_STA_COUNT; staId++)
    {
        if (pAdapter->aStaInfo[staId].isUsed)//                                                   
            vosStatus = hdd_softap_DeregisterSTA( pAdapter, staId);

        if (!VOS_IS_STATUS_SUCCESS(vosStatus))
        {
            VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_ERROR,
                       "%s: Failed to deregister sta Id %d", __func__, staId);
        }
    }

    return vosStatus;
}

VOS_STATUS hdd_softap_change_STA_state( hdd_adapter_t *pAdapter, v_MACADDR_t *pDestMacAddress, WLANTL_STAStateType state)
{
    v_U8_t ucSTAId = WLAN_MAX_STA_COUNT;
    VOS_STATUS vosStatus = eHAL_STATUS_SUCCESS;
    v_CONTEXT_t pVosContext = (WLAN_HDD_GET_CTX(pAdapter))->pvosContext;

    VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_INFO,
               "%s: enter", __func__);

    if (VOS_STATUS_SUCCESS != hdd_softap_GetStaId(pAdapter, pDestMacAddress, &ucSTAId))
    {
        VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_ERROR,
                    "%s: Failed to find right station", __func__);
        return VOS_STATUS_E_FAILURE;
    }

    if (FALSE == vos_is_macaddr_equal(&pAdapter->aStaInfo[ucSTAId].macAddrSTA, pDestMacAddress))
    {
         VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_ERROR,
                    "%s: Station MAC address does not matching", __func__);
         return VOS_STATUS_E_FAILURE;
    }

    vosStatus = WLANTL_ChangeSTAState( pVosContext, ucSTAId, state );
    VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_INFO,
                   "%s: change station to state %d succeed", __func__, state);

    if (VOS_STATUS_SUCCESS == vosStatus)
    {
       pAdapter->aStaInfo[ucSTAId].tlSTAState = WLANTL_STA_AUTHENTICATED;
    }

    VOS_TRACE(VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_INFO,
                                     "%s exit\n",__func__);

    return vosStatus;
}


VOS_STATUS hdd_softap_GetStaId(hdd_adapter_t *pAdapter, v_MACADDR_t *pMacAddress, v_U8_t *staId)
{
    v_U8_t i;

    for (i = 0; i < WLAN_MAX_STA_COUNT; i++)
    {
        if (vos_mem_compare(&pAdapter->aStaInfo[i].macAddrSTA, pMacAddress, sizeof(v_MACADDR_t)) &&
            pAdapter->aStaInfo[i].isUsed)
        {
            *staId = i;
            return VOS_STATUS_SUCCESS;
        }
    }

    return VOS_STATUS_E_FAILURE;
}

VOS_STATUS hdd_softap_GetConnectedStaId(hdd_adapter_t *pAdapter, v_U8_t *staId)
{
    v_U8_t i;

    for (i = 0; i < WLAN_MAX_STA_COUNT; i++)
    {
        if (pAdapter->aStaInfo[i].isUsed &&
            (!vos_is_macaddr_broadcast(&pAdapter->aStaInfo[i].macAddrSTA)))
        {
            *staId = i;
            return VOS_STATUS_SUCCESS;
        }
    }

    return VOS_STATUS_E_FAILURE;
}

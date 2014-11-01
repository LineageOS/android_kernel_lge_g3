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

/******************************************************************************
*
* Name:  btcApi.c
*
* Description: Routines that make up the BTC API.
*
* Copyright 2008 (c) Qualcomm, Incorporated. All Rights Reserved.
* Qualcomm Confidential and Proprietary.
*
******************************************************************************/
#include "wlan_qct_wda.h"
#ifndef WLAN_MDM_CODE_REDUCTION_OPT
#include "aniGlobal.h"
#include "smsDebug.h"
#include "btcApi.h"
#include "cfgApi.h"
#include "pmc.h"
#include "smeQosInternal.h"
#ifdef FEATURE_WLAN_DIAG_SUPPORT
#include "vos_diag_core_event.h"
#include "vos_diag_core_log.h"
#endif /*                           */
static void btcLogEvent (tHalHandle hHal, tpSmeBtEvent pBtEvent);
static void btcRestoreHeartBeatMonitoringHandle(void* hHal);
static void btcUapsdCheck( tpAniSirGlobal pMac, tpSmeBtEvent pBtEvent );
VOS_STATUS btcCheckHeartBeatMonitoring(tHalHandle hHal, tpSmeBtEvent pBtEvent);
static void btcPowerStateCB( v_PVOID_t pContext, tPmcState pmcState );
static VOS_STATUS btcDeferEvent( tpAniSirGlobal pMac, tpSmeBtEvent pEvent );
static VOS_STATUS btcDeferDisconnEvent( tpAniSirGlobal pMac, tpSmeBtEvent pEvent );
#ifdef FEATURE_WLAN_DIAG_SUPPORT
static void btcDiagEventLog (tHalHandle hHal, tpSmeBtEvent pBtEvent);
#endif /*                           */
/*                                                                            
               
                                            
                                                  
                      
                                         
                                       
                                                                             */
VOS_STATUS btcOpen (tHalHandle hHal)
{
   tpAniSirGlobal pMac = PMAC_STRUCT(hHal);
   VOS_STATUS vosStatus;
   int i;

   /*                               */
   pMac->btc.btcConfig.btcExecutionMode = BTC_SMART_COEXISTENCE;
   pMac->btc.btcConfig.btcConsBtSlotsToBlockDuringDhcp = 0;
   pMac->btc.btcConfig.btcA2DPBtSubIntervalsDuringDhcp = BTC_MAX_NUM_ACL_BT_SUB_INTS;
   pMac->btc.btcConfig.btcBtIntervalMode1 = BTC_BT_INTERVAL_MODE1_DEFAULT;
   pMac->btc.btcConfig.btcWlanIntervalMode1 = BTC_WLAN_INTERVAL_MODE1_DEFAULT;
   pMac->btc.btcConfig.btcActionOnPmFail = BTC_START_NEXT;

   pMac->btc.btcConfig.btcStaticLenInqBt = BTC_STATIC_BT_LEN_INQ_DEF;
   pMac->btc.btcConfig.btcStaticLenPageBt = BTC_STATIC_BT_LEN_PAGE_DEF;
   pMac->btc.btcConfig.btcStaticLenConnBt = BTC_STATIC_BT_LEN_CONN_DEF;
   pMac->btc.btcConfig.btcStaticLenLeBt = BTC_STATIC_BT_LEN_LE_DEF;
   pMac->btc.btcConfig.btcStaticLenInqWlan = BTC_STATIC_WLAN_LEN_INQ_DEF;
   pMac->btc.btcConfig.btcStaticLenPageWlan = BTC_STATIC_WLAN_LEN_PAGE_DEF;
   pMac->btc.btcConfig.btcStaticLenConnWlan = BTC_STATIC_WLAN_LEN_CONN_DEF;
   pMac->btc.btcConfig.btcStaticLenLeWlan = BTC_STATIC_WLAN_LEN_LE_DEF;
   pMac->btc.btcConfig.btcDynMaxLenBt = BTC_DYNAMIC_BT_LEN_MAX_DEF;
   pMac->btc.btcConfig.btcDynMaxLenWlan = BTC_DYNAMIC_WLAN_LEN_MAX_DEF;
   pMac->btc.btcConfig.btcMaxScoBlockPerc = BTC_SCO_BLOCK_PERC_DEF;
   pMac->btc.btcConfig.btcDhcpProtOnA2dp = BTC_DHCP_ON_A2DP_DEF;
   pMac->btc.btcConfig.btcDhcpProtOnSco = BTC_DHCP_ON_SCO_DEF;

   pMac->btc.btcReady = VOS_FALSE;
   pMac->btc.btcEventState = 0;
   pMac->btc.btcHBActive = VOS_TRUE;
   pMac->btc.btcScanCompromise = VOS_FALSE;

   for (i = 0; i < MWS_COEX_MAX_VICTIM_TABLE; i++)
   {
      pMac->btc.btcConfig.mwsCoexVictimWANFreq[i] = 0;
      pMac->btc.btcConfig.mwsCoexVictimWLANFreq[i] = 0;
      pMac->btc.btcConfig.mwsCoexVictimConfig[i] = 0;
      pMac->btc.btcConfig.mwsCoexVictimConfig2[i] = 0;
   }

   for (i = 0; i < MWS_COEX_MAX_CONFIG; i++)
   {
      pMac->btc.btcConfig.mwsCoexConfig[i] = 0;
   }

   pMac->btc.btcConfig.mwsCoexModemBackoff = 0;
   pMac->btc.btcConfig.SARPowerBackoff = 0;

   vosStatus = vos_timer_init( &pMac->btc.restoreHBTimer,
                      VOS_TIMER_TYPE_SW,
                      btcRestoreHeartBeatMonitoringHandle,
                      (void*) hHal);
   if (!VOS_IS_STATUS_SUCCESS(vosStatus)) {
       VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, "btcOpen: Fail to init timer");
       return VOS_STATUS_E_FAILURE;
   }
   if( !HAL_STATUS_SUCCESS(pmcRegisterDeviceStateUpdateInd( pMac, btcPowerStateCB, pMac )) )
   {
       VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, "btcOpen: Fail to register PMC callback");
       return VOS_STATUS_E_FAILURE;
   }
   return VOS_STATUS_SUCCESS;
}
/*                                                                            
                
                                            
                                                  
                      
                                         
                                       
                                                                             */
VOS_STATUS btcClose (tHalHandle hHal)
{
   tpAniSirGlobal pMac = PMAC_STRUCT(hHal);
   VOS_STATUS vosStatus;
   pMac->btc.btcReady = VOS_FALSE;
   pMac->btc.btcUapsdOk = VOS_FALSE;
   vos_timer_stop(&pMac->btc.restoreHBTimer);
   vosStatus = vos_timer_destroy(&pMac->btc.restoreHBTimer);
   if (!VOS_IS_STATUS_SUCCESS(vosStatus)) {
       VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, "btcClose: Fail to destroy timer");
       return VOS_STATUS_E_FAILURE;
   }
   if(!HAL_STATUS_SUCCESS(
      pmcDeregisterDeviceStateUpdateInd(pMac, btcPowerStateCB)))
   {
       VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_FATAL,
         "%s: %d: cannot deregister with pmcDeregisterDeviceStateUpdateInd()",
                __func__, __LINE__);
   }

   return VOS_STATUS_SUCCESS;
}

/*                                                                            
                
                                                                             
                                                                             
                                                                          
                                                                             
                         
                                                  
                      
                                                                             */
VOS_STATUS btcReady (tHalHandle hHal)
{
    tpAniSirGlobal pMac = PMAC_STRUCT(hHal);
    v_U32_t cfgVal = 0;
    v_U8_t i;
    pMac->btc.btcReady = VOS_TRUE;
    pMac->btc.btcUapsdOk = VOS_TRUE;
    for(i=0; i < BT_MAX_SCO_SUPPORT; i++)
    {
        pMac->btc.btcScoHandles[i] = BT_INVALID_CONN_HANDLE;
    }

    //                                          
    ccmCfgGetInt(pMac, WNI_CFG_HEART_BEAT_THRESHOLD, &cfgVal);
    pMac->btc.btcHBCount = (v_U8_t)cfgVal;
    if (btcSendCfgMsg(hHal, &(pMac->btc.btcConfig)) != VOS_STATUS_SUCCESS)
    {
        return VOS_STATUS_E_FAILURE;
    }
    return VOS_STATUS_SUCCESS;
}

static VOS_STATUS btcSendBTEvent(tpAniSirGlobal pMac, tpSmeBtEvent pBtEvent)
{
   vos_msg_t msg;
   tpSmeBtEvent ptrSmeBtEvent = NULL;
   switch(pBtEvent->btEventType)
   {
      case BT_EVENT_CREATE_SYNC_CONNECTION:
      case BT_EVENT_SYNC_CONNECTION_UPDATED:
         if(pBtEvent->uEventParam.btSyncConnection.linkType != BT_SCO &&
            pBtEvent->uEventParam.btSyncConnection.linkType != BT_eSCO)
         {
            VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, "%s: "
               "Invalid link type %d for Sync Connection. BT event will be dropped ",
               __func__, pBtEvent->uEventParam.btSyncConnection.linkType);
            return VOS_STATUS_E_FAILURE;
         }
         break;
      case BT_EVENT_SYNC_CONNECTION_COMPLETE:
         if((pBtEvent->uEventParam.btSyncConnection.status == BT_CONN_STATUS_SUCCESS) &&
            ((pBtEvent->uEventParam.btSyncConnection.linkType != BT_SCO && pBtEvent->uEventParam.btSyncConnection.linkType != BT_eSCO) ||
             (pBtEvent->uEventParam.btSyncConnection.connectionHandle == BT_INVALID_CONN_HANDLE)))
         {
            VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, "%s: "
               "Invalid connection handle %d or link type %d for Sync Connection. BT event will be dropped ",
               __func__,
               pBtEvent->uEventParam.btSyncConnection.connectionHandle,
               pBtEvent->uEventParam.btSyncConnection.linkType);
            return VOS_STATUS_E_FAILURE;
         }
         break;
      case BT_EVENT_MODE_CHANGED:
         if(pBtEvent->uEventParam.btAclModeChange.mode >= BT_ACL_MODE_MAX)
         {
            VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, "%s: "
               "Invalid mode %d for ACL Connection. BT event will be dropped ",
               __func__,
               pBtEvent->uEventParam.btAclModeChange.mode);
            return VOS_STATUS_E_FAILURE;
         }
         break;
     case BT_EVENT_DEVICE_SWITCHED_OFF:
         pMac->btc.btcEventState = 0;
         break;
      default:
         break;
   }
   ptrSmeBtEvent = vos_mem_malloc(sizeof(tSmeBtEvent));
   if (NULL == ptrSmeBtEvent)
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, "%s: "
         "Not able to allocate memory for BT event", __func__);
      return VOS_STATUS_E_FAILURE;
   }
   btcLogEvent(pMac, pBtEvent);
#ifdef FEATURE_WLAN_DIAG_SUPPORT
   btcDiagEventLog(pMac, pBtEvent);
#endif
   vos_mem_copy(ptrSmeBtEvent, pBtEvent, sizeof(tSmeBtEvent));
   msg.type = WDA_SIGNAL_BT_EVENT;
   msg.reserved = 0;
   msg.bodyptr = ptrSmeBtEvent;
   if(VOS_STATUS_SUCCESS != vos_mq_post_message(VOS_MODULE_ID_WDA, &msg))
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, "%s: "
         "Not able to post WDA_SIGNAL_BT_EVENT message to WDA", __func__);
      vos_mem_free( ptrSmeBtEvent );
      return VOS_STATUS_E_FAILURE;
   }
   //                                                            
   //                                  
   (void)btcCheckHeartBeatMonitoring(pMac, pBtEvent);
   //                                        
   btcUapsdCheck( pMac, pBtEvent );
   return VOS_STATUS_SUCCESS;
   }

#ifndef WLAN_MDM_CODE_REDUCTION_OPT
/*                                                                            
                        
                                                                               
                                                                              
                                                                              
                              
                                                  
                                                                                 
                                                                                 
                      
                                                                              
                                                                                   
                                                                  
                                                         
                                                                             */
VOS_STATUS btcSignalBTEvent (tHalHandle hHal, tpSmeBtEvent pBtEvent)
{
   tpAniSirGlobal pMac = PMAC_STRUCT( hHal );
   VOS_STATUS vosStatus;
   if( NULL == pBtEvent )
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, "%s: "
         "Null pointer for SME BT Event", __func__);
      return VOS_STATUS_E_FAILURE;
   }
   if(( BTC_WLAN_ONLY == pMac->btc.btcConfig.btcExecutionMode ) ||
      ( BTC_PTA_ONLY == pMac->btc.btcConfig.btcExecutionMode ))
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, "%s: "
         "BTC execution mode not set to BTC_SMART_COEXISTENCE. BT event will be dropped", __func__);
      return VOS_STATUS_E_FAILURE;
   }
   if( pBtEvent->btEventType < 0 || pBtEvent->btEventType >= BT_EVENT_TYPE_MAX )
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, "%s: "
         "Invalid BT event %d being passed. BT event will be dropped",
          __func__, pBtEvent->btEventType);
      return VOS_STATUS_E_FAILURE;
   }
   //                                                     
   //                                                                                                 
   if( pMac->btc.fReplayBTEvents || !PMC_IS_CHIP_ACCESSIBLE(pmcGetPmcState( pMac )) )
   {
       //                          
       vosStatus = btcDeferEvent(pMac, pBtEvent);
       if( VOS_IS_STATUS_SUCCESS(vosStatus) )
       {
           pMac->btc.fReplayBTEvents = VOS_TRUE;
           return VOS_STATUS_SUCCESS;
       }
       else
       {
           return vosStatus;
       }
   }
    btcSendBTEvent(pMac, pBtEvent);
   return VOS_STATUS_SUCCESS;
}
#endif
/*                                                                            
                                   
                                                                                
                                                                                 
                                                                                  
                                                                                  
                                                               
                                                  
                                                                                 
                                                                                 
                      
                                                           
                                                    
                                                                             */
VOS_STATUS btcCheckHeartBeatMonitoring(tHalHandle hHal, tpSmeBtEvent pBtEvent)
{
   tpAniSirGlobal pMac = PMAC_STRUCT( hHal );
   VOS_STATUS vosStatus;
   switch(pBtEvent->btEventType)
   {
      //                                                              
      case BT_EVENT_INQUIRY_STARTED:
          pMac->btc.btcEventState |= BT_INQUIRY_STARTED;
          break;
      case BT_EVENT_PAGE_STARTED:
          pMac->btc.btcEventState |= BT_PAGE_STARTED;
          break;
      case BT_EVENT_CREATE_ACL_CONNECTION:
          pMac->btc.btcEventState |= BT_CREATE_ACL_CONNECTION_STARTED;
          break;
      case BT_EVENT_CREATE_SYNC_CONNECTION:
          pMac->btc.btcEventState |= BT_CREATE_SYNC_CONNECTION_STARTED;
          break;
      //                                                                     
      case BT_EVENT_INQUIRY_STOPPED:
          pMac->btc.btcEventState &= ~(BT_INQUIRY_STARTED);
          break;
      case BT_EVENT_PAGE_STOPPED:
          pMac->btc.btcEventState &= ~(BT_PAGE_STARTED);
          break;
      case BT_EVENT_ACL_CONNECTION_COMPLETE:
          pMac->btc.btcEventState &= ~(BT_CREATE_ACL_CONNECTION_STARTED);
          break;
      case BT_EVENT_SYNC_CONNECTION_COMPLETE:
          pMac->btc.btcEventState &= ~(BT_CREATE_SYNC_CONNECTION_STARTED);
          break;
      default:
          //                    
          return VOS_STATUS_SUCCESS;
   }
   //                                               
   if (pMac->btc.btcEventState) {
       if (pMac->btc.btcHBActive) {
           //                                    
           ccmCfgSetInt(pMac, WNI_CFG_HEART_BEAT_THRESHOLD, 0, NULL, eANI_BOOLEAN_FALSE);
           pMac->btc.btcHBActive = VOS_FALSE;
       }
       //                                           
       vos_timer_stop( &pMac->btc.restoreHBTimer);
       vosStatus= vos_timer_start( &pMac->btc.restoreHBTimer, BT_MAX_EVENT_DONE_TIMEOUT );
       if (!VOS_IS_STATUS_SUCCESS(vosStatus)) {
           VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, "btcCheckHeartBeatMonitoring: Fail to start timer");
           return VOS_STATUS_E_FAILURE;
       }
   } else {
       //                                                               
       if (!pMac->btc.btcHBActive) {
           ccmCfgSetInt(pMac, WNI_CFG_HEART_BEAT_THRESHOLD, pMac->btc.btcHBCount, NULL, eANI_BOOLEAN_FALSE);
           pMac->btc.btcHBActive = VOS_TRUE;
       }
       //                     
       vosStatus = vos_timer_stop( &pMac->btc.restoreHBTimer);
       if (!VOS_IS_STATUS_SUCCESS(vosStatus)) {
           VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, "btcCheckHeartBeatMonitoring: Fail to stop timer");
           return VOS_STATUS_E_FAILURE;
       }
   }
   return VOS_STATUS_SUCCESS;
}
/*                                                                            
                                           
                                                                             
                                                                            
                            
                                                  
                
                                                                             */
void btcRestoreHeartBeatMonitoringHandle(tHalHandle hHal)
{
    tpAniSirGlobal pMac = PMAC_STRUCT( hHal );
    if( !pMac->btc.btcHBActive )
    {
        tPmcState pmcState;
        //                                                     
        pmcState = pmcGetPmcState( pMac );
        if( PMC_IS_CHIP_ACCESSIBLE(pmcState) )
        {
            //                                       
            ccmCfgSetInt(pMac, WNI_CFG_HEART_BEAT_THRESHOLD, pMac->btc.btcHBCount, NULL, eANI_BOOLEAN_FALSE);
            VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, "BT event timeout, restoring back HeartBeat timer");
        }
        else
        {
            //        
            pMac->btc.btcEventReplay.fRestoreHBMonitor = VOS_TRUE;
        }
    }
}


/*                                                                            
                    
                                                                               
                                                                                  
                                                  
                                                                        
                                                                                    
                                           
                      
                                                           
                                                    
                                                                             */
VOS_STATUS btcSetConfig (tHalHandle hHal, tpSmeBtcConfig pSmeBtcConfig)
{
   tpAniSirGlobal pMac = PMAC_STRUCT( hHal );
   //                                    
   vos_mem_copy(&(pMac->btc.btcConfig), pSmeBtcConfig, sizeof(tSmeBtcConfig));
   //                                                                         
   //                                                                 
   if(pMac->btc.btcReady)
   {
      if(VOS_STATUS_SUCCESS != btcSendCfgMsg(hHal, pSmeBtcConfig))
      {
         VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_FATAL, 
            "Failure to send BTC config down");
         return VOS_STATUS_E_FAILURE;
      }
   }
   return VOS_STATUS_SUCCESS;
}
/*                                                                            
                        
                                                         
                                                  
                                                                        
                                                                                    
                                           
                      
                                                           
                                                    
                                                                             */
VOS_STATUS btcSendCfgMsg(tHalHandle hHal, tpSmeBtcConfig pSmeBtcConfig)
{
   tpSmeBtcConfig ptrSmeBtcConfig = NULL;
   vos_msg_t msg;
   if( NULL == pSmeBtcConfig )
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, "btcSendCfgMsg: "
         "Null pointer for BTC Config");
      return VOS_STATUS_E_FAILURE;
   }
   if( pSmeBtcConfig->btcExecutionMode >= BT_EXEC_MODE_MAX )
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, "btcSendCfgMsg: "
         "Invalid BT execution mode %d being set",
          pSmeBtcConfig->btcExecutionMode);
      return VOS_STATUS_E_FAILURE;
   }
   ptrSmeBtcConfig = vos_mem_malloc(sizeof(tSmeBtcConfig));
   if (NULL == ptrSmeBtcConfig)
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, "btcSendCfgMsg: "
         "Not able to allocate memory for SME BTC Config");
      return VOS_STATUS_E_FAILURE;
   }
   vos_mem_copy(ptrSmeBtcConfig, pSmeBtcConfig, sizeof(tSmeBtcConfig));
   msg.type = WDA_BTC_SET_CFG;
   msg.reserved = 0;
   msg.bodyptr = ptrSmeBtcConfig;
   if(VOS_STATUS_SUCCESS != vos_mq_post_message(VOS_MODULE_ID_WDA, &msg))
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, "btcSendCfgMsg: "
         "Not able to post WDA_BTC_SET_CFG message to WDA");
      vos_mem_free( ptrSmeBtcConfig );
      return VOS_STATUS_E_FAILURE;
   }
   return VOS_STATUS_SUCCESS;
}
/*                                                                            
                    
                                                                                 
                                                  
                                                                        
                                                                                    
                                           
                      
                                          
                                       
                                                                             */
VOS_STATUS btcGetConfig (tHalHandle hHal, tpSmeBtcConfig pSmeBtcConfig)
{
   tpAniSirGlobal pMac = PMAC_STRUCT( hHal );
   if( NULL == pSmeBtcConfig )
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, "btcGetConfig: "
         "Null pointer for BTC Config");
      return VOS_STATUS_E_FAILURE;
   }
   vos_mem_copy(pSmeBtcConfig, &(pMac->btc.btcConfig), sizeof(tSmeBtcConfig));
   return VOS_STATUS_SUCCESS;
}
/*
                                                      
                                          
                                                                                               
                                                      
                                                
                                                                                                        
                                                                 
*/
static tpSmeBtAclEventHist btcFindAclEventHist( tpAniSirGlobal pMac, v_U8_t *bdAddr, v_U16_t handle )
{
    int i, j;
    tpSmeBtAclEventHist pRet = NULL;
    tSmeBtcEventReplay *pReplay = &pMac->btc.btcEventReplay;
    for( i = 0; (i < BT_MAX_ACL_SUPPORT) && (NULL == pRet); i++ )
    {
        if( NULL != bdAddr )
        {
            //                 
            if( pReplay->btcEventHist.btAclConnectionEvent[i].bNextEventIdx )
            {
                for(j = 0; j < pReplay->btcEventHist.btAclConnectionEvent[i].bNextEventIdx; j++)
                {
                    if( vos_mem_compare(pReplay->btcEventHist.btAclConnectionEvent[i].btAclConnection[j].bdAddr,
                        bdAddr, 6) )
                    {
                        //        
                        pRet = &pReplay->btcEventHist.btAclConnectionEvent[i];
                        break;
                    }
                }
            }
        }
        else if( BT_INVALID_CONN_HANDLE != handle )
        {
            //                   
            if( pReplay->btcEventHist.btAclConnectionEvent[i].bNextEventIdx )
            {
                for(j = 0; j < pReplay->btcEventHist.btAclConnectionEvent[i].bNextEventIdx; j++)
                {
                    if( pReplay->btcEventHist.btAclConnectionEvent[i].btAclConnection[j].connectionHandle ==
                        handle )
                    {
                        //        
                        pRet = &pReplay->btcEventHist.btAclConnectionEvent[i];
                        break;
                    }
                }
            }
        }
        else if( 0 == pReplay->btcEventHist.btAclConnectionEvent[i].bNextEventIdx )
        {
            pRet = &pReplay->btcEventHist.btAclConnectionEvent[i];
            break;
        }
    }
    return (pRet);
}

/*
                                                        
                                          
                                                                                               
                                                      
                                                
                                                                                                        
                                                                 
*/
static tpSmeBtSyncEventHist btcFindSyncEventHist( tpAniSirGlobal pMac, v_U8_t *bdAddr, v_U16_t handle )
{
    int i, j;
    tpSmeBtSyncEventHist pRet = NULL;
    tSmeBtcEventReplay *pReplay = &pMac->btc.btcEventReplay;
    for( i = 0; (i < BT_MAX_SCO_SUPPORT) && (NULL == pRet); i++ )
    {
        if( NULL != bdAddr )
        {
            //                 
            if( pReplay->btcEventHist.btSyncConnectionEvent[i].bNextEventIdx )
            {
                for(j = 0; j < pReplay->btcEventHist.btAclConnectionEvent[i].bNextEventIdx; j++)
                {
                    if( vos_mem_compare(pReplay->btcEventHist.btSyncConnectionEvent[i].btSyncConnection[j].bdAddr,
                        bdAddr, 6) )
                    {
                        //        
                        pRet = &pReplay->btcEventHist.btSyncConnectionEvent[i];
                        break;
                    }
                }
            }
        }
        else if( BT_INVALID_CONN_HANDLE != handle )
        {
            //                   
            if( pReplay->btcEventHist.btSyncConnectionEvent[i].bNextEventIdx )
            {
                for(j = 0; j < pReplay->btcEventHist.btAclConnectionEvent[i].bNextEventIdx; j++)
                {
                    if( pReplay->btcEventHist.btSyncConnectionEvent[i].btSyncConnection[j].connectionHandle ==
                        handle )
                    {
                        //        
                        pRet = &pReplay->btcEventHist.btSyncConnectionEvent[i];
                        break;
                    }
                }
            }
        }
        else if( !pReplay->btcEventHist.btSyncConnectionEvent[i].bNextEventIdx )
        {
            pRet = &pReplay->btcEventHist.btSyncConnectionEvent[i];
            break;
        }
    }
    return (pRet);
}

/*
                                                                         
                                                         
                                                                                                      
*/
static tpSmeBtDisconnectEventHist btcFindDisconnEventHist( tpAniSirGlobal pMac, v_U16_t handle )
{
    tpSmeBtDisconnectEventHist pRet = NULL;
    tSmeBtcEventReplay *pReplay = &pMac->btc.btcEventReplay;
    int i;
    if( BT_INVALID_CONN_HANDLE != handle )
    {
        for(i = 0; i < BT_MAX_DISCONN_SUPPORT; i++)
        {
            if( pReplay->btcEventHist.btDisconnectEvent[i].fValid &&
                (handle == pReplay->btcEventHist.btDisconnectEvent[i].btDisconnect.connectionHandle) )
            {
                pRet = &pReplay->btcEventHist.btDisconnectEvent[i];
                break;
            }
        }
    }
    if( NULL == pRet )
    {
        //                
        for(i = 0; i < BT_MAX_DISCONN_SUPPORT; i++)
        {
            if( !pReplay->btcEventHist.btDisconnectEvent[i].fValid )
            {
                pRet = &pReplay->btcEventHist.btDisconnectEvent[i];
                break;
            }
        }
    }
    return (pRet);
}

/*
                                                                              
                                                          
                                                                                                      
*/
tpSmeBtAclModeChangeEventHist btcFindModeChangeEventHist( tpAniSirGlobal pMac, v_U16_t handle )
{
    tpSmeBtAclModeChangeEventHist pRet = NULL;
    tSmeBtcEventReplay *pReplay = &pMac->btc.btcEventReplay;
    int i;
    if( BT_INVALID_CONN_HANDLE != handle )
    {
        for(i = 0; i < BT_MAX_ACL_SUPPORT; i++)
        {
            if( pReplay->btcEventHist.btAclModeChangeEvent[i].fValid &&
                (handle == pReplay->btcEventHist.btAclModeChangeEvent[i].btAclModeChange.connectionHandle) )
            {
                pRet = &pReplay->btcEventHist.btAclModeChangeEvent[i];
                break;
            }
        }
    }
    if( NULL == pRet )
    {
        //                
        for(i = 0; i < BT_MAX_ACL_SUPPORT; i++)
        {
            if( !pReplay->btcEventHist.btAclModeChangeEvent[i].fValid )
            {
                pRet = &pReplay->btcEventHist.btAclModeChangeEvent[i];
                break;
            }
        }
    }
    return (pRet);
}

/*
                                                                             
                                                          
                                                                                                      
*/
tpSmeBtSyncUpdateHist btcFindSyncUpdateEventHist( tpAniSirGlobal pMac, v_U16_t handle )
{
    tpSmeBtSyncUpdateHist pRet = NULL;
    tSmeBtcEventReplay *pReplay = &pMac->btc.btcEventReplay;
    int i;
    if( BT_INVALID_CONN_HANDLE != handle )
    {
        for(i = 0; i < BT_MAX_SCO_SUPPORT; i++)
        {
            if( pReplay->btcEventHist.btSyncUpdateEvent[i].fValid &&
                (handle == pReplay->btcEventHist.btSyncUpdateEvent[i].btSyncConnection.connectionHandle) )
            {
                pRet = &pReplay->btcEventHist.btSyncUpdateEvent[i];
                break;
            }
        }
    }
    if( NULL == pRet )
    {
        //                
        for(i = 0; i < BT_MAX_SCO_SUPPORT; i++)
        {
            if( !pReplay->btcEventHist.btSyncUpdateEvent[i].fValid )
            {
                pRet = &pReplay->btcEventHist.btSyncUpdateEvent[i];
                break;
            }
        }
    }
    return (pRet);
}

/*
                                    
*/
static void btcReleaseAclEventHist( tpAniSirGlobal pMac, tpSmeBtAclEventHist pAclEventHist )
{
    vos_mem_zero( pAclEventHist, sizeof(tSmeBtAclEventHist) );
}

/*
                                     
*/
static void btcReleaseSyncEventHist( tpAniSirGlobal pMac, tpSmeBtSyncEventHist pSyncEventHist )
{
    vos_mem_zero( pSyncEventHist, sizeof(tSmeBtSyncEventHist) );
}

/*                             
                                           
                                                                                            
                                                                             
                                                                                                
                                      
*/
static VOS_STATUS btcDeferAclCreate( tpAniSirGlobal pMac, tpSmeBtEvent pEvent )
{
    VOS_STATUS status = VOS_STATUS_SUCCESS;
    tpSmeBtAclEventHist pAclEventHist;
    tSmeBtAclConnectionParam *pAclEvent = NULL;
    do
    {
        //            
        pAclEventHist = btcFindAclEventHist( pMac, pEvent->uEventParam.btAclConnection.bdAddr, 
                                    BT_INVALID_CONN_HANDLE );
        if( NULL == pAclEventHist )
        {
            //                                   
            //                            
            pAclEventHist = btcFindAclEventHist( pMac, NULL, BT_INVALID_CONN_HANDLE );
            if( NULL != pAclEventHist )
            {
                vos_mem_copy(&pAclEventHist->btAclConnection[0], &pEvent->uEventParam.btAclConnection, 
                                sizeof(tSmeBtAclConnectionParam));
                pAclEventHist->btEventType[0] = BT_EVENT_CREATE_ACL_CONNECTION;
                pAclEventHist->bNextEventIdx = 1;
            }
            else
            {
                smsLog(pMac, LOGE, FL(" failed to find ACL event slot"));
                status = VOS_STATUS_E_RESOURCES;
            }
            //    
            break;
        }
        else
        {
            //                                   
            if ((pAclEventHist->bNextEventIdx <= 0) ||
                (pAclEventHist->bNextEventIdx > BT_MAX_NUM_EVENT_ACL_DEFERRED))
            {
                VOS_ASSERT(0);
                status = VOS_STATUS_E_FAILURE;
                break;
            }
            pAclEvent = &pAclEventHist->btAclConnection[pAclEventHist->bNextEventIdx - 1];
            if(BT_EVENT_CREATE_ACL_CONNECTION == pAclEventHist->btEventType[pAclEventHist->bNextEventIdx - 1])
            {
                //                                                              
                if (pAclEvent)
                {
                    vos_mem_copy(pAclEvent,
                                 &pEvent->uEventParam.btAclConnection,
                                 sizeof(tSmeBtAclConnectionParam));
                }
                //    
                break;
            }
            else if(BT_EVENT_ACL_CONNECTION_COMPLETE == 
                        pAclEventHist->btEventType[pAclEventHist->bNextEventIdx - 1])
            {
                //                                                      
                if(BT_CONN_STATUS_SUCCESS == pAclEvent->status)
                {
                    tSmeBtEvent btEvent;
                    //                                                    
                    //                                                
                    smsLog(pMac, LOGE, FL("  Missing disconnect event on handle %d"), pAclEvent->connectionHandle);
                    //                       
                    btEvent.btEventType = BT_EVENT_DISCONNECTION_COMPLETE;
                    btEvent.uEventParam.btDisconnect.connectionHandle = pAclEvent->connectionHandle;
                    btcDeferDisconnEvent(pMac, &btEvent);
                }
            }
            //                          
            if(pAclEventHist->bNextEventIdx < BT_MAX_NUM_EVENT_ACL_DEFERRED)
            {
                pAclEventHist->btEventType[pAclEventHist->bNextEventIdx] = BT_EVENT_CREATE_ACL_CONNECTION;
                vos_mem_copy(&pAclEventHist->btAclConnection[pAclEventHist->bNextEventIdx], 
                                &pEvent->uEventParam.btAclConnection, 
                                sizeof(tSmeBtAclConnectionParam));
                pAclEventHist->bNextEventIdx++;
            }
            else
            {
                smsLog(pMac, LOGE, FL(" ACL event overflow"));
                VOS_ASSERT(0);
            }
        }
    }while(0);
    return status;
}

/*                            
                                                                       
                                                                                         
                                                                                  
*/
static VOS_STATUS btcDeferAclComplete( tpAniSirGlobal pMac, tpSmeBtEvent pEvent )
{
    VOS_STATUS status = VOS_STATUS_SUCCESS;
    tpSmeBtAclEventHist pAclEventHist;
    do
    {
        //            
        pAclEventHist = btcFindAclEventHist( pMac, pEvent->uEventParam.btAclConnection.bdAddr, 
                                    BT_INVALID_CONN_HANDLE );
        if(pAclEventHist)
        {
            VOS_ASSERT(pAclEventHist->bNextEventIdx >0);
            //         
            if(BT_CONN_STATUS_SUCCESS != pEvent->uEventParam.btAclConnection.status)
            {
                //                                                                            
                if(BT_EVENT_CREATE_ACL_CONNECTION == pAclEventHist->btEventType[pAclEventHist->bNextEventIdx-1])
                {
                    vos_mem_zero(&pAclEventHist->btAclConnection[pAclEventHist->bNextEventIdx-1], 
                                    sizeof(tSmeBtAclConnectionParam));
                    pAclEventHist->bNextEventIdx--;
                    //                    
                    break;
                }
                else
                {
                    smsLog(pMac, LOGE, FL(" ACL completion fail but last event(%d) not creation"),
                        pAclEventHist->btEventType[pAclEventHist->bNextEventIdx-1]);
                }
            }
        }
        if( NULL == pAclEventHist )
        {
            pAclEventHist = btcFindAclEventHist( pMac, NULL, BT_INVALID_CONN_HANDLE );
        }
        if(pAclEventHist)
        {
            if(pAclEventHist->bNextEventIdx < BT_MAX_NUM_EVENT_ACL_DEFERRED)
            {
                //               
                pAclEventHist->btEventType[pAclEventHist->bNextEventIdx] = BT_EVENT_ACL_CONNECTION_COMPLETE;
                vos_mem_copy(&pAclEventHist->btAclConnection[pAclEventHist->bNextEventIdx], 
                                &pEvent->uEventParam.btAclConnection, 
                                sizeof(tSmeBtAclConnectionParam));
                pAclEventHist->bNextEventIdx++;
            }
            else
            {
                smsLog(pMac, LOGE, FL(" ACL event overflow"));
                VOS_ASSERT(0);
            }
        }
        else
        {
            smsLog(pMac, LOGE, FL(" cannot find match for failed "
                   "BT_EVENT_ACL_CONNECTION_COMPLETE of bdAddr "
                   MAC_ADDRESS_STR),
                   MAC_ADDR_ARRAY(pEvent->uEventParam.btAclConnection.bdAddr));
            status = VOS_STATUS_E_EMPTY;
        }
    }while(0);
    return (status);
}

/*                              
                                                                   
                                  
                                                                         
                                                                      
                                       
*/
static VOS_STATUS btcDeferSyncCreate( tpAniSirGlobal pMac, tpSmeBtEvent pEvent )
{
    VOS_STATUS status = VOS_STATUS_SUCCESS;
    tpSmeBtSyncEventHist pSyncEventHist;
    tSmeBtSyncConnectionParam *pSyncEvent = NULL;
    do
    {
        //            
        pSyncEventHist = btcFindSyncEventHist( pMac, pEvent->uEventParam.btSyncConnection.bdAddr, 
                                    BT_INVALID_CONN_HANDLE );
        if( NULL == pSyncEventHist )
        {
            //                                   
            //                            
            pSyncEventHist = btcFindSyncEventHist( pMac, NULL, BT_INVALID_CONN_HANDLE );
            if( NULL != pSyncEventHist )
            {
                vos_mem_copy(&pSyncEventHist->btSyncConnection[0], &pEvent->uEventParam.btSyncConnection, 
                                sizeof(tSmeBtSyncConnectionParam));
                pSyncEventHist->btEventType[0] = BT_EVENT_CREATE_SYNC_CONNECTION;
                pSyncEventHist->bNextEventIdx = 1;
            }
            else
            {
                smsLog(pMac, LOGE, FL(" failed to find SYNC event slot"));
                status = VOS_STATUS_E_RESOURCES;
            }
            //    
            break;
        }
        else
        {
            //                                   
            if ((pSyncEventHist->bNextEventIdx <= 0) ||
                (pSyncEventHist->bNextEventIdx > BT_MAX_NUM_EVENT_SCO_DEFERRED))
            {
                VOS_ASSERT(0);
                status = VOS_STATUS_E_FAILURE;
                return status;
            }
            pSyncEvent = &pSyncEventHist->btSyncConnection[pSyncEventHist->bNextEventIdx - 1];
            if(BT_EVENT_CREATE_SYNC_CONNECTION == 
                pSyncEventHist->btEventType[pSyncEventHist->bNextEventIdx - 1])
            {
                //                                                              
                if(pSyncEvent)
                {
                    vos_mem_copy(pSyncEvent,
                                 &pEvent->uEventParam.btSyncConnection,
                                 sizeof(tSmeBtSyncConnectionParam));
                }
                //    
                break;
            }
            else if(BT_EVENT_SYNC_CONNECTION_COMPLETE == 
                        pSyncEventHist->btEventType[pSyncEventHist->bNextEventIdx - 1])
            {
                //                                                      
                if(BT_CONN_STATUS_SUCCESS == pSyncEvent->status)
                {
                    tSmeBtEvent btEvent;
                    //                                                    
                    //                                                
                    smsLog(pMac, LOGE, FL("  Missing disconnect event on handle %d"), pSyncEvent->connectionHandle);
                    //                       
                    btEvent.btEventType = BT_EVENT_DISCONNECTION_COMPLETE;
                    btEvent.uEventParam.btDisconnect.connectionHandle = pSyncEvent->connectionHandle;
                    btcDeferDisconnEvent(pMac, &btEvent);
                }
            }
            //                          
            if(pSyncEventHist->bNextEventIdx < BT_MAX_NUM_EVENT_SCO_DEFERRED)
            {
                pSyncEventHist->btEventType[pSyncEventHist->bNextEventIdx] = BT_EVENT_CREATE_SYNC_CONNECTION;
                vos_mem_copy(&pSyncEventHist->btSyncConnection[pSyncEventHist->bNextEventIdx], 
                                &pEvent->uEventParam.btSyncConnection, 
                                sizeof(tSmeBtSyncConnectionParam));
                pSyncEventHist->bNextEventIdx++;
            }
            else
            {
                smsLog(pMac, LOGE, FL(" SYNC event overflow"));
            }
        }
    }while(0);
    return status;
}

/*                             
                                                                       
                                                                                        
                         
                                                           
*/
static VOS_STATUS btcDeferSyncComplete( tpAniSirGlobal pMac, tpSmeBtEvent pEvent )
{
    VOS_STATUS status = VOS_STATUS_SUCCESS;
    tpSmeBtSyncEventHist pSyncEventHist;
    do
    {
        //            
        pSyncEventHist = btcFindSyncEventHist( pMac, pEvent->uEventParam.btSyncConnection.bdAddr, 
                                    BT_INVALID_CONN_HANDLE );
        if(pSyncEventHist)
        {
            VOS_ASSERT(pSyncEventHist->bNextEventIdx >0);
            //         
            if(BT_CONN_STATUS_SUCCESS != pEvent->uEventParam.btSyncConnection.status)
            {
                //                                                                            
                if(BT_EVENT_CREATE_SYNC_CONNECTION == pSyncEventHist->btEventType[pSyncEventHist->bNextEventIdx-1])
                {
                    vos_mem_zero(&pSyncEventHist->btSyncConnection[pSyncEventHist->bNextEventIdx-1], 
                                    sizeof(tSmeBtSyncConnectionParam));
                    pSyncEventHist->bNextEventIdx--;
                    //                    
                    break;
                }
                else
                {
                    smsLog(pMac, LOGE, FL(" SYNC completion fail but last event(%d) not creation"),
                        pSyncEventHist->btEventType[pSyncEventHist->bNextEventIdx-1]);
                }
            }
        }
        if(NULL == pSyncEventHist)
        {
            //                                         
            pSyncEventHist = btcFindSyncEventHist( pMac, NULL, BT_INVALID_CONN_HANDLE );
        }
        if(pSyncEventHist)
        {
            if(pSyncEventHist->bNextEventIdx < BT_MAX_NUM_EVENT_ACL_DEFERRED)
            {
                //               
                pSyncEventHist->btEventType[pSyncEventHist->bNextEventIdx] = BT_EVENT_SYNC_CONNECTION_COMPLETE;
                vos_mem_copy(&pSyncEventHist->btSyncConnection[pSyncEventHist->bNextEventIdx], 
                                &pEvent->uEventParam.btSyncConnection, 
                                sizeof(tSmeBtSyncConnectionParam));
                pSyncEventHist->bNextEventIdx++;
            }
            else
            {
                smsLog(pMac, LOGE, FL(" SYNC event overflow"));
            }
        }
        else
        {
            smsLog(pMac, LOGE, FL(" cannot find match for "
                   "BT_EVENT_SYNC_CONNECTION_COMPLETE of bdAddr "
                   MAC_ADDRESS_STR),
                   MAC_ADDR_ARRAY(pEvent->uEventParam.btSyncConnection.bdAddr));
            status = VOS_STATUS_E_EMPTY;
        }
    }while(0);
    return (status);
}

//                                                              
//                                            
//                                                                 
/*
                                  
                                            
                                                                                      
                        
                                                    
                                                      
                                                                                         
*/
static VOS_STATUS btcDeferDisconnectEventForACL( tpAniSirGlobal pMac, tpSmeBtEvent pEvent )
{
    VOS_STATUS status = VOS_STATUS_SUCCESS;
    tpSmeBtAclEventHist pAclEventHist;
    tpSmeBtAclModeChangeEventHist pModeChangeEventHist;
    v_BOOL_t fDone = VOS_FALSE;
    int i;
    pAclEventHist = btcFindAclEventHist( pMac, NULL, 
                                pEvent->uEventParam.btDisconnect.connectionHandle );
    if(pAclEventHist)
    {
        if( pAclEventHist->bNextEventIdx > BT_MAX_NUM_EVENT_ACL_DEFERRED)
        {
            smsLog(pMac, LOGE, FL(" ACL event history index:%d overflow, resetting to BT_MAX_NUM_EVENT_ACL_DEFERRED"), pAclEventHist->bNextEventIdx);
            pAclEventHist->bNextEventIdx = BT_MAX_NUM_EVENT_ACL_DEFERRED;
        }
        //                 
        for(i = pAclEventHist->bNextEventIdx - 1; i >= 0; i--)
        {
            if( BT_EVENT_ACL_CONNECTION_COMPLETE == pAclEventHist->btEventType[i] )
            {
                //                                
                if( (i > 0) && (BT_EVENT_CREATE_ACL_CONNECTION == pAclEventHist->btEventType[i - 1]) )
                {
                    fDone = VOS_TRUE;
                    if(i == 1)
                    {
                        //                           
                        btcReleaseAclEventHist(pMac, pAclEventHist);
                        break;
                    }
                    //                                                              
                    pAclEventHist->bNextEventIdx = (tANI_U8)(i - 1);
                    vos_mem_zero(&pAclEventHist->btAclConnection[i-1], sizeof(tSmeBtAclConnectionParam));
                    vos_mem_zero(&pAclEventHist->btAclConnection[i], sizeof(tSmeBtAclConnectionParam));
                    break;
                }
            }
        }//        
        if(!fDone)
        {
            //                          
            if(pAclEventHist->bNextEventIdx < BT_MAX_NUM_EVENT_ACL_DEFERRED)
            {
                pAclEventHist->btEventType[pAclEventHist->bNextEventIdx] = 
                    BT_EVENT_DISCONNECTION_COMPLETE;
                pAclEventHist->btAclConnection[pAclEventHist->bNextEventIdx].connectionHandle =
                    pEvent->uEventParam.btDisconnect.connectionHandle;
                pAclEventHist->bNextEventIdx++;
            }
            else
            {
                smsLog(pMac, LOGE, FL(" ACL event overflow"));
                status = VOS_STATUS_E_FAILURE;
            }
        }
    }
    else
    {
        status = VOS_STATUS_E_EXISTS;
    }
    //                                                     
    pModeChangeEventHist = btcFindModeChangeEventHist( pMac,  
                            pEvent->uEventParam.btDisconnect.connectionHandle );
    if( pModeChangeEventHist && pModeChangeEventHist->fValid )
    {
        pModeChangeEventHist->fValid = VOS_FALSE;
    }
    return status;
}

//                                                                                           
//                                                              
//                                            
//                                                                 
/*
                                   
                                                 
                                                                                                   
                        
                                                     
                                                 
                                                                                         
*/
static VOS_STATUS btcDeferDisconnectEventForSync( tpAniSirGlobal pMac, tpSmeBtEvent pEvent )
{
    VOS_STATUS status = VOS_STATUS_SUCCESS;
    tpSmeBtSyncEventHist pSyncEventHist;
    tpSmeBtSyncUpdateHist pSyncUpdateHist;
    v_BOOL_t fDone = VOS_FALSE;
    int i;
    pSyncEventHist = btcFindSyncEventHist( pMac, NULL, 
                                pEvent->uEventParam.btDisconnect.connectionHandle );
    if(pSyncEventHist)
    {
        if( pSyncEventHist->bNextEventIdx > BT_MAX_NUM_EVENT_SCO_DEFERRED)
        {
            smsLog(pMac, LOGE, FL(" SYNC event history index:%d overflow, resetting to BT_MAX_NUM_EVENT_SCO_DEFERRED"), pSyncEventHist->bNextEventIdx);
            pSyncEventHist->bNextEventIdx = BT_MAX_NUM_EVENT_SCO_DEFERRED;
        }
        //                 
        for(i = pSyncEventHist->bNextEventIdx - 1; i >= 0; i--)
        {
            //                                      
            if( BT_EVENT_SYNC_CONNECTION_COMPLETE == pSyncEventHist->btEventType[i] )
            {
                //                                
                if( (i > 0) && (BT_EVENT_CREATE_SYNC_CONNECTION == pSyncEventHist->btEventType[i - 1]) )
                {
                    fDone = VOS_TRUE;
                    if(i == 1)
                    {
                        //                           
                        btcReleaseSyncEventHist(pMac, pSyncEventHist);
                        break;
                    }
                    //                                                              
                    pSyncEventHist->bNextEventIdx = (tANI_U8)(i - 1);
                    vos_mem_zero(&pSyncEventHist->btSyncConnection[i-1], sizeof(tSmeBtSyncConnectionParam));
                    vos_mem_zero(&pSyncEventHist->btSyncConnection[i], sizeof(tSmeBtSyncConnectionParam));
                    break;
                }
            }
        }//        
        if(!fDone)
        {
            //                          
            if(pSyncEventHist->bNextEventIdx < BT_MAX_NUM_EVENT_SCO_DEFERRED)
            {
                pSyncEventHist->btEventType[pSyncEventHist->bNextEventIdx] = 
                    BT_EVENT_DISCONNECTION_COMPLETE;
                pSyncEventHist->btSyncConnection[pSyncEventHist->bNextEventIdx].connectionHandle =
                    pEvent->uEventParam.btDisconnect.connectionHandle;
                pSyncEventHist->bNextEventIdx++;
            }
            else
            {
                smsLog(pMac, LOGE, FL(" SYNC event overflow"));
                status = VOS_STATUS_E_FAILURE;
            }
        }
    }
    else
    {
        status = VOS_STATUS_E_EXISTS;
    }
    //                                                     
    pSyncUpdateHist = btcFindSyncUpdateEventHist( pMac,  
                            pEvent->uEventParam.btDisconnect.connectionHandle );
    if( pSyncUpdateHist && pSyncUpdateHist->fValid )
    {
        pSyncUpdateHist->fValid = VOS_FALSE;
    }
    return status;
}

/*
                           
                                                  
                                  
                                                            
*/
static VOS_STATUS btcDeferDisconnEvent( tpAniSirGlobal pMac, tpSmeBtEvent pEvent )
{
    VOS_STATUS status = VOS_STATUS_SUCCESS;
    tpSmeBtDisconnectEventHist pDisconnEventHist;
    if( BT_INVALID_CONN_HANDLE == pEvent->uEventParam.btDisconnect.connectionHandle )
    {
        smsLog( pMac, LOGE, FL(" invalid handle") );
        return (VOS_STATUS_E_INVAL);
    }
    //               
    status = btcDeferDisconnectEventForACL(pMac, pEvent);
    if(!VOS_IS_STATUS_SUCCESS(status))
    {
        status = btcDeferDisconnectEventForSync(pMac, pEvent);
    }
    if( !VOS_IS_STATUS_SUCCESS(status) )
    {
        //                         
        pDisconnEventHist = btcFindDisconnEventHist( pMac, 
            pEvent->uEventParam.btDisconnect.connectionHandle );
        if( pDisconnEventHist )
        {
            pDisconnEventHist->fValid = VOS_TRUE;
            vos_mem_copy( &pDisconnEventHist->btDisconnect, &pEvent->uEventParam.btDisconnect,
                sizeof(tSmeBtDisconnectParam) );
            status = VOS_STATUS_SUCCESS;
        }
        else
        {
            smsLog( pMac, LOGE, FL(" cannot find match for BT_EVENT_DISCONNECTION_COMPLETE of handle (%d)"),
                pEvent->uEventParam.btDisconnect.connectionHandle);
            status = VOS_STATUS_E_EMPTY;
        }
    }
    return (status);
}

/*
                                                                              
                                                           
*/
static VOS_STATUS btcDeferEvent( tpAniSirGlobal pMac, tpSmeBtEvent pEvent )
{
    VOS_STATUS status = VOS_STATUS_SUCCESS;
    tpSmeBtSyncUpdateHist pSyncUpdateHist;
    tpSmeBtAclModeChangeEventHist pModeChangeEventHist;
    tSmeBtcEventReplay *pReplay = &pMac->btc.btcEventReplay;
    switch(pEvent->btEventType)
    {
    case BT_EVENT_DEVICE_SWITCHED_ON:
        //                      
        vos_mem_zero( &pReplay->btcEventHist, sizeof(tSmeBtcEventHist) );
        pReplay->fBTSwitchOn = VOS_TRUE;
        pReplay->fBTSwitchOff = VOS_FALSE;
        break;
    case BT_EVENT_DEVICE_SWITCHED_OFF:
        //                      
        vos_mem_zero( &pReplay->btcEventHist, sizeof(tSmeBtcEventHist) );
        pReplay->fBTSwitchOff = VOS_TRUE;
        pReplay->fBTSwitchOn = VOS_FALSE;
        break;
    case BT_EVENT_INQUIRY_STARTED:
        pReplay->btcEventHist.nInquiryEvent++;
        break;
    case BT_EVENT_INQUIRY_STOPPED:
        pReplay->btcEventHist.nInquiryEvent--;
        break;
    case BT_EVENT_PAGE_STARTED:
        pReplay->btcEventHist.nPageEvent++;
        break;
    case BT_EVENT_PAGE_STOPPED:
        pReplay->btcEventHist.nPageEvent--;
        break;
    case BT_EVENT_CREATE_ACL_CONNECTION:
        status = btcDeferAclCreate(pMac, pEvent);
        break;
    case BT_EVENT_ACL_CONNECTION_COMPLETE:
        status = btcDeferAclComplete( pMac, pEvent );
        break;
    case BT_EVENT_CREATE_SYNC_CONNECTION:
        status = btcDeferSyncCreate(pMac, pEvent);
        break;
    case BT_EVENT_SYNC_CONNECTION_COMPLETE:
        status = btcDeferSyncComplete( pMac, pEvent );
        break;
    case BT_EVENT_SYNC_CONNECTION_UPDATED:
        if( BT_INVALID_CONN_HANDLE == pEvent->uEventParam.btDisconnect.connectionHandle )
        {
            smsLog( pMac, LOGE, FL(" invalid handle") );
            status = VOS_STATUS_E_INVAL;
            break;
        }
        //                                                      
        pSyncUpdateHist = btcFindSyncUpdateEventHist( pMac,  
                                    pEvent->uEventParam.btSyncConnection.connectionHandle );
        if(pSyncUpdateHist)
        {
            pSyncUpdateHist->fValid = VOS_TRUE;
            vos_mem_copy(&pSyncUpdateHist->btSyncConnection, &pEvent->uEventParam.btSyncConnection, 
                            sizeof(tSmeBtSyncConnectionParam));
        }
        else
        {
            smsLog( pMac, LOGE, FL(" cannot find match for BT_EVENT_SYNC_CONNECTION_UPDATED of handle (%d)"),
                pEvent->uEventParam.btSyncConnection.connectionHandle );
            status = VOS_STATUS_E_EMPTY;
        }
        break;
    case BT_EVENT_DISCONNECTION_COMPLETE:
        status = btcDeferDisconnEvent( pMac, pEvent );
        break;
    case BT_EVENT_MODE_CHANGED:
        if( BT_INVALID_CONN_HANDLE == pEvent->uEventParam.btDisconnect.connectionHandle )
        {
            smsLog( pMac, LOGE, FL(" invalid handle") );
            status = VOS_STATUS_E_INVAL;
            break;
        }
        //                                                        
        pModeChangeEventHist = btcFindModeChangeEventHist( pMac,  
                                    pEvent->uEventParam.btAclModeChange.connectionHandle );
        if(pModeChangeEventHist)
        {
            pModeChangeEventHist->fValid = VOS_TRUE;
            vos_mem_copy( &pModeChangeEventHist->btAclModeChange,
                            &pEvent->uEventParam.btAclModeChange, sizeof(tSmeBtAclModeChangeParam) );
        }
        else
        {
            smsLog( pMac, LOGE, FL(" cannot find match for BT_EVENT_MODE_CHANGED of handle (%d)"),
                pEvent->uEventParam.btAclModeChange.connectionHandle);
            status = VOS_STATUS_E_EMPTY;
        }
        break;
    case BT_EVENT_A2DP_STREAM_START:
        pReplay->btcEventHist.fA2DPStarted = VOS_TRUE;
        pReplay->btcEventHist.fA2DPStopped = VOS_FALSE;
        break;
    case BT_EVENT_A2DP_STREAM_STOP:
        pReplay->btcEventHist.fA2DPStopped = VOS_TRUE;
        pReplay->btcEventHist.fA2DPStarted = VOS_FALSE;
        break;
    default:
        smsLog( pMac, LOGE, FL(" event (%d) is not deferred"), pEvent->btEventType );
        status = VOS_STATUS_E_NOSUPPORT;
        break;
    }
    return (status);
}

/*
                                                   
                                       
                                                      
                                                    
                                                                       
                                              
                                                                          
                                            
                                                   
                                                                           
                                            
*/
static void btcReplayEvents( tpAniSirGlobal pMac )
{
    int i, j;
    tSmeBtEvent btEvent;
    tpSmeBtAclEventHist pAclHist;
    tpSmeBtSyncEventHist pSyncHist;
    tSmeBtcEventReplay *pReplay = &pMac->btc.btcEventReplay;
    //                                 
    //                                                               
    if( pReplay->fRestoreHBMonitor )
    {
        pReplay->fRestoreHBMonitor = VOS_FALSE;
        //                      
        if( !pMac->btc.btcHBActive ) 
        {
            ccmCfgSetInt(pMac, WNI_CFG_HEART_BEAT_THRESHOLD, pMac->btc.btcHBCount, NULL, eANI_BOOLEAN_FALSE);
            pMac->btc.btcHBActive = VOS_TRUE;
        }
    }
    if( pMac->btc.fReplayBTEvents )
    {
        /*                                                                       
                                                   */
        pMac->btc.fReplayBTEvents = VOS_FALSE;
        if( pReplay->fBTSwitchOff )
        {
            vos_mem_zero( &btEvent, sizeof(tSmeBtEvent) );
            btEvent.btEventType = BT_EVENT_DEVICE_SWITCHED_OFF;
            btcSendBTEvent( pMac, &btEvent );
            pReplay->fBTSwitchOff = VOS_FALSE;
        }
        else if( pReplay->fBTSwitchOn )
        {
            vos_mem_zero( &btEvent, sizeof(tSmeBtEvent) );
            btEvent.btEventType = BT_EVENT_DEVICE_SWITCHED_ON;
            btcSendBTEvent( pMac, &btEvent );
            pReplay->fBTSwitchOn = VOS_FALSE;
        }
        //                
        if( pReplay->btcEventHist.nInquiryEvent > 0 )
        {
            vos_mem_zero( &btEvent, sizeof(tSmeBtEvent) );
            btEvent.btEventType = BT_EVENT_INQUIRY_STARTED;
            i = pReplay->btcEventHist.nInquiryEvent;
            while(i--)
            {
                btcSendBTEvent( pMac, &btEvent );
            }
        }
        else if( pReplay->btcEventHist.nInquiryEvent < 0 )
        {
            vos_mem_zero( &btEvent, sizeof(tSmeBtEvent) );
            btEvent.btEventType = BT_EVENT_INQUIRY_STOPPED;
            i = pReplay->btcEventHist.nInquiryEvent;
            while(i++)
            {
                btcSendBTEvent( pMac, &btEvent );
            }
        }
        //    
        if( pReplay->btcEventHist.nPageEvent > 0 )
        {
            vos_mem_zero( &btEvent, sizeof(tSmeBtEvent) );
            btEvent.btEventType = BT_EVENT_PAGE_STARTED;
            i = pReplay->btcEventHist.nPageEvent;
            while(i--)
            {
                btcSendBTEvent( pMac, &btEvent );
            }
        }
        else if( pReplay->btcEventHist.nPageEvent < 0 )
        {
            vos_mem_zero( &btEvent, sizeof(tSmeBtEvent) );
            btEvent.btEventType = BT_EVENT_PAGE_STOPPED;
            i = pReplay->btcEventHist.nPageEvent;
            while(i++)
            {
                btcSendBTEvent( pMac, &btEvent );
            }
        }
        //                                             
        //          
        for( i = 0; i < BT_MAX_DISCONN_SUPPORT; i++ )
        {
            if( pReplay->btcEventHist.btDisconnectEvent[i].fValid )
            {
                vos_mem_zero( &btEvent, sizeof(tSmeBtEvent) );
                btEvent.btEventType = BT_EVENT_DISCONNECTION_COMPLETE;
                vos_mem_copy( &btEvent.uEventParam.btDisconnect, 
                    &pReplay->btcEventHist.btDisconnectEvent[i].btDisconnect, sizeof(tSmeBtDisconnectParam) );
                btcSendBTEvent( pMac, &btEvent );
            }
        }
        //   
        for( i = 0; i < BT_MAX_ACL_SUPPORT; i++ )
        {
            if( pReplay->btcEventHist.btAclConnectionEvent[i].bNextEventIdx )
            {
                pAclHist = &pReplay->btcEventHist.btAclConnectionEvent[i];
                //                                                
                for(j = 0; j < pAclHist->bNextEventIdx; j++)
                {
                    vos_mem_zero( &btEvent, sizeof(tSmeBtEvent) );
                    vos_mem_zero( &btEvent, sizeof(tSmeBtEvent) );
                    btEvent.btEventType = pAclHist->btEventType[j];
                    if(BT_EVENT_DISCONNECTION_COMPLETE != btEvent.btEventType)
                    {
                        //                                        
                       vos_mem_copy( &btEvent.uEventParam.btAclConnection, 
                                     &pAclHist->btAclConnection[j], sizeof(tSmeBtAclConnectionParam) );
                    }
                    else
                    {
                       btEvent.uEventParam.btDisconnect.connectionHandle = pAclHist->btAclConnection[j].connectionHandle;
                    }
                    btcSendBTEvent( pMac, &btEvent );
                }
            }
        }
        //           
        for( i = 0; i < BT_MAX_ACL_SUPPORT; i++ )
        {
            if( pReplay->btcEventHist.btAclModeChangeEvent[i].fValid )
            {
                vos_mem_zero( &btEvent, sizeof(tSmeBtEvent) );
                btEvent.btEventType = BT_EVENT_MODE_CHANGED;
                vos_mem_copy( &btEvent.uEventParam.btAclModeChange, 
                    &pReplay->btcEventHist.btAclModeChangeEvent[i].btAclModeChange, sizeof(tSmeBtAclModeChangeParam) );
                btcSendBTEvent( pMac, &btEvent );
            }
        }
       //    
        if( pReplay->btcEventHist.fA2DPStarted )
        {
            vos_mem_zero( &btEvent, sizeof(tSmeBtEvent) );
            btEvent.btEventType = BT_EVENT_A2DP_STREAM_START;
            btcSendBTEvent( pMac, &btEvent );
        }
        else if( pReplay->btcEventHist.fA2DPStopped )
        {
            vos_mem_zero( &btEvent, sizeof(tSmeBtEvent) );
            btEvent.btEventType = BT_EVENT_A2DP_STREAM_STOP;
            btcSendBTEvent( pMac, &btEvent );
        }
        //   
        for( i = 0; i < BT_MAX_SCO_SUPPORT; i++ )
        {
            if( pReplay->btcEventHist.btSyncConnectionEvent[i].bNextEventIdx )
            {
                pSyncHist = &pReplay->btcEventHist.btSyncConnectionEvent[i];
                //                                                 
                for(j = 0; j < pSyncHist->bNextEventIdx; j++)
                {
                    vos_mem_zero( &btEvent, sizeof(tSmeBtEvent) );
                    vos_mem_zero( &btEvent, sizeof(tSmeBtEvent) );
                    btEvent.btEventType = pSyncHist->btEventType[j];
                    if(BT_EVENT_DISCONNECTION_COMPLETE != btEvent.btEventType)
                    {
                        //                                       
                       vos_mem_copy( &btEvent.uEventParam.btSyncConnection, 
                                     &pSyncHist->btSyncConnection[j], sizeof(tSmeBtSyncConnectionParam) );
                    }
                    else
                    {
                        btEvent.uEventParam.btDisconnect.connectionHandle = pSyncHist->btSyncConnection[j].connectionHandle;
                    }
                    btcSendBTEvent( pMac, &btEvent );
                }
            }
        }
        //           
        for( i = 0; i < BT_MAX_SCO_SUPPORT; i++ )
        {
            if( pReplay->btcEventHist.btSyncUpdateEvent[i].fValid )
            {
                vos_mem_zero( &btEvent, sizeof(tSmeBtEvent) );
                btEvent.btEventType = BT_EVENT_SYNC_CONNECTION_UPDATED;
                vos_mem_copy( &btEvent.uEventParam.btSyncConnection, 
                            &pReplay->btcEventHist.btSyncUpdateEvent[i].btSyncConnection, 
                            sizeof(tSmeBtSyncConnectionParam) );
                btcSendBTEvent( pMac, &btEvent );
            }
        }
        //                
        vos_mem_zero( &pReplay->btcEventHist, sizeof(tSmeBtcEventHist) );
    }
}

static void btcPowerStateCB( v_PVOID_t pContext, tPmcState pmcState )
{
    tpAniSirGlobal pMac = PMAC_STRUCT(pContext);
    if( FULL_POWER == pmcState )
    {
        btcReplayEvents( pMac );
    }
}

/*                                                                            
                   
                                                      
                                                  
                                                                        
                                                                                  
                                           
                
                                                                             */
static void btcLogEvent (tHalHandle hHal, tpSmeBtEvent pBtEvent)
{
   v_U8_t bdAddrRev[6];
   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, "%s: "
               "Bluetooth Event %d received", __func__, pBtEvent->btEventType);
   switch(pBtEvent->btEventType)
   {
      case BT_EVENT_CREATE_SYNC_CONNECTION:
      case BT_EVENT_SYNC_CONNECTION_COMPLETE:
      case BT_EVENT_SYNC_CONNECTION_UPDATED:
          VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, "SCO Connection: "
               "connectionHandle = %d status = %d linkType %d "
               "scoInterval %d scoWindow %d retransmisisonWindow = %d ",
               pBtEvent->uEventParam.btSyncConnection.connectionHandle,
               pBtEvent->uEventParam.btSyncConnection.status,
               pBtEvent->uEventParam.btSyncConnection.linkType,
               pBtEvent->uEventParam.btSyncConnection.scoInterval,
               pBtEvent->uEventParam.btSyncConnection.scoWindow,
               pBtEvent->uEventParam.btSyncConnection.retransmisisonWindow);

          bdAddrRev[0] = pBtEvent->uEventParam.btSyncConnection.bdAddr[5];
          bdAddrRev[1] = pBtEvent->uEventParam.btSyncConnection.bdAddr[4];
          bdAddrRev[2] = pBtEvent->uEventParam.btSyncConnection.bdAddr[3];
          bdAddrRev[3] = pBtEvent->uEventParam.btSyncConnection.bdAddr[2];
          bdAddrRev[4] = pBtEvent->uEventParam.btSyncConnection.bdAddr[1];
          bdAddrRev[5] = pBtEvent->uEventParam.btSyncConnection.bdAddr[0];

          VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, "BD ADDR = "
                    MAC_ADDRESS_STR, MAC_ADDR_ARRAY(bdAddrRev));
          break;
      case BT_EVENT_CREATE_ACL_CONNECTION:
      case BT_EVENT_ACL_CONNECTION_COMPLETE:
          VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, "ACL Connection: "
               "connectionHandle = %d status = %d ",
               pBtEvent->uEventParam.btAclConnection.connectionHandle,
               pBtEvent->uEventParam.btAclConnection.status);

          bdAddrRev[0] = pBtEvent->uEventParam.btAclConnection.bdAddr[5];
          bdAddrRev[1] = pBtEvent->uEventParam.btAclConnection.bdAddr[4];
          bdAddrRev[2] = pBtEvent->uEventParam.btAclConnection.bdAddr[3];
          bdAddrRev[3] = pBtEvent->uEventParam.btAclConnection.bdAddr[2];
          bdAddrRev[4] = pBtEvent->uEventParam.btAclConnection.bdAddr[1];
          bdAddrRev[5] = pBtEvent->uEventParam.btAclConnection.bdAddr[0];

          VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, "BD ADDR = "
                    MAC_ADDRESS_STR, MAC_ADDR_ARRAY(bdAddrRev));
          break;
      case BT_EVENT_MODE_CHANGED:
          VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, "ACL Mode change : "
               "connectionHandle %d mode %d ",
               pBtEvent->uEventParam.btAclModeChange.connectionHandle,
               pBtEvent->uEventParam.btAclModeChange.mode);
          break;
      case BT_EVENT_DISCONNECTION_COMPLETE:
          VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, "Disconnect Event : "
               "connectionHandle %d ", pBtEvent->uEventParam.btAclModeChange.connectionHandle);
          break;
      default:
         break;
   }
 }

/*
                                                                                 
        
                                              
                                                                       
*/
v_BOOL_t btcIsReadyForUapsd( tHalHandle hHal )
{
    tpAniSirGlobal pMac = PMAC_STRUCT( hHal );
    return( pMac->btc.btcUapsdOk );
}

/*
                                                                               
                                                         
                                                                                     
                                                                                        
                                                    
*/
void btcUapsdCheck( tpAniSirGlobal pMac, tpSmeBtEvent pBtEvent )
{
   v_U8_t i;
   v_BOOL_t fLastUapsdState = pMac->btc.btcUapsdOk, fMoreSCO = VOS_FALSE;
   switch( pBtEvent->btEventType )
   {
   case BT_EVENT_DISCONNECTION_COMPLETE:
       if( (VOS_FALSE == pMac->btc.btcUapsdOk) && 
           BT_INVALID_CONN_HANDLE != pBtEvent->uEventParam.btDisconnect.connectionHandle )
       {
           //                                          
           for(i=0; i < BT_MAX_SCO_SUPPORT; i++)
           {
               if( (BT_INVALID_CONN_HANDLE != pMac->btc.btcScoHandles[i]) &&
                   (pMac->btc.btcScoHandles[i] != pBtEvent->uEventParam.btDisconnect.connectionHandle) )
               {
                   //                                        
                   fMoreSCO = VOS_TRUE;
               }
               else if( pMac->btc.btcScoHandles[i] == pBtEvent->uEventParam.btDisconnect.connectionHandle )
               {
                   pMac->btc.btcScoHandles[i] = BT_INVALID_CONN_HANDLE;
               }
           }
           if( !fMoreSCO && !pMac->btc.fA2DPUp )
           {
               //                       
               pMac->btc.btcUapsdOk = VOS_TRUE;
               smsLog( pMac, LOGE, "BT event (DISCONNECTION) happens, UAPSD-allowed flag (%d) change to TRUE",
                        pBtEvent->btEventType, pMac->btc.btcUapsdOk );
           }
       }
       break;
   case BT_EVENT_DEVICE_SWITCHED_OFF:
       smsLog( pMac, LOGE, "BT event (DEVICE_OFF) happens, UAPSD-allowed flag (%d) change to TRUE",
                        pBtEvent->btEventType, pMac->btc.btcUapsdOk );
       //            
       for(i=0; i < BT_MAX_SCO_SUPPORT; i++)
       {
           pMac->btc.btcScoHandles[i] = BT_INVALID_CONN_HANDLE;
       }
       pMac->btc.fA2DPUp = VOS_FALSE;
       pMac->btc.btcUapsdOk = VOS_TRUE;
       break;
   case BT_EVENT_A2DP_STREAM_STOP:
       smsLog( pMac, LOGE, "BT event  (A2DP_STREAM_STOP) happens, UAPSD-allowed flag (%d)",
            pMac->btc.btcUapsdOk );
       pMac->btc.fA2DPUp = VOS_FALSE;
       //                       
       for(i=0; i < BT_MAX_SCO_SUPPORT; i++)
       {
           if(pMac->btc.btcScoHandles[i] != BT_INVALID_CONN_HANDLE)
           {
              break;
           }
       }
       if( BT_MAX_SCO_SUPPORT == i )
       {
            pMac->btc.fA2DPTrafStop = VOS_TRUE;
           smsLog( pMac, LOGE, "BT_EVENT_A2DP_STREAM_STOP: UAPSD-allowed flag is now %d",
                   pMac->btc.btcUapsdOk );
       }
       break;

   case BT_EVENT_MODE_CHANGED:
       smsLog( pMac, LOGE, "BT event (BT_EVENT_MODE_CHANGED) happens, Mode (%d) UAPSD-allowed flag (%d)",
               pBtEvent->uEventParam.btAclModeChange.mode, pMac->btc.btcUapsdOk );
       if(pBtEvent->uEventParam.btAclModeChange.mode == BT_ACL_SNIFF)
       {
           //                       
           for(i=0; i < BT_MAX_SCO_SUPPORT; i++)
           {
               if(pMac->btc.btcScoHandles[i] != BT_INVALID_CONN_HANDLE)
               {
                   break;
               }
           }
           if( BT_MAX_SCO_SUPPORT == i )
           {
               if(VOS_TRUE == pMac->btc.fA2DPTrafStop)
               {
                   pMac->btc.btcUapsdOk = VOS_TRUE;
                   pMac->btc.fA2DPTrafStop = VOS_FALSE;
               }
               smsLog( pMac, LOGE, "BT_EVENT_MODE_CHANGED with Mode:%d UAPSD-allowed flag is now %d",
                       pBtEvent->uEventParam.btAclModeChange.mode,pMac->btc.btcUapsdOk );
           }
       }
       break;
   case BT_EVENT_CREATE_SYNC_CONNECTION:
       {
           pMac->btc.btcUapsdOk = VOS_FALSE;
           smsLog( pMac, LOGE, "BT_EVENT_CREATE_SYNC_CONNECTION (%d) happens, UAPSD-allowed flag (%d) change to FALSE",
                   pBtEvent->btEventType, pMac->btc.btcUapsdOk );
       }
       break;
   case BT_EVENT_SYNC_CONNECTION_COMPLETE:
       //                         
       if( BT_CONN_STATUS_FAIL != pBtEvent->uEventParam.btSyncConnection.status )
       {
           //                            
           for( i = 0; i < BT_MAX_SCO_SUPPORT; i++)
           {
               VOS_ASSERT(BT_INVALID_CONN_HANDLE != pBtEvent->uEventParam.btSyncConnection.connectionHandle);
               if( (BT_INVALID_CONN_HANDLE == pMac->btc.btcScoHandles[i]) &&
                   (BT_INVALID_CONN_HANDLE != pBtEvent->uEventParam.btSyncConnection.connectionHandle))
               {
                   pMac->btc.btcScoHandles[i] = pBtEvent->uEventParam.btSyncConnection.connectionHandle;
                   break;
               }
           }

           if( i >= BT_MAX_SCO_SUPPORT )
           {
               smsLog(pMac, LOGE, FL("Too many SCO, ignore this one"));
           }
       }
       else
       {
            //                       
           for(i=0; i < BT_MAX_SCO_SUPPORT; i++)
           {
               if(pMac->btc.btcScoHandles[i] != BT_INVALID_CONN_HANDLE)
               {
                   break;
               }
       }
           /*                                         */
           if( (BT_MAX_SCO_SUPPORT == i)  && !pMac->btc.fA2DPUp)           
           {
               pMac->btc.btcUapsdOk = VOS_TRUE;
           }
           smsLog(pMac, LOGE, FL("TSYNC complete failed"));
       }
       break;
   case BT_EVENT_A2DP_STREAM_START:
       smsLog( pMac, LOGE, "BT_EVENT_A2DP_STREAM_START (%d) happens, UAPSD-allowed flag (%d) change to FALSE",
                pBtEvent->btEventType, pMac->btc.btcUapsdOk );
       pMac->btc.fA2DPTrafStop = VOS_FALSE;
       pMac->btc.btcUapsdOk = VOS_FALSE;
       pMac->btc.fA2DPUp = VOS_TRUE;
       break;
   default:
       //                          
       smsLog( pMac, LOGE, "BT event (%d) happens, UAPSD-allowed flag (%d) no change",
                    pBtEvent->btEventType, pMac->btc.btcUapsdOk );
       break;
   }
   if(fLastUapsdState != pMac->btc.btcUapsdOk)
   {
      sme_QosTriggerUapsdChange( pMac );
   }
}

/*                                                                            
                        
                                                  
                                                  
                      
                                        
                                        
                                                                             */
eHalStatus btcHandleCoexInd(tHalHandle hHal, void* pMsg)
{
   tpAniSirGlobal pMac = PMAC_STRUCT( hHal );
   eHalStatus status = eHAL_STATUS_SUCCESS;
   tSirSmeCoexInd *pSmeCoexInd = (tSirSmeCoexInd *)pMsg;

   if (NULL == pMsg)
   {
      smsLog(pMac, LOGE, "in %s msg ptr is NULL", __func__);
      status = eHAL_STATUS_FAILURE;
   }
   else
   {
      //      
      smsLog(pMac, LOG1, "Coex indication in %s(), type %d",
             __func__, pSmeCoexInd->coexIndType);

     //                             
     if (pSmeCoexInd->coexIndType == SIR_COEX_IND_TYPE_DISABLE_HB_MONITOR)
     {
        //                                    
        ccmCfgSetInt(pMac, WNI_CFG_HEART_BEAT_THRESHOLD, 0, NULL, eANI_BOOLEAN_FALSE);
        pMac->btc.btcHBActive = VOS_FALSE;
     }

     //                            
     else if (pSmeCoexInd->coexIndType == SIR_COEX_IND_TYPE_ENABLE_HB_MONITOR)
     {
        if (!pMac->btc.btcHBActive) 
        {
           ccmCfgSetInt(pMac, WNI_CFG_HEART_BEAT_THRESHOLD, pMac->btc.btcHBCount, NULL, eANI_BOOLEAN_FALSE);
           pMac->btc.btcHBActive = VOS_TRUE;
        }
     }
     else if (pSmeCoexInd->coexIndType == SIR_COEX_IND_TYPE_SCAN_COMPROMISED)
     {
         pMac->btc.btcScanCompromise = VOS_TRUE;
         smsLog(pMac, LOGW, "Coex indication in %s(), type - SIR_COEX_IND_TYPE_SCAN_COMPROMISED",
             __func__);
     }
     else if (pSmeCoexInd->coexIndType == SIR_COEX_IND_TYPE_SCAN_NOT_COMPROMISED)
     {
         pMac->btc.btcScanCompromise = VOS_FALSE;
         smsLog(pMac, LOGW, "Coex indication in %s(), type - SIR_COEX_IND_TYPE_SCAN_NOT_COMPROMISED",
             __func__);
     }
     else if (pSmeCoexInd->coexIndType == SIR_COEX_IND_TYPE_DISABLE_AGGREGATION_IN_2p4)
     {
         if (pMac->roam.configParam.disableAggWithBtc)
         {
             ccmCfgSetInt(pMac, WNI_CFG_DEL_ALL_RX_BA_SESSIONS_2_4_G_BTC, 1,
                             NULL, eANI_BOOLEAN_FALSE);
             pMac->btc.btcBssfordisableaggr[0] = pSmeCoexInd->coexIndData[0] & 0xFF;
             pMac->btc.btcBssfordisableaggr[1] = pSmeCoexInd->coexIndData[0] >> 8;
             pMac->btc.btcBssfordisableaggr[2] = pSmeCoexInd->coexIndData[1] & 0xFF;
             pMac->btc.btcBssfordisableaggr[3] = pSmeCoexInd->coexIndData[1]  >> 8;
             pMac->btc.btcBssfordisableaggr[4] = pSmeCoexInd->coexIndData[2] & 0xFF;
             pMac->btc.btcBssfordisableaggr[5] = pSmeCoexInd->coexIndData[2] >> 8;
             smsLog(pMac, LOGW, "Coex indication in %s(), "
                    "type - SIR_COEX_IND_TYPE_DISABLE_AGGREGATION_IN_2p4 "
                    "for BSSID "MAC_ADDRESS_STR,__func__,
                    MAC_ADDR_ARRAY(pMac->btc.btcBssfordisableaggr));
         }
     }
     else if (pSmeCoexInd->coexIndType == SIR_COEX_IND_TYPE_ENABLE_AGGREGATION_IN_2p4)
     {
         if (pMac->roam.configParam.disableAggWithBtc)
         {
             ccmCfgSetInt(pMac, WNI_CFG_DEL_ALL_RX_BA_SESSIONS_2_4_G_BTC, 0,
                             NULL, eANI_BOOLEAN_FALSE);
             smsLog(pMac, LOGW,
             "Coex indication in %s(), type - SIR_COEX_IND_TYPE_ENABLE_AGGREGATION_IN_2p4",
                 __func__);
         }
     }
     //                        
     else
     {
        smsLog(pMac, LOGE, "unknown Coex indication type in %s()", __func__);
     }
   }

   return(status);
}

#ifdef FEATURE_WLAN_DIAG_SUPPORT
/*                                                                            
                       
                                                      
                                                  
                                                                        
                                                                                  
                                           
                
                                                                             */
static void btcDiagEventLog (tHalHandle hHal, tpSmeBtEvent pBtEvent)
{
   //                                        
   WLAN_VOS_DIAG_EVENT_DEF(btDiagEvent, vos_event_wlan_btc_type);
   {
       btDiagEvent.eventId = pBtEvent->btEventType;
       switch(pBtEvent->btEventType)
       {
            case BT_EVENT_CREATE_SYNC_CONNECTION:
            case BT_EVENT_SYNC_CONNECTION_COMPLETE:
            case BT_EVENT_SYNC_CONNECTION_UPDATED:
                btDiagEvent.connHandle = pBtEvent->uEventParam.btSyncConnection.connectionHandle;
                btDiagEvent.connStatus = pBtEvent->uEventParam.btSyncConnection.status;
                btDiagEvent.linkType   = pBtEvent->uEventParam.btSyncConnection.linkType;
                btDiagEvent.scoInterval = pBtEvent->uEventParam.btSyncConnection.scoInterval;
                btDiagEvent.scoWindow  = pBtEvent->uEventParam.btSyncConnection.scoWindow;
                btDiagEvent.retransWindow = pBtEvent->uEventParam.btSyncConnection.retransmisisonWindow;
                vos_mem_copy(btDiagEvent.btAddr, pBtEvent->uEventParam.btSyncConnection.bdAddr,
                              sizeof(btDiagEvent.btAddr));
                break;
            case BT_EVENT_CREATE_ACL_CONNECTION:
            case BT_EVENT_ACL_CONNECTION_COMPLETE:
                btDiagEvent.connHandle = pBtEvent->uEventParam.btAclConnection.connectionHandle;
                btDiagEvent.connStatus = pBtEvent->uEventParam.btAclConnection.status;
                vos_mem_copy(btDiagEvent.btAddr, pBtEvent->uEventParam.btAclConnection.bdAddr,
                             sizeof(btDiagEvent.btAddr));
                break;
            case BT_EVENT_MODE_CHANGED:
                btDiagEvent.connHandle = pBtEvent->uEventParam.btAclModeChange.connectionHandle;
                btDiagEvent.mode = pBtEvent->uEventParam.btAclModeChange.mode;
                break;
            case BT_EVENT_DISCONNECTION_COMPLETE:
                btDiagEvent.connHandle = pBtEvent->uEventParam.btAclModeChange.connectionHandle;
                break;
            default:
                break;
       }
   }
   WLAN_VOS_DIAG_EVENT_REPORT(&btDiagEvent, EVENT_WLAN_BTC);
}
#endif /*                           */
#endif /*                            */

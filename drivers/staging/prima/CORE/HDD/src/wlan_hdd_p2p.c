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

/**========================================================================

  \file  wlan_hdd_p2p.c

  \brief WLAN Host Device Driver implementation for P2P commands interface

  Copyright 2008 (c) Qualcomm, Incorporated.  All Rights Reserved.

  Qualcomm Confidential and Proprietary.

  ========================================================================*/

#include <wlan_hdd_includes.h>
#include <wlan_hdd_hostapd.h>
#include <net/cfg80211.h>
#include "sme_Api.h"
#include "wlan_hdd_p2p.h"
#include "sapApi.h"
#include "wlan_hdd_main.h"

#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/etherdevice.h>
#include <net/ieee80211_radiotap.h>
#ifdef FEATURE_WLAN_TDLS
#include "wlan_hdd_tdls.h"
#endif

//               
#define MS_TO_MUS(x)   ((x)*1000);

#ifdef WLAN_FEATURE_P2P_DEBUG
#define MAX_P2P_ACTION_FRAME_TYPE 9
const char *p2p_action_frame_type[]={"GO Negotiation Request",
                                     "GO Negotiation Response",
                                     "GO Negotiation Confirmation",
                                     "P2P Invitation Request",
                                     "P2P Invitation Response",
                                     "Device Discoverability Request",
                                     "Device Discoverability Response",
                                     "Provision Discovery Request",
                                     "Provision Discovery Response"};

/*                                          
                                          
                                              
                         
 */
tP2PConnectionStatus globalP2PConnectionStatus = P2P_NOT_ACTIVE;

#endif
#ifdef WLAN_FEATURE_TDLS_DEBUG
#define MAX_TDLS_ACTION_FRAME_TYPE 11
const char *tdls_action_frame_type[] = {"TDLS Setup Request",
                                        "TDLS Setup Response",
                                        "TDLS Setup Confirm",
                                        "TDLS Teardown",
                                        "TDLS Peer Traffic Indication",
                                        "TDLS Channel Switch Request",
                                        "TDLS Channel Switch Response",
                                        "TDLS Peer PSM Request",
                                        "TDLS Peer PSM Response",
                                        "TDLS Peer Traffic Response",
                                        "TDLS Discovery Request" };
#endif

extern struct net_device_ops net_ops_struct;

static int hdd_wlan_add_rx_radiotap_hdr( struct sk_buff *skb,
                                         int rtap_len, int flag );

static void hdd_wlan_tx_complete( hdd_adapter_t* pAdapter,
                                  hdd_cfg80211_state_t* cfgState,
                                  tANI_BOOLEAN actionSendSuccess );

static void hdd_sendMgmtFrameOverMonitorIface( hdd_adapter_t *pMonAdapter,
                                               tANI_U32 nFrameLength, 
                                               tANI_U8* pbFrames,
                                               tANI_U8 frameType );

eHalStatus wlan_hdd_remain_on_channel_callback( tHalHandle hHal, void* pCtx,
                                                eHalStatus status )
{
    hdd_adapter_t *pAdapter = (hdd_adapter_t*) pCtx;
    hdd_cfg80211_state_t *cfgState = WLAN_HDD_GET_CFG_STATE_PTR( pAdapter );
    hdd_remain_on_chan_ctx_t *pRemainChanCtx = cfgState->remain_on_chan_ctx;

    if( pRemainChanCtx == NULL )
    {
       hddLog( LOGW,
          "%s: No Rem on channel pending for which Rsp is received", __func__);
       return eHAL_STATUS_SUCCESS;
    }

    hddLog( LOG1, "Received remain on channel rsp");

    cfgState->remain_on_chan_ctx = NULL;

    if( REMAIN_ON_CHANNEL_REQUEST == pRemainChanCtx->rem_on_chan_request )
    {
        if( cfgState->buf )
        {
           hddLog( LOGP, 
                   "%s: We need to receive yet an ack from one of tx packet",
                   __func__);
        }
        cfg80211_remain_on_channel_expired(
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))
                              pRemainChanCtx->dev->ieee80211_ptr,
#else
                              pRemainChanCtx->dev,
#endif
                              pRemainChanCtx->cookie,
                              &pRemainChanCtx->chan,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,8,0))
                              pRemainChanCtx->chan_type,
#endif
                              GFP_KERNEL);
    }


    if ( ( WLAN_HDD_INFRA_STATION == pAdapter->device_mode ) ||
         ( WLAN_HDD_P2P_CLIENT == pAdapter->device_mode ) ||
         ( WLAN_HDD_P2P_DEVICE == pAdapter->device_mode )
       )
    {
        tANI_U8 sessionId = pAdapter->sessionId;
        if( REMAIN_ON_CHANNEL_REQUEST == pRemainChanCtx->rem_on_chan_request )
        {
            sme_DeregisterMgmtFrame(
                       hHal, sessionId,
                      (SIR_MAC_MGMT_FRAME << 2) | ( SIR_MAC_MGMT_PROBE_REQ << 4),
                       NULL, 0 );
        }
    }
    else if ( ( WLAN_HDD_SOFTAP== pAdapter->device_mode ) ||
              ( WLAN_HDD_P2P_GO == pAdapter->device_mode )
            )
    {
        WLANSAP_DeRegisterMgmtFrame(
                (WLAN_HDD_GET_CTX(pAdapter))->pvosContext,
                (SIR_MAC_MGMT_FRAME << 2) | ( SIR_MAC_MGMT_PROBE_REQ << 4),
                NULL, 0 );
    }

    vos_mem_free( pRemainChanCtx );
    pRemainChanCtx = NULL;
    complete(&pAdapter->cancel_rem_on_chan_var);
    return eHAL_STATUS_SUCCESS;
}

void wlan_hdd_cancel_existing_remain_on_channel(hdd_adapter_t *pAdapter)
{
    hdd_cfg80211_state_t *cfgState = WLAN_HDD_GET_CFG_STATE_PTR( pAdapter );
    int status = 0;

    if(cfgState->remain_on_chan_ctx != NULL)
    {
        hddLog( LOG1, "Cancel Existing Remain on Channel");

        /*                                                                    
                                                                         
                                                                               
                                    
         */
        status = wait_for_completion_interruptible_timeout(&pAdapter->rem_on_chan_ready_event,
               msecs_to_jiffies(WAIT_REM_CHAN_READY));
        if (!status)
        {
            hddLog( LOGE, 
                    "%s: timeout waiting for remain on channel ready indication",
                    __func__);
        }

        INIT_COMPLETION(pAdapter->cancel_rem_on_chan_var);
        
        /*                                           
                                                                           
                                 
        */
        if ( ( WLAN_HDD_INFRA_STATION == pAdapter->device_mode ) ||
             ( WLAN_HDD_P2P_CLIENT == pAdapter->device_mode ) ||
             ( WLAN_HDD_P2P_DEVICE == pAdapter->device_mode )
           )
        {
            sme_CancelRemainOnChannel( WLAN_HDD_GET_HAL_CTX( pAdapter ),
                                                     pAdapter->sessionId );
        }
        else if ( (WLAN_HDD_SOFTAP== pAdapter->device_mode) ||
                  (WLAN_HDD_P2P_GO == pAdapter->device_mode)
                )
        {
            WLANSAP_CancelRemainOnChannel(
                                     (WLAN_HDD_GET_CTX(pAdapter))->pvosContext);
        }

        status = wait_for_completion_interruptible_timeout(&pAdapter->cancel_rem_on_chan_var,
               msecs_to_jiffies(WAIT_CANCEL_REM_CHAN));

        if (!status)
        {
            hddLog( LOGE, 
                    "%s: timeout waiting for cancel remain on channel ready indication",
                    __func__);
        }
    }
}

int wlan_hdd_check_remain_on_channel(hdd_adapter_t *pAdapter)
{
   int status = 0;
   hdd_cfg80211_state_t *cfgState = WLAN_HDD_GET_CFG_STATE_PTR( pAdapter );

   if(WLAN_HDD_P2P_GO != pAdapter->device_mode)
   {
     //                                 
     //                             
     if( cfgState->remain_on_chan_ctx != NULL)
     {
        //                                            
        if( cfgState->buf == NULL)
        {
           wlan_hdd_cancel_existing_remain_on_channel(pAdapter);
        }
        else
        {
           hddLog( LOG1, "Cannot Cancel Existing Remain on Channel");
           status = -EBUSY;
        }
     }
   }
   return status;
}

static int wlan_hdd_request_remain_on_channel( struct wiphy *wiphy,
                                   struct net_device *dev,
                                   struct ieee80211_channel *chan,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,8,0))
                                   enum nl80211_channel_type channel_type,
#endif
                                   unsigned int duration, u64 *cookie,
                                   rem_on_channel_request_type_t request_type )
{
    hdd_adapter_t *pAdapter = WLAN_HDD_GET_PRIV_PTR(dev);
    hdd_remain_on_chan_ctx_t *pRemainChanCtx;
    hdd_cfg80211_state_t *cfgState = WLAN_HDD_GET_CFG_STATE_PTR( pAdapter );
    hddLog(VOS_TRACE_LEVEL_INFO, "%s: device_mode = %d",
                                 __func__,pAdapter->device_mode);

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,8,0))
    hddLog( LOG1,
        "chan(hw_val)0x%x chan(centerfreq) %d chan type 0x%x, duration %d",
        chan->hw_value, chan->center_freq, channel_type, duration );
#else
    hddLog( LOG1,
        "chan(hw_val)0x%x chan(centerfreq) %d, duration %d",
        chan->hw_value, chan->center_freq, duration );
#endif
    //                                        
    wlan_hdd_cancel_existing_remain_on_channel(pAdapter);

    /*                                                           
                                                                     
                                                                     
                                                      */
    if (WLAN_HDD_IS_LOAD_UNLOAD_IN_PROGRESS(((hdd_context_t *)pAdapter->pHddCtx)))
    {
        hddLog( LOGE,
                "%s: Wlan Load/Unload is in progress", __func__);
        return -EBUSY;
    }

    if (((hdd_context_t*)pAdapter->pHddCtx)->isLogpInProgress)
    {
        VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                "%s:LOGP in Progress. Ignore!!!", __func__);
        return -EAGAIN;
    }
    pRemainChanCtx = vos_mem_malloc( sizeof(hdd_remain_on_chan_ctx_t) );
    if( NULL == pRemainChanCtx )
    {
        hddLog(VOS_TRACE_LEVEL_FATAL,
             "%s: Not able to allocate memory for Channel context",
                                         __func__);
        return -ENOMEM;
    }

    vos_mem_copy( &pRemainChanCtx->chan, chan,
                   sizeof(struct ieee80211_channel) );

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,8,0))
    pRemainChanCtx->chan_type = channel_type;
#endif
    pRemainChanCtx->duration = duration;
    pRemainChanCtx->dev = dev;
    *cookie = (tANI_U32) pRemainChanCtx;
    pRemainChanCtx->cookie = *cookie;
    pRemainChanCtx->rem_on_chan_request = request_type;
    cfgState->remain_on_chan_ctx = pRemainChanCtx;
    cfgState->current_freq = chan->center_freq;
    
    INIT_COMPLETION(pAdapter->rem_on_chan_ready_event);

    //                                        
    if ( ( WLAN_HDD_INFRA_STATION == pAdapter->device_mode ) ||
         ( WLAN_HDD_P2P_CLIENT == pAdapter->device_mode ) ||
         ( WLAN_HDD_P2P_DEVICE == pAdapter->device_mode )
       )
    {
        tANI_U8 sessionId = pAdapter->sessionId;
        //                                        
        sme_RemainOnChannel(
                       WLAN_HDD_GET_HAL_CTX(pAdapter), sessionId,
                       chan->hw_value, duration,
                       wlan_hdd_remain_on_channel_callback, pAdapter,
                       (tANI_U8)(request_type == REMAIN_ON_CHANNEL_REQUEST)? TRUE:FALSE);

        if( REMAIN_ON_CHANNEL_REQUEST == request_type)
        {
            sme_RegisterMgmtFrame(WLAN_HDD_GET_HAL_CTX(pAdapter),
                                   sessionId, (SIR_MAC_MGMT_FRAME << 2) |
                                  (SIR_MAC_MGMT_PROBE_REQ << 4), NULL, 0 );
        }

    }
    else if ( ( WLAN_HDD_SOFTAP== pAdapter->device_mode ) ||
              ( WLAN_HDD_P2P_GO == pAdapter->device_mode )
            )
    {
        //                                        
        if (VOS_STATUS_SUCCESS != WLANSAP_RemainOnChannel(
                          (WLAN_HDD_GET_CTX(pAdapter))->pvosContext,
                          chan->hw_value, duration,
                          wlan_hdd_remain_on_channel_callback, pAdapter ))

        {
           VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                    "%s: WLANSAP_RemainOnChannel returned fail", __func__);
           cfgState->remain_on_chan_ctx = NULL;
           vos_mem_free (pRemainChanCtx);
           return -EINVAL;
        }


        if (VOS_STATUS_SUCCESS != WLANSAP_RegisterMgmtFrame(
                    (WLAN_HDD_GET_CTX(pAdapter))->pvosContext,
                    (SIR_MAC_MGMT_FRAME << 2) | ( SIR_MAC_MGMT_PROBE_REQ << 4),
                    NULL, 0 ))
        {
            VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                    "%s: WLANSAP_RegisterMgmtFrame returned fail", __func__);
            WLANSAP_CancelRemainOnChannel(
                    (WLAN_HDD_GET_CTX(pAdapter))->pvosContext);
            return -EINVAL;
        }

    }
    return 0;

}

int wlan_hdd_cfg80211_remain_on_channel( struct wiphy *wiphy,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))
                                struct wireless_dev *wdev,
#else
                                struct net_device *dev,
#endif
                                struct ieee80211_channel *chan,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,8,0))
                                enum nl80211_channel_type channel_type,
#endif
                                unsigned int duration, u64 *cookie )
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))
    struct net_device *dev = wdev->netdev;
#endif
    return wlan_hdd_request_remain_on_channel(wiphy, dev,
                                        chan,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,8,0))
                                        channel_type,
#endif
                                        duration, cookie,
                                        REMAIN_ON_CHANNEL_REQUEST);
}

void hdd_remainChanReadyHandler( hdd_adapter_t *pAdapter )
{
    hdd_cfg80211_state_t *cfgState = WLAN_HDD_GET_CFG_STATE_PTR( pAdapter );
    hdd_remain_on_chan_ctx_t* pRemainChanCtx = cfgState->remain_on_chan_ctx;

    hddLog( LOG1, "Ready on chan ind");

    if( pRemainChanCtx != NULL )
    {
        if( REMAIN_ON_CHANNEL_REQUEST == pRemainChanCtx->rem_on_chan_request )
        {
            cfg80211_ready_on_channel(
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))
                               pAdapter->dev->ieee80211_ptr,
#else
                               pAdapter->dev,
#endif
                               (tANI_U32)pRemainChanCtx,
                               &pRemainChanCtx->chan,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,8,0))
                               pRemainChanCtx->chan_type,
#endif
                               pRemainChanCtx->duration, GFP_KERNEL );
        }
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,38))
        else if( OFF_CHANNEL_ACTION_TX == pRemainChanCtx->rem_on_chan_request )
        {
            complete(&pAdapter->offchannel_tx_event);
        }
#endif
        complete(&pAdapter->rem_on_chan_ready_event);
    }
    else
    {
        hddLog( LOGW, "%s: No Pending Remain on channel Request", __func__);
    }
    return;
}

int wlan_hdd_cfg80211_cancel_remain_on_channel( struct wiphy *wiphy,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))
                                                struct wireless_dev *wdev,
#else
                                                struct net_device *dev,
#endif
                                                u64 cookie )
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))
    struct net_device *dev = wdev->netdev;
#endif
    hdd_adapter_t *pAdapter = WLAN_HDD_GET_PRIV_PTR(dev);
    hdd_cfg80211_state_t *cfgState = WLAN_HDD_GET_CFG_STATE_PTR( pAdapter );
    hdd_context_t *pHddCtx = WLAN_HDD_GET_CTX( pAdapter );
    int status;

    hddLog( LOG1, "Cancel remain on channel req");

    status = wlan_hdd_validate_context(pHddCtx);

    if (0 != status)
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                   "%s: HDD context is not valid", __func__);
        return status;
    }
    /*                                               
                                                  
     */
    if( (cfgState->remain_on_chan_ctx == NULL) ||
        (cfgState->remain_on_chan_ctx->cookie != cookie) )
    {
        hddLog( LOGE,
            "%s: No Remain on channel pending with specified cookie value",
             __func__);
        return -EINVAL;
    }

    /*                                                  
                                                    */
    status = wait_for_completion_interruptible_timeout(&pAdapter->rem_on_chan_ready_event,
            msecs_to_jiffies(WAIT_REM_CHAN_READY));
    if (!status)
    {
        hddLog( LOGE, 
                "%s: timeout waiting for remain on channel ready indication",
                __func__);
    }
    INIT_COMPLETION(pAdapter->cancel_rem_on_chan_var);
    /*                                           
                                                                       
                             
     */
    if ( ( WLAN_HDD_INFRA_STATION == pAdapter->device_mode ) ||
         ( WLAN_HDD_P2P_CLIENT == pAdapter->device_mode ) ||
         ( WLAN_HDD_P2P_DEVICE == pAdapter->device_mode )
       )
    {
        tANI_U8 sessionId = pAdapter->sessionId; 
        sme_CancelRemainOnChannel( WLAN_HDD_GET_HAL_CTX( pAdapter ),
                                            sessionId );
    }
    else if ( (WLAN_HDD_SOFTAP== pAdapter->device_mode) ||
              (WLAN_HDD_P2P_GO == pAdapter->device_mode)
            )
    {
        WLANSAP_CancelRemainOnChannel(
                                (WLAN_HDD_GET_CTX(pAdapter))->pvosContext);
    }
    else 
    {
       hddLog(VOS_TRACE_LEVEL_ERROR, "%s: Invalid device_mode = %d",
                            __func__, pAdapter->device_mode);
       return -EIO;
    }
    wait_for_completion_interruptible_timeout(&pAdapter->cancel_rem_on_chan_var,
            msecs_to_jiffies(WAIT_CANCEL_REM_CHAN));
    return 0;
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))
int wlan_hdd_action( struct wiphy *wiphy, struct wireless_dev *wdev,
                     struct ieee80211_channel *chan, bool offchan,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,8,0))
                     enum nl80211_channel_type channel_type,
                     bool channel_type_valid,
#endif
                     unsigned int wait,
                     const u8 *buf, size_t len,  bool no_cck,
                     bool dont_wait_for_ack, u64 *cookie )
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(3,3,0))
int wlan_hdd_action( struct wiphy *wiphy, struct net_device *dev,
                     struct ieee80211_channel *chan, bool offchan,
                     enum nl80211_channel_type channel_type,
                     bool channel_type_valid, unsigned int wait,
                     const u8 *buf, size_t len,  bool no_cck,
                     bool dont_wait_for_ack, u64 *cookie )
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,38))
int wlan_hdd_action( struct wiphy *wiphy, struct net_device *dev,
                     struct ieee80211_channel *chan, bool offchan,
                     enum nl80211_channel_type channel_type,
                     bool channel_type_valid, unsigned int wait,
                     const u8 *buf, size_t len, u64 *cookie )
#else
int wlan_hdd_action( struct wiphy *wiphy, struct net_device *dev,
                     struct ieee80211_channel *chan,
                     enum nl80211_channel_type channel_type,
                     bool channel_type_valid,
                     const u8 *buf, size_t len, u64 *cookie )
#endif //                  
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))
    struct net_device *dev = wdev->netdev;
#endif
    hdd_adapter_t *pAdapter = WLAN_HDD_GET_PRIV_PTR( dev );
    hdd_cfg80211_state_t *cfgState = WLAN_HDD_GET_CFG_STATE_PTR( pAdapter );
    hdd_context_t *pHddCtx = WLAN_HDD_GET_CTX( pAdapter );
    tANI_U16 extendedWait = 0;
    tANI_U8 type = WLAN_HDD_GET_TYPE_FRM_FC(buf[0]);
    tANI_U8 subType = WLAN_HDD_GET_SUBTYPE_FRM_FC(buf[0]);
    tActionFrmType actionFrmType;
    bool noack = 0;
    int status;
#ifdef WLAN_FEATURE_11W
    tANI_U8 *pTxFrmBuf = (tANI_U8 *) buf; //                                         
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,38))
    hdd_adapter_t *goAdapter;
#endif

    status = wlan_hdd_validate_context(pHddCtx);

    if (0 != status)
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                   "%s: HDD context is not valid", __func__);
        return status;
    }

#ifdef WLAN_FEATURE_P2P_DEBUG
    if ((type == SIR_MAC_MGMT_FRAME) &&
            (subType == SIR_MAC_MGMT_ACTION) &&
            (buf[WLAN_HDD_PUBLIC_ACTION_FRAME_OFFSET] == WLAN_HDD_PUBLIC_ACTION_FRAME))
    {
        actionFrmType = buf[WLAN_HDD_PUBLIC_ACTION_FRAME_TYPE_OFFSET];
        if(actionFrmType >= MAX_P2P_ACTION_FRAME_TYPE)
        {
            hddLog(VOS_TRACE_LEVEL_ERROR,"[P2P] unknown[%d] ---> OTA",
                                   actionFrmType);
        }
        else
        {
            hddLog(VOS_TRACE_LEVEL_ERROR,"[P2P] %s ---> OTA",
            p2p_action_frame_type[actionFrmType]);
            if( (actionFrmType == WLAN_HDD_PROV_DIS_REQ) &&
                (globalP2PConnectionStatus == P2P_NOT_ACTIVE) )
            {
                 globalP2PConnectionStatus = P2P_GO_NEG_PROCESS;
                 hddLog(LOGE,"[P2P State]Inactive state to "
                            "GO negotiation progress state");
            }
            else if( (actionFrmType == WLAN_HDD_GO_NEG_CNF) &&
                (globalP2PConnectionStatus == P2P_GO_NEG_PROCESS) )
            {
                 globalP2PConnectionStatus = P2P_GO_NEG_COMPLETED;
                 hddLog(LOGE,"[P2P State]GO nego progress to GO nego"
                             " completed state");
            }
        }
    }
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,3,0))
    noack = dont_wait_for_ack;
#endif

    //                                               
    //                           
    if (offchan && !wait)
        wait = ACTION_FRAME_DEFAULT_WAIT;

    hddLog(VOS_TRACE_LEVEL_INFO, "%s: device_mode = %d",
                            __func__,pAdapter->device_mode);

    //                                        
    //                                               
    //                                         
    if ( ( WLAN_HDD_SOFTAP == pAdapter->device_mode ) ||
         ( WLAN_HDD_P2P_GO == pAdapter->device_mode )
       )
    {
        if (type == SIR_MAC_MGMT_FRAME)
        {
            if (subType == SIR_MAC_MGMT_PROBE_RSP)
            {
                /*                                                             
                                                     
                   */ 
                goto err_rem_channel;
            }
            else if ((subType == SIR_MAC_MGMT_DISASSOC) ||
                    (subType == SIR_MAC_MGMT_DEAUTH))
            {
                /*                                                  
                                                                 
                                                                         
                                                                        
                                                                        
                              
                 */
                tANI_U8 dstMac[ETH_ALEN] = {0};
                memcpy(&dstMac, &buf[WLAN_HDD_80211_FRM_DA_OFFSET], ETH_ALEN);
                hddLog(VOS_TRACE_LEVEL_INFO,
                        "%s: Deauth/Disassoc received for STA:"
                        MAC_ADDRESS_STR,
                        __func__,
                        MAC_ADDR_ARRAY(dstMac));
                goto err_rem_channel;
            }
        }
    }

    if( NULL != cfgState->buf )
    {
        if ( !noack )
        {
            hddLog( LOGE, "(%s):Previous P2P Action frame packet pending",
                          __func__);
            hdd_cleanup_actionframe(pAdapter->pHddCtx, pAdapter);
        }
        else
        {
            hddLog( LOGE, "(%s):Pending Action frame packet return EBUSY",
                          __func__);
            return -EBUSY;
        }
    }

    hddLog( LOG1, "Action frame tx request");

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,38))
    goAdapter = hdd_get_adapter( pAdapter->pHddCtx, WLAN_HDD_P2P_GO );

    //                                                    
    //                                          
    if( goAdapter && ( ieee80211_frequency_to_channel(chan->center_freq)
                         == goAdapter->sessionCtx.ap.operatingChannel ) )
    {
        goto send_frame;
    }
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,38))
    if( offchan && wait)
    {
        int status;

        //                                             
        //                                                 

        if((cfgState->remain_on_chan_ctx != NULL) &&
           (cfgState->current_freq == chan->center_freq)
          )
        {
            hddLog(LOG1,"action frame: extending the wait time\n");
            extendedWait = (tANI_U16)wait;
            goto send_frame;
        }

        INIT_COMPLETION(pAdapter->offchannel_tx_event);

        status = wlan_hdd_request_remain_on_channel(wiphy, dev,
                                        chan,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,8,0))
                                        channel_type,
#endif
                                        wait, cookie,
                                        OFF_CHANNEL_ACTION_TX);

        if(0 != status)
        {
            if( (-EBUSY == status) &&
                (cfgState->current_freq == chan->center_freq) )
            {
                goto send_frame;
            }
            goto err_rem_channel;
        }
        /*                                                            
                                                                       
                           
         */
        extendedWait = (tANI_U16)wait;
        /*                                                      */
        status = wait_for_completion_interruptible_timeout(
                     &pAdapter->offchannel_tx_event,
                     msecs_to_jiffies(WAIT_CHANGE_CHANNEL_FOR_OFFCHANNEL_TX));
        if(!status)
        {
            hddLog( LOGE, "Not able to complete remain on channel request"
                          " within timeout period");
            goto err_rem_channel;
        }
    }
    else if ( offchan )
    {
        /*                                  
                                                */
        if(NULL == cfgState->remain_on_chan_ctx)
        {
            goto err_rem_channel;
        }
    }
    send_frame:
#endif

    if(!noack)
    {
        cfgState->buf = vos_mem_malloc( len ); //    
        if( cfgState->buf == NULL )
            return -ENOMEM;

        cfgState->len = len;

        vos_mem_copy( cfgState->buf, buf, len);

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,38))
        if( cfgState->remain_on_chan_ctx )
        {
            cfgState->action_cookie = cfgState->remain_on_chan_ctx->cookie;
            *cookie = cfgState->action_cookie;
        }
        else
        {
#endif
            *cookie = (tANI_U32) cfgState->buf;
            cfgState->action_cookie = *cookie;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,38))
        }
#endif
    } 

    if ( (WLAN_HDD_INFRA_STATION == pAdapter->device_mode) ||
         (WLAN_HDD_P2P_CLIENT == pAdapter->device_mode) ||
         ( WLAN_HDD_P2P_DEVICE == pAdapter->device_mode )
       )
    {
        tANI_U8 sessionId = pAdapter->sessionId;

        if ((type == SIR_MAC_MGMT_FRAME) &&
                (subType == SIR_MAC_MGMT_ACTION) &&
                (buf[WLAN_HDD_PUBLIC_ACTION_FRAME_OFFSET] == WLAN_HDD_PUBLIC_ACTION_FRAME))
        {
            actionFrmType = buf[WLAN_HDD_PUBLIC_ACTION_FRAME_TYPE_OFFSET];
            hddLog(LOG1, "Tx Action Frame %u \n", actionFrmType);
            if (actionFrmType == WLAN_HDD_PROV_DIS_REQ)
            {
                cfgState->actionFrmState = HDD_PD_REQ_ACK_PENDING;
                hddLog(LOG1, "%s: HDD_PD_REQ_ACK_PENDING \n", __func__);
            }
            else if (actionFrmType == WLAN_HDD_GO_NEG_REQ)
            {
                cfgState->actionFrmState = HDD_GO_NEG_REQ_ACK_PENDING;
                hddLog(LOG1, "%s: HDD_GO_NEG_REQ_ACK_PENDING \n", __func__);
            }
        }
#ifdef WLAN_FEATURE_11W
        if ((type == SIR_MAC_MGMT_FRAME) &&
                (subType == SIR_MAC_MGMT_ACTION) &&
                (buf[WLAN_HDD_PUBLIC_ACTION_FRAME_OFFSET] == WLAN_HDD_SA_QUERY_ACTION_FRAME))
        {
            VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                     "%s: Calling sme_sendAction. For Category %s", __func__, "SA Query");
            //                                                              
            WLAN_HDD_SET_WEP_FRM_FC(pTxFrmBuf[1]);
        }
#endif
        if (eHAL_STATUS_SUCCESS !=
               sme_sendAction( WLAN_HDD_GET_HAL_CTX(pAdapter),
                               sessionId, buf, len, extendedWait, noack))
        {
            VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                     "%s: sme_sendAction returned fail", __func__);
            goto err;
        }
    }
    else if( ( WLAN_HDD_SOFTAP== pAdapter->device_mode ) ||
              ( WLAN_HDD_P2P_GO == pAdapter->device_mode )
            )
     {
        if( VOS_STATUS_SUCCESS !=
             WLANSAP_SendAction( (WLAN_HDD_GET_CTX(pAdapter))->pvosContext,
                                  buf, len, 0 ) )
        {
            VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                    "%s: WLANSAP_SendAction returned fail", __func__);
            goto err;
        }
    }

    return 0;
err:
    if(!noack)
    {
       hdd_sendActionCnf( pAdapter, FALSE );
    }
    return 0;
err_rem_channel:
    *cookie = (tANI_U32)cfgState;
    cfg80211_mgmt_tx_status(
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))
                            pAdapter->dev->ieee80211_ptr,
#else
                            pAdapter->dev,
#endif
                            *cookie, buf, len, FALSE, GFP_KERNEL );
    return 0;
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,38))
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))
int wlan_hdd_cfg80211_mgmt_tx_cancel_wait(struct wiphy *wiphy,
                                          struct wireless_dev *wdev,
                                          u64 cookie)
{
    return wlan_hdd_cfg80211_cancel_remain_on_channel( wiphy, wdev, cookie );
}
#else
int wlan_hdd_cfg80211_mgmt_tx_cancel_wait(struct wiphy *wiphy,
                                          struct net_device *dev,
                                          u64 cookie)
{
    return wlan_hdd_cfg80211_cancel_remain_on_channel( wiphy, dev, cookie );
}
#endif
#endif

void hdd_sendActionCnf( hdd_adapter_t *pAdapter, tANI_BOOLEAN actionSendSuccess )
{
    hdd_cfg80211_state_t *cfgState = WLAN_HDD_GET_CFG_STATE_PTR( pAdapter );

    cfgState->actionFrmState = HDD_IDLE;

    hddLog( LOG1, "Send Action cnf, actionSendSuccess %d", actionSendSuccess);
    if( NULL == cfgState->buf )
    {
        return;
    }

    /*                                                                       
                                                 */
    if( cfgState->skb == NULL )
    {
        /*
                                                                             
                                                             
                                                                              
                                            
           */
         cfg80211_mgmt_tx_status(
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))
                pAdapter->dev->ieee80211_ptr,
#else
                pAdapter->dev,
#endif
                cfgState->action_cookie,
                cfgState->buf, cfgState->len, actionSendSuccess, GFP_KERNEL );
         vos_mem_free( cfgState->buf );
         cfgState->buf = NULL;
    }
    else
    {
        hdd_adapter_t* pMonAdapter =
                    hdd_get_adapter( pAdapter->pHddCtx, WLAN_HDD_MONITOR );
        if( pMonAdapter == NULL )
        {
            hddLog( LOGE, "Not able to get Monitor Adapter");
            cfgState->skb = NULL;
            vos_mem_free( cfgState->buf );
            cfgState->buf = NULL;
            complete(&pAdapter->tx_action_cnf_event);
            return;
        }
        /*                                                     */
        hdd_wlan_tx_complete( pMonAdapter, cfgState, actionSendSuccess );
        cfgState->skb = NULL;
        vos_mem_free( cfgState->buf );
        cfgState->buf = NULL;
        /*                                     */
        hdd_mon_tx_mgmt_pkt(pAdapter);
    }
    complete(&pAdapter->tx_action_cnf_event);
}

/* 
                
  
           
                                                                       
                                                                 
  
        
                                                                               
  
              
  
  
       
  
                                                      
                                          
  
                 
 */

int hdd_setP2pNoa( struct net_device *dev, tANI_U8 *command )
{
    hdd_adapter_t *pAdapter = WLAN_HDD_GET_PRIV_PTR(dev);
    tHalHandle hHal = WLAN_HDD_GET_HAL_CTX(pAdapter);
    VOS_STATUS status = VOS_STATUS_SUCCESS;
    tP2pPsConfig NoA;
    int count, duration, start_time;
    char *param;
    tANI_U8 ret = 0;

    param = strnchr(command, strlen(command), ' ');
    if (param == NULL)
      return -EINVAL;
    param++;
    ret = sscanf(param, "%d %d %d", &count, &start_time, &duration);
    if (ret < 3)
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
               "%s: P2P_SET GO NoA: fail to read param "
               "count=%d duration=%d interval=%d \n",
                __func__, count, start_time, duration);
        return -EINVAL;
    }
    VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO,
               "%s: P2P_SET GO NoA: count=%d duration=%d interval=%d \n",
                __func__, count, start_time, duration);
    duration = MS_TO_MUS(duration);
    /*             
                       
                       
     */
    NoA.opp_ps = 0;
    NoA.ctWindow = 0;
    if (count == 1)
    {
        NoA.duration = 0;
        NoA.single_noa_duration = duration;
        NoA.psSelection = P2P_POWER_SAVE_TYPE_SINGLE_NOA;
    }
    else
    {
        NoA.duration = duration;
        NoA.single_noa_duration = 0;
        NoA.psSelection = P2P_POWER_SAVE_TYPE_PERIODIC_NOA;
    }
    NoA.interval = MS_TO_MUS(100);
    NoA.count = count;
    NoA.sessionid = pAdapter->sessionId;

    VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO,
                "%s: P2P_PS_ATTR:oppPS %d ctWindow %d duration %d "
                "interval %d count %d single noa duration %d "
                "PsSelection %x \n", __func__, NoA.opp_ps, 
                NoA.ctWindow, NoA.duration, NoA.interval, 
                NoA.count, NoA.single_noa_duration,
                NoA.psSelection);

    sme_p2pSetPs(hHal, &NoA);
    return status;
}

/* 
                 
  
           
                                                                      
                                                                
  
        
                                                                              
  
              
  
  
       
  
                                                      
                                         
  
                 
 */

int hdd_setP2pOpps( struct net_device *dev, tANI_U8 *command )
{
    hdd_adapter_t *pAdapter = WLAN_HDD_GET_PRIV_PTR(dev);
    tHalHandle hHal = WLAN_HDD_GET_HAL_CTX(pAdapter);
    VOS_STATUS status = VOS_STATUS_SUCCESS;
    tP2pPsConfig NoA;
    char *param;
    int legacy_ps, opp_ps, ctwindow;
    tANI_U8 ret = 0;

    param = strnchr(command, strlen(command), ' ');
    if (param == NULL)
        return -EINVAL;
    param++;
    ret = sscanf(param, "%d %d %d", &legacy_ps, &opp_ps, &ctwindow);
    if (ret < 3)
    {
        VOS_TRACE (VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                 "%s: P2P_SET GO PS: fail to read param "
                 " legacy_ps=%d opp_ps=%d ctwindow=%d \n",
                 __func__, legacy_ps, opp_ps, ctwindow);
        return -EINVAL;
    }
    VOS_TRACE (VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO,
                 "%s: P2P_SET GO PS: legacy_ps=%d opp_ps=%d ctwindow=%d \n",
                 __func__, legacy_ps, opp_ps, ctwindow);

    /*             
                                   
     */

    /*                                                                        
                                                                       
                                                    
                                                                          
                           
                                                          
                                                                       
     */ 
    if (ctwindow != -1)
    {

        VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO,
                    "Opportunistic Power Save is %s \n", 
                    (TRUE == pAdapter->ops) ? "Enable" : "Disable" );

        if (ctwindow != pAdapter->ctw)
        {
            pAdapter->ctw = ctwindow;
        
            if(pAdapter->ops)
            {
                NoA.opp_ps = pAdapter->ops;
                NoA.ctWindow = pAdapter->ctw;
                NoA.duration = 0;
                NoA.single_noa_duration = 0;
                NoA.interval = 0;
                NoA.count = 0;
                NoA.psSelection = P2P_POWER_SAVE_TYPE_OPPORTUNISTIC;
                NoA.sessionid = pAdapter->sessionId;
 
                VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO,
                            "%s: P2P_PS_ATTR:oppPS %d ctWindow %d duration %d "
                            "interval %d count %d single noa duration %d "
                            "PsSelection %x \n", __func__, NoA.opp_ps, 
                            NoA.ctWindow, NoA.duration, NoA.interval, 
                            NoA.count, NoA.single_noa_duration,
                            NoA.psSelection);

               sme_p2pSetPs(hHal, &NoA);
           }
           return 0;
        }
    }

    if (opp_ps != -1)
    {
        pAdapter->ops = opp_ps;

        if ((opp_ps != -1) && (pAdapter->ctw)) 
        {
            NoA.opp_ps = opp_ps;
            NoA.ctWindow = pAdapter->ctw;
            NoA.duration = 0;
            NoA.single_noa_duration = 0;
            NoA.interval = 0;
            NoA.count = 0;
            NoA.psSelection = P2P_POWER_SAVE_TYPE_OPPORTUNISTIC;
            NoA.sessionid = pAdapter->sessionId;

            VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO,
                        "%s: P2P_PS_ATTR:oppPS %d ctWindow %d duration %d "
                        "interval %d count %d single noa duration %d "
                        "PsSelection %x \n", __func__, NoA.opp_ps, 
                        NoA.ctWindow, NoA.duration, NoA.interval, 
                        NoA.count, NoA.single_noa_duration,
                        NoA.psSelection);

           sme_p2pSetPs(hHal, &NoA);
        }
    }
    return status;
}

int hdd_setP2pPs( struct net_device *dev, void *msgData )
{
    hdd_adapter_t *pAdapter = WLAN_HDD_GET_PRIV_PTR(dev);
    tHalHandle hHal = WLAN_HDD_GET_HAL_CTX(pAdapter);
    VOS_STATUS status = VOS_STATUS_SUCCESS;
    tP2pPsConfig NoA;
    p2p_app_setP2pPs_t *pappNoA = (p2p_app_setP2pPs_t *) msgData;

    NoA.opp_ps = pappNoA->opp_ps;
    NoA.ctWindow = pappNoA->ctWindow;
    NoA.duration = pappNoA->duration;
    NoA.interval = pappNoA->interval;
    NoA.count = pappNoA->count;
    NoA.single_noa_duration = pappNoA->single_noa_duration;
    NoA.psSelection = pappNoA->psSelection;
    NoA.sessionid = pAdapter->sessionId;

    sme_p2pSetPs(hHal, &NoA);
    return status;
}

static tANI_U8 wlan_hdd_get_session_type( enum nl80211_iftype type )
{
    tANI_U8 sessionType;

    switch( type )
    {
        case NL80211_IFTYPE_AP:
            sessionType = WLAN_HDD_SOFTAP;
            break;
        case NL80211_IFTYPE_P2P_GO:
            sessionType = WLAN_HDD_P2P_GO;
            break;
        case NL80211_IFTYPE_P2P_CLIENT:
            sessionType = WLAN_HDD_P2P_CLIENT;
            break;
        case NL80211_IFTYPE_STATION:
            sessionType = WLAN_HDD_INFRA_STATION;
            break;
        case NL80211_IFTYPE_MONITOR:
            sessionType = WLAN_HDD_MONITOR;
            break;
        default:
            sessionType = WLAN_HDD_INFRA_STATION;
            break;
    }

    return sessionType;
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,7,0))
struct wireless_dev* wlan_hdd_add_virtual_intf(
                  struct wiphy *wiphy, const char *name,
                  enum nl80211_iftype type,
                  u32 *flags, struct vif_params *params )
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))
struct wireless_dev* wlan_hdd_add_virtual_intf(
                  struct wiphy *wiphy, char *name, enum nl80211_iftype type,
                  u32 *flags, struct vif_params *params )
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,38))
struct net_device* wlan_hdd_add_virtual_intf(
                  struct wiphy *wiphy, char *name, enum nl80211_iftype type,
                  u32 *flags, struct vif_params *params )
#else
int wlan_hdd_add_virtual_intf( struct wiphy *wiphy, char *name,
                               enum nl80211_iftype type,
                               u32 *flags, struct vif_params *params )
#endif
{
    hdd_context_t *pHddCtx = (hdd_context_t*) wiphy_priv(wiphy);
    hdd_adapter_t* pAdapter = NULL;

    ENTER();

    if(hdd_get_adapter(pHddCtx, wlan_hdd_get_session_type(type)) != NULL)
    {
       hddLog(VOS_TRACE_LEVEL_ERROR,"%s: Interface type %d already exists. Two"
                     "interfaces of same type are not supported currently.",__func__, type);
       return NULL;
    }

    if (pHddCtx->isLogpInProgress)
    {
       VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                "%s:LOGP in Progress. Ignore!!!", __func__);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,38))
       return NULL;
#else
       return -EAGAIN;
#endif
    }
    if ( pHddCtx->cfg_ini->isP2pDeviceAddrAdministrated )
    {
        if( (NL80211_IFTYPE_P2P_GO == type) || 
            (NL80211_IFTYPE_P2P_CLIENT == type) )
        {
            /*                                                         
                                                     
             */
            v_MACADDR_t p2pDeviceAddress = pHddCtx->p2pDeviceAddress;
            p2pDeviceAddress.bytes[4] ^= 0x80;
            pAdapter = hdd_open_adapter( pHddCtx, 
                                         wlan_hdd_get_session_type(type),
                                         name, p2pDeviceAddress.bytes,
                                         VOS_TRUE );
        }
    }
    else
    {
       pAdapter = hdd_open_adapter( pHddCtx, wlan_hdd_get_session_type(type),
                          name, wlan_hdd_get_intf_addr(pHddCtx), VOS_TRUE );
    }

    if( NULL == pAdapter)
    {
        hddLog(VOS_TRACE_LEVEL_ERROR,"%s: hdd_open_adapter failed",__func__);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,38))
        return NULL;
#else
        return -EINVAL;
#endif
    }
    EXIT();
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))
    return pAdapter->dev->ieee80211_ptr;
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,38))
    return pAdapter->dev;
#else
    return 0;
#endif
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))
int wlan_hdd_del_virtual_intf( struct wiphy *wiphy, struct wireless_dev *wdev )
#else
int wlan_hdd_del_virtual_intf( struct wiphy *wiphy, struct net_device *dev )
#endif
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))
    struct net_device *dev = wdev->netdev;
#endif
    hdd_context_t *pHddCtx = (hdd_context_t*) wiphy_priv(wiphy);
    hdd_adapter_t *pVirtAdapter = WLAN_HDD_GET_PRIV_PTR(dev);
    int status;
    ENTER();

    hddLog(VOS_TRACE_LEVEL_INFO, "%s: device_mode = %d",
           __func__,pVirtAdapter->device_mode);

    status = wlan_hdd_validate_context(pHddCtx);

    if (0 != status)
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                   "%s: HDD context is not valid", __func__);
        return status;
    }

    wlan_hdd_release_intf_addr( pHddCtx,
                                 pVirtAdapter->macAddressCurrent.bytes );

    hdd_stop_adapter( pHddCtx, pVirtAdapter );
    hdd_close_adapter( pHddCtx, pVirtAdapter, TRUE );
    EXIT();
    return 0;
}

void hdd_sendMgmtFrameOverMonitorIface( hdd_adapter_t *pMonAdapter,
                                        tANI_U32 nFrameLength,
                                        tANI_U8* pbFrames,
                                        tANI_U8 frameType )  
{
    //                                   
    int rxstat;
    struct sk_buff *skb = NULL;
    int needed_headroom = 0;
    int flag = HDD_RX_FLAG_IV_STRIPPED | HDD_RX_FLAG_DECRYPTED |
               HDD_RX_FLAG_MMIC_STRIPPED;
#ifdef WLAN_OPEN_SOURCE
#ifdef WLAN_FEATURE_HOLD_RX_WAKELOCK
    hdd_context_t* pHddCtx = (hdd_context_t*)(pMonAdapter->pHddCtx);
#endif
#endif
    hddLog( LOG1, FL("Indicate Frame over Monitor Intf"));

    if (NULL == pbFrames)
    {
        hddLog(LOGE, FL("NULL frame pointer"));
        return;
    }

    /*                                                      
                                                               
                
       */
     needed_headroom = sizeof(struct ieee80211_radiotap_header) + 4;

     //               
     skb = alloc_skb(VPKT_SIZE_BUFFER, GFP_ATOMIC);
     if (unlikely(NULL == skb))
     {
         hddLog( LOGW, FL("Unable to allocate skb"));
         return;
     }
     skb_reserve(skb, VPKT_SIZE_BUFFER);
     if (unlikely(skb_headroom(skb) < nFrameLength))
     {
         VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_FATAL,
                   "HDD [%d]: Insufficient headroom, "
                   "head[%p], data[%p], req[%d]",
                   __LINE__, skb->head, skb->data, nFrameLength);
         kfree_skb(skb);
         return ;
     }
     //                       
     memcpy(skb_push(skb, nFrameLength), pbFrames, nFrameLength);
     /*                              */
     if( 0 != hdd_wlan_add_rx_radiotap_hdr( skb, needed_headroom, flag ) )
     {
         hddLog( LOGE, FL("Not Able Add Radio Tap"));
         //        
         kfree_skb(skb);
         return ;
     }

     skb_reset_mac_header( skb );
     skb->dev = pMonAdapter->dev;
     skb->protocol = eth_type_trans( skb, skb->dev );
     skb->ip_summed = CHECKSUM_NONE;
#ifdef WLAN_OPEN_SOURCE
#ifdef WLAN_FEATURE_HOLD_RX_WAKELOCK
     wake_lock_timeout(&pHddCtx->rx_wake_lock, msecs_to_jiffies(HDD_WAKE_LOCK_DURATION));
#endif
#endif
     rxstat = netif_rx_ni(skb);
     if( NET_RX_SUCCESS == rxstat )
     {
         hddLog( LOG1, FL("Success"));
     }
     else
         hddLog( LOGE, FL("Failed %d"), rxstat);                   

     return ;
}

void hdd_indicateMgmtFrame( hdd_adapter_t *pAdapter,
                            tANI_U32 nFrameLength, 
                            tANI_U8* pbFrames,
                            tANI_U8 frameType,
                            tANI_U32 rxChan,
                            tANI_S8 rxRssi )
{
    tANI_U16 freq;
    tANI_U8 type = 0;
    tANI_U8 subType = 0;
    tActionFrmType actionFrmType;
    hdd_cfg80211_state_t *cfgState = NULL;

    hddLog(VOS_TRACE_LEVEL_INFO, "%s: Frame Type = %d Frame Length = %d\n",
            __func__, frameType, nFrameLength);

    if (NULL == pAdapter)
    {
        hddLog(LOGE, FL("pAdapter is NULL"));
        return;
    }

    if (0 == nFrameLength)
    {
        hddLog(LOGE, FL("Frame Length is Invalid ZERO"));
        return;
    }

    if (NULL == pbFrames)
    {
        hddLog(LOGE, FL("pbFrames is NULL"));
        return;
    }

    type = WLAN_HDD_GET_TYPE_FRM_FC(pbFrames[0]);
    subType = WLAN_HDD_GET_SUBTYPE_FRM_FC(pbFrames[0]);

    /*                                                        */
    if ((type == SIR_MAC_MGMT_FRAME) &&
            (subType != SIR_MAC_MGMT_PROBE_REQ))
    {
         pAdapter = hdd_get_adapter_by_macaddr( WLAN_HDD_GET_CTX(pAdapter),
                            &pbFrames[WLAN_HDD_80211_FRM_DA_OFFSET]);
         if (NULL == pAdapter)
         {
             /*                                                       
                                                                 
              */
             hddLog(VOS_TRACE_LEVEL_FATAL,"pAdapter for action frame is NULL Macaddr = "
                               MAC_ADDRESS_STR ,
                               MAC_ADDR_ARRAY(&pbFrames[WLAN_HDD_80211_FRM_DA_OFFSET]));
             hddLog(VOS_TRACE_LEVEL_FATAL, "%s: Frame Type = %d Frame Length = %d"
                              " subType = %d \n",__func__,frameType,nFrameLength,subType);
             return;
         }
    }


    if (NULL == pAdapter->dev)
    {
        hddLog( LOGE, FL("pAdapter->dev is NULL"));
        return;
    }

    if (WLAN_HDD_ADAPTER_MAGIC != pAdapter->magic)
    {
        hddLog( LOGE, FL("pAdapter has invalid magic"));
        return;
    }

    if ((WLAN_HDD_SOFTAP == pAdapter->device_mode) ||
        (WLAN_HDD_P2P_GO == pAdapter->device_mode ))
    {
        hdd_adapter_t *pMonAdapter =
            hdd_get_mon_adapter( WLAN_HDD_GET_CTX(pAdapter) );

        if (NULL != pMonAdapter)
        {
            hddLog( LOG1, FL("Indicate Frame over Monitor Interface"));
            hdd_sendMgmtFrameOverMonitorIface( pMonAdapter, nFrameLength,
                    pbFrames, frameType);
            return;
        }
    }

    //                                    
    //                         
    if( rxChan <= MAX_NO_OF_2_4_CHANNELS )
    {
        freq = ieee80211_channel_to_frequency( rxChan,
                IEEE80211_BAND_2GHZ);
    }
    else
    {
        freq = ieee80211_channel_to_frequency( rxChan,
                IEEE80211_BAND_5GHZ);
    }

    cfgState = WLAN_HDD_GET_CFG_STATE_PTR( pAdapter );
    
    if ((type == SIR_MAC_MGMT_FRAME) && 
        (subType == SIR_MAC_MGMT_ACTION))
    {
        if(pbFrames[WLAN_HDD_PUBLIC_ACTION_FRAME_OFFSET] == WLAN_HDD_PUBLIC_ACTION_FRAME)
        {
            //                    
            if((pbFrames[WLAN_HDD_PUBLIC_ACTION_FRAME_OFFSET+1] == SIR_MAC_ACTION_VENDOR_SPECIFIC) &&
                vos_mem_compare(&pbFrames[WLAN_HDD_PUBLIC_ACTION_FRAME_OFFSET+2], SIR_MAC_P2P_OUI, SIR_MAC_P2P_OUI_SIZE))
            //                  
            {
                actionFrmType = pbFrames[WLAN_HDD_PUBLIC_ACTION_FRAME_TYPE_OFFSET];
                hddLog(LOG1, "Rx Action Frame %u \n", actionFrmType);
#ifdef WLAN_FEATURE_P2P_DEBUG
                if(actionFrmType >= MAX_P2P_ACTION_FRAME_TYPE)
                {
                    hddLog(VOS_TRACE_LEVEL_ERROR,"[P2P] unknown[%d] <--- OTA",
                                                                actionFrmType);
                }
                else
                {
                    hddLog(VOS_TRACE_LEVEL_ERROR,"[P2P] %s <--- OTA",
                    p2p_action_frame_type[actionFrmType]);
                    if( (actionFrmType == WLAN_HDD_PROV_DIS_REQ) &&
                        (globalP2PConnectionStatus == P2P_NOT_ACTIVE) )
                    {
                         globalP2PConnectionStatus = P2P_GO_NEG_PROCESS;
                         hddLog(LOGE,"[P2P State]Inactive state to "
                           "GO negotiation progress state");
                    }
                    else if( (actionFrmType == WLAN_HDD_GO_NEG_CNF) &&
                        (globalP2PConnectionStatus == P2P_GO_NEG_PROCESS) )
                    {
                         globalP2PConnectionStatus = P2P_GO_NEG_COMPLETED;
                 hddLog(LOGE,"[P2P State]GO negotiation progress to "
                             "GO negotiation completed state");
                    }
                    else if( (actionFrmType == WLAN_HDD_INVITATION_REQ) &&
                        (globalP2PConnectionStatus == P2P_NOT_ACTIVE) )
                    {
                         globalP2PConnectionStatus = P2P_GO_NEG_COMPLETED;
                         hddLog(LOGE,"[P2P State]Inactive state to GO negotiation"
                                     " completed state Autonomous GO formation");
                    }
                }
#endif

                if (((actionFrmType == WLAN_HDD_PROV_DIS_RESP) &&
                            (cfgState->actionFrmState == HDD_PD_REQ_ACK_PENDING)) ||
                        ((actionFrmType == WLAN_HDD_GO_NEG_RESP) &&
                         (cfgState->actionFrmState == HDD_GO_NEG_REQ_ACK_PENDING)))
                {
                    hddLog(LOG1, "%s: ACK_PENDING and But received RESP for Action frame ",
                            __func__);
                    hdd_sendActionCnf(pAdapter, TRUE);
                }
            }
#ifdef FEATURE_WLAN_TDLS
            else if(pbFrames[WLAN_HDD_PUBLIC_ACTION_FRAME_OFFSET+1] == WLAN_HDD_PUBLIC_ACTION_TDLS_DISC_RESP)
            {
                u8 *mac = &pbFrames[WLAN_HDD_80211_FRM_DA_OFFSET+6];
#ifdef WLAN_FEATURE_TDLS_DEBUG
                hddLog(VOS_TRACE_LEVEL_ERROR,"[TDLS] TDLS Discovery Response," MAC_ADDRESS_STR " RSSI[%d] <--- OTA",
                 MAC_ADDR_ARRAY(mac),rxRssi);
#endif
                wlan_hdd_tdls_set_rssi(pAdapter, mac, rxRssi);
                wlan_hdd_tdls_recv_discovery_resp(pAdapter, mac);
            }
#endif
        }
#ifdef WLAN_FEATURE_TDLS_DEBUG
        if(pbFrames[WLAN_HDD_PUBLIC_ACTION_FRAME_OFFSET] == WLAN_HDD_TDLS_ACTION_FRAME)
        {
            actionFrmType = pbFrames[WLAN_HDD_PUBLIC_ACTION_FRAME_OFFSET+1];
            if(actionFrmType >= MAX_TDLS_ACTION_FRAME_TYPE)
            {
                hddLog(VOS_TRACE_LEVEL_ERROR,"[TDLS] unknown[%d] <--- OTA",
                                                            actionFrmType);
            }
            else
            {
                hddLog(VOS_TRACE_LEVEL_ERROR,"[TDLS] %s <--- OTA",
                    tdls_action_frame_type[actionFrmType]);
            }
        }
#endif
    }

    //                                    
    hddLog( LOG1, FL("Indicate Frame over NL80211 Interface"));

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))
    cfg80211_rx_mgmt( pAdapter->dev->ieee80211_ptr, freq, 0,
                      pbFrames, nFrameLength,
                      GFP_ATOMIC );
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0))
    cfg80211_rx_mgmt( pAdapter->dev, freq, 0,
                      pbFrames, nFrameLength,
                      GFP_ATOMIC );
#else
    cfg80211_rx_mgmt( pAdapter->dev, freq,
                      pbFrames, nFrameLength,
                      GFP_ATOMIC );
#endif //                  
}

/*
                                                         
 */
static int hdd_wlan_add_rx_radiotap_hdr (
             struct sk_buff *skb, int rtap_len, int flag )
{
    u8 rtap_temp[20] = {0};
    struct ieee80211_radiotap_header *rthdr;
    unsigned char *pos;
    u16 rx_flags = 0;

    rthdr = (struct ieee80211_radiotap_header *)(&rtap_temp[0]);

    /*                                           */
    rthdr->it_present = cpu_to_le32((1 << IEEE80211_RADIOTAP_FLAGS)   |
                                    (1 << IEEE80211_RADIOTAP_RX_FLAGS));
    rthdr->it_len = cpu_to_le16(rtap_len);

    pos = (unsigned char *) (rthdr + 1);

    /*                                                */

    /*                          */
    *pos = 0;
    pos++;

    /*                                             */
    /*                                                          */
    if ((pos - (u8 *)rthdr) & 1)
        pos++;
    put_unaligned_le16(rx_flags, pos);
    pos += 2;
    
    //                       
    memcpy(skb_push(skb, rtap_len), &rtap_temp[0], rtap_len);

    return 0;
}

static void hdd_wlan_tx_complete( hdd_adapter_t* pAdapter,
                                  hdd_cfg80211_state_t* cfgState,
                                  tANI_BOOLEAN actionSendSuccess )
{
    struct ieee80211_radiotap_header *rthdr;
    unsigned char *pos;
    struct sk_buff *skb = cfgState->skb;
#ifdef WLAN_OPEN_SOURCE
#ifdef WLAN_FEATURE_HOLD_RX_WAKELOCK
    hdd_context_t *pHddCtx = (hdd_context_t*)(pAdapter->pHddCtx);
#endif
#endif

    /*                                                */
    u32 rtHdrLen = sizeof(*rthdr) + 3;

    u8 *data;

    /*                                                                  
                                                                              
                                                                          
       */
    if( NULL == skb_pull(skb, skb->len) )
    {
        hddLog( LOGE, FL("Not Able to Pull %d byte from skb"), skb->len);
        kfree_skb(cfgState->skb);
        return;
    }

    data = skb_push( skb, cfgState->len );

    if (data == NULL)
    {
        hddLog( LOGE, FL("Not Able to Push %d byte to skb"), cfgState->len);
        kfree_skb( cfgState->skb );
        return;
    }

    memcpy( data, cfgState->buf, cfgState->len );

    /*                                      */
    if( skb_headroom(skb) < rtHdrLen )
    {
        hddLog( LOGE, FL("No headroom for rtap header"));
        kfree_skb(cfgState->skb);
        return;
    }

    rthdr = (struct ieee80211_radiotap_header*) skb_push( skb, rtHdrLen );

    memset( rthdr, 0, rtHdrLen );
    rthdr->it_len = cpu_to_le16( rtHdrLen );
    rthdr->it_present = cpu_to_le32((1 << IEEE80211_RADIOTAP_TX_FLAGS) |
                                    (1 << IEEE80211_RADIOTAP_DATA_RETRIES)
                                   );

    pos = (unsigned char *)( rthdr+1 );

    //              
    *pos = actionSendSuccess;
    pos += 2;

    //                 
    *pos = 0;
    pos++;

    skb_set_mac_header( skb, 0 );
    skb->ip_summed = CHECKSUM_NONE;
    skb->pkt_type  = PACKET_OTHERHOST;
    skb->protocol  = htons(ETH_P_802_2);
    memset( skb->cb, 0, sizeof( skb->cb ) );
#ifdef WLAN_OPEN_SOURCE
#ifdef WLAN_FEATURE_HOLD_RX_WAKELOCK
    wake_lock_timeout(&pHddCtx->rx_wake_lock, msecs_to_jiffies(HDD_WAKE_LOCK_DURATION));
#endif
#endif
    if (in_interrupt())
        netif_rx( skb );
    else
        netif_rx_ni( skb );

    /*                                              */
    netif_tx_start_all_queues( pAdapter->dev );

}


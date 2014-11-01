/*
  * Copyright (c) 2012-2014, The Linux Foundation. All rights reserved.
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

  \file  wlan_hdd_cfg80211.c

  \brief WLAN Host Device Driver implementation

  Copyright 2008 (c) Qualcomm Technologies, Inc.  All Rights Reserved.

  Qualcomm Technologies Confidential and Proprietary.

  ========================================================================*/

/*                                                                          

                       


                                                                       
                                                                


                                     


                                             
                                                                                     
                                            

                                                                       
                      
                                                                            */


#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/wireless.h>
#include <wlan_hdd_includes.h>
#include <net/arp.h>
#include <net/cfg80211.h>
#include <linux/wireless.h>
#include <wlan_hdd_wowl.h>
#include <aniGlobal.h>
#include "ccmApi.h"
#include "sirParams.h"
#include "dot11f.h"
#include "wlan_hdd_assoc.h"
#include "wlan_hdd_wext.h"
#include "sme_Api.h"
#include "wlan_hdd_p2p.h"
#include "wlan_hdd_cfg80211.h"
#include "wlan_hdd_hostapd.h"
#include "sapInternal.h"
#include "wlan_hdd_softap_tx_rx.h"
#include "wlan_hdd_main.h"
#include "wlan_hdd_assoc.h"
#include "wlan_hdd_power.h"
#ifdef WLAN_BTAMP_FEATURE
#include "bap_hdd_misc.h"
#endif
#include <qc_sap_ioctl.h>
#ifdef FEATURE_WLAN_TDLS
#include "wlan_hdd_tdls.h"
#include "wlan_hdd_wmm.h"
#include "wlan_qct_wda.h"
#endif
#include "wlan_nv.h"
#include "wlan_hdd_dev_pwr.h"

#define g_mode_rates_size (12)
#define a_mode_rates_size (8)
#define FREQ_BASE_80211G          (2407)
#define FREQ_BAND_DIFF_80211G     (5)
#define MAX_SCAN_SSID 9
#define MAX_PENDING_LOG 5
#define GET_IE_LEN_IN_BSS_DESC(lenInBss) ( lenInBss + sizeof(lenInBss) - \
        ((int) OFFSET_OF( tSirBssDescription, ieFields)))

#define HDD2GHZCHAN(freq, chan, flag)   {     \
    .band =  IEEE80211_BAND_2GHZ, \
    .center_freq = (freq), \
    .hw_value = (chan),\
    .flags = (flag), \
    .max_antenna_gain = 0 ,\
    .max_power = 30, \
}

#define HDD5GHZCHAN(freq, chan, flag)   {     \
    .band =  IEEE80211_BAND_5GHZ, \
    .center_freq = (freq), \
    .hw_value = (chan),\
    .flags = (flag), \
    .max_antenna_gain = 0 ,\
    .max_power = 30, \
}

#define HDD_G_MODE_RATETAB(rate, rate_id, flag)\
{\
    .bitrate = rate, \
    .hw_value = rate_id, \
    .flags = flag, \
}

#ifndef WLAN_FEATURE_TDLS_DEBUG
#define TDLS_LOG_LEVEL VOS_TRACE_LEVEL_INFO
#else
#define TDLS_LOG_LEVEL VOS_TRACE_LEVEL_ERROR
#endif

#ifdef WLAN_FEATURE_VOWIFI_11R
#define WLAN_AKM_SUITE_FT_8021X         0x000FAC03
#define WLAN_AKM_SUITE_FT_PSK           0x000FAC04
#endif

static const u32 hdd_cipher_suites[] =
{
    WLAN_CIPHER_SUITE_WEP40,
    WLAN_CIPHER_SUITE_WEP104,
    WLAN_CIPHER_SUITE_TKIP,
#ifdef FEATURE_WLAN_CCX
#define WLAN_CIPHER_SUITE_KRK 0x004096ff /*             */
    WLAN_CIPHER_SUITE_KRK,
    WLAN_CIPHER_SUITE_CCMP,
#else
    WLAN_CIPHER_SUITE_CCMP,
#endif
#ifdef FEATURE_WLAN_WAPI
    WLAN_CIPHER_SUITE_SMS4,
#endif
#ifdef WLAN_FEATURE_11W
    WLAN_CIPHER_SUITE_AES_CMAC,
#endif
};

static inline int is_broadcast_ether_addr(const u8 *addr)
{
    return ((addr[0] == 0xff) && (addr[1] == 0xff) && (addr[2] == 0xff) &&
            (addr[3] == 0xff) && (addr[4] == 0xff) && (addr[5] == 0xff));
}

static struct ieee80211_channel hdd_channels_2_4_GHZ[] =
{
    HDD2GHZCHAN(2412, 1, 0) ,
    HDD2GHZCHAN(2417, 2, 0) ,
    HDD2GHZCHAN(2422, 3, 0) ,
    HDD2GHZCHAN(2427, 4, 0) ,
    HDD2GHZCHAN(2432, 5, 0) ,
    HDD2GHZCHAN(2437, 6, 0) ,
    HDD2GHZCHAN(2442, 7, 0) ,
    HDD2GHZCHAN(2447, 8, 0) ,
    HDD2GHZCHAN(2452, 9, 0) ,
    HDD2GHZCHAN(2457, 10, 0) ,
    HDD2GHZCHAN(2462, 11, 0) ,
    HDD2GHZCHAN(2467, 12, 0) ,
    HDD2GHZCHAN(2472, 13, 0) ,
    HDD2GHZCHAN(2484, 14, 0) ,
};

static struct ieee80211_channel hdd_social_channels_2_4_GHZ[] =
{
    HDD2GHZCHAN(2412, 1, 0) ,
    HDD2GHZCHAN(2437, 6, 0) ,
    HDD2GHZCHAN(2462, 11, 0) ,
};

static struct ieee80211_channel hdd_channels_5_GHZ[] =
{
    HDD5GHZCHAN(4920, 240, 0) ,
    HDD5GHZCHAN(4940, 244, 0) ,
    HDD5GHZCHAN(4960, 248, 0) ,
    HDD5GHZCHAN(4980, 252, 0) ,
    HDD5GHZCHAN(5040, 208, 0) ,
    HDD5GHZCHAN(5060, 212, 0) ,
    HDD5GHZCHAN(5080, 216, 0) ,
    HDD5GHZCHAN(5180, 36, 0) ,
    HDD5GHZCHAN(5200, 40, 0) ,
    HDD5GHZCHAN(5220, 44, 0) ,
    HDD5GHZCHAN(5240, 48, 0) ,
    HDD5GHZCHAN(5260, 52, 0) ,
    HDD5GHZCHAN(5280, 56, 0) ,
    HDD5GHZCHAN(5300, 60, 0) ,
    HDD5GHZCHAN(5320, 64, 0) ,
    HDD5GHZCHAN(5500,100, 0) ,
    HDD5GHZCHAN(5520,104, 0) ,
    HDD5GHZCHAN(5540,108, 0) ,
    HDD5GHZCHAN(5560,112, 0) ,
    HDD5GHZCHAN(5580,116, 0) ,
    HDD5GHZCHAN(5600,120, 0) ,
    HDD5GHZCHAN(5620,124, 0) ,
    HDD5GHZCHAN(5640,128, 0) ,
    HDD5GHZCHAN(5660,132, 0) ,
    HDD5GHZCHAN(5680,136, 0) ,
    HDD5GHZCHAN(5700,140, 0) ,
#ifdef FEATURE_WLAN_CH144
    HDD5GHZCHAN(5720,144, 0) ,
#endif /*                    */
    HDD5GHZCHAN(5745,149, 0) ,
    HDD5GHZCHAN(5765,153, 0) ,
    HDD5GHZCHAN(5785,157, 0) ,
    HDD5GHZCHAN(5805,161, 0) ,
    HDD5GHZCHAN(5825,165, 0) ,
};

static struct ieee80211_rate g_mode_rates[] =
{
    HDD_G_MODE_RATETAB(10, 0x1, 0),
    HDD_G_MODE_RATETAB(20, 0x2, 0),
    HDD_G_MODE_RATETAB(55, 0x4, 0),
    HDD_G_MODE_RATETAB(110, 0x8, 0),
    HDD_G_MODE_RATETAB(60, 0x10, 0),
    HDD_G_MODE_RATETAB(90, 0x20, 0),
    HDD_G_MODE_RATETAB(120, 0x40, 0),
    HDD_G_MODE_RATETAB(180, 0x80, 0),
    HDD_G_MODE_RATETAB(240, 0x100, 0),
    HDD_G_MODE_RATETAB(360, 0x200, 0),
    HDD_G_MODE_RATETAB(480, 0x400, 0),
    HDD_G_MODE_RATETAB(540, 0x800, 0),
};

static struct ieee80211_rate a_mode_rates[] =
{
    HDD_G_MODE_RATETAB(60, 0x10, 0),
    HDD_G_MODE_RATETAB(90, 0x20, 0),
    HDD_G_MODE_RATETAB(120, 0x40, 0),
    HDD_G_MODE_RATETAB(180, 0x80, 0),
    HDD_G_MODE_RATETAB(240, 0x100, 0),
    HDD_G_MODE_RATETAB(360, 0x200, 0),
    HDD_G_MODE_RATETAB(480, 0x400, 0),
    HDD_G_MODE_RATETAB(540, 0x800, 0),
};

static struct ieee80211_supported_band wlan_hdd_band_2_4_GHZ =
{
    .channels = hdd_channels_2_4_GHZ,
    .n_channels = ARRAY_SIZE(hdd_channels_2_4_GHZ),
    .band       = IEEE80211_BAND_2GHZ,
    .bitrates = g_mode_rates,
    .n_bitrates = g_mode_rates_size,
    .ht_cap.ht_supported   = 1,
    .ht_cap.cap            =  IEEE80211_HT_CAP_SGI_20
                            | IEEE80211_HT_CAP_GRN_FLD
                            | IEEE80211_HT_CAP_DSSSCCK40
                            | IEEE80211_HT_CAP_LSIG_TXOP_PROT,
    .ht_cap.ampdu_factor   = IEEE80211_HT_MAX_AMPDU_64K,
    .ht_cap.ampdu_density  = IEEE80211_HT_MPDU_DENSITY_16,
    .ht_cap.mcs.rx_mask    = { 0xff, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    .ht_cap.mcs.rx_highest = cpu_to_le16( 72 ),
    .ht_cap.mcs.tx_params  = IEEE80211_HT_MCS_TX_DEFINED,
};

static struct ieee80211_supported_band wlan_hdd_band_p2p_2_4_GHZ =
{
    .channels = hdd_social_channels_2_4_GHZ,
    .n_channels = ARRAY_SIZE(hdd_social_channels_2_4_GHZ),
    .band       = IEEE80211_BAND_2GHZ,
    .bitrates = g_mode_rates,
    .n_bitrates = g_mode_rates_size,
    .ht_cap.ht_supported   = 1,
    .ht_cap.cap            =  IEEE80211_HT_CAP_SGI_20
                            | IEEE80211_HT_CAP_GRN_FLD
                            | IEEE80211_HT_CAP_DSSSCCK40
                            | IEEE80211_HT_CAP_LSIG_TXOP_PROT,
    .ht_cap.ampdu_factor   = IEEE80211_HT_MAX_AMPDU_64K,
    .ht_cap.ampdu_density  = IEEE80211_HT_MPDU_DENSITY_16,
    .ht_cap.mcs.rx_mask    = { 0xff, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    .ht_cap.mcs.rx_highest = cpu_to_le16( 72 ),
    .ht_cap.mcs.tx_params  = IEEE80211_HT_MCS_TX_DEFINED,
};

static struct ieee80211_supported_band wlan_hdd_band_5_GHZ =
{
    .channels = hdd_channels_5_GHZ,
    .n_channels = ARRAY_SIZE(hdd_channels_5_GHZ),
    .band     = IEEE80211_BAND_5GHZ,
    .bitrates = a_mode_rates,
    .n_bitrates = a_mode_rates_size,
    .ht_cap.ht_supported   = 1,
    .ht_cap.cap            =  IEEE80211_HT_CAP_SGI_20
                            | IEEE80211_HT_CAP_GRN_FLD
                            | IEEE80211_HT_CAP_DSSSCCK40
                            | IEEE80211_HT_CAP_LSIG_TXOP_PROT
                            | IEEE80211_HT_CAP_SGI_40
                            | IEEE80211_HT_CAP_SUP_WIDTH_20_40,
    .ht_cap.ampdu_factor   = IEEE80211_HT_MAX_AMPDU_64K,
    .ht_cap.ampdu_density  = IEEE80211_HT_MPDU_DENSITY_16,
    .ht_cap.mcs.rx_mask    = { 0xff, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    .ht_cap.mcs.rx_highest = cpu_to_le16( 72 ),
    .ht_cap.mcs.tx_params  = IEEE80211_HT_MCS_TX_DEFINED,
};

/*                                                                      
                                                */
static const struct ieee80211_txrx_stypes
wlan_hdd_txrx_stypes[NUM_NL80211_IFTYPES] = {
    [NL80211_IFTYPE_STATION] = {
        .tx = 0xffff,
        .rx = BIT(SIR_MAC_MGMT_ACTION) |
            BIT(SIR_MAC_MGMT_PROBE_REQ),
    },
    [NL80211_IFTYPE_AP] = {
        .tx = 0xffff,
        .rx = BIT(SIR_MAC_MGMT_ASSOC_REQ) |
            BIT(SIR_MAC_MGMT_REASSOC_REQ) |
            BIT(SIR_MAC_MGMT_PROBE_REQ) |
            BIT(SIR_MAC_MGMT_DISASSOC) |
            BIT(SIR_MAC_MGMT_AUTH) |
            BIT(SIR_MAC_MGMT_DEAUTH) |
            BIT(SIR_MAC_MGMT_ACTION),
    },
    [NL80211_IFTYPE_ADHOC] = {
        .tx = 0xffff,
        .rx = BIT(SIR_MAC_MGMT_ASSOC_REQ) |
            BIT(SIR_MAC_MGMT_REASSOC_REQ) |
            BIT(SIR_MAC_MGMT_PROBE_REQ) |
            BIT(SIR_MAC_MGMT_DISASSOC) |
            BIT(SIR_MAC_MGMT_AUTH) |
            BIT(SIR_MAC_MGMT_DEAUTH) |
            BIT(SIR_MAC_MGMT_ACTION),
    },
    [NL80211_IFTYPE_P2P_CLIENT] = {
        .tx = 0xffff,
        .rx = BIT(SIR_MAC_MGMT_ACTION) |
            BIT(SIR_MAC_MGMT_PROBE_REQ),
    },
    [NL80211_IFTYPE_P2P_GO] = {
        /*                                 */
        .tx = 0xffff,
        .rx = BIT(SIR_MAC_MGMT_ASSOC_REQ) |
            BIT(SIR_MAC_MGMT_REASSOC_REQ) |
            BIT(SIR_MAC_MGMT_PROBE_REQ) |
            BIT(SIR_MAC_MGMT_DISASSOC) |
            BIT(SIR_MAC_MGMT_AUTH) |
            BIT(SIR_MAC_MGMT_DEAUTH) |
            BIT(SIR_MAC_MGMT_ACTION),
    },
};

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0))
static const struct ieee80211_iface_limit
wlan_hdd_iface_limit[] = {
    {
        /*                                                              
                                                                   
                                                                          
                                                                          
                  
                                                                           
                                                                           
                                                                      
                                                                    
                                                                   
                                                                              
                                                                        
                                                                            
                                                                         
                                                                           
                                                                    
                              
         */
        .max = 3,
        .types = BIT(NL80211_IFTYPE_STATION),
    },
    {
        .max = 1,
        .types = BIT(NL80211_IFTYPE_ADHOC) | BIT(NL80211_IFTYPE_AP),
    },
    {
        .max = 1,
        .types = BIT(NL80211_IFTYPE_P2P_GO) |
                 BIT(NL80211_IFTYPE_P2P_CLIENT),
    },
};

/*                                                        */
static struct ieee80211_iface_combination
wlan_hdd_iface_combination = {
        .limits = wlan_hdd_iface_limit,
        .num_different_channels = 1,
        /*
                                                                           
                                                 
                                                                     
                               
                                                         
                                  
                                                                        
                                                           
         */
        .max_interfaces = WLAN_MAX_INTERFACES,
        .n_limits = ARRAY_SIZE(wlan_hdd_iface_limit),
        .beacon_int_infra_match = false,
};
#endif

static struct cfg80211_ops wlan_hdd_cfg80211_ops;

/*                                     */
struct index_data_rate_type
{
   v_U8_t   beacon_rate_index;
   v_U16_t  supported_rate[4];
};

/*                                                         
                                  
                                                                
                                        
                                                  
                                                              
                                            
                                                                  
 */
static const struct
{
   v_U8_t   beacon_rate_index;
   v_U16_t  supported_rate[4];
} supported_data_rate[] =
{
/*                                         */
   {2,   { 10,  10, 10, 0}},
   {4,   { 20,  20, 10, 0}},
   {11,  { 55,  20, 10, 0}},
   {12,  { 60,  55, 20, 0}},
   {18,  { 90,  55, 20, 0}},
   {22,  {110,  55, 20, 0}},
   {24,  {120,  90, 60, 0}},
   {36,  {180, 120, 60, 0}},
   {44,  {220, 180, 60, 0}},
   {48,  {240, 180, 90, 0}},
   {66,  {330, 180, 90, 0}},
   {72,  {360, 240, 90, 0}},
   {96,  {480, 240, 120, 0}},
   {108, {540, 240, 120, 0}}
};

/*                      */
static struct index_data_rate_type supported_mcs_rate[] =
{
/*                           */
   {0,  {65,  135,  72,  150}},
   {1,  {130, 270,  144, 300}},
   {2,  {195, 405,  217, 450}},
   {3,  {260, 540,  289, 600}},
   {4,  {390, 810,  433, 900}},
   {5,  {520, 1080, 578, 1200}},
   {6,  {585, 1215, 650, 1350}},
   {7,  {650, 1350, 722, 1500}}
};

#ifdef WLAN_FEATURE_11AC

#define DATA_RATE_11AC_MCS_MASK    0x03

struct index_vht_data_rate_type
{
   v_U8_t   beacon_rate_index;
   v_U16_t  supported_VHT80_rate[2];
   v_U16_t  supported_VHT40_rate[2];
   v_U16_t  supported_VHT20_rate[2];
};

typedef enum
{
   DATA_RATE_11AC_MAX_MCS_7,
   DATA_RATE_11AC_MAX_MCS_8,
   DATA_RATE_11AC_MAX_MCS_9,
   DATA_RATE_11AC_MAX_MCS_NA
} eDataRate11ACMaxMcs;

/*                          */
static struct index_vht_data_rate_type supported_vht_mcs_rate[] =
{
/*                                           */
   {0,  {293,  325},  {135,  150},  {65,   72}},
   {1,  {585,  650},  {270,  300},  {130,  144}},
   {2,  {878,  975},  {405,  450},  {195,  217}},
   {3,  {1170, 1300}, {540,  600},  {260,  289}},
   {4,  {1755, 1950}, {810,  900},  {390,  433}},
   {5,  {2340, 2600}, {1080, 1200}, {520,  578}},
   {6,  {2633, 2925}, {1215, 1350}, {585,  650}},
   {7,  {2925, 3250}, {1350, 1500}, {650,  722}},
   {8,  {3510, 3900}, {1620, 1800}, {780,  867}},
   {9,  {3900, 4333}, {1800, 2000}, {780,  867}}
};
#endif /*                   */

extern struct net_device_ops net_ops_struct;

#ifdef WLAN_NL80211_TESTMODE
enum wlan_hdd_tm_attr
{
    WLAN_HDD_TM_ATTR_INVALID = 0,
    WLAN_HDD_TM_ATTR_CMD     = 1,
    WLAN_HDD_TM_ATTR_DATA    = 2,
    WLAN_HDD_TM_ATTR_TYPE    = 3,
    /*           */
    WLAN_HDD_TM_ATTR_AFTER_LAST,
    WLAN_HDD_TM_ATTR_MAX       = WLAN_HDD_TM_ATTR_AFTER_LAST - 1,
};

enum wlan_hdd_tm_cmd
{
    WLAN_HDD_TM_CMD_WLAN_HB    = 1,
};

#define WLAN_HDD_TM_DATA_MAX_LEN    5000

static const struct nla_policy wlan_hdd_tm_policy[WLAN_HDD_TM_ATTR_MAX + 1] =
{
    [WLAN_HDD_TM_ATTR_CMD]        = { .type = NLA_U32 },
    [WLAN_HDD_TM_ATTR_DATA]       = { .type = NLA_BINARY,
                                    .len = WLAN_HDD_TM_DATA_MAX_LEN },
};
#endif /*                       */

#ifdef FEATURE_WLAN_CH_AVOID
/*
                                           
                                                                
                                           
 */
int wlan_hdd_send_avoid_freq_event(hdd_context_t *pHddCtx,
                                   tHddAvoidFreqList *pAvoidFreqList)
{
    struct sk_buff *vendor_event;

    ENTER();

    if (!pHddCtx)
    {
        VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                  "%s: HDD context is null", __func__);
        return -1;
    }

    if (!pAvoidFreqList)
    {
        VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                  "%s: pAvoidFreqList is null", __func__);
        return -1;
    }

    vendor_event = cfg80211_vendor_event_alloc(pHddCtx->wiphy,
                       sizeof(tHddAvoidFreqList),
                       QCOM_NL80211_VENDOR_SUBCMD_AVOID_FREQUENCY_INDEX,
                       GFP_KERNEL);
    if (!vendor_event)
    {
        VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                  "%s: cfg80211_vendor_event_alloc failed", __func__);
        return -1;
    }

    memcpy(skb_put(vendor_event, sizeof(tHddAvoidFreqList)),
                   (void *)pAvoidFreqList, sizeof(tHddAvoidFreqList));

    cfg80211_vendor_event(vendor_event, GFP_KERNEL);

    EXIT();
    return 0;
}
#endif /*                       */

/*                        */
static const struct nl80211_vendor_cmd_info wlan_hdd_cfg80211_vendor_events[] =
{
#ifdef FEATURE_WLAN_CH_AVOID
    {
        .vendor_id = QCOM_NL80211_VENDOR_ID,
        .subcmd = QCOM_NL80211_VENDOR_SUBCMD_AVOID_FREQUENCY
    },
#endif /*                       */
};

/*
                                          
                                                
                         
                                                     
 */
struct wiphy *wlan_hdd_cfg80211_wiphy_alloc(int priv_size)
{
    struct wiphy *wiphy;
    ENTER();

    /*
                            
     */
    wiphy = wiphy_new(&wlan_hdd_cfg80211_ops, priv_size);

    if (!wiphy)
    {
        /*                                                         */
        hddLog(VOS_TRACE_LEVEL_ERROR, "%s: wiphy init failed", __func__);
        return NULL;
    }


    return wiphy;
}

/*
                                          
                                                        
                                         
 */
int wlan_hdd_cfg80211_update_band(struct wiphy *wiphy, eCsrBand eBand)
{
    int i, j;
    eNVChannelEnabledType channelEnabledState;

    ENTER();
    for (i = 0; i < IEEE80211_NUM_BANDS; i++)
    {

        if (NULL == wiphy->bands[i])
        {
           hddLog(VOS_TRACE_LEVEL_ERROR,"%s: wiphy->bands[i] is NULL, i = %d",
                  __func__, i);
           continue;
        }

        for (j = 0; j < wiphy->bands[i]->n_channels; j++)
        {
            struct ieee80211_supported_band *band = wiphy->bands[i];

            channelEnabledState = vos_nv_getChannelEnabledState(
                                  band->channels[j].hw_value);

            if (IEEE80211_BAND_2GHZ == i && eCSR_BAND_5G == eBand) //        
            {
                //                               
                if (WLAN_HDD_IS_SOCIAL_CHANNEL(band->channels[j].center_freq) &&
                    NV_CHANNEL_ENABLE == channelEnabledState)
                    band->channels[j].flags &= ~IEEE80211_CHAN_DISABLED;
                else
                    band->channels[j].flags |= IEEE80211_CHAN_DISABLED;
                continue;
            }
            else if (IEEE80211_BAND_5GHZ == i && eCSR_BAND_24 == eBand) //        
            {
                band->channels[j].flags |= IEEE80211_CHAN_DISABLED;
                continue;
            }

            if (NV_CHANNEL_DISABLE == channelEnabledState ||
                NV_CHANNEL_INVALID == channelEnabledState)
            {
                band->channels[j].flags |= IEEE80211_CHAN_DISABLED;
            }
            else if (NV_CHANNEL_DFS == channelEnabledState)
            {
                band->channels[j].flags &= ~IEEE80211_CHAN_DISABLED;
                band->channels[j].flags |= IEEE80211_CHAN_RADAR;
            }
            else
            {
                band->channels[j].flags &= ~(IEEE80211_CHAN_DISABLED
                                             |IEEE80211_CHAN_RADAR);
            }
        }
    }
    return 0;
}
/*
                                   
                                                
                         
                                                                    
 */
int wlan_hdd_cfg80211_init(struct device *dev,
                               struct wiphy *wiphy,
                               hdd_config_t *pCfg
                               )
{
    int i, j;
    hdd_context_t *pHddCtx = wiphy_priv(wiphy);

    ENTER();

    /*                                                */
    set_wiphy_dev(wiphy, dev);

    wiphy->mgmt_stypes = wlan_hdd_txrx_stypes;

#ifndef CONFIG_ENABLE_LINUX_REG
    /*                                                   
                                  */
    wiphy->flags |= WIPHY_FLAG_STRICT_REGULATORY;
#endif

    /*                                                          
                                                          */
    wiphy->flags |=   WIPHY_FLAG_DISABLE_BEACON_HINTS;


#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0))
    wiphy->flags |= WIPHY_FLAG_HAVE_AP_SME
                 |  WIPHY_FLAG_AP_PROBE_RESP_OFFLOAD
                 |  WIPHY_FLAG_HAS_REMAIN_ON_CHANNEL
                    | WIPHY_FLAG_OFFCHAN_TX;
    wiphy->country_ie_pref = NL80211_COUNTRY_IE_IGNORE_CORE;
#endif

#if  defined (WLAN_FEATURE_VOWIFI_11R) || defined (FEATURE_WLAN_CCX) || defined(FEATURE_WLAN_LFR)
    if (pCfg->isFastTransitionEnabled
#ifdef FEATURE_WLAN_LFR
       || pCfg->isFastRoamIniFeatureEnabled
#endif
#ifdef FEATURE_WLAN_CCX
       || pCfg->isCcxIniFeatureEnabled
#endif
    )
    {
        wiphy->flags |= WIPHY_FLAG_SUPPORTS_FW_ROAM;
    }
#endif
#ifdef FEATURE_WLAN_TDLS
    wiphy->flags |= WIPHY_FLAG_SUPPORTS_TDLS
                 |  WIPHY_FLAG_TDLS_EXTERNAL_SETUP;
#endif
#ifdef FEATURE_WLAN_SCAN_PNO
    if (pCfg->configPNOScanSupport)
    {
        wiphy->flags |= WIPHY_FLAG_SUPPORTS_SCHED_SCAN;
        wiphy->max_sched_scan_ssids = SIR_PNO_MAX_SUPP_NETWORKS;
        wiphy->max_match_sets       = SIR_PNO_MAX_SUPP_NETWORKS;
        wiphy->max_sched_scan_ie_len = SIR_MAC_MAX_IE_LENGTH;
    }
#endif/*                     */

#ifdef CONFIG_ENABLE_LINUX_REG
    /*                                        
                                                        
                                                            
                                                    
                           */

    wiphy->reg_notifier = wlan_hdd_linux_reg_notifier;
#else
    wiphy->reg_notifier = wlan_hdd_crda_reg_notifier;
#endif

    wiphy->max_scan_ssids = MAX_SCAN_SSID;

    wiphy->max_scan_ie_len = SIR_MAC_MAX_IE_LENGTH;

    wiphy->max_acl_mac_addrs = MAX_ACL_MAC_ADDRESS;

    /*                                           */
    wiphy->interface_modes =   BIT(NL80211_IFTYPE_STATION)
                             | BIT(NL80211_IFTYPE_ADHOC)
                             | BIT(NL80211_IFTYPE_P2P_CLIENT)
                             | BIT(NL80211_IFTYPE_P2P_GO)
                             | BIT(NL80211_IFTYPE_AP);

    if( pCfg->advertiseConcurrentOperation )
    {
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0))
        if( pCfg->enableMCC )
        {
            /*                                        */
            wlan_hdd_iface_combination.num_different_channels = 2;

            if( !pCfg->allowMCCGODiffBI )
                wlan_hdd_iface_combination.beacon_int_infra_match = true;

        }
        wiphy->iface_combinations = &wlan_hdd_iface_combination;
        wiphy->n_iface_combinations = 1;
#endif
    }

    /*                                                               
                    */
    if( !pCfg->ShortGI20MhzEnable )
    {
        wlan_hdd_band_2_4_GHZ.ht_cap.cap     &= ~IEEE80211_HT_CAP_SGI_20;
        wlan_hdd_band_5_GHZ.ht_cap.cap       &= ~IEEE80211_HT_CAP_SGI_20;
        wlan_hdd_band_p2p_2_4_GHZ.ht_cap.cap &= ~IEEE80211_HT_CAP_SGI_20;
    }

    if( !pCfg->ShortGI40MhzEnable )
    {
        wlan_hdd_band_5_GHZ.ht_cap.cap       &= ~IEEE80211_HT_CAP_SGI_40;
    }

    if( !pCfg->nChannelBondingMode5GHz )
    {
        wlan_hdd_band_5_GHZ.ht_cap.cap &= ~IEEE80211_HT_CAP_SUP_WIDTH_20_40;
    }

   wiphy->bands[IEEE80211_BAND_2GHZ] = &wlan_hdd_band_2_4_GHZ;
   if (true == hdd_is_5g_supported(pHddCtx))
   {
       wiphy->bands[IEEE80211_BAND_5GHZ] = &wlan_hdd_band_5_GHZ;
   }

   for (i = 0; i < IEEE80211_NUM_BANDS; i++)
   {

       if (NULL == wiphy->bands[i])
       {
          hddLog(VOS_TRACE_LEVEL_ERROR,"%s: wiphy->bands[i] is NULL, i = %d",
                 __func__, i);
          continue;
       }

       for (j = 0; j < wiphy->bands[i]->n_channels; j++)
       {
           struct ieee80211_supported_band *band = wiphy->bands[i];

           if (IEEE80211_BAND_2GHZ == i && eCSR_BAND_5G == pCfg->nBandCapability) //        
           {
               //                               
               if (WLAN_HDD_IS_SOCIAL_CHANNEL(band->channels[j].center_freq))
                   band->channels[j].flags &= ~IEEE80211_CHAN_DISABLED;
               else
                   band->channels[j].flags |= IEEE80211_CHAN_DISABLED;
               continue;
           }
           else if (IEEE80211_BAND_5GHZ == i && eCSR_BAND_24 == pCfg->nBandCapability) //        
           {
               band->channels[j].flags |= IEEE80211_CHAN_DISABLED;
               continue;
           }
       }
    }
    /*                                             */
    wiphy->cipher_suites = hdd_cipher_suites;
    wiphy->n_cipher_suites = ARRAY_SIZE(hdd_cipher_suites);

    /*                                 */
    wiphy->signal_type = CFG80211_SIGNAL_TYPE_MBM;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,38))
    wiphy->max_remain_on_channel_duration = 1000;
#endif

    wiphy->n_vendor_commands = 0;
    wiphy->vendor_events = wlan_hdd_cfg80211_vendor_events;
    wiphy->n_vendor_events = ARRAY_SIZE(wlan_hdd_cfg80211_vendor_events);

    EXIT();
    return 0;
}

/*                                            */
int wlan_hdd_cfg80211_register(struct wiphy *wiphy)
{
    ENTER();
 /*                                      */
    if (0 > wiphy_register(wiphy))
    {
        /*             */
        hddLog(VOS_TRACE_LEVEL_ERROR,"%s: wiphy register failed", __func__);
        return -EIO;
    }

    EXIT();
    return 0;
}

/*                                                    
                                                   
                                                          
                                                 
                                           */
void wlan_hdd_cfg80211_update_reg_info(struct wiphy *wiphy)
{
    int j;
    hdd_context_t *pHddCtx = wiphy_priv(wiphy);
    tANI_U8 defaultCountryCode[3] = SME_INVALID_COUNTRY_CODE;
    //                                                                
    if (eHAL_STATUS_SUCCESS != sme_GetDefaultCountryCodeFrmNv(pHddCtx->hHal,
                                  &defaultCountryCode[0]))
    {
       hddLog(LOGE, FL("%s Failed to get default country code from NV"));
    }
    if ((defaultCountryCode[0]== 'U') && (defaultCountryCode[1]=='S'))
    {
       if (NULL == wiphy->bands[IEEE80211_BAND_5GHZ])
       {
          hddLog(VOS_TRACE_LEVEL_ERROR,"%s: wiphy->bands[IEEE80211_BAND_5GHZ] is NULL",__func__ );
          return;
       }
       for (j = 0; j < wiphy->bands[IEEE80211_BAND_5GHZ]->n_channels; j++)
       {
          struct ieee80211_supported_band *band = wiphy->bands[IEEE80211_BAND_5GHZ];
          //                                     
          if (WLAN_HDD_CHANNEL_IN_UNII_1_BAND(band->channels[j].center_freq))
             band->channels[j].flags |= IEEE80211_CHAN_PASSIVE_SCAN;
       }
    }
}

/*                                                               
                                                                      
                 
*/
void wlan_hdd_cfg80211_post_voss_start(hdd_adapter_t* pAdapter)
{
    tHalHandle hHal = WLAN_HDD_GET_HAL_CTX(pAdapter);
    /*                                                       */
    v_U16_t type = (SIR_MAC_MGMT_FRAME << 2) | ( SIR_MAC_MGMT_ACTION << 4);

    ENTER();

   /*                                                                
                                                                       
                                                                   */
    /*                     */
    sme_RegisterMgmtFrame(hHal, pAdapter->sessionId, type,
                         (v_U8_t*)GAS_INITIAL_REQ, GAS_INITIAL_REQ_SIZE );

    /*                      */
    sme_RegisterMgmtFrame(hHal, pAdapter->sessionId, type,
                         (v_U8_t*)GAS_INITIAL_RSP, GAS_INITIAL_RSP_SIZE );

    /*                      */
    sme_RegisterMgmtFrame(hHal, pAdapter->sessionId, type,
                         (v_U8_t*)GAS_COMEBACK_REQ, GAS_COMEBACK_REQ_SIZE );

    /*                       */
    sme_RegisterMgmtFrame(hHal, pAdapter->sessionId, type,
                         (v_U8_t*)GAS_COMEBACK_RSP, GAS_COMEBACK_RSP_SIZE );

    /*                   */
    sme_RegisterMgmtFrame(hHal, pAdapter->sessionId, type,
                         (v_U8_t*)P2P_PUBLIC_ACTION_FRAME,
                                  P2P_PUBLIC_ACTION_FRAME_SIZE );

    /*            */
    sme_RegisterMgmtFrame(hHal, pAdapter->sessionId, type,
                         (v_U8_t*)P2P_ACTION_FRAME,
                                  P2P_ACTION_FRAME_SIZE );

    /*                                  */
    sme_RegisterMgmtFrame(hHal, pAdapter->sessionId, type,
                         (v_U8_t*)WNM_BSS_ACTION_FRAME,
                                  WNM_BSS_ACTION_FRAME_SIZE );

#ifdef WLAN_FEATURE_11W
    /*                                */
    sme_RegisterMgmtFrame(hHal, pAdapter->sessionId, type,
                         (v_U8_t*)SA_QUERY_FRAME_RSP,
                                  SA_QUERY_FRAME_RSP_SIZE );
#endif /*                  */
}

void wlan_hdd_cfg80211_pre_voss_stop(hdd_adapter_t* pAdapter)
{
    tHalHandle hHal = WLAN_HDD_GET_HAL_CTX(pAdapter);
    /*                                                       */
    v_U16_t type = (SIR_MAC_MGMT_FRAME << 2) | ( SIR_MAC_MGMT_ACTION << 4);

    ENTER();

   /*                                                                
                                                                       
                                                                   */
    /*                     */

    sme_DeregisterMgmtFrame(hHal, pAdapter->sessionId, type,
                          (v_U8_t*)GAS_INITIAL_REQ, GAS_INITIAL_REQ_SIZE );

    /*                      */
    sme_DeregisterMgmtFrame(hHal, pAdapter->sessionId, type,
                         (v_U8_t*)GAS_INITIAL_RSP, GAS_INITIAL_RSP_SIZE );

    /*                      */
    sme_DeregisterMgmtFrame(hHal, pAdapter->sessionId, type,
                         (v_U8_t*)GAS_COMEBACK_REQ, GAS_COMEBACK_REQ_SIZE );

    /*                       */
    sme_DeregisterMgmtFrame(hHal, pAdapter->sessionId, type,
                         (v_U8_t*)GAS_COMEBACK_RSP, GAS_COMEBACK_RSP_SIZE );

    /*                   */
    sme_DeregisterMgmtFrame(hHal, pAdapter->sessionId, type,
                         (v_U8_t*)P2P_PUBLIC_ACTION_FRAME,
                                  P2P_PUBLIC_ACTION_FRAME_SIZE );

    /*            */
    sme_DeregisterMgmtFrame(hHal, pAdapter->sessionId, type,
                         (v_U8_t*)P2P_ACTION_FRAME,
                                  P2P_ACTION_FRAME_SIZE );

#ifdef WLAN_FEATURE_11W
    /*                                */
    sme_DeregisterMgmtFrame(hHal, pAdapter->sessionId, type,
                         (v_U8_t*)SA_QUERY_FRAME_RSP,
                                  SA_QUERY_FRAME_RSP_SIZE );
#endif /*                  */
}

#ifdef FEATURE_WLAN_WAPI
void wlan_hdd_cfg80211_set_key_wapi(hdd_adapter_t* pAdapter, u8 key_index,
                                     const u8 *mac_addr, u8 *key , int key_Len)
{
    hdd_station_ctx_t *pHddStaCtx = WLAN_HDD_GET_STATION_CTX_PTR(pAdapter);
    tCsrRoamSetKey  setKey;
    v_BOOL_t isConnected = TRUE;
    int status = 0;
    v_U32_t roamId= 0xFF;
    tANI_U8 *pKeyPtr = NULL;
    int n = 0;

    hddLog(VOS_TRACE_LEVEL_INFO, "%s: device_mode = %d\n",
                                        __func__,pAdapter->device_mode);

    vos_mem_zero(&setKey, sizeof(tCsrRoamSetKey));
    setKey.keyId = key_index; //             
    setKey.encType  = eCSR_ENCRYPT_TYPE_WPI; //                    
    setKey.keyDirection = eSIR_TX_RX;  //                              
    setKey.paeRole = 0 ; //             
    if (!mac_addr || is_broadcast_ether_addr(mac_addr))
    {
        vos_set_macaddr_broadcast( (v_MACADDR_t *)setKey.peerMac );
    }
    else
    {
        isConnected = hdd_connIsConnected(pHddStaCtx);
        vos_mem_copy(setKey.peerMac,&pHddStaCtx->conn_info.bssId,WNI_CFG_BSSID_LEN);
    }
    setKey.keyLength = key_Len;
    pKeyPtr = setKey.Key;
    memcpy( pKeyPtr, key, key_Len);

    hddLog(VOS_TRACE_LEVEL_INFO,"\n%s: WAPI KEY LENGTH:0x%04x",
                                            __func__, key_Len);
    for (n = 0 ; n < key_Len; n++)
        hddLog(VOS_TRACE_LEVEL_INFO, "%s WAPI KEY Data[%d]:%02x ",
                                           __func__,n,setKey.Key[n]);

    pHddStaCtx->roam_info.roamingState = HDD_ROAM_STATE_SETTING_KEY;
    if ( isConnected )
    {
        status= sme_RoamSetKey( WLAN_HDD_GET_HAL_CTX(pAdapter),
                             pAdapter->sessionId, &setKey, &roamId );
    }
    if ( status != 0 )
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                  "[%4d] sme_RoamSetKey returned ERROR status= %d",
                                                __LINE__, status );
        pHddStaCtx->roam_info.roamingState = HDD_ROAM_STATE_NONE;
    }
}
#endif /*                  */

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,4,0))
int wlan_hdd_cfg80211_alloc_new_beacon(hdd_adapter_t *pAdapter,
                                       beacon_data_t **ppBeacon,
                                       struct beacon_parameters *params)
#else
int wlan_hdd_cfg80211_alloc_new_beacon(hdd_adapter_t *pAdapter,
                                       beacon_data_t **ppBeacon,
                                       struct cfg80211_beacon_data *params,
                                       int dtim_period)
#endif
{
    int size;
    beacon_data_t *beacon = NULL;
    beacon_data_t *old = NULL;
    int head_len,tail_len;

    ENTER();
    if (params->head && !params->head_len)
        return -EINVAL;

    old = pAdapter->sessionCtx.ap.beacon;

    if (!params->head && !old)
        return -EINVAL;

#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,38))
    /*                                                       */
    if (!params->dtim_period)
        return -EINVAL;
#endif

    if(params->head)
        head_len = params->head_len;
    else
        head_len = old->head_len;

    if(params->tail || !old)
        tail_len = params->tail_len;
    else
        tail_len = old->tail_len;

    size = sizeof(beacon_data_t) + head_len + tail_len;

    beacon = kzalloc(size, GFP_KERNEL);

    if( beacon == NULL )
        return -ENOMEM;

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,4,0))
    if(params->dtim_period || !old )
        beacon->dtim_period = params->dtim_period;
    else
        beacon->dtim_period = old->dtim_period;
#else
    if(dtim_period || !old )
        beacon->dtim_period = dtim_period;
    else
        beacon->dtim_period = old->dtim_period;
#endif

    beacon->head = ((u8 *) beacon) + sizeof(beacon_data_t);
    beacon->tail = beacon->head + head_len;
    beacon->head_len = head_len;
    beacon->tail_len = tail_len;

    if(params->head) {
        memcpy (beacon->head,params->head,beacon->head_len);
    }
    else {
        if(old)
            memcpy (beacon->head,old->head,beacon->head_len);
    }

    if(params->tail) {
        memcpy (beacon->tail,params->tail,beacon->tail_len);
    }
    else {
       if(old)
           memcpy (beacon->tail,old->tail,beacon->tail_len);
    }

    *ppBeacon = beacon;

    kfree(old);

    return 0;

}

v_U8_t* wlan_hdd_cfg80211_get_ie_ptr(v_U8_t *pIes, int length, v_U8_t eid)
{
    int left = length;
    v_U8_t *ptr = pIes;
    v_U8_t elem_id,elem_len;

    while(left >= 2)
    {
        elem_id  =  ptr[0];
        elem_len =  ptr[1];
        left -= 2;
        if(elem_len > left)
        {
            hddLog(VOS_TRACE_LEVEL_FATAL,
                    FL("****Invalid IEs eid = %d elem_len=%d left=%d*****"),
                                                    eid,elem_len,left);
            return NULL;
        }
        if (elem_id == eid)
        {
            return ptr;
        }

        left -= elem_len;
        ptr += (elem_len + 2);
    }
    return NULL;
}

/*                                  */
static int wlan_hdd_rate_is_11g(u8 rate)
{
    static const u8 gRateArray[8] = {12, 18, 24, 36, 48, 72, 96, 108}; /*                 */
    u8 i;
    for (i = 0; i < 8; i++)
    {
        if(rate == gRateArray[i])
            return TRUE;
    }
    return FALSE;
}

/*                                                 */
static void wlan_hdd_check_11gmode(u8 *pIe, u8* require_ht,
                     u8* pCheckRatesfor11g, eSapPhyMode* pSapHw_mode)
{
    u8 i, num_rates = pIe[0];

    pIe += 1;
    for ( i = 0; i < num_rates; i++)
    {
        if( *pCheckRatesfor11g && (TRUE == wlan_hdd_rate_is_11g(pIe[i] & RATE_MASK)))
        {
            /*                                                       */
            *pSapHw_mode = eSAP_DOT11_MODE_11g;
            if (pIe[i] & BASIC_RATE_MASK)
            {
                /*                                                  
                                    
                 */
               *pSapHw_mode = eSAP_DOT11_MODE_11g_ONLY;
               *pCheckRatesfor11g = FALSE;
            }
        }
        else if((BASIC_RATE_MASK | WLAN_BSS_MEMBERSHIP_SELECTOR_HT_PHY) == pIe[i])
        {
            *require_ht = TRUE;
        }
    }
    return;
}

static void wlan_hdd_set_sapHwmode(hdd_adapter_t *pHostapdAdapter)
{
    tsap_Config_t *pConfig = &pHostapdAdapter->sessionCtx.ap.sapConfig;
    beacon_data_t *pBeacon = pHostapdAdapter->sessionCtx.ap.beacon;
    struct ieee80211_mgmt *pMgmt_frame = (struct ieee80211_mgmt*)pBeacon->head;
    u8 checkRatesfor11g = TRUE;
    u8 require_ht = FALSE;
    u8 *pIe=NULL;

    pConfig->SapHw_mode= eSAP_DOT11_MODE_11b;

    pIe = wlan_hdd_cfg80211_get_ie_ptr(&pMgmt_frame->u.beacon.variable[0],
                                       pBeacon->head_len, WLAN_EID_SUPP_RATES);
    if (pIe != NULL)
    {
        pIe += 1;
        wlan_hdd_check_11gmode(pIe, &require_ht, &checkRatesfor11g,
                               &pConfig->SapHw_mode);
    }

    pIe = wlan_hdd_cfg80211_get_ie_ptr(pBeacon->tail, pBeacon->tail_len,
                                WLAN_EID_EXT_SUPP_RATES);
    if (pIe != NULL)
    {

        pIe += 1;
        wlan_hdd_check_11gmode(pIe, &require_ht, &checkRatesfor11g,
                               &pConfig->SapHw_mode);
    }

    if( pConfig->channel > 14 )
    {
        pConfig->SapHw_mode= eSAP_DOT11_MODE_11a;
    }

    pIe = wlan_hdd_cfg80211_get_ie_ptr(pBeacon->tail, pBeacon->tail_len,
                                       WLAN_EID_HT_CAPABILITY);

    if(pIe)
    {
        pConfig->SapHw_mode= eSAP_DOT11_MODE_11n;
        if(require_ht)
            pConfig->SapHw_mode= eSAP_DOT11_MODE_11n_ONLY;
    }
}

static int wlan_hdd_add_ie(hdd_adapter_t* pHostapdAdapter, v_U8_t *genie,
                              v_U8_t *total_ielen, v_U8_t *oui, v_U8_t oui_size)
{
    v_U16_t ielen = 0;
    v_U8_t *pIe = NULL;
    beacon_data_t *pBeacon = pHostapdAdapter->sessionCtx.ap.beacon;

    pIe = wlan_hdd_get_vendor_oui_ie_ptr(oui, oui_size,
                                          pBeacon->tail, pBeacon->tail_len);

    if (pIe)
    {
        ielen = pIe[1] + 2;
        if ((*total_ielen + ielen) <= MAX_GENIE_LEN)
        {
            vos_mem_copy(&genie[*total_ielen], pIe, ielen);
        }
        else
        {
            hddLog( VOS_TRACE_LEVEL_ERROR, "**Ie Length is too big***");
            return -EINVAL;
        }
        *total_ielen += ielen;
    }
    return 0;
}

static void wlan_hdd_add_hostapd_conf_vsie(hdd_adapter_t* pHostapdAdapter,
                                           v_U8_t *genie, v_U8_t *total_ielen)
{
    beacon_data_t *pBeacon = pHostapdAdapter->sessionCtx.ap.beacon;
    int left = pBeacon->tail_len;
    v_U8_t *ptr = pBeacon->tail;
    v_U8_t elem_id, elem_len;
    v_U16_t ielen = 0;

    if ( NULL == ptr || 0 == left )
        return;

    while (left >= 2)
    {
        elem_id  = ptr[0];
        elem_len = ptr[1];
        left -= 2;
        if (elem_len > left)
        {
            hddLog( VOS_TRACE_LEVEL_ERROR,
                    "****Invalid IEs eid = %d elem_len=%d left=%d*****",
                    elem_id, elem_len, left);
            return;
        }
        if (IE_EID_VENDOR == elem_id)
        {
            /*                                                      
                                                   
             */
            if ((memcmp( &ptr[2], WPS_OUI_TYPE, WPS_OUI_TYPE_SIZE) != 0 ) &&
#ifdef WLAN_FEATURE_WFD
               (memcmp( &ptr[2], WFD_OUI_TYPE, WFD_OUI_TYPE_SIZE) != 0) &&
#endif
               (memcmp( &ptr[2], WHITELIST_OUI_TYPE, WPA_OUI_TYPE_SIZE) != 0) &&
               (memcmp( &ptr[2], BLACKLIST_OUI_TYPE, WPA_OUI_TYPE_SIZE) != 0) &&
               (memcmp( &ptr[2], "\x00\x50\xf2\x02", WPA_OUI_TYPE_SIZE) != 0) &&
               (memcmp( &ptr[2], WPA_OUI_TYPE, WPA_OUI_TYPE_SIZE) != 0) &&
               (memcmp( &ptr[2], P2P_OUI_TYPE, P2P_OUI_TYPE_SIZE) != 0))
            {
               ielen = ptr[1] + 2;
               if ((*total_ielen + ielen) <= MAX_GENIE_LEN)
               {
                   vos_mem_copy(&genie[*total_ielen], ptr, ielen);
                   *total_ielen += ielen;
               }
               else
               {
                   hddLog( VOS_TRACE_LEVEL_ERROR,
                           "IE Length is too big "
                           "IEs eid=%d elem_len=%d total_ie_lent=%d",
                           elem_id, elem_len, *total_ielen);
               }
            }
        }

        left -= elem_len;
        ptr += (elem_len + 2);
    }
    return;
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,4,0))
static int wlan_hdd_cfg80211_update_apies(hdd_adapter_t* pHostapdAdapter,
                            struct beacon_parameters *params)
#else
static int wlan_hdd_cfg80211_update_apies(hdd_adapter_t* pHostapdAdapter,
                                     struct cfg80211_beacon_data *params)
#endif
{
    v_U8_t *genie;
    v_U8_t total_ielen = 0;
    v_U8_t addIE[1] = {0};
    int ret = 0;

    genie = vos_mem_malloc(MAX_GENIE_LEN);

    if(genie == NULL) {

        return -ENOMEM;
    }

    if (0 != wlan_hdd_add_ie(pHostapdAdapter, genie,
                              &total_ielen, WPS_OUI_TYPE, WPS_OUI_TYPE_SIZE))
    {
         ret = -EINVAL;
         goto done;
    }

#ifdef WLAN_FEATURE_WFD
    if (0 != wlan_hdd_add_ie(pHostapdAdapter, genie,
                              &total_ielen, WFD_OUI_TYPE, WFD_OUI_TYPE_SIZE))
    {
         ret = -EINVAL;
         goto done;
    }
#endif

    if (0 != wlan_hdd_add_ie(pHostapdAdapter, genie,
                              &total_ielen, P2P_OUI_TYPE, P2P_OUI_TYPE_SIZE))
    {
         ret = -EINVAL;
         goto done;
    }

    if (WLAN_HDD_SOFTAP == pHostapdAdapter->device_mode)
    {
        wlan_hdd_add_hostapd_conf_vsie(pHostapdAdapter, genie, &total_ielen);
    }

    if (ccmCfgSetStr((WLAN_HDD_GET_CTX(pHostapdAdapter))->hHal,
       WNI_CFG_PROBE_RSP_BCN_ADDNIE_DATA, genie, total_ielen, NULL,
               eANI_BOOLEAN_FALSE)==eHAL_STATUS_FAILURE)
    {
        hddLog(LOGE,
               "Could not pass on WNI_CFG_PROBE_RSP_BCN_ADDNIE_DATA to CCM");
        ret = -EINVAL;
        goto done;
    }

    if (ccmCfgSetInt((WLAN_HDD_GET_CTX(pHostapdAdapter))->hHal,
          WNI_CFG_PROBE_RSP_BCN_ADDNIE_FLAG, 1,NULL,
          test_bit(SOFTAP_BSS_STARTED, &pHostapdAdapter->event_flags) ?
                   eANI_BOOLEAN_TRUE : eANI_BOOLEAN_FALSE)
          ==eHAL_STATUS_FAILURE)
    {
        hddLog(LOGE,
            "Could not pass on WNI_CFG_PROBE_RSP_BCN_ADDNIE_FLAG to CCM");
        ret = -EINVAL;
        goto done;
    }

    //                     
    if ( (params->proberesp_ies != NULL) && (params->proberesp_ies_len != 0) )
    {
        u16 rem_probe_resp_ie_len = params->proberesp_ies_len;
        u8 probe_rsp_ie_len[3] = {0};
        u8 counter = 0;
        /*                                                             
                                                               
                                                                         
                                                       
        */
        while ((rem_probe_resp_ie_len > 0) && (counter < 3))
        {
            if (rem_probe_resp_ie_len > MAX_CFG_STRING_LEN)
            {
                probe_rsp_ie_len[counter++] = MAX_CFG_STRING_LEN;
                rem_probe_resp_ie_len -= MAX_CFG_STRING_LEN;
            }
            else
            {
                probe_rsp_ie_len[counter++] = rem_probe_resp_ie_len;
                rem_probe_resp_ie_len = 0;
            }
        }

        rem_probe_resp_ie_len = 0;

        if (probe_rsp_ie_len[0] > 0)
        {
            if (ccmCfgSetStr((WLAN_HDD_GET_CTX(pHostapdAdapter))->hHal,
                            WNI_CFG_PROBE_RSP_ADDNIE_DATA1,
                            (tANI_U8*)&params->proberesp_ies[rem_probe_resp_ie_len],
                            probe_rsp_ie_len[0], NULL,
                            eANI_BOOLEAN_FALSE) == eHAL_STATUS_FAILURE)
            {
                 hddLog(LOGE,
                       "Could not pass on WNI_CFG_PROBE_RSP_ADDNIE_DATA1 to CCM");
                 ret = -EINVAL;
                 goto done;
            }
            rem_probe_resp_ie_len += probe_rsp_ie_len[0];
        }

        if (probe_rsp_ie_len[1] > 0)
        {
            if (ccmCfgSetStr((WLAN_HDD_GET_CTX(pHostapdAdapter))->hHal,
                            WNI_CFG_PROBE_RSP_ADDNIE_DATA2,
                            (tANI_U8*)&params->proberesp_ies[rem_probe_resp_ie_len],
                            probe_rsp_ie_len[1], NULL,
                            eANI_BOOLEAN_FALSE) == eHAL_STATUS_FAILURE)
            {
                 hddLog(LOGE,
                       "Could not pass on WNI_CFG_PROBE_RSP_ADDNIE_DATA2 to CCM");
                 ret = -EINVAL;
                 goto done;
            }
            rem_probe_resp_ie_len += probe_rsp_ie_len[1];
        }

        if (probe_rsp_ie_len[2] > 0)
        {
            if (ccmCfgSetStr((WLAN_HDD_GET_CTX(pHostapdAdapter))->hHal,
                            WNI_CFG_PROBE_RSP_ADDNIE_DATA3,
                            (tANI_U8*)&params->proberesp_ies[rem_probe_resp_ie_len],
                            probe_rsp_ie_len[2], NULL,
                            eANI_BOOLEAN_FALSE) == eHAL_STATUS_FAILURE)
            {
                 hddLog(LOGE,
                       "Could not pass on WNI_CFG_PROBE_RSP_ADDNIE_DATA3 to CCM");
                 ret = -EINVAL;
                 goto done;
            }
            rem_probe_resp_ie_len += probe_rsp_ie_len[2];
        }

        if (probe_rsp_ie_len[1] == 0 )
        {
            if ( eHAL_STATUS_FAILURE == ccmCfgSetStr((WLAN_HDD_GET_CTX(pHostapdAdapter))->hHal,
                            WNI_CFG_PROBE_RSP_ADDNIE_DATA2, (tANI_U8*)addIE, 0, NULL,
                            eANI_BOOLEAN_FALSE) )
            {
                hddLog(LOGE,
                   "Could not pass on WNI_CFG_PROBE_RSP_ADDNIE_DATA2 to CCM\n");
            }
        }

        if (probe_rsp_ie_len[2] == 0 )
        {
            if ( eHAL_STATUS_FAILURE == ccmCfgSetStr((WLAN_HDD_GET_CTX(pHostapdAdapter))->hHal,
                            WNI_CFG_PROBE_RSP_ADDNIE_DATA3, (tANI_U8*)addIE, 0, NULL,
                            eANI_BOOLEAN_FALSE) )
            {
                hddLog(LOGE,
                   "Could not pass on WNI_CFG_PROBE_RSP_ADDNIE_DATA3 to CCM\n");
            }
        }

        if (ccmCfgSetInt((WLAN_HDD_GET_CTX(pHostapdAdapter))->hHal,
             WNI_CFG_PROBE_RSP_ADDNIE_FLAG, 1,NULL,
             test_bit(SOFTAP_BSS_STARTED, &pHostapdAdapter->event_flags) ?
                      eANI_BOOLEAN_TRUE : eANI_BOOLEAN_FALSE)
             == eHAL_STATUS_FAILURE)
        {
           hddLog(LOGE,
             "Could not pass on WNI_CFG_PROBE_RSP_ADDNIE_FLAG to CCM");
           ret = -EINVAL;
           goto done;
        }
    }
    else
    {
        //                              
        wlan_hdd_reset_prob_rspies(pHostapdAdapter);

        hddLog(VOS_TRACE_LEVEL_INFO,
               "%s: No Probe Response IE received in set beacon",
               __func__);
    }

    //                       
    if ( (params->assocresp_ies != NULL) && (params->assocresp_ies_len != 0) )
    {
       if (ccmCfgSetStr((WLAN_HDD_GET_CTX(pHostapdAdapter))->hHal,
               WNI_CFG_ASSOC_RSP_ADDNIE_DATA, (tANI_U8*)params->assocresp_ies,
               params->assocresp_ies_len, NULL,
               eANI_BOOLEAN_FALSE) == eHAL_STATUS_FAILURE)
       {
            hddLog(LOGE,
                  "Could not pass on WNI_CFG_ASSOC_RSP_ADDNIE_DATA to CCM");
            ret = -EINVAL;
            goto done;
       }

       if (ccmCfgSetInt((WLAN_HDD_GET_CTX(pHostapdAdapter))->hHal,
          WNI_CFG_ASSOC_RSP_ADDNIE_FLAG, 1, NULL,
          test_bit(SOFTAP_BSS_STARTED, &pHostapdAdapter->event_flags) ?
                   eANI_BOOLEAN_TRUE : eANI_BOOLEAN_FALSE)
          == eHAL_STATUS_FAILURE)
       {
          hddLog(LOGE,
            "Could not pass on WNI_CFG_ASSOC_RSP_ADDNIE_FLAG to CCM");
          ret = -EINVAL;
          goto done;
       }
    }
    else
    {
        hddLog(VOS_TRACE_LEVEL_INFO,
               "%s: No Assoc Response IE received in set beacon",
               __func__);

        if ( eHAL_STATUS_FAILURE == ccmCfgSetInt((WLAN_HDD_GET_CTX(pHostapdAdapter))->hHal,
                            WNI_CFG_ASSOC_RSP_ADDNIE_FLAG, 0, NULL,
                            eANI_BOOLEAN_FALSE) )
        {
            hddLog(LOGE,
               "Could not pass on WNI_CFG_ASSOC_RSP_ADDNIE_FLAG to CCM\n");
        }
    }

done:
    vos_mem_free(genie);
    return ret;
}

/*
                                                
                                              
                                  
                                                                 
                
 */
VOS_STATUS wlan_hdd_validate_operation_channel(hdd_adapter_t *pAdapter,int channel)
{

    v_U32_t num_ch = 0;
    u8 valid_ch[WNI_CFG_VALID_CHANNEL_LIST_LEN];
    u32 indx = 0;
    tHalHandle hHal = WLAN_HDD_GET_HAL_CTX(pAdapter);
    v_U8_t fValidChannel = FALSE, count = 0;
    hdd_config_t *hdd_pConfig_ini= (WLAN_HDD_GET_CTX(pAdapter))->cfg_ini;

    num_ch = WNI_CFG_VALID_CHANNEL_LIST_LEN;

    if ( hdd_pConfig_ini->sapAllowAllChannel)
    {
         /*                      */
        for (count = RF_CHAN_1 ; count <= RF_CHAN_165 ; count++)
        {
            if ( channel == rfChannels[count].channelNum )
            {
                fValidChannel = TRUE;
                break;
            }
        }
        if (fValidChannel != TRUE)
        {
            hddLog(VOS_TRACE_LEVEL_ERROR,
                "%s: Invalid Channel [%d]", __func__, channel);
            return VOS_STATUS_E_FAILURE;
        }
    }
    else
    {
        if (0 != ccmCfgGetStr(hHal, WNI_CFG_VALID_CHANNEL_LIST,
            valid_ch, &num_ch))
            {
                hddLog(VOS_TRACE_LEVEL_ERROR,
                    "%s: failed to get valid channel list", __func__);
                return VOS_STATUS_E_FAILURE;
            }
        for (indx = 0; indx < num_ch; indx++)
        {
            if (channel == valid_ch[indx])
            {
                break;
            }
         }

        if (indx >= num_ch)
        {
            hddLog(VOS_TRACE_LEVEL_ERROR,
                "%s: Invalid Channel [%d]", __func__, channel);
            return VOS_STATUS_E_FAILURE;
        }
    }
    return VOS_STATUS_SUCCESS;

}

/* 
                                          
                                                  
 */
static int wlan_hdd_cfg80211_set_channel( struct wiphy *wiphy, struct net_device *dev,
                                   struct ieee80211_channel *chan,
                                   enum nl80211_channel_type channel_type
                                 )
{
    v_U32_t num_ch = 0;
    int channel = 0;
    hdd_adapter_t *pAdapter = NULL;
    int freq = chan->center_freq; /*                */
    hdd_context_t *pHddCtx;
    int status;

    ENTER();

    if( NULL == dev )
    {
        hddLog(VOS_TRACE_LEVEL_ERROR,
                "%s: Called with dev = NULL.\n", __func__);
        return -ENODEV;
    }
    pAdapter = WLAN_HDD_GET_PRIV_PTR( dev );

    hddLog(VOS_TRACE_LEVEL_INFO,
                "%s: device_mode = %d  freq = %d \n",__func__,
                            pAdapter->device_mode, chan->center_freq);

    pHddCtx = WLAN_HDD_GET_CTX(pAdapter);
    status = wlan_hdd_validate_context(pHddCtx);

    if (0 != status)
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                   "%s: HDD context is not valid", __func__);
        return status;
    }

    /*
                                 
                    
     */

    channel = ieee80211_frequency_to_channel(freq);

    /*                  */
    if ((WNI_CFG_CURRENT_CHANNEL_STAMIN > channel) ||
            (WNI_CFG_CURRENT_CHANNEL_STAMAX < channel))
    {
        hddLog(VOS_TRACE_LEVEL_ERROR,
                "%s: Channel [%d] is outside valid range from %d to %d\n",
                __func__, channel, WNI_CFG_CURRENT_CHANNEL_STAMIN,
                WNI_CFG_CURRENT_CHANNEL_STAMAX);
        return -EINVAL;
    }

    num_ch = WNI_CFG_VALID_CHANNEL_LIST_LEN;

    if ((WLAN_HDD_SOFTAP != pAdapter->device_mode) &&
       (WLAN_HDD_P2P_GO != pAdapter->device_mode))
    {
        if(VOS_STATUS_SUCCESS != wlan_hdd_validate_operation_channel(pAdapter,channel))
        {
            hddLog(VOS_TRACE_LEVEL_ERROR,
                    "%s: Invalid Channel [%d] \n", __func__, channel);
            return -EINVAL;
        }
        hddLog(VOS_TRACE_LEVEL_INFO_HIGH,
                "%s: set channel to [%d] for device mode =%d",
                          __func__, channel,pAdapter->device_mode);
    }
    if( (pAdapter->device_mode == WLAN_HDD_INFRA_STATION)
     || (pAdapter->device_mode == WLAN_HDD_P2P_CLIENT)
      )
    {
        hdd_wext_state_t *pWextState = WLAN_HDD_GET_WEXT_STATE_PTR(pAdapter);
        tCsrRoamProfile * pRoamProfile = &pWextState->roamProfile;
        hdd_station_ctx_t *pHddStaCtx = WLAN_HDD_GET_STATION_CTX_PTR(pAdapter);

        if (eConnectionState_IbssConnected == pHddStaCtx->conn_info.connState)
        {
           /*                                        */
            hddLog( VOS_TRACE_LEVEL_ERROR,
                   "%s: IBSS Associated, can't set the channel\n", __func__);
            return -EINVAL;
        }

        num_ch = pRoamProfile->ChannelInfo.numOfChannels = 1;
        pHddStaCtx->conn_info.operationChannel = channel;
        pRoamProfile->ChannelInfo.ChannelList =
            &pHddStaCtx->conn_info.operationChannel;
    }
    else if ((pAdapter->device_mode == WLAN_HDD_SOFTAP)
        ||   (pAdapter->device_mode == WLAN_HDD_P2P_GO)
            )
    {
        if (WLAN_HDD_P2P_GO == pAdapter->device_mode)
        {
            if(VOS_STATUS_SUCCESS !=
                       wlan_hdd_validate_operation_channel(pAdapter,channel))
            {
               hddLog(VOS_TRACE_LEVEL_ERROR,
                      "%s: Invalid Channel [%d] \n", __func__, channel);
               return -EINVAL;
            }
            (WLAN_HDD_GET_AP_CTX_PTR(pAdapter))->sapConfig.channel = channel;
        }
        else if ( WLAN_HDD_SOFTAP == pAdapter->device_mode )
        {
            hdd_config_t *cfg_param = (WLAN_HDD_GET_CTX(pAdapter))->cfg_ini;

            /*                                                                 
                                     
            */
            if ( cfg_param->apAutoChannelSelection )
            {
                (WLAN_HDD_GET_AP_CTX_PTR(pAdapter))->sapConfig.channel =
                                                          AUTO_CHANNEL_SELECT;
                hddLog(VOS_TRACE_LEVEL_INFO_HIGH,
                       "%s: set channel to auto channel (0) for device mode =%d",
                       __func__, pAdapter->device_mode);
            }
            else
            {
                if(VOS_STATUS_SUCCESS !=
                         wlan_hdd_validate_operation_channel(pAdapter,channel))
                {
                   hddLog(VOS_TRACE_LEVEL_ERROR,
                          "%s: Invalid Channel [%d] \n", __func__, channel);
                   return -EINVAL;
                }
                (WLAN_HDD_GET_AP_CTX_PTR(pAdapter))->sapConfig.channel = channel;
            }
        }
    }
    else
    {
        hddLog(VOS_TRACE_LEVEL_FATAL,
               "%s: Invalid device mode failed to set valid channel", __func__);
        return -EINVAL;
    }
    EXIT();
    return status;
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,4,0))
static int wlan_hdd_cfg80211_start_bss(hdd_adapter_t *pHostapdAdapter,
                            struct beacon_parameters *params)
#else
static int wlan_hdd_cfg80211_start_bss(hdd_adapter_t *pHostapdAdapter,
                                       struct cfg80211_beacon_data *params,
                                       const u8 *ssid, size_t ssid_len,
                                       enum nl80211_hidden_ssid hidden_ssid)
#endif
{
    tsap_Config_t *pConfig;
    beacon_data_t *pBeacon = NULL;
    struct ieee80211_mgmt *pMgmt_frame;
    v_U8_t *pIe=NULL;
    v_U16_t capab_info;
    eCsrAuthType RSNAuthType;
    eCsrEncryptionType RSNEncryptType;
    eCsrEncryptionType mcRSNEncryptType;
    int status = VOS_STATUS_SUCCESS;
    tpWLAN_SAPEventCB pSapEventCallback;
    hdd_hostapd_state_t *pHostapdState;
    v_U8_t wpaRsnIEdata[(SIR_MAC_MAX_IE_LENGTH * 2)+4];  //                                                                 
    v_CONTEXT_t pVosContext = (WLAN_HDD_GET_CTX(pHostapdAdapter))->pvosContext;
    tHalHandle hHal = WLAN_HDD_GET_HAL_CTX(pHostapdAdapter);
    struct qc_mac_acl_entry *acl_entry = NULL;
    v_SINT_t i;
    hdd_context_t *pHddCtx = WLAN_HDD_GET_CTX(pHostapdAdapter);
    eHddDot11Mode sapDot11Mode =
            (WLAN_HDD_GET_CTX(pHostapdAdapter))->cfg_ini->sapDot11Mode;

    ENTER();

    pHostapdState = WLAN_HDD_GET_HOSTAP_STATE_PTR(pHostapdAdapter);

    pConfig = &pHostapdAdapter->sessionCtx.ap.sapConfig;

    pBeacon = pHostapdAdapter->sessionCtx.ap.beacon;

    pMgmt_frame = (struct ieee80211_mgmt*)pBeacon->head;

    pConfig->beacon_int =  pMgmt_frame->u.beacon.beacon_int;

    //                                                   
    //                                          

    /*                                         */
    pConfig->protEnabled =
           (WLAN_HDD_GET_CTX(pHostapdAdapter))->cfg_ini->apProtEnabled;

    pConfig->dtim_period = pBeacon->dtim_period;

    hddLog(VOS_TRACE_LEVEL_INFO_HIGH,"****pConfig->dtim_period=%d***\n",
                                      pConfig->dtim_period);


    if (pHostapdAdapter->device_mode == WLAN_HDD_SOFTAP)
    {
        pIe = wlan_hdd_cfg80211_get_ie_ptr(pBeacon->tail, pBeacon->tail_len,
                                       WLAN_EID_COUNTRY);
        if(memcmp(pHddCtx->cfg_ini->apCntryCode, CFG_AP_COUNTRY_CODE_DEFAULT, 3) != 0)
        {
           tANI_BOOLEAN restartNeeded;
           pConfig->ieee80211d = 1;
           vos_mem_copy(pConfig->countryCode, pHddCtx->cfg_ini->apCntryCode, 3);
           sme_setRegInfo(hHal, pConfig->countryCode);
           sme_ResetCountryCodeInformation(hHal, &restartNeeded);
        }
        else if(pIe)
        {
            tANI_BOOLEAN restartNeeded;
            pConfig->ieee80211d = 1;
            vos_mem_copy(pConfig->countryCode, &pIe[2], 3);
            sme_setRegInfo(hHal, pConfig->countryCode);
            sme_ResetCountryCodeInformation(hHal, &restartNeeded);
        }
        else
        {
            pConfig->ieee80211d = 0;
        }
        /*
                                                          
                                     
        */
        if( AUTO_CHANNEL_SELECT != pConfig->channel )
        {
            if(VOS_STATUS_SUCCESS != wlan_hdd_validate_operation_channel(pHostapdAdapter,pConfig->channel))
            {
                 hddLog(VOS_TRACE_LEVEL_ERROR,
                         "%s: Invalid Channel [%d] \n", __func__, pConfig->channel);
                 return -EINVAL;
            }
        }
        else
        {
             if(1 != pHddCtx->is_dynamic_channel_range_set)
             {
                  hdd_config_t *hdd_pConfig= (WLAN_HDD_GET_CTX(pHostapdAdapter))->cfg_ini;
                  WLANSAP_SetChannelRange(hHal, hdd_pConfig->apStartChannelNum,
                       hdd_pConfig->apEndChannelNum,hdd_pConfig->apOperatingBand);
             }
                   pHddCtx->is_dynamic_channel_range_set = 0;
        }
    }
    else
    {
        pConfig->ieee80211d = 0;
    }
    pConfig->authType = eSAP_AUTO_SWITCH;

    capab_info = pMgmt_frame->u.beacon.capab_info;

    pConfig->privacy = (pMgmt_frame->u.beacon.capab_info &
                        WLAN_CAPABILITY_PRIVACY) ? VOS_TRUE : VOS_FALSE;

    (WLAN_HDD_GET_AP_CTX_PTR(pHostapdAdapter))->uPrivacy = pConfig->privacy;

    /*                             */
    pIe = wlan_hdd_get_wps_ie_ptr(pBeacon->tail, pBeacon->tail_len);

    if(pIe)
    {
        if(pIe[1] < (2 + WPS_OUI_TYPE_SIZE))
        {
            hddLog( VOS_TRACE_LEVEL_ERROR, "**Wps Ie Length is too small***\n");
            return -EINVAL;
        }
        else if(memcmp(&pIe[2], WPS_OUI_TYPE, WPS_OUI_TYPE_SIZE) == 0)
        {
             hddLog( VOS_TRACE_LEVEL_INFO, "** WPS IE(len %d) ***", (pIe[1]+2));
             /*                                                               
                         
              */
              if(SAP_WPS_ENABLED_UNCONFIGURED == pIe[15])
              {
                  pConfig->wps_state = SAP_WPS_ENABLED_UNCONFIGURED;
              } else if(SAP_WPS_ENABLED_CONFIGURED == pIe[15])
              {
                  pConfig->wps_state = SAP_WPS_ENABLED_CONFIGURED;
              }
        }
    }
    else
    {
        pConfig->wps_state = SAP_WPS_DISABLED;
    }
    pConfig->fwdWPSPBCProbeReq  = 1; //                                       

    pConfig->RSNWPAReqIELength = 0;
    pConfig->pRSNWPAReqIE = NULL;
    pIe = wlan_hdd_cfg80211_get_ie_ptr(pBeacon->tail, pBeacon->tail_len,
                                       WLAN_EID_RSN);
    if(pIe && pIe[1])
    {
        pConfig->RSNWPAReqIELength = pIe[1] + 2;
        memcpy(&wpaRsnIEdata[0], pIe, pConfig->RSNWPAReqIELength);
        pConfig->pRSNWPAReqIE = &wpaRsnIEdata[0];
        /*                                                            
                                                                     
                      
           */
        status = hdd_softap_unpackIE(
                        vos_get_context( VOS_MODULE_ID_SME, pVosContext),
                        &RSNEncryptType,
                        &mcRSNEncryptType,
                        &RSNAuthType,
                        pConfig->pRSNWPAReqIE[1]+2,
                        pConfig->pRSNWPAReqIE );

        if( VOS_STATUS_SUCCESS == status )
        {
            /*                                                   
                         
               */
            pConfig->RSNEncryptType = RSNEncryptType; //                                  
            pConfig->mcRSNEncryptType = mcRSNEncryptType;
            (WLAN_HDD_GET_AP_CTX_PTR(pHostapdAdapter))->ucEncryptType
                                                              = RSNEncryptType;
            hddLog( LOG1, FL("CSR AuthType = %d, "
                        "EncryptionType = %d mcEncryptionType = %d\n"),
                        RSNAuthType, RSNEncryptType, mcRSNEncryptType);
        }
    }

    pIe = wlan_hdd_get_vendor_oui_ie_ptr(WPA_OUI_TYPE, WPA_OUI_TYPE_SIZE,
                                         pBeacon->tail, pBeacon->tail_len);

    if(pIe && pIe[1] && (pIe[0] == DOT11F_EID_WPA))
    {
        if (pConfig->pRSNWPAReqIE)
        {
            /*                   */
            memcpy((&wpaRsnIEdata[0] + pConfig->RSNWPAReqIELength), pIe, pIe[1] + 2);
            pConfig->RSNWPAReqIELength += pIe[1] + 2;
        }
        else
        {
            pConfig->RSNWPAReqIELength = pIe[1] + 2;
            memcpy(&wpaRsnIEdata[0], pIe, pConfig->RSNWPAReqIELength);
            pConfig->pRSNWPAReqIE = &wpaRsnIEdata[0];
            status = hdd_softap_unpackIE(
                          vos_get_context( VOS_MODULE_ID_SME, pVosContext),
                          &RSNEncryptType,
                          &mcRSNEncryptType,
                          &RSNAuthType,
                          pConfig->pRSNWPAReqIE[1]+2,
                          pConfig->pRSNWPAReqIE );

            if( VOS_STATUS_SUCCESS == status )
            {
                /*                                                   
                             
                   */
                pConfig->RSNEncryptType = RSNEncryptType; //                                  
                pConfig->mcRSNEncryptType = mcRSNEncryptType;
                (WLAN_HDD_GET_AP_CTX_PTR(pHostapdAdapter))->ucEncryptType
                                                              = RSNEncryptType;
                hddLog( LOG1, FL("CSR AuthType = %d, "
                                "EncryptionType = %d mcEncryptionType = %d\n"),
                                RSNAuthType, RSNEncryptType, mcRSNEncryptType);
            }
        }
    }

    if (pConfig->RSNWPAReqIELength > sizeof wpaRsnIEdata) {
        hddLog( VOS_TRACE_LEVEL_ERROR, "**RSNWPAReqIELength is too large***");
        return -EINVAL;
    }

    pConfig->SSIDinfo.ssidHidden = VOS_FALSE;

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,4,0))
    if (params->ssid != NULL)
    {
        memcpy(pConfig->SSIDinfo.ssid.ssId, params->ssid, params->ssid_len);
        pConfig->SSIDinfo.ssid.length = params->ssid_len;
        if (params->hidden_ssid != NL80211_HIDDEN_SSID_NOT_IN_USE)
            pConfig->SSIDinfo.ssidHidden = VOS_TRUE;
    }
#else
    if (ssid != NULL)
    {
        memcpy(pConfig->SSIDinfo.ssid.ssId, ssid, ssid_len);
        pConfig->SSIDinfo.ssid.length = ssid_len;
        if (hidden_ssid != NL80211_HIDDEN_SSID_NOT_IN_USE)
            pConfig->SSIDinfo.ssidHidden = VOS_TRUE;
    }
#endif

    vos_mem_copy(pConfig->self_macaddr.bytes,
               pHostapdAdapter->macAddressCurrent.bytes, sizeof(v_MACADDR_t));

    /*               */
    pConfig->SapMacaddr_acl = eSAP_ACCEPT_UNLESS_DENIED;
    pConfig->num_accept_mac = 0;
    pConfig->num_deny_mac = 0;

    pIe = wlan_hdd_get_vendor_oui_ie_ptr(BLACKLIST_OUI_TYPE, WPA_OUI_TYPE_SIZE,
                                         pBeacon->tail, pBeacon->tail_len);

    /*                                      
                            
                            
                             
                             
                                                
                                                                                                
    */
    if ((pIe != NULL) && (pIe[1] != 0))
    {
        pConfig->SapMacaddr_acl = pIe[6];
        pConfig->num_deny_mac   = pIe[7];
        hddLog(VOS_TRACE_LEVEL_INFO,"acl type = %d no deny mac = %d\n",
                                     pIe[6], pIe[7]);
        if (pConfig->num_deny_mac > MAX_ACL_MAC_ADDRESS)
            pConfig->num_deny_mac = MAX_ACL_MAC_ADDRESS;
        acl_entry = (struct qc_mac_acl_entry *)(pIe + 8);
        for (i = 0; i < pConfig->num_deny_mac; i++)
        {
            vos_mem_copy(&pConfig->deny_mac[i], acl_entry->addr, sizeof(qcmacaddr));
            acl_entry++;
        }
    }
    pIe = wlan_hdd_get_vendor_oui_ie_ptr(WHITELIST_OUI_TYPE, WPA_OUI_TYPE_SIZE,
                                         pBeacon->tail, pBeacon->tail_len);

    /*                                      
                            
                            
                             
                             
                                                
                                                                                                
    */
    if ((pIe != NULL) && (pIe[1] != 0))
    {
        pConfig->SapMacaddr_acl = pIe[6];
        pConfig->num_accept_mac   = pIe[7];
        hddLog(VOS_TRACE_LEVEL_INFO,"acl type = %d no accept mac = %d\n",
                                      pIe[6], pIe[7]);
        if (pConfig->num_accept_mac > MAX_ACL_MAC_ADDRESS)
            pConfig->num_accept_mac = MAX_ACL_MAC_ADDRESS;
        acl_entry = (struct qc_mac_acl_entry *)(pIe + 8);
        for (i = 0; i < pConfig->num_accept_mac; i++)
        {
            vos_mem_copy(&pConfig->accept_mac[i], acl_entry->addr, sizeof(qcmacaddr));
            acl_entry++;
        }
    }

    wlan_hdd_set_sapHwmode(pHostapdAdapter);

#ifdef WLAN_FEATURE_11AC
    /*                                                         
                                                                          
                                                                    
                                                                          
     */
    if( ((pConfig->SapHw_mode == eSAP_DOT11_MODE_11n) ||
         (pConfig->SapHw_mode == eSAP_DOT11_MODE_11n_ONLY)) &&
        (( sapDot11Mode == eHDD_DOT11_MODE_AUTO ) ||
         ( sapDot11Mode == eHDD_DOT11_MODE_11ac ) ||
         ( sapDot11Mode == eHDD_DOT11_MODE_11ac_ONLY ) ) &&
         (sme_IsFeatureSupportedByDriver(DOT11AC)) &&
          (sme_IsFeatureSupportedByFW(DOT11AC)) )
    {
        pConfig->SapHw_mode = eSAP_DOT11_MODE_11ac;

        /*                                     */
        if (pConfig->channel <= 14 &&
            (WLAN_HDD_GET_CTX(pHostapdAdapter))->cfg_ini->enableVhtFor24GHzBand == FALSE)
        {
            pConfig->SapHw_mode = eSAP_DOT11_MODE_11n;
        }
    }
#endif

    if ( AUTO_CHANNEL_SELECT != pConfig->channel )
    {
        sme_SelectCBMode(hHal,
            sapConvertSapPhyModeToCsrPhyMode(pConfig->SapHw_mode),
            pConfig->channel);
    }
    //                                                                         
    pConfig->ht_capab =
                 (WLAN_HDD_GET_CTX(pHostapdAdapter))->cfg_ini->apProtection;

    if ( 0 != wlan_hdd_cfg80211_update_apies(pHostapdAdapter, params) )
    {
        hddLog(LOGE, FL("SAP Not able to set AP IEs"));
        return -EINVAL;
    }

    //                 
    pConfig->UapsdEnable =
          (WLAN_HDD_GET_CTX(pHostapdAdapter))->cfg_ini->apUapsdEnabled;
    //                      
    pConfig->obssProtEnabled =
           (WLAN_HDD_GET_CTX(pHostapdAdapter))->cfg_ini->apOBSSProtEnabled;

    hddLog(LOGW, FL("SOftAP macaddress : "MAC_ADDRESS_STR"\n"),
                 MAC_ADDR_ARRAY(pHostapdAdapter->macAddressCurrent.bytes));
    hddLog(LOGW,FL("ssid =%s\n"), pConfig->SSIDinfo.ssid.ssId);
    hddLog(LOGW,FL("beaconint=%d, channel=%d\n"), (int)pConfig->beacon_int,
                                                     (int)pConfig->channel);
    hddLog(LOGW,FL("hw_mode=%x\n"),  pConfig->SapHw_mode);
    hddLog(LOGW,FL("privacy=%d, authType=%d\n"), pConfig->privacy,
                                                 pConfig->authType);
    hddLog(LOGW,FL("RSN/WPALen=%d, \n"),(int)pConfig->RSNWPAReqIELength);
    hddLog(LOGW,FL("Uapsd = %d\n"),pConfig->UapsdEnable);
    hddLog(LOGW,FL("ProtEnabled = %d, OBSSProtEnabled = %d\n"),
                          pConfig->protEnabled, pConfig->obssProtEnabled);

    if(test_bit(SOFTAP_BSS_STARTED, &pHostapdAdapter->event_flags))
    {
        //                                 
        //                                                  
        hddLog( LOGE, "Bss Already started...Ignore the request");
        EXIT();
        return 0;
    }

    pConfig->persona = pHostapdAdapter->device_mode;

    pSapEventCallback = hdd_hostapd_SAPEventCB;
    if(WLANSAP_StartBss(pVosContext, pSapEventCallback, pConfig,
                 (v_PVOID_t)pHostapdAdapter->dev) != VOS_STATUS_SUCCESS)
    {
        hddLog(LOGE,FL("SAP Start Bss fail\n"));
        return -EINVAL;
    }

    hddLog(LOG1,
           FL("Waiting for Scan to complete(auto mode) and BSS to start"));

    status = vos_wait_single_event(&pHostapdState->vosEvent, 10000);

    if (!VOS_IS_STATUS_SUCCESS(status))
    {
        VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                 ("ERROR: HDD vos wait for single_event failed!!\n"));
        smeGetCommandQStatus(hHal);
        VOS_ASSERT(0);
    }

    //                                             
    set_bit(SOFTAP_BSS_STARTED, &pHostapdAdapter->event_flags);

#ifdef WLAN_FEATURE_P2P_DEBUG
    if (pHostapdAdapter->device_mode == WLAN_HDD_P2P_GO)
    {
         if(globalP2PConnectionStatus == P2P_GO_NEG_COMPLETED)
         {
             globalP2PConnectionStatus = P2P_GO_COMPLETED_STATE;
             hddLog(LOGE,"[P2P State] From Go nego completed to "
                         "Non-autonomous Group started");
         }
         else if(globalP2PConnectionStatus == P2P_NOT_ACTIVE)
         {
             globalP2PConnectionStatus = P2P_GO_COMPLETED_STATE;
             hddLog(LOGE,"[P2P State] From Inactive to "
                         "Autonomous Group started");
         }
    }
#endif

    pHostapdState->bCommit = TRUE;
    EXIT();

   return 0;
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,4,0))
static int wlan_hdd_cfg80211_add_beacon(struct wiphy *wiphy,
                                        struct net_device *dev,
                                        struct beacon_parameters *params)
{
    hdd_adapter_t *pAdapter =  WLAN_HDD_GET_PRIV_PTR(dev);
    hdd_context_t  *pHddCtx;
    int status;

    ENTER();

    hddLog(VOS_TRACE_LEVEL_INFO_HIGH, "device mode=%d\n",pAdapter->device_mode);

    pHddCtx = WLAN_HDD_GET_CTX(pAdapter);
    status = wlan_hdd_validate_context(pHddCtx);

    if (0 != status)
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                   "%s: HDD context is not valid", __func__);
        return status;
    }

    if ( (pAdapter->device_mode == WLAN_HDD_SOFTAP)
      || (pAdapter->device_mode == WLAN_HDD_P2P_GO)
       )
    {
        beacon_data_t *old,*new;

        old = pAdapter->sessionCtx.ap.beacon;

        if (old)
            return -EALREADY;

        status = wlan_hdd_cfg80211_alloc_new_beacon(pAdapter,&new,params);

        if(status != VOS_STATUS_SUCCESS)
        {
             hddLog(VOS_TRACE_LEVEL_FATAL,
                   "%s:Error!!! Allocating the new beacon\n",__func__);
             return -EINVAL;
        }

        pAdapter->sessionCtx.ap.beacon = new;

        status = wlan_hdd_cfg80211_start_bss(pAdapter, params);
    }

    EXIT();
    return status;
}

static int wlan_hdd_cfg80211_set_beacon(struct wiphy *wiphy,
                                        struct net_device *dev,
                                        struct beacon_parameters *params)
{
    hdd_adapter_t *pAdapter =  WLAN_HDD_GET_PRIV_PTR(dev);
    hdd_context_t  *pHddCtx;
    int status;

    ENTER();

    hddLog(VOS_TRACE_LEVEL_INFO, "%s: device_mode = %d\n",
                                __func__,pAdapter->device_mode);

    pHddCtx = WLAN_HDD_GET_CTX(pAdapter);
    status = wlan_hdd_validate_context(pHddCtx);

    if (0 != status)
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                   "%s: HDD context is not valid", __func__);
        return status;
    }

    if ((pAdapter->device_mode == WLAN_HDD_SOFTAP)
     || (pAdapter->device_mode == WLAN_HDD_P2P_GO)
       )
    {
        beacon_data_t *old,*new;

        old = pAdapter->sessionCtx.ap.beacon;

        if (!old)
            return -ENOENT;

        status = wlan_hdd_cfg80211_alloc_new_beacon(pAdapter,&new,params);

        if(status != VOS_STATUS_SUCCESS) {
            hddLog(VOS_TRACE_LEVEL_FATAL,
                   "%s: Error!!! Allocating the new beacon\n",__func__);
            return -EINVAL;
       }

       pAdapter->sessionCtx.ap.beacon = new;

       status = wlan_hdd_cfg80211_start_bss(pAdapter, params);
    }

    EXIT();
    return status;
}

#endif //                                            

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,4,0))
static int wlan_hdd_cfg80211_del_beacon(struct wiphy *wiphy,
                                        struct net_device *dev)
#else
static int wlan_hdd_cfg80211_stop_ap (struct wiphy *wiphy,
                                      struct net_device *dev)
#endif
{
    hdd_adapter_t *pAdapter =  WLAN_HDD_GET_PRIV_PTR(dev);
    hdd_context_t  *pHddCtx    = NULL;
    hdd_scaninfo_t *pScanInfo  = NULL;
    hdd_adapter_t  *staAdapter = NULL;
    VOS_STATUS status;

    ENTER();

    if (NULL == pAdapter)
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_FATAL,
                   "%s: HDD adapter context is Null", __func__);
        return -ENODEV;
    }

    pHddCtx = WLAN_HDD_GET_CTX(pAdapter);
    status = wlan_hdd_validate_context(pHddCtx);

    if (0 != status)
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                   "%s: HDD context is not valid", __func__);
        return status;
    }

    staAdapter = hdd_get_adapter(pAdapter->pHddCtx, WLAN_HDD_INFRA_STATION);
    if (NULL == staAdapter)
    {
        staAdapter = hdd_get_adapter(pAdapter->pHddCtx, WLAN_HDD_P2P_CLIENT);
        if (NULL == staAdapter)
        {
            VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO_HIGH,
                      "%s: HDD adapter context for STA/P2P-CLI is Null",
                      __func__);
        }
    }

    pScanInfo =  &pHddCtx->scan_info;

    hddLog(VOS_TRACE_LEVEL_INFO, "%s: device_mode = %d\n",
                              __func__,pAdapter->device_mode);

    if ((pScanInfo != NULL) && pScanInfo->mScanPending && staAdapter)
    {
        INIT_COMPLETION(pScanInfo->abortscan_event_var);
        hdd_abort_mac_scan(staAdapter->pHddCtx);
        status = wait_for_completion_interruptible_timeout(
                           &pScanInfo->abortscan_event_var,
                           msecs_to_jiffies(WLAN_WAIT_TIME_ABORTSCAN));
        if (!status)
        {
            VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                         "%s: Timeout occurred while waiting for abortscan" ,
                         __func__);
            VOS_ASSERT(pScanInfo->mScanPending);
        }
    }

    if ((pAdapter->device_mode == WLAN_HDD_SOFTAP)
     || (pAdapter->device_mode == WLAN_HDD_P2P_GO)
       )
    {
        beacon_data_t *old;

        old = pAdapter->sessionCtx.ap.beacon;

        if (!old)
            return -ENOENT;

        hdd_cleanup_actionframe(pHddCtx, pAdapter);

        mutex_lock(&pHddCtx->sap_lock);
        if(test_bit(SOFTAP_BSS_STARTED, &pAdapter->event_flags))
        {
            if ( VOS_STATUS_SUCCESS == (status = WLANSAP_StopBss(pHddCtx->pvosContext) ) )
            {
                hdd_hostapd_state_t *pHostapdState = WLAN_HDD_GET_HOSTAP_STATE_PTR(pAdapter);

                status = vos_wait_single_event(&pHostapdState->vosEvent, 10000);

                if (!VOS_IS_STATUS_SUCCESS(status))
                {
                    VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                             ("ERROR: HDD vos wait for single_event failed!!\n"));
                    VOS_ASSERT(0);
                }
            }
            clear_bit(SOFTAP_BSS_STARTED, &pAdapter->event_flags);
        }
        mutex_unlock(&pHddCtx->sap_lock);

        if(status != VOS_STATUS_SUCCESS)
        {
            hddLog(VOS_TRACE_LEVEL_FATAL,
                    "%s:Error!!! Stopping the BSS\n",__func__);
            return -EINVAL;
        }

        if (ccmCfgSetInt(pHddCtx->hHal,
            WNI_CFG_PROBE_RSP_BCN_ADDNIE_FLAG, 0,NULL, eANI_BOOLEAN_FALSE)
                                                    ==eHAL_STATUS_FAILURE)
        {
            hddLog(LOGE,
               "Could not pass on WNI_CFG_PROBE_RSP_BCN_ADDNIE_FLAG to CCM\n");
        }

        if ( eHAL_STATUS_FAILURE == ccmCfgSetInt(pHddCtx->hHal,
                            WNI_CFG_ASSOC_RSP_ADDNIE_FLAG, 0, NULL,
                            eANI_BOOLEAN_FALSE) )
        {
            hddLog(LOGE,
               "Could not pass on WNI_CFG_ASSOC_RSP_ADDNIE_FLAG to CCM\n");
        }

        //                              
        wlan_hdd_reset_prob_rspies(pAdapter);

        pAdapter->sessionCtx.ap.beacon = NULL;
        kfree(old);
#ifdef WLAN_FEATURE_P2P_DEBUG
        if((pAdapter->device_mode == WLAN_HDD_P2P_GO) &&
           (globalP2PConnectionStatus == P2P_GO_COMPLETED_STATE))
        {
            hddLog(LOGE,"[P2P State] From GO completed to Inactive state "
                        "GO got removed");
            globalP2PConnectionStatus = P2P_NOT_ACTIVE;
        }
#endif
    }
    EXIT();
    return status;
}

#if (LINUX_VERSION_CODE > KERNEL_VERSION(3,3,0))

static int wlan_hdd_cfg80211_start_ap(struct wiphy *wiphy,
                                      struct net_device *dev,
                                      struct cfg80211_ap_settings *params)
{
    hdd_adapter_t *pAdapter;
    hdd_context_t *pHddCtx;
    int            status;

    ENTER();

    if (NULL == dev)
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                   "%s: Device is Null", __func__);
        return -ENODEV;
    }

    pAdapter = WLAN_HDD_GET_PRIV_PTR(dev);
    if (NULL == pAdapter)
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                   "%s: HDD adapter is Null", __func__);
        return -ENODEV;
    }

    if (WLAN_HDD_ADAPTER_MAGIC != pAdapter->magic)
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                   "%s: HDD adapter magic is invalid", __func__);
        return -ENODEV;
    }

    pHddCtx = WLAN_HDD_GET_CTX(pAdapter);
    status = wlan_hdd_validate_context(pHddCtx);

    if (0 != status)
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                   "%s: HDD context is not valid", __func__);
        return status;
    }

    hddLog(VOS_TRACE_LEVEL_INFO_HIGH, "%s: device mode = %d",
           __func__, pAdapter->device_mode);

    if ((pAdapter->device_mode == WLAN_HDD_SOFTAP)
      || (pAdapter->device_mode == WLAN_HDD_P2P_GO)
       )
    {
        beacon_data_t  *old, *new;

        old = pAdapter->sessionCtx.ap.beacon;

        if (old)
            return -EALREADY;

        status = wlan_hdd_cfg80211_alloc_new_beacon(pAdapter, &new, &params->beacon, params->dtim_period);

        if (status != 0)
        {
             hddLog(VOS_TRACE_LEVEL_FATAL,
                   "%s:Error!!! Allocating the new beacon", __func__);
             return -EINVAL;
        }
        pAdapter->sessionCtx.ap.beacon = new;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))
        wlan_hdd_cfg80211_set_channel(wiphy, dev,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,8,0))
        params->channel, params->channel_type);
#else
        params->chandef.chan, cfg80211_get_chandef_type(&(params->chandef)));
#endif
#endif
        status = wlan_hdd_cfg80211_start_bss(pAdapter, &params->beacon, params->ssid,
                                             params->ssid_len, params->hidden_ssid);
    }

    EXIT();
    return status;
}


static int wlan_hdd_cfg80211_change_beacon(struct wiphy *wiphy,
                                        struct net_device *dev,
                                        struct cfg80211_beacon_data *params)
{
    hdd_adapter_t *pAdapter =  WLAN_HDD_GET_PRIV_PTR(dev);
    hdd_context_t *pHddCtx;
    int status;

    ENTER();

    hddLog(VOS_TRACE_LEVEL_INFO, "%s: device_mode = %d\n",
                                __func__, pAdapter->device_mode);

    pHddCtx = WLAN_HDD_GET_CTX(pAdapter);
    status = wlan_hdd_validate_context(pHddCtx);

    if (0 != status)
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                   "%s: HDD context is not valid", __func__);
        return status;
    }

    if ((pAdapter->device_mode == WLAN_HDD_SOFTAP)
     || (pAdapter->device_mode == WLAN_HDD_P2P_GO)
       )
    {
        beacon_data_t *old,*new;

        old = pAdapter->sessionCtx.ap.beacon;

        if (!old)
            return -ENOENT;

        status = wlan_hdd_cfg80211_alloc_new_beacon(pAdapter, &new, params, 0);

        if(status != VOS_STATUS_SUCCESS) {
            hddLog(VOS_TRACE_LEVEL_FATAL,
                   "%s: Error!!! Allocating the new beacon\n",__func__);
            return -EINVAL;
       }

       pAdapter->sessionCtx.ap.beacon = new;

       status = wlan_hdd_cfg80211_start_bss(pAdapter, params, NULL, 0, 0);
    }

    EXIT();
    return status;
}

#endif //                                            


static int wlan_hdd_cfg80211_change_bss (struct wiphy *wiphy,
                                      struct net_device *dev,
                                      struct bss_parameters *params)
{
    hdd_adapter_t *pAdapter =  WLAN_HDD_GET_PRIV_PTR(dev);

    ENTER();

    hddLog(VOS_TRACE_LEVEL_INFO, "%s: device_mode = %d\n",
                               __func__,pAdapter->device_mode);

    if((pAdapter->device_mode == WLAN_HDD_SOFTAP)
     ||  (pAdapter->device_mode == WLAN_HDD_P2P_GO)
      )
    {
        /*                                                               
                                         */
        if (-1 != params->ap_isolate)
        {
            pAdapter->sessionCtx.ap.apDisableIntraBssFwd = !!params->ap_isolate;
        }
    }

    EXIT();
    return 0;
}

/*                                          
                                     
*/
void* wlan_hdd_change_country_code_cb(void *pAdapter)
{
    hdd_adapter_t *call_back_pAdapter = pAdapter;
    complete(&call_back_pAdapter->change_country_code);
    return NULL;
}

/*
                                           
                                                                         
 */
int wlan_hdd_cfg80211_change_iface( struct wiphy *wiphy,
                                    struct net_device *ndev,
                                    enum nl80211_iftype type,
                                    u32 *flags,
                                    struct vif_params *params
                                  )
{
    struct wireless_dev *wdev;
    hdd_adapter_t *pAdapter = WLAN_HDD_GET_PRIV_PTR( ndev );
    hdd_context_t *pHddCtx = WLAN_HDD_GET_CTX( pAdapter );
    hdd_adapter_t  *pP2pAdapter = NULL;
    tCsrRoamProfile *pRoamProfile = NULL;
    eCsrRoamBssType LastBSSType;
    hdd_config_t *pConfig = NULL;
    eMib_dot11DesiredBssType connectedBssType;
    VOS_STATUS status;

    ENTER();

    status = wlan_hdd_validate_context(pHddCtx);

    if (0 != status)
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                   "%s: HDD context is not valid", __func__);
        return status;
    }

    hddLog(VOS_TRACE_LEVEL_INFO, "%s: device_mode = %d",
                             __func__, pAdapter->device_mode);

    pConfig = pHddCtx->cfg_ini;
    wdev = ndev->ieee80211_ptr;

#ifdef WLAN_BTAMP_FEATURE
    if((NL80211_IFTYPE_P2P_CLIENT == type)||
       (NL80211_IFTYPE_ADHOC == type)||
       (NL80211_IFTYPE_AP == type)||
       (NL80211_IFTYPE_P2P_GO == type))
    {
        pHddCtx->isAmpAllowed = VOS_FALSE;
        //                 
        status = WLANBAP_StopAmp();
        if(VOS_STATUS_SUCCESS != status )
        {
            pHddCtx->isAmpAllowed = VOS_TRUE;
            hddLog(VOS_TRACE_LEVEL_FATAL,
                   "%s: Failed to stop AMP", __func__);
            return -EINVAL;
        }
    }
#endif //                  
    /*                                       */
    wlan_hdd_clear_concurrency_mode(pHddCtx, pAdapter->device_mode);

    if( (pAdapter->device_mode == WLAN_HDD_INFRA_STATION)
      || (pAdapter->device_mode == WLAN_HDD_P2P_CLIENT)
      || (pAdapter->device_mode == WLAN_HDD_P2P_DEVICE)
      )
    {
        hdd_wext_state_t *pWextState = WLAN_HDD_GET_WEXT_STATE_PTR(pAdapter);
        pRoamProfile = &pWextState->roamProfile;
        LastBSSType = pRoamProfile->BSSType;

        switch (type)
        {
            case NL80211_IFTYPE_STATION:
            case NL80211_IFTYPE_P2P_CLIENT:
                hddLog(VOS_TRACE_LEVEL_INFO,
                   "%s: setting interface Type to INFRASTRUCTURE", __func__);
                pRoamProfile->BSSType = eCSR_BSS_TYPE_INFRASTRUCTURE;
#ifdef WLAN_FEATURE_11AC
                if(pConfig->dot11Mode == eHDD_DOT11_MODE_AUTO)
                {
                    pConfig->dot11Mode = eHDD_DOT11_MODE_11ac;
                }
#endif
                pRoamProfile->phyMode =
                hdd_cfg_xlate_to_csr_phy_mode(pConfig->dot11Mode);
                wdev->iftype = type;
                //                                                       
                if (NULL != strstr(ndev->name,"p2p"))
                {
                    pAdapter->device_mode = (type == NL80211_IFTYPE_STATION) ?
                                WLAN_HDD_P2P_DEVICE : WLAN_HDD_P2P_CLIENT;
                }
                else
                {
                    pAdapter->device_mode = (type == NL80211_IFTYPE_STATION) ?
                                WLAN_HDD_INFRA_STATION: WLAN_HDD_P2P_CLIENT;
                }
#ifdef FEATURE_WLAN_TDLS
                /*                                                           
                                                                           
                                                                              
                                                                          
                                       
                 */

                if ( pAdapter->device_mode == WLAN_HDD_P2P_CLIENT)
                {
                    if (0 != wlan_hdd_tdls_init (pAdapter))
                    {
                        return -EINVAL;
                    }
                }
#endif

                break;
            case NL80211_IFTYPE_ADHOC:
                hddLog(VOS_TRACE_LEVEL_INFO,
                  "%s: setting interface Type to ADHOC", __func__);
                pRoamProfile->BSSType = eCSR_BSS_TYPE_START_IBSS;
                pRoamProfile->phyMode =
                    hdd_cfg_xlate_to_csr_phy_mode(pConfig->dot11Mode);
                pAdapter->device_mode = WLAN_HDD_IBSS;
                wdev->iftype = type;
                break;

            case NL80211_IFTYPE_AP:
            case NL80211_IFTYPE_P2P_GO:
            {
                hddLog(VOS_TRACE_LEVEL_INFO_HIGH,
                      "%s: setting interface Type to %s", __func__,
                      (type == NL80211_IFTYPE_AP) ? "SoftAP" : "P2pGo");

                //                                        
                if (NL80211_IFTYPE_P2P_GO == type)
                {
                    wlan_hdd_cancel_existing_remain_on_channel(pAdapter);
                }
                if (NL80211_IFTYPE_AP == type)
                {
                     /*                                                                  
                                                                                        
                                                                                            
                                                                                    
                                                                       
                      */

                    //                
                    pP2pAdapter = hdd_get_adapter(pHddCtx, WLAN_HDD_P2P_DEVICE);

                    if (pP2pAdapter)
                    {
                        hdd_stop_adapter(pHddCtx, pP2pAdapter);
                        hdd_deinit_adapter(pHddCtx, pP2pAdapter);
                        hdd_close_adapter(pHddCtx, pP2pAdapter, VOS_TRUE);
                    }
                }

//                                                                                                  
                //                              
                if(VOS_STATUS_E_FAILURE ==
                       hdd_disable_bmps_imps(pHddCtx, WLAN_HDD_P2P_GO))
                {
                    //                 
                    VOS_ASSERT(0);
                }
//                                                                                                  

                //                    
                hdd_stop_adapter( pHddCtx, pAdapter );
                hdd_deinit_adapter( pHddCtx, pAdapter );
                memset(&pAdapter->sessionCtx, 0, sizeof(pAdapter->sessionCtx));
                pAdapter->device_mode = (type == NL80211_IFTYPE_AP) ?
                                   WLAN_HDD_SOFTAP : WLAN_HDD_P2P_GO;

//                                                                                                  
/*
                                                  
                                    
                                                            
                 
                                              
                                                                       
                     
                                          
                                     
                     
                 
*/
//                                                                                                  

                if ((WLAN_HDD_SOFTAP == pAdapter->device_mode) &&
                    (pConfig->apRandomBssidEnabled))
                {
                    /*                                                 
                                                                 */
                    get_random_bytes(&ndev->dev_addr[3], 3);
                    ndev->dev_addr[0] = 0x02;
                    ndev->dev_addr[1] = 0x1A;
                    ndev->dev_addr[2] = 0x11;
                    ndev->dev_addr[3] |= 0xF0;
                    memcpy(pAdapter->macAddressCurrent.bytes, ndev->dev_addr,
                           VOS_MAC_ADDR_SIZE);
                    pr_info("wlan: Generated HotSpot BSSID " MAC_ADDRESS_STR"\n",
                            MAC_ADDR_ARRAY(ndev->dev_addr));
                }

                hdd_set_ap_ops( pAdapter->dev );

                /*                                          
                                               
                                                      
                                                     */
                if((NL80211_IFTYPE_AP == type) &&
                   (memcmp(pConfig->apCntryCode, CFG_AP_COUNTRY_CODE_DEFAULT, 3) != 0))
                {
                    int status = 0;
                    VOS_TRACE(VOS_MODULE_ID_HDD,VOS_TRACE_LEVEL_INFO,
                         "%s: setting country code from INI ", __func__);
                    init_completion(&pAdapter->change_country_code);
                    status = (int)sme_ChangeCountryCode(pHddCtx->hHal,
                                     (void *)(tSmeChangeCountryCallback)
                                      wlan_hdd_change_country_code_cb,
                                      pConfig->apCntryCode, pAdapter,
                                      pHddCtx->pvosContext,
                                      eSIR_FALSE,
                                      eSIR_TRUE);
                    if (eHAL_STATUS_SUCCESS == status)
                    {
                        /*                     */
                        status = wait_for_completion_interruptible_timeout(
                                       &pAdapter->change_country_code,
                                       msecs_to_jiffies(WLAN_WAIT_TIME_COUNTRY));
                        if (status <= 0)
                        {
                            VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                                   "%s: SME Timed out while setting country code ",
                                                 __func__);
                        }
                    }
                    else
                    {
                         VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                          "%s: SME Change Country code failed \n",__func__);
                         return -EINVAL;
                    }
                }
                status = hdd_init_ap_mode(pAdapter);
                if(status != VOS_STATUS_SUCCESS)
                {
                    hddLog(VOS_TRACE_LEVEL_FATAL,
                           "%s: Error initializing the ap mode", __func__);
                    return -EINVAL;
                }
                hdd_set_conparam(1);

                /*                                                */
                if(wdev)
                {
                    wdev->iftype = type;
                    pHddCtx->change_iface = type;
                }
                else
                {
                    hddLog(VOS_TRACE_LEVEL_ERROR,
                            "%s: ERROR !!!! Wireless dev is NULL", __func__);
                    return -EINVAL;
                }
                goto done;
            }

            default:
                hddLog(VOS_TRACE_LEVEL_ERROR, "%s: Unsupported interface Type",
                        __func__);
                return -EOPNOTSUPP;
        }
    }
    else if ( (pAdapter->device_mode == WLAN_HDD_SOFTAP)
           || (pAdapter->device_mode == WLAN_HDD_P2P_GO)
            )
    {
       switch(type)
       {
           case NL80211_IFTYPE_STATION:
           case NL80211_IFTYPE_P2P_CLIENT:
           case NL80211_IFTYPE_ADHOC:
                hdd_stop_adapter( pHddCtx, pAdapter );
                hdd_deinit_adapter( pHddCtx, pAdapter );
                wdev->iftype = type;
                //                                                       
                if (NULL != strstr(ndev->name,"p2p"))
                {
                    pAdapter->device_mode = (type == NL80211_IFTYPE_STATION) ?
                                  WLAN_HDD_P2P_DEVICE : WLAN_HDD_P2P_CLIENT;
                }
                else
                {
                    pAdapter->device_mode = (type == NL80211_IFTYPE_STATION) ?
                                  WLAN_HDD_INFRA_STATION: WLAN_HDD_P2P_CLIENT;
                }
                hdd_set_conparam(0);
                pHddCtx->change_iface = type;
                memset(&pAdapter->sessionCtx, 0, sizeof(pAdapter->sessionCtx));
                hdd_set_station_ops( pAdapter->dev );
                status = hdd_init_station_mode( pAdapter );
                if( VOS_STATUS_SUCCESS != status )
                    return -EOPNOTSUPP;
                /*                                                                 
                                                                     
                 */
                if (pHddCtx->hdd_wlan_suspended)
                {
                    hdd_set_pwrparams(pHddCtx);
                }
                hdd_enable_bmps_imps(pHddCtx);
                goto done;
            case NL80211_IFTYPE_AP:
            case NL80211_IFTYPE_P2P_GO:
                wdev->iftype = type;
                pAdapter->device_mode = (type == NL80211_IFTYPE_AP) ?
                                        WLAN_HDD_SOFTAP : WLAN_HDD_P2P_GO;
               goto done;
           default:
                hddLog(VOS_TRACE_LEVEL_ERROR, "%s: Unsupported interface Type",
                        __func__);
                return -EOPNOTSUPP;

       }

    }
    else
    {
      return -EOPNOTSUPP;
    }


    if(pRoamProfile)
    {
        if ( LastBSSType != pRoamProfile->BSSType )
        {
            /*                                                */
            wdev->iftype = type;

            /*                                                 
                                                      */
            if ( hdd_connGetConnectedBssType(
                 WLAN_HDD_GET_STATION_CTX_PTR(pAdapter), &connectedBssType ) ||
                ( eCSR_BSS_TYPE_START_IBSS == LastBSSType ) )
            {
                /*                                  */
                INIT_COMPLETION(pAdapter->disconnect_comp_var);
                if( eHAL_STATUS_SUCCESS ==
                        sme_RoamDisconnect( WLAN_HDD_GET_HAL_CTX(pAdapter),
                                pAdapter->sessionId,
                                eCSR_DISCONNECT_REASON_UNSPECIFIED ) )
                {
                    wait_for_completion_interruptible_timeout(
                              &pAdapter->disconnect_comp_var,
                              msecs_to_jiffies(WLAN_WAIT_TIME_DISCONNECT));
                }
            }
        }
    }

done:
    /*                                  */
    wlan_hdd_set_concurrency_mode(pHddCtx, pAdapter->device_mode);

   /*                             
                                                    */
    if ( (pHddCtx->cfg_ini->thermalMitigationEnable) &&
         (~VOS_STA & pHddCtx->concurrency_mode) )
    {
        hddDevTmLevelChangedHandler(pHddCtx->parent_dev, 0);
    }

#ifdef WLAN_BTAMP_FEATURE
    if((NL80211_IFTYPE_STATION == type) && (pHddCtx->concurrency_mode <= 1) &&
       (pHddCtx->no_of_sessions[WLAN_HDD_INFRA_STATION] <=1))
    {
        //                   
        pHddCtx->isAmpAllowed = VOS_TRUE;
    }
#endif //                  
    EXIT();
    return 0;
}

#ifdef FEATURE_WLAN_TDLS
static int wlan_hdd_tdls_add_station(struct wiphy *wiphy,
          struct net_device *dev, u8 *mac, bool update, tCsrStaParams *StaParams)
{
    hdd_adapter_t *pAdapter = WLAN_HDD_GET_PRIV_PTR(dev);
    hdd_context_t *pHddCtx = wiphy_priv(wiphy);
    VOS_STATUS status;
    hddTdlsPeer_t *pTdlsPeer;

    ENTER();

    if (NULL == pHddCtx || NULL == pHddCtx->cfg_ini)
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                "Invalid arguments");
        return -EINVAL;
    }

    if ((eTDLS_SUPPORT_NOT_ENABLED == pHddCtx->tdls_mode) ||
        (eTDLS_SUPPORT_DISABLED == pHddCtx->tdls_mode))
    {
         VOS_TRACE( VOS_MODULE_ID_HDD, TDLS_LOG_LEVEL,
                    "%s: TDLS mode is disabled OR not enabled in FW."
                    MAC_ADDRESS_STR " Request declined.",
                    __func__, MAC_ADDR_ARRAY(mac));
        return -ENOTSUPP;
    }

    if (pHddCtx->isLogpInProgress)
    {
        VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                "%s:LOGP in Progress. Ignore!!!", __func__);
        wlan_hdd_tdls_set_link_status(pAdapter, mac, eTDLS_LINK_IDLE);
        return -EBUSY;
    }

    pTdlsPeer = wlan_hdd_tdls_find_peer(pAdapter, mac);

    if ( NULL == pTdlsPeer ) {
        VOS_TRACE( VOS_MODULE_ID_HDD, TDLS_LOG_LEVEL,
               "%s: " MAC_ADDRESS_STR " (update %d) not exist. return invalid",
               __func__, MAC_ADDR_ARRAY(mac), update);
        return -EINVAL;
    }

    /*                                                            */
    if ((0 == update) &&
        ((pTdlsPeer->link_status >= eTDLS_LINK_CONNECTING) ||
         (TDLS_STA_INDEX_VALID(pTdlsPeer->staId))))
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, TDLS_LOG_LEVEL,
                   "%s: " MAC_ADDRESS_STR
                   " link_status %d. staId %d. add station ignored.",
                   __func__, MAC_ADDR_ARRAY(mac), pTdlsPeer->link_status, pTdlsPeer->staId);
        return 0;
    }
    /*                                                       */
    if ((1 == update) &&
        ((pTdlsPeer->link_status > eTDLS_LINK_CONNECTING) ||
         (!TDLS_STA_INDEX_VALID(pTdlsPeer->staId))))
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, TDLS_LOG_LEVEL,
                   "%s: " MAC_ADDRESS_STR
                   " link status %d. staId %d. change station %s.",
                   __func__, MAC_ADDR_ARRAY(mac), pTdlsPeer->link_status, pTdlsPeer->staId,
                   (TDLS_STA_INDEX_VALID(pTdlsPeer->staId)) ? "ignored" : "declined");
        return (TDLS_STA_INDEX_VALID(pTdlsPeer->staId)) ? 0 : -EPERM;
    }

    /*                                                                 */
    if (NULL != wlan_hdd_tdls_is_progress(pHddCtx, mac, TRUE, TRUE))
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                   "%s: " MAC_ADDRESS_STR
                   " TDLS setup is ongoing. Request declined.",
                   __func__, MAC_ADDR_ARRAY(mac));
        goto error;
    }

    /*                                                             
                                                       
                                                                   */
    if (HDD_MAX_NUM_TDLS_STA <= wlan_hdd_tdlsConnectedPeers(pAdapter))
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                   "%s: " MAC_ADDRESS_STR
                   " TDLS Max peer already connected. Request declined.",
                    __func__, MAC_ADDR_ARRAY(mac));
        goto error;
    }
    else
    {
        hddTdlsPeer_t *pTdlsPeer;
        pTdlsPeer = wlan_hdd_tdls_find_peer(pAdapter, mac);
        if (pTdlsPeer && TDLS_IS_CONNECTED(pTdlsPeer))
        {
            VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                       "%s: " MAC_ADDRESS_STR " already connected. Request declined.",
                       __func__, MAC_ADDR_ARRAY(mac));
            return -EPERM;
        }
    }
    if (0 == update)
        wlan_hdd_tdls_set_link_status(pAdapter, mac, eTDLS_LINK_CONNECTING);

    /*            */
    if (NULL != StaParams)
    {
        VOS_TRACE(VOS_MODULE_ID_HDD, TDLS_LOG_LEVEL,
                  "%s: TDLS Peer Parameters.", __func__);
        if(StaParams->htcap_present)
        {
            VOS_TRACE(VOS_MODULE_ID_HDD, TDLS_LOG_LEVEL,
                      "ht_capa->cap_info: %0x", StaParams->HTCap.capInfo);
            VOS_TRACE(VOS_MODULE_ID_HDD, TDLS_LOG_LEVEL,
                      "ht_capa->extended_capabilities: %0x",
                      StaParams->HTCap.extendedHtCapInfo);
        }
        VOS_TRACE(VOS_MODULE_ID_HDD, TDLS_LOG_LEVEL,
                  "params->capability: %0x",StaParams->capability);
        VOS_TRACE(VOS_MODULE_ID_HDD, TDLS_LOG_LEVEL,
                  "params->ext_capab_len: %0x",StaParams->extn_capability);
        if(StaParams->vhtcap_present)
        {
            VOS_TRACE(VOS_MODULE_ID_HDD, TDLS_LOG_LEVEL,
                      "rxMcsMap %x rxHighest %x txMcsMap %x txHighest %x",
                      StaParams->VHTCap.suppMcs.rxMcsMap, StaParams->VHTCap.suppMcs.rxHighest,
                      StaParams->VHTCap.suppMcs.txMcsMap, StaParams->VHTCap.suppMcs.txHighest);
        }
        {
            int i = 0;
            VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO, "Supported rates:");
            for (i = 0; i < sizeof(StaParams->supported_rates); i++)
               VOS_TRACE(VOS_MODULE_ID_HDD, TDLS_LOG_LEVEL,
                          "[%d]: %x ", i, StaParams->supported_rates[i]);
        }
    }  /*                */
    else if ((1 == update) && (NULL == StaParams))
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                   "%s : update is true, but staParams is NULL. Error!", __func__);
        return -EPERM;
    }

    INIT_COMPLETION(pAdapter->tdls_add_station_comp);

    if (!update)
    {
        status = sme_AddTdlsPeerSta(WLAN_HDD_GET_HAL_CTX(pAdapter),
                pAdapter->sessionId, mac);
    }
    else
    {
        status = sme_ChangeTdlsPeerSta(WLAN_HDD_GET_HAL_CTX(pAdapter),
                                       pAdapter->sessionId, mac, StaParams);
    }

    status = wait_for_completion_interruptible_timeout(&pAdapter->tdls_add_station_comp,
           msecs_to_jiffies(WAIT_TIME_TDLS_ADD_STA));

    if (!status)
    {
        VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                "%s: timeout waiting for tdls add station indication",
                __func__);
        return -EPERM;
    }
    if ( eHAL_STATUS_SUCCESS != pAdapter->tdlsAddStaStatus)
    {
        VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                "%s: Add Station is unsucessful", __func__);
        return -EPERM;
    }

    return 0;

error:
    wlan_hdd_tdls_set_link_status(pAdapter, mac, eTDLS_LINK_IDLE);
    return -EPERM;

}
#endif

static int wlan_hdd_change_station(struct wiphy *wiphy,
                                         struct net_device *dev,
                                         u8 *mac,
                                         struct station_parameters *params)
{
    VOS_STATUS status = VOS_STATUS_SUCCESS;
    hdd_adapter_t *pAdapter = WLAN_HDD_GET_PRIV_PTR( dev );
    hdd_context_t *pHddCtx;
    hdd_station_ctx_t *pHddStaCtx;
    v_MACADDR_t STAMacAddress;
#ifdef FEATURE_WLAN_TDLS
    tCsrStaParams StaParams = {0};
    tANI_U8 isBufSta = 0;
#endif
    ENTER();

    if ((NULL == pAdapter))
    {
        VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_FATAL,
                  "invalid adapter ");
        return -EINVAL;
    }

    pHddCtx = WLAN_HDD_GET_CTX(pAdapter);
    pHddStaCtx = WLAN_HDD_GET_STATION_CTX_PTR(pAdapter);

    if ((NULL == pHddCtx) || (NULL == pHddStaCtx))
    {
        VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_FATAL,
                  "invalid HDD state or HDD station context");
        return -EINVAL;
    }

    if (pHddCtx->isLogpInProgress)
    {
        VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                "%s:LOGP in Progress. Ignore!!!", __func__);
        return -EAGAIN;
    }

    vos_mem_copy(STAMacAddress.bytes, mac, sizeof(v_MACADDR_t));

    if ((pAdapter->device_mode == WLAN_HDD_SOFTAP)
      || (pAdapter->device_mode == WLAN_HDD_P2P_GO))
    {
        if (params->sta_flags_set & BIT(NL80211_STA_FLAG_AUTHORIZED))
        {
            status = hdd_softap_change_STA_state( pAdapter, &STAMacAddress,
                                                  WLANTL_STA_AUTHENTICATED);

            if (status != VOS_STATUS_SUCCESS)
            {
                VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO,
                         "%s: Not able to change TL state to AUTHENTICATED", __func__);
                return -EINVAL;
            }
        }
    }
    else if ((pAdapter->device_mode == WLAN_HDD_INFRA_STATION)
          || (pAdapter->device_mode == WLAN_HDD_P2P_CLIENT)) {
#ifdef FEATURE_WLAN_TDLS
        if (params->sta_flags_set & BIT(NL80211_STA_FLAG_TDLS_PEER)) {
            StaParams.capability = params->capability;
            StaParams.uapsd_queues = params->uapsd_queues;
            StaParams.max_sp = params->max_sp;

            if (0 != params->ext_capab_len)
                vos_mem_copy(StaParams.extn_capability, params->ext_capab,
                             sizeof(StaParams.extn_capability));

            if (NULL != params->ht_capa)
            {
                StaParams.htcap_present = 1;
                vos_mem_copy(&StaParams.HTCap, params->ht_capa, sizeof(tSirHTCap));
            }

            StaParams.supported_rates_len = params->supported_rates_len;

            /*                                                                        
                                                                                          
                                                                                    
                                           
             */

            /*                                                                      
             */
            if (StaParams.supported_rates_len > SIR_MAC_MAX_SUPP_RATES)
                StaParams.supported_rates_len = SIR_MAC_MAX_SUPP_RATES;

            if (0 != StaParams.supported_rates_len) {
                int i = 0;
                vos_mem_copy(StaParams.supported_rates, params->supported_rates,
                             StaParams.supported_rates_len);
                VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO,
                           "Supported Rates with Length %d", StaParams.supported_rates_len);
                for (i=0; i < StaParams.supported_rates_len; i++)
                    VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO,
                               "[%d]: %0x", i, StaParams.supported_rates[i]);
            }

            if (NULL != params->vht_capa)
            {
                StaParams.vhtcap_present = 1;
                vos_mem_copy(&StaParams.VHTCap, params->vht_capa, sizeof(tSirVHTCap));
            }

            if (0 != params->ext_capab_len ) {
                /*                           */
                if ((1<<4) & StaParams.extn_capability[3]) {
                    isBufSta = 1;
                }
            }
            status = wlan_hdd_tdls_set_peer_caps( pAdapter, mac, params->uapsd_queues,
                                                  params->max_sp, isBufSta);
            if (VOS_STATUS_SUCCESS != status) {
                VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                          "%s: wlan_hdd_tdls_set_peer_caps failed!", __func__);
                return -EINVAL;
            }
            status = wlan_hdd_tdls_add_station(wiphy, dev, mac, 1, &StaParams);

            if (VOS_STATUS_SUCCESS != status) {
                VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                          "%s: sme_ChangeTdlsPeerSta failed!", __func__);
                return -EINVAL;
            }
        }
#endif
    }
    EXIT();
    return status;
}

/*
                                      
                                                          
 */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,38))
static int wlan_hdd_cfg80211_add_key( struct wiphy *wiphy,
                                      struct net_device *ndev,
                                      u8 key_index, bool pairwise,
                                      const u8 *mac_addr,
                                      struct key_params *params
                                      )
#else
static int wlan_hdd_cfg80211_add_key( struct wiphy *wiphy,
                                      struct net_device *ndev,
                                      u8 key_index, const u8 *mac_addr,
                                      struct key_params *params
                                      )
#endif
{
    hdd_adapter_t *pAdapter = WLAN_HDD_GET_PRIV_PTR( ndev );
    tCsrRoamSetKey  setKey;
    u8 groupmacaddr[WNI_CFG_BSSID_LEN] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    int status;
    v_U32_t roamId= 0xFF;
    v_CONTEXT_t pVosContext = (WLAN_HDD_GET_CTX(pAdapter))->pvosContext;
    hdd_hostapd_state_t *pHostapdState;
    VOS_STATUS vos_status;
    eHalStatus halStatus;
    hdd_context_t *pHddCtx;

    ENTER();

    pHddCtx = WLAN_HDD_GET_CTX(pAdapter);
    status = wlan_hdd_validate_context(pHddCtx);

    if (0 != status)
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                   "%s: HDD context is not valid", __func__);
        return status;
    }

    hddLog(VOS_TRACE_LEVEL_INFO, "%s: device_mode = %d",
            __func__, pAdapter->device_mode);

    if (CSR_MAX_NUM_KEY <= key_index)
    {
        hddLog(VOS_TRACE_LEVEL_ERROR, "%s: Invalid key index %d", __func__,
                key_index);

        return -EINVAL;
    }

    if (CSR_MAX_KEY_LEN < params->key_len)
    {
        hddLog(VOS_TRACE_LEVEL_ERROR, "%s: Invalid key length %d", __func__,
                params->key_len);

        return -EINVAL;
    }

    hddLog(VOS_TRACE_LEVEL_INFO,
           "%s: called with key index = %d & key length %d",
           __func__, key_index, params->key_len);

    /*                                */
    vos_mem_zero(&setKey,sizeof(tCsrRoamSetKey));
    setKey.keyId = key_index;
    setKey.keyLength = params->key_len;
    vos_mem_copy(&setKey.Key[0],params->key, params->key_len);

    switch (params->cipher)
    {
        case WLAN_CIPHER_SUITE_WEP40:
            setKey.encType = eCSR_ENCRYPT_TYPE_WEP40_STATICKEY;
            break;

        case WLAN_CIPHER_SUITE_WEP104:
            setKey.encType = eCSR_ENCRYPT_TYPE_WEP104_STATICKEY;
            break;

        case WLAN_CIPHER_SUITE_TKIP:
            {
                u8 *pKey = &setKey.Key[0];
                setKey.encType = eCSR_ENCRYPT_TYPE_TKIP;

                vos_mem_zero(pKey, CSR_MAX_KEY_LEN);

                /*                                              

                                                        
                                                        
                                                        
                                                         

                */
                /*                                                     

                                                        
                                                        
                                                        
                                                         
                  */
                /*                               */
                vos_mem_copy(pKey, params->key, 16);

                /*                     */
                vos_mem_copy(&pKey[16], &params->key[24], 8);

                /*                */
                vos_mem_copy(&pKey[24], &params->key[16], 8);


                break;
            }

        case WLAN_CIPHER_SUITE_CCMP:
            setKey.encType = eCSR_ENCRYPT_TYPE_AES;
            break;

#ifdef FEATURE_WLAN_WAPI
        case WLAN_CIPHER_SUITE_SMS4:
            {
                vos_mem_zero(&setKey,sizeof(tCsrRoamSetKey));
                wlan_hdd_cfg80211_set_key_wapi(pAdapter, key_index, mac_addr,
                        params->key, params->key_len);
                return 0;
            }
#endif

#ifdef FEATURE_WLAN_CCX
        case WLAN_CIPHER_SUITE_KRK:
            setKey.encType = eCSR_ENCRYPT_TYPE_KRK;
            break;
#endif

#ifdef WLAN_FEATURE_11W
        case WLAN_CIPHER_SUITE_AES_CMAC:
            setKey.encType = eCSR_ENCRYPT_TYPE_AES_CMAC;
            break;
#endif

        default:
            hddLog(VOS_TRACE_LEVEL_ERROR, "%s: unsupported cipher type %lu",
                    __func__, params->cipher);
            return -EOPNOTSUPP;
    }

    hddLog(VOS_TRACE_LEVEL_INFO_MED, "%s: encryption type %d",
            __func__, setKey.encType);

    if (
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,38))
                (!pairwise)
#else
                (!mac_addr || is_broadcast_ether_addr(mac_addr))
#endif
       )
    {
        /*              */
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO,
                "%s- %d: setting Broadcast key",
                __func__, __LINE__);
        setKey.keyDirection = eSIR_RX_ONLY;
        vos_mem_copy(setKey.peerMac,groupmacaddr,WNI_CFG_BSSID_LEN);
    }
    else
    {
        /*                 */
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO,
                "%s- %d: setting pairwise key",
                __func__, __LINE__);
        setKey.keyDirection = eSIR_TX_RX;
        vos_mem_copy(setKey.peerMac, mac_addr,WNI_CFG_BSSID_LEN);
    }
    if ((WLAN_HDD_IBSS == pAdapter->device_mode) && !pairwise)
    {
        setKey.keyDirection = eSIR_TX_RX;
        /*                 */
        status = sme_RoamSetKey( WLAN_HDD_GET_HAL_CTX(pAdapter),
            pAdapter->sessionId, &setKey, &roamId );

        if ( 0 != status )
        {
            hddLog(VOS_TRACE_LEVEL_ERROR,
                    "%s: sme_RoamSetKey failed, returned %d", __func__, status);
            return -EINVAL;
        }
        /*                                                      
                                                   */
        vos_mem_copy(&pAdapter->sessionCtx.station.ibss_enc_key,
                                    &setKey, sizeof(tCsrRoamSetKey));
        return status;
    }
    if ((pAdapter->device_mode == WLAN_HDD_SOFTAP) ||
           (pAdapter->device_mode == WLAN_HDD_P2P_GO))
    {
        pHostapdState = WLAN_HDD_GET_HOSTAP_STATE_PTR(pAdapter);
        if( pHostapdState->bssState == BSS_START )
        {
            status = WLANSAP_SetKeySta( pVosContext, &setKey);

            if ( status != eHAL_STATUS_SUCCESS )
            {
                VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                        "[%4d] WLANSAP_SetKeySta returned ERROR status= %d",
                        __LINE__, status );
            }
        }

        /*                 */
        else if( eCSR_ENCRYPT_TYPE_WEP40_STATICKEY  == setKey.encType ||
                eCSR_ENCRYPT_TYPE_WEP104_STATICKEY  == setKey.encType  )
        {
            //                                                                      
            hdd_ap_ctx_t *pAPCtx = WLAN_HDD_GET_AP_CTX_PTR(pAdapter);
            vos_mem_copy(&pAPCtx->wepKey[key_index], &setKey, sizeof(tCsrRoamSetKey));
        }
        else
        {
            //                                                                  
            hdd_ap_ctx_t *pAPCtx = WLAN_HDD_GET_AP_CTX_PTR(pAdapter);
            vos_mem_copy(&pAPCtx->groupKey, &setKey, sizeof(tCsrRoamSetKey));
        }
    }
    else if ( (pAdapter->device_mode == WLAN_HDD_INFRA_STATION) ||
              (pAdapter->device_mode == WLAN_HDD_P2P_CLIENT) )
    {
        hdd_wext_state_t *pWextState = WLAN_HDD_GET_WEXT_STATE_PTR(pAdapter);
        hdd_station_ctx_t *pHddStaCtx = WLAN_HDD_GET_STATION_CTX_PTR(pAdapter);

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,38))
        if (!pairwise)
#else
        if (!mac_addr || is_broadcast_ether_addr(mac_addr))
#endif
        {
            /*              */
            if (pHddStaCtx->roam_info.deferKeyComplete)
            {
                VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO,
                           "%s- %d: Perform Set key Complete",
                           __func__, __LINE__);
                hdd_PerformRoamSetKeyComplete(pAdapter);
            }
        }

        pWextState->roamProfile.Keys.KeyLength[key_index] = (u8)params->key_len;

        pWextState->roamProfile.Keys.defaultIndex = key_index;


        vos_mem_copy(&pWextState->roamProfile.Keys.KeyMaterial[key_index][0],
                params->key, params->key_len);


        pHddStaCtx->roam_info.roamingState = HDD_ROAM_STATE_SETTING_KEY;

        hddLog(VOS_TRACE_LEVEL_INFO_MED,
                "%s: set key for peerMac %2x:%2x:%2x:%2x:%2x:%2x, direction %d",
                __func__, setKey.peerMac[0], setKey.peerMac[1],
                setKey.peerMac[2], setKey.peerMac[3],
                setKey.peerMac[4], setKey.peerMac[5],
                setKey.keyDirection);

        vos_status = wlan_hdd_check_ula_done(pAdapter);

        if ( vos_status != VOS_STATUS_SUCCESS )
        {
            VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                    "[%4d] wlan_hdd_check_ula_done returned ERROR status= %d",
                    __LINE__, vos_status );

            pHddStaCtx->roam_info.roamingState = HDD_ROAM_STATE_NONE;

            return -EINVAL;

        }

#ifdef WLAN_FEATURE_VOWIFI_11R
        /*                                                                  
                                                                          
                   */
        halStatus = sme_FTUpdateKey( WLAN_HDD_GET_HAL_CTX(pAdapter), &setKey);
        if ( halStatus == eHAL_STATUS_FT_PREAUTH_KEY_SUCCESS )
        {
           hddLog(VOS_TRACE_LEVEL_INFO_MED,
                  "%s: Update PreAuth Key success", __func__);
           return 0;
        }
        else if ( halStatus == eHAL_STATUS_FT_PREAUTH_KEY_FAILED )
        {
           hddLog(VOS_TRACE_LEVEL_ERROR,
                  "%s: Update PreAuth Key failed", __func__);
           return -EINVAL;
        }
#endif /*                         */

        /*                             */
        status = sme_RoamSetKey( WLAN_HDD_GET_HAL_CTX(pAdapter),
                pAdapter->sessionId, &setKey, &roamId );

        if ( 0 != status )
        {
            hddLog(VOS_TRACE_LEVEL_ERROR,
                    "%s: sme_RoamSetKey failed, returned %d", __func__, status);
            pHddStaCtx->roam_info.roamingState = HDD_ROAM_STATE_NONE;
            return -EINVAL;
        }


        /*                                                                            
                                                                                    
                             */
        if ( (eCSR_BSS_TYPE_START_IBSS == pWextState->roamProfile.BSSType) &&
            !(  ( IW_AUTH_KEY_MGMT_802_1X
                    == (pWextState->authKeyMgmt & IW_AUTH_KEY_MGMT_802_1X))
                    && (eCSR_AUTH_TYPE_OPEN_SYSTEM == pHddStaCtx->conn_info.authType)
                 )
                &&
                (  (WLAN_CIPHER_SUITE_WEP40 == params->cipher)
                   || (WLAN_CIPHER_SUITE_WEP104 == params->cipher)
                )
           )
        {
            setKey.keyDirection = eSIR_RX_ONLY;
            vos_mem_copy(setKey.peerMac,groupmacaddr,WNI_CFG_BSSID_LEN);

            hddLog(VOS_TRACE_LEVEL_INFO_MED,
                    "%s: set key peerMac %2x:%2x:%2x:%2x:%2x:%2x, direction %d",
                    __func__, setKey.peerMac[0], setKey.peerMac[1],
                    setKey.peerMac[2], setKey.peerMac[3],
                    setKey.peerMac[4], setKey.peerMac[5],
                    setKey.keyDirection);

            status = sme_RoamSetKey( WLAN_HDD_GET_HAL_CTX(pAdapter),
                    pAdapter->sessionId, &setKey, &roamId );

            if ( 0 != status )
            {
                hddLog(VOS_TRACE_LEVEL_ERROR,
                        "%s: sme_RoamSetKey failed for group key (IBSS), returned %d",
                        __func__, status);
                pHddStaCtx->roam_info.roamingState = HDD_ROAM_STATE_NONE;
                return -EINVAL;
            }
        }
    }

    return 0;
}

/*
                                      
                                                   
 */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,38))
static int wlan_hdd_cfg80211_get_key(
                        struct wiphy *wiphy,
                        struct net_device *ndev,
                        u8 key_index, bool pairwise,
                        const u8 *mac_addr, void *cookie,
                        void (*callback)(void *cookie, struct key_params*)
                        )
#else
static int wlan_hdd_cfg80211_get_key(
                        struct wiphy *wiphy,
                        struct net_device *ndev,
                        u8 key_index, const u8 *mac_addr, void *cookie,
                        void (*callback)(void *cookie, struct key_params*)
                        )
#endif
{
    hdd_adapter_t *pAdapter = WLAN_HDD_GET_PRIV_PTR( ndev );
    hdd_wext_state_t *pWextState= WLAN_HDD_GET_WEXT_STATE_PTR(pAdapter);
    tCsrRoamProfile *pRoamProfile = &(pWextState->roamProfile);
    struct key_params params;

    ENTER();

    hddLog(VOS_TRACE_LEVEL_INFO, "%s: device_mode = %d\n",
                                 __func__,pAdapter->device_mode);

    memset(&params, 0, sizeof(params));

    if (CSR_MAX_NUM_KEY <= key_index)
    {
        return -EINVAL;
    }

    switch(pRoamProfile->EncryptionType.encryptionType[0])
    {
        case eCSR_ENCRYPT_TYPE_NONE:
            params.cipher = IW_AUTH_CIPHER_NONE;
            break;

        case eCSR_ENCRYPT_TYPE_WEP40_STATICKEY:
        case eCSR_ENCRYPT_TYPE_WEP40:
            params.cipher = WLAN_CIPHER_SUITE_WEP40;
            break;

        case eCSR_ENCRYPT_TYPE_WEP104_STATICKEY:
        case eCSR_ENCRYPT_TYPE_WEP104:
            params.cipher = WLAN_CIPHER_SUITE_WEP104;
            break;

        case eCSR_ENCRYPT_TYPE_TKIP:
            params.cipher = WLAN_CIPHER_SUITE_TKIP;
            break;

        case eCSR_ENCRYPT_TYPE_AES:
            params.cipher = WLAN_CIPHER_SUITE_AES_CMAC;
            break;

        default:
            params.cipher = IW_AUTH_CIPHER_NONE;
            break;
    }

    params.key_len = pRoamProfile->Keys.KeyLength[key_index];
    params.seq_len = 0;
    params.seq = NULL;
    params.key = &pRoamProfile->Keys.KeyMaterial[key_index][0];
    callback(cookie, &params);
    return 0;
}

/*
                                      
                                                      
 */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,38))
static int wlan_hdd_cfg80211_del_key( struct wiphy *wiphy,
                                      struct net_device *ndev,
                                      u8 key_index,
                                      bool pairwise,
                                      const u8 *mac_addr
                                    )
#else
static int wlan_hdd_cfg80211_del_key( struct wiphy *wiphy,
                                      struct net_device *ndev,
                                      u8 key_index,
                                      const u8 *mac_addr
                                    )
#endif
{
    int status = 0;

    //                                                                      
    //                                                                  
    //                                             
    //                                                               
    //                                  
#if 0
    hdd_adapter_t *pAdapter = WLAN_HDD_GET_PRIV_PTR( ndev );
    v_CONTEXT_t pVosContext = (WLAN_HDD_GET_CTX(pAdapter))->pvosContext;
    u8 groupmacaddr[WNI_CFG_BSSID_LEN] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    tCsrRoamSetKey  setKey;
    v_U32_t roamId= 0xFF;

    ENTER();

    hddLog(VOS_TRACE_LEVEL_INFO_HIGH, "%s: device_mode = %d\n",
                                     __func__,pAdapter->device_mode);

    if (CSR_MAX_NUM_KEY <= key_index)
    {
        hddLog(VOS_TRACE_LEVEL_ERROR, "%s: Invalid key index %d", __func__,
                key_index);

        return -EINVAL;
    }

    vos_mem_zero(&setKey,sizeof(tCsrRoamSetKey));
    setKey.keyId = key_index;

    if (mac_addr)
        vos_mem_copy(setKey.peerMac, mac_addr,WNI_CFG_BSSID_LEN);
    else
        vos_mem_copy(setKey.peerMac, groupmacaddr, WNI_CFG_BSSID_LEN);

    setKey.encType = eCSR_ENCRYPT_TYPE_NONE;

    if ((pAdapter->device_mode == WLAN_HDD_SOFTAP)
      || (pAdapter->device_mode == WLAN_HDD_P2P_GO)
       )
    {

        hdd_hostapd_state_t *pHostapdState =
                                  WLAN_HDD_GET_HOSTAP_STATE_PTR(pAdapter);
        if( pHostapdState->bssState == BSS_START)
        {
            status = WLANSAP_SetKeySta( pVosContext, &setKey);

            if ( status != eHAL_STATUS_SUCCESS )
            {
                VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                     "[%4d] WLANSAP_SetKeySta returned ERROR status= %d",
                     __LINE__, status );
            }
        }
    }
    else if ( (pAdapter->device_mode == WLAN_HDD_INFRA_STATION)
           || (pAdapter->device_mode == WLAN_HDD_P2P_CLIENT)
            )
    {
        hdd_station_ctx_t *pHddStaCtx = WLAN_HDD_GET_STATION_CTX_PTR(pAdapter);

        pHddStaCtx->roam_info.roamingState = HDD_ROAM_STATE_SETTING_KEY;

        hddLog(VOS_TRACE_LEVEL_INFO_MED,
                "%s: delete key for peerMac %2x:%2x:%2x:%2x:%2x:%2x",
                __func__, setKey.peerMac[0], setKey.peerMac[1],
                setKey.peerMac[2], setKey.peerMac[3],
                setKey.peerMac[4], setKey.peerMac[5]);
        if(pAdapter->sessionCtx.station.conn_info.connState ==
                                       eConnectionState_Associated)
        {
            status = sme_RoamSetKey( WLAN_HDD_GET_HAL_CTX(pAdapter),
                                   pAdapter->sessionId, &setKey, &roamId );

            if ( 0 != status )
            {
                hddLog(VOS_TRACE_LEVEL_ERROR,
                        "%s: sme_RoamSetKey failure, returned %d",
                                                     __func__, status);
                pHddStaCtx->roam_info.roamingState = HDD_ROAM_STATE_NONE;
                return -EINVAL;
            }
        }
    }
#endif
    EXIT();
    return status;
}

/*
                                              
                                                        
 */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,38))
static int wlan_hdd_cfg80211_set_default_key( struct wiphy *wiphy,
                                              struct net_device *ndev,
                                              u8 key_index,
                                              bool unicast, bool multicast)
#else
static int wlan_hdd_cfg80211_set_default_key( struct wiphy *wiphy,
                                              struct net_device *ndev,
                                              u8 key_index)
#endif
{
    hdd_adapter_t *pAdapter = WLAN_HDD_GET_PRIV_PTR( ndev );
    int status;
    hdd_wext_state_t *pWextState;
    hdd_station_ctx_t *pHddStaCtx;
    hdd_context_t *pHddCtx;

    ENTER();

    if ((NULL == pAdapter))
    {
       VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_FATAL,
          "invalid adapter");
       return -EINVAL;
    }

    pWextState = WLAN_HDD_GET_WEXT_STATE_PTR(pAdapter);
    pHddStaCtx = WLAN_HDD_GET_STATION_CTX_PTR(pAdapter);

    if ((NULL == pWextState) || (NULL == pHddStaCtx))
    {
        VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_FATAL,
                  "invalid Wext state or HDD context");
        return -EINVAL;
    }

    hddLog(VOS_TRACE_LEVEL_INFO, "%s: device_mode = %d key_index = %d \n",
                                         __func__,pAdapter->device_mode, key_index);

    if (CSR_MAX_NUM_KEY <= key_index)
    {
        hddLog(VOS_TRACE_LEVEL_ERROR, "%s: Invalid key index %d", __func__,
                key_index);

        return -EINVAL;
    }

    pHddCtx = WLAN_HDD_GET_CTX(pAdapter);
    status = wlan_hdd_validate_context(pHddCtx);

    if (0 != status)
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                   "%s: HDD context is not valid", __func__);
        return status;
    }

    if ((pAdapter->device_mode == WLAN_HDD_INFRA_STATION)
     || (pAdapter->device_mode == WLAN_HDD_P2P_CLIENT)
       )
    {
        if ( (eCSR_ENCRYPT_TYPE_TKIP !=
                pHddStaCtx->conn_info.ucEncryptionType) &&
             (eCSR_ENCRYPT_TYPE_AES !=
                pHddStaCtx->conn_info.ucEncryptionType)
           )
        {
            /*                                                  
                                                 */

            tCsrRoamSetKey  setKey;
            v_U32_t roamId= 0xFF;
            tCsrKeys *Keys = &pWextState->roamProfile.Keys;

            hddLog(VOS_TRACE_LEVEL_INFO_HIGH, "%s: default tx key index %d",
                    __func__, key_index);

            Keys->defaultIndex = (u8)key_index;
            vos_mem_zero(&setKey,sizeof(tCsrRoamSetKey));
            setKey.keyId = key_index;
            setKey.keyLength = Keys->KeyLength[key_index];

            vos_mem_copy(&setKey.Key[0],
                    &Keys->KeyMaterial[key_index][0],
                    Keys->KeyLength[key_index]);

            setKey.keyDirection = eSIR_TX_RX;

            vos_mem_copy(setKey.peerMac,
                    &pHddStaCtx->conn_info.bssId[0],
                    WNI_CFG_BSSID_LEN);

            if (Keys->KeyLength[key_index] == CSR_WEP40_KEY_LEN &&
               pWextState->roamProfile.EncryptionType.encryptionType[0] ==
               eCSR_ENCRYPT_TYPE_WEP104)
            {
                /*                                                                                     
                                                                                                 
                                                                                                    
                                                              */
                pWextState->roamProfile.EncryptionType.encryptionType[0] =
                   eCSR_ENCRYPT_TYPE_WEP40;
                pWextState->roamProfile.mcEncryptionType.encryptionType[0] =
                   eCSR_ENCRYPT_TYPE_WEP40;
            }

            setKey.encType =
                pWextState->roamProfile.EncryptionType.encryptionType[0];

            /*                       */
            status = sme_RoamSetKey( WLAN_HDD_GET_HAL_CTX(pAdapter),
                                   pAdapter->sessionId, &setKey, &roamId );

            if ( 0 != status )
            {
                hddLog(VOS_TRACE_LEVEL_ERROR,
                        "%s: sme_RoamSetKey failed, returned %d", __func__,
                        status);
                return -EINVAL;
            }
        }
    }

    /*                                                       */
    else if ( WLAN_HDD_SOFTAP == pAdapter->device_mode )
    {
        if ( (eCSR_ENCRYPT_TYPE_TKIP !=
                pWextState->roamProfile.EncryptionType.encryptionType[0]) &&
             (eCSR_ENCRYPT_TYPE_AES !=
                pWextState->roamProfile.EncryptionType.encryptionType[0])
           )
        {
            /*                                                           */
            hdd_ap_ctx_t *pAPCtx = WLAN_HDD_GET_AP_CTX_PTR(pAdapter);
            pAPCtx->wepKey[key_index].keyDirection = eSIR_TX_DEFAULT;
        }
    }

    return status;
}

/*
                                         
                                                                        
 */
static struct cfg80211_bss* wlan_hdd_cfg80211_inform_bss(
                    hdd_adapter_t *pAdapter, tCsrRoamConnectedProfile *roamProfile)
{
    struct net_device *dev = pAdapter->dev;
    struct wireless_dev *wdev = dev->ieee80211_ptr;
    struct wiphy *wiphy = wdev->wiphy;
    tSirBssDescription *pBssDesc = roamProfile->pBssDesc;
    int chan_no;
    int ie_length;
    const char *ie;
    unsigned int freq;
    struct ieee80211_channel *chan;
    int rssi = 0;
    struct cfg80211_bss *bss = NULL;

    ENTER();

    if( NULL == pBssDesc )
    {
        hddLog(VOS_TRACE_LEVEL_FATAL, "%s: pBssDesc is NULL\n", __func__);
        return bss;
    }

    chan_no = pBssDesc->channelId;
    ie_length = GET_IE_LEN_IN_BSS_DESC( pBssDesc->length );
    ie =  ((ie_length != 0) ? (const char *)&pBssDesc->ieFields: NULL);

    if( NULL == ie )
    {
       hddLog(VOS_TRACE_LEVEL_FATAL, "%s: IE of BSS descriptor is NULL\n", __func__);
       return bss;
    }

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,38))
    if (chan_no <= ARRAY_SIZE(hdd_channels_2_4_GHZ))
    {
        freq = ieee80211_channel_to_frequency(chan_no, IEEE80211_BAND_2GHZ);
    }
    else
    {
        freq = ieee80211_channel_to_frequency(chan_no, IEEE80211_BAND_5GHZ);
    }
#else
    freq = ieee80211_channel_to_frequency(chan_no);
#endif

    chan = __ieee80211_get_channel(wiphy, freq);

    bss = cfg80211_get_bss(wiphy, chan, pBssDesc->bssId,
                           &roamProfile->SSID.ssId[0], roamProfile->SSID.length,
                           WLAN_CAPABILITY_ESS, WLAN_CAPABILITY_ESS);
    if (bss == NULL)
    {
        rssi = (VOS_MIN ((pBssDesc->rssi + pBssDesc->sinr), 0))*100;

        return (cfg80211_inform_bss(wiphy, chan, pBssDesc->bssId,
                le64_to_cpu(*(__le64 *)pBssDesc->timeStamp),
                pBssDesc->capabilityInfo,
                pBssDesc->beaconInterval, ie, ie_length,
                rssi, GFP_KERNEL ));
}
    else
    {
        return bss;
    }
}



/*
                                               
                                                                        
 */
struct cfg80211_bss*
wlan_hdd_cfg80211_inform_bss_frame( hdd_adapter_t *pAdapter,
                                    tSirBssDescription *bss_desc
                                    )
{
    /*
                                                                           
                                                                             
                                                                        
                                                                            
                                                                                
                                                                          
                                
     */
    struct net_device *dev = pAdapter->dev;
    struct wireless_dev *wdev = dev->ieee80211_ptr;
    struct wiphy *wiphy = wdev->wiphy;
    int chan_no = bss_desc->channelId;
#ifdef WLAN_ENABLE_AGEIE_ON_SCAN_RESULTS
    qcom_ie_age *qie_age = NULL;
    int ie_length = GET_IE_LEN_IN_BSS_DESC( bss_desc->length ) + sizeof(qcom_ie_age);
#else
    int ie_length = GET_IE_LEN_IN_BSS_DESC( bss_desc->length );
#endif
    const char *ie =
        ((ie_length != 0) ? (const char *)&bss_desc->ieFields: NULL);
    unsigned int freq;
    struct ieee80211_channel *chan;
    struct ieee80211_mgmt *mgmt = NULL;
    struct cfg80211_bss *bss_status = NULL;
    size_t frame_len = sizeof (struct ieee80211_mgmt) + ie_length;
    int rssi = 0;
    hdd_context_t *pHddCtx;
    int status;
#ifdef WLAN_OPEN_SOURCE
    struct timespec ts;
#endif

    ENTER();

    pHddCtx = WLAN_HDD_GET_CTX(pAdapter);
    status = wlan_hdd_validate_context(pHddCtx);

    /*                                                                 */
    if (WLAN_HDD_IS_LOAD_UNLOAD_IN_PROGRESS(pHddCtx))
    {
         VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                    "%s:Loading_unloading in Progress. Ignore!!!",__func__);
         return NULL;
    }


    if (0 != status)
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                   "%s: HDD context is not valid", __func__);
        return NULL;
    }

    mgmt = kzalloc((sizeof (struct ieee80211_mgmt) + ie_length), GFP_KERNEL);
    if (!mgmt)
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                   "%s: memory allocation failed ", __func__);
        return NULL;
    }

    memcpy(mgmt->bssid, bss_desc->bssId, ETH_ALEN);

#ifdef WLAN_OPEN_SOURCE
    /*                                                    
                                                     */
    get_monotonic_boottime(&ts);
    mgmt->u.probe_resp.timestamp =
         ((u64)ts.tv_sec * 1000000) + (ts.tv_nsec / 1000);
#else
    /*                                                 */
    memcpy(&mgmt->u.probe_resp.timestamp, bss_desc->timeStamp,
            sizeof (bss_desc->timeStamp));

#endif

    mgmt->u.probe_resp.beacon_int = bss_desc->beaconInterval;
    mgmt->u.probe_resp.capab_info = bss_desc->capabilityInfo;

#ifdef WLAN_ENABLE_AGEIE_ON_SCAN_RESULTS
    /*                                                                */
    /*                                               */
    ie_length           -=sizeof(qcom_ie_age);
    qie_age =  (qcom_ie_age *)(mgmt->u.probe_resp.variable + ie_length);
    qie_age->element_id = QCOM_VENDOR_IE_ID;
    qie_age->len        = QCOM_VENDOR_IE_AGE_LEN;
    qie_age->oui_1      = QCOM_OUI1;
    qie_age->oui_2      = QCOM_OUI2;
    qie_age->oui_3      = QCOM_OUI3;
    qie_age->type       = QCOM_VENDOR_IE_AGE_TYPE;
    qie_age->age        = vos_timer_get_system_ticks() - bss_desc->nReceivedTime;
#endif

    memcpy(mgmt->u.probe_resp.variable, ie, ie_length);
    if (bss_desc->fProbeRsp)
    {
         mgmt->frame_control |=
                   (u16)(IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_PROBE_RESP);
    }
    else
    {
         mgmt->frame_control |=
                   (u16)(IEEE80211_FTYPE_MGMT | IEEE80211_STYPE_BEACON);
    }

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,38))
    if (chan_no <= ARRAY_SIZE(hdd_channels_2_4_GHZ) &&
        (wiphy->bands[IEEE80211_BAND_2GHZ] != NULL))
    {
        freq = ieee80211_channel_to_frequency(chan_no, IEEE80211_BAND_2GHZ);
    }
    else if ((chan_no > ARRAY_SIZE(hdd_channels_2_4_GHZ)) &&
        (wiphy->bands[IEEE80211_BAND_5GHZ] != NULL))

    {
        freq = ieee80211_channel_to_frequency(chan_no, IEEE80211_BAND_5GHZ);
    }
    else
    {
        kfree(mgmt);
        return NULL;
    }
#else
    freq = ieee80211_channel_to_frequency(chan_no);
#endif
    chan = __ieee80211_get_channel(wiphy, freq);
    /*                                                                        
                                                                                                                                                 
                                                                                                                 
                                                                                                                                 
                                                                                                                                             
                                                                                                    
                                                                                                                                               
                                                                                                                                         
                                                                                                                                                     
                                                                                                                                           
                                                
     */
    if(chan == NULL)
    {
       hddLog(VOS_TRACE_LEVEL_INFO, "%s chan pointer is NULL", __func__);
       kfree(mgmt);
       return NULL;
    }
    /*                                                            
                                                       
      */
    if (( eConnectionState_Associated ==
             pAdapter->sessionCtx.station.conn_info.connState ) &&
             ( VOS_TRUE == vos_mem_compare(bss_desc->bssId,
                             pAdapter->sessionCtx.station.conn_info.bssId,
                             WNI_CFG_BSSID_LEN)))
    {
       /*                                                               */
       rssi = (pAdapter->rssi * 100);
    }
    else
    {
       rssi = (VOS_MIN ((bss_desc->rssi + bss_desc->sinr), 0))*100;
    }

    bss_status = cfg80211_inform_bss_frame(wiphy, chan, mgmt,
            frame_len, rssi, GFP_KERNEL);
    kfree(mgmt);
    return bss_status;
}

/*
                                            
                                                               
 */
struct cfg80211_bss* wlan_hdd_cfg80211_update_bss_db( hdd_adapter_t *pAdapter,
                                      tCsrRoamInfo *pRoamInfo
                                      )
{
    tCsrRoamConnectedProfile roamProfile;
    tHalHandle hHal = WLAN_HDD_GET_HAL_CTX(pAdapter);
    struct cfg80211_bss *bss = NULL;

    ENTER();

    memset(&roamProfile, 0, sizeof(tCsrRoamConnectedProfile));
    sme_RoamGetConnectProfile(hHal, pAdapter->sessionId, &roamProfile);

    if (NULL != roamProfile.pBssDesc)
    {
        bss = wlan_hdd_cfg80211_inform_bss(pAdapter,
                &roamProfile);

        if (NULL == bss)
        {
            hddLog(VOS_TRACE_LEVEL_INFO, "%s: cfg80211_inform_bss return NULL",
                    __func__);
        }

        sme_RoamFreeConnectProfile(hHal, &roamProfile);
    }
    else
    {
        hddLog(VOS_TRACE_LEVEL_ERROR, "%s:  roamProfile.pBssDesc is NULL",
                __func__);
    }
    return bss;
}

/*
                                         
 */
static int wlan_hdd_cfg80211_update_bss( struct wiphy *wiphy,
                                         hdd_adapter_t *pAdapter
                                        )
{
    tHalHandle hHal = WLAN_HDD_GET_HAL_CTX(pAdapter);
    tCsrScanResultInfo *pScanResult;
    eHalStatus status = 0;
    tScanResultHandle pResult;
    struct cfg80211_bss *bss_status = NULL;
    hdd_context_t *pHddCtx;

    ENTER();

    pHddCtx = WLAN_HDD_GET_CTX(pAdapter);

    if (pHddCtx->isLogpInProgress)
    {
        VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_FATAL,
                   "%s:LOGP in Progress. Ignore!!!",__func__);
        return -EAGAIN;
    }


    /*                                                                 */
    if (WLAN_HDD_IS_LOAD_UNLOAD_IN_PROGRESS(pHddCtx))
    {
        VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                   "%s:Loading_unloading in Progress. Ignore!!!",__func__);
        return VOS_STATUS_E_PERM;
    }


    /*
                                                                    
     */
    status = sme_ScanGetResult(hHal, pAdapter->sessionId, NULL, &pResult);

    /*                 */
    if (NULL == pResult)
    {
        hddLog(VOS_TRACE_LEVEL_INFO, "%s: No scan result\n", __func__);
        return status;
    }

    pScanResult = sme_ScanResultGetFirst(hHal, pResult);

    while (pScanResult)
    {
        /*
                                                                          
                                                                     
                                                                            
                                                                          
                                                                         
                                                                         
                                                          
                                    
           */

        bss_status = wlan_hdd_cfg80211_inform_bss_frame(pAdapter,
                &pScanResult->BssDescriptor);


        if (NULL == bss_status)
        {
            hddLog(VOS_TRACE_LEVEL_INFO,
                    "%s: NULL returned by cfg80211_inform_bss\n", __func__);
        }
        else
        {
            cfg80211_put_bss(
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,9,0))
                             wiphy,
#endif
                             bss_status);
        }

        pScanResult = sme_ScanResultGetNext(hHal, pResult);
    }

    sme_ScanResultPurge(hHal, pResult);

    return 0;
}

void
hddPrintMacAddr(tCsrBssid macAddr, tANI_U8 logLevel)
{
    VOS_TRACE(VOS_MODULE_ID_HDD, logLevel,
              MAC_ADDRESS_STR, MAC_ADDR_ARRAY(macAddr));
} /*                                 */

void
hddPrintPmkId(tANI_U8 *pmkId, tANI_U8 logLevel)
{
    VOS_TRACE(VOS_MODULE_ID_HDD, logLevel,
              "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\n",
              pmkId[0], pmkId[1], pmkId[2], pmkId[3], pmkId[4],
              pmkId[5], pmkId[6], pmkId[7], pmkId[8], pmkId[9], pmkId[10],
              pmkId[11], pmkId[12], pmkId[13], pmkId[14], pmkId[15]);
} /*                               */

//                                                     
//                                                

//                                                                                                 
//                                                                      

#define dump_bssid(bssid) \
    { \
        hddLog(VOS_TRACE_LEVEL_INFO, "BSSID (MAC) address:\t"); \
        hddPrintMacAddr(bssid, VOS_TRACE_LEVEL_INFO);\
        hddLog(VOS_TRACE_LEVEL_INFO, "\n"); \
    }

#define dump_pmkid(pMac, pmkid) \
    { \
        hddLog(VOS_TRACE_LEVEL_INFO, "PMKSA-ID:\t"); \
        hddPrintPmkId(pmkid, VOS_TRACE_LEVEL_INFO);\
        hddLog(VOS_TRACE_LEVEL_INFO, "\n"); \
    }

#if defined(FEATURE_WLAN_LFR) && (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0))
/*
                                                     
                                                                           
 */
int wlan_hdd_cfg80211_pmksa_candidate_notify(
                    hdd_adapter_t *pAdapter, tCsrRoamInfo *pRoamInfo,
                    int index, bool preauth )
{
#ifdef FEATURE_WLAN_OKC
    struct net_device *dev = pAdapter->dev;
    hdd_context_t *pHddCtx = (hdd_context_t*)pAdapter->pHddCtx;

    ENTER();
    hddLog(VOS_TRACE_LEVEL_INFO, "%s is going to notify supplicant of:", __func__);

    if( NULL == pRoamInfo )
    {
        hddLog(VOS_TRACE_LEVEL_FATAL, "%s: pRoamInfo is NULL\n", __func__);
        return -EINVAL;
    }

    if (eANI_BOOLEAN_TRUE == hdd_is_okc_mode_enabled(pHddCtx))
    {
        dump_bssid(pRoamInfo->bssid);
        cfg80211_pmksa_candidate_notify(dev, index,
                                    pRoamInfo->bssid, preauth, GFP_KERNEL);
    }
#endif  /*                  */
    return 0;
}
#endif //                

/*
                                            
                                                          
  
 */
static eHalStatus hdd_cfg80211_scan_done_callback(tHalHandle halHandle,
        void *pContext, tANI_U32 scanId, eCsrScanStatus status)
{
    struct net_device *dev = (struct net_device *) pContext;
    //                                               
    hdd_adapter_t *pAdapter = WLAN_HDD_GET_PRIV_PTR( dev );
    hdd_context_t *pHddCtx = WLAN_HDD_GET_CTX(pAdapter);
    hdd_scaninfo_t *pScanInfo = &pHddCtx->scan_info;
    struct cfg80211_scan_request *req = NULL;
    int ret = 0;

    ENTER();

    hddLog(VOS_TRACE_LEVEL_INFO,
            "%s called with halHandle = %p, pContext = %p,"
            "scanID = %d, returned status = %d\n",
            __func__, halHandle, pContext, (int) scanId, (int) status);

    pScanInfo->mScanPendingCounter = 0;

    //                                                                 
    ret = wait_for_completion_interruptible_timeout(
                         &pScanInfo->scan_req_completion_event,
                         msecs_to_jiffies(WLAN_WAIT_TIME_SCAN_REQ));
    if (!ret)
    {
       VOS_ASSERT(pScanInfo->mScanPending);
       goto allow_suspend;
    }

    if(pScanInfo->mScanPending != VOS_TRUE)
    {
        VOS_ASSERT(pScanInfo->mScanPending);
        goto allow_suspend;
    }

    /*                  */
    if (pScanInfo->scanId != scanId)
    {
        hddLog(VOS_TRACE_LEVEL_INFO,
                "%s called with mismatched scanId pScanInfo->scanId = %d "
                "scanId = %d \n", __func__, (int) pScanInfo->scanId,
                (int) scanId);
    }

    ret = wlan_hdd_cfg80211_update_bss((WLAN_HDD_GET_CTX(pAdapter))->wiphy,
                                        pAdapter);

    if (0 > ret)
        hddLog(VOS_TRACE_LEVEL_INFO, "%s: NO SCAN result", __func__);


    /*                                            
                                      */
    if (pHddCtx->scan_info.waitScanResult)
    {
        /*                                                       
                                                      */
        if(WEXT_SCAN_PENDING_DELAY == pHddCtx->scan_info.scan_pending_option)
        {
            vos_event_set(&pHddCtx->scan_info.scan_finished_event);
        }
        /*                            */
        else if(WEXT_SCAN_PENDING_PIGGYBACK == pHddCtx->scan_info.scan_pending_option)
        {
            struct net_device *dev = pAdapter->dev;
            union iwreq_data wrqu;
            int we_event;
            char *msg;

            memset(&wrqu, '\0', sizeof(wrqu));
            we_event = SIOCGIWSCAN;
            msg = NULL;
            wireless_send_event(dev, we_event, &wrqu, msg);
        }
    }
    pHddCtx->scan_info.waitScanResult = FALSE;

    /*                  */
    req = pAdapter->request;

    if (!req)
    {
        hddLog(VOS_TRACE_LEVEL_ERROR, "request is became NULL\n");
        pScanInfo->mScanPending = VOS_FALSE;
        goto allow_suspend;
    }

    /*
                                  
     */
    req->n_ssids = 0;
    req->n_channels = 0;
    req->ie = 0;

    pAdapter->request = NULL;
    /*                           */
    pScanInfo->mScanPending = VOS_FALSE;

    /*
                                                            
                  
     */
    cfg80211_scan_done(req, false);
    complete(&pScanInfo->abortscan_event_var);

allow_suspend:
    /*                                             */
    hdd_allow_suspend();

    /*                                                                 
                                                                       
                                                                       
                                            */
    hdd_allow_suspend_timeout(1000);

#ifdef FEATURE_WLAN_TDLS
    wlan_hdd_tdls_scan_done_callback(pAdapter);
#endif

    EXIT();
    return 0;
}

/*
                              
                                                    
  
 */
v_BOOL_t hdd_isScanAllowed( hdd_context_t *pHddCtx )
{
    hdd_adapter_list_node_t *pAdapterNode = NULL, *pNext = NULL;
    hdd_station_ctx_t *pHddStaCtx = NULL;
    hdd_adapter_t *pAdapter = NULL;
    VOS_STATUS status = 0;
    v_U8_t staId = 0;
    v_U8_t *staMac = NULL;

    if (TRUE == pHddCtx->btCoexModeSet)
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO,
           FL("BTCoex Mode operation in progress, Do not allow scan"));
        return VOS_FALSE;
    }

    status = hdd_get_front_adapter ( pHddCtx, &pAdapterNode );

    while ( NULL != pAdapterNode && VOS_STATUS_SUCCESS == status )
    {
        pAdapter = pAdapterNode->pAdapter;

        if( pAdapter )
        {
            hddLog(VOS_TRACE_LEVEL_INFO,
                    "%s: Adapter with device mode %d exists",
                    __func__, pAdapter->device_mode);
            if ((WLAN_HDD_INFRA_STATION == pAdapter->device_mode) ||
                    (WLAN_HDD_P2P_CLIENT == pAdapter->device_mode) ||
                    (WLAN_HDD_P2P_DEVICE == pAdapter->device_mode))
            {
                pHddStaCtx = WLAN_HDD_GET_STATION_CTX_PTR(pAdapter);
                if ((eConnectionState_Associated == pHddStaCtx->conn_info.connState) &&
                        (VOS_FALSE == pHddStaCtx->conn_info.uIsAuthenticated))
                {
                    staMac = (v_U8_t *) &(pAdapter->macAddressCurrent.bytes[0]);
                    hddLog(VOS_TRACE_LEVEL_ERROR,
                           "%s: client " MAC_ADDRESS_STR
                           " is in the middle of WPS/EAPOL exchange.", __func__,
                            MAC_ADDR_ARRAY(staMac));
                    return VOS_FALSE;
                }
            }
            else if ((WLAN_HDD_SOFTAP == pAdapter->device_mode) ||
                    (WLAN_HDD_P2P_GO == pAdapter->device_mode))
            {
                for (staId = 0; staId < WLAN_MAX_STA_COUNT; staId++)
                {
                    if ((pAdapter->aStaInfo[staId].isUsed) &&
                            (WLANTL_STA_CONNECTED == pAdapter->aStaInfo[staId].tlSTAState))
                    {
                        staMac = (v_U8_t *) &(pAdapter->aStaInfo[staId].macAddrSTA.bytes[0]);

                        hddLog(VOS_TRACE_LEVEL_ERROR,
                               "%s: client " MAC_ADDRESS_STR " of SoftAP/P2P-GO is in the "
                               "middle of WPS/EAPOL exchange.", __func__,
                                MAC_ADDR_ARRAY(staMac));
                        return VOS_FALSE;
                    }
                }
            }
        }
        status = hdd_get_next_adapter ( pHddCtx, pAdapterNode, &pNext );
        pAdapterNode = pNext;
    }
    hddLog(VOS_TRACE_LEVEL_INFO,
            "%s: Scan allowed", __func__);
    return VOS_TRUE;
}

/*
                                   
                                                                      
                                                               
 */
int wlan_hdd_cfg80211_scan( struct wiphy *wiphy,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,6,0))
                            struct net_device *dev,
#endif
                            struct cfg80211_scan_request *request)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))
   struct net_device *dev = request->wdev->netdev;
#endif
    hdd_adapter_t *pAdapter = WLAN_HDD_GET_PRIV_PTR( dev );
    hdd_context_t *pHddCtx = WLAN_HDD_GET_CTX( pAdapter );
    hdd_wext_state_t *pwextBuf = WLAN_HDD_GET_WEXT_STATE_PTR(pAdapter);
    hdd_config_t *cfg_param = NULL;
    tCsrScanRequest scanRequest;
    tANI_U8 *channelList = NULL, i;
    v_U32_t scanId = 0;
    int status;
    hdd_scaninfo_t *pScanInfo = NULL;
    v_U8_t* pP2pIe = NULL;

    ENTER();

    hddLog(VOS_TRACE_LEVEL_INFO, "%s: device_mode = %d\n",
                                   __func__,pAdapter->device_mode);

    status = wlan_hdd_validate_context(pHddCtx);

    if (0 != status)
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                   "%s: HDD context is not valid", __func__);
        return status;
    }

    cfg_param = pHddCtx->cfg_ini;
    pScanInfo = &pHddCtx->scan_info;

    if ((WLAN_HDD_INFRA_STATION == pAdapter->device_mode) &&
        (eConnectionState_Connecting ==
           (WLAN_HDD_GET_STATION_CTX_PTR(pAdapter))->conn_info.connState))
    {
        hddLog(VOS_TRACE_LEVEL_ERROR,
                "%s: %p(%d) Connection in progress: Scan request denied (EBUSY)", __func__, \
                WLAN_HDD_GET_STATION_CTX_PTR(pAdapter), pAdapter->sessionId);
        return -EBUSY;
    }

#ifdef WLAN_BTAMP_FEATURE
    //                                          
    if (VOS_TRUE == WLANBAP_AmpSessionOn())
    {
        hddLog(VOS_TRACE_LEVEL_ERROR,
                "%s: No scanning when AMP is on", __func__);
        return -EOPNOTSUPP;
    }
#endif
    //                                             
    if (pAdapter->device_mode == WLAN_HDD_SOFTAP)
    {
        hddLog(VOS_TRACE_LEVEL_ERROR,
                "%s: Not scanning on device_mode = %d",
                                    __func__, pAdapter->device_mode);
        return -EOPNOTSUPP;
    }

    if (TRUE == pScanInfo->mScanPending)
    {
        if ( MAX_PENDING_LOG > pScanInfo->mScanPendingCounter++ )
        {
            hddLog(VOS_TRACE_LEVEL_ERROR, "%s: mScanPending is TRUE", __func__);
        }
        return -EBUSY;
    }

    //                                             
    //                                   
    //                                                 
    //                          
    if (0 != wlan_hdd_check_remain_on_channel(pAdapter))
    {
        hddLog(VOS_TRACE_LEVEL_ERROR, "%s: Remain On Channel Pending", __func__);
        return -EBUSY;
    }
#ifdef FEATURE_WLAN_TDLS
    /*                                                     
                                                                         
                                                                          
    */
    status = wlan_hdd_tdls_scan_callback (pAdapter,
                                          wiphy,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,6,0))
                                          dev,
#endif
                                          request);
    if(status <= 0)
    {
        hddLog(VOS_TRACE_LEVEL_INFO, "%s: TDLS Pending %d", __func__, status);
        return status;
    }
#endif

    if (mutex_lock_interruptible(&pHddCtx->tmInfo.tmOperationLock))
    {
        VOS_TRACE(VOS_MODULE_ID_HDD,VOS_TRACE_LEVEL_ERROR,
                  "%s: Acquire lock fail", __func__);
        return -EAGAIN;
    }
    if (TRUE == pHddCtx->tmInfo.tmAction.enterImps)
    {
        hddLog(VOS_TRACE_LEVEL_WARN,
               "%s: MAX TM Level Scan not allowed", __func__);
        mutex_unlock(&pHddCtx->tmInfo.tmOperationLock);
        return -EBUSY;
    }
    mutex_unlock(&pHddCtx->tmInfo.tmOperationLock);

    /*                                                
     */
    if (!hdd_isScanAllowed(pHddCtx))
    {
        hddLog(VOS_TRACE_LEVEL_ERROR, "%s: Scan not allowed", __func__);
        return -EBUSY;
    }

    vos_mem_zero( &scanRequest, sizeof(scanRequest));

    if (NULL != request)
    {
        hddLog(VOS_TRACE_LEVEL_INFO, "scan request for ssid = %d",
               (int)request->n_ssids);

        /*                                                                       
                                                                                    
                                             
         */
        if (request->ssids && '\0' == request->ssids->ssid[0])
        {
            request->n_ssids = 0;
        }

        if ((request->ssids) && (0 < request->n_ssids))
        {
            tCsrSSIDInfo *SsidInfo;
            int j;
            scanRequest.SSIDs.numOfSSIDs = request->n_ssids;
            /*                                          */
            SsidInfo = scanRequest.SSIDs.SSIDList =
                      ( tCsrSSIDInfo *)vos_mem_malloc(
                              request->n_ssids*sizeof(tCsrSSIDInfo));

            if(NULL == scanRequest.SSIDs.SSIDList)
            {
                hddLog(VOS_TRACE_LEVEL_ERROR,
                               "memory alloc failed SSIDInfo buffer");
                return -ENOMEM;
            }

            /*                                      */
            for(j = 0; j < request->n_ssids; j++, SsidInfo++)
            {
                /*                     */
                SsidInfo->SSID.length = request->ssids[j].ssid_len;
                vos_mem_copy(SsidInfo->SSID.ssId, &request->ssids[j].ssid[0],
                             SsidInfo->SSID.length);
                SsidInfo->SSID.ssId[SsidInfo->SSID.length] = '\0';
                hddLog(VOS_TRACE_LEVEL_INFO_HIGH, "SSID number %d:  %s",
                                                   j, SsidInfo->SSID.ssId);
            }
            /*                             */
            scanRequest.scanType = eSIR_ACTIVE_SCAN;
        }
        else if(WLAN_HDD_P2P_GO == pAdapter->device_mode)
        {
            /*                             */
            scanRequest.scanType = eSIR_ACTIVE_SCAN;
        }
        else
        {
            /*                                                            */
            scanRequest.scanType = pScanInfo->scan_mode;
        }
        scanRequest.minChnTime = cfg_param->nActiveMinChnTime;
        scanRequest.maxChnTime = cfg_param->nActiveMaxChnTime;
    }
    else
    {
        /*                             */
        scanRequest.scanType = eSIR_ACTIVE_SCAN;
        vos_mem_set( scanRequest.bssid, sizeof( tCsrBssid ), 0xff );

        /*                                      */
        scanRequest.minChnTime = 0;
        scanRequest.maxChnTime = 0;
    }

    /*                             */
    scanRequest.BSSType = eCSR_BSS_TYPE_ANY;

    /*                                      */

    /*                                    */
    if( request )
    {
        if( request->n_channels )
        {
            channelList = vos_mem_malloc( request->n_channels );
            if( NULL == channelList )
            {
                status = -ENOMEM;
                goto free_mem;
            }

            for( i = 0 ; i < request->n_channels ; i++ )
                channelList[i] = request->channels[i]->hw_value;
        }

        scanRequest.ChannelInfo.numOfChannels = request->n_channels;
        scanRequest.ChannelInfo.ChannelList = channelList;

        /*                              */
        scanRequest.requestType = eCSR_SCAN_REQUEST_FULL_SCAN;

        /*                                                              
                                                                             
                                                                            
         */

        /*                                                          
                                                                 
                                                                    
                                                                  
                                     
         */

        if( request->n_channels != WLAN_HDD_P2P_SINGLE_CHANNEL_SCAN )
        {
            sme_ScanFlushP2PResult( WLAN_HDD_GET_HAL_CTX(pAdapter),
                                                pAdapter->sessionId );
        }

        if( request->ie_len )
        {
            /*                                                       */
            memset( &pScanInfo->scanAddIE, 0, sizeof(pScanInfo->scanAddIE) );
            memcpy( pScanInfo->scanAddIE.addIEdata, request->ie, request->ie_len);
            pScanInfo->scanAddIE.length = request->ie_len;

            if((WLAN_HDD_INFRA_STATION == pAdapter->device_mode) ||
                (WLAN_HDD_P2P_CLIENT == pAdapter->device_mode) ||
                (WLAN_HDD_P2P_DEVICE == pAdapter->device_mode)
              )
            {
               pwextBuf->roamProfile.pAddIEScan = pScanInfo->scanAddIE.addIEdata;
               pwextBuf->roamProfile.nAddIEScanLength = pScanInfo->scanAddIE.length;
            }

            scanRequest.uIEFieldLen = pScanInfo->scanAddIE.length;
            scanRequest.pIEField = pScanInfo->scanAddIE.addIEdata;

            pP2pIe = wlan_hdd_get_p2p_ie_ptr((v_U8_t*)request->ie,
                                                       request->ie_len);
            if (pP2pIe != NULL)
            {
#ifdef WLAN_FEATURE_P2P_DEBUG
                if (((globalP2PConnectionStatus == P2P_GO_NEG_COMPLETED) ||
                    (globalP2PConnectionStatus == P2P_GO_NEG_PROCESS)) &&
                    (WLAN_HDD_P2P_CLIENT == pAdapter->device_mode))
                {
                    globalP2PConnectionStatus = P2P_CLIENT_CONNECTING_STATE_1;
                    hddLog(VOS_TRACE_LEVEL_ERROR,"[P2P State] Changing state from "
                                    "Go nego completed to Connection is started");
                    hddLog(VOS_TRACE_LEVEL_ERROR,"[P2P]P2P Scanning is started "
                                   "for 8way Handshake");
                }
                else if((globalP2PConnectionStatus == P2P_CLIENT_DISCONNECTED_STATE) &&
                        (WLAN_HDD_P2P_CLIENT == pAdapter->device_mode))
                {
                    globalP2PConnectionStatus = P2P_CLIENT_CONNECTING_STATE_2;
                    hddLog(VOS_TRACE_LEVEL_ERROR,"[P2P State] Changing state from "
                                    "Disconnected state to Connection is started");
                    hddLog(VOS_TRACE_LEVEL_ERROR,"[P2P]P2P Scanning is started "
                                                        "for 4way Handshake");
                }
#endif

                /*                                                         */
                if(TRUE == request->no_cck)
                {
                    hddLog(VOS_TRACE_LEVEL_INFO,
                           "%s: This is a P2P Search", __func__);
                    scanRequest.p2pSearch = 1;

                    if( request->n_channels == WLAN_HDD_P2P_SOCIAL_CHANNELS )
                    {
                         /*                                  */
                         scanRequest.requestType = eCSR_SCAN_P2P_DISCOVERY;
                    }

                    /*
                                                             
                                               
                    */
                    if(cfg_param->skipDfsChnlInP2pSearch)
                    {
                       scanRequest.skipDfsChnlInP2pSearch = 1;
                    }
                    else
                    {
                       scanRequest.skipDfsChnlInP2pSearch = 0;
                    }

                }
            }
        }
    }

    INIT_COMPLETION(pScanInfo->scan_req_completion_event);

    /*                                                                   
                                    
                                                                                  
                                                                                      
                                                                              
                                                                                
                                                    
     */
    hdd_prevent_suspend();

#ifdef CUSTOMER_LGE_DEBUG_LOG
    if (scanRequest.requestType != eCSR_SCAN_P2P_DISCOVERY) {
        hddLog(VOS_TRACE_LEVEL_ERROR, "%s: sme_ScanRequest, channel num = %d, request type = %d", __func__, scanRequest.ChannelInfo.numOfChannels, scanRequest.requestType);
    }
#endif
    status = sme_ScanRequest( WLAN_HDD_GET_HAL_CTX(pAdapter),
                              pAdapter->sessionId, &scanRequest, &scanId,
                              &hdd_cfg80211_scan_done_callback, dev );

    if (eHAL_STATUS_SUCCESS != status)
    {
        hddLog(VOS_TRACE_LEVEL_ERROR,
                "%s: sme_ScanRequest returned error %d", __func__, status);
        complete(&pScanInfo->scan_req_completion_event);
        if(eHAL_STATUS_RESOURCES == status)
        {
                hddLog(VOS_TRACE_LEVEL_INFO, "%s: HO is in progress.So defer the scan by informing busy",__func__);
                status = -EBUSY;
        } else {
                status = -EIO;
        }
        hdd_allow_suspend();
        goto free_mem;
    }

    pScanInfo->mScanPending = TRUE;
    pAdapter->request = request;
    pScanInfo->scanId = scanId;

    complete(&pScanInfo->scan_req_completion_event);

free_mem:
    if( scanRequest.SSIDs.SSIDList )
    {
        vos_mem_free(scanRequest.SSIDs.SSIDList);
    }

    if( channelList )
      vos_mem_free( channelList );

    EXIT();

    return status;
}


void hdd_select_cbmode( hdd_adapter_t *pAdapter,v_U8_t operationChannel)
{
    v_U8_t iniDot11Mode =
               (WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->dot11Mode;
    eHddDot11Mode   hddDot11Mode = iniDot11Mode;

    switch ( iniDot11Mode )
    {
       case eHDD_DOT11_MODE_AUTO:
       case eHDD_DOT11_MODE_11ac:
       case eHDD_DOT11_MODE_11ac_ONLY:
#ifdef WLAN_FEATURE_11AC
          hddDot11Mode = eHDD_DOT11_MODE_11ac;
#else
          hddDot11Mode = eHDD_DOT11_MODE_11n;
#endif
          break;
       case eHDD_DOT11_MODE_11n:
       case eHDD_DOT11_MODE_11n_ONLY:
          hddDot11Mode = eHDD_DOT11_MODE_11n;
          break;
       default:
          hddDot11Mode = iniDot11Mode;
          break;
    }
     /*                                                 */
    sme_SelectCBMode((WLAN_HDD_GET_CTX(pAdapter)->hHal),
                     hdd_cfg_xlate_to_csr_phy_mode(hddDot11Mode),
                     operationChannel);
}

/*
                                            
                                                         
 */
int wlan_hdd_cfg80211_connect_start( hdd_adapter_t  *pAdapter,
        const u8 *ssid, size_t ssid_len, const u8 *bssid, u8 operatingChannel)
{
    int status = 0;
    hdd_wext_state_t *pWextState;
    hdd_context_t *pHddCtx;
    v_U32_t roamId;
    tCsrRoamProfile *pRoamProfile;
    eMib_dot11DesiredBssType connectedBssType;
    eCsrAuthType RSNAuthType;

    ENTER();

    pWextState = WLAN_HDD_GET_WEXT_STATE_PTR(pAdapter);
    pHddCtx = WLAN_HDD_GET_CTX(pAdapter);

    status = wlan_hdd_validate_context(pHddCtx);
    if (status)
    {
        VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                  "%s: HDD context is not valid!", __func__);
        return status;
    }

    if (SIR_MAC_MAX_SSID_LENGTH < ssid_len)
    {
        hddLog(VOS_TRACE_LEVEL_ERROR, "%s: wrong SSID len", __func__);
        return -EINVAL;
    }

    pRoamProfile = &pWextState->roamProfile;

    if (pRoamProfile)
    {
        int ret = 0;
        hdd_station_ctx_t *pHddStaCtx;
        pHddStaCtx = WLAN_HDD_GET_STATION_CTX_PTR(pAdapter);
        hdd_connGetConnectedBssType(pHddStaCtx,&connectedBssType );

        if((eMib_dot11DesiredBssType_independent == connectedBssType) ||
           (eConnectionState_Associated == pHddStaCtx->conn_info.connState) ||
           (eConnectionState_IbssConnected == pHddStaCtx->conn_info.connState))
        {
            /*                         */
            INIT_COMPLETION(pAdapter->disconnect_comp_var);
            if( eHAL_STATUS_SUCCESS ==
                  sme_RoamDisconnect( WLAN_HDD_GET_HAL_CTX(pAdapter),
                            pAdapter->sessionId,
                            eCSR_DISCONNECT_REASON_UNSPECIFIED ) )
            {
                ret = wait_for_completion_interruptible_timeout(
                             &pAdapter->disconnect_comp_var,
                             msecs_to_jiffies(WLAN_WAIT_TIME_DISCONNECT));
                if (0 == ret)
                {
                    VOS_ASSERT(0);
                }
            }
        }
        else if(eConnectionState_Disconnecting == pHddStaCtx->conn_info.connState)
        {
            ret = wait_for_completion_interruptible_timeout(
                         &pAdapter->disconnect_comp_var,
                         msecs_to_jiffies(WLAN_WAIT_TIME_DISCONNECT));
            if (0 == ret)
            {
                VOS_ASSERT(0);
            }
        }

        if (HDD_WMM_USER_MODE_NO_QOS ==
                        (WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->WmmMode)
        {
            /*                           */
            pRoamProfile->uapsd_mask = 0;
        }
        else
        {
            /*                                            */
            pRoamProfile->uapsd_mask =
                     (WLAN_HDD_GET_CTX(pAdapter))->cfg_ini->UapsdMask;
        }

        pRoamProfile->SSIDs.numOfSSIDs = 1;
        pRoamProfile->SSIDs.SSIDList->SSID.length = ssid_len;
        vos_mem_zero(pRoamProfile->SSIDs.SSIDList->SSID.ssId,
                sizeof(pRoamProfile->SSIDs.SSIDList->SSID.ssId));
        vos_mem_copy((void *)(pRoamProfile->SSIDs.SSIDList->SSID.ssId),
                ssid, ssid_len);

        if (bssid)
        {
            pRoamProfile->BSSIDs.numOfBSSIDs = 1;
            vos_mem_copy((void *)(pRoamProfile->BSSIDs.bssid), bssid,
                    WNI_CFG_BSSID_LEN);
            /*                                                        
                                                                             
                                                                            
             */
            vos_mem_copy((void *)(pWextState->req_bssId), bssid,
                    WNI_CFG_BSSID_LEN);
        }
        else
        {
            vos_mem_zero((void *)(pRoamProfile->BSSIDs.bssid),WNI_CFG_BSSID_LEN);
        }

        if ((IW_AUTH_WPA_VERSION_WPA == pWextState->wpaVersion) ||
                (IW_AUTH_WPA_VERSION_WPA2 == pWextState->wpaVersion))
        {
            /*          */
            hdd_SetGENIEToCsr(pAdapter, &RSNAuthType);
            /*        */
            hdd_set_csr_auth_type(pAdapter, RSNAuthType);
        }
#ifdef FEATURE_WLAN_WAPI
        if (pAdapter->wapi_info.nWapiMode)
        {
            hddLog(LOG1, "%s: Setting WAPI AUTH Type and Encryption Mode values", __func__);
            switch (pAdapter->wapi_info.wapiAuthMode)
            {
                case WAPI_AUTH_MODE_PSK:
                {
                    hddLog(LOG1, "%s: WAPI AUTH TYPE: PSK: %d", __func__,
                                                   pAdapter->wapi_info.wapiAuthMode);
                    pRoamProfile->AuthType.authType[0] = eCSR_AUTH_TYPE_WAPI_WAI_PSK;
                    break;
                }
                case WAPI_AUTH_MODE_CERT:
                {
                    hddLog(LOG1, "%s: WAPI AUTH TYPE: CERT: %d", __func__,
                                                    pAdapter->wapi_info.wapiAuthMode);
                    pRoamProfile->AuthType.authType[0] = eCSR_AUTH_TYPE_WAPI_WAI_CERTIFICATE;
                    break;
                }
            } //              
            if ( pAdapter->wapi_info.wapiAuthMode == WAPI_AUTH_MODE_PSK ||
                pAdapter->wapi_info.wapiAuthMode == WAPI_AUTH_MODE_CERT)
            {
                hddLog(LOG1, "%s: WAPI PAIRWISE/GROUP ENCRYPTION: WPI", __func__);
                pRoamProfile->AuthType.numEntries = 1;
                pRoamProfile->EncryptionType.numEntries = 1;
                pRoamProfile->EncryptionType.encryptionType[0] = eCSR_ENCRYPT_TYPE_WPI;
                pRoamProfile->mcEncryptionType.numEntries = 1;
                pRoamProfile->mcEncryptionType.encryptionType[0] = eCSR_ENCRYPT_TYPE_WPI;
            }
        }
#endif /*                   */
#ifdef WLAN_FEATURE_GTK_OFFLOAD
        /*                                  */
        if ((WLAN_HDD_INFRA_STATION == pAdapter->device_mode) ||
            (WLAN_HDD_P2P_CLIENT == pAdapter->device_mode))
        {
            memset(&pHddStaCtx->gtkOffloadReqParams, 0,
                  sizeof (tSirGtkOffloadParams));
            pHddStaCtx->gtkOffloadReqParams.ulFlags = GTK_OFFLOAD_DISABLE;
        }
#endif
        pRoamProfile->csrPersona = pAdapter->device_mode;

        if( operatingChannel )
        {
           pRoamProfile->ChannelInfo.ChannelList = &operatingChannel;
           pRoamProfile->ChannelInfo.numOfChannels = 1;
        }
        else
        {
            pRoamProfile->ChannelInfo.ChannelList = NULL;
            pRoamProfile->ChannelInfo.numOfChannels = 0;
        }
        if ( (WLAN_HDD_IBSS == pAdapter->device_mode) && operatingChannel)
        {
            hdd_select_cbmode(pAdapter,operatingChannel);
        }
        /*                                                                                    
                                                                                            
                                                                                               
                                                                                                        
                                                                                                 
         */
        if (WLAN_HDD_INFRA_STATION == pAdapter->device_mode ||
            WLAN_HDD_P2P_CLIENT == pAdapter->device_mode)
            hdd_connSetConnectionState(WLAN_HDD_GET_STATION_CTX_PTR(pAdapter),
                                                 eConnectionState_Connecting);

        status = sme_RoamConnect( WLAN_HDD_GET_HAL_CTX(pAdapter),
                            pAdapter->sessionId, pRoamProfile, &roamId);

        if ((eHAL_STATUS_SUCCESS != status) &&
            (WLAN_HDD_INFRA_STATION == pAdapter->device_mode ||
             WLAN_HDD_P2P_CLIENT == pAdapter->device_mode))

        {
            hddLog(VOS_TRACE_LEVEL_ERROR, "%s: sme_RoamConnect (session %d) failed with "
                                      "status %d. -> NotConnected", __func__, pAdapter->sessionId, status);
            /*                              */
            hdd_connSetConnectionState(WLAN_HDD_GET_STATION_CTX_PTR(pAdapter),
                                             eConnectionState_NotConnected);
        }

        pRoamProfile->ChannelInfo.ChannelList = NULL;
        pRoamProfile->ChannelInfo.numOfChannels = 0;

    }
    else
    {
        hddLog(VOS_TRACE_LEVEL_ERROR, "%s: No valid Roam profile", __func__);
        return -EINVAL;
    }
    EXIT();
    return status;
}

/*
                                            
                                                                      
  
 */
static int wlan_hdd_cfg80211_set_auth_type(hdd_adapter_t *pAdapter,
        enum nl80211_auth_type auth_type)
{
    hdd_wext_state_t *pWextState = WLAN_HDD_GET_WEXT_STATE_PTR(pAdapter);
    hdd_station_ctx_t *pHddStaCtx = WLAN_HDD_GET_STATION_CTX_PTR(pAdapter);

    ENTER();

    /*                       */
    switch (auth_type)
    {
        case NL80211_AUTHTYPE_AUTOMATIC:
            hddLog(VOS_TRACE_LEVEL_INFO,
                    "%s: set authentication type to AUTOSWITCH", __func__);
            pHddStaCtx->conn_info.authType = eCSR_AUTH_TYPE_AUTOSWITCH;
            break;

        case NL80211_AUTHTYPE_OPEN_SYSTEM:
#ifdef WLAN_FEATURE_VOWIFI_11R
        case NL80211_AUTHTYPE_FT:
#endif /*                         */
            hddLog(VOS_TRACE_LEVEL_INFO,
                    "%s: set authentication type to OPEN", __func__);
            pHddStaCtx->conn_info.authType = eCSR_AUTH_TYPE_OPEN_SYSTEM;
            break;

        case NL80211_AUTHTYPE_SHARED_KEY:
            hddLog(VOS_TRACE_LEVEL_INFO,
                    "%s: set authentication type to SHARED", __func__);
            pHddStaCtx->conn_info.authType = eCSR_AUTH_TYPE_SHARED_KEY;
            break;
#ifdef FEATURE_WLAN_CCX
        case NL80211_AUTHTYPE_NETWORK_EAP:
            hddLog(VOS_TRACE_LEVEL_INFO,
                            "%s: set authentication type to CCKM WPA", __func__);
            pHddStaCtx->conn_info.authType = eCSR_AUTH_TYPE_CCKM_WPA;//                                                                
            break;
#endif


        default:
            hddLog(VOS_TRACE_LEVEL_ERROR,
                    "%s: Unsupported authentication type %d", __func__,
                    auth_type);
            pHddStaCtx->conn_info.authType = eCSR_AUTH_TYPE_UNKNOWN;
            return -EINVAL;
    }

    pWextState->roamProfile.AuthType.authType[0] =
                                        pHddStaCtx->conn_info.authType;
    return 0;
}

/*
                                   
                                                             
  
 */
static int wlan_hdd_set_akm_suite( hdd_adapter_t *pAdapter,
                                   u32 key_mgmt
                                   )
{
    hdd_wext_state_t *pWextState = WLAN_HDD_GET_WEXT_STATE_PTR(pAdapter);
    ENTER();

    /*                 */
    switch(key_mgmt)
    {
        case WLAN_AKM_SUITE_PSK:
#ifdef WLAN_FEATURE_VOWIFI_11R
        case WLAN_AKM_SUITE_FT_PSK:
#endif
            hddLog(VOS_TRACE_LEVEL_INFO, "%s: setting key mgmt type to PSK",
                    __func__);
            pWextState->authKeyMgmt |= IW_AUTH_KEY_MGMT_PSK;
            break;

        case WLAN_AKM_SUITE_8021X:
#ifdef WLAN_FEATURE_VOWIFI_11R
        case WLAN_AKM_SUITE_FT_8021X:
#endif
            hddLog(VOS_TRACE_LEVEL_INFO, "%s: setting key mgmt type to 8021x",
                    __func__);
            pWextState->authKeyMgmt |= IW_AUTH_KEY_MGMT_802_1X;
            break;
#ifdef FEATURE_WLAN_CCX
#define WLAN_AKM_SUITE_CCKM         0x00409600 /*                                */
#define IW_AUTH_KEY_MGMT_CCKM       8  /*                               */
        case WLAN_AKM_SUITE_CCKM:
            hddLog(VOS_TRACE_LEVEL_INFO, "%s: setting key mgmt type to CCKM",
                            __func__);
            pWextState->authKeyMgmt |= IW_AUTH_KEY_MGMT_CCKM;
            break;
#endif

        default:
            hddLog(VOS_TRACE_LEVEL_ERROR, "%s: Unsupported key mgmt type %d",
                    __func__, key_mgmt);
            return -EINVAL;

    }
    return 0;
}

/*
                                         
                                                   
                                 
 */
static int wlan_hdd_cfg80211_set_cipher( hdd_adapter_t *pAdapter,
                                u32 cipher,
                                bool ucast
                                )
{
    eCsrEncryptionType encryptionType = eCSR_ENCRYPT_TYPE_NONE;
    hdd_wext_state_t *pWextState = WLAN_HDD_GET_WEXT_STATE_PTR(pAdapter);
    hdd_station_ctx_t *pHddStaCtx = WLAN_HDD_GET_STATION_CTX_PTR(pAdapter);

    ENTER();

    if (!cipher)
    {
        hddLog(VOS_TRACE_LEVEL_ERROR, "%s: received cipher %d - considering none",
                __func__, cipher);
        encryptionType = eCSR_ENCRYPT_TYPE_NONE;
    }
    else
    {

        /*                     */
        switch (cipher)
        {
            case IW_AUTH_CIPHER_NONE:
                encryptionType = eCSR_ENCRYPT_TYPE_NONE;
                break;

            case WLAN_CIPHER_SUITE_WEP40:
                encryptionType = eCSR_ENCRYPT_TYPE_WEP40;
                break;

            case WLAN_CIPHER_SUITE_WEP104:
                encryptionType = eCSR_ENCRYPT_TYPE_WEP104;
                break;

            case WLAN_CIPHER_SUITE_TKIP:
                encryptionType = eCSR_ENCRYPT_TYPE_TKIP;
                break;

            case WLAN_CIPHER_SUITE_CCMP:
                encryptionType = eCSR_ENCRYPT_TYPE_AES;
                break;
#ifdef FEATURE_WLAN_WAPI
        case WLAN_CIPHER_SUITE_SMS4:
            encryptionType = eCSR_ENCRYPT_TYPE_WPI;
            break;
#endif

#ifdef FEATURE_WLAN_CCX
        case WLAN_CIPHER_SUITE_KRK:
            encryptionType = eCSR_ENCRYPT_TYPE_KRK;
            break;
#endif
            default:
                hddLog(VOS_TRACE_LEVEL_ERROR, "%s: Unsupported cipher type %d",
                        __func__, cipher);
                return -EOPNOTSUPP;
        }
    }

    if (ucast)
    {
        hddLog(VOS_TRACE_LEVEL_INFO, "%s: setting unicast cipher type to %d",
                __func__, encryptionType);
        pHddStaCtx->conn_info.ucEncryptionType            = encryptionType;
        pWextState->roamProfile.EncryptionType.numEntries = 1;
        pWextState->roamProfile.EncryptionType.encryptionType[0] =
                                          encryptionType;
    }
    else
    {
        hddLog(VOS_TRACE_LEVEL_INFO, "%s: setting mcast cipher type to %d",
                __func__, encryptionType);
        pHddStaCtx->conn_info.mcEncryptionType                       = encryptionType;
        pWextState->roamProfile.mcEncryptionType.numEntries        = 1;
        pWextState->roamProfile.mcEncryptionType.encryptionType[0] = encryptionType;
    }

    return 0;
}


/*
                                     
                                               
 */
int wlan_hdd_cfg80211_set_ie( hdd_adapter_t *pAdapter,
                              u8 *ie,
                              size_t ie_len
                              )
{
    hdd_wext_state_t *pWextState = WLAN_HDD_GET_WEXT_STATE_PTR(pAdapter);
    u8 *genie = ie;
    v_U16_t remLen = ie_len;
#ifdef FEATURE_WLAN_WAPI
    v_U32_t akmsuite[MAX_NUM_AKM_SUITES];
    u16 *tmp;
    v_U16_t akmsuiteCount;
    int *akmlist;
#endif
    ENTER();

    /*                           */
    pWextState->assocAddIE.length = 0;
    pWextState->roamProfile.bWPSAssociation = VOS_FALSE;

    while (remLen >= 2)
    {
        v_U16_t eLen = 0;
        v_U8_t elementId;
        elementId = *genie++;
        eLen  = *genie++;
        remLen -= 2;

        hddLog(VOS_TRACE_LEVEL_INFO, "%s: IE[0x%X], LEN[%d]\n",
            __func__, elementId, eLen);

        switch ( elementId )
        {
            case DOT11F_EID_WPA:
                if (4 > eLen) /*                                                                       */
                {
                    hddLog(VOS_TRACE_LEVEL_ERROR,
                              "%s: Invalid WPA IE", __func__);
                    return -EINVAL;
                }
                else if (0 == memcmp(&genie[0], "\x00\x50\xf2\x04", 4))
                {
                    v_U16_t curAddIELen = pWextState->assocAddIE.length;
                    hddLog (VOS_TRACE_LEVEL_INFO, "%s Set WPS IE(len %d)",
                            __func__, eLen + 2);

                    if( SIR_MAC_MAX_IE_LENGTH < (pWextState->assocAddIE.length + eLen) )
                    {
                       hddLog(VOS_TRACE_LEVEL_FATAL, "Cannot accommodate assocAddIE. "
                                                      "Need bigger buffer space");
                       VOS_ASSERT(0);
                       return -ENOMEM;
                    }
                    //                                                                                      
                    memcpy( pWextState->assocAddIE.addIEdata + curAddIELen, genie - 2, eLen + 2);
                    pWextState->assocAddIE.length += eLen + 2;

                    pWextState->roamProfile.bWPSAssociation = VOS_TRUE;
                    pWextState->roamProfile.pAddIEAssoc = pWextState->assocAddIE.addIEdata;
                    pWextState->roamProfile.nAddIEAssocLength = pWextState->assocAddIE.length;
                }
                else if (0 == memcmp(&genie[0], "\x00\x50\xf2", 3))
                {
                    hddLog (VOS_TRACE_LEVEL_INFO, "%s Set WPA IE (len %d)",__func__, eLen + 2);
                    memset( pWextState->WPARSNIE, 0, MAX_WPA_RSN_IE_LEN );
                    memcpy( pWextState->WPARSNIE, genie - 2, (eLen + 2) /*      */);
                    pWextState->roamProfile.pWPAReqIE = pWextState->WPARSNIE;
                    pWextState->roamProfile.nWPAReqIELength = eLen + 2;//       
                }
                else if ( (0 == memcmp(&genie[0], P2P_OUI_TYPE,
                                                         P2P_OUI_TYPE_SIZE)))
                {
                    v_U16_t curAddIELen = pWextState->assocAddIE.length;
                    hddLog (VOS_TRACE_LEVEL_INFO, "%s Set P2P IE(len %d)",
                            __func__, eLen + 2);

                    if( SIR_MAC_MAX_IE_LENGTH < (pWextState->assocAddIE.length + eLen) )
                    {
                       hddLog(VOS_TRACE_LEVEL_FATAL, "Cannot accommodate assocAddIE "
                                                      "Need bigger buffer space");
                       VOS_ASSERT(0);
                       return -ENOMEM;
                    }
                    //                                                                                      
                    memcpy( pWextState->assocAddIE.addIEdata + curAddIELen, genie - 2, eLen + 2);
                    pWextState->assocAddIE.length += eLen + 2;

                    pWextState->roamProfile.pAddIEAssoc = pWextState->assocAddIE.addIEdata;
                    pWextState->roamProfile.nAddIEAssocLength = pWextState->assocAddIE.length;
                }
#ifdef WLAN_FEATURE_WFD
                else if ( (0 == memcmp(&genie[0], WFD_OUI_TYPE,
                                                         WFD_OUI_TYPE_SIZE))
                        /*                                     */
                         && (WLAN_HDD_P2P_CLIENT == pAdapter->device_mode) )
                {
                    v_U16_t curAddIELen = pWextState->assocAddIE.length;
                    hddLog (VOS_TRACE_LEVEL_INFO, "%s Set WFD IE(len %d)",
                            __func__, eLen + 2);

                    if( SIR_MAC_MAX_IE_LENGTH < (pWextState->assocAddIE.length + eLen) )
                    {
                       hddLog(VOS_TRACE_LEVEL_FATAL, "Cannot accommodate assocAddIE "
                                                      "Need bigger buffer space");
                       VOS_ASSERT(0);
                       return -ENOMEM;
                    }
                    //                                                                      
                    //                         
                    memcpy( pWextState->assocAddIE.addIEdata + curAddIELen, genie - 2, eLen + 2);
                    pWextState->assocAddIE.length += eLen + 2;

                    pWextState->roamProfile.pAddIEAssoc = pWextState->assocAddIE.addIEdata;
                    pWextState->roamProfile.nAddIEAssocLength = pWextState->assocAddIE.length;
                }
#endif
                /*                                                            */
                else if ( (0 == memcmp(&genie[0], HS20_OUI_TYPE,
                                       HS20_OUI_TYPE_SIZE)) )
                {
                    v_U16_t curAddIELen = pWextState->assocAddIE.length;
                    hddLog (VOS_TRACE_LEVEL_INFO, "%s Set HS20 IE(len %d)",
                            __func__, eLen + 2);

                    if( SIR_MAC_MAX_IE_LENGTH < (pWextState->assocAddIE.length + eLen) )
                    {
                        hddLog(VOS_TRACE_LEVEL_FATAL, "Cannot accommodate assocAddIE "
                               "Need bigger buffer space");
                        VOS_ASSERT(0);
                        return -ENOMEM;
                    }
                    memcpy( pWextState->assocAddIE.addIEdata + curAddIELen, genie - 2, eLen + 2);
                    pWextState->assocAddIE.length += eLen + 2;

                    pWextState->roamProfile.pAddIEAssoc = pWextState->assocAddIE.addIEdata;
                    pWextState->roamProfile.nAddIEAssocLength = pWextState->assocAddIE.length;
                }

                break;
            case DOT11F_EID_RSN:
                hddLog (VOS_TRACE_LEVEL_INFO, "%s Set RSN IE(len %d)",__func__, eLen + 2);
                memset( pWextState->WPARSNIE, 0, MAX_WPA_RSN_IE_LEN );
                memcpy( pWextState->WPARSNIE, genie - 2, (eLen + 2)/*      */);
                pWextState->roamProfile.pRSNReqIE = pWextState->WPARSNIE;
                pWextState->roamProfile.nRSNReqIELength = eLen + 2; //       
                break;
                /*                                                                        */
            case DOT11F_EID_EXTCAP:
                {
                    v_U16_t curAddIELen = pWextState->assocAddIE.length;
                    hddLog (VOS_TRACE_LEVEL_INFO, "%s Set Extended CAPS IE(len %d)",
                            __func__, eLen + 2);

                    if( SIR_MAC_MAX_IE_LENGTH < (pWextState->assocAddIE.length + eLen) )
                    {
                       hddLog(VOS_TRACE_LEVEL_FATAL, "Cannot accommodate assocAddIE "
                                                      "Need bigger buffer space");
                       VOS_ASSERT(0);
                       return -ENOMEM;
                    }
                    memcpy( pWextState->assocAddIE.addIEdata + curAddIELen, genie - 2, eLen + 2);
                    pWextState->assocAddIE.length += eLen + 2;

                    pWextState->roamProfile.pAddIEAssoc = pWextState->assocAddIE.addIEdata;
                    pWextState->roamProfile.nAddIEAssocLength = pWextState->assocAddIE.length;
                    break;
                }
#ifdef FEATURE_WLAN_WAPI
            case WLAN_EID_WAPI:
                pAdapter->wapi_info.nWapiMode = 1;   //                         
                hddLog(VOS_TRACE_LEVEL_INFO,"WAPI MODE IS  %lu \n",
                                          pAdapter->wapi_info.nWapiMode);
                tmp = (u16 *)ie;
                tmp = tmp + 2; //                                 
                akmsuiteCount = WPA_GET_LE16(tmp);
                tmp = tmp + 1;
                akmlist = (int *)(tmp);
                if(akmsuiteCount <= MAX_NUM_AKM_SUITES)
                {
                    memcpy(akmsuite, akmlist, (4*akmsuiteCount));
                }
                else
                {
                    hddLog(VOS_TRACE_LEVEL_FATAL, "Invalid akmSuite count\n");
                    VOS_ASSERT(0);
                    return -EINVAL;
                }

                if (WAPI_PSK_AKM_SUITE == akmsuite[0])
                {
                    hddLog(VOS_TRACE_LEVEL_INFO, "%s: WAPI AUTH MODE SET TO PSK",
                                                            __func__);
                    pAdapter->wapi_info.wapiAuthMode = WAPI_AUTH_MODE_PSK;
                }
                if (WAPI_CERT_AKM_SUITE == akmsuite[0])
                {
                    hddLog(VOS_TRACE_LEVEL_INFO, "%s: WAPI AUTH MODE SET TO CERTIFICATE",
                                                             __func__);
                    pAdapter->wapi_info.wapiAuthMode = WAPI_AUTH_MODE_CERT;
                }
                break;
#endif
            default:
                hddLog (VOS_TRACE_LEVEL_ERROR,
                        "%s Set UNKNOWN IE %X", __func__, elementId);
                /*                                                         
                                                                         
                                             */
                break;
        }
        genie += eLen;
        remLen -= eLen;
    }
    EXIT();
    return 0;
}

/*
                               
                                           
  
 */
static bool hdd_isWPAIEPresent(u8 *ie, u8 ie_len)
{
    v_U8_t eLen = 0;
    v_U16_t remLen = ie_len;
    v_U8_t elementId = 0;

    while (remLen >= 2)
    {
        elementId = *ie++;
        eLen  = *ie++;
        remLen -= 2;
        if (eLen > remLen)
        {
            hddLog(VOS_TRACE_LEVEL_ERROR,
                "%s: IE length is wrong %d", __func__, eLen);
            return FALSE;
        }
        if ((elementId == DOT11F_EID_WPA) && (remLen > 5))
        {
          /*                     
                                           
                               */
            if (0 == memcmp(&ie[0], "\x00\x50\xf2\x01\x01", 5))
               return TRUE;
        }
        ie += eLen;
        remLen -= eLen;
    }
    return FALSE;
}

/*
                                          
                                                   
                                       
 */
int wlan_hdd_cfg80211_set_privacy( hdd_adapter_t *pAdapter,
                                   struct cfg80211_connect_params *req
                                   )
{
    int status = 0;
    hdd_wext_state_t *pWextState = WLAN_HDD_GET_WEXT_STATE_PTR(pAdapter);
    ENTER();

    /*               */
    pWextState->wpaVersion = IW_AUTH_WPA_VERSION_DISABLED;

    if (req->crypto.wpa_versions)
    {
        if (NL80211_WPA_VERSION_1 == req->crypto.wpa_versions)
        {
            pWextState->wpaVersion = IW_AUTH_WPA_VERSION_WPA;
        }
        else if (NL80211_WPA_VERSION_2 == req->crypto.wpa_versions)
        {
            pWextState->wpaVersion = IW_AUTH_WPA_VERSION_WPA2;
        }
    }

    hddLog(VOS_TRACE_LEVEL_INFO, "%s: set wpa version to %d", __func__,
            pWextState->wpaVersion);

    /*                       */
    status = wlan_hdd_cfg80211_set_auth_type(pAdapter, req->auth_type);

    if (0 > status)
    {
        hddLog(VOS_TRACE_LEVEL_ERROR,
                "%s: failed to set authentication type ", __func__);
        return status;
    }

    /*                 */
    if (req->crypto.n_akm_suites)
    {
        status = wlan_hdd_set_akm_suite(pAdapter, req->crypto.akm_suites[0]);
        if (0 > status)
        {
            hddLog(VOS_TRACE_LEVEL_ERROR, "%s: failed to set akm suite",
                    __func__);
            return status;
        }
    }

    /*                        */
    if (req->crypto.n_ciphers_pairwise)
    {
        status = wlan_hdd_cfg80211_set_cipher(pAdapter,
                                      req->crypto.ciphers_pairwise[0], true);
        if (0 > status)
        {
            hddLog(VOS_TRACE_LEVEL_ERROR,
                    "%s: failed to set unicast cipher type", __func__);
            return status;
        }
    }
    else
    {
        /*                                   */
        status = wlan_hdd_cfg80211_set_cipher(pAdapter, 0, true);
        if (0 > status)
        {
            hddLog(VOS_TRACE_LEVEL_ERROR,
                    "%s: failed to set unicast cipher type", __func__);
            return status;
        }
    }

    /*                     */
    status = wlan_hdd_cfg80211_set_cipher(pAdapter, req->crypto.cipher_group,
                                                                       false);

    if (0 > status)
    {
        hddLog(VOS_TRACE_LEVEL_ERROR, "%s: failed to set mcast cipher type",
                __func__);
        return status;
    }

#ifdef WLAN_FEATURE_11W
    pWextState->roamProfile.MFPEnabled = (req->mfp == NL80211_MFP_REQUIRED);
#endif

    /*                                                                */
    if (req->ie_len)
    {
        status = wlan_hdd_cfg80211_set_ie(pAdapter, req->ie, req->ie_len);
        if ( 0 > status)
        {
            hddLog(VOS_TRACE_LEVEL_ERROR, "%s: failed to parse the WPA/RSN IE",
                    __func__);
            return status;
        }
    }

    /*                                         */
    if (req->key && req->key_len)
    {
        if ( (WLAN_CIPHER_SUITE_WEP40 == req->crypto.ciphers_pairwise[0])
                || (WLAN_CIPHER_SUITE_WEP104 == req->crypto.ciphers_pairwise[0])
          )
        {
            if ( IW_AUTH_KEY_MGMT_802_1X
                    == (pWextState->authKeyMgmt & IW_AUTH_KEY_MGMT_802_1X  ))
            {
                hddLog(VOS_TRACE_LEVEL_ERROR, "%s: Dynamic WEP not supported",
                        __func__);
                return -EOPNOTSUPP;
            }
            else
            {
                u8 key_len = req->key_len;
                u8 key_idx = req->key_idx;

                if ((eCSR_SECURITY_WEP_KEYSIZE_MAX_BYTES >= key_len)
                        && (CSR_MAX_NUM_KEY > key_idx)
                  )
                {
                    hddLog(VOS_TRACE_LEVEL_INFO,
                     "%s: setting default wep key, key_idx = %hu key_len %hu",
                            __func__, key_idx, key_len);
                    vos_mem_copy(
                       &pWextState->roamProfile.Keys.KeyMaterial[key_idx][0],
                                  req->key, key_len);
                    pWextState->roamProfile.Keys.KeyLength[key_idx] =
                                                               (u8)key_len;
                    pWextState->roamProfile.Keys.defaultIndex = (u8)key_idx;
                }
            }
        }
    }

    return status;
}

/*
                                          
                                                   
                                       
 */
static int wlan_hdd_cfg80211_connect( struct wiphy *wiphy,
                                      struct net_device *ndev,
                                      struct cfg80211_connect_params *req
                                      )
{
    int status;
    hdd_adapter_t *pAdapter = WLAN_HDD_GET_PRIV_PTR( ndev );
    VOS_STATUS exitbmpsStatus = VOS_STATUS_E_INVAL;
    hdd_context_t *pHddCtx = NULL;

    ENTER();

    hddLog(VOS_TRACE_LEVEL_INFO,
             "%s: device_mode = %d\n",__func__,pAdapter->device_mode);

    pHddCtx = WLAN_HDD_GET_CTX(pAdapter);
    status = wlan_hdd_validate_context(pHddCtx);

    if (0 != status)
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                   "%s: HDD context is not valid", __func__);
        return status;
    }

#ifdef WLAN_BTAMP_FEATURE
    //                                                   
    if( VOS_TRUE == WLANBAP_AmpSessionOn() )
    {
        hddLog(VOS_TRACE_LEVEL_ERROR,
                "%s: No connection when AMP is on", __func__);
        return -ECONNREFUSED;
    }
#endif
    /*                              */
    status = wlan_hdd_cfg80211_set_privacy(pAdapter, req);

    if ( 0 > status)
    {
        hddLog(VOS_TRACE_LEVEL_ERROR, "%s: failed to set security params",
                __func__);
        return status;
    }

    //                                                       
    //                                                 
    if((WLAN_HDD_INFRA_STATION == pAdapter->device_mode) &&
        (vos_concurrent_sessions_running()))
    {
        exitbmpsStatus = hdd_disable_bmps_imps(pHddCtx, WLAN_HDD_INFRA_STATION);
    }

    if ( req->channel )
    {
        status = wlan_hdd_cfg80211_connect_start(pAdapter, req->ssid,
                                                  req->ssid_len, req->bssid,
                                                  req->channel->hw_value);
    }
    else
    {
        status = wlan_hdd_cfg80211_connect_start(pAdapter, req->ssid,
                                                  req->ssid_len, req->bssid, 0);
    }

    if (0 > status)
    {
        //                         
        if((VOS_STATUS_SUCCESS == exitbmpsStatus) &&
            (NULL != pHddCtx))
        {
            if (pHddCtx->hdd_wlan_suspended)
            {
                hdd_set_pwrparams(pHddCtx);
            }
           //                           
           hdd_enable_bmps_imps(pHddCtx);
        }

        hddLog(VOS_TRACE_LEVEL_ERROR, "%s: connect failed", __func__);
        return status;
    }
    pHddCtx->isAmpAllowed = VOS_FALSE;
    EXIT();
    return status;
}


/*
                                
                                                             
 */
int wlan_hdd_disconnect( hdd_adapter_t *pAdapter, u16 reason )
{
    int status;
    hdd_station_ctx_t *pHddStaCtx = WLAN_HDD_GET_STATION_CTX_PTR(pAdapter);
    hdd_context_t *pHddCtx = WLAN_HDD_GET_CTX(pAdapter);

    status = wlan_hdd_validate_context(pHddCtx);

    if (0 != status)
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                   "%s: HDD context is not valid", __func__);
        return status;
    }

    pHddCtx->isAmpAllowed = VOS_TRUE;
    pHddStaCtx->conn_info.connState = eConnectionState_NotConnected;
    INIT_COMPLETION(pAdapter->disconnect_comp_var);

    /*                */
    status = sme_RoamDisconnect( WLAN_HDD_GET_HAL_CTX(pAdapter),
                                 pAdapter->sessionId, reason);

    if ( 0 != status )
    {
        hddLog(VOS_TRACE_LEVEL_ERROR,
               "%s csrRoamDisconnect failure, returned %d \n",
               __func__, (int)status );
        return -EINVAL;
    }
    wait_for_completion_interruptible_timeout(
                &pAdapter->disconnect_comp_var,
                msecs_to_jiffies(WLAN_WAIT_TIME_DISCONNECT));
    /*              */
    netif_tx_disable(pAdapter->dev);
    netif_carrier_off(pAdapter->dev);
    return status;
}


/*
                                         
                                                             
 */
static int wlan_hdd_cfg80211_disconnect( struct wiphy *wiphy,
                                         struct net_device *dev,
                                         u16 reason
                                         )
{
    hdd_adapter_t *pAdapter = WLAN_HDD_GET_PRIV_PTR( dev );
    tCsrRoamProfile  *pRoamProfile =
                    &(WLAN_HDD_GET_WEXT_STATE_PTR(pAdapter))->roamProfile;
    int status;
    hdd_station_ctx_t *pHddStaCtx = WLAN_HDD_GET_STATION_CTX_PTR(pAdapter);
    hdd_context_t *pHddCtx = WLAN_HDD_GET_CTX(pAdapter);
#ifdef FEATURE_WLAN_TDLS
    tANI_U8 staIdx;
#endif

    ENTER();

    hddLog(VOS_TRACE_LEVEL_INFO, "%s: device_mode = %d\n",
                                    __func__,pAdapter->device_mode);

    hddLog(VOS_TRACE_LEVEL_INFO, "%s: Disconnect called with reason code %d",
            __func__, reason);

    status = wlan_hdd_validate_context(pHddCtx);

    if (0 != status)
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                   "%s: HDD context is not valid", __func__);
        return status;
    }

    if (NULL != pRoamProfile)
    {
        /*                                                                 */
        if ((pHddStaCtx->conn_info.connState == eConnectionState_Associated) ||
            (pHddStaCtx->conn_info.connState == eConnectionState_Connecting))
        {
            eCsrRoamDisconnectReason reasonCode =
                                       eCSR_DISCONNECT_REASON_UNSPECIFIED;
            hdd_scaninfo_t *pScanInfo;
            switch(reason)
            {
                case WLAN_REASON_MIC_FAILURE:
                    reasonCode = eCSR_DISCONNECT_REASON_MIC_ERROR;
                    break;

                case WLAN_REASON_DISASSOC_DUE_TO_INACTIVITY:
                case WLAN_REASON_DISASSOC_AP_BUSY:
                case WLAN_REASON_CLASS3_FRAME_FROM_NONASSOC_STA:
                    reasonCode = eCSR_DISCONNECT_REASON_DISASSOC;
                    break;

                case WLAN_REASON_PREV_AUTH_NOT_VALID:
                case WLAN_REASON_CLASS2_FRAME_FROM_NONAUTH_STA:
                    reasonCode = eCSR_DISCONNECT_REASON_DEAUTH;
                    break;

                case WLAN_REASON_DEAUTH_LEAVING:
                default:
                    reasonCode = eCSR_DISCONNECT_REASON_UNSPECIFIED;
                    break;
            }
            pHddStaCtx->conn_info.connState = eConnectionState_NotConnected;
            pScanInfo =  &pHddCtx->scan_info;
            if (pScanInfo->mScanPending)
            {
               hddLog(VOS_TRACE_LEVEL_INFO, "Disconnect is in progress, "
                              "Aborting Scan");
                hdd_abort_mac_scan(pHddCtx);
            }

#ifdef FEATURE_WLAN_TDLS
            /*                                      */
            for (staIdx = 0 ; staIdx < HDD_MAX_NUM_TDLS_STA; staIdx++)
            {
                if ((pHddCtx->tdlsConnInfo[staIdx].sessionId == pAdapter->sessionId) &&
                    (pHddCtx->tdlsConnInfo[staIdx].staId))
                {
                    uint8 *mac;
                    mac = pHddCtx->tdlsConnInfo[staIdx].peerMac.bytes;
                    VOS_TRACE(VOS_MODULE_ID_HDD, TDLS_LOG_LEVEL,
                            "%s: call sme_DeleteTdlsPeerSta staId %d sessionId %d " MAC_ADDRESS_STR,
                            __func__, pHddCtx->tdlsConnInfo[staIdx].staId, pAdapter->sessionId,
                            MAC_ADDR_ARRAY(mac));
                    sme_DeleteTdlsPeerSta(WLAN_HDD_GET_HAL_CTX(pAdapter),
                                          pAdapter->sessionId,
                                          mac);
                }
            }
#endif
            status = wlan_hdd_disconnect(pAdapter, reasonCode);
            if ( 0 != status )
            {
                hddLog(VOS_TRACE_LEVEL_ERROR,
                        "%s wlan_hdd_disconnect failure, returned %d \n",
                        __func__, (int)status );
                return -EINVAL;
            }
        }
        else
        {
            hddLog(VOS_TRACE_LEVEL_ERROR, "%s: unexpected cfg disconnect API"
                   "called while in %d state", __func__,
                    pHddStaCtx->conn_info.connState);
        }
    }
    else
    {
        hddLog(VOS_TRACE_LEVEL_ERROR, "%s: No valid roam profile", __func__);
    }

    return status;
}


/*
                                               
                                                   
                         
 */
static int wlan_hdd_cfg80211_set_privacy_ibss(
                                         hdd_adapter_t *pAdapter,
                                         struct cfg80211_ibss_params *params
                                         )
{
    int status = 0;
    hdd_wext_state_t *pWextState = WLAN_HDD_GET_WEXT_STATE_PTR(pAdapter);
    eCsrEncryptionType encryptionType = eCSR_ENCRYPT_TYPE_NONE;
    hdd_station_ctx_t *pHddStaCtx = WLAN_HDD_GET_STATION_CTX_PTR(pAdapter);

    ENTER();

    pWextState->wpaVersion = IW_AUTH_WPA_VERSION_DISABLED;

    if (params->ie_len && ( NULL != params->ie) )
    {
        if (wlan_hdd_cfg80211_get_ie_ptr (params->ie,
                            params->ie_len, WLAN_EID_RSN ))
        {
            pWextState->wpaVersion = IW_AUTH_WPA_VERSION_WPA2;
            encryptionType = eCSR_ENCRYPT_TYPE_AES;
        }
        else if ( hdd_isWPAIEPresent (params->ie, params->ie_len ))
        {
            tDot11fIEWPA dot11WPAIE;
            tHalHandle halHandle = WLAN_HDD_GET_HAL_CTX(pAdapter);
            u8 *ie;

            memset(&dot11WPAIE, 0, sizeof(dot11WPAIE));
            ie = wlan_hdd_cfg80211_get_ie_ptr (params->ie,
                                     params->ie_len, DOT11F_EID_WPA);
            if ( NULL != ie )
            {
                pWextState->wpaVersion = IW_AUTH_WPA_VERSION_WPA;
                //                  
                //                                                               
                dot11fUnpackIeWPA((tpAniSirGlobal) halHandle,
                                &ie[2+4],
                                ie[1] - 4,
                                &dot11WPAIE);
                /*                                                     
                                                          */
                encryptionType =
                  hdd_TranslateWPAToCsrEncryptionType(dot11WPAIE.multicast_cipher);
            }
        }

        status = wlan_hdd_cfg80211_set_ie(pAdapter, params->ie, params->ie_len);

        if (0 > status)
        {
            hddLog(VOS_TRACE_LEVEL_ERROR, "%s: failed to parse WPA/RSN IE",
                    __func__);
            return status;
        }
    }

    pWextState->roamProfile.AuthType.authType[0] =
                                pHddStaCtx->conn_info.authType =
                                eCSR_AUTH_TYPE_OPEN_SYSTEM;

    if (params->privacy)
    {
        /*                                                                      
                                                                             
                                             
                                                                   
                                     */
        /*                                              
                                        */

        encryptionType = eCSR_ENCRYPT_TYPE_WEP40_STATICKEY;
    }
    VOS_TRACE (VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO_HIGH,
                          "encryptionType=%d", encryptionType);
    pHddStaCtx->conn_info.ucEncryptionType                   = encryptionType;
    pWextState->roamProfile.EncryptionType.numEntries        = 1;
    pWextState->roamProfile.EncryptionType.encryptionType[0] = encryptionType;
    return status;
}

/*
                                        
                                               
 */
static int wlan_hdd_cfg80211_join_ibss( struct wiphy *wiphy,
                                        struct net_device *dev,
                                        struct cfg80211_ibss_params *params
                                       )
{
    hdd_adapter_t *pAdapter = WLAN_HDD_GET_PRIV_PTR( dev );
    hdd_wext_state_t *pWextState = WLAN_HDD_GET_WEXT_STATE_PTR(pAdapter);
    tCsrRoamProfile          *pRoamProfile;
    int status;
    hdd_station_ctx_t *pHddStaCtx = WLAN_HDD_GET_STATION_CTX_PTR(pAdapter);
    hdd_context_t *pHddCtx = WLAN_HDD_GET_CTX(pAdapter);

    ENTER();

    hddLog(VOS_TRACE_LEVEL_INFO,
                  "%s: device_mode = %d\n",__func__,pAdapter->device_mode);

    status = wlan_hdd_validate_context(pHddCtx);

    if (0 != status)
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                   "%s: HDD context is not valid", __func__);
        return status;
    }

    if (NULL == pWextState)
    {
        hddLog (VOS_TRACE_LEVEL_ERROR, "%s ERROR: Data Storage Corruption\n",
                __func__);
        return -EIO;
    }

    pRoamProfile = &pWextState->roamProfile;

    if ( eCSR_BSS_TYPE_START_IBSS != pRoamProfile->BSSType )
    {
        hddLog (VOS_TRACE_LEVEL_ERROR,
                "%s Interface type is not set to IBSS \n", __func__);
        return -EINVAL;
    }

    /*             */
    if (NULL !=
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,8,0))
                params->chandef.chan)
#else
                params->channel)
#endif
    {
        u8 channelNum;
        v_U32_t numChans = WNI_CFG_VALID_CHANNEL_LIST_LEN;
        v_U8_t validChan[WNI_CFG_VALID_CHANNEL_LIST_LEN];
        tHalHandle hHal = WLAN_HDD_GET_HAL_CTX(pAdapter);
        int indx;

        /*                    */
        channelNum =
               ieee80211_frequency_to_channel(
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,8,0))
                                            params->chandef.chan->center_freq);
#else
                                            params->channel->center_freq);
#endif

        if (0 != ccmCfgGetStr(hHal, WNI_CFG_VALID_CHANNEL_LIST,
                    validChan, &numChans))
        {
            hddLog(VOS_TRACE_LEVEL_ERROR, "%s: No valid channel list",
                    __func__);
            return -EOPNOTSUPP;
        }

        for (indx = 0; indx < numChans; indx++)
        {
            if (channelNum == validChan[indx])
            {
                break;
            }
        }
        if (indx >= numChans)
        {
            hddLog(VOS_TRACE_LEVEL_ERROR, "%s: Not valid Channel %d",
                    __func__, channelNum);
            return -EINVAL;
        }
        /*                             */
        hddLog(VOS_TRACE_LEVEL_INFO_HIGH, "%s: set channel %d", __func__,
                channelNum);
        pRoamProfile->ChannelInfo.numOfChannels = 1;
        pHddStaCtx->conn_info.operationChannel = channelNum;
        pRoamProfile->ChannelInfo.ChannelList =
            &pHddStaCtx->conn_info.operationChannel;
    }

    /*                                */
    status = wlan_hdd_cfg80211_set_privacy_ibss(pAdapter, params);
    if (status < 0)
    {
        hddLog(VOS_TRACE_LEVEL_ERROR, "%s: failed to set security parameters",
                __func__);
        return status;
    }

    /*                     */
    status = wlan_hdd_cfg80211_connect_start(pAdapter, params->ssid,
            params->ssid_len, params->bssid,
            pHddStaCtx->conn_info.operationChannel);

    if (0 > status)
    {
        hddLog(VOS_TRACE_LEVEL_ERROR, "%s: connect failed", __func__);
        return status;
    }

    return 0;
}

/*
                                         
                                         
 */
static int wlan_hdd_cfg80211_leave_ibss( struct wiphy *wiphy,
                                         struct net_device *dev
                                         )
{
    hdd_adapter_t *pAdapter = WLAN_HDD_GET_PRIV_PTR( dev );
    hdd_wext_state_t *pWextState = WLAN_HDD_GET_WEXT_STATE_PTR(pAdapter);
    tCsrRoamProfile *pRoamProfile;
    hdd_context_t *pHddCtx = WLAN_HDD_GET_CTX(pAdapter);
    int status;

    ENTER();

    status = wlan_hdd_validate_context(pHddCtx);

    if (0 != status)
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                   "%s: HDD context is not valid", __func__);
        return status;
    }

    hddLog(VOS_TRACE_LEVEL_INFO, "%s: device_mode = %d\n",__func__,pAdapter->device_mode);
    if (NULL == pWextState)
    {
        hddLog (VOS_TRACE_LEVEL_ERROR, "%s ERROR: Data Storage Corruption\n",
                __func__);
        return -EIO;
    }

    pRoamProfile = &pWextState->roamProfile;

    /*                                                        */
    if (eCSR_BSS_TYPE_START_IBSS != pRoamProfile->BSSType)
    {
        hddLog (VOS_TRACE_LEVEL_ERROR, "%s: BSS Type is not set to IBSS",
                __func__);
        return -EINVAL;
    }

    /*                          */
    INIT_COMPLETION(pAdapter->disconnect_comp_var);
    sme_RoamDisconnect( WLAN_HDD_GET_HAL_CTX(pAdapter), pAdapter->sessionId,
                                  eCSR_DISCONNECT_REASON_IBSS_LEAVE);

    return 0;
}

/*
                                               
                                                  
                                                     
 */
static int wlan_hdd_cfg80211_set_wiphy_params(struct wiphy *wiphy,
        u32 changed)
{
    hdd_context_t *pHddCtx = wiphy_priv(wiphy);
    tHalHandle hHal = pHddCtx->hHal;
    int status;

    ENTER();

    status = wlan_hdd_validate_context(pHddCtx);

    if (0 != status)
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                   "%s: HDD context is not valid", __func__);
        return status;
    }

    if (changed & WIPHY_PARAM_RTS_THRESHOLD)
    {
        u16 rts_threshold = (wiphy->rts_threshold == -1) ?
                               WNI_CFG_RTS_THRESHOLD_STAMAX :
                               wiphy->rts_threshold;

        if ((WNI_CFG_RTS_THRESHOLD_STAMIN > rts_threshold) ||
                (WNI_CFG_RTS_THRESHOLD_STAMAX < rts_threshold))
        {
            hddLog(VOS_TRACE_LEVEL_ERROR,
                    "%s: Invalid RTS Threshold value %hu",
                    __func__, rts_threshold);
            return -EINVAL;
        }

        if (0 != ccmCfgSetInt(hHal, WNI_CFG_RTS_THRESHOLD,
                    rts_threshold, ccmCfgSetCallback,
                    eANI_BOOLEAN_TRUE))
        {
            hddLog(VOS_TRACE_LEVEL_ERROR,
                    "%s: ccmCfgSetInt failed for rts_threshold value %hu",
                    __func__, rts_threshold);
            return -EIO;
        }

        hddLog(VOS_TRACE_LEVEL_INFO_MED, "%s: set rts threshold %hu", __func__,
                rts_threshold);
    }

    if (changed & WIPHY_PARAM_FRAG_THRESHOLD)
    {
        u16 frag_threshold = (wiphy->frag_threshold == -1) ?
                                WNI_CFG_FRAGMENTATION_THRESHOLD_STAMAX :
                                wiphy->frag_threshold;

        if ((WNI_CFG_FRAGMENTATION_THRESHOLD_STAMIN > frag_threshold)||
                (WNI_CFG_FRAGMENTATION_THRESHOLD_STAMAX < frag_threshold) )
        {
            hddLog(VOS_TRACE_LEVEL_ERROR,
                    "%s: Invalid frag_threshold value %hu", __func__,
                    frag_threshold);
            return -EINVAL;
        }

        if (0 != ccmCfgSetInt(hHal, WNI_CFG_FRAGMENTATION_THRESHOLD,
                    frag_threshold, ccmCfgSetCallback,
                    eANI_BOOLEAN_TRUE))
        {
            hddLog(VOS_TRACE_LEVEL_ERROR,
                    "%s: ccmCfgSetInt failed for frag_threshold value %hu",
                    __func__, frag_threshold);
            return -EIO;
        }

        hddLog(VOS_TRACE_LEVEL_INFO_MED, "%s: set frag threshold %hu", __func__,
                frag_threshold);
    }

    if ((changed & WIPHY_PARAM_RETRY_SHORT)
            || (changed & WIPHY_PARAM_RETRY_LONG))
    {
        u8 retry_value = (changed & WIPHY_PARAM_RETRY_SHORT) ?
                         wiphy->retry_short :
                         wiphy->retry_long;

        if ((WNI_CFG_LONG_RETRY_LIMIT_STAMIN > retry_value) ||
                (WNI_CFG_LONG_RETRY_LIMIT_STAMAX < retry_value))
        {
            hddLog(VOS_TRACE_LEVEL_ERROR, "%s: Invalid Retry count %hu",
                    __func__, retry_value);
            return -EINVAL;
        }

        if (changed & WIPHY_PARAM_RETRY_SHORT)
        {
            if (0 != ccmCfgSetInt(hHal, WNI_CFG_LONG_RETRY_LIMIT,
                        retry_value, ccmCfgSetCallback,
                        eANI_BOOLEAN_TRUE))
            {
                hddLog(VOS_TRACE_LEVEL_ERROR,
                        "%s: ccmCfgSetInt failed for long retry count %hu",
                        __func__, retry_value);
                return -EIO;
            }
            hddLog(VOS_TRACE_LEVEL_INFO_MED, "%s: set long retry count %hu",
                    __func__, retry_value);
        }
        else if (changed & WIPHY_PARAM_RETRY_SHORT)
        {
            if (0 != ccmCfgSetInt(hHal, WNI_CFG_SHORT_RETRY_LIMIT,
                        retry_value, ccmCfgSetCallback,
                        eANI_BOOLEAN_TRUE))
            {
                hddLog(VOS_TRACE_LEVEL_ERROR,
                        "%s: ccmCfgSetInt failed for short retry count %hu",
                        __func__, retry_value);
                return -EIO;
            }
            hddLog(VOS_TRACE_LEVEL_INFO_MED, "%s: set short retry count %hu",
                    __func__, retry_value);
        }
    }

    return 0;
}

/*
                                          
                                           
 */
static int wlan_hdd_cfg80211_set_txpower(struct wiphy *wiphy,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,8,0)
        struct wireless_dev *wdev,
#endif
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,35)
        enum tx_power_setting type,
#else
        enum nl80211_tx_power_setting type,
#endif
        int dbm)
{
    hdd_context_t *pHddCtx = (hdd_context_t*) wiphy_priv(wiphy);
    tHalHandle hHal = NULL;
    tSirMacAddr bssid = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    tSirMacAddr selfMac = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    int status;

    ENTER();

    status = wlan_hdd_validate_context(pHddCtx);

    if (0 != status)
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                   "%s: HDD context is not valid", __func__);
        return status;
    }

    hHal = pHddCtx->hHal;

    if (0 != ccmCfgSetInt(hHal, WNI_CFG_CURRENT_TX_POWER_LEVEL,
                dbm, ccmCfgSetCallback,
                eANI_BOOLEAN_TRUE))
    {
        hddLog(VOS_TRACE_LEVEL_ERROR,
                "%s: ccmCfgSetInt failed for tx power %hu", __func__, dbm);
        return -EIO;
    }

    hddLog(VOS_TRACE_LEVEL_INFO_MED, "%s: set tx power level %d dbm", __func__,
            dbm);

    switch(type)
    {
    case NL80211_TX_POWER_AUTOMATIC: /*                                      */
       /*              */
    case NL80211_TX_POWER_LIMITED: /*                                   */
       if( sme_SetMaxTxPower(hHal, bssid, selfMac, dbm) != eHAL_STATUS_SUCCESS )
       {
          hddLog(VOS_TRACE_LEVEL_ERROR, "%s: Setting maximum tx power failed",
                 __func__);
          return -EIO;
       }
       break;
    case NL80211_TX_POWER_FIXED: /*                                 */
       hddLog(VOS_TRACE_LEVEL_ERROR, "%s: NL80211_TX_POWER_FIXED not supported",
              __func__);
       return -EOPNOTSUPP;
       break;
    default:
       hddLog(VOS_TRACE_LEVEL_ERROR, "%s: Invalid power setting type %d",
              __func__, type);
       return -EIO;
    }

    return 0;
}

/*
                                          
                                            
 */
static int wlan_hdd_cfg80211_get_txpower(struct wiphy *wiphy,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,8,0)
                                         struct wireless_dev *wdev,
#endif
                                         int *dbm)
{

    hdd_adapter_t *pAdapter;
    hdd_context_t *pHddCtx = (hdd_context_t*) wiphy_priv(wiphy);
    int status;

    ENTER();

    status = wlan_hdd_validate_context(pHddCtx);

    if (0 != status)
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                   "%s: HDD context is not valid", __func__);
        *dbm = 0;
        return status;
    }

    pAdapter = hdd_get_adapter(pHddCtx, WLAN_HDD_INFRA_STATION);
    if (NULL == pAdapter)
    {
        hddLog(VOS_TRACE_LEVEL_FATAL, "%s: Not in station context " ,__func__);
        return -ENOENT;
    }

    wlan_hdd_get_classAstats(pAdapter);
    *dbm = pAdapter->hdd_stats.ClassA_stat.max_pwr;

    EXIT();
    return 0;
}

static int wlan_hdd_cfg80211_get_station(struct wiphy *wiphy, struct net_device *dev,
                                   u8* mac, struct station_info *sinfo)
{
    hdd_adapter_t *pAdapter = WLAN_HDD_GET_PRIV_PTR( dev );
    hdd_station_ctx_t *pHddStaCtx = WLAN_HDD_GET_STATION_CTX_PTR(pAdapter);
    int ssidlen = pHddStaCtx->conn_info.SSID.SSID.length;
    tANI_U8 rate_flags;

    hdd_context_t *pHddCtx = (hdd_context_t*) wiphy_priv(wiphy);
    hdd_config_t  *pCfg    = pHddCtx->cfg_ini;

    tANI_U8  OperationalRates[CSR_DOT11_SUPPORTED_RATES_MAX];
    tANI_U32 ORLeng = CSR_DOT11_SUPPORTED_RATES_MAX;
    tANI_U8  ExtendedRates[CSR_DOT11_EXTENDED_SUPPORTED_RATES_MAX];
    tANI_U32 ERLeng = CSR_DOT11_EXTENDED_SUPPORTED_RATES_MAX;
    tANI_U8  MCSRates[SIZE_OF_BASIC_MCS_SET];
    tANI_U32 MCSLeng = SIZE_OF_BASIC_MCS_SET;
    tANI_U16 maxRate = 0;
    tANI_U16 myRate;
    tANI_U16 currentRate = 0;
    tANI_U8  maxSpeedMCS = 0;
    tANI_U8  maxMCSIdx = 0;
    tANI_U8  rateFlag = 1;
    tANI_U8  i, j, rssidx;
    tANI_U16 temp;
    int status;

#ifdef WLAN_FEATURE_11AC
    tANI_U32 vht_mcs_map;
    eDataRate11ACMaxMcs vhtMaxMcs;
#endif /*                   */

    ENTER();

    if ((eConnectionState_Associated != pHddStaCtx->conn_info.connState) ||
            (0 == ssidlen))
    {
        hddLog(VOS_TRACE_LEVEL_INFO, "%s: Not associated or"
                    " Invalid ssidlen, %d", __func__, ssidlen);
        /*                 */
        return 0;
    }

    status = wlan_hdd_validate_context(pHddCtx);

    if (0 != status)
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                   "%s: HDD context is not valid", __func__);
        return status;
    }

    wlan_hdd_get_rssi(pAdapter, &sinfo->signal);
    sinfo->filled |= STATION_INFO_SIGNAL;

    wlan_hdd_get_station_stats(pAdapter);
    rate_flags = pAdapter->hdd_stats.ClassA_stat.tx_rate_flags;

    //                                  
    myRate = pAdapter->hdd_stats.ClassA_stat.tx_rate * 5;

#ifdef LINKSPEED_DEBUG_ENABLED
    pr_info("RSSI %d, RLMS %u, rate %d, rssi high %d, rssi mid %d, rssi low %d, rate_flags 0x%x, MCS %d\n",
            sinfo->signal,
            pCfg->reportMaxLinkSpeed,
            myRate,
            (int) pCfg->linkSpeedRssiHigh,
            (int) pCfg->linkSpeedRssiMid,
            (int) pCfg->linkSpeedRssiLow,
            (int) rate_flags,
            (int) pAdapter->hdd_stats.ClassA_stat.mcs_index);
#endif //                       

    if (eHDD_LINK_SPEED_REPORT_ACTUAL != pCfg->reportMaxLinkSpeed)
    {
        //                                                       
        if (eHDD_LINK_SPEED_REPORT_MAX == pCfg->reportMaxLinkSpeed)
        {
            //                              
            rssidx = 0;
        }
        else if (eHDD_LINK_SPEED_REPORT_MAX_SCALED == pCfg->reportMaxLinkSpeed)
        {
            //                                                
            if (sinfo->signal >= pCfg->linkSpeedRssiHigh)
            {
                //                              
                rssidx = 0;
            }
            else if (sinfo->signal >= pCfg->linkSpeedRssiMid)
            {
                //                    
                rssidx = 1;
            }
            else if (sinfo->signal >= pCfg->linkSpeedRssiLow)
            {
                //                    
                rssidx = 2;
            }
            else
            {
                //                    
                rssidx = 3;
            }
        }
        else
        {
            //                                             
            hddLog(VOS_TRACE_LEVEL_ERROR,
                   "%s: Invalid value for reportMaxLinkSpeed: %u",
                    __func__, pCfg->reportMaxLinkSpeed);
            rssidx = 0;
        }

        maxRate = 0;

        /*                    */
        if (0 != ccmCfgGetStr(WLAN_HDD_GET_HAL_CTX(pAdapter), WNI_CFG_OPERATIONAL_RATE_SET,
                             OperationalRates, &ORLeng))
        {
            hddLog(VOS_TRACE_LEVEL_ERROR, "%s: ccm api returned failure", __func__);
            /*                 */
            return 0;
        }

        for (i = 0; i < ORLeng; i++)
        {
            for (j = 0; j < (sizeof(supported_data_rate) / sizeof(supported_data_rate[0])); j ++)
            {
                /*                   */
                if (supported_data_rate[j].beacon_rate_index == (OperationalRates[i] & 0x7F))
                {
                    currentRate = supported_data_rate[j].supported_rate[rssidx];
                    break;
                }
            }
            /*                 */
            maxRate = (currentRate > maxRate)?currentRate:maxRate;
        }

        /*                       */
        if (0 != ccmCfgGetStr(WLAN_HDD_GET_HAL_CTX(pAdapter), WNI_CFG_EXTENDED_OPERATIONAL_RATE_SET,
                             ExtendedRates, &ERLeng))
        {
            hddLog(VOS_TRACE_LEVEL_ERROR, "%s: ccm api returned failure", __func__);
            /*                 */
            return 0;
        }

        for (i = 0; i < ERLeng; i++)
        {
            for (j = 0; j < (sizeof(supported_data_rate) / sizeof(supported_data_rate[0])); j ++)
            {
                if (supported_data_rate[j].beacon_rate_index == (ExtendedRates[i] & 0x7F))
                {
                    currentRate = supported_data_rate[j].supported_rate[rssidx];
                    break;
                }
            }
            /*                 */
            maxRate = (currentRate > maxRate)?currentRate:maxRate;
        }
        /*                    
                                                          
                                */
         if ((0 == rssidx) ||
              (eHDD_LINK_SPEED_REPORT_MAX == pCfg->reportMaxLinkSpeed))
        {
            if (0 != ccmCfgGetStr(WLAN_HDD_GET_HAL_CTX(pAdapter), WNI_CFG_CURRENT_MCS_SET,
                                 MCSRates, &MCSLeng))
            {
                hddLog(VOS_TRACE_LEVEL_ERROR, "%s: ccm api returned failure", __func__);
                /*                 */
                return 0;
            }
            rateFlag = 0;
#ifdef WLAN_FEATURE_11AC
            /*                                    */
            if (rate_flags & (eHAL_TX_RATE_VHT20|eHAL_TX_RATE_VHT40|eHAL_TX_RATE_VHT80))
            {
                ccmCfgGetInt(WLAN_HDD_GET_HAL_CTX(pAdapter), WNI_CFG_VHT_TX_MCS_MAP, &vht_mcs_map);
                vhtMaxMcs = (eDataRate11ACMaxMcs)(vht_mcs_map & DATA_RATE_11AC_MCS_MASK );
                if (rate_flags & eHAL_TX_RATE_SGI)
                {
                    rateFlag |= 1;
                }
                if (DATA_RATE_11AC_MAX_MCS_7 == vhtMaxMcs)
                {
                    maxMCSIdx = 7;
                }
                else if (DATA_RATE_11AC_MAX_MCS_8 == vhtMaxMcs)
                {
                    maxMCSIdx = 8;
                }
                else if (DATA_RATE_11AC_MAX_MCS_9 == vhtMaxMcs)
                {
                    //                       
                    if (rate_flags & eHAL_TX_RATE_VHT20)
                        maxMCSIdx = 8;
                    else
                        maxMCSIdx = 9;
                }

                if (rate_flags & eHAL_TX_RATE_VHT80)
                {
                    currentRate = supported_vht_mcs_rate[pAdapter->hdd_stats.ClassA_stat.mcs_index].supported_VHT80_rate[rateFlag];
                    maxRate = supported_vht_mcs_rate[maxMCSIdx].supported_VHT80_rate[rateFlag];
                }
                else if (rate_flags & eHAL_TX_RATE_VHT40)
                {
                    currentRate = supported_vht_mcs_rate[pAdapter->hdd_stats.ClassA_stat.mcs_index].supported_VHT40_rate[rateFlag];
                    maxRate = supported_vht_mcs_rate[maxMCSIdx].supported_VHT40_rate[rateFlag];
                }
                else if (rate_flags & eHAL_TX_RATE_VHT20)
                {
                    currentRate = supported_vht_mcs_rate[pAdapter->hdd_stats.ClassA_stat.mcs_index].supported_VHT20_rate[rateFlag];
                    maxRate = supported_vht_mcs_rate[maxMCSIdx].supported_VHT20_rate[rateFlag];
                }

                maxSpeedMCS = 1;
                if (currentRate > maxRate)
                {
                    maxRate = currentRate;
                }

            }
            else
#endif /*                   */
            {
                if (rate_flags & eHAL_TX_RATE_HT40)
                {
                    rateFlag |= 1;
                }
                if (rate_flags & eHAL_TX_RATE_SGI)
                {
                    rateFlag |= 2;
                }

                for (i = 0; i < MCSLeng; i++)
                {
                    temp = sizeof(supported_mcs_rate) / sizeof(supported_mcs_rate[0]);
                    for (j = 0; j < temp; j++)
                    {
                        if (supported_mcs_rate[j].beacon_rate_index == MCSRates[i])
                        {
                            currentRate = supported_mcs_rate[j].supported_rate[rateFlag];
                            break;
                        }
                    }
                    if ((j < temp) && (currentRate > maxRate))
                    {
                        maxRate     = currentRate;
                        maxSpeedMCS = 1;
                        maxMCSIdx   = supported_mcs_rate[j].beacon_rate_index;
                    }
                }
            }
        }

        else if (!(rate_flags & eHAL_TX_RATE_LEGACY))
        {
            maxRate = myRate;
            maxSpeedMCS = 1;
            maxMCSIdx = pAdapter->hdd_stats.ClassA_stat.mcs_index;
        }

        //                                                                
        if (((maxRate < myRate) && (0 == rssidx)) ||
             (0 == maxRate))
        {
           maxRate = myRate;
           if (rate_flags & eHAL_TX_RATE_LEGACY)
           {
              maxSpeedMCS = 0;
           }
           else
           {
              maxSpeedMCS = 1;
              maxMCSIdx = pAdapter->hdd_stats.ClassA_stat.mcs_index;
           }
        }

        if (rate_flags & eHAL_TX_RATE_LEGACY)
        {
            sinfo->txrate.legacy  = maxRate;
#ifdef LINKSPEED_DEBUG_ENABLED
            pr_info("Reporting legacy rate %d\n", sinfo->txrate.legacy);
#endif //                       
        }
        else
        {
            sinfo->txrate.mcs    = maxMCSIdx;
#ifdef WLAN_FEATURE_11AC
            sinfo->txrate.nss = 1;
            if (rate_flags & eHAL_TX_RATE_VHT80)
            {
                sinfo->txrate.flags |= RATE_INFO_FLAGS_VHT_MCS;
                sinfo->txrate.flags |= RATE_INFO_FLAGS_80_MHZ_WIDTH;
            }
            else if (rate_flags & eHAL_TX_RATE_VHT40)
            {
                sinfo->txrate.flags |= RATE_INFO_FLAGS_VHT_MCS;
                sinfo->txrate.flags |= RATE_INFO_FLAGS_40_MHZ_WIDTH;
            }
            else if (rate_flags & eHAL_TX_RATE_VHT20)
            {
                sinfo->txrate.flags |= RATE_INFO_FLAGS_VHT_MCS;
            }
#endif /*                   */
            if (rate_flags & (eHAL_TX_RATE_HT20 | eHAL_TX_RATE_HT40))
            {
                sinfo->txrate.flags |= RATE_INFO_FLAGS_MCS;
                if (rate_flags & eHAL_TX_RATE_HT40)
                {
                    sinfo->txrate.flags |= RATE_INFO_FLAGS_40_MHZ_WIDTH;
                }
            }
            if (rate_flags & eHAL_TX_RATE_SGI)
            {
                sinfo->txrate.flags |= RATE_INFO_FLAGS_SHORT_GI;
            }

#ifdef LINKSPEED_DEBUG_ENABLED
            pr_info("Reporting MCS rate %d flags %x\n",
                    sinfo->txrate.mcs,
                    sinfo->txrate.flags );
#endif //                       
        }
    }
    else
    {
        //                                        

        if (rate_flags & eHAL_TX_RATE_LEGACY)
        {
            //                                     
            sinfo->txrate.legacy = myRate;
#ifdef LINKSPEED_DEBUG_ENABLED
            pr_info("Reporting actual legacy rate %d\n", sinfo->txrate.legacy);
#endif //                       
        }
        else
        {
            //           
            sinfo->txrate.mcs = pAdapter->hdd_stats.ClassA_stat.mcs_index;
#ifdef WLAN_FEATURE_11AC
            sinfo->txrate.nss = 1;
            if (rate_flags & eHAL_TX_RATE_VHT80)
            {
                sinfo->txrate.flags |= RATE_INFO_FLAGS_VHT_MCS;
            }
            else
#endif /*                   */
            {
                sinfo->txrate.flags |= RATE_INFO_FLAGS_MCS;
            }
            if (rate_flags & eHAL_TX_RATE_SGI)
            {
                sinfo->txrate.flags |= RATE_INFO_FLAGS_SHORT_GI;
            }
            if (rate_flags & eHAL_TX_RATE_HT40)
            {
                sinfo->txrate.flags |= RATE_INFO_FLAGS_40_MHZ_WIDTH;
            }
#ifdef WLAN_FEATURE_11AC
            else if (rate_flags & eHAL_TX_RATE_VHT80)
            {
                sinfo->txrate.flags |= RATE_INFO_FLAGS_80_MHZ_WIDTH;
            }
#endif /*                   */
#ifdef LINKSPEED_DEBUG_ENABLED
            pr_info("Reporting actual MCS rate %d flags %x\n",
                    sinfo->txrate.mcs,
                    sinfo->txrate.flags );
#endif //                       
        }
    }
    sinfo->filled |= STATION_INFO_TX_BITRATE;

    sinfo->tx_packets =
       pAdapter->hdd_stats.summary_stat.tx_frm_cnt[0] +
       pAdapter->hdd_stats.summary_stat.tx_frm_cnt[1] +
       pAdapter->hdd_stats.summary_stat.tx_frm_cnt[2] +
       pAdapter->hdd_stats.summary_stat.tx_frm_cnt[3];

    sinfo->tx_retries =
       pAdapter->hdd_stats.summary_stat.retry_cnt[0] +
       pAdapter->hdd_stats.summary_stat.retry_cnt[1] +
       pAdapter->hdd_stats.summary_stat.retry_cnt[2] +
       pAdapter->hdd_stats.summary_stat.retry_cnt[3];

    sinfo->tx_failed =
       pAdapter->hdd_stats.summary_stat.fail_cnt[0] +
       pAdapter->hdd_stats.summary_stat.fail_cnt[1] +
       pAdapter->hdd_stats.summary_stat.fail_cnt[2] +
       pAdapter->hdd_stats.summary_stat.fail_cnt[3];

    sinfo->filled |=
       STATION_INFO_TX_PACKETS |
       STATION_INFO_TX_RETRIES |
       STATION_INFO_TX_FAILED;

       EXIT();
       return 0;
}

static int wlan_hdd_cfg80211_set_power_mgmt(struct wiphy *wiphy,
                     struct net_device *dev, bool mode, int timeout)
{
    hdd_adapter_t *pAdapter = WLAN_HDD_GET_PRIV_PTR(dev);
    hdd_context_t *pHddCtx;
    VOS_STATUS vos_status;
    int status;

    ENTER();

    if (NULL == pAdapter)
    {
        hddLog(VOS_TRACE_LEVEL_ERROR, "%s: Adapter is NULL\n", __func__);
        return -ENODEV;
    }

    pHddCtx = WLAN_HDD_GET_CTX(pAdapter);
    status = wlan_hdd_validate_context(pHddCtx);

    if (0 != status)
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                   "%s: HDD context is not valid", __func__);
        return status;
    }

    if ((DRIVER_POWER_MODE_AUTO == !mode) &&
        (TRUE == pHddCtx->hdd_wlan_suspended) &&
        (pHddCtx->cfg_ini->fhostArpOffload) &&
        (eConnectionState_Associated ==
             (WLAN_HDD_GET_STATION_CTX_PTR(pAdapter))->conn_info.connState))
    {

        hddLog(VOS_TRACE_LEVEL_INFO,
               "offload: in cfg80211_set_power_mgmt, calling arp offload");
        vos_status = hdd_conf_arp_offload(pAdapter, TRUE);
        if (!VOS_IS_STATUS_SUCCESS(vos_status))
        {
            hddLog(VOS_TRACE_LEVEL_INFO,
                   "%s:Failed to enable ARPOFFLOAD Feature %d\n",
                   __func__, vos_status);
        }
    }

    /*                                                                  
                                                  
                                                       
      */
    vos_status =  wlan_hdd_enter_bmps(pAdapter, !mode);

    EXIT();
    if (VOS_STATUS_E_FAILURE == vos_status)
    {
        return -EINVAL;
    }
    return 0;
}


#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,38))
static int wlan_hdd_set_default_mgmt_key(struct wiphy *wiphy,
                         struct net_device *netdev,
                         u8 key_index)
{
    ENTER();
    return 0;
}
#endif //                  

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0))
static int wlan_hdd_set_txq_params(struct wiphy *wiphy,
                   struct net_device *dev,
                   struct ieee80211_txq_params *params)
{
    ENTER();
    return 0;
}
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,38))
static int wlan_hdd_set_txq_params(struct wiphy *wiphy,
                   struct ieee80211_txq_params *params)
{
    ENTER();
    return 0;
}
#endif //                  

static int wlan_hdd_cfg80211_del_station(struct wiphy *wiphy,
                                         struct net_device *dev, u8 *mac)
{
    hdd_adapter_t *pAdapter = WLAN_HDD_GET_PRIV_PTR(dev);
    hdd_context_t *pHddCtx;
    VOS_STATUS vos_status;
    int status;
    v_U8_t staId;

    ENTER();
    if ( NULL == pAdapter )
    {
        hddLog(VOS_TRACE_LEVEL_ERROR, "%s: Invalid Adapter" ,__func__);
        return -EINVAL;
    }

    pHddCtx = WLAN_HDD_GET_CTX(pAdapter);
    status = wlan_hdd_validate_context(pHddCtx);

    if (0 != status)
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                   "%s: HDD context is not valid", __func__);
        return status;
    }

    if ( (WLAN_HDD_SOFTAP == pAdapter->device_mode)
       || (WLAN_HDD_P2P_GO == pAdapter->device_mode)
       )
    {
        if( NULL == mac )
        {
            v_U16_t i;
            for(i = 0; i < WLAN_MAX_STA_COUNT; i++)
            {
                if ((pAdapter->aStaInfo[i].isUsed) &&
                    (!pAdapter->aStaInfo[i].isDeauthInProgress))
                {
                    u8 *macAddr = pAdapter->aStaInfo[i].macAddrSTA.bytes;
                    hddLog(VOS_TRACE_LEVEL_INFO,
                           "%s: Delete STA with MAC::"
                            MAC_ADDRESS_STR,
                            __func__, MAC_ADDR_ARRAY(macAddr));
                    vos_status = hdd_softap_sta_deauth(pAdapter, macAddr);
                    if (VOS_IS_STATUS_SUCCESS(vos_status))
                        pAdapter->aStaInfo[i].isDeauthInProgress = TRUE;
                }
            }
        }
        else
        {

            vos_status = hdd_softap_GetStaId(pAdapter,(v_MACADDR_t *)mac, &staId);
            if (!VOS_IS_STATUS_SUCCESS(vos_status))
            {
                hddLog(VOS_TRACE_LEVEL_INFO,
                       "%s: Skip this DEL STA as this is not used::"
                       MAC_ADDRESS_STR,
                       __func__, MAC_ADDR_ARRAY(mac));
                return -ENOENT;
            }

            if( pAdapter->aStaInfo[staId].isDeauthInProgress == TRUE)
            {
                hddLog(VOS_TRACE_LEVEL_INFO,
                       "%s: Skip this DEL STA as deauth is in progress::"
                       MAC_ADDRESS_STR,
                       __func__, MAC_ADDR_ARRAY(mac));
                return -ENOENT;
            }

            pAdapter->aStaInfo[staId].isDeauthInProgress = TRUE;

            hddLog(VOS_TRACE_LEVEL_INFO,
                                "%s: Delete STA with MAC::"
                                MAC_ADDRESS_STR,
                                __func__,
                                MAC_ADDR_ARRAY(mac));

            vos_status = hdd_softap_sta_deauth(pAdapter, mac);
            if (!VOS_IS_STATUS_SUCCESS(vos_status))
            {
                pAdapter->aStaInfo[staId].isDeauthInProgress = FALSE;
                hddLog(VOS_TRACE_LEVEL_INFO,
                                "%s: STA removal failed for ::"
                                MAC_ADDRESS_STR,
                                __func__,
                                MAC_ADDR_ARRAY(mac));
                return -ENOENT;
            }

        }
    }

    EXIT();

    return 0;
}

static int wlan_hdd_cfg80211_add_station(struct wiphy *wiphy,
          struct net_device *dev, u8 *mac, struct station_parameters *params)
{
    int status = -EPERM;
#ifdef FEATURE_WLAN_TDLS
    u32 mask, set;
    ENTER();
    mask = params->sta_flags_mask;

    set = params->sta_flags_set;

#ifdef WLAN_FEATURE_TDLS_DEBUG
    VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
               "%s: mask 0x%x set 0x%x " MAC_ADDRESS_STR,
               __func__, mask, set, MAC_ADDR_ARRAY(mac));
#endif

    if (mask & BIT(NL80211_STA_FLAG_TDLS_PEER)) {
        if (set & BIT(NL80211_STA_FLAG_TDLS_PEER)) {
            status = wlan_hdd_tdls_add_station(wiphy, dev, mac, 0, NULL);
        }
    }
#endif
    return status;
}


#ifdef FEATURE_WLAN_LFR
#define MAX_PMKSAIDS_IN_CACHE 8

static tPmkidCacheInfo PMKIDCache[MAX_PMKSAIDS_IN_CACHE]; //                
static tANI_U32 PMKIDCacheIndex; //                      


static int wlan_hdd_cfg80211_set_pmksa(struct wiphy *wiphy, struct net_device *dev,
            struct cfg80211_pmksa *pmksa)
{
    tANI_U32 j=0;
    hdd_adapter_t *pAdapter = WLAN_HDD_GET_PRIV_PTR(dev);
    tHalHandle halHandle;
    eHalStatus result;
    int status;
    tANI_U8  BSSIDMatched = 0;
    hdd_context_t *pHddCtx;

    ENTER();

    //                  
    if ( NULL == pAdapter )
    {
        hddLog(VOS_TRACE_LEVEL_ERROR, "%s: Invalid Adapter" ,__func__);
        return -EINVAL;
    }

    pHddCtx = WLAN_HDD_GET_CTX(pAdapter);
    status = wlan_hdd_validate_context(pHddCtx);

    if (0 != status)
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                   "%s: HDD context is not valid", __func__);
        return status;
    }

    //                   
    halHandle = WLAN_HDD_GET_HAL_CTX(pAdapter);

    for (j = 0; j < PMKIDCacheIndex; j++)
    {
        if(vos_mem_compare(PMKIDCache[j].BSSID,
                    pmksa->bssid, WNI_CFG_BSSID_LEN))
        {
            /*                                              */
            BSSIDMatched = 1;
            vos_mem_copy(PMKIDCache[j].BSSID,
                    pmksa->bssid, WNI_CFG_BSSID_LEN);
            vos_mem_copy(PMKIDCache[j].PMKID,
                    pmksa->pmkid,
                    CSR_RSN_PMKID_SIZE);
            hddLog(VOS_TRACE_LEVEL_FATAL, "%s: Reusing cache entry %d.",
                    __func__, j );
            dump_bssid(pmksa->bssid);
            dump_pmkid(halHandle, pmksa->pmkid);
            break;
        }
    }

    /*                                                               */
    if(j == MAX_PMKSAIDS_IN_CACHE) PMKIDCacheIndex=0;

    if (!BSSIDMatched)
    {
        //                                                                    
        vos_mem_copy(PMKIDCache[PMKIDCacheIndex].BSSID,
                pmksa->bssid, ETHER_ADDR_LEN);
        vos_mem_copy(PMKIDCache[PMKIDCacheIndex].PMKID,
                pmksa->pmkid,
                CSR_RSN_PMKID_SIZE);
        hddLog(VOS_TRACE_LEVEL_FATAL, "%s: Adding a new cache entry %d.",
                __func__, PMKIDCacheIndex );
        dump_bssid(pmksa->bssid);
        dump_pmkid(halHandle, pmksa->pmkid);
        //                                    
        //                                                                           
        if (PMKIDCacheIndex <= (MAX_PMKSAIDS_IN_CACHE-1)) PMKIDCacheIndex++; else PMKIDCacheIndex = 0;
    }


    //                                                                    
    //                                                                           
    //                      
    hddLog(VOS_TRACE_LEVEL_FATAL, "%s: Calling csrRoamSetPMKIDCache with %d cache entries.",
            __func__, PMKIDCacheIndex );
    //                                      
    result = sme_RoamSetPMKIDCache(halHandle,pAdapter->sessionId,
                                    PMKIDCache,
                                    PMKIDCacheIndex);
    return 0;
}



static int wlan_hdd_cfg80211_del_pmksa(struct wiphy *wiphy, struct net_device *dev,
             struct cfg80211_pmksa *pmksa)
{
    tANI_U32 j=0;
    hdd_adapter_t *pAdapter = WLAN_HDD_GET_PRIV_PTR(dev);
    tHalHandle halHandle;
    tANI_U8  BSSIDMatched = 0;
    tANI_U8 *pBSSId;
    hdd_context_t *pHddCtx;
    int status = 0;

    hddLog(VOS_TRACE_LEVEL_DEBUG, "%s: deleting PMKSA with PMKSA_ID %d .",
            __func__,pmksa->pmkid);

    /*                   */
    if (NULL == pAdapter)
    {
       hddLog(VOS_TRACE_LEVEL_ERROR, "%s: Invalid Adapter" ,__func__);
       return -EINVAL;
    }

    pHddCtx = WLAN_HDD_GET_CTX(pAdapter);
    status = wlan_hdd_validate_context(pHddCtx);

    if (0 != status)
    {
        VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                   "%s: HDD context is not valid", __func__);
        return status;
    }

    /*                  */
    halHandle = WLAN_HDD_GET_HAL_CTX(pAdapter);

    /*                                     */
    if (0 == PMKIDCacheIndex)
    {
        hddLog(VOS_TRACE_LEVEL_ERROR, "%s: Invalid entry to delete" ,
              __func__);
        return -EINVAL;
    }

    /*                                                    
                                 
     */
    for (j = 0; j<PMKIDCacheIndex; j++)
    {
          if (vos_mem_compare(PMKIDCache[j].BSSID,
                             pmksa->bssid,
                             WNI_CFG_BSSID_LEN))
          {
             /*                     */
             BSSIDMatched = 1;

             if (j<PMKIDCacheIndex-1)
             {
                 /*                                                                 */
                 vos_mem_copy(PMKIDCache[j].BSSID,
                              PMKIDCache[PMKIDCacheIndex-1].BSSID,
                              WNI_CFG_BSSID_LEN);
                 vos_mem_copy(PMKIDCache[j].PMKID,
                              PMKIDCache[PMKIDCacheIndex-1].PMKID,
                              CSR_RSN_PMKID_SIZE);
              }

             /*                                              */
             pBSSId =(tANI_U8 *)(PMKIDCache[PMKIDCacheIndex-1].BSSID);
             vos_mem_zero(PMKIDCache[PMKIDCacheIndex-1].BSSID, WNI_CFG_BSSID_LEN);
             vos_mem_zero(PMKIDCache[PMKIDCacheIndex-1].PMKID, CSR_RSN_PMKID_SIZE);

             /*                            */
             PMKIDCacheIndex--;

             /*                                  */
             if (eHAL_STATUS_SUCCESS !=
                 sme_RoamDelPMKIDfromCache(halHandle, pAdapter->sessionId, pmksa->bssid))
             {
                hddLog(VOS_TRACE_LEVEL_ERROR,"%s: cannot delete PMKSA %d CONTENT.",
                          __func__,PMKIDCacheIndex);
                status = -EINVAL;
             }

             dump_bssid(pmksa->bssid);
             dump_pmkid(halHandle,pmksa->pmkid);

             break;
          }
    }

    /*                                                       */
    if (j == MAX_PMKSAIDS_IN_CACHE && !BSSIDMatched)
    {
       hddLog(VOS_TRACE_LEVEL_FATAL, "%s: No such PMKSA entry existed %d.",
             __func__,pmksa->bssid);
       dump_bssid(pmksa->bssid);
       dump_pmkid(halHandle, pmksa->pmkid);
       return -EINVAL;
    }
   return status;
}



static int wlan_hdd_cfg80211_flush_pmksa(struct wiphy *wiphy, struct net_device *dev)
{
    tANI_U32 j=0;
    hdd_adapter_t *pAdapter = WLAN_HDD_GET_PRIV_PTR(dev);
    tHalHandle halHandle;
    hdd_context_t *pHddCtx;
    tANI_U8 *pBSSId;
    int status = 0;

    hddLog(VOS_TRACE_LEVEL_DEBUG, "%s: flushing PMKSA ",__func__);

    /*                   */
    if (NULL == pAdapter)
    {
        hddLog(VOS_TRACE_LEVEL_ERROR,
               "%s: Invalid Adapter" ,__func__);
       return -EINVAL;
    }

    pHddCtx = WLAN_HDD_GET_CTX(pAdapter);
    status = wlan_hdd_validate_context(pHddCtx);

    if (0 != status)
    {
       hddLog(VOS_TRACE_LEVEL_ERROR,
              "%s: HDD context is not valid", __func__);
       return status;
    }

    /*                  */
    halHandle = WLAN_HDD_GET_HAL_CTX(pAdapter);

    /*                                     */
    if (0 == PMKIDCacheIndex)
    {
       hddLog(VOS_TRACE_LEVEL_ERROR, FL("No entries to flush"));
       return -EINVAL;
    }

    /*                                */
    for (j = 0; j<PMKIDCacheIndex; j++)
    {
          /*                                        */
          pBSSId =(tANI_U8 *)(PMKIDCache[j].BSSID);
          vos_mem_zero(PMKIDCache[j].BSSID, WNI_CFG_BSSID_LEN);
          vos_mem_zero(PMKIDCache[j].PMKID, CSR_RSN_PMKID_SIZE);

          /*                       */
          if (eHAL_STATUS_SUCCESS !=
              sme_RoamDelPMKIDfromCache(halHandle, pAdapter->sessionId, pBSSId))
          {
             hddLog(VOS_TRACE_LEVEL_ERROR ,"%s cannot flush PMKIDCache %d.",
                    __func__,j);
             status = -EINVAL;
          }
      }

    PMKIDCacheIndex = 0;
    return status;
}
#endif

#if defined(WLAN_FEATURE_VOWIFI_11R) && defined(KERNEL_SUPPORT_11R_CFG80211)
static int wlan_hdd_cfg80211_update_ft_ies(struct wiphy *wiphy,
          struct net_device *dev, struct cfg80211_update_ft_ies_params *ftie)
{
    hdd_adapter_t *pAdapter = WLAN_HDD_GET_PRIV_PTR(dev);
    hdd_station_ctx_t *pHddStaCtx;

    if (NULL == pAdapter)
    {
        hddLog(VOS_TRACE_LEVEL_ERROR, "%s: Adapter is NULL\n", __func__);
        return -ENODEV;
    }

    pHddStaCtx = WLAN_HDD_GET_STATION_CTX_PTR(pAdapter);

    //                                              
    if (eConnectionState_Associated != pHddStaCtx->conn_info.connState)
    {
        hddLog(LOGE, FL("Called with Ie of length = %d when not associated\n"),
               ftie->ie_len);
        hddLog(LOGE, FL("Should be Re-assoc Req IEs\n"));
    }

#ifdef WLAN_FEATURE_VOWIFI_11R_DEBUG
    hddLog(LOGE, FL("%s called with Ie of length = %d\n"), __func__,
           ftie->ie_len);
#endif

    //                                
    sme_SetFTIEs( WLAN_HDD_GET_HAL_CTX(pAdapter), pAdapter->sessionId,
                  (const u8 *)ftie->ie,
                  ftie->ie_len);
    return 0;
}
#endif

#ifdef FEATURE_WLAN_SCAN_PNO

void hdd_cfg80211_sched_scan_done_callback(void *callbackContext,
                              tSirPrefNetworkFoundInd *pPrefNetworkFoundInd)
{
    int ret;
    hdd_adapter_t* pAdapter = (hdd_adapter_t*)callbackContext;
    hdd_context_t *pHddCtx;

    if (NULL == pAdapter)
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_FATAL,
                   "%s: HDD adapter is Null", __func__);
        return ;
    }

    pHddCtx = WLAN_HDD_GET_CTX(pAdapter);
    if (NULL == pHddCtx)
    {
        VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                "%s: HDD context is Null!!!", __func__);
        return ;
    }

    spin_lock(&pHddCtx->schedScan_lock);
    if (TRUE == pHddCtx->isWiphySuspended)
    {
        pHddCtx->isSchedScanUpdatePending = TRUE;
        spin_unlock(&pHddCtx->schedScan_lock);
        hddLog(VOS_TRACE_LEVEL_INFO,
               "%s: Update cfg80211 scan database after it resume", __func__);
        return ;
    }
    spin_unlock(&pHddCtx->schedScan_lock);

    ret = wlan_hdd_cfg80211_update_bss(pHddCtx->wiphy, pAdapter);

    if (0 > ret)
        hddLog(VOS_TRACE_LEVEL_INFO, "%s: NO SCAN result", __func__);

    cfg80211_sched_scan_results(pHddCtx->wiphy);
    VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO,
            "%s: cfg80211 scan result database updated", __func__);
}

/*
                                    
                                        
 */
static eHalStatus wlan_hdd_is_pno_allowed(hdd_adapter_t *pAdapter)
{
   hdd_adapter_list_node_t *pAdapterNode = NULL, *pNext = NULL;
   hdd_adapter_t *pTempAdapter = NULL;
   hdd_station_ctx_t *pStaCtx;
   hdd_context_t *pHddCtx = WLAN_HDD_GET_CTX(pAdapter);
   int status = 0;
   status = hdd_get_front_adapter(pHddCtx, &pAdapterNode);

   /*                                                          
                                                           
                           
    */
   while ((NULL != pAdapterNode) && (VOS_STATUS_SUCCESS == status))
   {
        pTempAdapter = pAdapterNode->pAdapter;
        pStaCtx = WLAN_HDD_GET_STATION_CTX_PTR(pTempAdapter);

        if (((WLAN_HDD_INFRA_STATION == pTempAdapter->device_mode)
          && (eConnectionState_NotConnected != pStaCtx->conn_info.connState))
          || (WLAN_HDD_P2P_CLIENT == pTempAdapter->device_mode)
          || (WLAN_HDD_P2P_GO == pTempAdapter->device_mode)
          || (WLAN_HDD_SOFTAP == pTempAdapter->device_mode)
          )
        {
            return eHAL_STATUS_FAILURE;
        }
        status = hdd_get_next_adapter ( pHddCtx, pAdapterNode, &pNext );
        pAdapterNode = pNext;
   }
   return eHAL_STATUS_SUCCESS;
}

void hdd_cfg80211_sched_scan_start_status_cb(void *callbackContext, VOS_STATUS status)
{
    hdd_adapter_t *pAdapter = callbackContext;
    hdd_context_t *pHddCtx;

    if ((NULL == pAdapter) || (WLAN_HDD_ADAPTER_MAGIC != pAdapter->magic))
    {
        VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                  FL("Invalid adapter or adapter has invalid magic"));
        return;
    }

    pHddCtx = WLAN_HDD_GET_CTX(pAdapter);
    if (0 != wlan_hdd_validate_context(pHddCtx))
    {
        VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                  FL("HDD context is not valid"));
        return;
    }

    VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO,
              FL("PNO enable response status = %d"), status);

    pAdapter->pno_req_status = (status == VOS_STATUS_SUCCESS) ? 0 : -EBUSY;
    complete(&pAdapter->pno_comp_var);
}

/*
                                               
                             
 */
static int wlan_hdd_cfg80211_sched_scan_start(struct wiphy *wiphy,
          struct net_device *dev, struct cfg80211_sched_scan_request *request)
{
    hdd_adapter_t *pAdapter = WLAN_HDD_GET_PRIV_PTR(dev);
    tpSirPNOScanReq pPnoRequest = NULL;
    hdd_context_t *pHddCtx;
    tHalHandle hHal;
    v_U32_t i, indx, num_ch, tempInterval;
    u8 valid_ch[WNI_CFG_VALID_CHANNEL_LIST_LEN];
    u8 channels_allowed[WNI_CFG_VALID_CHANNEL_LIST_LEN];
    v_U32_t num_channels_allowed = WNI_CFG_VALID_CHANNEL_LIST_LEN;
    eHalStatus status = eHAL_STATUS_FAILURE;
    int ret = 0;

    if (NULL == pAdapter)
    {
        VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_FATAL,
                  "%s: HDD adapter is Null", __func__);
        return -ENODEV;
    }

    pHddCtx = WLAN_HDD_GET_CTX(pAdapter);
    ret = wlan_hdd_validate_context(pHddCtx);

    if (0 != ret)
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                   "%s: HDD context is not valid", __func__);
        return -EINVAL;
    }

    hHal = WLAN_HDD_GET_HAL_CTX(pAdapter);
    if (NULL == hHal)
    {
        VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                  "%s: HAL context  is Null!!!", __func__);
        return -EINVAL;
    }

    if (eHAL_STATUS_SUCCESS != wlan_hdd_is_pno_allowed(pAdapter))
    {
        VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_WARN,
                  FL("Cannot handle sched_scan"));
        return -EBUSY;
    }

    pPnoRequest = (tpSirPNOScanReq) vos_mem_malloc(sizeof (tSirPNOScanReq));
    if (NULL == pPnoRequest)
    {
        VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_FATAL,
                  "%s: vos_mem_malloc failed", __func__);
        return -ENOMEM;
    }

    memset(pPnoRequest, 0, sizeof (tSirPNOScanReq));
    pPnoRequest->enable = 1; /*           */
    pPnoRequest->ucNetworksCount = request->n_match_sets;

    if (( !pPnoRequest->ucNetworksCount ) ||
        ( pPnoRequest->ucNetworksCount > SIR_PNO_MAX_SUPP_NETWORKS ))
    {
        VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                  "Network input is not correct");
        ret = -EINVAL;
        goto error;
    }

    if ( SIR_PNO_MAX_NETW_CHANNELS_EX < request->n_channels )
    {
        VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                  "Incorrect number of channels");
        ret = -EINVAL;
        goto error;
    }

    /*                                            
                                    */
    if (0 != ccmCfgGetStr(hHal, WNI_CFG_VALID_CHANNEL_LIST,
            channels_allowed, &num_channels_allowed))
    {
        VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                "%s: failed to get valid channel list", __func__);
        ret = -EINVAL;
        goto error;
    }
    /*                                                    */
    num_ch = 0;
    for (i = 0; i < request->n_channels; i++)
    {
        for (indx = 0; indx < num_channels_allowed; indx++)
        {
            if (request->channels[i]->hw_value == channels_allowed[indx])
            {
                valid_ch[num_ch++] = request->channels[i]->hw_value;
                break ;
            }
        }
    }

    /*                             */
    for (i = 0; i < pPnoRequest->ucNetworksCount; i++)
    {
        pPnoRequest->aNetworks[i].ssId.length =
               request->match_sets[i].ssid.ssid_len;

        if (( 0 == pPnoRequest->aNetworks[i].ssId.length ) ||
            ( pPnoRequest->aNetworks[i].ssId.length > 32 ) )
        {
            VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                      "SSID Len %d is not correct for network %d",
                      pPnoRequest->aNetworks[i].ssId.length, i);
            ret = -EINVAL;
            goto error;
        }

        memcpy(pPnoRequest->aNetworks[i].ssId.ssId,
               request->match_sets[i].ssid.ssid,
               request->match_sets[i].ssid.ssid_len);
        pPnoRequest->aNetworks[i].authentication = 0; /*              */
        pPnoRequest->aNetworks[i].encryption     = 0; /*       */
        pPnoRequest->aNetworks[i].bcastNetwType  = 0; /*              */

        /*                                           */
        memcpy(pPnoRequest->aNetworks[i].aChannels, valid_ch, num_ch);
        pPnoRequest->aNetworks[i].ucChannelCount = num_ch;

        pPnoRequest->aNetworks[i].rssiThreshold = 0; //             
    }

    VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO,
              "request->ie_len = %d", request->ie_len);
    if ((0 < request->ie_len) && (NULL != request->ie))
    {
        pPnoRequest->us24GProbeTemplateLen = request->ie_len;
        memcpy(&pPnoRequest->p24GProbeTemplate, request->ie,
                pPnoRequest->us24GProbeTemplateLen);

        pPnoRequest->us5GProbeTemplateLen = request->ie_len;
        memcpy(&pPnoRequest->p5GProbeTemplate, request->ie,
                pPnoRequest->us5GProbeTemplateLen);
    }

    /*                                                         
                                                                             
                                                                        
                                                               
                                                                  
                                                                       
                                                                   
                            
     */
    if (0 == pHddCtx->cfg_ini->configPNOScanTimerRepeatValue)
        pPnoRequest->scanTimers.ucScanTimersCount = HDD_PNO_SCAN_TIMERS_SET_ONE;
    else
        pPnoRequest->scanTimers.ucScanTimersCount =
                                               HDD_PNO_SCAN_TIMERS_SET_MULTIPLE;

    tempInterval = (request->interval)/1000;
    VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO,
              "Base scan interval = %d PNOScanTimerRepeatValue = %d",
              tempInterval, pHddCtx->cfg_ini->configPNOScanTimerRepeatValue);
    for ( i = 0; i < pPnoRequest->scanTimers.ucScanTimersCount; i++)
    {
        pPnoRequest->scanTimers.aTimerValues[i].uTimerRepeat =
                                 pHddCtx->cfg_ini->configPNOScanTimerRepeatValue;
        pPnoRequest->scanTimers.aTimerValues[i].uTimerValue = tempInterval;
        tempInterval *= 2;
    }
    //                                     
    pPnoRequest->scanTimers.aTimerValues[i-1].uTimerRepeat = 0;

    pPnoRequest->modePNO = SIR_PNO_MODE_IMMEDIATE;

    INIT_COMPLETION(pAdapter->pno_comp_var);
    pPnoRequest->statusCallback = hdd_cfg80211_sched_scan_start_status_cb;
    pPnoRequest->callbackContext = pAdapter;
    pAdapter->pno_req_status = 0;

    status = sme_SetPreferredNetworkList(WLAN_HDD_GET_HAL_CTX(pAdapter),
                              pPnoRequest, pAdapter->sessionId,
                              hdd_cfg80211_sched_scan_done_callback, pAdapter);
    if (eHAL_STATUS_SUCCESS != status)
    {
        VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                  "Failed to enable PNO");
        ret = -EINVAL;
        goto error;
    }

    ret = wait_for_completion_timeout(
                 &pAdapter->pno_comp_var,
                  msecs_to_jiffies(WLAN_WAIT_TIME_PNO));
    if (0 >= ret)
    {
        //                                                     
        //                                     
        //                                                       
        //                                                   
        VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                  FL("Timed out waiting for PNO to be Enabled"));
        ret = 0;
        goto error;
    }

    ret = pAdapter->pno_req_status;

error:
    VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO,
              FL("PNO scanRequest offloaded ret = %d"), ret);
    vos_mem_free(pPnoRequest);
    return ret;
}

/*
                                              
                              
 */
static int wlan_hdd_cfg80211_sched_scan_stop(struct wiphy *wiphy,
          struct net_device *dev)
{
    eHalStatus status = eHAL_STATUS_FAILURE;
    hdd_adapter_t *pAdapter = WLAN_HDD_GET_PRIV_PTR(dev);
    hdd_context_t *pHddCtx;
    tHalHandle hHal;
    tpSirPNOScanReq pPnoRequest = NULL;
    int ret = 0;

    ENTER();

    if (NULL == pAdapter)
    {
        VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_FATAL,
                  "%s: HDD adapter is Null", __func__);
        return -ENODEV;
    }

    pHddCtx = WLAN_HDD_GET_CTX(pAdapter);

    if (NULL == pHddCtx)
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                "%s: HDD context is Null", __func__);
        return -ENODEV;
    }

    /*                                                      
                                                                        
                                                                          
                                                                               
                                                                              
                                                                             
                                                                               
                                           
     */
    if (pHddCtx->isLogpInProgress)
    {
        VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                  "%s: LOGP in Progress. Ignore!!!", __func__);
        return ret;
    }

    if (WLAN_HDD_IS_LOAD_UNLOAD_IN_PROGRESS(pHddCtx))
    {
        VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                  "%s: Unloading/Loading in Progress. Ignore!!!", __func__);
        return ret;
    }

    hHal = WLAN_HDD_GET_HAL_CTX(pAdapter);
    if (NULL == hHal)
    {
        VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                "%s: HAL context  is Null!!!", __func__);
        return -EINVAL;
    }

    pPnoRequest = (tpSirPNOScanReq) vos_mem_malloc(sizeof (tSirPNOScanReq));
    if (NULL == pPnoRequest)
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_FATAL,
                   "%s: vos_mem_malloc failed", __func__);
        return -ENOMEM;
    }

    memset(pPnoRequest, 0, sizeof (tSirPNOScanReq));
    pPnoRequest->enable = 0; /*             */
    pPnoRequest->ucNetworksCount = 0;

    status = sme_SetPreferredNetworkList(hHal, pPnoRequest,
                                pAdapter->sessionId,
                                NULL, pAdapter);
    if (eHAL_STATUS_SUCCESS != status)
    {
        VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                  "Failed to disabled PNO");
        ret = -EINVAL;
        goto error;
    }

error:
    VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO,
                   FL("PNO scan disabled ret = %d"), ret);
    vos_mem_free(pPnoRequest);

    EXIT();
    return ret;
}

#endif /*                     */


#ifdef FEATURE_WLAN_TDLS
static int wlan_hdd_cfg80211_tdls_mgmt(struct wiphy *wiphy, struct net_device *dev,
                     u8 *peer, u8 action_code,  u8 dialog_token,
                                                 u16 status_code, const u8 *buf, size_t len)
{

    hdd_adapter_t *pAdapter = WLAN_HDD_GET_PRIV_PTR(dev);
    hdd_context_t *pHddCtx = wiphy_priv(wiphy);
    u8 peerMac[6];
    VOS_STATUS status;
    int max_sta_failed = 0;
    int responder;
    long rc;

    if (NULL == pHddCtx || NULL == pHddCtx->cfg_ini)
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                "Invalid arguments");
        return -EINVAL;
    }

    if (pHddCtx->isLogpInProgress)
    {
        VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                "%s:LOGP in Progress. Ignore!!!", __func__);
        wlan_hdd_tdls_set_link_status(pAdapter, peer, eTDLS_LINK_IDLE);
        return -EBUSY;
    }

    if (eTDLS_SUPPORT_NOT_ENABLED == pHddCtx->tdls_mode)
    {
         VOS_TRACE( VOS_MODULE_ID_HDD, TDLS_LOG_LEVEL,
                    "%s: TDLS mode is disabled OR not enabled in FW."
                    MAC_ADDRESS_STR " action %d declined.",
                    __func__, MAC_ADDR_ARRAY(peer), action_code);
        return -ENOTSUPP;
    }

    /*                                                                       */
    if (SIR_MAC_TDLS_TEARDOWN != action_code)
    {
       /*                                                       */
        if (eTDLS_SUPPORT_DISABLED == pHddCtx->tdls_mode)
        {
             VOS_TRACE( VOS_MODULE_ID_HDD, TDLS_LOG_LEVEL,
                        "%s: " MAC_ADDRESS_STR
                        " TDLS mode is disabled. action %d declined.",
                        __func__, MAC_ADDR_ARRAY(peer), action_code);

        return -ENOTSUPP;
        }
    }

    if (WLAN_IS_TDLS_SETUP_ACTION(action_code))
    {
        if (NULL != wlan_hdd_tdls_is_progress(pHddCtx, peer, TRUE, TRUE))
        {
            VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                       "%s: " MAC_ADDRESS_STR
                       " TDLS setup is ongoing. action %d declined.",
                       __func__, MAC_ADDR_ARRAY(peer), action_code);
            return -EPERM;
        }
    }

    if (SIR_MAC_TDLS_SETUP_REQ == action_code ||
        SIR_MAC_TDLS_SETUP_RSP == action_code )
    {
        if (HDD_MAX_NUM_TDLS_STA <= wlan_hdd_tdlsConnectedPeers(pAdapter))
        {
            /*                                                       
                                                                          
                                                       
                                                         */
            if (SIR_MAC_TDLS_SETUP_REQ == action_code)
            {
                VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                           "%s: " MAC_ADDRESS_STR
                           " TDLS Max peer already connected. action %d declined.",
                           __func__, MAC_ADDR_ARRAY(peer), action_code);
                goto error;
            }
            else
            {
                /*                                                             
                                            */
                status_code = eSIR_MAC_UNSPEC_FAILURE_STATUS;
                VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                           "%s: " MAC_ADDRESS_STR
                           " TDLS Max peer already connected send response status %d",
                           __func__, MAC_ADDR_ARRAY(peer), status_code);
                max_sta_failed = -EPERM;
                /*                                                    
                     */
            }
        }
        else
        {
            hddTdlsPeer_t *pTdlsPeer;
            pTdlsPeer = wlan_hdd_tdls_find_peer(pAdapter, peer);
            if (pTdlsPeer && TDLS_IS_CONNECTED(pTdlsPeer))
            {
                VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                        "%s:" MAC_ADDRESS_STR " already connected. action %d declined.",
                        __func__, MAC_ADDR_ARRAY(peer), action_code);
                return -EPERM;
            }
        }
    }
    vos_mem_copy(peerMac, peer, 6);

#ifdef WLAN_FEATURE_TDLS_DEBUG
    VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
               "%s: " MAC_ADDRESS_STR " action %d, dialog_token %d status %d, len = %d",
               "tdls_mgmt", MAC_ADDR_ARRAY(peer),
               action_code, dialog_token, status_code, len);
#endif

    /*                                                         */
    responder = 0;
    if (SIR_MAC_TDLS_TEARDOWN == action_code)
    {

       hddTdlsPeer_t *pTdlsPeer;
       pTdlsPeer = wlan_hdd_tdls_find_peer(pAdapter, peerMac);

       if(pTdlsPeer && TDLS_IS_CONNECTED(pTdlsPeer))
            responder = pTdlsPeer->is_responder;
       else
       {
           VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                    "%s: " MAC_ADDRESS_STR " peer doesn't exist or not connected %d dialog_token %d status %d, len = %d",
                    __func__, MAC_ADDR_ARRAY(peer), (NULL == pTdlsPeer) ? -1 : pTdlsPeer->link_status,
                     dialog_token, status_code, len);
           return -EPERM;
       }
    }

    /*                                                     
                                                 */
    if ((SIR_MAC_TDLS_SETUP_RSP == action_code) ||
        (SIR_MAC_TDLS_DIS_RSP == action_code) ||
        (SIR_MAC_TDLS_DIS_REQ == action_code))
    {
        if (TRUE == sme_IsPmcBmps(WLAN_HDD_GET_HAL_CTX(pAdapter)))
        {
            VOS_TRACE( VOS_MODULE_ID_HDD, TDLS_LOG_LEVEL,
                       "%s: Sending frame action_code %u.Disable BMPS", __func__, action_code);
            hdd_disable_bmps_imps(pHddCtx, WLAN_HDD_INFRA_STATION);
        }
        if (SIR_MAC_TDLS_DIS_REQ != action_code)
            wlan_hdd_tdls_set_cap(pAdapter, peerMac, eTDLS_CAP_SUPPORTED);
    }

    /*                                                        */
    if (TDLS_CTX_MAGIC == pAdapter->mgmtTxCompletionStatus)
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
            "%s: " MAC_ADDRESS_STR " action %d couldn't sent, as one is pending. return EBUSY\n",
            __func__, MAC_ADDR_ARRAY(peer), action_code);
        return -EBUSY;
    }

    pAdapter->mgmtTxCompletionStatus = TDLS_CTX_MAGIC;
    INIT_COMPLETION(pAdapter->tdls_mgmt_comp);

    status = sme_SendTdlsMgmtFrame(WLAN_HDD_GET_HAL_CTX(pAdapter), pAdapter->sessionId,
            peerMac, action_code, dialog_token, status_code, (tANI_U8 *)buf, len, responder);

    if (VOS_STATUS_SUCCESS != status)
    {
        VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                "%s: sme_SendTdlsMgmtFrame failed!", __func__);
        pAdapter->mgmtTxCompletionStatus = FALSE;
        wlan_hdd_tdls_check_bmps(pAdapter);
        goto error;
    }

    rc = wait_for_completion_interruptible_timeout(&pAdapter->tdls_mgmt_comp,
                                                        msecs_to_jiffies(WAIT_TIME_TDLS_MGMT));

    if ((rc <= 0) || (TRUE != pAdapter->mgmtTxCompletionStatus))
    {
        VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                  "%s: Mgmt Tx Completion failed status %ld TxCompletion %lu",
                  __func__, rc, pAdapter->mgmtTxCompletionStatus);
        pAdapter->mgmtTxCompletionStatus = FALSE;
        wlan_hdd_tdls_check_bmps(pAdapter);
        goto error;
    }

    if (max_sta_failed)
    {
      wlan_hdd_tdls_check_bmps(pAdapter);
      return max_sta_failed;
    }

    if (SIR_MAC_TDLS_SETUP_RSP == action_code)
    {
        wlan_hdd_tdls_set_responder(pAdapter, peerMac, TRUE);
    }
    else if (SIR_MAC_TDLS_SETUP_CNF == action_code)
    {
        wlan_hdd_tdls_set_responder(pAdapter, peerMac, FALSE);
    }

    return 0;
error:
    /*                                                            
                                                                    
                                                                      
                                                  
    */
    if(max_sta_failed == 0 &&
           (WLAN_IS_TDLS_SETUP_ACTION(action_code)))
            wlan_hdd_tdls_set_link_status(pAdapter, peerMac, eTDLS_LINK_IDLE);
    return -EPERM;
}

static int wlan_hdd_cfg80211_tdls_oper(struct wiphy *wiphy, struct net_device *dev,
                 u8 *peer, enum nl80211_tdls_operation oper)
{
    hdd_adapter_t *pAdapter = WLAN_HDD_GET_PRIV_PTR(dev);
    hdd_context_t *pHddCtx = wiphy_priv(wiphy);
    int status;
#ifdef FEATURE_WLAN_TDLS_OXYGEN_DISAPPEAR_AP
    hdd_station_ctx_t *pHddStaCtx = WLAN_HDD_GET_STATION_CTX_PTR(pAdapter);
    tANI_U8 staIdx;
#endif
#ifdef WLAN_FEATURE_TDLS_DEBUG
    const char *tdls_oper_str[]= {
        "NL80211_TDLS_DISCOVERY_REQ",
        "NL80211_TDLS_SETUP",
        "NL80211_TDLS_TEARDOWN",
        "NL80211_TDLS_ENABLE_LINK",
        "NL80211_TDLS_DISABLE_LINK",
        "NL80211_TDLS_UNKNOWN_OPER"};
#endif
    hddTdlsPeer_t *pTdlsPeer;

    if ( NULL == peer )
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                   "%s: Invalid arguments", __func__);
        return -EINVAL;
    }

    status = wlan_hdd_validate_context(pHddCtx);

    if (0 != status)
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                   "%s: HDD context is not valid", __func__);
        return status;
    }

    pTdlsPeer = wlan_hdd_tdls_find_peer(pAdapter, peer);

    if ( NULL == pTdlsPeer ) {
        hddLog(VOS_TRACE_LEVEL_ERROR, "%s: " MAC_ADDRESS_STR " (oper %d) not exsting. ignored",
               __func__, MAC_ADDR_ARRAY(peer), (int)oper);
        return -EINVAL;
    }

#ifdef WLAN_FEATURE_TDLS_DEBUG
    if((int)oper > 4)
        oper = 5;

    VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
               "%s: " MAC_ADDRESS_STR " link_status %d (%s) ", "tdls_oper",
               MAC_ADDR_ARRAY(peer), pTdlsPeer->link_status,
               tdls_oper_str[(int)oper]);
#endif

    if( FALSE == pHddCtx->cfg_ini->fEnableTDLSSupport ||
        FALSE == sme_IsFeatureSupportedByFW(TDLS))
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                "TDLS Disabled in INI OR not enabled in FW. "
                "Cannot process TDLS commands");
        return -ENOTSUPP;
    }

    switch (oper) {
        case NL80211_TDLS_ENABLE_LINK:
            {
                VOS_STATUS status;
                tCsrTdlsLinkEstablishParams tdlsLinkEstablishParams;

                if (!TDLS_STA_INDEX_VALID(pTdlsPeer->staId))
                {
                    hddLog(VOS_TRACE_LEVEL_ERROR, "%s: Invalid Staion Index %u "
                           MAC_ADDRESS_STR " failed",
                           __func__, pTdlsPeer->staId, MAC_ADDR_ARRAY(peer));
                    return -EINVAL;
                }

                if (eTDLS_LINK_CONNECTED != pTdlsPeer->link_status)
                {
                    long ret;
                    if (IS_ADVANCE_TDLS_ENABLE) {

                        if (0 != wlan_hdd_tdls_get_link_establish_params(
                                   pAdapter, peer,&tdlsLinkEstablishParams)) {
                            return -EINVAL;
                        }
                        INIT_COMPLETION(pAdapter->tdls_link_establish_req_comp);

                        sme_SendTdlsLinkEstablishParams(WLAN_HDD_GET_HAL_CTX(pAdapter),
                            pAdapter->sessionId, peer, &tdlsLinkEstablishParams);
                        /*                                                      
                                                                                        
                                        
                         */
                        ret = wait_for_completion_interruptible_timeout(
                                &pAdapter->tdls_link_establish_req_comp,
                                msecs_to_jiffies(WAIT_TIME_TDLS_LINK_ESTABLISH_REQ));
                        if (ret <= 0)
                        {
                            VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                                      "%s: Link Establish Request Faled Status %ld",
                                      __func__, ret);
                            return -EINVAL;
                        }
                    }
                    wlan_hdd_tdls_set_peer_link_status(pTdlsPeer, eTDLS_LINK_CONNECTED);
                    /*                                 */
                    status = WLANTL_ChangeSTAState( pHddCtx->pvosContext,
                                                    pTdlsPeer->staId,
                                                    WLANTL_STA_AUTHENTICATED);
                    if (VOS_STATUS_SUCCESS == status)
                    {
                        if (pTdlsPeer->is_responder == 0)
                        {
                            v_U8_t staId = (v_U8_t)pTdlsPeer->staId;

                            wlan_hdd_tdls_timer_restart(pAdapter,
                                                        &pTdlsPeer->initiatorWaitTimeoutTimer,
                                                       WAIT_TIME_TDLS_INITIATOR);
                            /*                                                              
                                                                               */
                            WLANTL_SuspendDataTx( (WLAN_HDD_GET_CTX(pAdapter))->pvosContext,
                                                   &staId, NULL);
                        }
                        wlan_hdd_tdls_increment_peer_count(pAdapter);
                    }
                    wlan_hdd_tdls_check_bmps(pAdapter);

                    /*                                                                    */
                    if ((TRUE == pHddCtx->cfg_ini->fEnableTDLSBufferSta)
                        || pHddCtx->cfg_ini->fTDLSUapsdMask )
                    {
                        int ac;
                        uint8 ucAc[4] = { WLANTL_AC_VO,
                                          WLANTL_AC_VI,
                                          WLANTL_AC_BK,
                                          WLANTL_AC_BE };
                        uint8 tlTid[4] = { 7, 5, 2, 3 } ;
                        for(ac=0; ac < 4; ac++)
                        {
                            status = WLANTL_EnableUAPSDForAC( (WLAN_HDD_GET_CTX(pAdapter))->pvosContext,
                                                               pTdlsPeer->staId, ucAc[ac],
                                                               tlTid[ac], tlTid[ac], 0, 0,
                                                               WLANTL_BI_DIR );
                        }
                    }
                }

            }
            break;
        case NL80211_TDLS_DISABLE_LINK:
            {
                if(TDLS_STA_INDEX_VALID(pTdlsPeer->staId))
                {
                    long status;

                    INIT_COMPLETION(pAdapter->tdls_del_station_comp);

                    sme_DeleteTdlsPeerSta( WLAN_HDD_GET_HAL_CTX(pAdapter),
                            pAdapter->sessionId, peer );

                    status = wait_for_completion_interruptible_timeout(&pAdapter->tdls_del_station_comp,
                              msecs_to_jiffies(WAIT_TIME_TDLS_DEL_STA));
                    if (status <= 0)
                    {
                        wlan_hdd_tdls_set_peer_link_status(pTdlsPeer, eTDLS_LINK_IDLE);
                        VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                                  "%s: Del station failed status %ld",
                                  __func__, status);
                        return -EPERM;
                    }
                    wlan_hdd_tdls_set_peer_link_status(pTdlsPeer, eTDLS_LINK_IDLE);
                }
                else
                {
                    VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                              "%s: TDLS Peer Station doesn't exist.", __func__);
                }
#ifdef FEATURE_WLAN_TDLS_OXYGEN_DISAPPEAR_AP
                if (pHddTdlsCtx->defer_link_lost_indication)
                {
                    if (( TRUE == pHddCtx->cfg_ini->fEnableTDLSOxygenSupport ) &&
                        (wlan_hdd_tdlsConnectedPeers(pAdapter) == 0))
                    {
                        status = wlan_hdd_disconnect(pAdapter, eCSR_DISCONNECT_REASON_UNSPECIFIED);
                        if ( 0 != status)
                        {
                            hddLog(VOS_TRACE_LEVEL_ERROR,
                                   "%s wlan_hdd_disconnect failure, returned %d \n",
                                   __func__, (int)status );
                            return -EINVAL;
                        }
                    }
                }
#endif
            }
            break;
        case NL80211_TDLS_TEARDOWN:
        case NL80211_TDLS_SETUP:
        case NL80211_TDLS_DISCOVERY_REQ:
            /*                                                     */
            return -ENOTSUPP;
        default:
            return -ENOTSUPP;
    }
    return 0;
}

int wlan_hdd_cfg80211_send_tdls_discover_req(struct wiphy *wiphy,
                            struct net_device *dev, u8 *peer)
{
    hddLog(VOS_TRACE_LEVEL_INFO,
           "tdls send discover req: "MAC_ADDRESS_STR,
           MAC_ADDR_ARRAY(peer));

    return wlan_hdd_cfg80211_tdls_mgmt(wiphy, dev, peer,
                            WLAN_TDLS_DISCOVERY_REQUEST, 1, 0, NULL, 0);
}
#endif

#ifdef WLAN_FEATURE_GTK_OFFLOAD
/*
                                                           
                                                       
                   
 */
void wlan_hdd_cfg80211_update_replayCounterCallback(void *callbackContext,
                            tpSirGtkOffloadGetInfoRspParams pGtkOffloadGetInfoRsp)
{

    hdd_adapter_t *pAdapter = (hdd_adapter_t *)callbackContext;
    tANI_U8 tempReplayCounter[8];
    hdd_station_ctx_t *pHddStaCtx;

    ENTER();

    if (NULL == pAdapter)
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_FATAL,
                   "%s: HDD adapter is Null", __func__);
        return ;
    }

    if (NULL == pGtkOffloadGetInfoRsp)
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                   "%s: pGtkOffloadGetInfoRsp is Null", __func__);
        return ;
    }

    if (VOS_STATUS_SUCCESS != pGtkOffloadGetInfoRsp->ulStatus)
    {
        VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                "%s: wlan Failed to get replay counter value",
                __func__);
        return ;
    }

    pHddStaCtx = WLAN_HDD_GET_STATION_CTX_PTR(pAdapter);
    /*                       */
    pHddStaCtx->gtkOffloadReqParams.ullKeyReplayCounter =
                                   pGtkOffloadGetInfoRsp->ullKeyReplayCounter;

    {
        /*                                                    
                                     
         */
        int i;
        tANI_U8 *p = (tANI_U8 *)&pGtkOffloadGetInfoRsp->ullKeyReplayCounter;

        for (i = 0; i < 8; i++)
        {
            tempReplayCounter[7-i] = (tANI_U8)p[i];
        }
    }

    /*                             */
    cfg80211_gtk_rekey_notify(pAdapter->dev, pGtkOffloadGetInfoRsp->bssId,
          tempReplayCounter, GFP_KERNEL);
}

/*
                                             
                                                                     
 */
int wlan_hdd_cfg80211_set_rekey_data(struct wiphy *wiphy, struct net_device *dev,
                                     struct cfg80211_gtk_rekey_data *data)
{
    hdd_adapter_t *pAdapter = WLAN_HDD_GET_PRIV_PTR(dev);
    hdd_context_t *pHddCtx = wiphy_priv(wiphy);
    hdd_station_ctx_t *pHddStaCtx;
    tHalHandle hHal;
    int result;
    tSirGtkOffloadParams hddGtkOffloadReqParams;
    eHalStatus status = eHAL_STATUS_FAILURE;

    ENTER();

    if (NULL == pAdapter)
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_FATAL,
                   "%s: HDD adapter is Null", __func__);
        return -ENODEV;
    }

    result = wlan_hdd_validate_context(pHddCtx);

    if (0 != result)
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                   "%s: HDD context is not valid", __func__);
        return result;
    }

    pHddStaCtx = WLAN_HDD_GET_STATION_CTX_PTR(pAdapter);
    hHal = WLAN_HDD_GET_HAL_CTX(pAdapter);
    if (NULL == hHal)
    {
        VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                "%s: HAL context  is Null!!!", __func__);
        return -EAGAIN;
    }

    pHddStaCtx->gtkOffloadReqParams.ulFlags = GTK_OFFLOAD_ENABLE;
    memcpy(pHddStaCtx->gtkOffloadReqParams.aKCK, data->kck, NL80211_KCK_LEN);
    memcpy(pHddStaCtx->gtkOffloadReqParams.aKEK, data->kek, NL80211_KEK_LEN);
    memcpy(pHddStaCtx->gtkOffloadReqParams.bssId, &pHddStaCtx->conn_info.bssId,
          WNI_CFG_BSSID_LEN);
    {
        /*                                                
                                        
         */
        tANI_U8 *p =
              (tANI_U8 *)&pHddStaCtx->gtkOffloadReqParams.ullKeyReplayCounter;
        int i;

        for (i = 0; i < 8; i++)
        {
            p[7-i] = data->replay_ctr[i];
        }
    }

    if (TRUE == pHddCtx->hdd_wlan_suspended)
    {
        /*                                                             */
        memcpy(&hddGtkOffloadReqParams, &pHddStaCtx->gtkOffloadReqParams,
              sizeof (tSirGtkOffloadParams));
        status = sme_SetGTKOffload(hHal, &hddGtkOffloadReqParams,
                       pAdapter->sessionId);

        if (eHAL_STATUS_SUCCESS != status)
        {
            VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                    "%s: sme_SetGTKOffload failed, returned %d",
                    __func__, status);
            return status;
        }
        VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO,
                "%s: sme_SetGTKOffload successfull", __func__);
    }
    else
    {
        VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO,
                "%s: wlan not suspended GTKOffload request is stored",
                __func__);
    }

    return eHAL_STATUS_SUCCESS;
}
#endif /*                        */

/*
                                          
                                                     
 */
static int wlan_hdd_cfg80211_set_mac_acl(struct wiphy *wiphy,
                struct net_device *dev, const struct cfg80211_acl_data *params)
{
    int i;
    hdd_adapter_t *pAdapter =  WLAN_HDD_GET_PRIV_PTR(dev);
    hdd_hostapd_state_t *pHostapdState;
    tsap_Config_t *pConfig;
    v_CONTEXT_t pVosContext = NULL;
    hdd_context_t *pHddCtx;
    int status;

    ENTER();

    if (NULL == pAdapter)
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_FATAL,
                   "%s: HDD adapter is Null", __func__);
        return -ENODEV;
    }

    if (NULL == params)
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_FATAL,
                   "%s: params is Null", __func__);
        return -EINVAL;
    }

    pHddCtx = WLAN_HDD_GET_CTX(pAdapter);
    status = wlan_hdd_validate_context(pHddCtx);

    if (0 != status)
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                   "%s: HDD context is not valid", __func__);
        return status;
    }

    pVosContext = pHddCtx->pvosContext;
    pHostapdState = WLAN_HDD_GET_HOSTAP_STATE_PTR(pAdapter);

    if (NULL == pHostapdState)
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_FATAL,
                   "%s: pHostapdState is Null", __func__);
        return -EINVAL;
    }

    VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO,"acl policy: = %d"
             "no acl entries = %d", params->acl_policy, params->n_acl_entries);

    if (WLAN_HDD_SOFTAP == pAdapter->device_mode)
    {
        pConfig = &pAdapter->sessionCtx.ap.sapConfig;

        /*               */
        pConfig->num_accept_mac = 0;
        pConfig->num_deny_mac = 0;

        /* 
                                
                                                                            
                                         
                                                                           
                                           
         */
        if (NL80211_ACL_POLICY_DENY_UNLESS_LISTED == params->acl_policy)
        {
            pConfig->SapMacaddr_acl = eSAP_DENY_UNLESS_ACCEPTED;
        }
        else if (NL80211_ACL_POLICY_ACCEPT_UNLESS_LISTED == params->acl_policy)
        {
            pConfig->SapMacaddr_acl = eSAP_ACCEPT_UNLESS_DENIED;
        }
        else
        {
            VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                    "%s:Acl Policy : %d is not supported",
                                            __func__, params->acl_policy);
            return -ENOTSUPP;
        }

        if (eSAP_DENY_UNLESS_ACCEPTED == pConfig->SapMacaddr_acl)
        {
            pConfig->num_accept_mac = params->n_acl_entries;
            for (i = 0; i < params->n_acl_entries; i++)
            {
                VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO,
                    "** Add ACL MAC entry %i in WhiletList :"
                                MAC_ADDRESS_STR, i,
                                MAC_ADDR_ARRAY(params->mac_addrs[i].addr));

                vos_mem_copy(&pConfig->accept_mac[i], params->mac_addrs[i].addr,
                                                             sizeof(qcmacaddr));
            }
        }
        else if (eSAP_ACCEPT_UNLESS_DENIED == pConfig->SapMacaddr_acl)
        {
            pConfig->num_deny_mac = params->n_acl_entries;
            for (i = 0; i < params->n_acl_entries; i++)
            {
                VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO,
                    "** Add ACL MAC entry %i in BlackList :"
                                MAC_ADDRESS_STR, i,
                                MAC_ADDR_ARRAY(params->mac_addrs[i].addr));

                vos_mem_copy(&pConfig->deny_mac[i], params->mac_addrs[i].addr,
                                                           sizeof(qcmacaddr));
            }
        }

        if (VOS_STATUS_SUCCESS != WLANSAP_SetMacACL(pVosContext, pConfig))
        {
            VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                       "%s: SAP Set Mac Acl fail", __func__);
            return -EINVAL;
        }
    }
    else
    {
        VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                   "%s: Invalid device_mode = %d",
                                 __func__, pAdapter->device_mode);
        return -EINVAL;
    }

    return 0;
}

#ifdef WLAN_NL80211_TESTMODE
#ifdef FEATURE_WLAN_LPHB
static void wlan_hdd_cfg80211_lphb_wait_timeout_ind_handler
(
   void *pAdapter,
   void *indCont
)
{
   tSirLPHBTimeoutInd   *lphbTimeoutInd;
   struct sk_buff       *skb;

   VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
             "LPHB wait timeout indication arrived");

   if (NULL == indCont)
   {
      VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                "LPHB timeout, invalid argument");
      return;
   }

   lphbTimeoutInd = (tSirLPHBTimeoutInd *)indCont;
   skb = cfg80211_testmode_alloc_event_skb(
                  ((hdd_adapter_t *)pAdapter)->wdev.wiphy,
                  sizeof(tSirLPHBTimeoutInd),
                  GFP_ATOMIC);
   if (!skb)
   {
      VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
             "LPHB timeout, NL buffer alloc fail");
      return;
   }

   NLA_PUT_U32(skb, WLAN_HDD_TM_ATTR_CMD, WLAN_HDD_TM_CMD_WLAN_HB);
   NLA_PUT_U32(skb, WLAN_HDD_TM_ATTR_TYPE, lphbTimeoutInd->protocolType);
   NLA_PUT(skb, WLAN_HDD_TM_ATTR_DATA,
           sizeof(tSirLPHBTimeoutInd), lphbTimeoutInd);
   cfg80211_testmode_event(skb, GFP_ATOMIC);
   return;

nla_put_failure:
   VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
             "NLA Put fail");
   kfree_skb(skb);

   return;
}
#endif /*                   */

static int wlan_hdd_cfg80211_testmode(struct wiphy *wiphy, void *data, int len)
{
    struct nlattr *tb[WLAN_HDD_TM_ATTR_MAX + 1];
    int err = 0;
#ifdef FEATURE_WLAN_LPHB
    hdd_context_t *pHddCtx = wiphy_priv(wiphy);
#endif /*                   */

    err = nla_parse(tb, WLAN_HDD_TM_ATTR_MAX, data, len, wlan_hdd_tm_policy);
    if (err)
    {
        VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                  "%s Testmode INV ATTR", __func__);
        return err;
    }

    if (!tb[WLAN_HDD_TM_ATTR_CMD])
    {
        VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                  "%s Testmode INV CMD", __func__);
        return -EINVAL;
    }

    switch (nla_get_u32(tb[WLAN_HDD_TM_ATTR_CMD]))
    {
#ifdef FEATURE_WLAN_LPHB
        /*                                           */
        case WLAN_HDD_TM_CMD_WLAN_HB:
        {
            int   buf_len;
            void *buf;
            tSirLPHBReq *hb_params = NULL;

            if (!tb[WLAN_HDD_TM_ATTR_DATA])
            {
                VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                          "%s Testmode INV DATA", __func__);
                return -EINVAL;
            }

            buf = nla_data(tb[WLAN_HDD_TM_ATTR_DATA]);
            buf_len = nla_len(tb[WLAN_HDD_TM_ATTR_DATA]);
            hb_params = (tSirLPHBReq *)vos_mem_malloc(sizeof(tSirLPHBReq));
            if (NULL == hb_params)
            {
                VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                          "%s Request Buffer Alloc Fail", __func__);
                return -EINVAL;
            }

            vos_mem_copy(hb_params, buf, buf_len);

            /*                                                
                                           
                                          
                                           
                                                             
                                                                 */
            if ((LPHB_SET_EN_PARAMS_INDID == hb_params->cmd) &&
               (!pHddCtx->hdd_wlan_suspended))
            {
               /*                                               
                                        
                                                                         */
               pHddCtx->lphbEnableReq.enable  =
                                    hb_params->params.lphbEnableReq.enable;
               pHddCtx->lphbEnableReq.item    =
                                    hb_params->params.lphbEnableReq.item;
               pHddCtx->lphbEnableReq.session =
                                    hb_params->params.lphbEnableReq.session;
               vos_mem_free(hb_params);
            }
            else
            {
               eHalStatus smeStatus;

               /*                                                           */
               smeStatus = sme_LPHBConfigReq((tHalHandle)(pHddCtx->hHal),
                              hb_params,
                              wlan_hdd_cfg80211_lphb_wait_timeout_ind_handler);
               if (eHAL_STATUS_SUCCESS != smeStatus)
               {
                  VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                            "LPHB Config Fail, disable");
                  pHddCtx->lphbEnableReq.enable = 0;
                  vos_mem_free(hb_params);
               }
            }
            return 0;
         }
#endif /*                   */
        default:
            return -EOPNOTSUPP;
    }

   return err;
}
#endif /*                         */

static int wlan_hdd_cfg80211_dump_survey(struct wiphy *wiphy,
                                         struct net_device *dev,
                                         int idx, struct survey_info *survey)
{
    hdd_adapter_t *pAdapter = WLAN_HDD_GET_PRIV_PTR(dev);
    hdd_context_t *pHddCtx;
    hdd_station_ctx_t *pHddStaCtx;
    tHalHandle halHandle;
    v_U32_t channel = 0, freq = 0; /*                         */
    v_S7_t snr,rssi;
    int status, i, j, filled = 0;

    ENTER();


    if (NULL == pAdapter)
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_FATAL,
                   "%s: HDD adapter is Null", __func__);
        return -ENODEV;
    }

    if (NULL == wiphy)
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_FATAL,
                   "%s: wiphy is Null", __func__);
        return -ENODEV;
    }

    pHddCtx = WLAN_HDD_GET_CTX(pAdapter);
    status = wlan_hdd_validate_context(pHddCtx);

    if (0 != status)
    {
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                   "%s: HDD context is not valid", __func__);
        return status;
    }

    pHddStaCtx = WLAN_HDD_GET_STATION_CTX_PTR(pAdapter);

    if (0 == pHddCtx->cfg_ini->fEnableSNRMonitoring ||
        0 != pAdapter->survey_idx ||
        eConnectionState_Associated != pHddStaCtx->conn_info.connState)
    {
        /*                                                               
                                                                       
                                                                       
                                                                      
                                                                 
                                                                       
                             
         */
        pAdapter->survey_idx = 0;
        return -ENONET;
    }

    halHandle = WLAN_HDD_GET_HAL_CTX(pAdapter);

    wlan_hdd_get_snr(pAdapter, &snr);
    wlan_hdd_get_rssi(pAdapter, &rssi);

    sme_GetOperationChannel(halHandle, &channel, pAdapter->sessionId);
    hdd_wlan_get_freq(channel, &freq);


    for (i = 0; i < IEEE80211_NUM_BANDS; i++)
    {
        if (NULL == wiphy->bands[i])
        {
           VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
                     "%s: wiphy->bands[i] is NULL, i = %d", __func__, i);
           continue;
        }

        for (j = 0; j < wiphy->bands[i]->n_channels; j++)
        {
            struct ieee80211_supported_band *band = wiphy->bands[i];

            if (band->channels[j].center_freq == (v_U16_t)freq)
            {
                survey->channel = &band->channels[j];
                /*                                                            
                                                                          
                                                  
                                                       
                 */
                survey->noise = rssi - snr;
                survey->filled = SURVEY_INFO_NOISE_DBM;
                filled = 1;
            }
        }
     }

     if (filled)
        pAdapter->survey_idx = 1;
     else
     {
        pAdapter->survey_idx = 0;
        return -ENONET;
     }

     return 0;
}

/*
                                          
                                             
                                                                             
 */
int wlan_hdd_cfg80211_resume_wlan(struct wiphy *wiphy)
{
    hdd_context_t *pHddCtx = wiphy_priv(wiphy);
    hdd_adapter_t *pAdapter;
    hdd_adapter_list_node_t *pAdapterNode, *pNext;
    VOS_STATUS status = VOS_STATUS_SUCCESS;

    ENTER();

    if ( NULL == pHddCtx )
    {
        VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_FATAL,
              "%s: HddCtx validation failed", __func__);
        return 0;
    }

    if (pHddCtx->isLogpInProgress)
    {
        VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                  "%s: LOGP in Progress. Ignore!!!", __func__);
        return 0;
    }

    if (WLAN_HDD_IS_LOAD_UNLOAD_IN_PROGRESS(pHddCtx))
    {
        VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                  "%s: Unloading/Loading in Progress. Ignore!!!", __func__);
        return 0;
    }

    spin_lock(&pHddCtx->schedScan_lock);
    pHddCtx->isWiphySuspended = FALSE;
    if (TRUE != pHddCtx->isSchedScanUpdatePending)
    {
        spin_unlock(&pHddCtx->schedScan_lock);
        VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO,
              "%s: Return resume is not due to PNO indication", __func__);
        return 0;
    }
    //                                                               
    pHddCtx->isSchedScanUpdatePending = FALSE;
    spin_unlock(&pHddCtx->schedScan_lock);

    status =  hdd_get_front_adapter ( pHddCtx, &pAdapterNode );

    while ( NULL != pAdapterNode && VOS_STATUS_SUCCESS == status )
    {
        pAdapter = pAdapterNode->pAdapter;
        if ( (NULL != pAdapter) &&
             (WLAN_HDD_INFRA_STATION == pAdapter->device_mode) )
        {
            if (0 != wlan_hdd_cfg80211_update_bss(pHddCtx->wiphy, pAdapter))
                VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_WARN,
                      "%s: NO SCAN result", __func__);
            else
                cfg80211_sched_scan_results(pHddCtx->wiphy);

            VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO,
                    "%s : cfg80211 scan result database updated", __func__);

            return 0;

        }
        status = hdd_get_next_adapter ( pHddCtx, pAdapterNode, &pNext );
        pAdapterNode = pNext;
    }

    VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO,
          "%s: Failed to find Adapter", __func__);
    return 0;
}

/*
                                           
                                               
 */
int wlan_hdd_cfg80211_suspend_wlan(struct wiphy *wiphy,
                                   struct cfg80211_wowlan *wow)
{
    hdd_context_t *pHddCtx = wiphy_priv(wiphy);

    ENTER();
    if (NULL == pHddCtx)
    {
        VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_FATAL,
              "%s: HddCtx validation failed", __func__);
        return 0;
    }

    pHddCtx->isWiphySuspended = TRUE;

    EXIT();

    return 0;
}

/*              */
static struct cfg80211_ops wlan_hdd_cfg80211_ops =
{
    .add_virtual_intf = wlan_hdd_add_virtual_intf,
    .del_virtual_intf = wlan_hdd_del_virtual_intf,
    .change_virtual_intf = wlan_hdd_cfg80211_change_iface,
    .change_station = wlan_hdd_change_station,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,4,0))
    .add_beacon = wlan_hdd_cfg80211_add_beacon,
    .del_beacon = wlan_hdd_cfg80211_del_beacon,
    .set_beacon = wlan_hdd_cfg80211_set_beacon,
#else
    .start_ap = wlan_hdd_cfg80211_start_ap,
    .change_beacon = wlan_hdd_cfg80211_change_beacon,
    .stop_ap = wlan_hdd_cfg80211_stop_ap,
#endif
    .change_bss = wlan_hdd_cfg80211_change_bss,
    .add_key = wlan_hdd_cfg80211_add_key,
    .get_key = wlan_hdd_cfg80211_get_key,
    .del_key = wlan_hdd_cfg80211_del_key,
    .set_default_key = wlan_hdd_cfg80211_set_default_key,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,6,0))
    .set_channel = wlan_hdd_cfg80211_set_channel,
#endif
    .scan = wlan_hdd_cfg80211_scan,
    .connect = wlan_hdd_cfg80211_connect,
    .disconnect = wlan_hdd_cfg80211_disconnect,
    .join_ibss  = wlan_hdd_cfg80211_join_ibss,
    .leave_ibss = wlan_hdd_cfg80211_leave_ibss,
    .set_wiphy_params = wlan_hdd_cfg80211_set_wiphy_params,
    .set_tx_power = wlan_hdd_cfg80211_set_txpower,
    .get_tx_power = wlan_hdd_cfg80211_get_txpower,
    .remain_on_channel = wlan_hdd_cfg80211_remain_on_channel,
    .cancel_remain_on_channel =  wlan_hdd_cfg80211_cancel_remain_on_channel,
    .mgmt_tx =  wlan_hdd_action,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,38))
     .mgmt_tx_cancel_wait = wlan_hdd_cfg80211_mgmt_tx_cancel_wait,
     .set_default_mgmt_key = wlan_hdd_set_default_mgmt_key,
     .set_txq_params = wlan_hdd_set_txq_params,
#endif
     .get_station = wlan_hdd_cfg80211_get_station,
     .set_power_mgmt = wlan_hdd_cfg80211_set_power_mgmt,
     .del_station  = wlan_hdd_cfg80211_del_station,
     .add_station  = wlan_hdd_cfg80211_add_station,
#ifdef FEATURE_WLAN_LFR
     .set_pmksa = wlan_hdd_cfg80211_set_pmksa,
     .del_pmksa = wlan_hdd_cfg80211_del_pmksa,
     .flush_pmksa = wlan_hdd_cfg80211_flush_pmksa,
#endif
#if defined(WLAN_FEATURE_VOWIFI_11R) && defined(KERNEL_SUPPORT_11R_CFG80211)
     .update_ft_ies = wlan_hdd_cfg80211_update_ft_ies,
#endif
#ifdef FEATURE_WLAN_TDLS
     .tdls_mgmt = wlan_hdd_cfg80211_tdls_mgmt,
     .tdls_oper = wlan_hdd_cfg80211_tdls_oper,
#endif
#ifdef WLAN_FEATURE_GTK_OFFLOAD
     .set_rekey_data = wlan_hdd_cfg80211_set_rekey_data,
#endif /*                          */
#ifdef FEATURE_WLAN_SCAN_PNO
     .sched_scan_start = wlan_hdd_cfg80211_sched_scan_start,
     .sched_scan_stop = wlan_hdd_cfg80211_sched_scan_stop,
#endif /*                      */
     .resume = wlan_hdd_cfg80211_resume_wlan,
     .suspend = wlan_hdd_cfg80211_suspend_wlan,
     .set_mac_acl = wlan_hdd_cfg80211_set_mac_acl,
#ifdef WLAN_NL80211_TESTMODE
     .testmode_cmd = wlan_hdd_cfg80211_testmode,
#endif
     .dump_survey = wlan_hdd_cfg80211_dump_survey,
};


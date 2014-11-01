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

#include "limTypes.h"
#include "limUtils.h"
#include "limAssocUtils.h"
#include "limSecurityUtils.h"
#include "pmmApi.h"


//                                                                     
#define LIM_BACKGROUND_SCAN_PERIOD_DEFAULT_MS    5000
//                              
#define LIM_CHANNEL_SWITCH_TIMER_TICKS           1
//                         
#define LIM_QUIET_TIMER_TICKS                    100
//                                      
#define LIM_QUIET_BSS_TIMER_TICK                 100
//                                     
#define LIM_KEEPALIVE_TIMER_MS                   3000
//                                                  
#define LIM_JOIN_PROBE_REQ_TIMER_MS              200

//                                                                   
#define LIM_HB_TIMER_BEACON_INTERVAL             100

/*                                                               
                                               */
#define ACTIVE_TO_PASSIVE_CONVERISON_TIMEOUT     1000

/* 
                    
  
           
                                         
                                       
                                                      
  
        
  
              
     
  
       
     
  
                                                 
  
               
 */

v_UINT_t
limCreateTimers(tpAniSirGlobal pMac)
{
    tANI_U32 cfgValue, i=0;

    PELOG1(limLog(pMac, LOG1, FL("Creating Timers used by LIM module in Role %d"), pMac->lim.gLimSystemRole);)

    if (wlan_cfgGetInt(pMac, WNI_CFG_ACTIVE_MINIMUM_CHANNEL_TIME,
                  &cfgValue) != eSIR_SUCCESS)
    {
        /* 
                                                
                               
         */
        limLog(pMac, LOGP, FL("could not retrieve MinChannelTimeout value"));
    }
    cfgValue = SYS_MS_TO_TICKS(cfgValue);

    //                                                      
    if (tx_timer_create(&pMac->lim.limTimers.gLimMinChannelTimer,
                        "MIN CHANNEL TIMEOUT",
                        limTimerHandler, SIR_LIM_MIN_CHANNEL_TIMEOUT,
                        cfgValue, 0,
                        TX_NO_ACTIVATE) != TX_SUCCESS)
    {
        //                                    
        //          
        limLog(pMac, LOGP, FL("could not create MIN channel timer"));
        return TX_TIMER_ERROR;
    }
    PELOG2(limLog(pMac, LOG2, FL("Created MinChannelTimer"));)

    /*                                                              
                                                                   
                    
     */

    cfgValue = cfgValue/2 ;
    if( cfgValue >= 1)
    {
        //                                                            
        if (tx_timer_create(&pMac->lim.limTimers.gLimPeriodicProbeReqTimer,
                           "Periodic Probe Request Timer",
                           limTimerHandler, SIR_LIM_PERIODIC_PROBE_REQ_TIMEOUT,
                           cfgValue, 0,
                           TX_NO_ACTIVATE) != TX_SUCCESS)
        {
           //                                           
           //          
           limLog(pMac, LOGP, FL("could not create periodic probe timer"));
           goto err_timer;
        }
     }


    if (wlan_cfgGetInt(pMac, WNI_CFG_ACTIVE_MAXIMUM_CHANNEL_TIME,
                  &cfgValue) != eSIR_SUCCESS)
    {
        /* 
                                                
                               
         */
        limLog(pMac, LOGP,
               FL("could not retrieve MAXChannelTimeout value"));
    }
    cfgValue = SYS_MS_TO_TICKS(cfgValue);

    if (tx_timer_create(&pMac->lim.limTimers.gLimMaxChannelTimer,
                        "MAX CHANNEL TIMEOUT",
                        limTimerHandler, SIR_LIM_MAX_CHANNEL_TIMEOUT,
                        cfgValue, 0,
                        TX_NO_ACTIVATE) != TX_SUCCESS)
    {
        //                                    
        //          
        limLog(pMac, LOGP, FL("could not create MAX channel timer"));

        goto err_timer;
    }
    PELOG2(limLog(pMac, LOG2, FL("Created MaxChannelTimer"));)

    if (pMac->lim.gLimSystemRole != eLIM_AP_ROLE)
    {
        //                            
        if (tx_timer_create(&pMac->lim.limTimers.gLimChannelSwitchTimer,
                            "CHANNEL SWITCH TIMER",
                            limChannelSwitchTimerHandler,
                            0,                         //                 
                            LIM_CHANNEL_SWITCH_TIMER_TICKS,  //              
                            0,                         //                 
                            TX_NO_ACTIVATE) != TX_SUCCESS)
        {
            limLog(pMac, LOGP, FL("failed to create Channel Switch timer"));
            goto err_timer;
        }

        //
        //                   
        //                                           
        //                                          
        //
        if (tx_timer_create(&pMac->lim.limTimers.gLimQuietTimer,
                            "QUIET TIMER",
                            limQuietTimerHandler,
                            SIR_LIM_QUIET_TIMEOUT,     //                 
                            LIM_QUIET_TIMER_TICKS,     //              
                            0,                         //                 
                            TX_NO_ACTIVATE) != TX_SUCCESS)
        {
            limLog(pMac, LOGP, FL("failed to create Quiet Begin Timer"));
            goto err_timer;
        }

        //
        //                       
        //                                                 
        //                                               
        //                                                   
        //                  
        //
        if (tx_timer_create(&pMac->lim.limTimers.gLimQuietBssTimer,
                            "QUIET BSS TIMER",
                            limQuietBssTimerHandler,
                            SIR_LIM_QUIET_BSS_TIMEOUT, //                 
                            LIM_QUIET_BSS_TIMER_TICK,  //              
                            0,                         //                 
                            TX_NO_ACTIVATE) != TX_SUCCESS)
        {
            limLog(pMac, LOGP, FL("failed to create Quiet Begin Timer"));
            goto err_timer;
        }

        if (wlan_cfgGetInt(pMac, WNI_CFG_JOIN_FAILURE_TIMEOUT,
                      &cfgValue) != eSIR_SUCCESS)
        {
            /* 
                                                     
                                   
             */
            limLog(pMac, LOGP,
               FL("could not retrieve JoinFailureTimeout value"));
        }
        cfgValue = SYS_MS_TO_TICKS(cfgValue);

        //                                                
        if (tx_timer_create(&pMac->lim.limTimers.gLimJoinFailureTimer,
                        "JOIN FAILURE TIMEOUT",
                        limTimerHandler, SIR_LIM_JOIN_FAIL_TIMEOUT,
                        cfgValue, 0,
                        TX_NO_ACTIVATE) != TX_SUCCESS)
        {
            //                                      
            //          
            limLog(pMac, LOGP, FL("could not create Join failure timer"));

            goto err_timer;
        }

        //                                         
        if ((tx_timer_create(&pMac->lim.limTimers.gLimPeriodicJoinProbeReqTimer,
                        "Periodic Join Probe Request Timer",
                        limTimerHandler, SIR_LIM_PERIODIC_JOIN_PROBE_REQ_TIMEOUT,
                        SYS_MS_TO_TICKS(LIM_JOIN_PROBE_REQ_TIMER_MS), 0,
                        TX_NO_ACTIVATE)) != TX_SUCCESS)
        {
            //                                                     
            //          
            limLog(pMac, LOGP, FL("could not create Periodic Join Probe Request timer"));
            goto err_timer;
        }

        if (wlan_cfgGetInt(pMac, WNI_CFG_ASSOCIATION_FAILURE_TIMEOUT,
                      &cfgValue) != eSIR_SUCCESS)
        {
            /* 
                                                      
                                   
             */
            limLog(pMac, LOGP,
               FL("could not retrieve AssocFailureTimeout value"));
        }
        cfgValue = SYS_MS_TO_TICKS(cfgValue);

        //                                                       
        if (tx_timer_create(&pMac->lim.limTimers.gLimAssocFailureTimer,
                        "ASSOC FAILURE TIMEOUT",
                        limAssocFailureTimerHandler, LIM_ASSOC,
                        cfgValue, 0,
                        TX_NO_ACTIVATE) != TX_SUCCESS)
        {
            //                                       
            //          
            limLog(pMac, LOGP,
               FL("could not create Association failure timer"));

            goto err_timer;
        }
        if (wlan_cfgGetInt(pMac, WNI_CFG_REASSOCIATION_FAILURE_TIMEOUT,
                      &cfgValue) != eSIR_SUCCESS)
        {
            /* 
                                                        
                                   
             */
            limLog(pMac, LOGP,
               FL("could not retrieve ReassocFailureTimeout value"));
        }
        cfgValue = SYS_MS_TO_TICKS(cfgValue);

        //                                                       
        if (tx_timer_create(&pMac->lim.limTimers.gLimReassocFailureTimer,
                            "REASSOC FAILURE TIMEOUT",
                            limAssocFailureTimerHandler, LIM_REASSOC,
                            cfgValue, 0,
                            TX_NO_ACTIVATE) != TX_SUCCESS)
        {
            //                                         
            //          
            limLog(pMac, LOGP,
               FL("could not create Reassociation failure timer"));

            goto err_timer;
        }

        if (wlan_cfgGetInt(pMac, WNI_CFG_ADDTS_RSP_TIMEOUT, &cfgValue) != eSIR_SUCCESS)
            limLog(pMac, LOGP, FL("Fail to get WNI_CFG_ADDTS_RSP_TIMEOUT "));

        cfgValue = SYS_MS_TO_TICKS(cfgValue);

        //                                                  
        if (tx_timer_create(&pMac->lim.limTimers.gLimAddtsRspTimer,
                            "ADDTS RSP TIMEOUT",
                            limAddtsResponseTimerHandler,
                            SIR_LIM_ADDTS_RSP_TIMEOUT,
                            cfgValue, 0,
                            TX_NO_ACTIVATE) != TX_SUCCESS)
        {
            //                                      
            //          
            limLog(pMac, LOGP, FL("could not create Addts response timer"));

            goto err_timer;
        }

        if (wlan_cfgGetInt(pMac, WNI_CFG_AUTHENTICATE_FAILURE_TIMEOUT,
                      &cfgValue) != eSIR_SUCCESS)
        {
            /* 
                                                     
                                   
             */
            limLog(pMac, LOGP,
               FL("could not retrieve AuthFailureTimeout value"));
        }
        cfgValue = SYS_MS_TO_TICKS(cfgValue);

        //                                                
        if (tx_timer_create(&pMac->lim.limTimers.gLimAuthFailureTimer,
                            "AUTH FAILURE TIMEOUT",
                            limTimerHandler,
                            SIR_LIM_AUTH_FAIL_TIMEOUT,
                            cfgValue, 0,
                            TX_NO_ACTIVATE) != TX_SUCCESS)
        {
            //                                      
            //          
            limLog(pMac, LOGP, FL("could not create Auth failure timer"));

            goto err_timer;
        }

        if (wlan_cfgGetInt(pMac, WNI_CFG_BEACON_INTERVAL,
                      &cfgValue) != eSIR_SUCCESS)
        {
            /* 
                                                  
                                   
             */
            limLog(pMac, LOGP,
               FL("could not retrieve BEACON_INTERVAL value"));
        }
        cfgValue = SYS_MS_TO_TICKS(cfgValue);

        if (tx_timer_create(&pMac->lim.limTimers.gLimHeartBeatTimer,
                            "Heartbeat TIMEOUT",
                            limTimerHandler,
                            SIR_LIM_HEART_BEAT_TIMEOUT,
                            cfgValue,
                            0,
                            TX_NO_ACTIVATE) != TX_SUCCESS)
        {
            //                                  
            //          
            limLog(pMac, LOGP,
               FL("call to create heartbeat timer failed"));
            goto err_timer;
        }

        if (wlan_cfgGetInt(pMac, WNI_CFG_PROBE_AFTER_HB_FAIL_TIMEOUT,
                      &cfgValue) != eSIR_SUCCESS)
        {
            /* 
                                                   
                                         
             */
            limLog(pMac, LOGP,
               FL("could not retrieve PROBE_AFTER_HB_FAIL_TIMEOUT value"));
        }

        //                                        
        cfgValue = SYS_MS_TO_TICKS(cfgValue);

        if (tx_timer_create(&pMac->lim.limTimers.gLimProbeAfterHBTimer,
                            "Probe after Heartbeat TIMEOUT",
                            limTimerHandler,
                            SIR_LIM_PROBE_HB_FAILURE_TIMEOUT,
                            cfgValue,
                            0,
                            TX_NO_ACTIVATE) != TX_SUCCESS)
        {
            //                                                        
            //          
            limLog(pMac, LOGP,
                   FL("unable to create ProbeAfterHBTimer"));
            goto err_timer;
        }

        if (wlan_cfgGetInt(pMac, WNI_CFG_BACKGROUND_SCAN_PERIOD,
                      &cfgValue) != eSIR_SUCCESS)
        {
            /* 
                                                         
                                   
             */
            limLog(pMac, LOGP,
               FL("could not retrieve Background scan period value"));
        }

        /*
                                                                                  
                                                                          
                                                                            
                                                                
         */
        if (cfgValue == 0)
        {
            cfgValue = LIM_BACKGROUND_SCAN_PERIOD_DEFAULT_MS;
            pMac->lim.gLimBackgroundScanDisable = true;
        }
        else
            pMac->lim.gLimBackgroundScanDisable = false;

        cfgValue = SYS_MS_TO_TICKS(cfgValue);

        if (tx_timer_create(&pMac->lim.limTimers.gLimBackgroundScanTimer,
                            "Background scan TIMEOUT",
                            limTimerHandler,
                            SIR_LIM_CHANNEL_SCAN_TIMEOUT,
                            cfgValue,
                            cfgValue,
                            TX_NO_ACTIVATE) != TX_SUCCESS)
        {
            //                                        
            //          
            limLog(pMac, LOGP,
               FL("call to create background scan timer failed"));
            goto err_timer;
        }
#ifdef FEATURE_WLAN_TDLS_INTERNAL
        /*
                               
                                            
         */

        if (wlan_cfgGetInt(pMac, WNI_CFG_ASSOCIATION_FAILURE_TIMEOUT,
                                            &cfgValue) != eSIR_SUCCESS)
        {
            /*
                                                             
                                   
             */
            limLog(pMac, LOGP,
               FL("could not retrieve ReassocFailureTimeout value"));
        }
        cfgValue = SYS_MS_TO_TICKS(cfgValue);

        /*
                                                                          
         */
        if (tx_timer_create(&pMac->lim.limTimers.gLimTdlsDisRspWaitTimer,
                            "TDLS discovery response WAIT",
                            limTimerHandler,
                            SIR_LIM_TDLS_DISCOVERY_RSP_WAIT,
                            cfgValue, 0,
                            TX_NO_ACTIVATE) != TX_SUCCESS)
        {
            limLog(pMac, LOGP,
               FL("could not create TDLS discovery response wait timer"));
        goto err_timer;
        }
#endif
    }


    cfgValue = SYS_MS_TO_TICKS(LIM_HASH_MISS_TIMER_MS);

    if (tx_timer_create(
                        &pMac->lim.limTimers.gLimSendDisassocFrameThresholdTimer,
                        "Disassoc throttle TIMEOUT",
                        limSendDisassocFrameThresholdHandler,
                        SIR_LIM_HASH_MISS_THRES_TIMEOUT,
                        cfgValue,
                        cfgValue,
                        TX_AUTO_ACTIVATE) != TX_SUCCESS)
    {
        //                                                          
        //          
        limLog(pMac, LOGP,
               FL("create Disassociate throttle timer failed"));
        goto err_timer;
    }
    PELOG1(limLog(pMac, LOG1,
           FL("Created Disassociate throttle timer "));)

    /* 
                                                                     
     */

    if (wlan_cfgGetInt(pMac, WNI_CFG_KEEPALIVE_TIMEOUT,
                  &cfgValue) != eSIR_SUCCESS)
    {
        /* 
                                                
                               
         */
        limLog(pMac, LOGP,
               FL("could not retrieve keepalive timeout value"));
    }

    //                                                      
    if (cfgValue == 0)
    {
        cfgValue = LIM_KEEPALIVE_TIMER_MS;
        pMac->sch.keepAlive = 0;
    } else
        pMac->sch.keepAlive = 1;


    cfgValue = SYS_MS_TO_TICKS(cfgValue + SYS_TICK_DUR_MS - 1);

    if (tx_timer_create(&pMac->lim.limTimers.gLimKeepaliveTimer,
                        "KEEPALIVE_TIMEOUT",
                        limKeepaliveTmerHandler,
                        0,
                        cfgValue,
                        cfgValue,
                        (pMac->lim.gLimSystemRole == eLIM_AP_ROLE) ?
                         TX_AUTO_ACTIVATE : TX_NO_ACTIVATE)
                  != TX_SUCCESS)
    {
        //                                           
        limLog(pMac, LOGP, FL("Cannot create keepalive timer."));
        goto err_timer;
    }

    /* 
                                         
     */

    if (wlan_cfgGetInt(pMac, WNI_CFG_WT_CNF_TIMEOUT,
                  &cfgValue) != eSIR_SUCCESS)
    {
        /* 
                                               
                               
         */
        limLog(pMac, LOGP,
               FL("could not retrieve CNF timeout value"));
    }
    cfgValue = SYS_MS_TO_TICKS(cfgValue);

    for (i=0; i<pMac->lim.maxStation; i++)
    {
        if (tx_timer_create(&pMac->lim.limTimers.gpLimCnfWaitTimer[i],
                            "CNF_MISS_TIMEOUT",
                            limCnfWaitTmerHandler,
                            (tANI_U32)i,
                            cfgValue,
                            0,
                            TX_NO_ACTIVATE) != TX_SUCCESS)
        {
            //                                 
            limLog(pMac, LOGP, FL("Cannot create CNF wait timer."));
            goto err_timer;
        }
    }

    /*
                                                      
      
     */

    //                                    
    if (wlan_cfgGetInt(pMac, WNI_CFG_MAX_NUM_PRE_AUTH,
             &cfgValue) != eSIR_SUCCESS)
    {
        /*
                                          
                               
         */
        limLog(pMac, LOGP,
               FL("could not retrieve mac preauth value"));
    }
    pMac->lim.gLimPreAuthTimerTable.numEntry = cfgValue;
    pMac->lim.gLimPreAuthTimerTable.pTable = vos_mem_malloc(cfgValue*sizeof(tLimPreAuthNode));
    if(pMac->lim.gLimPreAuthTimerTable.pTable == NULL)
    {
        limLog(pMac, LOGP, FL("AllocateMemory failed!"));
        goto err_timer;
    }

    limInitPreAuthTimerTable(pMac, &pMac->lim.gLimPreAuthTimerTable);
    PELOG1(limLog(pMac, LOG1, FL("alloc and init table for preAuth timers"));)


    {
        /* 
                                        
         */
        if (wlan_cfgGetInt(pMac, WNI_CFG_OLBC_DETECT_TIMEOUT,
                      &cfgValue) != eSIR_SUCCESS)
        {
            /* 
                                                      
                                   
             */
            limLog(pMac, LOGP,
               FL("could not retrieve OLBD detect timeout value"));
        }

        cfgValue = SYS_MS_TO_TICKS(cfgValue);

        if (tx_timer_create(
                &pMac->lim.limTimers.gLimUpdateOlbcCacheTimer,
                "OLBC UPDATE CACHE TIMEOUT",
                limUpdateOlbcCacheTimerHandler,
                SIR_LIM_UPDATE_OLBC_CACHEL_TIMEOUT,
                cfgValue,
                cfgValue,
                TX_NO_ACTIVATE) != TX_SUCCESS)
        {
            //                                      
            //          
            limLog(pMac, LOGP, FL("Cannot create update OLBC cache timer"));
            goto err_timer;
        }
    }
#ifdef WLAN_FEATURE_VOWIFI_11R
    //                                               
    //                                                       
    //            
    //                                                        
    cfgValue = 1000;
    cfgValue = SYS_MS_TO_TICKS(cfgValue);

    if (tx_timer_create(&pMac->lim.limTimers.gLimFTPreAuthRspTimer,
                                    "FT PREAUTH RSP TIMEOUT",
                                    limTimerHandler, SIR_LIM_FT_PREAUTH_RSP_TIMEOUT,
                                    cfgValue, 0,
                                    TX_NO_ACTIVATE) != TX_SUCCESS)
    {
        //                                     
        //          
        limLog(pMac, LOGP, FL("could not create Join failure timer"));
        goto err_timer;
    }
#endif

#if defined(FEATURE_WLAN_CCX) && !defined(FEATURE_WLAN_CCX_UPLOAD)
    cfgValue = 5000;
    cfgValue = SYS_MS_TO_TICKS(cfgValue);

    if (tx_timer_create(&pMac->lim.limTimers.gLimCcxTsmTimer,
                                    "CCX TSM Stats TIMEOUT",
                                    limTimerHandler, SIR_LIM_CCX_TSM_TIMEOUT,
                                    cfgValue, 0,
                                    TX_NO_ACTIVATE) != TX_SUCCESS)
    {
        //                                     
        //          
        limLog(pMac, LOGP, FL("could not create Join failure timer"));
        goto err_timer;
    }
#endif /*                                              */

    cfgValue = 1000;
    cfgValue = SYS_MS_TO_TICKS(cfgValue);
    if (tx_timer_create(&pMac->lim.limTimers.gLimRemainOnChannelTimer,
                                    "FT PREAUTH RSP TIMEOUT",
                                    limTimerHandler, SIR_LIM_REMAIN_CHN_TIMEOUT,
                                    cfgValue, 0,
                                    TX_NO_ACTIVATE) != TX_SUCCESS)
    {
        //                                     
        //          
        limLog(pMac, LOGP, FL("could not create Join failure timer"));
        goto err_timer;
    }


    cfgValue = 1000;
    cfgValue = SYS_MS_TO_TICKS(cfgValue);
    if (tx_timer_create(&pMac->lim.limTimers.gLimDisassocAckTimer,
                                    "DISASSOC ACK TIMEOUT",
                                    limTimerHandler, SIR_LIM_DISASSOC_ACK_TIMEOUT,
                                    cfgValue, 0,
                                    TX_NO_ACTIVATE) != TX_SUCCESS)
    {
        limLog(pMac, LOGP, FL("could not DISASSOC ACK TIMEOUT timer"));
        goto err_timer;
    }

    cfgValue = 1000;
    cfgValue = SYS_MS_TO_TICKS(cfgValue);
    if (tx_timer_create(&pMac->lim.limTimers.gLimDeauthAckTimer,
                                    "DISASSOC ACK TIMEOUT",
                                    limTimerHandler, SIR_LIM_DEAUTH_ACK_TIMEOUT,
                                    cfgValue, 0,
                                    TX_NO_ACTIVATE) != TX_SUCCESS)
    {
        limLog(pMac, LOGP, FL("could not create DEAUTH ACK TIMEOUT timer"));
        goto err_timer;
    }

    cfgValue = LIM_INSERT_SINGLESHOTNOA_TIMEOUT_VALUE; //                                                                                                                                  
    cfgValue = SYS_MS_TO_TICKS(cfgValue);
    if (tx_timer_create(&pMac->lim.limTimers.gLimP2pSingleShotNoaInsertTimer,
                                    "Single Shot NOA Insert timeout",
                                    limTimerHandler, SIR_LIM_INSERT_SINGLESHOT_NOA_TIMEOUT,
                                    cfgValue, 0,
                                    TX_NO_ACTIVATE) != TX_SUCCESS)
    {
        limLog(pMac, LOGP, FL("could not create Single Shot NOA Insert Timeout timer"));
        goto err_timer;
    }

    cfgValue = ACTIVE_TO_PASSIVE_CONVERISON_TIMEOUT;
    cfgValue = SYS_MS_TO_TICKS(cfgValue);
    if (tx_timer_create(&pMac->lim.limTimers.gLimActiveToPassiveChannelTimer,
                                  "ACTIVE TO PASSIVE CHANNEL", limTimerHandler,
                 SIR_LIM_CONVERT_ACTIVE_CHANNEL_TO_PASSIVE, cfgValue, 0,
                 TX_NO_ACTIVATE) != TX_SUCCESS)
    {
        limLog(pMac, LOGW,FL("could not create timer for passive channel to active channel"));
        goto err_timer;
    }


    return TX_SUCCESS;

    err_timer:
        tx_timer_delete(&pMac->lim.limTimers.gLimDeauthAckTimer);
        tx_timer_delete(&pMac->lim.limTimers.gLimDisassocAckTimer);
        tx_timer_delete(&pMac->lim.limTimers.gLimRemainOnChannelTimer);
#if defined(FEATURE_WLAN_CCX) && !defined(FEATURE_WLAN_CCX_UPLOAD)
        tx_timer_delete(&pMac->lim.limTimers.gLimCcxTsmTimer);
#endif /*                                              */
        tx_timer_delete(&pMac->lim.limTimers.gLimFTPreAuthRspTimer);
        tx_timer_delete(&pMac->lim.limTimers.gLimUpdateOlbcCacheTimer);
        while(((tANI_S32)--i) >= 0)
        {
            tx_timer_delete(&pMac->lim.limTimers.gpLimCnfWaitTimer[i]);
        }
        tx_timer_delete(&pMac->lim.limTimers.gLimKeepaliveTimer);
        tx_timer_delete(&pMac->lim.limTimers.gLimSendDisassocFrameThresholdTimer);
        tx_timer_delete(&pMac->lim.limTimers.gLimBackgroundScanTimer);
        tx_timer_delete(&pMac->lim.limTimers.gLimProbeAfterHBTimer);
        tx_timer_delete(&pMac->lim.limTimers.gLimHeartBeatTimer);
        tx_timer_delete(&pMac->lim.limTimers.gLimAuthFailureTimer);
        tx_timer_delete(&pMac->lim.limTimers.gLimAddtsRspTimer);
        tx_timer_delete(&pMac->lim.limTimers.gLimReassocFailureTimer);
        tx_timer_delete(&pMac->lim.limTimers.gLimAssocFailureTimer);
        tx_timer_delete(&pMac->lim.limTimers.gLimJoinFailureTimer);
        tx_timer_delete(&pMac->lim.limTimers.gLimPeriodicJoinProbeReqTimer);
        tx_timer_delete(&pMac->lim.limTimers.gLimQuietBssTimer);
        tx_timer_delete(&pMac->lim.limTimers.gLimQuietTimer);
        tx_timer_delete(&pMac->lim.limTimers.gLimChannelSwitchTimer);
        tx_timer_delete(&pMac->lim.limTimers.gLimMaxChannelTimer);
        tx_timer_delete(&pMac->lim.limTimers.gLimPeriodicProbeReqTimer);
        tx_timer_delete(&pMac->lim.limTimers.gLimMinChannelTimer);
        tx_timer_delete(&pMac->lim.limTimers.gLimP2pSingleShotNoaInsertTimer);
        tx_timer_delete(&pMac->lim.limTimers.gLimActiveToPassiveChannelTimer);

        if(NULL != pMac->lim.gLimPreAuthTimerTable.pTable)
        {
            vos_mem_free(pMac->lim.gLimPreAuthTimerTable.pTable);
            pMac->lim.gLimPreAuthTimerTable.pTable = NULL;
        }

        return TX_TIMER_ERROR;

} /*                                 */



/* 
                    
  
           
                               
                                                               
                                                       
                                                                    
                                       
                                             
                                                             
                      
  
        
  
              
     
  
       
     
  
                                                                  
  
               
 */

void
limTimerHandler(void *pMacGlobal, tANI_U32 param)
{
    tANI_U32         statusCode;
    tSirMsgQ    msg;
    tpAniSirGlobal pMac = (tpAniSirGlobal)pMacGlobal;

    //                                              

    msg.type = (tANI_U16) param;
    msg.bodyptr = NULL;
    msg.bodyval = 0;

    if ((statusCode = limPostMsgApi(pMac, &msg)) != eSIR_SUCCESS)
        limLog(pMac, LOGE,
               FL("posting message %X to LIM failed, reason=%d"),
               msg.type, statusCode);
} /*                                 */


/* 
                                 
  
           
                                                                      
  
        
                                                             
                                  
  
              
     
  
       
     
  
                                           
  
               
 */

void
limAddtsResponseTimerHandler(void *pMacGlobal, tANI_U32 param)
{
    tSirMsgQ    msg;
    tpAniSirGlobal pMac = (tpAniSirGlobal)pMacGlobal;

    //                                              

    msg.type = SIR_LIM_ADDTS_RSP_TIMEOUT;
    msg.bodyval = param;
    msg.bodyptr = NULL;

    limPostMsgApi(pMac, &msg);
} /*                                             */


/* 
                                
  
           
                                                                    
  
        
                                                            
                                  
  
              
     
  
       
     
  
                                           
  
               
 */

void
limAuthResponseTimerHandler(void *pMacGlobal, tANI_U32 param)
{
    tSirMsgQ    msg;
    tpAniSirGlobal pMac = (tpAniSirGlobal)pMacGlobal;

    //                                              

    msg.type = SIR_LIM_AUTH_RSP_TIMEOUT;
    msg.bodyptr = NULL;
    msg.bodyval = (tANI_U32)param;

    limPostMsgApi(pMac, &msg);
} /*                                             */



/* 
                                
  
           
                                                                 
         
  
        
                                                              
                                  
  
              
     
  
       
     
  
                                                             
                                  
               
 */

void
limAssocFailureTimerHandler(void *pMacGlobal, tANI_U32 param)
{
    tSirMsgQ    msg;
    tpAniSirGlobal pMac = (tpAniSirGlobal)pMacGlobal;

#if  defined (WLAN_FEATURE_VOWIFI_11R) || defined (FEATURE_WLAN_CCX) || defined(FEATURE_WLAN_LFR)
    if((LIM_REASSOC == param) &&
       (NULL != pMac->lim.pSessionEntry) &&
       (pMac->lim.pSessionEntry->limMlmState == eLIM_MLM_WT_FT_REASSOC_RSP_STATE))
    {
        limLog(pMac, LOGE, FL("Reassoc timeout happened"));
        if(pMac->lim.reAssocRetryAttempt < LIM_MAX_REASSOC_RETRY_LIMIT)
        {
            limSendRetryReassocReqFrame(pMac, pMac->lim.pSessionEntry->pLimMlmReassocRetryReq, pMac->lim.pSessionEntry);
            pMac->lim.reAssocRetryAttempt++;
            limLog(pMac, LOGW, FL("Reassoc request retry is sent %d times"), pMac->lim.reAssocRetryAttempt);
            return;
        }
        else
        {
            limLog(pMac, LOGW, FL("Reassoc request retry MAX(%d) reached"), LIM_MAX_REASSOC_RETRY_LIMIT);
            if(NULL != pMac->lim.pSessionEntry->pLimMlmReassocRetryReq)
            {
                vos_mem_free( pMac->lim.pSessionEntry->pLimMlmReassocRetryReq);
                pMac->lim.pSessionEntry->pLimMlmReassocRetryReq = NULL;
            }
        }
    }
#endif
    //                                              

    msg.type = SIR_LIM_ASSOC_FAIL_TIMEOUT;
    msg.bodyval = (tANI_U32)param;
    msg.bodyptr = NULL;

    limPostMsgApi(pMac, &msg);
} /*                                             */


/* 
                                   
  
           
                                                                  
         
  
        
                                                                      
                                  
  
              
     
  
       
     
  
         
  
               
 */
void
limUpdateOlbcCacheTimerHandler(void *pMacGlobal, tANI_U32 param)
{
    tSirMsgQ    msg;
    tpAniSirGlobal pMac = (tpAniSirGlobal)pMacGlobal;

    //                                              

    msg.type = SIR_LIM_UPDATE_OLBC_CACHEL_TIMEOUT;
    msg.bodyval = 0;
    msg.bodyptr = NULL;

    limPostMsgApi(pMac, &msg);
} /*                                                */

/* 
                                
  
           
                                                           
                           
  
        
  
              
     
  
       
     
  
                                                    
                                                                
                                                            
  
               
 */

void
limDeactivateAndChangeTimer(tpAniSirGlobal pMac, tANI_U32 timerId)
{
    tANI_U32    val=0, val1=0;

    MTRACE(macTrace(pMac, TRACE_CODE_TIMER_DEACTIVATE, NO_SESSION, timerId));

    switch (timerId)
    {
        case eLIM_ADDTS_RSP_TIMER:
            pMac->lim.gLimAddtsRspTimerCount++;
            if (tx_timer_deactivate(&pMac->lim.limTimers.gLimAddtsRspTimer) != TX_SUCCESS)
            {
                //                                    
                //          
                limLog(pMac, LOGP,
                       FL("Unable to deactivate AddtsRsp timer"));
            }
            break;

        case eLIM_MIN_CHANNEL_TIMER:
            if (tx_timer_deactivate(&pMac->lim.limTimers.gLimMinChannelTimer)
                                         != TX_SUCCESS)
            {
                //                                        
                //          
                limLog(pMac, LOGP,
                       FL("Unable to deactivate min channel timer"));
            }

#if 0
            //                                             
            //                                               
            //                                               
            //              
            if( eLIM_QUIET_RUNNING == pMac->lim.gLimSpecMgmt.quietState &&
                pMac->lim.gLimTriggerBackgroundScanDuringQuietBss )
            {
                //                                                
                //                                       
                val = pMac->lim.gLimSpecMgmt.quietDuration;
            }
            else
            {
#endif
                if(pMac->lim.gpLimMlmScanReq)
                {
                    val = SYS_MS_TO_TICKS(pMac->lim.gpLimMlmScanReq->minChannelTime);
                    if (pMac->btc.btcScanCompromise)
                    {
                        if (pMac->lim.gpLimMlmScanReq->minChannelTimeBtc)
                        {
                            val = SYS_MS_TO_TICKS(pMac->lim.gpLimMlmScanReq->minChannelTimeBtc);
                            limLog(pMac, LOG1, FL("Using BTC Min Active Scan time"));
                        }
                        else
                        {
                            limLog(pMac, LOGE, FL("BTC Active Scan Min Time is Not Set"));
                        }
                    }
                }
                else
                {
                    limLog(pMac, LOGE, FL(" gpLimMlmScanReq is NULL "));
                    //                                               
                    break;
                }
#if 0
            }
#endif

            if (tx_timer_change(&pMac->lim.limTimers.gLimMinChannelTimer,
                                val, 0) != TX_SUCCESS)
            {
                //                                    
                //          
                limLog(pMac, LOGP, FL("Unable to change min channel timer"));
            }

            break;

        case eLIM_PERIODIC_PROBE_REQ_TIMER:
            if (tx_timer_deactivate(&pMac->lim.limTimers.gLimPeriodicProbeReqTimer)
                                         != TX_SUCCESS)
            {
                //                                        
                //          
                limLog(pMac, LOGP,
                       FL("Unable to deactivate periodic timer"));
            }

            val = SYS_MS_TO_TICKS(pMac->lim.gpLimMlmScanReq->minChannelTime)/2;
            if (pMac->btc.btcScanCompromise)
            {
               if (pMac->lim.gpLimMlmScanReq->minChannelTimeBtc)
               {
                   val = SYS_MS_TO_TICKS(pMac->lim.gpLimMlmScanReq->minChannelTimeBtc)/2;
               }
               else
               {
                   limLog(pMac, LOGE, FL("BTC Active Scan Min Time is Not Set"));
               }
            }
            if (tx_timer_change(&pMac->lim.limTimers.gLimPeriodicProbeReqTimer,
                                val, 0) != TX_SUCCESS)
            {
                //                                    
                //          
                limLog(pMac, LOGP, FL("Unable to change periodic timer"));
            }

            break;

        case eLIM_MAX_CHANNEL_TIMER:
            if (tx_timer_deactivate(&pMac->lim.limTimers.gLimMaxChannelTimer)
                                      != TX_SUCCESS)
            {
                //                                        
                //          
                limLog(pMac, LOGP,
                       FL("Unable to deactivate max channel timer"));
            }

            //                                             
            //                                               
            //                                               
            //              
            if (pMac->lim.gLimSystemRole != eLIM_AP_ROLE)
            {
#if 0

                if( eLIM_QUIET_RUNNING == pMac->lim.gLimSpecMgmt.quietState &&
                    pMac->lim.gLimTriggerBackgroundScanDuringQuietBss )
                {
                    //                                                
                    //                                       
                    val = pMac->lim.gLimSpecMgmt.quietDuration;
                }
                else
                {
#endif
                    if(pMac->lim.gpLimMlmScanReq)
                    {
                        val = SYS_MS_TO_TICKS(pMac->lim.gpLimMlmScanReq->maxChannelTime);
                        if (pMac->btc.btcScanCompromise)
                        {
                            if (pMac->lim.gpLimMlmScanReq->maxChannelTimeBtc)
                            {
                                val = SYS_MS_TO_TICKS(pMac->lim.gpLimMlmScanReq->maxChannelTimeBtc);
                                limLog(pMac, LOG1, FL("Using BTC Max Active Scan time"));
                            }
                            else
                            {
                                limLog(pMac, LOGE, FL("BTC Active Scan Max Time is Not Set"));
                            }
                        }
                    }
                    else
                    {
                        limLog(pMac, LOGE, FL(" gpLimMlmScanReq is NULL "));
                        //                                               
                        break;
                    }
#if 0
                }
#endif
            }

            if (tx_timer_change(&pMac->lim.limTimers.gLimMaxChannelTimer,
                                val, 0) != TX_SUCCESS)
            {
                //                                    
                //          
                limLog(pMac, LOGP,
                       FL("Unable to change max channel timer"));
            }

            break;

        case eLIM_JOIN_FAIL_TIMER:
            if (tx_timer_deactivate(&pMac->lim.limTimers.gLimJoinFailureTimer)
                                         != TX_SUCCESS)
            {
                /* 
                                                    
                                    
                 */
                limLog(pMac, LOGP,
                       FL("Unable to deactivate Join Failure timer"));
            }

            if (wlan_cfgGetInt(pMac, WNI_CFG_JOIN_FAILURE_TIMEOUT,
                          &val) != eSIR_SUCCESS)
            {
                /* 
                                                         
                                       
                 */
                limLog(pMac, LOGP,
                   FL("could not retrieve JoinFailureTimeout value"));
            }
            val = SYS_MS_TO_TICKS(val);

            if (tx_timer_change(&pMac->lim.limTimers.gLimJoinFailureTimer,
                                val, 0) != TX_SUCCESS)
            {
                /* 
                                                
                                    
                 */
                limLog(pMac, LOGP,
                       FL("Unable to change Join Failure timer"));
            }

            break;

        case eLIM_PERIODIC_JOIN_PROBE_REQ_TIMER:
            if (tx_timer_deactivate(&pMac->lim.limTimers.gLimPeriodicJoinProbeReqTimer)
                                         != TX_SUCCESS)
            {
                //                                              
                limLog(pMac, LOGP,
                       FL("Unable to deactivate periodic join request timer"));
            }

            val = SYS_MS_TO_TICKS(LIM_JOIN_PROBE_REQ_TIMER_MS);
            if (tx_timer_change(&pMac->lim.limTimers.gLimPeriodicJoinProbeReqTimer,
                                val, 0) != TX_SUCCESS)
            {
                //                                          
                //          
                limLog(pMac, LOGP, FL("Unable to change periodic join request timer"));
            }

            break;

        case eLIM_AUTH_FAIL_TIMER:
            if (tx_timer_deactivate(&pMac->lim.limTimers.gLimAuthFailureTimer)
                                              != TX_SUCCESS)
            {
                //                                         
                //          
                limLog(pMac, LOGP,
                       FL("Unable to deactivate auth failure timer"));
            }

            //                                        
            if (wlan_cfgGetInt(pMac, WNI_CFG_AUTHENTICATE_FAILURE_TIMEOUT,
                          &val) != eSIR_SUCCESS)
            {
                /* 
                                                         
                                       
                 */
                limLog(pMac, LOGP,
                   FL("could not retrieve AuthFailureTimeout value"));
            }
            val = SYS_MS_TO_TICKS(val);

            if (tx_timer_change(&pMac->lim.limTimers.gLimAuthFailureTimer,
                                val, 0) != TX_SUCCESS)
            {
                //                                               
                //          
                limLog(pMac, LOGP,
                       FL("unable to change Auth failure timer"));
            }

            break;

        case eLIM_ASSOC_FAIL_TIMER:
            if (tx_timer_deactivate(&pMac->lim.limTimers.gLimAssocFailureTimer) !=
                                    TX_SUCCESS)
            {
                //                                                
                //          
                limLog(pMac, LOGP,
                   FL("unable to deactivate Association failure timer"));
            }

            //                                        
            if (wlan_cfgGetInt(pMac, WNI_CFG_ASSOCIATION_FAILURE_TIMEOUT,
                          &val) != eSIR_SUCCESS)
            {
                /* 
                                                          
                                       
                 */
                limLog(pMac, LOGP,
                   FL("could not retrieve AssocFailureTimeout value"));
            }
            val = SYS_MS_TO_TICKS(val);

            if (tx_timer_change(&pMac->lim.limTimers.gLimAssocFailureTimer,
                                val, 0) != TX_SUCCESS)
            {
                //                                            
                //          
                limLog(pMac, LOGP,
                       FL("unable to change Assoc failure timer"));
            }

            break;

        case eLIM_REASSOC_FAIL_TIMER:
            if (tx_timer_deactivate(&pMac->lim.limTimers.gLimReassocFailureTimer) !=
                                    TX_SUCCESS)
            {
                //                                                  
                //          
                limLog(pMac, LOGP,
                   FL("unable to deactivate Reassoc failure timer"));
            }

            //                                        
            if (wlan_cfgGetInt(pMac, WNI_CFG_REASSOCIATION_FAILURE_TIMEOUT,
                          &val) != eSIR_SUCCESS)
            {
                /* 
                                                            
                                       
                 */
                limLog(pMac, LOGP,
                   FL("could not retrieve ReassocFailureTimeout value"));
            }
            val = SYS_MS_TO_TICKS(val);

            if (tx_timer_change(&pMac->lim.limTimers.gLimReassocFailureTimer,
                                val, 0) != TX_SUCCESS)
            {
                //                                              
                //          
                limLog(pMac, LOGP,
                   FL("unable to change Reassociation failure timer"));
            }

            break;

        case eLIM_HEART_BEAT_TIMER:
            if (tx_timer_deactivate(&pMac->lim.limTimers.gLimHeartBeatTimer) !=
                                    TX_SUCCESS)
            {
                //                                      
                //          
                limLog(pMac, LOGP,
                       FL("unable to deactivate Heartbeat timer"));
            }
            else
            {
                limLog(pMac, LOGW, FL("Deactivated heartbeat link monitoring"));
            }

            if (wlan_cfgGetInt(pMac, WNI_CFG_BEACON_INTERVAL,
                          &val) != eSIR_SUCCESS)
            {
                /* 
                                                      
                                       
                 */
                limLog(pMac, LOGP,
                       FL("could not retrieve BEACON_INTERVAL value"));
            }

            if (wlan_cfgGetInt(pMac, WNI_CFG_HEART_BEAT_THRESHOLD, &val1) !=
                                                          eSIR_SUCCESS)
                limLog(pMac, LOGP,
                   FL("could not retrieve heartbeat failure value"));

            //                                        
            val = SYS_MS_TO_TICKS(val * val1);

            if (tx_timer_change(&pMac->lim.limTimers.gLimHeartBeatTimer,
                                val, 0) != TX_SUCCESS)
            {
                //                                  
                //          
                limLog(pMac, LOGP,
                       FL("unable to change HeartBeat timer"));
            }
            else
            {
                limLog(pMac, LOGW, FL("HeartBeat timer value is changed = %lu"), val);
            }
            break;

        case eLIM_PROBE_AFTER_HB_TIMER:
            if (tx_timer_deactivate(&pMac->lim.limTimers.gLimProbeAfterHBTimer) !=
                                    TX_SUCCESS)
            {
                //                                      
                //          
                limLog(pMac, LOGP,
                   FL("unable to deactivate probeAfterHBTimer"));
            }
            else
            {
                limLog(pMac, LOG1, FL("Deactivated probe after hb timer"));
            }

            if (wlan_cfgGetInt(pMac, WNI_CFG_PROBE_AFTER_HB_FAIL_TIMEOUT,
                          &val) != eSIR_SUCCESS)
            {
                /* 
                                                       
                                             
                 */
                limLog(pMac, LOGP,
                   FL("could not retrieve PROBE_AFTER_HB_FAIL_TIMEOUT value"));
            }

            //                                        
            val = SYS_MS_TO_TICKS(val);

            if (tx_timer_change(&pMac->lim.limTimers.gLimProbeAfterHBTimer,
                                val, 0) != TX_SUCCESS)
            {
                //                                  
                //          
                limLog(pMac, LOGP,
                       FL("unable to change ProbeAfterHBTimer"));
            }
            else
            {
                limLog(pMac, LOGW, FL("Probe after HB timer value is changed = %lu"), val);
            }

            break;

        case eLIM_KEEPALIVE_TIMER:
            if (tx_timer_deactivate(&pMac->lim.limTimers.gLimKeepaliveTimer)
                            != TX_SUCCESS)
            {
                //                                      
                //          
                limLog(pMac, LOGP,
                   FL("unable to deactivate KeepaliveTimer timer"));
            }

            //                                        

            if (wlan_cfgGetInt(pMac, WNI_CFG_KEEPALIVE_TIMEOUT,
                          &val) != eSIR_SUCCESS)
            {
                /* 
                                                        
                                       
                 */
                limLog(pMac, LOGP,
                   FL("could not retrieve keepalive timeout value"));
            }
            if (val == 0)
            {
                val = 3000;
                pMac->sch.keepAlive = 0;
            } else
                pMac->sch.keepAlive = 1;



            val = SYS_MS_TO_TICKS(val + SYS_TICK_DUR_MS - 1);

            if (tx_timer_change(&pMac->lim.limTimers.gLimKeepaliveTimer,
                                val, val) != TX_SUCCESS)
            {
                //                                       
                //          
                limLog(pMac, LOGP,
                   FL("unable to change KeepaliveTimer timer"));
            }

            break;

        case eLIM_BACKGROUND_SCAN_TIMER:
            if (tx_timer_deactivate(&pMac->lim.limTimers.gLimBackgroundScanTimer)
                            != TX_SUCCESS)
            {
                //                                                
                //          
                limLog(pMac, LOGP,
                   FL("unable to deactivate BackgroundScanTimer timer"));
            }

            //                                        
            if (wlan_cfgGetInt(pMac, WNI_CFG_BACKGROUND_SCAN_PERIOD,
                          &val) != eSIR_SUCCESS)
            {
                /* 
                                                             
                                       
                 */
                limLog(pMac, LOGP,
                   FL("could not retrieve Background scan period value"));
            }
            if (val == 0)
            {
                val = LIM_BACKGROUND_SCAN_PERIOD_DEFAULT_MS;
                pMac->lim.gLimBackgroundScanDisable = true;
            }
            else
                pMac->lim.gLimBackgroundScanDisable = false;

            val = SYS_MS_TO_TICKS(val);

            if (tx_timer_change(&pMac->lim.limTimers.gLimBackgroundScanTimer,
                                val, val) != TX_SUCCESS)
            {
                //                                            
                //          
                limLog(pMac, LOGP,
                   FL("unable to change BackgroundScanTimer timer"));
            }

            break;

#if 0
        case eLIM_CHANNEL_SWITCH_TIMER:
            if (tx_timer_deactivate(&pMac->lim.limTimers.gLimChannelSwitchTimer) != eSIR_SUCCESS)
            {
                limLog(pMac, LOGP, FL("tx_timer_deactivate failed!"));
                return;
            }

            if (tx_timer_change(&pMac->lim.limTimers.gLimChannelSwitchTimer,
                        pMac->lim.gLimChannelSwitch.switchTimeoutValue,
                                    0) != TX_SUCCESS)
            {
                limLog(pMac, LOGP, FL("tx_timer_change failed "));
                return;
            }
            break;
#endif

        case eLIM_LEARN_DURATION_TIMER:
            break;

#if 0
        case eLIM_QUIET_BSS_TIMER:
            if (TX_SUCCESS !=
            tx_timer_deactivate(&pMac->lim.limTimers.gLimQuietBssTimer))
            {
                limLog( pMac, LOGE,
                  FL("Unable to de-activate gLimQuietBssTimer! Will attempt to activate anyway..."));
            }

            //                                                  
            //             
            if (TX_SUCCESS !=
                tx_timer_change( &pMac->lim.limTimers.gLimQuietBssTimer,
                  pMac->lim.gLimSpecMgmt.quietDuration,
                  0))
            {
                limLog( pMac, LOGE,
                  FL("Unable to change gLimQuietBssTimer! Will still attempt to activate anyway..."));
            }
            break;

        case eLIM_QUIET_TIMER:
            if( TX_SUCCESS != tx_timer_deactivate(&pMac->lim.limTimers.gLimQuietTimer))
            {
                limLog( pMac, LOGE,
                    FL( "Unable to deactivate gLimQuietTimer! Will still attempt to re-activate anyway..." ));
            }

            //                                    
            if( TX_SUCCESS != tx_timer_change( &pMac->lim.limTimers.gLimQuietTimer,
                              SYS_MS_TO_TICKS(pMac->lim.gLimSpecMgmt.quietTimeoutValue), 0))
            {
                limLog( pMac, LOGE,
                    FL( "Unable to change gLimQuietTimer! Will still attempt to re-activate anyway..." ));
            }
            break;
#endif

#if 0
        case eLIM_WPS_OVERLAP_TIMER:
            {
            //                             

              tANI_U32 WPSOverlapTimer = SYS_MS_TO_TICKS(LIM_WPS_OVERLAP_TIMER_MS);

              if (tx_timer_deactivate(
                     &pMac->lim.limTimers.gLimWPSOverlapTimerObj.gLimWPSOverlapTimer) != TX_SUCCESS)
              {
                  //                                           
                  //          
                  limLog(pMac, LOGP,
                         FL("Unable to deactivate WPS overlap timer"));
              }

              if (tx_timer_change(
                         &pMac->lim.limTimers.gLimWPSOverlapTimerObj.gLimWPSOverlapTimer,
                         WPSOverlapTimer, 0) != TX_SUCCESS)
              {
                  //                                       
                  //          
                  limLog(pMac, LOGP, FL("Unable to change WPS overlap timer"));

                  return;
              }

              limLog( pMac, LOGE,
                  FL("Setting WPS overlap TIMER to %d ticks"),
                  WPSOverlapTimer);
            }
            break;
#endif

#ifdef WLAN_FEATURE_VOWIFI_11R
        case eLIM_FT_PREAUTH_RSP_TIMER:
            if (tx_timer_deactivate(&pMac->lim.limTimers.gLimFTPreAuthRspTimer) != TX_SUCCESS)
            {
                /* 
                                                    
                                    
                 */
                limLog(pMac, LOGP, FL("Unable to deactivate Preauth response Failure timer"));
                return;
            }
            val = 1000;
            val = SYS_MS_TO_TICKS(val);
            if (tx_timer_change(&pMac->lim.limTimers.gLimFTPreAuthRspTimer,
                                                val, 0) != TX_SUCCESS)
            {
                /* 
                                               
                                   
                */
                limLog(pMac, LOGP, FL("Unable to change Join Failure timer"));
                return;
            }
            break;
#endif
#if defined(FEATURE_WLAN_CCX) && !defined(FEATURE_WLAN_CCX_UPLOAD)
         case eLIM_TSM_TIMER:
             if (tx_timer_deactivate(&pMac->lim.limTimers.gLimCcxTsmTimer)
                                                                != TX_SUCCESS)
             {
                 limLog(pMac, LOGE, FL("Unable to deactivate TSM timer"));
             }
             break;
#endif /*                                              */
        case eLIM_REMAIN_CHN_TIMER:
            if (tx_timer_deactivate(&pMac->lim.limTimers.gLimRemainOnChannelTimer) != TX_SUCCESS)
            {
                /* 
                                                    
                                    
                 */
                limLog(pMac, LOGP, FL("Unable to deactivate Remain on Chn timer"));
                return;
            }
            val = 1000;
            val = SYS_MS_TO_TICKS(val);
            if (tx_timer_change(&pMac->lim.limTimers.gLimRemainOnChannelTimer,
                                                val, 0) != TX_SUCCESS)
            {
                /* 
                                               
                                   
                */
                limLog(pMac, LOGP, FL("Unable to change timer"));
                return;
            }
            break;

    case eLIM_CONVERT_ACTIVE_CHANNEL_TO_PASSIVE:
            if (tx_timer_deactivate(&pMac->lim.limTimers.gLimActiveToPassiveChannelTimer) != TX_SUCCESS)
            {
                /* 
                                                                        
                             
                 */
                limLog(pMac, LOGP, FL("Unable to Deactivate "
                                      "Active to passive channel timer"));
                return;
            }
            val = ACTIVE_TO_PASSIVE_CONVERISON_TIMEOUT;
            val = SYS_MS_TO_TICKS(val);
            if (tx_timer_change(&pMac->lim.limTimers.gLimActiveToPassiveChannelTimer,
                                                val, 0) != TX_SUCCESS)
            {
                /* 
                                                                                
                                   
                */
                limLog(pMac, LOGP, FL("Unable to change timer"));
                return;
            }
            break;

    case eLIM_DISASSOC_ACK_TIMER:
            if (tx_timer_deactivate(&pMac->lim.limTimers.gLimDisassocAckTimer) != TX_SUCCESS)
            {
                /* 
                                                    
                                    
                 */
                limLog(pMac, LOGP, FL("Unable to deactivate Disassoc ack timer"));
                return;
            }
            val = 1000;
            val = SYS_MS_TO_TICKS(val);
            if (tx_timer_change(&pMac->lim.limTimers.gLimDisassocAckTimer,
                                                val, 0) != TX_SUCCESS)
            {
                /* 
                                               
                                   
                */
                limLog(pMac, LOGP, FL("Unable to change timer"));
                return;
            }
            break;

    case eLIM_DEAUTH_ACK_TIMER:
            if (tx_timer_deactivate(&pMac->lim.limTimers.gLimDeauthAckTimer) != TX_SUCCESS)
            {
                /* 
                                                    
                                    
                 */
                limLog(pMac, LOGP, FL("Unable to deactivate Deauth ack timer"));
                return;
            }
            val = 1000;
            val = SYS_MS_TO_TICKS(val);
            if (tx_timer_change(&pMac->lim.limTimers.gLimDeauthAckTimer,
                                                val, 0) != TX_SUCCESS)
            {
                /* 
                                               
                                   
                */
                limLog(pMac, LOGP, FL("Unable to change timer"));
                return;
            }
            break;

    case eLIM_INSERT_SINGLESHOT_NOA_TIMER:
        if (tx_timer_deactivate(&pMac->lim.limTimers.gLimP2pSingleShotNoaInsertTimer) != TX_SUCCESS)
        {
            /* 
                                                    
                           
        */
            limLog(pMac, LOGP, FL("Unable to deactivate SingleShot NOA Insert timer"));
            return;
        }
        val = LIM_INSERT_SINGLESHOTNOA_TIMEOUT_VALUE;
        val = SYS_MS_TO_TICKS(val);
        if (tx_timer_change(&pMac->lim.limTimers.gLimP2pSingleShotNoaInsertTimer,
                                            val, 0) != TX_SUCCESS)
        {
            /* 
                                                
                          
       */
            limLog(pMac, LOGP, FL("Unable to change timer"));
            return;
        }
        break;

        default:
            //                           
            break;
    }
} /*                                             */



/*                                                                
                                            
                                                            
                                     
 
           
                    
            
                                                                  */
void
limHeartBeatDeactivateAndChangeTimer(tpAniSirGlobal pMac, tpPESession psessionEntry)
{
   tANI_U32    val, val1;

   MTRACE(macTrace(pMac, TRACE_CODE_TIMER_DEACTIVATE, psessionEntry->peSessionId, eLIM_HEART_BEAT_TIMER));
#ifdef WLAN_ACTIVEMODE_OFFLOAD_FEATURE
   if(IS_ACTIVEMODE_OFFLOAD_FEATURE_ENABLE)
      return;
#endif

   if (tx_timer_deactivate(&pMac->lim.limTimers.gLimHeartBeatTimer) != TX_SUCCESS)
      limLog(pMac, LOGP, FL("Fail to deactivate HeartBeatTimer "));

   /*                                                                              
                                                                              
                                                                                       */

   //                                                 
   val = LIM_HB_TIMER_BEACON_INTERVAL;

   if (wlan_cfgGetInt(pMac, WNI_CFG_HEART_BEAT_THRESHOLD, &val1) != eSIR_SUCCESS)
      limLog(pMac, LOGP, FL("Fail to get WNI_CFG_HEART_BEAT_THRESHOLD "));

   PELOGW(limLog(pMac,LOGW,
            FL("HB Timer Int.=100ms * %d, Beacon Int.=%dms,Session Id=%d "),
            val1, psessionEntry->beaconParams.beaconInterval,
            psessionEntry->peSessionId);)

   /*                                                                     
                                                                             
                                              
                                                  
    */
   if (eSIR_IBSS_MODE == psessionEntry->bssType &&
         pMac->lim.gLimNumIbssPeers > 0)
   {
      val1 = val1 * pMac->lim.gLimNumIbssPeers;
   }

   //                                        
   val = SYS_MS_TO_TICKS(val * val1);

   if (tx_timer_change(&pMac->lim.limTimers.gLimHeartBeatTimer, val, 0) != TX_SUCCESS)
      limLog(pMac, LOGP, FL("Fail to change HeartBeatTimer"));

} /*                                                      */


/*                                                                
                                   
                                                        
                         
 
                                             
                    
            
                                                                  */
void
limReactivateHeartBeatTimer(tpAniSirGlobal pMac, tpPESession psessionEntry)
{
    PELOG3(limLog(pMac, LOG3, FL("Rxed Heartbeat. Count=%d"), psessionEntry->LimRxedBeaconCntDuringHB);)

#ifdef WLAN_ACTIVEMODE_OFFLOAD_FEATURE
    if(IS_ACTIVEMODE_OFFLOAD_FEATURE_ENABLE)
    {
       limLog(pMac, LOGW, FL("Active offload feature is enabled, FW takes care of HB monitoring"));
       return;
    }
#endif

    limHeartBeatDeactivateAndChangeTimer(pMac, psessionEntry);
    MTRACE(macTrace(pMac, TRACE_CODE_TIMER_ACTIVATE, psessionEntry->peSessionId, eLIM_HEART_BEAT_TIMER));

    //                                                              
    if(pMac->lim.limTimers.gLimHeartBeatTimer.initScheduleTimeInMsecs > 0)
    {
       /*
                                                                       
                                                                           
                                                                       
                            
        */
       if (psessionEntry->limSystemRole == eLIM_STA_IN_IBSS_ROLE &&
             IS_IBSS_HEARTBEAT_OFFLOAD_FEATURE_ENABLE)
       {
          if (tx_timer_deactivate(&pMac->lim.limTimers.gLimHeartBeatTimer)!= TX_SUCCESS)
          {
             limLog(pMac, LOGP,FL("IBSS HeartBeat Offloaded, Could not deactivate Heartbeat timer"));
          }
          else
          {
             limLog(pMac, LOGE, FL("IBSS HeartBeat Offloaded, Deactivated heartbeat link monitoring"));
          }
       }
       else
       {
          if (tx_timer_activate(&pMac->lim.limTimers.gLimHeartBeatTimer)!= TX_SUCCESS)
          {
             limLog(pMac, LOGP,FL("could not activate Heartbeat timer"));
          }
          else
          {
             limLog(pMac, LOGW, FL("Reactivated heartbeat link monitoring"));
          }
       }
       limResetHBPktCount(psessionEntry);
    }

} /*                                             */


/* 
                             
  
  
                                                              
  
        
  
              
     
  
                                                            
  
                                                    
                                        
  
                                          
                                           
 */
v_UINT_t limActivateHearBeatTimer(tpAniSirGlobal pMac, tpPESession psessionEntry)
{
    v_UINT_t status = TX_TIMER_ERROR;

#ifdef WLAN_ACTIVEMODE_OFFLOAD_FEATURE
    if(IS_ACTIVEMODE_OFFLOAD_FEATURE_ENABLE)
       return (TX_SUCCESS);
#endif

    if(TX_AIRGO_TMR_SIGNATURE == pMac->lim.limTimers.gLimHeartBeatTimer.tmrSignature)
    {
        //                             
        if( pMac->lim.limTimers.gLimHeartBeatTimer.initScheduleTimeInMsecs )
        {
           if (psessionEntry->limSystemRole == eLIM_STA_IN_IBSS_ROLE &&
               IS_IBSS_HEARTBEAT_OFFLOAD_FEATURE_ENABLE)
           {
              /*                         */
              status = tx_timer_deactivate(&pMac->lim.limTimers.gLimHeartBeatTimer);
              if (TX_SUCCESS != status)
              {
                 PELOGE(limLog(pMac, LOGE,
                 FL("IBSS HB Offload, Could not deactivate HB timer status(%d)"),
                 status);)
              }
              else
              {
                 PELOGE(limLog(pMac, LOGE,
                 FL("%s] IBSS HB Offloaded, Heartbeat timer deactivated"),
                 __func__);)
              }

           }
           else
           {
              status = tx_timer_activate(&pMac->lim.limTimers.gLimHeartBeatTimer);
              if ( TX_SUCCESS != status )
              {
                 PELOGE(limLog(pMac, LOGE,
                 FL("could not activate Heartbeat timer status(%d)"), status);)
              }
              else
              {
                 PELOGE(limLog(pMac, LOGW,
                 FL("%s] Activated Heartbeat timer status(%d)"), __func__, status);)
              }
           }
        }
        else
        {
            status = TX_SUCCESS;
        }
    }

    return (status);
}



/* 
                                        
  
  
                                                                           
                           
  
        
  
              
     
  
                                                            
  
                                                    
                                                                
                                                            
                          
  
               
 */

void
limDeactivateAndChangePerStaIdTimer(tpAniSirGlobal pMac, tANI_U32 timerId, tANI_U16 staId)
{
    tANI_U32    val;
    MTRACE(macTrace(pMac, TRACE_CODE_TIMER_DEACTIVATE, NO_SESSION, timerId));

    switch (timerId)
    {
        case eLIM_CNF_WAIT_TIMER:

            if (tx_timer_deactivate(&pMac->lim.limTimers.gpLimCnfWaitTimer[staId])
                            != TX_SUCCESS)
            {
                 limLog(pMac, LOGP,
                       FL("unable to deactivate CNF wait timer"));

            }

            //                                        

            if (wlan_cfgGetInt(pMac, WNI_CFG_WT_CNF_TIMEOUT,
                          &val) != eSIR_SUCCESS)
            {
                /* 
                                                  
                                       
                 */
                limLog(pMac, LOGP,
                   FL("could not retrieve cnf timeout value"));
            }
            val = SYS_MS_TO_TICKS(val);

            if (tx_timer_change(&pMac->lim.limTimers.gpLimCnfWaitTimer[staId],
                                val, val) != TX_SUCCESS)
            {
                //                            
                //          
                limLog(pMac, LOGP, FL("unable to change cnf wait timer"));
            }

            break;

        case eLIM_AUTH_RSP_TIMER:
        {
            tLimPreAuthNode *pAuthNode;

            pAuthNode = limGetPreAuthNodeFromIndex(pMac, &pMac->lim.gLimPreAuthTimerTable, staId);

            if (pAuthNode == NULL)
            {
                limLog(pMac, LOGP, FL("Invalid Pre Auth Index passed :%d"), staId);
                break;
            }

            if (tx_timer_deactivate(&pAuthNode->timer) != TX_SUCCESS)
            {
                //                                          
                //          
                limLog(pMac, LOGP, FL("unable to deactivate auth response timer"));
            }

            //                                        

            if (wlan_cfgGetInt(pMac, WNI_CFG_AUTHENTICATE_RSP_TIMEOUT, &val) != eSIR_SUCCESS)
            {
                /* 
                                                       
                                       
                 */
                limLog(pMac, LOGP,
                   FL("could not retrieve auth response timeout value"));
            }

            val = SYS_MS_TO_TICKS(val);

            if (tx_timer_change(&pAuthNode->timer, val, 0) != TX_SUCCESS)
            {
                //                                 
                //          
                limLog(pMac, LOGP, FL("unable to change auth rsp timer"));
            }
        }
            break;


        default:
            //                           
            break;

    }
}


/* 
                        
  
           
                                                      
  
        
  
              
     
  
       
     
  
                                                    
                          
  
               
 */

void limActivateCnfTimer(tpAniSirGlobal pMac, tANI_U16 staId, tpPESession psessionEntry)
{
    MTRACE(macTrace(pMac, TRACE_CODE_TIMER_ACTIVATE, psessionEntry->peSessionId, eLIM_CNF_WAIT_TIMER));
    pMac->lim.limTimers.gpLimCnfWaitTimer[staId].sessionId = psessionEntry->peSessionId;
    if (tx_timer_activate(&pMac->lim.limTimers.gpLimCnfWaitTimer[staId])
                != TX_SUCCESS)
    {
                limLog(pMac, LOGP,
                   FL("could not activate cnf wait timer"));
    }
}

/* 
                            
  
           
                                                      
  
        
  
              
     
  
       
     
  
                                                    
                       
  
               
 */

void limActivateAuthRspTimer(tpAniSirGlobal pMac, tLimPreAuthNode *pAuthNode)
{
    MTRACE(macTrace(pMac, TRACE_CODE_TIMER_ACTIVATE, NO_SESSION, eLIM_AUTH_RESP_TIMER));
    if (tx_timer_activate(&pAuthNode->timer) != TX_SUCCESS)
    {
        //                                    
        //          
        limLog(pMac, LOGP,
               FL("could not activate auth rsp timer"));
    }
}


/* 
                                         
  
           
                                                                                
  
        
  
              
  
       
     
  
         
  
               
 */

void
limSendDisassocFrameThresholdHandler(void *pMacGlobal, tANI_U32 param)
{
    tSirMsgQ    msg;
    tANI_U32         statusCode;
    tpAniSirGlobal pMac = (tpAniSirGlobal)pMacGlobal;

    msg.type = SIR_LIM_HASH_MISS_THRES_TIMEOUT;
    msg.bodyval = 0;
    msg.bodyptr = NULL;

    if ((statusCode = limPostMsgApi(pMac, &msg)) != eSIR_SUCCESS)
            limLog(pMac, LOGE,
        FL("posting to LIM failed, reason=%d"), statusCode);

}

/* 
                               
  
           
                                                                        
  
        
  
              
  
       
     
  
         
  
               
 */

void
limCnfWaitTmerHandler(void *pMacGlobal, tANI_U32 param)
{
    tSirMsgQ    msg;
    tANI_U32         statusCode;
    tpAniSirGlobal pMac = (tpAniSirGlobal)pMacGlobal;

    msg.type = SIR_LIM_CNF_WAIT_TIMEOUT;
    msg.bodyval = (tANI_U32)param;
    msg.bodyptr = NULL;

    if ((statusCode = limPostMsgApi(pMac, &msg)) != eSIR_SUCCESS)
            limLog(pMac, LOGE,
        FL("posting to LIM failed, reason=%d"), statusCode);

}

/* 
                            
  
           
                                                                 
  
        
  
              
  
       
     
  
         
  
               
 */

void
limKeepaliveTmerHandler(void *pMacGlobal, tANI_U32 param)
{
    tSirMsgQ    msg;
    tANI_U32         statusCode;
    tpAniSirGlobal pMac = (tpAniSirGlobal)pMacGlobal;

    msg.type = SIR_LIM_KEEPALIVE_TIMEOUT;
    msg.bodyval = (tANI_U32)param;
    msg.bodyptr = NULL;

    if ((statusCode = limPostMsgApi(pMac, &msg)) != eSIR_SUCCESS)
        limLog(pMac, LOGE,
               FL("posting to LIM failed, reason=%d"), statusCode);

}

void
limChannelSwitchTimerHandler(void *pMacGlobal, tANI_U32 param)
{
    tSirMsgQ    msg;
    tpAniSirGlobal pMac = (tpAniSirGlobal)pMacGlobal;

    PELOG1(limLog(pMac, LOG1,
        FL("ChannelSwitch Timer expired.  Posting msg to LIM "));)

    msg.type = SIR_LIM_CHANNEL_SWITCH_TIMEOUT;
    msg.bodyval = (tANI_U32)param;
    msg.bodyptr = NULL;

    limPostMsgApi(pMac, &msg);
}

void
limQuietTimerHandler(void *pMacGlobal, tANI_U32 param)
{
    tSirMsgQ    msg;
    tpAniSirGlobal pMac = (tpAniSirGlobal)pMacGlobal;

    msg.type = SIR_LIM_QUIET_TIMEOUT;
    msg.bodyval = (tANI_U32)param;
    msg.bodyptr = NULL;

    PELOG1(limLog(pMac, LOG1,
        FL("Post SIR_LIM_QUIET_TIMEOUT msg. "));)
    limPostMsgApi(pMac, &msg);
}

void
limQuietBssTimerHandler(void *pMacGlobal, tANI_U32 param)
{
    tSirMsgQ    msg;
    tpAniSirGlobal pMac = (tpAniSirGlobal)pMacGlobal;

    msg.type = SIR_LIM_QUIET_BSS_TIMEOUT;
    msg.bodyval = (tANI_U32)param;
    msg.bodyptr = NULL;
    PELOG1(limLog(pMac, LOG1,
        FL("Post SIR_LIM_QUIET_BSS_TIMEOUT msg. "));)
    limPostMsgApi(pMac, &msg);
}
#if 0
void
limWPSOverlapTimerHandler(void *pMacGlobal, tANI_U32 param)
{
    tSirMsgQ    msg;
    tpAniSirGlobal pMac = (tpAniSirGlobal)pMacGlobal;

    msg.type = SIR_LIM_WPS_OVERLAP_TIMEOUT;
    msg.bodyval = (tANI_U32)param;
    msg.bodyptr = NULL;
    PELOG1(limLog(pMac, LOG1,
        FL("Post SIR_LIM_WPS_OVERLAP_TIMEOUT msg. "));)
    limPostMsgApi(pMac, &msg);
}
#endif

#ifdef WLAN_ACTIVEMODE_OFFLOAD_FEATURE
/*                        */
/* 
                                
  
           
                                                      
                  
  
        
  
              
     
  
       
     
  
                           
  
               
 */
void
limMissedBeaconInActiveMode(void *pMacGlobal, tpPESession psessionEntry)
{
    tANI_U32         statusCode;
    tSirMsgQ    msg;
    tpAniSirGlobal pMac = (tpAniSirGlobal)pMacGlobal;

    //                                              
    if(IS_ACTIVEMODE_OFFLOAD_FEATURE_ENABLE)
    {
       msg.type = (tANI_U16) SIR_LIM_HEART_BEAT_TIMEOUT;
       msg.bodyptr = psessionEntry;
       msg.bodyval = 0;
       limLog(pMac, LOGE,
                 FL("Heartbeat failure from Riva"));
       if ((statusCode = limPostMsgApi(pMac, &msg)) != eSIR_SUCCESS)
          limLog(pMac, LOGE,
                 FL("posting message %X to LIM failed, reason=%d"),
                 msg.type, statusCode);
    }
}
#endif

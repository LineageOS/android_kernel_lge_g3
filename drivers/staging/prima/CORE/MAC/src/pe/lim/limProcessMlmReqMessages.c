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
#include "palTypes.h"
#include "wniCfgSta.h"
#include "aniGlobal.h"
#include "sirApi.h"
#include "sirParams.h"
#include "cfgApi.h"

#include "schApi.h"
#include "utilsApi.h"
#include "limUtils.h"
#include "limAssocUtils.h"
#include "limPropExtsUtils.h"
#include "limSecurityUtils.h"
#include "limSendMessages.h"
#include "pmmApi.h"
#include "limSendMessages.h"
//                            
#include "limSessionUtils.h"
#ifdef WLAN_FEATURE_VOWIFI_11R
#include <limFT.h>
#endif
#ifdef FEATURE_WLAN_DIAG_SUPPORT_LIM
#include "vos_diag_core_log.h"
#endif


//                                      
static void limProcessMlmStartReq(tpAniSirGlobal, tANI_U32 *);
static void limProcessMlmScanReq(tpAniSirGlobal, tANI_U32 *);
#ifdef FEATURE_OEM_DATA_SUPPORT
static void limProcessMlmOemDataReq(tpAniSirGlobal, tANI_U32 *);
#endif
static void limProcessMlmJoinReq(tpAniSirGlobal, tANI_U32 *);
static void limProcessMlmAuthReq(tpAniSirGlobal, tANI_U32 *);
static void limProcessMlmAssocReq(tpAniSirGlobal, tANI_U32 *);
static void limProcessMlmReassocReq(tpAniSirGlobal, tANI_U32 *);
static void limProcessMlmDisassocReq(tpAniSirGlobal, tANI_U32 *);
static void limProcessMlmDeauthReq(tpAniSirGlobal, tANI_U32 *);
static void limProcessMlmSetKeysReq(tpAniSirGlobal, tANI_U32 *);

static void limProcessMlmAddBAReq( tpAniSirGlobal, tANI_U32 * );
static void limProcessMlmAddBARsp( tpAniSirGlobal, tANI_U32 * );
static void limProcessMlmDelBAReq( tpAniSirGlobal, tANI_U32 * );

//                                    
static void limProcessMinChannelTimeout(tpAniSirGlobal);
static void limProcessMaxChannelTimeout(tpAniSirGlobal);
static void limProcessPeriodicProbeReqTimer(tpAniSirGlobal pMac);
static void limProcessJoinFailureTimeout(tpAniSirGlobal);
static void limProcessAuthFailureTimeout(tpAniSirGlobal);
static void limProcessAuthRspTimeout(tpAniSirGlobal, tANI_U32);
static void limProcessAssocFailureTimeout(tpAniSirGlobal, tANI_U32);
static void limProcessPeriodicJoinProbeReqTimer(tpAniSirGlobal);

static void limProcessMlmRemoveKeyReq(tpAniSirGlobal pMac, tANI_U32 * pMsgBuf);
void 
limSetChannel(tpAniSirGlobal pMac, tANI_U8 channel, tANI_U8 secChannelOffset, tPowerdBm maxTxPower, tANI_U8 peSessionId);
#define IS_MLM_SCAN_REQ_BACKGROUND_SCAN_AGGRESSIVE(pMac)    (pMac->lim.gpLimMlmScanReq->backgroundScanMode == eSIR_AGGRESSIVE_BACKGROUND_SCAN)
#define IS_MLM_SCAN_REQ_BACKGROUND_SCAN_NORMAL(pMac)        (pMac->lim.gpLimMlmScanReq->backgroundScanMode == eSIR_NORMAL_BACKGROUND_SCAN)

/* 
                             
  
           
                                                       
                                                  
  
        
                                                                
          
  
              
                                                                
                                                                     
                      
                                                                   
                          
  
       
  
                                                   
                                                            
                                                       
  
               
 */

void
limProcessMlmReqMessages(tpAniSirGlobal pMac, tpSirMsgQ Msg)
{
    switch (Msg->type)
    {
        case LIM_MLM_START_REQ:             limProcessMlmStartReq(pMac, Msg->bodyptr);   break;
        case LIM_MLM_SCAN_REQ:              limProcessMlmScanReq(pMac, Msg->bodyptr);    break;
#ifdef FEATURE_OEM_DATA_SUPPORT
        case LIM_MLM_OEM_DATA_REQ: limProcessMlmOemDataReq(pMac, Msg->bodyptr); break;
#endif
        case LIM_MLM_JOIN_REQ:              limProcessMlmJoinReq(pMac, Msg->bodyptr);    break;
        case LIM_MLM_AUTH_REQ:              limProcessMlmAuthReq(pMac, Msg->bodyptr);    break;
        case LIM_MLM_ASSOC_REQ:             limProcessMlmAssocReq(pMac, Msg->bodyptr);   break;
        case LIM_MLM_REASSOC_REQ:           limProcessMlmReassocReq(pMac, Msg->bodyptr); break;
        case LIM_MLM_DISASSOC_REQ:          limProcessMlmDisassocReq(pMac, Msg->bodyptr);  break;
        case LIM_MLM_DEAUTH_REQ:            limProcessMlmDeauthReq(pMac, Msg->bodyptr);  break;
        case LIM_MLM_SETKEYS_REQ:           limProcessMlmSetKeysReq(pMac, Msg->bodyptr);  break;
        case LIM_MLM_REMOVEKEY_REQ:         limProcessMlmRemoveKeyReq(pMac, Msg->bodyptr); break;
        case SIR_LIM_MIN_CHANNEL_TIMEOUT:   limProcessMinChannelTimeout(pMac);  break;
        case SIR_LIM_MAX_CHANNEL_TIMEOUT:   limProcessMaxChannelTimeout(pMac);  break;
        case SIR_LIM_PERIODIC_PROBE_REQ_TIMEOUT:
                               limProcessPeriodicProbeReqTimer(pMac);  break;
        case SIR_LIM_JOIN_FAIL_TIMEOUT:     limProcessJoinFailureTimeout(pMac);  break;
        case SIR_LIM_PERIODIC_JOIN_PROBE_REQ_TIMEOUT:
                                            limProcessPeriodicJoinProbeReqTimer(pMac); break;
        case SIR_LIM_AUTH_FAIL_TIMEOUT:     limProcessAuthFailureTimeout(pMac);  break;
        case SIR_LIM_AUTH_RSP_TIMEOUT:      limProcessAuthRspTimeout(pMac, Msg->bodyval);  break;
        case SIR_LIM_ASSOC_FAIL_TIMEOUT:    limProcessAssocFailureTimeout(pMac, Msg->bodyval);  break;
#ifdef WLAN_FEATURE_VOWIFI_11R
        case SIR_LIM_FT_PREAUTH_RSP_TIMEOUT:limProcessFTPreauthRspTimeout(pMac); break;
#endif
        case SIR_LIM_REMAIN_CHN_TIMEOUT:    limProcessRemainOnChnTimeout(pMac); break;
        case SIR_LIM_INSERT_SINGLESHOT_NOA_TIMEOUT:   
                                            limProcessInsertSingleShotNOATimeout(pMac); break;
        case SIR_LIM_CONVERT_ACTIVE_CHANNEL_TO_PASSIVE:
                                            limConvertActiveChannelToPassiveChannel(pMac); break;
        case SIR_LIM_DISASSOC_ACK_TIMEOUT:  limProcessDisassocAckTimeout(pMac); break;
        case SIR_LIM_DEAUTH_ACK_TIMEOUT:    limProcessDeauthAckTimeout(pMac); break;
        case LIM_MLM_ADDBA_REQ:             limProcessMlmAddBAReq( pMac, Msg->bodyptr ); break;
        case LIM_MLM_ADDBA_RSP:             limProcessMlmAddBARsp( pMac, Msg->bodyptr ); break;
        case LIM_MLM_DELBA_REQ:             limProcessMlmDelBAReq( pMac, Msg->bodyptr ); break;
        case LIM_MLM_TSPEC_REQ:                 
        default:
            break;
    } //                 
} /*                                    */


/* 
                   
  
           
                                                         
  
        
     
  
              
     
  
       
  
                                                 
               
 */

void
limSetScanMode(tpAniSirGlobal pMac)
{
    tSirLinkTrafficCheck checkTraffic;

    //                                                              
    pMac->lim.gLimCurrentScanChannelId = 0;

    if ( IS_MLM_SCAN_REQ_BACKGROUND_SCAN_AGGRESSIVE(pMac) )
    {
        checkTraffic = eSIR_DONT_CHECK_LINK_TRAFFIC_BEFORE_SCAN;
    }
    else if (IS_MLM_SCAN_REQ_BACKGROUND_SCAN_NORMAL(pMac))
    {
        checkTraffic = eSIR_CHECK_LINK_TRAFFIC_BEFORE_SCAN;
    }
    else
        checkTraffic = eSIR_CHECK_ROAMING_SCAN;

    PELOG1(limLog(pMac, LOG1, FL("Calling limSendHalInitScanReq"));)
    limSendHalInitScanReq(pMac, eLIM_HAL_INIT_SCAN_WAIT_STATE, checkTraffic);

    return ;
} /*                          */

//                         

/*                     
  
           
                                                     
  
        
                                                     
                                                     
  
              
     
  
       
  
                                                 
               
 */
tANI_U8
limIsLinkSuspended(tpAniSirGlobal pMac)
{
    return pMac->lim.gLimSystemInScanLearnMode; 
}
/* 
                   
  
           
                                                                                               
  
        
     
  
              
     
  
       
  
                                                 
                                                                    
                                                                              
                                                                       
               
 */
void
limSuspendLink(tpAniSirGlobal pMac, tSirLinkTrafficCheck trafficCheck,  SUSPEND_RESUME_LINK_CALLBACK callback, tANI_U32 *data)
{
   if( NULL == callback )
   {
      limLog( pMac, LOGE, "%s:%d: Invalid parameters", __func__, __LINE__ );
      return;
   }

   if( pMac->lim.gpLimSuspendCallback )
   {
      limLog( pMac, LOGE, "%s:%d: gLimSuspendLink callback is not NULL...something is wrong", __func__, __LINE__ );
      callback( pMac, eHAL_STATUS_FAILURE, data ); 
      return;
   }

   pMac->lim.gLimSystemInScanLearnMode = 1;
   pMac->lim.gpLimSuspendCallback = callback;
   pMac->lim.gpLimSuspendData = data;
   limSendHalInitScanReq(pMac, eLIM_HAL_SUSPEND_LINK_WAIT_STATE, trafficCheck );

   WDA_TrafficStatsTimerActivate(FALSE);
}

/* 
                  
  
           
                                                                                                                
  
        
     
  
              
     
  
       
  
                                                 
                                                                            
                                                                       
               
 */
void
limResumeLink(tpAniSirGlobal pMac, SUSPEND_RESUME_LINK_CALLBACK callback, tANI_U32 *data)
{
   if( NULL == callback )
   {
      limLog( pMac, LOGE, "%s:%d: Invalid parameters", __func__, __LINE__ );
      return;
   }

   if( pMac->lim.gpLimResumeCallback )
   {
      limLog( pMac, LOGE, "%s:%d: gLimResumeLink callback is not NULL...something is wrong", __func__, __LINE__ );
      callback( pMac, eHAL_STATUS_FAILURE, data ); 
      return;
   }

   pMac->lim.gpLimResumeCallback = callback;
   pMac->lim.gpLimResumeData = data;

   /*                                                                      
                                                                  
                                 
    */
   if( eLIM_HAL_IDLE_SCAN_STATE != pMac->lim.gLimHalScanState )
   {
      limSendHalFinishScanReq(pMac, eLIM_HAL_RESUME_LINK_WAIT_STATE );
   }
   else
   {
      limLog(pMac, LOGW, FL("Init Scan failed, we will not call finish scan."
                   " calling the callback with failure status"));
      pMac->lim.gpLimResumeCallback( pMac, eSIR_FAILURE, pMac->lim.gpLimResumeData);
      pMac->lim.gpLimResumeCallback = NULL;
      pMac->lim.gpLimResumeData = NULL;
      pMac->lim.gLimSystemInScanLearnMode = 0;
   }

   if(limIsInMCC(pMac))
   {
      WDA_TrafficStatsTimerActivate(TRUE);
   }
}
//                             


/* 
  
                                 
  
            
                                                                              
                                                                              
                  
  
 */
void
limChangeChannelWithCallback(tpAniSirGlobal pMac, tANI_U8 newChannel, 
    CHANGE_CHANNEL_CALLBACK callback, 
    tANI_U32 *cbdata, tpPESession psessionEntry)
{
    //                                              
#if defined WLAN_VOWIFI_DEBUG
        PELOGE(limLog( pMac, LOGE, "Switching channel to %d", newChannel);)
#endif
    psessionEntry->channelChangeReasonCode=LIM_SWITCH_CHANNEL_OPERATION;

    pMac->lim.gpchangeChannelCallback = callback;
    pMac->lim.gpchangeChannelData = cbdata;

    limSendSwitchChnlParams(pMac, newChannel,
        PHY_SINGLE_CHANNEL_CENTERED,
        psessionEntry->maxTxPower, psessionEntry->peSessionId);

    return;
}


/* 
                               
  
           
                                                       
  
        
  
              
     
  
       
     
  
                                                    
  
               
 */

void limContinuePostChannelScan(tpAniSirGlobal pMac)
{
    tANI_U8 channelNum;
    tANI_U8 handleError = 0;
    tANI_U8 i = 0;
    tSirRetStatus status = eSIR_SUCCESS;
    
    if( pMac->lim.abortScan || (NULL == pMac->lim.gpLimMlmScanReq ) ||
        (pMac->lim.gLimCurrentScanChannelId >
            (tANI_U32)(pMac->lim.gpLimMlmScanReq->channelList.numChannels - 1)))
    {
        pMac->lim.abortScan = 0;
        limDeactivateAndChangeTimer(pMac, eLIM_MIN_CHANNEL_TIMER);
        limDeactivateAndChangeTimer(pMac, eLIM_MAX_CHANNEL_TIMER);
        //                                                       
        //                                                               
        peSetResumeChannel(pMac, 0, 0);

        limSendHalFinishScanReq(pMac, eLIM_HAL_FINISH_SCAN_WAIT_STATE);
        return;
    }

    channelNum = limGetCurrentScanChannel(pMac);
    if ((pMac->lim.gpLimMlmScanReq->scanType == eSIR_ACTIVE_SCAN) &&
        (limActiveScanAllowed(pMac, channelNum)))
    {
        TX_TIMER *periodicScanTimer;
        PELOG2(limLog(pMac, LOG2, FL("ACTIVE Scan chan %d, sending probe"), channelNum);)

        do
        {
            /*                                                                                 
                    */
       
            PELOGE(limLog(pMac, LOG1, FL("sending ProbeReq number %d, for SSID %s on channel: %d"),
                                                i, pMac->lim.gpLimMlmScanReq->ssId[i].ssId, channelNum);)
            //                                  
            status = limSendProbeReqMgmtFrame( pMac, &pMac->lim.gpLimMlmScanReq->ssId[i],
               pMac->lim.gpLimMlmScanReq->bssId, channelNum, pMac->lim.gSelfMacAddr, 
               pMac->lim.gpLimMlmScanReq->dot11mode,
               pMac->lim.gpLimMlmScanReq->uIEFieldLen,
               (tANI_U8 *)(pMac->lim.gpLimMlmScanReq)+pMac->lim.gpLimMlmScanReq->uIEFieldOffset);
            
            if ( status != eSIR_SUCCESS)
            {
                PELOGE(limLog(pMac, LOGE, FL("send ProbeReq failed for SSID %s on channel: %d"),
                                                pMac->lim.gpLimMlmScanReq->ssId[i].ssId, channelNum);)
                limDeactivateAndChangeTimer(pMac, eLIM_MIN_CHANNEL_TIMER);
                limSendHalEndScanReq(pMac, channelNum, eLIM_HAL_END_SCAN_WAIT_STATE);
                return;
            }
            i++;
        } while (i < pMac->lim.gpLimMlmScanReq->numSsid);

        {
#if defined WLAN_FEATURE_VOWIFI
           //                                                                                      
           //                                                                                      
           //                 
           if( pMac->lim.gpLimMlmScanReq->minChannelTime != 0 )
           {
#endif
            //                            
            //                          
            limDeactivateAndChangeTimer(pMac, eLIM_MIN_CHANNEL_TIMER);

#ifdef GEN6_TODO
            /*                                                                     
                                                 
             */ 
            pMac->lim.limTimers.gLimMinChannelTimer.sessionId = sessionId;
#endif            
            
            MTRACE(macTrace(pMac, TRACE_CODE_TIMER_ACTIVATE, NO_SESSION, eLIM_MIN_CHANNEL_TIMER));

            if (tx_timer_activate(&pMac->lim.limTimers.gLimMinChannelTimer) != TX_SUCCESS)
            {
                limLog(pMac, LOGP, FL("could not start min channel timer"));
                return;
            }

            //                         
            limDeactivateAndChangeTimer(pMac, eLIM_MAX_CHANNEL_TIMER);
#if defined WLAN_FEATURE_VOWIFI
        }
           else
           {
#if defined WLAN_VOWIFI_DEBUG
              PELOGE(limLog( pMac, LOGE, "Min channel time == 0, Use only max chan timer" );)
#endif
              //                                                            
              limDeactivateAndChangeTimer(pMac, eLIM_MAX_CHANNEL_TIMER);
              MTRACE(macTrace(pMac, TRACE_CODE_TIMER_ACTIVATE, pMac->lim.limTimers.gLimMaxChannelTimer.sessionId, eLIM_MAX_CHANNEL_TIMER));
              if (tx_timer_activate(&pMac->lim.limTimers.gLimMaxChannelTimer)
                    == TX_TIMER_ERROR)
              {
                 //                                       
                 //          
                 limLog(pMac,LOGP, FL("could not start max channel timer"));
                 return; 
              }

    }
#endif
        }
        /*                                                                  
                         */
        periodicScanTimer = &pMac->lim.limTimers.gLimPeriodicProbeReqTimer;
        if (tx_timer_activate(periodicScanTimer) != TX_SUCCESS)
        {
             limLog(pMac, LOGP, FL("could not start periodic probe req "
                                                                  "timer"));
             return;
        }
        periodicScanTimer->sessionId = channelNum;
        MTRACE(macTrace(pMac, TRACE_CODE_TIMER_ACTIVATE, periodicScanTimer->sessionId, eLIM_PERIODIC_PROBE_REQ_TIMER));
    }
    else
    {
        tANI_U32 val;
        PELOG2(limLog(pMac, LOG2, FL("START PASSIVE Scan chan %d"), channelNum);)

        //                                            
        MTRACE(macTrace(pMac, TRACE_CODE_TIMER_DEACTIVATE, NO_SESSION, eLIM_MAX_CHANNEL_TIMER));
        if (tx_timer_deactivate(&pMac->lim.limTimers.gLimMaxChannelTimer)
                                      != TX_SUCCESS)
        {
            //                                        
            //          
            limLog(pMac, LOGP, FL("Unable to deactivate max channel timer"));
            return;
        }
        else
        {
            if (wlan_cfgGetInt(pMac, WNI_CFG_PASSIVE_MAXIMUM_CHANNEL_TIME,    
                          &val) != eSIR_SUCCESS)
            {
                /* 
                                                  
                                       
                 */
                limLog(pMac, LOGP, FL("could not retrieve passive max channel value"));
                return;
            }
            else
            {
                tANI_U32 val1 = 0;

                val = SYS_MS_TO_TICKS(val);
                //                        
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
                    val1 = pMac->lim.gLimSpecMgmt.quietDuration;
                }
                else
                {
                    val1 = SYS_MS_TO_TICKS(pMac->lim.gpLimMlmScanReq->maxChannelTime);
                }
#endif
                //                         
                val = (val > val1) ? val : val1;
                MTRACE(macTrace(pMac, TRACE_CODE_TIMER_ACTIVATE, NO_SESSION, eLIM_MAX_CHANNEL_TIMER));
                if (tx_timer_change(&pMac->lim.limTimers.gLimMaxChannelTimer,
                                val, 0) != TX_SUCCESS)
                {
                    //                                    
                    //          
                    limLog(pMac, LOGP, FL("Unable to change max channel timer"));
                    return;
                }
                else if (tx_timer_activate(&pMac->lim.limTimers.gLimMaxChannelTimer) != TX_SUCCESS)
                {
                    limLog(pMac, LOGP, FL("could not start max channel timer"));
                    return;
                }
          
            }
        }
        //                           
    } //                                                            

    if( handleError )
    {
        //
        //                                                    
        //                                                
        //                                                       
        //            
        //                              
        //                               
        //
        //                                                       
        //                                                               
        peSetResumeChannel(pMac, 0, 0);
        limSendHalFinishScanReq( pMac, eLIM_HAL_FINISH_SCAN_WAIT_STATE );
        //                                                        
    }
    else
    {
        limAddScanChannelInfo(pMac, channelNum);
    }

    return;
}





/*                           
  
           
                                                                                  
  
        
  
              
     
  
                                                                     
                                                                     
     
  
                                                    
                                                        
                                                            
  
               
 */


void limCovertChannelScanType(tpAniSirGlobal pMac,tANI_U8 channelNum, tANI_BOOLEAN passiveToActive)
{

    tANI_U32 i;
    tANI_U8  channelPair[WNI_CFG_SCAN_CONTROL_LIST_LEN];
    tANI_U32 len = WNI_CFG_SCAN_CONTROL_LIST_LEN;
    if (wlan_cfgGetStr(pMac, WNI_CFG_SCAN_CONTROL_LIST, channelPair, &len)
                    != eSIR_SUCCESS)
    {
        PELOGE(limLog(pMac, LOGE, FL("Unable to get scan control list"));)
        return ;
    }
    if (len > WNI_CFG_SCAN_CONTROL_LIST_LEN)
    {
        limLog(pMac, LOGE, FL("Invalid scan control list length:%d"), len);
        return ;
    }
    for (i=0; (i+1) < len; i+=2)
    {
        if (channelPair[i] == channelNum)
        {
             if ((eSIR_PASSIVE_SCAN == channelPair[i+1]) && TRUE == passiveToActive)
             {
                 PELOG1(limLog(pMac, LOG1, FL("Channel %d changed from Passive to Active"),
                                 channelNum);)
                 channelPair[i+1] = eSIR_ACTIVE_SCAN;
                 break ;
             }
             if ((eSIR_ACTIVE_SCAN == channelPair[i+1]) && FALSE == passiveToActive)
             {
                 PELOG1(limLog(pMac, LOG1, FL("Channel %d changed from Active to Passive"),
                                 channelNum);)
                 channelPair[i+1] = eSIR_PASSIVE_SCAN;
                 break ;
             }
       }
    }

    cfgSetStrNotify(pMac, WNI_CFG_SCAN_CONTROL_LIST, (tANI_U8 *)channelPair, len, FALSE);
    return ;
}




/*                       
  
           
                                                                                      
                                                                                   
                                                                      
  
        
  
              
     
  
                                                       
                                                                
     
  
                                                    
                                             
               
 */

void limSetDFSChannelList(tpAniSirGlobal pMac,tANI_U8 channelNum, tSirDFSChannelList *dfsChannelList)
{

    tANI_BOOLEAN passiveToActive = TRUE;
    if ((1 <= channelNum) && (165 >= channelNum))
    {
       if (eANI_BOOLEAN_TRUE == limIsconnectedOnDFSChannel(channelNum))
       {
          if (dfsChannelList->timeStamp[channelNum] == 0)
          {
             //                                                             
             PELOG1(limLog(pMac, LOG1, FL("Received first beacon on DFS channel: %d"), channelNum);)
             limCovertChannelScanType(pMac,channelNum, passiveToActive);
          }
          dfsChannelList->timeStamp[channelNum] = vos_timer_get_system_time();
       }
       else
       {
          PELOG1(limLog(pMac, LOG1, FL("Channel %d is Active"), channelNum);)
          return;
       }
       if (!tx_timer_running(&pMac->lim.limTimers.gLimActiveToPassiveChannelTimer))
       {
          tx_timer_activate(&pMac->lim.limTimers.gLimActiveToPassiveChannelTimer);
       }
    }
    else
    {
       PELOGE(limLog(pMac, LOGE, FL("Invalid Channel: %d"), channelNum);)
       return;
    }

    return;
}




/*
                                                                
                                                                          
                                                                                     
                                                                                          
                                                 
                                                                                                   
 
*/
static void __limCreateInitScanRawFrame(tpAniSirGlobal pMac, 
                                        tpInitScanParams pInitScanParam)
{
    tANI_U8   i;
    pInitScanParam->scanEntry.activeBSScnt = 0;
    
    /*                                                                         
                                                                              
                                                                               
                                                       
       */

    for(i =0; i < pMac->lim.maxBssId; i++)
    {
        if(pMac->lim.gpSession[i].valid == TRUE)
        {
            if(pMac->lim.gpSession[i].limMlmState == eLIM_MLM_LINK_ESTABLISHED_STATE)
            {
               if ((pMac->lim.gpSession[i].limSystemRole != eLIM_BT_AMP_STA_ROLE) &&
                   (pInitScanParam->scanEntry.activeBSScnt < HAL_NUM_BSSID))
                {
                    pInitScanParam->scanEntry.bssIdx[pInitScanParam->scanEntry.activeBSScnt] 
                        = pMac->lim.gpSession[i].bssIdx;
                    pInitScanParam->scanEntry.activeBSScnt++;

                }
            }
            else if( (eLIM_AP_ROLE == pMac->lim.gpSession[i].limSystemRole ) 
                    && ( VOS_P2P_GO_MODE == pMac->lim.gpSession[i].pePersona )
                   )
            {
                pInitScanParam->useNoA = TRUE;
            }
        }
    }
    if (pInitScanParam->scanEntry.activeBSScnt)
    {
        pInitScanParam->notifyBss = TRUE;
        pInitScanParam->frameType = SIR_MAC_DATA_FRAME;
        pInitScanParam->frameLength = 0;
    }
}

/*
                                                                 
                                                                                      
                       
                                                           
                                                                             
 
*/
static void __limCreateFinishScanRawFrame(tpAniSirGlobal pMac, 
                                          tpFinishScanParams pFinishScanParam)
{
    tANI_U8   i;
    pFinishScanParam->scanEntry.activeBSScnt = 0;

    for(i =0; i < pMac->lim.maxBssId; i++)
    {
        if(pMac->lim.gpSession[i].valid == TRUE)
        {
            if(pMac->lim.gpSession[i].limMlmState == eLIM_MLM_LINK_ESTABLISHED_STATE)
            {
                //                                                            
                //                             
               if ((pMac->lim.gpSession[i].limSystemRole != eLIM_BT_AMP_STA_ROLE) &&
                   (pFinishScanParam->scanEntry.activeBSScnt < HAL_NUM_BSSID))
                {
                    pFinishScanParam->scanEntry.bssIdx[pFinishScanParam->scanEntry.activeBSScnt] 
                        = pMac->lim.gpSession[i].bssIdx;
                    pFinishScanParam->scanEntry.activeBSScnt++;
                }
            }
        }
    }
    
    if (pFinishScanParam->scanEntry.activeBSScnt)
    {
        pFinishScanParam->notifyBss = TRUE;
        pFinishScanParam->frameType = SIR_MAC_DATA_FRAME;
        pFinishScanParam->frameLength = 0;
    }
}

void
limSendHalInitScanReq(tpAniSirGlobal pMac, tLimLimHalScanState nextState, tSirLinkTrafficCheck trafficCheck)
{


    tSirMsgQ                msg;
    tpInitScanParams        pInitScanParam;
    tSirRetStatus           rc = eSIR_SUCCESS;

    pInitScanParam = vos_mem_malloc(sizeof(*pInitScanParam));
    if ( NULL == pInitScanParam )
    {
        PELOGW(limLog(pMac, LOGW, FL("AllocateMemory() failed"));)
        goto error;
    }
    
    /*                                    */
    vos_mem_set((tANI_U8 *)pInitScanParam, sizeof(*pInitScanParam), 0);

    msg.type = WDA_INIT_SCAN_REQ;
    msg.bodyptr = pInitScanParam;
    msg.bodyval = 0;

    vos_mem_set((tANI_U8 *)&pInitScanParam->macMgmtHdr, sizeof(tSirMacMgmtHdr), 0);
    if (nextState == eLIM_HAL_INIT_LEARN_WAIT_STATE)
    {
        pInitScanParam->notifyBss = TRUE;
        pInitScanParam->notifyHost = FALSE;
        if (eSIR_CHECK_ROAMING_SCAN == trafficCheck)
        {
           pInitScanParam->scanMode = eHAL_SYS_MODE_ROAM_SCAN;
        }
        else
        {
           pInitScanParam->scanMode = eHAL_SYS_MODE_LEARN;
        }

        pInitScanParam->frameType = SIR_MAC_CTRL_CTS;
        __limCreateInitScanRawFrame(pMac, pInitScanParam);
        pInitScanParam->checkLinkTraffic = trafficCheck;
    }
    else
    {
        if(nextState == eLIM_HAL_SUSPEND_LINK_WAIT_STATE)
        {
           if (eSIR_CHECK_ROAMING_SCAN == trafficCheck)
           {
              pInitScanParam->scanMode = eHAL_SYS_MODE_ROAM_SUSPEND_LINK;
           }
           else
           {
              pInitScanParam->scanMode = eHAL_SYS_MODE_SUSPEND_LINK;
           }
           
        }
        else
        {
            if (eSIR_CHECK_ROAMING_SCAN == trafficCheck)
            {
               pInitScanParam->scanMode = eHAL_SYS_MODE_ROAM_SCAN;
            }
            else
            {
               pInitScanParam->scanMode = eHAL_SYS_MODE_SCAN;
            }
        }
        __limCreateInitScanRawFrame(pMac, pInitScanParam);
        if (pInitScanParam->useNoA)
        {
            pInitScanParam->scanDuration = pMac->lim.gTotalScanDuration;
        }
        /*                                                           
                                                
         */
        pInitScanParam->checkLinkTraffic = trafficCheck;
    }

    pMac->lim.gLimHalScanState = nextState;
    SET_LIM_PROCESS_DEFD_MESGS(pMac, false);
    MTRACE(macTraceMsgTx(pMac, NO_SESSION, msg.type));

    rc = wdaPostCtrlMsg(pMac, &msg);
    if (rc == eSIR_SUCCESS) {
        PELOG3(limLog(pMac, LOG3, FL("wdaPostCtrlMsg() return eSIR_SUCCESS pMac=%x nextState=%d"),
                    pMac, pMac->lim.gLimHalScanState);)
            return;
    }

    SET_LIM_PROCESS_DEFD_MESGS(pMac, true);
    vos_mem_free(pInitScanParam);
    PELOGW(limLog(pMac, LOGW, FL("wdaPostCtrlMsg failed, error code %d"), rc);)

error:
    switch(nextState)
    {
        case eLIM_HAL_START_SCAN_WAIT_STATE:
            limCompleteMlmScan(pMac, eSIR_SME_HAL_SCAN_INIT_FAILED);
            break;


            //                         
        case eLIM_HAL_SUSPEND_LINK_WAIT_STATE:
            pMac->lim.gLimHalScanState = eLIM_HAL_IDLE_SCAN_STATE;
            if( pMac->lim.gpLimSuspendCallback )
            {
                pMac->lim.gpLimSuspendCallback( pMac, rc, pMac->lim.gpLimSuspendData );
                pMac->lim.gpLimSuspendCallback = NULL;
                pMac->lim.gpLimSuspendData = NULL;
            }
            pMac->lim.gLimSystemInScanLearnMode = 0;
            break;
            //                             
        default:
            break;
    }
    pMac->lim.gLimHalScanState = eLIM_HAL_IDLE_SCAN_STATE;

    return ;
}

void
limSendHalStartScanReq(tpAniSirGlobal pMac, tANI_U8 channelNum, tLimLimHalScanState nextState)
{
    tSirMsgQ            msg;
    tpStartScanParams   pStartScanParam;
    tSirRetStatus       rc = eSIR_SUCCESS;

    /* 
                                                                                    
     */
    if(pMac->lim.gLimHalScanState != eLIM_HAL_START_SCAN_WAIT_STATE) 
    { 

        pStartScanParam = vos_mem_malloc(sizeof(*pStartScanParam));
        if ( NULL == pStartScanParam )
        {
            PELOGW(limLog(pMac, LOGW, FL("AllocateMemory() failed"));)
                goto error;
        }

        msg.type = WDA_START_SCAN_REQ;
        msg.bodyptr = pStartScanParam;
        msg.bodyval = 0;
        pStartScanParam->status = eHAL_STATUS_SUCCESS;
        pStartScanParam->scanChannel = (tANI_U8)channelNum;

        pMac->lim.gLimHalScanState = nextState;
        SET_LIM_PROCESS_DEFD_MESGS(pMac, false);

        MTRACE(macTraceMsgTx(pMac, NO_SESSION, msg.type));
        limLog(pMac, LOG1, FL("Channel %d"), channelNum);

            rc = wdaPostCtrlMsg(pMac, &msg);
        if (rc == eSIR_SUCCESS) {
            return;
        }

        SET_LIM_PROCESS_DEFD_MESGS(pMac, true);
        vos_mem_free(pStartScanParam);
        PELOGW(limLog(pMac, LOGW, FL("wdaPostCtrlMsg failed, error code %d"), rc);)

error:
        switch(nextState)
        {
            case eLIM_HAL_START_SCAN_WAIT_STATE:
                limCompleteMlmScan(pMac, eSIR_SME_HAL_SCAN_INIT_FAILED);
                break;


            default:
                break;
        }
        pMac->lim.gLimHalScanState = eLIM_HAL_IDLE_SCAN_STATE;

    }
    else
    {
        PELOGW(limLog(pMac, LOGW, FL("Invalid state for START_SCAN_REQ message=%d"), pMac->lim.gLimHalScanState);)
    }

    return;
}

void limSendHalEndScanReq(tpAniSirGlobal pMac, tANI_U8 channelNum, tLimLimHalScanState nextState)
{
    tSirMsgQ            msg;
    tpEndScanParams     pEndScanParam;
    tSirRetStatus       rc = eSIR_SUCCESS;

    /* 
                                                                                   
                                          
                                                                                    
                                                               
     */
    if((pMac->lim.gLimHalScanState != eLIM_HAL_END_SCAN_WAIT_STATE)  &&
       (pMac->lim.gLimHalScanState != eLIM_HAL_IDLE_SCAN_STATE)  &&
       (pMac->lim.gLimHalScanState == eLIM_HAL_SCANNING_STATE)  &&
       (pMac->lim.gLimHalScanState != eLIM_HAL_FINISH_SCAN_WAIT_STATE)  &&
       (pMac->lim.gLimHalScanState != eLIM_HAL_START_SCAN_WAIT_STATE))
    {
        pEndScanParam = vos_mem_malloc(sizeof(*pEndScanParam));
        if ( NULL == pEndScanParam )
        {
            PELOGW(limLog(pMac, LOGW, FL("AllocateMemory() failed"));)
                goto error;
        }

        msg.type = WDA_END_SCAN_REQ;
        msg.bodyptr = pEndScanParam;
        msg.bodyval = 0;
        pEndScanParam->status = eHAL_STATUS_SUCCESS;
        pEndScanParam->scanChannel = (tANI_U8)channelNum;

        pMac->lim.gLimHalScanState = nextState;
        SET_LIM_PROCESS_DEFD_MESGS(pMac, false);
        MTRACE(macTraceMsgTx(pMac, NO_SESSION, msg.type));

        rc = wdaPostCtrlMsg(pMac, &msg);
        if (rc == eSIR_SUCCESS) {
            return;
        }

        SET_LIM_PROCESS_DEFD_MESGS(pMac, true);
        vos_mem_free(pEndScanParam);
        PELOGW(limLog(pMac, LOGW, FL("wdaPostCtrlMsg failed, error code %d"), rc);)

            error:
            switch(nextState)
            {
                case eLIM_HAL_END_SCAN_WAIT_STATE:
                    limCompleteMlmScan(pMac, eSIR_SME_HAL_SCAN_END_FAILED);
                    break;


                default:
                    PELOGW(limLog(pMac, LOGW, FL("wdaPostCtrlMsg Rcvd invalid nextState %d"), nextState);)
                        break;
            }
        pMac->lim.gLimHalScanState = eLIM_HAL_IDLE_SCAN_STATE;
        PELOGW(limLog(pMac, LOGW, FL("wdaPostCtrlMsg failed, error code %d"), rc);)
    }
    else
    {
        PELOGW(limLog(pMac, LOGW, FL("Invalid state for END_SCAN_REQ message=%d"), pMac->lim.gLimHalScanState);)
    }


    return;
}

/* 
                            
  
           
                                                         
  
        
  
              
     
  
       
     
  
                                                    
                                                                                        
  
               
 */
void limSendHalFinishScanReq(tpAniSirGlobal pMac, tLimLimHalScanState nextState)
{

    tSirMsgQ            msg;
    tpFinishScanParams  pFinishScanParam;
    tSirRetStatus       rc = eSIR_SUCCESS;

    if(pMac->lim.gLimHalScanState == nextState)
    {
        /*
                                                                                                           
                                                                  
                                      
                                                                                                            
         */
        PELOGW(limLog(pMac, LOGW, FL("Next Scan State is same as the current state: %d "), nextState);)
            return;
    }

    pFinishScanParam = vos_mem_malloc(sizeof(*pFinishScanParam));
    if ( NULL == pFinishScanParam )
    {
        PELOGW(limLog(pMac, LOGW, FL("AllocateMemory() failed"));)
            goto error;
    }

    msg.type = WDA_FINISH_SCAN_REQ;
    msg.bodyptr = pFinishScanParam;
    msg.bodyval = 0;
    
    peGetResumeChannel(pMac, &pFinishScanParam->currentOperChannel, &pFinishScanParam->cbState);

    vos_mem_set((tANI_U8 *)&pFinishScanParam->macMgmtHdr, sizeof(tSirMacMgmtHdr), 0);

    if (nextState == eLIM_HAL_FINISH_LEARN_WAIT_STATE)
    {
        //                                  
        pFinishScanParam->scanMode = eHAL_SYS_MODE_LEARN;
        pFinishScanParam->notifyBss = FALSE;
        pFinishScanParam->notifyHost = FALSE;
        pFinishScanParam->frameType = 0;

        pFinishScanParam->frameLength = 0;
        pMac->lim.gLimHalScanState = nextState;
    }
    else
    {
        /*                                               
                                                           
                                      
         */
        if (nextState == eLIM_HAL_RESUME_LINK_WAIT_STATE)
        {
            pFinishScanParam->scanMode = eHAL_SYS_MODE_SUSPEND_LINK;
        }
        else
        {
            pFinishScanParam->scanMode = eHAL_SYS_MODE_SCAN;
        }
        pFinishScanParam->notifyHost = FALSE;
        __limCreateFinishScanRawFrame(pMac, pFinishScanParam);
        //                         
        pMac->lim.gLimHalScanState = nextState;
        //                             
    }

    SET_LIM_PROCESS_DEFD_MESGS(pMac, false);
    MTRACE(macTraceMsgTx(pMac, NO_SESSION, msg.type));

    rc = wdaPostCtrlMsg(pMac, &msg);
    if (rc == eSIR_SUCCESS) {
        return;
    }
    SET_LIM_PROCESS_DEFD_MESGS(pMac, true);
    vos_mem_free(pFinishScanParam);
    PELOGW(limLog(pMac, LOGW, FL("wdaPostCtrlMsg failed, error code %d"), rc);)

        error:
        if(nextState == eLIM_HAL_FINISH_SCAN_WAIT_STATE)
            limCompleteMlmScan(pMac, eSIR_SME_HAL_SCAN_FINISH_FAILED);
    //                         
        else if ( nextState == eLIM_HAL_RESUME_LINK_WAIT_STATE )
        {
            if( pMac->lim.gpLimResumeCallback )
            {
                pMac->lim.gpLimResumeCallback( pMac, rc, pMac->lim.gpLimResumeData );
                pMac->lim.gpLimResumeCallback = NULL;
                pMac->lim.gpLimResumeData = NULL;
                pMac->lim.gLimSystemInScanLearnMode = 0;
            }
        }
    //                             
    pMac->lim.gLimHalScanState = eLIM_HAL_IDLE_SCAN_STATE;
    return;
}

/* 
                           
  
           
                                                      
                                                            
                                            
  
        
                                                           
                                                           
                                                             
                                                           
                                                               
                                                           
                                                            
  
              
                                                            
                                                           
                  
  
       
     
  
                                                   
               
 */
void
limContinueChannelScan(tpAniSirGlobal pMac)
{
    tANI_U8                channelNum;

    PELOG1(limLog(pMac, LOG1, FL("Continue SCAN : chan %d tot %d"),
           pMac->lim.gLimCurrentScanChannelId,
           pMac->lim.gpLimMlmScanReq->channelList.numChannels);)

    if (pMac->lim.gLimCurrentScanChannelId >
        (tANI_U32) (pMac->lim.gpLimMlmScanReq->channelList.numChannels - 1) 
        || pMac->lim.abortScan)
    {
        pMac->lim.abortScan = 0;
        limDeactivateAndChangeTimer(pMac, eLIM_MIN_CHANNEL_TIMER);
        limDeactivateAndChangeTimer(pMac, eLIM_MAX_CHANNEL_TIMER);

        //                                                       
        //                                                               
        peSetResumeChannel(pMac, 0, 0);

        //                                     
        limSendHalFinishScanReq(pMac, eLIM_HAL_FINISH_SCAN_WAIT_STATE);
        return;
    }

    //                                           

    if ((pMac->lim.gLimReturnAfterFirstMatch & 0x40) ||
        (pMac->lim.gLimReturnAfterFirstMatch & 0x80))
    {
        while (pMac->lim.gLimCurrentScanChannelId <=
               (tANI_U32) (pMac->lim.gpLimMlmScanReq->channelList.numChannels - 1))
        {
            if (((limGetCurrentScanChannel(pMac) <= 14) &&
                  pMac->lim.gLim24Band11dScanDone) ||
                ((limGetCurrentScanChannel(pMac) > 14) &&
                  pMac->lim.gLim50Band11dScanDone))
            {
                limLog(pMac, LOGW, FL("skipping chan %d"),
                       limGetCurrentScanChannel(pMac));
                pMac->lim.gLimCurrentScanChannelId++;
            }
            else
                break;
        }

        if (pMac->lim.gLimCurrentScanChannelId >
            (tANI_U32) (pMac->lim.gpLimMlmScanReq->channelList.numChannels - 1))
        {
            pMac->lim.abortScan = 0;
            limDeactivateAndChangeTimer(pMac, eLIM_MIN_CHANNEL_TIMER);
            limDeactivateAndChangeTimer(pMac, eLIM_MAX_CHANNEL_TIMER);
            //                                     
            //                                                       
            //                                                               
            peSetResumeChannel(pMac, 0, 0);
            limSendHalFinishScanReq(pMac, eLIM_HAL_FINISH_SCAN_WAIT_STATE);
            return;
        }
    }

    channelNum = limGetCurrentScanChannel(pMac);
    PELOG2(limLog(pMac, LOG2, FL("Current Channel to be scanned is %d"),
           channelNum);)

    limSendHalStartScanReq(pMac, channelNum, eLIM_HAL_START_SCAN_WAIT_STATE);
    return;
} /*                                  */



/* 
                           
  
           
                                                      
                                                     
  
       
  
              
  
       
     
  
                                                   
               
 */
void
limRestorePreScanState(tpAniSirGlobal pMac)
{
    int i;
    
    //                                              
    limDeactivateAndChangeTimer(pMac, eLIM_MIN_CHANNEL_TIMER);
    limDeactivateAndChangeTimer(pMac, eLIM_MAX_CHANNEL_TIMER);

    /*                                                             
                                           
                                                   */
    if(((ePMM_STATE_BMPS_WAKEUP == pMac->pmm.gPmmState) ||
       (ePMM_STATE_READY == pMac->pmm.gPmmState))
        && (pMac->lim.gLimBackgroundScanMode != eSIR_ROAMING_SCAN ))
    {
      for(i=0;i<pMac->lim.maxBssId;i++)
      {
        if((peFindSessionBySessionId(pMac,i) != NULL) &&
           (pMac->lim.gpSession[i].valid == TRUE) && 
           (eLIM_MLM_LINK_ESTABLISHED_STATE == pMac->lim.gpSession[i].limMlmState) &&
           (!IS_ACTIVEMODE_OFFLOAD_FEATURE_ENABLE))
        {
          limReactivateHeartBeatTimer(pMac, peFindSessionBySessionId(pMac,i));
        }  
      }
    }

    /* 
                              
                                                   
                                  
     */
    //                     

    pMac->lim.gLimSystemInScanLearnMode = 0;
    PELOG1(limLog(pMac, LOG1, FL("Scan ended, took %d tu"), (tx_time_get() - pMac->lim.scanStartTime));)
} /*                              */

#ifdef FEATURE_OEM_DATA_SUPPORT

void limSendHalOemDataReq(tpAniSirGlobal pMac)
{
    tSirMsgQ msg;
    tpStartOemDataReq pStartOemDataReq = NULL;
    tSirRetStatus rc = eSIR_SUCCESS;
    tpLimMlmOemDataRsp pMlmOemDataRsp;
    tANI_U32 reqLen = 0;
    if(NULL == pMac->lim.gpLimMlmOemDataReq)
    {
        PELOGE(limLog(pMac, LOGE,  FL("Null pointer"));)
        goto error;
    }

    reqLen = sizeof(tStartOemDataReq);

    pStartOemDataReq = vos_mem_malloc(reqLen);
    if ( NULL == pStartOemDataReq )
    {
        PELOGE(limLog(pMac, LOGE,  FL("OEM_DATA: Could not allocate memory for pStartOemDataReq"));)
        goto error;
    }

    vos_mem_set((tANI_U8*)(pStartOemDataReq), reqLen, 0);

    //                                                        
    vos_mem_copy(pStartOemDataReq->selfMacAddr,
                 pMac->lim.gpLimMlmOemDataReq->selfMacAddr,
                 sizeof(tSirMacAddr));

    vos_mem_copy(pStartOemDataReq->oemDataReq,
                 pMac->lim.gpLimMlmOemDataReq->oemDataReq,
                 OEM_DATA_REQ_SIZE);

    //                                      
    msg.type = WDA_START_OEM_DATA_REQ;
    msg.bodyptr = pStartOemDataReq;
    msg.bodyval = 0;

    SET_LIM_PROCESS_DEFD_MESGS(pMac, false);
    MTRACE(macTraceMsgTx(pMac, NO_SESSION, msg.type));

    rc = wdaPostCtrlMsg(pMac, &msg);
    if(rc == eSIR_SUCCESS)
    {
        return;
    }

    SET_LIM_PROCESS_DEFD_MESGS(pMac, true);
    vos_mem_free(pStartOemDataReq);
    PELOGE(limLog(pMac, LOGE,  FL("OEM_DATA: posting WDA_START_OEM_DATA_REQ to HAL failed"));)

error:
    pMac->lim.gLimMlmState = pMac->lim.gLimPrevMlmState;
    MTRACE(macTrace(pMac, TRACE_CODE_MLM_STATE, NO_SESSION, pMac->lim.gLimMlmState));

    pMlmOemDataRsp = vos_mem_malloc(sizeof(tLimMlmOemDataRsp));
    if ( NULL == pMlmOemDataRsp )
    {
        limLog(pMac->hHdd, LOGP, FL("OEM_DATA: memory allocation for pMlmOemDataRsp failed under suspend link failure"));
        return;
    }

    if(NULL != pMac->lim.gpLimMlmOemDataReq)
    {
        vos_mem_free(pMac->lim.gpLimMlmOemDataReq);
        pMac->lim.gpLimMlmOemDataReq = NULL;
    }

    limPostSmeMessage(pMac, LIM_MLM_OEM_DATA_CNF, (tANI_U32*)pMlmOemDataRsp);

    return;
}
/* 
                               
  
            
                                                                                  
                               
         
      
  
               
      
  
        
  
                                                
               
 */

void limSetOemDataReqModeFailed(tpAniSirGlobal pMac, eHalStatus status, tANI_U32* data)
{
    tpLimMlmOemDataRsp pMlmOemDataRsp;

    pMac->lim.gLimMlmState = pMac->lim.gLimPrevMlmState;
    MTRACE(macTrace(pMac, TRACE_CODE_MLM_STATE, NO_SESSION, pMac->lim.gLimMlmState));

    pMlmOemDataRsp = vos_mem_malloc(sizeof(tLimMlmOemDataRsp));
    if ( NULL == pMlmOemDataRsp )
    {
        limLog(pMac->hHdd, LOGP, FL("OEM_DATA: memory allocation for pMlmOemDataRsp failed under suspend link failure"));
        return;
    }

    if (NULL != pMac->lim.gpLimMlmOemDataReq)
    {
        vos_mem_free(pMac->lim.gpLimMlmOemDataReq);
        pMac->lim.gpLimMlmOemDataReq = NULL;
    }

    vos_mem_set(pMlmOemDataRsp, sizeof(tLimMlmOemDataRsp), 0);

    limPostSmeMessage(pMac, LIM_MLM_OEM_DATA_CNF, (tANI_U32*)pMlmOemDataRsp);

    return;
}

/* 
                         
  
           
                                                                 
  
        
     
  
              
     
  
       
  
                                                 
               
 */

void limSetOemDataReqMode(tpAniSirGlobal pMac, eHalStatus status, tANI_U32* data)
{
    if(status != eHAL_STATUS_SUCCESS)
    {
        limLog(pMac, LOGE, FL("OEM_DATA: failed in suspend link"));
        goto error;
    }
    else
    {
        PELOGE(limLog(pMac, LOGE, FL("OEM_DATA: Calling limSendHalOemDataReq"));)
        limSendHalOemDataReq(pMac);
        return;
    }

error:
    limResumeLink(pMac, limSetOemDataReqModeFailed, NULL);
    return ;
} /*                                */

#endif //                        

static void
mlm_add_sta(
    tpAniSirGlobal  pMac,
    tpAddStaParams  pSta,
    tANI_U8        *pBssid,
    tANI_U8         htCapable,
    tpPESession     psessionEntry)  //                                     
{
    tANI_U32 val;
    int      i;
    

    pSta->staType = STA_ENTRY_SELF; //                 

    vos_mem_copy(pSta->bssId, pBssid, sizeof( tSirMacAddr ));
    vos_mem_copy(pSta->staMac, psessionEntry->selfMacAddr, sizeof(tSirMacAddr));

    /*                                                                  */

    if( eSIR_SUCCESS != wlan_cfgGetInt( pMac, WNI_CFG_LISTEN_INTERVAL, &val ))
        limLog(pMac, LOGP, FL("Couldn't get LISTEN_INTERVAL"));
    
    pSta->listenInterval = (tANI_U16) val;
    
    if (eSIR_SUCCESS != wlan_cfgGetInt(pMac, WNI_CFG_SHORT_PREAMBLE, &val) )
        limLog(pMac, LOGP, FL("Couldn't get SHORT_PREAMBLE"));
    pSta->shortPreambleSupported = (tANI_U8)val;

    pSta->assocId               = 0; //                      
    pSta->wmmEnabled            = 0;
    pSta->uAPSD                 = 0;
    pSta->maxSPLen              = 0;
    pSta->us32MaxAmpduDuration  = 0;
    pSta->maxAmpduSize          = 0; //                            


    /*                                                     */
    pSta->htLdpcCapable = 
                      (psessionEntry->txLdpcIniFeatureEnabled & 0x01);
    pSta->vhtLdpcCapable = 
                      ((psessionEntry->txLdpcIniFeatureEnabled >> 1)& 0x01);

    if(IS_DOT11_MODE_HT(psessionEntry->dot11mode)) 
    {
        pSta->htCapable         = htCapable;
        pSta->greenFieldCapable = limGetHTCapability( pMac, eHT_GREENFIELD, psessionEntry);
        pSta->txChannelWidthSet = limGetHTCapability( pMac, eHT_SUPPORTED_CHANNEL_WIDTH_SET, psessionEntry );
        pSta->mimoPS            = (tSirMacHTMIMOPowerSaveState)limGetHTCapability( pMac, eHT_MIMO_POWER_SAVE, psessionEntry );
        pSta->rifsMode          = limGetHTCapability( pMac, eHT_RIFS_MODE, psessionEntry );
        pSta->lsigTxopProtection = limGetHTCapability( pMac, eHT_LSIG_TXOP_PROTECTION, psessionEntry );
        pSta->delBASupport      = limGetHTCapability( pMac, eHT_DELAYED_BA, psessionEntry );
        pSta->maxAmpduDensity   = limGetHTCapability( pMac, eHT_MPDU_DENSITY, psessionEntry );
        pSta->maxAmsduSize      = limGetHTCapability( pMac, eHT_MAX_AMSDU_LENGTH, psessionEntry );
        pSta->fDsssCckMode40Mhz = limGetHTCapability( pMac, eHT_DSSS_CCK_MODE_40MHZ, psessionEntry);
        pSta->fShortGI20Mhz     = limGetHTCapability( pMac, eHT_SHORT_GI_20MHZ, psessionEntry);
        pSta->fShortGI40Mhz     = limGetHTCapability( pMac, eHT_SHORT_GI_40MHZ, psessionEntry);
    }
#ifdef WLAN_FEATURE_11AC
    if (psessionEntry->vhtCapability)
    {
        pSta->vhtCapable = VOS_TRUE;
        pSta->vhtTxBFCapable = psessionEntry->txBFIniFeatureEnabled;
    }
#endif
#ifdef WLAN_FEATURE_11AC
    limPopulateOwnRateSet(pMac, &pSta->supportedRates, NULL, false,psessionEntry,NULL);
#else
    limPopulateOwnRateSet(pMac, &pSta->supportedRates, NULL, false,psessionEntry);
#endif
    limFillSupportedRatesInfo(pMac, NULL, &pSta->supportedRates,psessionEntry);
    
    limLog( pMac, LOGE, FL( "GF: %d, ChnlWidth: %d, MimoPS: %d, lsigTXOP: %d, dsssCCK: %d, SGI20: %d, SGI40%d") ,
                                          pSta->greenFieldCapable, pSta->txChannelWidthSet, pSta->mimoPS, pSta->lsigTxopProtection, 
                                          pSta->fDsssCckMode40Mhz,pSta->fShortGI20Mhz, pSta->fShortGI40Mhz);

     if (VOS_P2P_GO_MODE == psessionEntry->pePersona)
     {
         pSta->p2pCapableSta = 1;
     }

    //                                                        
    for( i = 0; i < STACFG_MAX_TC; i++ )
    {
        pSta->staTCParams[i].txUseBA = eBA_DISABLE;
        pSta->staTCParams[i].rxUseBA = eBA_DISABLE;
    }
    
}

//
//                                    
//                                                        
//
tSirResultCodes
limMlmAddBss (
    tpAniSirGlobal      pMac,
    tLimMlmStartReq    *pMlmStartReq,
    tpPESession         psessionEntry)
{
    tSirMsgQ msgQ;
    tpAddBssParams pAddBssParams = NULL;
    tANI_U32 retCode;

    //                                           

    pAddBssParams = vos_mem_malloc(sizeof( tAddBssParams ));
    if ( NULL == pAddBssParams )
    {
        limLog( pMac, LOGE, FL( "Unable to allocate memory during ADD_BSS" ));
        //                                      
        return eSIR_SME_HAL_SEND_MESSAGE_FAIL;
    }

    vos_mem_set(pAddBssParams, sizeof(tAddBssParams), 0);

    //                              
    vos_mem_copy(pAddBssParams->bssId, pMlmStartReq->bssId,
                   sizeof( tSirMacAddr ));

    //                                  
    vos_mem_copy (pAddBssParams->selfMacAddr,
                  psessionEntry->selfMacAddr,
                  sizeof( tSirMacAddr ));
    
    pAddBssParams->bssType = pMlmStartReq->bssType;
    if ((pMlmStartReq->bssType == eSIR_IBSS_MODE) || 
        (pMlmStartReq->bssType == eSIR_BTAMP_AP_MODE)|| 
        (pMlmStartReq->bssType == eSIR_BTAMP_STA_MODE)) {
        pAddBssParams->operMode                 = BSS_OPERATIONAL_MODE_STA;
    }
    else if (pMlmStartReq->bssType == eSIR_INFRA_AP_MODE){
        pAddBssParams->operMode                 = BSS_OPERATIONAL_MODE_AP;
    }

    pAddBssParams->shortSlotTimeSupported = psessionEntry->shortSlotTimeSupported;

    pAddBssParams->beaconInterval               = pMlmStartReq->beaconPeriod;
    pAddBssParams->dtimPeriod                   = pMlmStartReq->dtimPeriod;
    pAddBssParams->cfParamSet.cfpCount          = pMlmStartReq->cfParamSet.cfpCount;
    pAddBssParams->cfParamSet.cfpPeriod         = pMlmStartReq->cfParamSet.cfpPeriod;
    pAddBssParams->cfParamSet.cfpMaxDuration    = pMlmStartReq->cfParamSet.cfpMaxDuration;
    pAddBssParams->cfParamSet.cfpDurRemaining   = pMlmStartReq->cfParamSet.cfpDurRemaining;

    pAddBssParams->rateSet.numRates = pMlmStartReq->rateSet.numRates;
    vos_mem_copy(pAddBssParams->rateSet.rate,
                 pMlmStartReq->rateSet.rate, pMlmStartReq->rateSet.numRates);

    pAddBssParams->nwType = pMlmStartReq->nwType;

    pAddBssParams->htCapable            = pMlmStartReq->htCapable;
#ifdef WLAN_FEATURE_11AC
    pAddBssParams->vhtCapable           = psessionEntry->vhtCapability;
    pAddBssParams->vhtTxChannelWidthSet = psessionEntry->vhtTxChannelWidthSet; 
#endif
    pAddBssParams->htOperMode           = pMlmStartReq->htOperMode;
    pAddBssParams->dualCTSProtection    = pMlmStartReq->dualCTSProtection;
    pAddBssParams->txChannelWidthSet    = pMlmStartReq->txChannelWidthSet;

    pAddBssParams->currentOperChannel   = pMlmStartReq->channelNumber;
    pAddBssParams->currentExtChannel    = pMlmStartReq->cbMode;

    /*                    */
    pAddBssParams->sessionId            = pMlmStartReq->sessionId; 

    //                                                     
    vos_mem_copy(&(pAddBssParams->ssId.ssId),
                 pMlmStartReq->ssId.ssId,
                 pMlmStartReq->ssId.length);
    pAddBssParams->ssId.length = pMlmStartReq->ssId.length;
    pAddBssParams->bHiddenSSIDEn = pMlmStartReq->ssidHidden;
    limLog( pMac, LOGE, FL( "TRYING TO HIDE SSID %d" ),pAddBssParams->bHiddenSSIDEn);
    //                                                                                   
    pAddBssParams->bProxyProbeRespEn = 0;
    pAddBssParams->obssProtEnabled = pMlmStartReq->obssProtEnabled;

#if defined WLAN_FEATURE_VOWIFI  
    pAddBssParams->maxTxPower = psessionEntry->maxTxPower;
#endif
    mlm_add_sta(pMac, &pAddBssParams->staContext,
                pAddBssParams->bssId, pAddBssParams->htCapable,psessionEntry);

    pAddBssParams->status   = eHAL_STATUS_SUCCESS;
    pAddBssParams->respReqd = 1;

    //                         
    psessionEntry->limMlmState = eLIM_MLM_WT_ADD_BSS_RSP_STATE;
    MTRACE(macTrace(pMac, TRACE_CODE_MLM_STATE, psessionEntry->peSessionId, psessionEntry->limMlmState));

    pAddBssParams->halPersona=psessionEntry->pePersona; //                                  

    pAddBssParams->bSpectrumMgtEnabled = psessionEntry->spectrumMgtEnabled;

#if defined WLAN_FEATURE_VOWIFI_11R
    pAddBssParams->extSetStaKeyParamValid = 0;
#endif

#ifdef WLAN_FEATURE_11W
    pAddBssParams->rmfEnabled = (psessionEntry->gStartBssRSNIe.RSN_Cap[0] >> 7) & 0x1;
    limLog( pMac, LOG1, FL("PMF capable value for BSS is %d"), pAddBssParams->rmfEnabled);
#endif

    //
    //           
    //                                                             
    //                                
    //
    msgQ.type       = WDA_ADD_BSS_REQ;
    msgQ.reserved   = 0;
    msgQ.bodyptr    = pAddBssParams;
    msgQ.bodyval    = 0;
    MTRACE(macTraceMsgTx(pMac, psessionEntry->peSessionId, msgQ.type));

    limLog( pMac, LOGW, FL( "Sending WDA_ADD_BSS_REQ..." ));
    if( eSIR_SUCCESS != (retCode = wdaPostCtrlMsg( pMac, &msgQ )))
    {
        limLog( pMac, LOGE, FL("Posting ADD_BSS_REQ to HAL failed, reason=%X"), retCode );
        vos_mem_free(pAddBssParams);
        return eSIR_SME_HAL_SEND_MESSAGE_FAIL;
    }

    return eSIR_SME_SUCCESS;
}


/* 
                          
  
           
                                                           
           
  
        
                                              
                                       
                                                         
                                                 
  
              
  
       
  
                                                    
                                                        
               
 */

static void
limProcessMlmStartReq(tpAniSirGlobal pMac, tANI_U32 *pMsgBuf)
{
    tLimMlmStartReq          *pMlmStartReq;
    tLimMlmStartCnf          mlmStartCnf;
    tpPESession              psessionEntry = NULL;
    
    if(pMsgBuf == NULL)
    {
        PELOGE(limLog(pMac, LOGE,FL("Buffer is Pointing to NULL"));)
        return;
    }
    
    pMlmStartReq = (tLimMlmStartReq *) pMsgBuf;
    if((psessionEntry = peFindSessionBySessionId(pMac,pMlmStartReq->sessionId))==NULL)
    {
        limLog(pMac, LOGP,FL("Session Does not exist for given sessionID"));
        mlmStartCnf.resultCode = eSIR_SME_REFUSED;
        goto end;
    }

    if (psessionEntry->limMlmState != eLIM_MLM_IDLE_STATE)
    {
        /* 
                                                                        
                                                  
         */
        PELOGE(limLog(pMac, LOGE, FL("received unexpected MLM_START_REQ in state %X"),psessionEntry->limMlmState);)
        limPrintMlmState(pMac, LOGE, psessionEntry->limMlmState);
        mlmStartCnf.resultCode = eSIR_SME_BSS_ALREADY_STARTED_OR_JOINED;
        goto end;
    }
    
    #if 0
     if (cfgSetInt(pMac, WNI_CFG_CURRENT_CHANNEL, pMlmStartReq->channelNumber)!= eSIR_SUCCESS)
            limLog(pMac, LOGP, FL("could not set CURRENT_CHANNEL at CFG"));
     
        pMac->lim.gLimCurrentChannelId = pMlmStartReq->channelNumber;
    #endif //                 


    //                                    
    #if 0 //                                                                                                       
     if (cfgSetStr(pMac, WNI_CFG_BSSID, (tANI_U8 *) pMlmStartReq->bssId, sizeof(tSirMacAddr))
        != eSIR_SUCCESS)
        limLog(pMac, LOGP, FL("could not update BSSID at CFG"));

   
    
    vos_mem_copy(  pMac->lim.gLimCurrentBssId,
                   pMlmStartReq->bssId,
                   sizeof(tSirMacAddr));
    #endif //                 

    #if 0
    if (cfgSetStr(pMac, WNI_CFG_SSID, (tANI_U8 *) &pMlmStartReq->ssId.ssId, pMlmStartReq->ssId.length)
        != eSIR_SUCCESS)
        limLog(pMac, LOGP, FL("could not update SSID at CFG"));
    #endif //                 
   
         
   //                                                              

        #if 0
        if (cfgSetStr(pMac, WNI_CFG_OPERATIONAL_RATE_SET,
           (tANI_U8 *) &pMac->lim.gpLimStartBssReq->operationalRateSet.rate,
           pMac->lim.gpLimStartBssReq->operationalRateSet.numRates)
        != eSIR_SUCCESS)
        limLog(pMac, LOGP, FL("could not update Operational Rateset at CFG"));
        #endif //                 
        


#if 0 //                                                                               
    if (psessionEntry->limSystemRole == eLIM_AP_ROLE)
    {
        if(pMac->lim.limTimers.gLimWPSOverlapTimerObj.isTimerCreated == eANI_BOOLEAN_FALSE)
        {
            if (tx_timer_create(&pMac->lim.limTimers.gLimWPSOverlapTimerObj.gLimWPSOverlapTimer,
                            "PS OVERLAP Timer",
                            limWPSOverlapTimerHandler,
                            SIR_LIM_WPS_OVERLAP_TIMEOUT, //                 
                            SYS_MS_TO_TICKS(LIM_WPS_OVERLAP_TIMER_MS),  //              
                            SYS_MS_TO_TICKS(LIM_WPS_OVERLAP_TIMER_MS),                         //                 
                            TX_AUTO_ACTIVATE /*               */) != TX_SUCCESS)
            {
                limLog(pMac, LOGP, FL("failed to create WPS overlap Timer"));
            }
            
            pMac->lim.limTimers.gLimWPSOverlapTimerObj.sessionId = pMlmStartReq->sessionId;
            pMac->lim.limTimers.gLimWPSOverlapTimerObj.isTimerCreated = eANI_BOOLEAN_TRUE;
            limLog(pMac, LOGE, FL("Create WPS overlap Timer, session=%d"), pMlmStartReq->sessionId);

            if (tx_timer_activate(&pMac->lim.limTimers.gLimWPSOverlapTimerObj.gLimWPSOverlapTimer) != TX_SUCCESS)
            {
                limLog(pMac, LOGP, FL("tx_timer_activate failed"));
            }    
       }
    }
#endif


   
    mlmStartCnf.resultCode = limMlmAddBss(pMac, pMlmStartReq,psessionEntry);

end:
    /*                      */
     mlmStartCnf.sessionId = pMlmStartReq->sessionId;
    
    //                                            
    vos_mem_free(pMsgBuf);

    //
    //                                                      
    //                                                  
    //                                                     
    //                         
    //
    if( eSIR_SME_SUCCESS != mlmStartCnf.resultCode )
      limPostSmeMessage(pMac, LIM_MLM_START_CNF, (tANI_U32 *) &mlmStartCnf);
} /*                             */


/*
                                                 
                                                                       
                       
                                                                      
                               
*/

static tANI_U8 __limMlmScanAllowed(tpAniSirGlobal pMac)
{
    int i;

    if(pMac->lim.gLimMlmState != eLIM_MLM_IDLE_STATE)
    {
        return FALSE;
    }
    for(i =0; i < pMac->lim.maxBssId; i++)
    {
        if(pMac->lim.gpSession[i].valid == TRUE)
        {
            if(!( ( (  (pMac->lim.gpSession[i].bssType == eSIR_INFRASTRUCTURE_MODE) ||
                       (pMac->lim.gpSession[i].limSystemRole == eLIM_BT_AMP_STA_ROLE))&&
                       (pMac->lim.gpSession[i].limMlmState == eLIM_MLM_LINK_ESTABLISHED_STATE) )||
                  
                  (    ( (pMac->lim.gpSession[i].bssType == eSIR_IBSS_MODE)||
                           (pMac->lim.gpSession[i].limSystemRole == eLIM_BT_AMP_AP_ROLE)||
                           (pMac->lim.gpSession[i].limSystemRole == eLIM_BT_AMP_STA_ROLE) )&&
                       (pMac->lim.gpSession[i].limMlmState == eLIM_MLM_BSS_STARTED_STATE) )
               ||  ( ( ( (pMac->lim.gpSession[i].bssType == eSIR_INFRA_AP_MODE) 
                      && ( pMac->lim.gpSession[i].pePersona == VOS_P2P_GO_MODE) )
                    || (pMac->lim.gpSession[i].limSystemRole == eLIM_AP_ROLE) )
                  && (pMac->lim.gpSession[i].limMlmState == eLIM_MLM_BSS_STARTED_STATE) )
                ))
            {
                return FALSE;

            }
        }
    }

    return TRUE;
}



/* 
                         
  
           
                                                          
           
  
        
  
              
  
       
  
                                                    
                                                        
               
 */

static void
limProcessMlmScanReq(tpAniSirGlobal pMac, tANI_U32 *pMsgBuf)
{
    tLimMlmScanCnf       mlmScanCnf;
    tANI_U8 i = 0;
    tANI_U32 val = 0;

    if (pMac->lim.gLimSystemInScanLearnMode)
    {
        PELOGE(limLog(pMac, LOGE,
               FL("Sending START_SCAN from LIM while one req is pending"));)
        vos_mem_free(pMsgBuf);
        /*                   */        
        mlmScanCnf.resultCode = eSIR_SME_SCAN_FAILED;
        mlmScanCnf.scanResultLength = 0;
        limPostSmeMessage(pMac,
                         LIM_MLM_SCAN_CNF,
                    (tANI_U32 *) &mlmScanCnf);
        return;
    }

 if(__limMlmScanAllowed(pMac) && 
    (((tLimMlmScanReq *) pMsgBuf)->channelList.numChannels != 0))
        
    {
        //                             
        pMac->lim.gpLimMlmScanReq = (tLimMlmScanReq *) pMsgBuf;

       PELOG3(limLog(pMac, LOG3, FL("Number of channels to scan are %d "),
               pMac->lim.gpLimMlmScanReq->channelList.numChannels);)

        pMac->lim.gLimPrevMlmState = pMac->lim.gLimMlmState;

        if (pMac->lim.gpLimMlmScanReq->scanType == eSIR_ACTIVE_SCAN)
            pMac->lim.gLimMlmState = eLIM_MLM_WT_PROBE_RESP_STATE;
        else //                  
            pMac->lim.gLimMlmState = eLIM_MLM_PASSIVE_SCAN_STATE;
        MTRACE(macTrace(pMac, TRACE_CODE_MLM_STATE, NO_SESSION, pMac->lim.gLimMlmState));

        pMac->lim.gLimSystemInScanLearnMode = 1;

        /*                                                                         
                                        */

        pMac->lim.gTotalScanDuration = 0;

        if (wlan_cfgGetInt(pMac, WNI_CFG_PASSIVE_MAXIMUM_CHANNEL_TIME, &val) != eSIR_SUCCESS)
        {
            /*
                                              
                                   
             */
            limLog(pMac, LOGP, FL("could not retrieve passive max channel value"));

            /*                              */
            val = 110;
        }

        for (i = 0; i < pMac->lim.gpLimMlmScanReq->channelList.numChannels; i++) {
            tANI_U8 channelNum = pMac->lim.gpLimMlmScanReq->channelList.channelNumber[i];

            if (limActiveScanAllowed(pMac, channelNum)) {
                /*                                                                    */
                pMac->lim.gTotalScanDuration += pMac->lim.gpLimMlmScanReq->minChannelTime + pMac->lim.gpLimMlmScanReq->maxChannelTime;
            } else {
                /*                                                                        
                                                                       
                 */
                pMac->lim.gTotalScanDuration += val;
            }
        }

        /*                                                                    */
        pMac->lim.gTotalScanDuration += 5;
        limSetScanMode(pMac);
    }
    else
    {
        /* 
                                                            
                                                            
                                  
                    
         */
        limLog(pMac, LOGW,
               FL("received unexpected MLM_SCAN_REQ in state %X OR zero number of channels: %X"),
               pMac->lim.gLimMlmState, ((tLimMlmScanReq *) pMsgBuf)->channelList.numChannels);
        limPrintMlmState(pMac, LOGW, pMac->lim.gLimMlmState);

        //                              
        //                          
        vos_mem_free(pMsgBuf);

        //                                             

        mlmScanCnf.resultCode = eSIR_SME_INVALID_PARAMETERS;
        mlmScanCnf.scanResultLength = 0;
        limPostSmeMessage(pMac,
                         LIM_MLM_SCAN_CNF,
                         (tANI_U32 *) &mlmScanCnf);
    }
} /*                            */

#ifdef FEATURE_OEM_DATA_SUPPORT
static void limProcessMlmOemDataReq(tpAniSirGlobal pMac, tANI_U32 *pMsgBuf)
{
    tLimMlmOemDataRsp*     pMlmOemDataRsp;
    
    if (((pMac->lim.gLimMlmState == eLIM_MLM_IDLE_STATE) ||
         (pMac->lim.gLimMlmState == eLIM_MLM_JOINED_STATE) ||
         (pMac->lim.gLimMlmState == eLIM_MLM_AUTHENTICATED_STATE) ||
         (pMac->lim.gLimMlmState == eLIM_MLM_BSS_STARTED_STATE) ||
         (pMac->lim.gLimMlmState == eLIM_MLM_LINK_ESTABLISHED_STATE)))
    {
        //                                       
        pMac->lim.gpLimMlmOemDataReq = (tLimMlmOemDataReq*)pMsgBuf;

        pMac->lim.gLimPrevMlmState = pMac->lim.gLimMlmState;

        MTRACE(macTrace(pMac, TRACE_CODE_MLM_STATE, NO_SESSION, pMac->lim.gLimMlmState));

        //                               
        limSuspendLink(pMac, eSIR_CHECK_LINK_TRAFFIC_BEFORE_SCAN, limSetOemDataReqMode, NULL);
    }
    else
    {
        /* 
                                                                
                    
         */

        PELOGW(limLog(pMac, LOGW, FL("OEM_DATA: unexpected LIM_MLM_OEM_DATA_REQ in invalid state %X"),pMac->lim.gLimMlmState);)

        limPrintMlmState(pMac, LOGW, pMac->lim.gLimMlmState);

        //                          
        vos_mem_free(pMsgBuf);

        //                                             
        pMlmOemDataRsp = vos_mem_malloc(sizeof(tLimMlmOemDataRsp));
        if ( pMlmOemDataRsp != NULL)
        {
            limPostSmeMessage(pMac, LIM_MLM_OEM_DATA_CNF, (tANI_U32*)pMlmOemDataRsp);
            vos_mem_free(pMlmOemDataRsp);
        }
        else
        {
            limLog(pMac, LOGP, FL("Could not allocate memory for pMlmOemDataRsp"));
            return;
        }
    }

    return;
}
#endif //                        


/* 
                                     
  
           
                                                               
               
  
        
                            
                                                              
                                          
                             
                                                             
  
              
  
       
  
                                                    
                                            
                                                                     
               
 */
static void
limProcessMlmPostJoinSuspendLink(tpAniSirGlobal pMac, eHalStatus status, tANI_U32 *ctx)
{
    tANI_U8             chanNum, secChanOffset;
    tLimMlmJoinCnf      mlmJoinCnf;
    tpPESession         psessionEntry = (tpPESession)ctx;
    tSirLinkState       linkState;

    if( eHAL_STATUS_SUCCESS != status )
    {
       limLog(pMac, LOGE, FL("Suspend link(NOTIFY_BSS) failed. still proceeding with join"));
    }
    psessionEntry->limPrevMlmState = psessionEntry->limMlmState;
    psessionEntry->limMlmState = eLIM_MLM_WT_JOIN_BEACON_STATE;
    MTRACE(macTrace(pMac, TRACE_CODE_MLM_STATE, psessionEntry->peSessionId, psessionEntry->limMlmState));

    limDeactivateAndChangeTimer(pMac, eLIM_JOIN_FAIL_TIMER);

    //                                                
    pMac->lim.limTimers.gLimJoinFailureTimer.sessionId = psessionEntry->peSessionId;

    linkState = ((psessionEntry->limSystemRole == eLIM_BT_AMP_STA_ROLE) ? eSIR_LINK_BTAMP_PREASSOC_STATE : eSIR_LINK_PREASSOC_STATE);
    limLog(pMac, LOG1, FL("[limProcessMlmJoinReq]: linkState:%d"),linkState);

    if (limSetLinkState(pMac, linkState, 
         psessionEntry->pLimMlmJoinReq->bssDescription.bssId, 
         psessionEntry->selfMacAddr, NULL, NULL) != eSIR_SUCCESS )
    {
        limLog(pMac, LOGE, FL("limSetLinkState to eSIR_LINK_PREASSOC_STATE Failed!!"));
        mlmJoinCnf.resultCode = eSIR_SME_RESOURCES_UNAVAILABLE;
        psessionEntry->limMlmState = eLIM_MLM_IDLE_STATE;
        MTRACE(macTrace(pMac, TRACE_CODE_MLM_STATE, psessionEntry->peSessionId, psessionEntry->limMlmState));
        goto error;
    }

    /*                                                              */
    // chanNum = pMac->lim.gpLimMlmJoinReq->bssDescription.channelId;
    
    chanNum = psessionEntry->currentOperChannel;
    secChanOffset = psessionEntry->htSecondaryChannelOffset;
    //                                                           
    psessionEntry->channelChangeReasonCode = LIM_SWITCH_CHANNEL_JOIN;
#if  defined (WLAN_FEATURE_VOWIFI_11R) || defined (FEATURE_WLAN_CCX) || defined(FEATURE_WLAN_LFR)
    psessionEntry->pLimMlmReassocRetryReq = NULL;
#endif

    limSetChannel(pMac, chanNum, secChanOffset, psessionEntry->maxTxPower, psessionEntry->peSessionId); 

    return;
error:
    mlmJoinCnf.resultCode = eSIR_SME_RESOURCES_UNAVAILABLE;
    mlmJoinCnf.sessionId = psessionEntry->peSessionId;
    mlmJoinCnf.protStatusCode = eSIR_MAC_UNSPEC_FAILURE_STATUS;
    limPostSmeMessage(pMac, LIM_MLM_JOIN_CNF, (tANI_U32 *) &mlmJoinCnf);

}



/* 
                         
  
           
                                                          
           
  
        
                              
                                                           
                                     
                                                         
            
                                                  
                                
                                   
                                                         
                              
                        
                                                          
                         
                              
                                 
  
              
  
       
  
                                                    
                                                        
               
 */

static void
limProcessMlmJoinReq(tpAniSirGlobal pMac, tANI_U32 *pMsgBuf)
{
    tLimMlmJoinCnf      mlmJoinCnf;
    tANI_U8             sessionId;
    tpPESession         psessionEntry;

    sessionId = ((tpLimMlmJoinReq)pMsgBuf)->sessionId;

    if((psessionEntry = peFindSessionBySessionId(pMac,sessionId))== NULL)
    {
        limLog(pMac, LOGP, FL("session does not exist for given sessionId"));

        goto error;
    }

    if (( (psessionEntry->limSystemRole != eLIM_AP_ROLE ) && (psessionEntry->limSystemRole != eLIM_BT_AMP_AP_ROLE )) &&
          ( (psessionEntry->limMlmState == eLIM_MLM_IDLE_STATE) ||
            (psessionEntry->limMlmState == eLIM_MLM_JOINED_STATE))  &&
        (SIR_MAC_GET_ESS( ((tpLimMlmJoinReq) pMsgBuf)->bssDescription.capabilityInfo) !=
             SIR_MAC_GET_IBSS( ((tpLimMlmJoinReq) pMsgBuf)->bssDescription.capabilityInfo)))
    {
        #if 0
        if (pMac->lim.gpLimMlmJoinReq)
            vos_mem_free(pMac->lim.gpLimMlmJoinReq);
        #endif //                                

        //                                   
        
        psessionEntry->pLimMlmJoinReq =(tpLimMlmJoinReq) pMsgBuf;
        
        if( isLimSessionOffChannel(pMac, sessionId) )
        {
          //            
          limSuspendLink(pMac, eSIR_DONT_CHECK_LINK_TRAFFIC_BEFORE_SCAN, 
                   limProcessMlmPostJoinSuspendLink, (tANI_U32*)psessionEntry );
        }
        else
        {
          //                        
          limProcessMlmPostJoinSuspendLink( pMac, eHAL_STATUS_SUCCESS,
                                                    (tANI_U32*) psessionEntry );
        }
                
        return;
    }
    else
    {
        /* 
                                                                      
                                    
                                                                 
              */
        PELOGE(limLog(pMac, LOGE,
               FL("Unexpected Join request for role %d state %X"),
               psessionEntry->limSystemRole,
               psessionEntry->limMlmState);)
        limPrintMlmState(pMac, LOGE, psessionEntry->limMlmState);
        
        limLog(pMac, LOGE, FL("Unexpected Join request for role %d state %X"),
               psessionEntry->limSystemRole, psessionEntry->limMlmState);
    }

error: 

        
        mlmJoinCnf.resultCode = eSIR_SME_RESOURCES_UNAVAILABLE;
        mlmJoinCnf.sessionId = sessionId;
        mlmJoinCnf.protStatusCode = eSIR_MAC_UNSPEC_FAILURE_STATUS;
        limPostSmeMessage(pMac, LIM_MLM_JOIN_CNF, (tANI_U32 *) &mlmJoinCnf);


} /*                            */



/* 
                         
  
           
                                                          
           
  
        
  
              
  
       
  
                                                    
                                                        
               
 */

static void
limProcessMlmAuthReq(tpAniSirGlobal pMac, tANI_U32 *pMsgBuf)
{
    tANI_U32                numPreAuthContexts;
    tSirMacAddr             currentBssId;
    tSirMacAuthFrameBody    authFrameBody;
    tLimMlmAuthCnf          mlmAuthCnf;
    struct tLimPreAuthNode  *preAuthNode;
    tpDphHashNode           pStaDs;
    tANI_U8                 sessionId;
    tpPESession             psessionEntry;

    if(pMsgBuf == NULL)
    {
        PELOGE(limLog(pMac, LOGE,FL("Buffer is Pointing to NULL"));)
        return;
    }

    pMac->lim.gpLimMlmAuthReq = (tLimMlmAuthReq *) pMsgBuf;
    sessionId = pMac->lim.gpLimMlmAuthReq->sessionId;
    if((psessionEntry= peFindSessionBySessionId(pMac,sessionId) )== NULL)
    {
        limLog(pMac, LOGP, FL("Session Does not exist for given sessionId"));
        return;
    }

    /* 
                                     
                                             
                             
                                                      
                                                       
                 
     */
     #if 0
    if (wlan_cfgGetStr(pMac, WNI_CFG_BSSID, currentBssId, &cfg) !=
                                eSIR_SUCCESS)
    {
        //                                          
        limLog(pMac, LOGP, FL("could not retrieve BSSID"));
    }
    #endif //                 

    sirCopyMacAddr(currentBssId,psessionEntry->bssId);

    if (((((psessionEntry->limSystemRole== eLIM_STA_ROLE) || (psessionEntry->limSystemRole == eLIM_BT_AMP_STA_ROLE)) &&
          ((psessionEntry->limMlmState == eLIM_MLM_JOINED_STATE) ||
           (psessionEntry->limMlmState ==
                                  eLIM_MLM_LINK_ESTABLISHED_STATE))) ||
         ((psessionEntry->limSystemRole == eLIM_STA_IN_IBSS_ROLE) &&
          (psessionEntry->limMlmState == eLIM_MLM_BSS_STARTED_STATE))) &&
        (limIsGroupAddr(pMac->lim.gpLimMlmAuthReq->peerMacAddr)
                                                   == false) &&
        (limIsAuthAlgoSupported(
                        pMac,
                        pMac->lim.gpLimMlmAuthReq->authType,
                        psessionEntry) == true)
        )        
    {
        /* 
                                                    
                                                    
                                
                                                        
                           
                                                       
                                                              
         */
        if ((((psessionEntry->limSystemRole == eLIM_STA_ROLE) ||(psessionEntry->limSystemRole == eLIM_BT_AMP_STA_ROLE) )&&
             (psessionEntry->limMlmState ==
                                  eLIM_MLM_LINK_ESTABLISHED_STATE) &&
             (((pStaDs = dphGetHashEntry(pMac, DPH_STA_HASH_INDEX_PEER, &psessionEntry->dph.dphHashTable)) != NULL) &&
              (pMac->lim.gpLimMlmAuthReq->authType ==
                                   pStaDs->mlmStaContext.authType)) &&
             (vos_mem_compare(pMac->lim.gpLimMlmAuthReq->peerMacAddr,
                        currentBssId,
                        sizeof(tSirMacAddr)) )) ||
            (((preAuthNode =
               limSearchPreAuthList(
                     pMac,
                     pMac->lim.gpLimMlmAuthReq->peerMacAddr)) != NULL) &&
             (preAuthNode->authType ==
                                   pMac->lim.gpLimMlmAuthReq->authType)))
        {
           PELOG2(limLog(pMac, LOG2,
                   FL("Already have pre-auth context with peer"));
            limPrintMacAddr(pMac, pMac->lim.gpLimMlmAuthReq->peerMacAddr,
                            LOG2);)

            mlmAuthCnf.resultCode = (tSirResultCodes)
                                    eSIR_MAC_SUCCESS_STATUS;
            

            goto end;
        }
        else
        {
            if (wlan_cfgGetInt(pMac, WNI_CFG_MAX_NUM_PRE_AUTH,
                          (tANI_U32 *) &numPreAuthContexts) != eSIR_SUCCESS)
            {
                limLog(pMac, LOGP,
                   FL("Could not retrieve NumPreAuthLimit from CFG"));
            }

            if (pMac->lim.gLimNumPreAuthContexts == numPreAuthContexts)
            {
                PELOGW(limLog(pMac, LOGW,
                       FL("Number of pre-auth reached max limit"));)

                //                                      
                mlmAuthCnf.resultCode =
                               eSIR_SME_MAX_NUM_OF_PRE_AUTH_REACHED;

                goto end;
            }
        }

        //                               
        if (preAuthNode)
            limDeletePreAuthNode(pMac,
                                 pMac->lim.gpLimMlmAuthReq->peerMacAddr);

        psessionEntry->limPrevMlmState = psessionEntry->limMlmState;
        psessionEntry->limMlmState = eLIM_MLM_WT_AUTH_FRAME2_STATE;
        MTRACE(macTrace(pMac, TRACE_CODE_MLM_STATE, psessionEntry->peSessionId, psessionEntry->limMlmState));

        //                                     
        authFrameBody.authAlgoNumber =
                                  (tANI_U8) pMac->lim.gpLimMlmAuthReq->authType;
        authFrameBody.authTransactionSeqNumber = SIR_MAC_AUTH_FRAME_1;
        authFrameBody.authStatusCode = 0;
        limSendAuthMgmtFrame(pMac,
                             &authFrameBody,
                             pMac->lim.gpLimMlmAuthReq->peerMacAddr,
                             LIM_NO_WEP_IN_FC,psessionEntry);

        //                                                
        pMac->lim.limTimers.gLimAuthFailureTimer.sessionId = sessionId;
 
        //                            
        MTRACE(macTrace(pMac, TRACE_CODE_TIMER_ACTIVATE, psessionEntry->peSessionId, eLIM_AUTH_FAIL_TIMER));
        if (tx_timer_activate(&pMac->lim.limTimers.gLimAuthFailureTimer)
                                       != TX_SUCCESS)
        {
            //                                     
            //          
            limLog(pMac, LOGP,
                   FL("could not start Auth failure timer"));
            //                                 
            limProcessAuthFailureTimeout(pMac);
        }

        return;
    }
    else
    {
        /* 
                                   
                                                            
         */
        mlmAuthCnf.resultCode = eSIR_SME_INVALID_PARAMETERS;

        goto end;
    }

end:
    vos_mem_copy((tANI_U8 *) &mlmAuthCnf.peerMacAddr,
                 (tANI_U8 *) &pMac->lim.gpLimMlmAuthReq->peerMacAddr,
                 sizeof(tSirMacAddr));

    mlmAuthCnf.authType = pMac->lim.gpLimMlmAuthReq->authType;
    mlmAuthCnf.sessionId = sessionId;

    //                          
    //                              
    vos_mem_free( pMac->lim.gpLimMlmAuthReq);
    pMac->lim.gpLimMlmAuthReq = NULL;
    limPostSmeMessage(pMac, LIM_MLM_AUTH_CNF, (tANI_U32 *) &mlmAuthCnf);
} /*                            */



/* 
                          
  
           
                                                           
           
  
        
  
              
  
       
  
                                                    
                                                        
               
 */

static void
limProcessMlmAssocReq(tpAniSirGlobal pMac, tANI_U32 *pMsgBuf)
{
    tSirMacAddr              currentBssId;
    tLimMlmAssocReq          *pMlmAssocReq;
    tLimMlmAssocCnf          mlmAssocCnf;
    tpPESession               psessionEntry;
   //                                    

    if(pMsgBuf == NULL)
    {
        PELOGE(limLog(pMac, LOGE,FL("Buffer is Pointing to NULL"));)
        return;
    }
    pMlmAssocReq = (tLimMlmAssocReq *) pMsgBuf;

    if( (psessionEntry = peFindSessionBySessionId(pMac,pMlmAssocReq->sessionId) )== NULL) 
    {
        limLog(pMac, LOGP,FL("Session Does not exist for given sessionID"));
        vos_mem_free(pMlmAssocReq);
        return;
    }

    #if 0
    if (wlan_cfgGetStr(pMac, WNI_CFG_BSSID, currentBssId, &cfg) !=
                                eSIR_SUCCESS)
    {
        //                                          
        limLog(pMac, LOGP, FL("could not retrieve BSSID"));
    }
    #endif //                 
    sirCopyMacAddr(currentBssId,psessionEntry->bssId);
    
    if ( (psessionEntry->limSystemRole != eLIM_AP_ROLE && psessionEntry->limSystemRole != eLIM_BT_AMP_AP_ROLE) &&
         (psessionEntry->limMlmState == eLIM_MLM_AUTHENTICATED_STATE || psessionEntry->limMlmState == eLIM_MLM_JOINED_STATE) &&
         (vos_mem_compare(pMlmAssocReq->peerMacAddr, currentBssId, sizeof(tSirMacAddr))) )
    {

        //                                                         
        pMac->lim.limTimers.gLimAssocFailureTimer.sessionId = pMlmAssocReq->sessionId;

        psessionEntry->limPrevMlmState = psessionEntry->limMlmState;
        psessionEntry->limMlmState = eLIM_MLM_WT_ASSOC_RSP_STATE;
        MTRACE(macTrace(pMac, TRACE_CODE_MLM_STATE, psessionEntry->peSessionId, psessionEntry->limMlmState));
 
        //                                            
        limSendAssocReqMgmtFrame(pMac, pMlmAssocReq,psessionEntry);

  //                                                                          
    if ((psessionEntry->bssType == eSIR_BTAMP_STA_MODE)||
        ((psessionEntry->bssType == eSIR_BTAMP_AP_MODE) && (psessionEntry->limSystemRole == eLIM_BT_AMP_STA_ROLE)))
    {
       if(limSetLinkState(pMac, eSIR_LINK_BTAMP_POSTASSOC_STATE, currentBssId, 
           psessionEntry->selfMacAddr, NULL, NULL) != eSIR_SUCCESS)
            PELOGE(limLog(pMac, LOGE,  FL("Failed to set the LinkState"));)
    }
        //                                 
        MTRACE(macTrace(pMac, TRACE_CODE_TIMER_ACTIVATE, psessionEntry->peSessionId, eLIM_ASSOC_FAIL_TIMER));
        if (tx_timer_activate(&pMac->lim.limTimers.gLimAssocFailureTimer)
                                              != TX_SUCCESS)
        {
            //                                      
            //          
            limLog(pMac, LOGP,
                   FL("could not start Association failure timer"));
            //                                  
            limProcessAssocFailureTimeout(pMac,LIM_ASSOC );
           
        }

        return;
    }
    else
    {
        /* 
                                                               
                                                             
                                                               
                                                             
         */

        //          
        PELOGW(limLog(pMac, LOGW,
           FL("received unexpected MLM_ASSOC_CNF in state %X for role=%d, MAC addr= "),
           psessionEntry->limMlmState,
           psessionEntry->limSystemRole);)
        limPrintMacAddr(pMac, pMlmAssocReq->peerMacAddr, LOGW);
        limPrintMlmState(pMac, LOGW, psessionEntry->limMlmState);

        mlmAssocCnf.resultCode = eSIR_SME_INVALID_PARAMETERS;
        mlmAssocCnf.protStatusCode = eSIR_MAC_UNSPEC_FAILURE_STATUS;

        goto end;
    }

end:
    /*                     */
    mlmAssocCnf.sessionId = pMlmAssocReq->sessionId;

    //                                       
    vos_mem_free(pMlmAssocReq);

    limPostSmeMessage(pMac, LIM_MLM_ASSOC_CNF, (tANI_U32 *) &mlmAssocCnf);
} /*                             */



/* 
                            
  
           
                                                             
           
  
        
  
              
  
       
  
                                                    
                                                        
               
 */

static void
limProcessMlmReassocReq(tpAniSirGlobal pMac, tANI_U32 *pMsgBuf)
{
    tANI_U8                       chanNum, secChannelOffset;
    struct tLimPreAuthNode  *pAuthNode;
    tLimMlmReassocReq       *pMlmReassocReq;
    tLimMlmReassocCnf       mlmReassocCnf;
    tpPESession             psessionEntry;

    if(pMsgBuf == NULL)
    {
        PELOGE(limLog(pMac, LOGE,FL("Buffer is Pointing to NULL"));)
        return;
    }

    pMlmReassocReq = (tLimMlmReassocReq *) pMsgBuf;
    
    if((psessionEntry = peFindSessionBySessionId(pMac,pMlmReassocReq->sessionId)) == NULL)
    {
        PELOGE(limLog(pMac, LOGE,FL("Session Does not exist for given sessionId"));)
        vos_mem_free(pMlmReassocReq);
        return;
    }
    
    if (((psessionEntry->limSystemRole != eLIM_AP_ROLE) && (psessionEntry->limSystemRole != eLIM_BT_AMP_AP_ROLE)) &&
         (psessionEntry->limMlmState == eLIM_MLM_LINK_ESTABLISHED_STATE))
    {
        if (psessionEntry->pLimMlmReassocReq)
            vos_mem_free(psessionEntry->pLimMlmReassocReq);

        /*                                                          */
        //                                      
        psessionEntry->pLimMlmReassocReq = pMlmReassocReq;
        
        //                                            
        pAuthNode = limSearchPreAuthList(pMac, psessionEntry->limReAssocbssId);

        if (!pAuthNode &&
            (!vos_mem_compare(pMlmReassocReq->peerMacAddr,
                       psessionEntry->bssId,
                       sizeof(tSirMacAddr)) ))
        {
            //                                           
            //                                        
            //               
            //                                              
            mlmReassocCnf.resultCode = eSIR_SME_STA_NOT_AUTHENTICATED;
            mlmReassocCnf.protStatusCode = eSIR_MAC_UNSPEC_FAILURE_STATUS;

            goto end;
        }

        //                                        
        pMac->lim.limTimers.gLimReassocFailureTimer.sessionId = pMlmReassocReq->sessionId;

        psessionEntry->limPrevMlmState = psessionEntry->limMlmState;
        psessionEntry->limMlmState    = eLIM_MLM_WT_REASSOC_RSP_STATE;
        MTRACE(macTrace(pMac, TRACE_CODE_MLM_STATE, psessionEntry->peSessionId, psessionEntry->limMlmState));

#if 0
        //                             
        if (wlan_cfgSetStr(pMac, WNI_CFG_BSSID,
                      pMac->lim.gLimReassocBssId,
                      sizeof(tSirMacAddr)) != eSIR_SUCCESS)
        {
            //                                           
            limLog(pMac, LOGP, FL("could not update BSSID at CFG"));
        }
#endif //                 

    /*                       */
   //                                                                        

        /* 
                                                  
                           
         */

        chanNum = psessionEntry->limReassocChannelId;
        secChannelOffset = psessionEntry->reAssocHtSecondaryChannelOffset;

        /*                                                            */
        //                                            

        //                                         
        limApplyConfiguration(pMac,psessionEntry);

        //                                                           
        /*                                                    */
        //                                                       
        psessionEntry->channelChangeReasonCode = LIM_SWITCH_CHANNEL_REASSOC;

        /*                                                         */
        limSetChannel(pMac, chanNum, secChannelOffset, psessionEntry->maxTxPower, psessionEntry->peSessionId);

        return;
    }
    else
    {
        /* 
                                                       
                                                         
                           
         */

        //          
        PELOGW(limLog(pMac, LOGW,
           FL("received unexpected MLM_REASSOC_CNF in state %X for role=%d, MAC addr= "),
           psessionEntry->limMlmState,
           psessionEntry->limSystemRole);)
        limPrintMacAddr(pMac, pMlmReassocReq->peerMacAddr,
                        LOGW);
        limPrintMlmState(pMac, LOGW, psessionEntry->limMlmState);

        mlmReassocCnf.resultCode = eSIR_SME_INVALID_PARAMETERS;
        mlmReassocCnf.protStatusCode = eSIR_MAC_UNSPEC_FAILURE_STATUS;

        goto end;
    }

end:
    mlmReassocCnf.protStatusCode = eSIR_MAC_UNSPEC_FAILURE_STATUS;
    /*                    */
    mlmReassocCnf.sessionId = pMlmReassocReq->sessionId;
    //                                         
    vos_mem_free(pMlmReassocReq);
    psessionEntry->pLimReAssocReq = NULL;

    limPostSmeMessage(pMac, LIM_MLM_REASSOC_CNF, (tANI_U32 *) &mlmReassocCnf);
} /*                               */


static void
limProcessMlmDisassocReqNtf(tpAniSirGlobal pMac, eHalStatus suspendStatus, tANI_U32 *pMsgBuf)
{
    tANI_U16                 aid;
    tSirMacAddr              currentBssId;
    tpDphHashNode            pStaDs;
    tLimMlmDisassocReq       *pMlmDisassocReq;
    tLimMlmDisassocCnf       mlmDisassocCnf;
    tpPESession              psessionEntry;
    extern tANI_BOOLEAN     sendDisassocFrame;

    if(eHAL_STATUS_SUCCESS != suspendStatus)
    {
        PELOGE(limLog(pMac, LOGE,FL("Suspend Status is not success %X"), suspendStatus);)
#if 0
        //                                                                  
        mlmDisassocCnf.resultCode = eSIR_SME_REFUSED;
        goto end;
#endif
    }

    pMlmDisassocReq = (tLimMlmDisassocReq *) pMsgBuf;


    if((psessionEntry = peFindSessionBySessionId(pMac,pMlmDisassocReq->sessionId))== NULL)
    {
    
        PELOGE(limLog(pMac, LOGE,
                  FL("session does not exist for given sessionId"));)
        mlmDisassocCnf.resultCode = eSIR_SME_INVALID_PARAMETERS;
        goto end;
    }
    #if 0
    if (wlan_cfgGetStr(pMac, WNI_CFG_BSSID, currentBssId, &cfg) !=
                                eSIR_SUCCESS)
    {
        //                                          
        limLog(pMac, LOGP, FL("could not retrieve BSSID"));
    }
    #endif //              
    sirCopyMacAddr(currentBssId,psessionEntry->bssId);

    switch (psessionEntry->limSystemRole)
    {
        case eLIM_STA_ROLE:
        case eLIM_BT_AMP_STA_ROLE:    
            if ( !vos_mem_compare(pMlmDisassocReq->peerMacAddr,
                          currentBssId,
                          sizeof(tSirMacAddr)) )
            {
                PELOGW(limLog(pMac, LOGW,
                   FL("received MLM_DISASSOC_REQ with invalid BSS id "));)
                limPrintMacAddr(pMac, pMlmDisassocReq->peerMacAddr, LOGW);

                //                                       

                mlmDisassocCnf.resultCode      =
                                       eSIR_SME_INVALID_PARAMETERS;

                goto end;
            }

            break;

        case eLIM_STA_IN_IBSS_ROLE:

            break;

        default: //             

            //             
            break;

    } //                                          

    /* 
                                                          
                                
     */
    pStaDs = dphLookupHashEntry(pMac, pMlmDisassocReq->peerMacAddr, &aid, &psessionEntry->dph.dphHashTable);
    if ((pStaDs == NULL) ||
        (pStaDs &&
         ((pStaDs->mlmStaContext.mlmState !=
                             eLIM_MLM_LINK_ESTABLISHED_STATE) &&
          (pStaDs->mlmStaContext.mlmState !=
                             eLIM_MLM_WT_ASSOC_CNF_STATE) &&
          (pStaDs->mlmStaContext.mlmState !=
                             eLIM_MLM_ASSOCIATED_STATE))))
    {
        /* 
                                                              
                                                 
                    
         */
        PELOGW(limLog(pMac, LOGW,
           FL("received MLM_DISASSOC_REQ for STA that either has no context or in some transit state, Addr= "));)
           limPrintMacAddr(pMac, pMlmDisassocReq->peerMacAddr, LOGW);

        //                                       

        mlmDisassocCnf.resultCode = eSIR_SME_INVALID_PARAMETERS;

        goto end;
    }

    //                                     
    pStaDs->mlmStaContext.disassocReason = (tSirMacReasonCodes)
                                           pMlmDisassocReq->reasonCode;
    pStaDs->mlmStaContext.cleanupTrigger = pMlmDisassocReq->disassocTrigger;

    //                                        
    if (sendDisassocFrame && (pMlmDisassocReq->reasonCode != eSIR_MAC_DISASSOC_DUE_TO_FTHANDOFF_REASON))
    {
        pMac->lim.limDisassocDeauthCnfReq.pMlmDisassocReq = pMlmDisassocReq;
        /*                                                        
                                                                 
                                                             
         */
        pStaDs->mlmStaContext.mlmState   = eLIM_MLM_WT_DEL_STA_RSP_STATE;

        /*                                                            
                                         */
        if ((pMlmDisassocReq->reasonCode == eSIR_MAC_DISASSOC_DUE_TO_INACTIVITY_REASON) &&
            (psessionEntry->limSystemRole == eLIM_AP_ROLE))
        {

             limSendDisassocMgmtFrame(pMac,
                                 pMlmDisassocReq->reasonCode,
                                 pMlmDisassocReq->peerMacAddr,
                                 psessionEntry, FALSE);

             /*                                            */
             limSendDisassocCnf(pMac);
        }
        else
        {
             limSendDisassocMgmtFrame(pMac,
                                 pMlmDisassocReq->reasonCode,
                                 pMlmDisassocReq->peerMacAddr,
                                 psessionEntry, TRUE);
        }
    }
    else
    {
       /*                                */
       sendDisassocFrame = 1;
       //                                       
       if(eSIR_SUCCESS != limCleanupRxPath(pMac, pStaDs,psessionEntry))
       {
           mlmDisassocCnf.resultCode = eSIR_SME_RESOURCES_UNAVAILABLE;
           goto end;
       }
       //                                            
       vos_mem_free(pMlmDisassocReq);
    }

    return;

end:
    vos_mem_copy((tANI_U8 *) &mlmDisassocCnf.peerMacAddr,
                 (tANI_U8 *) pMlmDisassocReq->peerMacAddr,
                 sizeof(tSirMacAddr));
    mlmDisassocCnf.aid = pMlmDisassocReq->aid;
    mlmDisassocCnf.disassocTrigger = pMlmDisassocReq->disassocTrigger;
    
    /*                     */
    mlmDisassocCnf.sessionId = pMlmDisassocReq->sessionId;

    //                                             
    vos_mem_free(pMlmDisassocReq);

    limPostSmeMessage(pMac,
                      LIM_MLM_DISASSOC_CNF,
                      (tANI_U32 *) &mlmDisassocCnf);
}

tANI_BOOLEAN limCheckDisassocDeauthAckPending(tpAniSirGlobal pMac,
                                              tANI_U8 *staMac
                                              )
{
    tLimMlmDisassocReq      *pMlmDisassocReq;
    tLimMlmDeauthReq        *pMlmDeauthReq;
    pMlmDisassocReq = pMac->lim.limDisassocDeauthCnfReq.pMlmDisassocReq;
    pMlmDeauthReq = pMac->lim.limDisassocDeauthCnfReq.pMlmDeauthReq;
    if (
            (pMlmDisassocReq &&
             (vos_mem_compare((tANI_U8 *) staMac,
                              (tANI_U8 *) &pMlmDisassocReq->peerMacAddr,
                              sizeof(tSirMacAddr)))) 
            ||
            (pMlmDeauthReq &&
             (vos_mem_compare((tANI_U8 *) staMac,
                              (tANI_U8 *) &pMlmDeauthReq->peerMacAddr,
                               sizeof(tSirMacAddr))))
       )
    {
        PELOG1(limLog(pMac, LOG1, FL("Disassoc/Deauth ack pending"));)
        return eANI_BOOLEAN_TRUE;
    }
     else
     {
        PELOG1(limLog(pMac, LOG1, FL("Disassoc/Deauth Ack not pending"));)
        return eANI_BOOLEAN_FALSE;
     }
}

void limCleanUpDisassocDeauthReq(tpAniSirGlobal pMac,
        tANI_U8 *staMac,
        tANI_BOOLEAN cleanRxPath)
{
    tLimMlmDisassocReq       *pMlmDisassocReq;
    tLimMlmDeauthReq        *pMlmDeauthReq;
    pMlmDisassocReq = pMac->lim.limDisassocDeauthCnfReq.pMlmDisassocReq;
    if (pMlmDisassocReq &&
            (vos_mem_compare((tANI_U8 *) staMac,
                             (tANI_U8 *) &pMlmDisassocReq->peerMacAddr,
                             sizeof(tSirMacAddr))))
    {
        if (cleanRxPath)
        {
            limProcessDisassocAckTimeout(pMac);
        }
        else
        {
            if (tx_timer_running(&pMac->lim.limTimers.gLimDisassocAckTimer))
            {
                limDeactivateAndChangeTimer(pMac, eLIM_DISASSOC_ACK_TIMER);
            }
            vos_mem_free(pMlmDisassocReq);
            pMac->lim.limDisassocDeauthCnfReq.pMlmDisassocReq = NULL;
        }
    }

    pMlmDeauthReq = pMac->lim.limDisassocDeauthCnfReq.pMlmDeauthReq;
    if (pMlmDeauthReq &&
            (vos_mem_compare((tANI_U8 *) staMac,
                             (tANI_U8 *) &pMlmDeauthReq->peerMacAddr,
                             sizeof(tSirMacAddr))))
    {
        if (cleanRxPath)
        {
            limProcessDeauthAckTimeout(pMac);
        }
        else
        {
            if (tx_timer_running(&pMac->lim.limTimers.gLimDeauthAckTimer))
            {
                limDeactivateAndChangeTimer(pMac, eLIM_DEAUTH_ACK_TIMER);
            }
            vos_mem_free(pMlmDeauthReq);
            pMac->lim.limDisassocDeauthCnfReq.pMlmDeauthReq = NULL;
        }
    }
}

void limProcessDisassocAckTimeout(tpAniSirGlobal pMac)
{
    limSendDisassocCnf(pMac);
}

/* 
                             
  
           
                                                              
           
  
        
  
              
  
       
  
                                                    
                                                        
               
 */

static void
limProcessMlmDisassocReq(tpAniSirGlobal pMac, tANI_U32 *pMsgBuf)
{
//                                 
//                                          
//                                    
    tLimMlmDisassocReq       *pMlmDisassocReq;
//                                            
    tpPESession              psessionEntry;
//                                              

    if(pMsgBuf == NULL)
    {
        PELOGE(limLog(pMac, LOGE,FL("Buffer is Pointing to NULL"));)
        return;
    }

    pMlmDisassocReq = (tLimMlmDisassocReq *) pMsgBuf;

    if((psessionEntry = peFindSessionBySessionId(pMac,pMlmDisassocReq->sessionId))== NULL)
    {
    
        PELOGE(limLog(pMac, LOGE,
                  FL("session does not exist for given sessionId"));)
        return;
    }

    limProcessMlmDisassocReqNtf( pMac, eHAL_STATUS_SUCCESS, (tANI_U32*) pMsgBuf );
    
} /*                                */

static void
limProcessMlmDeauthReqNtf(tpAniSirGlobal pMac, eHalStatus suspendStatus, tANI_U32 *pMsgBuf)
{
    tANI_U16                aid;
    tSirMacAddr             currentBssId;
    tpDphHashNode           pStaDs;
    struct tLimPreAuthNode  *pAuthNode;
    tLimMlmDeauthReq        *pMlmDeauthReq;
    tLimMlmDeauthCnf        mlmDeauthCnf;
    tpPESession             psessionEntry;


    if(eHAL_STATUS_SUCCESS != suspendStatus)
    {
        PELOGE(limLog(pMac, LOGE,FL("Suspend Status is not success %X"), suspendStatus);)
#if 0
        //                                                                  
        mlmDisassocCnf.resultCode = eSIR_SME_REFUSED;
        goto end;
#endif
    }

    pMlmDeauthReq = (tLimMlmDeauthReq *) pMsgBuf;

    if((psessionEntry = peFindSessionBySessionId(pMac,pMlmDeauthReq->sessionId))== NULL)
    {
    
        PELOGE(limLog(pMac, LOGE, FL("session does not exist for given sessionId"));)
        vos_mem_free(pMlmDeauthReq);
        return;
    }
    #if 0
    if (wlan_cfgGetStr(pMac, WNI_CFG_BSSID, currentBssId, &cfg) !=
                                eSIR_SUCCESS)
    {
        //                                          
        limLog(pMac, LOGP, FL("could not retrieve BSSID"));
    }
    #endif //              
    sirCopyMacAddr(currentBssId,psessionEntry->bssId);

    switch (psessionEntry->limSystemRole)
    {
        case eLIM_STA_ROLE:
        case eLIM_BT_AMP_STA_ROLE:
            switch (psessionEntry->limMlmState)
            {
                case eLIM_MLM_IDLE_STATE:
                    //                             
                    //                               
                    //                                
                    //                                 
                    break;

                case eLIM_MLM_AUTHENTICATED_STATE:
                case eLIM_MLM_WT_ASSOC_RSP_STATE:
                case eLIM_MLM_LINK_ESTABLISHED_STATE:
                    if (!vos_mem_compare(pMlmDeauthReq->peerMacAddr,
                                  currentBssId,
                                  sizeof(tSirMacAddr)) )
                    {
                        PELOGW(limLog(pMac, LOGW,
                           FL("received MLM_DEAUTH_REQ with invalid BSS id "));)
                        PELOGE(limLog(pMac, LOGE, FL("Peer MAC Addr : "));)
                        limPrintMacAddr(pMac, pMlmDeauthReq->peerMacAddr,LOGE);

                        PELOGE(limLog(pMac, LOGE, FL("\n CFG BSSID Addr : "));)
                        limPrintMacAddr(pMac, currentBssId,LOGE);

                        //                                     

                        mlmDeauthCnf.resultCode = eSIR_SME_INVALID_PARAMETERS;

                        goto end;
                    }

                    if ((psessionEntry->limMlmState ==
                                       eLIM_MLM_AUTHENTICATED_STATE) ||
                         (psessionEntry->limMlmState ==
                                       eLIM_MLM_WT_ASSOC_RSP_STATE))
                    {
                        //                            
                        //               
                        limSendDeauthMgmtFrame(
                                   pMac,
                                   pMlmDeauthReq->reasonCode,
                                   pMlmDeauthReq->peerMacAddr,
                                   psessionEntry, FALSE);

                        //                                     
                        mlmDeauthCnf.resultCode = eSIR_SME_SUCCESS;
                        psessionEntry->limMlmState = eLIM_MLM_IDLE_STATE;
                        MTRACE(macTrace(pMac, TRACE_CODE_MLM_STATE, psessionEntry->peSessionId, psessionEntry->limMlmState));
                        goto end;
                    }
                    else
                    {
                        //                      
                        //                      
                        //      
                    }

                    break;

                default:

                    PELOGW(limLog(pMac, LOGW,
                       FL("received MLM_DEAUTH_REQ with in state %d for peer "),
                       psessionEntry->limMlmState);)
                    limPrintMacAddr(pMac, pMlmDeauthReq->peerMacAddr, LOGW);
                    limPrintMlmState(pMac, LOGW, psessionEntry->limMlmState);

                    //                                     
                    mlmDeauthCnf.resultCode =
                                    eSIR_SME_STA_NOT_AUTHENTICATED;

                    goto end;
            }

            break;

        case eLIM_STA_IN_IBSS_ROLE:
            vos_mem_free(pMlmDeauthReq);

            return;

        default: //             
            break;

    } //                                          

    /* 
                                                          
                                  
     */
    pStaDs = dphLookupHashEntry(pMac, pMlmDeauthReq->peerMacAddr, &aid, &psessionEntry->dph.dphHashTable);

    if (pStaDs == NULL)
    {
        //                                                     
        pAuthNode = limSearchPreAuthList(pMac,
                                pMlmDeauthReq->peerMacAddr);

        if (pAuthNode == NULL)
        {
            /* 
                                                            
                                                           
                                                  
             */
            PELOGW(limLog(pMac, LOGW,
               FL("received MLM_DEAUTH_REQ for STA that does not have context, Addr="));)
            limPrintMacAddr(pMac, pMlmDeauthReq->peerMacAddr, LOGW);

            mlmDeauthCnf.resultCode =
                                    eSIR_SME_STA_NOT_AUTHENTICATED;
        }
        else
        {
            mlmDeauthCnf.resultCode = eSIR_SME_SUCCESS;

            //                                   
            limDeletePreAuthNode(pMac, pMlmDeauthReq->peerMacAddr);

            //                                            
            limSendDeauthMgmtFrame(pMac,
                                   pMlmDeauthReq->reasonCode,
                                   pMlmDeauthReq->peerMacAddr,
                                   psessionEntry, FALSE);
        }

        goto end;
    }
    else if ((pStaDs->mlmStaContext.mlmState !=
                                     eLIM_MLM_LINK_ESTABLISHED_STATE) &&
             (pStaDs->mlmStaContext.mlmState !=
                                          eLIM_MLM_WT_ASSOC_CNF_STATE))
    {
        /* 
                                                        
                                         
         */
        PELOGW(limLog(pMac, LOGW,
           FL("received MLM_DEAUTH_REQ for STA that either has no context or in some transit state, Addr="));)
        limPrintMacAddr(pMac, pMlmDeauthReq->peerMacAddr, LOGW);

        //                                     

        mlmDeauthCnf.resultCode    = eSIR_SME_INVALID_PARAMETERS;

        goto end;
    }

    //                                         
    pStaDs->mlmStaContext.disassocReason = (tSirMacReasonCodes)
                                           pMlmDeauthReq->reasonCode;
    pStaDs->mlmStaContext.cleanupTrigger = pMlmDeauthReq->deauthTrigger;

    pMac->lim.limDisassocDeauthCnfReq.pMlmDeauthReq = pMlmDeauthReq;

    /*                                                        
                                                             
                                                        
                                                             
                                                                
                                                                     
    */
    pStaDs->mlmStaContext.mlmState   = eLIM_MLM_WT_DEL_STA_RSP_STATE;

    //                                            
    limSendDeauthMgmtFrame(pMac, pMlmDeauthReq->reasonCode,
                           pMlmDeauthReq->peerMacAddr,
                           psessionEntry, TRUE);

    return;

end:
    vos_mem_copy((tANI_U8 *) &mlmDeauthCnf.peerMacAddr,
                 (tANI_U8 *) pMlmDeauthReq->peerMacAddr,
                 sizeof(tSirMacAddr));
    mlmDeauthCnf.deauthTrigger = pMlmDeauthReq->deauthTrigger;
    mlmDeauthCnf.aid           = pMlmDeauthReq->aid;
    mlmDeauthCnf.sessionId = pMlmDeauthReq->sessionId;

    //                         
    //                 
    vos_mem_free(pMlmDeauthReq);

    limPostSmeMessage(pMac,
                      LIM_MLM_DEAUTH_CNF,
                      (tANI_U32 *) &mlmDeauthCnf);

}


void limProcessDeauthAckTimeout(tpAniSirGlobal pMac)
{
    limSendDeauthCnf(pMac);
}

/* 
                           
  
           
                                                            
           
  
        
  
              
  
       
  
                                                    
                                                        
               
 */

static void
limProcessMlmDeauthReq(tpAniSirGlobal pMac, tANI_U32 *pMsgBuf)
{
//                                
//                                         
//                                   
//                                       
    tLimMlmDeauthReq        *pMlmDeauthReq;
//                                         
    tpPESession             psessionEntry;

    if(pMsgBuf == NULL)
    {
        PELOGE(limLog(pMac, LOGE,FL("Buffer is Pointing to NULL"));)
        return;
    }

    pMlmDeauthReq = (tLimMlmDeauthReq *) pMsgBuf;

    if((psessionEntry = peFindSessionBySessionId(pMac,pMlmDeauthReq->sessionId))== NULL)
    {
    
        PELOGE(limLog(pMac, LOGE, FL("session does not exist for given sessionId"));)
        return;
    }

    limProcessMlmDeauthReqNtf( pMac, eHAL_STATUS_SUCCESS, (tANI_U32*) pMsgBuf );

} /*                              */



/* 
                                        
  
                                                                      
           
  
        
  
              
  
       
  
                                                    
                                                        
               
 */

static void
limProcessMlmSetKeysReq(tpAniSirGlobal pMac, tANI_U32 *pMsgBuf)
{
tANI_U16           aid;
tANI_U16           staIdx = 0;
tANI_U32           defaultKeyId = 0;
tSirMacAddr        currentBssId;
tpDphHashNode      pStaDs;
tLimMlmSetKeysReq  *pMlmSetKeysReq;
tLimMlmSetKeysCnf  mlmSetKeysCnf;
tpPESession        psessionEntry;

  if(pMsgBuf == NULL)
  {
         PELOGE(limLog(pMac, LOGE,FL("Buffer is Pointing to NULL"));)
         return;
  }


  pMlmSetKeysReq = (tLimMlmSetKeysReq *) pMsgBuf;
  //                                         
  pMac->lim.gpLimMlmSetKeysReq = (void *) pMlmSetKeysReq;

  if((psessionEntry = peFindSessionBySessionId(pMac,pMlmSetKeysReq->sessionId))== NULL)
  {
    PELOGE(limLog(pMac, LOGE,FL("session does not exist for given sessionId"));)
    return;
  }

  limLog( pMac, LOGW,
      FL( "Received MLM_SETKEYS_REQ with parameters:"
        "AID [%d], ED Type [%d], # Keys [%d] & Peer MAC Addr - "),
      pMlmSetKeysReq->aid,
      pMlmSetKeysReq->edType,
      pMlmSetKeysReq->numKeys );
  limPrintMacAddr( pMac, pMlmSetKeysReq->peerMacAddr, LOGW );

    #if 0
    if( eSIR_SUCCESS != wlan_cfgGetStr( pMac, WNI_CFG_BSSID, currentBssId, &cfg )) {
    limLog( pMac, LOGP, FL("Could not retrieve BSSID"));
        return;
    }
    #endif //                 
    sirCopyMacAddr(currentBssId,psessionEntry->bssId);

    switch( psessionEntry->limSystemRole ) {
    case eLIM_STA_ROLE:
    case eLIM_BT_AMP_STA_ROLE:
      //                                                                   
      //                   
#ifndef FEATURE_WLAN_TDLS
        if((!limIsAddrBC( pMlmSetKeysReq->peerMacAddr ) ) &&
          (!vos_mem_compare(pMlmSetKeysReq->peerMacAddr,
                            currentBssId, sizeof(tSirMacAddr))) ){
            limLog( pMac, LOGW, FL("Received MLM_SETKEYS_REQ with invalid BSSID"));
        limPrintMacAddr( pMac, pMlmSetKeysReq->peerMacAddr, LOGW );

        //                                                     
        mlmSetKeysCnf.resultCode = eSIR_SME_INVALID_PARAMETERS;
        goto end;
      }
#endif
      //                                
      break;
    case eLIM_STA_IN_IBSS_ROLE:
    default: //       
      //             
      break;
  }

    /* 
                                                                    
                      
                                                              
                                                                     
      */
    if( limIsAddrBC( pMlmSetKeysReq->peerMacAddr )) {
        limLog( pMac, LOG1, FL("Trying to set Group Keys...%d "), pMlmSetKeysReq->sessionId);
        /*                                       
                                                 
                                                
          */
        switch( pMlmSetKeysReq->edType ) {
      case eSIR_ED_CCMP:
         
#ifdef WLAN_FEATURE_11W
      case eSIR_ED_AES_128_CMAC:
#endif
        staIdx = psessionEntry->staId;
        break;

      default:
        break;
    }
    }else {
        limLog( pMac, LOG1, FL("Trying to set Unicast Keys..."));
    /* 
                                              
                                                 
     */


    pStaDs = dphLookupHashEntry( pMac, pMlmSetKeysReq->peerMacAddr, &aid , &psessionEntry->dph.dphHashTable);

    if ((pStaDs == NULL) ||
           ((pStaDs->mlmStaContext.mlmState != eLIM_MLM_LINK_ESTABLISHED_STATE) && (psessionEntry->limSystemRole != eLIM_AP_ROLE))) {
        /* 
                                               
                                                
                                    
         */
            limLog( pMac, LOG1,
            FL("Received MLM_SETKEYS_REQ for STA that either has no context or in some transit state, Addr = "));
        limPrintMacAddr( pMac, pMlmSetKeysReq->peerMacAddr, LOGW );

        //                                     
        mlmSetKeysCnf.resultCode = eSIR_SME_INVALID_PARAMETERS;
        goto end;
        } else
      staIdx = pStaDs->staIndex;
  }

    if ((pMlmSetKeysReq->numKeys == 0) && (pMlmSetKeysReq->edType != eSIR_ED_NONE)) {
        //
        //                                                  
        //                     
        //
        //                                                   
        //                                               
        defaultKeyId = 0xff;
    }else if(pMlmSetKeysReq->key[0].keyId && 
             ((pMlmSetKeysReq->edType == eSIR_ED_WEP40) || 
              (pMlmSetKeysReq->edType == eSIR_ED_WEP104))){
        /*                                                       
                                                                          
                                                                          
                                      */
        defaultKeyId = pMlmSetKeysReq->key[0].keyId;
    }else
        defaultKeyId = 0;

    limLog( pMac, LOG1,
      FL( "Trying to set keys for STA Index [%d], using defaultKeyId [%d]" ),
      staIdx,
      defaultKeyId );

    if(limIsAddrBC( pMlmSetKeysReq->peerMacAddr )) {
  psessionEntry->limPrevMlmState = psessionEntry->limMlmState;
  psessionEntry->limMlmState = eLIM_MLM_WT_SET_BSS_KEY_STATE;
  MTRACE(macTrace(pMac, TRACE_CODE_MLM_STATE, psessionEntry->peSessionId, psessionEntry->limMlmState));
        limLog( pMac, LOG1, FL("Trying to set Group Keys...%d "),
            psessionEntry->peSessionId);

    //                                              
        limSendSetBssKeyReq(pMac, pMlmSetKeysReq,psessionEntry);
    return;
    }else {
    //                                                                         
        limSendSetStaKeyReq(pMac, pMlmSetKeysReq, staIdx, (tANI_U8) defaultKeyId,psessionEntry);
    return;
  }

end:
    mlmSetKeysCnf.sessionId= pMlmSetKeysReq->sessionId;
    limPostSmeSetKeysCnf( pMac, pMlmSetKeysReq, &mlmSetKeysCnf );

} /*                               */

/* 
                              
  
           
                                                               
           
  
        
  
              
  
       
  
                                                    
                                                        
               
 */

static void
limProcessMlmRemoveKeyReq(tpAniSirGlobal pMac, tANI_U32 *pMsgBuf)
{
tANI_U16           aid;
tANI_U16           staIdx = 0;
tSirMacAddr        currentBssId;
tpDphHashNode      pStaDs;
tLimMlmRemoveKeyReq  *pMlmRemoveKeyReq;
tLimMlmRemoveKeyCnf  mlmRemoveKeyCnf;
    tpPESession         psessionEntry;

    if(pMsgBuf == NULL)
    {
           PELOGE(limLog(pMac, LOGE,FL("Buffer is Pointing to NULL"));)
           return;
    }

    pMlmRemoveKeyReq = (tLimMlmRemoveKeyReq *) pMsgBuf;


    if((psessionEntry = peFindSessionBySessionId(pMac,pMlmRemoveKeyReq->sessionId))== NULL)
    {
        PELOGE(limLog(pMac, LOGE,
                    FL("session does not exist for given sessionId"));)
        vos_mem_free(pMsgBuf);
        return;
    }


    if( pMac->lim.gpLimMlmRemoveKeyReq != NULL )
    {
        //                            
        vos_mem_free(pMac->lim.gpLimMlmRemoveKeyReq);
        pMac->lim.gpLimMlmRemoveKeyReq = NULL;
    }
    //                                            
    pMac->lim.gpLimMlmRemoveKeyReq = (void *) pMlmRemoveKeyReq; 

    #if 0
    if( eSIR_SUCCESS != wlan_cfgGetStr( pMac,
        WNI_CFG_BSSID,
        currentBssId,
        &cfg ))
    limLog( pMac, LOGP, FL("Could not retrieve BSSID"));
    #endif //                 
    sirCopyMacAddr(currentBssId,psessionEntry->bssId);

    switch( psessionEntry->limSystemRole )
    {
        case eLIM_STA_ROLE:
        case eLIM_BT_AMP_STA_ROLE:
        if (( limIsAddrBC( pMlmRemoveKeyReq->peerMacAddr ) != true ) &&
           (!vos_mem_compare(pMlmRemoveKeyReq->peerMacAddr,
                            currentBssId,
                            sizeof(tSirMacAddr))))
        {
            limLog( pMac, LOGW,
            FL("Received MLM_REMOVEKEY_REQ with invalid BSSID"));
            limPrintMacAddr( pMac, pMlmRemoveKeyReq->peerMacAddr, LOGW );

            //                                                       
            mlmRemoveKeyCnf.resultCode = eSIR_SME_INVALID_PARAMETERS;
            goto end;
        }
        break;

        case eLIM_STA_IN_IBSS_ROLE:
        default: //             
                 //             
                break;
    }


    psessionEntry->limPrevMlmState = psessionEntry->limMlmState;
    if(limIsAddrBC( pMlmRemoveKeyReq->peerMacAddr )) //                                     
    {
        psessionEntry->limMlmState = eLIM_MLM_WT_REMOVE_BSS_KEY_STATE;
        MTRACE(macTrace(pMac, TRACE_CODE_MLM_STATE, psessionEntry->peSessionId, psessionEntry->limMlmState));    
        //                                                 
        limSendRemoveBssKeyReq( pMac,pMlmRemoveKeyReq,psessionEntry);
        return;
    }

  /* 
                                             
                                                    
    */
  pStaDs = dphLookupHashEntry( pMac, pMlmRemoveKeyReq->peerMacAddr, &aid, &psessionEntry->dph.dphHashTable );
  if ((pStaDs == NULL) ||
         (pStaDs &&
         (pStaDs->mlmStaContext.mlmState !=
                       eLIM_MLM_LINK_ESTABLISHED_STATE)))
  {
     /* 
                                               
                                              
                                  
       */
      limLog( pMac, LOGW,
          FL("Received MLM_REMOVEKEYS_REQ for STA that either has no context or in some transit state, Addr = "));
      limPrintMacAddr( pMac, pMlmRemoveKeyReq->peerMacAddr, LOGW );

      //                                       
      mlmRemoveKeyCnf.resultCode = eSIR_SME_INVALID_PARAMETERS;
      mlmRemoveKeyCnf.sessionId = pMlmRemoveKeyReq->sessionId;
      

      goto end;
  }
  else
    staIdx = pStaDs->staIndex;
  


    psessionEntry->limMlmState = eLIM_MLM_WT_REMOVE_STA_KEY_STATE;
    MTRACE(macTrace(pMac, TRACE_CODE_MLM_STATE, psessionEntry->peSessionId, psessionEntry->limMlmState));

    //                                                 
    limSendRemoveStaKeyReq( pMac,pMlmRemoveKeyReq,staIdx,psessionEntry);
    return;
 
end:
    limPostSmeRemoveKeyCnf( pMac,
      psessionEntry,
      pMlmRemoveKeyReq,
      &mlmRemoveKeyCnf );

} /*                                 */


/* 
                                
  
           
                                                         
                       
  
        
  
              
  
       
  
                                                    
               
 */

static void
limProcessMinChannelTimeout(tpAniSirGlobal pMac)
{
    tANI_U8 channelNum;
    
#ifdef GEN6_TODO
    //                                                                             
    //                                  
    
    //                                       
    tpPESession psessionEntry;

    if((psessionEntry = peFindSessionBySessionId(pMac, pMac->lim.limTimers.gLimMinChannelTimer.sessionId))== NULL) 
    {
        limLog(pMac, LOGP,FL("Session Does not exist for given sessionID"));
        return;
    }
#endif

    /*                                                       
                               */
    if (pMac->lim.gLimMlmState == eLIM_MLM_WT_PROBE_RESP_STATE &&
        pMac->lim.gLimHalScanState != eLIM_HAL_FINISH_SCAN_WAIT_STATE)
    {
        PELOG1(limLog(pMac, LOG1, FL("Scanning : min channel timeout occurred"));)

        //                             
        pMac->lim.limTimers.gLimPeriodicProbeReqTimer.sessionId = 0xff;
        limDeactivateAndChangeTimer(pMac, eLIM_MIN_CHANNEL_TIMER);
        limDeactivateAndChangeTimer(pMac, eLIM_PERIODIC_PROBE_REQ_TIMER);
        if (pMac->lim.gLimCurrentScanChannelId <=
                (tANI_U32)(pMac->lim.gpLimMlmScanReq->channelList.numChannels - 1))
        {
            channelNum = (tANI_U8)limGetCurrentScanChannel(pMac);
        }
        else
        {
            //                                                                                                   
            //                                                    
            if(pMac->lim.gpLimMlmScanReq->channelList.channelNumber)
            {
                channelNum = pMac->lim.gpLimMlmScanReq->channelList.channelNumber[pMac->lim.gpLimMlmScanReq->channelList.numChannels - 1];
            }
            else
            {
               channelNum = 1;
            }
        }

        limLog(pMac, LOGW,
           FL("Sending End Scan req from MIN_CH_TIMEOUT in state %X ch-%d"),
           pMac->lim.gLimMlmState,channelNum);
        limSendHalEndScanReq(pMac, channelNum, eLIM_HAL_END_SCAN_WAIT_STATE);
    }
    else
    {
    /* 
                                                      
                                                    
                     
         */
        limLog(pMac, LOGW,
           FL("received unexpected MIN channel timeout in mlme state %X and hal scan State %X"),
           pMac->lim.gLimMlmState,pMac->lim.gLimHalScanState);
        limPrintMlmState(pMac, LOGE, pMac->lim.gLimMlmState);
    }
} /*                                   */



/* 
                                
  
           
                                                         
                       
  
        
  
              
  
       
  
                                                    
               
 */

static void
limProcessMaxChannelTimeout(tpAniSirGlobal pMac)
{
    tANI_U8 channelNum;

    /*                                                       
                                */
    if ((pMac->lim.gLimMlmState == eLIM_MLM_WT_PROBE_RESP_STATE ||
        pMac->lim.gLimMlmState == eLIM_MLM_PASSIVE_SCAN_STATE) &&
        pMac->lim.gLimHalScanState != eLIM_HAL_FINISH_SCAN_WAIT_STATE)
    {
        PELOG1(limLog(pMac, LOG1, FL("Scanning : Max channel timed out"));)
        /* 
                                      
                                 
         */
        limDeactivateAndChangeTimer(pMac, eLIM_MAX_CHANNEL_TIMER);
        limDeactivateAndChangeTimer(pMac, eLIM_PERIODIC_PROBE_REQ_TIMER);
        pMac->lim.limTimers.gLimPeriodicProbeReqTimer.sessionId = 0xff;
        if (pMac->lim.gLimCurrentScanChannelId <=
                (tANI_U32)(pMac->lim.gpLimMlmScanReq->channelList.numChannels - 1))
        {
        channelNum = limGetCurrentScanChannel(pMac);
        }
        else
        {
            if(pMac->lim.gpLimMlmScanReq->channelList.channelNumber)
            {
                channelNum = pMac->lim.gpLimMlmScanReq->channelList.channelNumber[pMac->lim.gpLimMlmScanReq->channelList.numChannels - 1];
            }
            else
            {
               channelNum = 1;
            }
        }
        limLog(pMac, LOGW,
           FL("Sending End Scan req from MAX_CH_TIMEOUT in state %X on ch-%d"),
           pMac->lim.gLimMlmState,channelNum);
        limSendHalEndScanReq(pMac, channelNum, eLIM_HAL_END_SCAN_WAIT_STATE);
    }
    else
    {
        /* 
                                                      
                                          
                     
         */
        limLog(pMac, LOGW,
           FL("received unexpected MAX channel timeout in mlme state %X and hal scan state %X"),
           pMac->lim.gLimMlmState, pMac->lim.gLimHalScanState);
        limPrintMlmState(pMac, LOGW, pMac->lim.gLimMlmState);
    }
} /*                                   */

/* 
                                    
  
           
                                                            
                        
  
        
  
              
  
       
  
                                                    
               
 */

static void
limProcessPeriodicProbeReqTimer(tpAniSirGlobal pMac)
{
    tANI_U8 channelNum;
    tANI_U8 i = 0;
    tSirRetStatus status = eSIR_SUCCESS;
    TX_TIMER *pPeriodicProbeReqTimer;
    pPeriodicProbeReqTimer = &pMac->lim.limTimers.gLimPeriodicProbeReqTimer;

    if(vos_timer_getCurrentState(&pPeriodicProbeReqTimer->vosTimer)
         != VOS_TIMER_STATE_STOPPED)
    {
       PELOG1(limLog(pMac, LOG1, FL("Invalid state of timer"));)
       return;
    }

    if ((pMac->lim.gLimMlmState == eLIM_MLM_WT_PROBE_RESP_STATE) &&
        (pPeriodicProbeReqTimer->sessionId != 0xff))
    {
        tLimMlmScanReq *pLimMlmScanReq = pMac->lim.gpLimMlmScanReq;
        PELOG1(limLog(pMac, LOG1, FL("Scanning : Periodic scanning"));)
        /* 
                                           
                                 
         */
        channelNum = limGetCurrentScanChannel(pMac);
        do
        {
            /*                                                       
                                       
             */
             
            /*
                                                                         
                                                                         
                                                                           
                                                                           
             */
            status = limSendProbeReqMgmtFrame( pMac, &pLimMlmScanReq->ssId[i],
                     pLimMlmScanReq->bssId, channelNum, pMac->lim.gSelfMacAddr,
                     pLimMlmScanReq->dot11mode, pLimMlmScanReq->uIEFieldLen,
               (tANI_U8 *)(pLimMlmScanReq) + pLimMlmScanReq->uIEFieldOffset);


            if ( status != eSIR_SUCCESS)
            {
                PELOGE(limLog(pMac, LOGE, FL("send ProbeReq failed for SSID "
                                             "%s on channel: %d"),
                                              pLimMlmScanReq->ssId[i].ssId,
                                              channelNum);)
                return;
            }
            i++;
        } while (i < pLimMlmScanReq->numSsid);

        /*                      */
        MTRACE(macTrace(pMac, TRACE_CODE_TIMER_ACTIVATE, pPeriodicProbeReqTimer->sessionId, eLIM_PERIODIC_PROBE_REQ_TIMER));
        if (tx_timer_activate(pPeriodicProbeReqTimer) != TX_SUCCESS)
        {
             limLog(pMac, LOGP, FL("could not start periodic probe"
                                                   " req timer"));
             return;
        }
    }
    else
    {
        /* 
                                                   
                                          
                     
         */
        limLog(pMac, LOG1,
           FL("received unexpected Periodic scan timeout in state %X"),
           pMac->lim.gLimMlmState);
    }
} /*                                       */

/* 
                                 
  
           
                                                        
  
        
  
              
  
       
  
                                                    
               
 */

static void
limProcessJoinFailureTimeout(tpAniSirGlobal pMac)
{
    tLimMlmJoinCnf  mlmJoinCnf;
    tSirMacAddr bssid;
    tANI_U32 len;
#ifdef FEATURE_WLAN_DIAG_SUPPORT_LIM //                             
    vos_log_rssi_pkt_type *pRssiLog = NULL;
#endif //                             
    
    //                                             
    tpPESession psessionEntry;

    if((psessionEntry = peFindSessionBySessionId(pMac, pMac->lim.limTimers.gLimJoinFailureTimer.sessionId))== NULL) 
    {
        limLog(pMac, LOGE, FL("Session Does not exist for given sessionID"));
        return;
    }

#ifdef FEATURE_WLAN_DIAG_SUPPORT_LIM //                             
    WLAN_VOS_DIAG_LOG_ALLOC(pRssiLog,
                            vos_log_rssi_pkt_type, LOG_WLAN_RSSI_UPDATE_C);
    if (pRssiLog)
    {
        pRssiLog->rssi = psessionEntry->rssi;
    }
    WLAN_VOS_DIAG_LOG_REPORT(pRssiLog);
#endif //                             

    if (psessionEntry->limMlmState == eLIM_MLM_WT_JOIN_BEACON_STATE)
    {
        len = sizeof(tSirMacAddr);

        if (wlan_cfgGetStr(pMac, WNI_CFG_BSSID, bssid, &len) !=
                            eSIR_SUCCESS)
        {
            //                                          
            limLog(pMac, LOGP, FL("could not retrieve BSSID"));
            return;
        }

        //                                      
        limDeactivateAndChangeTimer(pMac, eLIM_JOIN_FAIL_TIMER);
        //                                                      
        limDeactivateAndChangeTimer(pMac, eLIM_PERIODIC_JOIN_PROBE_REQ_TIMER);
        /* 
                                                          
         */
        PELOGE(limLog(pMac, LOGE,  FL(" In state eLIM_MLM_WT_JOIN_BEACON_STATE."));)
        PELOGE(limLog(pMac, LOGE,  FL(" Join Failure Timeout occurred for session %d with BSS "),
                                        psessionEntry->peSessionId);
                                        limPrintMacAddr(pMac, psessionEntry->bssId, LOGE);)

        mlmJoinCnf.resultCode = eSIR_SME_JOIN_TIMEOUT_RESULT_CODE;
        mlmJoinCnf.protStatusCode = eSIR_MAC_UNSPEC_FAILURE_STATUS;

        psessionEntry->limMlmState = eLIM_MLM_IDLE_STATE;
        MTRACE(macTrace(pMac, TRACE_CODE_MLM_STATE, psessionEntry->peSessionId, psessionEntry->limMlmState));
        if(limSetLinkState(pMac, eSIR_LINK_IDLE_STATE, psessionEntry->bssId, 
            psessionEntry->selfMacAddr, NULL, NULL) != eSIR_SUCCESS)
            PELOGE(limLog(pMac, LOGE,  FL("Failed to set the LinkState"));)
        /*                      */
        mlmJoinCnf.sessionId = psessionEntry->peSessionId;
        
       
        //                                        
        if (psessionEntry->pLimMlmJoinReq)
        {
            vos_mem_free(psessionEntry->pLimMlmJoinReq);
            psessionEntry->pLimMlmJoinReq = NULL;
        }
        
        limPostSmeMessage(pMac,
                          LIM_MLM_JOIN_CNF,
                          (tANI_U32 *) &mlmJoinCnf);

        return;
    }
    else
    {
        /* 
                                                       
                                                       
                     
         */
        limLog(pMac, LOGW,
           FL("received unexpected JOIN failure timeout in state %X"),psessionEntry->limMlmState);
        limPrintMlmState(pMac, LOGW, psessionEntry->limMlmState);
    }
} /*                                    */


/* 
                                        
  
           
                                                            
                                
  
        
  
              
  
       
  
                                                    
               
 */

static void limProcessPeriodicJoinProbeReqTimer(tpAniSirGlobal pMac)
{
    tpPESession  psessionEntry;
    tSirMacSSid  ssId;

    if((psessionEntry = peFindSessionBySessionId(pMac, pMac->lim.limTimers.gLimPeriodicJoinProbeReqTimer.sessionId))== NULL)
    {
        limLog(pMac, LOGE,FL("session does not exist for given SessionId : %d"),
                              pMac->lim.limTimers.gLimPeriodicJoinProbeReqTimer.sessionId);
        return;
    }

    if((VOS_TRUE == tx_timer_running(&pMac->lim.limTimers.gLimJoinFailureTimer)) && 
                          (psessionEntry->limMlmState == eLIM_MLM_WT_JOIN_BEACON_STATE))
    {
        vos_mem_copy(ssId.ssId,
                     psessionEntry->ssId.ssId,
                     psessionEntry->ssId.length);
        ssId.length = psessionEntry->ssId.length;

        limSendProbeReqMgmtFrame( pMac, &ssId,
           psessionEntry->pLimMlmJoinReq->bssDescription.bssId, psessionEntry->currentOperChannel/*       */,
           psessionEntry->selfMacAddr, psessionEntry->dot11mode,
           psessionEntry->pLimJoinReq->addIEScan.length, psessionEntry->pLimJoinReq->addIEScan.addIEdata);

        limDeactivateAndChangeTimer(pMac, eLIM_PERIODIC_JOIN_PROBE_REQ_TIMER);

        //                                       
        if (tx_timer_activate(&pMac->lim.limTimers.gLimPeriodicJoinProbeReqTimer) != TX_SUCCESS)
        {
            limLog(pMac, LOGP, FL("could not activate Periodic Join req failure timer"));
            return;
        }
    }
    return;
} /*                                           */


/* 
                                 
  
           
                                                         
                       
  
        
  
              
  
       
  
                                                    
               
 */

static void
limProcessAuthFailureTimeout(tpAniSirGlobal pMac)
{
    //                                             
    tpPESession psessionEntry;
#ifdef FEATURE_WLAN_DIAG_SUPPORT_LIM //                             
    vos_log_rssi_pkt_type *pRssiLog = NULL;
#endif //                             

    if((psessionEntry = peFindSessionBySessionId(pMac, pMac->lim.limTimers.gLimAuthFailureTimer.sessionId))== NULL) 
    {
        limLog(pMac, LOGP,FL("Session Does not exist for given sessionID"));
        return;
    }
#ifdef FEATURE_WLAN_DIAG_SUPPORT_LIM //                             
    WLAN_VOS_DIAG_LOG_ALLOC(pRssiLog,
                            vos_log_rssi_pkt_type, LOG_WLAN_RSSI_UPDATE_C);
    if (pRssiLog)
    {
        pRssiLog->rssi = psessionEntry->rssi;
    }
    WLAN_VOS_DIAG_LOG_REPORT(pRssiLog);
#endif //                             

    switch (psessionEntry->limMlmState)
    {
        case eLIM_MLM_WT_AUTH_FRAME2_STATE:
        case eLIM_MLM_WT_AUTH_FRAME4_STATE:
            /* 
                                                             
                                           
                                                              
             */
             //                               
             if (VOS_P2P_CLIENT_MODE == psessionEntry->pePersona && psessionEntry->defaultAuthFailureTimeout)
             {
                 ccmCfgSetInt(pMac,WNI_CFG_AUTHENTICATE_FAILURE_TIMEOUT ,
                                       psessionEntry->defaultAuthFailureTimeout, NULL, eANI_BOOLEAN_FALSE);
             }
            limRestoreFromAuthState(pMac,eSIR_SME_AUTH_TIMEOUT_RESULT_CODE,eSIR_MAC_UNSPEC_FAILURE_REASON,psessionEntry);
            break;

        default:
            /* 
                                                           
                                                    
                         
             */
            PELOGE(limLog(pMac, LOGE, FL("received unexpected AUTH failure timeout in state %X"), psessionEntry->limMlmState);)
            limPrintMlmState(pMac, LOGE, psessionEntry->limMlmState);

            break;
    }
} /*                                    */



/* 
                             
  
           
                                                         
                       
  
        
  
              
  
       
  
                                                    
               
 */

static void
limProcessAuthRspTimeout(tpAniSirGlobal pMac, tANI_U32 authIndex)
{
    struct tLimPreAuthNode *pAuthNode;
    tpPESession        psessionEntry;
    tANI_U8            sessionId;

    pAuthNode = limGetPreAuthNodeFromIndex(pMac, &pMac->lim.gLimPreAuthTimerTable, authIndex);

    if (NULL == pAuthNode)
    {
        limLog(pMac, LOGW, FL("Invalid auth node"));
        return;
    } 

    if ((psessionEntry = peFindSessionByBssid(pMac, pAuthNode->peerMacAddr, &sessionId)) == NULL)
    {
        limLog(pMac, LOGW, FL("session does not exist for given BSSID "));
        return;
    } 

    if (psessionEntry->limSystemRole == eLIM_AP_ROLE ||
        psessionEntry->limSystemRole == eLIM_STA_IN_IBSS_ROLE)
    {
        if (pAuthNode->mlmState != eLIM_MLM_WT_AUTH_FRAME3_STATE)
        {
            /* 
                                                     
                                             
             */
            PELOGE(limLog(pMac, LOGE,
                        FL("received unexpected AUTH rsp timeout for MAC address "));
            limPrintMacAddr(pMac, pAuthNode->peerMacAddr, LOGE);)
        }
        else
        {
            //                              
            //                     
            pAuthNode->mlmState = eLIM_MLM_AUTH_RSP_TIMEOUT_STATE;
            pAuthNode->fTimerStarted = 0;
            PELOG1( limLog(pMac, LOG1,
                        FL("AUTH rsp timedout for MAC address "));
            limPrintMacAddr(pMac, pAuthNode->peerMacAddr, LOG1);)

            //                                        
            limDeactivateAndChangePerStaIdTimer(pMac,
                        eLIM_AUTH_RSP_TIMER,
                        pAuthNode->authNodeIdx);

            limDeletePreAuthNode(pMac, pAuthNode->peerMacAddr);
        }
    }
} /*                                */


/* 
                                  
  
           
                                                         
                       
  
        
  
              
  
       
  
                                                    
               
 */

static void
limProcessAssocFailureTimeout(tpAniSirGlobal pMac, tANI_U32 MsgType)
{

    tLimMlmAssocCnf     mlmAssocCnf;
    tpPESession         psessionEntry;
#ifdef FEATURE_WLAN_DIAG_SUPPORT_LIM //                             
    vos_log_rssi_pkt_type *pRssiLog = NULL;
#endif //                             
    
    //                                                                             
    tANI_U8 sessionId;
    
    if(MsgType == LIM_ASSOC)
    {
        sessionId = pMac->lim.limTimers.gLimAssocFailureTimer.sessionId;
    }
    else
    {
        sessionId = pMac->lim.limTimers.gLimReassocFailureTimer.sessionId;
    }
    
    if((psessionEntry = peFindSessionBySessionId(pMac, sessionId))== NULL) 
    {
        limLog(pMac, LOGP,FL("Session Does not exist for given sessionID"));
        return;
    }
#ifdef FEATURE_WLAN_DIAG_SUPPORT_LIM //                             
    WLAN_VOS_DIAG_LOG_ALLOC(pRssiLog,
                            vos_log_rssi_pkt_type, LOG_WLAN_RSSI_UPDATE_C);
    if (pRssiLog)
    {
        pRssiLog->rssi = psessionEntry->rssi;
    }
    WLAN_VOS_DIAG_LOG_REPORT(pRssiLog);
#endif //                             

    /* 
                                             
                                                          
     */




    /*                                                                  */
    /*                                                                             */
    WLANTL_AssocFailed (psessionEntry->staId);

    //          
    PELOG1(limLog(pMac, LOG1,
       FL("Re/Association Response not received before timeout "));)

    if (( (psessionEntry->limSystemRole == eLIM_AP_ROLE) || (psessionEntry->limSystemRole == eLIM_BT_AMP_AP_ROLE) )||
        ( (psessionEntry->limMlmState != eLIM_MLM_WT_ASSOC_RSP_STATE) &&
          (psessionEntry->limMlmState != eLIM_MLM_WT_REASSOC_RSP_STATE)  && 
          (psessionEntry->limMlmState != eLIM_MLM_WT_FT_REASSOC_RSP_STATE)))
    {
        /* 
                                                                
                                                         
         */

        //          
        limLog(pMac, LOGW,
           FL("received unexpected REASSOC failure timeout in state %X for role %d"),
           psessionEntry->limMlmState, psessionEntry->limSystemRole);
        limPrintMlmState(pMac, LOGW, psessionEntry->limMlmState);
    }
    else
    {

        if ((MsgType == LIM_ASSOC) || 
            ((MsgType == LIM_REASSOC) && (psessionEntry->limMlmState == eLIM_MLM_WT_FT_REASSOC_RSP_STATE)))
        {
            PELOGE(limLog(pMac, LOGE,  FL("(Re)Assoc Failure Timeout occurred."));)

            psessionEntry->limMlmState = eLIM_MLM_IDLE_STATE;
            MTRACE(macTrace(pMac, TRACE_CODE_MLM_STATE, psessionEntry->peSessionId, psessionEntry->limMlmState));

            //                                      
            limDeactivateAndChangeTimer(pMac, eLIM_ASSOC_FAIL_TIMER);

            //                                             
            //                  
            if (psessionEntry->pLimMlmJoinReq)
            {
                vos_mem_free(psessionEntry->pLimMlmJoinReq);
                psessionEntry->pLimMlmJoinReq = NULL;
            }

            //                                                       
            if (limSearchPreAuthList(pMac, psessionEntry->bssId))
            {
                PELOG1(limLog(pMac, LOG1, FL(" delete pre auth node for "
                       MAC_ADDRESS_STR), MAC_ADDR_ARRAY(psessionEntry->bssId));)
                limDeletePreAuthNode(pMac, psessionEntry->bssId);
            }

            mlmAssocCnf.resultCode =
                            eSIR_SME_ASSOC_TIMEOUT_RESULT_CODE;
            mlmAssocCnf.protStatusCode = 
                            eSIR_MAC_UNSPEC_FAILURE_STATUS;
            
            /*                     */
            mlmAssocCnf.sessionId = psessionEntry->peSessionId;
            if (MsgType == LIM_ASSOC)
                limPostSmeMessage(pMac, LIM_MLM_ASSOC_CNF, (tANI_U32 *) &mlmAssocCnf);
            else 
            {
                /*                                                             
                                                                       */
                mlmAssocCnf.resultCode = eSIR_SME_FT_REASSOC_TIMEOUT_FAILURE;
                limPostSmeMessage(pMac, LIM_MLM_REASSOC_CNF, (tANI_U32 *) &mlmAssocCnf);
        }
        }
        else
        {
            /* 
                                             
                                                            
             */
            psessionEntry->limMlmState = psessionEntry->limPrevMlmState;
            MTRACE(macTrace(pMac, TRACE_CODE_MLM_STATE, psessionEntry->peSessionId, psessionEntry->limMlmState));

            limRestorePreReassocState(pMac, 
                eSIR_SME_REASSOC_TIMEOUT_RESULT_CODE, eSIR_MAC_UNSPEC_FAILURE_STATUS,psessionEntry);
        }
    }
} /*                                     */



/* 
                       
  
           
                                                       
                        
  
        
  
              
  
       
  
                                                    
                                           
               
 */

void
limCompleteMlmScan(tpAniSirGlobal pMac, tSirResultCodes retCode)
{
    tLimMlmScanCnf    mlmScanCnf;

    //                            
    pMac->lim.gLimMlmState = pMac->lim.gLimPrevMlmState;
    MTRACE(macTrace(pMac, TRACE_CODE_MLM_STATE, NO_SESSION, pMac->lim.gLimMlmState));
    limRestorePreScanState(pMac);

    //                                 
    if( NULL != pMac->lim.gpLimMlmScanReq )
    {
        vos_mem_free(pMac->lim.gpLimMlmScanReq);
        pMac->lim.gpLimMlmScanReq = NULL;
    }

    mlmScanCnf.resultCode       = retCode;
    mlmScanCnf.scanResultLength = pMac->lim.gLimMlmScanResultLength;

    limPostSmeMessage(pMac, LIM_MLM_SCAN_CNF, (tANI_U32 *) &mlmScanCnf);

} /*                          */

/* 
                                    
  
                            
  
                                               
  
                                                   
  
               
 */
void limProcessMlmAddBAReq( tpAniSirGlobal pMac,
    tANI_U32 *pMsgBuf )
{
tSirRetStatus status = eSIR_SUCCESS;
tpLimMlmAddBAReq pMlmAddBAReq;
tpLimMlmAddBACnf pMlmAddBACnf;
  tpPESession     psessionEntry;
    
  if(pMsgBuf == NULL)
  {
      PELOGE(limLog(pMac, LOGE,FL("Buffer is Pointing to NULL"));)
           return;
  }

  pMlmAddBAReq = (tpLimMlmAddBAReq) pMsgBuf;
  if((psessionEntry = peFindSessionBySessionId(pMac,pMlmAddBAReq->sessionId))== NULL)
  {
      PELOGE(limLog(pMac, LOGE,
               FL("session does not exist for given sessionId"));)
      vos_mem_free(pMsgBuf);
      return;
  }
  

  //                            
  status = limSendAddBAReq( pMac, pMlmAddBAReq,psessionEntry);

  //
  //                                                      
  //                                                
  //                                                     
  //                           
  //
  if( eSIR_SUCCESS != status )
  {
    //                               

    pMlmAddBACnf = vos_mem_malloc(sizeof( tLimMlmAddBACnf ));
    if ( NULL == pMlmAddBACnf )
    {
      limLog( pMac, LOGP,
          FL("AllocateMemory failed with error code %d"));
      vos_mem_free(pMsgBuf);
      return;
    }
    else
    {
        vos_mem_set((void *) pMlmAddBACnf, sizeof( tLimMlmAddBACnf ), 0);
        vos_mem_copy((void *) pMlmAddBACnf->peerMacAddr,
                     (void *) pMlmAddBAReq->peerMacAddr,
                     sizeof( tSirMacAddr ));

      pMlmAddBACnf->baDialogToken = pMlmAddBAReq->baDialogToken;
      pMlmAddBACnf->baTID = pMlmAddBAReq->baTID;
      pMlmAddBACnf->baPolicy = pMlmAddBAReq->baPolicy;
      pMlmAddBACnf->baBufferSize = pMlmAddBAReq->baBufferSize;
      pMlmAddBACnf->baTimeout = pMlmAddBAReq->baTimeout;
      pMlmAddBACnf->sessionId = pMlmAddBAReq->sessionId;

      //                       
      pMlmAddBACnf->addBAResultCode = eSIR_MAC_UNSPEC_FAILURE_STATUS;

      limPostSmeMessage( pMac,
          LIM_MLM_ADDBA_CNF,
          (tANI_U32 *) pMlmAddBACnf );
    }

    //                   
    psessionEntry->limMlmState = psessionEntry->limPrevMlmState;
    MTRACE(macTrace(pMac, TRACE_CODE_MLM_STATE, psessionEntry->peSessionId, psessionEntry->limMlmState));

  }

  //                                              
  vos_mem_free(pMsgBuf);

}

/* 
                                                   
                                   
  
                            
  
                                               
  
                                                   
  
               
 */
void limProcessMlmAddBARsp( tpAniSirGlobal pMac,
    tANI_U32 *pMsgBuf )
{
tpLimMlmAddBARsp pMlmAddBARsp;
   tANI_U16 aid;
   tpDphHashNode pSta;
   tpPESession  psessionEntry;

    
    if(pMsgBuf == NULL)
    {
           PELOGE(limLog(pMac, LOGE,FL("Buffer is Pointing to NULL"));)
           return;
    }

    pMlmAddBARsp = (tpLimMlmAddBARsp) pMsgBuf;

    if(( psessionEntry = peFindSessionBySessionId(pMac,pMlmAddBARsp->sessionId))== NULL)
    {
        PELOGE(limLog(pMac, LOGE,
                  FL("session does not exist for given session ID"));)
        vos_mem_free(pMsgBuf);
        return;
    }
  

  //                            
  if( eSIR_SUCCESS != limSendAddBARsp( pMac,pMlmAddBARsp,psessionEntry))
  {
    limLog( pMac, LOGE,
    FL("Failed to send ADDBA Rsp to peer "));
    limPrintMacAddr( pMac, pMlmAddBARsp->peerMacAddr, LOGE );
    /*                                                          */
    pSta = dphLookupHashEntry( pMac, pMlmAddBARsp->peerMacAddr, &aid, 
            &psessionEntry->dph.dphHashTable);
     if( NULL != pSta )
    {
        limPostMsgDelBAInd( pMac, pSta, pMlmAddBARsp->baTID, eBA_RECIPIENT, 
                psessionEntry);
    }
  }

  //                                             
  //                                       


  //                                              
  vos_mem_free(pMsgBuf);

}

/* 
                                    
  
                            
  
                                               
  
                                                   
  
               
 */
void limProcessMlmDelBAReq( tpAniSirGlobal pMac,
    tANI_U32 *pMsgBuf )
{
    tSirRetStatus status = eSIR_SUCCESS;
    tpLimMlmDelBAReq pMlmDelBAReq;
    tpLimMlmDelBACnf pMlmDelBACnf;
    tpPESession  psessionEntry;
  
    
    if(pMsgBuf == NULL)
    {
           PELOGE(limLog(pMac, LOGE,FL("Buffer is Pointing to NULL"));)
           return;
    }

  //                                   
    pMlmDelBAReq = (tpLimMlmDelBAReq) pMsgBuf;

    if((psessionEntry = peFindSessionBySessionId(pMac,pMlmDelBAReq->sessionId))== NULL)
    {
        PELOGE(limLog(pMac, LOGE,FL("session does not exist for given bssId"));)
        vos_mem_free(pMsgBuf);
        return;
    }

  //                            
  if( eSIR_SUCCESS !=
      (status = limSendDelBAInd( pMac, pMlmDelBAReq,psessionEntry)))
    status = eSIR_SME_HAL_SEND_MESSAGE_FAIL;
  else
  {
    tANI_U16 aid;
    tpDphHashNode pSta;

    //                                             
    //                                       
    pSta = dphLookupHashEntry( pMac, pMlmDelBAReq->peerMacAddr, &aid , &psessionEntry->dph.dphHashTable);
    if( NULL != pSta )
    {
        status = limPostMsgDelBAInd( pMac,
         pSta,
          pMlmDelBAReq->baTID,
          pMlmDelBAReq->baDirection,psessionEntry);

    }
  }

  //
  //                                                
  //                                          
  //

  //                               

  pMlmDelBACnf = vos_mem_malloc(sizeof( tLimMlmDelBACnf ));
  if ( NULL == pMlmDelBACnf )
  {
    limLog( pMac, LOGP, FL("AllocateMemory failed"));
    vos_mem_free(pMsgBuf);
    return;
  }
  else
  {
    vos_mem_set((void *) pMlmDelBACnf, sizeof( tLimMlmDelBACnf ), 0);

    vos_mem_copy((void *) pMlmDelBACnf,
                 (void *) pMlmDelBAReq,
                 sizeof( tLimMlmDelBAReq ));

    //                         
    pMlmDelBACnf->delBAReasonCode = pMlmDelBAReq->delBAReasonCode;
    
    /*                     */
    pMlmDelBACnf->sessionId = pMlmDelBAReq->sessionId;

    limPostSmeMessage( pMac,
        LIM_MLM_DELBA_CNF,
        (tANI_U32 *) pMlmDelBACnf );
  }

  //                                              
  vos_mem_free(pMsgBuf);

}

/* 
                                         
  
                                                                                                        
  
              
  
                    
              
  
             
              
  
                                                 
                                                                             
               
 */
 
tSirRetStatus
limSMPowerSaveStateInd(tpAniSirGlobal pMac, tSirMacHTMIMOPowerSaveState state)
{
#if 0
    tSirRetStatus           retStatus = eSIR_SUCCESS;  
#if 0
    tANI_U32                  cfgVal1;          
    tANI_U16                   cfgVal2;                 
    tSirMacHTCapabilityInfo *pHTCapabilityInfo;         
    tpDphHashNode pSta = NULL;

    tpPESession psessionEntry  = &pMac->lim.gpSession[0]; //                                       
    /*                               */    
    if (pMac->lim.gLimSystemRole != eSYSTEM_STA_ROLE) {  
        PELOGE(limLog(pMac, LOGE, FL("Got PMC indication when System not in the STA Role"));)
        return eSIR_FAILURE;       
    }      

    if ((pMac->lim.gHTMIMOPSState == state) || (state == eSIR_HT_MIMO_PS_NA )) { 
        PELOGE(limLog(pMac, LOGE, FL("Got Indication when already in the same mode or State passed is NA:%d "),  state);)
        return eSIR_FAILURE;      
    }     

    if (!pMac->lim.htCapability){        
        PELOGW(limLog(pMac, LOGW, FL(" Not in 11n or HT capable mode"));)
        return eSIR_FAILURE;   
    }        

    /*                                                */
    if (wlan_cfgGetInt(pMac, WNI_CFG_HT_CAP_INFO, &cfgVal1) != eSIR_SUCCESS) {  
            limLog(pMac, LOGP, FL("could not retrieve HT Cap CFG "));
            return eSIR_FAILURE;     
    }          

    cfgVal2 = (tANI_U16)cfgVal1;            
    pHTCapabilityInfo = (tSirMacHTCapabilityInfo *) &cfgVal2;          
    pHTCapabilityInfo->mimoPowerSave = state;

    if(cfgSetInt(pMac, WNI_CFG_HT_CAP_INFO, *(tANI_U16*)pHTCapabilityInfo) != eSIR_SUCCESS) {   
        limLog(pMac, LOGP, FL("could not update HT Cap Info CFG"));
        return eSIR_FAILURE;
    }

    PELOG2(limLog(pMac, LOG2, FL(" The HT Capability for Mimo Pwr is updated to State: %u  "),state);)
    if (pMac->lim.gLimSmeState != eLIM_SME_LINK_EST_STATE) { 
        PELOG2(limLog(pMac, LOG2,FL(" The STA is not in the Connected/Link Est Sme_State: %d  "), pMac->lim.gLimSmeState);)
        /*                                                  */   
        pMac->lim.gHTMIMOPSState = state;          
        return eSIR_SUCCESS;    
    }              

    pSta = dphGetHashEntry(pMac, DPH_STA_HASH_INDEX_PEER, &psessionEntry->dph.dphHashTable);    
    if (!pSta->mlmStaContext.htCapability) {
        limLog( pMac, LOGE,FL( "limSendSMPowerState: Peer is not HT Capable " ));
        return eSIR_FAILURE;
    }
     
    if (isEnteringMimoPS(pMac->lim.gHTMIMOPSState, state)) {    
        tSirMacAddr             macAddr;      
        /*                              */    
        vos_mem_copy((tANI_U8 *)macAddr, psessionEntry->bssId, sizeof(tSirMacAddr));
        /*                                                */       
        retStatus = limSendSMPowerStateFrame(pMac, macAddr, state);       
        if (retStatus != eSIR_SUCCESS) {         
            PELOGE(limLog(pMac, LOGE, "Update SM POWER: Sending Action Frame has failed");)
            return retStatus;         
        }   
    }    

    /*                                            */
    pMac->lim.gLimPrevMlmState = pMac->lim.gLimMlmState;
    pMac->lim.gLimMlmState = eLIM_MLM_WT_SET_MIMOPS_STATE;
    MTRACE(macTrace(pMac, TRACE_CODE_MLM_STATE, 0, pMac->lim.gLimMlmState));

    /*                                                      */    
    retStatus = limPostSMStateUpdate( pMac,psessionEntry->staId, state);     

    PELOG2(limLog(pMac, LOG2, " Updated the New SMPS State");)
    /*                                                  */   
    pMac->lim.gHTMIMOPSState = state;          
#endif
    return retStatus;
#endif
return eSIR_SUCCESS;
}

#ifdef WLAN_FEATURE_11AC
ePhyChanBondState limGet11ACPhyCBState(tpAniSirGlobal pMac, tANI_U8 channel, tANI_U8 htSecondaryChannelOffset,tANI_U8 peerCenterChan, tpPESession  psessionEntry)
{
    ePhyChanBondState cbState = PHY_SINGLE_CHANNEL_CENTERED;

    if(!psessionEntry->apChanWidth)
    {
        return htSecondaryChannelOffset;
    }

    if ( (htSecondaryChannelOffset 
                 == PHY_DOUBLE_CHANNEL_LOW_PRIMARY)
       )
    {
        if ((channel + 2 ) == peerCenterChan )
            cbState =  PHY_QUADRUPLE_CHANNEL_20MHZ_LOW_40MHZ_CENTERED;
        else if ((channel + 6 ) == peerCenterChan )
            cbState =  PHY_QUADRUPLE_CHANNEL_20MHZ_LOW_40MHZ_LOW;
        else if ((channel - 2 ) == peerCenterChan )
            cbState =  PHY_QUADRUPLE_CHANNEL_20MHZ_LOW_40MHZ_HIGH;
        else 
            limLog (pMac, LOGP, 
                       FL("Invalid Channel Number = %d Center Chan = %d "),
                                 channel, peerCenterChan);
    }
    if ( (htSecondaryChannelOffset 
                 == PHY_DOUBLE_CHANNEL_HIGH_PRIMARY)
       )
    {
        if ((channel - 2 ) == peerCenterChan )
            cbState =  PHY_QUADRUPLE_CHANNEL_20MHZ_HIGH_40MHZ_CENTERED;
        else if ((channel + 2 ) == peerCenterChan )
            cbState =  PHY_QUADRUPLE_CHANNEL_20MHZ_HIGH_40MHZ_LOW;
        else if ((channel - 6 ) == peerCenterChan )
            cbState =  PHY_QUADRUPLE_CHANNEL_20MHZ_HIGH_40MHZ_HIGH;
        else 
           limLog (pMac, LOGP, 
                         FL("Invalid Channel Number = %d Center Chan = %d "),
                                            channel, peerCenterChan);
    }
    return cbState;
}

#endif

void 
limSetChannel(tpAniSirGlobal pMac, tANI_U8 channel, tANI_U8 secChannelOffset, tPowerdBm maxTxPower, tANI_U8 peSessionId)
{
#if !defined WLAN_FEATURE_VOWIFI
    tANI_U32 localPwrConstraint;
#endif
    tpPESession peSession;

    peSession = peFindSessionBySessionId (pMac, peSessionId);

    if ( NULL == peSession)
    {
       limLog (pMac, LOGP, FL("Invalid PE session = %d"), peSessionId);
       return;
    }
#if defined WLAN_FEATURE_VOWIFI  
#ifdef WLAN_FEATURE_11AC
    if ( peSession->vhtCapability )
    {
        limSendSwitchChnlParams( pMac, channel, limGet11ACPhyCBState( pMac,channel,secChannelOffset,peSession->apCenterChan, peSession), maxTxPower, peSessionId);
    }
    else
#endif
    {
        limSendSwitchChnlParams( pMac, channel, secChannelOffset, maxTxPower, peSessionId);
    }
#else
    if (wlan_cfgGetInt(pMac, WNI_CFG_LOCAL_POWER_CONSTRAINT, &localPwrConstraint) != eSIR_SUCCESS) {
           limLog(pMac, LOGP, FL("could not read WNI_CFG_LOCAL_POWER_CONSTRAINT from CFG"));
           return;
    }
    //                                
#ifdef WLAN_FEATURE_11AC
    if ( peSession->vhtCapability && peSession->vhtCapabilityPresentInBeacon)
    {
        limSendSwitchChnlParams( pMac, channel, limGet11ACPhyCBState( pMac,channel,secChannelOffset,peSession->apCenterChan, peSession), maxTxPower, peSessionId);
    }
    else
#endif
    {
        limSendSwitchChnlParams( pMac, channel, secChannelOffset, (tPowerdBm)localPwrConstraint, peSessionId);
    }
#endif

 }

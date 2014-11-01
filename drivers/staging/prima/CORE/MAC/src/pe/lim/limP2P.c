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
                        L I M _ P 2 P . C

  OVERVIEW:

  This software unit holds the implementation of the WLAN Protocol Engine for
  P2P.

  Copyright (c) 2011 QUALCOMM Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*                                                                           

                                           


                                                                       
                                                                


                             


                                      
                                                                             
                                                                              
                                               
                                                                           */


#include "limUtils.h"
#include "limSessionUtils.h"
#include "wlan_qct_wda.h"

#define   PROBE_RSP_IE_OFFSET    36
#define   BSSID_OFFSET           16
#define   ADDR2_OFFSET           10
#define   ACTION_OFFSET          24

/*                                                             
                               */
#define   MAX_TIME_TO_BE_ACTIVE_CHANNEL 9000



void limRemainOnChnlSuspendLinkHdlr(tpAniSirGlobal pMac, eHalStatus status,
                                       tANI_U32 *data);
void limRemainOnChnlSetLinkStat(tpAniSirGlobal pMac, eHalStatus status,
                                tANI_U32 *data, tpPESession psessionEntry);
void limExitRemainOnChannel(tpAniSirGlobal pMac, eHalStatus status,
                         tANI_U32 *data, tpPESession psessionEntry);
void limRemainOnChnRsp(tpAniSirGlobal pMac, eHalStatus status, tANI_U32 *data);
extern tSirRetStatus limSetLinkState(
                         tpAniSirGlobal pMac, tSirLinkState state,
                         tSirMacAddr bssId, tSirMacAddr selfMacAddr, 
                         tpSetLinkStateCallback callback, void *callbackArg);

static tSirRetStatus limCreateSessionForRemainOnChn(tpAniSirGlobal pMac, tPESession **ppP2pSession);
eHalStatus limP2PActionCnf(tpAniSirGlobal pMac, tANI_U32 txCompleteSuccess);
/*                                                                  
  
                                                          
                                                                     
                         
  
                                                                    */
void limSetLinkStateP2PCallback(tpAniSirGlobal pMac, void *callbackArg)
{
    //                                       
    if(pMac->lim.gpLimRemainOnChanReq)
    {
        limSendSmeRsp(pMac, eWNI_SME_REMAIN_ON_CHN_RDY_IND, eHAL_STATUS_SUCCESS, 
                     pMac->lim.gpLimRemainOnChanReq->sessionId, 0); 
    }
    else
    {
        //                                                     
        limLog( pMac, LOGE, FL(" NULL pointer of gpLimRemainOnChanReq") );
    }
}

/*                                                                  
  
                                                                    
                                                                           
  
                                                                    */


int limProcessRemainOnChnlReq(tpAniSirGlobal pMac, tANI_U32 *pMsg)
{

    /*                                                                                                               
                                                                                                                         
                                                                                                                   
                                                                 
                                                                                                                                       
     */
#ifdef CONC_OPER_AND_LISTEN_CHNL_SAME_OPTIMIZE
    tANI_U8 i;
    tpPESession psessionEntry;
#endif
#ifdef WLAN_FEATURE_P2P_INTERNAL
    tpPESession pP2pSession;
#endif

    tSirRemainOnChnReq *MsgBuff = (tSirRemainOnChnReq *)pMsg;
    pMac->lim.gpLimRemainOnChanReq = MsgBuff;

#ifdef CONC_OPER_AND_LISTEN_CHNL_SAME_OPTIMIZE
    for (i =0; i < pMac->lim.maxBssId;i++)
    {
        psessionEntry = peFindSessionBySessionId(pMac,i);

        if ( (psessionEntry != NULL) )
        {
            if (psessionEntry->currentOperChannel == MsgBuff->chnNum)
            {
                tANI_U32 val;
                tSirMacAddr nullBssid = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

                pMac->lim.p2pRemOnChanTimeStamp = vos_timer_get_system_time();
                pMac->lim.gTotalScanDuration = MsgBuff->duration;

                /*                                   */
                val = SYS_MS_TO_TICKS(MsgBuff->duration);

                limLog( pMac, LOG2, "Start listen duration = %d", val);
                if (tx_timer_change(
                        &pMac->lim.limTimers.gLimRemainOnChannelTimer, val, 0)
                                          != TX_SUCCESS)
                {
                    limLog(pMac, LOGP,
                          FL("Unable to change remain on channel Timer val"));
                    goto error;
                }
                else if(TX_SUCCESS != tx_timer_activate(
                                &pMac->lim.limTimers.gLimRemainOnChannelTimer))
                {
                    limLog(pMac, LOGP,
                    FL("Unable to activate remain on channel Timer"));
                    limDeactivateAndChangeTimer(pMac, eLIM_REMAIN_CHN_TIMER);
                    goto error;
                }

#ifdef WLAN_FEATURE_P2P_INTERNAL
                //                                   
                if(eSIR_SUCCESS != limCreateSessionForRemainOnChn(pMac, &pP2pSession))
                {
                    limLog( pMac, LOGE, "Unable to create session");
                    goto error;
                }
#endif

                if ((limSetLinkState(pMac, MsgBuff->isProbeRequestAllowed?
                                     eSIR_LINK_LISTEN_STATE:eSIR_LINK_SEND_ACTION_STATE,
                                     nullBssid, pMac->lim.gSelfMacAddr,
                                     limSetLinkStateP2PCallback, NULL)) != eSIR_SUCCESS)
                {
                    limLog( pMac, LOGE, "Unable to change link state");
                    goto error;
                }
                return FALSE;
            }
        }
    }
#endif
    pMac->lim.gLimPrevMlmState = pMac->lim.gLimMlmState;
    pMac->lim.gLimMlmState     = eLIM_MLM_P2P_LISTEN_STATE;

    pMac->lim.gTotalScanDuration = MsgBuff->duration;

    /*                                                                      */
    limSuspendLink(pMac, eSIR_CHECK_LINK_TRAFFIC_BEFORE_SCAN,
                   limRemainOnChnlSuspendLinkHdlr, NULL);
    return FALSE;

#ifdef CONC_OPER_AND_LISTEN_CHNL_SAME_OPTIMIZE
error:
    limRemainOnChnRsp(pMac,eHAL_STATUS_FAILURE, NULL);
    /*                             */
    return FALSE;
#endif
}


tSirRetStatus limCreateSessionForRemainOnChn(tpAniSirGlobal pMac, tPESession **ppP2pSession)
{
    tSirRetStatus nSirStatus = eSIR_FAILURE;
    tpPESession psessionEntry;
    tANI_U8 sessionId;
    tANI_U32 val;

    if(pMac->lim.gpLimRemainOnChanReq && ppP2pSession)
    {
        if((psessionEntry = peCreateSession(pMac,
           pMac->lim.gpLimRemainOnChanReq->selfMacAddr, &sessionId, 1)) == NULL)
        {
            limLog(pMac, LOGE, FL("Session Can not be created "));
            /*                            */
            return nSirStatus;
        }
        /*                                      */
        psessionEntry->peSessionId = sessionId;

        psessionEntry->limSystemRole = eLIM_P2P_DEVICE_ROLE;
        CFG_GET_STR( nSirStatus, pMac,  WNI_CFG_SUPPORTED_RATES_11A,
               psessionEntry->rateSet.rate, val , SIR_MAC_MAX_NUMBER_OF_RATES );
        psessionEntry->rateSet.numRates = val;

        CFG_GET_STR( nSirStatus, pMac, WNI_CFG_EXTENDED_OPERATIONAL_RATE_SET,
                     psessionEntry->extRateSet.rate, val,
                     WNI_CFG_EXTENDED_OPERATIONAL_RATE_SET_LEN );
        psessionEntry->extRateSet.numRates = val;

        sirCopyMacAddr(psessionEntry->selfMacAddr,
                       pMac->lim.gpLimRemainOnChanReq->selfMacAddr);

        psessionEntry->currentOperChannel = pMac->lim.gpLimRemainOnChanReq->chnNum;
        nSirStatus = eSIR_SUCCESS;
        *ppP2pSession = psessionEntry;
    }

    return nSirStatus;
}


/*                                                                  
  
                                                                      
  
  
                                                                    */

tSirRetStatus limRemainOnChnlChangeChnReq(tpAniSirGlobal pMac,
                                          eHalStatus status, tANI_U32 *data)
{
    tpPESession psessionEntry;
    tANI_U8 sessionId = 0;
    tSirRetStatus nSirStatus = eSIR_FAILURE;

    if( NULL == pMac->lim.gpLimRemainOnChanReq )
    {
        //                                
        PELOGE(limLog( pMac, LOGE, FL(" gpLimRemainOnChanReq is NULL") );)
        return nSirStatus;
    }

     /*                           */
    if (status != eHAL_STATUS_SUCCESS)
    {
        PELOGE(limLog( pMac, LOGE, FL(" Suspend link Failure ") );)
        goto error;
    }


    if((psessionEntry = peFindSessionByBssid(
        pMac,pMac->lim.gpLimRemainOnChanReq->selfMacAddr, &sessionId)) != NULL)
    {
        goto change_channel;
    }
    else /*                                              */
    {
        /*                             */
        if(eSIR_SUCCESS != limCreateSessionForRemainOnChn(pMac, &psessionEntry))
        {
            limLog(pMac, LOGE, FL("Session Can not be created "));
            /*                            */
            goto error;
        }
    }

change_channel:
    /*                                                */
    limChangeChannelWithCallback(pMac,
                              pMac->lim.gpLimRemainOnChanReq->chnNum,
                              limRemainOnChnlSetLinkStat, NULL, psessionEntry);
     return eSIR_SUCCESS;

error:
     limRemainOnChnRsp(pMac,eHAL_STATUS_FAILURE, NULL);
     return eSIR_FAILURE;
}

void limRemainOnChnlSuspendLinkHdlr(tpAniSirGlobal pMac, eHalStatus status,
                                       tANI_U32 *data)
{
    limRemainOnChnlChangeChnReq(pMac, status, data);
    return;
}

/*                                                                  
  
                                                                         
  
                                                                    */
void limRemainOnChnlSetLinkStat(tpAniSirGlobal pMac, eHalStatus status,
                                tANI_U32 *data, tpPESession psessionEntry)
{
    tANI_U32 val;
    tSirRemainOnChnReq *MsgRemainonChannel = pMac->lim.gpLimRemainOnChanReq;
    tSirMacAddr             nullBssid = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    if (status != eHAL_STATUS_SUCCESS)
    {
        limLog( pMac, LOGE, "%s: Change channel not successful");
        goto error1;
    }

    //                                                    
    pMac->lim.limTimers.gLimRemainOnChannelTimer.sessionId =
                                                psessionEntry->peSessionId;
    pMac->lim.p2pRemOnChanTimeStamp = vos_timer_get_system_time();
    pMac->lim.gTotalScanDuration = MsgRemainonChannel->duration;

      /*                                   */
    val = SYS_MS_TO_TICKS(MsgRemainonChannel->duration);

    limLog( pMac, LOG2, "Start listen duration = %d", val);
    if (tx_timer_change(&pMac->lim.limTimers.gLimRemainOnChannelTimer,
                                                val, 0) != TX_SUCCESS)
    {
       /* 
                                                              
        */
        limLog(pMac, LOGP,
               FL("Unable to change remain on channel Timer val"));
        goto error;
    }

    if(TX_SUCCESS !=
       tx_timer_activate(&pMac->lim.limTimers.gLimRemainOnChannelTimer))
    {
        limLog( pMac, LOGE,
                  "%s: remain on channel Timer Start Failed", __func__);
        goto error;
    }

    if ((limSetLinkState(pMac, MsgRemainonChannel->isProbeRequestAllowed?
                         eSIR_LINK_LISTEN_STATE:eSIR_LINK_SEND_ACTION_STATE,nullBssid,
                         pMac->lim.gSelfMacAddr, limSetLinkStateP2PCallback, 
                         NULL)) != eSIR_SUCCESS)
    {
        limLog( pMac, LOGE, "Unable to change link state");
        goto error;
    }

    return;
error:
    limDeactivateAndChangeTimer(pMac, eLIM_REMAIN_CHN_TIMER);
error1:
    limRemainOnChnRsp(pMac,eHAL_STATUS_FAILURE, NULL);
    return;
}

/*                                                                  
  
                                                                                   
                      
  
                                                                    */
void limProcessInsertSingleShotNOATimeout(tpAniSirGlobal pMac)
{
    /*                                                                                       
                         
     */
    limDeactivateAndChangeTimer(pMac, eLIM_INSERT_SINGLESHOT_NOA_TIMER);

    /*                                                                           */
    limProcessRegdDefdSmeReqAfterNOAStart(pMac);

    return;
}

/*                                                                 
                                                                  
                                                       
                                                          
                                                                    */
void limConvertActiveChannelToPassiveChannel(tpAniSirGlobal pMac )
{
    tANI_U32 currentTime;
    tANI_U32 lastTime = 0;
    tANI_U32 timeDiff;
    tANI_U8 i;
    currentTime = vos_timer_get_system_time();
    for (i = 1; i < SIR_MAX_24G_5G_CHANNEL_RANGE ; i++)
    {
        if ((pMac->lim.dfschannelList.timeStamp[i]) != 0)
        {
            lastTime = pMac->lim.dfschannelList.timeStamp[i];
            if (currentTime >= lastTime)
            {
                timeDiff = (currentTime - lastTime);
            }
            else
            {
                timeDiff = (0xFFFFFFFF - lastTime) + currentTime;
            }

            if (timeDiff >= MAX_TIME_TO_BE_ACTIVE_CHANNEL)
            {
                limCovertChannelScanType( pMac, i,FALSE);
                pMac->lim.dfschannelList.timeStamp[i] = 0;
            }
        }
    }
    /*                                                                 
                                                                                 
    */
    if (lastTime != 0)
    {
        if (tx_timer_activate(
                         &pMac->lim.limTimers.gLimActiveToPassiveChannelTimer)
                          != TX_SUCCESS)
        {
            limLog(pMac, LOGE, FL("Could not activate Active to Passive Channel  timer"));
        }
    }

    return;

}

/*                                                                  
  
                                                                
                       
  
                                                                    */

void limProcessRemainOnChnTimeout(tpAniSirGlobal pMac)
{
    tpPESession psessionEntry;
    tSirMacAddr             nullBssid = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    //                                                   
    //                                          
    if (tx_timer_running(&pMac->lim.limTimers.gLimRemainOnChannelTimer))
    {
        limLog( pMac, LOGE, 
                "still timer is running already and not processing limProcessRemainOnChnTimeout");
        return;
    }

    limDeactivateAndChangeTimer(pMac, eLIM_REMAIN_CHN_TIMER);

    if (NULL == pMac->lim.gpLimRemainOnChanReq)
    {
        limLog( pMac, LOGE, "No Remain on channel pending");
        return;
    }

    /*                                   */
    if (limSetLinkState(pMac, eSIR_LINK_IDLE_STATE, nullBssid,
        pMac->lim.gSelfMacAddr, NULL, NULL) != eSIR_SUCCESS)
    {
        limLog( pMac, LOGE, "Unable to change link state");
        return;
    }

    if (pMac->lim.gLimMlmState  != eLIM_MLM_P2P_LISTEN_STATE )
    {
        limRemainOnChnRsp(pMac,eHAL_STATUS_SUCCESS, NULL);
    }
    else
    {
        /*                 */
        if((psessionEntry = peFindSessionBySessionId(pMac,
            pMac->lim.limTimers.gLimRemainOnChannelTimer.sessionId))== NULL)
        {
            limLog(pMac, LOGE,
                  FL("Session Does not exist for given sessionID"));
            goto error;
        }

        limExitRemainOnChannel(pMac, eHAL_STATUS_SUCCESS, NULL, psessionEntry);
        return;
error:
        limRemainOnChnRsp(pMac,eHAL_STATUS_FAILURE, NULL);
    }
    return;
}


/*                                                                  
  
                                                                           
            
  
                                                                    */

void limExitRemainOnChannel(tpAniSirGlobal pMac, eHalStatus status,
                         tANI_U32 *data, tpPESession psessionEntry)
{
    
    if (status != eHAL_STATUS_SUCCESS)
    {
        PELOGE(limLog( pMac, LOGE, "Remain on Channel Failed");)
        goto error;
    }
    //                                                       
    //                                                               
    peSetResumeChannel(pMac, 0, 0);
    limResumeLink(pMac, limRemainOnChnRsp, NULL);
    return;
error:
    limRemainOnChnRsp(pMac,eHAL_STATUS_FAILURE, NULL);
    return;
}

/*                                                                  
  
                                                   
  
                                                                    */
void limRemainOnChnRsp(tpAniSirGlobal pMac, eHalStatus status, tANI_U32 *data)
{
    tpPESession psessionEntry;
    tANI_U8             sessionId;
    tSirRemainOnChnReq *MsgRemainonChannel = pMac->lim.gpLimRemainOnChanReq;
    tSirMacAddr             nullBssid = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    if ( NULL == MsgRemainonChannel )
    {
        PELOGE(limLog( pMac, LOGP,
             "%s: No Pointer for Remain on Channel Req", __func__);)
        return;
    }

    //                                            
    //                  
    if(eHAL_STATUS_FAILURE == status)
    {
       //                                  
       limDeactivateAndChangeTimer(pMac, eLIM_REMAIN_CHN_TIMER);

       //                          
       /*                                   */
       if (limSetLinkState(pMac, eSIR_LINK_IDLE_STATE, nullBssid,
           pMac->lim.gSelfMacAddr, NULL, NULL) != eSIR_SUCCESS)
       {
           limLog( pMac, LOGE, "Unable to change link state");
       }

       pMac->lim.gLimSystemInScanLearnMode = 0;
       pMac->lim.gLimHalScanState = eLIM_HAL_IDLE_SCAN_STATE;
    }

    /*                    */
    if((psessionEntry = peFindSessionByBssid(pMac,
                 MsgRemainonChannel->selfMacAddr,&sessionId)) != NULL)
    {
        if ( eLIM_P2P_DEVICE_ROLE == psessionEntry->limSystemRole )
        {
           peDeleteSession( pMac, psessionEntry);
        }
    }

    /*                          */
    limSendSmeRsp(pMac, eWNI_SME_REMAIN_ON_CHN_RSP, status, 
                  MsgRemainonChannel->sessionId, 0);

    vos_mem_free(pMac->lim.gpLimRemainOnChanReq);
    pMac->lim.gpLimRemainOnChanReq = NULL;

    pMac->lim.gLimMlmState = pMac->lim.gLimPrevMlmState;

    /*                                                                     
                                                   */
    if (pMac->lim.mgmtFrameSessionId != 0xff)
    {
       limP2PActionCnf(pMac, 0);
    }

    return;
}

/*                                                                  
  
                                                          
                                          
  
                                                                    */
void limSendSmeMgmtFrameInd(
                    tpAniSirGlobal pMac, tANI_U8 frameType,
                    tANI_U8  *frame, tANI_U32 frameLen, tANI_U16 sessionId,
                    tANI_U32 rxChannel, tpPESession psessionEntry,
                    tANI_S8 rxRssi)
{
    tSirMsgQ              mmhMsg;
    tpSirSmeMgmtFrameInd pSirSmeMgmtFrame = NULL;
    tANI_U16              length;

    length = sizeof(tSirSmeMgmtFrameInd) + frameLen;

    pSirSmeMgmtFrame = vos_mem_malloc(length);
    if (NULL == pSirSmeMgmtFrame)
    {
        limLog(pMac, LOGP,
               FL("AllocateMemory failed for eWNI_SME_LISTEN_RSP"));
        return;
    }
    vos_mem_set((void*)pSirSmeMgmtFrame, length, 0);

    pSirSmeMgmtFrame->mesgType = eWNI_SME_MGMT_FRM_IND;
    pSirSmeMgmtFrame->mesgLen = length;
    pSirSmeMgmtFrame->sessionId = sessionId;
    pSirSmeMgmtFrame->frameType = frameType;
    pSirSmeMgmtFrame->rxRssi = rxRssi;

    /*
                                                                              
                                                            
                                                                             
                                                                               
                                                                     
                                                                            
                                                                              
                                                                      
                          
                                                                   
     */
    if( VOS_FALSE ==
        tx_timer_running(&pMac->lim.limTimers.gLimRemainOnChannelTimer) )
    {
        tpPESession pTempSessionEntry = psessionEntry;
        if( ( (NULL != pTempSessionEntry) ||
              (pTempSessionEntry = limIsApSessionActive(pMac)) ) &&
            (SIR_BAND_5_GHZ == limGetRFBand(pTempSessionEntry->currentOperChannel)) )
        {
            rxChannel = pTempSessionEntry->currentOperChannel;
        }
    }

    pSirSmeMgmtFrame->rxChan = rxChannel;

    vos_mem_zero(pSirSmeMgmtFrame->frameBuf, frameLen);
    vos_mem_copy(pSirSmeMgmtFrame->frameBuf, frame, frameLen);

    mmhMsg.type = eWNI_SME_MGMT_FRM_IND;
    mmhMsg.bodyptr = pSirSmeMgmtFrame;
    mmhMsg.bodyval = 0;

    if(VOS_TRUE == tx_timer_running(&pMac->lim.limTimers.gLimRemainOnChannelTimer) && 
            ( (psessionEntry != NULL) && (psessionEntry->pePersona != VOS_P2P_GO_MODE)) &&
            (frameType == SIR_MAC_MGMT_ACTION))
    {
            unsigned int chanWaitTime, vStatus ;

            limLog( pMac, LOG1, FL("Rx: Extend the gLimRemainOnChannelTimer = %d "),
                                                              pMac->lim.gTotalScanDuration);

            pMac->lim.p2pRemOnChanTimeStamp = vos_timer_get_system_time();

            chanWaitTime = SYS_MS_TO_TICKS(pMac->lim.gTotalScanDuration);
            vStatus = tx_timer_deactivate(&pMac->lim.limTimers.gLimRemainOnChannelTimer);

            if (VOS_STATUS_SUCCESS != vStatus)
            {     
                limLog( pMac, LOGE, FL("Rx: Extend the gLimRemainOnChannelTimer"));
            } 

            if (tx_timer_change(&pMac->lim.limTimers.gLimRemainOnChannelTimer, chanWaitTime, 0) != TX_SUCCESS)
            {
                limLog( pMac, LOGE, FL("Unable to change the gLimRemainOnChannelTimer"));
            }

            if (tx_timer_activate(&pMac->lim.limTimers.gLimRemainOnChannelTimer) != 0)
            {
                limLog( pMac, LOGE, FL("Unable to active the gLimRemainOnChannelTimer"));
            } 
    }
    else
    {
       if(frameType == SIR_MAC_MGMT_ACTION)
            limLog( pMac, LOGE, FL("Rx: NO REMAIN ON CHANNEL and recd action frame "));
    }

    limSysProcessMmhMsgApi(pMac, &mmhMsg, ePROT);
    return;
} /*                               */


eHalStatus limP2PActionCnf(tpAniSirGlobal pMac, tANI_U32 txCompleteSuccess)
{
    if (pMac->lim.mgmtFrameSessionId != 0xff)
    {
        /*                                                                            
                                           */
        limSendSmeRsp(pMac, eWNI_SME_ACTION_FRAME_SEND_CNF,
                (txCompleteSuccess ? eSIR_SME_SUCCESS : eSIR_SME_SEND_ACTION_FAIL),
                pMac->lim.mgmtFrameSessionId, 0);
        pMac->lim.mgmtFrameSessionId = 0xff;
    }

    return eHAL_STATUS_SUCCESS;
}


void limSetHtCaps(tpAniSirGlobal pMac, tpPESession psessionEntry, tANI_U8 *pIeStartPtr,tANI_U32 nBytes)
{
    v_U8_t              *pIe=NULL;
    tDot11fIEHTCaps     dot11HtCap;

    PopulateDot11fHTCaps(pMac, psessionEntry, &dot11HtCap);
    pIe = limGetIEPtr(pMac,pIeStartPtr, nBytes,
                                       DOT11F_EID_HTCAPS,ONE_BYTE);
   limLog( pMac, LOG2, FL("pIe 0x%x dot11HtCap.supportedMCSSet[0]=0x%x"),
        (tANI_U32)pIe,dot11HtCap.supportedMCSSet[0]);
    if(pIe)
    {
        tHtCaps *pHtcap = (tHtCaps *)&pIe[2]; //                                         
        pHtcap->advCodingCap = dot11HtCap.advCodingCap;
        pHtcap->supportedChannelWidthSet = dot11HtCap.supportedChannelWidthSet;
        pHtcap->mimoPowerSave = dot11HtCap.mimoPowerSave;
        pHtcap->greenField = dot11HtCap.greenField;
        pHtcap->shortGI20MHz = dot11HtCap.shortGI20MHz;
        pHtcap->shortGI40MHz = dot11HtCap.shortGI40MHz;
        pHtcap->txSTBC = dot11HtCap.txSTBC;
        pHtcap->rxSTBC = dot11HtCap.rxSTBC;
        pHtcap->delayedBA = dot11HtCap.delayedBA  ;
        pHtcap->maximalAMSDUsize = dot11HtCap.maximalAMSDUsize;
        pHtcap->dsssCckMode40MHz = dot11HtCap.dsssCckMode40MHz;
        pHtcap->psmp = dot11HtCap.psmp;
        pHtcap->stbcControlFrame = dot11HtCap.stbcControlFrame;
        pHtcap->lsigTXOPProtection = dot11HtCap.lsigTXOPProtection;
        pHtcap->maxRxAMPDUFactor = dot11HtCap.maxRxAMPDUFactor;
        pHtcap->mpduDensity = dot11HtCap.mpduDensity;
        vos_mem_copy((void *)pHtcap->supportedMCSSet,
                     (void *)(dot11HtCap.supportedMCSSet),
                      sizeof(pHtcap->supportedMCSSet));
        pHtcap->pco = dot11HtCap.pco;
        pHtcap->transitionTime = dot11HtCap.transitionTime;
        pHtcap->mcsFeedback = dot11HtCap.mcsFeedback;
        pHtcap->txBF = dot11HtCap.txBF;
        pHtcap->rxStaggeredSounding = dot11HtCap.rxStaggeredSounding;
        pHtcap->txStaggeredSounding = dot11HtCap.txStaggeredSounding;
        pHtcap->rxZLF = dot11HtCap.rxZLF;
        pHtcap->txZLF = dot11HtCap.txZLF;
        pHtcap->implicitTxBF = dot11HtCap.implicitTxBF;
        pHtcap->calibration = dot11HtCap.calibration;
        pHtcap->explicitCSITxBF = dot11HtCap.explicitCSITxBF;
        pHtcap->explicitUncompressedSteeringMatrix =
            dot11HtCap.explicitUncompressedSteeringMatrix;
        pHtcap->explicitBFCSIFeedback = dot11HtCap.explicitBFCSIFeedback;
        pHtcap->explicitUncompressedSteeringMatrixFeedback =
            dot11HtCap.explicitUncompressedSteeringMatrixFeedback;
        pHtcap->explicitCompressedSteeringMatrixFeedback =
            dot11HtCap.explicitCompressedSteeringMatrixFeedback;
        pHtcap->csiNumBFAntennae = dot11HtCap.csiNumBFAntennae;
        pHtcap->uncompressedSteeringMatrixBFAntennae =
            dot11HtCap.uncompressedSteeringMatrixBFAntennae;
        pHtcap->compressedSteeringMatrixBFAntennae =
            dot11HtCap.compressedSteeringMatrixBFAntennae;
        pHtcap->antennaSelection = dot11HtCap.antennaSelection;
        pHtcap->explicitCSIFeedbackTx = dot11HtCap.explicitCSIFeedbackTx;
        pHtcap->antennaIndicesFeedbackTx = dot11HtCap.antennaIndicesFeedbackTx;
        pHtcap->explicitCSIFeedback = dot11HtCap.explicitCSIFeedback;
        pHtcap->antennaIndicesFeedback = dot11HtCap.antennaIndicesFeedback;
        pHtcap->rxAS = dot11HtCap.rxAS;
        pHtcap->txSoundingPPDUs = dot11HtCap.txSoundingPPDUs;
    }
}


void limSendP2PActionFrame(tpAniSirGlobal pMac, tpSirMsgQ pMsg)
{
    tSirMbMsgP2p *pMbMsg = (tSirMbMsgP2p *)pMsg->bodyptr;
    tANI_U32            nBytes;
    tANI_U8             *pFrame;
    void                *pPacket;
    eHalStatus          halstatus;
    tANI_U8             txFlag = 0;
    tpSirMacFrameCtl    pFc = (tpSirMacFrameCtl ) pMbMsg->data;
    tANI_U8             noaLen = 0;
    tANI_U8             noaStream[SIR_MAX_NOA_ATTR_LEN + (2*SIR_P2P_IE_HEADER_LEN)];
    tANI_U8             origLen = 0;
    tANI_U8             sessionId = 0;
    v_U8_t              *pP2PIe = NULL;
    tpPESession         psessionEntry;
    v_U8_t              *pPresenceRspNoaAttr = NULL;
    v_U8_t              *pNewP2PIe = NULL;
    v_U16_t             remainLen = 0;

    nBytes = pMbMsg->msgLen - sizeof(tSirMbMsg);

    limLog( pMac, LOG1, FL("sending pFc->type=%d pFc->subType=%d"),
                            pFc->type, pFc->subType);

    psessionEntry = peFindSessionByBssid(pMac,
                   (tANI_U8*)pMbMsg->data + BSSID_OFFSET, &sessionId);

    /*                                                                   
                         */  
    if( NULL == psessionEntry )
    {
        psessionEntry = peFindSessionByBssid(pMac,
                   (tANI_U8*)pMbMsg->data + ADDR2_OFFSET, &sessionId);
    }

    if( NULL == psessionEntry )
    {
        tANI_U8             isSessionActive = 0;
        tANI_U8             i;
        
        /*                                                                  
                                                                     
                                                                         */ 
        for (i =0; i < pMac->lim.maxBssId;i++)
        {
            psessionEntry = peFindSessionBySessionId(pMac,i);
            if ( NULL != psessionEntry)
            {
                isSessionActive = 1;
                break;
            }
        }
        if( !isSessionActive )
        {
            limSendSmeRsp(pMac, eWNI_SME_ACTION_FRAME_SEND_CNF, 
                          eHAL_STATUS_FAILURE, pMbMsg->sessionId, 0);
            return;
        }
    }

    if ((SIR_MAC_MGMT_FRAME == pFc->type)&&
        ((SIR_MAC_MGMT_PROBE_RSP == pFc->subType)||
        (SIR_MAC_MGMT_ACTION == pFc->subType)))
    {
        //                                                     
        if (SIR_MAC_MGMT_PROBE_RSP == pFc->subType)
        {
            //                               
            pP2PIe = limGetP2pIEPtr(pMac,
                          (tANI_U8*)pMbMsg->data + PROBE_RSP_IE_OFFSET,
                          nBytes - PROBE_RSP_IE_OFFSET);
            while ((NULL != pP2PIe) && (SIR_MAC_MAX_IE_LENGTH == pP2PIe[1]))
            {
                remainLen = nBytes - (pP2PIe - (tANI_U8*)pMbMsg->data);
                if (remainLen > 2)
                {
                     pNewP2PIe = limGetP2pIEPtr(pMac,
                                pP2PIe+SIR_MAC_MAX_IE_LENGTH + 2, remainLen);
                }
                if (pNewP2PIe)
                {
                    pP2PIe = pNewP2PIe;
                    pNewP2PIe = NULL;
                }
                else
                {
                    break;
                }
            } //            
        }
        else
        {
            if (SIR_MAC_ACTION_VENDOR_SPECIFIC_CATEGORY ==
                *((v_U8_t *)pMbMsg->data+ACTION_OFFSET))
            {
                tpSirMacP2PActionFrameHdr pActionHdr =
                    (tpSirMacP2PActionFrameHdr)((v_U8_t *)pMbMsg->data +
                                                        ACTION_OFFSET);
                if (vos_mem_compare( pActionHdr->Oui,
                    SIR_MAC_P2P_OUI, SIR_MAC_P2P_OUI_SIZE ) &&
                    (SIR_MAC_ACTION_P2P_SUBTYPE_PRESENCE_RSP ==
                    pActionHdr->OuiSubType))
                { //                                
                    pP2PIe = limGetP2pIEPtr(pMac,
                                 (v_U8_t *)pMbMsg->data + ACTION_OFFSET +
                                 sizeof(tSirMacP2PActionFrameHdr),
                                 (nBytes - ACTION_OFFSET -
                                 sizeof(tSirMacP2PActionFrameHdr)));
                    if( NULL != pP2PIe )
                    {
                        //                                                   
                        pPresenceRspNoaAttr =
                        limGetIEPtr(pMac,pP2PIe + SIR_P2P_IE_HEADER_LEN,
                                    pP2PIe[1], SIR_P2P_NOA_ATTR,TWO_BYTE);
                     }
                }
            }
        }

        if (pP2PIe != NULL)
        {
            //                               
            noaLen = limGetNoaAttrStream(pMac, noaStream, psessionEntry);
            //                                      
            if (noaLen > 0)
            {
                origLen = pP2PIe[1];
               //                          
                if (pPresenceRspNoaAttr)
                {
                    v_U16_t noaAttrLen = pPresenceRspNoaAttr[1] |
                                        (pPresenceRspNoaAttr[2]<<8);
                    /*                                         */
                    origLen -= (noaAttrLen + 1 + 2);
                    //                          
                    nBytes  -= (noaAttrLen + 1 + 2);
                    //                            
                    pP2PIe[1] = origLen;
                }
                if ((pP2PIe[1] + (tANI_U16)noaLen)> SIR_MAC_MAX_IE_LENGTH)
                {
                    //                                               
                    noaLen = limGetNoaAttrStreamInMultP2pIes(pMac, noaStream,
                                 noaLen,((pP2PIe[1] + (tANI_U16)noaLen)-
                                 SIR_MAC_MAX_IE_LENGTH));
                    pP2PIe[1] = SIR_MAC_MAX_IE_LENGTH;
                }
                else
                {
                    pP2PIe[1] += noaLen; //                              
                }
                nBytes += noaLen;
                limLog( pMac, LOGE,
                        FL("noaLen=%d origLen=%d pP2PIe=0x%x"
                        " nBytes=%d nBytesToCopy=%d "),
                                   noaLen,origLen,pP2PIe,nBytes,
                   ((pP2PIe + origLen + 2) - (v_U8_t *)pMbMsg->data));
            }
        }

        if (SIR_MAC_MGMT_PROBE_RSP == pFc->subType)
        {
            limSetHtCaps( pMac, psessionEntry, (tANI_U8*)pMbMsg->data + PROBE_RSP_IE_OFFSET,
                           nBytes - PROBE_RSP_IE_OFFSET);
        }
        
        /*                                                                
                                                                                                
                                                                                                      
                                                                                     */
        if ((SIR_MAC_MGMT_ACTION == pFc->subType) &&
                (0 != pMbMsg->wait))
        {
            if (tx_timer_running(&pMac->lim.limTimers.gLimRemainOnChannelTimer))
            {
                tANI_U32 val = 0;
                tx_timer_deactivate(&pMac->lim.limTimers.gLimRemainOnChannelTimer);
                /*                                   */
                pMac->lim.p2pRemOnChanTimeStamp = vos_timer_get_system_time();
                pMac->lim.gTotalScanDuration = pMbMsg->wait;

                val = SYS_MS_TO_TICKS(pMbMsg->wait);

                limLog(pMac, LOG1,
                        FL("Tx: Extending the gLimRemainOnChannelTimer"));
                if (tx_timer_change(
                            &pMac->lim.limTimers.gLimRemainOnChannelTimer, val, 0)
                        != TX_SUCCESS)
                {
                    limLog(pMac, LOGP,
                            FL("Unable to change remain on channel Timer val"));
                    return;
                }
                else if(TX_SUCCESS != tx_timer_activate(
                            &pMac->lim.limTimers.gLimRemainOnChannelTimer))
                {
                    limLog(pMac, LOGP,
                            FL("Unable to activate remain on channel Timer"));
                    limDeactivateAndChangeTimer(pMac, eLIM_REMAIN_CHN_TIMER);
                    return;
                }
            }
            else
            {
                limLog(pMac, LOGE,
                            FL("Failed to Send Action frame \n"));
                limSendSmeRsp(pMac, eWNI_SME_ACTION_FRAME_SEND_CNF, 
                        eHAL_STATUS_FAILURE, pMbMsg->sessionId, 0);
                return;
            }
        }
    }


    //                                  
    halstatus = palPktAlloc( pMac->hHdd, HAL_TXRX_FRM_802_11_MGMT,
                      (tANI_U16)nBytes, ( void** ) &pFrame, (void**) &pPacket);
    if ( ! HAL_STATUS_SUCCESS ( halstatus ) )
    {
        limLog( pMac, LOGE, FL("Failed to allocate %d bytes for a Probe"
          " Request."), nBytes );
        return;
    }

    //          
    vos_mem_set(pFrame, nBytes, 0);

    if ((noaLen > 0) && (noaLen<(SIR_MAX_NOA_ATTR_LEN + SIR_P2P_IE_HEADER_LEN)))
    {
        //                                          
        v_U32_t nBytesToCopy = ((pP2PIe + origLen + 2 ) -
                                (v_U8_t *)pMbMsg->data);
        vos_mem_copy(pFrame, pMbMsg->data, nBytesToCopy);
        vos_mem_copy((pFrame + nBytesToCopy), noaStream, noaLen);
        vos_mem_copy((pFrame + nBytesToCopy + noaLen),
        pMbMsg->data + nBytesToCopy, nBytes - nBytesToCopy - noaLen);

    }
    else
    {
        vos_mem_copy(pFrame, pMbMsg->data, nBytes);
    }

    /*                                                                     
                                                          
     */
    txFlag |= HAL_USE_BD_RATE2_FOR_MANAGEMENT_FRAME;

    if ( (SIR_MAC_MGMT_PROBE_RSP == pFc->subType) ||
         (pMbMsg->noack)
        )
    {
        halstatus = halTxFrame( pMac, pPacket, (tANI_U16)nBytes,
                        HAL_TXRX_FRM_802_11_MGMT, ANI_TXDIR_TODS,
                        7,/*                           */ limTxComplete, pFrame,
                        txFlag );

        if (!pMbMsg->noack)
        {
           limSendSmeRsp(pMac, eWNI_SME_ACTION_FRAME_SEND_CNF, 
               halstatus, pMbMsg->sessionId, 0);
        }
        pMac->lim.mgmtFrameSessionId = 0xff;
    }
    else
    {
        pMac->lim.mgmtFrameSessionId = 0xff;
        halstatus = halTxFrameWithTxComplete( pMac, pPacket, (tANI_U16)nBytes,
                        HAL_TXRX_FRM_802_11_MGMT, ANI_TXDIR_TODS,
                        7,/*                           */ limTxComplete, pFrame,
                        limP2PActionCnf, txFlag );

        if ( ! HAL_STATUS_SUCCESS ( halstatus ) )
        {
             limLog( pMac, LOGE, FL("could not send action frame!" ));
             limSendSmeRsp(pMac, eWNI_SME_ACTION_FRAME_SEND_CNF, halstatus, 
                pMbMsg->sessionId, 0);
             pMac->lim.mgmtFrameSessionId = 0xff;
        }
        else
        {
             pMac->lim.mgmtFrameSessionId = pMbMsg->sessionId;
             limLog( pMac, LOG2, FL("lim.actionFrameSessionId = %lu" ),
                     pMac->lim.mgmtFrameSessionId);

        }
    }

    return;
}


void limAbortRemainOnChan(tpAniSirGlobal pMac)
{
    if(VOS_TRUE == tx_timer_running(
                                &pMac->lim.limTimers.gLimRemainOnChannelTimer))
    {
        //                                                 
        limDeactivateAndChangeTimer(pMac, eLIM_REMAIN_CHN_TIMER);
        limProcessRemainOnChnTimeout(pMac);
    }
    return;
}

/*                              */
tSirRetStatus __limProcessSmeNoAUpdate(tpAniSirGlobal pMac, tANI_U32 *pMsgBuf)
{
    tpP2pPsConfig pNoA;
    tpP2pPsParams pMsgNoA;
    tSirMsgQ msg;

    pNoA = (tpP2pPsConfig) pMsgBuf;

    pMsgNoA = vos_mem_malloc(sizeof( tP2pPsConfig ));
    if (NULL == pMsgNoA)
    {
        limLog( pMac, LOGE,
                     FL( "Unable to allocate memory during NoA Update" ));
        return eSIR_MEM_ALLOC_FAILED;
    }

    vos_mem_set((tANI_U8 *)pMsgNoA, sizeof(tP2pPsConfig), 0);
    pMsgNoA->opp_ps = pNoA->opp_ps;
    pMsgNoA->ctWindow = pNoA->ctWindow;
    pMsgNoA->duration = pNoA->duration;
    pMsgNoA->interval = pNoA->interval;
    pMsgNoA->count = pNoA->count;
    pMsgNoA->single_noa_duration = pNoA->single_noa_duration;
    pMsgNoA->psSelection = pNoA->psSelection;

    msg.type = WDA_SET_P2P_GO_NOA_REQ;
    msg.reserved = 0;
    msg.bodyptr = pMsgNoA;
    msg.bodyval = 0;

    if(eSIR_SUCCESS != wdaPostCtrlMsg(pMac, &msg))
    {
        limLog(pMac, LOGE, FL("halPostMsgApi failed"));
        return eSIR_FAILURE;
    }

    return eSIR_SUCCESS;
} /*                                   */



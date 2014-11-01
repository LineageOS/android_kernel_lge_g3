/*
 * Copyright (c) 2012-2014 The Linux Foundation. All rights reserved.
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

#include "sirCommon.h"
#include "aniGlobal.h"

#include "schApi.h"
#include "limApi.h"
#include "limSendMessages.h"
#include "cfgApi.h"
#include "limSessionUtils.h"
#include "limFT.h"


#include "pmmApi.h"
#include "pmmDebug.h"
#include "sirApi.h"
#include "wmmApsd.h"

#include "limSendSmeRspMessages.h"
#include "limTimerUtils.h"
#include "limTrace.h"
#include "limUtils.h"
#include "VossWrapper.h"
#ifdef INTEGRATION_READY
#include "vos_status.h" //          
#include "vos_mq.h"     //                     
#endif

#include "wlan_qct_wda.h"

#define LIM_ADMIT_MASK_FLAG_ACBE 1
#define LIM_ADMIT_MASK_FLAG_ACBK 2
#define LIM_ADMIT_MASK_FLAG_ACVI 4
#define LIM_ADMIT_MASK_FLAG_ACVO 8

//                                                                     
/* 
                
  
            
                        
  
         
  
               
  
        
  
              
              
               
 */

tSirRetStatus
pmmInitialize(tpAniSirGlobal pMac)
{


    pmmResetStats(pMac);

    pMac->pmm.gPmmBeaconInterval = WNI_CFG_BEACON_INTERVAL_STADEF;
    pMac->pmm.gPmmState = ePMM_STATE_READY;



    pMac->pmm.inMissedBeaconScenario = FALSE;

    return eSIR_SUCCESS;
}

//                                                                     
/* 
                
  
            
                        
  
         
  
               
  
        
  
              
  
               
 */

void
pmmResetStats(void *pvMac)
{
    tpAniSirGlobal pMac = (tpAniSirGlobal)pvMac;

    pMac->pmm.BmpsmaxSleepTime = 0;
    pMac->pmm.BmpsavgSleepTime = 0;
    pMac->pmm.BmpsminSleepTime = 0;
    pMac->pmm.BmpscntSleep = 0;

    pMac->pmm.BmpsmaxTimeAwake = 0;
    pMac->pmm.BmpsavgTimeAwake = 0;
    pMac->pmm.BmpsminTimeAwake = 0;
    pMac->pmm.BmpscntAwake = 0;

    pMac->pmm.BmpsWakeupTimeStamp = 0;
    pMac->pmm.BmpsSleepTimeStamp = 0;

    pMac->pmm.BmpsHalReqFailCnt = 0;
    pMac->pmm.BmpsInitFailCnt = 0;
    pMac->pmm.BmpsInitFailCnt= 0;
    pMac->pmm.BmpsInvStateCnt= 0;
    pMac->pmm.BmpsPktDrpInSleepMode= 0;
    pMac->pmm.BmpsReqInInvalidRoleCnt= 0;
    pMac->pmm.BmpsSleeReqFailCnt= 0;
    pMac->pmm.BmpsWakeupIndCnt= 0;

    pMac->pmm.ImpsWakeupTimeStamp = 0;
    pMac->pmm.ImpsSleepTimeStamp = 0;
    pMac->pmm.ImpsMaxTimeAwake = 0;
    pMac->pmm.ImpsMinTimeAwake = 0;
    pMac->pmm.ImpsAvgTimeAwake = 0;
    pMac->pmm.ImpsCntAwake = 0;

    pMac->pmm.ImpsCntSleep = 0;
    pMac->pmm.ImpsMaxSleepTime = 0;
    pMac->pmm.ImpsMinSleepTime = 0;
    pMac->pmm.ImpsAvgSleepTime = 0;

    pMac->pmm.ImpsSleepErrCnt = 0;
    pMac->pmm.ImpsWakeupErrCnt = 0;
    pMac->pmm.ImpsLastErr = 0;
    pMac->pmm.ImpsInvalidStateCnt = 0;

    return;
}



//                                                                     
/* 
                             
  
            
                                                               
                                                                           
                                          
  
         
  
               
  
        
  
         
               
 */

void pmmInitBmpsResponseHandler(tpAniSirGlobal pMac, tpSirMsgQ limMsg )
{


    tPmmState nextState = pMac->pmm.gPmmState;
    tSirResultCodes retStatus = eSIR_SME_SUCCESS;
    tpPESession     psessionEntry;
    tpEnterBmpsParams pEnterBmpsParams;

    /*                                                            
                                                 
    */
    SET_LIM_PROCESS_DEFD_MESGS(pMac, true);

    if(pMac->pmm.gPmmState != ePMM_STATE_BMPS_WT_INIT_RSP)
    {
        PELOGE(pmmLog(pMac, LOGE,
            FL("pmmBmps: Received 'InitPwrSaveRsp' while in incorrect state: %d"),
            pMac->pmm.gPmmState);)

        retStatus = eSIR_SME_INVALID_PMM_STATE;
        pmmBmpsUpdateInvalidStateCnt(pMac);
        goto failure;
    }

    if (NULL == limMsg->bodyptr)
    {
        PELOGE(pmmLog(pMac, LOGE, FL("pmmBmps: Received SIR_HAL_ENTER_BMPS_RSP with NULL "));)
        goto failure;
    }
    pEnterBmpsParams = (tpEnterBmpsParams)(limMsg->bodyptr);

    //                                                                                       
    //                                                                               
    if(pEnterBmpsParams->status == eHAL_STATUS_SUCCESS)
    {
        PELOG2(pmmLog(pMac, LOG2,
            FL("pmmBmps: Received successful response from HAL to enter BMPS_POWER_SAVE "));)

        pMac->pmm.gPmmState = ePMM_STATE_BMPS_SLEEP;

        //                        
        pmmUpdatePwrSaveStats(pMac);

        //                             
        pMac->sys.gSysEnableScanMode = false;

        if (pMac->lim.gLimTimersCreated)
        {
            /*                                 */
            if(pMac->lim.limTimers.gLimHeartBeatTimer.pMac)
            {
                MTRACE(macTrace(pMac, TRACE_CODE_TIMER_DEACTIVATE, NO_SESSION, eLIM_HEART_BEAT_TIMER));
                tx_timer_deactivate(&pMac->lim.limTimers.gLimHeartBeatTimer);
            }
        }
        limSendSmeRsp(pMac, eWNI_PMC_ENTER_BMPS_RSP,  retStatus, 0, 0);
    }
    else
    {
        //                                                 
        PELOGE(pmmLog(pMac, LOGE,
            FL("pmmBmps: BMPS_INIT_PWR_SAVE_REQ failed, informing SME"));)

        pmmBmpsUpdateInitFailureCnt(pMac);
        nextState = ePMM_STATE_BMPS_WAKEUP;
        retStatus = eSIR_SME_BMPS_REQ_FAILED;
        goto failure;
    }
    return;

failure:
    psessionEntry = peGetValidPowerSaveSession(pMac);
    if(psessionEntry != NULL)
    {
       if (pMac->lim.gLimTimersCreated && pMac->lim.limTimers.gLimHeartBeatTimer.pMac)
       {
           if(VOS_TRUE != tx_timer_running(&pMac->lim.limTimers.gLimHeartBeatTimer))
           {
               PELOGE(pmmLog(pMac, LOGE, FL("Unexpected heartbeat timer not running"));)
               limReactivateHeartBeatTimer(pMac, psessionEntry);
           }
       }
    }

    //                                      
    pMac->pmm.gPmmState = nextState;
    pmmBmpsUpdateSleepReqFailureCnt(pMac);
    limSendSmeRsp(pMac, eWNI_PMC_ENTER_BMPS_RSP, retStatus, 0, 0);
    return;

}

//                                                                     
/* 
                            
  
            
                                                    
  
         
  
               
  
        
  
                                                

               
 */

void pmmExitBmpsRequestHandler(tpAniSirGlobal pMac, tpExitBmpsInfo pExitBmpsInfo)
{
    tSirResultCodes respStatus = eSIR_SME_SUCCESS;

    tPmmState origState = pMac->pmm.gPmmState;

    if (NULL == pExitBmpsInfo)
    {
        respStatus = eSIR_SME_BMPS_REQ_REJECT;
        PELOGW(pmmLog(pMac, LOGW, FL("pmmBmps: Rcvd EXIT_BMPS with NULL body"));)
        goto failure;
    }

#ifdef FEATURE_WLAN_DIAG_SUPPORT
    limDiagEventReport(pMac, WLAN_PE_DIAG_EXIT_BMPS_REQ_EVENT,
                       peGetValidPowerSaveSession(pMac), 0,
                       (tANI_U16)pExitBmpsInfo->exitBmpsReason);
#endif //                         

    /*                                                      
                                                                
                                                            
                                                                     
                                                                    
     */
    if(limIsSystemInScanState(pMac))
    {
        PELOGW(pmmLog(pMac, LOGW, 
            FL("pmmBmps: Device is already awake and scanning, returning success to PMC "));)
        limSendSmeRsp(pMac, eWNI_PMC_EXIT_BMPS_RSP, respStatus, 0, 0);
        return;
    }

#ifdef CUSTOMER_LGE_DEBUG_LOG
     PELOGW(pmmLog(pMac, LOGE, FL("Request ExitBMPS:: reason code = %d, PMM Status = %d "), pMac->pmm.gPmmExitBmpsReasonCode, pMac->pmm.gPmmState);)
#endif

    /*                                               */
    PELOGW(pmmLog(pMac, LOGW, FL("pmmBmps: Sending eWNI_PMC_EXIT_BMPS_REQ to HAL"));)
    if (pMac->pmm.gPmmState == ePMM_STATE_BMPS_SLEEP)
    {
        /*                                                           
                                                                   
         */
        pMac->pmm.gPmmExitBmpsReasonCode = pExitBmpsInfo->exitBmpsReason;
        vos_mem_free(pExitBmpsInfo);

        PELOGW(pmmLog(pMac, LOGW, 
            FL("pmmBmps: Rcvd EXIT_BMPS with reason code%d "), pMac->pmm.gPmmExitBmpsReasonCode);)


        //                                    
        pMac->pmm.gPmmState = ePMM_STATE_BMPS_WT_WAKEUP_RSP;
        if(pmmSendChangePowerSaveMsg(pMac) !=  eSIR_SUCCESS)
        {
            /*                       */
            respStatus = eSIR_SME_BMPS_REQ_REJECT;
            pmmBmpsUpdateHalReqFailureCnt(pMac);
            goto failure;
        }
        else
        {
            PELOG1(pmmLog(pMac, LOG1,
                          FL("pmmBmps: eWNI_PMC_EXIT_BMPS_REQ was successfully sent to HAL"));)
        }
    }
    else
    {
        PELOGE(pmmLog(pMac, LOGE, 
                      FL("pmmBmps: eWNI_PMC_EXIT_BMPS_REQ received in invalid state: %d"),
            pMac->pmm.gPmmState );)

        respStatus = eSIR_SME_INVALID_PMM_STATE;
        pmmBmpsUpdateInvalidStateCnt(pMac);
        vos_mem_free(pExitBmpsInfo);
        goto failure;
    }
    return;

failure:
    pMac->pmm.gPmmState = origState;
    pmmBmpsUpdateWakeupReqFailureCnt(pMac);
    limSendSmeRsp(pMac, eWNI_PMC_EXIT_BMPS_RSP, respStatus, 0, 0);
}


//                                                                     
/* 
                     
  
            
                                                             
                                                           
                                                     
                                                                   
          
  
         
  
               
  
        
  
                                                             
                                                

               
 */


void pmmInitBmpsPwrSave(tpAniSirGlobal pMac)
{
    tSirRetStatus   retStatus = eSIR_SUCCESS;
    tSirResultCodes respStatus = eSIR_SME_SUCCESS;
    tpPESession     psessionEntry;

    tPmmState origState = pMac->pmm.gPmmState;

    if((psessionEntry = peGetValidPowerSaveSession(pMac))== NULL)
    {
        respStatus = eSIR_SME_BMPS_REQ_REJECT;
        goto failure;
    }
#ifndef WLAN_ACTIVEMODE_OFFLOAD_FEATURE
    //                                                 
    if (limSendBeaconFilterInfo(pMac, psessionEntry) != eSIR_SUCCESS)
    {
        pmmLog(pMac, LOGE, FL("Fail to send Beacon Filter Info "));
    }
#else
    if(!IS_ACTIVEMODE_OFFLOAD_FEATURE_ENABLE)
    {
        if (limSendBeaconFilterInfo(pMac, psessionEntry) != eSIR_SUCCESS)
        {
            pmmLog(pMac, LOGE, FL("Fail to send Beacon Filter Info "));
        }
    }
#endif

#ifdef FEATURE_WLAN_DIAG_SUPPORT 
    limDiagEventReport(pMac, WLAN_PE_DIAG_ENTER_BMPS_REQ_EVENT, psessionEntry, 0, 0);
#endif //                         

    if ( ((pMac->pmm.gPmmState != ePMM_STATE_READY) &&
         (pMac->pmm.gPmmState != ePMM_STATE_BMPS_WAKEUP)) ||
         limIsSystemInScanState(pMac) ||
         limIsChanSwitchRunning(pMac) ||
         limIsInQuietDuration(pMac) )
    {
        PELOGE(pmmLog(pMac, LOGE, 
            FL("pmmBmps: BMPS Request received in invalid state PMM=%d, SME=%d, rejecting the initpwrsave request"),
            pMac->pmm.gPmmState, pMac->lim.gLimSmeState);)

        respStatus = eSIR_SME_INVALID_PMM_STATE;
        pmmBmpsUpdateInvalidStateCnt(pMac);
        goto failure;
    }

    //                                                                                                               
    if(pMac->pmm.inMissedBeaconScenario)
    {
       if (pMac->lim.gLimTimersCreated && pMac->lim.limTimers.gLimHeartBeatTimer.pMac)
       {
           if(VOS_TRUE != tx_timer_running(&pMac->lim.limTimers.gLimHeartBeatTimer))
           {
               PELOGE(pmmLog(pMac, LOGE, FL("Unexpected heartbeat timer not running"));)
               limReactivateHeartBeatTimer(pMac, psessionEntry);
           }
       }
       respStatus = eSIR_SME_BMPS_REQ_REJECT;
       goto failure;
    }

    /*                                                                         
                                                                           
                                                        
     */
     /*                                                                */
#if 0     
    if ( (pMac->sys.gSysEnableLinkMonitorMode) && (pMac->lim.limTimers.gLimHeartBeatTimer.pMac) )
    {
        if(VOS_TRUE != tx_timer_running(&pMac->lim.limTimers.gLimHeartBeatTimer)) 
        {
            PELOGE(pmmLog(pMac, LOGE, 
                FL("Reject BMPS_REQ because HeartBeatTimer is not running. "));)
            respStatus = eSIR_SME_BMPS_REQ_FAILED;
            goto failure;
        }
    }
#endif

    //                                                                             
    //                              

    //                                                                                      
    pMac->pmm.gPmmState = ePMM_STATE_BMPS_WT_INIT_RSP;
    if((retStatus = pmmSendInitPowerSaveMsg(pMac,psessionEntry)) != eSIR_SUCCESS)
    {
        PELOGE(pmmLog(pMac, LOGE, 
            FL("pmmBmps: Init Power Save Request Failed: Sending Response: %d"),
            retStatus);)

        respStatus = eSIR_SME_BMPS_REQ_REJECT;
        pmmBmpsUpdateHalReqFailureCnt(pMac);
        goto failure;
    }
    //                              
    pMac->pmm.sessionId = psessionEntry->peSessionId;
    return;

failure:

    //                                        
    pMac->pmm.gPmmState =origState;
    limSendSmeRsp(pMac, eWNI_PMC_ENTER_BMPS_RSP, respStatus, 0, 0);
    return;
}


/* 
                              
  
           
                                                                
                                                          
  
        
  
              
     
  
       
     
  
                                                
                                                     
 */
tSirRetStatus pmmSendChangePowerSaveMsg(tpAniSirGlobal pMac)
{
    tSirRetStatus  retStatus = eSIR_SUCCESS;
    tpExitBmpsParams  pExitBmpsParams;
    tSirMsgQ msgQ;
    tpPESession psessionEntry;
    tANI_U8  currentOperatingChannel = limGetCurrentOperatingChannel(pMac);

    pExitBmpsParams = vos_mem_malloc(sizeof(*pExitBmpsParams));
    if ( NULL == pExitBmpsParams )
    {
        pmmLog(pMac, LOGW, FL("Failed to allocate memory"));
        retStatus = eSIR_MEM_ALLOC_FAILED;
        return retStatus;
    }

    if((psessionEntry = peGetValidPowerSaveSession(pMac)) == NULL )
    {
        retStatus = eSIR_FAILURE;
        vos_mem_free(pExitBmpsParams);
        return retStatus;
    }

    vos_mem_set( (tANI_U8 *)pExitBmpsParams, sizeof(*pExitBmpsParams), 0);
    msgQ.type = WDA_EXIT_BMPS_REQ;
    msgQ.reserved = 0;
    msgQ.bodyptr = pExitBmpsParams;
    msgQ.bodyval = 0;

    /*                                                       
                                                                          
                                                           
                                         
                                                 
     */
    if ( !(SIR_IS_FULL_POWER_REASON_DISCONNECTED(pMac->pmm.gPmmExitBmpsReasonCode) ||
          ( (eSME_MISSED_BEACON_IND_RCVD == pMac->pmm.gPmmExitBmpsReasonCode) && 
             limIsconnectedOnDFSChannel(currentOperatingChannel))))
        pExitBmpsParams->sendDataNull = 1;

    pExitBmpsParams->bssIdx = psessionEntry->bssIdx;
   
    /*                                                
                                        
     */
    SET_LIM_PROCESS_DEFD_MESGS(pMac, false);
    MTRACE(macTraceMsgTx(pMac, NO_SESSION, msgQ.type));
    retStatus = wdaPostCtrlMsg( pMac, &msgQ);
    if( eSIR_SUCCESS != retStatus )
    {
        PELOGE(pmmLog( pMac, LOGE, FL("Sending WDA_EXIT_BMPS_REQ failed, reason=%X "), retStatus );)
        vos_mem_free(pExitBmpsParams);
        return retStatus;
    }

    PELOG1(pmmLog(pMac, LOG1, FL("WDA_EXIT_BMPS_REQ has been successfully sent to HAL"));)
    return retStatus;
}


/* 
                            
  
           
                                                                 
                                                                
                                 
  
        
  
              
     
  
       
     
  
                                                
                                             
  
                                                     
 */

tSirRetStatus  pmmSendInitPowerSaveMsg(tpAniSirGlobal pMac,tpPESession psessionEntry)
{
    tSirRetStatus   retCode = eSIR_SUCCESS;
    tSirMsgQ msgQ;
    tpEnterBmpsParams pBmpsParams = NULL;
    tANI_U32    rssiFilterPeriod = 5;
    tANI_U32    numBeaconPerRssiAverage = 20;
    tANI_U32    bRssiFilterEnable = FALSE;

    if(psessionEntry->currentBssBeaconCnt == 0 )
    {
        PELOGE(pmmLog( pMac, LOGE,  FL("Beacon count is zero, can not retrieve the TSF, failing the Enter Bmps Request"));)
        return eSIR_FAILURE;
    }

    pBmpsParams = vos_mem_malloc(sizeof(tEnterBmpsParams));
    if ( NULL == pBmpsParams )
    {
        pmmLog(pMac, LOGP, "PMM: Not able to allocate memory for Enter Bmps");
        return eSIR_FAILURE;
    }

    pMac->pmm.inMissedBeaconScenario = FALSE;
    pBmpsParams->respReqd = TRUE;

    pBmpsParams->tbtt = psessionEntry->lastBeaconTimeStamp;
    pBmpsParams->dtimCount = psessionEntry->lastBeaconDtimCount;
    pBmpsParams->dtimPeriod = psessionEntry->lastBeaconDtimPeriod;
    pBmpsParams->bssIdx = psessionEntry->bssIdx;

    /*                                                                
                                                                            
                                                                
                                                                   
                                                              
                                                                     */

    if(wlan_cfgGetInt(pMac, WNI_CFG_RSSI_FILTER_PERIOD, &rssiFilterPeriod) != eSIR_SUCCESS)
        pmmLog(pMac, LOGP, FL("pmmCfg: cfgGet failed for Rssi filter period"));
    pBmpsParams->rssiFilterPeriod = (tANI_U8)rssiFilterPeriod;

    if(wlan_cfgGetInt(pMac, WNI_CFG_PS_ENABLE_RSSI_MONITOR, &bRssiFilterEnable) != eSIR_SUCCESS)
        pmmLog(pMac, LOGP, FL("pmmCfg: cfgGet failed for Rssi monitor enable flag"));
    pBmpsParams->bRssiFilterEnable = bRssiFilterEnable;

    /*                                                
                                                  */
    if(wlan_cfgGetInt(pMac, WNI_CFG_NUM_BEACON_PER_RSSI_AVERAGE, &numBeaconPerRssiAverage) != eSIR_SUCCESS)
        pmmLog(pMac, LOGP, FL("pmmCfg: cfgGet failed for num beacon per rssi"));

    pBmpsParams->numBeaconPerRssiAverage =
            (tANI_U8)GET_MIN_VALUE((tANI_U8) numBeaconPerRssiAverage, WNI_CFG_NUM_BEACON_PER_RSSI_AVERAGE_STAMAX);

    pmmLog (pMac, LOG1,
        "%s: [INFOLOG]RssiFilterInfo..%d %x %x", __func__, (int)pBmpsParams->bRssiFilterEnable,
        (unsigned int)pBmpsParams->rssiFilterPeriod, (unsigned int)pBmpsParams->numBeaconPerRssiAverage);

    msgQ.type = WDA_ENTER_BMPS_REQ;
    msgQ.reserved = 0;
    msgQ.bodyptr = pBmpsParams;
    msgQ.bodyval = 0;

    PELOG2(pmmLog( pMac, LOG2,
        FL( "pmmBmps: Sending WDA_ENTER_BMPS_REQ" ));)

    /*                                                      
                                   
     */
    SET_LIM_PROCESS_DEFD_MESGS(pMac, false);

    MTRACE(macTraceMsgTx(pMac, psessionEntry->peSessionId, msgQ.type));
    if( eSIR_SUCCESS != (retCode = wdaPostCtrlMsg( pMac, &msgQ )))
    {
        vos_mem_free(pBmpsParams);
        PELOGE(pmmLog( pMac, LOGE,
            FL("Posting WDA_ENTER_BMPS_REQ to HAL failed, reason=%X"),
            retCode );)
    }

    return retCode;
}

/* 
                        
  
           
                                                           
  
       
  
                                                
                                             
  
                                                     
 */
tSirRetStatus pmmSendPowerSaveCfg(tpAniSirGlobal pMac, tpSirPowerSaveCfg pUpdatedPwrSaveCfg)
{
    tSirRetStatus   retCode = eSIR_SUCCESS;
    tSirMsgQ    msgQ;
    tANI_U32    listenInterval;
    tANI_U32    HeartBeatCount = 1;
    tANI_U32    maxPsPoll;
    tANI_U32    numBeaconPerRssiAverage;
    tANI_U32    minRssiThreshold;
    tANI_U32    nthBeaconFilter;
    tANI_U32    broadcastFrameFilter;
    tANI_U32    rssiFilterPeriod;
    tANI_U32    ignoreDtim;

    if (NULL == pUpdatedPwrSaveCfg)
        goto returnFailure;

    if(pMac->lim.gLimSmeState != eLIM_SME_IDLE_STATE  )
    {
        pmmLog(pMac, LOGE,
            FL("pmmCfg: Power Save Configuration received in invalid global sme state %d"),
            pMac->lim.gLimSmeState);
        retCode = eSIR_SME_INVALID_STATE;
        goto returnFailure;
    }

    //                                        
    if(wlan_cfgGetInt(pMac, WNI_CFG_LISTEN_INTERVAL, &listenInterval) != eSIR_SUCCESS)
        pmmLog(pMac, LOGP, FL("pmmCfg: cfgGet failed for listen interval"));
    pUpdatedPwrSaveCfg->listenInterval = (tANI_U16)listenInterval;

    if(wlan_cfgGetInt(pMac, WNI_CFG_HEART_BEAT_THRESHOLD, &HeartBeatCount) != eSIR_SUCCESS)
        pmmLog(pMac, LOGP, FL("pmmCfg: cfgGet failed for heart beat thresh"));

    pMac->lim.gLimHeartBeatCount = HeartBeatCount;
    pUpdatedPwrSaveCfg->HeartBeatCount = HeartBeatCount;

    if(wlan_cfgGetInt(pMac, WNI_CFG_NTH_BEACON_FILTER, &nthBeaconFilter) != eSIR_SUCCESS)
        pmmLog(pMac, LOGP, FL("pmmCfg: cfgGet failed for Nth beacon filter"));
    pUpdatedPwrSaveCfg->nthBeaconFilter = nthBeaconFilter;

    if(wlan_cfgGetInt(pMac, WNI_CFG_MAX_PS_POLL, &maxPsPoll) != eSIR_SUCCESS)
        pmmLog(pMac, LOGP, FL("pmmCfg: cfgGet failed for max poll"));
    pUpdatedPwrSaveCfg->maxPsPoll = maxPsPoll;

    if(wlan_cfgGetInt(pMac, WNI_CFG_MIN_RSSI_THRESHOLD, &minRssiThreshold) != eSIR_SUCCESS)
        pmmLog(pMac, LOGP, FL("pmmCfg: cfgGet failed for min RSSI Threshold"));
    pUpdatedPwrSaveCfg->minRssiThreshold = minRssiThreshold;

    if(wlan_cfgGetInt(pMac, WNI_CFG_NUM_BEACON_PER_RSSI_AVERAGE, &numBeaconPerRssiAverage) != eSIR_SUCCESS)
        pmmLog(pMac, LOGP, FL("pmmCfg: cfgGet failed for num beacon per rssi"));
    pUpdatedPwrSaveCfg->numBeaconPerRssiAverage = (tANI_U8) numBeaconPerRssiAverage;

    if(wlan_cfgGetInt(pMac, WNI_CFG_RSSI_FILTER_PERIOD, &rssiFilterPeriod) != eSIR_SUCCESS)
        pmmLog(pMac, LOGP, FL("pmmCfg: cfgGet failed for Rssi filter period"));
    pUpdatedPwrSaveCfg->rssiFilterPeriod = (tANI_U8) rssiFilterPeriod;

    if(wlan_cfgGetInt(pMac, WNI_CFG_BROADCAST_FRAME_FILTER_ENABLE, &broadcastFrameFilter) != eSIR_SUCCESS)
        pmmLog(pMac, LOGP, FL("pmmCfg: cfgGet failed for Nth beacon filter"));
    pUpdatedPwrSaveCfg->broadcastFrameFilter = (tANI_U8) broadcastFrameFilter;

    if(wlan_cfgGetInt(pMac, WNI_CFG_IGNORE_DTIM, &ignoreDtim) != eSIR_SUCCESS)
        pmmLog(pMac, LOGP, FL("pmmCfg: cfgGet failed for ignoreDtim"));
    pUpdatedPwrSaveCfg->ignoreDtim = (tANI_U8) ignoreDtim;

    //                                             
    vos_mem_copy( (tANI_U8 *) &pMac->pmm.gPmmCfg,  pUpdatedPwrSaveCfg, sizeof(tSirPowerSaveCfg));


    msgQ.type = WDA_PWR_SAVE_CFG;
    msgQ.reserved = 0;
    msgQ.bodyptr = pUpdatedPwrSaveCfg;
    msgQ.bodyval = 0;

    PELOG1(pmmLog( pMac, LOG1, FL( "pmmBmps: Sending WDA_PWR_SAVE_CFG to HAL"));)
    MTRACE(macTraceMsgTx(pMac, NO_SESSION, msgQ.type));
    if( eSIR_SUCCESS != (retCode = wdaPostCtrlMsg( pMac, &msgQ )))
    {
        pmmLog( pMac, LOGP,
            FL("Posting WDA_PWR_SAVE_CFG to HAL failed, reason=%X"),
            retCode );
        goto returnFailure;
    }
    return retCode;

returnFailure:

    /*                                                */
    if (NULL != pUpdatedPwrSaveCfg)
    {
        vos_mem_free(pUpdatedPwrSaveCfg);
    }
    return retCode;
}

/* 
                             
  
           
                                                                      
                                      
        
  
              
     
  
       
     
  
                                                
                                                                     
  
                
 */
void pmmExitBmpsResponseHandler(tpAniSirGlobal pMac,  tpSirMsgQ limMsg)
{
    tpExitBmpsParams  pExitBmpsRsp;
    eHalStatus  rspStatus;
    tANI_U8 PowersavesessionId;
    tpPESession psessionEntry;
    tSirResultCodes retStatus = eSIR_SME_SUCCESS;
    
    /*                                                     */
    PowersavesessionId = pMac->pmm.sessionId;

    /*                                                            
                                                
     */

    SET_LIM_PROCESS_DEFD_MESGS(pMac, true);

    if((psessionEntry = peFindSessionBySessionId(pMac,PowersavesessionId))==NULL)
    {
        pmmLog(pMac, LOGP,FL("Session Does not exist for given sessionID"));
        return;
    }

    

    /*                          */
    pmmUpdateWakeupStats(pMac);

    if (NULL == limMsg->bodyptr)
    {
        pmmLog(pMac, LOGE, FL("Received SIR_HAL_EXIT_BMPS_RSP with NULL "));
        return;
    }
    pExitBmpsRsp = (tpExitBmpsParams)(limMsg->bodyptr);
    rspStatus = pExitBmpsRsp->status;

    if(pMac->pmm.gPmmState != ePMM_STATE_BMPS_WT_WAKEUP_RSP)
    {
        PELOGE(pmmLog(pMac, LOGE,
            FL("Received SIR_HAL_EXIT_BMPS_RSP while in incorrect state: %d"),
            pMac->pmm.gPmmState);)

        retStatus = eSIR_SME_INVALID_PMM_STATE;
        pmmBmpsUpdateInvalidStateCnt(pMac);
    }
    else
    {
        PELOGW(pmmLog(pMac, LOGW, FL("Received SIR_HAL_EXIT_BMPS_RSP in correct state. "));)
    }

    /*                                              
                                                  
     */
    switch (rspStatus)
    {
        case eHAL_STATUS_SUCCESS:
            retStatus = eSIR_SME_SUCCESS;
            break;

        default:
            {
                /*                                                              
                                                                                     
                                                             
                 */
                retStatus = eSIR_SME_BMPS_REQ_FAILED;
            }
            break;

    }

    pMac->pmm.gPmmState = ePMM_STATE_BMPS_WAKEUP;
    pmmUpdateWakeupStats(pMac);

    //                        
    pMac->sys.gSysEnableScanMode = true;

    //                     
   if(IS_FEATURE_SUPPORTED_BY_FW(SLM_SESSIONIZATION) )
   {
       limSendSmeRsp(pMac, eWNI_PMC_EXIT_BMPS_RSP, retStatus, 
                  psessionEntry->smeSessionId, psessionEntry->transactionId);
   }
   else
   {
       limSendSmeRsp(pMac, eWNI_PMC_EXIT_BMPS_RSP, retStatus, 0, 0);
   }

    if ( pMac->pmm.gPmmExitBmpsReasonCode == eSME_MISSED_BEACON_IND_RCVD)
    {
        PELOGW(pmmLog(pMac, LOGW, FL("Rcvd SIR_HAL_EXIT_BMPS_RSP with MISSED_BEACON"));)
        pmmMissedBeaconHandler(pMac);
    }
    else if(pMac->pmm.inMissedBeaconScenario)
    {
        PELOGW(pmmLog(pMac, LOGW, FL("Rcvd SIR_HAL_EXIT_BMPS_RSP in missed beacon scenario but reason code not correct"));)
        pmmMissedBeaconHandler(pMac);
    }
    else
    {
        //                       
        limReactivateHeartBeatTimer(pMac, psessionEntry);
    }
    return;
}


/* 
                           
  
           
                                                                       
                                                       
  
       
                                                
               
 */
void pmmMissedBeaconHandler(tpAniSirGlobal pMac)
{
    tANI_U8 pwrSaveSessionId;
    tANI_U32 beaconInterval = 0;
    tANI_U32 heartBeatInterval = pMac->lim.gLimHeartBeatCount;
    tpPESession psessionEntry;
    
    /*                                                     */
    pwrSaveSessionId = pMac->pmm.sessionId;

    if((psessionEntry = peFindSessionBySessionId(pMac,pwrSaveSessionId))==NULL)
    {
        pmmLog(pMac, LOGE,FL("Session Does not exist for given sessionID"));
        return;
    }


    PELOGE(pmmLog(pMac, LOG1, FL("The device woke up due to MISSED BEACON "));)

    /*                                            */
    if((pMac->lim.limTimers.gLimHeartBeatTimer.pMac) &&
       (pMac->lim.gLimTimersCreated))
    {
        if (wlan_cfgGetInt(pMac, WNI_CFG_BEACON_INTERVAL, &beaconInterval) != eSIR_SUCCESS)
            PELOG1(pmmLog(pMac, LOG1, FL("Fail to get BEACON_INTERVAL value"));)

#ifdef CUSTOMER_LGE_DEBUG_LOG
        PELOGE(pmmLog(pMac, LOGE, FL("WLAN Wakeup by MISSED BEACON, HeartBeat Interval = %d "), beaconInterval);)
#endif

        /*                                         */
        heartBeatInterval= SYS_MS_TO_TICKS(beaconInterval * heartBeatInterval);

        if( tx_timer_change(&pMac->lim.limTimers.gLimHeartBeatTimer,
                            (tANI_U32)heartBeatInterval, 0) != TX_SUCCESS)
        {
            PELOG1(pmmLog(pMac, LOG1, FL("Fail to change HeartBeat timer"));)
        }

        /*                        */
        if(LIM_IS_CONNECTION_ACTIVE(psessionEntry))
        {
            if(psessionEntry->LimRxedBeaconCntDuringHB < MAX_NO_BEACONS_PER_HEART_BEAT_INTERVAL)
                pMac->lim.gLimHeartBeatBeaconStats[psessionEntry->LimRxedBeaconCntDuringHB]++;
            else
                pMac->lim.gLimHeartBeatBeaconStats[0]++;
        }

        /*                                                                               
                                                                                    
                                       
         */
        //                                                              
        limResetHBPktCount(psessionEntry); //                                                        
        pmmSendMessageToLim(pMac, SIR_LIM_HEART_BEAT_TIMEOUT);
    }
    else
    {
        PELOGE(pmmLog(pMac, LOGE, FL("HeartBeat Timer is not created, cannot re-activate"));)
    }

    return;
}


/* 
                               
  
           
                                                                  
                                                                                 
             
                                                                             
                                                                                          
                                                                   
                                                                                  
                                                                          
                                                                                           
                                                                                     
                                                                                                     
                                                                                                        
                            
        
  
              
     
  
       
     
  
                                                
                                                                     
  
                
 */

void pmmExitBmpsIndicationHandler(tpAniSirGlobal pMac, tANI_U8 mode, eHalStatus rspStatus)
{

    tANI_U32 beaconInterval = 0;
    tANI_U32 heartBeatInterval = pMac->lim.gLimHeartBeatCount;
    tANI_U8  powersavesessionId;
    tpPESession psessionEntry;

    /*                                                     */
    powersavesessionId = pMac->pmm.sessionId;

    psessionEntry = peFindSessionBySessionId(pMac,powersavesessionId);

    if(psessionEntry == NULL)
    {
      PELOGE(pmmLog(pMac, LOGE,
             FL("Session does Not exist with given sessionId :%d "),powersavesessionId);)
      return;
    }

    /*                                                                    
                                                                             
                                                          
     */

    PELOGW(pmmLog(pMac, LOGW, 
           FL("pmmBmps: Received SIR_HAL_EXIT_BMPS_IND from HAL, Exiting BMPS sleep mode")); )


    pMac->pmm.gPmmState = ePMM_STATE_BMPS_WAKEUP;
    pmmUpdateWakeupStats(pMac);

    /*                         */
    pMac->sys.gSysEnableScanMode = true;

    pmmBmpsUpdateWakeupIndCnt(pMac);

    /*                                         */
    limSendSmeRsp(pMac,
                  eWNI_PMC_EXIT_BMPS_IND,
                  eSIR_SME_SUCCESS, 0, 0);

    switch(rspStatus)
    {

        /*                                                                       
                                                                                  
                                                          
         */
        case eHAL_STATUS_HEARTBEAT_TMOUT:
            {
                PELOG1(pmmLog(pMac, LOG1,
                              FL("pmmBmps: The device woke up due to HeartBeat Timeout"));)

                /*                                            */
                if((pMac->lim.limTimers.gLimHeartBeatTimer.pMac) &&
                   (pMac->lim.gLimTimersCreated))
                {

                    /*                                            */
                    beaconInterval = psessionEntry->beaconParams.beaconInterval;

                    /*                                         */
                    heartBeatInterval= SYS_MS_TO_TICKS(beaconInterval * heartBeatInterval);

                    if(tx_timer_change(&pMac->lim.limTimers.gLimHeartBeatTimer,
                                       (tANI_U32)heartBeatInterval, 0) != TX_SUCCESS)
                    {
                        PELOG1(pmmLog(pMac, LOG1,
                               FL("pmmBmps: Unable to change HeartBeat timer"));)
                    }

                    /*                        */
                    if(LIM_IS_CONNECTION_ACTIVE(psessionEntry))
                    {
                        if(psessionEntry->LimRxedBeaconCntDuringHB < MAX_NO_BEACONS_PER_HEART_BEAT_INTERVAL)
                            pMac->lim.gLimHeartBeatBeaconStats[psessionEntry->LimRxedBeaconCntDuringHB]++;
                        else
                            pMac->lim.gLimHeartBeatBeaconStats[0]++;
                    }

                    /*                                                                           
                                                                                                
                                                   
                     */
                    pmmSendMessageToLim(pMac, SIR_LIM_HEART_BEAT_TIMEOUT);

                }
                else
                {

                    PELOGE(pmmLog(pMac, LOGE, 
                           FL("pmmBmps: HeartBeat Timer is not created, cannot re-activate"));)
                }
            }
            break;

        case eHAL_STATUS_NTH_BEACON_DELIVERY:
            break;

        default:
            break;

    }

    return;

}


//                                                                     
/* 
                    
  
                                                                  
  
         
  
               
  
        
  
              
               
 */

void pmmProcessMessage(tpAniSirGlobal pMac, tpSirMsgQ pMsg)
{
    switch (pMsg->type)
    {
        case eWNI_PMC_PWR_SAVE_CFG:
        {
            tpSirPowerSaveCfg pPSCfg;
            tSirMbMsg *pMbMsg = (tSirMbMsg *)pMsg->bodyptr;

            pPSCfg = vos_mem_malloc(sizeof(tSirPowerSaveCfg));
            if ( NULL == pPSCfg )
            {
                pmmLog(pMac, LOGP, "PMM: Not able to allocate memory for PMC Config");
            }
            (void) vos_mem_copy(pPSCfg, pMbMsg->data, sizeof(tSirPowerSaveCfg));
            (void) pmmSendPowerSaveCfg(pMac, pPSCfg);
        }
            break;

        case eWNI_PMC_ENTER_BMPS_REQ:
            pmmInitBmpsPwrSave(pMac);
            break;

        case WDA_ENTER_BMPS_RSP:
            pmmInitBmpsResponseHandler(pMac, pMsg);
            break;

        case eWNI_PMC_EXIT_BMPS_REQ:
        {
            tpExitBmpsInfo  pExitBmpsInfo;
            tSirMbMsg      *pMbMsg = (tSirMbMsg *)pMsg->bodyptr;

            pExitBmpsInfo = vos_mem_malloc(sizeof(tExitBmpsInfo));
            if ( NULL == pExitBmpsInfo )
            {
                pmmLog(pMac, LOGP, "PMM: Failed to allocate memory for Exit BMPS Info ");
            }
            (void) vos_mem_copy(pExitBmpsInfo, pMbMsg->data, sizeof(tExitBmpsInfo));
            (void) pmmExitBmpsRequestHandler(pMac, pExitBmpsInfo);
        }
            break;

        case WDA_EXIT_BMPS_RSP:
            pmmExitBmpsResponseHandler(pMac, pMsg);
            break;

        case WDA_EXIT_BMPS_IND:
            pmmExitBmpsIndicationHandler(pMac, SIR_PM_ACTIVE_MODE, (eHalStatus)pMsg->bodyval);
            break;

        case eWNI_PMC_ENTER_IMPS_REQ:
            pmmEnterImpsRequestHandler(pMac);
            break;

        case WDA_ENTER_IMPS_RSP:
            pmmEnterImpsResponseHandler(pMac, (eHalStatus)pMsg->bodyval);
            break;

        case eWNI_PMC_EXIT_IMPS_REQ:
            pmmExitImpsRequestHandler(pMac);
            break;

        case WDA_EXIT_IMPS_RSP:
            pmmExitImpsResponseHandler(pMac, (eHalStatus)pMsg->bodyval);
            break;

        case eWNI_PMC_ENTER_UAPSD_REQ:
            pmmEnterUapsdRequestHandler(pMac);
            break;

        case WDA_ENTER_UAPSD_RSP:
            pmmEnterUapsdResponseHandler(pMac, pMsg);
            break;

        case eWNI_PMC_EXIT_UAPSD_REQ:
            pmmExitUapsdRequestHandler(pMac);
            break;

        case WDA_EXIT_UAPSD_RSP:
            pmmExitUapsdResponseHandler(pMac, pMsg);
            break;

        case eWNI_PMC_WOWL_ADD_BCAST_PTRN:
            pmmSendWowlAddBcastPtrn(pMac, pMsg);
            break;

        case eWNI_PMC_WOWL_DEL_BCAST_PTRN:
            pmmSendWowlDelBcastPtrn(pMac, pMsg);
            break;

        case eWNI_PMC_ENTER_WOWL_REQ:
            pmmEnterWowlRequestHandler(pMac, pMsg);
            break;

        case WDA_WOWL_ENTER_RSP:
            pmmEnterWowlanResponseHandler(pMac, pMsg);
            break;

        case eWNI_PMC_EXIT_WOWL_REQ:
            pmmExitWowlanRequestHandler(pMac);
            break;

        case WDA_WOWL_EXIT_RSP:
            pmmExitWowlanResponseHandler(pMac, pMsg);
            break;
#ifdef WLAN_FEATURE_PACKET_FILTERING
        case WDA_PACKET_COALESCING_FILTER_MATCH_COUNT_RSP:
            pmmFilterMatchCountResponseHandler(pMac, pMsg);
            break;
#endif //                              


#ifdef WLAN_FEATURE_GTK_OFFLOAD
        case WDA_GTK_OFFLOAD_GETINFO_RSP:
            pmmGTKOffloadGetInfoResponseHandler(pMac, pMsg);
            break;
#endif //                         

        default:
            PELOGW(pmmLog(pMac, LOGW, 
                FL("PMM: Unknown message in pmmMsgQ type %d, potential memory leak!!"),
                pMsg->type);)
    }

    if (NULL != pMsg->bodyptr)
    {
        vos_mem_free(pMsg->bodyptr);
        pMsg->bodyptr = NULL;
    }
}






//                                                                     
/* 
                 
  
            
                                          
  
         
  
               
  
        
  
                                 
               
 */

tSirRetStatus
pmmPostMessage(tpAniSirGlobal pMac, tpSirMsgQ pMsg)
{
    VOS_STATUS vosStatus;
    vosStatus = vos_mq_post_message(VOS_MQ_ID_PE, (vos_msg_t *) pMsg);
    if(!VOS_IS_STATUS_SUCCESS(vosStatus))
    {
        pmmLog(pMac, LOGP, FL("vos_mq_post_message failed with status code %d"), vosStatus);
        return eSIR_FAILURE;
    }

    return eSIR_SUCCESS;
}





/* 
                        
  
                                                                              
  
         
  
               
  
        
  
                                               
               
 */

void pmmUpdatePwrSaveStats(tpAniSirGlobal pMac)
{
/*
                           

                                                                

                                      
                                                                                       
        
                                         

                                               
     
                                               
     

                                                                                  
     
                                               
     

                                                                                                                                          

                             
           
*/
}




/* 
                        
  
                                                            
  
         
  
               
  
        
  
                                               
               
 */

void pmmUpdateWakeupStats(tpAniSirGlobal pMac)
{
/*

                               

                                                                     
                                                                                        

                                                   
         
                                                   
         

                                                                                      
         
                                                   
         

                                                                                                                                              

                                 
               
*/
}

//                                                                     
/* 
                             
  
            
                                                                        
                                                             
  
         
  
               
       
  
        
  
                                
                
 */
void pmmEnterImpsRequestHandler (tpAniSirGlobal pMac)
{

    tSirResultCodes resultCode = eSIR_SME_SUCCESS;
    tSirRetStatus   retStatus = eSIR_SUCCESS;
    tPmmState       origState = pMac->pmm.gPmmState;

#ifdef FEATURE_WLAN_DIAG_SUPPORT 
    limDiagEventReport(pMac, WLAN_PE_DIAG_ENTER_IMPS_REQ_EVENT, peGetValidPowerSaveSession(pMac), 0, 0);
#endif //                         

    /*                                                */
    if(peIsAnySessionActive(pMac))
    {
        /*                                                      
                           
         */
        if (!(pMac->pmc.ImpsReqFailCnt & 0xF))
        {
            pePrintActiveSession(pMac);
            macTraceDumpAll(pMac,0,0,100);
        }
        resultCode = eSIR_SME_INVALID_STATE;
        pmmLog(pMac, LOGE, FL("Session is active go to failure resultCode = "
               "eSIR_SME_INVALID_STATE (%d)"),resultCode);
        goto failure;
    }

    if ( ((pMac->pmm.gPmmState != ePMM_STATE_READY) &&
          (pMac->pmm.gPmmState != ePMM_STATE_IMPS_WAKEUP)) ||
         ((pMac->lim.gLimSmeState != eLIM_SME_IDLE_STATE) &&
          (pMac->lim.gLimSmeState != eLIM_SME_JOIN_FAILURE_STATE)) ||
         (pMac->lim.gLimMlmState != eLIM_MLM_IDLE_STATE) ||
         limIsChanSwitchRunning (pMac) ||
         limIsInQuietDuration (pMac) )
    {
        PELOGE(pmmLog(pMac, LOGE, 
              FL("pmmImps: PMM State = %d, Global MLM State = %d, Global SME State = %d, rejecting the sleep mode request"),
              pMac->pmm.gPmmState, pMac->lim.gLimMlmState, pMac->lim.gLimSmeState);)

        resultCode = eSIR_SME_INVALID_PMM_STATE;
        pmmImpsUpdateErrStateStats(pMac);
        goto failure;
    }

    //                                            
    pMac->pmm.gPmmState = ePMM_STATE_IMPS_WT_SLEEP_RSP;
    if( (retStatus = pmmImpsSendChangePwrSaveMsg(pMac, SIR_PM_SLEEP_MODE)) != eSIR_SUCCESS)
    {
        PELOGE(pmmLog(pMac, LOGE, 
               FL("pmmImps: IMPS Sleep Request failed: sending response: %x"), retStatus);)

        resultCode = eSIR_SME_IMPS_REQ_FAILED;
        goto failure;
    }
    else
    {
        PELOG1(pmmLog(pMac, LOG1,
               FL("pmmImps: Waiting for SoftMac response for IMPS request"));)
    }
    return;

failure:
    pMac->pmm.gPmmState = origState;
    pmmImpsUpdateSleepErrStats(pMac, retStatus);

    limSendSmeRsp(pMac,
                  eWNI_PMC_ENTER_IMPS_RSP,
                  resultCode, 0, 0);

}

//                                                                     
/* 
                              
  
            
                                                                       
                                                                         
                                                                       
                                                         
  
         
  
               
       
  
        
  
                                             
                
 */
void pmmEnterImpsResponseHandler (tpAniSirGlobal pMac, eHalStatus rspStatus)
{
    tPmmState nextState = pMac->pmm.gPmmState;
    tSirResultCodes resultCode = eSIR_SME_SUCCESS;

    /*                                                            
                                             
     */
    SET_LIM_PROCESS_DEFD_MESGS(pMac, true);

    if(pMac->pmm.gPmmState != ePMM_STATE_IMPS_WT_SLEEP_RSP)
    {
        PELOGE(pmmLog(pMac, LOGE, 
               FL("pmmImps: Receives IMPS sleep rsp in invalid state: %d"),
               pMac->pmm.gPmmState);)

        resultCode = eSIR_SME_INVALID_PMM_STATE;
        pmmImpsUpdateErrStateStats(pMac);

        goto failure;
    }

    if(eHAL_STATUS_SUCCESS == rspStatus)
    {
        //                                               
        pMac->pmm.gPmmState = ePMM_STATE_IMPS_SLEEP;

        PELOG2(pmmLog(pMac, LOG2,
            FL("pmmImps: Received successful WDA_ENTER_IMPS_RSP from HAL"));)

        //                            
        pmmImpsUpdatePwrSaveStats(pMac);

        limSendSmeRsp(pMac,
                      eWNI_PMC_ENTER_IMPS_RSP,
                      resultCode, 0, 0);
    }
    else
    {
        //                                            
        nextState = ePMM_STATE_IMPS_WAKEUP;
        resultCode = eSIR_SME_CANNOT_ENTER_IMPS;
        goto failure;
    }
    return;

failure:
    PELOGE(pmmLog(pMac, LOGE, 
           FL("pmmImpsSleepRsp failed, Ret Code: %d, next state will be: %d"),
           rspStatus,
           pMac->pmm.gPmmState);)

    pMac->pmm.gPmmState = nextState;

    limSendSmeRsp(pMac,
                  eWNI_PMC_ENTER_IMPS_RSP,
                  resultCode, 0, 0);
}


//                                                                     
/* 
                            
  
            
                                                                           
                                                              
  
         
  
               
       
  
        
  
                                
                
 */
void pmmExitImpsRequestHandler (tpAniSirGlobal pMac)
{
    tSirRetStatus retStatus = eSIR_SUCCESS;
    tSirResultCodes resultCode = eSIR_SME_SUCCESS;

    tPmmState origState = pMac->pmm.gPmmState;

#ifdef FEATURE_WLAN_DIAG_SUPPORT 
    limDiagEventReport(pMac, WLAN_PE_DIAG_EXIT_IMPS_REQ_EVENT, peGetValidPowerSaveSession(pMac), 0, 0);
#endif //                         

    if (ePMM_STATE_IMPS_SLEEP == pMac->pmm.gPmmState)
    {
        pMac->pmm.gPmmState = ePMM_STATE_IMPS_WT_WAKEUP_RSP;
        if( (retStatus = pmmImpsSendChangePwrSaveMsg(pMac, SIR_PM_ACTIVE_MODE)) !=
            eSIR_SUCCESS)
        {
            PELOGE(pmmLog(pMac, LOGE, 
                   FL("pmmImps: Wakeup request message sent to SoftMac failed"));)
            resultCode = eSIR_SME_IMPS_REQ_FAILED;
            goto failure;
        }
    }
    else
    {
        //                     
        PELOGE(pmmLog(pMac, LOGE, 
                      FL("pmmImps: Wakeup Req received in invalid state: %x"),
                      pMac->pmm.gPmmState);)

        resultCode = eSIR_SME_INVALID_PMM_STATE;
        pmmImpsUpdateErrStateStats(pMac);

        goto failure;
    }
    return;

failure:
    PELOGE(pmmLog (pMac, LOGE, 
                   FL("pmmImps: Changing to IMPS wakeup mode failed, Ret Code: %d, Next State: %d"),
                   retStatus, pMac->pmm.gPmmState);)

    pMac->pmm.gPmmState = origState;
    pmmImpsUpdateWakeupErrStats(pMac, retStatus);

    limSendSmeRsp(pMac,
                  eWNI_PMC_EXIT_IMPS_RSP,
                  resultCode, 0, 0);
}


//                                                                     
/* 
                             
  
            
                                                                       
                                                                         
                                                                       
                                                         
  
         
  
               
       
  
        
  
                               
               
 */
void pmmExitImpsResponseHandler(tpAniSirGlobal pMac, eHalStatus rspStatus)
{
    tSirResultCodes resultCode = eSIR_SME_SUCCESS;

    /*                                                            
                                            
     */
    SET_LIM_PROCESS_DEFD_MESGS(pMac, true);

    if (pMac->pmm.gPmmState != ePMM_STATE_IMPS_WT_WAKEUP_RSP)
    {
        PELOGE(pmmLog(pMac, LOGE, 
                      FL("pmmImps: Received 'Wakeup' response in invalid state: %d"),
                      pMac->pmm.gPmmState);)

        resultCode = eSIR_SME_INVALID_PMM_STATE;
        pmmImpsUpdateErrStateStats(pMac);
    }

    switch(rspStatus)
    {
    case eHAL_STATUS_SUCCESS:
        {
            resultCode = eSIR_SME_SUCCESS;
            PELOG2(pmmLog(pMac, LOG2, 
                          FL("pmmImps: Received WDA_EXIT_IMPS_RSP with Successful response from HAL"));)
        }
        break;

        default:
            {
                resultCode = eSIR_SME_IMPS_REQ_FAILED;
                PELOGW(pmmLog(pMac, LOGW, 
                              FL("pmmImps: Received WDA_EXIT_IMPS_RSP with Failure Status from HAL"));)
            }
            break;

    }

    pMac->pmm.gPmmState = ePMM_STATE_IMPS_WAKEUP;

    //                            
    pmmImpsUpdateWakeupStats(pMac);

    limSendSmeRsp(pMac,
                  eWNI_PMC_EXIT_IMPS_RSP,
                  resultCode, 0, 0);
    return;

}

//                                                                     
/* 
                              
  
            
                                                               
                                                                
                                                                  
                                                 
                              
  
        
  
                                    
                    
 */
void pmmEnterUapsdRequestHandler (tpAniSirGlobal pMac)
{
    tSirResultCodes resultCode = eSIR_SME_SUCCESS;
    tSirRetStatus   retStatus = eSIR_SUCCESS;

    tPmmState origState = pMac->pmm.gPmmState;

#ifdef FEATURE_WLAN_DIAG_SUPPORT 
    limDiagEventReport(pMac, WLAN_PE_DIAG_ENTER_UAPSD_REQ_EVENT, peGetValidPowerSaveSession(pMac), 0, 0);
#endif //                         

    if ( (pMac->pmm.gPmmState != ePMM_STATE_BMPS_SLEEP) ||
         limIsSystemInScanState(pMac) )
    {
        PELOGE(pmmLog(pMac, LOGE,
            FL("pmmUapsd: PMM State = %d, Global MLM State = %d, Global SME State = %d, rejecting the sleep mode request"),
            pMac->pmm.gPmmState, pMac->lim.gLimMlmState, pMac->lim.gLimSmeState);)

        resultCode = eSIR_SME_INVALID_PMM_STATE;
        goto failure;
    }

    pMac->pmm.gPmmState = ePMM_STATE_UAPSD_WT_SLEEP_RSP;

    if( (retStatus = pmmUapsdSendChangePwrSaveMsg(pMac, SIR_PM_SLEEP_MODE)) != eSIR_SUCCESS)
    {
        PELOGE(pmmLog(pMac, LOGE,
            FL("pmmUapsd: HAL_ENTER_UAPSD_REQ failed with response: %x"), retStatus);)
        resultCode = eSIR_SME_UAPSD_REQ_FAILED;
        goto failure;
    }

    PELOG1(pmmLog(pMac, LOG1, FL("pmmUapsd: Waiting for WDA_ENTER_UAPSD_RSP "));)
    return;

failure:
    pMac->pmm.gPmmState = origState;
    limSendSmeRsp(pMac, eWNI_PMC_ENTER_UAPSD_RSP, resultCode, 0, 0);
    return;
}


//                                                                     
/* 
                               
  
            
                                                                
                                                                         
                                                
  
        
  
                 
               
 */
void pmmEnterUapsdResponseHandler(tpAniSirGlobal pMac, tpSirMsgQ limMsg)
{
    tpUapsdParams    pUapsdRspMsg;
    tSirResultCodes  retStatus = eSIR_SME_SUCCESS;

    tANI_U8 PowersavesessionId;
    tpPESession psessionEntry;

    /*                                                            
                                                  
     */
    SET_LIM_PROCESS_DEFD_MESGS(pMac, true);

    /*                                                     */
    PowersavesessionId = pMac->pmm.sessionId;

    if (NULL == limMsg->bodyptr)
    {
        PELOGE(pmmLog(pMac, LOGE, FL("pmmUapsd: Received SIR_HAL_ENTER_UAPSD_RSP with NULL "));)
        return;
    }

    pUapsdRspMsg = (tpUapsdParams)(limMsg->bodyptr);

    if((psessionEntry = peFindSessionBySessionId(pMac,PowersavesessionId))==NULL)
    {
        pmmLog(pMac, LOGP,FL("Session Does not exist for given sessionID"));
        return;
    }

    if(pMac->pmm.gPmmState != ePMM_STATE_UAPSD_WT_SLEEP_RSP)
    {
        PELOGE(pmmLog(pMac, LOGE,
            FL("pmmUapsd: Received SIR_HAL_ENTER_UAPSD_RSP while in incorrect state: %d"),
            pMac->pmm.gPmmState);)
        limSendSmeRsp(pMac, eWNI_PMC_ENTER_UAPSD_RSP, eSIR_SME_INVALID_PMM_STATE, 0, 0);        
        return;
    }

    if(pUapsdRspMsg->status == eHAL_STATUS_SUCCESS)
    {
        PELOGW(pmmLog(pMac, LOGW,
            FL("pmmUapsd: Received successful response from HAL to enter UAPSD mode "));)
        pMac->pmm.gPmmState = ePMM_STATE_UAPSD_SLEEP;
    }
    else
    {
        PELOGE(pmmLog(pMac, LOGE,
            FL("pmmUapsd: SIR_HAL_ENTER_UAPSD_RSP failed, informing SME"));)
        pMac->pmm.gPmmState = ePMM_STATE_BMPS_SLEEP;
        retStatus = eSIR_SME_UAPSD_REQ_FAILED;
    }

    if(IS_FEATURE_SUPPORTED_BY_FW(SLM_SESSIONIZATION))
    {
        limSendSmeRsp(pMac, eWNI_PMC_ENTER_UAPSD_RSP, retStatus, 
                        psessionEntry->smeSessionId, psessionEntry->transactionId);
    }
    else
    {
        limSendSmeRsp(pMac, eWNI_PMC_ENTER_UAPSD_RSP, retStatus, 0, 0);
    }

    return;
}


//                                                                     
/* 
                             
  
            
                                                              
                                                                
                                                           
                                                 
                                 
  
        
  
                                    
                    
 */
void pmmExitUapsdRequestHandler(tpAniSirGlobal pMac)
{
    tSirRetStatus retStatus = eSIR_SUCCESS;
    tSirResultCodes resultCode = eSIR_SME_SUCCESS;

    tPmmState origState = pMac->pmm.gPmmState;

#ifdef FEATURE_WLAN_DIAG_SUPPORT 
    limDiagEventReport(pMac, WLAN_PE_DIAG_EXIT_UAPSD_REQ_EVENT, peGetValidPowerSaveSession(pMac), 0, 0);
#endif //                         

    if (ePMM_STATE_UAPSD_SLEEP == pMac->pmm.gPmmState)
    {
        pMac->pmm.gPmmState = ePMM_STATE_UAPSD_WT_WAKEUP_RSP;
        if( (retStatus = pmmUapsdSendChangePwrSaveMsg(pMac, SIR_PM_ACTIVE_MODE)) !=
                                                eSIR_SUCCESS)
        {
            PELOGE(pmmLog(pMac, LOGE,
                FL("pmmUapsd: sending EXIT_UAPSD to HAL failed "));)
            resultCode = eSIR_SME_UAPSD_REQ_FAILED;
            goto failure;
        }
    }
    else
    {
        PELOGE(pmmLog(pMac, LOGE,
            FL("pmmUapsd: Rcv EXIT_UAPSD from PMC in invalid state: %x"),
            pMac->pmm.gPmmState);)

        resultCode = eSIR_SME_INVALID_PMM_STATE;
        goto failure;
    }
    return;

failure:
    pMac->pmm.gPmmState = origState;
    PELOGE(pmmLog(pMac, LOGE,
        FL("pmmUapsd: Waking up from UAPSD mode failed, Ret Code: %d, Next State: %d"),
        retStatus, pMac->pmm.gPmmState);)
    limSendSmeRsp(pMac, eWNI_PMC_EXIT_UAPSD_RSP, resultCode, 0, 0);
}


//                                                                     
/* 
                              
  
            
                                                                    
                                                               
                                                                
                                           
  
        
  
                                    
                    
 */
void pmmExitUapsdResponseHandler(tpAniSirGlobal pMac,  tpSirMsgQ limMsg)
{
    tSirResultCodes resultCode = eSIR_SME_SUCCESS;
    tANI_U8 PowersavesessionId;
    tpPESession psessionEntry;
    tUapsdParams  *pUapsdExitRspParams;

    /*                                                            
                                                 
     */
    SET_LIM_PROCESS_DEFD_MESGS(pMac, true);

    if (pMac->pmm.gPmmState != ePMM_STATE_UAPSD_WT_WAKEUP_RSP)
    {
        PELOGE(pmmLog(pMac, LOGE,
            FL("Received HAL_EXIT_UAPSD_RSP in invalid state: %d"),
            pMac->pmm.gPmmState);)
        limSendSmeRsp(pMac, eWNI_PMC_EXIT_UAPSD_RSP, eSIR_SME_INVALID_PMM_STATE, 0, 0);
        return;
    }
    pUapsdExitRspParams = (tUapsdParams *)(limMsg->bodyptr);

    PowersavesessionId = pMac->pmm.sessionId;
    if((psessionEntry = peFindSessionBySessionId(pMac,PowersavesessionId))==NULL)
    {
        pmmLog(pMac, LOGP,FL("Session Does not exist for given sessionID"));
        return;
    }

    if(NULL == pUapsdExitRspParams )
    {
        PELOGE(pmmLog(pMac, LOGE,
            FL("Received HAL_EXIT_UAPSD_RSP message with zero parameters:"));)
            limSendSmeRsp(pMac, eWNI_PMC_EXIT_UAPSD_RSP, eSIR_SME_UAPSD_REQ_FAILED, 0, 0);
        return;
    }
    switch(pUapsdExitRspParams->status)
    {
        case eHAL_STATUS_SUCCESS:
            resultCode = eSIR_SME_SUCCESS;
            PELOGW(pmmLog(pMac, LOGW,
                FL("Received SIR_HAL_EXIT_UAPSD_RSP with Successful response "));)
            break;
        default:
            resultCode = eSIR_SME_UAPSD_REQ_FAILED;
            PELOGE(pmmLog(pMac, LOGW,
                FL("Received SIR_HAL_EXIT_UAPSD_RSP with Failure Status"));)
            break;
    }

    pMac->pmm.gPmmState = ePMM_STATE_BMPS_SLEEP;

    if(IS_FEATURE_SUPPORTED_BY_FW(SLM_SESSIONIZATION))
    {
        limSendSmeRsp(pMac, eWNI_PMC_EXIT_UAPSD_RSP, resultCode, psessionEntry->smeSessionId,
                      psessionEntry->transactionId);
    }
    else
    {
        limSendSmeRsp(pMac, eWNI_PMC_EXIT_UAPSD_RSP, resultCode, 0, 0);
    }
    return;
}

/*                                                              
                                
                                                          
                        
                             
                             
             
                                                                */
void pmmSendWowlAddBcastPtrn(tpAniSirGlobal pMac,  tpSirMsgQ pMsg)
{
    tpSirWowlAddBcastPtrn  pBcastPtrn;
    tSirMbMsg              *pMbMsg = (tSirMbMsg *)pMsg->bodyptr;
    tSirRetStatus          retCode = eSIR_SUCCESS;
    tSirMsgQ               msgQ;

    pBcastPtrn = vos_mem_malloc(sizeof(*pBcastPtrn));
    if ( NULL == pBcastPtrn )
    {
        pmmLog(pMac, LOGP, FL("Fail to allocate memory for WoWLAN Add Bcast Pattern "));
        return;
    }
    (void) vos_mem_copy(pBcastPtrn, pMbMsg->data, sizeof(*pBcastPtrn));

    if (NULL == pBcastPtrn)
    {
        pmmLog(pMac, LOGE, FL("Add broadcast pattern message is NULL "));
        return;
    }

    msgQ.type = WDA_WOWL_ADD_BCAST_PTRN;
    msgQ.reserved = 0;
    msgQ.bodyptr = pBcastPtrn;
    msgQ.bodyval = 0;

    pmmLog(pMac, LOG1, FL( "Sending WDA_WOWL_ADD_BCAST_PTRN to HAL"));
#ifdef FEATURE_WLAN_DIAG_SUPPORT 
    limDiagEventReport(pMac, WLAN_PE_DIAG_WOWL_ADD_BCAST_PTRN_EVENT, NULL, 0, 0);
#endif //                         

    if( eSIR_SUCCESS != (retCode = wdaPostCtrlMsg( pMac, &msgQ )))
    {
        if (pBcastPtrn != NULL)
            vos_mem_free(pBcastPtrn);
        pmmLog( pMac, LOGP, FL("Posting WDA_WOWL_ADD_BCAST_PTRN failed, reason=%X"), retCode );
    }
    return;
}

/*                                                              
                                
                                                          
                        
                             
                             
             
                                                                */
void pmmSendWowlDelBcastPtrn(tpAniSirGlobal pMac, tpSirMsgQ pMsg)
{
    tpSirWowlDelBcastPtrn   pDeletePtrn;
    tSirMbMsg               *pMbMsg = (tSirMbMsg *)pMsg->bodyptr;
    tSirRetStatus           retCode = eSIR_SUCCESS;
    tSirMsgQ                msgQ;

    pDeletePtrn = vos_mem_malloc(sizeof(*pDeletePtrn));
    if ( NULL == pDeletePtrn )
    {
        pmmLog(pMac, LOGP, FL("Fail to allocate memory for WoWLAN Delete Bcast Pattern "));
        return;
    }
    (void) vos_mem_copy(pDeletePtrn, pMbMsg->data, sizeof(*pDeletePtrn));

    if (NULL == pDeletePtrn)
    {
        pmmLog(pMac, LOGE, FL("Delete broadcast pattern message is NULL "));
        return;
    }

    msgQ.type = WDA_WOWL_DEL_BCAST_PTRN;
    msgQ.reserved = 0;
    msgQ.bodyptr = pDeletePtrn;
    msgQ.bodyval = 0;

    pmmLog(pMac, LOG1, FL( "Sending WDA_WOWL_DEL_BCAST_PTRN"));
#ifdef FEATURE_WLAN_DIAG_SUPPORT 
    limDiagEventReport(pMac, WLAN_PE_DIAG_WOWL_DEL_BCAST_PTRN_EVENT, NULL, 0, 0);
#endif //                         

    if( eSIR_SUCCESS != (retCode = wdaPostCtrlMsg( pMac, &msgQ )))
    {
        if (NULL != pDeletePtrn)
            vos_mem_free(pDeletePtrn);
        pmmLog( pMac, LOGP, FL("Posting WDA_WOWL_DEL_BCAST_PTRN failed, reason=%X"), retCode );
    }
    return;
}

/*                                                           
                                   
                                                             
                                                  
                                           
                             
                             
             
                                                             */
void pmmEnterWowlRequestHandler(tpAniSirGlobal pMac, tpSirMsgQ pMsg)
{
    tpSirSmeWowlEnterParams  pSmeWowlParams = NULL;
    tpSirHalWowlEnterParams  pHalWowlParams = NULL;
    tSirRetStatus  retCode = eSIR_SUCCESS;
    tANI_U32  cfgValue = 0;
    tSirMbMsg *pMbMsg = (tSirMbMsg *)pMsg->bodyptr;
    tpPESession pSessionEntry = NULL;
    tANI_U8  peSessionId = 0;

#ifdef FEATURE_WLAN_DIAG_SUPPORT 
    limDiagEventReport(pMac, WLAN_PE_DIAG_ENTER_WOWL_REQ_EVENT, NULL, 0, 0);
#endif //                         

    pSmeWowlParams = (tpSirSmeWowlEnterParams)(pMbMsg->data);
    if (NULL == pSmeWowlParams)
    {
        pmmLog(pMac, LOGE,
               FL("NULL message received"));
        return;
    }

    pSessionEntry = peFindSessionByBssid(pMac, pSmeWowlParams->bssId,
                                         &peSessionId);
    if (NULL == pSessionEntry)
    {
        pmmLog(pMac, LOGE,
               FL("session does not exist for given BSSId"));
        goto end;
    }
    pMac->pmm.sessionId = peSessionId;

//                          
#if 0
    if (pMac->lim.gLimSmeState != eLIM_SME_LINK_EST_STATE)
    {
        pmmLog(pMac, LOGE, FL("Rcvd PMC_ENTER_WOWL_REQ when station is not associated "));
        limSendSmeRsp(pMac, eWNI_PMC_ENTER_WOWL_RSP, eSIR_SME_STA_NOT_ASSOCIATED, 0, 0);
        goto end;
    }
#endif


    if ((pMac->pmm.gPmmState != ePMM_STATE_BMPS_SLEEP) && (pMac->pmm.gPmmState != ePMM_STATE_WOWLAN))
    {
        pmmLog(pMac, LOGE, FL("Rcvd PMC_ENTER_WOWL_REQ in invalid Power Save state "));
        limSendSmeRsp(pMac, eWNI_PMC_ENTER_WOWL_RSP, eSIR_SME_INVALID_PMM_STATE, 0, 0);
        goto end;
    }

    pHalWowlParams = vos_mem_malloc(sizeof(*pHalWowlParams));
    if ( NULL == pHalWowlParams )
    {
        pmmLog(pMac, LOGP, FL("Fail to allocate memory for Enter Wowl Request "));
        goto end;
    }
    (void) vos_mem_set((tANI_U8 *)pHalWowlParams, sizeof(*pHalWowlParams), 0);

    //                          
    pHalWowlParams->ucMagicPktEnable = pSmeWowlParams->ucMagicPktEnable;
    pHalWowlParams->ucPatternFilteringEnable = pSmeWowlParams->ucPatternFilteringEnable;
    (void)vos_mem_copy( (tANI_U8 *)pHalWowlParams->magicPtrn, (tANI_U8 *)pSmeWowlParams->magicPtrn,
                         sizeof(tSirMacAddr) );

#ifdef WLAN_WAKEUP_EVENTS
    pHalWowlParams->ucWoWEAPIDRequestEnable = pSmeWowlParams->ucWoWEAPIDRequestEnable;
    pHalWowlParams->ucWoWEAPOL4WayEnable = pSmeWowlParams->ucWoWEAPOL4WayEnable;
    pHalWowlParams->ucWowNetScanOffloadMatch = pSmeWowlParams->ucWowNetScanOffloadMatch;
    pHalWowlParams->ucWowGTKRekeyError = pSmeWowlParams->ucWowGTKRekeyError;
    pHalWowlParams->ucWoWBSSConnLoss = pSmeWowlParams->ucWoWBSSConnLoss;
#endif //                   

    pHalWowlParams->bssIdx = pSessionEntry->bssIdx;

    if(wlan_cfgGetInt(pMac, WNI_CFG_WOWLAN_UCAST_PATTERN_FILTER_ENABLE, &cfgValue) != eSIR_SUCCESS)
    {
        pmmLog(pMac, LOGP, FL("cfgGet failed for WNI_CFG_WOWLAN_UCAST_PATTERN_FILTER_ENABLE"));
        goto end;
    }
    pHalWowlParams->ucUcastPatternFilteringEnable = (tANI_U8)cfgValue;

    if(wlan_cfgGetInt(pMac, WNI_CFG_WOWLAN_CHANNEL_SWITCH_ENABLE, &cfgValue) != eSIR_SUCCESS)
    {
        pmmLog(pMac, LOGP, FL("cfgGet failed for WNI_CFG_WOWLAN_CHANNEL_SWITCH_ENABLE"));
        goto end;
    }
    pHalWowlParams->ucWowChnlSwitchRcv = (tANI_U8)cfgValue;

    if(wlan_cfgGetInt(pMac, WNI_CFG_WOWLAN_DEAUTH_ENABLE, &cfgValue) != eSIR_SUCCESS)
    {
       pmmLog(pMac, LOGP, FL("cfgGet failed for WNI_CFG_WOWLAN_DEAUTH_ENABLE "));
       goto end;
    }
    pHalWowlParams->ucWowDeauthRcv = (tANI_U8)cfgValue;

    if(wlan_cfgGetInt(pMac, WNI_CFG_WOWLAN_DISASSOC_ENABLE, &cfgValue) != eSIR_SUCCESS)
    {
        pmmLog(pMac, LOGP, FL("cfgGet failed for WNI_CFG_WOWLAN_DEAUTH_ENABLE "));
        goto end;
    }
    pHalWowlParams->ucWowDisassocRcv = (tANI_U8)cfgValue;

    if(wlan_cfgGetInt(pMac, WNI_CFG_WOWLAN_MAX_MISSED_BEACON, &cfgValue) != eSIR_SUCCESS)
    {
        pmmLog(pMac, LOGP, FL("cfgGet failed for WNI_CFG_WOWLAN_MAX_MISSED_BEACON "));
        goto end;
    }
    pHalWowlParams->ucWowMaxMissedBeacons = (tANI_U8)cfgValue;

    if(wlan_cfgGetInt(pMac, WNI_CFG_WOWLAN_MAX_SLEEP_PERIOD, &cfgValue) != eSIR_SUCCESS)
    {
        pmmLog(pMac, LOGP, FL("cfgGet failed for WNI_CFG_WOWLAN_MAX_SLEEP_PERIOD "));
        goto end;
    }
    pHalWowlParams->ucWowMaxSleepUsec = (tANI_U8)cfgValue;

    //                   
    if( eSIR_SUCCESS != (retCode = pmmSendWowlEnterRequest( pMac, pHalWowlParams)))
    {
        pmmLog(pMac, LOGE, FL("Send ENTER_WOWL_REQ to HAL failed, reasonCode %d "), retCode);
        limSendSmeRsp(pMac, eWNI_PMC_ENTER_WOWL_RSP, eSIR_SME_WOWL_ENTER_REQ_FAILED, 0, 0);
        goto end;
    }
    return;

end:
    if (pHalWowlParams != NULL)
        vos_mem_free(pHalWowlParams);
    return;
}


/*                                                              
                                
                                                           
                                                               
                                                        
                                              
                                      
                                                
                      
                                                                */
tSirRetStatus pmmSendWowlEnterRequest(tpAniSirGlobal pMac, tpSirHalWowlEnterParams pHalWowlParams)
{
    tSirRetStatus             retCode = eSIR_SUCCESS;
    tSirMsgQ                  msgQ;

    if (NULL == pHalWowlParams)
        return eSIR_FAILURE;

    msgQ.type = WDA_WOWL_ENTER_REQ;
    msgQ.reserved = 0;
    msgQ.bodyptr = pHalWowlParams;
    msgQ.bodyval = 0;

    /*                                        
                                    
     */
    SET_LIM_PROCESS_DEFD_MESGS(pMac, false);

    retCode = wdaPostCtrlMsg(pMac, &msgQ);
    if( eSIR_SUCCESS != retCode )
    {
        pmmLog( pMac, LOGE, FL("Posting WDA_WOWL_ENTER_REQ failed, reason=%X"), retCode );
        return retCode;
    }
    return retCode;
}

/*                                                           
                                      
                                                    
                                                  
                             
                               
             
                                                             */
void pmmEnterWowlanResponseHandler(tpAniSirGlobal pMac, tpSirMsgQ limMsg)
{
    tpSirHalWowlEnterParams  pWowlEnterParams;
    eHalStatus               rspStatus;
    tSirResultCodes          smeRspCode = eSIR_SME_SUCCESS;

    /*                                                      
                                                   
     */
    SET_LIM_PROCESS_DEFD_MESGS(pMac, true);

    pWowlEnterParams = (tpSirHalWowlEnterParams)(limMsg->bodyptr);
    if (NULL == pWowlEnterParams)
    {
        pmmLog(pMac, LOGE, FL("Recvd WDA_WOWL_ENTER_RSP with NULL msg "));
        smeRspCode = eSIR_SME_WOWL_ENTER_REQ_FAILED;
    }
    else
    {
        rspStatus = pWowlEnterParams->status;

        if(rspStatus == eHAL_STATUS_SUCCESS)
        {
            pmmLog(pMac, LOGW, FL("Rcv successful response from HAL to enter WOWLAN "));
            pMac->pmm.gPmmState = ePMM_STATE_WOWLAN;
        }
        else
        {
            pmmLog(pMac, LOGE, FL("HAL enter WOWLAN failed, informing SME"));
            smeRspCode = eSIR_SME_WOWL_ENTER_REQ_FAILED;
        }
    }

    limSendSmeRsp(pMac, eWNI_PMC_ENTER_WOWL_RSP, smeRspCode, 0, 0);
    return;
}

/*                                                           
                                    
                                                       
                                            
                             
             
                                                             */
void pmmExitWowlanRequestHandler(tpAniSirGlobal pMac)
{
    tSirRetStatus retStatus = eSIR_SUCCESS;
    tSirResultCodes smeRspCode = eSIR_SME_SUCCESS;
    tpPESession pSessionEntry;
    tpSirHalWowlExitParams  pHalWowlMsg = NULL;
    tANI_U8            PowersavesessionId = 0;

    PowersavesessionId = pMac->pmm.sessionId;

    if((pSessionEntry = peFindSessionBySessionId(pMac,PowersavesessionId)) == NULL )
    {
        PELOGW(pmmLog(pMac, LOGE, FL("pmmWowl : failed to allocate memory"));)
        smeRspCode = eSIR_SME_WOWL_EXIT_REQ_FAILED;
        goto failure;
    }

    pHalWowlMsg = vos_mem_malloc(sizeof(*pHalWowlMsg));
    if ( NULL == pHalWowlMsg )
    {
        pmmLog(pMac, LOGP, FL("Fail to allocate memory for WoWLAN Add Bcast Pattern "));
        smeRspCode = eSIR_SME_WOWL_EXIT_REQ_FAILED;
        goto failure;
    }

#ifdef FEATURE_WLAN_DIAG_SUPPORT 
    limDiagEventReport(pMac, WLAN_PE_DIAG_EXIT_WOWL_REQ_EVENT, NULL, 0, 0);
#endif //                         

    if ( pMac->pmm.gPmmState != ePMM_STATE_WOWLAN )
    {
        pmmLog(pMac, LOGE,
            FL("Exit WOWLAN Request received in invalid state PMM=%d "),
            pMac->pmm.gPmmState);
        smeRspCode = eSIR_SME_INVALID_PMM_STATE;
        goto failure;
    }

    (void) vos_mem_set((tANI_U8 *)pHalWowlMsg, sizeof(*pHalWowlMsg), 0);
    pHalWowlMsg->bssIdx = pSessionEntry->bssIdx;

    if((retStatus = pmmSendExitWowlReq(pMac, pHalWowlMsg)) != eSIR_SUCCESS)
    {
        pmmLog(pMac, LOGE,
            FL("Fail to send WDA_WOWL_EXIT_REQ, reason code %d"),
            retStatus);
        smeRspCode = eSIR_SME_WOWL_EXIT_REQ_FAILED;
        goto failure;
    }
    return;

failure:
    if (pHalWowlMsg != NULL)
       vos_mem_free(pHalWowlMsg);
    limSendSmeRsp(pMac, eWNI_PMC_EXIT_WOWL_RSP, smeRspCode, 0, 0);
    return;
}

/*                                                           
                           
                                                  
                        
                             
             
                                                             */
tSirRetStatus  pmmSendExitWowlReq(tpAniSirGlobal pMac, tpSirHalWowlExitParams pHalWowlParams)
{
    tSirRetStatus  retCode = eSIR_SUCCESS;
    tSirMsgQ       msgQ;

    if (NULL == pHalWowlParams)
        return eSIR_FAILURE;

    msgQ.type = WDA_WOWL_EXIT_REQ;
    msgQ.reserved = 0;
    msgQ.bodyptr = pHalWowlParams;
    msgQ.bodyval = 0;

    pmmLog(pMac, LOGW, FL("Sending WDA_WOWL_EXIT_REQ"));

    /*                                             
                                           
     */
    SET_LIM_PROCESS_DEFD_MESGS(pMac, false);

    if( eSIR_SUCCESS != (retCode = wdaPostCtrlMsg( pMac, &msgQ )))
        pmmLog( pMac, LOGE,
            FL("Posting WDA_WOWL_EXIT_REQ failed, reason=%X"),
            retCode );

    return retCode;
}

/*                                                           
                                     
                                                             
                                                      
                             
             
                                                             */
void pmmExitWowlanResponseHandler(tpAniSirGlobal pMac, tpSirMsgQ limMsg)
{

    tpSirHalWowlExitParams  pHalWowlRspMsg;
    eHalStatus   rspStatus = eHAL_STATUS_FAILURE;

    /*                                                      
                                                  
     */
    SET_LIM_PROCESS_DEFD_MESGS(pMac, true);

    pHalWowlRspMsg = (tpSirHalWowlExitParams)(limMsg->bodyptr);
    if (NULL == pHalWowlRspMsg)
    {
        pmmLog(pMac, LOGE, FL("Recvd WDA_WOWL_ENTER_RSP with NULL msg "));
    }
    else
    {
        //                               
        pMac->pmm.gPmmState = ePMM_STATE_BMPS_SLEEP;
        rspStatus = pHalWowlRspMsg->status;
    }

    if( rspStatus == eHAL_STATUS_SUCCESS)
    {
        pmmLog(pMac, LOGW, FL("Rcvd successful rsp from HAL to exit WOWLAN "));
        limSendSmeRsp(pMac, eWNI_PMC_EXIT_WOWL_RSP, eSIR_SME_SUCCESS, 0, 0);
    }
    else
    {
        pmmLog(pMac, LOGE, FL("Rcvd failure rsp from HAL to exit WOWLAN "));
        limSendSmeRsp(pMac, eWNI_PMC_EXIT_WOWL_RSP, eSIR_SME_WOWL_EXIT_REQ_FAILED, 0, 0);
    }
    return;
}


//                                                                     
/* 
                              
  
            
                                                                  
  
         
  
               
       
  
        
  
                                
                                 
                
 */

tSirRetStatus pmmImpsSendChangePwrSaveMsg(tpAniSirGlobal pMac, tANI_U8 mode)
{
    tSirRetStatus retStatus = eSIR_SUCCESS;
    tSirMsgQ msgQ;

    if (SIR_PM_SLEEP_MODE == mode)
    {
        msgQ.type = WDA_ENTER_IMPS_REQ;
        PELOG2(pmmLog (pMac, LOG2, FL("Sending WDA_ENTER_IMPS_REQ to HAL"));)
    }
    else
    {
        msgQ.type = WDA_EXIT_IMPS_REQ;
        PELOG2(pmmLog (pMac, LOG2, FL("Sending WDA_EXIT_IMPS_REQ to HAL"));)
    }

    msgQ.reserved = 0;
    msgQ.bodyptr = NULL;
    msgQ.bodyval = 0;

    /*                                                      
                                                        
     */
    SET_LIM_PROCESS_DEFD_MESGS(pMac, false);
    MTRACE(macTraceMsgTx(pMac, NO_SESSION, msgQ.type));
    retStatus = wdaPostCtrlMsg(pMac, &msgQ);
    if ( eSIR_SUCCESS != retStatus )
    {
        PELOGE(pmmLog(pMac, LOGE, 
            FL("WDA_ENTER/EXIT_IMPS_REQ to HAL failed, reason=%X"), retStatus);)
    }

    return retStatus;
}

//                                                                     
/* 
                               
  
            
                                                             
                                
  
        
  
                                         
                                          
                         
 */
tSirRetStatus pmmUapsdSendChangePwrSaveMsg (tpAniSirGlobal pMac, tANI_U8 mode)
{
    tSirRetStatus retStatus = eSIR_SUCCESS;
    tpUapsdParams pUapsdParams = NULL;
    tSirMsgQ msgQ;
    tpPESession pSessionEntry;
    tpExitUapsdParams pExitUapsdParams = NULL;

    if((pSessionEntry = peGetValidPowerSaveSession(pMac)) == NULL )
    {
        PELOGW(pmmLog(pMac, LOGW, FL("pmmUapsd : failed to allocate memory"));)
        retStatus = eSIR_FAILURE;
        return retStatus;
    }

    if (SIR_PM_SLEEP_MODE == mode)
    {
        pUapsdParams = vos_mem_malloc(sizeof(tUapsdParams));
        if ( NULL == pUapsdParams )
        {
            PELOGW(pmmLog(pMac, LOGW, FL("pmmUapsd : failed to allocate memory"));)
            retStatus = eSIR_MEM_ALLOC_FAILED;
            return retStatus;
        }

        vos_mem_set( (tANI_U8 *)pUapsdParams, sizeof(tUapsdParams), 0);
        msgQ.type = WDA_ENTER_UAPSD_REQ;
        msgQ.bodyptr = pUapsdParams;

        /*
                                                                             
                                                                                              
        */

        if ( pMac->lim.gAcAdmitMask[SIR_MAC_DIRECTION_DNLINK] & LIM_ADMIT_MASK_FLAG_ACBE)
        {
            pUapsdParams->beDeliveryEnabled = LIM_UAPSD_GET(ACBE, pMac->lim.gUapsdPerAcDeliveryEnableMask);
        }
        else
        {
            pUapsdParams->beDeliveryEnabled = LIM_UAPSD_GET(ACBE, pMac->lim.gUapsdPerAcBitmask);
        }
        if ( pMac->lim.gAcAdmitMask[SIR_MAC_DIRECTION_DNLINK] & LIM_ADMIT_MASK_FLAG_ACBK)
        {
            pUapsdParams->bkDeliveryEnabled = LIM_UAPSD_GET(ACBK, pMac->lim.gUapsdPerAcDeliveryEnableMask);
        }
        else
        {
            pUapsdParams->bkDeliveryEnabled = LIM_UAPSD_GET(ACBK, pMac->lim.gUapsdPerAcBitmask);
        }
        if  ( pMac->lim.gAcAdmitMask[SIR_MAC_DIRECTION_DNLINK] & LIM_ADMIT_MASK_FLAG_ACVI)
        {
            pUapsdParams->viDeliveryEnabled = LIM_UAPSD_GET(ACVI, pMac->lim.gUapsdPerAcDeliveryEnableMask);
        }
        else
        {
            pUapsdParams->viDeliveryEnabled = LIM_UAPSD_GET(ACVI, pMac->lim.gUapsdPerAcBitmask);
        }

        if ( pMac->lim.gAcAdmitMask[SIR_MAC_DIRECTION_DNLINK] & LIM_ADMIT_MASK_FLAG_ACVO)
        {
            pUapsdParams->voDeliveryEnabled = LIM_UAPSD_GET(ACVO, pMac->lim.gUapsdPerAcDeliveryEnableMask);
        }
        else
        {
            pUapsdParams->voDeliveryEnabled = LIM_UAPSD_GET(ACVO, pMac->lim.gUapsdPerAcBitmask);
        }

        /*
                                                                            
                                                                                               
        */

        if ( pMac->lim.gAcAdmitMask[SIR_MAC_DIRECTION_UPLINK] & LIM_ADMIT_MASK_FLAG_ACBE)
        {
             pUapsdParams->beTriggerEnabled = LIM_UAPSD_GET(ACBE, pMac->lim.gUapsdPerAcTriggerEnableMask);
        }
        else
        {
             pUapsdParams->beTriggerEnabled = LIM_UAPSD_GET(ACBE, pMac->lim.gUapsdPerAcBitmask);
        }
        if ( pMac->lim.gAcAdmitMask[SIR_MAC_DIRECTION_UPLINK] & LIM_ADMIT_MASK_FLAG_ACBK)
        {
             pUapsdParams->bkTriggerEnabled = LIM_UAPSD_GET(ACBK, pMac->lim.gUapsdPerAcTriggerEnableMask);
        }
        else
        {
             pUapsdParams->bkTriggerEnabled = LIM_UAPSD_GET(ACBK, pMac->lim.gUapsdPerAcBitmask);
        }
        if ( pMac->lim.gAcAdmitMask[SIR_MAC_DIRECTION_UPLINK] & LIM_ADMIT_MASK_FLAG_ACVI)
        {
             pUapsdParams->viTriggerEnabled = LIM_UAPSD_GET(ACVI, pMac->lim.gUapsdPerAcTriggerEnableMask);
        }
        else
        {
             pUapsdParams->viTriggerEnabled = LIM_UAPSD_GET(ACVI, pMac->lim.gUapsdPerAcBitmask);
        }

        if ( pMac->lim.gAcAdmitMask[SIR_MAC_DIRECTION_UPLINK] & LIM_ADMIT_MASK_FLAG_ACVO)
        {
             pUapsdParams->voTriggerEnabled = LIM_UAPSD_GET(ACVO, pMac->lim.gUapsdPerAcTriggerEnableMask);
        }
        else
        {
             pUapsdParams->voTriggerEnabled = LIM_UAPSD_GET(ACVO, pMac->lim.gUapsdPerAcBitmask);
        }

        pUapsdParams->bssIdx = pSessionEntry->bssIdx;

        PELOGW(pmmLog(pMac, LOGW,
                      FL("UAPSD Mask:  static = 0x%x, DeliveryEnabled = 0x%x, TriggerEnabled = 0x%x "),
            pMac->lim.gUapsdPerAcBitmask,
            pMac->lim.gUapsdPerAcDeliveryEnableMask,
            pMac->lim.gUapsdPerAcTriggerEnableMask);)

        PELOGW(pmmLog(pMac, LOGW, FL("Delivery Enabled: BK=%d, BE=%d, Vi=%d, Vo=%d "),
            pUapsdParams->bkDeliveryEnabled, 
            pUapsdParams->beDeliveryEnabled, 
            pUapsdParams->viDeliveryEnabled, 
            pUapsdParams->voDeliveryEnabled);)

        PELOGW(pmmLog(pMac, LOGW, FL("Trigger Enabled: BK=%d, BE=%d, Vi=%d, Vo=%d "),
            pUapsdParams->bkTriggerEnabled, 
            pUapsdParams->beTriggerEnabled, 
            pUapsdParams->viTriggerEnabled, 
            pUapsdParams->voTriggerEnabled);)

        PELOGW(pmmLog (pMac, LOGW, FL("pmmUapsd: Sending WDA_ENTER_UAPSD_REQ to HAL"));)
    }
    else
    {
        pExitUapsdParams = vos_mem_malloc(sizeof(tExitUapsdParams));
        if ( NULL == pExitUapsdParams )
        {
            PELOGW(pmmLog(pMac, LOGW, FL("pmmUapsd : failed to allocate memory"));)
            retStatus = eSIR_MEM_ALLOC_FAILED;
            return retStatus;
        }

        vos_mem_set( (tANI_U8 *)pExitUapsdParams, sizeof(tExitUapsdParams), 0);
        msgQ.type = WDA_EXIT_UAPSD_REQ;
        msgQ.bodyptr = pExitUapsdParams;
        pExitUapsdParams->bssIdx = pSessionEntry->bssIdx;
        PELOGW(pmmLog (pMac, LOGW, FL("pmmUapsd: Sending WDA_EXIT_UAPSD_REQ to HAL"));)
    }

    /*                                                      
                                         
     */
    SET_LIM_PROCESS_DEFD_MESGS(pMac, false);

    msgQ.reserved = 0;
    msgQ.bodyval = 0;
    MTRACE(macTraceMsgTx(pMac, NO_SESSION, msgQ.type));
    retStatus = wdaPostCtrlMsg(pMac, &msgQ);
    if ( eSIR_SUCCESS != retStatus )
    {
        PELOGE(pmmLog(pMac, LOGE, 
            FL("pmmUapsd: WDA_ENTER/EXIT_UAPSD_REQ to HAL failed, reason=%X"),
            retStatus);)
        if (SIR_PM_SLEEP_MODE == mode)
            vos_mem_free(pUapsdParams);
        else
            vos_mem_free(pExitUapsdParams);
    }

    return retStatus;
}


//                                                                     
/* 
                            
  
            
                                                                     
                           
  
         
  
               
       
  
        
  
                                
                
 */

void pmmImpsUpdatePwrSaveStats(tpAniSirGlobal pMac)
{
/*
                           

                                                                

                                      
     
                                                                                         
     
        
     
                      
     

                                               
     
                                               
     

                                                                                  
     
                                               
     

                                                                                                                                    

                               

           
*/
}


//                                                                     
/* 
                           
  
            
                                                                  
                           
  
         
  
               
       
  
        
  
                                
                
 */

void pmmImpsUpdateWakeupStats (tpAniSirGlobal pMac)
{
/*
                           

                                                                 

                                                                                    

                                               
     
                                               
     

                                                                                  
     
                                               
     

                                                                                                                                        

                               

           
*/
}

//                                                                  
void pmmImpsUpdateSleepErrStats(tpAniSirGlobal pMac,
                                tSirRetStatus retStatus)
{
    pMac->pmm.ImpsSleepErrCnt++;
    pMac->pmm.ImpsLastErr = retStatus;
    return;
}

//                                                                        
void pmmImpsUpdateWakeupErrStats(tpAniSirGlobal pMac,
                                 tSirRetStatus retStatus)
{
    pMac->pmm.ImpsWakeupErrCnt++;
    pMac->pmm.ImpsLastErr = retStatus;
    return;
}


//                                                            
//                             
void pmmImpsUpdateErrStateStats(tpAniSirGlobal pMac)
{
    pMac->pmm.ImpsInvalidStateCnt++;
    return;
}

//                                                      
void pmmImpsUpdatePktDropStats(tpAniSirGlobal pMac)
{

    pMac->pmm.ImpsPktDrpInSleepMode++;
    return;
}

//                                                      
void pmmBmpsUpdatePktDropStats(tpAniSirGlobal pMac)
{

    pMac->pmm.BmpsPktDrpInSleepMode++;
    return;
}

//                                                         
void pmmBmpsUpdateInitFailureCnt(tpAniSirGlobal pMac)
{

    pMac->pmm.BmpsInitFailCnt++;
    return;
}

//                                                             
void pmmBmpsUpdateSleepReqFailureCnt(tpAniSirGlobal pMac)
{

    pMac->pmm.BmpsSleeReqFailCnt++;
    return;
}

//                                                              
void pmmBmpsUpdateWakeupReqFailureCnt(tpAniSirGlobal pMac)
{

    pMac->pmm.BmpsWakeupReqFailCnt++;
    return;
}

//                                                                                     
void pmmBmpsUpdateInvalidStateCnt(tpAniSirGlobal pMac)
{

    pMac->pmm.BmpsInvStateCnt++;
    return;
}

//                                                                    
void pmmBmpsUpdateWakeupIndCnt(tpAniSirGlobal pMac)
{
    pMac->pmm.BmpsWakeupIndCnt++;
    return;
}

//                                                                    
void pmmBmpsUpdateHalReqFailureCnt(tpAniSirGlobal pMac)
{
    pMac->pmm.BmpsHalReqFailCnt++;
    return;
}

//                                                                      
//                    
void pmmBmpsUpdateReqInInvalidRoleCnt(tpAniSirGlobal pMac)
{
    pMac->pmm.BmpsReqInInvalidRoleCnt++;
    return;
}

#if 0
//                            
void pmmUpdateDroppedPktStats(tpAniSirGlobal pMac)
{
    switch (pMac->pmm.gPmmState)
    {
    case ePMM_STATE_BMPS_SLEEP:
        pmmBmpsUpdatePktDropStats(pMac);
        break;

    case ePMM_STATE_IMPS_SLEEP:
        pmmImpsUpdatePktDropStats(pMac);
        break;

    default:
        break;
    }
    return;

}
#endif

//                                  
void pmmResetPmmState(tpAniSirGlobal pMac)
{
    pMac->pmm.gPmmState = ePMM_STATE_READY;
    
    pMac->pmm.inMissedBeaconScenario = FALSE;
    return;
}

/*                                           */
void pmmSendMessageToLim(tpAniSirGlobal pMac,
                         tANI_U32 msgId)
{
    tSirMsgQ limMsg;
    tANI_U32 statusCode;

    limMsg.type = (tANI_U16) msgId;
    limMsg.bodyptr = NULL;
    limMsg.bodyval = 0;

    if ((statusCode = limPostMsgApi(pMac, &limMsg)) != eSIR_SUCCESS)
    {
          PELOGW(pmmLog(pMac, LOGW,
            FL("posting message %X to LIM failed, reason=%d"),
            limMsg.type, statusCode);)
    }
}

#ifdef WLAN_FEATURE_PACKET_FILTERING
void pmmFilterMatchCountResponseHandler(tpAniSirGlobal pMac, tpSirMsgQ limMsg)
{
    tpSirRcvFltPktMatchRsp  pRcvFltPktMatchCntRsp;
    eHalStatus              rspStatus;
    tSirResultCodes         smeRspCode = eSIR_SME_SUCCESS;

    /*                                                      
                                                                             
     */
    SET_LIM_PROCESS_DEFD_MESGS(pMac, true);

    pRcvFltPktMatchCntRsp = (tpSirRcvFltPktMatchRsp)(limMsg->bodyptr);
    if (NULL == pRcvFltPktMatchCntRsp)
    {
        pmmLog(pMac, LOGE, FL("Received "
            "WDA_PACKET_COALESCING_FILTER_MATCH_COUNT_RSP with NULL msg "));
        smeRspCode = eSIR_SME_PC_FILTER_MATCH_COUNT_REQ_FAILED;
    }
    else
    {
        rspStatus = pRcvFltPktMatchCntRsp->status;
        if (eHAL_STATUS_SUCCESS == rspStatus)
        {
            pmmLog(pMac, LOGE, FL("Rcv successful response from HAL to get "
                "Packet Coalescing Filter Match Count"));
        }
        else
        {
            pmmLog(pMac, LOGE, FL("HAL failed to get Packet Coalescing "
                "Filter Match Count, informing SME"));
            smeRspCode = eSIR_SME_PC_FILTER_MATCH_COUNT_REQ_FAILED;
        }
    }

    limSendSmeRsp(pMac, eWNI_PMC_PACKET_COALESCING_FILTER_MATCH_COUNT_RSP, 
                  smeRspCode, 0, 0);
    return;
}
#endif //                              

#ifdef WLAN_FEATURE_GTK_OFFLOAD
void pmmGTKOffloadGetInfoResponseHandler(tpAniSirGlobal pMac, tpSirMsgQ limMsg)
{
    tpSirGtkOffloadGetInfoRspParams  pGtkOffloadGetInfoRspParams;
    eHalStatus                       rspStatus;
    tSirResultCodes                  smeRspCode = eSIR_SME_SUCCESS;

    /*                                                      
                                                            
     */
    SET_LIM_PROCESS_DEFD_MESGS(pMac, true);

    pGtkOffloadGetInfoRspParams = (tpSirGtkOffloadGetInfoRspParams)(limMsg->bodyptr);
    if (NULL == pGtkOffloadGetInfoRspParams)
    {
        pmmLog(pMac, LOGE, FL("Received WDA_GTK_OFFLOAD_GETINFO_RSP with NULL msg "));
        smeRspCode = eSIR_SME_GTK_OFFLOAD_GETINFO_REQ_FAILED;
    }
    else
    {
        rspStatus = pGtkOffloadGetInfoRspParams->ulStatus;
        if(rspStatus == eHAL_STATUS_SUCCESS)
        {
            pmmLog(pMac, LOGW, FL("Rcv successful response from HAL to get GTK Offload Information"));
        }
        else
        {
            pmmLog(pMac, LOGE, FL("HAL failed to get GTK Offload Information, informing SME"));
            smeRspCode = eSIR_SME_GTK_OFFLOAD_GETINFO_REQ_FAILED;
        }
    }

    limSendSmeRsp(pMac, eWNI_PMC_GTK_OFFLOAD_GETINFO_RSP, smeRspCode, 0, 0);
    return;
}
#endif //                         

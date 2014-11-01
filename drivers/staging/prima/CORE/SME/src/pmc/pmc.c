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
* Name:  pmc.c
*
* Description:
      Power Management Control (PMC) processing routines.
*
* Copyright 2008 (c) Qualcomm Technologies, Inc. All Rights Reserved.
*     Qualcomm Technologies Confidential and Proprietary.
*
*
******************************************************************************/

#include "palTypes.h"
#include "aniGlobal.h"
#include "csrLinkList.h"
#include "csrApi.h"
#include "smeInside.h"
#include "sme_Api.h"
#include "smsDebug.h"
#include "pmc.h"
#include "wlan_qct_wda.h"
#include "wlan_ps_wow_diag.h"
#include <vos_power.h>
#include "csrInsideApi.h"

static void pmcProcessDeferredMsg( tpAniSirGlobal pMac );

/*                                                                             
 
                              
 
              
                                           
 
             
                                 
 
          
                                                    
                                                        
 
                                                                             */
eHalStatus pmcEnterLowPowerState (tHalHandle hHal)
{
    tpAniSirGlobal pMac = PMAC_STRUCT(hHal);

    pmcLog(pMac, LOG2, FL("Entering pmcEnterLowPowerState"));

    /*                                             */
    if (pMac->pmc.pmcState == LOW_POWER)
        return eHAL_STATUS_SUCCESS;

    /*                            */
    if (vos_timer_stop(&pMac->pmc.hImpsTimer) != VOS_STATUS_SUCCESS)
    {
        pmcLog(pMac, LOGE, FL("Cannot cancel IMPS timer"));
        return eHAL_STATUS_FAILURE;
    }

    pmcStopTrafficTimer(hHal);

    if (vos_timer_stop(&pMac->pmc.hExitPowerSaveTimer) != VOS_STATUS_SUCCESS)
    {
        pmcLog(pMac, LOGE, FL("Cannot cancel exit power save mode timer"));
        return eHAL_STATUS_FAILURE;
    }

    /*                       */
    pmcDoCallbacks(hHal, eHAL_STATUS_FAILURE);

    /*               */
    pMac->pmc.pmcState = LOW_POWER;

    return eHAL_STATUS_SUCCESS;
}


/*                                                                             
 
                             
 
              
                                              
 
             
                                 
 
          
                                                    
                                                        
 
                                                                             */
eHalStatus pmcExitLowPowerState (tHalHandle hHal)
{
    tpAniSirGlobal pMac = PMAC_STRUCT(hHal);

    pmcLog(pMac, LOG2, FL("Entering pmcExitLowPowerState"));

    /*                                                                */
    if (pMac->pmc.pmcState != LOW_POWER)
    {
        pmcLog(pMac, LOGE, FL("Cannot exit Low Power State if not in that state"));
        return eHAL_STATUS_FAILURE;
    }

    /*                                                        */
    if ((pMac->pmc.hwWlanSwitchState == ePMC_SWITCH_OFF) || (pMac->pmc.swWlanSwitchState == ePMC_SWITCH_OFF))
        return eHAL_STATUS_SUCCESS;

    /*               */
    pMac->pmc.pmcState = FULL_POWER;
    if(pmcShouldBmpsTimerRun(pMac))
    {
        if (pmcStartTrafficTimer(hHal, pMac->pmc.bmpsConfig.trafficMeasurePeriod) != eHAL_STATUS_SUCCESS)
            return eHAL_STATUS_FAILURE;
    }

    return eHAL_STATUS_SUCCESS;
}


/*                                                                             
 
                               
 
              
                                                
 
             
                                 
 
          
                                                    
                                                        
 
                                                                             */
eHalStatus pmcEnterFullPowerState (tHalHandle hHal)
{
    tpAniSirGlobal pMac = PMAC_STRUCT(hHal);

    pmcLog(pMac, LOG2, FL("Entering pmcEnterFullPowerState"));

    /*                                         */
    switch (pMac->pmc.pmcState)
    {

    /*                              */
    case FULL_POWER:
        break;

    /*                                                 
                                   */
    case REQUEST_FULL_POWER:
    case REQUEST_IMPS:
    case REQUEST_BMPS:
    case REQUEST_STANDBY:

        /*               */
        pMac->pmc.pmcState = FULL_POWER;
        pMac->pmc.requestFullPowerPending = FALSE;

        if(pmcShouldBmpsTimerRun(pMac))
            (void)pmcStartTrafficTimer(hHal, pMac->pmc.bmpsConfig.trafficMeasurePeriod);

        pmcProcessDeferredMsg( pMac );
        /*                       */
        pmcDoCallbacks(hHal, eHAL_STATUS_SUCCESS);

        /*                                                                  
                                                                             
                                                                           */
        pmcDoDeviceStateUpdateCallbacks(hHal, FULL_POWER);
        break;

    /*                                                           */
    default:
        pmcLog(pMac, LOGE, FL("Trying to enter Full Power State from state %d"), pMac->pmc.pmcState);
        PMC_ABORT;
        return eHAL_STATUS_FAILURE;
    }

    pmcLog(pMac, LOG1, "PMC: Enter full power done: Cancel XO Core ON vote");
    if (vos_chipVoteXOCore(NULL, NULL, NULL, VOS_FALSE) != VOS_STATUS_SUCCESS)
    {
        pmcLog(pMac, LOGE, "Could not cancel XO Core ON vote. Not returning failure. "
                                "Power consumed will be high");
    }

    return eHAL_STATUS_SUCCESS;
}


/*                                                                             
 
                                      
 
              
                                                        
 
             
                                 
                                                            
 
          
                                                    
                                                        
 
                                                                             */
eHalStatus pmcEnterRequestFullPowerState (tHalHandle hHal, tRequestFullPowerReason fullPowerReason)
{
    tpAniSirGlobal pMac = PMAC_STRUCT(hHal);
    vos_call_status_type callType;
    VOS_STATUS status;

    pmcLog(pMac, LOG2, FL("Entering pmcEnterRequestFullPowerState"));

    /*                                                       */
    switch (pMac->pmc.pmcState)
    {

    /*                                                 */
    case FULL_POWER:
        pmcLog(pMac, LOGE, FL("Requesting Full Power State when already there"));
        return eHAL_STATUS_FAILURE;

    /*                                                           */
    case LOW_POWER:
        pmcLog(pMac, LOGE, FL("Cannot request exit from Low Power State"));
        return eHAL_STATUS_FAILURE;

    /*                                                                  
                                                              */
    case REQUEST_IMPS:
    case REQUEST_START_UAPSD:
    case REQUEST_STOP_UAPSD:
    case REQUEST_STANDBY:
    case REQUEST_BMPS:
    case REQUEST_ENTER_WOWL:
    case REQUEST_EXIT_WOWL:
        pmcLog(pMac, LOGW, FL("Request for full power is being buffered. "
            "Current state is %d"), pMac->pmc.pmcState);
        //                                                                  
        if( !pMac->pmc.requestFullPowerPending )
        {
            pMac->pmc.requestFullPowerPending = TRUE;
            pMac->pmc.requestFullPowerReason = fullPowerReason;
        }
        return eHAL_STATUS_SUCCESS;

    /*                                                */
    case IMPS:
        if ( pMac->pmc.rfSuppliesVotedOff )
        {
            status = vos_chipVoteOnRFSupply(&callType, NULL, NULL);
            VOS_ASSERT( VOS_IS_STATUS_SUCCESS( status ) );

            status = vos_chipVoteOnXOBuffer(&callType, NULL, NULL);
            VOS_ASSERT( VOS_IS_STATUS_SUCCESS( status ) );

            pMac->pmc.rfSuppliesVotedOff = FALSE;
        }

        if (pmcIssueCommand( pMac, eSmeCommandExitImps, NULL, 0, FALSE ) != eHAL_STATUS_SUCCESS)
        {
            return eHAL_STATUS_FAILURE;
        }
        return eHAL_STATUS_SUCCESS;

    /*                                                */
    case BMPS:
    {
        tExitBmpsInfo exitBmpsInfo;
        exitBmpsInfo.exitBmpsReason = fullPowerReason;

        if (pmcIssueCommand(hHal, eSmeCommandExitBmps, &exitBmpsInfo, sizeof(tExitBmpsInfo), FALSE)
               != eHAL_STATUS_SUCCESS)
        {
            return eHAL_STATUS_FAILURE;
        }
        return eHAL_STATUS_SUCCESS;
    }
    /*                                      */
    case REQUEST_FULL_POWER:
        return eHAL_STATUS_SUCCESS;

    /*                                                */
    case STANDBY:
        if ( pMac->pmc.rfSuppliesVotedOff )
        {
            status = vos_chipVoteOnXOBuffer(&callType, NULL, NULL);
            if(VOS_STATUS_SUCCESS != status)
            {
                return eHAL_STATUS_FAILURE;
            }
            status = vos_chipVoteOnRFSupply(&callType, NULL, NULL);
            if(VOS_STATUS_SUCCESS != status)
            {
                return eHAL_STATUS_FAILURE;
            }

            pMac->pmc.rfSuppliesVotedOff = FALSE;
        }

        if (pmcIssueCommand(hHal, eSmeCommandExitImps, NULL, 0, FALSE) !=
            eHAL_STATUS_SUCCESS)
        {
            pmcLog(pMac, LOGE, "PMC: failure to send message "
            "eWNI_PMC_EXIT_IMPS_REQ");
            return eHAL_STATUS_FAILURE;
        }

        return eHAL_STATUS_SUCCESS;

    /*                                               */
    case UAPSD:
        //                                                                               
        if (pmcIssueCommand(hHal, eSmeCommandExitUapsd, &fullPowerReason, sizeof(tRequestFullPowerReason), FALSE) !=
            eHAL_STATUS_SUCCESS)
        {
            pmcLog(pMac, LOGE, "PMC: failure to send message "
            "eWNI_PMC_EXIT_UAPSD_REQ");
            return eHAL_STATUS_FAILURE;
        }
        return eHAL_STATUS_SUCCESS;

    /*                                              */
    case WOWL:
        if (pmcIssueCommand(hHal, eSmeCommandExitWowl, &fullPowerReason, sizeof(tRequestFullPowerReason), FALSE) !=
            eHAL_STATUS_SUCCESS)
        {
            pmcLog(pMac, LOGP, "PMC: failure to send message "
            "eWNI_PMC_EXIT_WOWL_REQ");
            return eHAL_STATUS_FAILURE;
        }
        return eHAL_STATUS_SUCCESS;

    /*                                                                   */
    default:
        pmcLog(pMac, LOGE,
               FL("Trying to enter Request Full Power State from state %d"), pMac->pmc.pmcState);
        PMC_ABORT;
        return eHAL_STATUS_FAILURE;
    }
}


/*                                                                             
 
                                 
 
              
                                                  
 
             
                                 
 
          
                                                    
                                                        
 
                                                                             */
eHalStatus pmcEnterRequestImpsState (tHalHandle hHal)
{
    tpAniSirGlobal pMac = PMAC_STRUCT(hHal);

    pmcLog(pMac, LOG2, FL("Entering pmcEnterRequestImpsState"));

    /*                                                          */
    if (pMac->pmc.pmcState != FULL_POWER)
    {
        pmcLog(pMac, LOGE, FL("Trying to enter Request IMPS State from state %d"), pMac->pmc.pmcState);
        return eHAL_STATUS_FAILURE;
    }

    /*                                                                 */
    pmcStopTrafficTimer(hHal);

    /*                                          */
    if (pmcIssueCommand(hHal, eSmeCommandEnterImps, NULL, 0, FALSE) != eHAL_STATUS_SUCCESS)
    {
        pmcLog(pMac, LOGE, "PMC: failure to send message eWNI_PMC_ENTER_IMPS_REQ");
        pMac->pmc.pmcState = FULL_POWER;
        if(pmcShouldBmpsTimerRun(pMac))
            (void)pmcStartTrafficTimer(hHal, pMac->pmc.bmpsConfig.trafficMeasurePeriod);
        return eHAL_STATUS_FAILURE;
     }

    pmcLog(pMac, LOG2, FL("eWNI_PMC_ENTER_IMPS_REQ sent to PE"));

    return eHAL_STATUS_SUCCESS;
}


/*                                                                             
 
                          
 
              
                                          
 
             
                                 
 
          
                                                    
                                                        
 
                                                                             */
eHalStatus pmcEnterImpsState (tHalHandle hHal)
{
    tpAniSirGlobal pMac = PMAC_STRUCT(hHal);
    vos_call_status_type callType;
    VOS_STATUS status;
    pmcLog(pMac, LOG2, FL("Entering pmcEnterImpsState"));

    /*                                                    */
    if (pMac->pmc.pmcState != REQUEST_IMPS)
    {
        pmcLog(pMac, LOGE, FL("Trying to enter IMPS State from state %d"), pMac->pmc.pmcState);
        return eHAL_STATUS_FAILURE;
    }

    /*               */
    pMac->pmc.pmcState = IMPS;

    /*                                                               
                                 */
    if (pMac->pmc.requestFullPowerPending)
    {

        /*                               */
        return pmcEnterRequestFullPowerState(hHal, pMac->pmc.requestFullPowerReason);
    }

    /*                                                             */
    if(0 != pMac->pmc.impsPeriod)
    {
        if (vos_timer_start(&pMac->pmc.hImpsTimer, pMac->pmc.impsPeriod) != VOS_STATUS_SUCCESS)
        {
            PMC_ABORT;
            pMac->pmc.ImpsReqTimerFailed = VOS_TRUE;
            if (!(pMac->pmc.ImpsReqTimerfailCnt & 0xF))
            {
                pMac->pmc.ImpsReqTimerfailCnt++;
                pmcLog(pMac, LOGE,
                       FL("Cannot start IMPS timer, FailCnt - %d"), pMac->pmc.ImpsReqTimerfailCnt);
            }
            pmcEnterRequestFullPowerState(hHal, eSME_REASON_OTHER);
            return eHAL_STATUS_FAILURE;
        }
        if (pMac->pmc.ImpsReqTimerfailCnt)
        {
           pmcLog(pMac, LOGE,
                  FL("Start IMPS timer was failed %d times before success"), pMac->pmc.ImpsReqTimerfailCnt);
        }
        pMac->pmc.ImpsReqTimerfailCnt = 0;
    }

    //                                                                      
    //                                      
    status = vos_chipVoteOffRFSupply(&callType, NULL, NULL);
    VOS_ASSERT( VOS_IS_STATUS_SUCCESS( status ) );

    status = vos_chipVoteOffXOBuffer(&callType, NULL, NULL);
    VOS_ASSERT( VOS_IS_STATUS_SUCCESS( status ) );

    pMac->pmc.rfSuppliesVotedOff= TRUE;

    return eHAL_STATUS_SUCCESS;
}


/*                                                                             
 
                                 
 
              
                                                  
 
             
                                 
 
          
                                                    
                                                        
 
                                                                             */
eHalStatus pmcEnterRequestBmpsState (tHalHandle hHal)
{
    tpAniSirGlobal pMac = PMAC_STRUCT(hHal);

    pmcLog(pMac, LOG2, FL("Entering pmcEnterRequestBmpsState"));

    /*                                                          */
    if (pMac->pmc.pmcState != FULL_POWER)
    {
        pmcLog(pMac, LOGE,
               FL("Trying to enter Request BMPS State from state %d"), pMac->pmc.pmcState);
        return eHAL_STATUS_FAILURE;
    }

    /*                                                                                  
                                                                                            */
    pmcStopTrafficTimer(hHal);

    /*                                          */
    if ( !pMac->pmc.bmpsRequestQueued )
    {
        pMac->pmc.bmpsRequestQueued = eANI_BOOLEAN_TRUE;
        if(pmcIssueCommand(hHal, eSmeCommandEnterBmps, NULL, 0, FALSE) != eHAL_STATUS_SUCCESS)
        {
            pmcLog(pMac, LOGE, "PMC: failure to send message eWNI_PMC_ENTER_BMPS_REQ");
            pMac->pmc.bmpsRequestQueued = eANI_BOOLEAN_FALSE;
            pMac->pmc.pmcState = FULL_POWER;
            if(pmcShouldBmpsTimerRun(pMac))
            {
                (void)pmcStartTrafficTimer(hHal, pMac->pmc.bmpsConfig.trafficMeasurePeriod);
            }
            return eHAL_STATUS_FAILURE;
        }
    }
    else
    {
        pmcLog(pMac, LOGE, "PMC: enter BMPS command already queued");
        //                           
        if(pmcShouldBmpsTimerRun(pMac))
        {
            (void)pmcStartTrafficTimer(hHal, pMac->pmc.bmpsConfig.trafficMeasurePeriod);
        }
        return eHAL_STATUS_SUCCESS;
    }

    pmcLog(pMac, LOGW, FL("eWNI_PMC_ENTER_BMPS_REQ sent to PE"));

    return eHAL_STATUS_SUCCESS;
}


/*                                                                             
 
                          
 
              
                                          
 
             
                                 
 
          
                                                    
                                                        
 
                                                                             */
eHalStatus pmcEnterBmpsState (tHalHandle hHal)
{
    tpAniSirGlobal pMac = PMAC_STRUCT(hHal);

    pmcLog(pMac, LOG2, FL("Entering pmcEnterBmpsState"));

    /*                                          */
    if (pMac->pmc.pmcState != REQUEST_BMPS &&
        pMac->pmc.pmcState != REQUEST_START_UAPSD &&
        pMac->pmc.pmcState != REQUEST_STOP_UAPSD &&
        pMac->pmc.pmcState != REQUEST_ENTER_WOWL &&
        pMac->pmc.pmcState != REQUEST_EXIT_WOWL)
    {
        pmcLog(pMac, LOGE, FL("Trying to enter BMPS State from state %d"), pMac->pmc.pmcState);
        return eHAL_STATUS_FAILURE;
    }

    /*               */
    pMac->pmc.pmcState = BMPS;

   /*                                                            
                                                                    
                                          */
    pmcDoDeviceStateUpdateCallbacks(hHal, BMPS);

    /*                                                                                          */
    if (pMac->pmc.requestFullPowerPending)
    {

        /*                               */
        pmcLog(pMac, LOGW, FL("Pending Full Power request found on entering BMPS mode. "
                  "Start exit BMPS exit sequence"));
        //                                                                          
        pmcEnterRequestFullPowerState(hHal, pMac->pmc.requestFullPowerReason);
        return eHAL_STATUS_SUCCESS;
    }

    /*                                                                                */
    if (pMac->pmc.wowlModeRequired && pMac->pmc.uapsdSessionRequired)
    {
        pmcLog(pMac, LOGW, FL("Both UAPSD and WOWL is required on entering BMPS mode. "
               "UAPSD will be prioritized over WOWL"));
    }

    /*                  */
    if (pMac->pmc.uapsdSessionRequired)
    {
        pmcLog(pMac, LOGW, FL("UAPSD session is required on entering BMPS mode. "
                  "Start UAPSD entry sequence"));
        pmcEnterRequestStartUapsdState(hHal);
        return eHAL_STATUS_SUCCESS;
    }

    /*                 */
    if (pMac->pmc.wowlModeRequired)
    {
        pmcLog(pMac, LOGW, FL("WOWL is required on entering BMPS mode. "
                  "Start WOWL entry sequence"));
        pmcRequestEnterWowlState(hHal, &(pMac->pmc.wowlEnterParams));
    }

    return eHAL_STATUS_SUCCESS;
}


/*                                                                             
 
                          
 
              
                                                           
 
             
                                 
 
          
                            
                                              
 
                                                                             */
tANI_BOOLEAN pmcPowerSaveCheck (tHalHandle hHal)
{
    tpAniSirGlobal pMac = PMAC_STRUCT(hHal);
    tListElem *pEntry;
    tpPowerSaveCheckEntry pPowerSaveCheckEntry;
    tANI_BOOLEAN (*checkRoutine) (void *checkContext);
    tANI_BOOLEAN bResult=FALSE;

    pmcLog(pMac, LOG2, FL("Entering pmcPowerSaveCheck"));

    /*                                                                
                                                             */
    pEntry = csrLLPeekHead(&pMac->pmc.powerSaveCheckList, FALSE);
    while (pEntry != NULL)
    {
        pPowerSaveCheckEntry = GET_BASE_ADDR(pEntry, tPowerSaveCheckEntry, link);
        checkRoutine = pPowerSaveCheckEntry->checkRoutine;

        /*                                                                              
                       */
        if (NULL != checkRoutine)
        {
            if (!checkRoutine(pPowerSaveCheckEntry->checkContext))
            {
                pmcLog(pMac, LOGE, FL("pmcPowerSaveCheck fail!"));
                bResult = FALSE;
                break;
            }
            else
            {
                bResult = TRUE;
            }
        }
        pEntry = csrLLNext(&pMac->pmc.powerSaveCheckList, pEntry, FALSE);
    }

    return bResult;
}


/*                                                                             
 
                                      
 
              
                                                                  
 
             
                                 
 
          
                                                   
                                                      
 
                                                                             */
eHalStatus pmcSendPowerSaveConfigMessage (tHalHandle hHal)
{
    tpAniSirGlobal pMac = PMAC_STRUCT(hHal);
    tSirPowerSaveCfg powerSaveConfig;

    pmcLog(pMac, LOG2, FL("Entering pmcSendPowerSaveConfigMessage"));

    palZeroMemory(pMac->hHdd, &(powerSaveConfig), sizeof(tSirPowerSaveCfg));

    switch (pMac->pmc.bmpsConfig.forwardBeacons)
    {
    case ePMC_NO_BEACONS:
        powerSaveConfig.beaconFwd = ePM_BEACON_FWD_NONE;
        break;
    case ePMC_BEACONS_WITH_TIM_SET:
        powerSaveConfig.beaconFwd = ePM_BEACON_FWD_TIM;
        break;
    case ePMC_BEACONS_WITH_DTIM_SET:
        powerSaveConfig.beaconFwd = ePM_BEACON_FWD_DTIM;
        break;
    case ePMC_NTH_BEACON:
        powerSaveConfig.beaconFwd = ePM_BEACON_FWD_NTH;
        powerSaveConfig.nthBeaconFwd = (tANI_U16)pMac->pmc.bmpsConfig.valueOfN;
        break;
    case ePMC_ALL_BEACONS:
        powerSaveConfig.beaconFwd = ePM_BEACON_FWD_NTH;
        powerSaveConfig.nthBeaconFwd = 1;
        break;
    }
    powerSaveConfig.fEnablePwrSaveImmediately = pMac->pmc.bmpsConfig.setPmOnLastFrame;
    powerSaveConfig.fPSPoll = pMac->pmc.bmpsConfig.usePsPoll;
    powerSaveConfig.fEnableBeaconEarlyTermination =
        pMac->pmc.bmpsConfig.enableBeaconEarlyTermination;
    powerSaveConfig.bcnEarlyTermWakeInterval =
        pMac->pmc.bmpsConfig.bcnEarlyTermWakeInterval;

    /*                                                                          
                                                */
    ccmCfgSetInt(pMac, WNI_CFG_LISTEN_INTERVAL, pMac->pmc.bmpsConfig.bmpsPeriod,
        NULL, eANI_BOOLEAN_FALSE);

    if( pMac->pmc.pmcState == IMPS || pMac->pmc.pmcState == REQUEST_IMPS )
    {
        //                      
        eHalStatus status = pmcDeferMsg( pMac, eWNI_PMC_PWR_SAVE_CFG,
                                    &powerSaveConfig, sizeof(tSirPowerSaveCfg) );

        if( eHAL_STATUS_PMC_PENDING == status )
        {
            return eHAL_STATUS_SUCCESS;
        }
        else
        {
            //                                    
            if( !HAL_STATUS_SUCCESS( status ) )
            {
                return ( status );
            }
            //                                                     
        }
    }
    /*                                                                            
               */
    if (pmcSendMessage(hHal, eWNI_PMC_PWR_SAVE_CFG, &powerSaveConfig, sizeof(tSirPowerSaveCfg))
        != eHAL_STATUS_SUCCESS)
    {
        pmcLog(pMac, LOGE, FL("Send of eWNI_PMC_PWR_SAVE_CFG to PE failed"));
        return eHAL_STATUS_FAILURE;
    }

    return eHAL_STATUS_SUCCESS;
}


/*                                                                             
 
                       
 
              
                              
 
             
                                 
                                       
                                           
                                           
 
          
                                                   
                                                      
 
                                                                             */
eHalStatus pmcSendMessage (tpAniSirGlobal pMac, tANI_U16 messageType, void *pMessageData, tANI_U32 messageSize)
{
    tSirMbMsg *pMsg;

    pmcLog(pMac, LOG2, FL("Entering pmcSendMessage, message type %d"), messageType);

    /*                               */
    if (palAllocateMemory(pMac->hHdd, (void **)&pMsg, WNI_CFG_MB_HDR_LEN + messageSize) != eHAL_STATUS_SUCCESS)
    {
        pmcLog(pMac, LOGE, FL("Cannot allocate memory for message"));
        PMC_ABORT;
        return eHAL_STATUS_FAILURE;
    }
    pMsg->type = messageType;
    pMsg->msgLen = (tANI_U16) (WNI_CFG_MB_HDR_LEN + messageSize);
    if (messageSize > 0)
    {
        if (palCopyMemory(pMac->hHdd, pMsg->data, pMessageData, messageSize) != eHAL_STATUS_SUCCESS)
        {
            pmcLog(pMac, LOGE, FL("Cannot copy message data"));
            PMC_ABORT;
            return eHAL_STATUS_FAILURE;
        }
    }

    /*               */
    if (palSendMBMessage(pMac->hHdd, pMsg) != eHAL_STATUS_SUCCESS)
    {
        pmcLog(pMac, LOGE, FL("Cannot send message"));
        PMC_ABORT;
        return eHAL_STATUS_FAILURE;
    }

    return eHAL_STATUS_SUCCESS;
}


/*                                                                             
 
                       
 
              
                                                                        
                                 
 
             
                                 
                                                             
 
          
            
 
                                                                             */
void pmcDoCallbacks (tHalHandle hHal, eHalStatus callbackStatus)
{
    tpAniSirGlobal pMac = PMAC_STRUCT(hHal);
    tListElem *pEntry;
    tpRequestFullPowerEntry pRequestFullPowerEntry;

    pmcLog(pMac, LOG2, FL("Entering pmcDoCallbacks"));

    /*                             */
    if (pMac->pmc.impsCallbackRoutine != NULL)
    {
        pMac->pmc.impsCallbackRoutine(pMac->pmc.impsCallbackContext, callbackStatus);
        pMac->pmc.impsCallbackRoutine = NULL;
    }

    /*                                                                    */
    while (NULL != (pEntry = csrLLRemoveHead(&pMac->pmc.requestFullPowerList, TRUE)))
    {
        pRequestFullPowerEntry = GET_BASE_ADDR(pEntry, tRequestFullPowerEntry, link);
        if (pRequestFullPowerEntry->callbackRoutine)
           pRequestFullPowerEntry->callbackRoutine(pRequestFullPowerEntry->callbackContext, callbackStatus);
        if (palFreeMemory(pMac->hHdd, pRequestFullPowerEntry) != eHAL_STATUS_SUCCESS)
        {
            pmcLog(pMac, LOGE, FL("Cannot free request full power routine list entry"));
            PMC_ABORT;
        }
    }

}


/*                                                                             
 
                             
 
              
                                                                
                                             
 
             
                                 
 
          
                                                    
                                                     
 
                                                                             */
eHalStatus pmcStartTrafficTimer (tHalHandle hHal, tANI_U32 expirationTime)
{
    tpAniSirGlobal pMac = PMAC_STRUCT(hHal);
    VOS_STATUS vosStatus;

    pmcLog(pMac, LOG2, FL("Entering pmcStartTrafficTimer"));

    vosStatus = vos_timer_start(&pMac->pmc.hTrafficTimer, expirationTime);
    if ( !VOS_IS_STATUS_SUCCESS(vosStatus) )
    {
        if( VOS_STATUS_E_ALREADY == vosStatus )
        {
            //                                                    
            pmcLog(pMac, LOGW, FL("  traffic timer is already started"));
        }
        else
        {
            pmcLog(pMac, LOGP, FL("Cannot start traffic timer"));
            return eHAL_STATUS_FAILURE;
        }
    }

    return eHAL_STATUS_SUCCESS;
}


/*                                                                             
 
                            
 
              
                                                                               
                                             
 
             
                                 
 
 
                                                                             */
void pmcStopTrafficTimer (tHalHandle hHal)
{
    tpAniSirGlobal pMac = PMAC_STRUCT(hHal);
    pmcLog(pMac, LOG2, FL("Entering pmcStopTrafficTimer"));
    vos_timer_stop(&pMac->pmc.hTrafficTimer);
}


/*                                                                             
 
                            
 
              
                                    
 
             
                                 
 
          
            
 
                                                                             */
void pmcImpsTimerExpired (tHalHandle hHal)
{
    tpAniSirGlobal pMac = PMAC_STRUCT(hHal);

    pmcLog(pMac, LOG2, FL("Entering pmcImpsTimerExpired"));

    /*                                                                                       */
    if (pMac->pmc.pmcState != IMPS)
    {
        pmcLog(pMac, LOGE, FL("Got IMPS timer expiration in state %d"), pMac->pmc.pmcState);
        PMC_ABORT;
        return;
    }

    /*                                                */
    pmcEnterRequestFullPowerState(hHal, eSME_REASON_OTHER);
}


/*                                                                             
 
                               
 
              
                                                   
 
             
                                 
 
          
            
 
                                                                             */
void pmcTrafficTimerExpired (tHalHandle hHal)
{

    tpAniSirGlobal pMac = PMAC_STRUCT(hHal);
    VOS_STATUS vosStatus;

    pmcLog(pMac, LOG2, FL("BMPS Traffic timer expired"));

    /*                                                                                             */
    if (pMac->pmc.pmcState != FULL_POWER)
    {
        pmcLog(pMac, LOGE, FL("Got traffic timer expiration in state %d"), pMac->pmc.pmcState);
        return;
    }

    /*                                                     */
    if(pMac->pmc.remainInPowerActiveTillDHCP)
    {
        pmcLog(pMac, LOG2, FL("BMPS Traffic Timer expired before DHCP completion ignore enter BMPS"));
        pMac->pmc.remainInPowerActiveThreshold++;
        if( pMac->pmc.remainInPowerActiveThreshold >= DHCP_REMAIN_POWER_ACTIVE_THRESHOLD)
        {
           pmcLog(pMac, LOGE,
                  FL("Remain in power active DHCP threshold reached FALLBACK to enable enter BMPS"));
           /*                                                    */
           pMac->pmc.remainInPowerActiveTillDHCP = FALSE;
           pMac->pmc.remainInPowerActiveThreshold = 0;
        }
        //                                                       
        vosStatus = vos_timer_start(&pMac->pmc.hTrafficTimer, pMac->pmc.bmpsConfig.trafficMeasurePeriod);
        if ( !VOS_IS_STATUS_SUCCESS(vosStatus) && (VOS_STATUS_E_ALREADY != vosStatus) )
        {
            pmcLog(pMac, LOGP, FL("Cannot re-start traffic timer"));
        }
        return;
    }

    /*                                        */
    pMac->pmc.remainInPowerActiveThreshold = 0;

    /*                                      */
    if (!pmcShouldBmpsTimerRun(pMac))
    {
        pmcLog(pMac, LOGE, FL("BMPS timer should not be running"));
        return;
    }

#ifdef FEATURE_WLAN_TDLS
    if (pMac->isTdlsPowerSaveProhibited)
    {
       pmcLog(pMac, LOGE, FL("TDLS peer(s) connected/discovery sent. Dont enter BMPS"));
       return;
    }
#endif

    if (pmcPowerSaveCheck(hHal))
    {
        pmcLog(pMac, LOGW, FL("BMPS entry criteria satisfied. Requesting BMPS state"));
        (void)pmcEnterRequestBmpsState(hHal);
    }
    else
    {
        /*                                                                                       */
        pmcLog(pMac, LOGE, FL("Power Save check failed. Retry BMPS again later"));
        //                                                                         
        vosStatus = vos_timer_start(&pMac->pmc.hTrafficTimer, pMac->pmc.bmpsConfig.trafficMeasurePeriod);
        if ( !VOS_IS_STATUS_SUCCESS(vosStatus) && (VOS_STATUS_E_ALREADY != vosStatus) )
        {
            pmcLog(pMac, LOGP, FL("Cannot start traffic timer"));
            return;
        }
    }
}


/*                                                                             
 
                                     
 
              
                                                                                
 
             
                                 
 
          
            
 
                                                                             */
void pmcExitPowerSaveTimerExpired (tHalHandle hHal)
{
    tpAniSirGlobal pMac = PMAC_STRUCT(hHal);

    pmcLog(pMac, LOG2, FL("Entering pmcExitPowerSaveTimerExpired"));

    /*                                                                                                        */
    if (pMac->pmc.requestFullPowerPending)

        /*                                                */
        pmcEnterRequestFullPowerState(hHal, pMac->pmc.requestFullPowerReason);
}

/*                                                                             
 
                           
 
              
                                                                           
                     
 
             
                                 
                                         
 
          
            
 
                                                                             */
void pmcDoBmpsCallbacks (tHalHandle hHal, eHalStatus callbackStatus)
{
   tpAniSirGlobal pMac = PMAC_STRUCT(hHal);
   tListElem *pEntry;
   tpRequestBmpsEntry pRequestBmpsEntry;

   pmcLog(pMac, LOG2, "PMC: entering pmcDoBmpsCallbacks");

   /*                                                              */
   csrLLLock(&pMac->pmc.requestBmpsList);
   pEntry = csrLLRemoveHead(&pMac->pmc.requestBmpsList, FALSE);
   while (pEntry != NULL)
   {
      pRequestBmpsEntry = GET_BASE_ADDR(pEntry, tRequestBmpsEntry, link);
      if (pRequestBmpsEntry->callbackRoutine)
         pRequestBmpsEntry->callbackRoutine(pRequestBmpsEntry->callbackContext,
         callbackStatus);
      palFreeMemory(pMac->hHdd, pRequestBmpsEntry);
      pEntry = csrLLRemoveHead(&pMac->pmc.requestBmpsList, FALSE);
   }
   csrLLUnlock(&pMac->pmc.requestBmpsList);
}




/*                                                                             
 
                                 
 
              
                                                                            
                      
 
             
                                 
                                         
 
          
            
 
                                                                             */
void pmcDoStartUapsdCallbacks (tHalHandle hHal, eHalStatus callbackStatus)
{
   tpAniSirGlobal pMac = PMAC_STRUCT(hHal);
   tListElem *pEntry;
   tpStartUapsdEntry pStartUapsdEntry;

   pmcLog(pMac, LOG2, "PMC: entering pmcDoStartUapsdCallbacks");
   csrLLLock(&pMac->pmc.requestStartUapsdList);
   /*                                                                     */
   pEntry = csrLLRemoveHead(&pMac->pmc.requestStartUapsdList, FALSE);
   while (pEntry != NULL)
   {
      pStartUapsdEntry = GET_BASE_ADDR(pEntry, tStartUapsdEntry, link);
      pStartUapsdEntry->callbackRoutine(pStartUapsdEntry->callbackContext,
         callbackStatus);
      palFreeMemory(pMac->hHdd, pStartUapsdEntry);
      pEntry = csrLLRemoveHead(&pMac->pmc.requestStartUapsdList, FALSE);
   }
   csrLLUnlock(&pMac->pmc.requestStartUapsdList);
}

/*                                                                             
 
                                       
 
              
                                           
 
             
                                 
 
          
                                                    
                                                        
 
                                                                             */
eHalStatus pmcEnterRequestStartUapsdState (tHalHandle hHal)
{
   tpAniSirGlobal pMac = PMAC_STRUCT(hHal);
   v_BOOL_t fFullPower = VOS_FALSE;     //                                    

   pmcLog(pMac, LOG2, "PMC: entering pmcEnterRequestStartUapsdState");

   /*                                                           */
   switch (pMac->pmc.pmcState)
   {
      case FULL_POWER:
         /*                                                                  */
         if (!pmcPowerSaveCheck(hHal))
         {
            pmcLog(pMac, LOGW, "PMC: Power save check failed. UAPSD request "
                      "will be accepted and buffered");
            /*                                                       */
            pMac->pmc.uapsdSessionRequired = TRUE;
            /*                                           */
            if(pmcShouldBmpsTimerRun(pMac))
               (void)pmcStartTrafficTimer(hHal, pMac->pmc.bmpsConfig.trafficMeasurePeriod);
            break;
         }
         else
         {
            pMac->pmc.uapsdSessionRequired = TRUE;
            //               
#ifndef WLAN_MDM_CODE_REDUCTION_OPT
            if( btcIsReadyForUapsd( pMac ) )
#endif /*                            */
            {
               /*                                                            
                                                                 */
               if(pmcEnterRequestBmpsState(hHal) != eHAL_STATUS_SUCCESS)
               {
                   pmcLog(pMac, LOGE, "PMC: Device in Full Power. Enter Request Bmps failed. "
                            "UAPSD request will be dropped ");
                  return eHAL_STATUS_FAILURE;
               }
            }
#ifndef WLAN_MDM_CODE_REDUCTION_OPT
            else
            {
               (void)pmcStartTrafficTimer(hHal, pMac->pmc.bmpsConfig.trafficMeasurePeriod);
            }
#endif /*                            */
         }
         break;

      case BMPS:
         //                                           
#ifndef WLAN_MDM_CODE_REDUCTION_OPT
         if( btcIsReadyForUapsd(pMac) )
#endif /*                            */
         {
            /*                                           */
            if (pmcIssueCommand(hHal, eSmeCommandEnterUapsd, NULL, 0, FALSE) !=
               eHAL_STATUS_SUCCESS)
            {
               pmcLog(pMac, LOGE, "PMC: failure to send message "
                  "eWNI_PMC_ENTER_BMPS_REQ");
               return eHAL_STATUS_FAILURE;
            }
         }
#ifndef WLAN_MDM_CODE_REDUCTION_OPT
         else
         {
            //                                                                    
            pmcLog(pMac, LOGE, " PMC state = %d",pMac->pmc.pmcState);
            pMac->pmc.uapsdSessionRequired = TRUE;
            /*                                                  
                                             */
            //                      
         }
#endif /*                            */
         break;

      case REQUEST_START_UAPSD:
#ifndef WLAN_MDM_CODE_REDUCTION_OPT
         if( !btcIsReadyForUapsd(pMac) )
         {
            //                                              
            fFullPower = VOS_TRUE;
         }
#endif
         break;

      case REQUEST_BMPS:
        /*                                */
        pMac->pmc.uapsdSessionRequired = TRUE;
#ifndef WLAN_MDM_CODE_REDUCTION_OPT
        if( !btcIsReadyForUapsd(pMac) )
         {
            //                                              
            fFullPower = VOS_TRUE;
         }
#endif /*                            */
        break;

      default:
         pmcLog(pMac, LOGE, "PMC: trying to enter UAPSD State from state %d",
            pMac->pmc.pmcState);
         return eHAL_STATUS_FAILURE;
   }

   if(fFullPower)
   {
      if( eHAL_STATUS_PMC_PENDING != pmcRequestFullPower( pMac, NULL, NULL, eSME_REASON_OTHER ) )
      {
         //                
         pmcLog(pMac, LOGE, FL(" fail to request full power because BTC"));
      }
   }

   return eHAL_STATUS_SUCCESS;
}

/*                                                                             
 
                           
 
              
                                           
 
             
                                 
 
          
                                                    
                                                        
 
                                                                             */
eHalStatus pmcEnterUapsdState (tHalHandle hHal)
{
   tpAniSirGlobal pMac = PMAC_STRUCT(hHal);

   pmcLog(pMac, LOG2, "PMC: entering pmcEnterUapsdState");

   /*                                                      */
   if (pMac->pmc.pmcState != REQUEST_START_UAPSD )
   {
      pmcLog(pMac, LOGE, "PMC: trying to enter UAPSD State from state %d",
      pMac->pmc.pmcState);
      return eHAL_STATUS_FAILURE;
   }

   /*               */
   pMac->pmc.pmcState = UAPSD;

   /*                                                             
                                                                     
                                       */
   pmcDoDeviceStateUpdateCallbacks(hHal, UAPSD);

   /*                                                               
                             */
   if (pMac->pmc.requestFullPowerPending)
   {
      /*                                */
      return pmcEnterRequestFullPowerState(hHal, pMac->pmc.requestFullPowerReason);
   }

   return eHAL_STATUS_SUCCESS;
}


/*                                                                             
 
                                      
 
              
                                          
 
             
                                 
 
          
                                                    
                                                        
 
                                                                             */
eHalStatus pmcEnterRequestStopUapsdState (tHalHandle hHal)
{
   tpAniSirGlobal pMac = PMAC_STRUCT(hHal);

   pmcLog(pMac, LOG2, "PMC: entering pmcEnterRequestStopUapsdState");

   /*                                                 */
   if (pMac->pmc.pmcState == REQUEST_STOP_UAPSD)
   {
      return eHAL_STATUS_SUCCESS;
   }

   /*                                                    */
   if (pMac->pmc.pmcState != UAPSD)
   {
      pmcLog(pMac, LOGE, "PMC: trying to enter Request Stop UAPSD State from "
         "state %d", pMac->pmc.pmcState);
      return eHAL_STATUS_FAILURE;
   }

   /*                                          */
   if (pmcIssueCommand(hHal, eSmeCommandExitUapsd, NULL, 0, FALSE) !=
      eHAL_STATUS_SUCCESS)
   {
      pmcLog(pMac, LOGE, "PMC: failure to send message "
         "eWNI_PMC_EXIT_UAPSD_REQ");
      return eHAL_STATUS_FAILURE;
   }

   return eHAL_STATUS_SUCCESS;
}

/*                                                                             
 
                                    
 
              
                                                     
 
             
                                 
 
          
                                                    
                                                        
 
                                                                             */
eHalStatus pmcEnterRequestStandbyState (tHalHandle hHal)
{
   tpAniSirGlobal pMac = PMAC_STRUCT(hHal);

   pmcLog(pMac, LOG2, "PMC: entering pmcEnterRequestStandbyState");

   /*                                                     */
   if (pMac->pmc.pmcState != FULL_POWER)
   {
      pmcLog(pMac, LOGE, "PMC: trying to enter Standby State from "
         "state %d", pMac->pmc.pmcState);
      return eHAL_STATUS_FAILURE;
   }

   //                                                          
   pmcStopTrafficTimer(hHal);

   /*                                                                          
                                                                */
   if (pmcIssueCommand(hHal, eSmeCommandEnterStandby, NULL, 0, FALSE) !=
      eHAL_STATUS_SUCCESS)
   {
      pmcLog(pMac, LOGE, "PMC: failure to send message "
         "eWNI_PMC_ENTER_IMPS_REQ");
      pMac->pmc.pmcState = FULL_POWER;

      if(pmcShouldBmpsTimerRun(pMac))
          (void)pmcStartTrafficTimer(hHal, pMac->pmc.bmpsConfig.trafficMeasurePeriod);
      return eHAL_STATUS_FAILURE;
   }

   return eHAL_STATUS_SUCCESS;
}

/*                                                                             
 
                             
 
              
                                             
 
             
                                 
 
          
                                                    
                                                        
 
                                                                             */
eHalStatus pmcEnterStandbyState (tHalHandle hHal)
{
   tpAniSirGlobal pMac = PMAC_STRUCT(hHal);
   vos_call_status_type callType;
   VOS_STATUS status;

   pmcLog(pMac, LOG2, "PMC: entering pmcEnterStandbyState");

   /*                                                          */
   if (pMac->pmc.pmcState != REQUEST_STANDBY)
   {
      pmcLog(pMac, LOGE, "PMC: trying to enter STANDBY State from state %d",
         pMac->pmc.pmcState);
      return eHAL_STATUS_FAILURE;
   }

   /*               */
   pMac->pmc.pmcState = STANDBY;

   /*                                                               
                                */
   if (pMac->pmc.requestFullPowerPending)
   {
      /*                                  */
      return pmcEnterRequestFullPowerState(hHal, pMac->pmc.requestFullPowerReason);
   }

   //                                                                             
   //              
   status = vos_chipVoteOffRFSupply(&callType, NULL, NULL);
   VOS_ASSERT( VOS_IS_STATUS_SUCCESS( status ) );

   status = vos_chipVoteOffXOBuffer(&callType, NULL, NULL);
   VOS_ASSERT( VOS_IS_STATUS_SUCCESS( status ) );

   pMac->pmc.rfSuppliesVotedOff= TRUE;

   return eHAL_STATUS_SUCCESS;
}

/*                                                                             
 
                              
 
              
                                                  
 
             
                                 
                                         
 
          
            
 
                                                                             */
void pmcDoStandbyCallbacks (tHalHandle hHal, eHalStatus callbackStatus)
{
   tpAniSirGlobal pMac = PMAC_STRUCT(hHal);

   pmcLog(pMac, LOG2, "PMC: entering pmcDoStandbyCallbacks");

   /*                                */
   if (pMac->pmc.standbyCallbackRoutine != NULL)
      pMac->pmc.standbyCallbackRoutine(pMac->pmc.standbyCallbackContext, callbackStatus);
   pMac->pmc.standbyCallbackRoutine = NULL;
   pMac->pmc.standbyCallbackContext = NULL;
}

/*                                                                             
 
                       
 
              
                         
 
             
                                 
 
          
                                                                  
 
                                                                             */
tPmcState pmcGetPmcState (tHalHandle hHal)
{
    tpAniSirGlobal pMac = PMAC_STRUCT(hHal);
    return pMac->pmc.pmcState;
}

const char* pmcGetPmcStateStr(tPmcState state)
{
    switch(state)
    {
        case STOPPED:
            return "STOPPED";
        case FULL_POWER:
            return "FULL_POWER";
        case LOW_POWER:
            return "LOW_POWER";
        case IMPS:
            return "IMPS";
        case BMPS:
            return "BMPS";
        case UAPSD:
            return "UAPSD";
        case STANDBY:
            return "STANDBY";
        case REQUEST_IMPS:
            return "REQUEST_IMPS";
        case REQUEST_BMPS:
            return "REQUEST_BMPS";
        case REQUEST_START_UAPSD:
            return "REQUEST_START_UAPSD";
        case REQUEST_STOP_UAPSD:
            return "REQUEST_STOP_UAPSD";
        case REQUEST_FULL_POWER:
            return "REQUEST_FULL_POWER";
        case REQUEST_STANDBY:
            return "REQUEST_STANDBY";
        case REQUEST_ENTER_WOWL:
            return "REQUEST_ENTER_WOWL";
        case REQUEST_EXIT_WOWL:
            return "REQUEST_EXIT_WOWL";
        case WOWL:
            return "WOWL";
        default:
            break;
    }

    return "UNKNOWN";
}

void pmcDoDeviceStateUpdateCallbacks (tHalHandle hHal, tPmcState state)
{
    tpAniSirGlobal pMac = PMAC_STRUCT(hHal);
    tListElem *pEntry;
    tpDeviceStateUpdateIndEntry pDeviceStateUpdateIndEntry;
    void (*callbackRoutine) (void *callbackContext, tPmcState pmcState);

    pmcLog(pMac, LOG2, FL("PMC - Update registered modules of new device "
           "state: %s"), pmcGetPmcStateStr(state));

    /*                                                            */
    pEntry = csrLLPeekHead(&pMac->pmc.deviceStateUpdateIndList, FALSE);
    while (pEntry != NULL)
    {
        pDeviceStateUpdateIndEntry = GET_BASE_ADDR(pEntry, tDeviceStateUpdateIndEntry, link);
        callbackRoutine = pDeviceStateUpdateIndEntry->callbackRoutine;
        callbackRoutine(pDeviceStateUpdateIndEntry->callbackContext, state);
        pEntry = csrLLNext(&pMac->pmc.deviceStateUpdateIndList, pEntry, FALSE);
    }
}

/*                                                                             
 
                                 
 
              
                                          
 
             
                                 
 
          
                                                    
                                                      
 
                                                                             */
eHalStatus pmcRequestEnterWowlState(tHalHandle hHal, tpSirSmeWowlEnterParams wowlEnterParams)
{
   tpAniSirGlobal pMac = PMAC_STRUCT(hHal);
   pmcLog(pMac, LOG2, "PMC: entering pmcRequestEnterWowlState");

   switch (pMac->pmc.pmcState)
   {
      case FULL_POWER:
         /*                                                             */
         if(pmcEnterRequestBmpsState(hHal) != eHAL_STATUS_SUCCESS)
         {
            pmcLog(pMac, LOGE, "PMC: Device in Full Power. pmcEnterRequestBmpsState failed. "
                    "Cannot enter WOWL");
            return eHAL_STATUS_FAILURE;
         }
         break;

      case REQUEST_BMPS:
         pmcLog(pMac, LOGW, "PMC: BMPS transaction going on. WOWL request "
                    "will be buffered");
         break;

      case BMPS:
      case WOWL:
         /*                                                                      
                                                                                  
                                             */
         if (pmcIssueCommand(hHal, eSmeCommandEnterWowl, wowlEnterParams, sizeof(tSirSmeWowlEnterParams), FALSE) !=
            eHAL_STATUS_SUCCESS)
         {
            pmcLog(pMac, LOGE, "PMC: failure to send message eWNI_PMC_ENTER_WOWL_REQ");
            return eHAL_STATUS_FAILURE;
         }
         break;

      case REQUEST_ENTER_WOWL:
         //                                                              
         pmcLog(pMac, LOGE, "PMC: Enter WOWL transaction already going on. New WOWL request "
                    "will be rejected");
         return eHAL_STATUS_FAILURE;

      case REQUEST_EXIT_WOWL:
         pmcLog(pMac, LOGW, "PMC: Exit WOWL transaction going on. New WOWL request "
                   "will be buffered");
         break;

      default:
         pmcLog(pMac, LOGE, "PMC: Trying to enter WOWL State from state %s",
            pmcGetPmcStateStr(pMac->pmc.pmcState));
         return eHAL_STATUS_FAILURE;
   }

   return eHAL_STATUS_SUCCESS;
}

/*                                                                             
 
                          
 
              
                                          
 
             
                                 
 
          
                                                    
                                                        
 
                                                                             */
eHalStatus pmcEnterWowlState (tHalHandle hHal)
{
   tpAniSirGlobal pMac = PMAC_STRUCT(hHal);

   pmcLog(pMac, LOG2, "PMC: entering pmcEnterWowlState");

   /*                                                    */
   if (pMac->pmc.pmcState != REQUEST_ENTER_WOWL )
   {
      pmcLog(pMac, LOGP, "PMC: trying to enter WOWL State from state %d",
        pMac->pmc.pmcState);
      return eHAL_STATUS_FAILURE;
   }

   /*               */
   pMac->pmc.pmcState = WOWL;

   /*                                     */
   pMac->pmc.wowlModeRequired = FALSE;

   /*                                                               
                             */
   if (pMac->pmc.requestFullPowerPending)
   {
      /*                               */
      return pmcEnterRequestFullPowerState(hHal, pMac->pmc.requestFullPowerReason);
   }

   return eHAL_STATUS_SUCCESS;
}

/*                                                                             
 
                                
 
              
                                     
 
             
                                 
 
          
                                               
                                                 
 
                                                                             */
eHalStatus pmcRequestExitWowlState(tHalHandle hHal)
{
    tpAniSirGlobal pMac = PMAC_STRUCT(hHal);

    pmcLog(pMac, LOG2, "PMC: entering pmcRequestExitWowlState");

    switch (pMac->pmc.pmcState)
    {
        case WOWL:
            /*                                         */
            if (pmcIssueCommand(hHal, eSmeCommandExitWowl, NULL, 0, FALSE) !=
                eHAL_STATUS_SUCCESS)
            {
                pmcLog(pMac, LOGP, "PMC: failure to send message eWNI_PMC_EXIT_WOWL_REQ");
                return eHAL_STATUS_FAILURE;
            }
            break;

        case REQUEST_ENTER_WOWL:
            pmcLog(pMac, LOGP, "PMC: Rcvd exit WOWL even before enter WOWL was completed");
            return eHAL_STATUS_FAILURE;

        default:
            pmcLog(pMac, LOGW, "PMC: Got exit WOWL in state %s. Nothing to do as already out of WOWL",
            pmcGetPmcStateStr(pMac->pmc.pmcState));
            break;
    }

    return eHAL_STATUS_SUCCESS;
}

/*                                                                             
 
                                
 
              
                                
 
             
                                 
 
               
 
                                                                             */
void pmcDoEnterWowlCallbacks (tHalHandle hHal, eHalStatus callbackStatus)
{
   tpAniSirGlobal pMac = PMAC_STRUCT(hHal);

   pmcLog(pMac, LOG2, "PMC: entering pmcDoWowlCallbacks");

   /*                             */
   if (pMac->pmc.enterWowlCallbackRoutine != NULL)
      pMac->pmc.enterWowlCallbackRoutine(pMac->pmc.enterWowlCallbackContext, callbackStatus);

   pMac->pmc.enterWowlCallbackRoutine = NULL;
   pMac->pmc.enterWowlCallbackContext = NULL;
}


static void pmcProcessDeferredMsg( tpAniSirGlobal pMac )
{
    tPmcDeferredMsg *pDeferredMsg;
    tListElem *pEntry;

    while( NULL != ( pEntry = csrLLRemoveHead( &pMac->pmc.deferredMsgList, eANI_BOOLEAN_TRUE ) ) )
    {
        pDeferredMsg = GET_BASE_ADDR( pEntry, tPmcDeferredMsg, link );
        switch (pDeferredMsg->messageType)
        {
        case eWNI_PMC_WOWL_ADD_BCAST_PTRN:
            VOS_ASSERT( pDeferredMsg->size == sizeof(tSirWowlAddBcastPtrn) );
            if (pmcSendMessage(pMac, eWNI_PMC_WOWL_ADD_BCAST_PTRN,
                    &pDeferredMsg->u.wowlAddPattern, sizeof(tSirWowlAddBcastPtrn))
                    != eHAL_STATUS_SUCCESS)
            {
                pmcLog(pMac, LOGE, FL("Send of eWNI_PMC_WOWL_ADD_BCAST_PTRN to PE failed"));
            }
            break;

        case eWNI_PMC_WOWL_DEL_BCAST_PTRN:
            VOS_ASSERT( pDeferredMsg->size == sizeof(tSirWowlDelBcastPtrn) );
            if (pmcSendMessage(pMac, eWNI_PMC_WOWL_DEL_BCAST_PTRN,
                    &pDeferredMsg->u.wowlDelPattern, sizeof(tSirWowlDelBcastPtrn))
                    != eHAL_STATUS_SUCCESS)
            {
                pmcLog(pMac, LOGE, FL("Send of eWNI_PMC_WOWL_ADD_BCAST_PTRN to PE failed"));
            }
            break;

        case eWNI_PMC_PWR_SAVE_CFG:
            VOS_ASSERT( pDeferredMsg->size == sizeof(tSirPowerSaveCfg) );
            if (pmcSendMessage(pMac, eWNI_PMC_PWR_SAVE_CFG,
                    &pDeferredMsg->u.powerSaveConfig, sizeof(tSirPowerSaveCfg))
                != eHAL_STATUS_SUCCESS)
            {
                pmcLog(pMac, LOGE, FL("Send of eWNI_PMC_PWR_SAVE_CFG to PE failed"));
            }
            break;

        default:
            pmcLog(pMac, LOGE, FL("unknown message (%d)"), pDeferredMsg->messageType);
            break;
        }
        //                            
        palFreeMemory( pMac->hHdd, pDeferredMsg );
    } //     
}


eHalStatus pmcDeferMsg( tpAniSirGlobal pMac, tANI_U16 messageType, void *pData, tANI_U32 size)
{
    tPmcDeferredMsg *pDeferredMsg;
    eHalStatus status;

    if( !HAL_STATUS_SUCCESS( palAllocateMemory( pMac->hHdd, (void **)&pDeferredMsg, sizeof(tPmcDeferredMsg) ) ) )
    {
        pmcLog(pMac, LOGE, FL("Cannot allocate memory for callback context"));
        return eHAL_STATUS_RESOURCES;
    }
    palZeroMemory( pMac->hHdd, pDeferredMsg, sizeof(tPmcDeferredMsg) );
    pDeferredMsg->messageType = messageType;
    pDeferredMsg->size = (tANI_U16)size;
    if( pData )
    {
        if( !HAL_STATUS_SUCCESS( palCopyMemory( pMac->hHdd, &pDeferredMsg->u.data,
                                    pData, size ) ) )
        {
            pmcLog(pMac, LOGE, FL("Cannot copy pattern for callback context"));
            palFreeMemory( pMac->hHdd, pDeferredMsg );
            return eHAL_STATUS_FAILURE;
        }
    }
    csrLLInsertTail( &pMac->pmc.deferredMsgList, &pDeferredMsg->link, eANI_BOOLEAN_TRUE );
    //                                                                                      
    //                                  
    status = pmcRequestFullPower( pMac, NULL, NULL, eSME_REASON_OTHER );
    if( eHAL_STATUS_PMC_PENDING != status )
    {
        //                                    
        if( csrLLRemoveEntry( &pMac->pmc.deferredMsgList, &pDeferredMsg->link, eANI_BOOLEAN_TRUE ) )
        {
            palFreeMemory( pMac->hHdd, pDeferredMsg );
        }
        if( !HAL_STATUS_SUCCESS( status ) )
        {
            pmcLog(pMac, LOGE, FL("failed to request full power status = %d"), status);
        }
    }

    return (status);
}

void pmcReleaseCommand( tpAniSirGlobal pMac, tSmeCmd *pCommand )
{
    if(!pCommand->u.pmcCmd.fReleaseWhenDone)
    {
        //                                                      
        pCommand->u.pmcCmd.size = 0;
        smeReleaseCommand( pMac, pCommand );
    }
    else
    {
        //                                                                            
        palFreeMemory(pMac->hHdd, pCommand);
    }
}


//                                                                                   
//                                                                                             
//                           
void pmcAbortCommand( tpAniSirGlobal pMac, tSmeCmd *pCommand, tANI_BOOLEAN fStopping )
{
    if( eSmePmcCommandMask & pCommand->command )
    {
        if( !fStopping )
        {
            switch( pCommand->command )
            {
            case eSmeCommandEnterImps:
                pmcLog(pMac, LOGE, FL("aborting request to enter IMPS"));
                pmcEnterFullPowerState(pMac);
                break;

            case eSmeCommandExitImps:
                pmcLog(pMac, LOGE, FL("aborting request to exit IMPS "));
                pmcEnterFullPowerState(pMac);
                break;

            case eSmeCommandEnterBmps:
                pmcLog(pMac, LOGE, FL("aborting request to enter BMPS "));
                pMac->pmc.bmpsRequestQueued = eANI_BOOLEAN_FALSE;
                pmcEnterFullPowerState(pMac);
                pmcDoBmpsCallbacks(pMac, eHAL_STATUS_FAILURE);
                break;

            case eSmeCommandExitBmps:
                pmcLog(pMac, LOGE, FL("aborting request to exit BMPS "));
                pmcEnterFullPowerState(pMac);
                break;

            case eSmeCommandEnterUapsd:
                pmcLog(pMac, LOGE, FL("aborting request to enter UAPSD "));
                //                                                                                   
                pMac->pmc.uapsdSessionRequired = FALSE;
                pmcDoStartUapsdCallbacks(pMac, eHAL_STATUS_FAILURE);
                break;

            case eSmeCommandExitUapsd:
                pmcLog(pMac, LOGE, FL("aborting request to exit UAPSD "));
                break;

            case eSmeCommandEnterWowl:
                pmcLog(pMac, LOGE, FL("aborting request to enter WOWL "));
                pmcDoEnterWowlCallbacks(pMac, eHAL_STATUS_FAILURE);
                break;

            case eSmeCommandExitWowl:
                pmcLog(pMac, LOGE, FL("aborting request to exit WOWL "));
                break;

            case eSmeCommandEnterStandby:
                pmcLog(pMac, LOGE, FL("aborting request to enter Standby "));
                pmcDoStandbyCallbacks(pMac, eHAL_STATUS_FAILURE);
                break;

            default:
                pmcLog(pMac, LOGE, FL("Request for PMC command (%d) is dropped"), pCommand->command);
                break;
            }
        }//          
        pmcReleaseCommand( pMac, pCommand );
    }
}



//                                                                     
//                                                                                          
#define PMC_IS_COMMAND_CANNOT_FAIL(cmdType)\
    ( (eSmeCommandEnterStandby == (cmdType )) ||\
      (eSmeCommandExitImps == (cmdType )) ||\
      (eSmeCommandExitBmps == (cmdType )) ||\
      (eSmeCommandExitUapsd == (cmdType )) ||\
      (eSmeCommandExitWowl == (cmdType )) )

eHalStatus pmcPrepareCommand( tpAniSirGlobal pMac, eSmeCommandType cmdType, void *pvParam,
                            tANI_U32 size, tSmeCmd **ppCmd )
{
    eHalStatus status = eHAL_STATUS_RESOURCES;
    tSmeCmd *pCommand = NULL;

    VOS_ASSERT( ppCmd );
    do
    {
        pCommand = smeGetCommandBuffer( pMac );
        if ( pCommand )
        {
            //                                         
            pCommand->u.pmcCmd.fReleaseWhenDone = FALSE;
        }
        else
        {
            pmcLog( pMac, LOGE,
                    FL(" fail to get command buffer for command 0x%X curState = %d"), cmdType, pMac->pmc.pmcState );
            //                                       
            if( PMC_IS_COMMAND_CANNOT_FAIL(cmdType) )
            {
                pmcLog( pMac, LOGE,
                        FL(" command 0x%X  cannot fail try allocating memory for it"), cmdType );
                status = palAllocateMemory(pMac->hHdd, (void **)&pCommand, sizeof(tSmeCmd));
                if(!HAL_STATUS_SUCCESS(status))
                {
                    VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_FATAL, "%s fail to allocate memory for command (0x%X)",
                        __func__, cmdType);
                    pCommand = NULL;
                    break;
                }
                palZeroMemory(pMac->hHdd, pCommand, sizeof(tSmeCmd));
                //                                         
                pCommand->u.pmcCmd.fReleaseWhenDone = TRUE;
            }
            else
        {
            break;
        }
        }
        pCommand->command = cmdType;
        pCommand->u.pmcCmd.size = size;
        //                                                                 
        //                              
        pCommand->u.pmcCmd.fullPowerReason = eSME_REASON_OTHER;
        switch ( cmdType )
        {
        case eSmeCommandEnterImps:
        case eSmeCommandExitImps:
        case eSmeCommandEnterBmps:
        case eSmeCommandEnterUapsd:
        case eSmeCommandEnterStandby:
            status = eHAL_STATUS_SUCCESS;
            break;

        case eSmeCommandExitUapsd:
        case eSmeCommandExitWowl:
            status = eHAL_STATUS_SUCCESS;
            if( pvParam )
            {
                pCommand->u.pmcCmd.fullPowerReason = *( (tRequestFullPowerReason *)pvParam );
            }
            break;

        case eSmeCommandExitBmps:
            status = eHAL_STATUS_SUCCESS;
            if( pvParam )
            {
                pCommand->u.pmcCmd.u.exitBmpsInfo = *( (tExitBmpsInfo *)pvParam );
                pCommand->u.pmcCmd.fullPowerReason = pCommand->u.pmcCmd.u.exitBmpsInfo.exitBmpsReason;
            }
            else
            {
                pmcLog( pMac, LOGE, (" exit BMPS must have a reason code") );
            }
            break;

        case eSmeCommandEnterWowl:
            status = eHAL_STATUS_SUCCESS;
            if( pvParam )
            {
                pCommand->u.pmcCmd.u.enterWowlInfo = *( ( tSirSmeWowlEnterParams * )pvParam );
            }
            break;

        default:
            pmcLog( pMac, LOGE, FL("  invalid command type %d"), cmdType );
            status = eHAL_STATUS_INVALID_PARAMETER;
            break;
        }

    } while( 0 );

    if( HAL_STATUS_SUCCESS( status ) && pCommand )
    {
        *ppCmd = pCommand;
    }
    else if( pCommand )
    {
        pmcReleaseCommand( pMac, pCommand );
    }

    return (status);
}


eHalStatus pmcIssueCommand( tpAniSirGlobal pMac, eSmeCommandType cmdType, void *pvParam,
                            tANI_U32 size, tANI_BOOLEAN fPutToListHead )
{
    eHalStatus status = eHAL_STATUS_RESOURCES;
    tSmeCmd *pCommand = NULL;

    status = pmcPrepareCommand( pMac, cmdType, pvParam, size, &pCommand );
    if( HAL_STATUS_SUCCESS( status ) && pCommand )
    {
        smePushCommand( pMac, pCommand, fPutToListHead );
    }
    else if( pCommand )
    {
        pmcReleaseCommand( pMac, pCommand );
    }

    return( status );
}



tANI_BOOLEAN pmcProcessCommand( tpAniSirGlobal pMac, tSmeCmd *pCommand )
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    VOS_STATUS vstatus;
    tANI_BOOLEAN fRemoveCmd = eANI_BOOLEAN_TRUE;

    do
    {
        switch ( pCommand->command )
        {
        case eSmeCommandEnterImps:
            if( FULL_POWER == pMac->pmc.pmcState )
            {
                status = pmcEnterImpsCheck( pMac );
                if( HAL_STATUS_SUCCESS( status ) )
                {
                    /*               */
                    pMac->pmc.pmcState = REQUEST_IMPS;
                    status = pmcSendMessage(pMac, eWNI_PMC_ENTER_IMPS_REQ, NULL, 0);
                    if( HAL_STATUS_SUCCESS( status ) )
                    {
                        /*                                                                       
                                                                                  */
                        if ( FULL_POWER != pMac->pmc.pmcState )
                        {
                            fRemoveCmd = eANI_BOOLEAN_FALSE;
                        }
                    }
                }
                if( !HAL_STATUS_SUCCESS( status ) )
                {
                    pmcLog(pMac, LOGE,
                           "PMC: failure to send message eWNI_PMC_ENTER_IMPS_REQ or pmcEnterImpsCheck failed");
                    pmcEnterFullPowerState( pMac );
                    if(pmcShouldBmpsTimerRun(pMac))
                        (void)pmcStartTrafficTimer(pMac, pMac->pmc.bmpsConfig.trafficMeasurePeriod);
                }
            }//          
            break;

        case eSmeCommandExitImps:
            pMac->pmc.requestFullPowerPending = FALSE;
            if( ( IMPS == pMac->pmc.pmcState ) || ( STANDBY == pMac->pmc.pmcState ) )
            {
                //                                                                   
                if( STANDBY == pMac->pmc.pmcState )
                {
                    //                       
                    csrScanResumeIMPS(pMac);
                }

                status = pmcSendMessage(pMac, eWNI_PMC_EXIT_IMPS_REQ, NULL, 0);
                if ( HAL_STATUS_SUCCESS( status ) )
                {
                    pMac->pmc.pmcState = REQUEST_FULL_POWER;
                    pmcLog(pMac, LOG2, FL("eWNI_PMC_EXIT_IMPS_REQ sent to PE"));
                    fRemoveCmd = eANI_BOOLEAN_FALSE;
                }
                else
                {
                    pmcLog(pMac, LOGE,
                           FL("eWNI_PMC_EXIT_IMPS_REQ fail to be sent to PE status %d"), status);
                    //                                                                             
                    pmcEnterFullPowerState(pMac);
                }
            }
            break;

        case eSmeCommandEnterBmps:
#ifdef CUSTOMER_LGE_DEBUG_LOG
            pmcLog(pMac, LOGE, "Request EnterBMPS,  PMC Status = %d", pMac->pmc.pmcState);
#endif
            if( FULL_POWER == pMac->pmc.pmcState )
            {
                //                                                                       
                status = pmcEnterBmpsCheck( pMac );
                if( HAL_STATUS_SUCCESS( status ) )
                {
                    /*                  */
                    pMac->pmc.pmcState = REQUEST_BMPS;
                    pmcLog(pMac, LOG2, "PMC: Enter BMPS req done: Force XO Core ON");
                    vstatus = vos_chipVoteXOCore(NULL, NULL, NULL, VOS_TRUE);
                    if ( !VOS_IS_STATUS_SUCCESS(vstatus) )
                    {
                        pmcLog(pMac, LOGE, "Could not turn XO Core ON. Can't go to BMPS");
                    }
                    else /*                                */
                    {
                    /*                                          */
                    status = pmcSendMessage(pMac, eWNI_PMC_ENTER_BMPS_REQ, NULL, 0);
                    if ( HAL_STATUS_SUCCESS( status ) )
                    {
                        fRemoveCmd = eANI_BOOLEAN_FALSE;
                    }
                    else
                    {
                        pmcLog(pMac, LOGE, "Fail to send enter BMPS msg to PE");
                            /*                             */
                            pmcLog(pMac, LOGW, "In module init: Cancel the vote for XO CORE ON "
                                                             "since send enter bmps failed");
                            if (vos_chipVoteXOCore(NULL, NULL, NULL, VOS_FALSE) != VOS_STATUS_SUCCESS)
                            {
                                pmcLog(pMac, LOGE, "Could not cancel XO Core ON vote."
                                                   "Not returning failure."
                                                   "Power consumed will be high");
                            }

                        }
                    }
                }
                if( !HAL_STATUS_SUCCESS( status ) )
                {
                    pmcLog(pMac, LOGE,
                           "PMC: failure to send message eWNI_PMC_ENTER_BMPS_REQ status %d", status);
                    pMac->pmc.bmpsRequestQueued = eANI_BOOLEAN_FALSE;
                    pmcEnterFullPowerState(pMac);
                    //                                                       
                    pmcDoBmpsCallbacks(pMac, eHAL_STATUS_FAILURE);
                    if(pmcShouldBmpsTimerRun(pMac))
                        (void)pmcStartTrafficTimer(pMac, pMac->pmc.bmpsConfig.trafficMeasurePeriod);
                }
            }
            break;

        case eSmeCommandExitBmps:
#ifdef CUSTOMER_LGE_DEBUG_LOG
            pmcLog(pMac, LOGE, "Request ExitBMPS,  PMC Status = %d", pMac->pmc.pmcState);
#endif
            if( BMPS == pMac->pmc.pmcState )
            {
                pMac->pmc.requestFullPowerPending = FALSE;

                status = pmcSendMessage( pMac, eWNI_PMC_EXIT_BMPS_REQ,
                            &pCommand->u.pmcCmd.u.exitBmpsInfo, sizeof(tExitBmpsInfo) );
                if ( HAL_STATUS_SUCCESS( status ) )
                {
                    pMac->pmc.pmcState = REQUEST_FULL_POWER;
                    fRemoveCmd = eANI_BOOLEAN_FALSE;
                    pmcLog(pMac, LOG2, FL("eWNI_PMC_EXIT_BMPS_REQ sent to PE"));

                }
                else
                {
                    pmcLog(pMac, LOGE, FL("eWNI_PMC_EXIT_BMPS_REQ fail to be sent to PE status %d"), status);
                    pmcEnterFullPowerState(pMac);
                }
            }
            break;

        case eSmeCommandEnterUapsd:
            if( BMPS == pMac->pmc.pmcState )
            {
                pMac->pmc.uapsdSessionRequired = TRUE;
                status = pmcSendMessage(pMac, eWNI_PMC_ENTER_UAPSD_REQ, NULL, 0);
                if ( HAL_STATUS_SUCCESS( status ) )
                {
                    pMac->pmc.pmcState = REQUEST_START_UAPSD;
                    fRemoveCmd = eANI_BOOLEAN_FALSE;
                }
                else
                {
                    pmcLog(pMac, LOGE, "PMC: failure to send message "
                       "eWNI_PMC_ENTER_BMPS_REQ");
                    //                                                                                        
                    pMac->pmc.uapsdSessionRequired = FALSE;
                    pmcDoStartUapsdCallbacks(pMac, eHAL_STATUS_FAILURE);
                }
            }
            break;

        case eSmeCommandExitUapsd:
           if( UAPSD == pMac->pmc.pmcState )
           {
               pMac->pmc.requestFullPowerPending = FALSE;
                /*                                                 */
               if (pMac->pmc.pmcState == REQUEST_STOP_UAPSD)
               {
                   break;
               }

               /*                                          */
               status = pmcSendMessage(pMac, eWNI_PMC_EXIT_UAPSD_REQ, NULL, 0);
               if ( HAL_STATUS_SUCCESS( status ) )
               {
                   /*                                                                
                                                                                   */
                   pMac->pmc.pmcState = REQUEST_STOP_UAPSD;
                   pMac->pmc.requestFullPowerPending = TRUE;
                   pMac->pmc.requestFullPowerReason = pCommand->u.pmcCmd.fullPowerReason;
                   fRemoveCmd = eANI_BOOLEAN_FALSE;
               }
               else
               {
                   pmcLog(pMac, LOGE, "PMC: failure to send message "
                      "eWNI_PMC_EXIT_UAPSD_REQ");
                   pmcEnterBmpsState(pMac);
               }
           }

           break;

        case eSmeCommandEnterWowl:
            if( ( BMPS == pMac->pmc.pmcState ) || ( WOWL == pMac->pmc.pmcState ) )
            {
                status = pmcSendMessage(pMac, eWNI_PMC_ENTER_WOWL_REQ,
                        &pCommand->u.pmcCmd.u.enterWowlInfo, sizeof(tSirSmeWowlEnterParams));
                if ( HAL_STATUS_SUCCESS( status ) )
                {
                    pMac->pmc.pmcState = REQUEST_ENTER_WOWL;
                    fRemoveCmd = eANI_BOOLEAN_FALSE;
                }
                else
                {
                    pmcLog(pMac, LOGE, "PMC: failure to send message eWNI_PMC_ENTER_WOWL_REQ");
                    pmcDoEnterWowlCallbacks(pMac, eHAL_STATUS_FAILURE);
                }
            }
            else
            {
                fRemoveCmd = eANI_BOOLEAN_TRUE;
            }
            break;

        case eSmeCommandExitWowl:
            if( WOWL == pMac->pmc.pmcState )
            {
                pMac->pmc.requestFullPowerPending = FALSE;
                pMac->pmc.pmcState = REQUEST_EXIT_WOWL;
                status = pmcSendMessage(pMac, eWNI_PMC_EXIT_WOWL_REQ, NULL, 0);
                if ( HAL_STATUS_SUCCESS( status ) )
                {
                    fRemoveCmd = eANI_BOOLEAN_FALSE;
                    pMac->pmc.requestFullPowerPending = TRUE;
                    pMac->pmc.requestFullPowerReason = pCommand->u.pmcCmd.fullPowerReason;
                }
                else
                {
                    pmcLog(pMac, LOGP, "PMC: failure to send message eWNI_PMC_EXIT_WOWL_REQ");
                    pmcEnterBmpsState(pMac);
                }
            }
            break;

        case eSmeCommandEnterStandby:
            if( FULL_POWER == pMac->pmc.pmcState )
            {
               //                                                                             
               //                                                                                       
               //                                                                                          
               //                                                                                           
               //                                                                                            
               //                                     
               if (csrIsIBSSStarted(pMac) || csrIsBTAMPStarted(pMac))
               {
                  VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_FATAL,
                      "WLAN: IBSS or BT-AMP session present. Cannot honor standby request");

                  pmcDoStandbyCallbacks(pMac, eHAL_STATUS_PMC_NOT_NOW);
                  if(pmcShouldBmpsTimerRun(pMac))
                      (void)pmcStartTrafficTimer(pMac, pMac->pmc.bmpsConfig.trafficMeasurePeriod);
                  break;
               }

                //                                                          
                pmcStopTrafficTimer(pMac);

                /*               */
                pMac->pmc.pmcState = REQUEST_STANDBY;

                /*                                                                          
                                                                            */
                status = pmcSendMessage(pMac, eWNI_PMC_ENTER_IMPS_REQ, NULL, 0);
                if ( HAL_STATUS_SUCCESS( status ) )
                {
                    //                        
                    csrScanSuspendIMPS(pMac);
                    fRemoveCmd = eANI_BOOLEAN_FALSE;
                }
                else
                {
                    pmcLog(pMac, LOGE, "PMC: failure to send message "
                        "eWNI_PMC_ENTER_IMPS_REQ");
                    pmcEnterFullPowerState(pMac);
                    pmcDoStandbyCallbacks(pMac, eHAL_STATUS_FAILURE);
                    /*                                                                            
                              */
                    if(pmcShouldBmpsTimerRun(pMac))
                        (void)pmcStartTrafficTimer(pMac, pMac->pmc.bmpsConfig.trafficMeasurePeriod);
                }
            }
            break;

        default:
            pmcLog( pMac, LOGE, FL("  invalid command type %d"), pCommand->command );
            break;
        }

    } while( 0 );

    return( fRemoveCmd );
}

eHalStatus pmcEnterImpsCheck( tpAniSirGlobal pMac )
{

    if( !PMC_IS_READY(pMac) )
    {
        pmcLog(pMac, LOGE, FL("Requesting IMPS when PMC not ready"));
        pmcLog(pMac, LOGE, FL("pmcReady = %d pmcState = %s"),
            pMac->pmc.pmcReady, pmcGetPmcStateStr(pMac->pmc.pmcState));
        return eHAL_STATUS_FAILURE;
    }

    /*                           */
    if (!pMac->pmc.impsEnabled)
    {
        pmcLog(pMac, LOG2, FL("IMPS is disabled"));
        return eHAL_STATUS_PMC_DISABLED;
    }

    /*                                                 */
    if ((pMac->pmc.powerSource == AC_POWER) && !pMac->pmc.impsConfig.enterOnAc)
    {
        pmcLog(pMac, LOG2, FL("IMPS is disabled when operating on AC power"));
        return eHAL_STATUS_PMC_AC_POWER;
    }

    /*                                                                  */
    if (!pmcPowerSaveCheck(pMac))
    {
        pmcLog(pMac, LOG2, FL("IMPS cannot be entered now"));
        return eHAL_STATUS_PMC_NOT_NOW;
    }

    /*                                                                       
                               */
    if (!pmcAllowImps(pMac))
    {
        pmcLog(pMac, LOG2, FL("IMPS cannot be entered now"));
        return eHAL_STATUS_PMC_NOT_NOW;
    }

    /*                           */
    if ((pMac->pmc.pmcState == REQUEST_IMPS) || (pMac->pmc.pmcState == IMPS) ||
        (pMac->pmc.pmcState == REQUEST_FULL_POWER))
    {
        pmcLog(pMac, LOG2, FL("Already in IMPS"));
        return eHAL_STATUS_PMC_ALREADY_IN_IMPS;
    }

    /*                                                 */
    if(vos_is_load_unload_in_progress( VOS_MODULE_ID_VOSS, NULL))
    {
       pmcLog(pMac, LOGW, FL("Driver load/unload is in progress"));
       return eHAL_STATUS_PMC_NOT_NOW;
    }

    return ( eHAL_STATUS_SUCCESS );
}

/*                                                                                       
                                                                                         
                  */
eHalStatus pmcEnterBmpsCheck( tpAniSirGlobal pMac )
{

   /*                           */
   if (!pMac->pmc.bmpsEnabled)
   {
      pmcLog(pMac, LOGE, "PMC: Cannot initiate BMPS. BMPS is disabled");
      return eHAL_STATUS_PMC_DISABLED;
   }

   if( !PMC_IS_READY(pMac) )
   {
       pmcLog(pMac, LOGE, FL("Requesting BMPS when PMC not ready"));
       pmcLog(pMac, LOGE, FL("pmcReady = %d pmcState = %s"),
           pMac->pmc.pmcReady, pmcGetPmcStateStr(pMac->pmc.pmcState));
       return eHAL_STATUS_FAILURE;
   }

   /*                                                            */
   if (!pmcValidateConnectState( pMac ))
   {
      pmcLog(pMac, LOGE, "PMC: STA not associated with an AP with single active session. BMPS cannot be entered");
      return eHAL_STATUS_FAILURE;
   }

   /*                                                         */
   if (pMac->pmc.pmcState != FULL_POWER)
   {
      pmcLog(pMac, LOGE,
             "PMC: Device not in full power. Cannot request BMPS. pmcState %d", pMac->pmc.pmcState);
      return eHAL_STATUS_FAILURE;
   }
   /*                                                                  */
   if (!pmcPowerSaveCheck(pMac))
   {
      pmcLog(pMac, LOGE, "PMC: Power save check failed. BMPS cannot be entered now");
      return eHAL_STATUS_PMC_NOT_NOW;
   }

    //                                                      
    //                          
    if(!IS_FEATURE_SUPPORTED_BY_FW(SLM_SESSIONIZATION))
    {
        pmcLog(pMac, LOG1, FL("doBMPSWorkaround %u"), pMac->roam.configParam.doBMPSWorkaround);
        if (pMac->roam.configParam.doBMPSWorkaround)
        {
            pMac->roam.configParam.doBMPSWorkaround = 0;
            pmcLog(pMac, LOG1,
                   FL("reset doBMPSWorkaround to disabled %u"), pMac->roam.configParam.doBMPSWorkaround);
            csrDisconnectAllActiveSessions(pMac);
            pmcLog(pMac, LOGE,
                   "PMC: doBMPSWorkaround was enabled. First Disconnect all sessions. pmcState %d", pMac->pmc.pmcState);
            return eHAL_STATUS_FAILURE;
        }
     }

   return ( eHAL_STATUS_SUCCESS );
}

tANI_BOOLEAN pmcShouldBmpsTimerRun( tpAniSirGlobal pMac )
{
    /*                                                                   
                                                                         
                                                                          
                                                
     */
    if (!(pMac->pmc.bmpsEnabled && (pMac->pmc.autoBmpsEntryEnabled ||
          pMac->pmc.uapsdSessionRequired || pMac->pmc.bmpsRequestedByHdd ||
          pMac->pmc.wowlModeRequired )))
    {
        pmcLog(pMac, LOG1, FL("BMPS is not enabled or not required"));
        return eANI_BOOLEAN_FALSE;
    }

    if(pMac->pmc.isHostPsEn && pMac->pmc.remainInPowerActiveTillDHCP)
    {
        pmcLog(pMac, LOG1,
               FL("Host controlled ps enabled and host wants active mode, so dont allow BMPS"));
        return eANI_BOOLEAN_FALSE;
    }

    if ((vos_concurrent_sessions_running()) &&
        ((csrIsConcurrentInfraConnected( pMac ) ||
        (vos_get_concurrency_mode()& VOS_SAP) ||
        (vos_get_concurrency_mode()& VOS_P2P_GO))))
    {
        pmcLog(pMac, LOG1, FL("Multiple Sessions/GO/SAP sessions . BMPS should not be started"));
        return eANI_BOOLEAN_FALSE;
    }
    /*                                                                      
                        */
    if (!csrIsInfraConnected(pMac))
    {
        pmcLog(pMac, LOG1, FL("No Infra Session or multiple sessions. BMPS should not be started"));
        return eANI_BOOLEAN_FALSE;
    }
    return eANI_BOOLEAN_TRUE;
}


#ifdef FEATURE_WLAN_DIAG_SUPPORT

#define PMC_DIAG_EVT_TIMER_INTERVAL ( 5000 )

void pmcDiagEvtTimerExpired (tHalHandle hHal)
{
    tpAniSirGlobal pMac = PMAC_STRUCT(hHal);
    WLAN_VOS_DIAG_EVENT_DEF(psRequest, vos_event_wlan_powersave_payload_type);

    vos_mem_zero(&psRequest, sizeof(vos_event_wlan_powersave_payload_type));
    psRequest.event_subtype = WLAN_PMC_CURRENT_STATE;
    psRequest.pmc_current_state = pMac->pmc.pmcState;

    WLAN_VOS_DIAG_EVENT_REPORT(&psRequest, EVENT_WLAN_POWERSAVE_GENERIC);

    pmcLog(pMac, LOGW, FL("DIAG event timer expired"));

    /*              */
    if (pmcStartDiagEvtTimer(hHal) != eHAL_STATUS_SUCCESS)
    {
        pmcLog(pMac, LOGP, FL("Cannot re-arm DIAG evt timer"));
    }
    vos_timer_start(&pMac->pmc.hDiagEvtTimer, PMC_DIAG_EVT_TIMER_INTERVAL);
}

eHalStatus pmcStartDiagEvtTimer (tHalHandle hHal)
{
    tpAniSirGlobal pMac = PMAC_STRUCT(hHal);

    pmcLog(pMac, LOG2, FL("Entering pmcStartDiagEvtTimer"));

    if ( vos_timer_start(&pMac->pmc.hDiagEvtTimer, PMC_DIAG_EVT_TIMER_INTERVAL) != VOS_STATUS_SUCCESS)
    {
       pmcLog(pMac, LOGP, FL("Cannot start DIAG evt timer"));
       return eHAL_STATUS_FAILURE;
    }

    return eHAL_STATUS_SUCCESS;
}

void pmcStopDiagEvtTimer (tHalHandle hHal)
{
    tpAniSirGlobal pMac = PMAC_STRUCT(hHal);
    pmcLog(pMac, LOG2, FL("Entering pmcStopDiagEvtTimer"));
    (void)vos_timer_stop(&pMac->pmc.hDiagEvtTimer);
}
#endif

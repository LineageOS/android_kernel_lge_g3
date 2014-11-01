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
 * Copyright (c) 2011-2013 Qualcomm Atheros, Inc.
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 *
 * Airgo Networks, Inc proprietary. All rights reserved.
 * limSendMessages.c: Provides functions to send messages or Indications to HAL.
 * Author:    Sunit Bhatia
 * Date:       09/21/2006
 * History:-
 * Date        Modified by            Modification Information
 *
 * --------------------------------------------------------------------------
 *
 */
#include "limSendMessages.h"
#include "cfgApi.h"
#include "limTrace.h"
#ifdef FEATURE_WLAN_DIAG_SUPPORT_LIM //                         
#include "vos_diag_core_log.h"
#endif //                          
/*                                                
                                                     
                                                      
                              
           
                             
                   
                  
                
                     
 */
static tBeaconFilterIe beaconFilterTable[] = {
   {SIR_MAC_DS_PARAM_SET_EID,    0, {0, 0, DS_PARAM_CHANNEL_MASK, 0}},
   {SIR_MAC_ERP_INFO_EID,        0, {0, 0, ERP_FILTER_MASK,       0}},
   {SIR_MAC_EDCA_PARAM_SET_EID,  0, {0, 0, EDCA_FILTER_MASK,      0}},
   {SIR_MAC_QOS_CAPABILITY_EID,  0, {0, 0, QOS_FILTER_MASK,       0}},
   {SIR_MAC_CHNL_SWITCH_ANN_EID, 1, {0, 0, 0,                     0}},
   {SIR_MAC_HT_INFO_EID,         0, {0, 0, HT_BYTE0_FILTER_MASK,  0}},  
   {SIR_MAC_HT_INFO_EID,         0, {2, 0, HT_BYTE2_FILTER_MASK,  0}}, 
   {SIR_MAC_HT_INFO_EID,         0, {5, 0, HT_BYTE5_FILTER_MASK,  0}}
#if defined WLAN_FEATURE_VOWIFI
   ,{SIR_MAC_PWR_CONSTRAINT_EID,  0, {0, 0, 0, 0}}
#endif
#ifdef WLAN_FEATURE_11AC
   ,{SIR_MAC_VHT_OPMODE_EID,     0,  {0, 0, 0, 0}}
   ,{SIR_MAC_VHT_OPERATION_EID,  0,  {0, 0, VHTOP_CHWIDTH_MASK, 0}}
#endif
};

/* 
                    
  
           
                                                                                
  
        
  
              
     
  
       
     
  
                                                
                                                                 
                                                 
                                                  
  
                                                     
 */
tSirRetStatus limSendCFParams(tpAniSirGlobal pMac, tANI_U8 bssIdx, tANI_U8 cfpCount, tANI_U8 cfpPeriod)
{
    tpUpdateCFParams pCFParams = NULL;
    tSirRetStatus   retCode = eSIR_SUCCESS;
    tSirMsgQ msgQ;

    pCFParams = vos_mem_malloc(sizeof( tUpdateCFParams ));
    if ( NULL == pCFParams )
      {
        limLog( pMac, LOGP,
            FL( "Unable to allocate memory during Update CF Params" ));
        retCode = eSIR_MEM_ALLOC_FAILED;
        goto returnFailure;
      }
    vos_mem_set( (tANI_U8 *) pCFParams, sizeof(tUpdateCFParams), 0);
    pCFParams->cfpCount = cfpCount;
    pCFParams->cfpPeriod = cfpPeriod;
    pCFParams->bssIdx     = bssIdx;

    msgQ.type = WDA_UPDATE_CF_IND;
    msgQ.reserved = 0;
    msgQ.bodyptr = pCFParams;
    msgQ.bodyval = 0;
    limLog( pMac, LOG3,
                FL( "Sending WDA_UPDATE_CF_IND..." ));
    MTRACE(macTraceMsgTx(pMac, NO_SESSION, msgQ.type));
    if( eSIR_SUCCESS != (retCode = wdaPostCtrlMsg( pMac, &msgQ )))
    {
        vos_mem_free(pCFParams);
        limLog( pMac, LOGP,
                    FL("Posting  WDA_UPDATE_CF_IND to WDA failed, reason=%X"),
                    retCode );
    }
returnFailure:
    return retCode;
}

/* 
                        
  
           
                                                                            
                                                                             
  
        
  
              
     
  
       
     
  
                                                
                                                        
                                                                       
  
                                                     
 */
tSirRetStatus limSendBeaconParams(tpAniSirGlobal pMac, 
                                  tpUpdateBeaconParams pUpdatedBcnParams,
                                  tpPESession  psessionEntry )
{
    tpUpdateBeaconParams pBcnParams = NULL;
    tSirRetStatus   retCode = eSIR_SUCCESS;
    tSirMsgQ msgQ;

    pBcnParams = vos_mem_malloc(sizeof(*pBcnParams));
    if ( NULL == pBcnParams )
    {
        limLog( pMac, LOGP,
            FL( "Unable to allocate memory during Update Beacon Params" ));
        return eSIR_MEM_ALLOC_FAILED;
    }
    vos_mem_copy((tANI_U8 *) pBcnParams,  pUpdatedBcnParams, sizeof(*pBcnParams));
    msgQ.type = WDA_UPDATE_BEACON_IND;
    msgQ.reserved = 0;
    msgQ.bodyptr = pBcnParams;
    msgQ.bodyval = 0;
    PELOG3(limLog( pMac, LOG3,
                FL( "Sending WDA_UPDATE_BEACON_IND, paramChangeBitmap in hex = %x" ),
                    pUpdatedBcnParams->paramChangeBitmap);)
    if(NULL == psessionEntry)
    {
        MTRACE(macTraceMsgTx(pMac, NO_SESSION, msgQ.type));
    }
    else
    {
        MTRACE(macTraceMsgTx(pMac, psessionEntry->peSessionId, msgQ.type));
    }
    if( eSIR_SUCCESS != (retCode = wdaPostCtrlMsg( pMac, &msgQ )))
    {
        vos_mem_free(pBcnParams);
        limLog( pMac, LOGP,
                    FL("Posting  WDA_UPDATE_BEACON_IND to WDA failed, reason=%X"),
                    retCode );
    }
    limSendBeaconInd(pMac, psessionEntry);
    return retCode;
}

/* 
                            
  
           
                                                                   
  
        
  
              
     
  
       
     
  
                                                
                                                          
                                                                    
                                                               
  
                                                     
 */
#if !defined WLAN_FEATURE_VOWIFI  
tSirRetStatus limSendSwitchChnlParams(tpAniSirGlobal pMac,
                                      tANI_U8 chnlNumber,
                                      ePhyChanBondState secondaryChnlOffset,
                                      tANI_U8 localPwrConstraint, tANI_U8 peSessionId)
#else
tSirRetStatus limSendSwitchChnlParams(tpAniSirGlobal pMac,
                                      tANI_U8 chnlNumber,
                                      ePhyChanBondState secondaryChnlOffset,
                                      tPowerdBm maxTxPower, tANI_U8 peSessionId)
#endif
{
    tpSwitchChannelParams pChnlParams = NULL;
    tSirRetStatus   retCode = eSIR_SUCCESS;
    tSirMsgQ msgQ;
    tpPESession pSessionEntry;
    if((pSessionEntry = peFindSessionBySessionId(pMac, peSessionId)) == NULL)
    {
       limLog( pMac, LOGP,
             FL( "Unable to get Session for session Id %d" ), peSessionId);
       return eSIR_FAILURE;
    }
    pChnlParams = vos_mem_malloc(sizeof( tSwitchChannelParams ));
    if ( NULL == pChnlParams )
    {
        limLog( pMac, LOGP,
            FL( "Unable to allocate memory during Switch Channel Params" ));
        retCode = eSIR_MEM_ALLOC_FAILED;
        goto returnFailure;
    }
    vos_mem_set((tANI_U8 *) pChnlParams, sizeof(tSwitchChannelParams), 0);
    pChnlParams->secondaryChannelOffset = secondaryChnlOffset;
    pChnlParams->channelNumber= chnlNumber;
#if defined WLAN_FEATURE_VOWIFI  
    pChnlParams->maxTxPower = maxTxPower;
    vos_mem_copy( pChnlParams->selfStaMacAddr, pSessionEntry->selfMacAddr, sizeof(tSirMacAddr) );
#else
    pChnlParams->localPowerConstraint = localPwrConstraint;
#endif
    vos_mem_copy(  pChnlParams->bssId, pSessionEntry->bssId, sizeof(tSirMacAddr) );
    pChnlParams->peSessionId = peSessionId;
    
    //                                                                     
    SET_LIM_PROCESS_DEFD_MESGS(pMac, false);
    msgQ.type = WDA_CHNL_SWITCH_REQ;
    msgQ.reserved = 0;
    msgQ.bodyptr = pChnlParams;
    msgQ.bodyval = 0;
#if defined WLAN_FEATURE_VOWIFI  
    PELOG3(limLog( pMac, LOG3,
        FL( "Sending WDA_CHNL_SWITCH_REQ with SecondaryChnOffset - %d, ChannelNumber - %d, maxTxPower - %d"),
        pChnlParams->secondaryChannelOffset, pChnlParams->channelNumber, pChnlParams->maxTxPower);)
#else
    PELOG3(limLog( pMac, LOG3,
        FL( "Sending WDA_CHNL_SWITCH_REQ with SecondaryChnOffset - %d, ChannelNumber - %d, LocalPowerConstraint - %d"),
        pChnlParams->secondaryChannelOffset, pChnlParams->channelNumber, pChnlParams->localPowerConstraint);)
#endif
    MTRACE(macTraceMsgTx(pMac, peSessionId, msgQ.type));
    if( eSIR_SUCCESS != (retCode = wdaPostCtrlMsg( pMac, &msgQ )))
    {
        vos_mem_free(pChnlParams);
        limLog( pMac, LOGP,
                    FL("Posting  WDA_CHNL_SWITCH_REQ to WDA failed, reason=%X"),
                    retCode );
    }
returnFailure:
    return retCode;
}

/* 
                      
  
           
                                                                               
  
        
  
              
     
  
       
     
  
                                                
                                                                     
                                                                              
                                                                                                
  
                                                     
 */
tSirRetStatus limSendEdcaParams(tpAniSirGlobal pMac, tSirMacEdcaParamRecord *pUpdatedEdcaParams, tANI_U16 bssIdx, tANI_BOOLEAN highPerformance)
{
    tEdcaParams *pEdcaParams = NULL;
    tSirRetStatus   retCode = eSIR_SUCCESS;
    tSirMsgQ msgQ;

    pEdcaParams = vos_mem_malloc(sizeof(tEdcaParams));
    if ( NULL == pEdcaParams )
    {
        limLog( pMac, LOGP,
            FL( "Unable to allocate memory during Update EDCA Params" ));
        retCode = eSIR_MEM_ALLOC_FAILED;
        return retCode;
    }
    pEdcaParams->bssIdx = bssIdx;
    pEdcaParams->acbe = pUpdatedEdcaParams[EDCA_AC_BE];
    pEdcaParams->acbk = pUpdatedEdcaParams[EDCA_AC_BK];
    pEdcaParams->acvi = pUpdatedEdcaParams[EDCA_AC_VI];
    pEdcaParams->acvo = pUpdatedEdcaParams[EDCA_AC_VO];
    pEdcaParams->highPerformance = highPerformance;
    msgQ.type = WDA_UPDATE_EDCA_PROFILE_IND;
    msgQ.reserved = 0;
    msgQ.bodyptr = pEdcaParams;
    msgQ.bodyval = 0;
    {
        tANI_U8 i;
        PELOG1(limLog( pMac, LOG1,FL("Sending WDA_UPDATE_EDCA_PROFILE_IND with EDCA Parameters:" ));)
        for(i=0; i<MAX_NUM_AC; i++)
        {
            PELOG1(limLog(pMac, LOG1, FL("AC[%d]:  AIFSN %d, ACM %d, CWmin %d, CWmax %d, TxOp %d "),
                   i, pUpdatedEdcaParams[i].aci.aifsn, pUpdatedEdcaParams[i].aci.acm, 
                   pUpdatedEdcaParams[i].cw.min, pUpdatedEdcaParams[i].cw.max, pUpdatedEdcaParams[i].txoplimit);)
        }
    }
    MTRACE(macTraceMsgTx(pMac, NO_SESSION, msgQ.type));
    if( eSIR_SUCCESS != (retCode = wdaPostCtrlMsg( pMac, &msgQ )))
    {
        vos_mem_free(pEdcaParams);
        limLog( pMac, LOGP,
                    FL("Posting  WDA_UPDATE_EDCA_PROFILE_IND to WDA failed, reason=%X"),
                    retCode );
    }
    return retCode;
}

/* 
                           
  
            
                                                                      
                                                                         
                                                                              
                                                                     
                                     
                                                                      
                                                         
  
                                                
                                                               
                                                   
               
 */
 void limSetActiveEdcaParams(tpAniSirGlobal pMac, tSirMacEdcaParamRecord *plocalEdcaParams, tpPESession psessionEntry)
{
    tANI_U8   ac, newAc, i;
    tANI_U8   acAdmitted;
#ifdef FEATURE_WLAN_DIAG_SUPPORT_LIM //                         
    vos_log_qos_edca_pkt_type *log_ptr = NULL;
#endif //                          
    //                                                                
    psessionEntry->gLimEdcaParamsActive[EDCA_AC_BE] = plocalEdcaParams[EDCA_AC_BE];
    psessionEntry->gLimEdcaParamsActive[EDCA_AC_BK] = plocalEdcaParams[EDCA_AC_BK];
    psessionEntry->gLimEdcaParamsActive[EDCA_AC_VI] = plocalEdcaParams[EDCA_AC_VI];
    psessionEntry->gLimEdcaParamsActive[EDCA_AC_VO] = plocalEdcaParams[EDCA_AC_VO];
    /*                                                                   
                                                    
                                                                          
                                    
      
                                                                            
                                       
                                                                           
                                                                           
                                                                               
                                                          
                                                 
     */
    limLog(pMac, LOG1, FL("adAdmitMask[UPLINK] = 0x%x "),  pMac->lim.gAcAdmitMask[SIR_MAC_DIRECTION_UPLINK] );
    limLog(pMac, LOG1, FL("adAdmitMask[DOWNLINK] = 0x%x "),  pMac->lim.gAcAdmitMask[SIR_MAC_DIRECTION_DNLINK] );
    for (ac = EDCA_AC_BK; ac <= EDCA_AC_VO; ac++)
    {
        acAdmitted = ( (pMac->lim.gAcAdmitMask[SIR_MAC_DIRECTION_UPLINK] & (1 << ac)) >> ac );
        limLog(pMac, LOG1, FL("For AC[%d]: acm=%d,  acAdmit=%d "), ac, plocalEdcaParams[ac].aci.acm, acAdmitted);
        if ( (plocalEdcaParams[ac].aci.acm == 1) && (acAdmitted == 0) )
        {
            limLog(pMac, LOG1, FL("We need to downgrade AC %d!! "), ac);
            newAc = EDCA_AC_BE;
            for (i=ac-1; i>0; i--)
            {
                if (plocalEdcaParams[i].aci.acm == 0)
                {
                    newAc = i;
                    break;
                }
            }
            limLog(pMac, LOGW, FL("Downgrading AC %d ---> AC %d "), ac, newAc);
            psessionEntry->gLimEdcaParamsActive[ac] = plocalEdcaParams[newAc];
        }
    }
//                        
#ifdef FEATURE_WLAN_DIAG_SUPPORT_LIM //                         
    WLAN_VOS_DIAG_LOG_ALLOC(log_ptr, vos_log_qos_edca_pkt_type, LOG_WLAN_QOS_EDCA_C);
    if(log_ptr)
    {
       log_ptr->aci_be = psessionEntry->gLimEdcaParamsActive[EDCA_AC_BE].aci.aci;
       log_ptr->cw_be  = psessionEntry->gLimEdcaParamsActive[EDCA_AC_BE].cw.max << 4 |
          psessionEntry->gLimEdcaParamsActive[EDCA_AC_BE].cw.min;
       log_ptr->txoplimit_be = psessionEntry->gLimEdcaParamsActive[EDCA_AC_BE].txoplimit;
       log_ptr->aci_bk = psessionEntry->gLimEdcaParamsActive[EDCA_AC_BK].aci.aci;
       log_ptr->cw_bk  = psessionEntry->gLimEdcaParamsActive[EDCA_AC_BK].cw.max << 4 |
          psessionEntry->gLimEdcaParamsActive[EDCA_AC_BK].cw.min;
       log_ptr->txoplimit_bk = psessionEntry->gLimEdcaParamsActive[EDCA_AC_BK].txoplimit;
       log_ptr->aci_vi = psessionEntry->gLimEdcaParamsActive[EDCA_AC_VI].aci.aci;
       log_ptr->cw_vi  = psessionEntry->gLimEdcaParamsActive[EDCA_AC_VI].cw.max << 4 |
          psessionEntry->gLimEdcaParamsActive[EDCA_AC_VI].cw.min;
       log_ptr->txoplimit_vi = psessionEntry->gLimEdcaParamsActive[EDCA_AC_VI].txoplimit;
       log_ptr->aci_vo = psessionEntry->gLimEdcaParamsActive[EDCA_AC_VO].aci.aci;
       log_ptr->cw_vo  = psessionEntry->gLimEdcaParamsActive[EDCA_AC_VO].cw.max << 4 |
          psessionEntry->gLimEdcaParamsActive[EDCA_AC_VO].cw.min;
       log_ptr->txoplimit_vo = psessionEntry->gLimEdcaParamsActive[EDCA_AC_VO].txoplimit;
    }
    WLAN_VOS_DIAG_LOG_REPORT(log_ptr);
#endif //                         
    
    return;
 }

/*                                                           
                        
                                                         
                             
                                 
             
                                                             */
 //                                   
#if 0
tSirRetStatus limSetLinkState(tpAniSirGlobal pMac, tSirLinkState state,tSirMacAddr bssId)
{
    tSirMsgQ msg;
    tSirRetStatus retCode;
    msg.type = WDA_SET_LINK_STATE;
    msg.bodyval = (tANI_U32) state;
    msg.bodyptr = NULL;
    MTRACE(macTraceMsgTx(pMac, 0, msg.type));
    retCode = wdaPostCtrlMsg(pMac, &msg);
    if (retCode != eSIR_SUCCESS)
        limLog(pMac, LOGP, FL("Posting link state %d failed, reason = %x "), retCode);
    return retCode;
}
#endif // 
tSirRetStatus limSetLinkState(tpAniSirGlobal pMac, tSirLinkState state,tSirMacAddr bssId, 
                              tSirMacAddr selfMacAddr, tpSetLinkStateCallback callback, 
                              void *callbackArg) 
{
    tSirMsgQ msgQ;
    tSirRetStatus retCode;
    tpLinkStateParams pLinkStateParams = NULL;
    //                 
    pLinkStateParams = vos_mem_malloc(sizeof(tLinkStateParams));
    if ( NULL == pLinkStateParams )
    {
        limLog( pMac, LOGP,
        FL( "Unable to allocate memory while sending Set Link State" ));
        retCode = eSIR_SME_RESOURCES_UNAVAILABLE;
        return retCode;
    }
    vos_mem_set((tANI_U8 *) pLinkStateParams, sizeof(tLinkStateParams), 0);
    pLinkStateParams->state        = state;
    pLinkStateParams->callback     = callback;
    pLinkStateParams->callbackArg  = callbackArg;
     
    /*                  */
    sirCopyMacAddr(pLinkStateParams->bssid,bssId);
    sirCopyMacAddr(pLinkStateParams->selfMacAddr, selfMacAddr);

    msgQ.type = WDA_SET_LINK_STATE;
    msgQ.reserved = 0;
    msgQ.bodyptr = pLinkStateParams;
    msgQ.bodyval = 0;
    
    MTRACE(macTraceMsgTx(pMac, NO_SESSION, msgQ.type));

    retCode = (tANI_U32)wdaPostCtrlMsg(pMac, &msgQ);
    if (retCode != eSIR_SUCCESS)
    {
        vos_mem_free(pLinkStateParams);
        limLog(pMac, LOGP, FL("Posting link state %d failed, reason = %x "), retCode);
    }
    return retCode;
}
#ifdef WLAN_FEATURE_VOWIFI_11R
extern tSirRetStatus limSetLinkStateFT(tpAniSirGlobal pMac, tSirLinkState 
state,tSirMacAddr bssId, tSirMacAddr selfMacAddr, int ft, tpPESession psessionEntry)
{
    tSirMsgQ msgQ;
    tSirRetStatus retCode;
    tpLinkStateParams pLinkStateParams = NULL;
    //                 
    pLinkStateParams = vos_mem_malloc(sizeof(tLinkStateParams));
    if ( NULL == pLinkStateParams )
    {
        limLog( pMac, LOGP,
        FL( "Unable to allocate memory while sending Set Link State" ));
        retCode = eSIR_SME_RESOURCES_UNAVAILABLE;
        return retCode;
    }
    vos_mem_set((tANI_U8 *) pLinkStateParams, sizeof(tLinkStateParams), 0);
    pLinkStateParams->state = state;
    /*                  */
    sirCopyMacAddr(pLinkStateParams->bssid,bssId);
    sirCopyMacAddr(pLinkStateParams->selfMacAddr, selfMacAddr);
    pLinkStateParams->ft = 1;
    pLinkStateParams->session = psessionEntry;

    msgQ.type = WDA_SET_LINK_STATE;
    msgQ.reserved = 0;
    msgQ.bodyptr = pLinkStateParams;
    msgQ.bodyval = 0;
    if(NULL == psessionEntry)
    {
        MTRACE(macTraceMsgTx(pMac, NO_SESSION, msgQ.type));
    }
    else
    {
        MTRACE(macTraceMsgTx(pMac, psessionEntry->peSessionId, msgQ.type));
    }

    retCode = (tANI_U32)wdaPostCtrlMsg(pMac, &msgQ);
    if (retCode != eSIR_SUCCESS)
    {
        vos_mem_free(pLinkStateParams);
        limLog(pMac, LOGP, FL("Posting link state %d failed, reason = %x "), retCode);
    }
    return retCode;
}
#endif

/*                                                           
                             
                                                         
                                 
                                            
             
                                                             */
tSirRetStatus limSendSetTxPowerReq(tpAniSirGlobal pMac,  tANI_U32 *pMsgBuf)
{
    tSirSetTxPowerReq   *txPowerReq;
    tSirRetStatus        retCode = eSIR_SUCCESS;
    tSirMsgQ             msgQ;
    tpPESession          psessionEntry;
    tANI_U8              sessionId = 0;

    if (NULL == pMsgBuf)
        return eSIR_FAILURE;

    txPowerReq = vos_mem_malloc(sizeof(tSirSetTxPowerReq));
    if ( NULL == txPowerReq )
    {
        return eSIR_FAILURE;
    }
    vos_mem_copy(txPowerReq, (tSirSetTxPowerReq *)pMsgBuf, sizeof(tSirSetTxPowerReq));

    /*                                             */
    psessionEntry = peFindSessionByBssid(pMac, txPowerReq->bssId, &sessionId);
    if (NULL == psessionEntry)
    {
        vos_mem_free(txPowerReq);
        limLog(pMac, LOGE, FL("Session does not exist for given BSSID"));
        return eSIR_FAILURE;
    }

    /*                         */
    txPowerReq->bssIdx = psessionEntry->bssIdx;

    msgQ.type = WDA_SET_TX_POWER_REQ;
    msgQ.reserved = 0;
    msgQ.bodyptr = txPowerReq;
    msgQ.bodyval = 0;
    PELOGW(limLog(pMac, LOGW, FL("Sending WDA_SET_TX_POWER_REQ to WDA"));)
    MTRACE(macTraceMsgTx(pMac, NO_SESSION, msgQ.type));
    retCode = wdaPostCtrlMsg(pMac, &msgQ);
    if (eSIR_SUCCESS != retCode)
    {
        limLog(pMac, LOGP, FL("Posting WDA_SET_TX_POWER_REQ to WDA failed, reason=%X"), retCode);
        vos_mem_free(txPowerReq);
        return retCode;
    }

    return retCode;
}
/*                                                           
                             
                                                        
                                 
                                            
             
                                                             */
tSirRetStatus limSendGetTxPowerReq(tpAniSirGlobal pMac,  tpSirGetTxPowerReq pTxPowerReq)
{
    tSirRetStatus  retCode = eSIR_SUCCESS;
    tSirMsgQ       msgQ;
    if (NULL == pTxPowerReq)
        return retCode;
    msgQ.type = WDA_GET_TX_POWER_REQ;
    msgQ.reserved = 0;
    msgQ.bodyptr = pTxPowerReq;
    msgQ.bodyval = 0;
    PELOGW(limLog(pMac, LOGW, FL( "Sending WDA_GET_TX_POWER_REQ to WDA"));)
    MTRACE(macTraceMsgTx(pMac, NO_SESSION, msgQ.type));
    if( eSIR_SUCCESS != (retCode = wdaPostCtrlMsg( pMac, &msgQ )))
    {
        limLog( pMac, LOGP, FL("Posting WDA_GET_TX_POWER_REQ to WDA failed, reason=%X"), retCode );
        if (NULL != pTxPowerReq)
        {
            vos_mem_free(pTxPowerReq);
        }
        return retCode;
    }
    return retCode;
}
/*                                                           
                                
                                               
                             
             
                                                             */
tSirRetStatus limSendBeaconFilterInfo(tpAniSirGlobal pMac,tpPESession psessionEntry)
{
    tpBeaconFilterMsg  pBeaconFilterMsg = NULL;
    tSirRetStatus      retCode = eSIR_SUCCESS;
    tSirMsgQ           msgQ;
    tANI_U8            *ptr;
    tANI_U32           i;
    tANI_U32           msgSize;
    tpBeaconFilterIe   pIe;

    if( psessionEntry == NULL )
    {
        limLog( pMac, LOGE, FL("Fail to find the right session "));
        retCode = eSIR_FAILURE;
        return retCode;
    }
    msgSize = sizeof(tBeaconFilterMsg) + sizeof(beaconFilterTable);
    pBeaconFilterMsg = vos_mem_malloc(msgSize);
    if ( NULL == pBeaconFilterMsg )
    {
        limLog( pMac, LOGP, FL("Fail to allocate memory for beaconFiilterMsg "));
        retCode = eSIR_MEM_ALLOC_FAILED;
        return retCode;
    }
    vos_mem_set((tANI_U8 *) pBeaconFilterMsg, msgSize, 0);
    //                                 
    //                                               
    //                                                            
    pBeaconFilterMsg->capabilityInfo = psessionEntry->limCurrentBssCaps;
    pBeaconFilterMsg->capabilityMask = CAPABILITY_FILTER_MASK;
    pBeaconFilterMsg->beaconInterval = (tANI_U16) psessionEntry->beaconParams.beaconInterval;
    //                                           
    pBeaconFilterMsg->ieNum = (tANI_U16) (sizeof(beaconFilterTable) / sizeof(tBeaconFilterIe));
    //               
    pBeaconFilterMsg->bssIdx = psessionEntry->bssIdx;

    //                                                         
    ptr = (tANI_U8 *)pBeaconFilterMsg + sizeof(tBeaconFilterMsg);
    for(i=0; i < (pBeaconFilterMsg->ieNum); i++)
    {
        pIe = (tpBeaconFilterIe) ptr;
        pIe->elementId = beaconFilterTable[i].elementId;
        pIe->checkIePresence = beaconFilterTable[i].checkIePresence;
        pIe->byte.offset = beaconFilterTable[i].byte.offset;
        pIe->byte.value =  beaconFilterTable[i].byte.value;
        pIe->byte.bitMask =  beaconFilterTable[i].byte.bitMask;
        pIe->byte.ref =  beaconFilterTable[i].byte.ref; 
        ptr += sizeof(tBeaconFilterIe);
    }
    msgQ.type = WDA_BEACON_FILTER_IND;
    msgQ.reserved = 0;
    msgQ.bodyptr = pBeaconFilterMsg;
    msgQ.bodyval = 0;
    limLog( pMac, LOG3, FL( "Sending WDA_BEACON_FILTER_IND..." ));
    MTRACE(macTraceMsgTx(pMac, psessionEntry->peSessionId, msgQ.type));
    if( eSIR_SUCCESS != (retCode = wdaPostCtrlMsg( pMac, &msgQ )))
    {
        vos_mem_free(pBeaconFilterMsg);
        limLog( pMac, LOGP,
            FL("Posting  WDA_BEACON_FILTER_IND to WDA failed, reason=%X"),
            retCode );
        return retCode;
    }
    return retCode;
}

#ifdef WLAN_FEATURE_11AC
tSirRetStatus limSendModeUpdate(tpAniSirGlobal pMac, 
                                tUpdateVHTOpMode *pTempParam,
                                tpPESession  psessionEntry )
{
    tUpdateVHTOpMode *pVhtOpMode = NULL;
    tSirRetStatus   retCode = eSIR_SUCCESS;
    tSirMsgQ msgQ;

    pVhtOpMode = vos_mem_malloc(sizeof(tUpdateVHTOpMode));
    if ( NULL == pVhtOpMode )
    {
        limLog( pMac, LOGP,
            FL( "Unable to allocate memory during Update Op Mode" ));
        return eSIR_MEM_ALLOC_FAILED;
    }
    vos_mem_copy((tANI_U8 *)pVhtOpMode, pTempParam, sizeof(tUpdateVHTOpMode));
    msgQ.type =  WDA_UPDATE_OP_MODE;
    msgQ.reserved = 0;
    msgQ.bodyptr = pVhtOpMode;
    msgQ.bodyval = 0;
    PELOG3(limLog( pMac, LOG3,
                FL( "Sending WDA_UPDATE_OP_MODE" ));)
    if(NULL == psessionEntry)
    {
        MTRACE(macTraceMsgTx(pMac, NO_SESSION, msgQ.type));
    }
    else
    {
        MTRACE(macTraceMsgTx(pMac, psessionEntry->peSessionId, msgQ.type));
    }
    if( eSIR_SUCCESS != (retCode = wdaPostCtrlMsg( pMac, &msgQ )))
    {
        vos_mem_free(pVhtOpMode);
        limLog( pMac, LOGP,
                    FL("Posting  WDA_UPDATE_OP_MODE to WDA failed, reason=%X"),
                    retCode );
    }

    return retCode;
}
#endif 

#ifdef FEATURE_WLAN_TDLS_INTERNAL
/*                                                           
                                 
                                                           
                             
         
             
                                                             */
tSirRetStatus limSendTdlsLinkEstablish(tpAniSirGlobal pMac, tANI_U8 bIsPeerResponder, tANI_U8 linkIdenOffset, 
                tANI_U8 ptiBufStatusOffset, tANI_U8 ptiFrameLen, tANI_U8 *ptiFrame, tANI_U8 *extCapability)
{
    tSirMsgQ msgQ;
    tSirRetStatus retCode;
    tpSirTdlsLinkEstablishInd pTdlsLinkEstablish = NULL;

    //                 
    pTdlsLinkEstablish = vos_mem_malloc(sizeof(tSirTdlsLinkEstablishInd));
    if ( NULL == pTdlsLinkEstablish )
    {
        limLog( pMac, LOGP,
        FL( "Unable to allocate memory while sending Tdls Link Establish " ));

        retCode = eSIR_SME_RESOURCES_UNAVAILABLE;
        return retCode;
    }

    vos_mem_set((tANI_U8 *) pTdlsLinkEstablish, sizeof(tSirTdlsLinkEstablishInd), 0);

    pTdlsLinkEstablish->bIsResponder = !!bIsPeerResponder; 
    pTdlsLinkEstablish->linkIdenOffset = linkIdenOffset;
    pTdlsLinkEstablish->ptiBufStatusOffset = ptiBufStatusOffset;
    pTdlsLinkEstablish->ptiTemplateLen = ptiFrameLen;
    /*                        */
    vos_mem_copy(pTdlsLinkEstablish->ptiTemplateBuf, ptiFrame, ptiFrameLen);
    /*                            */
    vos_mem_copy((tANI_U8 *) pTdlsLinkEstablish->extCapability,  extCapability, sizeof(pTdlsLinkEstablish->extCapability));

    msgQ.type = SIR_HAL_TDLS_LINK_ESTABLISH;
    msgQ.reserved = 0;
    msgQ.bodyptr = pTdlsLinkEstablish;
    msgQ.bodyval = 0;
    
    MTRACE(macTraceMsgTx(pMac, 0, msgQ.type));

    retCode = (tANI_U32)wdaPostCtrlMsg(pMac, &msgQ);
    if (retCode != eSIR_SUCCESS)
    {
        vos_mem_free(pTdlsLinkEstablish);
        limLog(pMac, LOGP, FL("Posting tdls link establish %d failed, reason = %x "), retCode);
    }

    return retCode;
}

/*                                                           
                                
                                                                              
                             
         
             
                                                             */
tSirRetStatus limSendTdlsLinkTeardown(tpAniSirGlobal pMac, tANI_U16 staId)
{
    tSirMsgQ msgQ;
    tSirRetStatus retCode;
    tpSirTdlsLinkTeardownInd pTdlsLinkTeardown = NULL;

    //                 
    pTdlsLinkTeardown = vos_mem_malloc(sizeof(tSirTdlsLinkTeardownInd));
    if ( NULL == pTdlsLinkTeardown )
    {
        limLog( pMac, LOGP,
        FL( "Unable to allocate memory while sending Tdls Link Teardown " ));

        retCode = eSIR_SME_RESOURCES_UNAVAILABLE;
        return retCode;
    }

    vos_mem_set((tANI_U8 *) pTdlsLinkTeardown, sizeof(tSirTdlsLinkTeardownInd), 0);

    pTdlsLinkTeardown->staId = staId;

    msgQ.type = SIR_HAL_TDLS_LINK_TEARDOWN;
    msgQ.reserved = 0;
    msgQ.bodyptr = pTdlsLinkTeardown;
    msgQ.bodyval = 0;
    
    MTRACE(macTraceMsgTx(pMac, 0, msgQ.type));

    retCode = (tANI_U32)wdaPostCtrlMsg(pMac, &msgQ);
    if (retCode != eSIR_SUCCESS)
    {
        vos_mem_free(pTdlsLinkTeardown);
        limLog(pMac, LOGP, FL("Posting tdls link teardown %d failed, reason = %x "), retCode);
    }

    return retCode;
}

#endif

#ifdef WLAN_FEATURE_11W
/*                                                           
                                   
                                                          
                                                        
                             
                                                         
                 
                                                     
               
                                                             */
tSirRetStatus limSendExcludeUnencryptInd(tpAniSirGlobal pMac,
                                         tANI_BOOLEAN excludeUnenc,
                                         tpPESession  psessionEntry)
{
    tSirRetStatus   retCode = eSIR_SUCCESS;
    tSirMsgQ msgQ;
    tSirWlanExcludeUnencryptParam * pExcludeUnencryptParam;

    pExcludeUnencryptParam = vos_mem_malloc(sizeof(tSirWlanExcludeUnencryptParam));
    if ( NULL == pExcludeUnencryptParam )
    {
        limLog(pMac, LOGP,
            FL( "Unable to allocate memory during limSendExcludeUnencryptInd"));
        return eSIR_MEM_ALLOC_FAILED;
    }

    pExcludeUnencryptParam->excludeUnencrypt = excludeUnenc;
    sirCopyMacAddr(pExcludeUnencryptParam->bssId, psessionEntry->bssId);

    msgQ.type =  WDA_EXCLUDE_UNENCRYPTED_IND;
    msgQ.reserved = 0;
    msgQ.bodyptr = pExcludeUnencryptParam;
    msgQ.bodyval = 0;
    PELOG3(limLog(pMac, LOG3,
                FL("Sending WDA_EXCLUDE_UNENCRYPTED_IND"));)
    MTRACE(macTraceMsgTx(pMac, psessionEntry->peSessionId, msgQ.type));
    retCode = wdaPostCtrlMsg(pMac, &msgQ);
    if (eSIR_SUCCESS != retCode)
    {
        vos_mem_free(pExcludeUnencryptParam);
        limLog(pMac, LOGP,
               FL("Posting  WDA_EXCLUDE_UNENCRYPTED_IND to WDA failed, reason=%X"),
               retCode);
    }

    return retCode;
}
#endif


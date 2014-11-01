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

#include "vos_types.h"
#include "wniApi.h"
#include "sirCommon.h"
#include "aniGlobal.h"

#include "wniCfgSta.h"
#include "sysDef.h"
#include "cfgApi.h"


#include "schApi.h"
#include "utilsApi.h"
#include "limUtils.h"
#include "limSecurityUtils.h"
#include "limSerDesUtils.h"
#include "limSendSmeRspMessages.h"
#include "limIbssPeerMgmt.h"
#include "limSessionUtils.h"


/* 
                  
  
           
                                                                
                                                                      
                                                                
            
  
         
  
        
  
              
     
  
       
     
  
                                                      
                                             
                                                                
                                                   
  
               
 */

void
limSendSmeRsp(tpAniSirGlobal pMac, tANI_U16 msgType,
              tSirResultCodes resultCode,tANI_U8 smesessionId, tANI_U16 smetransactionId)
{
    tSirMsgQ    mmhMsg;
    tSirSmeRsp  *pSirSmeRsp;

    PELOG1(limLog(pMac, LOG1,
           FL("Sending message %s with reasonCode %s"),
           limMsgStr(msgType), limResultCodeStr(resultCode));)

    pSirSmeRsp = vos_mem_malloc(sizeof(tSirSmeRsp));
    if ( NULL == pSirSmeRsp )
    {
        //                                 
        limLog(pMac, LOGP,
               FL("call to AllocateMemory failed for eWNI_SME_*_RSP"));

        return;
    }
  
    pSirSmeRsp->messageType = msgType;
    pSirSmeRsp->length      = sizeof(tSirSmeRsp);
    pSirSmeRsp->statusCode  = resultCode;

    /*                                          */
    pSirSmeRsp->sessionId = smesessionId;
    pSirSmeRsp->transactionId = smetransactionId;


    mmhMsg.type = msgType;
    mmhMsg.bodyptr = pSirSmeRsp;
    mmhMsg.bodyval = 0;
    MTRACE(macTraceMsgTx(pMac, smesessionId, mmhMsg.type));

#ifdef FEATURE_WLAN_DIAG_SUPPORT_LIM //                          
   {
    tpPESession psessionEntry = peGetValidPowerSaveSession(pMac);
    switch(msgType)
    {
        case eWNI_PMC_ENTER_BMPS_RSP:
            limDiagEventReport(pMac, WLAN_PE_DIAG_ENTER_BMPS_RSP_EVENT, psessionEntry, (tANI_U16)resultCode, 0);
            break;
        case eWNI_PMC_EXIT_BMPS_RSP:
            limDiagEventReport(pMac, WLAN_PE_DIAG_EXIT_BMPS_RSP_EVENT, psessionEntry, (tANI_U16)resultCode, 0);
            break;
        case eWNI_PMC_ENTER_IMPS_RSP:
            limDiagEventReport(pMac, WLAN_PE_DIAG_ENTER_IMPS_RSP_EVENT, psessionEntry, (tANI_U16)resultCode, 0);
            break;        
        case eWNI_PMC_EXIT_IMPS_RSP:
            limDiagEventReport(pMac, WLAN_PE_DIAG_EXIT_IMPS_RSP_EVENT, psessionEntry, (tANI_U16)resultCode, 0);
            break;
        case eWNI_PMC_ENTER_UAPSD_RSP:
            limDiagEventReport(pMac, WLAN_PE_DIAG_ENTER_UAPSD_RSP_EVENT, psessionEntry, (tANI_U16)resultCode, 0);
            break;
        case eWNI_PMC_EXIT_UAPSD_RSP:
            limDiagEventReport(pMac, WLAN_PE_DIAG_EXIT_UAPSD_RSP_EVENT, psessionEntry, (tANI_U16)resultCode, 0);
            break;
        case eWNI_SME_SWITCH_CHL_RSP:
            limDiagEventReport(pMac, WLAN_PE_DIAG_SWITCH_CHL_RSP_EVENT, NULL, (tANI_U16)resultCode, 0);
            break;
        case eWNI_SME_STOP_BSS_RSP:
            limDiagEventReport(pMac, WLAN_PE_DIAG_STOP_BSS_RSP_EVENT, NULL, (tANI_U16)resultCode, 0);
            break;      
        case eWNI_PMC_ENTER_WOWL_RSP:
            limDiagEventReport(pMac, WLAN_PE_DIAG_ENTER_WOWL_RSP_EVENT, psessionEntry, (tANI_U16)resultCode, 0);
            break;
        case eWNI_PMC_EXIT_WOWL_RSP:
            limDiagEventReport(pMac, WLAN_PE_DIAG_EXIT_WOWL_RSP_EVENT, psessionEntry, (tANI_U16)resultCode, 0);
            break;          
    } 
   }  
#endif //                         
    
    limSysProcessMmhMsgApi(pMac, &mmhMsg,  ePROT);
} /*                         */


/* 
                                        
  
           
                                                   
                                        
  
         
  
        
  
              
     
  
       
     
  
                                                      
                                          
                                                                 
                                                 
  
               
 */
static void limSendSmeJoinReassocRspAfterResume( tpAniSirGlobal pMac, 
                                       eHalStatus status, tANI_U32 *ctx)
{
    tSirMsgQ         mmhMsg;
    tpSirSmeJoinRsp  pSirSmeJoinRsp = (tpSirSmeJoinRsp) ctx;

    mmhMsg.type = pSirSmeJoinRsp->messageType;
    mmhMsg.bodyptr = pSirSmeJoinRsp;
    mmhMsg.bodyval = 0;
    MTRACE(macTraceMsgTx(pMac, NO_SESSION, mmhMsg.type));
    limSysProcessMmhMsgApi(pMac, &mmhMsg,  ePROT);
}


/* 
                             
  
           
                                                                
                                                                     
                      
  
         
  
        
  
              
     
  
       
     
  
                                                      
                                             
                                                                
                                                   
  
               
 */

void
limSendSmeJoinReassocRsp(tpAniSirGlobal pMac, tANI_U16 msgType,
                         tSirResultCodes resultCode, tANI_U16 protStatusCode,
                         tpPESession psessionEntry,tANI_U8 smesessionId,tANI_U16 smetransactionId)
{
    tpSirSmeJoinRsp  pSirSmeJoinRsp;
    tANI_U32 rspLen;
    tpDphHashNode pStaDs    = NULL;

#ifdef FEATURE_WLAN_DIAG_SUPPORT_LIM //                          
    if (msgType == eWNI_SME_REASSOC_RSP)
        limDiagEventReport(pMac, WLAN_PE_DIAG_REASSOC_RSP_EVENT, psessionEntry, (tANI_U16)resultCode, 0);
    else
        limDiagEventReport(pMac, WLAN_PE_DIAG_JOIN_RSP_EVENT, psessionEntry, (tANI_U16)resultCode, 0);
#endif //                         

    PELOG1(limLog(pMac, LOG1,
           FL("Sending message %s with reasonCode %s"),
           limMsgStr(msgType), limResultCodeStr(resultCode));)

    if(psessionEntry == NULL)
    {

        rspLen = sizeof(tSirSmeJoinRsp);   

        pSirSmeJoinRsp = vos_mem_malloc(rspLen);
        if ( NULL == pSirSmeJoinRsp )
        {
            //                                 
            limLog(pMac, LOGP, FL("call to AllocateMemory failed for JOIN/REASSOC_RSP"));
            return;
        }
         
        vos_mem_set((tANI_U8*)pSirSmeJoinRsp, rspLen, 0);
         
         
        pSirSmeJoinRsp->beaconLength = 0;
        pSirSmeJoinRsp->assocReqLength = 0;
        pSirSmeJoinRsp->assocRspLength = 0;
    }

    else
    {
        rspLen = psessionEntry->assocReqLen + psessionEntry->assocRspLen + 
            psessionEntry->bcnLen + 
#ifdef WLAN_FEATURE_VOWIFI_11R
            psessionEntry->RICDataLen +
#endif
#ifdef FEATURE_WLAN_CCX            
            psessionEntry->tspecLen + 
#endif            
            sizeof(tSirSmeJoinRsp) - sizeof(tANI_U8) ;

        pSirSmeJoinRsp = vos_mem_malloc(rspLen);
        if ( NULL == pSirSmeJoinRsp )
        {
            //                                 
            limLog(pMac, LOGP,
               FL("call to AllocateMemory failed for JOIN/REASSOC_RSP"));

            return;
        }

        vos_mem_set((tANI_U8*)pSirSmeJoinRsp, rspLen, 0);

        if (resultCode == eSIR_SME_SUCCESS)
        {
            pStaDs = dphGetHashEntry(pMac, DPH_STA_HASH_INDEX_PEER, &psessionEntry->dph.dphHashTable);
            if (pStaDs == NULL)
            {
                PELOGE(limLog(pMac, LOGE, FL("could not Get Self Entry for the station"));)
            }
            else
            {
                //                     
                pSirSmeJoinRsp->staId = pStaDs->staIndex;
                pSirSmeJoinRsp->ucastSig   = pStaDs->ucUcastSig;
                pSirSmeJoinRsp->bcastSig   = pStaDs->ucBcastSig;
            }
        }

        pSirSmeJoinRsp->beaconLength = 0;
        pSirSmeJoinRsp->assocReqLength = 0;
        pSirSmeJoinRsp->assocRspLength = 0;
#ifdef WLAN_FEATURE_VOWIFI_11R
        pSirSmeJoinRsp->parsedRicRspLen = 0;
#endif
#ifdef FEATURE_WLAN_CCX            
        pSirSmeJoinRsp->tspecIeLen = 0;
#endif
        
        if(resultCode == eSIR_SME_SUCCESS)
        {

            if(psessionEntry->beacon != NULL)
            {
                pSirSmeJoinRsp->beaconLength = psessionEntry->bcnLen;
                vos_mem_copy( pSirSmeJoinRsp->frames, psessionEntry->beacon,
                              pSirSmeJoinRsp->beaconLength);
                vos_mem_free( psessionEntry->beacon);
                psessionEntry->beacon = NULL;
#ifdef WLAN_FEATURE_VOWIFI_11R_DEBUG
                PELOG1(limLog(pMac, LOG1, FL("Beacon=%d"), psessionEntry->bcnLen);)
#endif
            }
        
            if(psessionEntry->assocReq != NULL)
            {
                pSirSmeJoinRsp->assocReqLength = psessionEntry->assocReqLen;
                vos_mem_copy( pSirSmeJoinRsp->frames + psessionEntry->bcnLen,
                              psessionEntry->assocReq, pSirSmeJoinRsp->assocReqLength);
                vos_mem_free( psessionEntry->assocReq);
                psessionEntry->assocReq = NULL;
#ifdef WLAN_FEATURE_VOWIFI_11R_DEBUG
                PELOG1(limLog(pMac, LOG1, FL("AssocReq=%d"), psessionEntry->assocReqLen);)
#endif
            }
            if(psessionEntry->assocRsp != NULL)
            {
                pSirSmeJoinRsp->assocRspLength = psessionEntry->assocRspLen;
                vos_mem_copy( pSirSmeJoinRsp->frames + psessionEntry->bcnLen +
                                               psessionEntry->assocReqLen,
                              psessionEntry->assocRsp,
                              pSirSmeJoinRsp->assocRspLength);
                vos_mem_free( psessionEntry->assocRsp);
                psessionEntry->assocRsp = NULL;
            }           
#ifdef WLAN_FEATURE_VOWIFI_11R
            if(psessionEntry->ricData != NULL)
            {
                pSirSmeJoinRsp->parsedRicRspLen = psessionEntry->RICDataLen;
                vos_mem_copy( pSirSmeJoinRsp->frames + psessionEntry->bcnLen +
                                 psessionEntry->assocReqLen + psessionEntry->assocRspLen,
                              psessionEntry->ricData, pSirSmeJoinRsp->parsedRicRspLen);
                vos_mem_free(psessionEntry->ricData);
                psessionEntry->ricData = NULL;
                PELOG1(limLog(pMac, LOG1, FL("RicLength=%d"), pSirSmeJoinRsp->parsedRicRspLen);)
            }
#endif
#ifdef FEATURE_WLAN_CCX            
            if(psessionEntry->tspecIes != NULL)
            {
                pSirSmeJoinRsp->tspecIeLen = psessionEntry->tspecLen;
                vos_mem_copy( pSirSmeJoinRsp->frames + psessionEntry->bcnLen +
                                psessionEntry->assocReqLen + psessionEntry->assocRspLen +
                                  psessionEntry->RICDataLen,
                              psessionEntry->tspecIes, pSirSmeJoinRsp->tspecIeLen);
                vos_mem_free(psessionEntry->tspecIes);
                psessionEntry->tspecIes = NULL;
                PELOG1(limLog(pMac, LOG1, FL("CCX-TspecLen=%d"), psessionEntry->tspecLen);)
            }
#endif            
            pSirSmeJoinRsp->aid = psessionEntry->limAID;
#ifdef WLAN_FEATURE_VOWIFI_11R_DEBUG
            PELOG1(limLog(pMac, LOG1, FL("AssocRsp=%d"), psessionEntry->assocRspLen);)
#endif
        }
        else
        {

            if(psessionEntry->beacon != NULL)
            {
                vos_mem_free(psessionEntry->beacon);
                psessionEntry->beacon = NULL;
            }

            if(psessionEntry->assocReq != NULL)
            {
                vos_mem_free( psessionEntry->assocReq);
                psessionEntry->assocReq = NULL;
            }

            if(psessionEntry->assocRsp != NULL)
            {
                vos_mem_free( psessionEntry->assocRsp);
                psessionEntry->assocRsp = NULL;
            }

        }
    }


    pSirSmeJoinRsp->messageType = msgType;
    pSirSmeJoinRsp->length = (tANI_U16) rspLen;
    pSirSmeJoinRsp->statusCode = resultCode;
    pSirSmeJoinRsp->protStatusCode = protStatusCode;
    
    /*                                          */
    pSirSmeJoinRsp->sessionId = smesessionId;
    pSirSmeJoinRsp->transactionId = smetransactionId;
    
    if(IS_MCC_SUPPORTED && limIsLinkSuspended( pMac ) )
    {
        if( psessionEntry && psessionEntry->limSmeState == eLIM_SME_LINK_EST_STATE )
        {
            
#ifdef WLAN_FEATURE_11AC
            if (psessionEntry->vhtCapability)
            {
                ePhyChanBondState htSecondaryChannelOffset;
               /*                                 */
                 htSecondaryChannelOffset = limGet11ACPhyCBState(pMac, 
                                    psessionEntry->currentOperChannel,
                                    psessionEntry->htSecondaryChannelOffset,
                                    psessionEntry->apCenterChan,
                                    psessionEntry);
                peSetResumeChannel( pMac, psessionEntry->currentOperChannel, htSecondaryChannelOffset);
            }
            else 
#endif
               peSetResumeChannel( pMac, psessionEntry->currentOperChannel, psessionEntry->htSecondaryChannelOffset);
        }
        else
        {
            peSetResumeChannel( pMac, 0, 0);
        }
        limResumeLink( pMac, limSendSmeJoinReassocRspAfterResume, 
                                              (tANI_U32*) pSirSmeJoinRsp );
    }
    else
    {
        limSendSmeJoinReassocRspAfterResume( pMac, eHAL_STATUS_SUCCESS,
                                              (tANI_U32*) pSirSmeJoinRsp );
    }
} /*                                    */



/* 
                          
  
           
                                                         
                                              
  
         
  
        
  
              
     
  
       
     
  
                                                      
                                             
                                                                
                                                   
  
               
 */

void
limSendSmeStartBssRsp(tpAniSirGlobal pMac,
                      tANI_U16 msgType, tSirResultCodes resultCode,tpPESession psessionEntry,
                      tANI_U8 smesessionId,tANI_U16 smetransactionId)
{


    tANI_U16            size = 0;
    tSirMsgQ            mmhMsg;
    tSirSmeStartBssRsp  *pSirSmeRsp;
    tANI_U16            ieLen;
    tANI_U16            ieOffset, curLen;

    PELOG1(limLog(pMac, LOG1, FL("Sending message %s with reasonCode %s"),
           limMsgStr(msgType), limResultCodeStr(resultCode));)

    size = sizeof(tSirSmeStartBssRsp);

    if(psessionEntry == NULL)
    {
         pSirSmeRsp = vos_mem_malloc(size);
         if ( NULL == pSirSmeRsp )
         {
            //                                 
            limLog(pMac, LOGP,FL("call to AllocateMemory failed for eWNI_SME_START_BSS_RSP"));
            return;
         }
         vos_mem_set((tANI_U8*)pSirSmeRsp, size, 0);
                      
    }
    else
    {
        //                                                                  
        ieOffset = sizeof(tAniBeaconStruct) + SIR_MAC_B_PR_SSID_OFFSET;
        ieLen = pMac->sch.schObject.gSchBeaconOffsetBegin + pMac->sch.schObject.gSchBeaconOffsetEnd - ieOffset;
        //                                     
        size += ieLen;

        pSirSmeRsp = vos_mem_malloc(size);
        if ( NULL == pSirSmeRsp )
        {
            //                                 
            limLog(pMac, LOGP,
            FL("call to AllocateMemory failed for eWNI_SME_START_BSS_RSP"));

            return;
        }
        vos_mem_set((tANI_U8*)pSirSmeRsp, size, 0);
        size = sizeof(tSirSmeStartBssRsp);
        if (resultCode == eSIR_SME_SUCCESS)
        {

                sirCopyMacAddr(pSirSmeRsp->bssDescription.bssId, psessionEntry->bssId);
        
                /*                                   */
                pSirSmeRsp->bssDescription.beaconInterval = (tANI_U16) psessionEntry->beaconParams.beaconInterval;
                pSirSmeRsp->bssType         = psessionEntry->bssType;

                if (cfgGetCapabilityInfo( pMac, &pSirSmeRsp->bssDescription.capabilityInfo,psessionEntry)
                != eSIR_SUCCESS)
                limLog(pMac, LOGP, FL("could not retrieve Capabilities value"));

                limGetPhyMode(pMac, (tANI_U32 *)&pSirSmeRsp->bssDescription.nwType, psessionEntry);

#if 0
            if (wlan_cfgGetInt(pMac, WNI_CFG_CURRENT_CHANNEL, &len) != eSIR_SUCCESS)
                limLog(pMac, LOGP, FL("could not retrieve CURRENT_CHANNEL from CFG"));
           
#endif//                   
            
                pSirSmeRsp->bssDescription.channelId = psessionEntry->currentOperChannel;

                pSirSmeRsp->bssDescription.aniIndicator = 1;

                curLen = pMac->sch.schObject.gSchBeaconOffsetBegin - ieOffset;
                vos_mem_copy( (tANI_U8 *) &pSirSmeRsp->bssDescription.ieFields,
                           pMac->sch.schObject.gSchBeaconFrameBegin + ieOffset,
                          (tANI_U32)curLen);

                vos_mem_copy( ((tANI_U8 *) &pSirSmeRsp->bssDescription.ieFields) + curLen,
                           pMac->sch.schObject.gSchBeaconFrameEnd,
                          (tANI_U32)pMac->sch.schObject.gSchBeaconOffsetEnd);


                //                                                                           
                pSirSmeRsp->bssDescription.length = sizeof(tSirBssDescription) -
                                                sizeof(tANI_U16) - sizeof(tANI_U32) +
                                                ieLen;
                //                                                                                
                size += ieLen - sizeof(tANI_U32);
        }

            

        
    }

    pSirSmeRsp->messageType     = msgType;
    pSirSmeRsp->length          = size;

    /*                                          */
    pSirSmeRsp->sessionId       = smesessionId;
    pSirSmeRsp->transactionId   = smetransactionId;
    pSirSmeRsp->statusCode      = resultCode;
    if(psessionEntry != NULL )
    pSirSmeRsp->staId           = psessionEntry->staId; //                                             
      

    mmhMsg.type = msgType;
    mmhMsg.bodyptr = pSirSmeRsp;
    mmhMsg.bodyval = 0;
    if(psessionEntry == NULL)
    {
        MTRACE(macTraceMsgTx(pMac, NO_SESSION, mmhMsg.type));
    }
    else
    {
        MTRACE(macTraceMsgTx(pMac, psessionEntry->peSessionId, mmhMsg.type));
    }
#ifdef FEATURE_WLAN_DIAG_SUPPORT_LIM //                          
    limDiagEventReport(pMac, WLAN_PE_DIAG_START_BSS_RSP_EVENT, psessionEntry, (tANI_U16)resultCode, 0);
#endif //                         

    limSysProcessMmhMsgApi(pMac, &mmhMsg,  ePROT);
} /*                                 */





#define LIM_MAX_NUM_OF_SCAN_RESULTS_REPORTED  20
#define LIM_SIZE_OF_EACH_BSS  400 //                         


/* 
                      
  
           
                                                                
                                                      
            
  
         
  
        
  
              
     
  
       
     
  
                                                      
                                                  
                                                                
                                                
  
               
 */

void
limSendSmeScanRsp(tpAniSirGlobal pMac, tANI_U16 length,
                  tSirResultCodes resultCode,tANI_U8  smesessionId,tANI_U16 smetranscationId)
{
    tSirMsgQ              mmhMsg;
    tpSirSmeScanRsp       pSirSmeScanRsp=NULL;
    tLimScanResultNode    *ptemp = NULL;
    tANI_U16              msgLen, allocLength, curMsgLen = 0;
    tANI_U16              i, bssCount;
    tANI_U8               *pbBuf;
    tSirBssDescription    *pDesc;

    PELOG1(limLog(pMac, LOG1,
       FL("Sending message SME_SCAN_RSP with length=%d reasonCode %s"),
       length, limResultCodeStr(resultCode));)

    if (resultCode != eSIR_SME_SUCCESS)
    {
        limPostSmeScanRspMessage(pMac, length, resultCode,smesessionId,smetranscationId);
        return;
    }

    mmhMsg.type = eWNI_SME_SCAN_RSP;
    i = 0;
    bssCount = 0;
    msgLen = 0;
    allocLength = LIM_MAX_NUM_OF_SCAN_RESULTS_REPORTED * LIM_SIZE_OF_EACH_BSS;
    pSirSmeScanRsp = vos_mem_malloc(allocLength);
    if ( NULL == pSirSmeScanRsp )
    {
        //          
        limLog(pMac, LOGP,
                   FL("call to AllocateMemory failed for eWNI_SME_SCAN_RSP"));

        return;
    }
    for (i = 0; i < LIM_MAX_NUM_OF_SCAN_RESULTS; i++)
    {
        //                                        
        ptemp = pMac->lim.gLimCachedScanHashTable[i];
        while(ptemp)
        {
            pbBuf = ((tANI_U8 *)pSirSmeScanRsp) + msgLen;
            if(0 == bssCount)
            {
                msgLen = sizeof(tSirSmeScanRsp) -
                           sizeof(tSirBssDescription) +
                           ptemp->bssDescription.length +
                           sizeof(ptemp->bssDescription.length);
                pDesc = pSirSmeScanRsp->bssDescription;
            }
            else
            {
                msgLen += ptemp->bssDescription.length +
                          sizeof(ptemp->bssDescription.length);
                pDesc = (tSirBssDescription *)pbBuf;
            }
            if( (allocLength < msgLen) ||
                 (LIM_MAX_NUM_OF_SCAN_RESULTS_REPORTED <= bssCount++) )
            {
                pSirSmeScanRsp->statusCode  =
                    eSIR_SME_MORE_SCAN_RESULTS_FOLLOW;
                pSirSmeScanRsp->messageType = eWNI_SME_SCAN_RSP;
                pSirSmeScanRsp->length      = curMsgLen;
                mmhMsg.bodyptr = pSirSmeScanRsp;
                mmhMsg.bodyval = 0;
                MTRACE(macTraceMsgTx(pMac, NO_SESSION, mmhMsg.type));
                limSysProcessMmhMsgApi(pMac, &mmhMsg, ePROT);
                pSirSmeScanRsp = vos_mem_malloc(allocLength);
                if ( NULL == pSirSmeScanRsp )
                {
                    //          
                    limLog(pMac, LOGP,
                                 FL("call to AllocateMemory failed for eWNI_SME_SCAN_RSP"));
                    return;
                }
                msgLen = sizeof(tSirSmeScanRsp) -
                           sizeof(tSirBssDescription) +
                           ptemp->bssDescription.length +
                           sizeof(ptemp->bssDescription.length);
                pDesc = pSirSmeScanRsp->bssDescription;
                bssCount = 1;
            }
            curMsgLen = msgLen;

            PELOG2(limLog(pMac, LOG2, FL("ScanRsp : msgLen %d, bssDescr Len=%d"),
                          msgLen, ptemp->bssDescription.length);)
            pDesc->length
                    = ptemp->bssDescription.length;
            vos_mem_copy( (tANI_U8 *) &pDesc->bssId,
                          (tANI_U8 *) &ptemp->bssDescription.bssId,
                           ptemp->bssDescription.length);

            PELOG2(limLog(pMac, LOG2, FL("BssId "));
            limPrintMacAddr(pMac, ptemp->bssDescription.bssId, LOG2);)

            pSirSmeScanRsp->sessionId   = smesessionId;
            pSirSmeScanRsp->transcationId = smetranscationId;

            ptemp = ptemp->next;
        } //            
    } //                                                 

    if(0 == bssCount)
    {
       limPostSmeScanRspMessage(pMac, length, resultCode, smesessionId, smetranscationId);
       if (NULL != pSirSmeScanRsp)
       {
           vos_mem_free( pSirSmeScanRsp);
           pSirSmeScanRsp = NULL;
       }
    }
    else
    {
        //                  
        pSirSmeScanRsp->statusCode  = eSIR_SME_SUCCESS;
        pSirSmeScanRsp->messageType = eWNI_SME_SCAN_RSP;
        pSirSmeScanRsp->length = curMsgLen;

        /*                                              */
        pSirSmeScanRsp->sessionId   = smesessionId;
        pSirSmeScanRsp->transcationId = smetranscationId;

        mmhMsg.type = eWNI_SME_SCAN_RSP;
        mmhMsg.bodyptr = pSirSmeScanRsp;
        mmhMsg.bodyval = 0;
        MTRACE(macTraceMsgTx(pMac, NO_SESSION, mmhMsg.type));
        limSysProcessMmhMsgApi(pMac, &mmhMsg, ePROT);
        PELOG2(limLog(pMac, LOG2, FL("statusCode : eSIR_SME_SUCCESS"));)
    }

    return;

} /*                             */

#ifdef WLAN_FEATURE_ROAM_SCAN_OFFLOAD
/* 
                         
  
           
                                                                
                                                               
                                           
  
         
  
        
  
              
     
  
       
     
  
                                                      
                                                  
                                                                
                                                
  
               
 */

void
limSendSmeLfrScanRsp(tpAniSirGlobal pMac, tANI_U16 length,
                     tSirResultCodes resultCode,tANI_U8  smesessionId,tANI_U16 smetranscationId)
{
    tSirMsgQ              mmhMsg;
    tpSirSmeScanRsp       pSirSmeScanRsp=NULL;
    tLimScanResultNode    *ptemp = NULL;
    tANI_U16              msgLen, allocLength, curMsgLen = 0;
    tANI_U16              i, bssCount;
    tANI_U8               *pbBuf;
    tSirBssDescription    *pDesc;

    PELOG1(limLog(pMac, LOG1,
       FL("Sending message SME_SCAN_RSP with length=%d reasonCode %s\n"),
       length, limResultCodeStr(resultCode));)

    if (resultCode != eSIR_SME_SUCCESS)
    {
        limPostSmeScanRspMessage(pMac, length, resultCode,smesessionId,smetranscationId);
        return;
    }

    mmhMsg.type = eWNI_SME_SCAN_RSP;
    i = 0;
    bssCount = 0;
    msgLen = 0;
    allocLength = LIM_MAX_NUM_OF_SCAN_RESULTS_REPORTED * LIM_SIZE_OF_EACH_BSS;
    pSirSmeScanRsp = vos_mem_malloc(allocLength);
    if ( NULL == pSirSmeScanRsp )
    {
        //          
        limLog(pMac, LOGP,
                   FL("call to AllocateMemory failed for eWNI_SME_SCAN_RSP\n"));

        return;
    }
    for (i = 0; i < LIM_MAX_NUM_OF_SCAN_RESULTS; i++)
    {
        //                                        
        ptemp = pMac->lim.gLimCachedLfrScanHashTable[i];
        while(ptemp)
        {
            pbBuf = ((tANI_U8 *)pSirSmeScanRsp) + msgLen;
            if(0 == bssCount)
            {
                msgLen = sizeof(tSirSmeScanRsp) -
                           sizeof(tSirBssDescription) +
                           ptemp->bssDescription.length +
                           sizeof(ptemp->bssDescription.length);
                pDesc = pSirSmeScanRsp->bssDescription;
            }
            else
            {
                msgLen += ptemp->bssDescription.length +
                          sizeof(ptemp->bssDescription.length);
                pDesc = (tSirBssDescription *)pbBuf;
            }
            if ( (allocLength < msgLen) ||
                 (LIM_MAX_NUM_OF_SCAN_RESULTS_REPORTED <= bssCount++) )
            {
                pSirSmeScanRsp->statusCode  =
                    eSIR_SME_MORE_SCAN_RESULTS_FOLLOW;
                pSirSmeScanRsp->messageType = eWNI_SME_SCAN_RSP;
                pSirSmeScanRsp->length      = curMsgLen;
                mmhMsg.bodyptr = pSirSmeScanRsp;
                mmhMsg.bodyval = 0;
                MTRACE(macTraceMsgTx(pMac, NO_SESSION, mmhMsg.type));
                limSysProcessMmhMsgApi(pMac, &mmhMsg, ePROT);
                pSirSmeScanRsp = vos_mem_malloc(allocLength);
                if ( NULL == pSirSmeScanRsp )
                {
                    //          
                    limLog(pMac, LOGP,
                                 FL("call to AllocateMemory failed for eWNI_SME_SCAN_RSP\n"));
                    return;
                }
                msgLen = sizeof(tSirSmeScanRsp) -
                           sizeof(tSirBssDescription) +
                           ptemp->bssDescription.length +
                           sizeof(ptemp->bssDescription.length);
                pDesc = pSirSmeScanRsp->bssDescription;
                bssCount = 1;
            }
            curMsgLen = msgLen;

            PELOG2(limLog(pMac, LOG2, FL("ScanRsp : msgLen %d, bssDescr Len=%d\n"),
                          msgLen, ptemp->bssDescription.length);)
            pDesc->length
                    = ptemp->bssDescription.length;
            vos_mem_copy( (tANI_U8 *) &pDesc->bssId,
                          (tANI_U8 *) &ptemp->bssDescription.bssId,
                           ptemp->bssDescription.length);

            PELOG2(limLog(pMac, LOG2, FL("BssId "));
            limPrintMacAddr(pMac, ptemp->bssDescription.bssId, LOG2);)

            pSirSmeScanRsp->sessionId   = smesessionId;
            pSirSmeScanRsp->transcationId = smetranscationId;

            ptemp = ptemp->next;
        } //            
    } //                                                 

    if (0 == bssCount)
    {
       limPostSmeScanRspMessage(pMac, length, resultCode, smesessionId, smetranscationId);
       if (NULL != pSirSmeScanRsp)
       {
           vos_mem_free( pSirSmeScanRsp);
           pSirSmeScanRsp = NULL;
       }
    }
    else
    {
        //                  
        pSirSmeScanRsp->statusCode  = eSIR_SME_SUCCESS;
        pSirSmeScanRsp->messageType = eWNI_SME_SCAN_RSP;
        pSirSmeScanRsp->length = curMsgLen;

        /*                                              */
        pSirSmeScanRsp->sessionId   = smesessionId;
        pSirSmeScanRsp->transcationId = smetranscationId;

        mmhMsg.type = eWNI_SME_SCAN_RSP;
        mmhMsg.bodyptr = pSirSmeScanRsp;
        mmhMsg.bodyval = 0;
        MTRACE(macTraceMsgTx(pMac, NO_SESSION, mmhMsg.type));
        limSysProcessMmhMsgApi(pMac, &mmhMsg, ePROT);
        PELOG2(limLog(pMac, LOG2, FL("statusCode : eSIR_SME_SUCCESS\n"));)
    }

    return;

} /*                                */
#endif //                              

/* 
                             
  
           
                                                         
                                                    
  
       
     
  
                                                      
                                                  
                                         
  
               
 */

void
limPostSmeScanRspMessage(tpAniSirGlobal    pMac,     
                      tANI_U16               length,
                      tSirResultCodes   resultCode,tANI_U8 smesessionId, tANI_U16 smetransactionId)
{
    tpSirSmeScanRsp   pSirSmeScanRsp;
    tSirMsgQ          mmhMsg;

    PELOG1(limLog(pMac, LOG1,
       FL("limPostSmeScanRspMessage: send SME_SCAN_RSP (len %d, reasonCode %s). "),
       length, limResultCodeStr(resultCode));)

    pSirSmeScanRsp = vos_mem_malloc(length);
    if ( NULL == pSirSmeScanRsp )
    {
        limLog(pMac, LOGP, FL("AllocateMemory failed for eWNI_SME_SCAN_RSP"));
        return;
    }
    vos_mem_set((void*)pSirSmeScanRsp, length, 0);

    pSirSmeScanRsp->messageType = eWNI_SME_SCAN_RSP;
    pSirSmeScanRsp->length      = length;

    if(sizeof(tSirSmeScanRsp) <= length)
    {
        pSirSmeScanRsp->bssDescription->length = sizeof(tSirBssDescription);
    }

    pSirSmeScanRsp->statusCode  = resultCode;

    /*                                         */
    pSirSmeScanRsp->sessionId = smesessionId;
    pSirSmeScanRsp->transcationId = smetransactionId;
    
    mmhMsg.type = eWNI_SME_SCAN_RSP;
    mmhMsg.bodyptr = pSirSmeScanRsp;
    mmhMsg.bodyval = 0;

    MTRACE(macTraceMsgTx(pMac, NO_SESSION, mmhMsg.type));
#ifdef FEATURE_WLAN_DIAG_SUPPORT_LIM //                          
    limDiagEventReport(pMac, WLAN_PE_DIAG_SCAN_RSP_EVENT, NULL, (tANI_U16)resultCode, 0);
#endif //                         

    limSysProcessMmhMsgApi(pMac, &mmhMsg, ePROT);
    return;

}  /*                              */

#ifdef FEATURE_OEM_DATA_SUPPORT

/* 
                         
  
           
                                                                
                                                          
            
  
         
  
        
  
              
     
  
       
     
  
                                                      
                                                
                                                                
                                                    
  
               
 */

void limSendSmeOemDataRsp(tpAniSirGlobal pMac, tANI_U32* pMsgBuf, tSirResultCodes resultCode)
{
    tSirMsgQ                      mmhMsg;
    tSirOemDataRsp*               pSirSmeOemDataRsp=NULL;
    tLimMlmOemDataRsp*            pMlmOemDataRsp=NULL;
    tANI_U16                      msgLength;

    
    //                                  
    pMlmOemDataRsp = (tLimMlmOemDataRsp*)(pMsgBuf);

    msgLength = sizeof(tSirOemDataRsp);

    //                                       
    pSirSmeOemDataRsp = vos_mem_malloc(msgLength);
    if (NULL == pSirSmeOemDataRsp)
    {
        limLog(pMac, LOGP, FL("call to AllocateMemory failed for pSirSmeOemDataRsp"));
        return;
    }

#if defined (ANI_LITTLE_BYTE_ENDIAN)
    sirStoreU16N((tANI_U8*)&pSirSmeOemDataRsp->length, msgLength);
    sirStoreU16N((tANI_U8*)&pSirSmeOemDataRsp->messageType, eWNI_SME_OEM_DATA_RSP);
#else
    pSirSmeOemDataRsp->length = msgLength;
    pSirSmeOemDataRsp->messageType = eWNI_SME_OEM_DATA_RSP;
#endif

    vos_mem_copy(pSirSmeOemDataRsp->oemDataRsp, pMlmOemDataRsp->oemDataRsp, OEM_DATA_RSP_SIZE);

    //                                        
    vos_mem_free(pMlmOemDataRsp);

    mmhMsg.type = eWNI_SME_OEM_DATA_RSP;
    mmhMsg.bodyptr = pSirSmeOemDataRsp;
    mmhMsg.bodyval = 0;

    limSysProcessMmhMsgApi(pMac, &mmhMsg, ePROT);

    return;
}  /*                          */

#endif


/* 
                      
  
           
                                                             
                                    
  
         
  
        
  
              
     
  
       
     
  
                                                      
                                                                
                                                
  
               
 */
void
limSendSmeAuthRsp(tpAniSirGlobal pMac,
                  tSirResultCodes statusCode,
                  tSirMacAddr peerMacAddr,
                  tAniAuthType authType,
                  tANI_U16   protStatusCode,
                  tpPESession psessionEntry,tANI_U8 smesessionId,
                  tANI_U16 smetransactionId)
{
#if 0
    tSirMsgQ       mmhMsg;
    tSirSmeAuthRsp *pSirSmeAuthRsp;

    pSirSmeAuthRsp = vos_mem_malloc(sizeof(tSirSmeAuthRsp));
    if (NULL == pSirSmeAuthRsp)
    {
        //          
        limLog(pMac, LOGP,
               FL("call to AllocateMemory failed for eWNI_SME_AUTH_RSP"));

        return;
    }

   

    if(psessionEntry != NULL)
    {
        vos_mem_copy( (tANI_U8 *) pSirSmeAuthRsp->peerMacAddr,
                  (tANI_U8 *) peerMacAddr, sizeof(tSirMacAddr));
        pSirSmeAuthRsp->authType    = authType;
          
    }

    pSirSmeAuthRsp->messageType = eWNI_SME_AUTH_RSP;
    pSirSmeAuthRsp->length      = sizeof(tSirSmeAuthRsp);
    pSirSmeAuthRsp->statusCode  = statusCode;
    pSirSmeAuthRsp->protStatusCode = protStatusCode;
    
    /*                                      */
    pSirSmeAuthRsp->sessionId = smesessionId;
    pSirSmeAuthRsp->transactionId = smetransactionId;  

    mmhMsg.type = eWNI_SME_AUTH_RSP;
    mmhMsg.bodyptr = pSirSmeAuthRsp;
    mmhMsg.bodyval = 0;
    MTRACE(macTraceMsgTx(pMac, 0, mmhMsg.type));
    limSysProcessMmhMsgApi(pMac, &mmhMsg,  ePROT);
#endif
} /*                             */


void limSendSmeDisassocDeauthNtf( tpAniSirGlobal pMac,
                                eHalStatus status, tANI_U32 *pCtx )
{
    tSirMsgQ                mmhMsg;
    tSirMsgQ                *pMsg = (tSirMsgQ*) pCtx;

    mmhMsg.type = pMsg->type;
    mmhMsg.bodyptr = pMsg;
    mmhMsg.bodyval = 0;

    MTRACE(macTraceMsgTx(pMac, NO_SESSION, mmhMsg.type));

    limSysProcessMmhMsgApi(pMac, &mmhMsg, ePROT);
}
/* 
                          
  
           
                                                             
                                            
  
         
  
        
  
              
     
  
       
                                                           
                                                
                          
  
                                                                
                                                      
                                                                   
                                                                    
                                                                  
                                               
  
               
 */
void
limSendSmeDisassocNtf(tpAniSirGlobal pMac,
                      tSirMacAddr peerMacAddr,
                      tSirResultCodes reasonCode,
                      tANI_U16 disassocTrigger,
                      tANI_U16 aid,
                      tANI_U8 smesessionId,
                      tANI_U16 smetransactionId,
                      tpPESession psessionEntry)
{

    tANI_U8                     *pBuf;
    tSirSmeDisassocRsp      *pSirSmeDisassocRsp;
    tSirSmeDisassocInd      *pSirSmeDisassocInd;
    tANI_U32 *pMsg;
    
    switch (disassocTrigger)
    {
        case eLIM_PEER_ENTITY_DISASSOC:
            return;

        case eLIM_HOST_DISASSOC:
            /* 
                                             
                                            
             */

            pSirSmeDisassocRsp = vos_mem_malloc(sizeof(tSirSmeDisassocRsp));
            if ( NULL == pSirSmeDisassocRsp )
            {
                //          
                limLog(pMac, LOGP,
                   FL("call to AllocateMemory failed for eWNI_SME_DISASSOC_RSP"));

                return;
            }

            pSirSmeDisassocRsp->messageType = eWNI_SME_DISASSOC_RSP;
            pSirSmeDisassocRsp->length      = sizeof(tSirSmeDisassocRsp);
            //         
            pBuf = (tANI_U8 *) &pSirSmeDisassocRsp->sessionId;
            *pBuf = smesessionId;
            pBuf++;

            //             
            limCopyU16(pBuf, smetransactionId);
            pBuf += sizeof(tANI_U16);

            //                      
            limCopyU32(pBuf, reasonCode);
            pBuf += sizeof(tSirResultCodes);

            //           
            vos_mem_copy( pBuf, peerMacAddr, sizeof(tSirMacAddr));
            pBuf += sizeof(tSirMacAddr);

            //                    
            //                                                      


          
#ifdef FEATURE_WLAN_DIAG_SUPPORT_LIM //                          

            limDiagEventReport(pMac, WLAN_PE_DIAG_DISASSOC_RSP_EVENT,
                                      psessionEntry, (tANI_U16)reasonCode, 0);
#endif
            pMsg = (tANI_U32*) pSirSmeDisassocRsp;
            break;

        default:
            /* 
                                                              
                                                         
                                             
             */
            pSirSmeDisassocInd = vos_mem_malloc(sizeof(tSirSmeDisassocInd));
            if ( NULL == pSirSmeDisassocInd )
            {
                //          
                limLog(pMac, LOGP,
                   FL("call to AllocateMemory failed for eWNI_SME_DISASSOC_IND"));

                return;
            }

            pSirSmeDisassocInd->messageType = eWNI_SME_DISASSOC_IND;
            pSirSmeDisassocInd->length      = sizeof(tSirSmeDisassocInd);
            
            /*                                          */
            pSirSmeDisassocInd->sessionId = smesessionId;
            pSirSmeDisassocInd->transactionId = smetransactionId;
            pSirSmeDisassocInd->reasonCode = reasonCode;
            pBuf = (tANI_U8 *) &pSirSmeDisassocInd->statusCode;

            limCopyU32(pBuf, reasonCode);
            pBuf += sizeof(tSirResultCodes);

            vos_mem_copy( pBuf, psessionEntry->bssId, sizeof(tSirMacAddr));
            pBuf += sizeof(tSirMacAddr);

            vos_mem_copy( pBuf, peerMacAddr, sizeof(tSirMacAddr));


#ifdef FEATURE_WLAN_DIAG_SUPPORT_LIM //                          
            limDiagEventReport(pMac, WLAN_PE_DIAG_DISASSOC_IND_EVENT,
                                              psessionEntry, (tANI_U16)reasonCode, 0);
#endif
            pMsg = (tANI_U32*) pSirSmeDisassocInd;

            break;
    }

    /*                               */
    if((psessionEntry != NULL) && ((psessionEntry ->limSystemRole ==  eLIM_STA_ROLE) ||
                                  (psessionEntry ->limSystemRole ==  eLIM_BT_AMP_STA_ROLE)) )
    {
        peDeleteSession(pMac,psessionEntry);
    }
        
    limSendSmeDisassocDeauthNtf( pMac, eHAL_STATUS_SUCCESS,
                                              (tANI_U32*) pMsg );
} /*                                 */


/*                                                                   
                                                         
   
                                                              
                                                               
                
    
                                    
                                        
               
     
                                                                    */
void
limSendSmeDisassocInd(tpAniSirGlobal pMac, tpDphHashNode pStaDs,tpPESession psessionEntry)
{
    tSirMsgQ  mmhMsg;
    tSirSmeDisassocInd *pSirSmeDisassocInd;

    pSirSmeDisassocInd = vos_mem_malloc(sizeof(tSirSmeDisassocInd));
    if ( NULL == pSirSmeDisassocInd )
    {
        limLog(pMac, LOGP, FL("AllocateMemory failed for eWNI_SME_DISASSOC_IND"));
        return;
    }

    pSirSmeDisassocInd->messageType = eWNI_SME_DISASSOC_IND;
    pSirSmeDisassocInd->length = sizeof(tSirSmeDisassocInd);

    pSirSmeDisassocInd->sessionId     =  psessionEntry->smeSessionId;
    pSirSmeDisassocInd->transactionId =  psessionEntry->transactionId;
    pSirSmeDisassocInd->statusCode    =  pStaDs->mlmStaContext.disassocReason;
    pSirSmeDisassocInd->reasonCode    =  pStaDs->mlmStaContext.disassocReason;

    vos_mem_copy( pSirSmeDisassocInd->bssId, psessionEntry->bssId, sizeof(tSirMacAddr));

    vos_mem_copy( pSirSmeDisassocInd->peerMacAddr, pStaDs->staAddr, sizeof(tSirMacAddr));

    pSirSmeDisassocInd->staId = pStaDs->staIndex;

    mmhMsg.type = eWNI_SME_DISASSOC_IND;
    mmhMsg.bodyptr = pSirSmeDisassocInd;
    mmhMsg.bodyval = 0;

    MTRACE(macTraceMsgTx(pMac, psessionEntry->peSessionId, mmhMsg.type));
#ifdef FEATURE_WLAN_DIAG_SUPPORT_LIM //                          
    limDiagEventReport(pMac, WLAN_PE_DIAG_DISASSOC_IND_EVENT, psessionEntry, 0, (tANI_U16)pStaDs->mlmStaContext.disassocReason); 
#endif //                         

    limSysProcessMmhMsgApi(pMac, &mmhMsg, ePROT);
 
} /*                                 */


/*                                                                   
                                                     
   
                                                                
                                                             
                
    
                                    
                                        
               
     
                                                                    */
void
limSendSmeDeauthInd(tpAniSirGlobal pMac, tpDphHashNode pStaDs, tpPESession psessionEntry)                   
{
    tSirMsgQ  mmhMsg;
    tSirSmeDeauthInd  *pSirSmeDeauthInd;

    pSirSmeDeauthInd = vos_mem_malloc(sizeof(tSirSmeDeauthInd));
    if ( NULL == pSirSmeDeauthInd )
    {
        limLog(pMac, LOGP, FL("AllocateMemory failed for eWNI_SME_DEAUTH_IND "));
        return;
    }

    pSirSmeDeauthInd->messageType = eWNI_SME_DEAUTH_IND;
    pSirSmeDeauthInd->length = sizeof(tSirSmeDeauthInd);

    pSirSmeDeauthInd->sessionId = psessionEntry->smeSessionId;
    pSirSmeDeauthInd->transactionId = psessionEntry->transactionId;
    if(eSIR_INFRA_AP_MODE == psessionEntry->bssType)
    {
        pSirSmeDeauthInd->statusCode = (tSirResultCodes)pStaDs->mlmStaContext.cleanupTrigger;
    }
    else
    {
        //                                              
        pSirSmeDeauthInd->statusCode = (tSirResultCodes)pStaDs->mlmStaContext.disassocReason;
    }
    //     
    vos_mem_copy( pSirSmeDeauthInd->bssId, psessionEntry->bssId, sizeof(tSirMacAddr));
    //           
    vos_mem_copy( pSirSmeDeauthInd->peerMacAddr, pStaDs->staAddr, sizeof(tSirMacAddr));
    pSirSmeDeauthInd->reasonCode = pStaDs->mlmStaContext.disassocReason;


    pSirSmeDeauthInd->staId = pStaDs->staIndex;

    mmhMsg.type = eWNI_SME_DEAUTH_IND;
    mmhMsg.bodyptr = pSirSmeDeauthInd;
    mmhMsg.bodyval = 0;

    MTRACE(macTraceMsgTx(pMac, psessionEntry->peSessionId, mmhMsg.type));
#ifdef FEATURE_WLAN_DIAG_SUPPORT_LIM //                          
    limDiagEventReport(pMac, WLAN_PE_DIAG_DEAUTH_IND_EVENT, psessionEntry, 0, pStaDs->mlmStaContext.cleanupTrigger);
#endif //                         

    limSysProcessMmhMsgApi(pMac, &mmhMsg, ePROT);
    return;
} /*                               */

#ifdef FEATURE_WLAN_TDLS
/* 
                            
  
           
                                                                        
  
        
  
              
  
       
     
  
                                                   
                                                         
                                                       
                                                    
               
 */
void
limSendSmeTDLSDelStaInd(tpAniSirGlobal pMac, tpDphHashNode pStaDs, tpPESession psessionEntry, tANI_U16 reasonCode)
{
    tSirMsgQ  mmhMsg;
    tSirTdlsDelStaInd  *pSirTdlsDelStaInd;

    pSirTdlsDelStaInd = vos_mem_malloc(sizeof(tSirTdlsDelStaInd));
    if ( NULL == pSirTdlsDelStaInd )
    {
        limLog(pMac, LOGP, FL("AllocateMemory failed for eWNI_SME_TDLS_DEL_STA_IND "));
        return;
    }

    //           
    pSirTdlsDelStaInd->messageType = eWNI_SME_TDLS_DEL_STA_IND;
    pSirTdlsDelStaInd->length = sizeof(tSirTdlsDelStaInd);

    //         
    pSirTdlsDelStaInd->sessionId = psessionEntry->smeSessionId;

    //           
    vos_mem_copy( pSirTdlsDelStaInd->peerMac, pStaDs->staAddr, sizeof(tSirMacAddr));

    //     
    limCopyU16((tANI_U8*)(&pSirTdlsDelStaInd->staId), (tANI_U16)pStaDs->staIndex);

    //          
    limCopyU16((tANI_U8*)(&pSirTdlsDelStaInd->reasonCode), reasonCode);

    mmhMsg.type = eWNI_SME_TDLS_DEL_STA_IND;
    mmhMsg.bodyptr = pSirTdlsDelStaInd;
    mmhMsg.bodyval = 0;


    limSysProcessMmhMsgApi(pMac, &mmhMsg, ePROT);
    return;
}/*                                   */

/* 
                                   
  
           
                                                                     
                  
  
        
  
              
  
       
     
  
                                                   
                                                       
               
 */
void
limSendSmeTDLSDeleteAllPeerInd(tpAniSirGlobal pMac, tpPESession psessionEntry)
{
    tSirMsgQ  mmhMsg;
    tSirTdlsDelAllPeerInd  *pSirTdlsDelAllPeerInd;

    pSirTdlsDelAllPeerInd = vos_mem_malloc(sizeof(tSirTdlsDelAllPeerInd));
    if ( NULL == pSirTdlsDelAllPeerInd )
    {
        limLog(pMac, LOGP, FL("AllocateMemory failed for eWNI_SME_TDLS_DEL_ALL_PEER_IND"));
        return;
    }

    //           
    pSirTdlsDelAllPeerInd->messageType = eWNI_SME_TDLS_DEL_ALL_PEER_IND;
    pSirTdlsDelAllPeerInd->length = sizeof(tSirTdlsDelAllPeerInd);

    //         
    pSirTdlsDelAllPeerInd->sessionId = psessionEntry->smeSessionId;

    mmhMsg.type = eWNI_SME_TDLS_DEL_ALL_PEER_IND;
    mmhMsg.bodyptr = pSirTdlsDelAllPeerInd;
    mmhMsg.bodyval = 0;


    limSysProcessMmhMsgApi(pMac, &mmhMsg, ePROT);
    return;
}/*                                          */

/* 
                               
  
           
                                                                          
                  
  
        
  
              
  
       
     
  
                                                   
                                                       
                                                               
               
 */
void
limSendSmeMgmtTXCompletion(tpAniSirGlobal pMac,
                           tpPESession psessionEntry,
                           tANI_U32 txCompleteStatus)
{
    tSirMsgQ  mmhMsg;
    tSirMgmtTxCompletionInd  *pSirMgmtTxCompletionInd;

    pSirMgmtTxCompletionInd = vos_mem_malloc(sizeof(tSirMgmtTxCompletionInd));
    if ( NULL == pSirMgmtTxCompletionInd )
    {
        limLog(pMac, LOGP, FL("AllocateMemory failed for eWNI_SME_MGMT_FRM_TX_COMPLETION_IND"));
        return;
    }

    //           
    pSirMgmtTxCompletionInd->messageType = eWNI_SME_MGMT_FRM_TX_COMPLETION_IND;
    pSirMgmtTxCompletionInd->length = sizeof(tSirMgmtTxCompletionInd);

    //         
    pSirMgmtTxCompletionInd->sessionId = psessionEntry->smeSessionId;

    pSirMgmtTxCompletionInd->txCompleteStatus = txCompleteStatus;

    mmhMsg.type = eWNI_SME_MGMT_FRM_TX_COMPLETION_IND;
    mmhMsg.bodyptr = pSirMgmtTxCompletionInd;
    mmhMsg.bodyval = 0;


    limSysProcessMmhMsgApi(pMac, &mmhMsg, ePROT);
    return;
}/*                                          */
#endif


/* 
                        
  
           
                                                             
                                            
  
         
  
        
  
              
     
  
       
                                                           
                                                                     
  
                                                                
                                                          
                                                                    
                                                                     
                                                                          
                                       
  
               
 */
void
limSendSmeDeauthNtf(tpAniSirGlobal pMac, tSirMacAddr peerMacAddr, tSirResultCodes reasonCode,
                    tANI_U16 deauthTrigger, tANI_U16 aid,tANI_U8 smesessionId, tANI_U16 smetransactionId)
{
    tANI_U8             *pBuf;
    tSirSmeDeauthRsp    *pSirSmeDeauthRsp;
    tSirSmeDeauthInd    *pSirSmeDeauthInd;
    tpPESession         psessionEntry;
    tANI_U8             sessionId;
    tANI_U32            *pMsg;

    psessionEntry = peFindSessionByBssid(pMac,peerMacAddr,&sessionId);  
    switch (deauthTrigger)
    {
        case eLIM_PEER_ENTITY_DEAUTH:
            return;
            
        case eLIM_HOST_DEAUTH:
            /* 
                                                          
                                
             */
            pSirSmeDeauthRsp = vos_mem_malloc(sizeof(tSirSmeDeauthRsp));
            if ( NULL == pSirSmeDeauthRsp )
            {
                //          
                limLog(pMac, LOGP,
                   FL("call to AllocateMemory failed for eWNI_SME_DEAUTH_RSP"));

                return;
            }

            pSirSmeDeauthRsp->messageType = eWNI_SME_DEAUTH_RSP;
            pSirSmeDeauthRsp->length      = sizeof(tSirSmeDeauthRsp);
            pSirSmeDeauthRsp->statusCode = reasonCode;
            pSirSmeDeauthRsp->sessionId = smesessionId;
            pSirSmeDeauthRsp->transactionId = smetransactionId;  

            pBuf  = (tANI_U8 *) pSirSmeDeauthRsp->peerMacAddr;
            vos_mem_copy( pBuf, peerMacAddr, sizeof(tSirMacAddr));

#ifdef FEATURE_WLAN_DIAG_SUPPORT_LIM //                          
            limDiagEventReport(pMac, WLAN_PE_DIAG_DEAUTH_RSP_EVENT,
                                      psessionEntry, 0, (tANI_U16)reasonCode);
#endif
            pMsg = (tANI_U32*)pSirSmeDeauthRsp;

            break;

        default:
            /* 
                                                                  
                                                         
                                             
             */
            pSirSmeDeauthInd = vos_mem_malloc(sizeof(tSirSmeDeauthInd));
            if ( NULL == pSirSmeDeauthInd )
            {
                //          
                limLog(pMac, LOGP,
                   FL("call to AllocateMemory failed for eWNI_SME_DEAUTH_Ind"));

                return;
            }

            pSirSmeDeauthInd->messageType = eWNI_SME_DEAUTH_IND;
            pSirSmeDeauthInd->length      = sizeof(tSirSmeDeauthInd);
            pSirSmeDeauthInd->reasonCode = eSIR_MAC_UNSPEC_FAILURE_REASON;

            //          
            pBuf = (tANI_U8*) &pSirSmeDeauthInd->sessionId;
            *pBuf++ = smesessionId;

            //              
            limCopyU16(pBuf, smetransactionId);
            pBuf += sizeof(tANI_U16);

            //            
            limCopyU32(pBuf, reasonCode);
            pBuf += sizeof(tSirResultCodes);

            //     
            vos_mem_copy( pBuf, psessionEntry->bssId, sizeof(tSirMacAddr));
            pBuf += sizeof(tSirMacAddr);

            //           
            vos_mem_copy( pSirSmeDeauthInd->peerMacAddr, peerMacAddr, sizeof(tSirMacAddr));

#ifdef FEATURE_WLAN_DIAG_SUPPORT_LIM //                          
            limDiagEventReport(pMac, WLAN_PE_DIAG_DEAUTH_IND_EVENT,
                                        psessionEntry, 0, (tANI_U16)reasonCode);
#endif //                         
            pMsg = (tANI_U32*)pSirSmeDeauthInd;

            break;
    }
    
    /*                               */
    if(psessionEntry != NULL)
    {
        peDeleteSession(pMac,psessionEntry);
    }   

    limSendSmeDisassocDeauthNtf( pMac, eHAL_STATUS_SUCCESS,
                                              (tANI_U32*) pMsg );

} /*                               */


/* 
                                
  
           
                                                             
                                                 
  
         
  
        
  
              
     
  
       
  
                                                                         
                                                                      
                                                           
                                                                              
                                        
  
               
 */
void
limSendSmeWmStatusChangeNtf(tpAniSirGlobal pMac, tSirSmeStatusChangeCode statusChangeCode,
                                 tANI_U32 *pStatusChangeInfo, tANI_U16 infoLen, tANI_U8 sessionId)
{
    tSirMsgQ                  mmhMsg;
    tSirSmeWmStatusChangeNtf  *pSirSmeWmStatusChangeNtf;
    pSirSmeWmStatusChangeNtf = vos_mem_malloc(sizeof(tSirSmeWmStatusChangeNtf));
    if ( NULL == pSirSmeWmStatusChangeNtf )
    {
        limLog(pMac, LOGE,
          FL("call to AllocateMemory failed for eWNI_SME_WM_STATUS_CHANGE_NTF"));
          return;
    }


    mmhMsg.type = eWNI_SME_WM_STATUS_CHANGE_NTF;
    mmhMsg.bodyval = 0;
    mmhMsg.bodyptr = pSirSmeWmStatusChangeNtf;

    switch(statusChangeCode)
    {
        case eSIR_SME_RADAR_DETECTED:

            break;

        case eSIR_SME_CB_LEGACY_BSS_FOUND_BY_AP:
            break;

        case eSIR_SME_BACKGROUND_SCAN_FAIL:
            limPackBkgndScanFailNotify(pMac,
                                       statusChangeCode,
                                       (tpSirBackgroundScanInfo)pStatusChangeInfo,
                                       pSirSmeWmStatusChangeNtf, sessionId);
            break;

        default:
        pSirSmeWmStatusChangeNtf->messageType = eWNI_SME_WM_STATUS_CHANGE_NTF;
        pSirSmeWmStatusChangeNtf->statusChangeCode = statusChangeCode;
        pSirSmeWmStatusChangeNtf->length = sizeof(tSirSmeWmStatusChangeNtf);
        pSirSmeWmStatusChangeNtf->sessionId = sessionId;
        if(sizeof(pSirSmeWmStatusChangeNtf->statusChangeInfo) >= infoLen)
        {
            vos_mem_copy( (tANI_U8 *)&pSirSmeWmStatusChangeNtf->statusChangeInfo,
                          (tANI_U8 *)pStatusChangeInfo, infoLen);
        }
        limLog(pMac, LOGE, FL("***---*** StatusChg: code 0x%x, length %d ***---***"),
               statusChangeCode, infoLen);
        break;
    }


    MTRACE(macTraceMsgTx(pMac, sessionId, mmhMsg.type));
    if (eSIR_SUCCESS != limSysProcessMmhMsgApi(pMac, &mmhMsg, ePROT))
    {
        vos_mem_free(pSirSmeWmStatusChangeNtf);
        limLog( pMac, LOGP, FL("limSysProcessMmhMsgApi failed"));
    }

} /*                                       */


/* 
                            
  
           
                                                             
                                          
  
         
  
        
  
              
     
  
       
  
                                                      
                                                           
                                               
                                                                      
                              
                                                                
                                                      
  
               
 */
void
limSendSmeSetContextRsp(tpAniSirGlobal pMac,
                        tSirMacAddr peerMacAddr, tANI_U16 aid,
                        tSirResultCodes resultCode,
                        tpPESession psessionEntry,tANI_U8 smesessionId,tANI_U16 smetransactionId)
{

    tANI_U8                   *pBuf;
    tSirMsgQ             mmhMsg;
    tSirSmeSetContextRsp *pSirSmeSetContextRsp;

    pSirSmeSetContextRsp = vos_mem_malloc(sizeof(tSirSmeSetContextRsp));
    if ( NULL == pSirSmeSetContextRsp )
    {
        //          
        limLog(pMac, LOGP,
               FL("call to AllocateMemory failed for SmeSetContextRsp"));

        return;
    }

    pSirSmeSetContextRsp->messageType = eWNI_SME_SETCONTEXT_RSP;
    pSirSmeSetContextRsp->length      = sizeof(tSirSmeSetContextRsp);
    pSirSmeSetContextRsp->statusCode  = resultCode;

    pBuf = pSirSmeSetContextRsp->peerMacAddr;

    vos_mem_copy( pBuf, (tANI_U8 *) peerMacAddr, sizeof(tSirMacAddr));
    pBuf += sizeof(tSirMacAddr);


    /*                                      */
    pSirSmeSetContextRsp->sessionId = smesessionId;
    pSirSmeSetContextRsp->transactionId = smetransactionId;

    mmhMsg.type = eWNI_SME_SETCONTEXT_RSP;
    mmhMsg.bodyptr = pSirSmeSetContextRsp;
    mmhMsg.bodyval = 0;
    if(NULL == psessionEntry)
    {
        MTRACE(macTraceMsgTx(pMac, NO_SESSION, mmhMsg.type));
    }
    else
    {
        MTRACE(macTraceMsgTx(pMac, psessionEntry->peSessionId, mmhMsg.type));
    }

#ifdef FEATURE_WLAN_DIAG_SUPPORT_LIM //                          
    limDiagEventReport(pMac, WLAN_PE_DIAG_SETCONTEXT_RSP_EVENT, psessionEntry, (tANI_U16)resultCode, 0);
#endif //                         
    
    limSysProcessMmhMsgApi(pMac, &mmhMsg, ePROT);
} /*                                   */

/* 
                           
  
           
                                                             
                                         
  
         
  
        
  
              
     
  
       
  
                                                      
                                                           
                                              
                                                                      
                              
                                                                
                                                     
  
               
 */
void
limSendSmeRemoveKeyRsp(tpAniSirGlobal pMac,
                        tSirMacAddr peerMacAddr,
                        tSirResultCodes resultCode,
                        tpPESession psessionEntry,tANI_U8 smesessionId,
                        tANI_U16 smetransactionId)
{
    tANI_U8                 *pBuf;
    tSirMsgQ                mmhMsg;
    tSirSmeRemoveKeyRsp     *pSirSmeRemoveKeyRsp;

    pSirSmeRemoveKeyRsp = vos_mem_malloc(sizeof(tSirSmeRemoveKeyRsp));
    if ( NULL == pSirSmeRemoveKeyRsp )
    {
        //          
        limLog(pMac, LOGP,
               FL("call to AllocateMemory failed for SmeRemoveKeyRsp"));

        return;
    }

    

    if(psessionEntry != NULL)
    {
        pBuf = pSirSmeRemoveKeyRsp->peerMacAddr;
        vos_mem_copy( pBuf, (tANI_U8 *) peerMacAddr, sizeof(tSirMacAddr));
    }
    
    pSirSmeRemoveKeyRsp->messageType = eWNI_SME_REMOVEKEY_RSP;
    pSirSmeRemoveKeyRsp->length      = sizeof(tSirSmeRemoveKeyRsp);
    pSirSmeRemoveKeyRsp->statusCode  = resultCode;
        
    /*                                      */
    pSirSmeRemoveKeyRsp->sessionId = smesessionId;
    pSirSmeRemoveKeyRsp->transactionId = smetransactionId;   
    
    mmhMsg.type = eWNI_SME_REMOVEKEY_RSP;
    mmhMsg.bodyptr = pSirSmeRemoveKeyRsp;
    mmhMsg.bodyval = 0;
    if(NULL == psessionEntry)
    {
        MTRACE(macTraceMsgTx(pMac, NO_SESSION, mmhMsg.type));
    }
    else
    {
        MTRACE(macTraceMsgTx(pMac, psessionEntry->peSessionId, mmhMsg.type));
    }
    limSysProcessMmhMsgApi(pMac, &mmhMsg, ePROT);
} /*                                   */


/* 
                                 
  
           
                                                             
                                            
  
         
  
        
  
              
     
  
       
                                                                     
                                                                      
  
              
               
 */
void
limSendSmePromiscuousModeRsp(tpAniSirGlobal pMac)
{
#if 0
    tSirMsgQ   mmhMsg;
    tSirMbMsg  *pMbMsg;

    pMbMsg = vos_mem_malloc(sizeof(tSirMbMsg));
    if ( NULL == pMbMsg )
    {
        //          
        limLog(pMac, LOGP, FL("call to AllocateMemory failed"));

        return;
    }

    pMbMsg->type   = eWNI_SME_PROMISCUOUS_MODE_RSP;
    pMbMsg->msgLen = 4;

    mmhMsg.type = eWNI_SME_PROMISCUOUS_MODE_RSP;
    mmhMsg.bodyptr = pMbMsg;
    mmhMsg.bodyval = 0;
    MTRACE(macTraceMsgTx(pMac, 0, mmhMsg.type));
    limSysProcessMmhMsgApi(pMac, &mmhMsg, ePROT);
#endif
} /*                                        */



/* 
                             
  
           
                                                              
                                            
  
         
  
        
  
              
     
  
       
                                                                 
                                                                
                                                
  
                                                 
               
 */

void
limSendSmeNeighborBssInd(tpAniSirGlobal pMac,
                         tLimScanResultNode *pBssDescr)
{
    tSirMsgQ                 msgQ;
    tANI_U32                      val;
    tSirSmeNeighborBssInd    *pNewBssInd;

    if ((pMac->lim.gLimSmeState != eLIM_SME_LINK_EST_WT_SCAN_STATE) ||
        ((pMac->lim.gLimSmeState == eLIM_SME_LINK_EST_WT_SCAN_STATE) &&
         pMac->lim.gLimRspReqd))
    {
        //                                       
        //                                  
        //                                          
        return;
    }

    if (wlan_cfgGetInt(pMac, WNI_CFG_NEW_BSS_FOUND_IND, &val) != eSIR_SUCCESS)
    {
        limLog(pMac, LOGP, FL("could not get NEIGHBOR_BSS_IND from CFG"));

        return;
    }

    if (val == 0)
        return;

    /* 
                                             
                                   
                                              
                                                    
                                  
     */
    val = pBssDescr->bssDescription.length + sizeof(tANI_U16) + sizeof(tANI_U32) + sizeof(tANI_U8);
    pNewBssInd = vos_mem_malloc(val);
    if ( NULL == pNewBssInd )
    {
        //          
        limLog(pMac, LOGP,
           FL("call to AllocateMemory failed for eWNI_SME_NEIGHBOR_BSS_IND"));

        return;
    }

    pNewBssInd->messageType = eWNI_SME_NEIGHBOR_BSS_IND;
    pNewBssInd->length      = (tANI_U16) val;
    pNewBssInd->sessionId = 0;

    vos_mem_copy( (tANI_U8 *) pNewBssInd->bssDescription,
                  (tANI_U8 *) &pBssDescr->bssDescription,
                  pBssDescr->bssDescription.length + sizeof(tANI_U16));

    msgQ.type = eWNI_SME_NEIGHBOR_BSS_IND;
    msgQ.bodyptr = pNewBssInd;
    msgQ.bodyval = 0;
    MTRACE(macTraceMsgTx(pMac, NO_SESSION, msgQ.type));
    limSysProcessMmhMsgApi(pMac, &msgQ, ePROT);
} /*                                    */

/*                                                                   
                                                       
                                                            
                                               
                                    
                                          
                                               
               
     
                                                                    */
void
limSendSmeAddtsRsp(tpAniSirGlobal pMac, tANI_U8 rspReqd, tANI_U32 status, tpPESession psessionEntry, 
        tSirMacTspecIE tspec, tANI_U8 smesessionId, tANI_U16 smetransactionId)  
{
    tpSirAddtsRsp     rsp;
    tSirMsgQ          mmhMsg;

    if (! rspReqd)
        return;

    rsp = vos_mem_malloc(sizeof(tSirAddtsRsp));
    if ( NULL == rsp )
    {
        limLog(pMac, LOGP, FL("AllocateMemory failed for ADDTS_RSP"));
        return;
    }

    vos_mem_set( (tANI_U8 *) rsp, sizeof(*rsp), 0);
    rsp->messageType = eWNI_SME_ADDTS_RSP;
    rsp->rc = status;
    rsp->rsp.status = (enum eSirMacStatusCodes) status;
    //                                                                                            
    rsp->rsp.tspec = tspec;
    /*                                          */
    rsp->sessionId = smesessionId;
    rsp->transactionId = smetransactionId;
    
    mmhMsg.type = eWNI_SME_ADDTS_RSP;
    mmhMsg.bodyptr = rsp;
    mmhMsg.bodyval = 0;
    if(NULL == psessionEntry)
    {
        MTRACE(macTraceMsgTx(pMac, NO_SESSION, mmhMsg.type));
    }
    else
    {
        MTRACE(macTraceMsgTx(pMac, psessionEntry->peSessionId, mmhMsg.type));
    }
#ifdef FEATURE_WLAN_DIAG_SUPPORT_LIM //                          
    limDiagEventReport(pMac, WLAN_PE_DIAG_ADDTS_RSP_EVENT, psessionEntry, 0, 0);
#endif //                         
    
    limSysProcessMmhMsgApi(pMac, &mmhMsg, ePROT);
    return;
}

void
limSendSmeAddtsInd(tpAniSirGlobal pMac, tpSirAddtsReqInfo addts)
{
    tpSirAddtsRsp rsp;
    tSirMsgQ      mmhMsg;

    limLog(pMac, LOGW, "SendSmeAddtsInd (token %d, tsid %d, up %d)",
           addts->dialogToken,
           addts->tspec.tsinfo.traffic.tsid,
           addts->tspec.tsinfo.traffic.userPrio);

    rsp = vos_mem_malloc(sizeof(tSirAddtsRsp));
    if ( NULL == rsp )
    {
        //          
        limLog(pMac, LOGP, FL("AllocateMemory failed for ADDTS_IND"));
        return;
    }
    vos_mem_set( (tANI_U8 *) rsp, sizeof(*rsp), 0);

    rsp->messageType     = eWNI_SME_ADDTS_IND;

    vos_mem_copy( (tANI_U8 *) &rsp->rsp, (tANI_U8 *) addts, sizeof(*addts));

    mmhMsg.type = eWNI_SME_ADDTS_IND;
    mmhMsg.bodyptr = rsp;
    mmhMsg.bodyval = 0;
    MTRACE(macTraceMsgTx(pMac, NO_SESSION, mmhMsg.type));
    limSysProcessMmhMsgApi(pMac, &mmhMsg, ePROT);
}

void
limSendSmeDeltsRsp(tpAniSirGlobal pMac, tpSirDeltsReq delts, tANI_U32 status,tpPESession psessionEntry,tANI_U8 smesessionId,tANI_U16 smetransactionId)
{
    tpSirDeltsRsp rsp;
    tSirMsgQ      mmhMsg;

    limLog(pMac, LOGW, "SendSmeDeltsRsp (aid %d, tsid %d, up %d) status %d",
           delts->aid,
           delts->req.tsinfo.traffic.tsid,
           delts->req.tsinfo.traffic.userPrio,
           status);
    if (! delts->rspReqd)
        return;

    rsp = vos_mem_malloc(sizeof(tSirDeltsRsp));
    if ( NULL == rsp )
    {
        //          
        limLog(pMac, LOGP, FL("AllocateMemory failed for DELTS_RSP"));
        return;
    }
    vos_mem_set( (tANI_U8 *) rsp, sizeof(*rsp), 0);
  
    if(psessionEntry != NULL)
    {
        
        rsp->aid             = delts->aid;
        vos_mem_copy( (tANI_U8 *) &rsp->macAddr[0], (tANI_U8 *) &delts->macAddr[0], 6);
        vos_mem_copy( (tANI_U8 *) &rsp->rsp, (tANI_U8 *) &delts->req, sizeof(tSirDeltsReqInfo));
    } 

    
    rsp->messageType     = eWNI_SME_DELTS_RSP;
    rsp->rc              = status;

    /*                                          */
    rsp->sessionId = smesessionId;
    rsp->transactionId = smetransactionId;

    mmhMsg.type = eWNI_SME_DELTS_RSP;
    mmhMsg.bodyptr = rsp;
    mmhMsg.bodyval = 0;
    if(NULL == psessionEntry)
    {
        MTRACE(macTraceMsgTx(pMac, NO_SESSION, mmhMsg.type));
    }
    else
    {
        MTRACE(macTraceMsgTx(pMac, psessionEntry->peSessionId, mmhMsg.type));
    }
#ifdef FEATURE_WLAN_DIAG_SUPPORT_LIM //                          
    limDiagEventReport(pMac, WLAN_PE_DIAG_DELTS_RSP_EVENT, psessionEntry, (tANI_U16)status, 0);
#endif //                         
    
    limSysProcessMmhMsgApi(pMac, &mmhMsg, ePROT);
}

void
limSendSmeDeltsInd(tpAniSirGlobal pMac, tpSirDeltsReqInfo delts, tANI_U16 aid,tpPESession psessionEntry)
{
    tpSirDeltsRsp rsp;
    tSirMsgQ      mmhMsg;

    limLog(pMac, LOGW, "SendSmeDeltsInd (aid %d, tsid %d, up %d)",
           aid,
           delts->tsinfo.traffic.tsid,
           delts->tsinfo.traffic.userPrio);

    rsp = vos_mem_malloc(sizeof(tSirDeltsRsp));
    if ( NULL == rsp )
    {
        //          
        limLog(pMac, LOGP, FL("AllocateMemory failed for DELTS_IND"));
        return;
    }
    vos_mem_set( (tANI_U8 *) rsp, sizeof(*rsp), 0);

    rsp->messageType     = eWNI_SME_DELTS_IND;
    rsp->rc              = eSIR_SUCCESS;
    rsp->aid             = aid;
    vos_mem_copy( (tANI_U8 *) &rsp->rsp, (tANI_U8 *) delts, sizeof(*delts));

    /*                                               */

    rsp->sessionId = psessionEntry->smeSessionId;
    rsp->transactionId = psessionEntry->transactionId;

    mmhMsg.type = eWNI_SME_DELTS_IND;
    mmhMsg.bodyptr = rsp;
    mmhMsg.bodyval = 0;
    MTRACE(macTraceMsgTx(pMac, psessionEntry->peSessionId, mmhMsg.type));
#ifdef FEATURE_WLAN_DIAG_SUPPORT_LIM //                          
    limDiagEventReport(pMac, WLAN_PE_DIAG_DELTS_IND_EVENT, psessionEntry, 0, 0);
#endif //                         

    limSysProcessMmhMsgApi(pMac, &mmhMsg, ePROT);
}

/* 
                       
  
           
                                                                     
                                                                    
                                
  
         
  
        
  
              
     
  
       
     
  
                                                      
                                                   
                            
   
  
               
 */

void
limSendSmeStatsRsp(tpAniSirGlobal pMac, tANI_U16 msgType, void* stats)
{
    tSirMsgQ              mmhMsg;
    tSirSmeRsp           *pMsgHdr = (tSirSmeRsp*) stats;

    switch(msgType)
    {
        case WDA_STA_STAT_RSP:
            mmhMsg.type = eWNI_SME_STA_STAT_RSP;
            break;
        case WDA_AGGR_STAT_RSP:
            mmhMsg.type = eWNI_SME_AGGR_STAT_RSP;
            break;
        case WDA_GLOBAL_STAT_RSP:
            mmhMsg.type = eWNI_SME_GLOBAL_STAT_RSP;
            break;
        case WDA_STAT_SUMM_RSP:
            mmhMsg.type = eWNI_SME_STAT_SUMM_RSP;
            break;      
        default:
            mmhMsg.type = msgType; //                       
            break;
    }

    pMsgHdr->messageType = mmhMsg.type; 

    mmhMsg.bodyptr = stats;
    mmhMsg.bodyval = 0;
    MTRACE(macTraceMsgTx(pMac, NO_SESSION, mmhMsg.type));
    limSysProcessMmhMsgApi(pMac, &mmhMsg, ePROT);                                                  

    return;

} /*                              */

/* 
                              
  
           
                                                                     
                                                                    
                                
  
         
  
        
  
              
     
  
       
     
  
                                                      
                                                   
                            
   
  
               
 */

void
limSendSmePEStatisticsRsp(tpAniSirGlobal pMac, tANI_U16 msgType, void* stats)
{
    tSirMsgQ              mmhMsg;
    tANI_U8 sessionId;
    tAniGetPEStatsRsp *pPeStats = (tAniGetPEStatsRsp *) stats;
    tpPESession pPeSessionEntry;

    //                                  
    pPeSessionEntry = peFindSessionByStaId(pMac, pPeStats->staId, &sessionId);

    //                   
    if(NULL != pPeSessionEntry)
    {
      //                   
      pPeStats->sessionId = pPeSessionEntry->smeSessionId;
    }
 
    pPeStats->msgType = eWNI_SME_GET_STATISTICS_RSP;
    

    //                                        
    mmhMsg.type = eWNI_SME_GET_STATISTICS_RSP;

    mmhMsg.bodyptr = stats;
    mmhMsg.bodyval = 0;
    MTRACE(macTraceMsgTx(pMac, NO_SESSION, mmhMsg.type));
    limSysProcessMmhMsgApi(pMac, &mmhMsg, ePROT);                                                  

    return;

} /*                                     */

#if defined WLAN_FEATURE_VOWIFI_11R || defined FEATURE_WLAN_CCX || defined(FEATURE_WLAN_LFR)
/* 
                               
  
           
                                                             
                                                                    
                                  
  
         
  
        
  
              
     
  
       
     
  
                                                      
                                                  
                            
  
  
               
 */

void
limSendSmePEGetRoamRssiRsp(tpAniSirGlobal pMac, tANI_U16 msgType, void* stats)
{
    tSirMsgQ              mmhMsg;
    tANI_U8 sessionId;
    tAniGetRoamRssiRsp *pPeStats = (tAniGetRoamRssiRsp *) stats;
    tpPESession pPeSessionEntry = NULL;

    //                                  
    pPeSessionEntry = peFindSessionByStaId(pMac, pPeStats->staId, &sessionId);

    //                   
    if(NULL != pPeSessionEntry)
    {
      //                   
      pPeStats->sessionId = pPeSessionEntry->smeSessionId;
    }

    pPeStats->msgType = eWNI_SME_GET_ROAM_RSSI_RSP;

    //                                        
    mmhMsg.type = eWNI_SME_GET_ROAM_RSSI_RSP;

    mmhMsg.bodyptr = stats;
    mmhMsg.bodyval = 0;
    MTRACE(macTraceMsgTx(pMac, sessionId, mmhMsg.type));
    limSysProcessMmhMsgApi(pMac, &mmhMsg, ePROT);

    return;

} /*                                      */

#endif


#if defined(FEATURE_WLAN_CCX) && defined(FEATURE_WLAN_CCX_UPLOAD)
/* 
                          
  
           
                                                             
                                                                    
                                  
  
         
                                                   
                                       
  
               
 */

void
limSendSmePECcxTsmRsp(tpAniSirGlobal pMac, tAniGetTsmStatsRsp *pStats)
{
    tSirMsgQ            mmhMsg;
    tANI_U8             sessionId;
    tAniGetTsmStatsRsp *pPeStats = (tAniGetTsmStatsRsp *) pStats;
    tpPESession         pPeSessionEntry = NULL;

    //                                  
    pPeSessionEntry = peFindSessionByStaId(pMac, pPeStats->staId, &sessionId);

    //                   
    if(NULL != pPeSessionEntry)
    {
      //                   
      pPeStats->sessionId = pPeSessionEntry->smeSessionId;
    }
    else
    {
        PELOGE(limLog(pMac, LOGE, FL("Session not found for the Sta id(%d)"),
            pPeStats->staId);)
        return;
    }

    pPeStats->msgType = eWNI_SME_GET_TSM_STATS_RSP;
    pPeStats->tsmMetrics.RoamingCount = pPeSessionEntry->ccxContext.tsm.tsmMetrics.RoamingCount;
    pPeStats->tsmMetrics.RoamingDly = pPeSessionEntry->ccxContext.tsm.tsmMetrics.RoamingDly;

    mmhMsg.type = eWNI_SME_GET_TSM_STATS_RSP;
    mmhMsg.bodyptr = pStats;
    mmhMsg.bodyval = 0;
    MTRACE(macTraceMsgTx(pMac, sessionId, mmhMsg.type));
    limSysProcessMmhMsgApi(pMac, &mmhMsg, ePROT);

    return;
} /*                                 */

#endif /*                                              */


void
limSendSmeIBSSPeerInd(
    tpAniSirGlobal      pMac,
    tSirMacAddr peerMacAddr,
    tANI_U16    staIndex, 
    tANI_U8     ucastIdx,
    tANI_U8     bcastIdx,
    tANI_U8  *beacon, 
    tANI_U16 beaconLen, 
    tANI_U16 msgType,
    tANI_U8 sessionId)
{
    tSirMsgQ                  mmhMsg;
    tSmeIbssPeerInd *pNewPeerInd;
    
    pNewPeerInd = vos_mem_malloc(sizeof(tSmeIbssPeerInd) + beaconLen);
    if ( NULL == pNewPeerInd )
    {
        PELOGE(limLog(pMac, LOGE, FL("Failed to allocate memory"));)
        return;
    }
    
    vos_mem_set((void *) pNewPeerInd, (sizeof(tSmeIbssPeerInd) + beaconLen), 0);

    vos_mem_copy( (tANI_U8 *) pNewPeerInd->peerAddr,
                   peerMacAddr, sizeof(tSirMacAddr));
    pNewPeerInd->staId= staIndex;
    pNewPeerInd->ucastSig = ucastIdx;
    pNewPeerInd->bcastSig = bcastIdx;
    pNewPeerInd->mesgLen = sizeof(tSmeIbssPeerInd) + beaconLen;
    pNewPeerInd->mesgType = msgType;
    pNewPeerInd->sessionId = sessionId;

    if ( beacon != NULL )
    {
        vos_mem_copy((void*) ((tANI_U8*)pNewPeerInd+sizeof(tSmeIbssPeerInd)),
                     (void*)beacon, beaconLen);
    }

    mmhMsg.type    = msgType;
//                                            
    mmhMsg.bodyptr = pNewPeerInd;
    MTRACE(macTraceMsgTx(pMac, sessionId, mmhMsg.type));
    limSysProcessMmhMsgApi(pMac, &mmhMsg, ePROT);
    
}


/*                                                                   
                                                          
   
                                                                     
                                                              
    
                                    
                                                           
               
     
                                                                    */
void limSendExitBmpsInd(tpAniSirGlobal pMac, tExitBmpsReason reasonCode)
{
    tSirMsgQ  mmhMsg;
    tANI_U16  msgLen = 0;
    tpSirSmeExitBmpsInd  pExitBmpsInd;
 
    msgLen = sizeof(tSirSmeExitBmpsInd);
    pExitBmpsInd = vos_mem_malloc(msgLen);
    if ( NULL == pExitBmpsInd )
    {
        limLog(pMac, LOGP, FL("AllocateMemory failed for PMC_EXIT_BMPS_IND "));
        return;
    }
    vos_mem_set(pExitBmpsInd, msgLen, 0);

    pExitBmpsInd->mesgType = eWNI_PMC_EXIT_BMPS_IND;
    pExitBmpsInd->mesgLen = msgLen;
    pExitBmpsInd->exitBmpsReason = reasonCode;
    pExitBmpsInd->statusCode = eSIR_SME_SUCCESS;

    mmhMsg.type = eWNI_PMC_EXIT_BMPS_IND;
    mmhMsg.bodyptr = pExitBmpsInd;
    mmhMsg.bodyval = 0;
  
    PELOG1(limLog(pMac, LOG1, FL("Sending eWNI_PMC_EXIT_BMPS_IND to SME. "));)
    MTRACE(macTraceMsgTx(pMac, NO_SESSION, mmhMsg.type));
#ifdef FEATURE_WLAN_DIAG_SUPPORT_LIM //                          
    limDiagEventReport(pMac, WLAN_PE_DIAG_EXIT_BMPS_IND_EVENT, peGetValidPowerSaveSession(pMac), 0, (tANI_U16)reasonCode);
#endif //                         
    
    limSysProcessMmhMsgApi(pMac, &mmhMsg,  ePROT);
    return;

} /*                              */




/*                                                                          
                                                                     

    
                                                                   
                                                 
    
     
                                                                            */

void limHandleDeleteBssRsp(tpAniSirGlobal pMac,tpSirMsgQ MsgQ)
{
    tpPESession psessionEntry;
    tpDeleteBssParams pDelBss = (tpDeleteBssParams)(MsgQ->bodyptr);
    if((psessionEntry = peFindSessionBySessionId(pMac,pDelBss->sessionId))==NULL)
    {
        limLog(pMac, LOGP,FL("Session Does not exist for given sessionID"));
        return;
    }
    if (psessionEntry->limSystemRole == eLIM_STA_IN_IBSS_ROLE)
    {
        limIbssDelBssRsp(pMac, MsgQ->bodyptr,psessionEntry);
    }    
    else if(psessionEntry->limSystemRole == eLIM_UNKNOWN_ROLE)
    {
         limProcessSmeDelBssRsp(pMac, MsgQ->bodyval,psessionEntry);
    }
           
    else
         limProcessMlmDelBssRsp(pMac,MsgQ,psessionEntry);
    
}

#ifdef WLAN_FEATURE_VOWIFI_11R
/*                                                                   
                                                               
                                                                  
                                               
                                    
                                          
                                                         
               
     
                                                                    */
void
limSendSmeAggrQosRsp(tpAniSirGlobal pMac, tpSirAggrQosRsp aggrQosRsp, 
                     tANI_U8 smesessionId)
{
    tSirMsgQ         mmhMsg;

    mmhMsg.type = eWNI_SME_FT_AGGR_QOS_RSP;
    mmhMsg.bodyptr = aggrQosRsp;
    mmhMsg.bodyval = 0;
    MTRACE(macTraceMsgTx(pMac, smesessionId, mmhMsg.type));
    limSysProcessMmhMsgApi(pMac, &mmhMsg, ePROT);

    return;
}
#endif

/*                                                                   
                                                                      
                                                     
   
                                                           
    
                                    
               
     
                                                                    */
void
limSendSmePreChannelSwitchInd(tpAniSirGlobal pMac)
{
    tSirMsgQ         mmhMsg;

    mmhMsg.type = eWNI_SME_PRE_SWITCH_CHL_IND;
    mmhMsg.bodyptr = NULL;
    mmhMsg.bodyval = 0;
    MTRACE(macTraceMsgTx(pMac, NO_SESSION, mmhMsg.type));
    limSysProcessMmhMsgApi(pMac, &mmhMsg, ePROT);

    return;
}

/*                                                                   
                                                                       
                                                            
   
                                                            
    
                                    
               
     
                                                                    */
void
limSendSmePostChannelSwitchInd(tpAniSirGlobal pMac)
{
    tSirMsgQ         mmhMsg;

    mmhMsg.type = eWNI_SME_POST_SWITCH_CHL_IND;
    mmhMsg.bodyptr = NULL;
    mmhMsg.bodyval = 0;
    MTRACE(macTraceMsgTx(pMac, NO_SESSION, mmhMsg.type));
    limSysProcessMmhMsgApi(pMac, &mmhMsg, ePROT);

    return;
}

void limSendSmeMaxAssocExceededNtf(tpAniSirGlobal pMac, tSirMacAddr peerMacAddr,
                    tANI_U8 smesessionId)
{
    tSirMsgQ         mmhMsg;
    tSmeMaxAssocInd *pSmeMaxAssocInd;

    pSmeMaxAssocInd = vos_mem_malloc(sizeof(tSmeMaxAssocInd));
    if ( NULL == pSmeMaxAssocInd )
    {
        PELOGE(limLog(pMac, LOGE, FL("Failed to allocate memory"));)
        return;
    }    
    vos_mem_set((void *) pSmeMaxAssocInd, sizeof(tSmeMaxAssocInd),0);
    vos_mem_copy( (tANI_U8 *)pSmeMaxAssocInd->peerMac,
                  (tANI_U8 *)peerMacAddr, sizeof(tSirMacAddr));
    pSmeMaxAssocInd->mesgType  = eWNI_SME_MAX_ASSOC_EXCEEDED;
    pSmeMaxAssocInd->mesgLen    = sizeof(tSmeMaxAssocInd); 
    pSmeMaxAssocInd->sessionId = smesessionId;
    mmhMsg.type = pSmeMaxAssocInd->mesgType;
    mmhMsg.bodyptr = pSmeMaxAssocInd;
    PELOG1(limLog(pMac, LOG1, FL("msgType %s peerMacAddr "MAC_ADDRESS_STR
                  " sme session id %d"), "eWNI_SME_MAX_ASSOC_EXCEEDED", MAC_ADDR_ARRAY(peerMacAddr));)
    MTRACE(macTraceMsgTx(pMac, smesessionId, mmhMsg.type));
    limSysProcessMmhMsgApi(pMac, &mmhMsg, ePROT);

    return;
}
#ifdef WLAN_FEATURE_ROAM_SCAN_OFFLOAD
/*                                                                   
                                              
                                     

                                                                
                                                                
                                  

                                    
                                     
              
     
                                                                    */
void
limSendSmeCandidateFoundInd(tpAniSirGlobal pMac, tANI_U8  sessionId)
{
    tSirMsgQ  mmhMsg;
    tSirSmeCandidateFoundInd *pSirSmeCandidateFoundInd;

    pSirSmeCandidateFoundInd = vos_mem_malloc(sizeof(tSirSmeCandidateFoundInd));
    if ( NULL == pSirSmeCandidateFoundInd )
    {
        limLog(pMac, LOGP, FL("AllocateMemory failed for eWNI_SME_CANDIDATE_FOUND_IND\n"));
        return;
    }

    pSirSmeCandidateFoundInd->messageType = eWNI_SME_CANDIDATE_FOUND_IND;
    pSirSmeCandidateFoundInd->length = sizeof(tSirSmeDisassocInd);

    pSirSmeCandidateFoundInd->sessionId     =  sessionId;


    limLog( pMac, LOGE, FL("posting candidate ind to SME"));
    mmhMsg.type = eWNI_SME_CANDIDATE_FOUND_IND;
    mmhMsg.bodyptr = pSirSmeCandidateFoundInd;
    mmhMsg.bodyval = 0;

    limSysProcessMmhMsgApi(pMac, &mmhMsg, ePROT);

} /*                                       */
#endif //                              

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

                      s a p F s m . C

  OVERVIEW:

  This software unit holds the implementation of the WLAN SAP Finite
  State Machine modules

  DEPENDENCIES:

  Are listed for each API below.


  Copyright (c) 2010 QUALCOMM Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*                                                                           

                                           


                                                                       
                                                                



                                      
                                                                             
                                 

                                                                           */


/*                                                                            
                
                                                                            */
#include "sapInternal.h"
//                                
#include "sme_Api.h"
//                                
#include "pmcApi.h"
#include "wlan_nv.h"

/*                                                                            
                                         
                                                                            */

/*                                                                            
                    
                                                                            */

/*                                                                            
                          
                                                                            */

/*                                                                            
                                            
                                                                            */
#ifdef FEATURE_WLAN_CH_AVOID
extern safeChannelType safeChannels[];
#endif /*                       */
/*                                                                            
                              
                                                                            */

/*                                                                            
                                               
                                                                            */
#ifdef SOFTAP_CHANNEL_RANGE
static VOS_STATUS sapGetChannelList(ptSapContext sapContext, v_U8_t **channelList,
                                 v_U8_t  *numberOfChannels);
#endif
/*                                                                            
                                    
                                                                           */

/*                                                                            
                                          
                                                                            */

/*                                                                          
                          

             
                                                      

              
       

            

      
                                     

              

        

              
                                                                            */
static inline void sapEventInit(ptWLAN_SAPEvent sapEvent)
{
   sapEvent->event = eSAP_MAC_SCAN_COMPLETE;
   sapEvent->params = 0;
   sapEvent->u1 = 0;
   sapEvent->u2 = 0;
}

/*                                                                          
                               

             
                                                

              
       

            

      
                                   
                                     

              
                                                                

                               

              
                                                                            */
VOS_STATUS
sapGotoChannelSel
(
    ptSapContext sapContext,
    ptWLAN_SAPEvent sapEvent
)
{
    /*                         */
    eHalStatus halStatus;
    tCsrScanRequest scanRequest;/*                                       */
    v_U32_t    scanRequestID = 0;
    VOS_STATUS vosStatus = VOS_STATUS_SUCCESS;

#ifdef SOFTAP_CHANNEL_RANGE
    v_U8_t     *channelList = NULL;
    v_U8_t     numOfChannels = 0 ;
#endif
    tHalHandle hHal;
    tANI_U8   channel;

    hHal = (tHalHandle)vos_get_context( VOS_MODULE_ID_SME, sapContext->pvosGCtx);
    if (NULL == hHal)
    {
        /*                           */
        VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_FATAL,
                   "In %s, invalid hHal", __func__);
        return VOS_STATUS_E_FAULT;
    }

    /*                                                                                                                             */
    if (vos_get_concurrency_mode() == VOS_STA_SAP)
    {
        channel = sme_GetConcurrentOperationChannel(hHal);

        if (channel)
        { /*                                                           
                                                             */
            sapContext->channel = channel;
        }
    }

    if (sapContext->channel == AUTO_CHANNEL_SELECT)
    {
        vos_mem_zero(&scanRequest, sizeof(scanRequest));

        /*                              */
        scanRequest.scanType = eSIR_PASSIVE_SCAN;

        /*                                      */
        scanRequest.minChnTime = 0;
        scanRequest.maxChnTime = 0;

        /*                             */
        scanRequest.BSSType = eCSR_BSS_TYPE_ANY;

#ifndef SOFTAP_CHANNEL_RANGE
        /*                      */
        scanRequest.ChannelInfo.numOfChannels = 0;

        scanRequest.ChannelInfo.ChannelList = NULL;

        scanRequest.requestType = eCSR_SCAN_REQUEST_FULL_SCAN;//                           

#else

        sapGetChannelList(sapContext, &channelList, &numOfChannels);

        /*                             */
        scanRequest.ChannelInfo.numOfChannels = numOfChannels;

        scanRequest.ChannelInfo.ChannelList = channelList;

        scanRequest.requestType = eCSR_SCAN_SOFTAP_CHANNEL_RANGE;

        sapContext->channelList = channelList;

#endif
        /*                              */

        VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, calling sme_ScanRequest", __func__);

        halStatus = sme_ScanRequest(hHal,
                            0,//                          
                            &scanRequest,
                            &scanRequestID,//                                                                                          
                            &WLANSAP_ScanCallback,//                                 
                            sapContext);//                                       
        if (eHAL_STATUS_SUCCESS != halStatus)
        {
            VOS_TRACE(VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR, "%s:sme_ScanRequest  fail %d!!!", __func__, halStatus);
            VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH, "SoftAP Configuring for default channel, Ch= %d", sapContext->channel);
            /*                                             */
            sapContext->channel = SAP_DEFAULT_CHANNEL;

#ifdef SOFTAP_CHANNEL_RANGE
            if(sapContext->channelList != NULL)
            {
                sapContext->channel = sapContext->channelList[0];
                vos_mem_free(sapContext->channelList);
                sapContext->channelList = NULL;
            }
#endif
            /*                             */
            sapEventInit(sapEvent);
            /*              */
            vosStatus = sapFsm(sapContext, sapEvent);
        }
        else
        {
            VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, return from sme_ScanRequest, scanRequestID=%d, Ch= %d",
                   __func__, scanRequestID, sapContext->channel);
        }

    }
    else
    {
        VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, for configured channel, Ch= %d", __func__, sapContext->channel);
        /*                             */
        //                                                                  
        sapEventInit(sapEvent);
        /*              */
        vosStatus = sapFsm(sapContext, sapEvent);
    }

    /*                                                                                               */
    /*                                                                                                                */
    VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, before exiting sapGotoChannelSel channel=%d", __func__, sapContext->channel);

    return VOS_STATUS_SUCCESS;
}//                  

/*                                                                          
                             

             
                                                     

              
       

            

      
                                   
                                     
                                               
                                            

              
                                                                

                               

              
                                                                            */
VOS_STATUS
sapGotoStarting
(
    ptSapContext sapContext,
    ptWLAN_SAPEvent sapEvent,
    eCsrRoamBssType bssType
)
{
    /*            */
    tHalHandle hHal = VOS_GET_HAL_CB(sapContext->pvosGCtx);
    eHalStatus halStatus;

    /*                                                                      */
    char key_material[32]={ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1,};
    sapContext->key_type = 0x05;
    sapContext->key_length = 32;
    vos_mem_copy(sapContext->key_material, key_material, sizeof(key_material));  /*                        */

    VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s", __func__);
    if (NULL == hHal)
    {
        /*                           */
        VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_FATAL,
                   "In %s, invalid hHal", __func__);
        return VOS_STATUS_E_FAULT;
    }

    //                                     
    halStatus = pmcRequestFullPower( hHal,
                            WLANSAP_pmcFullPwrReqCB,
                            sapContext,
                            eSME_REASON_OTHER);

    /*                             */
    halStatus = sme_OpenSession(hHal,
                        &WLANSAP_RoamCallback,
                        sapContext,
                        sapContext->self_mac_addr,
                        &sapContext->sessionId);

    if(eHAL_STATUS_SUCCESS != halStatus )
    {
        VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR, "Error: In %s calling sme_RoamConnect status = %d", __func__, halStatus);
        return VOS_STATUS_E_FAILURE;
    }

    return VOS_STATUS_SUCCESS;
}//                

/*                                                                          
                                  

             
                                             

              
       

            

      
                                   
                                            

              
                                                                

                               

              
                                                                            */
VOS_STATUS
sapGotoDisconnecting
(
    ptSapContext sapContext
)
{
    eHalStatus halStatus;
    tHalHandle hHal;

    hHal = VOS_GET_HAL_CB(sapContext->pvosGCtx);
    if (NULL == hHal)
    {
        /*                           */
        VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                   "In %s, invalid hHal", __func__);
        return VOS_STATUS_E_FAULT;
    }

    sapFreeRoamProfile(&sapContext->csrRoamProfile);
    halStatus = sme_RoamStopBss(hHal, sapContext->sessionId);
    if(eHAL_STATUS_SUCCESS != halStatus )
    {
        VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR, "Error: In %s calling sme_RoamStopBss status = %d", __func__, halStatus);
        return VOS_STATUS_E_FAILURE;
    }

    return VOS_STATUS_SUCCESS;
}

static eHalStatus sapRoamSessionCloseCallback(void *pContext)
{
    ptSapContext sapContext = (ptSapContext)pContext;
    return sapSignalHDDevent(sapContext, NULL,
                    eSAP_STOP_BSS_EVENT, (v_PVOID_t) eSAP_STATUS_SUCCESS);
}

/*                                                                          
                                 

             
                                                           

              
       

            

      
                                   
                                     
                                            

              
                                                                

                               

              
                                                                            */
VOS_STATUS
sapGotoDisconnected
(
    ptSapContext sapContext
)
{
    VOS_STATUS vosStatus = VOS_STATUS_E_FAILURE;
    tWLAN_SAPEvent sapEvent;
    //                           
    //                                                                    
    sapEvent.event = eSAP_MAC_READY_FOR_CONNECTIONS;//          
    sapEvent.params = 0;
    sapEvent.u1 = 0;
    sapEvent.u2 = 0;
    /*              */
    vosStatus = sapFsm(sapContext, &sapEvent);

    return vosStatus;
}

/*                                                                          
                               

             
                                                                  

              
       

            

      
                                   
                                                  
                                    
                                                             

              
                                                                

                               

              
                                                                            */
VOS_STATUS
sapSignalHDDevent
(
    ptSapContext sapContext, /*                  */
    tCsrRoamInfo *pCsrRoamInfo,
    eSapHddEvent sapHddevent,
    void         *context
)
{
    VOS_STATUS  vosStatus = VOS_STATUS_SUCCESS;
    tSap_Event sapApAppEvent; /*                                  */
    /*                                                                       */

    /*                                                  */
    VOS_ASSERT(sapContext->pfnSapEventCallback);

    switch (sapHddevent)
    {
        case eSAP_STA_ASSOC_IND:
            //                                                    
            sapApAppEvent.sapHddEventCode = eSAP_STA_ASSOC_IND;

            vos_mem_copy( &sapApAppEvent.sapevt.sapAssocIndication.staMac, pCsrRoamInfo->peerMac,sizeof(tSirMacAddr));
            sapApAppEvent.sapevt.sapAssocIndication.staId = pCsrRoamInfo->staId;
            sapApAppEvent.sapevt.sapAssocIndication.status = 0;
            //                                                  
            sapApAppEvent.sapevt.sapAssocIndication.beaconLength = pCsrRoamInfo->beaconLength;
            sapApAppEvent.sapevt.sapAssocIndication.beaconPtr = pCsrRoamInfo->beaconPtr;
            sapApAppEvent.sapevt.sapAssocIndication.assocReqLength = pCsrRoamInfo->assocReqLength;
            sapApAppEvent.sapevt.sapAssocIndication.assocReqPtr = pCsrRoamInfo->assocReqPtr;
            sapApAppEvent.sapevt.sapAssocIndication.fWmmEnabled = pCsrRoamInfo->wmmEnabledSta;
            if ( pCsrRoamInfo->u.pConnectedProfile != NULL )
            {
               sapApAppEvent.sapevt.sapAssocIndication.negotiatedAuthType = pCsrRoamInfo->u.pConnectedProfile->AuthType;
               sapApAppEvent.sapevt.sapAssocIndication.negotiatedUCEncryptionType = pCsrRoamInfo->u.pConnectedProfile->EncryptionType;
               sapApAppEvent.sapevt.sapAssocIndication.negotiatedMCEncryptionType = pCsrRoamInfo->u.pConnectedProfile->mcEncryptionType;
               sapApAppEvent.sapevt.sapAssocIndication.fAuthRequired = pCsrRoamInfo->fAuthRequired;
            }
            break;
       case eSAP_START_BSS_EVENT:
            VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, SAP event callback event = %s",
                __func__, "eSAP_START_BSS_EVENT");
            sapApAppEvent.sapHddEventCode = eSAP_START_BSS_EVENT;
            sapApAppEvent.sapevt.sapStartBssCompleteEvent.status = (eSapStatus )context;
            if(pCsrRoamInfo != NULL ){
                sapApAppEvent.sapevt.sapStartBssCompleteEvent.staId = pCsrRoamInfo->staId;
            }
            else
                sapApAppEvent.sapevt.sapStartBssCompleteEvent.staId = 0;
            sapApAppEvent.sapevt.sapStartBssCompleteEvent.operatingChannel = (v_U8_t)sapContext->channel;
            break;

        case eSAP_STOP_BSS_EVENT:
            VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, SAP event callback event = %s",
                       __func__, "eSAP_STOP_BSS_EVENT");
            sapApAppEvent.sapHddEventCode = eSAP_STOP_BSS_EVENT;
            sapApAppEvent.sapevt.sapStopBssCompleteEvent.status = (eSapStatus )context;
            break;

        case eSAP_STA_ASSOC_EVENT:
        {
            VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, SAP event callback event = %s",
                __func__, "eSAP_STA_ASSOC_EVENT");
            if (pCsrRoamInfo->fReassocReq)
                sapApAppEvent.sapHddEventCode = eSAP_STA_REASSOC_EVENT;
            else
                sapApAppEvent.sapHddEventCode = eSAP_STA_ASSOC_EVENT;

            //                                                          
            vos_mem_copy( &sapApAppEvent.sapevt.sapStationAssocReassocCompleteEvent.staMac,
                         pCsrRoamInfo->peerMac,sizeof(tSirMacAddr));
            sapApAppEvent.sapevt.sapStationAssocReassocCompleteEvent.staId = pCsrRoamInfo->staId ;
            sapApAppEvent.sapevt.sapStationAssocReassocCompleteEvent.statusCode = pCsrRoamInfo->statusCode;
            sapApAppEvent.sapevt.sapStationAssocReassocCompleteEvent.iesLen = pCsrRoamInfo->rsnIELen;
            vos_mem_copy(sapApAppEvent.sapevt.sapStationAssocReassocCompleteEvent.ies, pCsrRoamInfo->prsnIE,
                        pCsrRoamInfo->rsnIELen);

            if(pCsrRoamInfo->addIELen)
            {
                v_U8_t  len = sapApAppEvent.sapevt.sapStationAssocReassocCompleteEvent.iesLen;
                sapApAppEvent.sapevt.sapStationAssocReassocCompleteEvent.iesLen
                                                        += pCsrRoamInfo->addIELen;
                vos_mem_copy(&sapApAppEvent.sapevt.sapStationAssocReassocCompleteEvent.ies[len], pCsrRoamInfo->paddIE,
                            pCsrRoamInfo->addIELen);
            }

            sapApAppEvent.sapevt.sapStationAssocReassocCompleteEvent.wmmEnabled = pCsrRoamInfo->wmmEnabledSta;
            sapApAppEvent.sapevt.sapStationAssocReassocCompleteEvent.status = (eSapStatus )context;
            //                              
            //                                                                                                                  
            break;
        }

        case eSAP_STA_DISASSOC_EVENT:
            VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, SAP event callback event = %s",
                       __func__, "eSAP_STA_DISASSOC_EVENT");
            sapApAppEvent.sapHddEventCode = eSAP_STA_DISASSOC_EVENT;

            vos_mem_copy( &sapApAppEvent.sapevt.sapStationDisassocCompleteEvent.staMac,
                          pCsrRoamInfo->peerMac, sizeof(tSirMacAddr));
            sapApAppEvent.sapevt.sapStationDisassocCompleteEvent.staId = pCsrRoamInfo->staId;
            if (pCsrRoamInfo->reasonCode == eCSR_ROAM_RESULT_FORCED)
                sapApAppEvent.sapevt.sapStationDisassocCompleteEvent.reason = eSAP_USR_INITATED_DISASSOC;
            else
                sapApAppEvent.sapevt.sapStationDisassocCompleteEvent.reason = eSAP_MAC_INITATED_DISASSOC;

            sapApAppEvent.sapevt.sapStationDisassocCompleteEvent.statusCode = pCsrRoamInfo->statusCode;
            sapApAppEvent.sapevt.sapStationDisassocCompleteEvent.status = (eSapStatus )context;
            break;

        case eSAP_STA_SET_KEY_EVENT:
            VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, SAP event callback event = %s",
                       __func__, "eSAP_STA_SET_KEY_EVENT");
            sapApAppEvent.sapHddEventCode = eSAP_STA_SET_KEY_EVENT;
            sapApAppEvent.sapevt.sapStationSetKeyCompleteEvent.status = (eSapStatus )context;
            vos_mem_copy(&sapApAppEvent.sapevt.sapStationSetKeyCompleteEvent.peerMacAddr,
                         pCsrRoamInfo->peerMac,sizeof(tSirMacAddr));
            break;

        case eSAP_STA_DEL_KEY_EVENT :
            VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, SAP event callback event = %s",
                       __func__, "eSAP_STA_DEL_KEY_EVENT");
            sapApAppEvent.sapHddEventCode = eSAP_STA_DEL_KEY_EVENT;
            sapApAppEvent.sapevt.sapStationDeleteKeyCompleteEvent.status = (eSapStatus )context;
            //                                                
            //                                                               
            break;

        case eSAP_STA_MIC_FAILURE_EVENT :
            VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, SAP event callback event = %s",
                        __func__, "eSAP_STA_MIC_FAILURE_EVENT");
            sapApAppEvent.sapHddEventCode = eSAP_STA_MIC_FAILURE_EVENT;
            vos_mem_copy( &sapApAppEvent.sapevt.sapStationMICFailureEvent.srcMacAddr,
                          pCsrRoamInfo->u.pMICFailureInfo->srcMacAddr,
                          sizeof(tSirMacAddr));
            vos_mem_copy( &sapApAppEvent.sapevt.sapStationMICFailureEvent.staMac,
                          pCsrRoamInfo->u.pMICFailureInfo->taMacAddr,
                          sizeof(tSirMacAddr));
            vos_mem_copy( &sapApAppEvent.sapevt.sapStationMICFailureEvent.dstMacAddr,
                          pCsrRoamInfo->u.pMICFailureInfo->dstMacAddr,
                          sizeof(tSirMacAddr));
            sapApAppEvent.sapevt.sapStationMICFailureEvent.multicast = pCsrRoamInfo->u.pMICFailureInfo->multicast;
            sapApAppEvent.sapevt.sapStationMICFailureEvent.IV1 = pCsrRoamInfo->u.pMICFailureInfo->IV1;
            sapApAppEvent.sapevt.sapStationMICFailureEvent.keyId = pCsrRoamInfo->u.pMICFailureInfo->keyId;
            vos_mem_copy( sapApAppEvent.sapevt.sapStationMICFailureEvent.TSC,
                          pCsrRoamInfo->u.pMICFailureInfo->TSC,
                          SIR_CIPHER_SEQ_CTR_SIZE);
            break;

        case eSAP_ASSOC_STA_CALLBACK_EVENT:
            VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, SAP event callback event = %s",
                       __func__, "eSAP_ASSOC_STA_CALLBACK_EVENT");
            break;

        case eSAP_WPS_PBC_PROBE_REQ_EVENT:
            sapApAppEvent.sapHddEventCode = eSAP_WPS_PBC_PROBE_REQ_EVENT;

            vos_mem_copy( &sapApAppEvent.sapevt.sapPBCProbeReqEvent.WPSPBCProbeReq,
                          pCsrRoamInfo->u.pWPSPBCProbeReq,
                          sizeof(tSirWPSPBCProbeReq));
            break;

       case eSAP_INDICATE_MGMT_FRAME:
            VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH,
                                 "In %s, SAP event callback event = %s",
                                __func__, "eSAP_INDICATE_MGMT_FRAME");
            sapApAppEvent.sapHddEventCode = eSAP_INDICATE_MGMT_FRAME;
            sapApAppEvent.sapevt.sapManagementFrameInfo.nFrameLength
                                           = pCsrRoamInfo->nFrameLength;
            sapApAppEvent.sapevt.sapManagementFrameInfo.pbFrames
                                           = pCsrRoamInfo->pbFrames;
            sapApAppEvent.sapevt.sapManagementFrameInfo.frameType
                                           = pCsrRoamInfo->frameType;
            sapApAppEvent.sapevt.sapManagementFrameInfo.rxChan
                                           = pCsrRoamInfo->rxChan;

            break;
       case eSAP_REMAIN_CHAN_READY:
            VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH,
                                 "In %s, SAP event callback event = %s",
                                __func__, "eSAP_REMAIN_CHAN_READY");
           sapApAppEvent.sapHddEventCode = eSAP_REMAIN_CHAN_READY;
            break;
       case eSAP_SEND_ACTION_CNF:
            VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH,
                                 "In %s, SAP event callback event = %s",
                                __func__, "eSAP_SEND_ACTION_CNF");
            sapApAppEvent.sapHddEventCode = eSAP_SEND_ACTION_CNF;
            sapApAppEvent.sapevt.sapActionCnf.actionSendSuccess = (eSapStatus)context;
            break;

       case eSAP_DISCONNECT_ALL_P2P_CLIENT:
            VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH,
                             "In %s, SAP event callback event = %s",
                            __func__, "eSAP_DISCONNECT_ALL_P2P_CLIENT");
            sapApAppEvent.sapHddEventCode = eSAP_DISCONNECT_ALL_P2P_CLIENT;
            sapApAppEvent.sapevt.sapActionCnf.actionSendSuccess = (eSapStatus)context;
            break;

       case eSAP_MAC_TRIG_STOP_BSS_EVENT :
            VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH,
                             "In %s, SAP event callback event = %s",
                            __func__, "eSAP_MAC_TRIG_STOP_BSS_EVENT");
            sapApAppEvent.sapHddEventCode = eSAP_MAC_TRIG_STOP_BSS_EVENT;
            sapApAppEvent.sapevt.sapActionCnf.actionSendSuccess = (eSapStatus)context;
            break;


        case eSAP_UNKNOWN_STA_JOIN:
            VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH,
                       "In %s, SAP event callback event = %s",
                       __func__, "eSAP_UNKNOWN_STA_JOIN");
            sapApAppEvent.sapHddEventCode = eSAP_UNKNOWN_STA_JOIN;
            vos_mem_copy((v_PVOID_t)sapApAppEvent.sapevt.sapUnknownSTAJoin.macaddr.bytes,
                         (v_PVOID_t)context, sizeof(v_MACADDR_t));
            break;

        case eSAP_MAX_ASSOC_EXCEEDED:
            VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH,
                    "In %s, SAP event callback event = %s",
                    __func__, "eSAP_MAX_ASSOC_EXCEEDED");
            sapApAppEvent.sapHddEventCode = eSAP_MAX_ASSOC_EXCEEDED;
            vos_mem_copy((v_PVOID_t)sapApAppEvent.sapevt.sapMaxAssocExceeded.macaddr.bytes,
                    (v_PVOID_t)pCsrRoamInfo->peerMac, sizeof(v_MACADDR_t));
            break;

        default:
            VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR, "In %s, SAP Unknown callback event = %d",
                       __func__,sapHddevent);
            break;
    }
    vosStatus = (*sapContext->pfnSapEventCallback)
                (
                 &sapApAppEvent,
                 sapContext->pUsrContext//                                       
                 );

    return vosStatus;

} /*                             */

/*                                                                          
                    

             
                                    

              
       

            

      
                                   
                                     
                                            

              
                                                                

                               

              
                                                                            */
VOS_STATUS
sapFsm
(
    ptSapContext sapContext,    /*                  */
    ptWLAN_SAPEvent sapEvent   /*                     */
)
{
   /*                                              
                                
     */
    eSapFsmStates_t stateVar = sapContext->sapsMachine; /*                                           */
    tCsrRoamInfo    *roamInfo = (tCsrRoamInfo *)(sapEvent->params);
    v_U32_t msg = sapEvent->event;  /*                                   */
    VOS_STATUS vosStatus = VOS_STATUS_E_FAILURE;

    switch (stateVar)
    {
        case eSAP_DISCONNECTED:
            if ((msg == eSAP_HDD_START_INFRA_BSS))
            {
                /*                                                                              */
                VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, new from state %s => %s",
                            __func__, "eSAP_DISCONNECTED", "eSAP_CH_SELECT");

                /*                                         */
                if (sapContext->isSapSessionOpen == eSAP_TRUE)
                {
                   VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_FATAL,
                        "%s:SME Session is already opened\n",__func__);
                   return VOS_STATUS_E_EXISTS;
                }

                sapContext->sessionId = 0xff;

                /*                     */
                sapContext->sapsMachine = eSAP_CH_SELECT;

                /*                         */
                vosStatus = sapGotoChannelSel(sapContext, sapEvent);

                /*                                                                              */
                VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, from state %s => %s",
                           __func__, "eSAP_DISCONNECTED", "eSAP_CH_SELECT");
            }
            else
            {
                 VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR, "In %s, in state %s, event msg %d",
                             __func__, "eSAP_DISCONNECTED", msg);
            }
            break;

        case eSAP_CH_SELECT:
            if (msg == eSAP_MAC_SCAN_COMPLETE)
            {
                 /*                                                                          */
                 VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, from state %s => %s",
                            __func__, "eSAP_CH_SELECT", "eSAP_STARTING");
                 //                                          
                 sapContext->sapsMachine = eSAP_STARTING;
                 //                    
                 sapContext->csrRoamProfile.ChannelInfo.numOfChannels = 1;
                 sapContext->csrRoamProfile.ChannelInfo.ChannelList = &sapContext->csrRoamProfile.operationChannel;
                 sapContext->csrRoamProfile.operationChannel = (tANI_U8)sapContext->channel;
                 vosStatus = sapGotoStarting( sapContext, sapEvent, eCSR_BSS_TYPE_INFRA_AP);
                 /*                                                                          */
                 VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, from state %s => %s",
                             __func__, "eSAP_CH_SELECT", "eSAP_STARTING");
            }
            else
            {
                VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR, "In %s, in state %s, invalid event msg %d",
                            __func__, "eSAP_CH_SELECT", msg);
            }
            break;

        case eSAP_STARTING:
            if (msg == eSAP_MAC_START_BSS_SUCCESS )
            {
                /*                                                                        */
                VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, from state channel = %d %s => %s",
                            __func__,sapContext->channel, "eSAP_STARTING", "eSAP_STARTED");

                 sapContext->sapsMachine = eSAP_STARTED;
                 /*                           */
                 vosStatus = sapSignalHDDevent( sapContext, roamInfo, eSAP_START_BSS_EVENT, (v_PVOID_t)eSAP_STATUS_SUCCESS);

                 /*                                                                        */
                 VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, from state %s => %s",
                            __func__, "eSAP_STARTING", "eSAP_STARTED");
             }
             else if (msg == eSAP_MAC_START_FAILS)
             {
                 /*                                                                 */
                 VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, from state %s => %s",
                            __func__, "eSAP_STARTING", "eSAP_DISCONNECTED");

                 /*                           */
                 vosStatus = sapSignalHDDevent( sapContext, NULL, eSAP_START_BSS_EVENT,(v_PVOID_t) eSAP_STATUS_FAILURE);
                 vosStatus =  sapGotoDisconnected(sapContext);

                 /*                       */
                 sapContext->sapsMachine = eSAP_DISCONNECTED;
             }
             else if (msg == eSAP_HDD_STOP_INFRA_BSS)
             {
                 /*                                                                            */
                 VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, from state %s => %s",
                             __func__, "eSAP_STARTING", "eSAP_DISCONNECTING");

                 /*                       */
                 sapContext->sapsMachine = eSAP_DISCONNECTED;
                 vosStatus = sapSignalHDDevent( sapContext, NULL, eSAP_START_BSS_EVENT, (v_PVOID_t)eSAP_STATUS_FAILURE);
                 vosStatus = sapGotoDisconnected(sapContext);
                 /*                      */

                 if (eSAP_TRUE == sapContext->isSapSessionOpen)
                 {
                    tHalHandle hHal = VOS_GET_HAL_CB(sapContext->pvosGCtx);
                    if (NULL == hHal)
                    {
                       VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                                  "In %s, NULL hHal in state %s, msg %d",
                                  __func__, "eSAP_STARTING", msg);
                    }
                    else if (eHAL_STATUS_SUCCESS ==
                         sme_CloseSession(hHal,
                                         sapContext->sessionId, NULL, NULL))
                     {
                         sapContext->isSapSessionOpen = eSAP_FALSE;
                     }
                 }
             }
             else
             {
                 VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                             "In %s, in state %s, invalid event msg %d",
                             __func__, "eSAP_STARTING", msg);
                 /*                          */
             }
             break;

        case eSAP_STARTED:
            if (msg == eSAP_HDD_STOP_INFRA_BSS)
            {
                /*                                                                             */
                VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, from state %s => %s",
                           __func__, "eSAP_STARTED", "eSAP_DISCONNECTING");
                sapContext->sapsMachine = eSAP_DISCONNECTING;
                vosStatus = sapGotoDisconnecting(sapContext);
            }
            else
            {
                VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR, "In %s, in state %s, invalid event msg %d",
                           __func__, "eSAP_STARTED", msg);
            }
            break;

        case eSAP_DISCONNECTING:
            if (msg == eSAP_MAC_READY_FOR_CONNECTIONS)
            {
                /*                                                                                  */
                VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, from state %s => %s",
                          __func__, "eSAP_DISCONNECTING", "eSAP_DISCONNECTED");

                sapContext->sapsMachine = eSAP_DISCONNECTED;
                /*                      */
                if (eSAP_TRUE == sapContext->isSapSessionOpen)
                {
                    tHalHandle hHal = VOS_GET_HAL_CB(sapContext->pvosGCtx);

                    if (NULL == hHal)
                    {
                        VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                                   "In %s, NULL hHal in state %s, msg %d",
                                   __func__, "eSAP_DISCONNECTING", msg);
                    }
                    else
                    {
                        sapContext->isSapSessionOpen = eSAP_FALSE;
                        if (!HAL_STATUS_SUCCESS(
                            sme_CloseSession(hHal,
                                     sapContext->sessionId,
                                     sapRoamSessionCloseCallback, sapContext)))
                        {
                            vosStatus = sapSignalHDDevent(sapContext, NULL,
                                              eSAP_STOP_BSS_EVENT,
                                              (v_PVOID_t) eSAP_STATUS_SUCCESS);
                        }
                    }
                }
            }
            else
            {
                VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                           "In %s, in state %s, invalid event msg %d",
                          __func__, "eSAP_DISCONNECTING", msg);
            }
            break;
      }
      return vosStatus;
}//       


eSapStatus
sapconvertToCsrProfile(tsap_Config_t *pconfig_params, eCsrRoamBssType bssType, tCsrRoamProfile *profile)
{
    //                                         
    profile->BSSType = eCSR_BSS_TYPE_INFRA_AP;
    profile->SSIDs.numOfSSIDs = 1;
    profile->csrPersona = pconfig_params->persona;

    vos_mem_zero(profile->SSIDs.SSIDList[0].SSID.ssId,
                 sizeof(profile->SSIDs.SSIDList[0].SSID.ssId));

    //                                          
    profile->SSIDs.SSIDList[0].ssidHidden =  pconfig_params->SSIDinfo.ssidHidden;

    profile->SSIDs.SSIDList[0].SSID.length = pconfig_params->SSIDinfo.ssid.length;
    vos_mem_copy(&profile->SSIDs.SSIDList[0].SSID.ssId, pconfig_params->SSIDinfo.ssid.ssId,
                  sizeof(pconfig_params->SSIDinfo.ssid.ssId));

    profile->negotiatedAuthType = eCSR_AUTH_TYPE_OPEN_SYSTEM;

    if (pconfig_params->authType == eSAP_OPEN_SYSTEM)
    {
        profile->negotiatedAuthType = eCSR_AUTH_TYPE_OPEN_SYSTEM;
    }
    else if (pconfig_params->authType == eSAP_SHARED_KEY)
    {
        profile->negotiatedAuthType = eCSR_AUTH_TYPE_SHARED_KEY;
    }
    else
    {
        profile->negotiatedAuthType = eCSR_AUTH_TYPE_AUTOSWITCH;
    }

    profile->AuthType.numEntries = 1;
    profile->AuthType.authType[0] = eCSR_AUTH_TYPE_OPEN_SYSTEM;

    //                              
    profile->EncryptionType.numEntries = 1;
    profile->EncryptionType.encryptionType[0] = pconfig_params->RSNEncryptType;

    profile->mcEncryptionType.numEntries = 1;
    profile->mcEncryptionType.encryptionType[0] = pconfig_params->mcRSNEncryptType;

    if (pconfig_params->privacy & eSAP_SHARED_KEY)
    {
        profile->AuthType.authType[0] = eCSR_AUTH_TYPE_SHARED_KEY;
    }

    profile->privacy = pconfig_params->privacy;
    profile->fwdWPSPBCProbeReq = pconfig_params->fwdWPSPBCProbeReq;

    if (pconfig_params->authType == eSAP_SHARED_KEY)
    {
        profile->csr80211AuthType = eSIR_SHARED_KEY;
    }
    else if (pconfig_params->authType == eSAP_OPEN_SYSTEM)
    {
        profile->csr80211AuthType = eSIR_OPEN_SYSTEM;
    }
    else
    {
        profile->csr80211AuthType = eSIR_AUTO_SWITCH;
    }

    //                                     
    profile->pWPAReqIE = NULL;
    profile->nWPAReqIELength = 0;

    //                  
    profile->pRSNReqIE = NULL;
    profile->nRSNReqIELength = pconfig_params->RSNWPAReqIELength;
    if (pconfig_params->RSNWPAReqIELength)
    {
        profile->pRSNReqIE = vos_mem_malloc(pconfig_params->RSNWPAReqIELength);
        if( NULL == profile->pRSNReqIE )
        {
           VOS_TRACE(VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR, " %s Fail to alloc memory", __func__);
           return eSAP_STATUS_FAILURE;
        }
        vos_mem_copy(profile->pRSNReqIE, pconfig_params->pRSNWPAReqIE, pconfig_params->RSNWPAReqIELength);
        profile->nRSNReqIELength = pconfig_params->RSNWPAReqIELength;
    }

    //                 
    profile->CBMode = eCSR_CB_OFF;

    //                                  
    //                                                                 
    profile->phyMode = pconfig_params->SapHw_mode; /*                    */

    //                        
    profile->beaconInterval = (tANI_U16)pconfig_params->beacon_int;

    //                
    profile->dtimPeriod = pconfig_params->dtim_period;

    //                    
    profile->ApUapsdEnable = pconfig_params->UapsdEnable;

    //                            
    profile->protEnabled       = pconfig_params->protEnabled;
    profile->obssProtEnabled   = pconfig_params->obssProtEnabled;
    profile->cfg_protection    = pconfig_params->ht_capab;

    //            
    if (pconfig_params->countryCode[0])
        vos_mem_copy(profile->countryCode, pconfig_params->countryCode, WNI_CFG_COUNTRY_CODE_LEN);
    profile->ieee80211d = pconfig_params->ieee80211d;
    //               
    profile->wps_state = pconfig_params->wps_state;

    return eSAP_STATUS_SUCCESS; /*           */
}

/* 
                                             
                           
 */
eCsrPhyMode sapConvertSapPhyModeToCsrPhyMode( eSapPhyMode sapPhyMode )
{
    switch (sapPhyMode)
    {
      case (eSAP_DOT11_MODE_abg):
         return eCSR_DOT11_MODE_abg;
      case (eSAP_DOT11_MODE_11b):
         return eCSR_DOT11_MODE_11b;
      case (eSAP_DOT11_MODE_11g):
         return eCSR_DOT11_MODE_11g;
      case (eSAP_DOT11_MODE_11n):
         return eCSR_DOT11_MODE_11n;
      case (eSAP_DOT11_MODE_11g_ONLY):
         return eCSR_DOT11_MODE_11g_ONLY;
      case (eSAP_DOT11_MODE_11n_ONLY):
         return eCSR_DOT11_MODE_11n_ONLY;
      case (eSAP_DOT11_MODE_11b_ONLY):
         return eCSR_DOT11_MODE_11b_ONLY;
#ifdef WLAN_FEATURE_11AC
      case (eSAP_DOT11_MODE_11ac_ONLY):
         return eCSR_DOT11_MODE_11ac_ONLY;
      case (eSAP_DOT11_MODE_11ac):
         return eCSR_DOT11_MODE_11ac;
#endif
      default:
         return eCSR_DOT11_MODE_AUTO;
    }
}

void sapFreeRoamProfile(tCsrRoamProfile *profile)
{
   if(profile->pRSNReqIE)
   {
      vos_mem_free(profile->pRSNReqIE);
      profile->pRSNReqIE = NULL;
   }
}


void
sapSortMacList(v_MACADDR_t *macList, v_U8_t size)
{
    v_U8_t outer, inner;
    v_MACADDR_t temp;
    v_SINT_t nRes = -1;

    for(outer = 0; outer < size; outer++)
    {
        for(inner = 0; inner < size - 1; inner++)
        {
            nRes = vos_mem_compare2((macList + inner)->bytes, (macList + inner + 1)->bytes, sizeof(v_MACADDR_t));
            if (nRes > 0)
            {
                vos_mem_copy(temp.bytes, (macList + inner + 1)->bytes, sizeof(v_MACADDR_t));
                vos_mem_copy((macList + inner + 1)->bytes, (macList + inner)->bytes, sizeof(v_MACADDR_t));
                vos_mem_copy((macList + inner)->bytes, temp.bytes, sizeof(v_MACADDR_t));
             }
        }
    }
}

eSapBool
sapSearchMacList(v_MACADDR_t *macList, v_U8_t num_mac, v_U8_t *peerMac, v_U8_t *index)
{
    v_SINT_t nRes = -1;
    v_S7_t nStart = 0, nEnd, nMiddle;
    nEnd = num_mac - 1;

    while (nStart <= nEnd)
    {
        nMiddle = (nStart + nEnd) / 2;
        nRes = vos_mem_compare2(&macList[nMiddle], peerMac, sizeof(v_MACADDR_t));

        if (0 == nRes)
        {
            VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH,
                    "search SUCC");
            //                                                       
            //                                          
            if (index != NULL)
            {
                *index = (v_U8_t) nMiddle;
                VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH,
                        "index %d", *index);
            }
            return eSAP_TRUE;
        }
        if (nRes < 0)
            nStart = nMiddle + 1;
        else
            nEnd = nMiddle - 1;
    }

    VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH,
            "search not succ");
    return eSAP_FALSE;
}

void
sapAddMacToACL(v_MACADDR_t *macList, v_U8_t *size, v_U8_t *peerMac)
{
    v_SINT_t nRes = -1;
    int i;
    VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH,"add acl entered");
    for (i=((*size)-1); i>=0; i--)
    {
        nRes = vos_mem_compare2(&macList[i], peerMac, sizeof(v_MACADDR_t));
        if (nRes > 0)
        {
            /*                                                                                
                                          */
            vos_mem_copy((macList+i+1)->bytes,(macList+i)->bytes, sizeof(v_MACADDR_t));
        }
        else
        {
            break;
        }
    }
    //                                                                                 
    vos_mem_copy((macList+i+1)->bytes, peerMac, sizeof(v_MACADDR_t));
    //                        
    (*size)++;
}

void
sapRemoveMacFromACL(v_MACADDR_t *macList, v_U8_t *size, v_U8_t index)
{
    int i;
    VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH,"remove acl entered");
    /*                                                                                                
                                                                                                 
                                                                                      */
    if (macList==NULL) return;
    for (i=index; i<((*size)-1); i++)
    {
        /*                                                                                
                                                       */
        vos_mem_copy((macList+i)->bytes,(macList+i+1)->bytes, sizeof(v_MACADDR_t));
    }
    //                                                                             
    vos_mem_zero((macList+(*size)-1)->bytes, sizeof(v_MACADDR_t));
    //                         
    (*size)--;
}

void sapPrintACL(v_MACADDR_t *macList, v_U8_t size)
{
    int i;
    v_BYTE_t *macArray;
    VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH,"print acl entered");
    if (size==0) return;
    for (i=0; i<size; i++)
    {
        macArray = (macList+i)->bytes;
        VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH,
                "** ACL entry %i - "MAC_ADDRESS_STR, i,
                MAC_ADDR_ARRAY(macArray));
    }
    return;
}

VOS_STATUS
sapIsPeerMacAllowed(ptSapContext sapContext, v_U8_t *peerMac)
{
    if (eSAP_ALLOW_ALL == sapContext->eSapMacAddrAclMode)
              return VOS_STATUS_SUCCESS;

    if (sapSearchMacList(sapContext->acceptMacList, sapContext->nAcceptMac, peerMac, NULL))
        return VOS_STATUS_SUCCESS;

    if (sapSearchMacList(sapContext->denyMacList, sapContext->nDenyMac, peerMac, NULL))
    {
        VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH,
                  "In %s, Peer "MAC_ADDRESS_STR" in deny list",
                  __func__, MAC_ADDR_ARRAY(peerMac));
        return VOS_STATUS_E_FAILURE;
    }

    //                                                                      
    if (eSAP_ACCEPT_UNLESS_DENIED == sapContext->eSapMacAddrAclMode)
        return VOS_STATUS_SUCCESS;

    //                                                                           
    if (eSAP_DENY_UNLESS_ACCEPTED == sapContext->eSapMacAddrAclMode)
    {
        VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH,
                  "In %s, Peer "MAC_ADDRESS_STR" denied, Mac filter mode is eSAP_DENY_UNLESS_ACCEPTED",
                  __func__,  MAC_ADDR_ARRAY(peerMac));
        return VOS_STATUS_E_FAILURE;
    }

    /*                                                                                           
                                                                                 
     */
    if (eSAP_SUPPORT_ACCEPT_AND_DENY == sapContext->eSapMacAddrAclMode)
    {
        sapSignalHDDevent(sapContext, NULL, eSAP_UNKNOWN_STA_JOIN, (v_PVOID_t)peerMac);
        VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH,
                  "In %s, Peer "MAC_ADDRESS_STR" denied, Mac filter mode is eSAP_SUPPORT_ACCEPT_AND_DENY",
                  __func__, MAC_ADDR_ARRAY(peerMac));
        return VOS_STATUS_E_FAILURE;
    }
    return VOS_STATUS_SUCCESS;
}

#ifdef SOFTAP_CHANNEL_RANGE
static VOS_STATUS sapGetChannelList(ptSapContext sapContext,
                                 v_U8_t **channelList, v_U8_t *numberOfChannels)
{
    v_U32_t startChannelNum;
    v_U32_t endChannelNum;
    v_U32_t operatingBand;
    v_U8_t  loopCount;
    v_U8_t *list;
    v_U8_t channelCount;
    v_U8_t bandStartChannel;
    v_U8_t bandEndChannel ;
    v_U32_t enableLTECoex;
    tHalHandle hHal = VOS_GET_HAL_CB(sapContext->pvosGCtx);
#ifdef FEATURE_WLAN_CH_AVOID
    v_U8_t i;
#endif


    if (NULL == hHal)
    {
        VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
             "Invalid HAL pointer from pvosGCtx on sapGetChannelList");
        *numberOfChannels = 0;
        *channelList = NULL;
        return VOS_STATUS_E_FAULT;
    }

    ccmCfgGetInt(hHal, WNI_CFG_SAP_CHANNEL_SELECT_START_CHANNEL, &startChannelNum);
    ccmCfgGetInt(hHal, WNI_CFG_SAP_CHANNEL_SELECT_END_CHANNEL, &endChannelNum);
    ccmCfgGetInt(hHal, WNI_CFG_SAP_CHANNEL_SELECT_OPERATING_BAND, &operatingBand);
    ccmCfgGetInt(hHal, WNI_CFG_ENABLE_LTE_COEX, &enableLTECoex);

    VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO,
             "%s:sapGetChannelList: startChannel %d,EndChannel %d,Operatingband:%d",
             __func__,startChannelNum,endChannelNum,operatingBand);

    switch(operatingBand)
    {
        case RF_SUBBAND_2_4_GHZ:
           bandStartChannel = RF_CHAN_1;
           bandEndChannel = RF_CHAN_14;
           break;

        case RF_SUBBAND_5_LOW_GHZ:
           bandStartChannel = RF_CHAN_36;
           bandEndChannel = RF_CHAN_64;
           break;

        case RF_SUBBAND_5_MID_GHZ:
           bandStartChannel = RF_CHAN_100;
#ifndef FEATURE_WLAN_CH144
           bandEndChannel = RF_CHAN_140;
#else
           bandEndChannel = RF_CHAN_144;
#endif /*                    */
           break;

        case RF_SUBBAND_5_HIGH_GHZ:
           bandStartChannel = RF_CHAN_149;
           bandEndChannel = RF_CHAN_165;
           break;

        default:
           VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
             "sapGetChannelList:OperatingBand not valid ");
           /*                */
           bandStartChannel = RF_CHAN_1;
           bandEndChannel = RF_CHAN_14;
           break;
    }
    /*                                                   */
    if (enableLTECoex && (bandStartChannel == RF_CHAN_1)
       && (bandEndChannel == RF_CHAN_14))
    {
        /*                                                */
        bandEndChannel = RF_CHAN_9;
    }
    /*                                              */
    list = (v_U8_t *)vos_mem_malloc(NUM_5GHZ_CHANNELS);
    if (NULL == list)
    {
        VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                   "%s: Unable to allocate channel list", __func__);
        *numberOfChannels = 0;
        *channelList = NULL;
        return VOS_STATUS_E_RESOURCES;
    }

    /*                                                  */
    channelCount = 0;
    for( loopCount = bandStartChannel; loopCount <= bandEndChannel; loopCount++ )
    {
        if((startChannelNum <= rfChannels[loopCount].channelNum)&&
            (endChannelNum >= rfChannels[loopCount].channelNum ))
        {
            if( regChannels[loopCount].enabled )
            {
#ifdef FEATURE_WLAN_CH_AVOID
                for( i = 0; i < NUM_20MHZ_RF_CHANNELS; i++ )
                {
                    if( (safeChannels[i].channelNumber ==
                                rfChannels[loopCount].channelNum) )
                    {
                        /*                          */
                        if(VOS_TRUE == safeChannels[i].isSafe)
                        {
#endif
                            list[channelCount] =
                                     rfChannels[loopCount].channelNum;
                            channelCount++;
#ifdef FEATURE_WLAN_CH_AVOID
                        }
                        break;
                    }
                }
#endif
            }
        }
    }
    if (0 == channelCount)
    {
        VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
        "sapGetChannelList:No active channels present in the given range for the current region");
        /*                                                                 */
        if (enableLTECoex && (startChannelNum > bandEndChannel))
        {
            VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_FATAL,
            "sapGetChannelList:SAP cannot be started as LTE COEX restricted 2.4GHz limits");
        }
    }

    /*                                                       */
    *numberOfChannels = channelCount;
    if(channelCount != 0)
    {
       *channelList = list;
    }
    else
    {
       *channelList = NULL;
        vos_mem_free(list);
    }
    return VOS_STATUS_SUCCESS;
}
#endif

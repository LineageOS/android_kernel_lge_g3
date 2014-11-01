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

#ifdef FEATURE_OEM_DATA_SUPPORT
/*                                                                             
                                                                               


                      

                                                       
                                                                              */
#include "aniGlobal.h"
#include "oemDataApi.h"
#include "palApi.h"
#include "smeInside.h"
#include "smsDebug.h"

#include "csrSupport.h"
#include "wlan_qct_tl.h"

#include "vos_diag_core_log.h"
#include "vos_diag_core_event.h"

/*                                                                            
                              
                                                                                        
                      
                                                                                 */

eHalStatus oemData_OemDataReqOpen(tHalHandle hHal)
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tpAniSirGlobal pMac = PMAC_STRUCT(hHal);

    do
    {
        //                                    
        vos_mem_set(&(pMac->oemData), sizeof(tOemDataStruct), 0);
        if(!HAL_STATUS_SUCCESS(status))
        {
            smsLog(pMac, LOGE, "oemData_OemDataReqOpen: Cannot allocate memory for the timer function");
            break;
        }
    } while(0);

    return status;
}

/*                                                                            
                               
                                                                     
                      
                                                                                 */

eHalStatus oemData_OemDataReqClose(tHalHandle hHal)
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tpAniSirGlobal pMac = PMAC_STRUCT(hHal);

    do
    {
        if(!HAL_STATUS_SUCCESS(status))
        {
            smsLog(pMac, LOGE, "oemData_OemDataReqClose: Failed in oemData_OemDataReqClose at StopTimers");
            break;
        }

        if(pMac->oemData.pOemDataRsp != NULL)
        {
            vos_mem_free(pMac->oemData.pOemDataRsp);
        }

        //                                    
        vos_mem_set(&(pMac->oemData), sizeof(tOemDataStruct), 0);
    } while(0);

    return eHAL_STATUS_SUCCESS;
}

/*                                                                            
                                        
                                                                            
                                                   
                      
                                                                                 */
void oemData_ReleaseOemDataReqCommand(tpAniSirGlobal pMac, tSmeCmd *pOemDataCmd, eOemDataReqStatus oemDataReqStatus)
{
    //               
    pOemDataCmd->u.oemDataCmd.callback(pMac, pOemDataCmd->u.oemDataCmd.pContext, pOemDataCmd->u.oemDataCmd.oemDataReqID, oemDataReqStatus);

    //                                              
    if(csrLLRemoveEntry(&pMac->sme.smeCmdActiveList, &pOemDataCmd->Link, LL_ACCESS_LOCK))
    {
        vos_mem_set(&(pOemDataCmd->u.oemDataCmd), sizeof(tOemDataCmd), 0);

        //                                                      
        smeReleaseCommand(pMac, pOemDataCmd);
    }
    else
    {
        smsLog(pMac, LOGE, "OEM_DATA: **************** oemData_ReleaseOemDataReqCommand cannot release the command");
    }
}

/*                                                                            
                          
                                  
                                               
                                                                          
                                                                    
                                                          
                      
                                                                                 */
eHalStatus oemData_OemDataReq(tHalHandle hHal,
                                tANI_U8 sessionId,
                                tOemDataReqConfig *oemDataReqConfig,
                                tANI_U32 *pOemDataReqID,
                                oemData_OemDataReqCompleteCallback callback,
                                void *pContext)
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tpAniSirGlobal pMac = PMAC_STRUCT( hHal );
    tSmeCmd *pOemDataCmd = NULL;

    do
    {
        if( !CSR_IS_SESSION_VALID( pMac, sessionId ) )
        {
           status = eHAL_STATUS_FAILURE;
           break;
        }

        pMac->oemData.oemDataReqConfig.sessionId = sessionId;
        pMac->oemData.callback = callback;
        pMac->oemData.pContext = pContext;
        pMac->oemData.oemDataReqID = *(pOemDataReqID);

        vos_mem_copy((v_VOID_t*)(pMac->oemData.oemDataReqConfig.oemDataReq), (v_VOID_t*)(oemDataReqConfig->oemDataReq), OEM_DATA_REQ_SIZE);

        pMac->oemData.oemDataReqActive = eANI_BOOLEAN_FALSE;

        pOemDataCmd = smeGetCommandBuffer(pMac);

        //                                      
        if(pOemDataCmd)
        {
            pOemDataCmd->command = eSmeCommandOemDataReq;
            pOemDataCmd->u.oemDataCmd.callback = callback;
            pOemDataCmd->u.oemDataCmd.pContext = pContext;
            pOemDataCmd->u.oemDataCmd.oemDataReqID = pMac->oemData.oemDataReqID;

            //                        
            pOemDataCmd->u.oemDataCmd.oemDataReq.sessionId = pMac->oemData.oemDataReqConfig.sessionId;
            vos_mem_copy((v_VOID_t*)(pOemDataCmd->u.oemDataCmd.oemDataReq.oemDataReq),
                                    (v_VOID_t*)(pMac->oemData.oemDataReqConfig.oemDataReq), OEM_DATA_REQ_SIZE);
        }
        else
        {
            status = eHAL_STATUS_FAILURE;
            break;
        }

        //                                               
        //                                                                     
        //                                                                   
        smePushCommand(pMac, pOemDataCmd, eANI_BOOLEAN_FALSE);

    } while(0);

    if(!HAL_STATUS_SUCCESS(status) && pOemDataCmd)
    {
        oemData_ReleaseOemDataReqCommand(pMac, pOemDataCmd, eOEM_DATA_REQ_FAILURE);
        pMac->oemData.oemDataReqActive = eANI_BOOLEAN_FALSE;
    }

    return status;
}

/*                                                                            
                                
                                                          
                
                                                       
                      
                                                                                 */
eHalStatus oemData_SendMBOemDataReq(tpAniSirGlobal pMac, tOemDataReq *pOemDataReq)
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tSirOemDataReq* pMsg;
    tANI_U16 msgLen;
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, pOemDataReq->sessionId );

    smsLog(pMac, LOGW, "OEM_DATA: entering Function %s", __func__);

    msgLen = (tANI_U16)(sizeof(tSirOemDataReq));

    status = palAllocateMemory(pMac->hHdd, (void**)&pMsg, msgLen);
    if(HAL_STATUS_SUCCESS(status))
    {
        palZeroMemory(pMac->hHdd, pMsg, msgLen);
        pMsg->messageType = pal_cpu_to_be16((tANI_U16)eWNI_SME_OEM_DATA_REQ);
        palCopyMemory(pMac->hHdd, pMsg->selfMacAddr, pSession->selfMacAddr, sizeof(tSirMacAddr) );
        status = palCopyMemory(pMac->hHdd, pMsg->oemDataReq, pOemDataReq->oemDataReq, OEM_DATA_REQ_SIZE);
        if(HAL_STATUS_SUCCESS(status))
        {
            smsLog(pMac, LOGW, "OEM_DATA: sending message to pe%s", __func__);
            status = palSendMBMessage(pMac->hHdd, pMsg);
        }
        else
        {
            palFreeMemory(pMac->hHdd, pMsg);
        }
    }

    smsLog(pMac, LOGW, "OEM_DATA: exiting Function %s", __func__);

    return status;
}

/*                                                                            
                                        
                                                                              
                                
                      
                                                                                 */
eHalStatus oemData_ProcessOemDataReqCommand(tpAniSirGlobal pMac, tSmeCmd *pOemDataReqCmd)
{
    eHalStatus status = eHAL_STATUS_SUCCESS;

    //                                                      
    //                                                                
    //                                                    
    //                                                               
    //                                                          
    //                             
    //          
    //            
    //                 

    if(eHAL_STATUS_SUCCESS == oemData_IsOemDataReqAllowed(pMac))
    {
        smsLog(pMac, LOG1, "%s: OEM_DATA REQ allowed in the current mode", __func__);
        pMac->oemData.oemDataReqActive = eANI_BOOLEAN_TRUE;
        status = oemData_SendMBOemDataReq(pMac, &(pOemDataReqCmd->u.oemDataCmd.oemDataReq));
    }
    else
    {
        smsLog(pMac, LOG1, "%s: OEM_DATA REQ not allowed in the current mode", __func__);
        oemData_ReleaseOemDataReqCommand(pMac, pOemDataReqCmd, eOEM_DATA_REQ_INVALID_MODE);
        pMac->oemData.oemDataReqActive = eANI_BOOLEAN_FALSE;
    }

    return status;
}

/*                                                                            
                            
                                                                             
                                               
                      
                                                                                 */
eHalStatus sme_HandleOemDataRsp(tHalHandle hHal, tANI_U8* pMsg)
{
    eHalStatus                         status = eHAL_STATUS_SUCCESS;
    tpAniSirGlobal                     pMac;
    tListElem                          *pEntry = NULL;
    tSmeCmd                            *pCommand = NULL;
    tSirOemDataRsp*                    pOemDataRsp = NULL;
    pMac = PMAC_STRUCT(hHal);

    smsLog(pMac, LOG1, "%s: OEM_DATA Entering", __func__);

    do
    {
        if(pMsg == NULL)
        {
            smsLog(pMac, LOGE, "in %s msg ptr is NULL", __func__);
            status = eHAL_STATUS_FAILURE;
            break;
        }

        pEntry = csrLLPeekHead( &pMac->sme.smeCmdActiveList, LL_ACCESS_LOCK );
        if(pEntry)
        {
            pCommand = GET_BASE_ADDR( pEntry, tSmeCmd, Link );
            if(eSmeCommandOemDataReq == pCommand->command)
            {
                pOemDataRsp = (tSirOemDataRsp*)pMsg;

                //                                                       
                status = sme_AcquireGlobalLock(&pMac->sme);
                if(!HAL_STATUS_SUCCESS(status))
                {
                    break;
                }

                if(pMac->oemData.pOemDataRsp != NULL)
                {
                    vos_mem_free(pMac->oemData.pOemDataRsp);
                }
                pMac->oemData.pOemDataRsp = (tOemDataRsp*)vos_mem_malloc(sizeof(tOemDataRsp));

                if(pMac->oemData.pOemDataRsp == NULL)
                {
                    sme_ReleaseGlobalLock(&pMac->sme);
                    smsLog(pMac, LOGE, "in %s vos_mem_malloc failed for pMac->oemData.pOemDataRsp", __func__);
                    status = eHAL_STATUS_FAILURE;
                    break;
                }

                smsLog(pMac, LOGE, "Before memory copy");
                vos_mem_copy((v_VOID_t*)(pMac->oemData.pOemDataRsp), (v_VOID_t*)(&pOemDataRsp->oemDataRsp), sizeof(tOemDataRsp));
                smsLog(pMac, LOGE, "after memory copy");
                sme_ReleaseGlobalLock(&pMac->sme);
            }
            else
            {
                smsLog(pMac, LOGE, "in %s eWNI_SME_OEM_DATA_RSP Received but NO REQs are ACTIVE ...",
                    __func__);
                status = eHAL_STATUS_FAILURE;
                break;
            }
        }
        else
        {
            smsLog(pMac, LOGE, "in %s eWNI_SME_OEM_DATA_RSP Received but NO commands are ACTIVE ...", __func__);
            status = eHAL_STATUS_FAILURE;
            break;
        }

        oemData_ReleaseOemDataReqCommand(pMac, pCommand, eHAL_STATUS_SUCCESS);
        pMac->oemData.oemDataReqActive = eANI_BOOLEAN_FALSE;

    } while(0);

    return status;
}

/*                                                                            
                                   
                                                                        
                               
                      
                                                                                 */
eHalStatus oemData_IsOemDataReqAllowed(tHalHandle hHal)
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tANI_U32 sessionId;

    tpAniSirGlobal pMac = PMAC_STRUCT(hHal);

    for(sessionId = 0; sessionId < CSR_ROAM_SESSION_MAX; sessionId++)
    {
        if(CSR_IS_SESSION_VALID(pMac, sessionId))
        {
            //                                                  
            if(csrIsConnStateIbss(pMac, sessionId) || csrIsBTAMP(pMac, sessionId) )
            {
                //                                                  
                smsLog(pMac, LOGW, "OEM DATA REQ is not allowed due to IBSS|BTAMP exist in session %d", sessionId);
                status = eHAL_STATUS_CSR_WRONG_STATE;
                break;
            }
        }
    }

    smsLog(pMac, LOG1, "Exiting oemData_IsOemDataReqAllowed with status %d", status);

    return (status);
}

#endif /*                        */

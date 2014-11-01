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

#include "schApi.h"
#include "limUtils.h"
#include "limTypes.h"
#include "limSecurityUtils.h"
#include "limPropExtsUtils.h"
#include "limSendMessages.h"
#include "limSerDesUtils.h"
#include "limAdmitControl.h"
#include "limStaHashApi.h"
#include "dot11f.h"
#include "wmmApsd.h"
#include "limTrace.h"
#ifdef FEATURE_WLAN_DIAG_SUPPORT
#include "vos_diag_core_event.h"
#endif //                         
#include "limIbssPeerMgmt.h"
#include "limSessionUtils.h"
#include "limSession.h"
#include "vos_nvitem.h"

/*                                                                                                     
                                                                                                  
                                                                                                      
                     */
static tAniBool glimTriggerBackgroundScanDuringQuietBss_Status = eSIR_TRUE;

/*                                                      */
static const tANI_U8 abChannel[]= {36,40,44,48,52,56,60,64,100,104,108,112,116,
            120,124,128,132,136,140,149,153,157,161,165};

#ifdef WLAN_FEATURE_ROAM_SCAN_OFFLOAD
static const tANI_U8 aUnsortedChannelList[]= {52,56,60,64,100,104,108,112,116,
            120,124,128,132,136,140,36,40,44,48,149,153,157,161,165};
#endif

//                                                                            
#define SUCCESS 1                   //                              

#define MAX_BA_WINDOW_SIZE_FOR_CISCO 25
/*                                                               
                          
                              
                                 
                                                       
                                                               */

tpDialogueToken
limAssignDialogueToken(tpAniSirGlobal pMac)
{
    static tANI_U8 token;
    tpDialogueToken pCurrNode;
    pCurrNode = vos_mem_malloc(sizeof(tDialogueToken));
    if ( NULL == pCurrNode )
    {
        PELOGE(limLog(pMac, LOGE, FL("AllocateMemory failed"));)
        return NULL;
    }

    vos_mem_set((void *) pCurrNode, sizeof(tDialogueToken), 0);
    //                                      
    if(NULL == pMac->lim.pDialogueTokenHead)
    {
        pMac->lim.pDialogueTokenHead = pMac->lim.pDialogueTokenTail = pCurrNode;
    }
    else
    {
        pMac->lim.pDialogueTokenTail->next = pCurrNode;
        pMac->lim.pDialogueTokenTail = pCurrNode;
    }
    //                                                        
    pCurrNode->next = NULL;
    pCurrNode->token = token++;

    /*                                         */
    if (0 == pCurrNode->token)
       pCurrNode->token = token;

    PELOG4(limLog(pMac, LOG4, FL("token assigned = %d"), pCurrNode->token);)
    return pCurrNode;
}

/*                                                               
                                   
                                                                                               
                                 
                        
                           
                       
                                            
                                                               */


tSirRetStatus
limSearchAndDeleteDialogueToken(tpAniSirGlobal pMac, tANI_U8 token, tANI_U16 assocId, tANI_U16 tid)
{
    tpDialogueToken pCurrNode = pMac->lim.pDialogueTokenHead;
    tpDialogueToken pPrevNode = pMac->lim.pDialogueTokenHead;

    //                    
    if(NULL == pCurrNode)
      return eSIR_FAILURE;

    //                                        
    if(pCurrNode &&
        (assocId == pCurrNode->assocId) &&
        (tid == pCurrNode->tid))
    {
        pMac->lim.pDialogueTokenHead = pCurrNode->next;        
        //                                                                               
        if(NULL == pMac->lim.pDialogueTokenHead)
            pMac->lim.pDialogueTokenTail = NULL;
        vos_mem_free(pCurrNode);
        pMac->lim.pDialogueTokenHead = NULL;
        return eSIR_SUCCESS;
    }

    //                                                  
    pCurrNode = pCurrNode->next;
    while(NULL != pCurrNode )
    {
        if(token == pCurrNode->token)
        {
            break;
        }

        pPrevNode = pCurrNode;
        pCurrNode = pCurrNode->next;
    }

    if(pCurrNode &&
        (assocId == pCurrNode->assocId) &&
        (tid == pCurrNode->tid))
    {
        pPrevNode->next = pCurrNode->next;
        //                                                                                                     
        if(NULL == pCurrNode->next)
              pMac->lim.pDialogueTokenTail = pPrevNode;
        vos_mem_free(pCurrNode);
        pMac->lim.pDialogueTokenHead = NULL;
        return eSIR_SUCCESS;
    }

    PELOGW(limLog(pMac, LOGW, FL("LIM does not have matching dialogue token node"));)
    return eSIR_FAILURE;

}


/*                                                               
                              
                                                           
                                 
                
                                                               */
void
limDeleteDialogueTokenList(tpAniSirGlobal pMac)
{
    tpDialogueToken pCurrNode = pMac->lim.pDialogueTokenHead;

    while(NULL != pMac->lim.pDialogueTokenHead)
    {
        pCurrNode = pMac->lim.pDialogueTokenHead;    
        pMac->lim.pDialogueTokenHead = pMac->lim.pDialogueTokenHead->next;
        vos_mem_free(pCurrNode);
        pCurrNode = NULL;
    }
    pMac->lim.pDialogueTokenTail = NULL;
}

void
limGetBssidFromBD(tpAniSirGlobal pMac, tANI_U8 * pRxPacketInfo, tANI_U8 *bssId, tANI_U32 *pIgnore)
{
    tpSirMacDataHdr3a pMh = WDA_GET_RX_MPDUHEADER3A(pRxPacketInfo);
    *pIgnore = 0;

    if (pMh->fc.toDS == 1 && pMh->fc.fromDS == 0)
    {
        vos_mem_copy( bssId, pMh->addr1, 6);
        *pIgnore = 1;
    }
    else if (pMh->fc.toDS == 0 && pMh->fc.fromDS == 1)
    {
        vos_mem_copy ( bssId, pMh->addr2, 6);
        *pIgnore = 1;
    }
    else if (pMh->fc.toDS == 0 && pMh->fc.fromDS == 0)
    {
        vos_mem_copy( bssId, pMh->addr3, 6);
        *pIgnore = 0;
    }
    else
    {
        vos_mem_copy( bssId, pMh->addr1, 6);
        *pIgnore = 1;
    }
}

char *
limMlmStateStr(tLimMlmStates state)
{
    switch (state)
    {
        case eLIM_MLM_OFFLINE_STATE:
            return "eLIM_MLM_OFFLINE_STATE";
        case eLIM_MLM_IDLE_STATE:
            return "eLIM_MLM_IDLE_STATE";
        case eLIM_MLM_WT_PROBE_RESP_STATE:
            return "eLIM_MLM_WT_PROBE_RESP_STATE";
        case eLIM_MLM_PASSIVE_SCAN_STATE:
            return "eLIM_MLM_PASSIVE_SCAN_STATE";
        case eLIM_MLM_WT_JOIN_BEACON_STATE:
            return "eLIM_MLM_WT_JOIN_BEACON_STATE";
        case eLIM_MLM_JOINED_STATE:
            return "eLIM_MLM_JOINED_STATE";
        case eLIM_MLM_BSS_STARTED_STATE:
            return "eLIM_MLM_BSS_STARTED_STATE";
        case eLIM_MLM_WT_AUTH_FRAME2_STATE:
            return "eLIM_MLM_WT_AUTH_FRAME2_STATE";
        case eLIM_MLM_WT_AUTH_FRAME3_STATE:
            return "eLIM_MLM_WT_AUTH_FRAME3_STATE";
        case eLIM_MLM_WT_AUTH_FRAME4_STATE:
            return "eLIM_MLM_WT_AUTH_FRAME4_STATE";
        case eLIM_MLM_AUTH_RSP_TIMEOUT_STATE:
            return "eLIM_MLM_AUTH_RSP_TIMEOUT_STATE";
        case eLIM_MLM_AUTHENTICATED_STATE:
            return "eLIM_MLM_AUTHENTICATED_STATE";
        case eLIM_MLM_WT_ASSOC_RSP_STATE:
            return "eLIM_MLM_WT_ASSOC_RSP_STATE";
        case eLIM_MLM_WT_REASSOC_RSP_STATE:
            return "eLIM_MLM_WT_REASSOC_RSP_STATE";
        case eLIM_MLM_WT_FT_REASSOC_RSP_STATE:
            return "eLIM_MLM_WT_FT_REASSOC_RSP_STATE";
        case eLIM_MLM_WT_DEL_STA_RSP_STATE:
            return "eLIM_MLM_WT_DEL_STA_RSP_STATE";
        case eLIM_MLM_WT_DEL_BSS_RSP_STATE:
            return "eLIM_MLM_WT_DEL_BSS_RSP_STATE";
        case eLIM_MLM_WT_ADD_STA_RSP_STATE:
            return "eLIM_MLM_WT_ADD_STA_RSP_STATE";
        case eLIM_MLM_WT_ADD_BSS_RSP_STATE:
            return "eLIM_MLM_WT_ADD_BSS_RSP_STATE";
        case eLIM_MLM_REASSOCIATED_STATE:
            return "eLIM_MLM_REASSOCIATED_STATE";
        case eLIM_MLM_LINK_ESTABLISHED_STATE:
            return "eLIM_MLM_LINK_ESTABLISHED_STATE";
        case eLIM_MLM_WT_ASSOC_CNF_STATE:
            return "eLIM_MLM_WT_ASSOC_CNF_STATE";
        case eLIM_MLM_WT_ADD_BSS_RSP_ASSOC_STATE:
            return "eLIM_MLM_WT_ADD_BSS_RSP_ASSOC_STATE";
        case eLIM_MLM_WT_ADD_BSS_RSP_REASSOC_STATE:
            return "eLIM_MLM_WT_ADD_BSS_RSP_REASSOC_STATE";
        case eLIM_MLM_WT_ADD_BSS_RSP_FT_REASSOC_STATE:
            return "eLIM_MLM_WT_ADD_BSS_RSP_FT_REASSOC_STATE";
        case eLIM_MLM_WT_ASSOC_DEL_STA_RSP_STATE:
            return "eLIM_MLM_WT_ASSOC_DEL_STA_RSP_STATE";
        case eLIM_MLM_WT_SET_BSS_KEY_STATE:
            return "eLIM_MLM_WT_SET_BSS_KEY_STATE";
        case eLIM_MLM_WT_SET_STA_KEY_STATE:
            return "eLIM_MLM_WT_SET_STA_KEY_STATE";
        default:
            return "INVALID MLM state";
    }
}

void
limPrintMlmState(tpAniSirGlobal pMac, tANI_U16 logLevel, tLimMlmStates state)
{
    limLog(pMac, logLevel, limMlmStateStr(state));
}

char *
limSmeStateStr(tLimSmeStates state)
{
#ifdef FIXME_GEN6
    switch (state)
    {
        case eLIM_SME_OFFLINE_STATE:
            return "eLIM_SME_OFFLINE_STATE";
        case eLIM_SME_IDLE_STATE:
            return "eLIM_SME_IDLE_STATE";
        case eLIM_SME_SUSPEND_STATE:
            return "eLIM_SME_SUSPEND_STATE";
        case eLIM_SME_WT_SCAN_STATE:
            return "eLIM_SME_WT_SCAN_STATE";
        case eLIM_SME_WT_JOIN_STATE:
            return "eLIM_SME_WT_JOIN_STATE";
        case eLIM_SME_WT_AUTH_STATE:
            return "eLIM_SME_WT_AUTH_STATE";
        case eLIM_SME_WT_ASSOC_STATE:
            return "eLIM_SME_WT_ASSOC_STATE";
        case eLIM_SME_WT_REASSOC_STATE:
            return "eLIM_SME_WT_REASSOC_STATE";
        case eLIM_SME_WT_REASSOC_LINK_FAIL_STATE:
            return "eLIM_SME_WT_REASSOC_LINK_FAIL_STATE";
        case eLIM_SME_JOIN_FAILURE_STATE:
            return "eLIM_SME_JOIN_FAILURE_STATE";
        case eLIM_SME_ASSOCIATED_STATE:
            return "eLIM_SME_ASSOCIATED_STATE";
        case eLIM_SME_REASSOCIATED_STATE:
            return "eLIM_SME_REASSOCIATED_STATE";
        case eLIM_SME_LINK_EST_STATE:
            return "eLIM_SME_LINK_EST_STATE";
        case eLIM_SME_LINK_EST_WT_SCAN_STATE:
            return "eLIM_SME_LINK_EST_WT_SCAN_STATE";
        case eLIM_SME_WT_PRE_AUTH_STATE:
            return "eLIM_SME_WT_PRE_AUTH_STATE";
        case eLIM_SME_WT_DISASSOC_STATE:
            return "eLIM_SME_WT_DISASSOC_STATE";
        case eLIM_SME_WT_DEAUTH_STATE:
            return "eLIM_SME_WT_DEAUTH_STATE";
        case eLIM_SME_WT_START_BSS_STATE:
            return "eLIM_SME_WT_START_BSS_STATE";
        case eLIM_SME_WT_STOP_BSS_STATE:
            return "eLIM_SME_WT_STOP_BSS_STATE";
        case eLIM_SME_NORMAL_STATE:
            return "eLIM_SME_NORMAL_STATE";
        case eLIM_SME_CHANNEL_SCAN_STATE:
            return "eLIM_SME_CHANNEL_SCAN_STATE";
        case eLIM_SME_NORMAL_CHANNEL_SCAN_STATE:
            return "eLIM_SME_NORMAL_CHANNEL_SCAN_STATE";
        default:
            return "INVALID SME state";
    }
#endif
return "";
}


char* limDot11ModeStr(tpAniSirGlobal pMac, tANI_U8 dot11Mode)
{
#ifdef FIXME_GEN6

    switch(dot11Mode)
        {
            case WNI_CFG_DOT11_MODE_ALL:
                return "ALL";
            case WNI_CFG_DOT11_MODE_11A:
                return "11A";
            case WNI_CFG_DOT11_MODE_11B:
                return "11B";
            case WNI_CFG_DOT11_MODE_11G:
                return "11G";
            case WNI_CFG_DOT11_MODE_11N:
                return "11N";
            case WNI_CFG_DOT11_MODE_POLARIS:
                return "Polaris";
            case WNI_CFG_DOT11_MODE_TITAN:
                return "Titan";
            case WNI_CFG_DOT11_MODE_TAURUS:
                return "Taurus";
            default:
                return "Invalid Dot11 Mode";
        }
#endif
return "";
}


char* limStaOpRateModeStr(tStaRateMode opRateMode)
{
#ifdef FIXME_GEN6

    switch(opRateMode)
        {
            case eSTA_TAURUS:
                return "Taurus";
            case eSTA_11a:
                return "11A";
            case eSTA_11b:
                return "11B";
            case eSTA_11bg:
                return "11G";
            case eSTA_11n:
                return "11N";
            case eSTA_POLARIS:
                return "Polaris";
            case eSTA_TITAN:
                return "Titan";
            default:
                return "Invalid Dot11 Mode";
        }
#endif
return "";
}

char* limBssTypeStr(tSirBssType bssType)
{
    switch(bssType)
    {
        case eSIR_INFRASTRUCTURE_MODE:
            return "eSIR_INFRASTRUCTURE_MODE";
        case eSIR_IBSS_MODE:
            return "eSIR_IBSS_MODE";
        case eSIR_BTAMP_STA_MODE:
            return "eSIR_BTAMP_STA_MODE";
        case eSIR_BTAMP_AP_MODE:
            return "eSIR_BTAMP_AP_MODE";
        case eSIR_AUTO_MODE:
            return "eSIR_AUTO_MODE";
        default:
            return "Invalid BSS Type";
    }
}

void
limPrintSmeState(tpAniSirGlobal pMac, tANI_U16 logLevel, tLimSmeStates state)
{
    limLog(pMac, logLevel, limSmeStateStr(state));
}

char *limMsgStr(tANI_U32 msgType)
{
#ifdef FIXME_GEN6
    switch (msgType)
    {
        case eWNI_SME_START_REQ:
            return "eWNI_SME_START_REQ";
        case eWNI_SME_START_RSP:
            return "eWNI_SME_START_RSP";
        case eWNI_SME_SYS_READY_IND:
            return "eWNI_SME_SYS_READY_IND";
        case eWNI_SME_SCAN_REQ:
            return "eWNI_SME_SCAN_REQ";
#ifdef FEATURE_OEM_DATA_SUPPORT
        case eWNI_SME_OEM_DATA_REQ:
            return "eWNI_SME_OEM_DATA_REQ";
        case eWNI_SME_OEM_DATA_RSP:
            return "eWNI_SME_OEM_DATA_RSP";
#endif
        case eWNI_SME_SCAN_RSP:
            return "eWNI_SME_SCAN_RSP";
        case eWNI_SME_JOIN_REQ:
            return "eWNI_SME_JOIN_REQ";
        case eWNI_SME_JOIN_RSP:
            return "eWNI_SME_JOIN_RSP";
        case eWNI_SME_SETCONTEXT_REQ:
            return "eWNI_SME_SETCONTEXT_REQ";
        case eWNI_SME_SETCONTEXT_RSP:
            return "eWNI_SME_SETCONTEXT_RSP";
        case eWNI_SME_REASSOC_REQ:
            return "eWNI_SME_REASSOC_REQ";
        case eWNI_SME_REASSOC_RSP:
            return "eWNI_SME_REASSOC_RSP";
        case eWNI_SME_AUTH_REQ:
            return "eWNI_SME_AUTH_REQ";
        case eWNI_SME_AUTH_RSP:
            return "eWNI_SME_AUTH_RSP";
        case eWNI_SME_DISASSOC_REQ:
            return "eWNI_SME_DISASSOC_REQ";
        case eWNI_SME_DISASSOC_RSP:
            return "eWNI_SME_DISASSOC_RSP";
        case eWNI_SME_DISASSOC_IND:
            return "eWNI_SME_DISASSOC_IND";
        case eWNI_SME_DISASSOC_CNF:
            return "eWNI_SME_DISASSOC_CNF";
        case eWNI_SME_DEAUTH_REQ:
            return "eWNI_SME_DEAUTH_REQ";
        case eWNI_SME_DEAUTH_RSP:
            return "eWNI_SME_DEAUTH_RSP";
        case eWNI_SME_DEAUTH_IND:
            return "eWNI_SME_DEAUTH_IND";
        case eWNI_SME_WM_STATUS_CHANGE_NTF:
            return "eWNI_SME_WM_STATUS_CHANGE_NTF";
        case eWNI_SME_START_BSS_REQ:
            return "eWNI_SME_START_BSS_REQ";
        case eWNI_SME_START_BSS_RSP:
            return "eWNI_SME_START_BSS_RSP";
        case eWNI_SME_AUTH_IND:
            return "eWNI_SME_AUTH_IND";
        case eWNI_SME_ASSOC_IND:
            return "eWNI_SME_ASSOC_IND";
        case eWNI_SME_ASSOC_CNF:
            return "eWNI_SME_ASSOC_CNF";
        case eWNI_SME_REASSOC_IND:
            return "eWNI_SME_REASSOC_IND";
        case eWNI_SME_REASSOC_CNF:
            return "eWNI_SME_REASSOC_CNF";
        case eWNI_SME_SWITCH_CHL_REQ:
            return "eWNI_SME_SWITCH_CHL_REQ";
        case eWNI_SME_SWITCH_CHL_RSP:
            return "eWNI_SME_SWITCH_CHL_RSP";
        case eWNI_SME_SWITCH_CHL_CB_PRIMARY_REQ:
            return "eWNI_SME_SWITCH_CHL_CB_PRIMARY_REQ";
        case eWNI_SME_SWITCH_CHL_CB_SECONDARY_REQ:
            return "eWNI_SME_SWITCH_CHL_CB_SECONDARY_REQ";
        case eWNI_SME_STOP_BSS_REQ:
            return "eWNI_SME_STOP_BSS_REQ";
        case eWNI_SME_STOP_BSS_RSP:
            return "eWNI_SME_STOP_BSS_RSP";
        case eWNI_SME_PROMISCUOUS_MODE_REQ:
            return "eWNI_SME_PROMISCUOUS_MODE_REQ";
        case eWNI_SME_PROMISCUOUS_MODE_RSP:
            return "eWNI_SME_PROMISCUOUS_MODE_RSP";
        case eWNI_SME_NEIGHBOR_BSS_IND:
            return "eWNI_SME_NEIGHBOR_BSS_IND";
        case eWNI_SME_MEASUREMENT_REQ:
            return "eWNI_SME_MEASUREMENT_REQ";
        case eWNI_SME_MEASUREMENT_RSP:
            return "eWNI_SME_MEASUREMENT_RSP";
        case eWNI_SME_MEASUREMENT_IND:
            return "eWNI_SME_MEASUREMENT_IND";
        case eWNI_SME_SET_WDS_INFO_REQ:
            return "eWNI_SME_SET_WDS_INFO_REQ";
        case eWNI_SME_SET_WDS_INFO_RSP:
            return "eWNI_SME_SET_WDS_INFO_RSP";
        case eWNI_SME_WDS_INFO_IND:
            return "eWNI_SME_WDS_INFO_IND";
        case eWNI_SME_DEAUTH_CNF:
            return "eWNI_SME_DEAUTH_CNF";
        case eWNI_SME_MIC_FAILURE_IND:
            return "eWNI_SME_MIC_FAILURE_IND";
        case eWNI_SME_ADDTS_REQ:
            return "eWNI_SME_ADDTS_REQ";
        case eWNI_SME_ADDTS_RSP:
            return "eWNI_SME_ADDTS_RSP";
        case eWNI_SME_ADDTS_CNF:
            return "eWNI_SME_ADDTS_CNF";
        case eWNI_SME_ADDTS_IND:
            return "eWNI_SME_ADDTS_IND";
        case eWNI_SME_DELTS_REQ:
            return "eWNI_SME_DELTS_REQ";
        case eWNI_SME_DELTS_RSP:
            return "eWNI_SME_DELTS_RSP";
        case eWNI_SME_DELTS_IND:
            return "eWNI_SME_DELTS_IND";
#if defined WLAN_FEATURE_VOWIFI_11R || defined FEATURE_WLAN_CCX || defined(FEATURE_WLAN_LFR)
        case eWNI_SME_GET_ROAM_RSSI_REQ:
            return "eWNI_SME_GET_ROAM_RSSI_REQ";
        case eWNI_SME_GET_ROAM_RSSI_RSP:
            return "eWNI_SME_GET_ROAM_RSSI_RSP";
#endif

        case WDA_SUSPEND_ACTIVITY_RSP:
            return "WDA_SUSPEND_ACTIVITY_RSP";
        case SIR_LIM_RETRY_INTERRUPT_MSG:
            return "SIR_LIM_RETRY_INTERRUPT_MSG";
        case SIR_BB_XPORT_MGMT_MSG:
            return "SIR_BB_XPORT_MGMT_MSG";
        case SIR_LIM_INV_KEY_INTERRUPT_MSG:
            return "SIR_LIM_INV_KEY_INTERRUPT_MSG";
        case SIR_LIM_KEY_ID_INTERRUPT_MSG:
            return "SIR_LIM_KEY_ID_INTERRUPT_MSG";
        case SIR_LIM_REPLAY_THRES_INTERRUPT_MSG:
            return "SIR_LIM_REPLAY_THRES_INTERRUPT_MSG";
        case SIR_LIM_MIN_CHANNEL_TIMEOUT:
            return "SIR_LIM_MIN_CHANNEL_TIMEOUT";
        case SIR_LIM_MAX_CHANNEL_TIMEOUT:
            return "SIR_LIM_MAX_CHANNEL_TIMEOUT";
        case SIR_LIM_JOIN_FAIL_TIMEOUT:
            return "SIR_LIM_JOIN_FAIL_TIMEOUT";
        case SIR_LIM_AUTH_FAIL_TIMEOUT:
            return "SIR_LIM_AUTH_FAIL_TIMEOUT";
        case SIR_LIM_AUTH_RSP_TIMEOUT:
            return "SIR_LIM_AUTH_RSP_TIMEOUT";
        case SIR_LIM_ASSOC_FAIL_TIMEOUT:
            return "SIR_LIM_ASSOC_FAIL_TIMEOUT";
        case SIR_LIM_REASSOC_FAIL_TIMEOUT:
            return "SIR_LIM_REASSOC_FAIL_TIMEOUT";
        case SIR_LIM_HEART_BEAT_TIMEOUT:
            return "SIR_LIM_HEART_BEAT_TIMEOUT";
        case SIR_LIM_ADDTS_RSP_TIMEOUT:
            return "SIR_LIM_ADDTS_RSP_TIMEOUT";
        case SIR_LIM_CHANNEL_SCAN_TIMEOUT:
            return "SIR_LIM_CHANNEL_SCAN_TIMEOUT";
        case SIR_LIM_LINK_TEST_DURATION_TIMEOUT:
            return "SIR_LIM_LINK_TEST_DURATION_TIMEOUT";
        case SIR_LIM_HASH_MISS_THRES_TIMEOUT:
            return "SIR_LIM_HASH_MISS_THRES_TIMEOUT";
        case SIR_LIM_KEEPALIVE_TIMEOUT:
            return "SIR_LIM_KEEPALIVE_TIMEOUT";
        case SIR_LIM_UPDATE_OLBC_CACHEL_TIMEOUT:
            return "SIR_LIM_UPDATE_OLBC_CACHEL_TIMEOUT";
        case SIR_LIM_CNF_WAIT_TIMEOUT:
            return "SIR_LIM_CNF_WAIT_TIMEOUT";
        case SIR_LIM_RADAR_DETECT_IND:
            return "SIR_LIM_RADAR_DETECT_IND";
#ifdef WLAN_FEATURE_VOWIFI_11R
        case SIR_LIM_FT_PREAUTH_RSP_TIMEOUT:
            return "SIR_LIM_FT_PREAUTH_RSP_TIMEOUT";
#endif

        case SIR_HAL_APP_SETUP_NTF:
            return "SIR_HAL_APP_SETUP_NTF";
        case SIR_HAL_INITIAL_CAL_FAILED_NTF:
            return "SIR_HAL_INITIAL_CAL_FAILED_NTF";
        case SIR_HAL_NIC_OPER_NTF:
            return "SIR_HAL_NIC_OPER_NTF";
        case SIR_HAL_INIT_START_REQ:
            return "SIR_HAL_INIT_START_REQ";
        case SIR_HAL_SHUTDOWN_REQ:
            return "SIR_HAL_SHUTDOWN_REQ";
        case SIR_HAL_SHUTDOWN_CNF:
            return "SIR_HAL_SHUTDOWN_CNF";
        case SIR_HAL_RESET_REQ:
            return "SIR_HAL_RESET_REQ";
        case SIR_HAL_RESET_CNF:
            return "SIR_HAL_RESET_CNF";
        case SIR_WRITE_TO_TD:
            return "SIR_WRITE_TO_TD";

        case WNI_CFG_PARAM_UPDATE_IND:
            return "WNI_CFG_PARAM_UPDATE_IND";
        case WNI_CFG_DNLD_REQ:
            return "WNI_CFG_DNLD_REQ";
        case WNI_CFG_DNLD_CNF:
            return "WNI_CFG_DNLD_CNF";
        case WNI_CFG_GET_RSP:
            return "WNI_CFG_GET_RSP";
        case WNI_CFG_SET_CNF:
            return "WNI_CFG_SET_CNF";
        case WNI_CFG_GET_ATTRIB_RSP:
            return "WNI_CFG_GET_ATTRIB_RSP";
        case WNI_CFG_ADD_GRP_ADDR_CNF:
            return "WNI_CFG_ADD_GRP_ADDR_CNF";
        case WNI_CFG_DEL_GRP_ADDR_CNF:
            return "WNI_CFG_DEL_GRP_ADDR_CNF";
        case ANI_CFG_GET_RADIO_STAT_RSP:
            return "ANI_CFG_GET_RADIO_STAT_RSP";
        case ANI_CFG_GET_PER_STA_STAT_RSP:
            return "ANI_CFG_GET_PER_STA_STAT_RSP";
        case ANI_CFG_GET_AGG_STA_STAT_RSP:
            return "ANI_CFG_GET_AGG_STA_STAT_RSP";
        case ANI_CFG_CLEAR_STAT_RSP:
            return "ANI_CFG_CLEAR_STAT_RSP";
        case WNI_CFG_DNLD_RSP:
            return "WNI_CFG_DNLD_RSP";
        case WNI_CFG_GET_REQ:
            return "WNI_CFG_GET_REQ";
        case WNI_CFG_SET_REQ:
            return "WNI_CFG_SET_REQ";
        case WNI_CFG_SET_REQ_NO_RSP:
            return "WNI_CFG_SET_REQ_NO_RSP";
        case eWNI_PMC_ENTER_IMPS_RSP:
            return "eWNI_PMC_ENTER_IMPS_RSP";
        case eWNI_PMC_EXIT_IMPS_RSP:
            return "eWNI_PMC_EXIT_IMPS_RSP";
        case eWNI_PMC_ENTER_BMPS_RSP:
            return "eWNI_PMC_ENTER_BMPS_RSP";
        case eWNI_PMC_EXIT_BMPS_RSP:
            return "eWNI_PMC_EXIT_BMPS_RSP";
        case eWNI_PMC_EXIT_BMPS_IND:
            return "eWNI_PMC_EXIT_BMPS_IND";
        case eWNI_SME_SET_BCN_FILTER_REQ:
            return "eWNI_SME_SET_BCN_FILTER_REQ";
#if defined(FEATURE_WLAN_CCX) && defined(FEATURE_WLAN_CCX_UPLOAD)
        case eWNI_SME_GET_TSM_STATS_REQ:
            return "eWNI_SME_GET_TSM_STATS_REQ";
        case eWNI_SME_GET_TSM_STATS_RSP:
            return "eWNI_SME_GET_TSM_STATS_RSP";
#endif /*                                             */
        default:
            return "INVALID SME message";
    }
#endif
return "";
}



char *limResultCodeStr(tSirResultCodes resultCode)
{
    switch (resultCode)
    {
      case eSIR_SME_SUCCESS:
            return "eSIR_SME_SUCCESS";
      case eSIR_EOF_SOF_EXCEPTION:
            return "eSIR_EOF_SOF_EXCEPTION";
      case eSIR_BMU_EXCEPTION:
            return "eSIR_BMU_EXCEPTION";
      case eSIR_LOW_PDU_EXCEPTION:
            return "eSIR_LOW_PDU_EXCEPTION";
      case eSIR_USER_TRIG_RESET:
            return"eSIR_USER_TRIG_RESET";
      case eSIR_LOGP_EXCEPTION:
            return "eSIR_LOGP_EXCEPTION";
      case eSIR_CP_EXCEPTION:
            return "eSIR_CP_EXCEPTION";
      case eSIR_STOP_BSS:
            return "eSIR_STOP_BSS";
      case eSIR_AHB_HANG_EXCEPTION:
            return "eSIR_AHB_HANG_EXCEPTION";
      case eSIR_DPU_EXCEPTION:
            return "eSIR_DPU_EXCEPTION";
      case eSIR_RXP_EXCEPTION:
            return "eSIR_RXP_EXCEPTION";
      case eSIR_MCPU_EXCEPTION:
            return "eSIR_MCPU_EXCEPTION";
      case eSIR_MCU_EXCEPTION:
            return "eSIR_MCU_EXCEPTION";
      case eSIR_MTU_EXCEPTION:
            return "eSIR_MTU_EXCEPTION";
      case eSIR_MIF_EXCEPTION:
            return "eSIR_MIF_EXCEPTION";
      case eSIR_FW_EXCEPTION:
            return "eSIR_FW_EXCEPTION";
      case eSIR_MAILBOX_SANITY_CHK_FAILED:
            return "eSIR_MAILBOX_SANITY_CHK_FAILED";
      case eSIR_RADIO_HW_SWITCH_STATUS_IS_OFF:
            return "eSIR_RADIO_HW_SWITCH_STATUS_IS_OFF";
      case eSIR_CFB_FLAG_STUCK_EXCEPTION:
            return "eSIR_CFB_FLAG_STUCK_EXCEPTION";
      case eSIR_SME_BASIC_RATES_NOT_SUPPORTED_STATUS:
            return "eSIR_SME_BASIC_RATES_NOT_SUPPORTED_STATUS";
      case eSIR_SME_INVALID_PARAMETERS:
            return "eSIR_SME_INVALID_PARAMETERS";
      case eSIR_SME_UNEXPECTED_REQ_RESULT_CODE:
            return "eSIR_SME_UNEXPECTED_REQ_RESULT_CODE";
      case eSIR_SME_RESOURCES_UNAVAILABLE:
            return "eSIR_SME_RESOURCES_UNAVAILABLE";
      case eSIR_SME_SCAN_FAILED:
            return "eSIR_SME_SCAN_FAILED";
      case eSIR_SME_BSS_ALREADY_STARTED_OR_JOINED:
            return "eSIR_SME_BSS_ALREADY_STARTED_OR_JOINED";
      case eSIR_SME_LOST_LINK_WITH_PEER_RESULT_CODE:
            return "eSIR_SME_LOST_LINK_WITH_PEER_RESULT_CODE";
      case eSIR_SME_REFUSED:
            return "eSIR_SME_REFUSED";
      case eSIR_SME_JOIN_TIMEOUT_RESULT_CODE:
            return "eSIR_SME_JOIN_TIMEOUT_RESULT_CODE";
      case eSIR_SME_AUTH_TIMEOUT_RESULT_CODE:
            return "eSIR_SME_AUTH_TIMEOUT_RESULT_CODE";
      case eSIR_SME_ASSOC_TIMEOUT_RESULT_CODE:
            return "eSIR_SME_ASSOC_TIMEOUT_RESULT_CODE";
      case eSIR_SME_REASSOC_TIMEOUT_RESULT_CODE:
            return "eSIR_SME_REASSOC_TIMEOUT_RESULT_CODE";
      case eSIR_SME_MAX_NUM_OF_PRE_AUTH_REACHED:
            return "eSIR_SME_MAX_NUM_OF_PRE_AUTH_REACHED";
      case eSIR_SME_AUTH_REFUSED:
            return "eSIR_SME_AUTH_REFUSED";
      case eSIR_SME_INVALID_WEP_DEFAULT_KEY:
            return "eSIR_SME_INVALID_WEP_DEFAULT_KEY";
      case eSIR_SME_ASSOC_REFUSED:
            return "eSIR_SME_ASSOC_REFUSED";
      case eSIR_SME_REASSOC_REFUSED:
            return "eSIR_SME_REASSOC_REFUSED";
      case eSIR_SME_STA_NOT_AUTHENTICATED:
            return "eSIR_SME_STA_NOT_AUTHENTICATED";
      case eSIR_SME_STA_NOT_ASSOCIATED:
            return "eSIR_SME_STA_NOT_ASSOCIATED";
      case eSIR_SME_STA_DISASSOCIATED:
            return "eSIR_SME_STA_DISASSOCIATED";
      case eSIR_SME_ALREADY_JOINED_A_BSS:
            return "eSIR_SME_ALREADY_JOINED_A_BSS";
      case eSIR_ULA_COMPLETED:
            return "eSIR_ULA_COMPLETED";
      case eSIR_ULA_FAILURE:
            return "eSIR_ULA_FAILURE";
      case eSIR_SME_LINK_ESTABLISHED:
            return "eSIR_SME_LINK_ESTABLISHED";
      case eSIR_SME_UNABLE_TO_PERFORM_MEASUREMENTS:
            return "eSIR_SME_UNABLE_TO_PERFORM_MEASUREMENTS";
      case eSIR_SME_UNABLE_TO_PERFORM_DFS:
            return "eSIR_SME_UNABLE_TO_PERFORM_DFS";
      case eSIR_SME_DFS_FAILED:
            return "eSIR_SME_DFS_FAILED";
      case eSIR_SME_TRANSFER_STA:
            return "eSIR_SME_TRANSFER_STA";
      case eSIR_SME_INVALID_LINK_TEST_PARAMETERS:
            return "eSIR_SME_INVALID_LINK_TEST_PARAMETERS";
      case eSIR_SME_LINK_TEST_MAX_EXCEEDED:
            return "eSIR_SME_LINK_TEST_MAX_EXCEEDED";
      case eSIR_SME_UNSUPPORTED_RATE:
            return "eSIR_SME_UNSUPPORTED_RATE";
      case eSIR_SME_LINK_TEST_TIMEOUT:
            return "eSIR_SME_LINK_TEST_TIMEOUT";
      case eSIR_SME_LINK_TEST_COMPLETE:
            return "eSIR_SME_LINK_TEST_COMPLETE";
      case eSIR_SME_LINK_TEST_INVALID_STATE:
            return "eSIR_SME_LINK_TEST_INVALID_STATE";
      case eSIR_SME_LINK_TEST_INVALID_ADDRESS:
            return "eSIR_SME_LINK_TEST_INVALID_ADDRESS";
      case eSIR_SME_POLARIS_RESET:
            return "eSIR_SME_POLARIS_RESET";
      case eSIR_SME_SETCONTEXT_FAILED:
            return "eSIR_SME_SETCONTEXT_FAILED";
      case eSIR_SME_BSS_RESTART:
            return "eSIR_SME_BSS_RESTART";
      case eSIR_SME_MORE_SCAN_RESULTS_FOLLOW:
            return "eSIR_SME_MORE_SCAN_RESULTS_FOLLOW";
      case eSIR_SME_INVALID_ASSOC_RSP_RXED:
            return "eSIR_SME_INVALID_ASSOC_RSP_RXED";
      case eSIR_SME_MIC_COUNTER_MEASURES:
            return "eSIR_SME_MIC_COUNTER_MEASURES";
      case eSIR_SME_ADDTS_RSP_TIMEOUT:
            return "eSIR_SME_ADDTS_RSP_TIMEOUT";
      case eSIR_SME_RECEIVED:
            return "eSIR_SME_RECEIVED";
      case eSIR_SME_CHANNEL_SWITCH_FAIL:
            return "eSIR_SME_CHANNEL_SWITCH_FAIL";
#ifdef GEN4_SCAN
      case eSIR_SME_CHANNEL_SWITCH_DISABLED:
            return "eSIR_SME_CHANNEL_SWITCH_DISABLED";
      case eSIR_SME_HAL_SCAN_INIT_FAILED:
            return "eSIR_SME_HAL_SCAN_INIT_FAILED";
      case eSIR_SME_HAL_SCAN_START_FAILED:
            return "eSIR_SME_HAL_SCAN_START_FAILED";
      case eSIR_SME_HAL_SCAN_END_FAILED:
            return "eSIR_SME_HAL_SCAN_END_FAILED";
      case eSIR_SME_HAL_SCAN_FINISH_FAILED:
            return "eSIR_SME_HAL_SCAN_FINISH_FAILED";
      case eSIR_SME_HAL_SEND_MESSAGE_FAIL:
            return "eSIR_SME_HAL_SEND_MESSAGE_FAIL";
#else //          
      case eSIR_SME_CHANNEL_SWITCH_DISABLED:
            return "eSIR_SME_CHANNEL_SWITCH_DISABLED";
      case eSIR_SME_HAL_SEND_MESSAGE_FAIL:
            return "eSIR_SME_HAL_SEND_MESSAGE_FAIL";
#endif //          

        default:
            return "INVALID resultCode";
    }
}

void
limPrintMsgName(tpAniSirGlobal pMac, tANI_U16 logLevel, tANI_U32 msgType)
{
    limLog(pMac, logLevel, limMsgStr(msgType));
}

void
limPrintMsgInfo(tpAniSirGlobal pMac, tANI_U16 logLevel, tSirMsgQ *msg)
{
    if (logLevel <= pMac->utils.gLogDbgLevel[SIR_LIM_MODULE_ID - LOG_FIRST_MODULE_ID])
    {
        switch (msg->type)
        {
            case SIR_BB_XPORT_MGMT_MSG:
                limPrintMsgName(pMac, logLevel,msg->type);
                break;
            default:
                limPrintMsgName(pMac, logLevel,msg->type);
                break;
        }
    }
}

/* 
               
  
           
                                                        
                                      
  
         
  
        
  
              
     
  
       
     
  
                                                    
               
 */
void
limInitMlm(tpAniSirGlobal pMac)
{
    tANI_U32 retVal;

    pMac->lim.gLimTimersCreated = 0;

    MTRACE(macTrace(pMac, TRACE_CODE_MLM_STATE, NO_SESSION, pMac->lim.gLimMlmState));

    //                                   
    limReInitScanResults(pMac); //              

#ifdef WLAN_FEATURE_ROAM_SCAN_OFFLOAD
    //                                       
    //                                                                      
    //                                                                         
    //                             
    limReInitLfrScanResults(pMac);
#endif
  
    //                                        
    pMac->lim.gLimNumPreAuthContexts = 0;

    //                                              
    limInitPreAuthList(pMac);

    //pMac->lim.gpLimMlmJoinReq = NULL;

    if (pMac->lim.gLimTimersCreated)
        return;

    //                          
    retVal = limCreateTimers(pMac);
    if(retVal == TX_SUCCESS)
    {
        pMac->lim.gLimTimersCreated = 1;
    }
    else
    {
        limLog(pMac, LOGP, FL(" limCreateTimers Failed to create lim timers "));
    }
} /*                      */



/* 
                  
  
           
                                                   
                                       
  
         
  
        
  
              
     
  
       
                                                                   
                                  
  
                                                    
               
               
 */
void
limCleanupMlm(tpAniSirGlobal pMac)
{
    tANI_U32   n;
    tLimPreAuthNode *pAuthNode;

    if (pMac->lim.gLimTimersCreated == 1)
    {
        //                                              
        tx_timer_deactivate(&pMac->lim.limTimers.gLimMinChannelTimer);
        tx_timer_delete(&pMac->lim.limTimers.gLimMinChannelTimer);
        tx_timer_deactivate(&pMac->lim.limTimers.gLimMaxChannelTimer);
        tx_timer_delete(&pMac->lim.limTimers.gLimMaxChannelTimer);
        tx_timer_deactivate(&pMac->lim.limTimers.gLimPeriodicProbeReqTimer);
        tx_timer_delete(&pMac->lim.limTimers.gLimPeriodicProbeReqTimer);


        //                                            
        tx_timer_deactivate(&pMac->lim.limTimers.gLimChannelSwitchTimer);
        tx_timer_delete(&pMac->lim.limTimers.gLimChannelSwitchTimer);


        //                                            
        tx_timer_deactivate(&pMac->lim.limTimers.gLimAddtsRspTimer);
        tx_timer_delete(&pMac->lim.limTimers.gLimAddtsRspTimer);

        //                                          
        tx_timer_deactivate(&pMac->lim.limTimers.gLimJoinFailureTimer);
        tx_timer_delete(&pMac->lim.limTimers.gLimJoinFailureTimer);

        //                                                         
        tx_timer_deactivate(&pMac->lim.limTimers.gLimPeriodicJoinProbeReqTimer);
        tx_timer_delete(&pMac->lim.limTimers.gLimPeriodicJoinProbeReqTimer);

        //                                                 
        tx_timer_deactivate(&pMac->lim.limTimers.gLimAssocFailureTimer);
        tx_timer_delete(&pMac->lim.limTimers.gLimAssocFailureTimer);

        //                                                   
        tx_timer_deactivate(&pMac->lim.limTimers.gLimReassocFailureTimer);
        tx_timer_delete(&pMac->lim.limTimers.gLimReassocFailureTimer);

        //                                                    
        tx_timer_deactivate(&pMac->lim.limTimers.gLimAuthFailureTimer);
        tx_timer_delete(&pMac->lim.limTimers.gLimAuthFailureTimer);

        //                                       
        tx_timer_deactivate(&pMac->lim.limTimers.gLimHeartBeatTimer);
        tx_timer_delete(&pMac->lim.limTimers.gLimHeartBeatTimer);

        //                                                            
        tx_timer_deactivate(&pMac->lim.limTimers.gLimProbeAfterHBTimer);
        tx_timer_delete(&pMac->lim.limTimers.gLimProbeAfterHBTimer);

        //                                   
        tx_timer_deactivate(&pMac->lim.limTimers.gLimQuietTimer);
        tx_timer_delete(&pMac->lim.limTimers.gLimQuietTimer);

        //                                       
        tx_timer_deactivate(&pMac->lim.limTimers.gLimQuietBssTimer);
        tx_timer_delete(&pMac->lim.limTimers.gLimQuietBssTimer);

        //                                                 
        tx_timer_deactivate(&pMac->lim.limTimers.gLimBackgroundScanTimer);
        tx_timer_delete(&pMac->lim.limTimers.gLimBackgroundScanTimer);


        //                                     
        for (n = 0; n < pMac->lim.maxStation; n++)
        {
            tx_timer_deactivate(&pMac->lim.limTimers.gpLimCnfWaitTimer[n]);
            tx_timer_delete(&pMac->lim.limTimers.gpLimCnfWaitTimer[n]);
        }

        //                                      
        tx_timer_deactivate(&pMac->lim.limTimers.gLimKeepaliveTimer);
        tx_timer_delete(&pMac->lim.limTimers.gLimKeepaliveTimer);

        pAuthNode = pMac->lim.gLimPreAuthTimerTable.pTable;
        
        //                                             
        limDeletePreAuthList(pMac);

        for (n = 0; n < pMac->lim.gLimPreAuthTimerTable.numEntry; n++,pAuthNode++)
        {
            //                                   
            //                                       
            tx_timer_delete(&pAuthNode->timer);
        }



        //                                               
        tx_timer_deactivate(&pMac->lim.limTimers.gLimSendDisassocFrameThresholdTimer);
        tx_timer_delete(&pMac->lim.limTimers.gLimSendDisassocFrameThresholdTimer);

        tx_timer_deactivate(&pMac->lim.limTimers.gLimUpdateOlbcCacheTimer);
        tx_timer_delete(&pMac->lim.limTimers.gLimUpdateOlbcCacheTimer);
        tx_timer_deactivate(&pMac->lim.limTimers.gLimPreAuthClnupTimer);
        tx_timer_delete(&pMac->lim.limTimers.gLimPreAuthClnupTimer);

#if 0 //                                       
        if (pMac->lim.gLimSystemRole == eLIM_AP_ROLE)
        {
            if(pMac->lim.limTimers.gLimWPSOverlapTimerObj.isTimerCreated == eANI_BOOLEAN_TRUE)
            {
                tx_timer_deactivate(&pMac->lim.limTimers.gLimWPSOverlapTimerObj.gLimWPSOverlapTimer);
                tx_timer_delete(&pMac->lim.limTimers.gLimWPSOverlapTimerObj.gLimWPSOverlapTimer);
                pMac->lim.limTimers.gLimWPSOverlapTimerObj.isTimerCreated = eANI_BOOLEAN_FALSE;
            }
        }
#endif
#ifdef WLAN_FEATURE_VOWIFI_11R
        //                                                
        tx_timer_deactivate(&pMac->lim.limTimers.gLimFTPreAuthRspTimer);
        tx_timer_delete(&pMac->lim.limTimers.gLimFTPreAuthRspTimer);
#endif

        //                                              
        tx_timer_deactivate(&pMac->lim.limTimers.gLimRemainOnChannelTimer);
        tx_timer_delete(&pMac->lim.limTimers.gLimRemainOnChannelTimer);

#if defined(FEATURE_WLAN_CCX) && !defined(FEATURE_WLAN_CCX_UPLOAD)
        //                          
        tx_timer_deactivate(&pMac->lim.limTimers.gLimCcxTsmTimer);
        tx_timer_delete(&pMac->lim.limTimers.gLimCcxTsmTimer);
#endif /*                                              */

        tx_timer_deactivate(&pMac->lim.limTimers.gLimDisassocAckTimer);
        tx_timer_delete(&pMac->lim.limTimers.gLimDisassocAckTimer);

        tx_timer_deactivate(&pMac->lim.limTimers.gLimDeauthAckTimer);
        tx_timer_delete(&pMac->lim.limTimers.gLimDeauthAckTimer);

        tx_timer_deactivate(&pMac->lim.limTimers.gLimP2pSingleShotNoaInsertTimer);
        tx_timer_delete(&pMac->lim.limTimers.gLimP2pSingleShotNoaInsertTimer);

        tx_timer_deactivate(&pMac->lim.limTimers.gLimActiveToPassiveChannelTimer);
        tx_timer_delete(&pMac->lim.limTimers.gLimActiveToPassiveChannelTimer);

        pMac->lim.gLimTimersCreated = 0;
    }

    //                          
    limReInitScanResults(pMac);
#ifdef WLAN_FEATURE_ROAM_SCAN_OFFLOAD
    //                          
    limReInitLfrScanResults(pMac);
#endif

} /*                         */



/* 
                  
  
           
                                                   
                               
  
         
  
        
  
              
     
  
       
     
  
                                                    
               
 */

void
limCleanupLmm(tpAniSirGlobal pMac)
{
} /*                         */



/* 
                
  
           
                                                            
                                                                
  
        
  
              
     
  
       
     
  
                                                                   
                                                         
  
                                                                 
 */

tANI_U8
limIsAddrBC(tSirMacAddr macAddr)
{
    int i;
    for (i = 0; i < 6; i++)
    {
        if ((macAddr[i] & 0xFF) != 0xFF)
            return false;
    }

    return true;
} /*                             */



/* 
                   
  
           
                                                            
                                                                    
  
        
                                                                
                                   
  
              
     
  
       
     
  
                                                                   
                                                     
  
                                                             
 */

tANI_U8
limIsGroupAddr(tSirMacAddr macAddr)
{
    if ((macAddr[0] & 0x01) == 0x01)
        return true;
    else
        return false;
} /*                                */

/* 
                        
  
           
                                                            
                                                               
  
        
     
  
              
     
  
       
     
  
                                                     
                                                  
               
 */

tANI_U32
limPostMsgApiNoWait(tpAniSirGlobal pMac, tSirMsgQ *pMsg)
{
    limProcessMessages(pMac, pMsg);
    return TX_SUCCESS;
} /*                               */



/* 
                    
  
           
                                                      
               
  
        
  
              
     
  
       
                                           
                                         
  
                
 */

void
limPrintMacAddr(tpAniSirGlobal pMac, tSirMacAddr macAddr, tANI_U8 logLevel)
{
    limLog(pMac, logLevel,
           FL(MAC_ADDRESS_STR), MAC_ADDR_ARRAY(macAddr));
} /*                                 */






/*
                         
  
           
                                                              
  
         
                                                    
  
        
  
              
     
  
       
     
  
          
       
 */

void limResetDeferredMsgQ(tpAniSirGlobal pMac)
{
    pMac->lim.gLimDeferredMsgQ.size =
    pMac->lim.gLimDeferredMsgQ.write =
    pMac->lim.gLimDeferredMsgQ.read = 0;

}


#define LIM_DEFERRED_Q_CHECK_THRESHOLD  (MAX_DEFERRED_QUEUE_LEN/2)
#define LIM_MAX_NUM_MGMT_FRAME_DEFERRED (MAX_DEFERRED_QUEUE_LEN/2)

/*
                         
  
           
                                                                         
            
  
         
                                                    
                                  
  
        
  
              
     
  
       
     
  
          
       
 */

tANI_U8 limWriteDeferredMsgQ(tpAniSirGlobal pMac, tpSirMsgQ limMsg)
{
    PELOG1(limLog(pMac, LOG1,
           FL("**  Queue a deferred message (size %d, write %d) - type 0x%x  **"),
           pMac->lim.gLimDeferredMsgQ.size, pMac->lim.gLimDeferredMsgQ.write,
           limMsg->type);)

        /*
                                                       
          */
    if (pMac->lim.gLimDeferredMsgQ.size >= MAX_DEFERRED_QUEUE_LEN)
    {
        if(!(pMac->lim.deferredMsgCnt & 0xF))
        {
            PELOGE(limLog(pMac, LOGE, FL("Deferred Message Queue is full. Msg:%d Messages Failed:%d"), limMsg->type, ++pMac->lim.deferredMsgCnt);)
        }
        else
        {
            pMac->lim.deferredMsgCnt++;
        }
        return TX_QUEUE_FULL;
    }

    /*
                                                                      
                                                                      
              
     */
    if (pMac->lim.gLimDeferredMsgQ.size > 0)
    {
        PELOGW(limLog(pMac, LOGW, FL("%d Deferred messages (type 0x%x, scan %d, global sme %d, global mlme %d, addts %d)"),
               pMac->lim.gLimDeferredMsgQ.size, limMsg->type,
               limIsSystemInScanState(pMac),
               pMac->lim.gLimSmeState, pMac->lim.gLimMlmState,
               pMac->lim.gLimAddtsSent);)
    }

    /*
                                                                                           
     */
    if( SIR_BB_XPORT_MGMT_MSG == limMsg->type )
    {
        if( LIM_DEFERRED_Q_CHECK_THRESHOLD < pMac->lim.gLimDeferredMsgQ.size )
        {
            tANI_U16 idx, count = 0;
            for(idx = 0; idx < pMac->lim.gLimDeferredMsgQ.size; idx++)
            {
                if( SIR_BB_XPORT_MGMT_MSG == pMac->lim.gLimDeferredMsgQ.deferredQueue[idx].type )
                {
                    count++;
                }
            }
            if( LIM_MAX_NUM_MGMT_FRAME_DEFERRED < count )
            {
                //                                                       
                PELOGW(limLog(pMac, LOGW, FL("Cannot deferred. Msg: %d Too many (count=%d) already"), limMsg->type, count);)
                //                                     
                return TX_QUEUE_FULL;
            }
        }
    }

    ++pMac->lim.gLimDeferredMsgQ.size;

    /*                                                             */
    if(pMac->lim.deferredMsgCnt != 0)
    {
        pMac->lim.deferredMsgCnt = 0;
    }

    /*
                                                                
     */
    if (pMac->lim.gLimDeferredMsgQ.write >= MAX_DEFERRED_QUEUE_LEN)
        pMac->lim.gLimDeferredMsgQ.write = 0;

    /*
                                                                   
     */
    vos_mem_copy( (tANI_U8 *)&pMac->lim.gLimDeferredMsgQ.deferredQueue[
                    pMac->lim.gLimDeferredMsgQ.write++],
                  (tANI_U8 *)limMsg,
                  sizeof(tSirMsgQ));
    return TX_SUCCESS;

}

/*
                        
  
           
                                                                  
            
  
         
                                                    
  
        
  
              
     
  
       
  
  
          
                                                                
 */

tSirMsgQ* limReadDeferredMsgQ(tpAniSirGlobal pMac)
{
    tSirMsgQ    *msg;

    /*
                                             
     */
    if (pMac->lim.gLimDeferredMsgQ.size <= 0)
        return NULL;

    /*
                               
     */
    pMac->lim.gLimDeferredMsgQ.size--;

    /*
                                                      
     */
    msg = &pMac->lim.gLimDeferredMsgQ.deferredQueue[pMac->lim.gLimDeferredMsgQ.read];

    /*
                               
     */
    pMac->lim.gLimDeferredMsgQ.read++;

    /*
                                                               
     */
    if (pMac->lim.gLimDeferredMsgQ.read >= MAX_DEFERRED_QUEUE_LEN)
        pMac->lim.gLimDeferredMsgQ.read = 0;

   PELOG1(limLog(pMac, LOG1,
           FL("**  DeQueue a deferred message (size %d read %d) - type 0x%x  **"),
           pMac->lim.gLimDeferredMsgQ.size, pMac->lim.gLimDeferredMsgQ.read,
           msg->type);)

   PELOG1(limLog(pMac, LOG1, FL("DQ msg -- scan %d, global sme %d, global mlme %d, addts %d"),
           limIsSystemInScanState(pMac),
           pMac->lim.gLimSmeState, pMac->lim.gLimMlmState,
           pMac->lim.gLimAddtsSent);)

    return(msg);
}

tSirRetStatus
limSysProcessMmhMsgApi(tpAniSirGlobal pMac,
                    tSirMsgQ *pMsg,
                    tANI_U8 qType)
{
//      
   SysProcessMmhMsg(pMac, pMsg);
   return eSIR_SUCCESS;
}

char *limFrameStr(tANI_U32 type, tANI_U32 subType)
{
#ifdef FIXME_GEN6

    if (type == SIR_MAC_MGMT_FRAME)
    {
        switch (subType)
        {
            case SIR_MAC_MGMT_ASSOC_REQ:
                return "MAC_MGMT_ASSOC_REQ";
            case SIR_MAC_MGMT_ASSOC_RSP:
                return "MAC_MGMT_ASSOC_RSP";
            case SIR_MAC_MGMT_REASSOC_REQ:
                return "MAC_MGMT_REASSOC_REQ";
            case SIR_MAC_MGMT_REASSOC_RSP:
                return "MAC_MGMT_REASSOC_RSP";
            case SIR_MAC_MGMT_PROBE_REQ:
                return "MAC_MGMT_PROBE_REQ";
            case SIR_MAC_MGMT_PROBE_RSP:
                return "MAC_MGMT_PROBE_RSP";
            case SIR_MAC_MGMT_BEACON:
                return "MAC_MGMT_BEACON";
            case SIR_MAC_MGMT_ATIM:
                return "MAC_MGMT_ATIM";
            case SIR_MAC_MGMT_DISASSOC:
                return "MAC_MGMT_DISASSOC";
            case SIR_MAC_MGMT_AUTH:
                return "MAC_MGMT_AUTH";
            case SIR_MAC_MGMT_DEAUTH:
                return "MAC_MGMT_DEAUTH";
            case SIR_MAC_MGMT_ACTION:
                return "MAC_MGMT_ACTION";
            case SIR_MAC_MGMT_RESERVED15:
                return "MAC_MGMT_RESERVED15";
            default:
                return "Unknown MGMT Frame";
        }
    }

    else if (type == SIR_MAC_CTRL_FRAME)
    {
        switch (subType)
        {
            case SIR_MAC_CTRL_RR:
                return "MAC_CTRL_RR";
            case SIR_MAC_CTRL_BAR:
                return "MAC_CTRL_BAR";
            case SIR_MAC_CTRL_BA:
                return "MAC_CTRL_BA";
            case SIR_MAC_CTRL_PS_POLL:
                return "MAC_CTRL_PS_POLL";
            case SIR_MAC_CTRL_RTS:
                return "MAC_CTRL_RTS";
            case SIR_MAC_CTRL_CTS:
                return "MAC_CTRL_CTS";
            case SIR_MAC_CTRL_ACK:
                return "MAC_CTRL_ACK";
            case SIR_MAC_CTRL_CF_END:
                return "MAC_CTRL_CF_END";
            case SIR_MAC_CTRL_CF_END_ACK:
                return "MAC_CTRL_CF_END_ACK";
            default:
                return "Unknown CTRL Frame";
        }
    }

    else if (type == SIR_MAC_DATA_FRAME)
    {
        switch (subType)
        {
            case SIR_MAC_DATA_DATA:
                return "MAC_DATA_DATA";
            case SIR_MAC_DATA_DATA_ACK:
                return "MAC_DATA_DATA_ACK";
            case SIR_MAC_DATA_DATA_POLL:
                return "MAC_DATA_DATA_POLL";
            case SIR_MAC_DATA_DATA_ACK_POLL:
                return "MAC_DATA_DATA_ACK_POLL";
            case SIR_MAC_DATA_NULL:
                return "MAC_DATA_NULL";
            case SIR_MAC_DATA_NULL_ACK:
                return "MAC_DATA_NULL_ACK";
            case SIR_MAC_DATA_NULL_POLL:
                return "MAC_DATA_NULL_POLL";
            case SIR_MAC_DATA_NULL_ACK_POLL:
                return "MAC_DATA_NULL_ACK_POLL";
            case SIR_MAC_DATA_QOS_DATA:
                return "MAC_DATA_QOS_DATA";
            case SIR_MAC_DATA_QOS_DATA_ACK:
                return "MAC_DATA_QOS_DATA_ACK";
            case SIR_MAC_DATA_QOS_DATA_POLL:
                return "MAC_DATA_QOS_DATA_POLL";
            case SIR_MAC_DATA_QOS_DATA_ACK_POLL:
                return "MAC_DATA_QOS_DATA_ACK_POLL";
            case SIR_MAC_DATA_QOS_NULL:
                return "MAC_DATA_QOS_NULL";
            case SIR_MAC_DATA_QOS_NULL_ACK:
                return "MAC_DATA_QOS_NULL_ACK";
            case SIR_MAC_DATA_QOS_NULL_POLL:
                return "MAC_DATA_QOS_NULL_POLL";
            case SIR_MAC_DATA_QOS_NULL_ACK_POLL:
                return "MAC_DATA_QOS_NULL_ACK_POLL";
            default:
                return "Unknown Data Frame";
        }
    }
    else
        return "Unknown";
#endif
return "";
}

void limHandleUpdateOlbcCache(tpAniSirGlobal pMac)
{
    int i;
    static int enable;
    tUpdateBeaconParams beaconParams;

    tpPESession       psessionEntry = limIsApSessionActive(pMac);

    if (psessionEntry == NULL)
    {
        PELOGE(limLog(pMac, LOGE, FL(" Session not found"));)
        return;
    }

    vos_mem_set( ( tANI_U8* )&beaconParams, sizeof( tUpdateBeaconParams), 0);
    beaconParams.bssIdx = psessionEntry->bssIdx;
    
    beaconParams.paramChangeBitmap = 0;
    /*
                                                                    
                                                                     
                          
     */
    if (!enable)
    {

            PELOG2(limLog(pMac, LOG2, FL("Resetting OLBC cache"));)
            psessionEntry->gLimOlbcParams.numSta = 0;
            psessionEntry->gLimOverlap11gParams.numSta = 0;
            psessionEntry->gLimOverlapHt20Params.numSta = 0;
            psessionEntry->gLimNonGfParams.numSta = 0;
            psessionEntry->gLimLsigTxopParams.numSta = 0;

        for (i=0; i < LIM_PROT_STA_OVERLAP_CACHE_SIZE; i++)
            pMac->lim.protStaOverlapCache[i].active = false;

        enable = 1;
    }
    else
    {

        if (!psessionEntry->gLimOlbcParams.numSta)
        {
            if (psessionEntry->gLimOlbcParams.protectionEnabled)
            {
                if (!psessionEntry->gLim11bParams.protectionEnabled)
                {
                    PELOG1(limLog(pMac, LOG1, FL("Overlap cache all clear and no 11B STA detected"));)
                    limEnable11gProtection(pMac, false, true, &beaconParams, psessionEntry);
                }
            }
        }

        if (!psessionEntry->gLimOverlap11gParams.numSta)
        {
            if (psessionEntry->gLimOverlap11gParams.protectionEnabled)
            {
                if (!psessionEntry->gLim11gParams.protectionEnabled)
                {
                    PELOG1(limLog(pMac, LOG1, FL("Overlap cache all clear and no 11G STA detected"));)
                    limEnableHtProtectionFrom11g(pMac, false, true, &beaconParams,psessionEntry);
                }
            }
        }

        if (!psessionEntry->gLimOverlapHt20Params.numSta)
        {
            if (psessionEntry->gLimOverlapHt20Params.protectionEnabled)
            {
                if (!psessionEntry->gLimHt20Params.protectionEnabled)
                {
                    PELOG1(limLog(pMac, LOG1, FL("Overlap cache all clear and no HT20 STA detected"));)
                    limEnable11gProtection(pMac, false, true, &beaconParams,psessionEntry);
                }
            }
        }

        enable = 0;
    }

    if(beaconParams.paramChangeBitmap)
    {
        schSetFixedBeaconFields(pMac,psessionEntry);
        limSendBeaconParams(pMac, &beaconParams, psessionEntry);
    }

    //                 
    if (tx_timer_activate(&pMac->lim.limTimers.gLimUpdateOlbcCacheTimer) != TX_SUCCESS)
    {
        limLog(pMac, LOGE, FL("tx_timer_activate failed"));
    }
}

/* 
                  
  
           
                                                     
  
  
        
  
              
     
  
       
     
  
                       
  
  
                                               
 */

tANI_U8
limIsNullSsid( tSirMacSSid *pSsid )
{
    tANI_U8 fNullSsid = false;
    tANI_U32 SsidLength;
    tANI_U8 *pSsidStr;

    do
    {
        if ( 0 == pSsid->length )
        {
            fNullSsid = true;
            break;
        }

#define ASCII_SPACE_CHARACTER 0x20
        /*                                                                  
                                                      */
        if( ASCII_SPACE_CHARACTER == pSsid->ssId[0])
        {
            SsidLength = pSsid->length;
            pSsidStr = pSsid->ssId;
            /*                                              */
            while ( SsidLength )
            {
                if( ASCII_SPACE_CHARACTER != *pSsidStr )
                    break;
    
                pSsidStr++;
                SsidLength--;
            }
    
            if( 0 == SsidLength )
            {
                fNullSsid = true;
                break;
            }
        }
        else
        {
            /*                                            */
            SsidLength = pSsid->length;
            pSsidStr = pSsid->ssId;

            while ( SsidLength )
            {
                if( *pSsidStr )
                    break;

                pSsidStr++;
                SsidLength--;
            }

            if( 0 == SsidLength )
            {
                fNullSsid = true;
                break;
            }
        }
    }
    while( 0 );

    return fNullSsid;
} /*                               */




/*                                                               
                          
                                           
                                  
                                   
                                                 
                                  
                                        
                 
                                                               */
void
limUpdateProtStaParams(tpAniSirGlobal pMac,
tSirMacAddr peerMacAddr, tLimProtStaCacheType protStaCacheType,
tHalBitVal gfSupported, tHalBitVal lsigTxopSupported,
tpPESession psessionEntry)
{
  tANI_U32 i;

  PELOG1(limLog(pMac,LOG1, FL("A STA is associated:"));
  limLog(pMac,LOG1, FL("Addr : "));
  limPrintMacAddr(pMac, peerMacAddr, LOG1);)

  for (i=0; i<LIM_PROT_STA_CACHE_SIZE; i++)
  {
      if (psessionEntry->protStaCache[i].active)
      {
          PELOG1(limLog(pMac, LOG1, FL("Addr: "));)
          PELOG1(limPrintMacAddr(pMac, psessionEntry->protStaCache[i].addr, LOG1);)

          if (vos_mem_compare(
              psessionEntry->protStaCache[i].addr,
              peerMacAddr, sizeof(tSirMacAddr)))
          {
              PELOG1(limLog(pMac, LOG1, FL("matching cache entry at %d already active."), i);)
              return;
          }
      }
  }

  for (i=0; i<LIM_PROT_STA_CACHE_SIZE; i++)
  {
      if (!psessionEntry->protStaCache[i].active)
          break;
  }

  if (i >= LIM_PROT_STA_CACHE_SIZE)
  {
      PELOGE(limLog(pMac, LOGE, FL("No space in ProtStaCache"));)
      return;
  }

  vos_mem_copy( psessionEntry->protStaCache[i].addr,
                peerMacAddr,
                sizeof(tSirMacAddr));

  psessionEntry->protStaCache[i].protStaCacheType = protStaCacheType;
  psessionEntry->protStaCache[i].active = true;
  if(eLIM_PROT_STA_CACHE_TYPE_llB == protStaCacheType)
  {
      psessionEntry->gLim11bParams.numSta++;
      limLog(pMac,LOG1, FL("11B, "));
  }
  else if(eLIM_PROT_STA_CACHE_TYPE_llG == protStaCacheType)
  {
      psessionEntry->gLim11gParams.numSta++;
      limLog(pMac,LOG1, FL("11G, "));
  }
  else   if(eLIM_PROT_STA_CACHE_TYPE_HT20 == protStaCacheType)
  {
      psessionEntry->gLimHt20Params.numSta++;
      limLog(pMac,LOG1, FL("HT20, "));
  }

  if(!gfSupported)
  {
     psessionEntry->gLimNonGfParams.numSta++;
      limLog(pMac,LOG1, FL("NonGf, "));
  }
  if(!lsigTxopSupported)
  {
      psessionEntry->gLimLsigTxopParams.numSta++;
      limLog(pMac,LOG1, FL("!lsigTxopSupported"));
  }
}//                                                                      

/*                                                               
                         
                                                                           
                                                                                  
                                                                            
                                  
                                   
                 
                                                               */
void
limDecideApProtection(tpAniSirGlobal pMac, tSirMacAddr peerMacAddr, tpUpdateBeaconParams pBeaconParams,tpPESession psessionEntry)
{
    tANI_U16              tmpAid;
    tpDphHashNode    pStaDs;
    tSirRFBand  rfBand = SIR_BAND_UNKNOWN;
    tANI_U32 phyMode;
    tLimProtStaCacheType protStaCacheType = eLIM_PROT_STA_CACHE_TYPE_INVALID;
    tHalBitVal gfSupported = eHAL_SET, lsigTxopSupported = eHAL_SET;

    pBeaconParams->paramChangeBitmap = 0;
    //                                          
    pStaDs = dphLookupHashEntry(pMac, peerMacAddr, &tmpAid, &psessionEntry->dph.dphHashTable);
    if(NULL == pStaDs)
    {
      PELOG1(limLog(pMac, LOG1, FL("pStaDs is NULL"));)
      return;
    }
    limGetRfBand(pMac, &rfBand, psessionEntry);
    //                       
    if(SIR_BAND_5_GHZ == rfBand)
    {
        //                                                                                              
        //                                                                           
        if(true == psessionEntry->htCapability)
        {
            //                                              
            //                                         
            if(false == pStaDs->mlmStaContext.htCapability)
            {
                limEnable11aProtection(pMac, true, false, pBeaconParams,psessionEntry);
                return;
            }
        }
    }
    else if(SIR_BAND_2_4_GHZ== rfBand)
    {
        limGetPhyMode(pMac, &phyMode, psessionEntry);

        //                                                          
        if ((phyMode == WNI_CFG_PHY_MODE_11G) &&
              (false == psessionEntry->htCapability))
        {

            if (pStaDs->erpEnabled== eHAL_CLEAR)
            {
                protStaCacheType = eLIM_PROT_STA_CACHE_TYPE_llB;
                //                  
                PELOG3(limLog(pMac, LOG3, FL("Enabling protection from 11B"));)
                limEnable11gProtection(pMac, true, false, pBeaconParams,psessionEntry);
            }
        }

        //           
        if (true == psessionEntry->htCapability)
        {
            //                                            
            if ((pStaDs->erpEnabled == eHAL_CLEAR) &&
                (!pStaDs->mlmStaContext.htCapability))
            {
                protStaCacheType = eLIM_PROT_STA_CACHE_TYPE_llB;
                //                  
                PELOG3(limLog(pMac, LOG3, FL("Enabling protection from 11B"));)
                limEnable11gProtection(pMac, true, false, pBeaconParams, psessionEntry);
            }
            //                                                         
            else if(!pStaDs->mlmStaContext.htCapability)
            {
                protStaCacheType = eLIM_PROT_STA_CACHE_TYPE_llG;
                //                 
                limEnableHtProtectionFrom11g(pMac, true, false, pBeaconParams, psessionEntry);
            }
            //                                                 
            //                                   
            //                                                                               
        }
    }

    //                                                                            
    if((true == psessionEntry->htCapability) &&
        (true == pStaDs->mlmStaContext.htCapability))
    {
        if(!pStaDs->htGreenfield)
        {
          limEnableHTNonGfProtection(pMac, true, false, pBeaconParams, psessionEntry);
          gfSupported = eHAL_CLEAR;
        }
        //                           
        if(eHT_CHANNEL_WIDTH_20MHZ == pStaDs->htSupportedChannelWidthSet)
        {
            protStaCacheType = eLIM_PROT_STA_CACHE_TYPE_HT20;
            limEnableHT20Protection(pMac, true, false, pBeaconParams, psessionEntry);
        }
        //                                                     
        if(!pStaDs->htLsigTXOPProtection)
        {
            limEnableHTLsigTxopProtection(pMac, false, false, pBeaconParams,psessionEntry);
            lsigTxopSupported = eHAL_CLEAR;
        }
    }

    limUpdateProtStaParams(pMac, peerMacAddr, protStaCacheType,
              gfSupported, lsigTxopSupported, psessionEntry);

    return;
}


/*                                                               
                                 
                                                           
                                  
                                              
                                       
                 
                                                               */
void
limEnableOverlap11gProtection(tpAniSirGlobal pMac,
tpUpdateBeaconParams pBeaconParams, tpSirMacMgmtHdr pMh,tpPESession psessionEntry)
{
    limUpdateOverlapStaParam(pMac, pMh->bssId, &(psessionEntry->gLimOlbcParams));

    if (psessionEntry->gLimOlbcParams.numSta &&
        !psessionEntry->gLimOlbcParams.protectionEnabled)
    {
        //                  
        PELOG1(limLog(pMac, LOG1, FL("OLBC happens!!!"));)
        limEnable11gProtection(pMac, true, true, pBeaconParams,psessionEntry);
    }
}


/*                                                               
                          
                                                                 
                                  
                                   
                                              
                 
                                                               */
void
limUpdateShortPreamble(tpAniSirGlobal pMac, tSirMacAddr peerMacAddr,
    tpUpdateBeaconParams pBeaconParams, tpPESession psessionEntry)
{
    tANI_U16         tmpAid;
    tpDphHashNode    pStaDs;
    tANI_U32         phyMode;
    tANI_U16         i;

    //                                          
    pStaDs = dphLookupHashEntry(pMac, peerMacAddr, &tmpAid, &psessionEntry->dph.dphHashTable);

    limGetPhyMode(pMac, &phyMode, psessionEntry);

    if (pStaDs != NULL && phyMode == WNI_CFG_PHY_MODE_11G)

    {        
        if (pStaDs->shortPreambleEnabled == eHAL_CLEAR)
        {
            PELOG1(limLog(pMac,LOG1,FL("Short Preamble is not enabled in Assoc Req from "));
                    limPrintMacAddr(pMac, peerMacAddr, LOG1);)

                for (i=0; i<LIM_PROT_STA_CACHE_SIZE; i++)
                {
                if ((psessionEntry->limSystemRole == eLIM_AP_ROLE )  &&
                     psessionEntry->gLimNoShortParams.staNoShortCache[i].active)
                    {
                        if (vos_mem_compare(
                                    psessionEntry->gLimNoShortParams.staNoShortCache[i].addr,
                                    peerMacAddr, sizeof(tSirMacAddr)))
                            return;
                }else if(psessionEntry->limSystemRole != eLIM_AP_ROLE)
                {
                    if (pMac->lim.gLimNoShortParams.staNoShortCache[i].active)
                     {
                         if (vos_mem_compare(
                                             pMac->lim.gLimNoShortParams.staNoShortCache[i].addr,
                                             peerMacAddr, sizeof(tSirMacAddr)))
                             return;
                      }
                    }
                }


            for (i=0; i<LIM_PROT_STA_CACHE_SIZE; i++)
            {
                if ( (psessionEntry->limSystemRole == eLIM_AP_ROLE )  &&
                      !psessionEntry->gLimNoShortParams.staNoShortCache[i].active)
                     break;
                else        
                {
                    if (!pMac->lim.gLimNoShortParams.staNoShortCache[i].active)
                    break;
                }
            }

            if (i >= LIM_PROT_STA_CACHE_SIZE)
            {
                if(psessionEntry->limSystemRole == eLIM_AP_ROLE){
                    limLog(pMac, LOGE, FL("No space in Short cache (#active %d, #sta %d) for sta "),
                            i, psessionEntry->gLimNoShortParams.numNonShortPreambleSta);
                    limPrintMacAddr(pMac, peerMacAddr, LOGE);
                    return;
                }
                else
                {
                    limLog(pMac, LOGE, FL("No space in Short cache (#active %d, #sta %d) for sta "),
                            i, pMac->lim.gLimNoShortParams.numNonShortPreambleSta);
                    limPrintMacAddr(pMac, peerMacAddr, LOGE);
                    return;
                }

            }


            if (psessionEntry->limSystemRole == eLIM_AP_ROLE){
                vos_mem_copy( psessionEntry->gLimNoShortParams.staNoShortCache[i].addr,
                        peerMacAddr,  sizeof(tSirMacAddr));
                psessionEntry->gLimNoShortParams.staNoShortCache[i].active = true;
                psessionEntry->gLimNoShortParams.numNonShortPreambleSta++;
            }else
            {
                vos_mem_copy(  pMac->lim.gLimNoShortParams.staNoShortCache[i].addr,
                               peerMacAddr,  sizeof(tSirMacAddr));
                pMac->lim.gLimNoShortParams.staNoShortCache[i].active = true;
                pMac->lim.gLimNoShortParams.numNonShortPreambleSta++;        
            } 


            //                     
            PELOG1(limLog(pMac, LOG1, FL("Disabling short preamble"));)

            if (limEnableShortPreamble(pMac, false, pBeaconParams, psessionEntry) != eSIR_SUCCESS)
                PELOGE(limLog(pMac, LOGE, FL("Cannot enable long preamble"));)
        }
    }
}

/*                                                               
                          
                                                                  
                                  
                                   
                                              
                 
                                                               */

void
limUpdateShortSlotTime(tpAniSirGlobal pMac, tSirMacAddr peerMacAddr,
    tpUpdateBeaconParams pBeaconParams, tpPESession psessionEntry)
{
    tANI_U16              tmpAid;
    tpDphHashNode    pStaDs;
    tANI_U32 phyMode;
    tANI_U32 val;
    tANI_U16 i;

    //                                          
    pStaDs = dphLookupHashEntry(pMac, peerMacAddr, &tmpAid, &psessionEntry->dph.dphHashTable);
    limGetPhyMode(pMac, &phyMode, psessionEntry);

    /*                                                                                                                    
                                                         
     */
    if (pStaDs != NULL && phyMode == WNI_CFG_PHY_MODE_11G)
    {
        /*                                                                                                          
                                                                                                                               
                                   
         */
        if (pStaDs->shortSlotTimeEnabled == eHAL_CLEAR)
        {
            PELOG1(limLog(pMac, LOG1, FL("Short Slot Time is not enabled in Assoc Req from "));
            limPrintMacAddr(pMac, peerMacAddr, LOG1);)
            for (i=0; i<LIM_PROT_STA_CACHE_SIZE; i++)
            {
                if ((psessionEntry->limSystemRole == eLIM_AP_ROLE ) &&
                     psessionEntry->gLimNoShortSlotParams.staNoShortSlotCache[i].active)
                {
                    if (vos_mem_compare(
                         psessionEntry->gLimNoShortSlotParams.staNoShortSlotCache[i].addr,
                         peerMacAddr, sizeof(tSirMacAddr)))
                        return;
                }
                else if(psessionEntry->limSystemRole != eLIM_AP_ROLE )
                {
                    if (pMac->lim.gLimNoShortSlotParams.staNoShortSlotCache[i].active)
                    {
                        if (vos_mem_compare(
                            pMac->lim.gLimNoShortSlotParams.staNoShortSlotCache[i].addr,
                            peerMacAddr, sizeof(tSirMacAddr)))
                            return;
                     }
                }
            }

            for (i=0; i<LIM_PROT_STA_CACHE_SIZE; i++)
            {
                if ((psessionEntry->limSystemRole == eLIM_AP_ROLE ) &&
                     !psessionEntry->gLimNoShortSlotParams.staNoShortSlotCache[i].active)
                    break;
                 else
                 {
                     if (!pMac->lim.gLimNoShortSlotParams.staNoShortSlotCache[i].active)
                          break;
                 }
            }

            if (i >= LIM_PROT_STA_CACHE_SIZE)
            {
                if(psessionEntry->limSystemRole == eLIM_AP_ROLE){
                    limLog(pMac, LOGE, FL("No space in ShortSlot cache (#active %d, #sta %d) for sta "),
                            i, psessionEntry->gLimNoShortSlotParams.numNonShortSlotSta);
                    limPrintMacAddr(pMac, peerMacAddr, LOGE);
                    return;
                }else
                {
                    limLog(pMac, LOGE, FL("No space in ShortSlot cache (#active %d, #sta %d) for sta "),
                           i, pMac->lim.gLimNoShortSlotParams.numNonShortSlotSta);
                    limPrintMacAddr(pMac, peerMacAddr, LOGE);
                    return;
                }
            }


            if(psessionEntry->limSystemRole == eLIM_AP_ROLE){
                vos_mem_copy(  psessionEntry->gLimNoShortSlotParams.staNoShortSlotCache[i].addr,
                               peerMacAddr, sizeof(tSirMacAddr));
                psessionEntry->gLimNoShortSlotParams.staNoShortSlotCache[i].active = true;
                psessionEntry->gLimNoShortSlotParams.numNonShortSlotSta++;
            }else
            {
                vos_mem_copy( pMac->lim.gLimNoShortSlotParams.staNoShortSlotCache[i].addr,
                          peerMacAddr, sizeof(tSirMacAddr));
                pMac->lim.gLimNoShortSlotParams.staNoShortSlotCache[i].active = true;
                pMac->lim.gLimNoShortSlotParams.numNonShortSlotSta++;
            }
            wlan_cfgGetInt(pMac, WNI_CFG_11G_SHORT_SLOT_TIME_ENABLED, &val);

            /*                                                                                                                              
                                                                                                           
             */
            if ( (psessionEntry->limSystemRole == eLIM_AP_ROLE) && 
                 (val && psessionEntry->gLimNoShortSlotParams.numNonShortSlotSta && psessionEntry->shortSlotTimeSupported))
            {
                //                      
                pBeaconParams->fShortSlotTime = false;
                pBeaconParams->paramChangeBitmap |= PARAM_SHORT_SLOT_TIME_CHANGED;
                PELOG1(limLog(pMac, LOG1, FL("Disable short slot time. Enable long slot time."));)
                psessionEntry->shortSlotTimeSupported = false;
            }
            else if ( psessionEntry->limSystemRole != eLIM_AP_ROLE)
            {
                if (val && pMac->lim.gLimNoShortSlotParams.numNonShortSlotSta && psessionEntry->shortSlotTimeSupported)
                {
                    //                      
                    pBeaconParams->fShortSlotTime = false;
                    pBeaconParams->paramChangeBitmap |= PARAM_SHORT_SLOT_TIME_CHANGED;
                    PELOG1(limLog(pMac, LOG1, FL("Disable short slot time. Enable long slot time."));)
                    psessionEntry->shortSlotTimeSupported = false;
                 }
            }
        }
    }
}


/*                                                               
                                 
                                                                   
                                  
                                           
                 
                                                               */
void
limDecideStaProtectionOnAssoc(tpAniSirGlobal pMac,
    tpSchBeaconStruct pBeaconStruct, tpPESession psessionEntry)
{
    tSirRFBand rfBand = SIR_BAND_UNKNOWN;
    tANI_U32 phyMode = WNI_CFG_PHY_MODE_NONE;

    limGetRfBand(pMac, &rfBand, psessionEntry);
    limGetPhyMode(pMac, &phyMode, psessionEntry);

    if(SIR_BAND_5_GHZ == rfBand)
    {
        if((eSIR_HT_OP_MODE_MIXED == pBeaconStruct->HTInfo.opMode)  ||
                    (eSIR_HT_OP_MODE_OVERLAP_LEGACY == pBeaconStruct->HTInfo.opMode))
        {
            if(pMac->lim.cfgProtection.fromlla)
                psessionEntry->beaconParams.llaCoexist = true;
        }
        else if(eSIR_HT_OP_MODE_NO_LEGACY_20MHZ_HT == pBeaconStruct->HTInfo.opMode)
        {
            if(pMac->lim.cfgProtection.ht20)
                psessionEntry->beaconParams.ht20Coexist = true;
        }

    }
    else if(SIR_BAND_2_4_GHZ == rfBand)
    {
        //             
        //                                                        
        //                                   
        //                             
        //                                      
        //                                                                               

        //                                      
        //                     
         /*                               */
        if (phyMode != WNI_CFG_PHY_MODE_11B) 
        {
            if (pMac->lim.cfgProtection.fromllb &&
                pBeaconStruct->erpPresent &&
                (pBeaconStruct->erpIEInfo.useProtection ||
                pBeaconStruct->erpIEInfo.nonErpPresent))
            {
                psessionEntry->beaconParams.llbCoexist = true;
            }
            //                                 
            else
            {
                psessionEntry->beaconParams.llbCoexist = false;
            }
        }
        //                                            
        if((psessionEntry->htCapability) &&
              (pBeaconStruct->HTInfo.present))
        {
            tDot11fIEHTInfo htInfo = pBeaconStruct->HTInfo;
           
            //                             
            psessionEntry->beaconParams.gHTObssMode =  (tANI_U8)htInfo.obssNonHTStaPresent;

            
          //                                      
            //                                                      
            if(pMac->lim.cfgProtection.fromllg &&
                  ((eSIR_HT_OP_MODE_MIXED == htInfo.opMode)  ||
                        (eSIR_HT_OP_MODE_OVERLAP_LEGACY == htInfo.opMode))&&
                      (!psessionEntry->beaconParams.llbCoexist))
            {
                if(pMac->lim.cfgProtection.fromllg)
                    psessionEntry->beaconParams.llgCoexist = true;
            }

            //                                                                    
            //                                           
            //                                                                        
            if(eSIR_HT_OP_MODE_NO_LEGACY_20MHZ_HT == htInfo.opMode)
            {
                //                                    
                psessionEntry->beaconParams.llgCoexist = false;
          //                                     
          if(pMac->lim.cfgProtection.ht20)
                psessionEntry->beaconParams.ht20Coexist = true;
            }
            //                                                
            //                                                                        
            if(eSIR_HT_OP_MODE_PURE == htInfo.opMode)
            {
                psessionEntry->beaconParams.llgCoexist = false;
                psessionEntry->beaconParams.ht20Coexist = false;
            }

        }
    }

    //                                                                                             
    if((psessionEntry->htCapability) &&
          (pBeaconStruct->HTInfo.present))
    {
        tDot11fIEHTInfo htInfo = pBeaconStruct->HTInfo;
        psessionEntry->beaconParams.fRIFSMode = 
            ( tANI_U8 ) htInfo.rifsMode;
        psessionEntry->beaconParams.llnNonGFCoexist = 
            ( tANI_U8 )htInfo.nonGFDevicesPresent;
        psessionEntry->beaconParams.fLsigTXOPProtectionFullSupport = 
            ( tANI_U8 )htInfo.lsigTXOPProtectionFullSupport;
    }
}


/*                                                               
                          
                                                                          
                                  
                                              
                 
                                                               */
void
limDecideStaProtection(tpAniSirGlobal pMac,
    tpSchBeaconStruct pBeaconStruct, tpUpdateBeaconParams pBeaconParams, tpPESession psessionEntry)
{

    tSirRFBand rfBand = SIR_BAND_UNKNOWN;
    tANI_U32 phyMode = WNI_CFG_PHY_MODE_NONE;

    limGetRfBand(pMac, &rfBand, psessionEntry);
    limGetPhyMode(pMac, &phyMode, psessionEntry);
       
    if(SIR_BAND_5_GHZ == rfBand)
    {
        //                  
        if((true == psessionEntry->htCapability) &&
            (pBeaconStruct->HTInfo.present))
        {
            //                                                                                                 
            if((eSIR_HT_OP_MODE_MIXED == pBeaconStruct->HTInfo.opMode) ||
              (eSIR_HT_OP_MODE_OVERLAP_LEGACY == pBeaconStruct->HTInfo.opMode))
            {
                limEnable11aProtection(pMac, true, false, pBeaconParams,psessionEntry);
            }
            //                                                                                              
            //                               
            else if(eSIR_HT_OP_MODE_NO_LEGACY_20MHZ_HT== pBeaconStruct->HTInfo.opMode)
            {
                limEnable11aProtection(pMac, false, false, pBeaconParams,psessionEntry);            
                limEnableHT20Protection(pMac, true, false, pBeaconParams,psessionEntry);
            }
            else if(eSIR_HT_OP_MODE_PURE == pBeaconStruct->HTInfo.opMode)
            {
                limEnable11aProtection(pMac, false, false, pBeaconParams,psessionEntry);            
                limEnableHT20Protection(pMac, false, false, pBeaconParams,psessionEntry);
            }
        }
    }
    else if(SIR_BAND_2_4_GHZ == rfBand)
    {
        /*              
                                                                   
                                              
                                        
                                                 
                                                                                          
         */

        if (phyMode != WNI_CFG_PHY_MODE_11B) 
        {
            if (pBeaconStruct->erpPresent &&
                  (pBeaconStruct->erpIEInfo.useProtection ||
                  pBeaconStruct->erpIEInfo.nonErpPresent))
            {
                limEnable11gProtection(pMac, true, false, pBeaconParams, psessionEntry);
            }
            //                                 
            else
            {
                //                                   
                limEnable11gProtection(pMac, false, false, pBeaconParams, psessionEntry);
            }
         }

        //                                            
        if((psessionEntry->htCapability) &&
              (pBeaconStruct->HTInfo.present))
        {
          
            tDot11fIEHTInfo htInfo = pBeaconStruct->HTInfo;
            //                                           
            if(((eSIR_HT_OP_MODE_MIXED == htInfo.opMode)  ||
                  (eSIR_HT_OP_MODE_OVERLAP_LEGACY == htInfo.opMode))&&
                (!psessionEntry->beaconParams.llbCoexist))
            {
                limEnableHtProtectionFrom11g(pMac, true, false, pBeaconParams,psessionEntry);
        
            }

            //                                                                                                    
            //                                                                                                              
            //                                                                                  
            if ( pMac->lim.gHTOperMode != ( tSirMacHTOperatingMode )htInfo.opMode )
            {
                pMac->lim.gHTOperMode       = ( tSirMacHTOperatingMode )htInfo.opMode;

                 //                                                                    
                 //                                           
                 //                                                                        
                if(eSIR_HT_OP_MODE_NO_LEGACY_20MHZ_HT == htInfo.opMode)
                {
                    //                                    
                    limEnableHtProtectionFrom11g(pMac, false, false, pBeaconParams,psessionEntry);
        
                    limEnableHT20Protection(pMac, true, false, pBeaconParams,psessionEntry);
                }
                //                                                
                //                                                                        
                else if(eSIR_HT_OP_MODE_PURE == htInfo.opMode)
                {
                    limEnableHtProtectionFrom11g(pMac, false, false, pBeaconParams,psessionEntry);
                    limEnableHT20Protection(pMac, false, false, pBeaconParams,psessionEntry);
            
                }
            }
        }
    }

    //                                                                        
    if((psessionEntry->htCapability) &&
          (pBeaconStruct->HTInfo.present))
    {
        tDot11fIEHTInfo htInfo = pBeaconStruct->HTInfo;    
        //                                                                             
        //                                                                                   
        if ( psessionEntry->beaconParams.fRIFSMode != 
                ( tANI_U8 ) htInfo.rifsMode )
        {
            pBeaconParams->fRIFSMode = 
                psessionEntry->beaconParams.fRIFSMode  = 
                ( tANI_U8 ) htInfo.rifsMode;
            pBeaconParams->paramChangeBitmap |= PARAM_RIFS_MODE_CHANGED;
        }

        if ( psessionEntry->beaconParams.llnNonGFCoexist != 
                htInfo.nonGFDevicesPresent )
        {
            pBeaconParams->llnNonGFCoexist = 
                psessionEntry->beaconParams.llnNonGFCoexist = 
                ( tANI_U8 )htInfo.nonGFDevicesPresent;
            pBeaconParams->paramChangeBitmap |= 
                PARAM_NON_GF_DEVICES_PRESENT_CHANGED;
        }

        if ( psessionEntry->beaconParams.fLsigTXOPProtectionFullSupport != 
                ( tANI_U8 )htInfo.lsigTXOPProtectionFullSupport )
        {
            pBeaconParams->fLsigTXOPProtectionFullSupport =  
                psessionEntry->beaconParams.fLsigTXOPProtectionFullSupport = 
                ( tANI_U8 )htInfo.lsigTXOPProtectionFullSupport;
            pBeaconParams->paramChangeBitmap |= 
                PARAM_LSIG_TXOP_FULL_SUPPORT_CHANGED;
        }
        
    //                                                                                
    //                                                                           
    //                            
       if ( psessionEntry->beaconParams.gHTObssMode != ( tANI_U8 )htInfo.obssNonHTStaPresent )
            psessionEntry->beaconParams.gHTObssMode = ( tANI_U8 )htInfo.obssNonHTStaPresent ;
            
    }
}


/* 
                                   
  
           
                                                                
                                                                
                                
  
  
       
                                                           
               
 */
void limProcessChannelSwitchTimeout(tpAniSirGlobal pMac)
{
    tpPESession psessionEntry = NULL;
    tANI_U8    channel; //                                                            
   
    if((psessionEntry = peFindSessionBySessionId(pMac, pMac->lim.limTimers.gLimChannelSwitchTimer.sessionId))== NULL) 
    {
        limLog(pMac, LOGP,FL("Session Does not exist for given sessionID"));
        return;
    }

    if (psessionEntry->limSystemRole != eLIM_STA_ROLE)
    {
        PELOGW(limLog(pMac, LOGW, "Channel switch can be done only in STA role, Current Role = %d", psessionEntry->limSystemRole);)
        return;
    }
    channel = psessionEntry->gLimChannelSwitch.primaryChannel;
    /*
                                                                       
                                                                          
                                                      
     */
    if(!limIsSystemInActiveState(pMac))
    {
        PELOGW(limLog(pMac, LOGW, FL("Device is not in active state, cannot switch channel"));)
        return;
    }
         
    //                                             
    psessionEntry->gLimChannelSwitch.switchTimeoutValue = 0;

    /*                                                      */
    psessionEntry->gLimSpecMgmt.dot11hChanSwState = eLIM_11H_CHANSW_END;
    
    /*                                                           
                                                                     
                             
     */
    if(!limIsChannelValidForChannelSwitch(pMac, channel))
    {
        /*                                                       */
        if(limRestorePreChannelSwitchState(pMac, psessionEntry) != eSIR_SUCCESS)
        {
            limLog(pMac, LOGP, FL("Could not restore pre-channelSwitch (11h) state, resetting the system"));
            return;
        }

        /*                                                               
                                                                         
                                       
         */
        limTearDownLinkWithAp(pMac, 
                        pMac->lim.limTimers.gLimChannelSwitchTimer.sessionId,
                        eSIR_MAC_UNSPEC_FAILURE_REASON);
        return;
    }
    limCovertChannelScanType(pMac, psessionEntry->currentOperChannel, false);
    pMac->lim.dfschannelList.timeStamp[psessionEntry->currentOperChannel] = 0;
    switch(psessionEntry->gLimChannelSwitch.state)
    {
        case eLIM_CHANNEL_SWITCH_PRIMARY_ONLY:
            PELOGW(limLog(pMac, LOGW, FL("CHANNEL_SWITCH_PRIMARY_ONLY "));)
            limSwitchPrimaryChannel(pMac, psessionEntry->gLimChannelSwitch.primaryChannel,psessionEntry);
            psessionEntry->gLimChannelSwitch.state = eLIM_CHANNEL_SWITCH_IDLE;
            break;

        case eLIM_CHANNEL_SWITCH_SECONDARY_ONLY:
            PELOGW(limLog(pMac, LOGW, FL("CHANNEL_SWITCH_SECONDARY_ONLY "));)
            limSwitchPrimarySecondaryChannel(pMac, psessionEntry,
                                             psessionEntry->currentOperChannel,
                                             psessionEntry->gLimChannelSwitch.secondarySubBand);
            psessionEntry->gLimChannelSwitch.state = eLIM_CHANNEL_SWITCH_IDLE;
            break;

        case eLIM_CHANNEL_SWITCH_PRIMARY_AND_SECONDARY:
            PELOGW(limLog(pMac, LOGW, FL("CHANNEL_SWITCH_PRIMARY_AND_SECONDARY"));)
            limSwitchPrimarySecondaryChannel(pMac, psessionEntry,
                                             psessionEntry->gLimChannelSwitch.primaryChannel,
                                             psessionEntry->gLimChannelSwitch.secondarySubBand);
            psessionEntry->gLimChannelSwitch.state = eLIM_CHANNEL_SWITCH_IDLE;
            break;

        case eLIM_CHANNEL_SWITCH_IDLE:
        default:
            PELOGE(limLog(pMac, LOGE, FL("incorrect state "));)
            if(limRestorePreChannelSwitchState(pMac, psessionEntry) != eSIR_SUCCESS)
            {
                limLog(pMac, LOGP, FL("Could not restore pre-channelSwitch (11h) state, resetting the system"));
            }
            return;  /*                                               */
    }
}

/* 
                           
  
           
                                                     
                                                        
                     
  
       
                                                           
                                                              
                       
 */
void
limUpdateChannelSwitch(struct sAniSirGlobal *pMac,  tpSirProbeRespBeacon pBeacon, tpPESession psessionEntry)
{

    tANI_U16                         beaconPeriod;
    tChannelSwitchPropIEStruct       *pPropChnlSwitch;
    tDot11fIEChanSwitchAnn           *pChnlSwitch;
#ifdef WLAN_FEATURE_11AC
    tDot11fIEWiderBWChanSwitchAnn    *pWiderChnlSwitch;
#endif

    beaconPeriod = psessionEntry->beaconParams.beaconInterval;

    /*                                                             
                                  
     */
    if (pBeacon->propIEinfo.propChannelSwitchPresent)
    {
        pPropChnlSwitch = &(pBeacon->propIEinfo.channelSwitch);

        /*                                               */
        /*                                                      */
        psessionEntry->gLimChannelSwitch.state = eLIM_CHANNEL_SWITCH_PRIMARY_AND_SECONDARY;
        psessionEntry->gLimChannelSwitch.primaryChannel = pPropChnlSwitch->primaryChannel;
        psessionEntry->gLimChannelSwitch.secondarySubBand = (ePhyChanBondState)pPropChnlSwitch->subBand;
        psessionEntry->gLimChannelSwitch.switchCount = pPropChnlSwitch->channelSwitchCount;
        psessionEntry->gLimChannelSwitch.switchTimeoutValue =
                 SYS_MS_TO_TICKS(beaconPeriod)* (pPropChnlSwitch->channelSwitchCount);
        psessionEntry->gLimChannelSwitch.switchMode = pPropChnlSwitch->mode;
    }
    else
    {
       pChnlSwitch = &(pBeacon->channelSwitchIE);
       psessionEntry->gLimChannelSwitch.primaryChannel = pChnlSwitch->newChannel;
       psessionEntry->gLimChannelSwitch.switchCount = pChnlSwitch->switchCount;
       psessionEntry->gLimChannelSwitch.switchTimeoutValue =
                 SYS_MS_TO_TICKS(beaconPeriod)* (pChnlSwitch->switchCount);
       psessionEntry->gLimChannelSwitch.switchMode = pChnlSwitch->switchMode; 
#ifdef WLAN_FEATURE_11AC
       pWiderChnlSwitch = &(pBeacon->WiderBWChanSwitchAnn);
       if(pBeacon->WiderBWChanSwitchAnnPresent) 
       {
           psessionEntry->gLimWiderBWChannelSwitch.newChanWidth = pWiderChnlSwitch->newChanWidth;
           psessionEntry->gLimWiderBWChannelSwitch.newCenterChanFreq0 = pWiderChnlSwitch->newCenterChanFreq0;
           psessionEntry->gLimWiderBWChannelSwitch.newCenterChanFreq1 = pWiderChnlSwitch->newCenterChanFreq1;
       }
#endif

        /*                                                */
        psessionEntry->gLimChannelSwitch.state = eLIM_CHANNEL_SWITCH_PRIMARY_ONLY;
        psessionEntry->gLimChannelSwitch.secondarySubBand = PHY_SINGLE_CHANNEL_CENTERED;

        /*                                                                     
                                                          
         */
        if (psessionEntry->htSupportedChannelWidthSet)
        {
            if (pBeacon->extChannelSwitchPresent)
            {
                if ((pBeacon->extChannelSwitchIE.secondaryChannelOffset == PHY_DOUBLE_CHANNEL_LOW_PRIMARY) || 
                    (pBeacon->extChannelSwitchIE.secondaryChannelOffset == PHY_DOUBLE_CHANNEL_HIGH_PRIMARY))
                {
                    psessionEntry->gLimChannelSwitch.state = eLIM_CHANNEL_SWITCH_PRIMARY_AND_SECONDARY;
                    psessionEntry->gLimChannelSwitch.secondarySubBand = pBeacon->extChannelSwitchIE.secondaryChannelOffset;
                }
#ifdef WLAN_FEATURE_11AC
                if(psessionEntry->vhtCapability && pBeacon->WiderBWChanSwitchAnnPresent)
                {
                    if (pWiderChnlSwitch->newChanWidth == WNI_CFG_VHT_CHANNEL_WIDTH_80MHZ)
                    {
                        if(pBeacon->extChannelSwitchPresent)
                        {
                            if ((pBeacon->extChannelSwitchIE.secondaryChannelOffset == PHY_DOUBLE_CHANNEL_LOW_PRIMARY) ||
                                (pBeacon->extChannelSwitchIE.secondaryChannelOffset == PHY_DOUBLE_CHANNEL_HIGH_PRIMARY))
                            {
                                psessionEntry->gLimChannelSwitch.state = eLIM_CHANNEL_SWITCH_PRIMARY_AND_SECONDARY;
                                psessionEntry->gLimChannelSwitch.secondarySubBand = limGet11ACPhyCBState(pMac, 
                                                                                                         psessionEntry->gLimChannelSwitch.primaryChannel,
                                                                                                         pBeacon->extChannelSwitchIE.secondaryChannelOffset,
                                                                                                         pWiderChnlSwitch->newCenterChanFreq0,
                                                                                                         psessionEntry);
                            }
                        }
                    }
                }
#endif
            }
        }
     }
    if (eSIR_SUCCESS != limStartChannelSwitch(pMac, psessionEntry))
    {
        PELOGW(limLog(pMac, LOGW, FL("Could not start Channel Switch"));)
    }

    limLog(pMac, LOGW,
        FL("session %d primary chl %d, subband %d, count  %d (%d ticks) "),
        psessionEntry->peSessionId,
        psessionEntry->gLimChannelSwitch.primaryChannel,
        psessionEntry->gLimChannelSwitch.secondarySubBand,
        psessionEntry->gLimChannelSwitch.switchCount,
        psessionEntry->gLimChannelSwitch.switchTimeoutValue);
    return;
}

/* 
                               
  
           
                                                                          
                                                                            
                                  
   
        
  
              
  
       
  
                                                    
  
               
 */
void limCancelDot11hChannelSwitch(tpAniSirGlobal pMac, tpPESession psessionEntry)
{
    if (psessionEntry->limSystemRole != eLIM_STA_ROLE)
        return;
        
    PELOGW(limLog(pMac, LOGW, FL("Received a beacon without channel switch IE"));)
    MTRACE(macTrace(pMac, TRACE_CODE_TIMER_DEACTIVATE, psessionEntry->peSessionId, eLIM_CHANNEL_SWITCH_TIMER));

    if (tx_timer_deactivate(&pMac->lim.limTimers.gLimChannelSwitchTimer) != eSIR_SUCCESS)
    {
        PELOGE(limLog(pMac, LOGE, FL("tx_timer_deactivate failed!"));)
    }

    /*                                                       */
    if (limRestorePreChannelSwitchState(pMac, psessionEntry) != eSIR_SUCCESS)
    {
        PELOGE(limLog(pMac, LOGE, FL("LIM: Could not restore pre-channelSwitch (11h) state, resetting the system"));)
                
    }
}

/*                                               
                            
                                                
                                              

            
            
                                               */
void limCancelDot11hQuiet(tpAniSirGlobal pMac, tpPESession psessionEntry)
{
    if (psessionEntry->limSystemRole != eLIM_STA_ROLE)
        return;

    if (psessionEntry->gLimSpecMgmt.quietState == eLIM_QUIET_BEGIN) 
    {
         MTRACE(macTrace(pMac, TRACE_CODE_TIMER_DEACTIVATE, psessionEntry->peSessionId, eLIM_QUIET_TIMER));
        if (tx_timer_deactivate(&pMac->lim.limTimers.gLimQuietTimer) != TX_SUCCESS)
        {
            PELOGE(limLog(pMac, LOGE, FL("tx_timer_deactivate failed"));)
        }
    }
    else if (psessionEntry->gLimSpecMgmt.quietState == eLIM_QUIET_RUNNING)
    {
        MTRACE(macTrace(pMac, TRACE_CODE_TIMER_DEACTIVATE, psessionEntry->peSessionId, eLIM_QUIET_BSS_TIMER));
        if (tx_timer_deactivate(&pMac->lim.limTimers.gLimQuietBssTimer) != TX_SUCCESS)
        {
            PELOGE(limLog(pMac, LOGE, FL("tx_timer_deactivate failed"));)
        }
        /* 
                                                                                   
                                                                                         
         */
        if(!((psessionEntry->gLimSpecMgmt.dot11hChanSwState == eLIM_11H_CHANSW_RUNNING) &&
                (psessionEntry->gLimChannelSwitch.switchMode == eSIR_CHANSW_MODE_SILENT)))
        {
            limFrameTransmissionControl(pMac, eLIM_TX_ALL, eLIM_RESUME_TX);
            limRestorePreQuietState(pMac, psessionEntry);
        }
    }
    psessionEntry->gLimSpecMgmt.quietState = eLIM_QUIET_INIT;
}

/* 
                         
  
            
                                           
                                
  
         
                                                      
  
                                            
                                                 
                                                
                                                     
                                           
            
                                      
                                                 
  
               
                            
                                     
  
        
  
                                                 
  
               
 */
void limProcessQuietTimeout(tpAniSirGlobal pMac)
{
    //                                             
    //                 
    tpPESession psessionEntry;

    if((psessionEntry = peFindSessionBySessionId(pMac, pMac->lim.limTimers.gLimQuietTimer.sessionId))== NULL) 
    {
        limLog(pMac, LOGE,FL("Session Does not exist for given sessionID"));
        return;
    }

  PELOG1(limLog(pMac, LOG1, FL("quietState = %d"), psessionEntry->gLimSpecMgmt.quietState);)
  switch( psessionEntry->gLimSpecMgmt.quietState )
  {
    case eLIM_QUIET_BEGIN:
      //                                            
      //                                                        
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
            psessionEntry->gLimSpecMgmt.quietDuration,
            0))
      {
          limLog( pMac, LOGE,
            FL("Unable to change gLimQuietBssTimer! Will still attempt to activate anyway..."));
      }
      MTRACE(macTrace(pMac, TRACE_CODE_TIMER_ACTIVATE, pMac->lim.limTimers.gLimQuietTimer.sessionId, eLIM_QUIET_BSS_TIMER));
#ifdef GEN6_TODO
        /*                                                                     
                          
         */
        pMac->lim.limTimers.gLimQuietBssTimer.sessionId = sessionId;
#endif              
      if( TX_SUCCESS !=
          tx_timer_activate( &pMac->lim.limTimers.gLimQuietBssTimer ))
      {
        limLog( pMac, LOGW,
            FL("Unable to activate gLimQuietBssTimer! The STA will be unable to honor Quiet BSS..."));
      }
      else
      {
        //                                 
        psessionEntry->gLimSpecMgmt.quietState = eLIM_QUIET_RUNNING;

        /*                                                                                     */
        /*                                                            */
        if( pMac->lim.gLimTriggerBackgroundScanDuringQuietBss &&
          (eSIR_TRUE == (glimTriggerBackgroundScanDuringQuietBss_Status = limTriggerBackgroundScanDuringQuietBss( pMac ))) )
        {
           limLog( pMac, LOG2,
               FL("Attempting to trigger a background scan..."));
        }
        else
        {
           //                                              
           /*                         */
           limFrameTransmissionControl(pMac, eLIM_TX_ALL, eLIM_STOP_TX);

           limLog( pMac, LOG2,
                FL("Quiet BSS: STA shutting down for %d ticks"),
                psessionEntry->gLimSpecMgmt.quietDuration );
        }
      }
      break;

    case eLIM_QUIET_RUNNING:
    case eLIM_QUIET_INIT:
    case eLIM_QUIET_END:
    default:
      //
      //                              
      //
      break;
  }
}

/* 
                            
  
            
                                             
                                    
  
         
              
                                                         
                                                    
                
                                                         
               
                                                         
                                                       
              
                                                        
                                                      
                               
                                                       
                                    
  
               
                                           
                                                        
                                                         
                                          
                                              
  
               
  
        
  
                                                 
  
               
 */
void limProcessQuietBssTimeout( tpAniSirGlobal pMac )
{
    tpPESession psessionEntry;

    if((psessionEntry = peFindSessionBySessionId(pMac, pMac->lim.limTimers.gLimQuietBssTimer.sessionId))== NULL) 
    {
        limLog(pMac, LOGP,FL("Session Does not exist for given sessionID"));
        return;
    }

  PELOG1(limLog(pMac, LOG1, FL("quietState = %d"), psessionEntry->gLimSpecMgmt.quietState);)
  if (eLIM_AP_ROLE == psessionEntry->limSystemRole)
  {
  }
  else
  {
    //              
    switch( psessionEntry->gLimSpecMgmt.quietState )
    {
      case eLIM_QUIET_RUNNING:
        //                              
        psessionEntry->gLimSpecMgmt.quietState = eLIM_QUIET_INIT;

        if( !pMac->lim.gLimTriggerBackgroundScanDuringQuietBss || (glimTriggerBackgroundScanDuringQuietBss_Status == eSIR_FALSE) )
        {
          //                                                                          
          if (!((psessionEntry->gLimSpecMgmt.dot11hChanSwState == eLIM_11H_CHANSW_RUNNING) &&
                  (psessionEntry->gLimChannelSwitch.switchMode == eSIR_CHANSW_MODE_SILENT)))
          {
              limFrameTransmissionControl(pMac, eLIM_TX_ALL, eLIM_RESUME_TX);
              limRestorePreQuietState(pMac, psessionEntry);
          }
      
          /*                   */
          if(glimTriggerBackgroundScanDuringQuietBss_Status == eSIR_FALSE)
              glimTriggerBackgroundScanDuringQuietBss_Status = eSIR_TRUE;

          limLog( pMac, LOG2,
              FL("Quiet BSS: Resuming traffic..."));
        }
        else
        {
          //
          //                                         
          //                                            
          //                                               
          //
          limLog( pMac, LOG2,
              FL("Background scan should be complete now..."));
        }
        break;

      case eLIM_QUIET_INIT:
      case eLIM_QUIET_BEGIN:
      case eLIM_QUIET_END:
        PELOG2(limLog(pMac, LOG2, FL("Quiet state not in RUNNING"));)
        /*                                                                   */
        limFrameTransmissionControl(pMac, eLIM_TX_ALL, eLIM_RESUME_TX);
        limRestorePreQuietState(pMac, psessionEntry);
        psessionEntry->gLimSpecMgmt.quietState = eLIM_QUIET_INIT;
        break;

      default:
        //
        //                              
        //
        break;
    }
  }
}
/* 
                              
  
                                                                                         
  
         
  
               
  
        
  
                                                
  
               
 */
#if 0
void limProcessWPSOverlapTimeout(tpAniSirGlobal pMac)
{

    tpPESession psessionEntry;
    tANI_U32 sessionId;
    
    if (tx_timer_activate(&pMac->lim.limTimers.gLimWPSOverlapTimerObj.gLimWPSOverlapTimer) != TX_SUCCESS)
    {
            limLog(pMac, LOGP, FL("tx_timer_activate failed"));
    }

    sessionId = pMac->lim.limTimers.gLimWPSOverlapTimerObj.sessionId;

    PELOGE(limLog(pMac, LOGE, FL("WPS overlap timeout, sessionId=%d"), sessionId);)

    if((psessionEntry = peFindSessionBySessionId(pMac, sessionId)) == NULL) 
    {
        PELOGE(limLog(pMac, LOGP,FL("Session Does not exist for given sessionID"));)
        return;
    }
    
    limWPSPBCTimeout(pMac, psessionEntry);
}
#endif

/*                                               
                             
                                 

           
            
                                               */
void limStartQuietTimer(tpAniSirGlobal pMac, tANI_U8 sessionId)
{
    tpPESession psessionEntry;
    psessionEntry = peFindSessionBySessionId(pMac, sessionId);

    if(psessionEntry == NULL) {
        limLog(pMac, LOGP,FL("Session Does not exist for given sessionID"));
        return;
    }


    if (psessionEntry->limSystemRole != eLIM_STA_ROLE)
        return;
    //                                                
    limCancelDot11hQuiet(pMac, psessionEntry);
    
    MTRACE(macTrace(pMac, TRACE_CODE_TIMER_ACTIVATE, sessionId, eLIM_QUIET_TIMER));
    if( TX_SUCCESS != tx_timer_deactivate(&pMac->lim.limTimers.gLimQuietTimer))
    {
        limLog( pMac, LOGE,
            FL( "Unable to deactivate gLimQuietTimer! Will still attempt to re-activate anyway..." ));
    }

    //                                    
    if( TX_SUCCESS != tx_timer_change( &pMac->lim.limTimers.gLimQuietTimer,
                      SYS_MS_TO_TICKS(psessionEntry->gLimSpecMgmt.quietTimeoutValue), 0))
    {
        limLog( pMac, LOGE,
            FL( "Unable to change gLimQuietTimer! Will still attempt to re-activate anyway..." ));
    }
    
    pMac->lim.limTimers.gLimQuietTimer.sessionId = sessionId;
    if( TX_SUCCESS != tx_timer_activate(&pMac->lim.limTimers.gLimQuietTimer))
    {
        limLog( pMac, LOGE,
            FL("Unable to activate gLimQuietTimer! STA cannot honor Quiet BSS!"));
        limRestorePreQuietState(pMac, psessionEntry);

        psessionEntry->gLimSpecMgmt.quietState = eLIM_QUIET_INIT;
        return;
    }
}


/*                                                                            */
/* 
                                                              
                                                                           
  
                                                       
                                                       
 */
void
limUtilCountStaAdd(
    tpAniSirGlobal pMac,
    tpDphHashNode  pSta,
    tpPESession psessionEntry)
{

    if ((! pSta) || (! pSta->valid) || (! pSta->aniPeer) || (pSta->fAniCount))
        return;

    pSta->fAniCount = 1;

    if (pMac->lim.gLimNumOfAniSTAs++ != 0)
        return;

    //                                                       
    schEdcaProfileUpdate(pMac, psessionEntry);
}

void
limUtilCountStaDel(
    tpAniSirGlobal pMac,
    tpDphHashNode  pSta,
    tpPESession psessionEntry)
{

    if ((pSta == NULL) || (pSta->aniPeer == eHAL_CLEAR) || (! pSta->fAniCount))
        return;

    /*                                                                  
                                                                    
                                                                                  
     */
    if (pSta->valid == 1)
         return;

    pSta->fAniCount = 0;

    if (pMac->lim.gLimNumOfAniSTAs <= 0)
    {
        limLog(pMac, LOGE, FL("CountStaDel: ignoring Delete Req when AniPeer count is %d"),
               pMac->lim.gLimNumOfAniSTAs);
        return;
    }

    pMac->lim.gLimNumOfAniSTAs--;

    if (pMac->lim.gLimNumOfAniSTAs != 0)
        return;

    //                                                      
    schEdcaProfileUpdate(pMac, psessionEntry);
}

/* 
                          
  
           
                                                                               
                                                                      
   
       
                                                             
                                                              
                                       
                                                  
               
 */
void limSwitchChannelCback(tpAniSirGlobal pMac, eHalStatus status, 
                           tANI_U32 *data, tpPESession psessionEntry)
{
   tSirMsgQ    mmhMsg = {0};
   tSirSmeSwitchChannelInd *pSirSmeSwitchChInd;

   psessionEntry->currentOperChannel = psessionEntry->currentReqChannel; 
   
   /*                                                       */
   if (limRestorePreChannelSwitchState(pMac, psessionEntry) != eSIR_SUCCESS)
   {
      limLog(pMac, LOGP, FL("Could not restore pre-channelSwitch (11h) state, resetting the system"));
      return;
   }
   
   mmhMsg.type = eWNI_SME_SWITCH_CHL_REQ;
   pSirSmeSwitchChInd = vos_mem_malloc(sizeof(tSirSmeSwitchChannelInd));
   if ( NULL == pSirSmeSwitchChInd )
   {
      limLog(pMac, LOGP, FL("Failed to allocate buffer for buffer descriptor"));
      return;
   }
  
   pSirSmeSwitchChInd->messageType = eWNI_SME_SWITCH_CHL_REQ;
   pSirSmeSwitchChInd->length = sizeof(tSirSmeSwitchChannelInd);
   pSirSmeSwitchChInd->newChannelId = psessionEntry->gLimChannelSwitch.primaryChannel;
   pSirSmeSwitchChInd->sessionId = psessionEntry->smeSessionId;
   //      
   vos_mem_copy( pSirSmeSwitchChInd->bssId, psessionEntry->bssId, sizeof(tSirMacAddr));
   mmhMsg.bodyptr = pSirSmeSwitchChInd;
   mmhMsg.bodyval = 0;
   
   MTRACE(macTraceMsgTx(pMac, psessionEntry->peSessionId, mmhMsg.type));
   
   SysProcessMmhMsg(pMac, &mmhMsg);
}

/* 
                            
  
           
                                                       
                                                               
  
       
                                                      
                                      
               
 */
void limSwitchPrimaryChannel(tpAniSirGlobal pMac, tANI_U8 newChannel,tpPESession psessionEntry)
{
#if !defined WLAN_FEATURE_VOWIFI  
    tANI_U32 localPwrConstraint;
#endif
    
    PELOG3(limLog(pMac, LOG3, FL("limSwitchPrimaryChannel: old chnl %d --> new chnl %d "),
           psessionEntry->currentOperChannel, newChannel);)
    psessionEntry->currentReqChannel = newChannel;
    psessionEntry->limRFBand = limGetRFBand(newChannel);

    psessionEntry->channelChangeReasonCode=LIM_SWITCH_CHANNEL_OPERATION;

    pMac->lim.gpchangeChannelCallback = limSwitchChannelCback;
    pMac->lim.gpchangeChannelData = NULL;

#if defined WLAN_FEATURE_VOWIFI  
    limSendSwitchChnlParams(pMac, newChannel, PHY_SINGLE_CHANNEL_CENTERED,
                                                   psessionEntry->maxTxPower, psessionEntry->peSessionId);
#else
    if(wlan_cfgGetInt(pMac, WNI_CFG_LOCAL_POWER_CONSTRAINT, &localPwrConstraint) != eSIR_SUCCESS)
    {
        limLog( pMac, LOGP, FL( "Unable to read Local Power Constraint from cfg" ));
        return;
    }
    limSendSwitchChnlParams(pMac, newChannel, PHY_SINGLE_CHANNEL_CENTERED,
                                                   (tPowerdBm)localPwrConstraint, psessionEntry->peSessionId);
#endif
    return;
}

/* 
                                     
  
           
                                                            
                                                          
                                                         
                                                                 
                                     
  
       
                                                      
                                                              
                                         
                                                 
                                               
                                                 
               
 */
void limSwitchPrimarySecondaryChannel(tpAniSirGlobal pMac, tpPESession psessionEntry, tANI_U8 newChannel, ePhyChanBondState subband)
{
#if !defined WLAN_FEATURE_VOWIFI  
    tANI_U32 localPwrConstraint;
#endif

#if !defined WLAN_FEATURE_VOWIFI  
    if(wlan_cfgGetInt(pMac, WNI_CFG_LOCAL_POWER_CONSTRAINT, &localPwrConstraint) != eSIR_SUCCESS) {
        limLog( pMac, LOGP, FL( "Unable to get Local Power Constraint from cfg" ));
        return;
    }
#endif

#if defined WLAN_FEATURE_VOWIFI  
                limSendSwitchChnlParams(pMac, newChannel, subband, psessionEntry->maxTxPower, psessionEntry->peSessionId);
#else
                limSendSwitchChnlParams(pMac, newChannel, subband, (tPowerdBm)localPwrConstraint, psessionEntry->peSessionId);
#endif

    //                                                                            
    if (psessionEntry->currentOperChannel != newChannel)
    {
        limLog(pMac, LOGW,
            FL("switch old chnl %d --> new chnl %d "),
            psessionEntry->currentOperChannel, newChannel);
        psessionEntry->currentOperChannel = newChannel;
    }
    if (psessionEntry->htSecondaryChannelOffset != subband)
    {
        limLog(pMac, LOGW,
            FL("switch old sec chnl %d --> new sec chnl %d "),
            psessionEntry->htSecondaryChannelOffset, subband);
        psessionEntry->htSecondaryChannelOffset = subband;
        if (psessionEntry->htSecondaryChannelOffset == PHY_SINGLE_CHANNEL_CENTERED)
        {
            psessionEntry->htSupportedChannelWidthSet = WNI_CFG_CHANNEL_BONDING_MODE_DISABLE;
        }
        else
        {
            psessionEntry->htSupportedChannelWidthSet = WNI_CFG_CHANNEL_BONDING_MODE_ENABLE;
        }
        psessionEntry->htRecommendedTxWidthSet = psessionEntry->htSupportedChannelWidthSet;
    }

    return;
}


/* 
                         
  
           
                                                            
  
        
                                                                                          
                                                                               
                                                                          
  
              
  
       
  
                                                     
                                    
               
 */

tANI_U8 limActiveScanAllowed(
    tpAniSirGlobal pMac,
    tANI_U8             channelNum)
{
    tANI_U32 i;
    tANI_U8  channelPair[WNI_CFG_SCAN_CONTROL_LIST_LEN];
    tANI_U32 len = WNI_CFG_SCAN_CONTROL_LIST_LEN;
    if (wlan_cfgGetStr(pMac, WNI_CFG_SCAN_CONTROL_LIST, channelPair, &len)
        != eSIR_SUCCESS)
    {
        PELOGE(limLog(pMac, LOGE, FL("Unable to get scan control list"));)
        return false;
    }

    if (len > WNI_CFG_SCAN_CONTROL_LIST_LEN)
    {
        limLog(pMac, LOGE, FL("Invalid scan control list length:%d"),
               len);
        return false;
    }

    for (i=0; (i+1) < len; i+=2)
    {
        if (channelPair[i] == channelNum)
            return ((channelPair[i+1] == eSIR_ACTIVE_SCAN) ? true : false);
    }
    return false;
}

/* 
                                           
  
           
                                               
                                                       
                                                         
  
        
                     
                                                        
                  
                        
                                                     
                                                     
                           
                                                      
                                     
                                                     
          
  
              
  
       
                                             
                                                     
                                      
                                       
  
                                               
                                                        
                             
 */
tAniBool limTriggerBackgroundScanDuringQuietBss( tpAniSirGlobal pMac )
{
    tAniBool bScanTriggered = eSIR_FALSE;

    

    //                                       
    tpPESession psessionEntry = &pMac->lim.gpSession[0];

    if (psessionEntry->limSystemRole != eLIM_STA_ROLE)
        return bScanTriggered;
    
    if( !psessionEntry->lim11hEnable )
    {
        tSirMacChanNum bgScanChannelList[WNI_CFG_BG_SCAN_CHANNEL_LIST_LEN];
        tANI_U32 len = WNI_CFG_BG_SCAN_CHANNEL_LIST_LEN;

        //                                

        //                                          
        if( eSIR_SUCCESS == wlan_cfgGetStr( pMac,
          WNI_CFG_BG_SCAN_CHANNEL_LIST,
          (tANI_U8 *) bgScanChannelList,
          (tANI_U32 *) &len ))
        {
            //                                                  
        //                                             
            if( psessionEntry->currentOperChannel!=
                bgScanChannelList[pMac->lim.gLimBackgroundScanChannelId] )
            {
            //                                                    
            //                                                   
            //                                             
            limTriggerBackgroundScan( pMac );

            bScanTriggered = eSIR_TRUE;
        }
        else
        {
            limLog( pMac, LOGW,
                FL("The next SCAN channel is the current operating channel on which a Quiet BSS is requested.! A background scan will not be triggered during this Quiet BSS period..."));
        }
    }
    else
    {
      limLog( pMac, LOGW,
          FL("Unable to retrieve WNI_CFG_VALID_CHANNEL_LIST from CFG! A background scan will not be triggered during this Quiet BSS period..."));
    }
  }
  return bScanTriggered;
}


/* 
                       
  
           
                                                                               
                                                   
  
        
                                                                        
                                                        
                              
                   
                                      
                                           
  
  
              
     
  
       
  
                                                
                                                
                                                                                    
                              
 */

tANI_U8 limGetHTCapability( tpAniSirGlobal pMac,
        tANI_U32 htCap, tpPESession psessionEntry)
{
tANI_U8 retVal = 0;
tANI_U8 *ptr;
tANI_U32  cfgValue;
tSirMacHTCapabilityInfo macHTCapabilityInfo = {0};
tSirMacExtendedHTCapabilityInfo macExtHTCapabilityInfo = {0};
tSirMacTxBFCapabilityInfo macTxBFCapabilityInfo = {0};
tSirMacASCapabilityInfo macASCapabilityInfo = {0};

  //
  //                                                    
  //                                                    
  //         
  //
  if( htCap >= eHT_ANTENNA_SELECTION &&
      htCap < eHT_SI_GRANULARITY )
  {
    //                                     
    if( eSIR_SUCCESS != wlan_cfgGetInt( pMac, WNI_CFG_AS_CAP, &cfgValue ))
      cfgValue = 0;
    ptr = (tANI_U8 *) &macASCapabilityInfo;
    *((tANI_U8 *)ptr) =  (tANI_U8) (cfgValue & 0xff);
  }
  else
  {
    if( htCap >= eHT_TX_BEAMFORMING &&
        htCap < eHT_ANTENNA_SELECTION )
    {
      //                                          
      if( eSIR_SUCCESS != wlan_cfgGetInt( pMac, WNI_CFG_TX_BF_CAP, &cfgValue ))
        cfgValue = 0;
      ptr = (tANI_U8 *) &macTxBFCapabilityInfo;
      *((tANI_U32 *)ptr) =  (tANI_U32) (cfgValue);
    }
    else
    {
      if( htCap >= eHT_PCO &&
          htCap < eHT_TX_BEAMFORMING )
      {
        //                             
        if( eSIR_SUCCESS != wlan_cfgGetInt( pMac, WNI_CFG_EXT_HT_CAP_INFO, &cfgValue ))
          cfgValue = 0;
        ptr = (tANI_U8 *) &macExtHTCapabilityInfo;
        *((tANI_U16 *)ptr) =  (tANI_U16) (cfgValue & 0xffff);
      }
      else
      {
        if( htCap < eHT_MAX_RX_AMPDU_FACTOR )
        {
          //                    
          if( eSIR_SUCCESS != wlan_cfgGetInt( pMac, WNI_CFG_HT_CAP_INFO, &cfgValue ))
            cfgValue = 0;
          ptr = (tANI_U8 *) &macHTCapabilityInfo;
          //                                                                                                          
          *ptr++ = cfgValue & 0xff;
          *ptr = (cfgValue >> 8) & 0xff;
        }
      }
    }
  }

  switch( htCap )
  {
    case eHT_LSIG_TXOP_PROTECTION:
      retVal = pMac->lim.gHTLsigTXOPProtection;
      break;

    case eHT_STBC_CONTROL_FRAME:
      retVal = (tANI_U8) macHTCapabilityInfo.stbcControlFrame;
      break;

    case eHT_PSMP:
      retVal = pMac->lim.gHTPSMPSupport;
      break;

    case eHT_DSSS_CCK_MODE_40MHZ:
      retVal = pMac->lim.gHTDsssCckRate40MHzSupport;
      break;

    case eHT_MAX_AMSDU_LENGTH:
      retVal = (tANI_U8) macHTCapabilityInfo.maximalAMSDUsize;
      break;

    case eHT_DELAYED_BA:
      retVal = (tANI_U8) macHTCapabilityInfo.delayedBA;
      break;

    case eHT_RX_STBC:
      retVal = (tANI_U8) macHTCapabilityInfo.rxSTBC;
      break;

    case eHT_TX_STBC:
      retVal = (tANI_U8) macHTCapabilityInfo.txSTBC;
      break;

    case eHT_SHORT_GI_40MHZ:
      retVal = (tANI_U8) macHTCapabilityInfo.shortGI40MHz;
      break;

    case eHT_SHORT_GI_20MHZ:
      retVal = (tANI_U8) macHTCapabilityInfo.shortGI20MHz;
      break;

    case eHT_GREENFIELD:
      retVal = (tANI_U8) macHTCapabilityInfo.greenField;
      break;

    case eHT_MIMO_POWER_SAVE:
      retVal = (tANI_U8) pMac->lim.gHTMIMOPSState;
      break;

    case eHT_SUPPORTED_CHANNEL_WIDTH_SET:
      retVal = (tANI_U8) psessionEntry->htSupportedChannelWidthSet;
      break;

    case eHT_ADVANCED_CODING:
      retVal = (tANI_U8) macHTCapabilityInfo.advCodingCap;
      break;

    case eHT_MAX_RX_AMPDU_FACTOR:
      retVal = pMac->lim.gHTMaxRxAMpduFactor;
      break;

    case eHT_MPDU_DENSITY:
      retVal = pMac->lim.gHTAMpduDensity;
      break;

    case eHT_PCO:
      retVal = (tANI_U8) macExtHTCapabilityInfo.pco;
      break;

    case eHT_TRANSITION_TIME:
      retVal = (tANI_U8) macExtHTCapabilityInfo.transitionTime;
      break;

    case eHT_MCS_FEEDBACK:
      retVal = (tANI_U8) macExtHTCapabilityInfo.mcsFeedback;
      break;

    case eHT_TX_BEAMFORMING:
      retVal = (tANI_U8) macTxBFCapabilityInfo.txBF;
      break;

    case eHT_ANTENNA_SELECTION:
      retVal = (tANI_U8) macASCapabilityInfo.antennaSelection;
      break;

    case eHT_SI_GRANULARITY:
      retVal = pMac->lim.gHTServiceIntervalGranularity;
      break;

    case eHT_CONTROLLED_ACCESS:
      retVal = pMac->lim.gHTControlledAccessOnly;
      break;

    case eHT_RIFS_MODE:
      retVal = psessionEntry->beaconParams.fRIFSMode;
      break;

    case eHT_RECOMMENDED_TX_WIDTH_SET:
      retVal = psessionEntry->htRecommendedTxWidthSet;
      break;

    case eHT_EXTENSION_CHANNEL_OFFSET:
      retVal = psessionEntry->htSecondaryChannelOffset;
      break;

    case eHT_OP_MODE:
      if(psessionEntry->limSystemRole == eLIM_AP_ROLE )
          retVal = psessionEntry->htOperMode;
      else
          retVal = pMac->lim.gHTOperMode;
      break;

    case eHT_BASIC_STBC_MCS:
      retVal = pMac->lim.gHTSTBCBasicMCS;
      break;

    case eHT_DUAL_CTS_PROTECTION:
      retVal = pMac->lim.gHTDualCTSProtection;
      break;

    case eHT_LSIG_TXOP_PROTECTION_FULL_SUPPORT:
      retVal = psessionEntry->beaconParams.fLsigTXOPProtectionFullSupport;
      break;

    case eHT_PCO_ACTIVE:
      retVal = pMac->lim.gHTPCOActive;
      break;

    case eHT_PCO_PHASE:
      retVal = pMac->lim.gHTPCOPhase;
      break;

    default:
      break;
  }

  return retVal;
}

void limGetMyMacAddr(tpAniSirGlobal pMac, tANI_U8 *mac)
{
    vos_mem_copy( mac, pMac->lim.gLimMyMacAddr, sizeof(tSirMacAddr));
    return;
}




/*                                                               
                          
                                                               
                                                                           
                                                                                             
                                              
                 
                                                               */
tSirRetStatus
limEnable11aProtection(tpAniSirGlobal pMac, tANI_U8 enable,
    tANI_U8 overlap, tpUpdateBeaconParams pBeaconParams,tpPESession psessionEntry)
{
    if(NULL == psessionEntry)
    {
        PELOG3(limLog(pMac, LOG3, FL("psessionEntry is NULL"));)
        return eSIR_FAILURE;
    }        
        //                                           
        if(overlap)
        {
        }
        else
        {
            //                              
            if ((psessionEntry->limSystemRole == eLIM_AP_ROLE) &&
                (!psessionEntry->cfgProtection.fromlla))
            {
                //                     
                PELOG3(limLog(pMac, LOG3, FL("protection from 11a is disabled"));)
                return eSIR_SUCCESS;
            }
        }

    if (enable)
    {
        //                                                          
        //              
        if(((eLIM_AP_ROLE == psessionEntry->limSystemRole)||(eLIM_BT_AMP_AP_ROLE == psessionEntry->limSystemRole))&&
              (true == psessionEntry->htCapability))
        {
            if(overlap)
            {
                pMac->lim.gLimOverlap11aParams.protectionEnabled = true;
                if((eSIR_HT_OP_MODE_OVERLAP_LEGACY != pMac->lim.gHTOperMode) &&
                   (eSIR_HT_OP_MODE_MIXED != pMac->lim.gHTOperMode))
                {
                    pMac->lim.gHTOperMode = eSIR_HT_OP_MODE_OVERLAP_LEGACY;
                    psessionEntry->htOperMode = eSIR_HT_OP_MODE_OVERLAP_LEGACY;
                    limEnableHtRifsProtection(pMac, true, overlap, pBeaconParams,psessionEntry);          
                    limEnableHtOBSSProtection(pMac,  true, overlap, pBeaconParams,psessionEntry);         
                }
            }
            else
            {
                psessionEntry->gLim11aParams.protectionEnabled = true;
                if(eSIR_HT_OP_MODE_MIXED != pMac->lim.gHTOperMode)
                {
                    pMac->lim.gHTOperMode = eSIR_HT_OP_MODE_MIXED;
                    psessionEntry->htOperMode = eSIR_HT_OP_MODE_MIXED;
                    limEnableHtRifsProtection(pMac, true, overlap, pBeaconParams,psessionEntry);
                    limEnableHtOBSSProtection(pMac,  true, overlap, pBeaconParams,psessionEntry);         
                    
                }
            }
        }

        //                                        
        if(false == psessionEntry->beaconParams.llaCoexist)
        {
            PELOG1(limLog(pMac, LOG1, FL(" => protection from 11A Enabled"));)
            pBeaconParams->llaCoexist = psessionEntry->beaconParams.llaCoexist = true;
            pBeaconParams->paramChangeBitmap |= PARAM_llACOEXIST_CHANGED;
        }
    }
    else if (true == psessionEntry->beaconParams.llaCoexist)
    {
        //            
        //                                                    
        //                                      
        if(eLIM_AP_ROLE == psessionEntry->limSystemRole)
        {
            if(overlap)
            {
                //                                   
                pMac->lim.gLimOverlap11aParams.protectionEnabled = false;

                //                                                    
                if(psessionEntry->htCapability)
                {
                   //                                                               
                    if(!(pMac->lim.gLimOverlap11aParams.protectionEnabled ||
                         pMac->lim.gLimOverlapHt20Params.protectionEnabled ||
                         pMac->lim.gLimOverlapNonGfParams.protectionEnabled))

                    {
                        //                                              
                        if(eSIR_HT_OP_MODE_OVERLAP_LEGACY == pMac->lim.gHTOperMode)
                        {
                            limEnableHtRifsProtection(pMac, false, overlap, pBeaconParams,psessionEntry);
                            limEnableHtOBSSProtection(pMac,  false, overlap, pBeaconParams,psessionEntry);        

                            if(psessionEntry->gLimHt20Params.protectionEnabled)
                                pMac->lim.gHTOperMode = eSIR_HT_OP_MODE_NO_LEGACY_20MHZ_HT;
                            else
                                pMac->lim.gHTOperMode = eSIR_HT_OP_MODE_PURE;
                        }
                    }
                }
            }
            else
            {
                //                                     
                psessionEntry->gLim11aParams.protectionEnabled = false;
                limEnableHtOBSSProtection(pMac,  false, overlap, pBeaconParams,psessionEntry);

                //                                             
                //                                     
                //                                

                //                                                         
                if(pMac->lim.gLimOverlap11aParams.protectionEnabled ||
                   pMac->lim.gLimOverlapHt20Params.protectionEnabled ||
                   pMac->lim.gLimOverlapNonGfParams.protectionEnabled)

                {
                        pMac->lim.gHTOperMode = eSIR_HT_OP_MODE_OVERLAP_LEGACY;
                        psessionEntry->htOperMode = eSIR_HT_OP_MODE_OVERLAP_LEGACY;
                        limEnableHtRifsProtection(pMac, true, overlap, pBeaconParams,psessionEntry);
                }
                else if(psessionEntry->gLimHt20Params.protectionEnabled)
                {
                        pMac->lim.gHTOperMode = eSIR_HT_OP_MODE_NO_LEGACY_20MHZ_HT;
                        psessionEntry->htOperMode = eSIR_HT_OP_MODE_NO_LEGACY_20MHZ_HT;
                        limEnableHtRifsProtection(pMac, false, overlap, pBeaconParams,psessionEntry);
                }
                else
                {
                        pMac->lim.gHTOperMode = eSIR_HT_OP_MODE_PURE;
                        psessionEntry->htOperMode = eSIR_HT_OP_MODE_PURE;
                        limEnableHtRifsProtection(pMac, false, overlap, pBeaconParams,psessionEntry);
                }
            }
        if(!pMac->lim.gLimOverlap11aParams.protectionEnabled &&
           !psessionEntry->gLim11aParams.protectionEnabled)
            {
                PELOG1(limLog(pMac, LOG1, FL("===> Protection from 11A Disabled"));)
                pBeaconParams->llaCoexist = psessionEntry->beaconParams.llaCoexist = false;
                pBeaconParams->paramChangeBitmap |= PARAM_llACOEXIST_CHANGED;
            }
        }
        //                
        else
        {
            PELOG1(limLog(pMac, LOG1, FL("===> Protection from 11A Disabled"));)
            pBeaconParams->llaCoexist = psessionEntry->beaconParams.llaCoexist = false;
            pBeaconParams->paramChangeBitmap |= PARAM_llACOEXIST_CHANGED;
        }
    }

    return eSIR_SUCCESS;
}

/*                                                               
                          
                                                               
                                                                           
                                                                                             
                                              
                 
                                                               */

tSirRetStatus
limEnable11gProtection(tpAniSirGlobal pMac, tANI_U8 enable,
    tANI_U8 overlap, tpUpdateBeaconParams pBeaconParams,tpPESession psessionEntry)
{

    //                                           
    if(overlap)
    {
    }
    else
    {
        //                              
        if((psessionEntry->limSystemRole == eLIM_AP_ROLE ) &&
                !psessionEntry->cfgProtection.fromllb)
        {
            //                     
            PELOG1(limLog(pMac, LOG1, FL("protection from 11b is disabled"));)
            return eSIR_SUCCESS;
        }else if(psessionEntry->limSystemRole != eLIM_AP_ROLE)
        {   
            if(!pMac->lim.cfgProtection.fromllb)
            {
                //                     
                PELOG1(limLog(pMac, LOG1, FL("protection from 11b is disabled"));)
                return eSIR_SUCCESS;
            }
        }
    }

    if (enable)
    {
        //                                                          
        //              
        if(eLIM_AP_ROLE == psessionEntry->limSystemRole)
        {
            if(overlap)
            {
                psessionEntry->gLimOlbcParams.protectionEnabled = true;
                PELOGE(limLog(pMac, LOGE, FL("protection from olbc is enabled"));)
                if(true == psessionEntry->htCapability)
                {
                    if((eSIR_HT_OP_MODE_OVERLAP_LEGACY != psessionEntry->htOperMode) &&
                            (eSIR_HT_OP_MODE_MIXED != psessionEntry->htOperMode))
                    {
                        psessionEntry->htOperMode = eSIR_HT_OP_MODE_OVERLAP_LEGACY;
                    }
                    //                                                                                         
                    //                                                                   
                    limEnableHtRifsProtection(pMac, true, overlap, pBeaconParams,psessionEntry);
                    //                                                                     
                    //                                                                       
                    limEnableHtOBSSProtection(pMac,  true, overlap, pBeaconParams, psessionEntry);
                }
            }
            else
            {
                psessionEntry->gLim11bParams.protectionEnabled = true;
                PELOGE(limLog(pMac, LOGE, FL("protection from 11b is enabled"));)
                if(true == psessionEntry->htCapability)
                {
                    if(eSIR_HT_OP_MODE_MIXED != psessionEntry->htOperMode)
                    {
                        psessionEntry->htOperMode = eSIR_HT_OP_MODE_MIXED;
                        limEnableHtRifsProtection(pMac, true, overlap, pBeaconParams,psessionEntry);
                        limEnableHtOBSSProtection(pMac,  true, overlap, pBeaconParams,psessionEntry);     
                    }
                }
            }
        }else if ((eLIM_BT_AMP_AP_ROLE == psessionEntry->limSystemRole) &&
                (true == psessionEntry->htCapability))
            {
                if(overlap)
                {
                    psessionEntry->gLimOlbcParams.protectionEnabled = true;
                    if((eSIR_HT_OP_MODE_OVERLAP_LEGACY != pMac->lim.gHTOperMode) &&
                            (eSIR_HT_OP_MODE_MIXED != pMac->lim.gHTOperMode))
                    {
                        pMac->lim.gHTOperMode = eSIR_HT_OP_MODE_OVERLAP_LEGACY;
                    }
                    //                                                                                         
                    //                                                                   
                    limEnableHtRifsProtection(pMac, true, overlap, pBeaconParams,psessionEntry);
                    //                                                                     
                    //                                                                       
                    limEnableHtOBSSProtection(pMac,  true, overlap, pBeaconParams, psessionEntry);
                }
                else
                {
                    psessionEntry->gLim11bParams.protectionEnabled = true;
                    if(eSIR_HT_OP_MODE_MIXED != pMac->lim.gHTOperMode)
                    { 
                        pMac->lim.gHTOperMode = eSIR_HT_OP_MODE_MIXED;
                        limEnableHtRifsProtection(pMac, true, overlap, pBeaconParams,psessionEntry);
                        limEnableHtOBSSProtection(pMac,  true, overlap, pBeaconParams,psessionEntry);     
                    }
                }
            }

        //                                        
        if(false == psessionEntry->beaconParams.llbCoexist)
        {
            PELOG1(limLog(pMac, LOG1, FL("=> 11G Protection Enabled"));)
            pBeaconParams->llbCoexist = psessionEntry->beaconParams.llbCoexist = true;
            pBeaconParams->paramChangeBitmap |= PARAM_llBCOEXIST_CHANGED;
        }
    }
    else if (true == psessionEntry->beaconParams.llbCoexist)
    {
        //            
        //                                                    
        //                                      
        if(eLIM_AP_ROLE == psessionEntry->limSystemRole)
        {
            if(overlap)
            {
                //                                   
                psessionEntry->gLimOlbcParams.protectionEnabled = false;

                //                                                   
                if(psessionEntry->htCapability)
                {
                    //                                                               
                    if(!(psessionEntry->gLimOverlap11gParams.protectionEnabled ||
                                psessionEntry->gLimOverlapHt20Params.protectionEnabled ||
                                psessionEntry->gLimOverlapNonGfParams.protectionEnabled))
                    {
                        //                                              
                        if(eSIR_HT_OP_MODE_OVERLAP_LEGACY == psessionEntry->htOperMode)
                        {
                            limEnableHtRifsProtection(pMac, false, overlap, pBeaconParams,psessionEntry);
                            limEnableHtOBSSProtection(pMac,  false, overlap, pBeaconParams,psessionEntry);            
                            if(psessionEntry->gLimHt20Params.protectionEnabled){
                                //                                            
                                //                                                               
                                psessionEntry->htOperMode = eSIR_HT_OP_MODE_PURE;
                            }
                            else
                                psessionEntry->htOperMode = eSIR_HT_OP_MODE_PURE;
                        }
                    }
                }
            }
            else
            {
                //                                     
                psessionEntry->gLim11bParams.protectionEnabled = false;
                PELOGE(limLog(pMac, LOGE, FL("===> 11B Protection Disabled"));)
                    //                                             
                if(!psessionEntry->gLim11gParams.protectionEnabled)
                {
                    //                                     
                    //                                
                    limEnableHtOBSSProtection(pMac,  false, overlap, pBeaconParams,psessionEntry);            

                    //                                                         
                    if(psessionEntry->gLimOlbcParams.protectionEnabled ||
                            psessionEntry->gLimOverlap11gParams.protectionEnabled ||
                            psessionEntry->gLimOverlapHt20Params.protectionEnabled ||
                            psessionEntry->gLimOverlapNonGfParams.protectionEnabled)
                    {
                        psessionEntry->htOperMode = eSIR_HT_OP_MODE_OVERLAP_LEGACY;
                        PELOGE(limLog(pMac, LOGE, FL("===> 11G Protection Disabled"));)
                        limEnableHtRifsProtection(pMac, true, overlap, pBeaconParams,psessionEntry);
                    }
                    else if(psessionEntry->gLimHt20Params.protectionEnabled)
                    {
                        //                                        
                        //                                                               
                        psessionEntry->htOperMode = eSIR_HT_OP_MODE_PURE;
                        PELOGE(limLog(pMac, LOGE, FL("===> 11G Protection Disabled"));)
                        limEnableHtRifsProtection(pMac, false, overlap, pBeaconParams,psessionEntry);
                    }
                    else
                    {
                        psessionEntry->htOperMode = eSIR_HT_OP_MODE_PURE;
                        limEnableHtRifsProtection(pMac, false, overlap, pBeaconParams,psessionEntry);
                    }
                }
            }
            if(!psessionEntry->gLimOlbcParams.protectionEnabled &&
                    !psessionEntry->gLim11bParams.protectionEnabled)
            {
                PELOGE(limLog(pMac, LOGE, FL("===> 11G Protection Disabled"));)
                pBeaconParams->llbCoexist = psessionEntry->beaconParams.llbCoexist = false;
                pBeaconParams->paramChangeBitmap |= PARAM_llBCOEXIST_CHANGED;
            }
        }else if(eLIM_BT_AMP_AP_ROLE == psessionEntry->limSystemRole)
            {
                if(overlap)
                {
                    //                                   
                psessionEntry->gLimOlbcParams.protectionEnabled = false;

                    //                                                    
                    if(psessionEntry->htCapability)
                    {
                        //                                                               
                        if(!(pMac->lim.gLimOverlap11gParams.protectionEnabled ||
                                    pMac->lim.gLimOverlapHt20Params.protectionEnabled ||
                                    pMac->lim.gLimOverlapNonGfParams.protectionEnabled))

                        {
                            //                                              
                            if(eSIR_HT_OP_MODE_OVERLAP_LEGACY == pMac->lim.gHTOperMode)
                            {
                                limEnableHtRifsProtection(pMac, false, overlap, pBeaconParams,psessionEntry);
                                limEnableHtOBSSProtection(pMac,  false, overlap, pBeaconParams,psessionEntry);            
                            if(psessionEntry->gLimHt20Params.protectionEnabled)
                                    pMac->lim.gHTOperMode = eSIR_HT_OP_MODE_NO_LEGACY_20MHZ_HT;
                                else
                                    pMac->lim.gHTOperMode = eSIR_HT_OP_MODE_PURE;
                            }
                        }
                    }
                }
                else
                {
                    //                                     
                psessionEntry->gLim11bParams.protectionEnabled = false;
                    //                                             
                if(!psessionEntry->gLim11gParams.protectionEnabled)
                    {
                        //                                     
                        //                                
                        limEnableHtOBSSProtection(pMac,  false, overlap, pBeaconParams,psessionEntry);            

                        //                                                         
                    if(psessionEntry->gLimOlbcParams.protectionEnabled ||
                                pMac->lim.gLimOverlap11gParams.protectionEnabled ||
                                pMac->lim.gLimOverlapHt20Params.protectionEnabled ||
                                pMac->lim.gLimOverlapNonGfParams.protectionEnabled)

                        {
                            pMac->lim.gHTOperMode = eSIR_HT_OP_MODE_OVERLAP_LEGACY;
                            limEnableHtRifsProtection(pMac, true, overlap, pBeaconParams,psessionEntry);
                        }
                    else if(psessionEntry->gLimHt20Params.protectionEnabled)
                        {
                            pMac->lim.gHTOperMode = eSIR_HT_OP_MODE_NO_LEGACY_20MHZ_HT;
                            limEnableHtRifsProtection(pMac, false, overlap, pBeaconParams,psessionEntry);
                        }
                        else
                        {
                            pMac->lim.gHTOperMode = eSIR_HT_OP_MODE_PURE;
                            limEnableHtRifsProtection(pMac, false, overlap, pBeaconParams,psessionEntry);
                        }
                    }
                }
            if(!psessionEntry->gLimOlbcParams.protectionEnabled &&
                  !psessionEntry->gLim11bParams.protectionEnabled)
                {
                    PELOG1(limLog(pMac, LOG1, FL("===> 11G Protection Disabled"));)
                pBeaconParams->llbCoexist = psessionEntry->beaconParams.llbCoexist = false;
                    pBeaconParams->paramChangeBitmap |= PARAM_llBCOEXIST_CHANGED;
                }
            }
        //                
            else
            {
                PELOG1(limLog(pMac, LOG1, FL("===> 11G Protection Disabled"));)
            pBeaconParams->llbCoexist = psessionEntry->beaconParams.llbCoexist = false;
                pBeaconParams->paramChangeBitmap |= PARAM_llBCOEXIST_CHANGED;
            }
    }
    return eSIR_SUCCESS;
}
    
/*                                                               
                                
                                                           
                                                                           
                                                                                             
                                              
                 
                                                               */
tSirRetStatus
limEnableHtProtectionFrom11g(tpAniSirGlobal pMac, tANI_U8 enable,
    tANI_U8 overlap, tpUpdateBeaconParams pBeaconParams,tpPESession psessionEntry)
{
    if(!psessionEntry->htCapability)
        return eSIR_SUCCESS; //                                             

    //                                           
    if(overlap)
    {
        if((psessionEntry->limSystemRole == eLIM_AP_ROLE ) && (!psessionEntry->cfgProtection.overlapFromllg))
        {
            //                     
            PELOG3(limLog(pMac, LOG3, FL("overlap protection from 11g is disabled")););
            return eSIR_SUCCESS;
        }else if ((psessionEntry->limSystemRole == eLIM_BT_AMP_AP_ROLE) && (!pMac->lim.cfgProtection.overlapFromllg))
        {
            //                     
            PELOG3(limLog(pMac, LOG3, FL("overlap protection from 11g is disabled")););
            return eSIR_SUCCESS;
        }
        }
    else
    {
        //                              
       if((psessionEntry->limSystemRole == eLIM_AP_ROLE ) && 
           !psessionEntry->cfgProtection.fromllg){
            //                     
            PELOG3(limLog(pMac, LOG3, FL("protection from 11g is disabled"));)
            return eSIR_SUCCESS;
         }else if(psessionEntry->limSystemRole != eLIM_AP_ROLE )
       {
          if(!pMac->lim.cfgProtection.fromllg)
           {
                //                     
                PELOG3(limLog(pMac, LOG3, FL("protection from 11g is disabled"));)
                return eSIR_SUCCESS;
            }
        }
     }
    if (enable)
    {
        //                                                          
        //              

        if(eLIM_AP_ROLE == psessionEntry->limSystemRole)
        {
            if(overlap)
            {
                psessionEntry->gLimOverlap11gParams.protectionEnabled = true;
                //                          
                //                                                       
                //                                                                 
                if((eSIR_HT_OP_MODE_OVERLAP_LEGACY != psessionEntry->htOperMode) &&
                    (eSIR_HT_OP_MODE_MIXED != psessionEntry->htOperMode))
                {
                    psessionEntry->htOperMode = eSIR_HT_OP_MODE_OVERLAP_LEGACY;
                }
                limEnableHtRifsProtection(pMac, true, overlap, pBeaconParams,psessionEntry);
                limEnableHtOBSSProtection(pMac,  true, overlap, pBeaconParams, psessionEntry);
            }
            else
            {
                //                                                 
                //                                             
                psessionEntry->gLim11gParams.protectionEnabled = true;
                if(eSIR_HT_OP_MODE_MIXED != psessionEntry->htOperMode)
                {
                    psessionEntry->htOperMode = eSIR_HT_OP_MODE_MIXED;
                    limEnableHtRifsProtection(pMac, true, overlap, pBeaconParams,psessionEntry);
                    limEnableHtOBSSProtection(pMac,  true, overlap, pBeaconParams,psessionEntry);
                }
            }
        }else if(eLIM_BT_AMP_AP_ROLE == psessionEntry->limSystemRole)
        {
            if(overlap)
            {
                pMac->lim.gLimOverlap11gParams.protectionEnabled = true;
                //                          
                //                                                       
                //                                                                 
                if((eSIR_HT_OP_MODE_OVERLAP_LEGACY != pMac->lim.gHTOperMode) &&
                    (eSIR_HT_OP_MODE_MIXED != pMac->lim.gHTOperMode))
                {
                    pMac->lim.gHTOperMode = eSIR_HT_OP_MODE_OVERLAP_LEGACY;
                    limEnableHtRifsProtection(pMac, true, overlap, pBeaconParams,psessionEntry);
                }
            }
            else
            {
                //                                                 
                //                                             
                psessionEntry->gLim11gParams.protectionEnabled = true;
                if(eSIR_HT_OP_MODE_MIXED != pMac->lim.gHTOperMode)
                {
                    pMac->lim.gHTOperMode = eSIR_HT_OP_MODE_MIXED;
                    limEnableHtRifsProtection(pMac, true, overlap, pBeaconParams,psessionEntry);
                    limEnableHtOBSSProtection(pMac,  true, overlap, pBeaconParams,psessionEntry);
                }
            }
        }

        //                                        
        if(false == psessionEntry->beaconParams.llgCoexist)
        {
            pBeaconParams->llgCoexist = psessionEntry->beaconParams.llgCoexist = true;
            pBeaconParams->paramChangeBitmap |= PARAM_llGCOEXIST_CHANGED;
        }
        else if (true == psessionEntry->gLimOverlap11gParams.protectionEnabled)
        {
            //                                                                          
            //                                                                                             
            //                                       
            pBeaconParams->paramChangeBitmap |= PARAM_llGCOEXIST_CHANGED;
        }
    }
    else if (true == psessionEntry->beaconParams.llgCoexist)
    {
        //            
        //                                                    
        //                                      

        if(eLIM_AP_ROLE == psessionEntry->limSystemRole)
        {
            if(overlap)
            {
                //                                   
                if (psessionEntry->gLim11gParams.numSta == 0)
                psessionEntry->gLimOverlap11gParams.protectionEnabled = false;

                //                                                               
                if(!(psessionEntry->gLimOlbcParams.protectionEnabled ||
                    psessionEntry->gLimOverlapHt20Params.protectionEnabled ||
                    psessionEntry->gLimOverlapNonGfParams.protectionEnabled))
                {
                    //                                              
                    if(eSIR_HT_OP_MODE_OVERLAP_LEGACY == psessionEntry->htOperMode)
                    {
                        limEnableHtRifsProtection(pMac, false, overlap, pBeaconParams,psessionEntry);
                        limEnableHtOBSSProtection(pMac,  false, overlap, pBeaconParams,psessionEntry);            

                        if(psessionEntry->gLimHt20Params.protectionEnabled){
                            //                                        
                            //                                                               
                            psessionEntry->htOperMode = eSIR_HT_OP_MODE_PURE;
                        }
                        else
                            psessionEntry->htOperMode = eSIR_HT_OP_MODE_PURE;
                    }
                }
            }
            else
            {
                //                                     
                psessionEntry->gLim11gParams.protectionEnabled = false;
                //                                             
                if(!psessionEntry->gLim11bParams.protectionEnabled)
                {

                    //                                     
                    //                                
                    limEnableHtOBSSProtection(pMac,  false, overlap, pBeaconParams,psessionEntry);            

                    //                                                         
                    if(psessionEntry->gLimOlbcParams.protectionEnabled ||
                        psessionEntry->gLimOverlap11gParams.protectionEnabled ||
                        psessionEntry->gLimOverlapHt20Params.protectionEnabled ||
                        psessionEntry->gLimOverlapNonGfParams.protectionEnabled)

                    {
                        psessionEntry->htOperMode = eSIR_HT_OP_MODE_OVERLAP_LEGACY;
                        limEnableHtRifsProtection(pMac, true, overlap, pBeaconParams,psessionEntry);
                    }
                    else if(psessionEntry->gLimHt20Params.protectionEnabled)
                    {
                        //                                        
                        //                                                               
                        psessionEntry->htOperMode = eSIR_HT_OP_MODE_PURE;
                        limEnableHtRifsProtection(pMac, false, overlap, pBeaconParams,psessionEntry);
                    }
                    else
                    {
                        psessionEntry->htOperMode = eSIR_HT_OP_MODE_PURE;
                        limEnableHtRifsProtection(pMac, false, overlap, pBeaconParams,psessionEntry);
                    }
                }
            }
            if(!psessionEntry->gLimOverlap11gParams.protectionEnabled &&
                  !psessionEntry->gLim11gParams.protectionEnabled)
            {
                PELOG1(limLog(pMac, LOG1, FL("===> Protection from 11G Disabled"));)
                pBeaconParams->llgCoexist = psessionEntry->beaconParams.llgCoexist = false;
                pBeaconParams->paramChangeBitmap |= PARAM_llGCOEXIST_CHANGED;
            }
        }else if(eLIM_BT_AMP_AP_ROLE == psessionEntry->limSystemRole)
        {
            if(overlap)
            {
                //                                   
                pMac->lim.gLimOverlap11gParams.protectionEnabled = false;

                //                                                               
                if(!(psessionEntry->gLimOlbcParams.protectionEnabled ||
                    psessionEntry->gLimOverlapHt20Params.protectionEnabled ||
                    psessionEntry->gLimOverlapNonGfParams.protectionEnabled))
                {
                    //                                              
                    if(eSIR_HT_OP_MODE_OVERLAP_LEGACY == pMac->lim.gHTOperMode)
                    {
                        limEnableHtRifsProtection(pMac, false, overlap, pBeaconParams,psessionEntry);
                        limEnableHtOBSSProtection(pMac,  false, overlap, pBeaconParams,psessionEntry);            

                        if(psessionEntry->gLimHt20Params.protectionEnabled)
                            pMac->lim.gHTOperMode = eSIR_HT_OP_MODE_NO_LEGACY_20MHZ_HT;
                        else
                            pMac->lim.gHTOperMode = eSIR_HT_OP_MODE_PURE;
                    }
                }
            }
            else
            {
                //                                     
                psessionEntry->gLim11gParams.protectionEnabled = false;
                //                                             
                if(!psessionEntry->gLim11bParams.protectionEnabled)
                {

                    //                                     
                    //                                
                    limEnableHtOBSSProtection(pMac,  false, overlap, pBeaconParams,psessionEntry);            

                    //                                                         
                    if(psessionEntry->gLimOlbcParams.protectionEnabled ||
                        pMac->lim.gLimOverlap11gParams.protectionEnabled ||
                        pMac->lim.gLimOverlapHt20Params.protectionEnabled ||
                        pMac->lim.gLimOverlapNonGfParams.protectionEnabled)

                    {
                        pMac->lim.gHTOperMode = eSIR_HT_OP_MODE_OVERLAP_LEGACY;
                        limEnableHtRifsProtection(pMac, true, overlap, pBeaconParams,psessionEntry);
                    }
                    else if(psessionEntry->gLimHt20Params.protectionEnabled)
                    {
                        pMac->lim.gHTOperMode = eSIR_HT_OP_MODE_NO_LEGACY_20MHZ_HT;
                        limEnableHtRifsProtection(pMac, false, overlap, pBeaconParams,psessionEntry);
                    }
                    else
                    {
                        pMac->lim.gHTOperMode = eSIR_HT_OP_MODE_PURE;
                        limEnableHtRifsProtection(pMac, false, overlap, pBeaconParams,psessionEntry);
                    }
                }
            }
            if(!pMac->lim.gLimOverlap11gParams.protectionEnabled &&
                  !psessionEntry->gLim11gParams.protectionEnabled)
            {
                PELOG1(limLog(pMac, LOG1, FL("===> Protection from 11G Disabled"));)
                pBeaconParams->llgCoexist = psessionEntry->beaconParams.llgCoexist = false;
                pBeaconParams->paramChangeBitmap |= PARAM_llGCOEXIST_CHANGED;
            }
        }
        //                
        else
        {
            PELOG1(limLog(pMac, LOG1, FL("===> Protection from 11G Disabled"));)
            pBeaconParams->llgCoexist = psessionEntry->beaconParams.llgCoexist = false;
            pBeaconParams->paramChangeBitmap |= PARAM_llGCOEXIST_CHANGED;
        }
    }
    return eSIR_SUCCESS;
}
//                                                                                        
//                                      

/*                                                               
                             
                                                       
                                                                           
                                                                                             
                                              
                 
                                                               */
tSirRetStatus
limEnableHtOBSSProtection(tpAniSirGlobal pMac, tANI_U8 enable,
    tANI_U8 overlap, tpUpdateBeaconParams pBeaconParams,tpPESession psessionEntry)
{


    if(!psessionEntry->htCapability)
        return eSIR_SUCCESS; //                                          

    //                                           
    if(overlap)
    {
        //                                           
    } 
    else 
    {
        //                              
        if((psessionEntry->limSystemRole == eLIM_AP_ROLE) && !psessionEntry->cfgProtection.obss) 
        { //                      
            //                     
            PELOG1(limLog(pMac, LOG1, FL("protection from Obss is disabled"));)
            return eSIR_SUCCESS;
        }else if(psessionEntry->limSystemRole != eLIM_AP_ROLE)
        {
            if(!pMac->lim.cfgProtection.obss) 
            { //                      
                //                     
                PELOG1(limLog(pMac, LOG1, FL("protection from Obss is disabled"));)
                return eSIR_SUCCESS;
            }
        }
    }


    if (eLIM_AP_ROLE == psessionEntry->limSystemRole){
        if ((enable) && (false == psessionEntry->beaconParams.gHTObssMode) )
        {
            PELOG1(limLog(pMac, LOG1, FL("=>obss protection enabled"));)
            psessionEntry->beaconParams.gHTObssMode = true;
            pBeaconParams->paramChangeBitmap |= PARAM_OBSS_MODE_CHANGED; //                                    
        
         }
         else if (!enable && (true == psessionEntry->beaconParams.gHTObssMode)) 
         {
            PELOG1(limLog(pMac, LOG1, FL("===> obss Protection disabled"));)
            psessionEntry->beaconParams.gHTObssMode = false;
            pBeaconParams->paramChangeBitmap |= PARAM_OBSS_MODE_CHANGED;

         }
//                                                                                         
         if (!enable && !overlap)
         {
             psessionEntry->gLimOverlap11gParams.protectionEnabled = false;
         }
    } else
    {
        if ((enable) && (false == psessionEntry->beaconParams.gHTObssMode) )
        {
            PELOG1(limLog(pMac, LOG1, FL("=>obss protection enabled"));)
            psessionEntry->beaconParams.gHTObssMode = true;
            pBeaconParams->paramChangeBitmap |= PARAM_OBSS_MODE_CHANGED; //                                    

        }
        else if (!enable && (true == psessionEntry->beaconParams.gHTObssMode)) 
        {

            PELOG1(limLog(pMac, LOG1, FL("===> obss Protection disabled"));)
            psessionEntry->beaconParams.gHTObssMode = false;
            pBeaconParams->paramChangeBitmap |= PARAM_OBSS_MODE_CHANGED;

        }
    }
    return eSIR_SUCCESS;
}
/*                                                               
                           
                                                            
                                                                           
                                                                                             
                                              
                 
                                                               */
tSirRetStatus
limEnableHT20Protection(tpAniSirGlobal pMac, tANI_U8 enable,
    tANI_U8 overlap, tpUpdateBeaconParams pBeaconParams,tpPESession psessionEntry)
{
    if(!psessionEntry->htCapability)
        return eSIR_SUCCESS; //                                          

        //                                           
        if(overlap)
        {
        }
        else
        {
            //                              
            if((psessionEntry->limSystemRole == eLIM_AP_ROLE ) &&
                !psessionEntry->cfgProtection.ht20)
            {
                //                     
                PELOG3(limLog(pMac, LOG3, FL("protection from HT20 is disabled"));)
                return eSIR_SUCCESS;
            }else if(psessionEntry->limSystemRole != eLIM_AP_ROLE )
            {
                if(!pMac->lim.cfgProtection.ht20)
                {
                    //                     
                    PELOG3(limLog(pMac, LOG3, FL("protection from HT20 is disabled"));)
                    return eSIR_SUCCESS;
                }
            }
        }

    if (enable)
    {
        //                                                          
        //              

        if(eLIM_AP_ROLE == psessionEntry->limSystemRole){
            if(overlap)
            {
                psessionEntry->gLimOverlapHt20Params.protectionEnabled = true;
                if((eSIR_HT_OP_MODE_OVERLAP_LEGACY != psessionEntry->htOperMode) &&
                    (eSIR_HT_OP_MODE_MIXED != psessionEntry->htOperMode))
                {
                    psessionEntry->htOperMode = eSIR_HT_OP_MODE_OVERLAP_LEGACY;
                    limEnableHtRifsProtection(pMac, true, overlap, pBeaconParams,psessionEntry);
                }
            }
            else
            {
               psessionEntry->gLimHt20Params.protectionEnabled = true;
                if(eSIR_HT_OP_MODE_PURE == psessionEntry->htOperMode)
                {
                    //                                        
                    //                                                               
                    psessionEntry->htOperMode = eSIR_HT_OP_MODE_PURE;
                    limEnableHtRifsProtection(pMac, false, overlap, pBeaconParams,psessionEntry);
                    limEnableHtOBSSProtection(pMac,  false, overlap, pBeaconParams,psessionEntry);
                }
            }
        }else if(eLIM_BT_AMP_AP_ROLE == psessionEntry->limSystemRole)
        {
            if(overlap)
            {
                pMac->lim.gLimOverlapHt20Params.protectionEnabled = true;
                if((eSIR_HT_OP_MODE_OVERLAP_LEGACY != pMac->lim.gHTOperMode) &&
                    (eSIR_HT_OP_MODE_MIXED != pMac->lim.gHTOperMode))
                {
                    pMac->lim.gHTOperMode = eSIR_HT_OP_MODE_OVERLAP_LEGACY;
                    limEnableHtRifsProtection(pMac, true, overlap, pBeaconParams,psessionEntry);
                }
            }
            else
            {
                psessionEntry->gLimHt20Params.protectionEnabled = true;
                if(eSIR_HT_OP_MODE_PURE == pMac->lim.gHTOperMode)
                {
                    pMac->lim.gHTOperMode = eSIR_HT_OP_MODE_NO_LEGACY_20MHZ_HT;
                    limEnableHtRifsProtection(pMac, false, overlap, pBeaconParams,psessionEntry);
                    limEnableHtOBSSProtection(pMac,  false, overlap, pBeaconParams,psessionEntry);
                }
            }
        }

        //                                        
        if(false == psessionEntry->beaconParams.ht20Coexist)
        {
            PELOG1(limLog(pMac, LOG1, FL("=> Prtection from HT20 Enabled"));)
            pBeaconParams->ht20MhzCoexist = psessionEntry->beaconParams.ht20Coexist = true;
            pBeaconParams->paramChangeBitmap |= PARAM_HT20MHZCOEXIST_CHANGED;
        }
    }
    else if (true == psessionEntry->beaconParams.ht20Coexist)
    {
        //            
        //                                                    
        //                                      
        if(eLIM_AP_ROLE == psessionEntry->limSystemRole){
            if(overlap)
            {
                //                                   
                psessionEntry->gLimOverlapHt20Params.protectionEnabled = false;

                //                                                               
                if(!(psessionEntry->gLimOlbcParams.protectionEnabled ||
                    psessionEntry->gLimOverlap11gParams.protectionEnabled ||
                    psessionEntry->gLimOverlapHt20Params.protectionEnabled ||
                    psessionEntry->gLimOverlapNonGfParams.protectionEnabled))
                {

                    //                                              
                    if(eSIR_HT_OP_MODE_OVERLAP_LEGACY == psessionEntry->htOperMode)
                    {
                        if(psessionEntry->gLimHt20Params.protectionEnabled)
                        {
                            //                                           
                            //                                                               
                            psessionEntry->htOperMode = eSIR_HT_OP_MODE_PURE;
                            limEnableHtRifsProtection(pMac, false, overlap, pBeaconParams,psessionEntry);
                            limEnableHtOBSSProtection(pMac,  false, overlap, pBeaconParams,psessionEntry);            
                        }
                        else
                        {
                            psessionEntry->htOperMode = eSIR_HT_OP_MODE_PURE;
                        }
                    }
                }
            }
            else
            {
                //                                     
                psessionEntry->gLimHt20Params.protectionEnabled = false;

                //                                
                if(eSIR_HT_OP_MODE_NO_LEGACY_20MHZ_HT == psessionEntry->htOperMode)
                {
                    psessionEntry->htOperMode = eSIR_HT_OP_MODE_PURE;
                    limEnableHtRifsProtection(pMac, false, overlap, pBeaconParams,psessionEntry);
                    limEnableHtOBSSProtection(pMac,  false, overlap, pBeaconParams,psessionEntry);        
                }
            }
            PELOG1(limLog(pMac, LOG1, FL("===> Protection from HT 20 Disabled"));)
            pBeaconParams->ht20MhzCoexist = psessionEntry->beaconParams.ht20Coexist = false;
            pBeaconParams->paramChangeBitmap |= PARAM_HT20MHZCOEXIST_CHANGED;
        }else if(eLIM_BT_AMP_AP_ROLE == psessionEntry->limSystemRole)
        {
            if(overlap)
            {
                //                                   
                pMac->lim.gLimOverlapHt20Params.protectionEnabled = false;

                //                                                               
                if(!(psessionEntry->gLimOlbcParams.protectionEnabled ||
                    pMac->lim.gLimOverlap11gParams.protectionEnabled ||
                    pMac->lim.gLimOverlapHt20Params.protectionEnabled ||
                    pMac->lim.gLimOverlapNonGfParams.protectionEnabled))
                {

                    //                                              
                    if(eSIR_HT_OP_MODE_OVERLAP_LEGACY == pMac->lim.gHTOperMode)
                    {
                        if(psessionEntry->gLimHt20Params.protectionEnabled)
                        {
                            pMac->lim.gHTOperMode = eSIR_HT_OP_MODE_NO_LEGACY_20MHZ_HT;
                            limEnableHtRifsProtection(pMac, false, overlap, pBeaconParams,psessionEntry);
                            limEnableHtOBSSProtection(pMac,  false, overlap, pBeaconParams,psessionEntry);            
                        }
                        else
                        {
                            pMac->lim.gHTOperMode = eSIR_HT_OP_MODE_PURE;
                        }
                    }
                }
            }
            else
            {
                //                                     
                psessionEntry->gLimHt20Params.protectionEnabled = false;

                //                                
                if(eSIR_HT_OP_MODE_NO_LEGACY_20MHZ_HT == pMac->lim.gHTOperMode)
                {
                    pMac->lim.gHTOperMode = eSIR_HT_OP_MODE_PURE;
                    limEnableHtRifsProtection(pMac, false, overlap, pBeaconParams,psessionEntry);
                    limEnableHtOBSSProtection(pMac,  false, overlap, pBeaconParams,psessionEntry);        
                }
            }
            PELOG1(limLog(pMac, LOG1, FL("===> Protection from HT 20 Disabled"));)
            pBeaconParams->ht20MhzCoexist = psessionEntry->beaconParams.ht20Coexist = false;
            pBeaconParams->paramChangeBitmap |= PARAM_HT20MHZCOEXIST_CHANGED;
        }
        //                
        else
        {
            PELOG1(limLog(pMac, LOG1, FL("===> Protection from HT20 Disabled"));)
            pBeaconParams->ht20MhzCoexist = psessionEntry->beaconParams.ht20Coexist = false;
            pBeaconParams->paramChangeBitmap |= PARAM_HT20MHZCOEXIST_CHANGED;
        }
    }

    return eSIR_SUCCESS;
}

/*                                                               
                              
                                                             
                                                                           
                                                                                             
                                              
                 
                                                               */
tSirRetStatus
limEnableHTNonGfProtection(tpAniSirGlobal pMac, tANI_U8 enable,
    tANI_U8 overlap, tpUpdateBeaconParams pBeaconParams,tpPESession psessionEntry)
{
    if(!psessionEntry->htCapability)
        return eSIR_SUCCESS; //                                          

        //                                           
        if(overlap)
        {
        }
        else
        {
            //                              
            if((psessionEntry->limSystemRole == eLIM_AP_ROLE ) &&
                !psessionEntry->cfgProtection.nonGf)
            {
                //                     
                PELOG3(limLog(pMac, LOG3, FL("protection from NonGf is disabled"));)
                return eSIR_SUCCESS;
            }else if(psessionEntry->limSystemRole != eLIM_AP_ROLE)
            {
                //                              
                if(!pMac->lim.cfgProtection.nonGf)
                {
                    //                     
                    PELOG3(limLog(pMac, LOG3, FL("protection from NonGf is disabled"));)
                    return eSIR_SUCCESS;
                 }
            }
        }
    if(psessionEntry->limSystemRole == eLIM_AP_ROLE){
        if ((enable) && (false == psessionEntry->beaconParams.llnNonGFCoexist))
        {
            PELOG1(limLog(pMac, LOG1, FL(" => Prtection from non GF Enabled"));)
            pBeaconParams->llnNonGFCoexist = psessionEntry->beaconParams.llnNonGFCoexist = true;
            pBeaconParams->paramChangeBitmap |= PARAM_NON_GF_DEVICES_PRESENT_CHANGED;
        }
        else if (!enable && (true == psessionEntry->beaconParams.llnNonGFCoexist))
        {
            PELOG1(limLog(pMac, LOG1, FL("===> Protection from Non GF Disabled"));)
            pBeaconParams->llnNonGFCoexist = psessionEntry->beaconParams.llnNonGFCoexist = false;
            pBeaconParams->paramChangeBitmap |= PARAM_NON_GF_DEVICES_PRESENT_CHANGED;
        }
    }else
    {
        if ((enable) && (false == psessionEntry->beaconParams.llnNonGFCoexist))
        {
            PELOG1(limLog(pMac, LOG1, FL(" => Prtection from non GF Enabled"));)
            pBeaconParams->llnNonGFCoexist = psessionEntry->beaconParams.llnNonGFCoexist = true;
            pBeaconParams->paramChangeBitmap |= PARAM_NON_GF_DEVICES_PRESENT_CHANGED;
        }
        else if (!enable && (true == psessionEntry->beaconParams.llnNonGFCoexist))
        {
            PELOG1(limLog(pMac, LOG1, FL("===> Protection from Non GF Disabled"));)
            pBeaconParams->llnNonGFCoexist = psessionEntry->beaconParams.llnNonGFCoexist = false;
            pBeaconParams->paramChangeBitmap |= PARAM_NON_GF_DEVICES_PRESENT_CHANGED;
        }
    }

    return eSIR_SUCCESS;
}

/*                                                               
                                 
                                                           
                                                                           
                                                                                             
                                              
                 
                                                               */
tSirRetStatus
limEnableHTLsigTxopProtection(tpAniSirGlobal pMac, tANI_U8 enable,
    tANI_U8 overlap, tpUpdateBeaconParams pBeaconParams,tpPESession psessionEntry)
{
    if(!psessionEntry->htCapability)
        return eSIR_SUCCESS; //                                          

        //                                           
        if(overlap)
        {
        }
        else
        {
            //                              
            if((psessionEntry->limSystemRole == eLIM_AP_ROLE ) &&
               !psessionEntry->cfgProtection.lsigTxop)
            {
                //                     
                PELOG3(limLog(pMac, LOG3, FL(" protection from LsigTxop not supported is disabled"));)
                return eSIR_SUCCESS;
            }else if(psessionEntry->limSystemRole != eLIM_AP_ROLE)
            {
                //                              
                if(!pMac->lim.cfgProtection.lsigTxop)
                {
                    //                     
                    PELOG3(limLog(pMac, LOG3, FL(" protection from LsigTxop not supported is disabled"));)
                    return eSIR_SUCCESS;
                }
            }
        }


    if(psessionEntry->limSystemRole == eLIM_AP_ROLE){
        if ((enable) && (false == psessionEntry->beaconParams.fLsigTXOPProtectionFullSupport))
        {
            PELOG1(limLog(pMac, LOG1, FL(" => Prtection from LsigTxop Enabled"));)
            pBeaconParams->fLsigTXOPProtectionFullSupport = psessionEntry->beaconParams.fLsigTXOPProtectionFullSupport = true;
            pBeaconParams->paramChangeBitmap |= PARAM_LSIG_TXOP_FULL_SUPPORT_CHANGED;
        }
        else if (!enable && (true == psessionEntry->beaconParams.fLsigTXOPProtectionFullSupport))
        {
            PELOG1(limLog(pMac, LOG1, FL("===> Protection from LsigTxop Disabled"));)
            pBeaconParams->fLsigTXOPProtectionFullSupport= psessionEntry->beaconParams.fLsigTXOPProtectionFullSupport = false;
            pBeaconParams->paramChangeBitmap |= PARAM_LSIG_TXOP_FULL_SUPPORT_CHANGED;
        }
    }else
    {
        if ((enable) && (false == psessionEntry->beaconParams.fLsigTXOPProtectionFullSupport))
        {
            PELOG1(limLog(pMac, LOG1, FL(" => Prtection from LsigTxop Enabled"));)
            pBeaconParams->fLsigTXOPProtectionFullSupport = psessionEntry->beaconParams.fLsigTXOPProtectionFullSupport = true;
            pBeaconParams->paramChangeBitmap |= PARAM_LSIG_TXOP_FULL_SUPPORT_CHANGED;
        }
        else if (!enable && (true == psessionEntry->beaconParams.fLsigTXOPProtectionFullSupport))
        {
            PELOG1(limLog(pMac, LOG1, FL("===> Protection from LsigTxop Disabled"));)
            pBeaconParams->fLsigTXOPProtectionFullSupport= psessionEntry->beaconParams.fLsigTXOPProtectionFullSupport = false;
            pBeaconParams->paramChangeBitmap |= PARAM_LSIG_TXOP_FULL_SUPPORT_CHANGED;
        }
    }
    return eSIR_SUCCESS;
}
//                                                                                        
//                                      
/*                                                               
                             
                                                       
                                                                           
                                                                                             
                                              
                 
                                                               */
tSirRetStatus
limEnableHtRifsProtection(tpAniSirGlobal pMac, tANI_U8 enable,
    tANI_U8 overlap, tpUpdateBeaconParams pBeaconParams,tpPESession psessionEntry)
{
    if(!psessionEntry->htCapability)
        return eSIR_SUCCESS; //                                          


        //                                           
        if(overlap)
        {
        }
        else
        {
             //                              
            if((psessionEntry->limSystemRole == eLIM_AP_ROLE) &&
               !psessionEntry->cfgProtection.rifs)
            {
                //                     
                PELOG3(limLog(pMac, LOG3, FL(" protection from Rifs is disabled"));)
                return eSIR_SUCCESS;
            }else if(psessionEntry->limSystemRole != eLIM_AP_ROLE )
            {
               //                              
               if(!pMac->lim.cfgProtection.rifs)
               {
                  //                     
                  PELOG3(limLog(pMac, LOG3, FL(" protection from Rifs is disabled"));)
                  return eSIR_SUCCESS;
               }
            }
        }

    if(psessionEntry->limSystemRole == eLIM_AP_ROLE){
        //                                                                                 
        if ((!enable) && (false == psessionEntry->beaconParams.fRIFSMode))
        {
            PELOG1(limLog(pMac, LOG1, FL(" => Rifs protection Disabled"));)
            pBeaconParams->fRIFSMode = psessionEntry->beaconParams.fRIFSMode = true;
            pBeaconParams->paramChangeBitmap |= PARAM_RIFS_MODE_CHANGED;
        }
        //                                                                                 
        else if (enable && (true == psessionEntry->beaconParams.fRIFSMode))
        {
            PELOG1(limLog(pMac, LOG1, FL("===> Rifs Protection Enabled"));)
            pBeaconParams->fRIFSMode = psessionEntry->beaconParams.fRIFSMode = false;
            pBeaconParams->paramChangeBitmap |= PARAM_RIFS_MODE_CHANGED;
        }
    }else
    {
        //                                                                                 
        if ((!enable) && (false == psessionEntry->beaconParams.fRIFSMode))
        {
            PELOG1(limLog(pMac, LOG1, FL(" => Rifs protection Disabled"));)
            pBeaconParams->fRIFSMode = psessionEntry->beaconParams.fRIFSMode = true;
            pBeaconParams->paramChangeBitmap |= PARAM_RIFS_MODE_CHANGED;
        }
    //                                                                                 
        else if (enable && (true == psessionEntry->beaconParams.fRIFSMode))
        {
            PELOG1(limLog(pMac, LOG1, FL("===> Rifs Protection Enabled"));)
            pBeaconParams->fRIFSMode = psessionEntry->beaconParams.fRIFSMode = false;
            pBeaconParams->paramChangeBitmap |= PARAM_RIFS_MODE_CHANGED;
        }
    }
    return eSIR_SUCCESS;
}

//                                                                      
/* 
                         
  
            
                                
  
         
  
               
  
        
  
                                                             
               
 */

tSirRetStatus
limEnableShortPreamble(tpAniSirGlobal pMac, tANI_U8 enable, tpUpdateBeaconParams pBeaconParams, tpPESession psessionEntry)
{
    tANI_U32 val;

    if (wlan_cfgGetInt(pMac, WNI_CFG_SHORT_PREAMBLE, &val) != eSIR_SUCCESS)
    {
        /*                                                                */
        limLog(pMac, LOGP, FL("could not retrieve short preamble flag"));
        return eSIR_FAILURE;
    }

    if (!val)  
        return eSIR_SUCCESS;

    if (wlan_cfgGetInt(pMac, WNI_CFG_11G_SHORT_PREAMBLE_ENABLED, &val) != eSIR_SUCCESS)
    {
        limLog(pMac, LOGP, FL("could not retrieve 11G short preamble switching  enabled flag"));
        return eSIR_FAILURE;
    }

    if (!val)   //                                          
        return eSIR_SUCCESS;

    if ( psessionEntry->limSystemRole == eLIM_AP_ROLE )
    {
        if (enable && (psessionEntry->beaconParams.fShortPreamble == 0))
        {
            PELOG1(limLog(pMac, LOG1, FL("===> Short Preamble Enabled"));)
            psessionEntry->beaconParams.fShortPreamble = true;
            pBeaconParams->fShortPreamble = (tANI_U8) psessionEntry->beaconParams.fShortPreamble;
            pBeaconParams->paramChangeBitmap |= PARAM_SHORT_PREAMBLE_CHANGED;
        }
        else if (!enable && (psessionEntry->beaconParams.fShortPreamble == 1))
        {
            PELOG1(limLog(pMac, LOG1, FL("===> Short Preamble Disabled"));)
            psessionEntry->beaconParams.fShortPreamble = false;
            pBeaconParams->fShortPreamble = (tANI_U8) psessionEntry->beaconParams.fShortPreamble;
            pBeaconParams->paramChangeBitmap |= PARAM_SHORT_PREAMBLE_CHANGED;
        }
    }

    return eSIR_SUCCESS;
        }

/* 
                
  
            
                                                                      
  
         
                                                        
                                                                    
                                              
                                                                 
                                                              
                                                                   
                                                              
                                                                               
                                                                              
  
               
                                                             
        
                                                                          
                                                                          
                                            
  
                       
                                                                              
                                                           
  
                                                            
  
               
 */
void limTxComplete( tHalHandle hHal, void *pData )
{
  tpAniSirGlobal pMac;
  pMac = (tpAniSirGlobal)hHal;

#ifdef FIXME_PRIMA
  /*                                                                  */
#ifdef TRACE_RECORD
    {
        tpSirMacMgmtHdr mHdr;
        v_U8_t         *pRxBd;
        vos_pkt_t      *pVosPkt;
        VOS_STATUS      vosStatus;



        pVosPkt = (vos_pkt_t *)pData;
        vosStatus = vos_pkt_peek_data( pVosPkt, 0, (v_PVOID_t *)&pRxBd, WLANHAL_RX_BD_HEADER_SIZE);

        if(VOS_IS_STATUS_SUCCESS(vosStatus))
        {
            mHdr = WDA_GET_RX_MAC_HEADER(pRxBd);
            MTRACE(macTrace(pMac, TRACE_CODE_TX_COMPLETE, NO_SESSION, mHdr->fc.subType);)

        }   
    }
#endif
#endif

  palPktFree( pMac->hHdd,
              HAL_TXRX_FRM_802_11_MGMT,
              (void *) NULL,           //                                                         
              (void *) pData );        //                                                                                    
}

/* 
                                                                                 
                                                              
                         
                                                                                        
                                   
                                                                                       
                                                                                                 
  
  
                                              
  
                                                                      
                 
  
                                                                     
  
  
 */

void limUpdateStaRunTimeHTSwitchChnlParams( tpAniSirGlobal   pMac,
                                  tDot11fIEHTInfo *pHTInfo,
                                  tANI_U8          bssIdx,
                                  tpPESession      psessionEntry)
{
    ePhyChanBondState secondaryChnlOffset = PHY_SINGLE_CHANNEL_CENTERED;
#if !defined WLAN_FEATURE_VOWIFI  
    tANI_U32 localPwrConstraint;
#endif
    
   //                                                                          
   if( !limGetHTCapability( pMac, eHT_SUPPORTED_CHANNEL_WIDTH_SET, psessionEntry ))
        return;

#if !defined WLAN_FEATURE_VOWIFI  
    if(wlan_cfgGetInt(pMac, WNI_CFG_LOCAL_POWER_CONSTRAINT, &localPwrConstraint) != eSIR_SUCCESS) {
        limLog( pMac, LOGP, FL( "Unable to get Local Power Constraint from cfg" ));
        return;
    }
#endif

    if ( psessionEntry->htSecondaryChannelOffset != ( tANI_U8 ) pHTInfo->secondaryChannelOffset ||
         psessionEntry->htRecommendedTxWidthSet  != ( tANI_U8 ) pHTInfo->recommendedTxWidthSet )
    {
        psessionEntry->htSecondaryChannelOffset = ( ePhyChanBondState ) pHTInfo->secondaryChannelOffset;
        psessionEntry->htRecommendedTxWidthSet  = ( tANI_U8 ) pHTInfo->recommendedTxWidthSet;
        if ( eHT_CHANNEL_WIDTH_40MHZ == psessionEntry->htRecommendedTxWidthSet )
            secondaryChnlOffset = (ePhyChanBondState)pHTInfo->secondaryChannelOffset;

        //           
        limLog( pMac, LOGW,  FL( "Channel Information in HT IE change"
                                 "d; sending notification to HAL." ) );
        limLog( pMac, LOGW,  FL( "Primary Channel: %d, Secondary Chan"
                                 "nel Offset: %d, Channel Width: %d" ),
                pHTInfo->primaryChannel, secondaryChnlOffset,
                psessionEntry->htRecommendedTxWidthSet );
        psessionEntry->channelChangeReasonCode=LIM_SWITCH_CHANNEL_OPERATION;
        pMac->lim.gpchangeChannelCallback = NULL;
        pMac->lim.gpchangeChannelData = NULL;

#if defined WLAN_FEATURE_VOWIFI  
        limSendSwitchChnlParams( pMac, ( tANI_U8 ) pHTInfo->primaryChannel,
                                 secondaryChnlOffset, psessionEntry->maxTxPower, psessionEntry->peSessionId);
#else
        limSendSwitchChnlParams( pMac, ( tANI_U8 ) pHTInfo->primaryChannel,
                                 secondaryChnlOffset, (tPowerdBm)localPwrConstraint, psessionEntry->peSessionId);
#endif

        //                                                                
       if (eLIM_STA_IN_IBSS_ROLE == psessionEntry->limSystemRole)
        {
            schSetFixedBeaconFields(pMac,psessionEntry);
        }

    }
} //                                 

/* 
                                                                       
                                
  
  
                                              
  
                                                                    
                                           
  
  
  
 */

void limUpdateStaRunTimeHTCapability( tpAniSirGlobal   pMac,
                                      tDot11fIEHTCaps *pHTCaps )
{

    if ( pMac->lim.gHTLsigTXOPProtection != ( tANI_U8 ) pHTCaps->lsigTXOPProtection )
    {
        pMac->lim.gHTLsigTXOPProtection = ( tANI_U8 ) pHTCaps->lsigTXOPProtection;
       //                                
    }

    if ( pMac->lim.gHTAMpduDensity != ( tANI_U8 ) pHTCaps->mpduDensity )
    {
       pMac->lim.gHTAMpduDensity = ( tANI_U8 ) pHTCaps->mpduDensity;
       //                                
    }

    if ( pMac->lim.gHTMaxRxAMpduFactor != ( tANI_U8 ) pHTCaps->maxRxAMPDUFactor )
    {
       pMac->lim.gHTMaxRxAMpduFactor = ( tANI_U8 ) pHTCaps->maxRxAMPDUFactor;
       //                                
    }


} //                                     

/* 
                                                                      
                                
  
  
                                                  
  
                                                                     
                 
  
  
 */

void limUpdateStaRunTimeHTInfo( tpAniSirGlobal  pMac,
                                tDot11fIEHTInfo *pHTInfo, tpPESession psessionEntry)
{
    if ( psessionEntry->htRecommendedTxWidthSet != ( tANI_U8 )pHTInfo->recommendedTxWidthSet )
    {
        psessionEntry->htRecommendedTxWidthSet = ( tANI_U8 )pHTInfo->recommendedTxWidthSet;
        //                                
    }

    if ( psessionEntry->beaconParams.fRIFSMode != ( tANI_U8 )pHTInfo->rifsMode )
    {
        psessionEntry->beaconParams.fRIFSMode = ( tANI_U8 )pHTInfo->rifsMode;
        //                                
    }

    if ( pMac->lim.gHTServiceIntervalGranularity != ( tANI_U8 )pHTInfo->serviceIntervalGranularity )
    {
        pMac->lim.gHTServiceIntervalGranularity = ( tANI_U8 )pHTInfo->serviceIntervalGranularity;
        //                                
    }

    if ( pMac->lim.gHTOperMode != ( tSirMacHTOperatingMode )pHTInfo->opMode )
    {
        pMac->lim.gHTOperMode = ( tSirMacHTOperatingMode )pHTInfo->opMode;
        //                                
    }

    if ( psessionEntry->beaconParams.llnNonGFCoexist != pHTInfo->nonGFDevicesPresent )
    {
        psessionEntry->beaconParams.llnNonGFCoexist = ( tANI_U8 )pHTInfo->nonGFDevicesPresent;
    }

    if ( pMac->lim.gHTSTBCBasicMCS != ( tANI_U8 )pHTInfo->basicSTBCMCS )
    {
        pMac->lim.gHTSTBCBasicMCS = ( tANI_U8 )pHTInfo->basicSTBCMCS;
        //                                
    }

    if ( pMac->lim.gHTDualCTSProtection != ( tANI_U8 )pHTInfo->dualCTSProtection )
    {
        pMac->lim.gHTDualCTSProtection = ( tANI_U8 )pHTInfo->dualCTSProtection;
        //                                
    }

    if ( pMac->lim.gHTSecondaryBeacon != ( tANI_U8 )pHTInfo->secondaryBeacon )
    {
        pMac->lim.gHTSecondaryBeacon = ( tANI_U8 )pHTInfo->secondaryBeacon;
        //                                
    }

    if ( psessionEntry->beaconParams.fLsigTXOPProtectionFullSupport != ( tANI_U8 )pHTInfo->lsigTXOPProtectionFullSupport )
    {
        psessionEntry->beaconParams.fLsigTXOPProtectionFullSupport = ( tANI_U8 )pHTInfo->lsigTXOPProtectionFullSupport;
        //                                
    }

    if ( pMac->lim.gHTPCOActive != ( tANI_U8 )pHTInfo->pcoActive )
    {
        pMac->lim.gHTPCOActive = ( tANI_U8 )pHTInfo->pcoActive;
        //                                
    }

    if ( pMac->lim.gHTPCOPhase != ( tANI_U8 )pHTInfo->pcoPhase )
    {
        pMac->lim.gHTPCOPhase = ( tANI_U8 )pHTInfo->pcoPhase;
        //                                
    }

} //                               


/*                                                               
                            
                                           
                            
                         
                         
                             
                                                               */

tSirRetStatus limProcessHalIndMessages(tpAniSirGlobal pMac, tANI_U32 msgId, void *msgParam )
{
  //                                                                                         
  tSirMsgQ msg;

  switch(msgId)
  {
    case SIR_LIM_DEL_TS_IND:
    case SIR_LIM_ADD_BA_IND:    
    case SIR_LIM_DEL_BA_ALL_IND:
    case SIR_LIM_DELETE_STA_CONTEXT_IND:        
    case SIR_LIM_BEACON_GEN_IND:
      msg.type = (tANI_U16) msgId;
      msg.bodyptr = msgParam;
      msg.bodyval = 0;
      break;

    default:
      vos_mem_free(msgParam);
      limLog(pMac, LOGP, FL("invalid message id = %d received"), msgId);
      return eSIR_FAILURE;
  }

  if (limPostMsgApi(pMac, &msg) != eSIR_SUCCESS)
  {
    vos_mem_free(msgParam);
    limLog(pMac, LOGP, FL("limPostMsgApi failed for msgid = %d"), msg.type);
    return eSIR_FAILURE;
  }
  return eSIR_SUCCESS;
}

/*                                                               
                       
                                                                                         
                            
                                  
                                
                              
                                                               */

tSirRetStatus
limValidateDeltsReq(tpAniSirGlobal pMac, tpSirDeltsReq pDeltsReq, tSirMacAddr peerMacAddr,tpPESession psessionEntry)
{
    tpDphHashNode pSta;
    tANI_U8            tsStatus;
    tSirMacTSInfo *tsinfo;
    tANI_U32 i;
    tANI_U8 tspecIdx;
    /*       
                            
                           
             
                                      
                               
     */
    if(pDeltsReq == NULL)
    {
      PELOGE(limLog(pMac, LOGE, FL("Delete TS request pointer is NULL"));)
      return eSIR_FAILURE;
    }

    if ((psessionEntry->limSystemRole == eLIM_STA_ROLE)||(psessionEntry->limSystemRole == eLIM_BT_AMP_STA_ROLE))
    {
        tANI_U32 val;

        //                               
        pSta = dphGetHashEntry(pMac, DPH_STA_HASH_INDEX_PEER, &psessionEntry->dph.dphHashTable);

        val = sizeof(tSirMacAddr);
        #if 0
        if (wlan_cfgGetStr(pMac, WNI_CFG_BSSID, peerMacAddr, &val) != eSIR_SUCCESS)
        {
            //                                          
            limLog(pMac, LOGP, FL("could not retrieve BSSID"));
            return eSIR_FAILURE;
        }
       #endif//                  
       sirCopyMacAddr(peerMacAddr,psessionEntry->bssId);
       
    }
    else
    {
        tANI_U16 assocId;
        tANI_U8 *macaddr = (tANI_U8 *) peerMacAddr;

        assocId = pDeltsReq->aid;
        if (assocId != 0)
            pSta = dphGetHashEntry(pMac, assocId, &psessionEntry->dph.dphHashTable);
        else
            pSta = dphLookupHashEntry(pMac, pDeltsReq->macAddr, &assocId, &psessionEntry->dph.dphHashTable);

        if (pSta != NULL)
            //                                   
            for (i =0; i < sizeof(tSirMacAddr); i++)
                macaddr[i] = pSta->staAddr[i];
    }

    if (pSta == NULL)
    {
        PELOGE(limLog(pMac, LOGE, "Cannot find station context for delts req");)
        return eSIR_FAILURE;
    }

    if ((! pSta->valid) ||
        (pSta->mlmStaContext.mlmState != eLIM_MLM_LINK_ESTABLISHED_STATE))
    {
        PELOGE(limLog(pMac, LOGE, "Invalid Sta (or state) for DelTsReq");)
        return eSIR_FAILURE;
    }

    pDeltsReq->req.wsmTspecPresent = 0;
    pDeltsReq->req.wmeTspecPresent = 0;
    pDeltsReq->req.lleTspecPresent = 0;

    if ((pSta->wsmEnabled) &&
        (pDeltsReq->req.tspec.tsinfo.traffic.accessPolicy != SIR_MAC_ACCESSPOLICY_EDCA))
        pDeltsReq->req.wsmTspecPresent = 1;
    else if (pSta->wmeEnabled)
        pDeltsReq->req.wmeTspecPresent = 1;
    else if (pSta->lleEnabled)
        pDeltsReq->req.lleTspecPresent = 1;
    else
    {
        PELOGW(limLog(pMac, LOGW, FL("DELTS_REQ ignore - qos is disabled"));)
        return eSIR_FAILURE;
    }

    tsinfo = pDeltsReq->req.wmeTspecPresent ? &pDeltsReq->req.tspec.tsinfo
                                            : &pDeltsReq->req.tsinfo;
   PELOG1(limLog(pMac, LOG1,
           FL("received DELTS_REQ message (wmeTspecPresent = %d, lleTspecPresent = %d, wsmTspecPresent = %d, tsid %d,  up %d, direction = %d)"),
           pDeltsReq->req.wmeTspecPresent, pDeltsReq->req.lleTspecPresent, pDeltsReq->req.wsmTspecPresent,
           tsinfo->traffic.tsid, tsinfo->traffic.userPrio, tsinfo->traffic.direction);)

       //                                         

    if (limAdmitControlDeleteTS(pMac, pSta->assocId, tsinfo, &tsStatus, &tspecIdx)
        != eSIR_SUCCESS)
    {
       PELOGE(limLog(pMac, LOGE, "ERROR DELTS request for sta assocId %d (tsid %d, up %d)",
               pSta->assocId, tsinfo->traffic.tsid, tsinfo->traffic.userPrio);)
        return eSIR_FAILURE;
    }
    else if ((tsinfo->traffic.accessPolicy == SIR_MAC_ACCESSPOLICY_HCCA) ||
             (tsinfo->traffic.accessPolicy == SIR_MAC_ACCESSPOLICY_BOTH))
    {
      //              
    }
    else
    {
      if((tsinfo->traffic.accessPolicy == SIR_MAC_ACCESSPOLICY_EDCA) && 
           psessionEntry->gLimEdcaParams[upToAc(tsinfo->traffic.userPrio)].aci.acm)
      {
        //                                
        if(eSIR_SUCCESS != limSendHalMsgDelTs(pMac,
                                              pSta->staIndex,
                                              tspecIdx,
                                              pDeltsReq->req,
                                              psessionEntry->peSessionId,
                                              psessionEntry->bssId))
        {
          limLog(pMac, LOGW, FL("DelTs with UP %d failed in limSendHalMsgDelTs - ignoring request"),
                           tsinfo->traffic.userPrio);
           return eSIR_FAILURE;
        }
      }
    }
    return eSIR_SUCCESS;
}

/*                                                               
                             
                                                             
                            
             
                                                               */
void
limRegisterHalIndCallBack(tpAniSirGlobal pMac)
{
    tSirMsgQ msg;
    tpHalIndCB pHalCB;

    pHalCB = vos_mem_malloc(sizeof(tHalIndCB));
    if ( NULL == pHalCB )
    {
       limLog(pMac, LOGP, FL("AllocateMemory() failed"));
       return;
    }

    pHalCB->pHalIndCB = limProcessHalIndMessages;

    msg.type = WDA_REGISTER_PE_CALLBACK;
    msg.bodyptr = pHalCB;
    msg.bodyval = 0;
    
    MTRACE(macTraceMsgTx(pMac, NO_SESSION, msg.type));
    if(eSIR_SUCCESS != wdaPostCtrlMsg(pMac, &msg))
    {
        vos_mem_free(pHalCB);
        limLog(pMac, LOGP, FL("wdaPostCtrlMsg() failed"));
    }
    
    return;
}


/*                                                               
                      

                                                                        
                                                                                                           
                            
                       
            
                                                             */
void
limProcessAddBaInd(tpAniSirGlobal pMac, tpSirMsgQ limMsg)
{
    tANI_U8             i;
    tANI_U8             tid;
    tANI_U16            assocId;
    tpDphHashNode       pSta;
    tpAddBaCandidate    pBaCandidate;
    tANI_U32            baCandidateCnt;
    tpBaActivityInd     pBaActivityInd;
    tpPESession         psessionEntry;
    tANI_U8             sessionId;
#ifdef FEATURE_WLAN_TDLS
    boolean             htCapable = FALSE;
#endif
    

    if (limMsg->bodyptr == NULL)
        return;
    
    pBaActivityInd = (tpBaActivityInd)limMsg->bodyptr;
    baCandidateCnt = pBaActivityInd->baCandidateCnt;

    if ((psessionEntry = peFindSessionByBssid(pMac,pBaActivityInd->bssId,&sessionId))== NULL)
    {
        limLog(pMac, LOGE,FL("session does not exist for given BSSId"));
        vos_mem_free(limMsg->bodyptr);
        limMsg->bodyptr = NULL;
        return;
    }
       
    //                                                                                
#ifdef FEATURE_WLAN_TDLS
    if ((baCandidateCnt  > pMac->lim.maxStation))
#else
    if ((baCandidateCnt  > pMac->lim.maxStation) || !psessionEntry->htCapability )
#endif
    {
        vos_mem_free(limMsg->bodyptr);
        limMsg->bodyptr = NULL;
        return;
    }

#ifdef FEATURE_WLAN_TDLS
    //                                                                                         
    //             
    pBaCandidate =  (tpAddBaCandidate) (((tANI_U8*)pBaActivityInd) + sizeof(tBaActivityInd));

    for (i=0; i<baCandidateCnt; i++, pBaCandidate++)
    {
       pSta = dphLookupHashEntry(pMac, pBaCandidate->staAddr, &assocId, &psessionEntry->dph.dphHashTable);
       if ((NULL == pSta) || (!pSta->valid))
           continue;

       if (STA_ENTRY_TDLS_PEER == pSta->staType)
           htCapable = pSta->mlmStaContext.htCapability;
       else
           htCapable = psessionEntry->htCapability;

       if (htCapable)
           break;
    }
    if (!htCapable)
    {
        vos_mem_free(limMsg->bodyptr);
        limMsg->bodyptr = NULL;
        return;
    }
#endif
  
    //                                             
    limDeleteDialogueTokenList(pMac);
    pBaCandidate =  (tpAddBaCandidate) (((tANI_U8*)pBaActivityInd) + sizeof(tBaActivityInd));

    for (i=0; i<baCandidateCnt; i++, pBaCandidate++)
    {
       pSta = dphLookupHashEntry(pMac, pBaCandidate->staAddr, &assocId, &psessionEntry->dph.dphHashTable);
       if ((NULL == pSta) || (!pSta->valid))
           continue;

        for (tid=0; tid<STACFG_MAX_TC; tid++)
        {
            if((eBA_DISABLE == pSta->tcCfg[tid].fUseBATx) &&
                 (pBaCandidate->baInfo[tid].fBaEnable))
            {
                limLog(pMac, LOGE, FL("BA setup for staId = %d, TID: %d, SSN: %d"),
                        pSta->staIndex, tid, pBaCandidate->baInfo[tid].startingSeqNum);
                limPostMlmAddBAReq(pMac, pSta, tid, pBaCandidate->baInfo[tid].startingSeqNum,psessionEntry);  
            }
        }
    }
    vos_mem_free(limMsg->bodyptr);
    limMsg->bodyptr = NULL;
    return;
}


/*                                                               
                            
                                                               
                          
                            
                                  
                             
             
                                                             */

void 
limDeleteBASessions(tpAniSirGlobal pMac, tpPESession pSessionEntry,
                    tANI_U32 baDirection)
{
    tANI_U32 i;
    tANI_U8 tid;
    tpDphHashNode pSta;

    if (NULL == pSessionEntry)
    {
        limLog(pMac, LOGE, FL("Session does not exist"));
    }
    else
    {
        for(tid = 0; tid < STACFG_MAX_TC; tid++)
        {
            if ((eLIM_AP_ROLE == pSessionEntry->limSystemRole) ||
                (pSessionEntry->limSystemRole == eLIM_BT_AMP_AP_ROLE) ||
                (eLIM_STA_IN_IBSS_ROLE == pSessionEntry->limSystemRole) ||
                (pSessionEntry->limSystemRole == eLIM_P2P_DEVICE_GO))
            {
                for (i = 0; i < pMac->lim.maxStation; i++)
                {
                    pSta = pSessionEntry->dph.dphHashTable.pDphNodeArray + i;
                    if (pSta && pSta->added)
                    {
                        if ((eBA_ENABLE == pSta->tcCfg[tid].fUseBATx) &&
                                       (baDirection & BA_INITIATOR))
                        {
                            limPostMlmDelBAReq(pMac, pSta, eBA_INITIATOR, tid,
                                               eSIR_MAC_UNSPEC_FAILURE_REASON,
                                               pSessionEntry);
                        }
                        if ((eBA_ENABLE == pSta->tcCfg[tid].fUseBARx) &&
                                        (baDirection & BA_RECIPIENT))
                        {
                            limPostMlmDelBAReq(pMac, pSta, eBA_RECIPIENT, tid,
                                               eSIR_MAC_UNSPEC_FAILURE_REASON,
                                               pSessionEntry);
                        }
                    }
                }
            }
            else if ((eLIM_STA_ROLE == pSessionEntry->limSystemRole) ||
                     (eLIM_BT_AMP_STA_ROLE == pSessionEntry->limSystemRole) ||
                     (eLIM_P2P_DEVICE_ROLE == pSessionEntry->limSystemRole))
            {
                pSta = dphGetHashEntry(pMac, DPH_STA_HASH_INDEX_PEER,
                                       &pSessionEntry->dph.dphHashTable);
                if (pSta && pSta->added)
                {
                    if ((eBA_ENABLE == pSta->tcCfg[tid].fUseBATx) &&
                                    (baDirection & BA_INITIATOR))
                    {
                        limPostMlmDelBAReq(pMac, pSta, eBA_INITIATOR, tid,
                                           eSIR_MAC_UNSPEC_FAILURE_REASON,
                                           pSessionEntry);
                    }
                    if ((eBA_ENABLE == pSta->tcCfg[tid].fUseBARx) &&
                                    (baDirection & BA_RECIPIENT))
                    {
                        limPostMlmDelBAReq(pMac, pSta, eBA_RECIPIENT, tid,
                                           eSIR_MAC_UNSPEC_FAILURE_REASON,
                                           pSessionEntry);
                    }
                }
            }
        }
    }
}

/*                                                               
                           
                                             
                           
            
                                                             */

void limDelAllBASessions(tpAniSirGlobal pMac)
{
    tANI_U32 i;
    tpPESession pSessionEntry;

    for (i = 0; i < pMac->lim.maxBssId; i++)
    {
        pSessionEntry = peFindSessionBySessionId(pMac, i);
        if (pSessionEntry)
        {
            limDeleteBASessions(pMac, pSessionEntry, BA_BOTH_DIRECTIONS);
        }
    }
}

/*                                                               
                              
                                                                
             
                           
            
                                                             */

void limDelPerBssBASessionsBtc(tpAniSirGlobal pMac)
{
    tANI_U8 sessionId;
    tpPESession pSessionEntry;
    pSessionEntry = peFindSessionByBssid(pMac,pMac->btc.btcBssfordisableaggr,
                                                                &sessionId);
    if (pSessionEntry)
    {
        PELOGW(limLog(pMac, LOGW,
        "Deleting the BA for session %d as host got BTC event", sessionId);)
        limDeleteBASessions(pMac, pSessionEntry, BA_RECIPIENT);
    }
}

/*                                                               
                      
                                                                                                     
                                                                                                           
                            
                       
            
                                                             */
void
limProcessDelTsInd(tpAniSirGlobal pMac, tpSirMsgQ limMsg)
{
  tpDphHashNode         pSta;
  tpDelTsParams         pDelTsParam = (tpDelTsParams) (limMsg->bodyptr);
  tpSirDeltsReq         pDelTsReq = NULL;
  tSirMacAddr           peerMacAddr;
  tpSirDeltsReqInfo     pDelTsReqInfo;
  tpLimTspecInfo        pTspecInfo;
  tpPESession           psessionEntry;
  tANI_U8               sessionId;  

if((psessionEntry = peFindSessionByBssid(pMac,pDelTsParam->bssId,&sessionId))== NULL)
    {
         limLog(pMac, LOGE,FL("session does not exist for given BssId"));
         vos_mem_free(limMsg->bodyptr);
         limMsg->bodyptr = NULL;
         return;
    }

  pTspecInfo = &(pMac->lim.tspecInfo[pDelTsParam->tspecIdx]);
  if(pTspecInfo->inuse == false)
  {
    PELOGE(limLog(pMac, LOGE, FL("tspec entry with index %d is not in use"), pDelTsParam->tspecIdx);)
    goto error1;
  }

  pSta = dphGetHashEntry(pMac, pTspecInfo->assocId, &psessionEntry->dph.dphHashTable);
  if(pSta == NULL)
  {
    limLog(pMac, LOGE, FL("Could not find entry in DPH table for assocId = %d"),
                pTspecInfo->assocId);
    goto error1;
  }

  pDelTsReq = vos_mem_malloc(sizeof(tSirDeltsReq));
  if ( NULL == pDelTsReq )
  {
     PELOGE(limLog(pMac, LOGE, FL("AllocateMemory() failed"));)
     goto error1;
  }

  vos_mem_set( (tANI_U8 *)pDelTsReq, sizeof(tSirDeltsReq), 0);

  if(pSta->wmeEnabled)
    vos_mem_copy( &(pDelTsReq->req.tspec), &(pTspecInfo->tspec), sizeof(tSirMacTspecIE));
  else
    vos_mem_copy( &(pDelTsReq->req.tsinfo), &(pTspecInfo->tspec.tsinfo), sizeof(tSirMacTSInfo));


  //                
  if (eSIR_SUCCESS != limValidateDeltsReq(pMac, pDelTsReq, peerMacAddr,psessionEntry))
  {
    PELOGE(limLog(pMac, LOGE, FL("limValidateDeltsReq failed"));)
    goto error2;
  }
  PELOG1(limLog(pMac, LOG1, "Sent DELTS request to station with "
         "assocId = %d MacAddr = "MAC_ADDRESS_STR,
         pDelTsReq->aid, MAC_ADDR_ARRAY(peerMacAddr));)

  limSendDeltsReqActionFrame(pMac, peerMacAddr, pDelTsReq->req.wmeTspecPresent, &pDelTsReq->req.tsinfo, &pDelTsReq->req.tspec,
          psessionEntry);

  //                                          
  pDelTsReqInfo = vos_mem_malloc(sizeof(tSirDeltsReqInfo));
  if ( NULL == pDelTsReqInfo )
  {
     PELOGE(limLog(pMac, LOGE, FL("AllocateMemory() failed"));)
     goto error3;
  }
  vos_mem_set( (tANI_U8 *)pDelTsReqInfo, sizeof(tSirDeltsReqInfo), 0);

  if(pSta->wmeEnabled)
    vos_mem_copy( &(pDelTsReqInfo->tspec), &(pTspecInfo->tspec), sizeof(tSirMacTspecIE));
  else
    vos_mem_copy( &(pDelTsReqInfo->tsinfo), &(pTspecInfo->tspec.tsinfo), sizeof(tSirMacTSInfo));

  limSendSmeDeltsInd(pMac, pDelTsReqInfo, pDelTsReq->aid,psessionEntry);

error3:
  vos_mem_free(pDelTsReqInfo);
error2:
  vos_mem_free(pDelTsReq);
error1:
  vos_mem_free(limMsg->bodyptr);
  limMsg->bodyptr = NULL;
  return;
}

/* 
                                    
  
                         
  
                                               
  
                                                 
  
                                                
                                                        
                                          
  
                                                       
                                                      
 */
tSirRetStatus limPostMlmAddBAReq( tpAniSirGlobal pMac,
    tpDphHashNode pStaDs,
    tANI_U8 tid, tANI_U16 startingSeqNum,tpPESession psessionEntry)
{
    tSirRetStatus status = eSIR_SUCCESS;
    tpLimMlmAddBAReq pMlmAddBAReq = NULL;
    tpDialogueToken dialogueTokenNode;
    tANI_U32        val = 0;

  //                                       
  //                                           
  //                                        
  //                                
    //                    

  //                               
  pMlmAddBAReq = vos_mem_malloc(sizeof( tLimMlmAddBAReq ));
  if ( NULL == pMlmAddBAReq )
  {
    limLog( pMac, LOGP, FL("AllocateMemory failed"));
    status = eSIR_MEM_ALLOC_FAILED;
    goto returnFailure;
  }

  vos_mem_set( (void *) pMlmAddBAReq, sizeof( tLimMlmAddBAReq ), 0);

  //                  
  vos_mem_copy(
      pMlmAddBAReq->peerMacAddr,
      pStaDs->staAddr,
      sizeof( tSirMacAddr ));

  //               
  pMlmAddBAReq->baTID = tid;

  //                                               
  //                        
  pMlmAddBAReq->baPolicy = (pStaDs->baPolicyFlag >> tid) & 0x1;

  //               
  //                                                     
  //                                                    
  //                             
  if ((TRUE == psessionEntry->isCiscoVendorAP) &&
        (eHT_CHANNEL_WIDTH_80MHZ != pStaDs->htSupportedChannelWidthSet))
  {
      /*                                                              
                                                   */
      limLog( pMac, LOGW,
          FL( "Requesting ADDBA with Cisco 1225 AP, window size 25"));
      pMlmAddBAReq->baBufferSize = MAX_BA_WINDOW_SIZE_FOR_CISCO;
  }
  else
      pMlmAddBAReq->baBufferSize = 0;

  limLog( pMac, LOGW,
      FL( "Requesting an ADDBA to setup a %s BA session with STA %d for TID %d" ),
      (pMlmAddBAReq->baPolicy ? "Immediate": "Delayed"),
      pStaDs->staIndex,
      tid );

  //           
  if (wlan_cfgGetInt(pMac, WNI_CFG_BA_TIMEOUT, &val) != eSIR_SUCCESS)
  {
     limLog(pMac, LOGE, FL("could not retrieve BA TIME OUT Param CFG"));
     status = eSIR_FAILURE;
     goto returnFailure;
  }
  pMlmAddBAReq->baTimeout = val; //        

  //                      
  //                                              
  //                                                                                                              
  pMlmAddBAReq->addBAFailureTimeout = 2000; //        

  //                            
  pMlmAddBAReq->baSSN = startingSeqNum;

  /*                     */
  pMlmAddBAReq->sessionId = psessionEntry->peSessionId;

  LIM_SET_STA_BA_STATE(pStaDs, tid, eLIM_BA_STATE_WT_ADD_RSP);

  dialogueTokenNode = limAssignDialogueToken(pMac);
  if (NULL == dialogueTokenNode)
  {
     limLog(pMac, LOGE, FL("could not assign dialogue token"));
     status = eSIR_FAILURE;
     goto returnFailure;
  }

  pMlmAddBAReq->baDialogToken = dialogueTokenNode->token;
  //                                                      
  dialogueTokenNode->assocId = pStaDs->assocId;
  dialogueTokenNode->tid = tid;
  //                       
  limPostMlmMessage( pMac,
      LIM_MLM_ADDBA_REQ,
      (tANI_U32 *) pMlmAddBAReq );
  return eSIR_SUCCESS;

returnFailure:
  vos_mem_free(pMlmAddBAReq);
  return status;
}

/* 
                                              
                                                 
                                         
  
                         
  
                                               
  
                                                          
                                     
  
                                            
  
                                              
  
                               
  
                                
  
                                         
  
                                      
  
                                                       
                                                      
 */
tSirRetStatus limPostMlmAddBARsp( tpAniSirGlobal pMac,
    tSirMacAddr peerMacAddr,
    tSirMacStatusCodes baStatusCode,
    tANI_U8 baDialogToken,
    tANI_U8 baTID,
    tANI_U8 baPolicy,
    tANI_U16 baBufferSize,
    tANI_U16 baTimeout,
    tpPESession psessionEntry)
{
tSirRetStatus status = eSIR_SUCCESS;
tpLimMlmAddBARsp pMlmAddBARsp;

  //                               
  pMlmAddBARsp = vos_mem_malloc(sizeof( tLimMlmAddBARsp ));
  if ( NULL == pMlmAddBARsp )
  {
    limLog( pMac, LOGE,
        FL("AllocateMemory failed with error code %d"),
        status );

    status = eSIR_MEM_ALLOC_FAILED;
    goto returnFailure;
  }

  vos_mem_set( (void *) pMlmAddBARsp, sizeof( tLimMlmAddBARsp ), 0);

  //                  
  vos_mem_copy(
      pMlmAddBARsp->peerMacAddr,
      peerMacAddr,
      sizeof( tSirMacAddr ));

  pMlmAddBARsp->baDialogToken = baDialogToken;
  pMlmAddBARsp->addBAResultCode = baStatusCode;
  pMlmAddBARsp->baTID = baTID;
  pMlmAddBARsp->baPolicy = baPolicy;
  pMlmAddBARsp->baBufferSize = baBufferSize;
  pMlmAddBARsp->baTimeout = baTimeout;

  /*                     */
  pMlmAddBARsp->sessionId = psessionEntry->peSessionId;

  //                       
  limPostMlmMessage( pMac,
      LIM_MLM_ADDBA_RSP,
      (tANI_U32 *) pMlmAddBARsp );

returnFailure:

  return status;
}

/* 
                                              
                                                 
                                         
  
                         
  
                                               
  
                                                      
                                            
  
                                     
  
                                                             
  
                                            
  
                                                       
                                                      
 */
tSirRetStatus limPostMlmDelBAReq( tpAniSirGlobal pMac,
    tpDphHashNode pSta,
    tANI_U8 baDirection,
    tANI_U8 baTID,
    tSirMacReasonCodes baReasonCode,
    tpPESession psessionEntry)
{
tSirRetStatus status = eSIR_SUCCESS;
tpLimMlmDelBAReq pMlmDelBAReq;
tLimBAState curBaState;

if(NULL == pSta)
    return eSIR_FAILURE;

LIM_GET_STA_BA_STATE(pSta, baTID, &curBaState);

  //                                       
  if( eLIM_BA_STATE_IDLE != curBaState)
  {
    limLog( pMac, LOGE,
        FL( "Received unexpected DELBA REQ when STA BA state for tid = %d is %d" ),
        baTID,
        curBaState);

    status = eSIR_FAILURE;
    goto returnFailure;
  }

  //                               
  pMlmDelBAReq = vos_mem_malloc(sizeof( tLimMlmDelBAReq ));
  if ( NULL == pMlmDelBAReq )
  {
    limLog( pMac, LOGE,
        FL("AllocateMemory failed with error code %d"),
        status );

    status = eSIR_MEM_ALLOC_FAILED;
    goto returnFailure;
  }

  vos_mem_set( (void *) pMlmDelBAReq, sizeof( tLimMlmDelBAReq ), 0);

  //                  
  vos_mem_copy(
      pMlmDelBAReq->peerMacAddr,
      pSta->staAddr,
      sizeof( tSirMacAddr ));

  pMlmDelBAReq->baDirection = baDirection;
  pMlmDelBAReq->baTID = baTID;
  pMlmDelBAReq->delBAReasonCode = baReasonCode;

  /*                     */
  pMlmDelBAReq->sessionId = psessionEntry->peSessionId;

  //                                                        
  //                                                               
  //                                                     
  if(((eBA_RECIPIENT == baDirection) && (eBA_DISABLE == pSta->tcCfg[baTID].fUseBARx)) ||
      ((eBA_INITIATOR == baDirection) && (eBA_DISABLE == pSta->tcCfg[baTID].fUseBATx)))
  {
        //                            
        if( eSIR_SUCCESS !=
            (status = limSendDelBAInd( pMac, pMlmDelBAReq,psessionEntry)))
          status = eSIR_FAILURE;
  
        vos_mem_free(pMlmDelBAReq);
        return status;
  }


  //                           
  LIM_SET_STA_BA_STATE(pSta, pMlmDelBAReq->baTID, eLIM_BA_STATE_WT_DEL_RSP);

  //                       
  limPostMlmMessage( pMac,
      LIM_MLM_DELBA_REQ,
      (tANI_U32 *) pMlmDelBAReq );

returnFailure:

  return status;
}

/* 
                                             
                                        
  
                         
  
                                               
  
                                                        
                         
  
                                                     
  
                                                           
  
                            
  
                                                   
  
                                                                  
  
                                                            
  
                                                                
  
               
  
 */
tSirRetStatus limPostMsgAddBAReq( tpAniSirGlobal pMac,
    tpDphHashNode pSta,
    tANI_U8 baDialogToken,
    tANI_U8 baTID,
    tANI_U8 baPolicy,
    tANI_U16 baBufferSize,
    tANI_U16 baTimeout,
    tANI_U16 baSSN,
    tANI_U8 baDirection, 
    tpPESession psessionEntry)
{
tpAddBAParams pAddBAParams = NULL;
tSirRetStatus retCode = eSIR_SUCCESS;
tSirMsgQ msgQ;

#ifdef WLAN_SOFTAP_VSTA_FEATURE
  //                                 
  if (!(IS_HWSTA_IDX(pSta->staIndex)))
  {
    retCode = eHAL_STATUS_FAILURE;
    goto returnFailure;
  }
#endif //                        

  //                           
  pAddBAParams = vos_mem_malloc(sizeof( tAddBAParams ));
  if ( NULL == pAddBAParams )
  {
    limLog( pMac, LOGE,
        FL("AllocateMemory failed")
         );

    retCode = eSIR_MEM_ALLOC_FAILED;
    goto returnFailure;
  }

  vos_mem_set( (void *) pAddBAParams, sizeof( tAddBAParams ), 0);

  //                          
  vos_mem_copy(
      (void *) pAddBAParams->peerMacAddr,
      (void *) pSta->staAddr,
      sizeof( tSirMacAddr ));

  //                            
  pAddBAParams->staIdx = pSta->staIndex;
  pAddBAParams->baDialogToken = baDialogToken;
  pAddBAParams->baTID = baTID;
  pAddBAParams->baPolicy = baPolicy;
  pAddBAParams->baBufferSize = baBufferSize;
  pAddBAParams->baTimeout = baTimeout;
  pAddBAParams->baSSN = baSSN;
  pAddBAParams->baDirection = baDirection;
  pAddBAParams->respReqd = 1;

  /*                      */
  pAddBAParams->sessionId = psessionEntry->peSessionId;

  //                           
  msgQ.type = WDA_ADDBA_REQ;
  //
  //            
  //                                                             
  //                                
  //
  msgQ.reserved = 0;
  msgQ.bodyptr = pAddBAParams;
  msgQ.bodyval = 0;

  limLog( pMac, LOGW,
      FL( "Sending WDA_ADDBA_REQ..." ));

  //                                                   
  SET_LIM_PROCESS_DEFD_MESGS(pMac, false);

  MTRACE(macTraceMsgTx(pMac, psessionEntry->peSessionId, msgQ.type));
#ifdef FEATURE_WLAN_DIAG_SUPPORT_LIM //                          
    limDiagEventReport(pMac, WLAN_PE_DIAG_HAL_ADDBA_REQ_EVENT, psessionEntry, 0, 0);
#endif //                         
  
  if( eSIR_SUCCESS != (retCode = wdaPostCtrlMsg( pMac, &msgQ )))
    limLog( pMac, LOGE,
        FL("Posting WDA_ADDBA_REQ to HAL failed! Reason = %d"),
        retCode );
  else
    return retCode;

returnFailure:

  //            
  if( NULL != pAddBAParams )
    vos_mem_free( pAddBAParams );

  return retCode;

}

/* 
                                             
                                             
  
                         
  
                                               
  
                                                        
                         
  
                                                           
  
                                                          
                                        
  
               
  
 */
tSirRetStatus limPostMsgDelBAInd( tpAniSirGlobal pMac,
    tpDphHashNode pSta,
    tANI_U8 baTID,
    tANI_U8 baDirection,
    tpPESession psessionEntry)
{
tpDelBAParams pDelBAParams = NULL;
tSirRetStatus retCode = eSIR_SUCCESS;
tSirMsgQ msgQ;

  //                               
  pDelBAParams = vos_mem_malloc(sizeof( tDelBAParams ));
  if ( NULL == pDelBAParams )
  {
    limLog( pMac, LOGE,
        FL("AllocateMemory failed")
        );

    retCode = eSIR_MEM_ALLOC_FAILED;
    goto returnFailure;
  }

  vos_mem_set( (void *) pDelBAParams, sizeof( tDelBAParams ), 0);

  //                            
  pDelBAParams->staIdx = pSta->staIndex;
  pDelBAParams->baTID = baTID;
  pDelBAParams->baDirection = baDirection;

  /*                      */
  

  //                                                                           
  //                                                     

  //                           
  msgQ.type = WDA_DELBA_IND;
  //
  //       
  //                                                             
  //                                
  //
  msgQ.reserved = 0;
  msgQ.bodyptr = pDelBAParams;
  msgQ.bodyval = 0;

  limLog( pMac, LOGW,
      FL( "Sending SIR_HAL_DELBA_IND..." ));

  MTRACE(macTraceMsgTx(pMac, psessionEntry->peSessionId, msgQ.type));
#ifdef FEATURE_WLAN_DIAG_SUPPORT_LIM //                          
    limDiagEventReport(pMac, WLAN_PE_DIAG_HAL_DELBA_IND_EVENT, psessionEntry, 0, 0);
#endif //                         

  if( eSIR_SUCCESS != (retCode = wdaPostCtrlMsg( pMac, &msgQ )))
    limLog( pMac, LOGE,
        FL("Posting WDA_DELBA_IND to HAL failed! Reason = %d"),
        retCode );
  else
  {
    //                               
    if( eBA_INITIATOR == baDirection)
    {
      pSta->tcCfg[baTID].fUseBATx = 0;
      pSta->tcCfg[baTID].txBufSize = 0;
    }
    else
    {
      pSta->tcCfg[baTID].fUseBARx = 0;
      pSta->tcCfg[baTID].rxBufSize = 0;
    }
  
    return retCode;
  }

returnFailure:

  //            
  if( NULL != pDelBAParams )
    vos_mem_free( pDelBAParams );

  return retCode;

}

/* 
                                       
  
                                                                                                 
  
              
  
                    
              
  
             
              
  
                                                 
                                                                             
               
 */
tSirRetStatus 
limPostSMStateUpdate(tpAniSirGlobal pMac, 
        tANI_U16 staIdx, tSirMacHTMIMOPowerSaveState state)
{
    tSirRetStatus             retCode = eSIR_SUCCESS;
    tSirMsgQ                    msgQ;
    tpSetMIMOPS            pMIMO_PSParams;

    msgQ.reserved = 0;
    msgQ.type = WDA_SET_MIMOPS_REQ;

    //                                
    pMIMO_PSParams = vos_mem_malloc(sizeof(tSetMIMOPS));
    if ( NULL == pMIMO_PSParams )
    {
        limLog( pMac, LOGP,FL(" AllocateMemory failed"));
        return eSIR_MEM_ALLOC_FAILED;
    }

    pMIMO_PSParams->htMIMOPSState = state;
    pMIMO_PSParams->staIdx = staIdx;
    pMIMO_PSParams->fsendRsp = true;
    msgQ.bodyptr = pMIMO_PSParams;
    msgQ.bodyval = 0;

    limLog( pMac, LOG2, FL( "Sending WDA_SET_MIMOPS_REQ..." ));

    MTRACE(macTraceMsgTx(pMac, NO_SESSION, msgQ.type));
    retCode = wdaPostCtrlMsg( pMac, &msgQ );
    if (eSIR_SUCCESS != retCode)
    {
        limLog( pMac, LOGP, FL("Posting WDA_SET_MIMOPS_REQ to HAL failed! Reason = %d"), retCode );
        vos_mem_free(pMIMO_PSParams);
        return retCode;
    }
    
    return retCode;
}

void limPktFree (
    tpAniSirGlobal  pMac,
    eFrameType      frmType,
    tANI_U8         *pRxPacketInfo,
    void            *pBody)
{
    (void) pMac; (void) frmType; (void) pRxPacketInfo; (void) pBody;
}

/* 
                         
  
           
                                                    
                                                          
                                                   
  
        
     
  
              
     
  
       
     
  
                                          
                                                     
               
 */

void
limGetBDfromRxPacket(tpAniSirGlobal pMac, void *body, tANI_U32 **pRxPacketInfo)
{
    *pRxPacketInfo = (tANI_U32 *) body;
} /*                                */





void limRessetScanChannelInfo(tpAniSirGlobal pMac)
{
    vos_mem_set(&pMac->lim.scanChnInfo, sizeof(tLimScanChnInfo), 0);
}


void limAddScanChannelInfo(tpAniSirGlobal pMac, tANI_U8 channelId)
{
    tANI_U8 i;
    tANI_BOOLEAN fFound = eANI_BOOLEAN_FALSE;

    for(i = 0; i < pMac->lim.scanChnInfo.numChnInfo; i++)
    {
        if(pMac->lim.scanChnInfo.scanChn[i].channelId == channelId)
        {
            pMac->lim.scanChnInfo.scanChn[i].numTimeScan++;
            fFound = eANI_BOOLEAN_TRUE;
            break;
        }
    }
    if(eANI_BOOLEAN_FALSE == fFound)
    {
        if(pMac->lim.scanChnInfo.numChnInfo < SIR_MAX_SUPPORTED_CHANNEL_LIST)
        {
            pMac->lim.scanChnInfo.scanChn[pMac->lim.scanChnInfo.numChnInfo].channelId = channelId;
            pMac->lim.scanChnInfo.scanChn[pMac->lim.scanChnInfo.numChnInfo++].numTimeScan = 1;
        }
        else
        {
            PELOGW(limLog(pMac, LOGW, FL(" -- number of channels exceed mac"));)
        }
    }
}


/* 
                                                    
  
                                                             
                                                                   
              
  
                    
              
  
             
              
  
                                                 
                                                                 
               
 */
tAniBool 
limIsChannelValidForChannelSwitch(tpAniSirGlobal pMac, tANI_U8 channel)
{
    tANI_U8  index;
    tANI_U32    validChannelListLen = WNI_CFG_VALID_CHANNEL_LIST_LEN;
    tSirMacChanNum   validChannelList[WNI_CFG_VALID_CHANNEL_LIST_LEN];

    if (wlan_cfgGetStr(pMac, WNI_CFG_VALID_CHANNEL_LIST,
          (tANI_U8 *)validChannelList,
          (tANI_U32 *)&validChannelListLen) != eSIR_SUCCESS)
    {
        PELOGE(limLog(pMac, LOGE, FL("could not retrieve valid channel list"));)
        return (eSIR_FALSE);
    }

    for(index = 0; index < validChannelListLen; index++)
    {
        if(validChannelList[index] == channel)
            return (eSIR_TRUE);
    }

    /*                                                   */
    return (eSIR_FALSE);
}

/*                                                       
                                
                                                  

            
                                                   
                                                    
                              
                                                        */
static eHalStatus
__limFillTxControlParams(tpAniSirGlobal pMac, tpTxControlParams  pTxCtrlMsg,
                                        tLimQuietTxMode type, tLimControlTx mode)
{

    //                                       
    tpPESession psessionEntry = &pMac->lim.gpSession[0];
    
    if (mode == eLIM_STOP_TX)
        pTxCtrlMsg->stopTx =  eANI_BOOLEAN_TRUE;
    else
        pTxCtrlMsg->stopTx =  eANI_BOOLEAN_FALSE;
    
    switch (type)
    {
        case eLIM_TX_ALL:
            /*                                        */
            pTxCtrlMsg->fCtrlGlobal = 1;
            break;

        case eLIM_TX_BSS_BUT_BEACON:
            /*                                                                      
                                          
              */
            pTxCtrlMsg->ctrlBss = 1;
            pTxCtrlMsg->bssBitmap    |= (1 << psessionEntry->bssIdx);
            break;

        case eLIM_TX_STA:
            /*                                                                      
                                                                                  
                              
              */
        case eLIM_TX_BSS:
            //            
        default:
            PELOGW(limLog(pMac, LOGW, FL("Invalid case: Not Handled"));)
            return eHAL_STATUS_FAILURE;
    }

    return eHAL_STATUS_SUCCESS;
}

/* 
                                              
  
                                                                     
                                                                     
                                                               
                                       
              
  
                    
              
  
             
              
  
                                                 
               
 */
void limFrameTransmissionControl(tpAniSirGlobal pMac, tLimQuietTxMode type, tLimControlTx mode)
{

    eHalStatus status = eHAL_STATUS_FAILURE;
    tpTxControlParams pTxCtrlMsg;
    tSirMsgQ          msgQ;
    tANI_U8           nBytes = 0;  //                                         

    /*                                                           
                                            */
    nBytes = (tANI_U8)HALMSG_NUMBYTES_STATION_BITMAP(pMac->lim.maxStation);

    pTxCtrlMsg = vos_mem_malloc(sizeof(*pTxCtrlMsg) + nBytes);
    if ( NULL == pTxCtrlMsg )
    {
        limLog(pMac, LOGP, FL("AllocateMemory() failed"));
        return;
    }

    vos_mem_set((void *) pTxCtrlMsg,
               (sizeof(*pTxCtrlMsg) + nBytes), 0);
    status = __limFillTxControlParams(pMac, pTxCtrlMsg, type, mode);
    if (status != eHAL_STATUS_SUCCESS)
    {
        vos_mem_free(pTxCtrlMsg);
        limLog(pMac, LOGP, FL("__limFillTxControlParams failed, status = %d"), status);
        return;
    }
    
    msgQ.bodyptr = (void *) pTxCtrlMsg;
    msgQ.bodyval = 0;
    msgQ.reserved = 0;
    msgQ.type = WDA_TRANSMISSION_CONTROL_IND;

    MTRACE(macTraceMsgTx(pMac, NO_SESSION, msgQ.type));
    if(wdaPostCtrlMsg( pMac, &msgQ) != eSIR_SUCCESS)
    {
        vos_mem_free(pTxCtrlMsg);
        limLog( pMac, LOGP, FL("Posting Message to HAL failed"));
        return;
    }

    if (mode == eLIM_STOP_TX)
        {
            PELOG1(limLog(pMac, LOG1, FL("Stopping the transmission of all packets, indicated softmac"));)
        }
    else
        {
            PELOG1(limLog(pMac, LOG1, FL("Resuming the transmission of all packets, indicated softmac"));)
        }
    return;
}


/* 
                                                  
  
                                                         
                                                        
                             
              
  
                    
              
  
             
              
  
                                                 
               
 */

tSirRetStatus 
limRestorePreChannelSwitchState(tpAniSirGlobal pMac, tpPESession psessionEntry)
{

    tSirRetStatus retCode = eSIR_SUCCESS;
    tANI_U32      val = 0;

    if (psessionEntry->limSystemRole != eLIM_STA_ROLE)
        return retCode;
    
    /*                                                  */
    psessionEntry->gLimSpecMgmt.dot11hChanSwState = eLIM_11H_CHANSW_INIT;

    /*                                               */
    limFrameTransmissionControl(pMac, eLIM_TX_ALL, eLIM_RESUME_TX);

    /*                    */
    limSendSmePostChannelSwitchInd(pMac);

    //                                         
    if(pMac->lim.gLimBackgroundScanTerminate == FALSE)
    {       
        /*                                                              */
        if ((retCode = wlan_cfgGetInt(pMac, WNI_CFG_BACKGROUND_SCAN_PERIOD,
                      &val)) != eSIR_SUCCESS)

        {
            limLog(pMac, LOGP, FL("could not retrieve Background scan period value"));
            return (retCode);   
        }

        if (val > 0 && TX_TIMER_VALID(pMac->lim.limTimers.gLimBackgroundScanTimer))
        {
            MTRACE(macTrace(pMac, TRACE_CODE_TIMER_ACTIVATE,
                     psessionEntry->peSessionId, eLIM_BACKGROUND_SCAN_TIMER));
            if(tx_timer_activate(&pMac->lim.limTimers.gLimBackgroundScanTimer) != TX_SUCCESS)
            {
                limLog(pMac, LOGP, FL("Could not restart background scan timer, doing LOGP"));
                return (eSIR_FAILURE);
            }

        }
    }

    /*                        */
    if (TX_TIMER_VALID(pMac->lim.limTimers.gLimHeartBeatTimer))
    {
        MTRACE(macTrace(pMac, TRACE_CODE_TIMER_ACTIVATE,
                 psessionEntry->peSessionId, eLIM_HEART_BEAT_TIMER));
        if((limActivateHearBeatTimer(pMac, psessionEntry) != TX_SUCCESS) &&
              (!IS_ACTIVEMODE_OFFLOAD_FEATURE_ENABLE))
        {
            limLog(pMac, LOGP, FL("Could not restart heartbeat timer, doing LOGP"));
            return (eSIR_FAILURE);
        }
    }
    return (retCode);
}


/*                                             
                                 
                                    

           
            
                                             */
tSirRetStatus limRestorePreQuietState(tpAniSirGlobal pMac, tpPESession psessionEntry)
{

    tSirRetStatus retCode = eSIR_SUCCESS;
    tANI_U32      val = 0;
 
    if (pMac->lim.gLimSystemRole != eLIM_STA_ROLE)
             return retCode;
 
    /*                                         */
    psessionEntry->gLimSpecMgmt.quietState = eLIM_QUIET_INIT;

    /*                                               */
    if (psessionEntry->gLimSpecMgmt.quietState == eLIM_QUIET_RUNNING)
        limFrameTransmissionControl(pMac, eLIM_TX_ALL, eLIM_RESUME_TX);


    //                                     
    if(pMac->lim.gLimBackgroundScanTerminate == FALSE)
    {     
        /*                                                              */
        if ((retCode = wlan_cfgGetInt(pMac, WNI_CFG_BACKGROUND_SCAN_PERIOD,
                      &val)) != eSIR_SUCCESS)

        {
            limLog(pMac, LOGP, FL("could not retrieve Background scan period value"));
            return (retCode);   
        }

        if (val > 0 && TX_TIMER_VALID(pMac->lim.limTimers.gLimBackgroundScanTimer))
        {
            MTRACE(macTrace(pMac, TRACE_CODE_TIMER_ACTIVATE, psessionEntry->peSessionId, eLIM_BACKGROUND_SCAN_TIMER));
            if(tx_timer_activate(&pMac->lim.limTimers.gLimBackgroundScanTimer) != TX_SUCCESS)
            {
                limLog(pMac, LOGP, FL("Could not restart background scan timer, doing LOGP"));
                return (eSIR_FAILURE);
            }

        }
    }

    /*                        */
    if (TX_TIMER_VALID(pMac->lim.limTimers.gLimHeartBeatTimer))
    {
        MTRACE(macTrace(pMac, TRACE_CODE_TIMER_ACTIVATE, psessionEntry->peSessionId, eLIM_HEART_BEAT_TIMER));
        if(limActivateHearBeatTimer(pMac, psessionEntry) != TX_SUCCESS)
        {
            limLog(pMac, LOGP, FL("Could not restart heartbeat timer, doing LOGP"));
            return (eSIR_FAILURE);
        }
    }
    return (retCode);
}


/* 
                                              
  
                                                           
                                                          
                                                           
                                 
              
  
                    
              
  
             
              
  
                                                 
                        
               
 */
void 
limPrepareFor11hChannelSwitch(tpAniSirGlobal pMac, tpPESession psessionEntry)
{
    if (psessionEntry->limSystemRole != eLIM_STA_ROLE)
        return;
     
    /*                                                 */
    psessionEntry->gLimSpecMgmt.dot11hChanSwState = eLIM_11H_CHANSW_RUNNING;

    /*                                                      */
    limDeactivateAndChangeTimer(pMac, eLIM_BACKGROUND_SCAN_TIMER);

    /*                                                        */
    limHeartBeatDeactivateAndChangeTimer(pMac, psessionEntry);

    if(pMac->lim.gLimSmeState == eLIM_SME_LINK_EST_WT_SCAN_STATE ||
        pMac->lim.gLimSmeState == eLIM_SME_CHANNEL_SCAN_STATE)
    {
        PELOGE(limLog(pMac, LOGE, FL("Posting finish scan as we are in scan state"));)
        /*                              */
        if (GET_LIM_PROCESS_DEFD_MESGS(pMac))
        {
            //                                                       
            //                                                               
            peSetResumeChannel(pMac, 0, 0);
            limSendHalFinishScanReq(pMac, eLIM_HAL_FINISH_SCAN_WAIT_STATE);
        }
        else
        {
            limRestorePreChannelSwitchState(pMac, psessionEntry);
        }
        return;
    }
    else
    {
        PELOGE(limLog(pMac, LOGE, FL("Not in scan state, start channel switch timer"));)
        /*                                              */
        limStopTxAndSwitchChannel(pMac, psessionEntry->peSessionId);
    }
}



/*                                                     
                       

                                                            
           
                                  
                             
                                                    

                           
                                                     */
tSirNwType limGetNwType(tpAniSirGlobal pMac, tANI_U8 channelNum, tANI_U32 type, tpSchBeaconStruct pBeacon)
{
    tSirNwType nwType = eSIR_11B_NW_TYPE;

    if (type == SIR_MAC_DATA_FRAME)
    {
        if ((channelNum > 0) && (channelNum < 15))
        {
            nwType = eSIR_11G_NW_TYPE;
        }
        else
        {
            nwType = eSIR_11A_NW_TYPE;
        }
    }
    else
    {
        if ((channelNum > 0) && (channelNum < 15))
        {
            int i;
            //                  
            //                                       
            //                                     
            for (i = 0; i < pBeacon->supportedRates.numRates; i++)
            {
                if (sirIsArate(pBeacon->supportedRates.rate[i] & 0x7f))
                {
                    nwType = eSIR_11G_NW_TYPE;
                    break;
                }
            }
            if (pBeacon->extendedRatesPresent)
            {
                PELOG3(limLog(pMac, LOG3, FL("Beacon, nwtype=G"));)
                nwType = eSIR_11G_NW_TYPE;
            }
        }
        else
        {
            //           
            PELOG3(limLog(pMac, LOG3,FL("Beacon, nwtype=A"));)
            nwType = eSIR_11A_NW_TYPE;
        }
    }
    return nwType;
}


/*                                                          
                                  
                                               

           
                                               
                      
                                                           */
tANI_U8 limGetChannelFromBeacon(tpAniSirGlobal pMac, tpSchBeaconStruct pBeacon)
{
    tANI_U8 channelNum = 0;

    if (pBeacon->dsParamsPresent)
        channelNum = pBeacon->channelNumber;
    else if(pBeacon->HTInfo.present)
        channelNum = pBeacon->HTInfo.primaryChannel;
    else
        channelNum = pBeacon->channelNumber;

    return channelNum;
}


/*                                                           
                             
                                                   
                                             
                                         
                                                             
                                
 
                                                           
                                              
                                                            
                                                 
 
                             
                                
                               
             
                                                             */
void limSetTspecUapsdMask(tpAniSirGlobal pMac, tSirMacTSInfo *pTsInfo, tANI_U32 action)
{
    tANI_U8   userPrio = (tANI_U8)pTsInfo->traffic.userPrio;
    tANI_U16  direction = pTsInfo->traffic.direction;  
    tANI_U8   ac = upToAc(userPrio);

    PELOG1(limLog(pMac, LOG1, FL(" Set UAPSD mask for AC %d, direction %d, action=%d (1=set,0=clear) "),ac, direction, action );)

    /*                                            
                                           
                                           
                                           
                                           
     */
    ac = ((~ac) & 0x3);

    if (action == CLEAR_UAPSD_MASK) 
    {
        if (direction == SIR_MAC_DIRECTION_UPLINK)
            pMac->lim.gUapsdPerAcTriggerEnableMask &= ~(1 << ac);
        else if (direction == SIR_MAC_DIRECTION_DNLINK)   
            pMac->lim.gUapsdPerAcDeliveryEnableMask &= ~(1 << ac);
        else if (direction == SIR_MAC_DIRECTION_BIDIR)
        {
            pMac->lim.gUapsdPerAcTriggerEnableMask &= ~(1 << ac);
            pMac->lim.gUapsdPerAcDeliveryEnableMask &= ~(1 << ac);
        }
    }
    else if (action == SET_UAPSD_MASK)
    {
        if (direction == SIR_MAC_DIRECTION_UPLINK)
            pMac->lim.gUapsdPerAcTriggerEnableMask |= (1 << ac);
        else if (direction == SIR_MAC_DIRECTION_DNLINK)   
            pMac->lim.gUapsdPerAcDeliveryEnableMask |= (1 << ac);    
        else if (direction == SIR_MAC_DIRECTION_BIDIR)
        {
            pMac->lim.gUapsdPerAcTriggerEnableMask |= (1 << ac);
            pMac->lim.gUapsdPerAcDeliveryEnableMask |= (1 << ac);      
        }
    }

    limLog(pMac, LOGE, FL("New pMac->lim.gUapsdPerAcTriggerEnableMask = 0x%x "), pMac->lim.gUapsdPerAcTriggerEnableMask );
    limLog(pMac, LOGE, FL("New pMac->lim.gUapsdPerAcDeliveryEnableMask = 0x%x "), pMac->lim.gUapsdPerAcDeliveryEnableMask );

    return;
}



void limHandleHeartBeatTimeout(tpAniSirGlobal pMac )
{

    tANI_U8 i;
    for(i =0;i < pMac->lim.maxBssId;i++)
    {
        if(pMac->lim.gpSession[i].valid == TRUE )
        {
            if(pMac->lim.gpSession[i].bssType == eSIR_IBSS_MODE)
            {
                limIbssHeartBeatHandle(pMac,&pMac->lim.gpSession[i]);
                break;
            }

            if((pMac->lim.gpSession[i].bssType == eSIR_INFRASTRUCTURE_MODE) &&
                (pMac->lim.gpSession[i].limSystemRole == eLIM_STA_ROLE))
            {
                limHandleHeartBeatFailure(pMac,&pMac->lim.gpSession[i]);
            }
        }
     }
     for(i=0; i< pMac->lim.maxBssId; i++)
     {
        if(pMac->lim.gpSession[i].valid == TRUE )
        {
            if((pMac->lim.gpSession[i].bssType == eSIR_INFRASTRUCTURE_MODE) &&
                (pMac->lim.gpSession[i].limSystemRole == eLIM_STA_ROLE))
            {
                if(pMac->lim.gpSession[i].LimHBFailureStatus == eANI_BOOLEAN_TRUE)
                {
                    /*                                                                 */
                    PELOGW(limLog(pMac, LOGW,FL("Sending Probe for Session: %d"),
                            i);)
                    limDeactivateAndChangeTimer(pMac, eLIM_PROBE_AFTER_HB_TIMER);
                    MTRACE(macTrace(pMac, TRACE_CODE_TIMER_ACTIVATE, 0, eLIM_PROBE_AFTER_HB_TIMER));
                    if (tx_timer_activate(&pMac->lim.limTimers.gLimProbeAfterHBTimer) != TX_SUCCESS)
                    {
                        limLog(pMac, LOGP, FL("Fail to re-activate Probe-after-heartbeat timer"));
                        limReactivateHeartBeatTimer(pMac, &pMac->lim.gpSession[i]);
                    }
                    break;
                }
            }
        }
    }
}

void limHandleHeartBeatTimeoutForSession(tpAniSirGlobal pMac, tpPESession psessionEntry)
{
    if(psessionEntry->valid == TRUE )
    {
        if(psessionEntry->bssType == eSIR_IBSS_MODE)
        {
            limIbssHeartBeatHandle(pMac,psessionEntry);
        }
        if((psessionEntry->bssType == eSIR_INFRASTRUCTURE_MODE) &&
             (psessionEntry->limSystemRole == eLIM_STA_ROLE))
        {
            limHandleHeartBeatFailure(pMac,psessionEntry);
        }
    }
    /*                                                                                                  
                                */
    if(psessionEntry->valid == TRUE )
    {
        if((psessionEntry->bssType == eSIR_INFRASTRUCTURE_MODE) &&
            (psessionEntry->limSystemRole == eLIM_STA_ROLE))
        {
            if(psessionEntry->LimHBFailureStatus == eANI_BOOLEAN_TRUE)
            {
                /*                                                                 */
                PELOGW(limLog(pMac, LOGW,FL("Sending Probe for Session: %d"),
                       psessionEntry->bssIdx);)
                limDeactivateAndChangeTimer(pMac, eLIM_PROBE_AFTER_HB_TIMER);
                MTRACE(macTrace(pMac, TRACE_CODE_TIMER_ACTIVATE, 0, eLIM_PROBE_AFTER_HB_TIMER));
                if (tx_timer_activate(&pMac->lim.limTimers.gLimProbeAfterHBTimer) != TX_SUCCESS)
                {
                    limLog(pMac, LOGP, FL("Fail to re-activate Probe-after-heartbeat timer"));
                    limReactivateHeartBeatTimer(pMac, psessionEntry);
                }
            }
        }
    }
}


tANI_U8 limGetCurrentOperatingChannel(tpAniSirGlobal pMac)
{
    tANI_U8 i;
    for(i =0;i < pMac->lim.maxBssId;i++)
    {
        if(pMac->lim.gpSession[i].valid == TRUE )
        {
            if((pMac->lim.gpSession[i].bssType == eSIR_INFRASTRUCTURE_MODE) &&
                (pMac->lim.gpSession[i].limSystemRole == eLIM_STA_ROLE))
            {
                return pMac->lim.gpSession[i].currentOperChannel;
            }
        }
    }
    return 0;
}

void limProcessAddStaRsp(tpAniSirGlobal pMac,tpSirMsgQ limMsgQ)
{
     
    tpPESession         psessionEntry;
//                                  
    tpAddStaParams      pAddStaParams;

    pAddStaParams = (tpAddStaParams)limMsgQ->bodyptr;    
    
    if((psessionEntry = peFindSessionBySessionId(pMac,pAddStaParams->sessionId))==NULL)
    {
        limLog(pMac, LOGP,FL("Session Does not exist for given sessionID"));
        vos_mem_free(pAddStaParams);
        return;
    }
    if (psessionEntry->limSystemRole == eLIM_STA_IN_IBSS_ROLE)
        (void) limIbssAddStaRsp(pMac, limMsgQ->bodyptr,psessionEntry);
#ifdef FEATURE_WLAN_TDLS
    else if(pMac->lim.gLimAddStaTdls)
    {
        limProcessTdlsAddStaRsp(pMac, limMsgQ->bodyptr, psessionEntry) ;
        pMac->lim.gLimAddStaTdls = FALSE ;
    }    
#endif
    else
        limProcessMlmAddStaRsp(pMac, limMsgQ,psessionEntry);
                
}


void limUpdateBeacon(tpAniSirGlobal pMac)
{
    tANI_U8 i;
   
    for(i =0;i < pMac->lim.maxBssId;i++)
    {
        if(pMac->lim.gpSession[i].valid == TRUE )
        {
            if( ( (pMac->lim.gpSession[i].limSystemRole == eLIM_AP_ROLE) || 
                (pMac->lim.gpSession[i].limSystemRole == eLIM_STA_IN_IBSS_ROLE) )
                && (eLIM_SME_NORMAL_STATE == pMac->lim.gpSession[i].limSmeState)
               )
            {
                schSetFixedBeaconFields(pMac,&pMac->lim.gpSession[i]);
                limSendBeaconInd(pMac, &pMac->lim.gpSession[i]);
             }
            else
            {
                if( (pMac->lim.gpSession[i].limSystemRole == eLIM_BT_AMP_AP_ROLE)||
                    (pMac->lim.gpSession[i].limSystemRole == eLIM_BT_AMP_STA_ROLE))
                {
                    
                    if(pMac->lim.gpSession[i].statypeForBss == STA_ENTRY_SELF)
                    {
                        schSetFixedBeaconFields(pMac,&pMac->lim.gpSession[i]);
                    }
                }
            }
        }
    }   
}

void limHandleHeartBeatFailureTimeout(tpAniSirGlobal pMac)
{
    tANI_U8 i;
    tpPESession psessionEntry;
    /*                                                                                       */
    for(i =0; i < pMac->lim.maxBssId; i++)
    {
        if(pMac->lim.gpSession[i].valid == TRUE)
        {
            psessionEntry = &pMac->lim.gpSession[i];
            if(psessionEntry->LimHBFailureStatus == eANI_BOOLEAN_TRUE)
            {
                limLog(pMac, LOGE, FL("Probe_hb_failure: SME %d, MLME %d, HB-Count %d"),psessionEntry->limSmeState,
                        psessionEntry->limMlmState, psessionEntry->LimRxedBeaconCntDuringHB);
#ifdef FEATURE_WLAN_DIAG_SUPPORT_LIM //                         
                limDiagEventReport(pMac, WLAN_PE_DIAG_HB_FAILURE_TIMEOUT, psessionEntry, 0, 0);
#endif
                if (psessionEntry->limMlmState == eLIM_MLM_LINK_ESTABLISHED_STATE)
                {
                    if (psessionEntry->limSmeState != eLIM_SME_WT_DISASSOC_STATE)
                    {
                        limLog(pMac, LOGE, FL("Probe_hb_failure: for session:%d " ),psessionEntry->peSessionId);
                        /*                                                             */
                        limTearDownLinkWithAp(pMac,
                                              psessionEntry->peSessionId,
                                              eSIR_BEACON_MISSED);
                        pMac->lim.gLimProbeFailureAfterHBfailedCnt++ ;
                    }
                    else //                        
                    {
                        limReactivateHeartBeatTimer(pMac, psessionEntry);
                    }
                }
                else
                {
                    limLog(pMac, LOGE, FL("Unexpected wt-probe-timeout in state "));
                    limPrintMlmState(pMac, LOGE, psessionEntry->limMlmState);
                    limReactivateHeartBeatTimer(pMac, psessionEntry);
                }

            }
        }
    }
    /*                                                                                              */
    //                                                                 
}


/*
                                                                                        
*/
tpPESession limIsIBSSSessionActive(tpAniSirGlobal pMac)
{
    tANI_U8 i;
   
    for(i =0;i < pMac->lim.maxBssId;i++)
    {
        if( (pMac->lim.gpSession[i].valid) &&
             (pMac->lim.gpSession[i].limSystemRole == eLIM_STA_IN_IBSS_ROLE))
             return (&pMac->lim.gpSession[i]);
    }

    return NULL;
}

tpPESession limIsApSessionActive(tpAniSirGlobal pMac)
{
    tANI_U8 i;
   
    for(i =0;i < pMac->lim.maxBssId;i++)
    {
        if( (pMac->lim.gpSession[i].valid) &&
             ( (pMac->lim.gpSession[i].limSystemRole == eLIM_AP_ROLE) ||
               (pMac->lim.gpSession[i].limSystemRole == eLIM_BT_AMP_AP_ROLE)))
             return (&pMac->lim.gpSession[i]);
    }

    return NULL;
}

/*                                                          
                                 
                                                                   
           
                                                    
            
                                                           */

void limHandleDeferMsgError(tpAniSirGlobal pMac, tpSirMsgQ pLimMsg)
{
      if(SIR_BB_XPORT_MGMT_MSG == pLimMsg->type) 
        {
            vos_pkt_return_packet((vos_pkt_t*)pLimMsg->bodyptr);
        }
      else if(pLimMsg->bodyptr != NULL)
      {
          vos_mem_free( pLimMsg->bodyptr);
          pLimMsg->bodyptr = NULL;
      }
}


#ifdef FEATURE_WLAN_DIAG_SUPPORT
/*                                                          
                         
                                        
           
                
            
             
                 
            
                                                           */
void limDiagEventReport(tpAniSirGlobal pMac, tANI_U16 eventType, tpPESession pSessionEntry, tANI_U16 status, tANI_U16 reasonCode)
{
    tSirMacAddr nullBssid = { 0, 0, 0, 0, 0, 0 };
    WLAN_VOS_DIAG_EVENT_DEF(peEvent, vos_event_wlan_pe_payload_type);

    vos_mem_set(&peEvent, sizeof(vos_event_wlan_pe_payload_type), 0);

    if (NULL == pSessionEntry)
    {
       vos_mem_copy( peEvent.bssid, nullBssid, sizeof(tSirMacAddr));
       peEvent.sme_state = (tANI_U16)pMac->lim.gLimSmeState;
       peEvent.mlm_state = (tANI_U16)pMac->lim.gLimMlmState;

    }
    else
    {
       vos_mem_copy(peEvent.bssid, pSessionEntry->bssId, sizeof(tSirMacAddr));
       peEvent.sme_state = (tANI_U16)pSessionEntry->limSmeState;
       peEvent.mlm_state = (tANI_U16)pSessionEntry->limMlmState;
    }
    peEvent.event_type = eventType;
    peEvent.status = status;
    peEvent.reason_code = reasonCode;

    WLAN_VOS_DIAG_EVENT_REPORT(&peEvent, EVENT_WLAN_PE);
    return;
}

#endif /*                           */

void limProcessAddStaSelfRsp(tpAniSirGlobal pMac,tpSirMsgQ limMsgQ)
{

   tpAddStaSelfParams      pAddStaSelfParams;
   tSirMsgQ                mmhMsg;
   tpSirSmeAddStaSelfRsp   pRsp;

   
   pAddStaSelfParams = (tpAddStaSelfParams)limMsgQ->bodyptr;    

   pRsp = vos_mem_malloc(sizeof(tSirSmeAddStaSelfRsp));
   if ( NULL == pRsp )
   {
      //                                 
      limLog(pMac, LOGP, FL("call to AllocateMemory failed for Add Sta self RSP"));
      vos_mem_free(pAddStaSelfParams);
      limMsgQ->bodyptr = NULL;
      return;
   }

   vos_mem_set((tANI_U8*)pRsp, sizeof(tSirSmeAddStaSelfRsp), 0);

   pRsp->mesgType = eWNI_SME_ADD_STA_SELF_RSP;
   pRsp->mesgLen = (tANI_U16) sizeof(tSirSmeAddStaSelfRsp);
   pRsp->status = pAddStaSelfParams->status;

   vos_mem_copy( pRsp->selfMacAddr, pAddStaSelfParams->selfMacAddr, sizeof(tSirMacAddr) );

   vos_mem_free(pAddStaSelfParams);
   limMsgQ->bodyptr = NULL;

   mmhMsg.type = eWNI_SME_ADD_STA_SELF_RSP;
   mmhMsg.bodyptr = pRsp;
   mmhMsg.bodyval = 0;
   MTRACE(macTraceMsgTx(pMac, NO_SESSION, mmhMsg.type));
   limSysProcessMmhMsgApi(pMac, &mmhMsg,  ePROT);

}

void limProcessDelStaSelfRsp(tpAniSirGlobal pMac,tpSirMsgQ limMsgQ)
{

   tpDelStaSelfParams      pDelStaSelfParams;
   tSirMsgQ                mmhMsg;
   tpSirSmeDelStaSelfRsp   pRsp;

   
   pDelStaSelfParams = (tpDelStaSelfParams)limMsgQ->bodyptr;    

   pRsp = vos_mem_malloc(sizeof(tSirSmeDelStaSelfRsp));
   if ( NULL == pRsp )
   {
      //                                 
      limLog(pMac, LOGP, FL("call to AllocateMemory failed for Add Sta self RSP"));
      vos_mem_free(pDelStaSelfParams);
      limMsgQ->bodyptr = NULL;
      return;
   }

   vos_mem_set((tANI_U8*)pRsp, sizeof(tSirSmeDelStaSelfRsp), 0);

   pRsp->mesgType = eWNI_SME_DEL_STA_SELF_RSP;
   pRsp->mesgLen = (tANI_U16) sizeof(tSirSmeDelStaSelfRsp);
   pRsp->status = pDelStaSelfParams->status;

   vos_mem_copy( pRsp->selfMacAddr, pDelStaSelfParams->selfMacAddr, sizeof(tSirMacAddr) );

   vos_mem_free(pDelStaSelfParams);
   limMsgQ->bodyptr = NULL;

   mmhMsg.type = eWNI_SME_DEL_STA_SELF_RSP;
   mmhMsg.bodyptr = pRsp;
   mmhMsg.bodyval = 0;
   MTRACE(macTraceMsgTx(pMac, NO_SESSION, mmhMsg.type));
   limSysProcessMmhMsgApi(pMac, &mmhMsg,  ePROT);

}

/*                                                              
                                             
                                                        
                                                             
                                                                   
                                                                */
tANI_U8 limUnmapChannel(tANI_U8 mapChannel)
{
   if( mapChannel > 0 && mapChannel < 25 )
#ifdef WLAN_FEATURE_ROAM_SCAN_OFFLOAD
       if (IS_ROAM_SCAN_OFFLOAD_FEATURE_ENABLE)
           return aUnsortedChannelList[mapChannel -1];
       else
#endif
     return abChannel[mapChannel -1];
   else
     return 0;
}


v_U8_t* limGetIEPtr(tpAniSirGlobal pMac, v_U8_t *pIes, int length, v_U8_t eid,eSizeOfLenField size_of_len_field)
{
    int left = length;
    v_U8_t *ptr = pIes;
    v_U8_t elem_id;
    v_U16_t elem_len;
   
    while(left >= (size_of_len_field+1))
    {   
        elem_id  =  ptr[0];
        if (size_of_len_field == TWO_BYTE)
        {
            elem_len = ((v_U16_t) ptr[1]) | (ptr[2]<<8);
        }
        else
        {
            elem_len =  ptr[1];
        }
    
            
        left -= (size_of_len_field+1);
        if(elem_len > left)
        {
            limLog(pMac, LOGE,
                    FL("****Invalid IEs eid = %d elem_len=%d left=%d*****"),
                                                    eid,elem_len,left);
            return NULL;
        }
        if (elem_id == eid) 
        {
            return ptr;
        }
   
        left -= elem_len;
        ptr += (elem_len + (size_of_len_field+1));
    }
    return NULL;
}

/*                                      
                                                                          
 */
v_U8_t* limGetVendorIEOuiPtr(tpAniSirGlobal pMac, tANI_U8 *oui, tANI_U8 oui_size, tANI_U8 *ie, tANI_U16 ie_len)
{   
    int left = ie_len;
    v_U8_t *ptr = ie;
    v_U8_t elem_id, elem_len;

    while(left >= 2)
    {
        elem_id  = ptr[0];
        elem_len = ptr[1];
        left -= 2;
        if(elem_len > left)
        {
            limLog( pMac, LOGE, 
               FL("****Invalid IEs eid = %d elem_len=%d left=%d*****"),
                                               elem_id,elem_len,left);
            return NULL;
        }
        if (SIR_MAC_EID_VENDOR == elem_id) 
        {
            if(memcmp(&ptr[2], oui, oui_size)==0)
                return ptr;
        }
 
        left -= elem_len;
        ptr += (elem_len + 2);
    }
    return NULL;
}

//                                                                                             

v_U8_t limBuildP2pIe(tpAniSirGlobal pMac, tANI_U8 *ie, tANI_U8 *data, tANI_U8 ie_len)
{
    int length = 0;
    tANI_U8 *ptr = ie; 

    ptr[length++] = SIR_MAC_EID_VENDOR;
    ptr[length++] = ie_len + SIR_MAC_P2P_OUI_SIZE;
    vos_mem_copy(&ptr[length], SIR_MAC_P2P_OUI, SIR_MAC_P2P_OUI_SIZE);
    vos_mem_copy(&ptr[length + SIR_MAC_P2P_OUI_SIZE], data, ie_len);
    return (ie_len + SIR_P2P_IE_HEADER_LEN);
}

//                                                                                                     

v_U8_t limGetNoaAttrStreamInMultP2pIes(tpAniSirGlobal pMac,v_U8_t* noaStream,v_U8_t noaLen,v_U8_t overFlowLen)
{
   v_U8_t overFlowP2pStream[SIR_MAX_NOA_ATTR_LEN];

   if ((noaLen <= (SIR_MAX_NOA_ATTR_LEN+SIR_P2P_IE_HEADER_LEN)) &&
       (noaLen >= overFlowLen) && (overFlowLen <= SIR_MAX_NOA_ATTR_LEN))
   {
       vos_mem_copy(overFlowP2pStream,
                     noaStream + noaLen - overFlowLen, overFlowLen);
       noaStream[noaLen - overFlowLen] = SIR_MAC_EID_VENDOR;
       noaStream[noaLen - overFlowLen + 1] = overFlowLen + SIR_MAC_P2P_OUI_SIZE;
       vos_mem_copy(noaStream+noaLen-overFlowLen + 2,
                     SIR_MAC_P2P_OUI, SIR_MAC_P2P_OUI_SIZE);
       vos_mem_copy(noaStream+noaLen + 2 + SIR_MAC_P2P_OUI_SIZE - overFlowLen,
                    overFlowP2pStream, overFlowLen);
   }

   return (noaLen + SIR_P2P_IE_HEADER_LEN);

}

//                                                                                                     
v_U8_t limGetNoaAttrStream(tpAniSirGlobal pMac, v_U8_t*pNoaStream,tpPESession psessionEntry)
{
    v_U8_t len=0;

    v_U8_t   *pBody = pNoaStream; 
    
   
    if   ( (psessionEntry != NULL) && (psessionEntry->valid) && 
           (psessionEntry->pePersona == VOS_P2P_GO_MODE))
    { 
       if ((!(psessionEntry->p2pGoPsUpdate.uNoa1Duration)) && (!(psessionEntry->p2pGoPsUpdate.uNoa2Duration))
            && (!psessionEntry->p2pGoPsUpdate.oppPsFlag)
          )
         return 0; //                               


        pBody[0] = SIR_P2P_NOA_ATTR;
        
        pBody[3] = psessionEntry->p2pGoPsUpdate.index;
        pBody[4] = psessionEntry->p2pGoPsUpdate.ctWin | (psessionEntry->p2pGoPsUpdate.oppPsFlag<<7);
        len = 5;
        pBody += len;
        
        
        if (psessionEntry->p2pGoPsUpdate.uNoa1Duration)
        {
            *pBody = psessionEntry->p2pGoPsUpdate.uNoa1IntervalCnt; 
            pBody += 1;
            len +=1;
             
            *((tANI_U32 *)(pBody)) = sirSwapU32ifNeeded(psessionEntry->p2pGoPsUpdate.uNoa1Duration);
            pBody   += sizeof(tANI_U32);               
            len +=4;
            
            *((tANI_U32 *)(pBody)) = sirSwapU32ifNeeded(psessionEntry->p2pGoPsUpdate.uNoa1Interval);
            pBody   += sizeof(tANI_U32);               
            len +=4;
            
            *((tANI_U32 *)(pBody)) = sirSwapU32ifNeeded(psessionEntry->p2pGoPsUpdate.uNoa1StartTime);
            pBody   += sizeof(tANI_U32);               
            len +=4;
            
        }
        
        if (psessionEntry->p2pGoPsUpdate.uNoa2Duration)
        {
            *pBody = psessionEntry->p2pGoPsUpdate.uNoa2IntervalCnt; 
            pBody += 1;
            len +=1;
             
            *((tANI_U32 *)(pBody)) = sirSwapU32ifNeeded(psessionEntry->p2pGoPsUpdate.uNoa2Duration);
            pBody   += sizeof(tANI_U32);               
            len +=4;
            
            *((tANI_U32 *)(pBody)) = sirSwapU32ifNeeded(psessionEntry->p2pGoPsUpdate.uNoa2Interval);
            pBody   += sizeof(tANI_U32);               
            len +=4;
            
            *((tANI_U32 *)(pBody)) = sirSwapU32ifNeeded(psessionEntry->p2pGoPsUpdate.uNoa2StartTime);
            pBody   += sizeof(tANI_U32);               
            len +=4;

        }
    

        pBody = pNoaStream + 1;            
        *((tANI_U16 *)(pBody)) = sirSwapU16ifNeeded(len-3);/*                                        */

        return (len);

    }    
    return 0;
        
}

void peSetResumeChannel(tpAniSirGlobal pMac, tANI_U16 channel, ePhyChanBondState phyCbState)
{

   pMac->lim.gResumeChannel = channel;
   pMac->lim.gResumePhyCbState = phyCbState;
}

/*                                                                          
  
                                                                                               

                                                                                                   
                                                
    
                                                                   
                                                                                   
  
     
  
                                                                            */
void peGetResumeChannel(tpAniSirGlobal pMac, tANI_U8* resumeChannel, ePhyChanBondState* resumePhyCbState)
{

    //                                                                               
    //                                                                    
    //                                                                                 
    //                                                                                         
    //                                                    
    if( !limIsInMCC(pMac) )    
    {
        //                                  
        peGetActiveSessionChannel(pMac, resumeChannel, resumePhyCbState);
    }
    else
    {
        *resumeChannel = pMac->lim.gResumeChannel;
        *resumePhyCbState = pMac->lim.gResumePhyCbState;
    }
    return;
}

tANI_BOOLEAN limIsNOAInsertReqd(tpAniSirGlobal pMac)
{
    tANI_U8 i;
    for(i =0; i < pMac->lim.maxBssId; i++)
    {
        if(pMac->lim.gpSession[i].valid == TRUE)
        {
            if( (eLIM_AP_ROLE == pMac->lim.gpSession[i].limSystemRole ) 
                    && ( VOS_P2P_GO_MODE == pMac->lim.gpSession[i].pePersona )
                   )
            {
                return TRUE;
            }
        }
    }
    return FALSE;
}


tANI_BOOLEAN limIsconnectedOnDFSChannel(tANI_U8 currentChannel)
{
    if(NV_CHANNEL_DFS == vos_nv_getChannelEnabledState(currentChannel))
    {
        return eANI_BOOLEAN_TRUE;
    }
    else
    {
        return eANI_BOOLEAN_FALSE;
    }
}

#ifdef WLAN_FEATURE_11AC
tANI_BOOLEAN limCheckVHTOpModeChange( tpAniSirGlobal pMac, tpPESession psessionEntry, tANI_U8 chanWidth, tANI_U8 staId)
{
    tUpdateVHTOpMode tempParam;
    
    tempParam.opMode = chanWidth;
    tempParam.staId  = staId;

    limSendModeUpdate( pMac, &tempParam, psessionEntry );

    return eANI_BOOLEAN_TRUE;
}
#endif

tANI_U8 limGetShortSlotFromPhyMode(tpAniSirGlobal pMac, tpPESession psessionEntry, tANI_U32 phyMode)
{
    tANI_U8 val=0;

    if (phyMode == WNI_CFG_PHY_MODE_11A)
    {
        //                                
        //                                                                                                                           
        val = true;
    }
    else if (phyMode == WNI_CFG_PHY_MODE_11G)
    {
        if ((psessionEntry->pePersona == VOS_STA_SAP_MODE) ||
            (psessionEntry->pePersona == VOS_P2P_GO_MODE))
        {
            val = true;
        }

        //                                       

        if (psessionEntry->limMlmState == eLIM_MLM_WT_JOIN_BEACON_STATE)
            //             
            val = SIR_MAC_GET_SHORT_SLOT_TIME( psessionEntry->limCurrentBssCaps);
        else if (psessionEntry->limMlmState == eLIM_MLM_WT_REASSOC_RSP_STATE)
            //                       
            val = SIR_MAC_GET_SHORT_SLOT_TIME( psessionEntry->limReassocBssCaps);
    }
    else //                                                                                     
    {
        //                            
        val = false;
    }
    limLog(pMac, LOG1, FL("phyMode = %u shortslotsupported = %u"), phyMode, val);
    return val;
}

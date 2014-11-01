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
/*          */
/*                                                                          
               
                                                                          */


#include "aniGlobal.h"

#include "smeInside.h"
#include "vos_diag_core_event.h"
#include "vos_diag_core_log.h"

#ifdef WLAN_FEATURE_VOWIFI_11R
#include "smsDebug.h"
#include "utilsParser.h"
#endif
#if defined(FEATURE_WLAN_CCX) && !defined(FEATURE_WLAN_CCX_UPLOAD)
#include <csrCcx.h>
#endif

#ifndef WLAN_MDM_CODE_REDUCTION_OPT
/*                                                                         */
#define SME_QOS_MIN_PHY_RATE         0x5B8D80    
#define SME_QOS_SURPLUS_BW_ALLOWANCE  0x2000     /*                        */
/*                                                                           
                                                             
                                                                           */
#define SME_QOS_32BIT_MAX  0xFFFFFFFF
#define SME_QOS_16BIT_MAX  0xFFFF
#define SME_QOS_16BIT_MSB  0x8000
/*                                                                           
                                                            
                                                                           */
#define SME_QOS_BOUNDED_U32_ADD_Y_TO_X( _x, _y )                            \
  do                                                                        \
  {                                                                         \
    (_x) = ( (SME_QOS_32BIT_MAX-(_x))<(_y) ) ?                              \
    (SME_QOS_32BIT_MAX) : (_x)+(_y);                                        \
  } while(0)
/*                                                                           
                                                                         
                                                             
                                                                           */
#define SME_QOS_TSPEC_INDEX_0            0
#define SME_QOS_TSPEC_INDEX_1            1
#define SME_QOS_TSPEC_INDEX_MAX          2
#define SME_QOS_TSPEC_MASK_BIT_1_SET     1
#define SME_QOS_TSPEC_MASK_BIT_2_SET     2
#define SME_QOS_TSPEC_MASK_BIT_1_2_SET   3
#define SME_QOS_TSPEC_MASK_CLEAR         0

//                                                                        
#define SME_QOS_SEARCH_KEY_INDEX_1       1
#define SME_QOS_SEARCH_KEY_INDEX_2       2
#define SME_QOS_SEARCH_KEY_INDEX_3       4
#define SME_QOS_SEARCH_KEY_INDEX_4       8  //               
#define SME_QOS_SEARCH_KEY_INDEX_5       0x10  //                
//                                          
#define SME_QOS_SEARCH_SESSION_ID_ANY    CSR_ROAM_SESSION_MAX
#define SME_QOS_ACCESS_POLICY_EDCA       1
#define SME_QOS_MAX_TID                  255
#define SME_QOS_TSPEC_IE_LENGTH          61
#define SME_QOS_TSPEC_IE_TYPE            2
#define SME_QOS_MIN_FLOW_ID              1
#define SME_QOS_MAX_FLOW_ID              0xFFFFFFFE
#define SME_QOS_INVALID_FLOW_ID          0xFFFFFFFF
//                                              
//                                                              
#define SME_QOS_MIN_DIALOG_TOKEN         1
#define SME_QOS_MAX_DIALOG_TOKEN         0xFF
/*                                                                           
                   
                                                                          */
/*                                                                           
                                                         
                                                                           */
typedef enum
{
   SME_QOS_CLOSED = 0,
   SME_QOS_INIT,
   SME_QOS_LINK_UP,
   SME_QOS_REQUESTED,
   SME_QOS_QOS_ON,
   SME_QOS_HANDOFF,
   
}sme_QosStates;
/*                                                                           
                                                 
                                                                           */
typedef enum
{
   SME_QOS_RELEASE_DEFAULT = 0,
   SME_QOS_RELEASE_BY_AP,
}sme_QosRelTriggers;
/*                                                                           
                                       
                                                                           */
typedef enum
{
   SME_QOS_SETUP_REQ = 0,
   SME_QOS_RELEASE_REQ,
   SME_QOS_MODIFY_REQ,
   SME_QOS_RESEND_REQ,
   SME_QOS_CMD_MAX
}sme_QosCmdType;
/*                                                                           
                                                                           
                                                                           */
typedef enum
{
   SME_QOS_REASON_SETUP = 0,
   SME_QOS_REASON_RELEASE,
   SME_QOS_REASON_MODIFY,
   SME_QOS_REASON_MODIFY_PENDING,
   SME_QOS_REASON_REQ_SUCCESS,
   SME_QOS_REASON_MAX
}sme_QosReasonType;

/*                                                                           
                                                                            
                                      
                                                                           */
sme_QosEdcaAcType sme_QosUPtoACMap[SME_QOS_WMM_UP_MAX] =
{
   SME_QOS_EDCA_AC_BE, /*                 */
   SME_QOS_EDCA_AC_BK, /*                 */
   SME_QOS_EDCA_AC_BK, /*                 */
   SME_QOS_EDCA_AC_BE, /*                 */
   SME_QOS_EDCA_AC_VI, /*                 */
   SME_QOS_EDCA_AC_VI, /*                 */
   SME_QOS_EDCA_AC_VO, /*                 */
   SME_QOS_EDCA_AC_VO  /*                 */
};

/*                                                                           
                                                                             
                
                                                                           
                                 
                                                                           */
sme_QosWmmUpType sme_QosACtoUPMap[SME_QOS_EDCA_AC_MAX] = 
{
   SME_QOS_WMM_UP_BE,   /*       */
   SME_QOS_WMM_UP_BK,   /*       */
   SME_QOS_WMM_UP_VI,   /*       */
   SME_QOS_WMM_UP_VO    /*       */
};
/*                                                                           
           
                                                                                
                                                                       
                                                                           */
typedef struct sme_QosFlowInfoEntry_s
{
    tListElem             link;  /*            */
    v_U8_t                sessionId;
    v_U8_t                tspec_mask;
    sme_QosReasonType     reason;
    v_U32_t               QosFlowID;
    sme_QosEdcaAcType     ac_type;
    sme_QosWmmTspecInfo   QoSInfo;
    void                * HDDcontext;
    sme_QosCallback       QoSCallback;
    v_BOOL_t              hoRenewal;//                                            
                                     //                                         
                                     //                                           
                                     //                                           
} sme_QosFlowInfoEntry;
/*                                                                           
           
                                                                    
                                                                           */
typedef struct sme_QosSetupCmdInfo_s
{
    v_U32_t               QosFlowID;
    sme_QosWmmTspecInfo   QoSInfo;
    void                 *HDDcontext;
    sme_QosCallback       QoSCallback;
    sme_QosWmmUpType      UPType;
    v_BOOL_t              hoRenewal;//                                            
                                     //                                         
                                     //                                           
                                     //                                           
} sme_QosSetupCmdInfo;
/*                                                                           
           
                                                             
                                                                           */
typedef struct sme_QosModifyCmdInfo_s
{
    v_U32_t               QosFlowID;
    sme_QosEdcaAcType     ac;
    sme_QosWmmTspecInfo   QoSInfo;
} sme_QosModifyCmdInfo;
/*                                                                           
           
                                                             
                                                                           */
typedef struct sme_QosResendCmdInfo_s
{
    v_U8_t                tspecMask;
    sme_QosEdcaAcType     ac;
    sme_QosWmmTspecInfo   QoSInfo;
} sme_QosResendCmdInfo;
/*                                                                           
           
                                                              
                                                                           */
typedef struct sme_QosReleaseCmdInfo_s
{
    v_U32_t               QosFlowID;
} sme_QosReleaseCmdInfo;
/*                                                                           
           
                                                               
                                                                           */
typedef struct sme_QosCmdInfo_s
{
    sme_QosCmdType        command;
    tpAniSirGlobal        pMac;
    v_U8_t                sessionId;
    union
    {
       sme_QosSetupCmdInfo    setupCmdInfo;
       sme_QosModifyCmdInfo   modifyCmdInfo;
       sme_QosResendCmdInfo   resendCmdInfo;
       sme_QosReleaseCmdInfo  releaseCmdInfo;
    }u;
} sme_QosCmdInfo;
/*                                                                           
           
                                                                               
                                     
                                                                           */
typedef struct sme_QosCmdInfoEntry_s
{
    tListElem             link;  /*            */
    sme_QosCmdInfo        cmdInfo;
} sme_QosCmdInfoEntry;
/*                                                                           
           
                                                                             
                                                                              
                                                                           */
typedef struct sme_QosACInfo_s
{
   v_U8_t                 num_flows[SME_QOS_TSPEC_INDEX_MAX];
   sme_QosStates          curr_state;
   sme_QosStates          prev_state;
   sme_QosWmmTspecInfo    curr_QoSInfo[SME_QOS_TSPEC_INDEX_MAX];
   sme_QosWmmTspecInfo    requested_QoSInfo[SME_QOS_TSPEC_INDEX_MAX];
   v_BOOL_t               reassoc_pending;//                          
   //                                                                       
   //                                                           
   v_U8_t                 tspec_mask_status; //                                             
   v_U8_t                 tspec_pending;//                                          
   v_BOOL_t               hoRenewal;//                                            
                                    //                                         
                                    //                                           
                                    //                                           
#ifdef WLAN_FEATURE_VOWIFI_11R
   v_U8_t                 ricIdentifier[SME_QOS_TSPEC_INDEX_MAX];
   /*                                                                         
                                                        */
   tSirAddtsRsp           addTsRsp[SME_QOS_TSPEC_INDEX_MAX];
#endif
   sme_QosRelTriggers     relTrig;

} sme_QosACInfo;
/*                                                                           
           
                                                                               
                             
                                                                           */
typedef struct sme_QosSessionInfo_s
{
   //                                
   v_U8_t                 sessionId;
   //                                
   v_BOOL_t               sessionActive;
   //                             
   sme_QosACInfo          ac_info[SME_QOS_EDCA_AC_MAX];
   //                                 
   //                                                              
   v_U8_t                 apsdMask;
   //                                         
   sme_QosAssocInfo       assocInfo;
   //                                   
   v_U32_t                roamID;
   //                                                                        
   //                                         
   v_BOOL_t               readyForPowerSave;
   //                                                       
   v_BOOL_t               handoffRequested;
   //                                                                     
   v_BOOL_t               uapsdAlreadyRequested;
   //                                                  
   tDblLinkList           bufferedCommandList;

#ifdef WLAN_FEATURE_VOWIFI_11R
   v_BOOL_t               ftHandoffInProgress;
#endif

} sme_QosSessionInfo;
/*                                                                           
           
                                                                               
                  
                                                                           */
typedef union sme_QosSearchKey_s
{
   v_U32_t               QosFlowID;
   sme_QosEdcaAcType     ac_type;
   sme_QosReasonType     reason;
}sme_QosSearchKey;
/*                                                                           
           
                                                                                
                                                                       
                 
                 
                                                                           */
typedef struct sme_QosSearchInfo_s
{
   v_U8_t           sessionId;
   v_U8_t           index;
   sme_QosSearchKey key;
   sme_QosWmmDirType   direction;
   v_U8_t              tspec_mask;
}sme_QosSearchInfo;
/*                                                                           
           
                                          
                                                                           */
struct sme_QosCb_s
{
   //                  
   tpAniSirGlobal   pMac;
   //                
   sme_QosSessionInfo     sessionInfo[CSR_ROAM_SESSION_MAX];
   //             
   tDblLinkList           flow_list;
   //                    
   sme_QosWmmTspecInfo    def_QoSInfo[SME_QOS_EDCA_AC_MAX];
   //                              
   v_U32_t                nextFlowId;
   //                                   
   v_U8_t                nextDialogToken;
}sme_QosCb;
typedef eHalStatus (*sme_QosProcessSearchEntry)(tpAniSirGlobal pMac, tListElem *pEntry);
/*                                                                           
                                                       
                                                                          */
sme_QosStatusType sme_QosInternalSetupReq(tpAniSirGlobal pMac, 
                                          v_U8_t sessionId,
                                          sme_QosWmmTspecInfo * pQoSInfo,
                                          sme_QosCallback QoSCallback, 
                                          void * HDDcontext,
                                          sme_QosWmmUpType UPType, 
                                          v_U32_t QosFlowID,
                                          v_BOOL_t buffered_cmd,
                                          v_BOOL_t hoRenewal);
sme_QosStatusType sme_QosInternalModifyReq(tpAniSirGlobal pMac, 
                                           sme_QosWmmTspecInfo * pQoSInfo,
                                           v_U32_t QosFlowID,
                                           v_BOOL_t buffered_cmd);
sme_QosStatusType sme_QosInternalReleaseReq(tpAniSirGlobal pMac, 
                                            v_U32_t QosFlowID,
                                            v_BOOL_t buffered_cmd);
sme_QosStatusType sme_QosSetup(tpAniSirGlobal pMac,
                               v_U8_t sessionId,
                               sme_QosWmmTspecInfo *pTspec_Info, 
                               sme_QosEdcaAcType ac);
eHalStatus sme_QosAddTsReq(tpAniSirGlobal pMac,
                           v_U8_t sessionId,
                           sme_QosWmmTspecInfo * pTspec_Info,
                           sme_QosEdcaAcType ac);
eHalStatus sme_QosDelTsReq(tpAniSirGlobal pMac,
                           v_U8_t sessionId,
                           sme_QosEdcaAcType ac,
                           v_U8_t tspec_mask);
eHalStatus sme_QosProcessAddTsRsp(tpAniSirGlobal pMac, void *pMsgBuf);
eHalStatus sme_QosProcessDelTsInd(tpAniSirGlobal pMac, void *pMsgBuf);
eHalStatus sme_QosProcessDelTsRsp(tpAniSirGlobal pMac, void *pMsgBuf);
eHalStatus sme_QosProcessAssocCompleteEv(tpAniSirGlobal pMac, v_U8_t sessionId, void * pEvent_info);
eHalStatus sme_QosProcessReassocReqEv(tpAniSirGlobal pMac, v_U8_t sessionId, void * pEvent_info);
eHalStatus sme_QosProcessReassocSuccessEv(tpAniSirGlobal pMac, v_U8_t sessionId, void * pEvent_info);
eHalStatus sme_QosProcessReassocFailureEv(tpAniSirGlobal pMac, v_U8_t sessionId, void * pEvent_info);
eHalStatus sme_QosProcessDisconnectEv(tpAniSirGlobal pMac, v_U8_t sessionId, void * pEvent_info);
eHalStatus sme_QosProcessJoinReqEv(tpAniSirGlobal pMac, v_U8_t sessionId, void * pEvent_info);
eHalStatus sme_QosProcessHandoffAssocReqEv(tpAniSirGlobal pMac, v_U8_t sessionId, void * pEvent_info);
eHalStatus sme_QosProcessHandoffSuccessEv(tpAniSirGlobal pMac, v_U8_t sessionId, void * pEvent_info);
eHalStatus sme_QosProcessHandoffFailureEv(tpAniSirGlobal pMac, v_U8_t sessionId, void * pEvent_info);
#ifdef WLAN_FEATURE_VOWIFI_11R
eHalStatus sme_QosProcessPreauthSuccessInd(tpAniSirGlobal pMac, v_U8_t sessionId, void * pEvent_info);
eHalStatus sme_QosProcessSetKeySuccessInd(tpAniSirGlobal pMac, v_U8_t sessionId, void * pEvent_info);
eHalStatus sme_QosProcessAggrQosRsp(tpAniSirGlobal pMac, void *pMsgBuf);
eHalStatus sme_QosFTAggrQosReq( tpAniSirGlobal pMac, v_U8_t sessionId );
#endif
eHalStatus sme_QosProcessAddTsSuccessRsp(tpAniSirGlobal pMac, 
                                         v_U8_t sessionId,
                                         tSirAddtsRspInfo * pRsp);
eHalStatus sme_QosProcessAddTsFailureRsp(tpAniSirGlobal pMac, 
                                         v_U8_t sessionId,
                                         tSirAddtsRspInfo * pRsp);
eHalStatus sme_QosAggregateParams(
   sme_QosWmmTspecInfo * pInput_Tspec_Info,
   sme_QosWmmTspecInfo * pCurrent_Tspec_Info,
   sme_QosWmmTspecInfo * pUpdated_Tspec_Info);
static eHalStatus sme_QosUpdateParams(v_U8_t sessionId,
                                      sme_QosEdcaAcType ac,
                                      v_U8_t tspec_mask, 
                                      sme_QosWmmTspecInfo * pTspec_Info);
sme_QosWmmUpType sme_QosAcToUp(sme_QosEdcaAcType ac);
sme_QosEdcaAcType sme_QosUpToAc(sme_QosWmmUpType up);
v_BOOL_t sme_QosIsACM(tpAniSirGlobal pMac, tSirBssDescription *pSirBssDesc, 
                      sme_QosEdcaAcType ac, tDot11fBeaconIEs *pIes);
tListElem *sme_QosFindInFlowList(sme_QosSearchInfo search_key);
eHalStatus sme_QosFindAllInFlowList(tpAniSirGlobal pMac,
                                    sme_QosSearchInfo search_key, 
                                    sme_QosProcessSearchEntry fnp);
static void sme_QosStateTransition(v_U8_t sessionId,
                                   sme_QosEdcaAcType ac,
                                   sme_QosStates new_state);
eHalStatus sme_QosBufferCmd(sme_QosCmdInfo *pcmd, v_BOOL_t insert_head);
static eHalStatus sme_QosProcessBufferedCmd(v_U8_t sessionId);
eHalStatus sme_QosSaveAssocInfo(sme_QosSessionInfo *pSession, sme_QosAssocInfo *pAssoc_info);
eHalStatus sme_QosSetupFnp(tpAniSirGlobal pMac, tListElem *pEntry);
eHalStatus sme_QosModificationNotifyFnp(tpAniSirGlobal pMac, tListElem *pEntry);
eHalStatus sme_QosModifyFnp(tpAniSirGlobal pMac, tListElem *pEntry);
eHalStatus sme_QosDelTsIndFnp(tpAniSirGlobal pMac, tListElem *pEntry);
eHalStatus sme_QosReassocSuccessEvFnp(tpAniSirGlobal pMac, tListElem *pEntry);
eHalStatus sme_QosAddTsFailureFnp(tpAniSirGlobal pMac, tListElem *pEntry);
eHalStatus sme_QosAddTsSuccessFnp(tpAniSirGlobal pMac, tListElem *pEntry);
static v_BOOL_t sme_QosIsRspPending(v_U8_t sessionId, sme_QosEdcaAcType ac);
static v_BOOL_t sme_QosIsUapsdActive(void);
void sme_QosPmcFullPowerCallback(void *callbackContext, eHalStatus status);
void sme_QosPmcStartUapsdCallback(void *callbackContext, eHalStatus status);
v_BOOL_t sme_QosPmcCheckRoutine(void *callbackContext);
void sme_QosPmcDeviceStateUpdateInd(void *callbackContext, tPmcState pmcState);
eHalStatus sme_QosProcessOutOfUapsdMode(tpAniSirGlobal pMac);
eHalStatus sme_QosProcessIntoUapsdMode(tpAniSirGlobal pMac);
static eHalStatus sme_QosBufferExistingFlows(tpAniSirGlobal pMac,
                                             v_U8_t sessionId);
static eHalStatus sme_QosDeleteExistingFlows(tpAniSirGlobal pMac,
                                             v_U8_t sessionId);
static void sme_QosCleanupCtrlBlkForHandoff(tpAniSirGlobal pMac,
                                            v_U8_t sessionId);
static eHalStatus sme_QosDeleteBufferedRequests(tpAniSirGlobal pMac,
                                                v_U8_t sessionId);
v_BOOL_t sme_QosValidateRequestedParams(tpAniSirGlobal pMac,
    sme_QosWmmTspecInfo * pQoSInfo,
    v_U8_t sessionId);

extern eHalStatus sme_AcquireGlobalLock( tSmeStruct *psSme);
extern eHalStatus sme_ReleaseGlobalLock( tSmeStruct *psSme);
static eHalStatus qosIssueCommand( tpAniSirGlobal pMac, v_U8_t sessionId,
                                   eSmeCommandType cmdType, sme_QosWmmTspecInfo * pQoSInfo,
                                   sme_QosEdcaAcType ac, v_U8_t tspec_mask );
/*
                                                                       
*/
static sme_QosStatusType sme_QosReRequestAddTS(tpAniSirGlobal pMac,
                                               v_U8_t sessionId,
                                               sme_QosWmmTspecInfo * pQoSInfo,
                                               sme_QosEdcaAcType ac,
                                               v_U8_t tspecMask);
static void sme_QosInitACs(tpAniSirGlobal pMac, v_U8_t sessionId);
static eHalStatus sme_QosRequestReassoc(tpAniSirGlobal pMac, tANI_U8 sessionId,
                                        tCsrRoamModifyProfileFields *pModFields,
                                        v_BOOL_t fForce );
static v_U32_t sme_QosAssignFlowId(void);
static v_U8_t sme_QosAssignDialogToken(void);
static eHalStatus sme_QosUpdateTspecMask(v_U8_t sessionId,
                                      sme_QosSearchInfo search_key,
                                      v_U8_t new_tspec_mask);
/*                                                                           
                                                  
                                                                          */
/*                                                                           
                                                                               
                   
                                                                
    
                           
                                                                            */
eHalStatus sme_QosOpen(tpAniSirGlobal pMac)
{
   sme_QosSessionInfo *pSession;
   v_U8_t sessionId;
   eHalStatus status;
   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
             "%s: %d: initializing SME-QoS module",
             __func__, __LINE__);
   //                      
   //                                               
   vos_mem_zero(&sme_QosCb, sizeof(sme_QosCb));
   sme_QosCb.pMac = pMac;
   sme_QosCb.nextFlowId = SME_QOS_MIN_FLOW_ID;
   sme_QosCb.nextDialogToken = SME_QOS_MIN_DIALOG_TOKEN;
   //              
   status = csrLLOpen(pMac->hHdd, &sme_QosCb.flow_list);
   if (!HAL_STATUS_SUCCESS(status))
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_FATAL,
                "%s: %d: cannot initialize Flow List",
                __func__, __LINE__);
      return eHAL_STATUS_FAILURE;
   }
   
   for (sessionId = 0; sessionId < CSR_ROAM_SESSION_MAX; ++sessionId)
   {
      pSession = &sme_QosCb.sessionInfo[sessionId];
      pSession->sessionId = sessionId;
      //                                            
      sme_QosInitACs(pMac, sessionId);
      //                                               
      status = csrLLOpen(pMac->hHdd, &pSession->bufferedCommandList);
      if (!HAL_STATUS_SUCCESS(status))
      {
         VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_FATAL,
                   "%s: %d: cannot initialize cmd list for session %d",
                   __func__, __LINE__,
                   sessionId);
         return eHAL_STATUS_FAILURE;
      }
      pSession->readyForPowerSave = VOS_TRUE;
   }
   //                                                                            
   //                                                                   
   //                                                                          
   if(!HAL_STATUS_SUCCESS(
      pmcRegisterPowerSaveCheck(pMac, sme_QosPmcCheckRoutine, pMac)))
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_FATAL,
                "%s: %d: cannot register with pmcRegisterPowerSaveCheck()",
                __func__, __LINE__);
      return eHAL_STATUS_FAILURE;
   }
   //                                                                          
   //                                                                           
   //                                                                       
   //                     
   if(!HAL_STATUS_SUCCESS(
      pmcRegisterDeviceStateUpdateInd(pMac, sme_QosPmcDeviceStateUpdateInd, pMac)))
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_FATAL,
                "%s: %d: cannot register with pmcRegisterDeviceStateUpdateInd()",
                __func__, __LINE__);
      return eHAL_STATUS_FAILURE;
   }
   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
             "%s: %d: done initializing SME-QoS module",
             __func__, __LINE__);
   return eHAL_STATUS_SUCCESS;
}
/*                                                                           
                                                                              
                                                                           
                        
                                                                
    
                           
                                                                            */
eHalStatus sme_QosClose(tpAniSirGlobal pMac)
{
   sme_QosSessionInfo *pSession;
   sme_QosEdcaAcType ac;
   v_U8_t sessionId;
   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
             "%s: %d: closing down SME-QoS",
             __func__, __LINE__);
   //                    
   if(!HAL_STATUS_SUCCESS(
      pmcDeregisterDeviceStateUpdateInd(pMac, sme_QosPmcDeviceStateUpdateInd)))
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_FATAL,
                "%s: %d: cannot deregister with pmcDeregisterDeviceStateUpdateInd()",
                __func__, __LINE__);
   }
   if(!HAL_STATUS_SUCCESS(
      pmcDeregisterPowerSaveCheck(pMac, sme_QosPmcCheckRoutine)))
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_FATAL,
                "%s: %d: cannot deregister with pmcDeregisterPowerSaveCheck()",
                __func__, __LINE__);
   }
   //                     
   //                   
   csrLLClose(&sme_QosCb.flow_list);
   //                              
   for(sessionId = 0; sessionId < CSR_ROAM_SESSION_MAX; ++sessionId)
   {
      pSession = &sme_QosCb.sessionInfo[sessionId];
      if (pSession == NULL)
            continue;

       sme_QosInitACs(pMac, sessionId);
       //                                   
       pSession->apsdMask = 0;

       pSession->uapsdAlreadyRequested = VOS_FALSE;
       pSession->handoffRequested = VOS_FALSE;
       pSession->readyForPowerSave = VOS_TRUE;
       pSession->roamID = 0;
       //                             
       sme_QosDeleteBufferedRequests(pMac, sessionId);
       //                      
       sme_QosDeleteExistingFlows(pMac, sessionId);

       //                                             
       if (pSession->assocInfo.pBssDesc) {
          vos_mem_free(pSession->assocInfo.pBssDesc);
          pSession->assocInfo.pBssDesc = NULL;
       }

      //                                          
      csrLLClose(&pSession->bufferedCommandList);
      for(ac = SME_QOS_EDCA_AC_BE; ac < SME_QOS_EDCA_AC_MAX; ac++) 
      {
         sme_QosStateTransition(sessionId, ac, SME_QOS_CLOSED);
      }
      pSession->sessionActive = VOS_FALSE;
      pSession->readyForPowerSave = VOS_TRUE;
   }
   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
             "%s: %d: closed down QoS",
             __func__, __LINE__);
   return eHAL_STATUS_SUCCESS;
}
/*                                                                          
                                                                               
                                                                           
                                                                              
                                                                            
                                                                            
                 
  
                                               
                                                           
                                                                               
                                                                  
                                                                       
                                                                             
                                                                                
                                     
                                                                              
                                                                 
                                                                               
                                                          
                                                                              
                                                                          
                                                                              
                           
                                                                            
                               
                  
                                                    
  
                                                      
     
  
                                                                            */
sme_QosStatusType sme_QosSetupReq(tHalHandle hHal, tANI_U32 sessionId,
                                  sme_QosWmmTspecInfo * pQoSInfo,
                                  sme_QosCallback QoSCallback,
                                  void * HDDcontext,
                                  sme_QosWmmUpType UPType, v_U32_t * pQosFlowID)
{
   sme_QosSessionInfo *pSession;
   eHalStatus lock_status = eHAL_STATUS_FAILURE;
   tpAniSirGlobal pMac = PMAC_STRUCT( hHal );
   sme_QosStatusType status;
   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
             "%s: %d: QoS Setup requested by client on session %d",
             __func__, __LINE__,
             sessionId);
   lock_status = sme_AcquireGlobalLock( &pMac->sme );
   if ( !HAL_STATUS_SUCCESS( lock_status ) )
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                "%s: %d: Unable to obtain lock",
                __func__, __LINE__);
      return SME_QOS_STATUS_SETUP_FAILURE_RSP;
   }
   //                              
   if (!CSR_IS_SESSION_VALID( pMac, sessionId ))
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                "%s: %d: Supplied Session ID %d is invalid",
                __func__, __LINE__,
                sessionId);
      status = SME_QOS_STATUS_SETUP_FAILURE_RSP;
   }
   else
   {
      //                               
      pSession = &sme_QosCb.sessionInfo[sessionId];
      if (!pSession->sessionActive)
      { 
         VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                   "%s: %d: Supplied Session ID %d is inactive",
                   __func__, __LINE__,
                   sessionId);
         status = SME_QOS_STATUS_SETUP_FAILURE_RSP;
      }
      else
      {
         //                
         *pQosFlowID = sme_QosAssignFlowId();
         VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                   "%s: %d: QoS request on session %d assigned Flow ID %d",
                   __func__, __LINE__,
                   sessionId, *pQosFlowID);
         //                                         
         //                              
         status = sme_QosInternalSetupReq(pMac, (v_U8_t)sessionId, pQoSInfo,
                                          QoSCallback, HDDcontext, UPType,
                                          *pQosFlowID, VOS_FALSE, VOS_FALSE);
      }
   }
   sme_ReleaseGlobalLock( &pMac->sme );
   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
             "%s: %d: QoS setup return status on session %d is %d",
             __func__, __LINE__,
             sessionId, status);
   return status;
}

/*                                                                          
                                                                            
                                                                           
                                                                                
                                                                             
                                                                              
                                                                            
                                                
  
                                               
                                                                               
                                                                  
                                                                             
                           
                                                                             
                                            
                  
                                                                        
  
                                                
     
  
                                                                            */
sme_QosStatusType sme_QosModifyReq(tHalHandle hHal, 
                                   sme_QosWmmTspecInfo * pQoSInfo,
                                   v_U32_t QosFlowID)
{
   eHalStatus lock_status = eHAL_STATUS_FAILURE;
   tpAniSirGlobal pMac = PMAC_STRUCT( hHal );
   sme_QosStatusType status;
   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
             "%s: %d: QoS Modify requested by client for Flow %d",
             __func__, __LINE__,
             QosFlowID);
   lock_status = sme_AcquireGlobalLock( &pMac->sme );
   if ( !HAL_STATUS_SUCCESS( lock_status ) )
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                "%s: %d: Unable to obtain lock",
                __func__, __LINE__);
      return SME_QOS_STATUS_MODIFY_SETUP_FAILURE_RSP;
   }
   //                                                                        
   status = sme_QosInternalModifyReq(pMac, pQoSInfo, QosFlowID, VOS_FALSE);
   sme_ReleaseGlobalLock( &pMac->sme );
   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
             "%s: %d: QoS Modify return status on Flow %d is %d",
             __func__, __LINE__,
             QosFlowID, status);
   return status;
}
/*                                                                          
                                                                             
                                                                           
                                                                            
                                                                            
  
                                               
                                                                                
                                                                            
                               
  
                                                      
  
     
  
                                                                            */
sme_QosStatusType sme_QosReleaseReq(tHalHandle hHal, v_U32_t QosFlowID)
{
   eHalStatus lock_status = eHAL_STATUS_FAILURE;
   tpAniSirGlobal pMac = PMAC_STRUCT( hHal );
   sme_QosStatusType status;
   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
             "%s: %d: QoS Release requested by client for Flow %d",
             __func__, __LINE__,
             QosFlowID);
   lock_status = sme_AcquireGlobalLock( &pMac->sme );
   if ( !HAL_STATUS_SUCCESS( lock_status ) )
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                "%s: %d: Unable to obtain lock",
                __func__, __LINE__);
      return SME_QOS_STATUS_RELEASE_FAILURE_RSP;
   }
   //                                                                         
   status = sme_QosInternalReleaseReq(pMac, QosFlowID, VOS_FALSE);
   sme_ReleaseGlobalLock( &pMac->sme );
   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
             "%s: %d: QoS Release return status on Flow %d is %d",
             __func__, __LINE__,
             QosFlowID, status);
   return status;
}
/*                                                                          
                                                                          
                               
  
                                                              
                                                                               
                                                                         
  
                                                       
  
     
  
                                                                            */
eHalStatus sme_QosSetParams(tpAniSirGlobal pMac, sme_QosWmmTspecInfo * pQoSInfo)
{
   sme_QosEdcaAcType ac;
   //            
   ac = sme_QosUpToAc(pQoSInfo->ts_info.up);
   if(SME_QOS_EDCA_AC_MAX == ac)
   {
      //       
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                "%s: %d: Invalid AC %d (via UP %d)",
                __func__, __LINE__,
                ac, pQoSInfo->ts_info.up );
      return eHAL_STATUS_FAILURE;
   }
   //                                        
   sme_QosCb.def_QoSInfo[ac] = *pQoSInfo;
   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
             "%s: %d: QoS default params set for AC %d (via UP %d)",
             __func__, __LINE__,
             ac, pQoSInfo->ts_info.up );
   return eHAL_STATUS_SUCCESS;
}

void qosReleaseCommand( tpAniSirGlobal pMac, tSmeCmd *pCommand )
{
   vos_mem_zero( &pCommand->u.qosCmd, sizeof( tGenericQosCmd ) );
   smeReleaseCommand( pMac, pCommand );
}

/*                                                                          
                                                                              
                                              
  
                                                              
                                                                       
                                                                              
                                       
                                                                            
  
                                                         
  
     
  
                                                                            */
eHalStatus sme_QosMsgProcessor( tpAniSirGlobal pMac,  v_U16_t msg_type, 
                                void *pMsgBuf)
{
   eHalStatus status = eHAL_STATUS_FAILURE;
   tListElem *pEntry;
   tSmeCmd *pCommand;
   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
             "%s: %d: msg = %d for QoS",
             __func__, __LINE__, msg_type);
   //                                                              
   switch(msg_type)
   {
      case eWNI_SME_ADDTS_RSP:
         pEntry = csrLLPeekHead(&pMac->sme.smeCmdActiveList, LL_ACCESS_LOCK);
         if( pEntry )
         {
             pCommand = GET_BASE_ADDR(pEntry, tSmeCmd, Link);
             if( eSmeCommandAddTs == pCommand->command )
             {
                status = sme_QosProcessAddTsRsp(pMac, pMsgBuf);
                if( csrLLRemoveEntry( &pMac->sme.smeCmdActiveList, pEntry, LL_ACCESS_LOCK ) )
                {
                   qosReleaseCommand( pMac, pCommand );
                }
                smeProcessPendingQueue( pMac );
             }
         }
         break;
      case eWNI_SME_DELTS_RSP:
         pEntry = csrLLPeekHead(&pMac->sme.smeCmdActiveList, LL_ACCESS_LOCK);
         if( pEntry )
         {
             pCommand = GET_BASE_ADDR(pEntry, tSmeCmd, Link);
             if( eSmeCommandDelTs == pCommand->command )
             {
                status = sme_QosProcessDelTsRsp(pMac, pMsgBuf);
                if( csrLLRemoveEntry( &pMac->sme.smeCmdActiveList, pEntry, LL_ACCESS_LOCK ) )
                {
                   qosReleaseCommand( pMac, pCommand );
                }
                smeProcessPendingQueue( pMac );
             }
         }
         break;
      case eWNI_SME_DELTS_IND:
         status = sme_QosProcessDelTsInd(pMac, pMsgBuf);
         break;
#ifdef WLAN_FEATURE_VOWIFI_11R
      case eWNI_SME_FT_AGGR_QOS_RSP:
         status = sme_QosProcessAggrQosRsp(pMac, pMsgBuf);
         break;
#endif

      default:
         //       
         VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                   "%s: %d: unknown msg type = %d",
                   __func__, __LINE__, msg_type);
         break;
   }
   return status;
}
/*                                                                          
                                                                                
                                                                          
  
                                                              
                                                                             
                                                            
  
                    
  
     
  
                                                                            */
eHalStatus sme_QosValidateParams(tpAniSirGlobal pMac, 
                                 tSirBssDescription *pBssDesc)
{
   tDot11fBeaconIEs *pIes = NULL;
   eHalStatus status = eHAL_STATUS_FAILURE;
   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH,
             "%s: %d: validation for QAP & APSD",
             __func__, __LINE__);
   do
   {
      if(!HAL_STATUS_SUCCESS(csrGetParsedBssDescriptionIEs(pMac, pBssDesc, &pIes)))
      {
         //       
         VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                   "%s: %d: csrGetParsedBssDescriptionIEs() failed",
                   __func__, __LINE__);
         break;
      }
      //                                         
      if( !CSR_IS_QOS_BSS(pIes) )
      {
         //       
         VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                   "%s: %d: AP doesn't support QoS",
                   __func__, __LINE__);
         
         break;
      }
      if(!(pIes->WMMParams.qosInfo & SME_QOS_AP_SUPPORTS_APSD) &&
         !(pIes->WMMInfoAp.uapsd))
      {
         //       
         VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                   "%s: %d: AP doesn't support APSD",
                   __func__, __LINE__);
         break;
      }
      status = eHAL_STATUS_SUCCESS;
   }while(0);
   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
             "%s: %d: validated with status = %d",
             __func__, __LINE__, status);
   if(pIes)
   {
      vos_mem_free(pIes);
   }
   return status;
}
/*                                                                          
                                                                                
                                                                            
                                                              
                                                                 
                                                       
  
                    
  
     
  
                                                                            */
eHalStatus sme_QosCsrEventInd(tpAniSirGlobal pMac,
                              v_U8_t sessionId,
                              sme_QosCsrEventIndType ind, 
                              void *pEvent_info)
{
   eHalStatus status = eHAL_STATUS_FAILURE;
   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
             "%s: %d: On Session %d Event %d received from CSR",
             __func__, __LINE__,
             sessionId, ind );
   switch(ind)
   {
      case SME_QOS_CSR_ASSOC_COMPLETE:
         //                                   
         status = sme_QosProcessAssocCompleteEv(pMac, sessionId, pEvent_info);
         break;
      case SME_QOS_CSR_REASSOC_REQ:
         //                               
         status = sme_QosProcessReassocReqEv(pMac, sessionId, pEvent_info);
         break;
      case SME_QOS_CSR_REASSOC_COMPLETE:
         //                                   
         status = sme_QosProcessReassocSuccessEv(pMac, sessionId, pEvent_info);
         break;
      case SME_QOS_CSR_REASSOC_FAILURE:
         //                               
         status = sme_QosProcessReassocFailureEv(pMac, sessionId, pEvent_info);
         break;
      case SME_QOS_CSR_DISCONNECT_REQ:
      case SME_QOS_CSR_DISCONNECT_IND:
         //                               
         status = sme_QosProcessDisconnectEv(pMac, sessionId, pEvent_info);
         break;
      case SME_QOS_CSR_JOIN_REQ:
         //                               
         status = sme_QosProcessJoinReqEv(pMac, sessionId, pEvent_info);
         break;
      case SME_QOS_CSR_HANDOFF_ASSOC_REQ:
         //                               
         status = sme_QosProcessHandoffAssocReqEv(pMac, sessionId, pEvent_info);
         break;
      case SME_QOS_CSR_HANDOFF_COMPLETE:
         //                               
         status = sme_QosProcessHandoffSuccessEv(pMac, sessionId, pEvent_info);
         break;
      case SME_QOS_CSR_HANDOFF_FAILURE:
         //                               
         status = sme_QosProcessHandoffFailureEv(pMac, sessionId, pEvent_info);
         break;
#ifdef WLAN_FEATURE_VOWIFI_11R
      case SME_QOS_CSR_PREAUTH_SUCCESS_IND:
         status = sme_QosProcessPreauthSuccessInd(pMac, sessionId, pEvent_info);
         break;
#if defined(FEATURE_WLAN_CCX) || defined(FEATURE_WLAN_LFR)
      case SME_QOS_CSR_SET_KEY_SUCCESS_IND:
         status = sme_QosProcessSetKeySuccessInd(pMac, sessionId, pEvent_info);
         break;
#endif
#endif
      default:
         //       
         VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                   "%s: %d: On Session %d Unknown Event %d received from CSR",
                   __func__, __LINE__,
                   sessionId, ind );
         break;
   }
   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
             "%s: %d: On Session %d processed Event %d with status %d",
             __func__, __LINE__,
             sessionId, ind, status );
   return status;
}
/*                                                                          
                                                                              
                                         
                                                                 
                                                              
                                                                         

                                                                 
  
     
  
                                                                            */
v_U8_t sme_QosGetACMMask(tpAniSirGlobal pMac, tSirBssDescription *pSirBssDesc, tDot11fBeaconIEs *pIes)
{
   sme_QosEdcaAcType ac;
   v_U8_t acm_mask = 0;
   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
             "%s: %d: invoked",
             __func__, __LINE__);
   for(ac = SME_QOS_EDCA_AC_BE; ac < SME_QOS_EDCA_AC_MAX; ac++)
   {
      if(sme_QosIsACM(pMac, pSirBssDesc, ac, pIes))
      {
         acm_mask = acm_mask | (1 << (SME_QOS_EDCA_AC_VO - ac));
      }
      
   }
   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
             "%s: %d: mask is %d",
             __func__, __LINE__, acm_mask);
   return acm_mask;
}
/*                                                                           
                                                      
                                                                          */
/*                                                                          
                                                                                
           
                                                                              
                                                                               
                                     
  
                                                              
                                                                               
                                                                  
                                                                       
                                                                             
                                                                                
                                     
                                                                              
                                                                 
                                                                               
                                                          
                                                                              
                                                                          
                                                                             
                           
                                                                            
                               
                                                                             
                              
                  
                                                    
  
                                                      
     
  
                                                                            */
sme_QosStatusType sme_QosInternalSetupReq(tpAniSirGlobal pMac, 
                                          v_U8_t sessionId,
                                          sme_QosWmmTspecInfo * pQoSInfo,
                                          sme_QosCallback QoSCallback, 
                                          void * HDDcontext,
                                          sme_QosWmmUpType UPType, 
                                          v_U32_t QosFlowID,
                                          v_BOOL_t buffered_cmd,
                                          v_BOOL_t hoRenewal)
{
   sme_QosSessionInfo *pSession;
   sme_QosACInfo *pACInfo;
   sme_QosEdcaAcType ac;
   sme_QosWmmTspecInfo Tspec_Info;
   sme_QosStates new_state = SME_QOS_CLOSED;
   sme_QosFlowInfoEntry *pentry = NULL;
   sme_QosCmdInfo  cmd;
   sme_QosStatusType status = SME_QOS_STATUS_SETUP_FAILURE_RSP;
   v_U8_t tmask = 0;
   v_U8_t new_tmask = 0;
   sme_QosSearchInfo search_key;
   v_BOOL_t bufferCommand;
   eHalStatus hstatus;
   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
             "%s: %d: invoked on session %d for flow %d",
             __func__, __LINE__,
             sessionId, QosFlowID);
   pSession = &sme_QosCb.sessionInfo[sessionId];
   //                                                            
   if(!pQoSInfo)
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_WARN, 
                "%s: %d: caller sent an empty QoS param list, using defaults",
                __func__, __LINE__);
      //                                  
      ac = sme_QosUpToAc(UPType);
      if(SME_QOS_EDCA_AC_MAX == ac)
      {
         VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                   "%s: %d: invalid AC %d from UP %d",
                   __func__, __LINE__,
                   ac, UPType);
         
         return SME_QOS_STATUS_SETUP_INVALID_PARAMS_RSP;
      }
      Tspec_Info = sme_QosCb.def_QoSInfo[ac];
   }
   else
   {
      //            
      ac = sme_QosUpToAc(pQoSInfo->ts_info.up);
      if(SME_QOS_EDCA_AC_MAX == ac)
      {
         VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                   "%s: %d: invalid AC %d from UP %d",
                   __func__, __LINE__,
                   ac, pQoSInfo->ts_info.up);
         
         return SME_QOS_STATUS_SETUP_INVALID_PARAMS_RSP;
      }
      //                   
      if(!sme_QosValidateRequestedParams(pMac, pQoSInfo, sessionId))
      {
         VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                   "%s: %d: invalid params",
                   __func__, __LINE__);
         return SME_QOS_STATUS_SETUP_INVALID_PARAMS_RSP;
      }
      Tspec_Info = *pQoSInfo;
   }
   pACInfo = &pSession->ac_info[ac];
   //                                                            
   pSession->readyForPowerSave = VOS_FALSE;
   //                                               
   bufferCommand = VOS_FALSE;
   //                                                
   //                                                                          
   //                                                                      
   //                                                                           
   //                                          
   if(sme_QosIsRspPending(sessionId, ac))
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_MED, 
                "%s: %d: buffering the setup request for flow %d in state %d "
                "since another request is pending",
                __func__, __LINE__, 
                QosFlowID, pACInfo->curr_state );
      bufferCommand = VOS_TRUE;
   }
   else
   {
      //                                                    
      //                                 
      hstatus = pmcRequestFullPower(pMac, sme_QosPmcFullPowerCallback,
                                    pSession, eSME_REASON_OTHER);
      if( eHAL_STATUS_PMC_PENDING == hstatus )
      {
         VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_MED, 
                   "%s: %d: buffering the setup request for flow %d in state %d, "
                   "waiting for full power",
                   __func__, __LINE__, 
                   QosFlowID, pACInfo->curr_state );
         bufferCommand = VOS_TRUE;
      }
   }
   if (bufferCommand)
   {
      //                              
      cmd.command = SME_QOS_SETUP_REQ;
      cmd.pMac = pMac;
      cmd.sessionId = sessionId;
      cmd.u.setupCmdInfo.HDDcontext = HDDcontext;
      cmd.u.setupCmdInfo.QoSInfo = Tspec_Info;
      cmd.u.setupCmdInfo.QoSCallback = QoSCallback;
      cmd.u.setupCmdInfo.UPType = UPType;
      cmd.u.setupCmdInfo.hoRenewal = hoRenewal;
      cmd.u.setupCmdInfo.QosFlowID = QosFlowID;
      hstatus = sme_QosBufferCmd(&cmd, buffered_cmd);
      if(!HAL_STATUS_SUCCESS(hstatus))
      {
         VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                   "%s: %d: couldn't buffer the setup request in state = %d",
                   __func__, __LINE__,
                   pACInfo->curr_state );
         //                             
         //                                             
         pSession->readyForPowerSave = VOS_TRUE;
         return SME_QOS_STATUS_SETUP_FAILURE_RSP;
      }
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                "%s: %d: Buffered setup request for flow = %d",
                __func__, __LINE__,
                QosFlowID);
      return SME_QOS_STATUS_SETUP_REQ_PENDING_RSP;
   }

   //                                                           
   switch(pACInfo->curr_state)
   {
   case SME_QOS_LINK_UP:
      //                                                     
      //                                                                       
      status = sme_QosSetup(pMac, sessionId, &Tspec_Info, ac);
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                "%s: %d: On session %d with AC %d in state SME_QOS_LINK_UP "
                "sme_QosSetup returned with status %d",
                __func__, __LINE__,
                sessionId, ac, status);
      if(SME_QOS_STATUS_SETUP_REQ_PENDING_RSP != status)
      {
         //                                             
         //                          
         pSession->readyForPowerSave = VOS_TRUE;
      }
      if((SME_QOS_STATUS_SETUP_REQ_PENDING_RSP == status)||
         (SME_QOS_STATUS_SETUP_SUCCESS_NO_ACM_NO_APSD_RSP == status) ||
         (SME_QOS_STATUS_SETUP_SUCCESS_APSD_SET_ALREADY == status))
      {
         //                                      
         //                                
         pentry = vos_mem_malloc(sizeof(*pentry));
         if (!pentry)
         {
            VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR,
                      "%s: %d: couldn't allocate memory for the new "
                      "entry in the Flow List",
                      __func__, __LINE__);
            return SME_QOS_STATUS_SETUP_FAILURE_RSP;
         }
         pentry->ac_type = ac;
         pentry->HDDcontext = HDDcontext;
         pentry->QoSCallback = QoSCallback;
         pentry->hoRenewal = hoRenewal;
         pentry->QosFlowID = QosFlowID;
         pentry->sessionId = sessionId;
         //                                                       
         //                                                    
         pACInfo->requested_QoSInfo[SME_QOS_TSPEC_INDEX_0] = Tspec_Info;
         if(SME_QOS_STATUS_SETUP_REQ_PENDING_RSP == status)
         {
            if(pACInfo->tspec_mask_status &&
               !pACInfo->reassoc_pending)
            {
               VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                         "%s: %d: On session %d with AC %d in state "
                            "SME_QOS_LINK_UP tspec_mask_status is %d "
                         "but should not be set yet",
                         __func__, __LINE__,
                         sessionId, ac, pACInfo->tspec_mask_status);
               //      
               VOS_ASSERT(0);
               vos_mem_free(pentry);
               return SME_QOS_STATUS_SETUP_FAILURE_RSP;
            }
            pACInfo->tspec_mask_status = SME_QOS_TSPEC_MASK_BIT_1_SET;
            if(!pACInfo->reassoc_pending)
            {
               //                                                              
               pACInfo->tspec_pending = 1;
            }
             
            pentry->reason = SME_QOS_REASON_SETUP;
            new_state = SME_QOS_REQUESTED;
         }
         else
         {
            //                                                   
            //                                              
            pentry->reason = SME_QOS_REASON_REQ_SUCCESS;
            new_state = SME_QOS_QOS_ON;
            pACInfo->tspec_mask_status = SME_QOS_TSPEC_MASK_BIT_1_SET;
            pACInfo->curr_QoSInfo[SME_QOS_TSPEC_INDEX_0] = Tspec_Info;
            if(buffered_cmd && !pentry->hoRenewal)
            {
               QoSCallback(pMac, HDDcontext, 
                           &pACInfo->curr_QoSInfo[SME_QOS_TSPEC_INDEX_0],
                           status,
                           pentry->QosFlowID);
            }
            pentry->hoRenewal = VOS_FALSE;
         }
         pACInfo->num_flows[SME_QOS_TSPEC_INDEX_0]++;

         //                                                                  
         //                    
         pentry->tspec_mask = pACInfo->tspec_mask_status;
         pentry->QoSInfo = Tspec_Info;
         VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                   "%s: %d: Creating entry on session %d at %p with flowID %d",
                   __func__, __LINE__,
                   sessionId, pentry, QosFlowID);
         csrLLInsertTail(&sme_QosCb.flow_list, &pentry->link, VOS_TRUE);
      }
      else
      {
         //                                             
         VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                   "%s: %d: On session %d unexpected status %d "
                   "returned by sme_QosSetup",
                   __func__, __LINE__,
                   sessionId, status);
         new_state = pACInfo->curr_state;
         if(buffered_cmd && hoRenewal)
         {
            QoSCallback(pMac, HDDcontext, 
                        &pACInfo->curr_QoSInfo[SME_QOS_TSPEC_INDEX_0],
                        SME_QOS_STATUS_RELEASE_QOS_LOST_IND,
                        QosFlowID);
         }
      }
      break;
   case SME_QOS_HANDOFF:
   case SME_QOS_REQUESTED:
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_MED, 
                "%s: %d: Buffering setup request for flow %d in state = %d",
                __func__, __LINE__,
                QosFlowID, pACInfo->curr_state );
      //          
      cmd.command = SME_QOS_SETUP_REQ;
      cmd.pMac = pMac;
      cmd.sessionId = sessionId;
      cmd.u.setupCmdInfo.HDDcontext = HDDcontext;
      cmd.u.setupCmdInfo.QoSInfo = Tspec_Info;
      cmd.u.setupCmdInfo.QoSCallback = QoSCallback;
      cmd.u.setupCmdInfo.UPType = UPType;
      cmd.u.setupCmdInfo.hoRenewal = hoRenewal;
      cmd.u.setupCmdInfo.QosFlowID = QosFlowID;
      hstatus = sme_QosBufferCmd(&cmd, buffered_cmd);
      if(!HAL_STATUS_SUCCESS(hstatus))
      {
         VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                   "%s: %d: On session %d couldn't buffer the setup "
                   "request for flow %d in state = %d",
                   __func__, __LINE__,
                   sessionId, QosFlowID, pACInfo->curr_state );
         //                             
         //                                             
         pSession->readyForPowerSave = VOS_TRUE;
         return SME_QOS_STATUS_SETUP_FAILURE_RSP;
      }
      status = SME_QOS_STATUS_SETUP_REQ_PENDING_RSP;
      new_state = pACInfo->curr_state;
      break;
   case SME_QOS_QOS_ON:
      
      //                                         
      if((pACInfo->num_flows[SME_QOS_TSPEC_INDEX_0] > 0)||
         (pACInfo->num_flows[SME_QOS_TSPEC_INDEX_1] > 0))
      {
         //                                                                     
         if(CSR_IS_ADDTS_WHEN_ACMOFF_SUPPORTED(pMac) ||
            sme_QosIsACM(pMac, pSession->assocInfo.pBssDesc, ac, NULL))
         {
            VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_MED, 
                      "%s: %d: tspec_mask_status = %d for AC = %d",
                      __func__, __LINE__,
                      pACInfo->tspec_mask_status, ac);
            if(!pACInfo->tspec_mask_status)
            {
               VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                         "%s: %d: tspec_mask_status can't be 0 for ac = %d in "
                         "state = %d",
                         __func__, __LINE__,
                         ac, pACInfo->curr_state);
               //      
               VOS_ASSERT(0);
               //                              
               //                                             
               pSession->readyForPowerSave = VOS_TRUE;
               return status;
            }
            /*                  */
            if(SME_QOS_TSPEC_MASK_BIT_1_2_SET != pACInfo->tspec_mask_status)
            {
              /*                                                                   */
              /*                                                                          
                                                                                               */
              if((Tspec_Info.ts_info.direction == SME_QOS_WMM_TS_DIR_BOTH) ||
                 (pACInfo->curr_QoSInfo[pACInfo->tspec_mask_status - 1].
                      ts_info.direction == SME_QOS_WMM_TS_DIR_BOTH))
              {
                //                                                     
                tmask = pACInfo->tspec_mask_status;
              }
              /*                                                                          
                                                                                         */
              else if(pACInfo->curr_QoSInfo[pACInfo->tspec_mask_status - 1].
                  ts_info.direction == Tspec_Info.ts_info.direction)
              {
                //                                                     
                tmask = pACInfo->tspec_mask_status;
              }
              /*                                       */
              else
              {
                //                                                         
                tmask = SME_QOS_TSPEC_MASK_CLEAR;
                new_tmask = SME_QOS_TSPEC_MASK_BIT_1_2_SET & ~pACInfo->tspec_mask_status;
                pACInfo->tspec_mask_status = SME_QOS_TSPEC_MASK_BIT_1_2_SET;
              }
            }
            else
            {
              /*                                               */
              /*                                                                       
                                                                           */
              if(Tspec_Info.ts_info.direction == SME_QOS_WMM_TS_DIR_BOTH)
              {
                //                                                                
                tmask = SME_QOS_TSPEC_MASK_BIT_1_2_SET;
                pACInfo->tspec_mask_status = SME_QOS_TSPEC_MASK_BIT_1_SET;
              }
              /*                                   
                                                                               */
              else if(pACInfo->curr_QoSInfo[SME_QOS_TSPEC_INDEX_0].ts_info.direction != 
                   Tspec_Info.ts_info.direction)
              {
                //                               
                tmask = SME_QOS_TSPEC_MASK_BIT_2_SET;
              }
              else
              {
                //                               
                tmask = SME_QOS_TSPEC_MASK_BIT_1_SET;
              }
            }
         }
         else
         {
            //       
            //                                                        
            //                                                   
            tmask = SME_QOS_TSPEC_MASK_BIT_1_SET;
         }
         VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_MED,
                   "%s: %d: tmask = %d, new_tmask = %d in state = %d",
                   __func__, __LINE__,
                   tmask, new_tmask, pACInfo->curr_state );
         VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_MED,
                   "%s: %d: tspec_mask_status = %d for AC = %d",
                   __func__, __LINE__,
                   pACInfo->tspec_mask_status, ac);
         if(tmask)
         {
            //                           
            if(tmask != SME_QOS_TSPEC_MASK_BIT_1_2_SET)
            {
              hstatus = sme_QosAggregateParams(&Tspec_Info, 
                                               &pACInfo->curr_QoSInfo[tmask - 1],
                                               &pACInfo->requested_QoSInfo[tmask - 1]);
            }
            else
            {
              /*                                                                        
                                                       */
              tmask = SME_QOS_TSPEC_MASK_BIT_1_SET;

              if((hstatus = sme_QosAggregateParams(&Tspec_Info, 
                                                   &pACInfo->curr_QoSInfo[SME_QOS_TSPEC_INDEX_0],
                                                   &pACInfo->requested_QoSInfo[tmask - 1]))
                          == eHAL_STATUS_SUCCESS)
              {
                hstatus = sme_QosAggregateParams(&pACInfo->curr_QoSInfo[SME_QOS_TSPEC_INDEX_1], 
                                                 &pACInfo->requested_QoSInfo[tmask - 1],
                                                 NULL);
              }
            }

            if(!HAL_STATUS_SUCCESS(hstatus))
            {
               //       
               VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                         "%s: %d: failed to aggregate params",
                         __func__, __LINE__);
               //                              
               //                                             
               pSession->readyForPowerSave = VOS_TRUE;
               return SME_QOS_STATUS_SETUP_FAILURE_RSP;
            }
         }
         else
         {
            tmask = new_tmask;
            pACInfo->requested_QoSInfo[tmask-1] = Tspec_Info;
         }
      }
      else
      {
         VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                   "%s: %d: no flows running for ac = %d while in state = %d",
                   __func__, __LINE__,
                   ac, pACInfo->curr_state );
         //      
         VOS_ASSERT(0);
         //                              
         //                                             
         pSession->readyForPowerSave = VOS_TRUE;
         return status;
      }
      //                                                            
      pACInfo->requested_QoSInfo[tmask - 1].ts_info.up = Tspec_Info.ts_info.up;
      status = sme_QosSetup(pMac, sessionId,
                            &pACInfo->requested_QoSInfo[tmask - 1], ac);
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                "%s: %d: On session %d with AC %d in state SME_QOS_QOS_ON "
                "sme_QosSetup returned with status %d",
                __func__, __LINE__,
                sessionId, ac, status);
      if(SME_QOS_STATUS_SETUP_REQ_PENDING_RSP != status)
      {
         //                                             
         //                          
         pSession->readyForPowerSave = VOS_TRUE;
      }
      if((SME_QOS_STATUS_SETUP_REQ_PENDING_RSP == status)||
         (SME_QOS_STATUS_SETUP_SUCCESS_NO_ACM_NO_APSD_RSP == status) ||
         (SME_QOS_STATUS_SETUP_SUCCESS_APSD_SET_ALREADY == status))
      {
         //                                      
         //                                
         pentry = (sme_QosFlowInfoEntry *) vos_mem_malloc(sizeof(*pentry));
         if (!pentry)
         {
            VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR,
                      "%s: %d: couldn't allocate memory for the new "
                      "entry in the Flow List",
                      __func__, __LINE__);
            return SME_QOS_STATUS_SETUP_FAILURE_RSP;
         }
         pentry->ac_type = ac;
         pentry->HDDcontext = HDDcontext;
         pentry->QoSCallback = QoSCallback;
         pentry->hoRenewal = hoRenewal;
         pentry->QosFlowID = QosFlowID;
         pentry->sessionId = sessionId;
         VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                   "%s: %d: Creating flow %d",
                   __func__, __LINE__,
                   QosFlowID);
         if((SME_QOS_STATUS_SETUP_SUCCESS_NO_ACM_NO_APSD_RSP == status)||
            (SME_QOS_STATUS_SETUP_SUCCESS_APSD_SET_ALREADY == status))
         {
            new_state = pACInfo->curr_state;
            pentry->reason = SME_QOS_REASON_REQ_SUCCESS;
            pACInfo->curr_QoSInfo[SME_QOS_TSPEC_INDEX_0] = 
               pACInfo->requested_QoSInfo[SME_QOS_TSPEC_INDEX_0];
            if(buffered_cmd && !pentry->hoRenewal)
            {
               QoSCallback(pMac, HDDcontext, 
                           &pACInfo->curr_QoSInfo[SME_QOS_TSPEC_INDEX_0],
                           status,
                           pentry->QosFlowID);
            }
            if(SME_QOS_STATUS_SETUP_SUCCESS_APSD_SET_ALREADY == status)
            {
               //                                                   
               //                                                  
               if(!pentry->hoRenewal)
               {
                  vos_mem_zero(&search_key, sizeof(sme_QosSearchInfo));
                  search_key.key.ac_type = ac;
                  search_key.index = SME_QOS_SEARCH_KEY_INDEX_2;
                  search_key.sessionId = sessionId;
                  hstatus = sme_QosFindAllInFlowList(pMac, search_key,
                                                     sme_QosSetupFnp);
                  if(!HAL_STATUS_SUCCESS(hstatus))
                  {
                     VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                               "%s: %d: couldn't notify other "
                               "entries on this AC =%d",
                               __func__, __LINE__, ac);
                  }
               }
            }
            pentry->hoRenewal = VOS_FALSE;
         }
         else
         {
            //                                     
            new_state = SME_QOS_REQUESTED;
            pentry->reason = SME_QOS_REASON_SETUP;
            //                                                       
            //                                          
            pACInfo->tspec_pending = tmask;
         }
         pACInfo->num_flows[tmask - 1]++;
         //                                                                  
         //                    
         pentry->tspec_mask = tmask;
         pentry->QoSInfo = Tspec_Info;
         VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                   "%s: %d: On session %d creating entry at %p with flowID %d",
                   __func__, __LINE__,
                   sessionId, pentry, QosFlowID);
         csrLLInsertTail(&sme_QosCb.flow_list, &pentry->link, VOS_TRUE);
      }
      else
      {
         //                                             
         VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                   "%s: %d: On session %d unexpected status %d "
                   "returned by sme_QosSetup",
                   __func__, __LINE__,
                   sessionId, status);
         new_state = pACInfo->curr_state;
      }
      break;
   case SME_QOS_CLOSED:
   case SME_QOS_INIT:
   default:
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                "%s: %d: setup requested in unexpected state = %d",
                __func__, __LINE__,
                pACInfo->curr_state);
      //                              
      //                                             
      pSession->readyForPowerSave = VOS_TRUE;
      VOS_ASSERT(0);
      new_state = pACInfo->curr_state;
   }
   /*                                                              
                                                                                
                                                                              
           
   */
   if((new_state != pACInfo->curr_state)&&
      (!(pACInfo->reassoc_pending && 
         (SME_QOS_HANDOFF == pACInfo->curr_state))))
   {
      sme_QosStateTransition(sessionId, ac, new_state);
   }
   
   if(pACInfo->reassoc_pending && 
      (SME_QOS_HANDOFF == pACInfo->curr_state))
   {
      pACInfo->prev_state = SME_QOS_REQUESTED;
   }
   if((SME_QOS_STATUS_SETUP_SUCCESS_NO_ACM_NO_APSD_RSP == status) ||
      (SME_QOS_STATUS_SETUP_SUCCESS_APSD_SET_ALREADY == status)) 
   {
      (void)sme_QosProcessBufferedCmd(sessionId);
   }
   return status;
}

/*                                                                          
                                                                               
                                                                               
                                                                              
                                                                            
                                                
  
                                                              
                                                                               
                                                                  
                                                                             
                           
                                                                             
                                            
                  
                                                                        
  
                                                
     
  
                                                                            */
sme_QosStatusType sme_QosInternalModifyReq(tpAniSirGlobal pMac, 
                                           sme_QosWmmTspecInfo * pQoSInfo,
                                           v_U32_t QosFlowID,
                                           v_BOOL_t buffered_cmd)
{
   tListElem *pEntry= NULL;
   sme_QosSessionInfo *pSession;
   sme_QosACInfo *pACInfo;
   sme_QosFlowInfoEntry *pNewEntry= NULL;
   sme_QosFlowInfoEntry *flow_info = NULL;
   sme_QosEdcaAcType ac;
   sme_QosStates new_state = SME_QOS_CLOSED;
   sme_QosStatusType status = SME_QOS_STATUS_MODIFY_SETUP_FAILURE_RSP;
   sme_QosWmmTspecInfo Aggr_Tspec_Info;
   sme_QosSearchInfo search_key;
   sme_QosCmdInfo  cmd;
   v_U8_t sessionId;
   v_BOOL_t bufferCommand;
   eHalStatus hstatus;
   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
             "%s: %d: invoked for flow %d",
             __func__, __LINE__,
             QosFlowID);

   vos_mem_zero(&search_key, sizeof(sme_QosSearchInfo));
   //                                                          
   search_key.key.QosFlowID = QosFlowID;
   search_key.index = SME_QOS_SEARCH_KEY_INDEX_1;
   search_key.sessionId = SME_QOS_SEARCH_SESSION_ID_ANY;
   //                                                            
   pEntry = sme_QosFindInFlowList(search_key);
   if(!pEntry)
   {
      //       
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                "%s: %d: no match found for flowID = %d",
                __func__, __LINE__,
                QosFlowID);
      return SME_QOS_STATUS_MODIFY_SETUP_INVALID_PARAMS_RSP;
   }
   //            
   flow_info = GET_BASE_ADDR( pEntry, sme_QosFlowInfoEntry, link );
   ac = flow_info->ac_type;

   sessionId = flow_info->sessionId;
   pSession = &sme_QosCb.sessionInfo[sessionId];
   pACInfo = &pSession->ac_info[ac];

   //                   
   if(!sme_QosValidateRequestedParams(pMac, pQoSInfo, sessionId))
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                "%s: %d: invalid params",
                __func__, __LINE__);
      return SME_QOS_STATUS_MODIFY_SETUP_INVALID_PARAMS_RSP;
   }
   //                                                                       
   if((pQoSInfo->ts_info.direction != flow_info->QoSInfo.ts_info.direction) ||
      (pQoSInfo->ts_info.up != flow_info->QoSInfo.ts_info.up) ||
      (pQoSInfo->ts_info.tid != flow_info->QoSInfo.ts_info.tid))
   {
     VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
               "%s: %d: Modification of direction/tid/up is not allowed",
               __func__, __LINE__);

     return SME_QOS_STATUS_MODIFY_SETUP_INVALID_PARAMS_RSP;
   }
   //                                                            
   pSession->readyForPowerSave = VOS_FALSE;
   //                                               
   bufferCommand = VOS_FALSE;
   //                                                
   //                                                                          
   //                                                                      
   //                                                                           
   //                                          
   if(sme_QosIsRspPending(sessionId, ac))
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_MED, 
                "%s: %d: buffering the modify request for flow %d in state %d "
                "since another request is pending",
                __func__, __LINE__, 
                QosFlowID, pACInfo->curr_state );
      bufferCommand = VOS_TRUE;
   }
   else
   {
      //                                                    
      //                                 
      hstatus = pmcRequestFullPower(pMac, sme_QosPmcFullPowerCallback,
                                    pSession, eSME_REASON_OTHER);
      if( eHAL_STATUS_PMC_PENDING == hstatus )
      {
         VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_MED, 
                   "%s: %d: buffering the modify request for flow %d in state %d, "
                   "waiting for full power",
                   __func__, __LINE__, 
                   QosFlowID, pACInfo->curr_state );
         bufferCommand = VOS_TRUE;
      }
   }
   if (bufferCommand)
   {
      //                              
      cmd.command = SME_QOS_MODIFY_REQ;
      cmd.pMac = pMac;
      cmd.sessionId = sessionId;
      cmd.u.modifyCmdInfo.QosFlowID = QosFlowID;
      cmd.u.modifyCmdInfo.QoSInfo = *pQoSInfo;
      hstatus = sme_QosBufferCmd(&cmd, buffered_cmd);
      if(!HAL_STATUS_SUCCESS(hstatus))
      {
         VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                   "%s: %d: couldn't buffer the modify request in state = %d",
                   __func__, __LINE__,
                   pACInfo->curr_state );
         //                             
         //                                             
         pSession->readyForPowerSave = VOS_TRUE;
         return SME_QOS_STATUS_MODIFY_SETUP_FAILURE_RSP;
      }
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                "%s: %d: Buffered modify request for flow = %d",
                __func__, __LINE__,
                QosFlowID);
      return SME_QOS_STATUS_MODIFY_SETUP_PENDING_RSP;
   }
   //                                                          
   switch(pACInfo->curr_state)
   {
   case SME_QOS_QOS_ON:
      //                                                                   
      //                                                                    
      //                          
      pNewEntry = (sme_QosFlowInfoEntry *) vos_mem_malloc(sizeof(*pNewEntry));
      if (!pNewEntry)
      {
         VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR,
                   "%s: %d: couldn't allocate memory for the new "
                   "entry in the Flow List",
                   __func__, __LINE__);
         //                              
         //                                             
         pSession->readyForPowerSave = VOS_TRUE;
         return SME_QOS_STATUS_MODIFY_SETUP_FAILURE_RSP;
      }
      pNewEntry->ac_type = ac;
      pNewEntry->sessionId = sessionId;
      pNewEntry->HDDcontext = flow_info->HDDcontext;
      pNewEntry->QoSCallback = flow_info->QoSCallback;
      pNewEntry->QosFlowID = flow_info->QosFlowID;
      pNewEntry->reason = SME_QOS_REASON_MODIFY_PENDING;
      //                                                                  
      //                                                                 
      pNewEntry->tspec_mask = flow_info->tspec_mask;
      pNewEntry->QoSInfo = *pQoSInfo;
      //                                                           
      flow_info->reason = SME_QOS_REASON_MODIFY;
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                "%s: %d: On session %d creating modified "
                "entry at %p with flowID %d",
                __func__, __LINE__,
                sessionId, pNewEntry, pNewEntry->QosFlowID);
      //                                                     
      csrLLInsertTail(&sme_QosCb.flow_list, &pNewEntry->link, VOS_TRUE);
      //                                   
      hstatus = sme_QosUpdateParams(sessionId,
                                    ac, pNewEntry->tspec_mask, 
                                    &Aggr_Tspec_Info);
      if(HAL_STATUS_SUCCESS(hstatus))
      {
         pACInfo->requested_QoSInfo[pNewEntry->tspec_mask -1] = Aggr_Tspec_Info;
         //                            
         status = sme_QosSetup(pMac, sessionId,
                               &pACInfo->requested_QoSInfo[pNewEntry->tspec_mask -1],
                               ac);
         VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                   "%s: %d: On session %d with AC %d in state SME_QOS_QOS_ON "
                   "sme_QosSetup returned with status %d",
                   __func__, __LINE__,
                   sessionId, ac, status);
         if(SME_QOS_STATUS_SETUP_REQ_PENDING_RSP != status)
         {
            //                                             
            //                          
            pSession->readyForPowerSave = VOS_TRUE;
         }
         if(SME_QOS_STATUS_SETUP_REQ_PENDING_RSP == status) 
         {
            new_state = SME_QOS_REQUESTED;
            status = SME_QOS_STATUS_MODIFY_SETUP_PENDING_RSP;
            pACInfo->tspec_pending = pNewEntry->tspec_mask;
         }
         else if((SME_QOS_STATUS_SETUP_SUCCESS_NO_ACM_NO_APSD_RSP == status) ||
                 (SME_QOS_STATUS_SETUP_SUCCESS_APSD_SET_ALREADY == status))
         {
            new_state = SME_QOS_QOS_ON;

            vos_mem_zero(&search_key, sizeof(sme_QosSearchInfo));
            //                                                         
            search_key.key.ac_type = ac;
            search_key.index = SME_QOS_SEARCH_KEY_INDEX_2;
            search_key.sessionId = sessionId;
            hstatus = sme_QosFindAllInFlowList(pMac, search_key,
                                               sme_QosModifyFnp);
            if(!HAL_STATUS_SUCCESS(hstatus))
            {
               status = SME_QOS_STATUS_MODIFY_SETUP_FAILURE_RSP;
            }
            if(SME_QOS_STATUS_MODIFY_SETUP_FAILURE_RSP != status)
            {
               pACInfo->curr_QoSInfo[pNewEntry->tspec_mask -1] = 
                  pACInfo->requested_QoSInfo[pNewEntry->tspec_mask -1];
               if(SME_QOS_STATUS_SETUP_SUCCESS_APSD_SET_ALREADY == status)
               {
                  status = SME_QOS_STATUS_MODIFY_SETUP_SUCCESS_APSD_SET_ALREADY;
                  vos_mem_zero(&search_key, sizeof(sme_QosSearchInfo));
                  search_key.key.ac_type = ac;
                  search_key.index = SME_QOS_SEARCH_KEY_INDEX_2;
                  search_key.sessionId = sessionId;
                  hstatus = sme_QosFindAllInFlowList(pMac, search_key, 
                                                     sme_QosModificationNotifyFnp);
                  if(!HAL_STATUS_SUCCESS(hstatus))
                  {
                     VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                               "%s: %d: couldn't notify other "
                               "entries on this AC =%d",
                               __func__, __LINE__, ac);
                  }
               }
               else if(SME_QOS_STATUS_SETUP_SUCCESS_NO_ACM_NO_APSD_RSP == status)
               {
                  status = SME_QOS_STATUS_MODIFY_SETUP_SUCCESS_NO_ACM_NO_APSD_RSP;
               }
            }
            if(buffered_cmd)
            {
               flow_info->QoSCallback(pMac, flow_info->HDDcontext, 
                                      &pACInfo->curr_QoSInfo[pNewEntry->tspec_mask -1],
                                      status,
                                      flow_info->QosFlowID);
            }
            
         }
         else
         {
            //                                             
            VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                      "%s: %d: On session %d unexpected status %d "
                      "returned by sme_QosSetup",
                      __func__, __LINE__,
                      sessionId, status);
            new_state = SME_QOS_QOS_ON;
         }
      }
      else
      {
         //       
         VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                   "%s: %d: sme_QosUpdateParams() failed",
                   __func__, __LINE__);
         //                              
         //                                             
         pSession->readyForPowerSave = VOS_TRUE;
         new_state = SME_QOS_LINK_UP;
      }
      /*                                                                   
                                                                            
                           
      */
      if(!(pACInfo->reassoc_pending && 
           (SME_QOS_HANDOFF == pACInfo->curr_state)))
      {
         sme_QosStateTransition(sessionId, ac, new_state);
      }
      else
      {
         pACInfo->prev_state = SME_QOS_REQUESTED;
      }
      break;
   case SME_QOS_HANDOFF:
   case SME_QOS_REQUESTED:
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_MED, 
                "%s: %d: Buffering modify request for flow %d in state = %d",
                __func__, __LINE__,
                QosFlowID, pACInfo->curr_state );
      //          
      cmd.command = SME_QOS_MODIFY_REQ;
      cmd.pMac = pMac;
      cmd.sessionId = sessionId;
      cmd.u.modifyCmdInfo.QosFlowID = QosFlowID;
      cmd.u.modifyCmdInfo.QoSInfo = *pQoSInfo;
      hstatus = sme_QosBufferCmd(&cmd, buffered_cmd);
      if(!HAL_STATUS_SUCCESS(hstatus))
      {
         VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                   "%s: %d: couldn't buffer the modify request in state = %d",
                   __func__, __LINE__,
                   pACInfo->curr_state );
         //                             
         //                                             
         pSession->readyForPowerSave = VOS_TRUE;
         return SME_QOS_STATUS_MODIFY_SETUP_FAILURE_RSP;
      }
      status = SME_QOS_STATUS_MODIFY_SETUP_PENDING_RSP;
      break;
   case SME_QOS_CLOSED:
   case SME_QOS_INIT:
   case SME_QOS_LINK_UP:
   default:
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                "%s: %d: modify requested in unexpected state = %d",
                __func__, __LINE__,
                pACInfo->curr_state);
      //                              
      //                                             
      pSession->readyForPowerSave = VOS_TRUE;
      break;
   }
   if((SME_QOS_STATUS_MODIFY_SETUP_SUCCESS_NO_ACM_NO_APSD_RSP == status) ||
      (SME_QOS_STATUS_MODIFY_SETUP_SUCCESS_APSD_SET_ALREADY == status)) 
   {
      (void)sme_QosProcessBufferedCmd(sessionId);
   }
   return status;
}
/*                                                                          
                                                                                
                                                       
  
                                                              
                                                                                 
                                                                            
                               
  
                                                      
  
     
  
                                                                            */
sme_QosStatusType sme_QosInternalReleaseReq(tpAniSirGlobal pMac, 
                                            v_U32_t QosFlowID,
                                            v_BOOL_t buffered_cmd)
{
   tListElem *pEntry= NULL;
   sme_QosSessionInfo *pSession;
   sme_QosACInfo *pACInfo;
   sme_QosFlowInfoEntry *flow_info = NULL;
   sme_QosFlowInfoEntry *pDeletedFlow = NULL;
   sme_QosEdcaAcType ac;
   sme_QosStates new_state = SME_QOS_CLOSED;
   sme_QosStatusType status = SME_QOS_STATUS_RELEASE_FAILURE_RSP;
   sme_QosWmmTspecInfo Aggr_Tspec_Info;
   sme_QosSearchInfo search_key;
   sme_QosCmdInfo  cmd;
   tCsrRoamModifyProfileFields modifyProfileFields;
   v_BOOL_t  deltsIssued = VOS_FALSE;
   v_U8_t sessionId;
   v_BOOL_t bufferCommand;
   eHalStatus hstatus;
   v_BOOL_t biDirectionalFlowsPresent = VOS_FALSE;
   v_BOOL_t uplinkFlowsPresent = VOS_FALSE;
   v_BOOL_t downlinkFlowsPresent = VOS_FALSE;
   tListElem *pResult= NULL;
   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
             "%s: %d: invoked for flow %d",
             __func__, __LINE__,
             QosFlowID);

   vos_mem_zero(&search_key, sizeof(sme_QosSearchInfo));
   //                                                          
   search_key.key.QosFlowID = QosFlowID;
   search_key.index = SME_QOS_SEARCH_KEY_INDEX_1;
   search_key.sessionId = SME_QOS_SEARCH_SESSION_ID_ANY;
   //                                                            
   pEntry = sme_QosFindInFlowList(search_key);
   
   if(!pEntry)
   {
      //       
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                "%s: %d: no match found for flowID = %d",
                __func__, __LINE__,
                QosFlowID);
      return SME_QOS_STATUS_RELEASE_INVALID_PARAMS_RSP;
   }
   //            
   flow_info = GET_BASE_ADDR( pEntry, sme_QosFlowInfoEntry, link );
   ac = flow_info->ac_type;
   sessionId = flow_info->sessionId;
   pSession = &sme_QosCb.sessionInfo[sessionId];
   pACInfo = &pSession->ac_info[ac];
   //                                                            
   pSession->readyForPowerSave = VOS_FALSE;
   //                                               
   bufferCommand = VOS_FALSE;
   //                                                
   //                                                                          
   //                                                                      
   //                                                                           
   //                                          
   if(sme_QosIsRspPending(sessionId, ac))
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_MED, 
                "%s: %d: buffering the release request for flow %d in state %d "
                "since another request is pending",
                __func__, __LINE__, 
                QosFlowID, pACInfo->curr_state );
      bufferCommand = VOS_TRUE;
   }
   else
   {
      //                                                    
      //                                
      hstatus = pmcRequestFullPower(pMac, sme_QosPmcFullPowerCallback,
                                    pSession, eSME_REASON_OTHER);
      if( eHAL_STATUS_PMC_PENDING == hstatus )
      {
         VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_MED, 
                   "%s: %d: buffering the release request for flow %d in state %d, "
                   "waiting for full power",
                   __func__, __LINE__, 
                   QosFlowID, pACInfo->curr_state );
         bufferCommand = VOS_TRUE;
      }
   }
   if (bufferCommand)
   {
      //                              
      cmd.command = SME_QOS_RELEASE_REQ;
      cmd.pMac = pMac;
      cmd.sessionId = sessionId;
      cmd.u.releaseCmdInfo.QosFlowID = QosFlowID;
      hstatus = sme_QosBufferCmd(&cmd, buffered_cmd);
      if(!HAL_STATUS_SUCCESS(hstatus))
      {
         VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                   "%s: %d: couldn't buffer the release request in state = %d",
                   __func__, __LINE__,
                   pACInfo->curr_state );
         //                             
         //                                             
         pSession->readyForPowerSave = VOS_TRUE;
         return SME_QOS_STATUS_RELEASE_FAILURE_RSP;
      }
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                "%s: %d: Buffered release request for flow = %d",
                __func__, __LINE__,
                QosFlowID);
      return SME_QOS_STATUS_RELEASE_REQ_PENDING_RSP;
   }
   //                                                          
   switch(pACInfo->curr_state)
   {
   case SME_QOS_QOS_ON:
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_MED, 
                "%s: %d: tspec_mask_status = %d for AC = %d with "
                "entry tspec_mask = %d",
                __func__, __LINE__, 
                pACInfo->tspec_mask_status, ac, flow_info->tspec_mask);

      //                                         
      if(pACInfo->num_flows[flow_info->tspec_mask - 1] > 1)
      {
         //                                                                 
         //            
         flow_info->reason = SME_QOS_REASON_RELEASE;

         /*                                                       
                                                       
                                   
                           
                               
                                                                                   
                                                                                       
                        
                                                                                      
                                                                                    
                                                                                   
                   */
         if(flow_info->QoSInfo.ts_info.direction == SME_QOS_WMM_TS_DIR_BOTH)
         {
           vos_mem_zero(&search_key, sizeof(sme_QosSearchInfo));
           //                                                          
           search_key.key.ac_type = ac;
           search_key.index = SME_QOS_SEARCH_KEY_INDEX_4;
           search_key.sessionId = sessionId;
           search_key.direction = SME_QOS_WMM_TS_DIR_BOTH;
           pResult = sme_QosFindInFlowList(search_key);
           if(pResult)
             biDirectionalFlowsPresent = VOS_TRUE;

           if(!biDirectionalFlowsPresent)
           {
             //                                                       

             //                            
             search_key.direction = SME_QOS_WMM_TS_DIR_UPLINK;
             pResult = sme_QosFindInFlowList(search_key);
             if(pResult)
               uplinkFlowsPresent = VOS_TRUE;

             //                              
             search_key.direction = SME_QOS_WMM_TS_DIR_DOWNLINK;
             pResult = sme_QosFindInFlowList(search_key);
             if(pResult)
               downlinkFlowsPresent = VOS_TRUE;

             if(uplinkFlowsPresent && downlinkFlowsPresent)
             {
               //                                                                                             

               vos_mem_zero(&search_key, sizeof(sme_QosSearchInfo));
               //                                                 
               search_key.key.ac_type = ac;
               search_key.index = SME_QOS_SEARCH_KEY_INDEX_4;
               search_key.sessionId = sessionId;
               search_key.direction = SME_QOS_WMM_TS_DIR_DOWNLINK;
               sme_QosUpdateTspecMask(sessionId, search_key, SME_QOS_TSPEC_MASK_BIT_2_SET);

               //                               
               hstatus = sme_QosUpdateParams(sessionId,
                                             ac, SME_QOS_TSPEC_MASK_BIT_2_SET,
                                             &Aggr_Tspec_Info);

               VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR,
                         "%s: %d: On session %d buffering the AddTS request "
                            "for AC %d in state %d as Addts is pending "
                         "on other Tspec index of this AC",
                         __func__, __LINE__,
                         sessionId, ac, pACInfo->curr_state);

               //                                                            
               //                                                                    
               //                            
               cmd.command = SME_QOS_RESEND_REQ;
               cmd.pMac = pMac;
               cmd.sessionId = sessionId;
               cmd.u.resendCmdInfo.ac = ac;
               cmd.u.resendCmdInfo.tspecMask = SME_QOS_TSPEC_MASK_BIT_2_SET;
               cmd.u.resendCmdInfo.QoSInfo = Aggr_Tspec_Info;
               pACInfo->requested_QoSInfo[SME_QOS_TSPEC_MASK_BIT_2_SET - 1] = Aggr_Tspec_Info;
               if(!HAL_STATUS_SUCCESS(sme_QosBufferCmd(&cmd, VOS_FALSE)))
               {
                  VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR,
                            "%s: %d: On session %d unable to buffer the AddTS "
                            "request for AC %d TSPEC %d in state %d",
                            __func__, __LINE__,
                            sessionId, ac, SME_QOS_TSPEC_MASK_BIT_2_SET, pACInfo->curr_state);

                  //                             
                  //                                             
                  pSession->readyForPowerSave = VOS_TRUE;

                  return SME_QOS_STATUS_MODIFY_SETUP_FAILURE_RSP;
               }
               pACInfo->tspec_mask_status = SME_QOS_TSPEC_MASK_BIT_1_2_SET;

             }
           }
         }

         /*                                          
                                                             
                                                               */
         hstatus = sme_QosUpdateParams(sessionId,
                                       ac, flow_info->tspec_mask,
                                       &Aggr_Tspec_Info);
         if(HAL_STATUS_SUCCESS(hstatus))
         {
            pACInfo->requested_QoSInfo[flow_info->tspec_mask - 1] = Aggr_Tspec_Info;
            //                            
            status = sme_QosSetup(pMac, sessionId,
                                  &pACInfo->requested_QoSInfo[flow_info->tspec_mask - 1], ac);
            VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                      "%s: %d: On session %d with AC %d in state SME_QOS_QOS_ON "
                      "sme_QosSetup returned with status %d",
                      __func__, __LINE__,
                      sessionId, ac, status);
            if(SME_QOS_STATUS_SETUP_REQ_PENDING_RSP != status)
            {
               //                                             
               //                          
               pSession->readyForPowerSave = VOS_TRUE;
            }
            if(SME_QOS_STATUS_SETUP_REQ_PENDING_RSP == status) 
            {
               new_state = SME_QOS_REQUESTED;
               status = SME_QOS_STATUS_RELEASE_REQ_PENDING_RSP;
               pACInfo->tspec_pending = flow_info->tspec_mask;
            }
            else if((SME_QOS_STATUS_SETUP_SUCCESS_NO_ACM_NO_APSD_RSP == status) ||
                    (SME_QOS_STATUS_SETUP_SUCCESS_APSD_SET_ALREADY == status))
            {
               new_state = SME_QOS_QOS_ON;
               pACInfo->num_flows[flow_info->tspec_mask - 1]--;
               pACInfo->curr_QoSInfo[flow_info->tspec_mask - 1] =
                  pACInfo->requested_QoSInfo[flow_info->tspec_mask - 1];
               //                               
               VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                         "%s: %d: Deleting entry at %p with flowID %d",
                         __func__, __LINE__,
                         flow_info, QosFlowID);
               csrLLRemoveEntry(&sme_QosCb.flow_list, pEntry, VOS_TRUE );
               pDeletedFlow = flow_info;
               if(SME_QOS_STATUS_SETUP_SUCCESS_APSD_SET_ALREADY == status)
               {
                  vos_mem_zero(&search_key, sizeof(sme_QosSearchInfo));
                  search_key.key.ac_type = ac;
                  search_key.index = SME_QOS_SEARCH_KEY_INDEX_2;
                  search_key.sessionId = sessionId;
                  hstatus = sme_QosFindAllInFlowList(pMac, search_key, 
                                                     sme_QosSetupFnp);
                  if(!HAL_STATUS_SUCCESS(hstatus))
                  {
                     VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                               "%s: %d: couldn't notify other "
                               "entries on this AC =%d",
                               __func__, __LINE__, ac);
                  }
               }
               status = SME_QOS_STATUS_RELEASE_SUCCESS_RSP;
               if(buffered_cmd)
               {
                  flow_info->QoSCallback(pMac, flow_info->HDDcontext, 
                                         &pACInfo->curr_QoSInfo[flow_info->tspec_mask - 1],
                                         status,
                                         flow_info->QosFlowID);
               }
            }
            else
            {
               //                                             
               VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                         "%s: %d: On session %d unexpected status %d "
                         "returned by sme_QosSetup",
                         __func__, __LINE__,
                         sessionId, status);
               new_state = SME_QOS_LINK_UP;
               pACInfo->num_flows[flow_info->tspec_mask - 1]--;
               pACInfo->curr_QoSInfo[flow_info->tspec_mask - 1] =
                  pACInfo->requested_QoSInfo[flow_info->tspec_mask - 1];
               //                               
               VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                         "%s: %d: On session %d deleting entry at "
                         "%p with flowID %d",
                         __func__, __LINE__,
                         sessionId, flow_info, QosFlowID);
               csrLLRemoveEntry(&sme_QosCb.flow_list, pEntry, VOS_TRUE );
               pDeletedFlow = flow_info;
               if(buffered_cmd)
               {
                  flow_info->QoSCallback(pMac, flow_info->HDDcontext, 
                                         &pACInfo->curr_QoSInfo[flow_info->tspec_mask - 1],
                                         status,
                                         flow_info->QosFlowID);
               }
            }
         }
         else
         {
            //       
            VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                      "%s: %d: sme_QosUpdateParams() failed",
                      __func__, __LINE__);
            //                              
            //                                             
            pSession->readyForPowerSave = VOS_TRUE;
            new_state = SME_QOS_LINK_UP;
            if(buffered_cmd)
            {
               flow_info->QoSCallback(pMac, flow_info->HDDcontext, 
                                      &pACInfo->curr_QoSInfo[flow_info->tspec_mask - 1],
                                      status,
                                      flow_info->QosFlowID);
            }
         }
      }
      else
      {
         //                                               
         status = SME_QOS_STATUS_RELEASE_SUCCESS_RSP;
#if defined(FEATURE_WLAN_CCX) && !defined(FEATURE_WLAN_CCX_UPLOAD)
         if (ac == SME_QOS_EDCA_AC_VO)
         {
            //                                                       
            //                          
            csrNeighborRoamIndicateVoiceBW( pMac, pACInfo->curr_QoSInfo[0].peak_data_rate, FALSE );
         }
#endif
         //                               
         if(CSR_IS_ADDTS_WHEN_ACMOFF_SUPPORTED(pMac) ||
            sme_QosIsACM(pMac, pSession->assocInfo.pBssDesc, ac, NULL))
         {
            //                                               
            if(SME_QOS_TSPEC_MASK_BIT_1_2_SET != pACInfo->tspec_mask_status)
            {
               //                                         
               //                                           
               pSession->apsdMask &= ~(1 << (SME_QOS_EDCA_AC_VO - ac));
               //                                                                 
               csrGetModifyProfileFields(pMac, flow_info->sessionId, &modifyProfileFields);
               modifyProfileFields.uapsd_mask = pSession->apsdMask; 
               csrSetModifyProfileFields(pMac, flow_info->sessionId, &modifyProfileFields);
               if(!pSession->apsdMask)
               {
                  //                                   
                  //                                     
                  if (!sme_QosIsUapsdActive())
                  {
                     //                                         
                     //                                      
                     (void)pmcStopUapsd(pMac);
                  }
               }
            }
            if (SME_QOS_RELEASE_DEFAULT == pACInfo->relTrig)
            {
               //          
               hstatus = qosIssueCommand(pMac, sessionId, eSmeCommandDelTs,
                                         NULL, ac, flow_info->tspec_mask);
               if(!HAL_STATUS_SUCCESS(hstatus))
               {
                  //       
                  VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR,
                            "%s: %d: sme_QosDelTsReq() failed",
                            __func__, __LINE__);
                  status = SME_QOS_STATUS_RELEASE_FAILURE_RSP;
                  //                                               
                  //                          
                  pSession->readyForPowerSave = VOS_TRUE;
               }
               else
               {
                  pACInfo->tspec_mask_status &= SME_QOS_TSPEC_MASK_BIT_1_2_SET &
                                                (~flow_info->tspec_mask);
                  deltsIssued = VOS_TRUE;
               }
            }
            else
            {
               pSession->readyForPowerSave = VOS_TRUE;
               pACInfo->tspec_mask_status &= SME_QOS_TSPEC_MASK_BIT_1_2_SET &
                                              (~flow_info->tspec_mask);
               deltsIssued = VOS_TRUE;
            }
         }
         else if(pSession->apsdMask & (1 << (SME_QOS_EDCA_AC_VO - ac)))
         {
            //             
            csrGetModifyProfileFields(pMac, sessionId, &modifyProfileFields);
            modifyProfileFields.uapsd_mask |= pSession->apsdMask;
            modifyProfileFields.uapsd_mask &= ~(1 << (SME_QOS_EDCA_AC_VO - ac));
            pSession->apsdMask &= ~(1 << (SME_QOS_EDCA_AC_VO - ac));
            if(!pSession->apsdMask)
            {
               //                                   
               //                                     
               if (!sme_QosIsUapsdActive())
               {
                  //                                         
                  //                                      
                  (void)pmcStopUapsd(pMac);
               }
            }
            hstatus = sme_QosRequestReassoc(pMac, sessionId,
                                            &modifyProfileFields, VOS_FALSE);
            if(!HAL_STATUS_SUCCESS(hstatus))
            {
               //       
               VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                         "%s: %d: Reassoc failed",
                         __func__, __LINE__);
               status = SME_QOS_STATUS_RELEASE_FAILURE_RSP;
               //                                               
               //                          
               pSession->readyForPowerSave = VOS_TRUE;
            }
            else
            {
               pACInfo->reassoc_pending = VOS_FALSE;//               
               pACInfo->prev_state = SME_QOS_LINK_UP;
               pACInfo->tspec_pending = 0;
            }
         }
         else
         {
            VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                      "%s: %d: nothing to do for AC = %d",
                      __func__, __LINE__, ac);
            //                                               
            //                          
            pSession->readyForPowerSave = VOS_TRUE;
         }

         if (SME_QOS_RELEASE_BY_AP == pACInfo->relTrig)
         {
            flow_info->QoSCallback(pMac, flow_info->HDDcontext,
                          &pACInfo->curr_QoSInfo[flow_info->tspec_mask - 1],
                          SME_QOS_STATUS_RELEASE_QOS_LOST_IND,
                          flow_info->QosFlowID);

            VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH,
                      "%s: %d: Deleting entry at %p with flowID %d",
                      __func__, __LINE__,
                      flow_info, flow_info->QosFlowID);
         }
         else if(buffered_cmd)
         {
            flow_info->QoSCallback(pMac, flow_info->HDDcontext,
                                   NULL,
                                   status,
                                   flow_info->QosFlowID);
         }

         if(SME_QOS_STATUS_RELEASE_FAILURE_RSP == status)
         {
            break;
         }

         if(((SME_QOS_TSPEC_MASK_BIT_1_2_SET & ~flow_info->tspec_mask) > 0) &&
            ((SME_QOS_TSPEC_MASK_BIT_1_2_SET & ~flow_info->tspec_mask) <= 
                SME_QOS_TSPEC_INDEX_MAX))
         {
            if(pACInfo->num_flows[(SME_QOS_TSPEC_MASK_BIT_1_2_SET & 
                                    ~flow_info->tspec_mask) - 1] > 0)
            {
               new_state = SME_QOS_QOS_ON;
            }
            else
            {
               new_state = SME_QOS_LINK_UP;
            }         
         }
         else
         {
            VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                      "%s: %d: Exceeded the array bounds of pACInfo->num_flows",
                      __func__, __LINE__);
            VOS_ASSERT (0);
            return SME_QOS_STATUS_RELEASE_INVALID_PARAMS_RSP;
         }

         if(VOS_FALSE == deltsIssued)
         {
            vos_mem_zero(&pACInfo->curr_QoSInfo[flow_info->tspec_mask - 1], 
                      sizeof(sme_QosWmmTspecInfo));
         }
         vos_mem_zero(&pACInfo->requested_QoSInfo[flow_info->tspec_mask - 1], 
                      sizeof(sme_QosWmmTspecInfo));
         pACInfo->num_flows[flow_info->tspec_mask - 1]--;
         //                               
         VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                   "%s: %d: On session %d deleting entry at %p with flowID %d",
                   __func__, __LINE__,
                   sessionId, flow_info, QosFlowID);
         csrLLRemoveEntry(&sme_QosCb.flow_list, pEntry, VOS_TRUE );
         pDeletedFlow = flow_info;
         pACInfo->relTrig = SME_QOS_RELEASE_DEFAULT;
      }
      /*                                                                   
                                                                            
                           
      */
      if(SME_QOS_HANDOFF != pACInfo->curr_state)
      {
         sme_QosStateTransition(sessionId, ac, new_state);
      }
      if(pACInfo->reassoc_pending)
      {
         pACInfo->prev_state = SME_QOS_REQUESTED;
      }
      break;
   case SME_QOS_HANDOFF:
   case SME_QOS_REQUESTED:
      //          
      cmd.command = SME_QOS_RELEASE_REQ;
      cmd.pMac = pMac;
      cmd.sessionId = sessionId;
      cmd.u.releaseCmdInfo.QosFlowID = QosFlowID;
      hstatus = sme_QosBufferCmd(&cmd, buffered_cmd);
      if(!HAL_STATUS_SUCCESS(hstatus))
      {
         VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                   "%s: %d: couldn't buffer the release request in state = %d",
                   __func__, __LINE__,
                   pACInfo->curr_state );
         //                              
         //                                             
         pSession->readyForPowerSave = VOS_TRUE;
         return SME_QOS_STATUS_RELEASE_FAILURE_RSP;
      }
      status = SME_QOS_STATUS_RELEASE_REQ_PENDING_RSP;
      break;
   case SME_QOS_CLOSED:
   case SME_QOS_INIT:
   case SME_QOS_LINK_UP:
   default:
      //               
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                "%s: %d: release request in unexpected state = %d",
                __func__, __LINE__,
                pACInfo->curr_state );
      //      
      VOS_ASSERT(0);
      //                              
      //                                             
      pSession->readyForPowerSave = VOS_TRUE;
      break;
   }
   //                                         
   if (pDeletedFlow)
   {
      vos_mem_free(pDeletedFlow);
   }
   if((SME_QOS_STATUS_RELEASE_SUCCESS_RSP == status)) 
   {
      (void)sme_QosProcessBufferedCmd(sessionId);
   }
   return status;
}

/*                                                                          
                                                                        
                                                                                
                                                              
                                                                 
                                                                
                                                                             
                                                          
                                                                
  
                                                                     
                                                                               
       
                                                             
                                                                      
                                                                              
                                                             
                                                             
                                                               
                                                               
                                                             
                                                             
                                                                      
                                                                      
                                                             
                                                             
                                                               
                                                                             
                                                             
                                                             
  
     
  
                                                                            */
sme_QosStatusType sme_QosSetup(tpAniSirGlobal pMac,
                               v_U8_t sessionId,
                               sme_QosWmmTspecInfo *pTspec_Info, 
                               sme_QosEdcaAcType ac)
{
   sme_QosSessionInfo *pSession;
   sme_QosACInfo *pACInfo;
   sme_QosStatusType status = SME_QOS_STATUS_SETUP_FAILURE_RSP;
   tDot11fBeaconIEs *pIes = NULL;
   tCsrRoamModifyProfileFields modifyProfileFields;
   eHalStatus hstatus;
   if( !CSR_IS_SESSION_VALID( pMac, sessionId ) )
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                "%s: %d: Session Id %d is invalid",
                __func__, __LINE__,
                sessionId);
      return status;
   }
   pSession = &sme_QosCb.sessionInfo[sessionId];
   if( !pSession->sessionActive )
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                "%s: %d: Session %d is inactive",
                __func__, __LINE__,
                sessionId);
      return status;
   }
   if(!pSession->assocInfo.pBssDesc)
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                "%s: %d: Session %d has an Invalid BSS Descriptor",
                __func__, __LINE__,
                sessionId);
      return status;
   }
   hstatus = csrGetParsedBssDescriptionIEs(pMac,
                                           pSession->assocInfo.pBssDesc,
                                           &pIes);
   if(!HAL_STATUS_SUCCESS(hstatus))
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR,
                "%s: %d: On session %d unable to parse BSS IEs",
                __func__, __LINE__,
                sessionId);
      return status;
   }

   /*                               */

   if( !CSR_IS_QOS_BSS(pIes) )
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                "%s: %d: On session %d AP doesn't support QoS",
                __func__, __LINE__,
                sessionId);
      vos_mem_free(pIes);
      //                                             
      return SME_QOS_STATUS_SETUP_NOT_QOS_AP_RSP;
   }

   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_DEBUG,
             "%s: UAPSD/PSB set %d: ", __func__, __LINE__,
             pTspec_Info->ts_info.psb);

   pACInfo = &pSession->ac_info[ac];
   do
   {
      //                            
      if(CSR_IS_ADDTS_WHEN_ACMOFF_SUPPORTED(pMac) ||
         sme_QosIsACM(pMac, pSession->assocInfo.pBssDesc, ac, NULL))
      {
         //                                                    
         if(pTspec_Info->ts_info.psb && 
            (!pMac->pmc.uapsdEnabled ))
         {
            VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                      "%s: %d: Request is looking for APSD but PMC doesn't "
                      "have support for APSD",
                      __func__, __LINE__);
            break;
         }

         if (pTspec_Info->ts_info.psb &&
             !(pIes->WMMParams.qosInfo & SME_QOS_AP_SUPPORTS_APSD) &&
             !(pIes->WMMInfoAp.uapsd))
         {
            //                                                          
            VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR,
                      "%s: %d: On session %d AP doesn't support APSD",
                      __func__, __LINE__,
                      sessionId);
            break;
         }

         if(SME_QOS_MAX_TID == pTspec_Info->ts_info.tid)
         {
            //                                
            pTspec_Info->ts_info.tid =
               (v_U8_t)(SME_QOS_WMM_UP_NC - pTspec_Info->ts_info.up);
         }
         //           
         hstatus = qosIssueCommand(pMac, sessionId, eSmeCommandAddTs,
                                   pTspec_Info, ac, 0);
         if(!HAL_STATUS_SUCCESS(hstatus))
         {
            VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                      "%s: %d: sme_QosAddTsReq() failed",
                      __func__, __LINE__);
            break;
         }
         VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH,
                   "%s: %d: On session %d AddTS on AC %d is pending",
                   __func__, __LINE__,
                   sessionId, ac);
         status = SME_QOS_STATUS_SETUP_REQ_PENDING_RSP;
         break;
      }
      //                               
      //                                     
      if(0 == pTspec_Info->ts_info.psb)
      {
         //                      
         //                                                                    
         //                                                                    
         //                                                                   
         //                
         if(pSession->apsdMask & (1 << (SME_QOS_EDCA_AC_VO - ac)))
         {
            //                                                               
            //                       
            VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                      "%s: %d: On session %d reassoc needed "
                      "to disable APSD on AC %d",
                      __func__, __LINE__,
                      sessionId, ac);
            csrGetModifyProfileFields(pMac, sessionId, &modifyProfileFields);
            modifyProfileFields.uapsd_mask |= pSession->apsdMask;
            modifyProfileFields.uapsd_mask &= ~(1 << (SME_QOS_EDCA_AC_VO - ac));
            hstatus = sme_QosRequestReassoc(pMac, sessionId,
                                            &modifyProfileFields, VOS_FALSE);
            if(!HAL_STATUS_SUCCESS(hstatus))
            {
               //       
               VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                         "%s: %d: Unable to request reassociation",
                         __func__, __LINE__);
               break;
            }
            else
            {
               VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH,
                         "%s: %d: On session %d reassociation to enable "
                         "APSD on AC %d is pending",
                         __func__, __LINE__,
                         sessionId, ac);
               status = SME_QOS_STATUS_SETUP_REQ_PENDING_RSP;
               pACInfo->reassoc_pending = VOS_TRUE;
            }
         }
         else
         {
            //                              
            //                                            
            VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                      "%s: %d: Request is not looking for APSD & Admission "
                      "Control isn't mandatory for the AC",
                      __func__, __LINE__);
            //                         
            status = SME_QOS_STATUS_SETUP_SUCCESS_NO_ACM_NO_APSD_RSP;
         }
         break;
      }
      else if(!(pIes->WMMParams.qosInfo & SME_QOS_AP_SUPPORTS_APSD) &&
              !(pIes->WMMInfoAp.uapsd))
      {
         //                                                          
         VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                   "%s: %d: On session %d AP doesn't support APSD",
                   __func__, __LINE__,
                   sessionId);
         break;
      }
      else if(pSession->apsdMask & (1 << (SME_QOS_EDCA_AC_VO - ac)))
      {
         //                                
         //                                     
         status = SME_QOS_STATUS_SETUP_SUCCESS_APSD_SET_ALREADY;
         VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                   "%s: %d: Request is looking for APSD and it is already "
                   "set for the AC",
                   __func__, __LINE__);
         break;
      }
      else
      {
         //                                
         //                                 
         //                          
         if(pMac->pmc.uapsdEnabled)
         {
            VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                      "%s: %d: On session %d reassoc needed "
                      "to enable APSD on AC %d",
                      __func__, __LINE__,
                      sessionId, ac);
            //             
            //                                          
            //                              
            csrGetModifyProfileFields(pMac, sessionId, &modifyProfileFields);
            modifyProfileFields.uapsd_mask |= pSession->apsdMask;
            modifyProfileFields.uapsd_mask |= 1 << (SME_QOS_EDCA_AC_VO - ac);
            hstatus = sme_QosRequestReassoc(pMac, sessionId,
                                            &modifyProfileFields, VOS_FALSE);
            if(!HAL_STATUS_SUCCESS(hstatus))
            {
               //       
               VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                         "%s: %d: Unable to request reassociation",
                         __func__, __LINE__);
               break;
            }
            else
            {
               VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH,
                         "%s: %d: On session %d reassociation to enable "
                         "APSD on AC %d is pending",
                         __func__, __LINE__,
                         sessionId, ac);
               status = SME_QOS_STATUS_SETUP_REQ_PENDING_RSP;
               pACInfo->reassoc_pending = VOS_TRUE;
            }
         }
         else
         {
            //                                     
            VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                      "%s: %d: no support for APSD or BMPS from PMC",
                      __func__, __LINE__);
         }
      }
   }while(0);

   vos_mem_free(pIes);
   return status;
}

#if defined(FEATURE_WLAN_CCX) || defined(FEATURE_WLAN_LFR)
/*                                                                        
                                                                           
                                                                         
                                                                          
                                                             
 */
eHalStatus sme_QosProcessSetKeySuccessInd(tpAniSirGlobal pMac, v_U8_t sessionId, void * pEvent_info)
{
    VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_WARN, 
            "########### Set Key Complete #############");
    (void)sme_QosProcessBufferedCmd(sessionId);
    return eHAL_STATUS_SUCCESS;
}
#endif

#ifdef FEATURE_WLAN_CCX
/*                                                                          
                                                                      
                                                                           
  
                                                              
                                    
                                                              
                                                                  
                                    
  
                                                      
                                                                            */
eHalStatus sme_QosCCXSaveTspecResponse(tpAniSirGlobal pMac, v_U8_t sessionId, tDot11fIEWMMTSPEC *pTspec, v_U8_t ac, v_U8_t tspecIndex)
{
    tpSirAddtsRsp pAddtsRsp = &sme_QosCb.sessionInfo[sessionId].ac_info[ac].addTsRsp[tspecIndex];

    ac = sme_QosUPtoACMap[pTspec->user_priority];

    vos_mem_zero(pAddtsRsp, sizeof(tSirAddtsRsp));

    pAddtsRsp->messageType = eWNI_SME_ADDTS_RSP;
    pAddtsRsp->length = sizeof(tSirAddtsRsp);
    pAddtsRsp->rc = eSIR_SUCCESS;
    pAddtsRsp->sessionId = sessionId;
    pAddtsRsp->rsp.dialogToken = 0;
    pAddtsRsp->rsp.status = eSIR_SUCCESS;
    pAddtsRsp->rsp.wmeTspecPresent = pTspec->present;
    VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO, 
            "%s: Copy Tspec to local data structure ac=%d, tspecIdx=%d", 
            __func__, ac, tspecIndex);

    if (pAddtsRsp->rsp.wmeTspecPresent)
    {
        //                                                              
        ConvertWMMTSPEC(pMac, &pAddtsRsp->rsp.tspec, pTspec);
    }

    return eHAL_STATUS_SUCCESS;
}

/*                                                                          
                                                                         
                                                                            
                                                                              
                                                                            
                                                                           
                           
  
                                                              
                                    
                                                         
  
                                                      
                                                                            */
eHalStatus sme_QosCCXProcessReassocTspecRsp(tpAniSirGlobal pMac, v_U8_t sessionId, void* pEvent_info)
{
    sme_QosSessionInfo *pSession;
    sme_QosACInfo *pACInfo;
    tDot11fIEWMMTSPEC *pTspecIE = NULL;
    tCsrRoamSession *pCsrSession = CSR_GET_SESSION( pMac, sessionId );
    tCsrRoamConnectedInfo *pCsrConnectedInfo = &pCsrSession->connectedInfo;
    eHalStatus status = eHAL_STATUS_FAILURE;
    v_U8_t ac, numTspec, cnt;
    v_U8_t tspec_flow_index, tspec_mask_status;
    v_U32_t tspecIeLen;

    pSession = &sme_QosCb.sessionInfo[sessionId];

    //                                                              
    //                          
    pTspecIE = (tDot11fIEWMMTSPEC *)(pCsrConnectedInfo->pbFrames + pCsrConnectedInfo->nBeaconLength +
        pCsrConnectedInfo->nAssocReqLength + pCsrConnectedInfo->nAssocRspLength + pCsrConnectedInfo->nRICRspLength);

    //                                                          
    //                                             
    tspecIeLen = pCsrConnectedInfo->nTspecIeLength;
    if (tspecIeLen < sizeof(tDot11fIEWMMTSPEC)) {
        VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR,
                FL("CCX Tspec IE len %d less than min %d"),
                tspecIeLen, sizeof(tDot11fIEWMMTSPEC));
        return eHAL_STATUS_FAILURE;
    }

    VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_WARN,
             "TspecLen = %d, pbFrames = %p, pTspecIE = %p",
             tspecIeLen, pCsrConnectedInfo->pbFrames, pTspecIE);

    numTspec = (tspecIeLen)/sizeof(tDot11fIEWMMTSPEC);
    for(cnt=0; cnt<numTspec; cnt++) {
        ac = sme_QosUpToAc(pTspecIE->user_priority);
        pACInfo = &pSession->ac_info[ac];
        tspec_mask_status = pACInfo->tspec_mask_status;
        VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_WARN,
                FL("UP=%d, ac=%d, tspec_mask_status=%x"),
                pTspecIE->user_priority, ac,  tspec_mask_status );

            for (tspec_flow_index = 0; tspec_flow_index < SME_QOS_TSPEC_INDEX_MAX; tspec_flow_index++) {
                if (tspec_mask_status & (1 << tspec_flow_index)) {
                VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_WARN, 
                FL("Found Tspec entry flow = %d AC = %d"),tspec_flow_index, ac);
                    sme_QosCCXSaveTspecResponse(pMac, sessionId, pTspecIE, ac, tspec_flow_index);
                } else {
                VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_WARN, 
                FL("Not found Tspec entry flow = %d AC = %d"),tspec_flow_index, ac);
                }
            }
        //                                                       
        pTspecIE++;
    }

    /*                                        */
    status = sme_QosFTAggrQosReq(pMac,sessionId);

    return status;
}

/*                                                                          
                                                                          
                                                                           
  
                                                              
                                       
                                       
  
               
                                                                            */
static void sme_QosCopyTspecInfo(tpAniSirGlobal pMac, sme_QosWmmTspecInfo *pTspec_Info, tSirMacTspecIE* pTspec)
{
    /*                                                                       
                                                                            
                                                                        */
    pTspec->delayBound        = pTspec_Info->delay_bound;
    pTspec->inactInterval     = pTspec_Info->inactivity_interval;
    pTspec->length            = SME_QOS_TSPEC_IE_LENGTH;
    pTspec->maxBurstSz        = pTspec_Info->max_burst_size;
    pTspec->maxMsduSz         = pTspec_Info->maximum_msdu_size;
    pTspec->maxSvcInterval    = pTspec_Info->max_service_interval;
    pTspec->meanDataRate      = pTspec_Info->mean_data_rate;
    pTspec->mediumTime        = pTspec_Info->medium_time;
    pTspec->minDataRate       = pTspec_Info->min_data_rate;
    pTspec->minPhyRate        = pTspec_Info->min_phy_rate;
    pTspec->minSvcInterval    = pTspec_Info->min_service_interval;
    pTspec->nomMsduSz         = pTspec_Info->nominal_msdu_size;
    pTspec->peakDataRate      = pTspec_Info->peak_data_rate;
    pTspec->surplusBw         = pTspec_Info->surplus_bw_allowance;
    pTspec->suspendInterval   = pTspec_Info->suspension_interval;
    pTspec->svcStartTime      = pTspec_Info->svc_start_time;
    pTspec->tsinfo.traffic.direction = pTspec_Info->ts_info.direction;

    //                                                                              
    if (pTspec_Info->ts_info.psb && btcIsReadyForUapsd(pMac)) {
        pTspec->tsinfo.traffic.psb = pTspec_Info->ts_info.psb;
    } else {
        pTspec->tsinfo.traffic.psb = 0;
        pTspec_Info->ts_info.psb = 0;
    }
    pTspec->tsinfo.traffic.tsid           = pTspec_Info->ts_info.tid;
    pTspec->tsinfo.traffic.userPrio       = pTspec_Info->ts_info.up;
    pTspec->tsinfo.traffic.accessPolicy   = SME_QOS_ACCESS_POLICY_EDCA;
    pTspec->tsinfo.traffic.burstSizeDefn  = pTspec_Info->ts_info.burst_size_defn;
    pTspec->tsinfo.traffic.ackPolicy      = pTspec_Info->ts_info.ack_policy;
    pTspec->type                          = SME_QOS_TSPEC_IE_TYPE;

    VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH,
            "%s: %d: up = %d, tid = %d",
            __func__, __LINE__,
            pTspec_Info->ts_info.up,
            pTspec_Info->ts_info.tid);
}

/*                                                                          
                                                                       
                                                                                   
                                                                        
                                
  
                                                              
                                    
                                                                                 
  
                                                       
                                                                            */
v_U8_t sme_QosCCxRetrieveTspecInfo(tpAniSirGlobal pMac, v_U8_t sessionId, tTspecInfo *pTspecInfo)
{
    sme_QosSessionInfo *pSession;
    sme_QosACInfo *pACInfo;
    v_U8_t tspec_mask_status = 0;
    v_U8_t tspec_pending_status = 0;
    v_U8_t ac, numTspecs = 0;
    tTspecInfo *pDstTspec = pTspecInfo;

    //                                                                

    pSession = &sme_QosCb.sessionInfo[sessionId];    

    for(ac = SME_QOS_EDCA_AC_BE; ac < SME_QOS_EDCA_AC_MAX; ac++) {
        volatile v_U8_t tspec_index = 0;

        pACInfo = &pSession->ac_info[ac];
        tspec_pending_status = pACInfo->tspec_pending;
        tspec_mask_status = pACInfo->tspec_mask_status;

        do {
            if (tspec_mask_status & SME_QOS_TSPEC_MASK_BIT_1_SET) {
                /*                                                                                            
                                       */
                if (tspec_pending_status & SME_QOS_TSPEC_MASK_BIT_1_SET) {
                    sme_QosCopyTspecInfo(pMac, &pACInfo->requested_QoSInfo[tspec_index], &pDstTspec->tspec);
                } else {
                    sme_QosCopyTspecInfo(pMac, &pACInfo->curr_QoSInfo[tspec_index], &pDstTspec->tspec);
                }
                pDstTspec->valid = TRUE;
                numTspecs++;
                pDstTspec++;
            }
            tspec_mask_status >>= 1;
            tspec_pending_status >>= 1;
            tspec_index++;
        } while (tspec_mask_status);
    }

    return numTspecs;
}

#endif

#ifdef WLAN_FEATURE_VOWIFI_11R

eHalStatus sme_QosCreateTspecRICIE(tpAniSirGlobal pMac, sme_QosWmmTspecInfo *pTspec_Info,
                                                       v_U8_t *pRICBuffer, v_U32_t *pRICLength, v_U8_t *pRICIdentifier)
{
    tDot11fIERICDataDesc    ricIE;
    tANI_U32                nStatus;

    VOS_ASSERT(NULL != pRICBuffer);
    VOS_ASSERT(NULL != pRICLength);
    VOS_ASSERT(NULL != pRICIdentifier);

    if (pRICBuffer == NULL || pRICIdentifier == NULL || pRICLength == NULL)
        return eHAL_STATUS_FAILURE;

    vos_mem_zero(&ricIE, sizeof(tDot11fIERICDataDesc));

    ricIE.present = 1;
    ricIE.RICData.present = 1;
    ricIE.RICData.resourceDescCount = 1;
    ricIE.RICData.statusCode = 0;
    ricIE.RICData.Identifier = sme_QosAssignDialogToken();
#ifndef USE_80211_WMMTSPEC_FOR_RIC
    ricIE.TSPEC.present = 1;
    ricIE.TSPEC.delay_bound = pTspec_Info->delay_bound;
    ricIE.TSPEC.inactivity_int = pTspec_Info->inactivity_interval;
    ricIE.TSPEC.burst_size = pTspec_Info->max_burst_size;
    ricIE.TSPEC.max_msdu_size = pTspec_Info->maximum_msdu_size;
    ricIE.TSPEC.max_service_int = pTspec_Info->max_service_interval;
    ricIE.TSPEC.mean_data_rate = pTspec_Info->mean_data_rate;
    ricIE.TSPEC.medium_time = 0;
    ricIE.TSPEC.min_data_rate = pTspec_Info->min_data_rate;
    ricIE.TSPEC.min_phy_rate = pTspec_Info->min_phy_rate;
    ricIE.TSPEC.min_service_int = pTspec_Info->min_service_interval;
    ricIE.TSPEC.size = pTspec_Info->nominal_msdu_size;
    ricIE.TSPEC.peak_data_rate = pTspec_Info->peak_data_rate;
    ricIE.TSPEC.surplus_bw_allowance = pTspec_Info->surplus_bw_allowance;
    ricIE.TSPEC.suspension_int = pTspec_Info->suspension_interval;
    ricIE.TSPEC.service_start_time = pTspec_Info->svc_start_time;
    ricIE.TSPEC.direction = pTspec_Info->ts_info.direction;
    //                                                                              
    if( pTspec_Info->ts_info.psb && btcIsReadyForUapsd(pMac) )
    {
       ricIE.TSPEC.psb = pTspec_Info->ts_info.psb;
    }
    else
    {
       ricIE.TSPEC.psb = 0;
    }
    ricIE.TSPEC.tsid = pTspec_Info->ts_info.tid;
    ricIE.TSPEC.user_priority = pTspec_Info->ts_info.up;
    ricIE.TSPEC.access_policy = SME_QOS_ACCESS_POLICY_EDCA;

    *pRICIdentifier = ricIE.RICData.Identifier;
    
    nStatus = dot11fPackIeRICDataDesc(pMac, &ricIE, pRICBuffer, sizeof(ricIE), pRICLength);
    if (DOT11F_FAILED(nStatus))
    {
        VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                FL("Packing of RIC Data of length %d failed with status %d"), 
                                        *pRICLength, nStatus);
    }
#else //          
    /*                                                                       
                                                                            
                                                                       */
    ricIE.WMMTSPEC.present = 1;
    ricIE.WMMTSPEC.version = 1;
    ricIE.WMMTSPEC.delay_bound = pTspec_Info->delay_bound;
    ricIE.WMMTSPEC.inactivity_int = pTspec_Info->inactivity_interval;
    ricIE.WMMTSPEC.burst_size = pTspec_Info->max_burst_size;
    ricIE.WMMTSPEC.max_msdu_size = pTspec_Info->maximum_msdu_size;
    ricIE.WMMTSPEC.max_service_int = pTspec_Info->max_service_interval;
    ricIE.WMMTSPEC.mean_data_rate = pTspec_Info->mean_data_rate;
    ricIE.WMMTSPEC.medium_time = 0;
    ricIE.WMMTSPEC.min_data_rate = pTspec_Info->min_data_rate;
    ricIE.WMMTSPEC.min_phy_rate = pTspec_Info->min_phy_rate;
    ricIE.WMMTSPEC.min_service_int = pTspec_Info->min_service_interval;
    ricIE.WMMTSPEC.size = pTspec_Info->nominal_msdu_size;
    ricIE.WMMTSPEC.peak_data_rate = pTspec_Info->peak_data_rate;
    ricIE.WMMTSPEC.surplus_bw_allowance = pTspec_Info->surplus_bw_allowance;
    ricIE.WMMTSPEC.suspension_int = pTspec_Info->suspension_interval;
    ricIE.WMMTSPEC.service_start_time = pTspec_Info->svc_start_time;
    ricIE.WMMTSPEC.direction = pTspec_Info->ts_info.direction;
    //                                                                              
    if( pTspec_Info->ts_info.psb && btcIsReadyForUapsd(pMac) )
    {
       ricIE.WMMTSPEC.psb = pTspec_Info->ts_info.psb;
    }
    else
    {
       ricIE.WMMTSPEC.psb = 0;
    }
    ricIE.WMMTSPEC.tsid = pTspec_Info->ts_info.tid;
    ricIE.WMMTSPEC.user_priority = pTspec_Info->ts_info.up;
    ricIE.WMMTSPEC.access_policy = SME_QOS_ACCESS_POLICY_EDCA;

    
    nStatus = dot11fPackIeRICDataDesc(pMac, &ricIE, pRICBuffer, sizeof(ricIE), pRICLength);
    if (DOT11F_FAILED(nStatus))
    {
        VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                FL("Packing of RIC Data of length %d failed with status %d"), 
                                        *pRICLength, nStatus);
    }
#endif /*             */
    *pRICIdentifier = ricIE.RICData.Identifier;
    return nStatus;
}

eHalStatus sme_QosProcessFTReassocReqEv(tpAniSirGlobal pMac, v_U8_t sessionId, void * pEvent_info)
{
    sme_QosSessionInfo *pSession;
    sme_QosACInfo *pACInfo;
    v_U8_t ac, qos_requested = FALSE;
    v_U8_t tspec_flow_index;
    sme_QosFlowInfoEntry *flow_info = NULL;
    tListElem *pEntry= NULL;

    VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH,
            FL("Invoked on session %d"), sessionId);

    pSession = &sme_QosCb.sessionInfo[sessionId];

    for(ac = SME_QOS_EDCA_AC_BE; ac < SME_QOS_EDCA_AC_MAX; ac++)
    {
        pACInfo = &pSession->ac_info[ac];
        qos_requested = FALSE;

        for (tspec_flow_index = 0; tspec_flow_index < SME_QOS_TSPEC_INDEX_MAX; tspec_flow_index++)
        {
            /*                                                                           */
            if ((pACInfo->ricIdentifier[tspec_flow_index] && !pACInfo->tspec_pending) ||
                    (pACInfo->tspec_mask_status & (1<<tspec_flow_index)))
            {
                VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO, 
                        FL("Copying the currentQos to requestedQos for AC=%d, flow=%d"),
                        ac, tspec_flow_index );

                pACInfo->requested_QoSInfo[tspec_flow_index] = pACInfo->curr_QoSInfo[tspec_flow_index];
                vos_mem_zero(&pACInfo->curr_QoSInfo[tspec_flow_index], sizeof(sme_QosWmmTspecInfo));
                qos_requested = TRUE;
            }
        }

        //                                                        
        //                              
        if (qos_requested) 
        {
            switch(pACInfo->curr_state)
            {
                case SME_QOS_HANDOFF:
                    sme_QosStateTransition(sessionId, ac, SME_QOS_REQUESTED);
                    break;
                default:
                    VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                            FL("FT Reassoc req event in unexpected state %d"), pACInfo->curr_state);
                    VOS_ASSERT(0);
            }
        }

    }

    /*                                                                          
                                                        */
    pSession->apsdMask = 0;
    pSession->uapsdAlreadyRequested = 0;
    pSession->readyForPowerSave = 0;

    /*                                                                       */
    pEntry = csrLLPeekHead( &sme_QosCb.flow_list, VOS_FALSE );
    if(!pEntry)
    {
        VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_WARN,
                "%s: %d: Flow List empty, nothing to update",
                __func__, __LINE__);
        return eHAL_STATUS_FAILURE;
    }

    do
    {
        flow_info = GET_BASE_ADDR( pEntry, sme_QosFlowInfoEntry, link );
        if(sessionId == flow_info->sessionId)
        {
            VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH,
                    "%s: %d: Changing FlowID %d reason to SETUP and HO renewal to FALSE",
                    __func__, __LINE__,
                    flow_info->QosFlowID);
            flow_info->reason = SME_QOS_REASON_SETUP;
            flow_info->hoRenewal = eANI_BOOLEAN_TRUE;
        }
        pEntry = csrLLNext( &sme_QosCb.flow_list, pEntry, VOS_FALSE );
    } while( pEntry );

    return eHAL_STATUS_SUCCESS;
}


eHalStatus sme_QosFTAggrQosReq( tpAniSirGlobal pMac, v_U8_t sessionId )
{
    tSirAggrQosReq *pMsg = NULL;
    sme_QosSessionInfo *pSession;
    eHalStatus status = eHAL_STATUS_FAILURE;
    int i, j = 0;
    VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH,
            "%s: %d: invoked on session %d", __func__, __LINE__,
            sessionId);

    pSession = &sme_QosCb.sessionInfo[sessionId];

    pMsg = (tSirAggrQosReq *)vos_mem_malloc(sizeof(tSirAggrQosReq));

    if (!pMsg)
    {
        //       
        VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR,
                "%s: %d: couldn't allocate memory for the msg buffer",
                __func__, __LINE__);

        return eHAL_STATUS_FAILURE;
      }

    vos_mem_zero(pMsg, sizeof(tSirAggrQosReq));

    pMsg->messageType = pal_cpu_to_be16((v_U16_t)eWNI_SME_FT_AGGR_QOS_REQ);
    pMsg->length = sizeof(tSirAggrQosReq);
    pMsg->sessionId = sessionId;
    pMsg->timeout = 0;
    pMsg->rspReqd = VOS_TRUE;
    vos_mem_copy( &pMsg->bssId[ 0 ],
            &pSession->assocInfo.pBssDesc->bssId[ 0 ],
            sizeof(tCsrBssid) );

    for( i = 0; i < SME_QOS_EDCA_AC_MAX; i++ )
    {
        for( j = 0; j < SME_QOS_TSPEC_INDEX_MAX; j++ )
        {
            VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO,
                    FL("ac=%d, tspec_mask_staus=%x, tspec_index=%d"),
                    i, pSession->ac_info[i].tspec_mask_status, j);
            VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO, 
                    FL("direction = %d"), pSession->ac_info[i].addTsRsp[j].rsp.tspec.tsinfo.traffic.direction);
            //                                       
            if ((pSession->ac_info[i].tspec_mask_status) & (1 << j))
            {
                tANI_U8 direction = pSession->ac_info[i].addTsRsp[j].rsp.tspec.tsinfo.traffic.direction;
                if ((direction == SME_QOS_WMM_TS_DIR_UPLINK) ||
                        (direction == SME_QOS_WMM_TS_DIR_BOTH))
                {
                    VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_WARN, 
                            FL("Found tspec entry AC=%d, flow=%d, direction = %d"), i, j, direction);
                    pMsg->aggrInfo.aggrAddTsInfo[i].dialogToken =
                        sme_QosAssignDialogToken();
                    pMsg->aggrInfo.aggrAddTsInfo[i].lleTspecPresent =
                        pSession->ac_info[i].addTsRsp[j].rsp.lleTspecPresent;
                    pMsg->aggrInfo.aggrAddTsInfo[i].numTclas =
                        pSession->ac_info[i].addTsRsp[j].rsp.numTclas;
                    vos_mem_copy( pMsg->aggrInfo.aggrAddTsInfo[i].tclasInfo,
                            pSession->ac_info[i].addTsRsp[j].rsp.tclasInfo,
                            SIR_MAC_TCLASIE_MAXNUM );
                    pMsg->aggrInfo.aggrAddTsInfo[i].tclasProc =
                        pSession->ac_info[i].addTsRsp[j].rsp.tclasProc;
                    pMsg->aggrInfo.aggrAddTsInfo[i].tclasProcPresent =
                        pSession->ac_info[i].addTsRsp[j].rsp.tclasProcPresent;
                    pMsg->aggrInfo.aggrAddTsInfo[i].tspec =
                        pSession->ac_info[i].addTsRsp[j].rsp.tspec;
                    pMsg->aggrInfo.aggrAddTsInfo[i].wmeTspecPresent =
                        pSession->ac_info[i].addTsRsp[j].rsp.wmeTspecPresent;
                    pMsg->aggrInfo.aggrAddTsInfo[i].wsmTspecPresent =
                        pSession->ac_info[i].addTsRsp[j].rsp.wsmTspecPresent;
                    pMsg->aggrInfo.tspecIdx |= ( 1 << i );

                    //                                                                    
                    //                                                      
                    pSession->ac_info[i].tspec_pending = (1<<j);
                }
            }
        }
    }

    VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO, 
            "Sending aggregated message to HAL 0x%x", pMsg->aggrInfo.tspecIdx);

    if(HAL_STATUS_SUCCESS(palSendMBMessage(pMac->hHdd, pMsg)))
    {
        status = eHAL_STATUS_SUCCESS;
        VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH,
                "%s: %d: sent down a AGGR QoS req to PE",
                __func__, __LINE__);
    }

    return status;
}

eHalStatus sme_QosProcessFTRICResponse(tpAniSirGlobal pMac, v_U8_t sessionId, tDot11fIERICDataDesc *pRicDataDesc, v_U8_t ac, v_U8_t tspecIndex)
{
    tANI_U8        i = 0;
    tpSirAddtsRsp   pAddtsRsp
        = &sme_QosCb.sessionInfo[sessionId].ac_info[ac].addTsRsp[tspecIndex];

    vos_mem_zero(pAddtsRsp, sizeof(tSirAddtsRsp));

    pAddtsRsp->messageType = eWNI_SME_ADDTS_RSP;
    pAddtsRsp->length = sizeof(tSirAddtsRsp);
    pAddtsRsp->rc = pRicDataDesc->RICData.statusCode;
    pAddtsRsp->sessionId = sessionId;
    pAddtsRsp->rsp.dialogToken = pRicDataDesc->RICData.Identifier;
    pAddtsRsp->rsp.status = pRicDataDesc->RICData.statusCode;
    pAddtsRsp->rsp.wmeTspecPresent = pRicDataDesc->TSPEC.present;
    if (pAddtsRsp->rsp.wmeTspecPresent)
    {
        //                                                            
        ConvertTSPEC(pMac, &pAddtsRsp->rsp.tspec, &pRicDataDesc->TSPEC);
    }

    pAddtsRsp->rsp.numTclas = pRicDataDesc->num_TCLAS;
    if (pAddtsRsp->rsp.numTclas)
    {
        for (i = 0; i < pAddtsRsp->rsp.numTclas; i++)
        {
            //                                               
            ConvertTCLAS(pMac, &pAddtsRsp->rsp.tclasInfo[i], &pRicDataDesc->TCLAS[i]);
        }
    }

    pAddtsRsp->rsp.tclasProcPresent = pRicDataDesc->TCLASSPROC.present;
    if (pAddtsRsp->rsp.tclasProcPresent)
        pAddtsRsp->rsp.tclasProc = pRicDataDesc->TCLASSPROC.processing;


    pAddtsRsp->rsp.schedulePresent = pRicDataDesc->Schedule.present;
    if (pAddtsRsp->rsp.schedulePresent)
   {
        //                                         
        ConvertSchedule(pMac, &pAddtsRsp->rsp.schedule, &pRicDataDesc->Schedule);
    }

    //                                                      
    //                     
    if (pRicDataDesc->TSDelay.present)
        ConvertTSDelay(pMac, &pAddtsRsp->rsp.delay, &pRicDataDesc->TSDelay);

    //                         
    if (pRicDataDesc->WMMTSPEC.present)
    {
        ConvertWMMTSPEC(pMac, &pAddtsRsp->rsp.tspec, &pRicDataDesc->WMMTSPEC);
    }
    //                                               
    return eHAL_STATUS_SUCCESS;
}
eHalStatus sme_QosProcessAggrQosRsp(tpAniSirGlobal pMac, void *pMsgBuf)
{
    tpSirAggrQosRsp pAggrRsp = (tpSirAggrQosRsp)pMsgBuf;
    tSirAddtsRsp   addtsRsp;
    eHalStatus status = eHAL_STATUS_SUCCESS;
    int i, j = 0;
    tANI_U8 sessionId = pAggrRsp->sessionId;

    VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO, 
            FL("Received AGGR_QOS resp from LIM"));

    /*                                                                     */
    for( i = 0; i < SIR_QOS_NUM_AC_MAX; i++ )
    {
        tANI_U8 tspec_mask_status = sme_QosCb.sessionInfo[sessionId].ac_info[i].tspec_mask_status;
        for( j = 0; j < SME_QOS_TSPEC_INDEX_MAX; j++ ) 
        {
            tANI_U8 direction = sme_QosCb.sessionInfo[sessionId].ac_info[i].
                addTsRsp[j].rsp.tspec.tsinfo.traffic.direction;

            VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO,
                    FL("Addts rsp from LIM AC=%d, flow=%d dir=%d, tspecIdx=%x"),
                    i, j, direction, pAggrRsp->aggrInfo.tspecIdx);
            //                                                   
            if( ((1<<i) & pAggrRsp->aggrInfo.tspecIdx) &&
                    ((tspec_mask_status) & (1<<j)) &&
                    ((direction == SME_QOS_WMM_TS_DIR_UPLINK) ||
                     (direction == SME_QOS_WMM_TS_DIR_BOTH)))
            {
                addtsRsp = sme_QosCb.sessionInfo[sessionId].ac_info[i].addTsRsp[j];
                addtsRsp.rc = pAggrRsp->aggrInfo.aggrRsp[i].status;
                addtsRsp.rsp.status = pAggrRsp->aggrInfo.aggrRsp[i].status;
                addtsRsp.rsp.tspec = pAggrRsp->aggrInfo.aggrRsp[i].tspec;

                VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO,
                        FL("Processing Addts rsp from LIM AC=%d, flow=%d"), i, j);
                /*                               */
                if (sme_QosProcessAddTsRsp(pMac, &addtsRsp) != eHAL_STATUS_SUCCESS)
                {
                    status = eHAL_STATUS_FAILURE;
                }
            }
        }
    }
   return status;
}


eHalStatus sme_QosProcessFTReassocRspEv(tpAniSirGlobal pMac, v_U8_t sessionId, void * pEvent_info)
{
    sme_QosSessionInfo *pSession;
    sme_QosACInfo *pACInfo;
    v_U8_t ac;
    v_U8_t tspec_flow_index;
    tDot11fIERICDataDesc *pRicDataDesc = NULL;
    eHalStatus            status = eHAL_STATUS_SUCCESS;
    tCsrRoamSession *pCsrSession = CSR_GET_SESSION( pMac, sessionId );
    tCsrRoamConnectedInfo *pCsrConnectedInfo = &pCsrSession->connectedInfo;
    tANI_U32    ricRspLen;
    /*                                            */ 
    if(pCsrConnectedInfo == NULL)
    {
        VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                FL("The connected info pointer is NULL"));
        return eHAL_STATUS_FAILURE;
    }
    ricRspLen = pCsrConnectedInfo->nRICRspLength;

    VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH,
            "%s: %d: invoked on session %d",
            __func__, __LINE__,
            sessionId);

    pSession = &sme_QosCb.sessionInfo[sessionId];

    pRicDataDesc = (tDot11fIERICDataDesc *)((pCsrConnectedInfo->pbFrames) +
        (pCsrConnectedInfo->nBeaconLength + pCsrConnectedInfo->nAssocReqLength +
        pCsrConnectedInfo->nAssocRspLength));

    for(ac = SME_QOS_EDCA_AC_BE; ac < SME_QOS_EDCA_AC_MAX; ac++)
    {
        pACInfo = &pSession->ac_info[ac];

        for (tspec_flow_index = 0; tspec_flow_index < SME_QOS_TSPEC_INDEX_MAX; tspec_flow_index++)
        {
            /*                                                                           */
            if (pACInfo->ricIdentifier[tspec_flow_index])
            {

                if (!ricRspLen)
                {
                    VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                            FL("RIC Response not received for AC %d on TSPEC Index %d, RIC Req Identifier = %d"),
                            ac, tspec_flow_index, pACInfo->ricIdentifier[tspec_flow_index]);
                    VOS_ASSERT(0);
                }
                else
                {
                    /*                                                      */
                    if (pRicDataDesc->present)
                    {
                        if (pRicDataDesc->RICData.present)
                        {
                            if (pRicDataDesc->RICData.Identifier != pACInfo->ricIdentifier[tspec_flow_index])
                            {
                                VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                                        FL("RIC response order not same as request sent. Request ID = %d, Response ID = %d"),
                                        pACInfo->ricIdentifier[tspec_flow_index], pRicDataDesc->RICData.Identifier);
                                VOS_ASSERT(0);
                            }
                            else
                            {
                                VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO, 
                                        FL("Processing RIC Response for AC %d, TSPEC Flow index %d with RIC ID %d "),
                                        ac, tspec_flow_index, pRicDataDesc->RICData.Identifier);
                                status = sme_QosProcessFTRICResponse(pMac, sessionId, pRicDataDesc, ac, tspec_flow_index);
                                if (eHAL_STATUS_SUCCESS != status)
                                {
                                    VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                                            FL("Failed with status %d for AC %d in TSPEC Flow index = %d"),
                                            status, ac, tspec_flow_index);
                                }
                            }
                            pRicDataDesc++;
                            ricRspLen -= sizeof(tDot11fIERICDataDesc);
                        }
                    }
                }
            }

        }
    }

    if (ricRspLen)
    {
        VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                FL("RIC Response still follows despite traversing through all ACs. Remaining len = %d"), ricRspLen);
        VOS_ASSERT(0);
    }

    /*                                        */
    status = sme_QosFTAggrQosReq(pMac,sessionId);

    return status;
}

#endif /*                         */



/*                                                                          
                                                                             
        
  
 
                                                                
                                                                 
                                                                             
                                                          
                                                                
                    
  
     
  
                                                                            */
eHalStatus sme_QosAddTsReq(tpAniSirGlobal pMac,
                           v_U8_t sessionId,
                           sme_QosWmmTspecInfo * pTspec_Info,
                           sme_QosEdcaAcType ac)
{
   tSirAddtsReq *pMsg = NULL;
   sme_QosSessionInfo *pSession;
   eHalStatus status = eHAL_STATUS_FAILURE;
#ifdef FEATURE_WLAN_CCX
   tCsrRoamSession *pCsrSession = CSR_GET_SESSION( pMac, sessionId );
#endif
#ifdef FEATURE_WLAN_DIAG_SUPPORT
   WLAN_VOS_DIAG_EVENT_DEF(qos, vos_event_wlan_qos_payload_type);
#endif
   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
             "%s: %d: invoked on session %d for AC %d",
             __func__, __LINE__,
             sessionId, ac);
   if (sessionId >= CSR_ROAM_SESSION_MAX)
   {
      //       
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR,
                "%s: %d: sessionId(%d) is invalid",
                __func__, __LINE__, sessionId);
      return eHAL_STATUS_FAILURE;
   }
   pSession = &sme_QosCb.sessionInfo[sessionId];
   pMsg = (tSirAddtsReq *)vos_mem_malloc(sizeof(tSirAddtsReq));
   if (!pMsg)
   {
      //       
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                "%s: %d: couldn't allocate memory for the msg buffer",
                __func__, __LINE__);
      return eHAL_STATUS_FAILURE;
   }
   vos_mem_zero(pMsg, sizeof(tSirAddtsReq));
   pMsg->messageType = pal_cpu_to_be16((v_U16_t)eWNI_SME_ADDTS_REQ);
   pMsg->length = sizeof(tSirAddtsReq);
   pMsg->sessionId = sessionId;
   pMsg->timeout = 0;
   pMsg->rspReqd = VOS_TRUE;
   pMsg->req.dialogToken = sme_QosAssignDialogToken();
   /*                                                                       
                                                                           
                                                                      */
   pMsg->req.tspec.delayBound = 0;
   pMsg->req.tspec.inactInterval = pTspec_Info->inactivity_interval;
   pMsg->req.tspec.length = SME_QOS_TSPEC_IE_LENGTH;
   pMsg->req.tspec.maxBurstSz = pTspec_Info->max_burst_size;
   pMsg->req.tspec.maxMsduSz = pTspec_Info->maximum_msdu_size;
   pMsg->req.tspec.maxSvcInterval = pTspec_Info->max_service_interval;
   pMsg->req.tspec.meanDataRate = pTspec_Info->mean_data_rate;
   pMsg->req.tspec.mediumTime = pTspec_Info->medium_time;
   pMsg->req.tspec.minDataRate = pTspec_Info->min_data_rate;
   pMsg->req.tspec.minPhyRate = pTspec_Info->min_phy_rate;
   pMsg->req.tspec.minSvcInterval = pTspec_Info->min_service_interval;
   pMsg->req.tspec.nomMsduSz = pTspec_Info->nominal_msdu_size;
   pMsg->req.tspec.peakDataRate = pTspec_Info->peak_data_rate;
   pMsg->req.tspec.surplusBw = pTspec_Info->surplus_bw_allowance;
   pMsg->req.tspec.suspendInterval = pTspec_Info->suspension_interval;
   pMsg->req.tspec.svcStartTime = 0;
   pMsg->req.tspec.tsinfo.traffic.direction = pTspec_Info->ts_info.direction;
   //                                                                              
   if( pTspec_Info->ts_info.psb 
         && btcIsReadyForUapsd(pMac) 
     )
   {
      pMsg->req.tspec.tsinfo.traffic.psb = pTspec_Info->ts_info.psb;
   }
   else
   {
      pMsg->req.tspec.tsinfo.traffic.psb = 0;
      pTspec_Info->ts_info.psb = 0;
   }
   pMsg->req.tspec.tsinfo.traffic.tsid = pTspec_Info->ts_info.tid;
   pMsg->req.tspec.tsinfo.traffic.userPrio = pTspec_Info->ts_info.up;
   pMsg->req.tspec.tsinfo.traffic.accessPolicy = SME_QOS_ACCESS_POLICY_EDCA;
   pMsg->req.tspec.tsinfo.traffic.burstSizeDefn = pTspec_Info->ts_info.burst_size_defn;
   pMsg->req.tspec.tsinfo.traffic.ackPolicy = pTspec_Info->ts_info.ack_policy;
   pMsg->req.tspec.type = SME_QOS_TSPEC_IE_TYPE;
   /*                              */
   if (NULL == pSession->assocInfo.pBssDesc)
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                "%s: %d: BSS descriptor is NULL so we don't send request to PE",
                __func__, __LINE__);
      vos_mem_free(pMsg);
      return eHAL_STATUS_FAILURE;
   }
   vos_mem_copy( &pMsg->bssId[ 0 ], 
                 &pSession->assocInfo.pBssDesc->bssId[ 0 ], 
                 sizeof(tCsrBssid) );
   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
             "%s: %d: up = %d, tid = %d",
             __func__, __LINE__, 
             pTspec_Info->ts_info.up,
             pTspec_Info->ts_info.tid);
#ifdef FEATURE_WLAN_CCX
   if(pCsrSession->connectedProfile.isCCXAssoc)
   {
      pMsg->req.tsrsIE.tsid = pTspec_Info->ts_info.up;
      pMsg->req.tsrsPresent = 1;
   }
#endif
   if(HAL_STATUS_SUCCESS(palSendMBMessage(pMac->hHdd, pMsg)))
   {
      status = eHAL_STATUS_SUCCESS;
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                "%s: %d: sent down a ADDTS req to PE",
                __func__, __LINE__);
      //                     
#ifdef FEATURE_WLAN_DIAG_SUPPORT          
      qos.eventId = SME_QOS_DIAG_ADDTS_REQ;
      qos.reasonCode = SME_QOS_DIAG_USER_REQUESTED;
      WLAN_VOS_DIAG_EVENT_REPORT(&qos, EVENT_WLAN_QOS);
#endif //                         
   }
   return status;
}
/*                                                                          
                                                                             
        
  
 
                                                                
                                                                   
                                                                
                                                                   
  
                    
  
     
  
                                                                            */
eHalStatus sme_QosDelTsReq(tpAniSirGlobal pMac,
                           v_U8_t sessionId,
                           sme_QosEdcaAcType ac,
                           v_U8_t tspec_mask)
{
   sme_QosSessionInfo *pSession;
   sme_QosACInfo *pACInfo;
   tSirDeltsReq *pMsg;
   sme_QosWmmTspecInfo *pTspecInfo;
   eHalStatus status = eHAL_STATUS_FAILURE;
#ifdef FEATURE_WLAN_DIAG_SUPPORT
   WLAN_VOS_DIAG_EVENT_DEF(qos, vos_event_wlan_qos_payload_type);
#endif
   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
             "%s: %d: invoked on session %d for AC %d",
             __func__, __LINE__,
             sessionId, ac);
   pMsg = (tSirDeltsReq *)vos_mem_malloc(sizeof(tSirDeltsReq));
   if (!pMsg)
   {
      //       
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                "%s: %d: couldn't allocate memory for the msg buffer",
                __func__, __LINE__);
      return eHAL_STATUS_FAILURE;
   }
   vos_mem_zero(pMsg, sizeof(tSirDeltsReq));
   //                                       
   pSession = &sme_QosCb.sessionInfo[sessionId];
   pACInfo = &pSession->ac_info[ac];
   pTspecInfo = &pACInfo->curr_QoSInfo[tspec_mask - 1];
   pMsg->messageType = pal_cpu_to_be16((v_U16_t)eWNI_SME_DELTS_REQ);
   pMsg->length = sizeof(tSirDeltsReq);
   pMsg->sessionId = sessionId;
   pMsg->rspReqd = VOS_TRUE;
   pMsg->req.tspec.delayBound = pTspecInfo->delay_bound;
   pMsg->req.tspec.inactInterval = pTspecInfo->inactivity_interval;
   pMsg->req.tspec.length = SME_QOS_TSPEC_IE_LENGTH;
   pMsg->req.tspec.maxBurstSz = pTspecInfo->max_burst_size;
   pMsg->req.tspec.maxMsduSz = pTspecInfo->maximum_msdu_size;
   pMsg->req.tspec.maxSvcInterval = pTspecInfo->max_service_interval;
   pMsg->req.tspec.meanDataRate = pTspecInfo->mean_data_rate;
   pMsg->req.tspec.mediumTime = pTspecInfo->medium_time;
   pMsg->req.tspec.minDataRate = pTspecInfo->min_data_rate;
   pMsg->req.tspec.minPhyRate = pTspecInfo->min_phy_rate;
   pMsg->req.tspec.minSvcInterval = pTspecInfo->min_service_interval;
   pMsg->req.tspec.nomMsduSz = pTspecInfo->nominal_msdu_size;
   pMsg->req.tspec.peakDataRate = pTspecInfo->peak_data_rate;
   pMsg->req.tspec.surplusBw = pTspecInfo->surplus_bw_allowance;
   pMsg->req.tspec.suspendInterval = pTspecInfo->suspension_interval;
   pMsg->req.tspec.svcStartTime = pTspecInfo->svc_start_time;
   pMsg->req.tspec.tsinfo.traffic.direction = pTspecInfo->ts_info.direction;
   pMsg->req.tspec.tsinfo.traffic.psb = pTspecInfo->ts_info.psb;
   pMsg->req.tspec.tsinfo.traffic.tsid = pTspecInfo->ts_info.tid;
   pMsg->req.tspec.tsinfo.traffic.userPrio = pTspecInfo->ts_info.up;
   pMsg->req.tspec.tsinfo.traffic.accessPolicy = SME_QOS_ACCESS_POLICY_EDCA;
   pMsg->req.tspec.tsinfo.traffic.burstSizeDefn = pTspecInfo->ts_info.burst_size_defn;
   pMsg->req.tspec.tsinfo.traffic.ackPolicy = pTspecInfo->ts_info.ack_policy;
   pMsg->req.tspec.type = SME_QOS_TSPEC_IE_TYPE;
   /*                              */
   if (NULL == pSession->assocInfo.pBssDesc)
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                "%s: %d: BSS descriptor is NULL so we don't send request to PE",
                __func__, __LINE__);
      vos_mem_free(pMsg);
      return eHAL_STATUS_FAILURE;
   }
   vos_mem_copy( &pMsg->bssId[ 0 ], 
                 &pSession->assocInfo.pBssDesc->bssId[ 0 ], 
                 sizeof(tCsrBssid) );

   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
             "%s: %d: up = %d, tid = %d",
             __func__, __LINE__, 
             pTspecInfo->ts_info.up,
             pTspecInfo->ts_info.tid);
   vos_mem_zero(&pACInfo->curr_QoSInfo[tspec_mask - 1], 
                sizeof(sme_QosWmmTspecInfo));
   if(HAL_STATUS_SUCCESS(palSendMBMessage(pMac->hHdd, pMsg)))
   {
      status = eHAL_STATUS_SUCCESS;
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                "%s: %d: sme_QosDelTsReq:Test: sent down a DELTS req to PE",
                __func__, __LINE__);
      //                     
#ifdef FEATURE_WLAN_DIAG_SUPPORT          
      qos.eventId = SME_QOS_DIAG_DELTS;
      qos.reasonCode = SME_QOS_DIAG_USER_REQUESTED;
      WLAN_VOS_DIAG_EVENT_REPORT(&qos, EVENT_WLAN_QOS);
#endif //                         
   }

   return status;
}


/*                                                                          
                                                           
                                  
  
                                                                
                                                             
  
                    
  
     
  
                                                                            */
eHalStatus sme_QosProcessAddTsRsp(tpAniSirGlobal pMac, void *pMsgBuf)
{
   tpSirAddtsRsp paddts_rsp = (tpSirAddtsRsp)pMsgBuf;
   sme_QosSessionInfo *pSession;
   v_U8_t sessionId = paddts_rsp->sessionId;
   eHalStatus status = eHAL_STATUS_FAILURE;
#ifdef WLAN_FEATURE_VOWIFI_11R
    sme_QosWmmUpType up = (sme_QosWmmUpType)paddts_rsp->rsp.tspec.tsinfo.traffic.userPrio;
    sme_QosACInfo *pACInfo;
    sme_QosEdcaAcType ac;
#endif
#ifdef FEATURE_WLAN_DIAG_SUPPORT
    WLAN_VOS_DIAG_EVENT_DEF(qos, vos_event_wlan_qos_payload_type);
#endif

    pSession = &sme_QosCb.sessionInfo[sessionId];

#ifdef WLAN_FEATURE_VOWIFI_11R
    VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
            "%s: %d: invoked on session %d for UP %d",
            __func__, __LINE__,
            sessionId, up);

    ac = sme_QosUpToAc(up);
    if(SME_QOS_EDCA_AC_MAX == ac)
    {
        //       
        VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                "%s: %d: invalid AC %d from UP %d",
                __func__, __LINE__, ac, up);

        return eHAL_STATUS_FAILURE;
    }
    pACInfo = &pSession->ac_info[ac];   
    if (SME_QOS_HANDOFF == pACInfo->curr_state)
    {
        smsLog(pMac, LOG1, FL("ADDTS Response received for AC %d in HANDOFF State.. Dropping"), ac);
        pSession->readyForPowerSave = VOS_TRUE;
        return eHAL_STATUS_SUCCESS;
    }
#endif

   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
             "%s: %d: Invoked on session %d with return code %d",
             __func__, __LINE__,
             sessionId, paddts_rsp->rc);
   //                                          
   //                         
   pSession->readyForPowerSave = VOS_TRUE;
   if(paddts_rsp->rc)
   {
      //                     
#ifdef FEATURE_WLAN_DIAG_SUPPORT          
      qos.eventId = SME_QOS_DIAG_ADDTS_RSP;
      qos.reasonCode = SME_QOS_DIAG_ADDTS_REFUSED;
      WLAN_VOS_DIAG_EVENT_REPORT(&qos, EVENT_WLAN_QOS);
#endif //                         
      status = sme_QosProcessAddTsFailureRsp(pMac, sessionId, &paddts_rsp->rsp);
   }
   else
   {
      status = sme_QosProcessAddTsSuccessRsp(pMac, sessionId, &paddts_rsp->rsp);
   }
   return status;
}
/*                                                                          
                                                           
                                  
  
                                                                
                                                             
  
                    
  
     
  
                                                                            */
eHalStatus sme_QosProcessDelTsRsp(tpAniSirGlobal pMac, void *pMsgBuf)
{
   tpSirDeltsRsp pDeltsRsp = (tpSirDeltsRsp)pMsgBuf;
   sme_QosSessionInfo *pSession;
   v_U8_t sessionId = pDeltsRsp->sessionId;
   //    
   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
             "%s: %d: Invoked on session %d with return code %d",
             __func__, __LINE__,
             sessionId, pDeltsRsp->rc);
   pSession = &sme_QosCb.sessionInfo[sessionId];
   //                                          
   //                         
   pSession->readyForPowerSave = VOS_TRUE;
   (void)sme_QosProcessBufferedCmd(sessionId);
   return eHAL_STATUS_SUCCESS;
}
/*                                                                          
                                                           
                                  
  
                                                                              
                           
                                                                
                                                             
  
                    
  
     
  
                                                                            */
eHalStatus sme_QosProcessDelTsInd(tpAniSirGlobal pMac, void *pMsgBuf)
{
   tpSirDeltsRsp pdeltsind = (tpSirDeltsRsp)pMsgBuf;
   sme_QosSessionInfo *pSession;
   sme_QosACInfo *pACInfo;
   v_U8_t sessionId = pdeltsind->sessionId;
   sme_QosEdcaAcType ac;
   sme_QosSearchInfo search_key;
   sme_QosWmmUpType up = (sme_QosWmmUpType)pdeltsind->rsp.tspec.tsinfo.traffic.userPrio;
#ifdef FEATURE_WLAN_DIAG_SUPPORT
   WLAN_VOS_DIAG_EVENT_DEF(qos, vos_event_wlan_qos_payload_type);
#endif
   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
             "%s: %d: Invoked on session %d for UP %d",
             __func__, __LINE__,
             sessionId, up);
   ac = sme_QosUpToAc(up);
   if(SME_QOS_EDCA_AC_MAX == ac)
   {
      //       
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                "%s: %d: invalid AC %d from UP %d",
                __func__, __LINE__,
                ac, up);
      return eHAL_STATUS_FAILURE;
   }
   pSession = &sme_QosCb.sessionInfo[sessionId];
   pACInfo = &pSession->ac_info[ac];

   vos_mem_zero(&search_key, sizeof(sme_QosSearchInfo));
   //                                                          
   search_key.key.ac_type = ac;
   search_key.index = SME_QOS_SEARCH_KEY_INDEX_2;
   search_key.sessionId = sessionId;
   //                                                                           
   //                                               
   if(!HAL_STATUS_SUCCESS(sme_QosFindAllInFlowList(pMac, search_key, sme_QosDelTsIndFnp)))
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                "%s: %d: no match found for ac = %d",
                __func__, __LINE__, 
                search_key.key.ac_type);
      //      
      VOS_ASSERT(0);
      return eHAL_STATUS_FAILURE;
   }

//                     
#ifdef FEATURE_WLAN_DIAG_SUPPORT
   qos.eventId = SME_QOS_DIAG_DELTS;
   qos.reasonCode = SME_QOS_DIAG_DELTS_IND_FROM_AP;
   WLAN_VOS_DIAG_EVENT_REPORT(&qos, EVENT_WLAN_QOS);
#endif //                         

   return eHAL_STATUS_SUCCESS;
}
/*                                                                          
                                                                  
                                                      
                                                            
  
                    
  
     
  
                                                                            */
eHalStatus sme_QosProcessAssocCompleteEv(tpAniSirGlobal pMac, v_U8_t sessionId, void * pEvent_info)
{
   sme_QosSessionInfo *pSession;
   sme_QosACInfo *pACInfo;
   eHalStatus status = eHAL_STATUS_FAILURE;
   sme_QosEdcaAcType ac = SME_QOS_EDCA_AC_BE;
   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
             "%s: %d: invoked on session %d",
             __func__, __LINE__,
             sessionId);
   pSession = &sme_QosCb.sessionInfo[sessionId];
   if(((SME_QOS_INIT == pSession->ac_info[SME_QOS_EDCA_AC_BE].curr_state)&&
       (SME_QOS_INIT == pSession->ac_info[SME_QOS_EDCA_AC_BK].curr_state)&&
       (SME_QOS_INIT == pSession->ac_info[SME_QOS_EDCA_AC_VI].curr_state)&&
       (SME_QOS_INIT == pSession->ac_info[SME_QOS_EDCA_AC_VO].curr_state)) ||
       (pSession->handoffRequested))
   {
      //                        
      if(!pEvent_info)
      {
         //       
         VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                   "%s: %d: pEvent_info is NULL",
                   __func__, __LINE__);
         return status;
      }
      if(!((sme_QosAssocInfo *)pEvent_info)->pBssDesc)
      {
         //       
         VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                   "%s: %d: pBssDesc is NULL",
                   __func__, __LINE__);
         return status;
      }
      if((pSession->assocInfo.pBssDesc) &&
         (csrIsBssidMatch(pMac, (tCsrBssid *)&pSession->assocInfo.pBssDesc->bssId, 
                          (tCsrBssid *) &(((sme_QosAssocInfo *)pEvent_info)->pBssDesc->bssId))))
      {
         VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                   "%s: %d: assoc with the same BSS, no update needed",
                   __func__, __LINE__);
      }
      else
      {
         status = sme_QosSaveAssocInfo(pSession, pEvent_info);
      }
   }
   else
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                "%s: %d: wrong state: BE %d, BK %d, VI %d, VO %d",
                __func__, __LINE__,
                pSession->ac_info[SME_QOS_EDCA_AC_BE].curr_state,
                pSession->ac_info[SME_QOS_EDCA_AC_BK].curr_state,
                pSession->ac_info[SME_QOS_EDCA_AC_VI].curr_state,
                pSession->ac_info[SME_QOS_EDCA_AC_VO].curr_state);
      //      
      VOS_ASSERT(0);
      return status;
   }
   //                      
   pSession->sessionActive = VOS_TRUE;
   if(pSession->handoffRequested)
   {
      pSession->handoffRequested = VOS_FALSE;
      //               
      (void)sme_QosProcessBufferedCmd(sessionId);
      status = eHAL_STATUS_SUCCESS;
   }
   else
   {
      for(ac = SME_QOS_EDCA_AC_BE; ac < SME_QOS_EDCA_AC_MAX; ac++) 
      {
         pACInfo = &pSession->ac_info[ac];
         switch(pACInfo->curr_state)
         {
            case SME_QOS_INIT:
               sme_QosStateTransition(sessionId, ac, SME_QOS_LINK_UP);
               break;
            case SME_QOS_LINK_UP:
            case SME_QOS_REQUESTED:
            case SME_QOS_QOS_ON:
            case SME_QOS_HANDOFF:
            case SME_QOS_CLOSED:
            default:
               VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                         "%s: %d: On session %d AC %d is in wrong state %d",
                         __func__, __LINE__,
                         sessionId, ac, pACInfo->curr_state);
               //      
               VOS_ASSERT(0);
               break;
         }
      }
   }
   return status;
}
/*                                                                          
                                                               
                                                   
                                                            
  
                    
  
     
  
                                                                            */
eHalStatus sme_QosProcessReassocReqEv(tpAniSirGlobal pMac, v_U8_t sessionId, void * pEvent_info)
{
   sme_QosSessionInfo *pSession;
   sme_QosACInfo *pACInfo;
   sme_QosEdcaAcType ac;
   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
             "%s: %d: invoked on session %d",
             __func__, __LINE__,
             sessionId);
   pSession = &sme_QosCb.sessionInfo[sessionId];

#ifdef WLAN_FEATURE_VOWIFI_11R
   if(pSession->ftHandoffInProgress)
   {
       VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH,
               "%s: %d: no need for state transition, should "
               "already be in handoff state",
               __func__, __LINE__);
       VOS_ASSERT(pSession->ac_info[0].curr_state == SME_QOS_HANDOFF);
       VOS_ASSERT(pSession->ac_info[1].curr_state == SME_QOS_HANDOFF);
       VOS_ASSERT(pSession->ac_info[2].curr_state == SME_QOS_HANDOFF);
       VOS_ASSERT(pSession->ac_info[3].curr_state == SME_QOS_HANDOFF);
       sme_QosProcessFTReassocReqEv(pMac, sessionId, pEvent_info);
       return eHAL_STATUS_SUCCESS;
   }
#endif

   if(pSession->handoffRequested)
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                "%s: %d: no need for state transition, should "
                "already be in handoff state",
                __func__, __LINE__);
      VOS_ASSERT(pSession->ac_info[0].curr_state == SME_QOS_HANDOFF);
      VOS_ASSERT(pSession->ac_info[1].curr_state == SME_QOS_HANDOFF);
      VOS_ASSERT(pSession->ac_info[2].curr_state == SME_QOS_HANDOFF);
      VOS_ASSERT(pSession->ac_info[3].curr_state == SME_QOS_HANDOFF);

      //                                                             
      sme_QosBufferExistingFlows(pMac, sessionId);
      //                                                
      sme_QosCleanupCtrlBlkForHandoff(pMac, sessionId);
      return eHAL_STATUS_SUCCESS;
   }
//                                                                         
#ifdef WLAN_FEATURE_VOWIFI_11R
   if(pSession->ftHandoffInProgress)
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                "%s: %d: no need for state transition, should "
                "already be in handoff state",
                __func__, __LINE__);
      VOS_ASSERT(pSession->ac_info[0].curr_state == SME_QOS_HANDOFF);
      VOS_ASSERT(pSession->ac_info[1].curr_state == SME_QOS_HANDOFF);
      VOS_ASSERT(pSession->ac_info[2].curr_state == SME_QOS_HANDOFF);
      VOS_ASSERT(pSession->ac_info[3].curr_state == SME_QOS_HANDOFF);

      sme_QosProcessFTReassocReqEv(pMac, sessionId, pEvent_info);
      return eHAL_STATUS_SUCCESS;
   }
#endif

   for(ac = SME_QOS_EDCA_AC_BE; ac < SME_QOS_EDCA_AC_MAX; ac++) 
   {
      pACInfo = &pSession->ac_info[ac];
      switch(pACInfo->curr_state)
      {
         case SME_QOS_LINK_UP:
         case SME_QOS_REQUESTED:
         case SME_QOS_QOS_ON:
            sme_QosStateTransition(sessionId, ac, SME_QOS_HANDOFF);
            break;
         case SME_QOS_HANDOFF:
            //                                                                         
            break;
         case SME_QOS_CLOSED:
         case SME_QOS_INIT:
         default:
            VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                      "%s: %d: On session %d AC %d is in wrong state %d",
                      __func__, __LINE__,
                      sessionId, ac, pACInfo->curr_state);
            //      
            VOS_ASSERT(0);
            break;
      }
   }
   return eHAL_STATUS_SUCCESS;
}
/*                                                                          
                                                                   
                                                        
                                                            
  
                    
  
     
  
                                                                            */
eHalStatus sme_QosProcessReassocSuccessEv(tpAniSirGlobal pMac, v_U8_t sessionId, void * pEvent_info)
{

   tCsrRoamSession *pCsrRoamSession = NULL;
   sme_QosSessionInfo *pSession;
   sme_QosACInfo *pACInfo;
   sme_QosEdcaAcType ac, ac_index;
   sme_QosSearchInfo search_key;
   sme_QosSearchInfo search_key1;
   eHalStatus status = eHAL_STATUS_FAILURE;
   tListElem *pEntry= NULL;
   sme_QosFlowInfoEntry *flow_info = NULL;

   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
             "%s: %d: invoked on session %d",
             __func__, __LINE__,
             sessionId);

   if (CSR_ROAM_SESSION_MAX <= sessionId) {
       VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR,
                "%s: %d: invoked on session %d",
                __func__, __LINE__,
                sessionId);
       return status;
   }

   pCsrRoamSession = CSR_GET_SESSION( pMac, sessionId );

   pSession = &sme_QosCb.sessionInfo[sessionId];
   //                                        
   //                       
   pSession->readyForPowerSave = VOS_TRUE;
   //                        
   if(!pEvent_info)
   {
      //       
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                "%s: %d: pEvent_info is NULL",
                __func__, __LINE__);
      return status;
   }
   if(!((sme_QosAssocInfo *)pEvent_info)->pBssDesc)
   {
      //       
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                "%s: %d: pBssDesc is NULL",
                __func__, __LINE__);
      return status;
   }
   status = sme_QosSaveAssocInfo(pSession, pEvent_info);
   if(status)
   {
      //       
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                "%s: %d: sme_QosSaveAssocInfo() failed",
                __func__, __LINE__);
   }
//                                                                            
   if(pSession->handoffRequested)
   {
      pSession->handoffRequested = VOS_FALSE;
      //               
      (void)sme_QosProcessBufferedCmd(sessionId);
      return eHAL_STATUS_SUCCESS;
   }
#ifdef WLAN_FEATURE_VOWIFI_11R
   if (pSession->ftHandoffInProgress)
   {
       if (csrRoamIs11rAssoc(pMac))
       {
           if (pCsrRoamSession && pCsrRoamSession->connectedInfo.nRICRspLength)
           {
               status = sme_QosProcessFTReassocRspEv(pMac, sessionId, pEvent_info);
           }
       }
#ifdef FEATURE_WLAN_CCX
       //                                                                
       if (csrRoamIsCCXAssoc(pMac))
       {
           if (pCsrRoamSession && pCsrRoamSession->connectedInfo.nTspecIeLength)
           {
               status = sme_QosCCXProcessReassocTspecRsp(pMac, sessionId, pEvent_info);
           }
       }
#endif
       pSession->ftHandoffInProgress = VOS_FALSE;
       pSession->handoffRequested = VOS_FALSE;
       return status;
   }
#endif

   pSession->sessionActive = VOS_TRUE;
   for(ac = SME_QOS_EDCA_AC_BE; ac < SME_QOS_EDCA_AC_MAX; ac++) 
   {
      pACInfo = &pSession->ac_info[ac];
      switch(pACInfo->curr_state)
      {
         case SME_QOS_HANDOFF:
            //                             
            sme_QosStateTransition(sessionId, ac, pACInfo->prev_state);
            //                                                  
            if(pACInfo->reassoc_pending)
            {
               //                                                               
               //                                           
               if(pACInfo->requested_QoSInfo[SME_QOS_TSPEC_INDEX_0].ts_info.psb)
               {
                  pSession->apsdMask |= 1 << (SME_QOS_EDCA_AC_VO - ac);
               }
               else
               {
                  pSession->apsdMask &= ~(1 << (SME_QOS_EDCA_AC_VO - ac));
               }
               pACInfo->reassoc_pending = VOS_FALSE;
               //                                                                    
               //                           
               sme_QosStateTransition(sessionId, ac, SME_QOS_QOS_ON);
               //                                     
               pACInfo->curr_QoSInfo[SME_QOS_TSPEC_INDEX_0] = 
                  pACInfo->requested_QoSInfo[SME_QOS_TSPEC_INDEX_0];
               vos_mem_zero(&search_key, sizeof(sme_QosSearchInfo));
               //                                                          
               search_key.key.ac_type = ac;
               search_key.index = SME_QOS_SEARCH_KEY_INDEX_2;
               search_key.sessionId = sessionId;
               //                                                        

               vos_mem_zero(&search_key1, sizeof(sme_QosSearchInfo));
               //                                                  
               search_key1.index = SME_QOS_SEARCH_KEY_INDEX_3;
               search_key1.key.reason = SME_QOS_REASON_SETUP;
               search_key1.sessionId = sessionId;
               for(ac_index = SME_QOS_EDCA_AC_BE; ac_index < SME_QOS_EDCA_AC_MAX; ac_index++)
               {
                  pEntry = sme_QosFindInFlowList(search_key1);
                  if(pEntry)
                  {
                     flow_info = GET_BASE_ADDR( pEntry, sme_QosFlowInfoEntry, link );
                     if(flow_info->ac_type == ac)
                     {
                        pACInfo->hoRenewal = flow_info->hoRenewal;
                        break;
                     }
                  }
               }
               //                                              
               //                                                                  
               if(!HAL_STATUS_SUCCESS(sme_QosFindAllInFlowList(pMac, search_key, sme_QosReassocSuccessEvFnp)))
               {
                  VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                            "%s: %d: no match found for ac = %d",
                            __func__, __LINE__,
                            search_key.key.ac_type);
                  //      
                  VOS_ASSERT(0);
                  return eHAL_STATUS_FAILURE;
               }
               pACInfo->hoRenewal = VOS_FALSE;
               vos_mem_zero(&pACInfo->requested_QoSInfo[SME_QOS_TSPEC_INDEX_0], 
                            sizeof(sme_QosWmmTspecInfo));
            }
            status = eHAL_STATUS_SUCCESS;
            break;
         case SME_QOS_INIT:
         case SME_QOS_CLOSED:
            //   
            status = eHAL_STATUS_SUCCESS;
            break;
         case SME_QOS_LINK_UP:
         case SME_QOS_REQUESTED:
         case SME_QOS_QOS_ON:
         default:
            VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                      "%s: %d: On session %d AC %d is in wrong state %d",
                      __func__, __LINE__,
                      sessionId, ac, pACInfo->curr_state);
            //      
            VOS_ASSERT(0);
            break;
      }
   }
   (void)sme_QosProcessBufferedCmd(sessionId);
   return status;
}

/*                                                                          
                                                                   
                                                       
                                                            
  
                    
  
     
  
                                                                            */
eHalStatus sme_QosProcessReassocFailureEv(tpAniSirGlobal pMac, v_U8_t sessionId, void * pEvent_info)
{
   sme_QosSessionInfo *pSession;
   sme_QosACInfo *pACInfo;
   sme_QosEdcaAcType ac;
   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
             "%s: %d: invoked on session %d",
             __func__, __LINE__,
             sessionId);
   pSession = &sme_QosCb.sessionInfo[sessionId];
   //                                        
   //                       
   pSession->readyForPowerSave = VOS_TRUE;
   for(ac = SME_QOS_EDCA_AC_BE; ac < SME_QOS_EDCA_AC_MAX; ac++) 
   {
      pACInfo = &pSession->ac_info[ac];
      switch(pACInfo->curr_state)
      {
         case SME_QOS_HANDOFF:
            sme_QosStateTransition(sessionId, ac, SME_QOS_INIT);
            if(pACInfo->reassoc_pending)
            {
               pACInfo->reassoc_pending = VOS_FALSE;
            }
            vos_mem_zero(&pACInfo->curr_QoSInfo[SME_QOS_TSPEC_INDEX_0], 
                         sizeof(sme_QosWmmTspecInfo));
            vos_mem_zero(&pACInfo->requested_QoSInfo[SME_QOS_TSPEC_INDEX_0], 
                         sizeof(sme_QosWmmTspecInfo));
            vos_mem_zero(&pACInfo->curr_QoSInfo[SME_QOS_TSPEC_INDEX_1], 
                         sizeof(sme_QosWmmTspecInfo));
            vos_mem_zero(&pACInfo->requested_QoSInfo[SME_QOS_TSPEC_INDEX_1], 
                         sizeof(sme_QosWmmTspecInfo));
            pACInfo->tspec_mask_status = SME_QOS_TSPEC_MASK_CLEAR;
            pACInfo->tspec_pending = 0;
            pACInfo->num_flows[SME_QOS_TSPEC_INDEX_0] = 0;
            pACInfo->num_flows[SME_QOS_TSPEC_INDEX_1] = 0;
            break;
         case SME_QOS_INIT:
         case SME_QOS_CLOSED:
            //   
            break;
         case SME_QOS_LINK_UP:
         case SME_QOS_REQUESTED:
         case SME_QOS_QOS_ON:
         default:
            VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                      "%s: %d: On session %d AC %d is in wrong state %d",
                      __func__, __LINE__,
                      sessionId, ac, pACInfo->curr_state);
            //      
            VOS_ASSERT(0);
            break;
      }
   }
   //                      
   sme_QosDeleteExistingFlows(pMac, sessionId);
   return eHAL_STATUS_SUCCESS;
}
/*                                                                          
                                                                    
                                                         
                                                            
  
                    
  
     
  
                                                                            */
eHalStatus sme_QosProcessHandoffAssocReqEv(tpAniSirGlobal pMac, v_U8_t sessionId, void * pEvent_info)
{
   sme_QosSessionInfo *pSession;
   sme_QosACInfo *pACInfo;
   v_U8_t ac;
   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
             "%s: %d: invoked on session %d",
             __func__, __LINE__,
             sessionId);
   pSession = &sme_QosCb.sessionInfo[sessionId];
   for(ac = SME_QOS_EDCA_AC_BE; ac < SME_QOS_EDCA_AC_MAX; ac++) 
   {
      pACInfo = &pSession->ac_info[ac];
      switch(pACInfo->curr_state)
      {
         case SME_QOS_LINK_UP:
         case SME_QOS_REQUESTED:
         case SME_QOS_QOS_ON:
            sme_QosStateTransition(sessionId, ac, SME_QOS_HANDOFF);
            break;
         case SME_QOS_HANDOFF:
            //               
#ifdef WLAN_FEATURE_VOWIFI_11R
            if(pSession->ftHandoffInProgress)
            {
               VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO,
                         "%s: %d: SME_QOS_CSR_HANDOFF_ASSOC_REQ received in "
                         "SME_QOS_HANDOFF state with FT in progress"
                         , __func__, __LINE__); 
               break; 
            }
#endif            

         case SME_QOS_CLOSED:
         case SME_QOS_INIT:
         default:
            VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                      "%s: %d: On session %d AC %d is in wrong state %d",
                      __func__, __LINE__,
                      sessionId, ac, pACInfo->curr_state);
            //      
            VOS_ASSERT(0);
            break;
      }
   }
   //                                                                 
   if (!pSession->ftHandoffInProgress) {
       pSession->handoffRequested = VOS_TRUE;
   }
   //                                   
   pSession->apsdMask = 0;
   //                                     
   if (!sme_QosIsUapsdActive())
   {
      //                                         
      //                                      
      (void)pmcStopUapsd(pMac);
   }
   pSession->uapsdAlreadyRequested = VOS_FALSE;
   return eHAL_STATUS_SUCCESS;
}
/*                                                                          
                                                                   
                                                        
                                                            
  
                    
  
     
  
                                                                            */
eHalStatus sme_QosProcessHandoffSuccessEv(tpAniSirGlobal pMac, v_U8_t sessionId, void * pEvent_info)
{
   sme_QosSessionInfo *pSession;
   sme_QosACInfo *pACInfo;
   v_U8_t ac;
   eHalStatus status = eHAL_STATUS_FAILURE;
   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
             "%s: %d: invoked on session %d",
             __func__, __LINE__,
             sessionId);
   pSession = &sme_QosCb.sessionInfo[sessionId];
   //                                        
   for(ac = SME_QOS_EDCA_AC_BE; ac < SME_QOS_EDCA_AC_MAX; ac++) 
   {
      pACInfo = &pSession->ac_info[ac];
      switch(pACInfo->curr_state)
      {
         case SME_QOS_HANDOFF:
            sme_QosStateTransition(sessionId, ac, pACInfo->prev_state);
            //                                                       
            if(SME_QOS_REQUESTED == pACInfo->curr_state)
            {
               pACInfo->curr_state = SME_QOS_LINK_UP;
            }
            status = eHAL_STATUS_SUCCESS;
            break;
         //                                                                  
         //                                                                  
         //                         
         case SME_QOS_REQUESTED:
            break;
         case SME_QOS_INIT:
         case SME_QOS_CLOSED:
         case SME_QOS_LINK_UP:
         case SME_QOS_QOS_ON:
         default:
#ifdef WLAN_FEATURE_VOWIFI_11R
/*                                                                             
                                  */
            if( pSession->ftHandoffInProgress )
               break;
#endif
            VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                      "%s: %d: On session %d AC %d is in wrong state %d",
                      __func__, __LINE__,
                      sessionId, ac, pACInfo->curr_state);
            //      
            VOS_ASSERT(0);
            break;
      }
   }
   return status;
}
/*                                                                          
                                                                   
                                                       
                                                            
  
                    
  
     
  
                                                                            */
eHalStatus sme_QosProcessHandoffFailureEv(tpAniSirGlobal pMac, v_U8_t sessionId, void * pEvent_info)
{
   sme_QosSessionInfo *pSession;
   sme_QosACInfo *pACInfo;
   v_U8_t ac;
   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
             "%s: %d: invoked on session %d",
             __func__, __LINE__,
             sessionId);
   pSession = &sme_QosCb.sessionInfo[sessionId];
   for(ac = SME_QOS_EDCA_AC_BE; ac < SME_QOS_EDCA_AC_MAX; ac++) 
   {
      pACInfo = &pSession->ac_info[ac];
      switch(pACInfo->curr_state)
      {
         case SME_QOS_HANDOFF:
            sme_QosStateTransition(sessionId, ac, SME_QOS_INIT);
            //                            
            vos_mem_zero(&pACInfo->curr_QoSInfo[SME_QOS_TSPEC_INDEX_0], 
                         sizeof(sme_QosWmmTspecInfo));
            vos_mem_zero(&pACInfo->requested_QoSInfo[SME_QOS_TSPEC_INDEX_0], 
                         sizeof(sme_QosWmmTspecInfo));
            vos_mem_zero(&pACInfo->curr_QoSInfo[SME_QOS_TSPEC_INDEX_1], 
                         sizeof(sme_QosWmmTspecInfo));
            vos_mem_zero(&pACInfo->requested_QoSInfo[SME_QOS_TSPEC_INDEX_1], 
                         sizeof(sme_QosWmmTspecInfo));
            pACInfo->tspec_mask_status = SME_QOS_TSPEC_MASK_CLEAR;
            pACInfo->tspec_pending = 0;
            pACInfo->num_flows[SME_QOS_TSPEC_INDEX_0] = 0;
            pACInfo->num_flows[SME_QOS_TSPEC_INDEX_1] = 0;
            break;
         case SME_QOS_INIT:
         case SME_QOS_CLOSED:
         case SME_QOS_LINK_UP:
         case SME_QOS_REQUESTED:
         case SME_QOS_QOS_ON:
         default:
            VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                      "%s: %d: On session %d AC %d is in wrong state %d",
                      __func__, __LINE__,
                      sessionId, ac, pACInfo->curr_state);
            //      
            VOS_ASSERT(0);
            break;
      }
   }
   //                    
   pSession->handoffRequested = VOS_FALSE;
   //                       
   if(pSession->assocInfo.pBssDesc)
   {
      vos_mem_free(pSession->assocInfo.pBssDesc);
      pSession->assocInfo.pBssDesc = NULL;
   }
   return eHAL_STATUS_SUCCESS;
}
/*                                                                          
                                                               
                                                                             
          
                                                            
  
                    
  
     
  
                                                                            */
eHalStatus sme_QosProcessDisconnectEv(tpAniSirGlobal pMac, v_U8_t sessionId, void * pEvent_info)
{
   sme_QosSessionInfo *pSession;
   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
             "%s: %d: invoked on session %d",
             __func__, __LINE__,
             sessionId);
   pSession = &sme_QosCb.sessionInfo[sessionId];
   if((pSession->handoffRequested)
#ifdef WLAN_FEATURE_VOWIFI_11R
/*                                                                             
                                  */
      && !pSession->ftHandoffInProgress
#endif
      )
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                "%s: %d: no need for state transition, should "
                "already be in handoff state",
                __func__, __LINE__);
      VOS_ASSERT(pSession->ac_info[0].curr_state == SME_QOS_HANDOFF);
      VOS_ASSERT(pSession->ac_info[1].curr_state == SME_QOS_HANDOFF);
      VOS_ASSERT(pSession->ac_info[2].curr_state == SME_QOS_HANDOFF);
      VOS_ASSERT(pSession->ac_info[3].curr_state == SME_QOS_HANDOFF);
      return eHAL_STATUS_SUCCESS;
   }
   sme_QosInitACs(pMac, sessionId);
   //                                   
   pSession->apsdMask = 0;
   //                                     
   if (!sme_QosIsUapsdActive())
   {
      //                                         
      //                                      
      (void)pmcStopUapsd(pMac);
   }
   pSession->uapsdAlreadyRequested = VOS_FALSE;
   pSession->handoffRequested = VOS_FALSE;
   pSession->readyForPowerSave = VOS_TRUE;
   pSession->roamID = 0;
   //                             
   sme_QosDeleteBufferedRequests(pMac, sessionId);
   //                      
   sme_QosDeleteExistingFlows(pMac, sessionId);
   //                       
   if(pSession->assocInfo.pBssDesc)
   {
      vos_mem_free(pSession->assocInfo.pBssDesc);
      pSession->assocInfo.pBssDesc = NULL;
   }
   sme_QosCb.sessionInfo[sessionId].sessionActive = VOS_FALSE;
   return eHAL_STATUS_SUCCESS;
}
/*                                                                          
                                                            
                                                
                                                            
  
                    
  
     
  
                                                                            */
eHalStatus sme_QosProcessJoinReqEv(tpAniSirGlobal pMac, v_U8_t sessionId, void * pEvent_info)
{
   sme_QosSessionInfo *pSession;
   sme_QosEdcaAcType ac;
   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
             "%s: %d: invoked on session %d",
             __func__, __LINE__,
             sessionId);
   pSession = &sme_QosCb.sessionInfo[sessionId];
   if(pSession->handoffRequested)
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                "%s: %d: no need for state transition, should "
                "already be in handoff state",
                __func__, __LINE__);
      VOS_ASSERT(pSession->ac_info[0].curr_state == SME_QOS_HANDOFF);
      VOS_ASSERT(pSession->ac_info[1].curr_state == SME_QOS_HANDOFF);
      VOS_ASSERT(pSession->ac_info[2].curr_state == SME_QOS_HANDOFF);
      VOS_ASSERT(pSession->ac_info[3].curr_state == SME_QOS_HANDOFF);
      //                                                             
      sme_QosBufferExistingFlows(pMac, sessionId);
      //                                                
      sme_QosCleanupCtrlBlkForHandoff(pMac, sessionId);
      return eHAL_STATUS_SUCCESS;
   }

   for(ac = SME_QOS_EDCA_AC_BE; ac < SME_QOS_EDCA_AC_MAX; ac++) 
   {
      sme_QosStateTransition(sessionId, ac, SME_QOS_INIT);
   }
   //                                      
   if(pSession->assocInfo.pBssDesc)
   {
      vos_mem_free(pSession->assocInfo.pBssDesc);
      pSession->assocInfo.pBssDesc = NULL;
   }
   return eHAL_STATUS_SUCCESS;
}

#ifdef WLAN_FEATURE_VOWIFI_11R
/*                                                                          
                                                                    
                                                           

                                                            
  
                    
  
     
  
                                                                            */
eHalStatus sme_QosProcessPreauthSuccessInd(tpAniSirGlobal pMac, v_U8_t sessionId, void * pEvent_info)
{
    sme_QosSessionInfo *pSession;
    sme_QosACInfo *pACInfo;
    v_U8_t ac;
    eHalStatus  status = eHAL_STATUS_SUCCESS;

    VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH,
            "%s: %d: invoked on session %d",
            __func__, __LINE__,
            sessionId);

    pSession = &sme_QosCb.sessionInfo[sessionId];

    for(ac = SME_QOS_EDCA_AC_BE; ac < SME_QOS_EDCA_AC_MAX; ac++)
    {
        pACInfo = &pSession->ac_info[ac];

        switch(pACInfo->curr_state)
        {
            case SME_QOS_LINK_UP:
            case SME_QOS_REQUESTED:
            case SME_QOS_QOS_ON:
                sme_QosStateTransition(sessionId, ac, SME_QOS_HANDOFF);
                break;
            case SME_QOS_HANDOFF:
                //               
            case SME_QOS_CLOSED:
            case SME_QOS_INIT:
            default:
                VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR,
                        "%s: %d: On session %d AC %d is in wrong state %d",
                        __func__, __LINE__,
                        sessionId, ac, pACInfo->curr_state);
                //      
                VOS_ASSERT(0);
                break;
        }
    }

    pSession->ftHandoffInProgress = VOS_TRUE;

    //                                                        
    if (csrRoamIs11rAssoc(pMac)) 
    {
        v_U16_t ricOffset = 0;
        v_U32_t ricIELength = 0;
        v_U8_t  *ricIE;
        v_U8_t  tspec_mask_status = 0;
        v_U8_t  tspec_pending_status = 0;

        /*                                                                                           
                                                                                                       
                                         */
        ricIE = pMac->ft.ftSmeContext.psavedFTPreAuthRsp->ric_ies;
        ricOffset = pMac->ft.ftSmeContext.psavedFTPreAuthRsp->ric_ies_length;

        /*                                                                                       */
        for(ac = SME_QOS_EDCA_AC_BE; ac < SME_QOS_EDCA_AC_MAX; ac++)
        {
            volatile v_U8_t   tspec_index = 0;
            ricIELength = 0;
            pACInfo = &pSession->ac_info[ac];
            tspec_pending_status = pACInfo->tspec_pending;
            tspec_mask_status = pACInfo->tspec_mask_status;
            vos_mem_zero(pACInfo->ricIdentifier, SME_QOS_TSPEC_INDEX_MAX);
            VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO, 
                    FL("AC %d ==> TSPEC status = %d, tspec pending = %d"), 
                    ac, tspec_mask_status, tspec_pending_status);

            do
            {
                if (tspec_mask_status & 0x1)
                {
                    /*                                                                                            
                                           */
                    if (tspec_pending_status & 0x1)
                    {
                        status = sme_QosCreateTspecRICIE(pMac, &pACInfo->requested_QoSInfo[tspec_index],
                                ricIE + ricOffset, &ricIELength, &pACInfo->ricIdentifier[tspec_index]);
                    }
                    else
                    {
                        status = sme_QosCreateTspecRICIE(pMac, &pACInfo->curr_QoSInfo[tspec_index],
                                ricIE + ricOffset, &ricIELength, &pACInfo->ricIdentifier[tspec_index]);
                    }
                }
                ricOffset += ricIELength;
                pMac->ft.ftSmeContext.psavedFTPreAuthRsp->ric_ies_length += ricIELength;

                tspec_mask_status >>= 1;
                tspec_pending_status >>= 1;
                tspec_index++;
            } while (tspec_mask_status);
        }
    }
    return status;
}

#endif


/*                                                                          
                                                                  
                                              
  
                                                                                
             
  
                                                                
                                                                        
  
                    
  
     
  
                                                                            */
eHalStatus sme_QosProcessAddTsFailureRsp(tpAniSirGlobal pMac, 
                                         v_U8_t sessionId,
                                         tSirAddtsRspInfo * pRsp)
{
   sme_QosSessionInfo *pSession;
   sme_QosACInfo *pACInfo;
   sme_QosEdcaAcType ac;
   sme_QosSearchInfo search_key;
   v_U8_t tspec_pending;
   sme_QosWmmUpType up = (sme_QosWmmUpType)pRsp->tspec.tsinfo.traffic.userPrio;
   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
             "%s: %d: invoked on session %d for UP %d",
             __func__, __LINE__,
             sessionId, up);
   ac = sme_QosUpToAc(up);
   if(SME_QOS_EDCA_AC_MAX == ac)
   {
      //       
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                "%s: %d: invalid AC %d from UP %d",
                __func__, __LINE__, ac, up);
      return eHAL_STATUS_FAILURE;
   }
   pSession = &sme_QosCb.sessionInfo[sessionId];
   pACInfo = &pSession->ac_info[ac];
   //                                             
   tspec_pending = pACInfo->tspec_pending;
   if(!tspec_pending)
   {
      //      
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                "%s: %d: On session %d an AddTS is not pending on AC %d",
                __func__, __LINE__,
                sessionId, ac);
      VOS_ASSERT(0);
      return eHAL_STATUS_FAILURE;
   }

   vos_mem_zero(&search_key, sizeof(sme_QosSearchInfo));
   //                                                          
   search_key.key.ac_type = ac;
   search_key.index = SME_QOS_SEARCH_KEY_INDEX_2;
   search_key.sessionId = sessionId;
   if(!HAL_STATUS_SUCCESS(sme_QosFindAllInFlowList(pMac, search_key, sme_QosAddTsFailureFnp)))
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                "%s: %d: On session %d no match found for ac = %d",
                __func__, __LINE__,
                sessionId, search_key.key.ac_type);
      //      
      VOS_ASSERT(0);
      return eHAL_STATUS_FAILURE;
   }
   vos_mem_zero(&pACInfo->requested_QoSInfo[tspec_pending - 1], 
                sizeof(sme_QosWmmTspecInfo));

   if((!pACInfo->num_flows[0])&&
      (!pACInfo->num_flows[1]))
   {
      pACInfo->tspec_mask_status &= SME_QOS_TSPEC_MASK_BIT_1_2_SET & 
         (~pACInfo->tspec_pending);
      sme_QosStateTransition(sessionId, ac, SME_QOS_LINK_UP);
   }
   else
   {
      sme_QosStateTransition(sessionId, ac, SME_QOS_QOS_ON);
   }
   pACInfo->tspec_pending = 0;

   (void)sme_QosProcessBufferedCmd(sessionId);

   return eHAL_STATUS_SUCCESS;
}

/*                                                                          
                                                                          
                                                                            
                                            
  
                                                                  
                                                                
                                                                        
                                                     
  
                    
  
     
  
                                                                            */
static eHalStatus sme_QosUpdateTspecMask(v_U8_t sessionId,
                                      sme_QosSearchInfo search_key,
                                      v_U8_t new_tspec_mask)
{
   tListElem *pEntry= NULL, *pNextEntry = NULL;
   sme_QosFlowInfoEntry *flow_info = NULL;
   sme_QosSessionInfo *pSession;
   sme_QosACInfo *pACInfo;

   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
             "%s: %d: invoked on session %d for AC %d TSPEC %d",
             __func__, __LINE__,
             sessionId, search_key.key.ac_type, new_tspec_mask);

   pSession = &sme_QosCb.sessionInfo[sessionId];
   
   if (search_key.key.ac_type < SME_QOS_EDCA_AC_MAX)
   {
   pACInfo = &pSession->ac_info[search_key.key.ac_type];
   }
   else
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                "%s: %d: Exceeded the array bounds of pSession->ac_info",
                __func__, __LINE__);
      VOS_ASSERT (0);
      return eHAL_STATUS_FAILURE;
   }

   pEntry = csrLLPeekHead( &sme_QosCb.flow_list, VOS_FALSE );
   if(!pEntry)
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                "%s: %d: Flow List empty, nothing to update",
                __func__, __LINE__);
      return eHAL_STATUS_FAILURE;
   }

   while( pEntry )
   {
      pNextEntry = csrLLNext( &sme_QosCb.flow_list, pEntry, VOS_FALSE );
      flow_info = GET_BASE_ADDR( pEntry, sme_QosFlowInfoEntry, link );

      if(search_key.sessionId == flow_info->sessionId)
      {
         if(search_key.index & SME_QOS_SEARCH_KEY_INDEX_4)
         {
            if((search_key.key.ac_type == flow_info->ac_type) &&
               (search_key.direction == flow_info->QoSInfo.ts_info.direction))
            {
               //   
               VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                         "%s: %d: Flow %d matches",
                         __func__, __LINE__,
                         flow_info->QosFlowID);
               pACInfo->num_flows[flow_info->tspec_mask - 1]--;
               pACInfo->num_flows[new_tspec_mask - 1]++;
               flow_info->tspec_mask = new_tspec_mask;
            }
         }
         else if(search_key.index & SME_QOS_SEARCH_KEY_INDEX_5)
         {
            if((search_key.key.ac_type == flow_info->ac_type) &&
               (search_key.tspec_mask == flow_info->tspec_mask))
            {
               //   
               VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                         "%s: %d: Flow %d matches",
                         __func__, __LINE__,
                         flow_info->QosFlowID);
               pACInfo->num_flows[flow_info->tspec_mask - 1]--;
               pACInfo->num_flows[new_tspec_mask - 1]++;
               flow_info->tspec_mask = new_tspec_mask;
            }
         }
      }

      pEntry = pNextEntry;
   }

   return eHAL_STATUS_SUCCESS;
}
/*                                                                          
                                                                  
                                              
  
                                                                            
                                                              
  
  
                                                                
                                                                        
  
                    
  
     
  
                                                                            */
eHalStatus sme_QosProcessAddTsSuccessRsp(tpAniSirGlobal pMac, 
                                         v_U8_t sessionId,
                                         tSirAddtsRspInfo * pRsp)
{
   sme_QosSessionInfo *pSession;
   sme_QosACInfo *pACInfo;
   sme_QosEdcaAcType ac, ac_index;
   sme_QosSearchInfo search_key;
   sme_QosSearchInfo search_key1;
   v_U8_t tspec_pending;
   tListElem *pEntry= NULL;
   sme_QosFlowInfoEntry *flow_info = NULL;
   sme_QosWmmUpType up = (sme_QosWmmUpType)pRsp->tspec.tsinfo.traffic.userPrio;
#ifdef FEATURE_WLAN_DIAG_SUPPORT
   WLAN_VOS_DIAG_EVENT_DEF(qos, vos_event_wlan_qos_payload_type);
   vos_log_qos_tspec_pkt_type *log_ptr = NULL;
#endif //                         
   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
             "%s: %d: invoked on session %d for UP %d",
             __func__, __LINE__,
             sessionId, up);
   pSession = &sme_QosCb.sessionInfo[sessionId];
   ac = sme_QosUpToAc(up);
   if(SME_QOS_EDCA_AC_MAX == ac)
   {
      //       
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                "%s: %d: invalid AC %d from UP %d",
                __func__, __LINE__, ac, up);
      return eHAL_STATUS_FAILURE;
   }
   pACInfo = &pSession->ac_info[ac];
   //                                             
   tspec_pending = pACInfo->tspec_pending;
   if(!tspec_pending)
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                "%s: %d: On session %d an AddTS is not pending on AC %d",
                __func__, __LINE__,
                sessionId, ac);
      return eHAL_STATUS_FAILURE;
   }
   //                                                                       
   //                                      
   if(pACInfo->requested_QoSInfo[tspec_pending - 1].ts_info.psb)
   {
      //                              
      pSession->apsdMask |= 1 << (SME_QOS_EDCA_AC_VO - ac);
   }
   else
   {
      if(((SME_QOS_TSPEC_MASK_BIT_1_2_SET & ~tspec_pending) > 0) &&
         ((SME_QOS_TSPEC_MASK_BIT_1_2_SET & ~tspec_pending) <= 
            SME_QOS_TSPEC_INDEX_MAX))
      {
      if(!pACInfo->requested_QoSInfo
         [(SME_QOS_TSPEC_MASK_BIT_1_2_SET & ~tspec_pending) - 1].ts_info.psb)
      {
         //                              
         pSession->apsdMask &= ~(1 << (SME_QOS_EDCA_AC_VO - ac));
      }
   }
      else
      {
         VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                   "%s: %d: Exceeded the array bounds of pACInfo->requested_QosInfo",
                   __func__, __LINE__);
         VOS_ASSERT (0);
         return eHAL_STATUS_FAILURE;
      }
   }

   pACInfo->curr_QoSInfo[tspec_pending - 1].ts_info.burst_size_defn =
                              pRsp->tspec.tsinfo.traffic.burstSizeDefn;
   pACInfo->curr_QoSInfo[tspec_pending - 1].ts_info.ack_policy =
                              pRsp->tspec.tsinfo.traffic.ackPolicy;
   pACInfo->curr_QoSInfo[tspec_pending - 1].ts_info.up =
                              pRsp->tspec.tsinfo.traffic.userPrio;
   pACInfo->curr_QoSInfo[tspec_pending - 1].ts_info.psb =
                                        pRsp->tspec.tsinfo.traffic.psb;
   pACInfo->curr_QoSInfo[tspec_pending - 1].ts_info.direction =
                                  pRsp->tspec.tsinfo.traffic.direction;
   pACInfo->curr_QoSInfo[tspec_pending - 1].ts_info.tid =
                                       pRsp->tspec.tsinfo.traffic.tsid;
   pACInfo->curr_QoSInfo[tspec_pending - 1].nominal_msdu_size =
                                       pRsp->tspec.nomMsduSz;
   pACInfo->curr_QoSInfo[tspec_pending - 1].maximum_msdu_size =
                                                 pRsp->tspec.maxMsduSz;
   pACInfo->curr_QoSInfo[tspec_pending - 1].min_service_interval =
                                            pRsp->tspec.minSvcInterval;
   pACInfo->curr_QoSInfo[tspec_pending - 1].max_service_interval =
                                            pRsp->tspec.maxSvcInterval;
   pACInfo->curr_QoSInfo[tspec_pending - 1].inactivity_interval =
                                             pRsp->tspec.inactInterval;
   pACInfo->curr_QoSInfo[tspec_pending - 1].suspension_interval =
                                           pRsp->tspec.suspendInterval;
   pACInfo->curr_QoSInfo[tspec_pending - 1].svc_start_time =
                                              pRsp->tspec.svcStartTime;
   pACInfo->curr_QoSInfo[tspec_pending - 1].min_data_rate =
                                              pRsp->tspec.minDataRate;
   pACInfo->curr_QoSInfo[tspec_pending - 1].mean_data_rate =
                                             pRsp->tspec.meanDataRate;
   pACInfo->curr_QoSInfo[tspec_pending - 1].peak_data_rate =
                                             pRsp->tspec.peakDataRate;
   pACInfo->curr_QoSInfo[tspec_pending - 1].max_burst_size =
                                               pRsp->tspec.maxBurstSz;
   pACInfo->curr_QoSInfo[tspec_pending - 1].delay_bound =
                                               pRsp->tspec.delayBound;

   pACInfo->curr_QoSInfo[tspec_pending - 1].min_phy_rate =
                                               pRsp->tspec.minPhyRate;
   pACInfo->curr_QoSInfo[tspec_pending - 1].surplus_bw_allowance =
                                                pRsp->tspec.surplusBw;
   pACInfo->curr_QoSInfo[tspec_pending - 1].medium_time =
                                               pRsp->tspec.mediumTime;

   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR,
             "%s: %d: On session %d AddTspec Medium Time %d",
             __func__, __LINE__,
             sessionId, pRsp->tspec.mediumTime);

   /*                                                                                   
                                                                   */
   if((pACInfo->curr_QoSInfo[pACInfo->tspec_pending - 1].ts_info.direction == SME_QOS_WMM_TS_DIR_BOTH) &&
      (pACInfo->num_flows[SME_QOS_TSPEC_INDEX_1] > 0))
   {
     vos_mem_zero(&search_key, sizeof(sme_QosSearchInfo));
     /*                                                                                                         */
     search_key.key.ac_type = ac;
     search_key.index = SME_QOS_SEARCH_KEY_INDEX_5;
     search_key.sessionId = sessionId;
     search_key.tspec_mask = SME_QOS_TSPEC_MASK_BIT_2_SET;
     sme_QosUpdateTspecMask(sessionId, search_key, SME_QOS_TSPEC_MASK_BIT_1_SET);
   }

   vos_mem_zero(&search_key1, sizeof(sme_QosSearchInfo));
   //                                                  
   search_key1.index = SME_QOS_SEARCH_KEY_INDEX_3;
   search_key1.key.reason = SME_QOS_REASON_SETUP;
   search_key1.sessionId = sessionId;
   for(ac_index = SME_QOS_EDCA_AC_BE; ac_index < SME_QOS_EDCA_AC_MAX; ac_index++)
   {
      pEntry = sme_QosFindInFlowList(search_key1);
      if(pEntry)
      {
         flow_info = GET_BASE_ADDR( pEntry, sme_QosFlowInfoEntry, link );
         if(flow_info->ac_type == ac)
         {
            pACInfo->hoRenewal = flow_info->hoRenewal;
            break;
         }
      }
   }
   vos_mem_zero(&search_key, sizeof(sme_QosSearchInfo));
   //                                                          
   search_key.key.ac_type = ac;
   search_key.index = SME_QOS_SEARCH_KEY_INDEX_2;
   search_key.sessionId = sessionId;
   //                                              
   //                                                                  
   if(!HAL_STATUS_SUCCESS(sme_QosFindAllInFlowList(pMac, search_key, sme_QosAddTsSuccessFnp)))
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                "%s: %d: On session %d no match found for ac %d",
                __func__, __LINE__,
                sessionId, search_key.key.ac_type);
      //      
      VOS_ASSERT(0);
      return eHAL_STATUS_FAILURE;
   }
   pACInfo->hoRenewal = VOS_FALSE;
   vos_mem_zero(&pACInfo->requested_QoSInfo[tspec_pending - 1], 
                sizeof(sme_QosWmmTspecInfo));
   //                     
#ifdef FEATURE_WLAN_DIAG_SUPPORT          
   qos.eventId = SME_QOS_DIAG_ADDTS_RSP;
   qos.reasonCode = SME_QOS_DIAG_ADDTS_ADMISSION_ACCEPTED;
   WLAN_VOS_DIAG_EVENT_REPORT(&qos, EVENT_WLAN_QOS);
   WLAN_VOS_DIAG_LOG_ALLOC(log_ptr, vos_log_qos_tspec_pkt_type, LOG_WLAN_QOS_TSPEC_C);
   if(log_ptr)
   {
      log_ptr->delay_bound = pACInfo->curr_QoSInfo[tspec_pending - 1].delay_bound;
      log_ptr->inactivity_interval = pACInfo->curr_QoSInfo[tspec_pending - 1].inactivity_interval;
      log_ptr->max_burst_size = pACInfo->curr_QoSInfo[tspec_pending - 1].max_burst_size;
      log_ptr->max_service_interval = pACInfo->curr_QoSInfo[tspec_pending - 1].max_service_interval;
      log_ptr->maximum_msdu_size = pACInfo->curr_QoSInfo[tspec_pending - 1].maximum_msdu_size;
      log_ptr->mean_data_rate = pACInfo->curr_QoSInfo[tspec_pending - 1].mean_data_rate;
      log_ptr->medium_time = pACInfo->curr_QoSInfo[tspec_pending - 1].medium_time;
      log_ptr->min_data_rate = pACInfo->curr_QoSInfo[tspec_pending - 1].min_data_rate;
      log_ptr->min_phy_rate = pACInfo->curr_QoSInfo[tspec_pending - 1].min_phy_rate;
      log_ptr->min_service_interval = pACInfo->curr_QoSInfo[tspec_pending - 1].min_service_interval;
      log_ptr->nominal_msdu_size = pACInfo->curr_QoSInfo[tspec_pending - 1].nominal_msdu_size;
      log_ptr->peak_data_rate = pACInfo->curr_QoSInfo[tspec_pending - 1].peak_data_rate;
      log_ptr->surplus_bw_allowance = pACInfo->curr_QoSInfo[tspec_pending - 1].surplus_bw_allowance;
      log_ptr->suspension_interval = pACInfo->curr_QoSInfo[tspec_pending - 1].surplus_bw_allowance;
      log_ptr->suspension_interval = pACInfo->curr_QoSInfo[tspec_pending - 1].suspension_interval;
      log_ptr->svc_start_time = pACInfo->curr_QoSInfo[tspec_pending - 1].svc_start_time;
      log_ptr->tsinfo[0] = pACInfo->curr_QoSInfo[tspec_pending - 1].ts_info.direction << 5 |
         pACInfo->curr_QoSInfo[tspec_pending - 1].ts_info.tid << 1;
      log_ptr->tsinfo[1] = pACInfo->curr_QoSInfo[tspec_pending - 1].ts_info.up << 11 |
         pACInfo->curr_QoSInfo[tspec_pending - 1].ts_info.psb << 10;
      log_ptr->tsinfo[2] = 0;
   }
   WLAN_VOS_DIAG_LOG_REPORT(log_ptr);
#endif //                         
#if defined(FEATURE_WLAN_CCX) && !defined(FEATURE_WLAN_CCX_UPLOAD)
   if (ac == SME_QOS_EDCA_AC_VO)
   {
      //                                                       
      //                          
      csrNeighborRoamIndicateVoiceBW( pMac, pACInfo->curr_QoSInfo[tspec_pending - 1].peak_data_rate, TRUE );
   }
#endif
   pACInfo->tspec_pending = 0;

   sme_QosStateTransition(sessionId, ac, SME_QOS_QOS_ON);


   (void)sme_QosProcessBufferedCmd(sessionId);
   return eHAL_STATUS_SUCCESS;
   
}
/*                                                                          
                                                                              
                                                                                
  
                                                                               
                                                                       
                                                                             
                                    

                    
  
     
  
                                                                            */
eHalStatus sme_QosAggregateParams(
   sme_QosWmmTspecInfo * pInput_Tspec_Info,
   sme_QosWmmTspecInfo * pCurrent_Tspec_Info,
   sme_QosWmmTspecInfo * pUpdated_Tspec_Info)
{
   sme_QosWmmTspecInfo TspecInfo;
   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
             "%s: %d: invoked",
             __func__, __LINE__);
   if(!pInput_Tspec_Info)
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                "%s: %d: input is NULL, nothing to aggregate",
                __func__, __LINE__);
      return eHAL_STATUS_FAILURE;
   }
   if(!pCurrent_Tspec_Info)
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                "%s: %d: Current is NULL, can't aggregate",
                __func__, __LINE__);
      return eHAL_STATUS_FAILURE;
   }
   vos_mem_copy(&TspecInfo, pCurrent_Tspec_Info, 
                sizeof(sme_QosWmmTspecInfo));
   TspecInfo.ts_info.psb = pInput_Tspec_Info->ts_info.psb;
   /*                                                                         
                                                                          
                                                                            */
   if(pCurrent_Tspec_Info->min_service_interval &&
      pInput_Tspec_Info->min_service_interval &&
      (pCurrent_Tspec_Info->ts_info.direction !=
      pInput_Tspec_Info->ts_info.direction))
   {
      TspecInfo.min_service_interval = VOS_MIN(
         pCurrent_Tspec_Info->min_service_interval,
         pInput_Tspec_Info->min_service_interval);
   }
   else if(pInput_Tspec_Info->min_service_interval)
   {
      TspecInfo.min_service_interval = pInput_Tspec_Info->min_service_interval;
   }
   if(pCurrent_Tspec_Info->max_service_interval &&
      pInput_Tspec_Info->max_service_interval &&
      (pCurrent_Tspec_Info->ts_info.direction !=
      pInput_Tspec_Info->ts_info.direction))
   {
      TspecInfo.max_service_interval = VOS_MIN(
         pCurrent_Tspec_Info->max_service_interval,
         pInput_Tspec_Info->max_service_interval);
   }
   else
   {
      TspecInfo.max_service_interval = pInput_Tspec_Info->max_service_interval;
   }
   /*                                                                         
                                                                      
             
                                                                            */
   if(pCurrent_Tspec_Info->ts_info.direction != 
      pInput_Tspec_Info->ts_info.direction)
   {
      TspecInfo.ts_info.direction = SME_QOS_WMM_TS_DIR_BOTH;
   }
   /*                                                                         
                                            
                                                                            */
   TspecInfo.maximum_msdu_size = VOS_MAX(pCurrent_Tspec_Info->maximum_msdu_size,
                                         pInput_Tspec_Info->maximum_msdu_size);

   /*                                                                         
                                                  
                                                                            */
   TspecInfo.inactivity_interval = VOS_MAX(pCurrent_Tspec_Info->inactivity_interval,
                                         pInput_Tspec_Info->inactivity_interval);

   /*                                                                         
                                    
                                                                          
                                                                            */
   if(pCurrent_Tspec_Info->delay_bound)
   {
      TspecInfo.delay_bound = VOS_MIN(pCurrent_Tspec_Info->delay_bound,
                                      pInput_Tspec_Info->delay_bound);
   }
   else
   {
      TspecInfo.delay_bound = pInput_Tspec_Info->delay_bound;
   }
   TspecInfo.max_burst_size = VOS_MAX(pCurrent_Tspec_Info->max_burst_size,
                                      pInput_Tspec_Info->max_burst_size);

   /*                                                                         
                                                                             
                
                                                                              
                      
                                           
                                                                            */
   TspecInfo.nominal_msdu_size = VOS_MAX(
      pCurrent_Tspec_Info->nominal_msdu_size & ~SME_QOS_16BIT_MSB,
      pInput_Tspec_Info->nominal_msdu_size & ~SME_QOS_16BIT_MSB);

   if( ((pCurrent_Tspec_Info->nominal_msdu_size == 0) ||
        (pCurrent_Tspec_Info->nominal_msdu_size & SME_QOS_16BIT_MSB)) &&
       ((pInput_Tspec_Info->nominal_msdu_size == 0) ||
        (pInput_Tspec_Info->nominal_msdu_size & SME_QOS_16BIT_MSB)))
   {
     TspecInfo.nominal_msdu_size |= SME_QOS_16BIT_MSB;
   }

   /*                                                                         
                 
                                     
                                                                            */
   SME_QOS_BOUNDED_U32_ADD_Y_TO_X( TspecInfo.peak_data_rate,
                                   pInput_Tspec_Info->peak_data_rate );
   SME_QOS_BOUNDED_U32_ADD_Y_TO_X( TspecInfo.min_data_rate,
                                   pInput_Tspec_Info->min_data_rate );
   /*                                                                    */
   SME_QOS_BOUNDED_U32_ADD_Y_TO_X( TspecInfo.mean_data_rate,
                                   pInput_Tspec_Info->mean_data_rate );

   /*                                                                         
                                                                           
                                                         
                                                                          
                      
                                                                        
                                                                         
               
                                                                            */
   TspecInfo.suspension_interval = 0;
   /*                                                                         
                                                                    
                           
                               
                                                                            */
   TspecInfo.medium_time = 0;               /*                              */
   TspecInfo.min_phy_rate = SME_QOS_MIN_PHY_RATE;
   TspecInfo.svc_start_time = 0;           /*                            */
   TspecInfo.surplus_bw_allowance += pInput_Tspec_Info->surplus_bw_allowance;
   if(TspecInfo.surplus_bw_allowance > SME_QOS_SURPLUS_BW_ALLOWANCE)
   {
      TspecInfo.surplus_bw_allowance = SME_QOS_SURPLUS_BW_ALLOWANCE;
   }
   /*                                                                          */
   if((pInput_Tspec_Info->ts_info.ack_policy == SME_QOS_WMM_TS_ACK_POLICY_HT_IMMEDIATE_BLOCK_ACK) ||
      (pCurrent_Tspec_Info->ts_info.ack_policy == SME_QOS_WMM_TS_ACK_POLICY_HT_IMMEDIATE_BLOCK_ACK))
   {
     TspecInfo.ts_info.ack_policy = SME_QOS_WMM_TS_ACK_POLICY_HT_IMMEDIATE_BLOCK_ACK;
   }

   if(pInput_Tspec_Info->ts_info.burst_size_defn || pCurrent_Tspec_Info->ts_info.burst_size_defn )
   {
     TspecInfo.ts_info.burst_size_defn = 1;
   }
   if(pUpdated_Tspec_Info)
   {
      vos_mem_copy(pUpdated_Tspec_Info, &TspecInfo, 
                   sizeof(sme_QosWmmTspecInfo));
   }
   else
   {
      vos_mem_copy(pCurrent_Tspec_Info, &TspecInfo, 
                   sizeof(sme_QosWmmTspecInfo));
   }
   return eHAL_STATUS_SUCCESS;
}
/*                                                                          
                                                                       
                                                                          
                
  
                                                                  
                                                                
                                                                    
  
                    
  
     
  
                                                                            */
static eHalStatus sme_QosUpdateParams(v_U8_t sessionId,
                                      sme_QosEdcaAcType ac,
                                      v_U8_t tspec_mask, 
                                      sme_QosWmmTspecInfo * pTspec_Info)
{
   tListElem *pEntry= NULL, *pNextEntry = NULL;
   sme_QosSessionInfo *pSession;
   sme_QosACInfo *pACInfo;
   sme_QosFlowInfoEntry *flow_info = NULL;
   sme_QosWmmTspecInfo Tspec_Info;
   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
             "%s: %d: invoked on session %d for AC %d TSPEC %d",
             __func__, __LINE__,
             sessionId, ac, tspec_mask);
   if(!pTspec_Info)
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                "%s: %d: output is NULL, can't aggregate",
                __func__, __LINE__);
      return eHAL_STATUS_FAILURE;
   }
   vos_mem_zero(&Tspec_Info, sizeof(sme_QosWmmTspecInfo));
   pEntry = csrLLPeekHead( &sme_QosCb.flow_list, VOS_FALSE );
   if(!pEntry)
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                "%s: %d: Flow List empty, nothing to update",
                __func__, __LINE__);
      return eHAL_STATUS_FAILURE;
   }
   pSession = &sme_QosCb.sessionInfo[sessionId];
   pACInfo = &pSession->ac_info[ac];
   //                      
   Tspec_Info.ts_info.up  = pACInfo->curr_QoSInfo[tspec_mask - 1].ts_info.up;
   Tspec_Info.ts_info.psb = pACInfo->curr_QoSInfo[tspec_mask - 1].ts_info.psb;
   Tspec_Info.ts_info.tid = pACInfo->curr_QoSInfo[tspec_mask - 1].ts_info.tid;
   while( pEntry )
   {
      pNextEntry = csrLLNext( &sme_QosCb.flow_list, pEntry, VOS_FALSE );
      flow_info = GET_BASE_ADDR( pEntry, sme_QosFlowInfoEntry, link );
      if((sessionId == flow_info->sessionId) &&
         (ac == flow_info->ac_type) &&
         (tspec_mask == flow_info->tspec_mask))
      {
         VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                   "%s: %d: Flow %d matches",
                   __func__, __LINE__,
                   flow_info->QosFlowID);
         
         if((SME_QOS_REASON_RELEASE == flow_info->reason ) ||
            (SME_QOS_REASON_MODIFY == flow_info->reason))
         {
            //   
            VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                      "%s: %d: Skipping Flow %d as it is marked "
                      "for release/modify",
                      __func__, __LINE__,
                      flow_info->QosFlowID);
         }
         else if(!HAL_STATUS_SUCCESS(sme_QosAggregateParams(&flow_info->QoSInfo, 
                                                            &Tspec_Info,
                                                            NULL)))
         {
            //       
            VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                      "%s: %d: sme_QosAggregateParams() failed",
                      __func__, __LINE__);
         }
      }
      pEntry = pNextEntry;
   }
   //                     
   *pTspec_Info = Tspec_Info;
   return eHAL_STATUS_SUCCESS;
}
/*                                                                          
                                                               
                                                                           
                                 
                                                                
                          
  
     
  
                                                                            */
sme_QosWmmUpType sme_QosAcToUp(sme_QosEdcaAcType ac)
{
   sme_QosWmmUpType up = SME_QOS_WMM_UP_MAX;
   if(ac >= 0 && ac < SME_QOS_EDCA_AC_MAX)
   {
      up = sme_QosACtoUPMap[ac];
   }
   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_MED, 
             "%s: %d: ac = %d up = %d returned",
             __func__, __LINE__, ac, up);
   return up;
}
/*                                                                          
                                                               
                                                              
                            
  
     
  
                                                                            */
sme_QosEdcaAcType sme_QosUpToAc(sme_QosWmmUpType up)
{
   sme_QosEdcaAcType ac = SME_QOS_EDCA_AC_MAX;
   if(up >= 0 && up < SME_QOS_WMM_UP_MAX)
   {
      ac = sme_QosUPtoACMap[up];
   }
   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_MED, 
             "%s: %d: up = %d ac = %d returned",
             __func__, __LINE__, up, ac);
   return ac;
}
/*                                                                          
                                                                            
                               
                                                                    
                                                                
                                                
  
              
  
     
  
                                                                            */
static void sme_QosStateTransition(v_U8_t sessionId,
                                   sme_QosEdcaAcType ac,
                                   sme_QosStates new_state)
{
   sme_QosSessionInfo *pSession;
   sme_QosACInfo *pACInfo;
   pSession = &sme_QosCb.sessionInfo[sessionId];
   pACInfo = &pSession->ac_info[ac];
   pACInfo->prev_state = pACInfo->curr_state;
   pACInfo->curr_state = new_state;
   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH,
             "%s: %d: On session %d new state=%d, old state=%d, for AC=%d",
             __func__, __LINE__, 
             sessionId, pACInfo->curr_state, pACInfo->prev_state, ac );
}
/*                                                                          
                                                                              
                
                                                                               
                         
                                                                          
                 
                 
                                                   
  
     
  
                                                                            */
tListElem *sme_QosFindInFlowList(sme_QosSearchInfo search_key)
{
   tListElem *pEntry= NULL, *pNextEntry = NULL;
   sme_QosFlowInfoEntry *flow_info = NULL;
   pEntry = csrLLPeekHead( &sme_QosCb.flow_list, VOS_FALSE );
   if(!pEntry)
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                "%s: %d: Flow List empty, can't search",
                __func__, __LINE__);
      return NULL;
   }
   while( pEntry )
   {
      pNextEntry = csrLLNext( &sme_QosCb.flow_list, pEntry, VOS_FALSE );
      flow_info = GET_BASE_ADDR( pEntry, sme_QosFlowInfoEntry, link );
      if((search_key.sessionId == flow_info->sessionId) ||
         (search_key.sessionId == SME_QOS_SEARCH_SESSION_ID_ANY))
      {
         if(search_key.index & SME_QOS_SEARCH_KEY_INDEX_1)
         {
            if(search_key.key.QosFlowID == flow_info->QosFlowID)
            {
               //   
               VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                         "%s: %d: match found on flowID, ending search",
                         __func__, __LINE__);
               break;
            }
         }
         else if(search_key.index & SME_QOS_SEARCH_KEY_INDEX_2)
         {
            if(search_key.key.ac_type == flow_info->ac_type)
            {
               //   
               VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                         "%s: %d: match found on ac, ending search",
                         __func__, __LINE__);
               break;
            }
         }
         else if(search_key.index & SME_QOS_SEARCH_KEY_INDEX_3)
         {
            if(search_key.key.reason == flow_info->reason)
            {
               //   
               VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                         "%s: %d: match found on reason, ending search",
                         __func__, __LINE__);
               break;
            }
         }
         else if(search_key.index & SME_QOS_SEARCH_KEY_INDEX_4)
         {
            if((search_key.key.ac_type == flow_info->ac_type) && 
               (search_key.direction == flow_info->QoSInfo.ts_info.direction))
            {
               //   
               VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                         "%s: %d: match found on reason, ending search",
                         __func__, __LINE__);

               break;
            }
         }
      }
      pEntry = pNextEntry;
   }
   return pEntry;
}
/*                                                                          
                                                                             
                                 
                                                                               
                         
                                                                          
                 
                 
                                                                              
                    
  
     
  
                                                                            */
eHalStatus sme_QosFindAllInFlowList(tpAniSirGlobal pMac,
                                    sme_QosSearchInfo search_key, 
                                    sme_QosProcessSearchEntry fnp)
{
   tListElem *pEntry= NULL, *pNextEntry = NULL;
   sme_QosSessionInfo *pSession;
   sme_QosFlowInfoEntry *flow_info = NULL;
   eHalStatus status = eHAL_STATUS_FAILURE;
   pEntry = csrLLPeekHead( &sme_QosCb.flow_list, VOS_FALSE );
   if(!pEntry)
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                "%s: %d: Flow List empty, can't search",
                __func__, __LINE__);
      return eHAL_STATUS_FAILURE;
   }
   while( pEntry )
   {
      pNextEntry = csrLLNext( &sme_QosCb.flow_list, pEntry, VOS_FALSE );
      flow_info = GET_BASE_ADDR( pEntry, sme_QosFlowInfoEntry, link );
      pSession = &sme_QosCb.sessionInfo[flow_info->sessionId];
      if((search_key.sessionId == flow_info->sessionId) ||
         (search_key.sessionId == SME_QOS_SEARCH_SESSION_ID_ANY))
      {
         if(search_key.index & SME_QOS_SEARCH_KEY_INDEX_1)
         {
            if(search_key.key.QosFlowID == flow_info->QosFlowID)
            {
               //   
               VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                         "%s: %d: match found on flowID, ending search",
                         __func__, __LINE__);
               status = fnp(pMac, pEntry);
               if(eHAL_STATUS_FAILURE == status)
               {
                  VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                            "%s: %d: Failed to process entry",
                            __func__, __LINE__);
                  break;
               }
            }
         }
         else if(search_key.index & SME_QOS_SEARCH_KEY_INDEX_2)
         {
            if(search_key.key.ac_type == flow_info->ac_type)
            {
               //   
               VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                         "%s: %d: match found on ac, ending search",
                         __func__, __LINE__);
               flow_info->hoRenewal = pSession->ac_info[flow_info->ac_type].hoRenewal;
               status = fnp(pMac, pEntry);
               if(eHAL_STATUS_FAILURE == status)
               {
                  VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                            "%s: %d: Failed to process entry",
                            __func__, __LINE__);
                  break;
               }
            }
         }
      }
      pEntry = pNextEntry;
   }
   return status;
}
/*                                                                          
                                                                      
                             
                                                                
  
                                                       
  
     
  
                                                                            */
v_BOOL_t sme_QosIsACM(tpAniSirGlobal pMac, tSirBssDescription *pSirBssDesc, 
                      sme_QosEdcaAcType ac, tDot11fBeaconIEs *pIes)
{
   v_BOOL_t ret_val = VOS_FALSE;
   tDot11fBeaconIEs *pIesLocal;
   if(!pSirBssDesc)
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                "%s: %d: pSirBssDesc is NULL",
                __func__, __LINE__);
      return VOS_FALSE;
   }

   if (NULL != pIes)
   {
      /*                                       */
      pIesLocal = pIes;
   }
   else
   {
      /*                                               */
      if (!HAL_STATUS_SUCCESS(csrGetParsedBssDescriptionIEs(pMac, pSirBssDesc, &pIesLocal)))
      {
         //       
         VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                   "%s: %d: csrGetParsedBssDescriptionIEs() failed",
                   __func__, __LINE__);
         return VOS_FALSE;
      }

      /*                                         */
   }

   if(CSR_IS_QOS_BSS(pIesLocal))
   {
       switch(ac)
       {
          case SME_QOS_EDCA_AC_BE:
             if(pIesLocal->WMMParams.acbe_acm) ret_val = VOS_TRUE;
             break;
          case SME_QOS_EDCA_AC_BK:
             if(pIesLocal->WMMParams.acbk_acm) ret_val = VOS_TRUE;
             break;
          case SME_QOS_EDCA_AC_VI:
             if(pIesLocal->WMMParams.acvi_acm) ret_val = VOS_TRUE;
             break;
          case SME_QOS_EDCA_AC_VO:
             if(pIesLocal->WMMParams.acvo_acm) ret_val = VOS_TRUE;
             break;
          default:
             VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                       "%s: %d: unknown AC = %d",
                       __func__, __LINE__, ac);
             //      
             VOS_ASSERT(0);
             break;
       }
   }//          
   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
             "%s: %d: ACM = %d for AC = %d",
             __func__, __LINE__, ret_val, ac );
   if (NULL == pIes)
   {
      /*                                         */
      vos_mem_free(pIesLocal);
   }
   return ret_val;
}
/*                                                                          
                                                                               
                                                                      

                    
  
     
  
                                                                            */
static eHalStatus sme_QosBufferExistingFlows(tpAniSirGlobal pMac,
                                             v_U8_t sessionId)
{
   tListElem *pEntry= NULL, *pNextEntry = NULL;
   sme_QosSessionInfo *pSession;
   sme_QosFlowInfoEntry *flow_info = NULL;
   sme_QosCmdInfo  cmd;
   pEntry = csrLLPeekHead( &sme_QosCb.flow_list, VOS_FALSE );
   if(!pEntry)
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH,
                "%s: %d: Flow List empty, nothing to buffer",
                __func__, __LINE__);
      return eHAL_STATUS_FAILURE;
   }
   while( pEntry )
   {
      pNextEntry = csrLLNext( &sme_QosCb.flow_list, pEntry, VOS_FALSE );
      flow_info = GET_BASE_ADDR( pEntry, sme_QosFlowInfoEntry, link );
      if (flow_info->sessionId == sessionId)
      {
         if((SME_QOS_REASON_REQ_SUCCESS == flow_info->reason )||
            (SME_QOS_REASON_SETUP == flow_info->reason ))
         {
            cmd.command = SME_QOS_SETUP_REQ;
            cmd.pMac = pMac;
            cmd.sessionId = sessionId;
            cmd.u.setupCmdInfo.HDDcontext = flow_info->HDDcontext;
            cmd.u.setupCmdInfo.QoSInfo = flow_info->QoSInfo;
            cmd.u.setupCmdInfo.QoSCallback = flow_info->QoSCallback;
            cmd.u.setupCmdInfo.UPType = SME_QOS_WMM_UP_MAX;//                   
            cmd.u.setupCmdInfo.QosFlowID = flow_info->QosFlowID;
            if(SME_QOS_REASON_SETUP == flow_info->reason )
            {
               cmd.u.setupCmdInfo.hoRenewal = VOS_FALSE;
            }
            else
            {
               cmd.u.setupCmdInfo.hoRenewal = VOS_TRUE;//                                
            }
            if(!HAL_STATUS_SUCCESS(sme_QosBufferCmd(&cmd, VOS_TRUE)))
            {
               VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                         "%s: %d: couldn't buffer the setup request for "
                         "flow %d in handoff state",
                         __func__, __LINE__,
                         flow_info->QosFlowID);
            }
            else
            {
               VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                         "%s: %d: buffered a setup request for "
                         "flow %d in handoff state",
                         __func__, __LINE__,
                         flow_info->QosFlowID);
            }
         }
         else if(SME_QOS_REASON_RELEASE == flow_info->reason ) 
         {
            cmd.command = SME_QOS_RELEASE_REQ;
            cmd.pMac = pMac;
            cmd.sessionId = sessionId;
            cmd.u.releaseCmdInfo.QosFlowID = flow_info->QosFlowID;
            if(!HAL_STATUS_SUCCESS(sme_QosBufferCmd(&cmd, VOS_TRUE)))
            {
               VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                         "%s: %d: couldn't buffer the release request for "
                         "flow %d in handoff state",
                         __func__, __LINE__,
                         flow_info->QosFlowID);
            }
            else
            {
               VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                         "%s: %d: buffered a release request for "
                         "flow %d in handoff state",
                         __func__, __LINE__,
                         flow_info->QosFlowID);
            }
         }
         else if(SME_QOS_REASON_MODIFY_PENDING == flow_info->reason)
         {
            cmd.command = SME_QOS_MODIFY_REQ;
            cmd.pMac = pMac;
            cmd.sessionId = sessionId;
            cmd.u.modifyCmdInfo.QosFlowID = flow_info->QosFlowID;
            cmd.u.modifyCmdInfo.QoSInfo = flow_info->QoSInfo;
            if(!HAL_STATUS_SUCCESS(sme_QosBufferCmd(&cmd, VOS_TRUE)))
            {
               VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                         "%s: %d: couldn't buffer the modify request for "
                         "flow %d in handoff state",
                         __func__, __LINE__,
                         flow_info->QosFlowID);
            }
            else
            {
               VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                         "%s: %d: buffered a modify request for "
                         "flow %d in handoff state",
                         __func__, __LINE__,
                         flow_info->QosFlowID);
            }
         }
         //                               
         VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                   "%s: %d: Deleting original entry at %p with flowID %d",
                   __func__, __LINE__,
                   flow_info, flow_info->QosFlowID);
         csrLLRemoveEntry(&sme_QosCb.flow_list, pEntry, VOS_TRUE );
         vos_mem_free(flow_info);
      }
      pEntry = pNextEntry;
   }
   pSession = &sme_QosCb.sessionInfo[sessionId];
   pSession->uapsdAlreadyRequested = VOS_FALSE;
   return eHAL_STATUS_SUCCESS;
}
/*                                                                          
                                                                               
                                              

                    
  
     
  
                                                                            */
static eHalStatus sme_QosDeleteExistingFlows(tpAniSirGlobal pMac,
                                             v_U8_t sessionId)
{
   tListElem *pEntry= NULL, *pNextEntry = NULL;
   sme_QosFlowInfoEntry *flow_info = NULL;
   pEntry = csrLLPeekHead( &sme_QosCb.flow_list, VOS_TRUE );
   if(!pEntry)
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_WARN,
                "%s: %d: Flow List empty, nothing to delete",
                __func__, __LINE__);
      return eHAL_STATUS_FAILURE;
   }
   while( pEntry )
   {
      pNextEntry = csrLLNext( &sme_QosCb.flow_list, pEntry, VOS_TRUE );
      flow_info = GET_BASE_ADDR( pEntry, sme_QosFlowInfoEntry, link );
      if (flow_info->sessionId == sessionId)
      {
         if((SME_QOS_REASON_REQ_SUCCESS == flow_info->reason )||
            (SME_QOS_REASON_SETUP == flow_info->reason )||
            (SME_QOS_REASON_RELEASE == flow_info->reason )||
            (SME_QOS_REASON_MODIFY == flow_info->reason ))
         {
            flow_info->QoSCallback(pMac, flow_info->HDDcontext, 
                                   NULL,
                                   SME_QOS_STATUS_RELEASE_QOS_LOST_IND,
                                   flow_info->QosFlowID);
         }
         VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH,
                   "%s: %d: Deleting entry at %p with flowID %d",
                   __func__, __LINE__,
                   flow_info, flow_info->QosFlowID);
         //                               
         csrLLRemoveEntry(&sme_QosCb.flow_list, pEntry, VOS_TRUE );
         vos_mem_free(flow_info);
      }
      pEntry = pNextEntry;
   }
   return eHAL_STATUS_SUCCESS;
}
/*                                                                          
                                                                                 
                                                                   
                                                                              
                             
                
                    
  
     
  
                                                                            */
eHalStatus sme_QosBufferCmd(sme_QosCmdInfo *pcmd, v_BOOL_t insert_head)
{
   sme_QosSessionInfo *pSession;
   sme_QosCmdInfoEntry * pentry = NULL;
   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
             "%s: %d: Invoked",
             __func__, __LINE__);
   pentry = (sme_QosCmdInfoEntry *) vos_mem_malloc(sizeof(sme_QosCmdInfoEntry));
   if (!pentry)
   {
      //       
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                "%s: %d: Memory allocation failure",
                __func__, __LINE__);
      return eHAL_STATUS_FAILURE;
   }
   //                        
   pentry->cmdInfo = *pcmd;
 
   pSession = &sme_QosCb.sessionInfo[pcmd->sessionId];
   if(insert_head) 
   {
      csrLLInsertHead(&pSession->bufferedCommandList, &pentry->link, VOS_TRUE);
   }
   else
   {
      csrLLInsertTail(&pSession->bufferedCommandList, &pentry->link, VOS_TRUE);
   }
   return eHAL_STATUS_SUCCESS;
}
/*                                                                          
                                                                              
                                                                

                    
  
     
  
                                                                            */
static eHalStatus sme_QosProcessBufferedCmd(v_U8_t sessionId)
{
   sme_QosSessionInfo *pSession;
   sme_QosCmdInfoEntry *pcmd = NULL;
   tListElem *pEntry= NULL;
   sme_QosStatusType hdd_status = SME_QOS_STATUS_SETUP_FAILURE_RSP;
   eHalStatus halStatus = eHAL_STATUS_SUCCESS;
   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
             "%s: %d: Invoked on session %d",
             __func__, __LINE__,
             sessionId);
   pSession = &sme_QosCb.sessionInfo[sessionId];
   if(!csrLLIsListEmpty( &pSession->bufferedCommandList, VOS_FALSE ))
   {
      pEntry = csrLLRemoveHead( &pSession->bufferedCommandList, VOS_TRUE );
      if(!pEntry)
      {
         //       
         VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                   "%s: %d: no more buffered commands on session %d",
                   __func__, __LINE__,
                   sessionId);
         pSession->readyForPowerSave = VOS_TRUE;
         return eHAL_STATUS_FAILURE;
      }
      pcmd = GET_BASE_ADDR( pEntry, sme_QosCmdInfoEntry, link );
      switch(pcmd->cmdInfo.command)
      {
      case SME_QOS_SETUP_REQ:
         hdd_status = sme_QosInternalSetupReq(pcmd->cmdInfo.pMac, 
                                              pcmd->cmdInfo.sessionId,
                                              &pcmd->cmdInfo.u.setupCmdInfo.QoSInfo,
                                              pcmd->cmdInfo.u.setupCmdInfo.QoSCallback, 
                                              pcmd->cmdInfo.u.setupCmdInfo.HDDcontext, 
                                              pcmd->cmdInfo.u.setupCmdInfo.UPType, 
                                              pcmd->cmdInfo.u.setupCmdInfo.QosFlowID, 
                                              VOS_TRUE,
                                              pcmd->cmdInfo.u.setupCmdInfo.hoRenewal);
         if(SME_QOS_STATUS_SETUP_FAILURE_RSP == hdd_status)
         {
            //       
            VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                      "%s: %d: sme_QosInternalSetupReq failed on session %d",
                      __func__, __LINE__,
                      sessionId);
            halStatus = eHAL_STATUS_FAILURE;
         }
         break;
      case SME_QOS_RELEASE_REQ:
         hdd_status = sme_QosInternalReleaseReq(pcmd->cmdInfo.pMac, 
                                                pcmd->cmdInfo.u.releaseCmdInfo.QosFlowID,
                                                VOS_TRUE);
         if(SME_QOS_STATUS_RELEASE_FAILURE_RSP == hdd_status)
         {
            //       
            VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                      "%s: %d: sme_QosInternalReleaseReq failed on session %d",
                      __func__, __LINE__,
                      sessionId);
            halStatus = eHAL_STATUS_FAILURE;
         }
         break;
      case SME_QOS_MODIFY_REQ:
         hdd_status = sme_QosInternalModifyReq(pcmd->cmdInfo.pMac, 
                                               &pcmd->cmdInfo.u.modifyCmdInfo.QoSInfo,
                                               pcmd->cmdInfo.u.modifyCmdInfo.QosFlowID,
                                               VOS_TRUE);
         if(SME_QOS_STATUS_MODIFY_SETUP_FAILURE_RSP == hdd_status)
         {
            //       
            VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                      "%s: %d: sme_QosInternalModifyReq failed on session %d",
                      __func__, __LINE__,
                      sessionId);
            halStatus = eHAL_STATUS_FAILURE;
         }
         break;
      case SME_QOS_RESEND_REQ:
         hdd_status = sme_QosReRequestAddTS(pcmd->cmdInfo.pMac, 
                                            pcmd->cmdInfo.sessionId,
                                            &pcmd->cmdInfo.u.resendCmdInfo.QoSInfo,
                                            pcmd->cmdInfo.u.resendCmdInfo.ac,
                                            pcmd->cmdInfo.u.resendCmdInfo.tspecMask);
         if(SME_QOS_STATUS_MODIFY_SETUP_FAILURE_RSP == hdd_status)
         {
            //       
            VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                      "%s: %d: sme_QosReRequestAddTS failed on session %d",
                      __func__, __LINE__,
                      sessionId);
            halStatus = eHAL_STATUS_FAILURE;
         }
         break;
      default:
         //       
         VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                   "%s: %d: On session %d unknown cmd = %d",
                   __func__, __LINE__,
                   sessionId, pcmd->cmdInfo.command);
         //      
         VOS_ASSERT(0);
         break;
      }
      //                                                        
      vos_mem_free(pcmd);
   }
   else
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                "%s: %d: cmd buffer empty",
                __func__, __LINE__);
      pSession->readyForPowerSave = VOS_TRUE;
   }
   return halStatus;
}
/*                                                                          
                                                                                  
                                                             

                    
  
     
  
                                                                            */
static eHalStatus sme_QosDeleteBufferedRequests(tpAniSirGlobal pMac,
                                                v_U8_t sessionId)
{
   sme_QosSessionInfo *pSession;
   sme_QosCmdInfoEntry *pcmd = NULL;
   tListElem *pEntry= NULL, *pNextEntry = NULL;
   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
             "%s: %d: Invoked on session %d",
             __func__, __LINE__, sessionId);
   pSession = &sme_QosCb.sessionInfo[sessionId];
   pEntry = csrLLPeekHead( &pSession->bufferedCommandList, VOS_TRUE );
   if(!pEntry)
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_WARN, 
                "%s: %d: Buffered List empty, nothing to delete on session %d",
                __func__, __LINE__,
                sessionId);
      return eHAL_STATUS_FAILURE;
   }
   while( pEntry )
   {
      pNextEntry = csrLLNext( &pSession->bufferedCommandList, pEntry, VOS_TRUE );
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO, 
                "%s: %d: deleting entry from buffered List",
                __func__, __LINE__);
      //                               
      csrLLRemoveEntry(&pSession->bufferedCommandList, pEntry, VOS_TRUE );
      //                   
      pcmd = GET_BASE_ADDR( pEntry, sme_QosCmdInfoEntry, link );
      vos_mem_free(pcmd);
      pEntry = pNextEntry;
   }
   return eHAL_STATUS_SUCCESS;
}
/*                                                                          
                                                                                
                                                                           
                                                                
                                                                                 
                                                                          
                                      
                       
                    
  
     
  
                                                                            */
eHalStatus sme_QosSaveAssocInfo(sme_QosSessionInfo *pSession, sme_QosAssocInfo *pAssoc_info)
{
   tSirBssDescription    *pBssDesc = NULL;
   v_U32_t                bssLen = 0;
   if(NULL == pAssoc_info)
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                "%s: %d: pAssoc_info is NULL",
                __func__, __LINE__);
      return eHAL_STATUS_FAILURE;
   }
   //                                      
   if(pSession->assocInfo.pBssDesc)
   {
      vos_mem_free(pSession->assocInfo.pBssDesc);
      pSession->assocInfo.pBssDesc = NULL;
   }
   bssLen = pAssoc_info->pBssDesc->length + 
      sizeof(pAssoc_info->pBssDesc->length);
   //                       
   pBssDesc = (tSirBssDescription *)vos_mem_malloc(bssLen);
   if (!pBssDesc)
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                "%s: %d: couldn't allocate memory for the bss Descriptor",
                __func__, __LINE__);
      return eHAL_STATUS_FAILURE;
   }
   vos_mem_copy(pBssDesc, pAssoc_info->pBssDesc, bssLen);
   pSession->assocInfo.pBssDesc = pBssDesc;
   //                             
   if(pAssoc_info->pProfile)
   {
       pSession->apsdMask |= pAssoc_info->pProfile->uapsd_mask;
   }
   //                                                    
   return eHAL_STATUS_SUCCESS;
}

/*                                                                          
                                                                                
                                                          
                                                              
                                                                                
  
                    
  
     
  
                                                                            */
eHalStatus sme_QosSetupFnp(tpAniSirGlobal pMac, tListElem *pEntry)
{
   sme_QosSessionInfo *pSession;
   sme_QosACInfo *pACInfo;
   sme_QosFlowInfoEntry *flow_info = NULL;
   sme_QosStatusType hdd_status = SME_QOS_STATUS_SETUP_MODIFIED_IND;
   sme_QosEdcaAcType ac;
   if(!pEntry)
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                "%s: %d: Entry is NULL",
                __func__, __LINE__);
      //      
      VOS_ASSERT(0);
      return eHAL_STATUS_FAILURE;
   }
   flow_info = GET_BASE_ADDR( pEntry, sme_QosFlowInfoEntry, link );
   ac = flow_info->ac_type;
   pSession = &sme_QosCb.sessionInfo[flow_info->sessionId];
   pACInfo = &pSession->ac_info[ac];
   if(SME_QOS_REASON_REQ_SUCCESS == flow_info->reason)
   {
      //                                                   
      flow_info->QoSCallback(pMac, flow_info->HDDcontext, 
                             &pACInfo->curr_QoSInfo[flow_info->tspec_mask - 1],
                             hdd_status,
                             flow_info->QosFlowID);
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                "%s: %d: Entry with flowID = %d getting notified",
                __func__, __LINE__,
                flow_info->QosFlowID);
   }
   return eHAL_STATUS_SUCCESS;
}
/*                                                                          
                                                                               
                                                                        
                                                              
                                                                                
  
                    
  
     
  
                                                                            */
eHalStatus sme_QosModificationNotifyFnp(tpAniSirGlobal pMac, tListElem *pEntry)
{
   sme_QosSessionInfo *pSession;
   sme_QosACInfo *pACInfo;
   sme_QosFlowInfoEntry *flow_info = NULL;
   sme_QosStatusType hdd_status = SME_QOS_STATUS_SETUP_MODIFIED_IND;
   sme_QosEdcaAcType ac;
   if(!pEntry)
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                "%s: %d: Entry is NULL",
                __func__, __LINE__);
      //      
      VOS_ASSERT(0);
      return eHAL_STATUS_FAILURE;
   }
   flow_info = GET_BASE_ADDR( pEntry, sme_QosFlowInfoEntry, link );
   ac = flow_info->ac_type;
   pSession = &sme_QosCb.sessionInfo[flow_info->sessionId];
   pACInfo = &pSession->ac_info[ac];
   if(SME_QOS_REASON_REQ_SUCCESS == flow_info->reason)
   {
      //                                                   
      flow_info->QoSCallback(pMac, flow_info->HDDcontext, 
                             &pACInfo->curr_QoSInfo[flow_info->tspec_mask - 1],
                             hdd_status,
                             flow_info->QosFlowID);
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                "%s: %d: Entry with flowID = %d getting notified",
                __func__, __LINE__,
                flow_info->QosFlowID);
   }
   return eHAL_STATUS_SUCCESS;
}
/*                                                                          
                                                                                
                                           
                                                              
                                                                                
  
                    
  
     
  
                                                                            */
eHalStatus sme_QosModifyFnp(tpAniSirGlobal pMac, tListElem *pEntry)
{
   sme_QosFlowInfoEntry *flow_info = NULL;
   if(!pEntry)
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                "%s: %d: Entry is NULL",
                __func__, __LINE__);
      VOS_ASSERT(0);
      return eHAL_STATUS_FAILURE;
   }
   flow_info = GET_BASE_ADDR( pEntry, sme_QosFlowInfoEntry, link );
   switch(flow_info->reason)
   {
   case SME_QOS_REASON_MODIFY_PENDING:
      //                                                                   
      flow_info->reason = SME_QOS_REASON_REQ_SUCCESS;
      break;
   case SME_QOS_REASON_MODIFY:
      //                                        
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                "%s: %d: Deleting original entry at %p with flowID %d",
                __func__, __LINE__,
                flow_info, flow_info->QosFlowID);
      csrLLRemoveEntry(&sme_QosCb.flow_list, pEntry, VOS_TRUE );
      //                   
      vos_mem_free(flow_info);
      break;
   default:
      break;
   }
   return eHAL_STATUS_SUCCESS;
}
/*                                                                          
                                                                                
                                                                                 
                           
                                                              
                                                                                
  
                    
  
     
  
                                                                            */
eHalStatus sme_QosDelTsIndFnp(tpAniSirGlobal pMac, tListElem *pEntry)
{
   sme_QosSessionInfo *pSession;
   sme_QosACInfo *pACInfo;
   sme_QosFlowInfoEntry *flow_info = NULL;
   sme_QosEdcaAcType ac;
   eHalStatus lock_status = eHAL_STATUS_FAILURE;
   sme_QosStatusType status;

   if(!pEntry)
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                "%s: %d: Entry is NULL",
                __func__, __LINE__);
      //      
      VOS_ASSERT(0);
      return eHAL_STATUS_FAILURE;
   }
   //                               
   flow_info = GET_BASE_ADDR( pEntry, sme_QosFlowInfoEntry, link );
   ac = flow_info->ac_type;
   pSession = &sme_QosCb.sessionInfo[flow_info->sessionId];
   pACInfo = &pSession->ac_info[ac];
   pACInfo->relTrig = SME_QOS_RELEASE_BY_AP;

   lock_status = sme_AcquireGlobalLock( &pMac->sme );
   if ( !HAL_STATUS_SUCCESS( lock_status ) )
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR,
                "%s: %d: Unable to obtain lock",
                __func__, __LINE__);
      return SME_QOS_STATUS_RELEASE_FAILURE_RSP;
   }
   //                                                                         
   status = sme_QosInternalReleaseReq(pMac, flow_info->QosFlowID, VOS_FALSE);
   sme_ReleaseGlobalLock( &pMac->sme );
   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
             "%s: %d: QoS Release return status on Flow %d is %d",
             __func__, __LINE__,
             flow_info->QosFlowID, status);

   return eHAL_STATUS_SUCCESS;
}
/*                                                                          
                                                                                 
                                                                                 
                                      
                                                              
                                                                                
  
                    
  
     
  
                                                                            */
eHalStatus sme_QosReassocSuccessEvFnp(tpAniSirGlobal pMac, tListElem *pEntry)
{
   sme_QosSessionInfo *pSession;
   sme_QosACInfo *pACInfo;
   sme_QosFlowInfoEntry *flow_info = NULL;
   v_BOOL_t delete_entry = VOS_FALSE;
   sme_QosStatusType hdd_status = SME_QOS_STATUS_SETUP_FAILURE_RSP;
   sme_QosEdcaAcType ac;
   eHalStatus pmc_status = eHAL_STATUS_FAILURE;
   if(!pEntry)
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                "%s: %d: Entry is NULL",
                __func__, __LINE__);
      //      
      VOS_ASSERT(0);
      return eHAL_STATUS_FAILURE;
   }
   flow_info = GET_BASE_ADDR(pEntry, sme_QosFlowInfoEntry, link);
   ac = flow_info->ac_type;
   pSession = &sme_QosCb.sessionInfo[flow_info->sessionId];
   pACInfo = &pSession->ac_info[ac];
   switch(flow_info->reason)
   {
   case SME_QOS_REASON_SETUP:
      hdd_status = SME_QOS_STATUS_SETUP_SUCCESS_IND;
      delete_entry = VOS_FALSE;
      flow_info->reason = SME_QOS_REASON_REQ_SUCCESS;
      //                                                                   
      //                                       
      if(pACInfo->requested_QoSInfo[SME_QOS_TSPEC_INDEX_0].ts_info.psb)
      {
         //                                                               
         //                                   
         if(!pSession->uapsdAlreadyRequested)
         {
            //                                                           
   
            pmc_status = pmcStartUapsd(pMac,
                                       sme_QosPmcStartUapsdCallback,
                                       pSession);
            //                                                                
            //                                                
         
            if(eHAL_STATUS_FAILURE == pmc_status)
            {
               hdd_status = SME_QOS_STATUS_SETUP_SUCCESS_IND_APSD_SET_FAILED;
               //                                  
               flow_info->hoRenewal = VOS_FALSE;
            }
            else if(eHAL_STATUS_PMC_PENDING == pmc_status)
            {
               //                                           
               pSession->uapsdAlreadyRequested = VOS_TRUE;
            }
            //                                            
         }
      }
      break;
   case SME_QOS_REASON_RELEASE:
      pACInfo->num_flows[SME_QOS_TSPEC_INDEX_0]--;
      //             
   case SME_QOS_REASON_MODIFY:
      delete_entry = VOS_TRUE;
      break;
   case SME_QOS_REASON_MODIFY_PENDING:
      hdd_status = SME_QOS_STATUS_MODIFY_SETUP_SUCCESS_IND;
      delete_entry = VOS_FALSE;
      flow_info->reason = SME_QOS_REASON_REQ_SUCCESS;
      if(pACInfo->requested_QoSInfo[SME_QOS_TSPEC_INDEX_0].ts_info.psb)
      {
   
         if(!pSession->uapsdAlreadyRequested)
         {
            //                                                           
            pmc_status = pmcStartUapsd(pMac,
                                       sme_QosPmcStartUapsdCallback,
                                       pSession);
         
            //                                                                
            //                                                
            if(eHAL_STATUS_FAILURE == pmc_status)
            {
               hdd_status = SME_QOS_STATUS_MODIFY_SETUP_SUCCESS_IND_APSD_SET_FAILED;
               //                                   
               flow_info->hoRenewal = VOS_FALSE;
            }
            else if(eHAL_STATUS_PMC_PENDING == pmc_status)
            {
               pSession->uapsdAlreadyRequested = VOS_TRUE;
            }
            //                                            
         }
      }
      break;
   case SME_QOS_REASON_REQ_SUCCESS:
      hdd_status = SME_QOS_STATUS_SETUP_MODIFIED_IND;
      //             
   default:
      delete_entry = VOS_FALSE;
      break;
   }
   if(!delete_entry)
   {
      if(!flow_info->hoRenewal)
      {
         flow_info->QoSCallback(pMac, flow_info->HDDcontext, 
                                &pACInfo->curr_QoSInfo[SME_QOS_TSPEC_INDEX_0],
                                hdd_status,
                                flow_info->QosFlowID);
      }
      else
      {
         flow_info->hoRenewal = VOS_FALSE;
      }
   }
   else
   {
      //                               
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                "%s: %d: Deleting entry at %p with flowID %d",
                __func__, __LINE__,
                flow_info, flow_info->QosFlowID);
      csrLLRemoveEntry(&sme_QosCb.flow_list, pEntry, VOS_TRUE );
      //                   
      vos_mem_free(flow_info);
   }
   
   return eHAL_STATUS_SUCCESS;
}
/*                                                                          
                                                                
                                                                                
                         
                                                                            
                                                                            
                                                                       
                                                                                 
      

                                                              
                                                                                
  
                    
  
     
  
                                                                            */
eHalStatus sme_QosAddTsFailureFnp(tpAniSirGlobal pMac, tListElem *pEntry)
{
   sme_QosSessionInfo *pSession;
   sme_QosACInfo *pACInfo;
   sme_QosFlowInfoEntry *flow_info = NULL;
   v_BOOL_t inform_hdd = VOS_FALSE;
   sme_QosStatusType hdd_status = SME_QOS_STATUS_SETUP_FAILURE_RSP;
   sme_QosEdcaAcType ac;
   if(!pEntry)
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                "%s: %d: Entry is NULL",
                __func__, __LINE__);
      //      
      VOS_ASSERT(0);
      return eHAL_STATUS_FAILURE;
   }
   flow_info = GET_BASE_ADDR( pEntry, sme_QosFlowInfoEntry, link );
   ac = flow_info->ac_type;
   pSession = &sme_QosCb.sessionInfo[flow_info->sessionId];
   pACInfo = &pSession->ac_info[ac];
   switch(flow_info->reason)
   {
   case SME_QOS_REASON_SETUP:
      hdd_status = SME_QOS_STATUS_SETUP_FAILURE_RSP;
      pACInfo->num_flows[pACInfo->tspec_pending - 1]--;
      inform_hdd = VOS_TRUE;
      break;
   case SME_QOS_REASON_RELEASE:
      hdd_status = SME_QOS_STATUS_RELEASE_FAILURE_RSP;
      pACInfo->num_flows[pACInfo->tspec_pending - 1]--;
      inform_hdd = VOS_TRUE;
      break;
   case SME_QOS_REASON_MODIFY_PENDING:
      hdd_status = SME_QOS_STATUS_MODIFY_SETUP_FAILURE_RSP;
      inform_hdd = VOS_TRUE;
      break;
   case SME_QOS_REASON_MODIFY:
      flow_info->reason = SME_QOS_REASON_REQ_SUCCESS;
   case SME_QOS_REASON_REQ_SUCCESS:
   default:
      inform_hdd = VOS_FALSE;
      break;
   }
   if(inform_hdd)
   {
      //                                                                        
      //       
      if(!flow_info->hoRenewal)
      {
         flow_info->QoSCallback(pMac, flow_info->HDDcontext, 
                                &pACInfo->curr_QoSInfo[pACInfo->tspec_pending - 1],
                                hdd_status,
                                flow_info->QosFlowID);
      }
      else
      {
         flow_info->QoSCallback(pMac, flow_info->HDDcontext, 
                                &pACInfo->curr_QoSInfo[pACInfo->tspec_pending - 1],
                                SME_QOS_STATUS_RELEASE_QOS_LOST_IND,
                                flow_info->QosFlowID);
      }
      //                               
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                "%s: %d: Deleting entry at %p with flowID %d",
                __func__, __LINE__,
                flow_info, flow_info->QosFlowID);
      csrLLRemoveEntry(&sme_QosCb.flow_list, pEntry, VOS_TRUE );
      //                   
      vos_mem_free(flow_info);
   }
   return eHAL_STATUS_SUCCESS;
}
/*                                                                          
                                                                
                                                                                
                                                                            
                     
                                                                            
                                                                            
                                                                       
                                                                                 
                                                                                
                                 
                                                              
                                                                                
  
                    
  
     
  
                                                                            */
eHalStatus sme_QosAddTsSuccessFnp(tpAniSirGlobal pMac, tListElem *pEntry)
{
   sme_QosSessionInfo *pSession;
   sme_QosACInfo *pACInfo;
   sme_QosFlowInfoEntry *flow_info = NULL;
   v_BOOL_t inform_hdd = VOS_FALSE;
   v_BOOL_t delete_entry = VOS_FALSE;
   sme_QosStatusType hdd_status = SME_QOS_STATUS_SETUP_FAILURE_RSP;
   sme_QosEdcaAcType ac;
   eHalStatus pmc_status = eHAL_STATUS_FAILURE;
   tCsrRoamModifyProfileFields modifyProfileFields;

   if(!pEntry)
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                "%s: %d: Entry is NULL",
                __func__, __LINE__);
      //      
      VOS_ASSERT(0);
      return eHAL_STATUS_FAILURE;
   }
   flow_info = GET_BASE_ADDR( pEntry, sme_QosFlowInfoEntry, link );
   ac = flow_info->ac_type;
   pSession = &sme_QosCb.sessionInfo[flow_info->sessionId];
   pACInfo = &pSession->ac_info[ac];
   if(flow_info->tspec_mask != pACInfo->tspec_pending)
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                "%s: %d: No need to notify the HDD, the ADDTS "
                "success is not for index = %d of the AC = %d",
                __func__, __LINE__,
                flow_info->tspec_mask, ac);
      return eHAL_STATUS_SUCCESS;
   }
   switch(flow_info->reason)
   {
   case SME_QOS_REASON_SETUP:
      hdd_status = SME_QOS_STATUS_SETUP_SUCCESS_IND;
      flow_info->reason = SME_QOS_REASON_REQ_SUCCESS;
      delete_entry = VOS_FALSE;
      inform_hdd = VOS_TRUE;
      //                                 
      if(pACInfo->requested_QoSInfo[pACInfo->tspec_pending - 1].ts_info.psb)
      {
         //                                                               
         //                                  
         if(!pSession->uapsdAlreadyRequested)
         {
            //                                                           
            pmc_status = pmcStartUapsd(pMac,
                                       sme_QosPmcStartUapsdCallback,
                                       pSession);
            //                                                                
            //                                                
            if(eHAL_STATUS_FAILURE == pmc_status)
            {
               hdd_status = SME_QOS_STATUS_SETUP_SUCCESS_IND_APSD_SET_FAILED;
               //                                   
               flow_info->hoRenewal = VOS_FALSE;
            }
            else if(eHAL_STATUS_PMC_PENDING == pmc_status)
            {
               //                                           
               pSession->uapsdAlreadyRequested = VOS_TRUE;
            }
            //                                            
         }
      }
      break;
   case SME_QOS_REASON_RELEASE:
      pACInfo->num_flows[pACInfo->tspec_pending - 1]--;
      hdd_status = SME_QOS_STATUS_RELEASE_SUCCESS_RSP;
      inform_hdd = VOS_TRUE;
      delete_entry = VOS_TRUE;
      break;
   case SME_QOS_REASON_MODIFY:
      delete_entry = VOS_TRUE;
      inform_hdd = VOS_FALSE;
      break;
   case SME_QOS_REASON_MODIFY_PENDING:
      hdd_status = SME_QOS_STATUS_MODIFY_SETUP_SUCCESS_IND;
      delete_entry = VOS_FALSE;
      flow_info->reason = SME_QOS_REASON_REQ_SUCCESS;
      inform_hdd = VOS_TRUE;
      //                                     
      if(pACInfo->requested_QoSInfo[pACInfo->tspec_pending - 1].ts_info.psb)
      {
         //                                                               
         //                                   
         if(!pSession->uapsdAlreadyRequested)
         {
            //                                                           
            pmc_status = pmcStartUapsd(pMac,
                                       sme_QosPmcStartUapsdCallback,
                                       pSession);
            //                                                                
            //                                                
            if(eHAL_STATUS_FAILURE == pmc_status)
            {
               hdd_status = SME_QOS_STATUS_MODIFY_SETUP_SUCCESS_IND_APSD_SET_FAILED;
               //                                   
               flow_info->hoRenewal = VOS_FALSE;
            }
            else if(eHAL_STATUS_PMC_PENDING == pmc_status)
            {
               //                                           
               pSession->uapsdAlreadyRequested = VOS_TRUE;
            }
            //                                            
         }
      }
      else
      {
        if((pACInfo->num_flows[flow_info->tspec_mask - 1] == 1) && 
           (SME_QOS_TSPEC_MASK_BIT_1_2_SET != pACInfo->tspec_mask_status))
        {
          //                                         
          //                                           
          pSession->apsdMask &= ~(1 << (SME_QOS_EDCA_AC_VO - ac));
          //                                                                 
          csrGetModifyProfileFields(pMac, flow_info->sessionId, &modifyProfileFields);
          modifyProfileFields.uapsd_mask = pSession->apsdMask; 
          csrSetModifyProfileFields(pMac, flow_info->sessionId, &modifyProfileFields);
          if(!pSession->apsdMask)
          {
             //                                   
             //                                     
             if (!sme_QosIsUapsdActive())
             {
                //                                         
                //                                      
                (void)pmcStopUapsd(pMac);
             }
          }
        }
      }
      break;
   case SME_QOS_REASON_REQ_SUCCESS:
      hdd_status = SME_QOS_STATUS_SETUP_MODIFIED_IND;
      inform_hdd = VOS_TRUE;
   default:
      delete_entry = VOS_FALSE;
      break;
   }
   if(inform_hdd)
   {
      if(!flow_info->hoRenewal)
      {
      
         flow_info->QoSCallback(pMac, flow_info->HDDcontext, 
                                &pACInfo->curr_QoSInfo[pACInfo->tspec_pending - 1],
                                hdd_status,
                                flow_info->QosFlowID);
      }
      else
      {
         flow_info->hoRenewal = VOS_FALSE;
      }
   }
   if(delete_entry)
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                "%s: %d: Deleting entry at %p with flowID %d",
                __func__, __LINE__,
                flow_info, flow_info->QosFlowID);
      //                               
      csrLLRemoveEntry(&sme_QosCb.flow_list, pEntry, VOS_TRUE );
      //                   
      vos_mem_free(flow_info);
   }

   return eHAL_STATUS_SUCCESS;
}
/*                                                                          
                                                                             
                                                                     
  
                                               
                                                                
  
                 
                                     
  
     
  
                                                                            */
static v_BOOL_t sme_QosIsRspPending(v_U8_t sessionId, sme_QosEdcaAcType ac)
{
   sme_QosSessionInfo *pSession;
   sme_QosACInfo *pACInfo;
   sme_QosEdcaAcType acIndex;
   v_BOOL_t status = VOS_FALSE;
   pSession = &sme_QosCb.sessionInfo[sessionId];
   for(acIndex = SME_QOS_EDCA_AC_BE; acIndex < SME_QOS_EDCA_AC_MAX; acIndex++) 
   {
      if(acIndex == ac)
      {
         continue;
      }
      pACInfo = &pSession->ac_info[acIndex];
      if((pACInfo->tspec_pending) || (pACInfo->reassoc_pending))
      {
         status = VOS_TRUE;
         break;
      }
   }
   return status;
}

/*                                                                          
                                                                        
                                    
                               
                                                                    

     

                                                                         */
void sme_QosUpdateHandOff(v_U8_t sessionId,
                          v_BOOL_t updateHandOff)
{
   sme_QosSessionInfo *pSession;
   pSession = &sme_QosCb.sessionInfo[sessionId];
   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_MED,
             "%s: %d: handoffRequested %d updateHandOff %d",
             __func__, __LINE__,pSession->handoffRequested,
             updateHandOff);

   pSession->handoffRequested = updateHandOff;

}

/*                                                                          
                                                                           
                                                   
                 
  
                                                                        
                                                               
  
     
  
                                                                            */
static v_BOOL_t sme_QosIsUapsdActive(void)
{
   sme_QosSessionInfo *pSession;
   v_U8_t sessionId;
   for (sessionId = 0; sessionId < CSR_ROAM_SESSION_MAX; ++sessionId)
   {
      pSession = &sme_QosCb.sessionInfo[sessionId];
      if ((pSession->sessionActive) && (pSession->apsdMask))
      {
         return VOS_TRUE;
      }
   }
   //                                      
   return VOS_FALSE;
}
/*                                                                          
                                                                               
                                                         
  
                                                                               
       
                                             
  
              
  
     
  
                                                                            */
void sme_QosPmcFullPowerCallback(void *callbackContext, eHalStatus status)
{
   sme_QosSessionInfo *pSession = callbackContext;
   if(HAL_STATUS_SUCCESS(status))
   {
      (void)sme_QosProcessBufferedCmd(pSession->sessionId);
   }
   else
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                "%s: %d: PMC failed to put the chip in Full power",
                __func__, __LINE__);
      //      
      VOS_ASSERT(0);
   }
}

/*                                                                          
                                                                                
                                                         
  
                                                                               
                                             
  
              
  
     
  
                                                                            */
void sme_QosPmcStartUapsdCallback(void *callbackContext, eHalStatus status)
{
   sme_QosSessionInfo *pSession = callbackContext;
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
   //                                                                
   //                                                             
   //                              
   //                                                         
   pSession->uapsdAlreadyRequested = VOS_FALSE;
}
/*                                                                          
                                                                               
                                                                 
                                                                            
                                                
  
                                                                                
                            
                 
  
                                                                         
                            
  
     
  
                                                                            */
v_BOOL_t sme_QosPmcCheckRoutine(void *callbackContext)
{
   sme_QosSessionInfo *pSession;
   v_U8_t sessionId;
   for (sessionId = 0; sessionId < CSR_ROAM_SESSION_MAX; ++sessionId)
   {
      pSession = &sme_QosCb.sessionInfo[sessionId];
      if ((pSession->sessionActive) &&
          (!pSession->readyForPowerSave))
      {
         return VOS_FALSE;
      }
   }
   //                                             
   return VOS_TRUE;
}
/*                                                                          
                                                                              
                                                       
  
                                                                         
                                          
                                                            
  
              
  
     
  
                                                                            */
void sme_QosPmcDeviceStateUpdateInd(void *callbackContext, tPmcState pmcState)
{
   eHalStatus status = eHAL_STATUS_FAILURE;
   tpAniSirGlobal pMac = PMAC_STRUCT( callbackContext );
   //                                                                            
   //                                                                        
   //                   
   switch(pmcState)
   {
   case FULL_POWER:
      status = sme_QosProcessOutOfUapsdMode(pMac);
      break;
   case UAPSD:
      status = sme_QosProcessIntoUapsdMode(pMac);
      break;
   default:
      status = eHAL_STATUS_SUCCESS;
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                "%s: %d: nothing to process in PMC state %d",
                __func__, __LINE__,
                pmcState);
   }
   if(!HAL_STATUS_SUCCESS(status))
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                "%s: %d: ignoring Device(PMC) state change to %d",
                __func__, __LINE__,
                pmcState);
   }

}
/*                                                                          
                                                                          
                                                                
  
                                                              
                    
  
     
  
                                                                            */
eHalStatus sme_QosProcessOutOfUapsdMode(tpAniSirGlobal pMac)
{
   sme_QosSessionInfo *pSession;
   tListElem *pEntry= NULL, *pNextEntry = NULL;
   sme_QosFlowInfoEntry *flow_info = NULL;
   
   pEntry = csrLLPeekHead( &sme_QosCb.flow_list, VOS_FALSE );
   if(!pEntry)
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, 
                "%s: %d: Flow List empty, can't search",
                __func__, __LINE__);
      return eHAL_STATUS_FAILURE;
   }
   while( pEntry )
   {
      pNextEntry = csrLLNext( &sme_QosCb.flow_list, pEntry, VOS_FALSE );
      flow_info = GET_BASE_ADDR( pEntry, sme_QosFlowInfoEntry, link );
      pSession = &sme_QosCb.sessionInfo[flow_info->sessionId];
      //                                                            
      if((flow_info->QoSInfo.max_service_interval ||
          flow_info->QoSInfo.min_service_interval) &&
         (SME_QOS_REASON_REQ_SUCCESS == flow_info->reason))
      {
         flow_info->QoSCallback(pMac, flow_info->HDDcontext, 
                                &pSession->ac_info[flow_info->ac_type].curr_QoSInfo[flow_info->tspec_mask - 1],
                                SME_QOS_STATUS_OUT_OF_APSD_POWER_MODE_IND,
                                flow_info->QosFlowID);
      }
      pEntry = pNextEntry;
   }
   return eHAL_STATUS_SUCCESS;
}
/*                                                                          
                                                                         
                                                     
  
                                                              
                    
  
     
  
                                                                            */
eHalStatus sme_QosProcessIntoUapsdMode(tpAniSirGlobal pMac)
{
   sme_QosSessionInfo *pSession;
   tListElem *pEntry= NULL, *pNextEntry = NULL;
   sme_QosFlowInfoEntry *flow_info = NULL;

   pEntry = csrLLPeekHead( &sme_QosCb.flow_list, VOS_FALSE );
   if(!pEntry)
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                "%s: %d: Flow List empty, can't search",
                __func__, __LINE__);
      return eHAL_STATUS_FAILURE;
   }
   while( pEntry )
   {
      pNextEntry = csrLLNext( &sme_QosCb.flow_list, pEntry, VOS_FALSE );
      flow_info = GET_BASE_ADDR( pEntry, sme_QosFlowInfoEntry, link );
      pSession = &sme_QosCb.sessionInfo[flow_info->sessionId];
      //                                                            
      if( (flow_info->QoSInfo.ts_info.psb) &&
         (SME_QOS_REASON_REQ_SUCCESS == flow_info->reason) )
      {
         flow_info->QoSCallback(pMac, flow_info->HDDcontext, 
                                &pSession->ac_info[flow_info->ac_type].curr_QoSInfo[flow_info->tspec_mask - 1],
                                SME_QOS_STATUS_INTO_APSD_POWER_MODE_IND,
                                flow_info->QosFlowID);
      }
      pEntry = pNextEntry;
   }
   return eHAL_STATUS_SUCCESS;
}

void sme_QosCleanupCtrlBlkForHandoff(tpAniSirGlobal pMac, v_U8_t sessionId)
{
   sme_QosSessionInfo *pSession;
   sme_QosACInfo *pACInfo;
   sme_QosEdcaAcType ac;
   pSession = &sme_QosCb.sessionInfo[sessionId];
   for(ac = SME_QOS_EDCA_AC_BE; ac < SME_QOS_EDCA_AC_MAX; ac++) 
   {
      pACInfo = &pSession->ac_info[ac];
      vos_mem_zero(pACInfo->curr_QoSInfo, 
                   sizeof(sme_QosWmmTspecInfo) * SME_QOS_TSPEC_INDEX_MAX);
      vos_mem_zero(pACInfo->requested_QoSInfo, 
                   sizeof(sme_QosWmmTspecInfo) * SME_QOS_TSPEC_INDEX_MAX);
      pACInfo->num_flows[0] = 0;
      pACInfo->num_flows[1] = 0;
      pACInfo->reassoc_pending = VOS_FALSE;
      pACInfo->tspec_mask_status = 0;
      pACInfo->tspec_pending = VOS_FALSE;
      pACInfo->hoRenewal = VOS_FALSE;
      pACInfo->prev_state = SME_QOS_LINK_UP;
   }
}

/*                                                                          
                                                                             
                                                                                       
  
                                               
                                                                               
                                                 
                                                           
  
                                                                                    
                                                                       
  
     
  
                                                                            */
v_BOOL_t sme_QosIsTSInfoAckPolicyValid(tpAniSirGlobal pMac,
    sme_QosWmmTspecInfo * pQoSInfo,
    v_U8_t sessionId)
{
  tDot11fBeaconIEs *pIes = NULL;
  sme_QosSessionInfo *pSession;
  eHalStatus hstatus;
  if( !CSR_IS_SESSION_VALID( pMac, sessionId ) )
  {
     VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
               "%s: %d: Session Id %d is invalid",
               __func__, __LINE__,
               sessionId);
     return VOS_FALSE;
  }

  pSession = &sme_QosCb.sessionInfo[sessionId];

  if( !pSession->sessionActive )
  {
     VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
               "%s: %d: Session %d is inactive",
               __func__, __LINE__,
               sessionId);
     return VOS_FALSE;
  }

  if(!pSession->assocInfo.pBssDesc)
  {
     VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
               "%s: %d: Session %d has an Invalid BSS Descriptor",
               __func__, __LINE__,
               sessionId);
     return VOS_FALSE;
  }

  hstatus = csrGetParsedBssDescriptionIEs(pMac,
                                          pSession->assocInfo.pBssDesc,
                                          &pIes);
  if(!HAL_STATUS_SUCCESS(hstatus))
  {
     VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR,
               "%s: %d: On session %d unable to parse BSS IEs",
               __func__, __LINE__,
               sessionId);
     return VOS_FALSE;
  }

  /*                                  */

  if(!pIes->HTCaps.present &&
     pQoSInfo->ts_info.ack_policy == SME_QOS_WMM_TS_ACK_POLICY_HT_IMMEDIATE_BLOCK_ACK)
  {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                "%s: %d: On session %d HT Caps aren't present but application set ack policy to HT ",
                __func__, __LINE__,
                sessionId);
      
      vos_mem_free(pIes);
      return VOS_FALSE;
  }

  vos_mem_free(pIes);
  return VOS_TRUE;
}

v_BOOL_t sme_QosValidateRequestedParams(tpAniSirGlobal pMac,
    sme_QosWmmTspecInfo * pQoSInfo,
    v_U8_t sessionId)
{
   v_BOOL_t rc = VOS_FALSE;

   do
   {
      if(SME_QOS_WMM_TS_DIR_RESV == pQoSInfo->ts_info.direction) break;
      if(!sme_QosIsTSInfoAckPolicyValid(pMac, pQoSInfo, sessionId)) break;

      rc = VOS_TRUE;
   }while(0);
   return rc;
}

static eHalStatus qosIssueCommand( tpAniSirGlobal pMac, v_U8_t sessionId,
                                   eSmeCommandType cmdType, sme_QosWmmTspecInfo * pQoSInfo,
                                   sme_QosEdcaAcType ac, v_U8_t tspec_mask )
{
    eHalStatus status = eHAL_STATUS_RESOURCES;
    tSmeCmd *pCommand = NULL;
    do
    {
        pCommand = smeGetCommandBuffer( pMac );
        if ( !pCommand )
        {
            VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR,
                         "%s: %d: fail to get command buffer for command %d",
                         __func__, __LINE__, cmdType);
            break;
        }
        pCommand->command = cmdType;
        pCommand->sessionId = sessionId;
        switch ( cmdType )
        {
        case eSmeCommandAddTs:
            if( pQoSInfo )
            {
                status = eHAL_STATUS_SUCCESS;
                pCommand->u.qosCmd.tspecInfo = *pQoSInfo;
                pCommand->u.qosCmd.ac = ac;
            }
            else
            {
               VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR,
                         "%s: %d: NULL pointer passed",
                         __func__, __LINE__);
               status = eHAL_STATUS_INVALID_PARAMETER;
            }
            break;
        case eSmeCommandDelTs:
            status = eHAL_STATUS_SUCCESS;
            pCommand->u.qosCmd.ac = ac;
            pCommand->u.qosCmd.tspec_mask = tspec_mask;
            break;
        default:
            VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR,
                      "%s: %d: invalid command type %d",
                      __func__, __LINE__, cmdType );
            status = eHAL_STATUS_INVALID_PARAMETER;
            break;
        }
    } while( 0 );
    if( HAL_STATUS_SUCCESS( status ) && pCommand )
    {
        smePushCommand( pMac, pCommand, eANI_BOOLEAN_FALSE );
    }
    else if( pCommand )
    {
        qosReleaseCommand( pMac, pCommand );
    }
    return( status );
}
tANI_BOOLEAN qosProcessCommand( tpAniSirGlobal pMac, tSmeCmd *pCommand )
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tANI_BOOLEAN fRemoveCmd = eANI_BOOLEAN_TRUE;
    do
    {
        switch ( pCommand->command )
        {
        case eSmeCommandAddTs:
            status = sme_QosAddTsReq( pMac, (v_U8_t)pCommand->sessionId, &pCommand->u.qosCmd.tspecInfo, pCommand->u.qosCmd.ac);
            if( HAL_STATUS_SUCCESS( status ) )
            {
                fRemoveCmd = eANI_BOOLEAN_FALSE;
                status = SME_QOS_STATUS_SETUP_REQ_PENDING_RSP;
            }
            break;
        case eSmeCommandDelTs:
            status = sme_QosDelTsReq( pMac, (v_U8_t)pCommand->sessionId, pCommand->u.qosCmd.ac, pCommand->u.qosCmd.tspec_mask );
            if( HAL_STATUS_SUCCESS( status ) )
            {
                fRemoveCmd = eANI_BOOLEAN_FALSE;
            }
            break;
        default:
            VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR,
                      "%s: %d: invalid command type %d",
                      __func__, __LINE__, pCommand->command );
            break;
        }//      
    } while(0);
    return( fRemoveCmd );
}

/*
                           
                                                         
                                                                       
                                                        
*/
sme_QosStatusType sme_QosTriggerUapsdChange( tpAniSirGlobal pMac )
{
   sme_QosSessionInfo *pSession;
   sme_QosACInfo *pACInfo;
   v_U8_t ac, tspec1 = 0, tspec2 = 0; 
   v_U8_t uapsd_mask;
   tDot11fBeaconIEs *pIesLocal;
   v_U8_t acm_mask;
   v_BOOL_t fIsUapsdNeeded;
   v_U8_t sessionId;
   v_BOOL_t addtsWhenACMNotSet = CSR_IS_ADDTS_WHEN_ACMOFF_SUPPORTED(pMac);
   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH,
             "%s: %d: Invoked",
             __func__, __LINE__);
   for (sessionId = 0; sessionId < CSR_ROAM_SESSION_MAX; ++sessionId)
   {
      pSession = &sme_QosCb.sessionInfo[sessionId];
      if( !pSession->sessionActive )
      {
         continue;
      }
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH,
                "%s: %d: Session %d is active",
                __func__, __LINE__,
                sessionId);
      if( HAL_STATUS_SUCCESS(csrGetParsedBssDescriptionIEs(pMac, pSession->assocInfo.pBssDesc, &pIesLocal)) )
      {
         //                 
         acm_mask = sme_QosGetACMMask(pMac, pSession->assocInfo.pBssDesc, pIesLocal);
         vos_mem_free(pIesLocal);
         //                                    
         uapsd_mask = pSession->apsdMask;
         //                                                      
         uapsd_mask &= ~acm_mask;
         //                                                                     
         for(ac = SME_QOS_EDCA_AC_BE; ac < SME_QOS_EDCA_AC_MAX; ac++)
         {
            pACInfo = &pSession->ac_info[ac];
            //                              
            if( SME_QOS_QOS_ON == pACInfo->curr_state )
            {
               //                              
               VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH,
                         "%s: %d: On session %d AC %d has QoS active",
                         __func__, __LINE__,
                         sessionId, ac);
               //                          
               if(( acm_mask & (1 << (SME_QOS_EDCA_AC_VO - ac)) ) || addtsWhenACMNotSet )
               {
                  //                                     
                  VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH,
                            "%s: %d: On session %d AC %d has ACM enabled",
                            __func__, __LINE__,
                            sessionId, ac);
                  //                       
                  if( pACInfo->tspec_mask_status )
                  {
                     //                                                        
                     if( SME_QOS_TSPEC_MASK_BIT_1_2_SET == pACInfo->tspec_mask_status )
                     {
                        //                      
                        tspec1 = SME_QOS_TSPEC_MASK_BIT_1_SET;
                        tspec2 = SME_QOS_TSPEC_MASK_BIT_2_SET;
                     }
                     else
                     {
                        //                                       
                        tspec1 = SME_QOS_TSPEC_MASK_BIT_1_2_SET & pACInfo->tspec_mask_status;
                     }
                     //                                   
                     fIsUapsdNeeded = (v_BOOL_t)(pACInfo->curr_QoSInfo[tspec1 - 1].ts_info.psb);
                     //                                           
                     if( fIsUapsdNeeded )
                     {
                        pACInfo->requested_QoSInfo[tspec1 - 1] = 
                           pACInfo->curr_QoSInfo[tspec1 - 1];
                        sme_QosReRequestAddTS( pMac, sessionId,
                                               &pACInfo->requested_QoSInfo[tspec1 - 1],
                                               ac,
                                               tspec1 );
                     }
                     //                   
                     if( tspec2 )
                     {
                        //                                   
                        fIsUapsdNeeded = (v_BOOL_t)(pACInfo->curr_QoSInfo[tspec2 - 1].ts_info.psb);
                        if( fIsUapsdNeeded )
                        {
                           //                     
                           //                              
                           pACInfo->requested_QoSInfo[tspec2 - 1] = 
                              pACInfo->curr_QoSInfo[tspec2 - 1];
                           sme_QosReRequestAddTS( pMac, sessionId,
                                                  &pACInfo->requested_QoSInfo[tspec2 - 1],
                                                  ac,
                                                  tspec2);
                        }
                     }
                  }
                  else
                  {
                     //                                                           
                     VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR,
                               "%s: %d: On session %d AC %d has QoS enabled and ACM is set, but no TSPEC",
                               __func__, __LINE__,
                               sessionId, ac);
                     VOS_ASSERT(0);
                  }
               }
               else
               {
                  //                                                                                       
                  fIsUapsdNeeded = (v_BOOL_t)(pACInfo->curr_QoSInfo[0].ts_info.psb);
                  if(fIsUapsdNeeded)
                  {
                     //                                                            
                     uapsd_mask |= 1 << (SME_QOS_EDCA_AC_VO - ac);
                     VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH,
                               "%s: %d: On session %d AC %d has ACM disabled, uapsd mask now 0x%X",
                               __func__, __LINE__,
                               sessionId, ac, uapsd_mask);
                  }
               }
            }
         }
         //                           
         if(uapsd_mask)
         {
            tCsrRoamModifyProfileFields modifyProfileFields;
            //                                    
            csrGetModifyProfileFields(pMac, sessionId, &modifyProfileFields);
            if( btcIsReadyForUapsd(pMac) )
            {
               modifyProfileFields.uapsd_mask = uapsd_mask;
            }
            else
            {  
               modifyProfileFields.uapsd_mask = 0;
            }
            //                         
            if(!HAL_STATUS_SUCCESS(sme_QosRequestReassoc(pMac, sessionId, &modifyProfileFields, VOS_TRUE)))
            {
               //       
               VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR,
                         "%s: %d: On Session %d Reassoc failed",
                         __func__, __LINE__,
                         sessionId);
            }
         }
      }
      else
      {
         VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR,
                   "%s: %d: On Session %d failed to parse IEs",
                   __func__, __LINE__,
                   sessionId);
      }
   }
   //                                
   return SME_QOS_STATUS_SETUP_SUCCESS_IND;
}

/*
                                                                       
*/
static sme_QosStatusType sme_QosReRequestAddTS(tpAniSirGlobal pMac,
                                               v_U8_t sessionId,
                                               sme_QosWmmTspecInfo * pQoSInfo,
                                               sme_QosEdcaAcType ac,
                                               v_U8_t tspecMask)
{
   sme_QosSessionInfo *pSession;
   sme_QosACInfo *pACInfo;
   sme_QosStatusType status = SME_QOS_STATUS_MODIFY_SETUP_FAILURE_RSP;
   sme_QosCmdInfo  cmd;
   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH,
             "%s: %d: Invoked on session %d for AC %d TSPEC %d",
             __func__, __LINE__,
             sessionId, ac, tspecMask);
   pSession = &sme_QosCb.sessionInfo[sessionId];
   pACInfo = &pSession->ac_info[ac];
   //                                                            
   pSession->readyForPowerSave = VOS_FALSE;
   //                                     
   //    
   //                                                       
   //                                                                         
   //                                                                      
   //                                                                           
   //                                          
   if(sme_QosIsRspPending(sessionId, ac) || 
      ( eHAL_STATUS_PMC_PENDING == pmcRequestFullPower(pMac, sme_QosPmcFullPowerCallback, pSession, eSME_REASON_OTHER)))
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR,
                "%s: %d: On session %d buffering the AddTS request "
                   "for AC %d in state %d as Addts is pending "
                "on other AC or waiting for full power",
                __func__, __LINE__,
                sessionId, ac, pACInfo->curr_state);
      //          
      cmd.command = SME_QOS_RESEND_REQ;
      cmd.pMac = pMac;
      cmd.sessionId = sessionId;
      cmd.u.resendCmdInfo.ac = ac;
      cmd.u.resendCmdInfo.tspecMask = tspecMask;
      cmd.u.resendCmdInfo.QoSInfo = *pQoSInfo;
      if(!HAL_STATUS_SUCCESS(sme_QosBufferCmd(&cmd, VOS_FALSE)))
      {
         VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR,
                   "%s: %d: On session %d unable to buffer the AddTS "
                   "request for AC %d TSPEC %d in state %d",
                   __func__, __LINE__,
                   sessionId, ac, tspecMask, pACInfo->curr_state);
         //                             
         //                                             
         pSession->readyForPowerSave = VOS_TRUE;
         return SME_QOS_STATUS_MODIFY_SETUP_FAILURE_RSP;
      }
      return SME_QOS_STATUS_MODIFY_SETUP_PENDING_RSP;
   }
   //                                                          
   switch(pACInfo->curr_state)
   {
   case SME_QOS_QOS_ON:
      {
         //                            
         pACInfo->hoRenewal = VOS_TRUE;
         status = sme_QosSetup(pMac, sessionId, pQoSInfo, ac);
         VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH,
                   "%s: %d: sme_QosSetup returned in SME_QOS_QOS_ON state on "
                   "AC %d with status =%d",
                   __func__, __LINE__,
                   ac, status);
         if(SME_QOS_STATUS_SETUP_REQ_PENDING_RSP != status)
         {
            //                                             
            //                          
            pSession->readyForPowerSave = VOS_TRUE;
         }
         if(SME_QOS_STATUS_SETUP_REQ_PENDING_RSP == status) 
         {
            status = SME_QOS_STATUS_MODIFY_SETUP_PENDING_RSP;
            pACInfo->tspec_pending = tspecMask;
         }
         else if((SME_QOS_STATUS_SETUP_SUCCESS_NO_ACM_NO_APSD_RSP == status) ||
                 (SME_QOS_STATUS_SETUP_SUCCESS_APSD_SET_ALREADY == status) ||
                 (SME_QOS_STATUS_SETUP_SUCCESS_IND_APSD_PENDING == status))
         {
            VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                      "%s: %d: UAPSD is setup already status = %d "
                      "returned by sme_QosSetup",
                      __func__, __LINE__,
                      status);  
         }
         else
         {
            //       
            VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                      "%s: %d: unexpected status = %d returned by sme_QosSetup",
                      __func__, __LINE__,
                      status);
         }
      }
      break;
   case SME_QOS_HANDOFF:
   case SME_QOS_REQUESTED:
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                "%s: %d: Re-Add request in state = %d  buffer the request",
                __func__, __LINE__,
                pACInfo->curr_state);
      cmd.command = SME_QOS_RESEND_REQ;
      cmd.pMac = pMac;
      cmd.sessionId = sessionId;
      cmd.u.resendCmdInfo.ac = ac;
      cmd.u.resendCmdInfo.tspecMask = tspecMask;
      cmd.u.resendCmdInfo.QoSInfo = *pQoSInfo;
      if(!HAL_STATUS_SUCCESS(sme_QosBufferCmd(&cmd, VOS_FALSE)))
      {
         VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                   "%s: %d: couldn't buffer the readd request in state = %d",
                   __func__, __LINE__,
                   pACInfo->curr_state );
         //                             
         //                                             
         pSession->readyForPowerSave = VOS_TRUE;
         return SME_QOS_STATUS_MODIFY_SETUP_FAILURE_RSP;
      }
      status = SME_QOS_STATUS_MODIFY_SETUP_PENDING_RSP;
      break;
   case SME_QOS_CLOSED:
   case SME_QOS_INIT:
   case SME_QOS_LINK_UP:
   default:
      //                 
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                "%s: %d: ReAdd request in unexpected state = %d",
                __func__, __LINE__,
                pACInfo->curr_state );
      //                              
      //                                             
      pSession->readyForPowerSave = VOS_TRUE;
      //        
      break;
   }
   if((SME_QOS_STATUS_MODIFY_SETUP_SUCCESS_NO_ACM_NO_APSD_RSP == status) ||
      (SME_QOS_STATUS_MODIFY_SETUP_SUCCESS_APSD_SET_ALREADY == status)) 
   {
      (void)sme_QosProcessBufferedCmd(sessionId);
   }
   return (status);
}

static void sme_QosInitACs(tpAniSirGlobal pMac, v_U8_t sessionId)
{
   sme_QosSessionInfo *pSession;
   sme_QosEdcaAcType ac;
   pSession = &sme_QosCb.sessionInfo[sessionId];
   for(ac = SME_QOS_EDCA_AC_BE; ac < SME_QOS_EDCA_AC_MAX; ac++) 
   {
      vos_mem_zero(&pSession->ac_info[ac], sizeof(sme_QosACInfo));
      sme_QosStateTransition(sessionId, ac, SME_QOS_INIT);
   }
}
static eHalStatus sme_QosRequestReassoc(tpAniSirGlobal pMac, tANI_U8 sessionId,
                                        tCsrRoamModifyProfileFields *pModFields,
                                        v_BOOL_t fForce )
{
   sme_QosSessionInfo *pSession;
   sme_QosACInfo *pACInfo;
   eHalStatus status;
   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH,
             "%s: %d: Invoked on session %d with UAPSD mask 0x%X",
             __func__, __LINE__,
             sessionId, pModFields->uapsd_mask);
   pSession = &sme_QosCb.sessionInfo[sessionId];
   status = csrReassoc(pMac, sessionId, pModFields, &pSession->roamID, fForce);
   if(HAL_STATUS_SUCCESS(status))
   {
      //                                                                   
      //                     
      sme_QosEdcaAcType ac;
      for(ac = SME_QOS_EDCA_AC_BE; ac < SME_QOS_EDCA_AC_MAX; ac++) 
      {
         pACInfo = &pSession->ac_info[ac];
         VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO, 
                   "%s: %d: AC[%d] is in state [%d]",
                   __func__, __LINE__,
                   ac, pACInfo->curr_state );
         //                                                              
         //                                                            
         //                               
         if(SME_QOS_HANDOFF != pACInfo->curr_state)
         {
            sme_QosStateTransition(sessionId, ac, SME_QOS_HANDOFF);
         }
      }
   }
   return status;
}
static v_U32_t sme_QosAssignFlowId(void)
{
   v_U32_t flowId;
   flowId = sme_QosCb.nextFlowId;
   if (SME_QOS_MAX_FLOW_ID == flowId)
   {
      //                                                                 
      //                                                    
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_FATAL, 
                "%s: %d: Software Test made the flow counter wrap, "
                "QoS may no longer be functional",
                __func__, __LINE__);
      sme_QosCb.nextFlowId = SME_QOS_MIN_FLOW_ID;
   }
   else
   {
      sme_QosCb.nextFlowId++;
   }
   return flowId;
}

static v_U8_t sme_QosAssignDialogToken(void)
{
   v_U8_t token;
   token = sme_QosCb.nextDialogToken;
   if (SME_QOS_MAX_DIALOG_TOKEN == token)
   {
      //           
      sme_QosCb.nextDialogToken = SME_QOS_MIN_DIALOG_TOKEN;
   }
   else
   {
      sme_QosCb.nextDialogToken++;
   }
   return token;
}
#endif /*                             */

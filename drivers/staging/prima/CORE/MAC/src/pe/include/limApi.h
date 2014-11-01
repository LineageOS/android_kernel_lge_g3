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
#ifndef __LIM_API_H
#define __LIM_API_H
#include "wniApi.h"
#include "sirApi.h"
#include "aniGlobal.h"
#include "sirMacProtDef.h"
#include "sirCommon.h"
#include "sirDebug.h"
#include "schGlobal.h"
#include "utilsApi.h"
#include "limGlobal.h"
#include "halMsgApi.h"
#include "wlan_qct_wdi_ds.h"
#include "wlan_qct_wda.h"
#define LIM_POL_SYS_SCAN_MODE      0
#define LIM_POL_SYS_LEARN_MODE     1

/*                          */
#define limResetHBPktCount(psessionEntry)   (psessionEntry->LimRxedBeaconCntDuringHB = 0)

/*                                                        */
/*                */
#define GET_LIM_SYSTEM_ROLE(psessionEntry)      (psessionEntry->limSystemRole)
#define LIM_IS_AP_ROLE(psessionEntry)           (GET_LIM_SYSTEM_ROLE(psessionEntry) == eLIM_AP_ROLE)
#define LIM_IS_STA_ROLE(psessionEntry)          (GET_LIM_SYSTEM_ROLE(psessionEntry) == eLIM_STA_ROLE)
#define LIM_IS_IBSS_ROLE(psessionEntry)         (GET_LIM_SYSTEM_ROLE(psessionEntry) == eLIM_STA_IN_IBSS_ROLE)
/*              */
#define GET_LIM_SME_STATE(pMac)                 (pMac->lim.gLimSmeState)
#define SET_LIM_SME_STATE(pMac, state)          (pMac->lim.gLimSmeState = state)
/*              */
#define GET_LIM_MLM_STATE(pMac)                 (pMac->lim.gLimMlmState)
#define SET_LIM_MLM_STATE(pMac, state)          (pMac->lim.gLimMlmState = state)
/*                           */
#define GET_LIM_STA_CONTEXT_MLM_STATE(pStaDs)   (pStaDs->mlmStaContext.mlmState)
#define SET_LIM_STA_CONTEXT_MLM_STATE(pStaDs, state)  (pStaDs->mlmStaContext.mlmState = state)
/*                */
#define GET_LIM_QUIET_STATE(pMac)               (pMac->lim.gLimSpecMgmt.quietState)
#define SET_LIM_QUIET_STATE(pMac, state)        (pMac->lim.gLimSpecMgmt.quietState = state)
#define LIM_IS_CONNECTION_ACTIVE(psessionEntry)  (psessionEntry->LimRxedBeaconCntDuringHB)
/*                             */
#define GET_LIM_PROCESS_DEFD_MESGS(pMac) (pMac->lim.gLimProcessDefdMsgs)
#define SET_LIM_PROCESS_DEFD_MESGS(pMac, val) (pMac->lim.gLimProcessDefdMsgs = val)
//                                
//               
//                                           
//                                      
#define LIM_IS_RADAR_DETECTED(pMac)         (pMac->lim.gLimSpecMgmt.fRadarDetCurOperChan)
#define LIM_SET_RADAR_DETECTED(pMac, val)   (pMac->lim.gLimSpecMgmt.fRadarDetCurOperChan = val)
#define LIM_MIN_BCN_PR_LENGTH  12
#define LIM_BCN_PR_CAPABILITY_OFFSET 10
typedef enum eMgmtFrmDropReason
{
    eMGMT_DROP_NO_DROP,
    eMGMT_DROP_NOT_LAST_IBSS_BCN,
    eMGMT_DROP_INFRA_BCN_IN_IBSS,
    eMGMT_DROP_SCAN_MODE_FRAME,
    eMGMT_DROP_NON_SCAN_MODE_FRAME,
    eMGMT_DROP_INVALID_SIZE,
}tMgmtFrmDropReason;


//                                                                  
extern void limPostTdDummyPktCallbak(void* pMacGlobals, unsigned int* pBd);
/* 
                                             
                                        
 */
extern tSirRetStatus limInitialize(tpAniSirGlobal);
tSirRetStatus peOpen(tpAniSirGlobal pMac, tMacOpenParameters *pMacOpenParam);
tSirRetStatus peClose(tpAniSirGlobal pMac);
tSirRetStatus limStart(tpAniSirGlobal pMac);
/* 
                                           
 */
void limRadarInit(tpAniSirGlobal pMac);
tSirRetStatus peStart(tpAniSirGlobal pMac);
void peStop(tpAniSirGlobal pMac);
tSirRetStatus pePostMsgApi(tpAniSirGlobal pMac, tSirMsgQ* pMsg);
tSirRetStatus peProcessMsg(tpAniSirGlobal pMac, tSirMsgQ* limMsg);
void limDumpInit(tpAniSirGlobal pMac);
/* 
                                 
                                            
 */
extern void limCleanup(tpAniSirGlobal);
//                                         
extern tANI_U32  limPostMsgApi(tpAniSirGlobal, tSirMsgQ *);
/* 
                                                    
                                                         
 */
extern void limMessageProcessor(tpAniSirGlobal, tpSirMsgQ);
extern void limProcessMessages(tpAniSirGlobal, tpSirMsgQ); //                       
/* 
                                                                    
 */
extern tANI_U8 limIsSystemInScanState(tpAniSirGlobal);
/* 
                                      
                                         
 */
extern tSirRetStatus limHandleIBSScoalescing(tpAniSirGlobal,
                                              tpSchBeaconStruct,
                                              tANI_U8 *,tpPESession);
//                                                                
 static inline tLimSmeStates
limGetSmeState(tpAniSirGlobal pMac) { return pMac->lim.gLimSmeState; }
//                                                                  
 static inline tLimSystemRole
limGetSystemRole(tpPESession psessionEntry) { return psessionEntry->limSystemRole; }
//                                                                      
extern void limReceivedHBHandler(tpAniSirGlobal, tANI_U8, tpPESession);
//                                            
extern void limCheckAndQuietBSS(tpAniSirGlobal);
//                                            
extern void limProcessWdsInfo(tpAniSirGlobal, tSirPropIEStruct);
//                                        
extern void limInitWdsInfoParams(tpAniSirGlobal);
//                                             
extern void limTriggerSTAdeletion(tpAniSirGlobal pMac, tpDphHashNode pStaDs, tpPESession psessionEntry);

#ifdef FEATURE_WLAN_TDLS
//                                                   
extern void limSendSmeTDLSDelStaInd(tpAniSirGlobal pMac, tpDphHashNode pStaDs, tpPESession psessionEntry, tANI_U16 reasonCode);
#endif

//                                                            
extern void limDetectChangeInApCapabilities(tpAniSirGlobal,
                                             tpSirProbeRespBeacon,tpPESession);
tSirRetStatus limUpdateShortSlot(tpAniSirGlobal pMac, 
                                                            tpSirProbeRespBeacon pBeacon, 
                                                            tpUpdateBeaconParams pBeaconParams,tpPESession);

//                                                                 
extern void limSendAddtsReq (tpAniSirGlobal pMac, tANI_U16 staid, tANI_U8 tsid, tANI_U8 userPrio, tANI_U8 wme);
//                                                                
extern void limSendDeltsReq (tpAniSirGlobal pMac, tANI_U16 staid, tANI_U8 tsid, tANI_U8 userPrio, tANI_U8 wme);
//                                                                                     
extern void limPostStartLearnModeMsgToSch(tpAniSirGlobal pMac);
#ifdef WLAN_FEATURE_11AC
extern ePhyChanBondState limGet11ACPhyCBState(tpAniSirGlobal pMac, tANI_U8 channel, tANI_U8 htSecondaryChannelOffset, tANI_U8 CenterChan,tpPESession );
#endif
tANI_U8 limIsSystemInActiveState(tpAniSirGlobal pMac);
#if 0 /*                                                                                */
tSirRetStatus limUpdateGlobalChannelBonding(tpAniSirGlobal pMac, tHalBitVal cbBit);
#endif /*   */

void limHandleLowRssiInd(tpAniSirGlobal pMac);
void limHandleBmpsStatusInd(tpAniSirGlobal pMac);
void limHandleMissedBeaconInd(tpAniSirGlobal pMac, tpSirMsgQ pMsg);
tMgmtFrmDropReason limIsPktCandidateForDrop(tpAniSirGlobal pMac, tANI_U8 *pRxPacketInfo, tANI_U32 subType);
void limMicFailureInd(tpAniSirGlobal pMac, tpSirMsgQ pMsg);
/*                                                                         */
//                        
extern void limSetBssid(tpAniSirGlobal pMac, tANI_U8 *bssId);
extern void limGetBssid(tpAniSirGlobal pMac, tANI_U8 *bssId);
extern void limGetMyMacAddr(tpAniSirGlobal pMac, tANI_U8 *mac);
extern tSirRetStatus limCheckRxSeqNumber(tpAniSirGlobal pMac, tANI_U8 *pRxPacketInfo);
#define limGetQosMode(psessionEntry, pVal) (*(pVal) = (psessionEntry)->limQosEnabled)
#define limGetWmeMode(psessionEntry, pVal) (*(pVal) = (psessionEntry)->limWmeEnabled)
#define limGetWsmMode(psessionEntry, pVal) (*(pVal) = (psessionEntry)->limWsmEnabled)
#define limGet11dMode(psessionEntry, pVal) (*(pVal) = (psessionEntry)->lim11dEnabled)
#define limGetAckPolicy(pMac, pVal)         (*(pVal) = pMac->lim.ackPolicy)
/*                                                                         */
static inline void limGetPhyMode(tpAniSirGlobal pMac, tANI_U32 *phyMode, tpPESession psessionEntry)
{
   *phyMode = psessionEntry ? psessionEntry->gLimPhyMode : pMac->lim.gLimPhyMode;
}

/*                                                                         */
static inline void limGetRfBand(tpAniSirGlobal pMac, tSirRFBand *band, tpPESession psessionEntry)
{
   *band = psessionEntry ? psessionEntry->limRFBand : SIR_BAND_UNKNOWN;
}

/*                                                                          
  
                                                       
  
                                                        
  
                                               
                                                
  
                                             
  
                                                                            */
tSirRetStatus peProcessMessages(tpAniSirGlobal pMac, tSirMsgQ* pMsg);
/*                                                               
             
                                                                
                                                          
                                                              
                                                     
                            
                            
            
                                                                 */
v_VOID_t peFreeMsg( tpAniSirGlobal pMac, tSirMsgQ* pMsg);
/*                                                          */
#endif /*             */

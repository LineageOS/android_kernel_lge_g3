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

#ifndef __PMM_API_H__
#define __PMM_API_H__

#include "sirCommon.h"
#include "schApi.h"
#include "halMsgApi.h"


//                
extern tSirRetStatus pmmInitialize(tpAniSirGlobal pMac);

//                              
extern tSirRetStatus pmmPostMessage(tpAniSirGlobal pMac, tpSirMsgQ pMsg);

//                                             
extern void pmmResetStats(void *pvMac);

//                             
extern void pmmProcessMessage(tpAniSirGlobal, tpSirMsgQ);

extern void pmmProcessPSPoll(tpAniSirGlobal, tANI_U8 *);
extern void pmmUpdatePSPollState(tpAniSirGlobal);
extern void pmmProcessRxActivity(tpAniSirGlobal, tANI_U16, tANI_U8);

extern void pmmGenerateTIM(tpAniSirGlobal, tANI_U8 **, tANI_U16 *, tANI_U8);


void pmmUpdateTIM(tpAniSirGlobal pMac, tpBeaconGenParams pBeaconGenParams);

//                    
extern void pmmUpdatePMMode(tpAniSirGlobal pMac, tANI_U16 staId, tANI_U8 pmMode);

//                   
extern void pmmUpdatePollablePMMode(tpAniSirGlobal, tANI_U16, tANI_U8, tANI_U16);
extern void pmmUpdateNonPollablePMMode(tpAniSirGlobal, tANI_U16, tANI_U8, tANI_U16);

/*                       */
void pmmHandleTimBasedDisassociation(tpAniSirGlobal pMac, tpPESession psessionEntry);

//                   
void pmmInitBmpsPwrSave(tpAniSirGlobal pMac);
tSirRetStatus  pmmSendInitPowerSaveMsg(tpAniSirGlobal pMac,tpPESession);
void pmmInitBmpsResponseHandler(tpAniSirGlobal pMac, tpSirMsgQ limMsg);
tSirRetStatus  pmmSendChangePowerSaveMsg(tpAniSirGlobal pMac);
tSirRetStatus pmmSendSleepIndicationToHal(tpAniSirGlobal pMac);

//                    
void pmmExitBmpsRequestHandler(tpAniSirGlobal pMac, tpExitBmpsInfo pExitBmpsInfo);
void pmmExitBmpsResponseHandler(tpAniSirGlobal pMac, tpSirMsgQ limMsg) ;
void pmmMissedBeaconHandler(tpAniSirGlobal pMac);

//                            
void pmmEnterUapsdRequestHandler (tpAniSirGlobal pMac);
void pmmEnterUapsdResponseHandler(tpAniSirGlobal pMac, tpSirMsgQ limMsg);
void pmmExitUapsdRequestHandler (tpAniSirGlobal pMac);
void pmmExitUapsdResponseHandler(tpAniSirGlobal pMac, tpSirMsgQ limMsg);
tSirRetStatus pmmUapsdSendChangePwrSaveMsg (tpAniSirGlobal pMac, tANI_U8 mode);

//                                              
void pmmEnterImpsRequestHandler(tpAniSirGlobal pMac);
void pmmEnterImpsResponseHandler(tpAniSirGlobal pMac, eHalStatus rspStatus);
void pmmExitImpsRequestHandler(tpAniSirGlobal pMac);
void pmmExitImpsResponseHandler(tpAniSirGlobal pMac, eHalStatus rspStatus);

//                         
void pmmSendWowlAddBcastPtrn(tpAniSirGlobal pMac, tpSirMsgQ pMsg);
void pmmSendWowlDelBcastPtrn(tpAniSirGlobal pMac, tpSirMsgQ pMsg);
void pmmEnterWowlRequestHandler(tpAniSirGlobal pMac, tpSirMsgQ pMsg);
tSirRetStatus pmmSendWowlEnterRequest(tpAniSirGlobal pMac, tpSirHalWowlEnterParams pHalWowlParams);
void pmmEnterWowlanResponseHandler(tpAniSirGlobal pMac, tpSirMsgQ limMsg);
tSirRetStatus  pmmSendExitWowlReq(tpAniSirGlobal pMac, tpSirHalWowlExitParams pHalWowlParams);
void pmmExitWowlanRequestHandler(tpAniSirGlobal pMac);
void pmmExitWowlanResponseHandler(tpAniSirGlobal pMac, tpSirMsgQ limMsg);

//                            
void pmmImpsUpdatePwrSaveStats(tpAniSirGlobal pMac);
void pmmImpsUpdateWakeupStats(tpAniSirGlobal pMac);
tSirRetStatus pmmImpsSendChangePwrSaveMsg(tpAniSirGlobal pMac, tANI_U8 mode);
void pmmImpsUpdateSleepErrStats(tpAniSirGlobal pMac, tSirRetStatus retStatus);
void pmmImpsUpdateWakeupErrStats(tpAniSirGlobal pMac, tSirRetStatus retStatus);
void pmmImpsUpdateErrStateStats(tpAniSirGlobal pMac);
void pmmImpsUpdatePktDropStats(tpAniSirGlobal pMac);

void pmmUpdatePwrSaveStats(tpAniSirGlobal pMac);
void pmmUpdateWakeupStats(tpAniSirGlobal pMac);
void pmmBmpsUpdatePktDropStats(tpAniSirGlobal pMac);
void pmmBmpsUpdateHalReqFailureCnt(tpAniSirGlobal pMac);
void pmmBmpsUpdateInitFailureCnt(tpAniSirGlobal pMac);
void pmmBmpsUpdateInvalidStateCnt(tpAniSirGlobal pMac);
void pmmBmpsUpdatePktDropStats(tpAniSirGlobal pMac);
void pmmBmpsUpdateReqInInvalidRoleCnt(tpAniSirGlobal pMac);
void pmmBmpsUpdateSleepReqFailureCnt(tpAniSirGlobal pMac);
void pmmBmpsUpdateWakeupIndCnt(tpAniSirGlobal pMac);
void pmmBmpsUpdateWakeupReqFailureCnt(tpAniSirGlobal pMac);
void pmmResetPmmState(tpAniSirGlobal pMac);
void pmmSendMessageToLim(tpAniSirGlobal pMac, tANI_U32 msgId);

//              
tSirRetStatus  pmmSendPowerSaveCfg(tpAniSirGlobal pMac, tpSirPowerSaveCfg pUpdatedPwrSaveCfg);

//                          
void pmmLowRssiHandler(tpAniSirGlobal pMac);

#ifdef WLAN_FEATURE_PACKET_FILTERING
void pmmFilterMatchCountResponseHandler(tpAniSirGlobal pMac, tpSirMsgQ limMsg);
#endif //                              

#ifdef WLAN_FEATURE_GTK_OFFLOAD
void pmmGTKOffloadGetInfoResponseHandler(tpAniSirGlobal pMac, tpSirMsgQ limMsg);
#endif //                         

#endif

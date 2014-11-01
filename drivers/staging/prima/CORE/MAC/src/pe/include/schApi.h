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

#ifndef __SCH_API_H__
#define __SCH_API_H__

#include "sirCommon.h"
#include "sirMacProtDef.h"

#include "aniGlobal.h"

//                                  
extern void schSendStartScanRsp(tpAniSirGlobal pMac);

//                                     
extern void schQosUpdateBroadcast(tpAniSirGlobal pMac, tpPESession psessionEntry);

//                                                               
extern void schSetDefaultEdcaParams(tpAniSirGlobal pMac, tpPESession psessionE);

//                             
extern void schQosUpdateLocal(tpAniSirGlobal pMac, tpPESession psessionEntry);

//                                   
extern void schEdcaProfileUpdate(tpAniSirGlobal pMac, tpPESession psessionEntry);

//                                        
extern tSirRetStatus schSetFixedBeaconFields(tpAniSirGlobal pMac,tpPESession psessionEntry);

//                 
extern void schInitialize(tpAniSirGlobal pMac);

//                    
extern void schInitGlobals(tpAniSirGlobal pMac);

//                             
extern void schInitializeCfPollTemplate(tpAniSirGlobal pMac);

//                            
extern void schInitializeCfEndTemplate(tpAniSirGlobal pMac);

//                                
extern void schProcessMessage(tpAniSirGlobal pMac,tpSirMsgQ pSchMsg);

//                                        
extern void schProcessPreBeaconInd(tpAniSirGlobal pMac, tpSirMsgQ limMsg);

//                                               
extern tSirRetStatus schPostMessage(tpAniSirGlobal pMac, tpSirMsgQ pMsg);


extern void schBeaconProcess(tpAniSirGlobal pMac, tANI_U8 *pRxPacketInfo, tpPESession psessionEntry);
extern tSirRetStatus schBeaconEdcaProcess(tpAniSirGlobal pMac, tSirMacEdcaParamSetIE *edca, tpPESession psessionEntry);


#define SCH_RR_TIMEOUT                   (SCH_RR_TIMEOUT_MS / SYS_TICK_DUR_MS)

void schSetBeaconInterval(tpAniSirGlobal pMac,tpPESession psessionEntry);

tSirRetStatus schSendBeaconReq( tpAniSirGlobal, tANI_U8 *, tANI_U16, tpPESession psessionEntry );

void limUpdateProbeRspTemplateIeBitmapBeacon1(tpAniSirGlobal,tDot11fBeacon1*,tANI_U32*,tDot11fProbeResponse*);
void limUpdateProbeRspTemplateIeBitmapBeacon2(tpAniSirGlobal,tDot11fBeacon2*,tANI_U32*,tDot11fProbeResponse*);
void SetProbeRspIeBitmap(tANI_U32*,tANI_U32);
tANI_U32 limSendProbeRspTemplateToHal(tpAniSirGlobal,tpPESession,
                                    tANI_U32*);


#endif

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

#ifndef __CFGAPI_H
#define __CFGAPI_H

#include <sirCommon.h>
#include <sirParams.h>
#include <sirMacProtDef.h>
#include <wniApi.h>
#include <aniGlobal.h>


/*                                                                     */
/*                                                                     */
/*                                                                     */
#define CFG_TYPE_STR                0x0000000
#define CFG_TYPE_INT                0x0000001
#define CFG_HOST_RE                 0x0000002
#define CFG_HOST_WE                 0x0000004

//           
typedef enum eCfgStatusTypes {
    CFG_INCOMPLETE,
    CFG_SUCCESS,
    CFG_FAILURE
} tCfgStatusTypes;

//                                    
typedef struct
{
    tANI_U8    keyMappingAddr[SIR_MAC_ADDR_LENGTH];
    tANI_U32   wepOn;
    tANI_U8    key[SIR_MAC_KEY_LENGTH]; 
    tANI_U32   status;
} tCfgWepKeyEntry;


/*                                                                     */
/*                                                                     */
/*                                                                     */

tANI_U32 cfgNeedRestart(tpAniSirGlobal pMac, tANI_U16 cfgId) ;
tANI_U32 cfgNeedReload(tpAniSirGlobal pMac, tANI_U16 cfgId) ;

//                             
void wlan_cfgInit(tpAniSirGlobal);

//                      
void cfgProcessMbMsg(tpAniSirGlobal, tSirMbMsg*);

//                             
tSirRetStatus cfgSetInt(tpAniSirGlobal, tANI_U16, tANI_U32);

//                                 
tSirRetStatus cfgCheckValid(tpAniSirGlobal, tANI_U16);

//                             
tSirRetStatus wlan_cfgGetInt(tpAniSirGlobal, tANI_U16, tANI_U32*);

//                             
tSirRetStatus cfgIncrementInt(tpAniSirGlobal, tANI_U16, tANI_U32);

//                            
tSirRetStatus cfgSetStr(tpAniSirGlobal, tANI_U16, tANI_U8*, tANI_U32);

tSirRetStatus cfgSetStrNotify(tpAniSirGlobal, tANI_U16, tANI_U8*, tANI_U32, int);

//                                                        
void processCfgDownloadReq(tpAniSirGlobal, tANI_U16, tANI_U32*);

//                            
tSirRetStatus wlan_cfgGetStr(tpAniSirGlobal, tANI_U16, tANI_U8*, tANI_U32*);

//                                     
tSirRetStatus wlan_cfgGetStrMaxLen(tpAniSirGlobal, tANI_U16, tANI_U32*);

//                                     
tSirRetStatus wlan_cfgGetStrLen(tpAniSirGlobal, tANI_U16, tANI_U32*);

//                                              
tPowerdBm cfgGetRegulatoryMaxTransmitPower(tpAniSirGlobal pMac, tANI_U8 channel);

//                         
void cfgDump(tANI_U32*);

//                                 
void cfgSave(void);

//                     
extern tSirRetStatus cfgGetCapabilityInfo(tpAniSirGlobal pMac, tANI_U16 *pCap,tpPESession psessionEntry);

//                     
extern void cfgSetCapabilityInfo(tpAniSirGlobal, tANI_U16);

//                    
void cfgCleanup(tpAniSirGlobal pMac);

extern tANI_U8 *gCfgParamName[];

#endif /*            */





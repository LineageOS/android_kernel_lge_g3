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
#ifndef __MAC_PROP_EXTS_H
#define __MAC_PROP_EXTS_H

#include "sirTypes.h"
#include "sirApi.h"
#include "aniSystemDefs.h"

//                              

//                

//                                             
#define SIR_MAC_PROP_EXT_RATES_TYPE     0
#define SIR_MAC_PROP_AP_NAME_TYPE       1
#define SIR_MAC_PROP_LOAD_INFO_TYPE     6
#define SIR_MAC_PROP_ASSOC_TYPE         7
#define SIR_MAC_PROP_LOAD_BALANCE_TYPE  8
#define SIR_MAC_PROP_LL_ATTR_TYPE       9
#define SIR_MAC_PROP_CAPABILITY         10  //                         
#define SIR_MAC_PROP_VERSION            11  //             
#define SIR_MAC_PROP_EDCAPARAMS         12  //                            
#define SIR_MAC_PROP_SRCMAC             13  //                     
#define SIR_MAC_PROP_TITAN              14  //                          
#define SIR_MAC_PROP_CHANNEL_SWITCH     15  //                                
#define SIR_MAC_PROP_QUIET_BSS          16  //                              
#define SIR_MAC_PROP_TRIG_STA_BK_SCAN   17  //                                                  
#define SIR_MAC_PROP_TAURUS              18  //                           

//                   
#define SIR_MAC_PROP_CAPABILITY_MIN      sizeof(tANI_U16)

//                                   
#define SIR_MAC_PROP_TRIG_STA_BK_SCAN_EID_MIN           0
#define SIR_MAC_PROP_TRIG_STA_BK_SCAN_EID_MAX           1

//                                                                  
//                                                                           
//                                                                        
//       
//                                                                                    
//                                                        
//                                   
//                                                                         
//                                                                
#define SIR_MAC_PROP_CAPABILITY_HCF           WNI_CFG_PROP_CAPABILITY_HCF
#define SIR_MAC_PROP_CAPABILITY_11EQOS        WNI_CFG_PROP_CAPABILITY_11EQOS
#define SIR_MAC_PROP_CAPABILITY_WME           WNI_CFG_PROP_CAPABILITY_WME
#define SIR_MAC_PROP_CAPABILITY_WSM           WNI_CFG_PROP_CAPABILITY_WSM
#define SIR_MAC_PROP_CAPABILITY_EXTRATES      WNI_CFG_PROP_CAPABILITY_EXTRATES
//                                                                 
#define SIR_MAC_PROP_CAPABILITY_EXTRATE_STOP  WNI_CFG_PROP_CAPABILITY_EXTRATE_STOP
#define SIR_MAC_PROP_CAPABILITY_TITAN         WNI_CFG_PROP_CAPABILITY_TITAN
#define SIR_MAC_PROP_CAPABILITY_TAURUS         WNI_CFG_PROP_CAPABILITY_TAURUS
#define SIR_MAC_PROP_CAPABILITY_ESCORT_PKT    WNI_CFG_PROP_CAPABILITY_ESCORT_PKT
//                                                
#define SIR_MAC_PROP_CAPABILITY_EDCAPARAMS    WNI_CFG_PROP_CAPABILITY_EDCAPARAMS
#define SIR_MAC_PROP_CAPABILITY_LOADINFO      WNI_CFG_PROP_CAPABILITY_LOADINFO
#define SIR_MAC_PROP_CAPABILITY_VERSION       WNI_CFG_PROP_CAPABILITY_VERSION
#define SIR_MAC_PROP_CAPABILITY_MAXBITOFFSET  WNI_CFG_PROP_CAPABILITY_MAXBITOFFSET

//                                                                       
#define PROP_CAPABILITY_SET(bitname, value) \
  ((value) = (value) | ((tANI_U16)(1 << SIR_MAC_PROP_CAPABILITY_ ## bitname)))

#define PROP_CAPABILITY_RESET(bitname, value) \
  ((value) = (value) & ~((tANI_U16)(1 << SIR_MAC_PROP_CAPABILITY_ ## bitname)))
        
#define PROP_CAPABILITY_GET(bitname, value) \
        (((value) >> SIR_MAC_PROP_CAPABILITY_ ## bitname) & 1)


#define IS_DOT11_MODE_PROPRIETARY(dot11Mode) \
        (((dot11Mode == WNI_CFG_DOT11_MODE_POLARIS) || \
          (dot11Mode == WNI_CFG_DOT11_MODE_TITAN) || \
          (dot11Mode == WNI_CFG_DOT11_MODE_TAURUS) || \
          (dot11Mode == WNI_CFG_DOT11_MODE_ALL)) ? TRUE: FALSE)

#define IS_DOT11_MODE_HT(dot11Mode) \
        (((dot11Mode == WNI_CFG_DOT11_MODE_11N) || \
          (dot11Mode ==  WNI_CFG_DOT11_MODE_11N_ONLY) || \
          (dot11Mode ==  WNI_CFG_DOT11_MODE_11AC) || \
          (dot11Mode ==  WNI_CFG_DOT11_MODE_11AC_ONLY) || \
          (dot11Mode ==  WNI_CFG_DOT11_MODE_TAURUS) || \
          (dot11Mode ==  WNI_CFG_DOT11_MODE_ALL)) ? TRUE: FALSE)

#ifdef WLAN_FEATURE_11AC
#define IS_DOT11_MODE_VHT(dot11Mode) \
        (((dot11Mode == WNI_CFG_DOT11_MODE_11AC) || \
          (dot11Mode ==  WNI_CFG_DOT11_MODE_11AC_ONLY) || \
          (dot11Mode ==  WNI_CFG_DOT11_MODE_TAURUS) || \
          (dot11Mode ==  WNI_CFG_DOT11_MODE_ALL)) ? TRUE: FALSE)
#endif
        /*
                                                                
                                                                  
                                                                                   
        */
#define IS_DOT11_MODE_TITAN_ALLOWED(dot11Mode) \
        (((dot11Mode == WNI_CFG_DOT11_MODE_TITAN) || \
          (dot11Mode == WNI_CFG_DOT11_MODE_TAURUS) || \
          (dot11Mode == WNI_CFG_DOT11_MODE_11G) || \
          (dot11Mode == WNI_CFG_DOT11_MODE_11N) || \
          (dot11Mode == WNI_CFG_DOT11_MODE_ALL)) ? TRUE: FALSE)


        /*
                                                                 
                                                                  
                                                                            
        */
#define IS_DOT11_MODE_TAURUS_ALLOWED(dot11Mode) \
        (((dot11Mode == WNI_CFG_DOT11_MODE_TAURUS) || \
          (dot11Mode == WNI_CFG_DOT11_MODE_11N) || \
          (dot11Mode == WNI_CFG_DOT11_MODE_ALL)) ? TRUE: FALSE)



#define IS_DOT11_MODE_POLARIS(dot11Mode)   IS_DOT11_MODE_PROPRIETARY(dot11Mode)

#define IS_DOT11_MODE_11B(dot11Mode)  \
            ((dot11Mode == WNI_CFG_DOT11_MODE_11B) ? TRUE : FALSE)

//                                   
//                                         
typedef enum eSirMacPropStatusCodes
{
    dummy
} tSirMacPropStatusCodes;

/* 
                                    
                                                                 
 */
typedef enum eSirMacPropReasonCodes
{
    eSIR_MAC_ULA_TIMEOUT_REASON=0xFF00
} tSirMacPropReasonCodes;


//                           
typedef struct sSirMacPropIE
{
    tANI_U8    elementID;    //                        
    tANI_U8    length;
    tANI_U8    oui[3];       //                           
    tANI_U8    info[1];
} tSirMacPropIE, *tpSirMacPropIE;


typedef struct sSirMacPropRateSet
{
    tANI_U8  numPropRates;
    tANI_U8  propRate[8];
} tSirMacPropRateSet, *tpSirMacPropRateSet;


typedef struct sSirMacPropLLSet
{
    tANI_U32  deferThreshold;
} tSirMacPropLLSet, *tpSirMacPropLLSet;

#define SIR_PROP_VERSION_STR_MAX 20
typedef struct sSirMacPropVersion
{
    tANI_U32  chip_rev;       //                    
    tANI_U8   card_type;      //             
    tANI_U8  build_version[SIR_PROP_VERSION_STR_MAX]; //                    
} tSirMacPropVersion, *tpSirMacPropVersion;
#define SIR_MAC_PROP_VERSION_MIN (SIR_PROP_VERSION_STR_MAX + sizeof(tANI_U32))


//             
#define TCID_0   0x01
#define TCID_1   0x02
#define TCID_2   0x04
#define TCID_3   0x08
#define TCID_4   0x10
#define TCID_5   0x20
#define TCID_6   0x40
#define TCID_7   0x80
#define TCID_ALL 0xFF

//                           
#define GET_CONCAT_STATE(ccBitmap,tcid) \
        ((ccBitmap) & (tcid))

//                         
#define GET_COMPRESSION_STATE(cpBitmap,tcid) \
        ((cpBitmap) & (tcid))

//                                 
#define GET_RFCS_OPER_STATE(revFcsState) (revFcsState & 0x01)
#define GET_RFCS_PATTERN_ID(revFcsState) ((revFcsState & 0x0E) >> 1)

/*                                 */
#define LIM_CB_LEGACY_BSS_DETECT_IDLE                   0
#define LIM_CB_LEGACY_BSS_DETECT_RUNNING                1

/*                                                */
#define LIM_RESTORE_CB_NUM_SCAN_INTERVAL_DEFAULT        2

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
typedef struct sQuietBssIEStruct
{

  //                                                     
  //                                                   
  //      
  //                                                
  //                                   
  //             
  tANI_U8 quietCount;

  //                                                       
  //                                                 
  //                              
  //                                          
  tANI_U8 quietPeriod;

  //                                                 
  //                           
  tANI_U16 quietDuration;

  //                                                     
  //                                                 
  //                                                      
  //                                 
  //                           
  tANI_U16 quietOffset;

} tQuietBssIEStruct, *tpQuietBssIEStruct;

typedef struct sChannelSwitchPropIEStruct
{
    tANI_U8                  mode;
    tANI_U8                  primaryChannel;
    tANI_U8                  subBand;
    tANI_U8                  channelSwitchCount;

} tChannelSwitchPropIEStruct, *tpChannelSwitchPropIEStruct;

//                                            
typedef struct sSirPropIEStruct
{
    tANI_U8                    aniIndicator;

    tANI_U8                    propRatesPresent:1;
    tANI_U8                    apNamePresent:1;
    tANI_U8                    loadBalanceInfoPresent:1;
    tANI_U8                    versionPresent:1;
    tANI_U8                    edcaParamPresent:1;
    tANI_U8                    capabilityPresent:1;
    tANI_U8                    titanPresent:1;
    tANI_U8                    taurusPresent:1;  
    tANI_U8                    propChannelSwitchPresent:1;
    tANI_U8                    quietBssPresent:1;
    tANI_U8                    triggerStaScanPresent:1;                
    tANI_U8                    rsvd:5;


    tSirMacPropRateSet    propRates;
    tAniApName            apName;           //                          
    tSirAlternateRadioInfo  alternateRadio; //                            
    tANI_U16              capability;       //                   
    tSirMacPropVersion    version;
    tSirMacEdcaParamSetIE edca;
    tChannelSwitchPropIEStruct  channelSwitch;
    tQuietBssIEStruct     quietBss;
    tANI_U8               triggerStaScanEnable;


} tSirPropIEStruct, *tpSirPropIEStruct;



#endif /*                   */

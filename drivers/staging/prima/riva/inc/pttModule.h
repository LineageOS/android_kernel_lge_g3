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

/**
 *
   Airgo Networks, Inc proprietary.
   All Rights Reserved, Copyright 2005
   This program is the confidential and proprietary product of Airgo Networks Inc.
   Any Unauthorized use, reproduction or transfer of this program is strictly prohibited.


   pttModule.h: global variable structure for pttModule
   Author:  Mark Nelson
   Date: 6/27/05

   History -
   Date         Modified by               Modification Information
  --------------------------------------------------------------------------

 */

#ifndef PTTMODULE_H
#define PTTMODULE_H
#include <wlan_phy.h>
#include <pttFrameGen.h>

#ifdef BUILD_QWPTTSTATIC
#include "wlan_hal_msg.h"       //                                       
#endif

//                     
#define MAX_NV_BIN_SIZE 2000

enum
{
   PTT_STATUS_SUCCESS = 0,
   PTT_STATUS_FAILURE = 1,
   PTT_MAX_VAL = 0XFFFF,
};
typedef tANI_U32 eQWPttStatus;


typedef PACKED_PRE struct PACKED_POST {
   tANI_U16 drvMjr;
   tANI_U16 drvMnr;
   tANI_U16 drvPtch;
   tANI_U16 drvBld;
   tANI_U16 pttMax;
   tANI_U16 pttMin;
   //                    
} sBuildReleaseParams;

typedef PACKED_PRE struct PACKED_POST {
   tANI_BOOLEAN agPktsDisabled; //        
   tANI_BOOLEAN bPktsDisabled;  //       
   tANI_BOOLEAN slrPktsDisabled;  //                  
   tANI_BOOLEAN n40PktsDisabled; //       
   tANI_BOOLEAN ac80PktsDisabled; //        
   tANI_BOOLEAN rsvd;
} sRxTypesDisabled;

typedef PACKED_PRE struct PACKED_POST {
   tANI_U32 totalRxPackets;
   tANI_U32 totalMacRxPackets;
   tANI_U32 totalMacFcsErrPackets;
} sRxFrameCounters;




/*                */
//                                                        
typedef enum {
   GRAB_RAM_RXFIR,
   GRAB_RAM_ADC,
   GRAB_RAM_ADC_80,
   GRAB_RAM_MAX_VAL = 0XFFFF,
} eGrabRamType;

#define GRAB_RAM_SIZE 6000
#define GRAB_RAM_SIZE_80MHZ_1_CHAIN 12000



//                                                              
typedef enum eGramDumpTrigType {
   eGRAM_DUMP_UNTRIGGERED,
   eGRAM_DUMP_TRIG_ON_11A,
   eGRAM_DUMP_TRIG_ON_11B,
   eGRAM_DUMP_TRIG_ON_11A_OR_11B
} tGramDumpTrigType;

typedef PACKED_PRE struct PACKED_POST {
   //                              
   eHalPhyRates rate;           //            
   ePhyChanBondState cbState;   //                            

   tANI_U8 channelId;           //                  
   tANI_U8 pwrTemplateIndex;    //                                              
   tANI_U8 reserved[2];

   //                             
   tTxChainPower txChains[PHY_MAX_TX_CHAINS];   //                          
} tTxPowerReport;

#define RXP_MAX_FILTER_IDX                64

typedef PACKED_PRE struct PACKED_POST
{
   uint32 rxpFilterForFTMPER[RXP_MAX_FILTER_IDX];
   uint32 max_pktlen;
   uint32 flt_disable0;
   uint32 flt_disable1;
   uint32 config2;
   uint32 push_wq_ctrl;
   uint32 push_wq_ctrl2;
   uint32 extra_frame_flt;
} sMACConfig;


typedef PACKED_PRE struct PACKED_POST {
   /*
                                                                 
                                                            
                                                                       
    */
   sPttFrameGenParams frameGenParams;
   tANI_U8 payload[MAX_PAYLOAD_SIZE];

   //                       
   tANI_U16 numWfmSamples;
   tANI_BOOLEAN wfmEnabled;
   tANI_BOOLEAN wfmStored;

   //                      
   tTxGain forcedTxGain[PHY_MAX_TX_CHAINS];  //                                                         
   tANI_U8 tpcPowerLut[PHY_MAX_TX_CHAINS][TPC_MEM_POWER_LUT_DEPTH];
   tTxGain tpcGainLut[PHY_MAX_TX_CHAINS][TPC_MEM_GAIN_LUT_DEPTH];

   //                    
   tANI_BOOLEAN frameGenEnabled;
   tANI_BOOLEAN phyDbgFrameGen; //                                                                                            
   
   //                    
   tANI_U8 tx_mode;
   
   tANI_BOOLEAN ftm_per_settings_saved;

   //               
   sRxChainsAgcEnable agcEnables;

   tANI_U32 *pADCCaptureCache;  //                                     

   //                                                                    

   sRxChainsRssi rssi;
} tPttModuleVariables;

#ifdef WCN_PRONTO
extern sMACConfig macConfig;
#endif

#endif /*             */

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

#ifndef __SCH_GLOBAL_H__
#define __SCH_GLOBAL_H__

#include "sirMacPropExts.h"
#include "limGlobal.h"

#include "parserApi.h"


#define ANI_SCH_ADAPTIVE_THRESHOLD_TH_CD    0x00000001
#define ANI_SCH_ADAPTIVE_THRESHOLD_TH_D0    0x00000002

#define ANI_SCH_ADAPTIVE_THRESHOLD_ALL      (ANI_SCH_ADAPTIVE_THRESHOLD_TH_CD | ANI_SCH_ADAPTIVE_THRESHOLD_TH_D0)

#define ANI_SCH_ADAPTIVE_ALGO_BAND_2GHZ     0x00000001
#define ANI_SCH_ADAPTIVE_ALGO_BAND_5GHZ     0x00000002

#define ANI_SCH_ADAPTIVE_ALGO_BAND_ALL      (ANI_SCH_ADAPTIVE_ALGO_BAND_2GHZ | ANI_SCH_ADAPTIVE_ALGO_BAND_5GHZ)


//                          

#define SCH_DIAG_RR_TIMEOUT_DELETE    0x1
#define SCH_DIAG_RR_LOWER_RATE        0x2

#ifdef WLAN_SOFTAP_VSTA_FEATURE
#define TIM_IE_SIZE 0xB
#else
#define TIM_IE_SIZE 0x7
#endif

//                                                                   

//                  
#define tSchBeaconStruct tSirProbeRespBeacon
#define tpSchBeaconStruct struct sSirProbeRespBeacon *

//                                                                    

//                                                              

//                                          
#define SCH_MAX_BEACON_SIZE    512

#define SCH_MAX_PROBE_RESP_SIZE 512

struct schMisc {

    tANI_U8 *gSchProbeRspTemplate;

    //                                                            
    tANI_U8 *gSchBeaconFrameBegin;

    //                                                           
    tANI_U8 *gSchBeaconFrameEnd;

    //                               
    tANI_U16 gSchBeaconOffsetBegin;
    //                              
    tANI_U16 gSchBeaconOffsetEnd;

    tANI_U16 gSchBeaconInterval;

    //                   
    tANI_U8 gSchCFPCount;

    //                        
    tANI_U8 gSchCFPDurRemaining;

    //                      
    tANI_U8 gSchCFPMaxDuration;

    //                    
    tANI_U8 gSchDTIMCount;

    //                                        
    tANI_U8 gSchCFPInitiated;

    //                                        
    tANI_U8 gSchCFBInitiated;

    //                                            
    tANI_U8 gSchCFPEnabled;

    //                                            
    tANI_U8 gSchCFBEnabled;

    //                                     

    //                                              
    tANI_U8  rrTimer[8];

    //                                                                     
    tANI_U16  rrTimeout[8];

    //                           
    tANI_U16  numRR[8];
    tANI_U16  numRRtimeouts[8];

    //                                                        
    tANI_U8   fBeaconChanged;

    tANI_U16 p2pIeOffset;

};

//                                                              

typedef struct schStaWaitList
{
    tANI_U16 staId;
    tANI_U16 count;
} tStaWaitList, *tpStaWaitList;


//                      
typedef struct sAniSirSch
{
    //                      
    struct  schMisc schObject;

    //                         

    //                               
    tANI_U8 gSchHcfEnabled;

    //                                         
    tANI_U8 gSchScanRequested;

    //                                                
    tANI_U8 gSchScanReqRcvd;


    //                                         
    tANI_U32 gSchGenBeacon;

#define SCH_MAX_ARR 100
    tANI_U32 gSchBeaconsWritten;
    tANI_U32 gSchBeaconsSent;
    tANI_U32 gSchBBXportRcvCnt;
    tANI_U32 gSchRRRcvCnt, qosNullCnt;
    tANI_U32 gSchBcnRcvCnt;
    tANI_U32 gSchUnknownRcvCnt;

    tANI_U32 gSchBcnParseErrorCnt;
    tANI_U32 gSchBcnIgnored;

    //                    
    //                         

    tANI_U32 numPoll, numData, numCorrupt;
    tANI_U32 numBogusInt, numTxAct0;

#define SCH_MAX_NUM_SCH 21
    //                                      
    //                      

    tANI_U32 lastBeaconLength;
    tANI_U16 rrTimeout;
    tANI_U32 pollPeriod;
    tANI_U32 keepAlive;
    tANI_U32 multipleSched;
    tANI_U32 pollFeedbackHist[8];
    tANI_U32 dataFeedbackHist[8];
    tANI_U32 maxPollTimeouts;
    tANI_U32 checkCfbFlagStuck;

    //               
    tpStaWaitList pStaWaitList;

    //                                                  
    tANI_U16 staWaitListIn;
    //                                               
    tANI_U16 staWaitListOut;
    //                                               
    tANI_U16 staWaitListCount;
    //                                        
    tANI_U16 staWaitListTotalWait;

    //                                                           
    tANI_U32 ignoreDph;

} tAniSirSch, *tpAniSirSch;


#endif

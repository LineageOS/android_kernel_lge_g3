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

#ifndef __PMM_GLOBAL_H__
#define __PMM_GLOBAL_H__

#include "sirApi.h"

typedef struct sPmmStaState
{
    //                                         
    tANI_U8 powerSave : 1;
    //                                        
    tANI_U8 cfPollable : 1;
    //                                                                 
    tANI_U8 psPollUpdate:2;

    //          
    tANI_U8 rsvd : 4;

    //                                                 
    tANI_U8 nextPS;
} tPmmStaState, *tpPmmStaState;


#define NO_STATE_CHANGE 0xFF

typedef enum ePmmState
{
    ePMM_STATE_INVALID,
    ePMM_STATE_READY,
    //    
    ePMM_STATE_BMPS_WT_INIT_RSP,
    ePMM_STATE_BMPS_WT_SLEEP_RSP,
    ePMM_STATE_BMPS_SLEEP,
    ePMM_STATE_BMPS_WT_WAKEUP_RSP,
    ePMM_STATE_BMPS_WAKEUP,
    //    
    ePMM_STATE_IMPS_WT_SLEEP_RSP,
    ePMM_STATE_IMPS_SLEEP,
    ePMM_STATE_IMPS_WT_WAKEUP_RSP,
    ePMM_STATE_IMPS_WAKEUP,
    //     
    ePMM_STATE_UAPSD_WT_SLEEP_RSP,
    ePMM_STATE_UAPSD_SLEEP,
    ePMM_STATE_UAPSD_WT_WAKEUP_RSP,

    //      
    ePMM_STATE_WOWLAN,

    ePMM_STATE_ERROR,
    ePMM_STATE_LAST,
}tPmmState;

typedef struct sPmmStaInfo
{
    tANI_U16 assocId;
    tANI_U32 staTxAckCnt;
}tPmmStaInfo, *tpPmmStaInfo;

typedef struct sPmmTim
{
    tANI_U8 *pTim;                    /*               */
    tANI_U8 minAssocId;
    tANI_U8 maxAssocId;
    tANI_U8 dtimCount;
    /*                                                                  */
    tANI_U8 numStaWithData;  /*                                                         */
    tpPmmStaInfo    pStaInfo;   /*                                                       */
} tPmmTim, *tpPmmTim;

typedef struct sAniSirPmm
{


    //                               
    //                     
    //                               
    //                                
    //                                
    //                               


    tANI_U64 BmpsmaxSleepTime;
    tANI_U64 BmpsavgSleepTime;
    tANI_U64 BmpsminSleepTime;
    tANI_U64 BmpscntSleep;

    tANI_U64 BmpsmaxTimeAwake;
    tANI_U64 BmpsavgTimeAwake;
    tANI_U64 BmpsminTimeAwake;
    tANI_U64 BmpscntAwake;

    tANI_U64 BmpsWakeupTimeStamp;
    tANI_U64 BmpsSleepTimeStamp;

    //                 
    tANI_U64 BmpsPktDrpInSleepMode;
    tANI_U64 BmpsInitFailCnt;
    tANI_U64 BmpsSleeReqFailCnt;
    tANI_U64 BmpsWakeupReqFailCnt;
    tANI_U64 BmpsInvStateCnt;
    tANI_U64 BmpsWakeupIndCnt;
    tANI_U64 BmpsHalReqFailCnt;
    tANI_U64 BmpsReqInInvalidRoleCnt;

    /*                                       */
    tANI_U64 ImpsWakeupTimeStamp;
    tANI_U64 ImpsSleepTimeStamp;

    tANI_U64 ImpsMaxTimeAwake;
    tANI_U64 ImpsMinTimeAwake;
    tANI_U64 ImpsAvgTimeAwake;
    tANI_U64 ImpsCntAwake;

    tANI_U64 ImpsCntSleep;
    tANI_U64 ImpsMaxSleepTime;
    tANI_U64 ImpsMinSleepTime;
    tANI_U64 ImpsAvgSleepTime;

    tANI_U64 ImpsSleepErrCnt;
    tANI_U64 ImpsWakeupErrCnt;
    tANI_U64 ImpsLastErr;

    tANI_U64 ImpsInvalidStateCnt;
    tANI_U64 ImpsPktDrpInSleepMode;


   //                                     
    tANI_U16 gPmmNextSta;

    //                                                 
    tANI_U16 gPmmNextCFPSta;

    //                            
    tANI_U16 gPmmNumSta;

    tANI_U8  gPmmPsPollUpdate:1; //                                                
    tANI_U8  rsvd: 7;

   //                                  
    /* 
                                                                     
                                                                  
     */

    tANI_U32 gPmmBeaconInterval;     //                                                           
    tSirPowerSaveCfg gPmmCfg;  //                                                                 
    //                                 
    tPmmState gPmmState;
    //                                                     
    tANI_U8 inMissedBeaconScenario;

    tPmmTim gPmmTim;


    //                                                      
    tExitBmpsReason   gPmmExitBmpsReasonCode;
    tANI_U8  sessionId;      //                                                                                   

} tAniSirPmm, *tpAniSirPmm;

#endif

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


#ifndef __DPH_GLOBAL_H__

#define __DPH_GLOBAL_H__


#include "limGlobal.h"

//                      

#include "sirMacProtDef.h"

#include "sirMacPropExts.h"

#include "sirApi.h"


//                                                               

#define DPH_STATS


//                              

#define DPH_TID_NOTALLOWED           0xFF


//                                                      

#define DPH_RATE_ADAPTATION_PERIOD     20


//             

#define DPH_PHY_RATE_1_INDEX     0

#define DPH_PHY_RATE_2_INDEX     1

#define DPH_PHY_RATE_5_5_INDEX   2

#define DPH_PHY_RATE_11_INDEX    3

#define DPH_PHY_RATE_6_INDEX     4

#define DPH_PHY_RATE_9_INDEX     5

#define DPH_PHY_RATE_12_INDEX    6

#define DPH_PHY_RATE_18_INDEX    7

#define DPH_PHY_RATE_24_INDEX    8

#define DPH_PHY_RATE_36_INDEX    9

#define DPH_PHY_RATE_48_INDEX   10

#define DPH_PHY_RATE_54_INDEX   11

#define DPH_PHY_RATE_72_INDEX   12

#define DPH_PHY_RATE_96_INDEX   13

#define DPH_PHY_RATE_108_INDEX  14

#define DPH_PHY_RATE_144_INDEX  15

#define DPH_PHY_RATE_MAX_INDEX  16


//                                                             

#define DPH_MAX_MGMT_WAIT_TIME  10000


//                                            

#define DPH_WAIT_HIST_STEP 20


//                                          

#define DPH_WAIT_HIST_SIZE  100


//                                              

#define DPH_MGMT_TCID                      4

#define DPH_KEEPALIVE_PROBE_RESPONSE_TCID  0


//                             

#define DPH_USE_MGMT_STAID  -1


//                  

#define DPH_NON_KEEPALIVE_FRAME  0

#define DPH_KEEPALIVE_FRAME      1


//                                                                                         

#define DPH_FC_BD_FILL_MASK  0xFFCC


//                                                    

#define DPH_ENABLE_HCF_TXOP_GEN_AT_TFP   0x00

#define DPH_DISABLE_HCF_TXOP_GEN_AT_TFP  0x02


//                                                     

#define DPH_ENABLE_EDCF_TXOP_GEN_AT_TFP   0x00

#define DPH_DISABLE_EDCF_TXOP_GEN_AT_TFP  0x01


#define DPH_DUMP_ALL_STA_ID     -1

#define DPH_DUMP_RX_BD           0

#define DPH_DUMP_TX_BD           1

#define DPH_DUMP_TX_MGMT_BD      2


//                                              

#define DPH_STA_HASH_INDEX_PEER   1


typedef struct sDphRateBasedCtr

{

    tANI_U32 hi;

    tANI_U32 lo;

} tDphRateBasedCtr;


typedef struct sDphPhyRates

{

    tANI_U8 dataRateX2;

    tANI_U8 ackRateX2;

    tANI_U8 rtsRateX2;

} tDphPhyRates;


typedef struct sDphIFSValues

{

    tANI_U8 sifs;

    tANI_U8 pifs;

    tANI_U8 difs;

    tANI_U8 preamble;

} tDphIFSValues;


typedef struct sDphQosParams

{

    tANI_U8                   addtsPresent;

    tSirAddtsReqInfo       addts;

    tSirMacQosCapabilityStaIE capability;

} tDphQosParams;


//                           

typedef struct sDphQueueAttr

{

    tANI_U16     valid : 1;

    tANI_U16     seqNum : 12;

    tANI_U16     ackPolicy : 2;

    tANI_U16     rsvd : 1;

} tDphQueueAttr, *tpDphQueueAttr;



typedef struct sCfgTrafficClass {

    //                             

    //                                                               

    //                                             

    tANI_U8 fDisableTx:1;

    tANI_U8 fDisableRx:1;

    tANI_U8 fUseBATx:1;

    tANI_U8 fUseBARx:1;


    //                                                                     

    tANI_U8 fRxCompBA:1;

    tANI_U8 fTxCompBA:1;


    //                                                           

    tANI_U8 fRxBApolicy:1;


    //                                                         

    tANI_U8 fTxBApolicy:1;


    //                      

    tANI_U8 role;


    //                                                         

    //                                          

    tANI_U16 rxBufSize;


    //                                                   

    tANI_U16 txBufSize;


    //                                        

    tANI_U16 tuTxBAWaitTimeout; //                                              


    tANI_U16 tuRxBAWaitTimeout; //                                                                 


} tCfgTrafficClass;



//                

typedef struct sDphHashNode

{


    //      

    //                                     

    //                       

    tANI_U8   valid : 1;

    tANI_U8   encPolicy : 3;

    tANI_U8   defaultKey : 1;

    tANI_U8   defaultKeyId : 2;

    tANI_U8   qosMode : 1;


    //      

    tANI_U8   erpEnabled : 1;

    tANI_U8   added : 1; //                                          

    tANI_U8   linkTestOn : 1;

    tANI_U8   shortPreambleEnabled : 1;

    tANI_U8   shortSlotTimeEnabled : 1;

    tANI_U8   stopTx:1;

    tANI_U8   wmeEnabled: 1; //                                       

    tANI_U8   lleEnabled: 1; //                                       


    //      

    tANI_U8   wsmEnabled: 1; //                                       

    tANI_U8   versionPresent:1; //                          

    tANI_U8   burstEnableForce:1; //                                     

    tANI_U8   staAuthenticated:1;    

    //                                

    tANI_U8  aniPeer:1;

    tANI_U8   titanPeer:1;                //                                         

    tANI_U8  fAniCount:1;

    tANI_U8   rmfEnabled:1;


    //                    

    tANI_U16   fragSize;


    //           

    tLimMlmStaContext mlmStaContext;


    //                                                               

    tANI_U8  timWaitCount;


    //                                         

    tANI_U32    curTxMpduCnt;




    //                                                  

    tANI_U8  numTimSent;


    //                   

    tDphQosParams  qos;


    //                                                           

    tSirMacPropVersion version;

    //                               

    tANI_U16                propCapability;


#ifdef PLM_WDS

    tANI_U8  wdsIndex;

    tANI_U8  wdsPeerBeaconSeen;

#endif


   //                   

   tANI_U16 baPolicyFlag;                 //                        


    /*

                                               

    */

    tSirSupportedRates supportedRates;


    tANI_U8 htGreenfield:1;

    tANI_U8 htShortGI40Mhz:1;

    tANI_U8 htShortGI20Mhz:1;

    //                                          

    tANI_U8 htDsssCckRate40MHzSupport:1;

    //                                                          

    tANI_U8 htLsigTXOPProtection:1;

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

    tANI_U8 htAMpduDensity:3;




    //                        

    //                        

    tANI_U8 htMaxAmsduLength;




    //                

    tSirMacHTMIMOPowerSaveState htMIMOPSState;


    //


    //                         

    tANI_U8 htMaxRxAMpduFactor:2;

    //

    //                         

    //                                         

    //                       

    //

    tANI_U8 htSupportedChannelWidthSet:1;
    tANI_U8 htSecondaryChannelOffset:2;
    tANI_U8 rsvd1:3;


    //                                                                     

    //                                         

    //                                                                     

    tANI_U8 dpuSig:4;                       //              

    tANI_U8 staSig:4;                       //              

    tANI_U8 staType;


    tANI_U16 bssId;                          //      

    tANI_U16 assocId;                       //               




    //                                           

    tANI_U16 staIndex;

    tANI_U8    staAddr[6];

    /*                                                                          

                                              */

    /*                     */

    tANI_U8     ucUcastSig;


    /*                       */

    tANI_U8     ucBcastSig;


    //

    //                                               

    //                                                 

    //                       

    //                              

    //                                   

    //

    tCfgTrafficClass tcCfg[STACFG_MAX_TC];


    //                

    //                                      

    //                                                        

    /*                        

     

                                                                        

                                                                            

                                                                              

                   */




    //                              

    //                                               

    tANI_U32 baState;

#ifdef WLAN_FEATURE_11AC
    tANI_U8  vhtSupportedChannelWidthSet;
    tANI_U8  vhtBeamFormerCapable;
#endif

    tANI_U8 htLdpcCapable;
    tANI_U8 vhtLdpcCapable;

    /*                                                            

                                                                    

                                                                      

                            

     */

    struct sDphHashNode  *next;


} tDphHashNode, *tpDphHashNode;


#include "dphHashTable.h"


//                                                                    


//                                              

//                                                         

//                                                             

#define DPH_PROT_OVERRIDE_NONANI_PEER_GET(cfgval)    ((cfgval) & 1)

#define DPH_PROT_OVERRIDE_ANI_PEER_GET(cfgval) (((cfgval) & 2) >> 1)


typedef struct sAniSirDph

{

    //                       

    dphHashTableClass dphHashTable;

} tAniSirDph, *tpAniSirDph;


#endif





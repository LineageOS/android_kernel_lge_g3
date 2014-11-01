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
#ifndef _HALMSGAPI_H_
#define _HALMSGAPI_H_

#include "halTypes.h"
#include "sirApi.h"
#include "sirParams.h"

#define HAL_NUM_BSSID 2
/*                             */
#define BSS_OPERATIONAL_MODE_AP     0
#define BSS_OPERATIONAL_MODE_STA    1

/*                                   */
#define STA_ENTRY_SELF              0
#define STA_ENTRY_OTHER             1
#define STA_ENTRY_BSSID             2
#define STA_ENTRY_BCAST             3 //                                                     
#define STA_ENTRY_PEER              STA_ENTRY_OTHER
#ifdef FEATURE_WLAN_TDLS
#define STA_ENTRY_TDLS_PEER         4
#endif /*                   */

#define STA_ENTRY_TRANSMITTER       STA_ENTRY_SELF
#define STA_ENTRY_RECEIVER          STA_ENTRY_OTHER

#define HAL_STA_INVALID_IDX 0xFF
#define HAL_BSS_INVALID_IDX 0xFF

#define HAL_BSSPERSONA_INVALID_IDX 0xFF

#define WLAN_BSS_PROTECTION_ON  1
#define WLAN_BSS_PROTECTION_OFF 0

/*                                                  */
#define HAL_MAX_NUM_BCAST_STATIONS      1
#define HAL_MIN_BCAST_STA_INDEX     ((HAL_MAX_NUM_BCAST_STATIONS>0)?0:HAL_STA_INVALID_IDX)
#define HAL_MAX_BCAST_STA_INDEX     ((HAL_MAX_NUM_BCAST_STATIONS>0)?(HAL_MAX_NUM_BCAST_STATIONS - 1):HAL_STA_INVALID_IDX)
#define HAL_MIN_STA_INDEX           ((HAL_MAX_BCAST_STA_INDEX!=HAL_STA_INVALID_IDX)?(HAL_MAX_BCAST_STA_INDEX+1):0)
#define HAL_MAX_STA_INDEX           (HAL_NUM_STA)

/*                                                                       */
#define HAL_SELF_STA_PER_BSS        1
#define HAL_BCAST_STA_PER_BSS       1

//                   
#define HAL_INVALID_CHANNEL_ID 0

/*                                                                        
                                                                         
                                                                 
 */
#define BSSIDX_INVALID             254

#define HAL_IS_VALID_BSS_INDEX(pMac, bssIdx)  ((BSSIDX_INVALID != (bssIdx)) && ((bssIdx) < (pMac)->hal.memMap.maxBssids))

//                 
typedef __ani_attr_pre_packed struct sAniBeaconStruct
{
    tANI_U32        beaconLength;   //                            
    tSirMacMgmtHdr  macHdr;         //                      
    //                         
} __ani_attr_packed tAniBeaconStruct, *tpAniBeaconStruct;

//                            
typedef __ani_attr_pre_packed struct sAniProbeRspStruct
{
    tSirMacMgmtHdr  macHdr;         //                        
    //                           
} __ani_attr_packed tAniProbeRspStruct, *tpAniProbeRspStruct;


//                  
typedef struct
{
    tANI_U8 disableTx;
    tANI_U8 disableRx;
    tANI_U8 rxCompBA;              //                                                                     
    tANI_U8 rxBApolicy;            //                                                           
    tANI_U8 txCompBA;              //                                                
    tANI_U8 txBApolicy;            //                                                         
    tANI_U8 rxUseBA;
    tANI_U8 txUseBA;
    tANI_U8 rxBufferSize;
    tANI_U8 txBufferSize;
    tANI_U16 txBAWaitTimeout;
    tANI_U16 rxBAWaitTimeout;
} tTCParams;


typedef struct
{
    //                                                                   
    //             
    tSirMacAddr bssId;

    //                                                           
    //                   
    tANI_U16 assocId;

    //                                                                              
    //                                                       
    //                                                      
    tANI_U8 staType;       //                                    

    tANI_U8 shortPreambleSupported;

    //                   
    tSirMacAddr staMac;

    //                 
    tANI_U16 listenInterval;

    //                    
    tANI_U8 wmmEnabled;

    //
    //                        
    //                    
    //                        
    //                        
    //
    tANI_U8 uAPSD;

    //              
    tANI_U8 maxSPLen;

    //                   
    tANI_U8 htCapable;

    //                                 
    //                                 
    //                                                
    tANI_U8 greenFieldCapable;

    //                                             
    tANI_U8 txChannelWidthSet;

    //                
    tSirMacHTMIMOPowerSaveState mimoPS;

    //                               
    tANI_U8 rifsMode;

    //                                
    //                              
    //                            
    tANI_U8 lsigTxopProtection;

    //                   
    tANI_U8 delBASupport;
    //                          

#ifdef ANI_DVT_DEBUG
    //                                                           
    //                             
    tANI_U8 primaryRateIndex, secondaryRateIndex, tertiaryRateIndex;
    tANI_U8 primaryRateIndex40, secondaryRateIndex40, tertiaryRateIndex40;
#endif

    //      
    //                           
    tANI_U8 us32MaxAmpduDuration;                //                  
    tANI_U8 maxAmpduSize;                        //                                   
    tANI_U8 maxAmpduDensity;                     //                                 
    tANI_U8 maxAmsduSize;                        //                               

    //              
    tTCParams staTCParams[STACFG_MAX_TC];

    //                                          
    tANI_U16 deCompEnable;
    tANI_U16 compEnable;
    tANI_U16 concatSeqRmv;
    tANI_U16 concatSeqIns;


    //              

    /* 
                                                       
                                                          
                                                           
                                           
      */
    tANI_U8 fDsssCckMode40Mhz;


    //                                  
    tANI_U8 fShortGI40Mhz;

    //                                  
    tANI_U8 fShortGI20Mhz;



    /*
                                                
                                                                      
     */
    tSirSupportedRates supportedRates;





    /*
                                                                                     
                                                    
     */
    //                                                          
    eHalStatus status;
    //                                                                           
    tANI_U8 staIdx;

    //                                                
    //                                                                     
    //                                                                             
    //                                                                   
    //                                     
    tANI_U8 bssIdx;

    /*                                                                                           
                                                                      
                                                                    
       */

   //                                                              
   //                                                                    
   //                                       
    tANI_U8 updateSta;
    //                                                              
    tANI_U8 respReqd;

    /*                                                */
    tANI_U8 rmfEnabled;

    /*                                                */
    tANI_U32 encryptType;
    
    /*                                                                          
                                              */
    /*                 */
    tANI_U8     ucUcastSig;

    /*                   */
    tANI_U8     ucBcastSig;

    tANI_U8     sessionId; //                                     
    //                                      

    /*                            */
    tANI_U8     p2pCapableSta;

#ifdef WLAN_FEATURE_11AC
    tANI_U8    vhtCapable;
    tANI_U8    vhtTxChannelWidthSet;
    tANI_U8    vhtTxBFCapable;
#endif

    tANI_U8    htLdpcCapable;
    tANI_U8    vhtLdpcCapable;
} tAddStaParams, *tpAddStaParams;


typedef struct
{
    //              
    tANI_U16 staIdx;
    tANI_U16 templIdx;
    tANI_U8   rateIdx;

    //                                                                     
    eHalStatus status;

    //                                                              
    tANI_U8 respReqd;

} tUpdateTxCmdTemplParams, *tpUpdateTxCmdTemplParams;
//                                








typedef struct
{
    //                                                                       
    //                      
    tANI_U16 staIdx;
    tANI_U16 assocId;
    eHalStatus  status;    //                                                  
    tANI_U8 respReqd;
    tANI_U8     sessionId; //                                      
    //                                                      
    //                              
} tDeleteStaParams, * tpDeleteStaParams;

/*
                                                                          
                                                                       
                                                                        
                                                          
 */
typedef struct
{
    tANI_U16        staIdx;
    tAniEdType      encType;        //                           
    tAniWepType     wepType;        //                   
    tANI_U8         defWEPIdx;      //                                                                 
    tSirKeys        key[SIR_MAC_MAX_NUM_OF_DEFAULT_KEYS];            //                                          
    tANI_U8         singleTidRc;    //                                                    
    /*
                                                  
                                                    
     */
    eHalStatus  status;    //                                                  
    tANI_U8     sessionId; //                                      

    //                                                      
    //                                             
} tSetStaKeyParams, *tpSetStaKeyParams;

//
//                                  
//                                
//
typedef struct
{
    //                  
    tSirMacAddr bssId;
#ifdef HAL_SELF_STA_PER_BSS
    //                 
    tSirMacAddr  selfMacAddr;
#endif
    //         
    //                                                                 
    tSirBssType bssType;

    //                  
    tANI_U8 operMode;

    //                                                 
    //                                              
    //                                   
    tSirNwType nwType;

    tANI_U8 shortSlotTimeSupported;
    tANI_U8 llaCoexist;
    tANI_U8 llbCoexist;
    tANI_U8 llgCoexist;
    tANI_U8 ht20Coexist;
    tANI_U8 llnNonGFCoexist;
    tANI_U8 fLsigTXOPProtectionFullSupport;
    tANI_U8 fRIFSMode;

    //                
    tSirMacBeaconInterval beaconInterval;

    //            
    tANI_U8 dtimPeriod;

    //             
    //                                   
    tSirMacCfParamSet cfParamSet;

    //             
    //                                   
    tSirMacRateSet rateSet;

    //                                               

    //                               
    tANI_U8 htCapable;

    //                               
    tANI_U8 obssProtEnabled;

    //                     
    tANI_U8 rmfEnabled;

    //                  
    //                                   
    tSirMacHTOperatingMode htOperMode;

    //                                          
    tANI_U8 dualCTSProtection;

    //                                             
    tANI_U8 txChannelWidthSet;

    //                          
    tANI_U8 currentOperChannel;

    //                                         
    tANI_U8 currentExtChannel;

    //                               
    //                                               
    //                                                                     
    tAddStaParams staContext;

    /*
                                                                                    
                                                   
    */
    //                                                          
    eHalStatus status;
    //                            
    //                                                      
    tANI_U16 bssIdx;

    //                                                                                          
    //                                                      
    tANI_U8    bcastDpuDescIndx;

    //                                                         
    //                                                      
    tANI_U8    bcastDpuSignature;

    //                                                                               
    tANI_U8    mgmtDpuDescIndx;

    //                                                            
    tANI_U8    mgmtDpuSignature;

   //                                                              
   //                                                                    
   //                                       
    tANI_U8 updateBss;

    //                                           
    tSirMacSSid ssId;

    //                                                                     
    //                                                           
    tANI_U8 respReqd;
    tANI_U8     sessionId; //                                      
    //                                                      
    //                                    

#if defined WLAN_FEATURE_VOWIFI
    tPowerdBm txMgmtPower; //                                                             
    tPowerdBm maxTxPower;  //                                                                
#endif

#if defined WLAN_FEATURE_VOWIFI_11R
    tANI_U8 extSetStaKeyParamValid; //                         
    tSetStaKeyParams extSetStaKeyParam;  //                               
#endif

    tANI_U8   ucMaxProbeRespRetryLimit;  //                          
    tANI_U8   bHiddenSSIDEn;             //                            
    tANI_U8   bProxyProbeRespEn;         //                                     
    tANI_U8   halPersona;         //                                                                       

    //                                                          
    tANI_U8 bSpectrumMgtEnabled;
#ifdef WLAN_FEATURE_11AC
    tANI_U8 vhtCapable;
    tANI_U8    vhtTxChannelWidthSet;
#endif
} tAddBssParams, * tpAddBssParams;

typedef struct
{
    tANI_U8 bssIdx;
    //                                                             
    eHalStatus status;
    //                                                                     
    //                                                           
    tANI_U8 respReqd;
    tANI_U8     sessionId; //                                      
                           //                              
    tSirMacAddr bssid; //                                       
} tDeleteBssParams, * tpDeleteBssParams;

//
//                         
//                                                                                   
//                    
//                        
//                            

typedef struct
{
    tANI_U8 staIdx;
    tANI_U8 uapsdACMask; 
    tANI_U8 maxSpLen;    
} tUpdateUapsdParams, * tpUpdateUapsdParams;

typedef struct sSirScanEntry
{
    tANI_U8 bssIdx[HAL_NUM_BSSID];
    tANI_U8 activeBSScnt;
}tSirScanEntry, *ptSirScanEntry;

//
//                                  
//                                  
//
typedef struct {

    eHalSysMode scanMode;

    tSirMacAddr bssid;

    tANI_U8 notifyBss;

    tANI_U8 useNoA;

    //                                                              
    tANI_U8 notifyHost;

    tANI_U8 frameLength;
    tANI_U8 frameType;     //                         

    //                                    
    tANI_U16 scanDuration;

    //                                                      
    tSirMacMgmtHdr macMgmtHdr;

    tSirScanEntry scanEntry;

    //                                                                       
    tSirLinkTrafficCheck    checkLinkTraffic;

    /*
                                                                                     
                                                    
     */
    //                                                            
    eHalStatus status;

} tInitScanParams, * tpInitScanParams;

typedef enum  eDelStaReasonCode{
   HAL_DEL_STA_REASON_CODE_KEEP_ALIVE = 0x1,
   HAL_DEL_STA_REASON_CODE_TIM_BASED  = 0x2,
   HAL_DEL_STA_REASON_CODE_RA_BASED   = 0x3,
   HAL_DEL_STA_REASON_CODE_UNKNOWN_A2 = 0x4
}tDelStaReasonCode;

//
//                                 
//                                          
//
typedef struct {
    tANI_U16    assocId;
    tANI_U16    staId;
    tSirMacAddr bssId; //                      
                       //                                     
    tSirMacAddr addr2;        //  
    tANI_U16    reasonCode;   //                                                                                                                                                      
} tDeleteStaContext, * tpDeleteStaContext;


//
//                                  
//                                   
//                                                                            
//
typedef struct {

    //                                   
    tANI_U8 scanChannel;

    /*
                                                                                    
                                                   
    */
    //                                                             
    eHalStatus status;

#if defined WLAN_FEATURE_VOWIFI
    tANI_U32 startTSF[2];
    tPowerdBm txMgmtPower; //                                                             
#endif
} tStartScanParams, * tpStartScanParams;

//
//                                  
//                                 
//                                                                            
//
typedef struct {

    //                                   
    tANI_U8 scanChannel;

    /*
                                                                                    
                                                   
    */
    //                                                           
    eHalStatus status;

} tEndScanParams, * tpEndScanParams;

//
//                                  
//                                    
//
typedef struct {

    //                                                
    //                                                                       
    //                                      
    //                                            
    //
    //                                     

    eHalSysMode scanMode;

    tSirMacAddr bssid;

    //                          
    tANI_U8 currentOperChannel;

    //                                                                     
    //                                                
    ePhyChanBondState cbState;

    //                                                            
    //                                                 
    tANI_U8 notifyBss;

    tANI_U8 notifyHost;

    tANI_U8 frameLength;
    tANI_U8 frameType;     //                         

    //                                                      
    tSirMacMgmtHdr macMgmtHdr;

    tSirScanEntry scanEntry;

    /*
                                                                                    
                                                   
    */
    //                                                              
    eHalStatus status;

} tFinishScanParams, * tpFinishScanParams;

#ifdef FEATURE_OEM_DATA_SUPPORT 

#ifndef OEM_DATA_REQ_SIZE
#define OEM_DATA_REQ_SIZE 134
#endif
#ifndef OEM_DATA_RSP_SIZE
#define OEM_DATA_RSP_SIZE 1968
#endif

typedef struct
{
    tSirMacAddr          selfMacAddr;
    eHalStatus           status;
    tANI_U8              oemDataReq[OEM_DATA_REQ_SIZE];
} tStartOemDataReq, *tpStartOemDataReq;

typedef struct 
{
    tANI_U8             oemDataRsp[OEM_DATA_RSP_SIZE];
} tStartOemDataRsp, *tpStartOemDataRsp;
#endif

typedef struct sBeaconGenStaInfo {
    tANI_U16    assocId;
    tANI_U32    staTxAckCnt;
}tBeaconGenStaInfo, *tpBeaconGenStaInfo;
//
//                                  
//                                   
//

typedef struct sBeaconGenParams {
    //                                                               
    tANI_U8                 bssIdx;
    tSirMacAddr           bssId;
#ifdef FIXME_VOLANS
    tANI_U8                 numOfSta;                /*                                                        */
    tANI_U8                 numOfStaWithoutData; /*                                                                  */
    tANI_U8                 fBroadcastTrafficPending ;
    tANI_U8                 dtimCount;
#endif
    tANI_U8                 rsvd[3];                /*                                                                    
                                                                                  */
/*                                                                                              
                                                                                 */
} tBeaconGenParams, * tpBeaconGenParams;

typedef struct {
    tSirMacAddr bssId;
    tANI_U8 *beacon;     //             
    tANI_U32 beaconLength; //                       
    tANI_U32 timIeOffset; //                                                 
    tANI_U16 p2pIeOffset; //                                               
} tSendbeaconParams, * tpSendbeaconParams;

typedef struct sSendProbeRespParams {
    tSirMacAddr bssId;
    tANI_U8      *pProbeRespTemplate; 
    tANI_U32     probeRespTemplateLen;
    tANI_U32     ucProxyProbeReqValidIEBmap[8];
} tSendProbeRespParams, * tpSendProbeRespParams;

/*
                                                                  
 */
typedef struct
{
    tANI_U8         bssIdx;
    tAniEdType      encType;
    tANI_U8         numKeys;
    tSirKeys        key[SIR_MAC_MAX_NUM_OF_DEFAULT_KEYS];
    tANI_U8         singleTidRc;    //                                                    
    /*
                                                  
                                                    
     */
    eHalStatus  status;     //                                                  
    tANI_U8     sessionId;  //                                      
                            //                                          
} tSetBssKeyParams, *tpSetBssKeyParams;

/*
                                                                       
 */
typedef struct
{
    tANI_U16         staIdx;
    tAniEdType      encType;    //                           
    tANI_U8           keyId;
    tANI_BOOLEAN    unicast;
    /*
                                                  
                                                    
     */
    eHalStatus  status;     //                                            
    tANI_U8     sessionId;  //                                      
                            //                                
                            //                
} tRemoveStaKeyParams, *tpRemoveStaKeyParams;

/*
                                                     
 */
typedef struct
{
    tANI_U8         bssIdx;
    tAniEdType     encType;
    tANI_U8         keyId;
    tANI_U8         wepType;
    /*
                                                  
                                                    
     */
    eHalStatus  status;    //                                            
    tANI_U8     sessionId; //                                      
                           //                                
                           //                
} tRemoveBssKeyParams, *tpRemoveBssKeyParams;

typedef struct
{
    //                                        
    tANI_U16 staIdx;
    tANI_U8 encMode;
    //                                                            
    eHalStatus status;
    //                      
    tANI_U32  sendBlocks;
    tANI_U32  recvBlocks;
    tANI_U32  replays;
    tANI_U8   micErrorCnt;
    tANI_U32  protExclCnt;
    tANI_U16  formatErrCnt;
    tANI_U16  unDecryptableCnt;
    tANI_U32  decryptErrCnt;
    tANI_U32  decryptOkCnt;

} tDpuStatsParams, * tpDpuStatsParams;


/*
                                               
 */
typedef struct
{
    tANI_U16        staIdx;
    /*
                                                  
                                                    
     */
    //                                                             
    eHalStatus status;
    tANI_U8    dpuDescIndx;
    tANI_U8    dpuSignature;
} tGetDpuParams, *tpGetDpuParams;



//                                  
typedef struct
{

    tANI_U8  bssIdx;

    //                                                               
    //                     
    tANI_U8 fShortPreamble;
    //                
    tANI_U8 fShortSlotTime;
    //               
    tANI_U16 beaconInterval;
    //                  
    tANI_U8 llaCoexist;
    tANI_U8 llbCoexist;
    tANI_U8 llgCoexist;
    tANI_U8 ht20MhzCoexist;
    tANI_U8 llnNonGFCoexist;
    tANI_U8 fLsigTXOPProtectionFullSupport;
    tANI_U8 fRIFSMode;

    tANI_U16 paramChangeBitmap;
}tUpdateBeaconParams, *tpUpdateBeaconParams;

#ifdef WLAN_FEATURE_11AC
typedef struct 
{
   tANI_U16   opMode;
   tANI_U16  staId;
}tUpdateVHTOpMode, *tpUpdateVHTOpMode;
#endif

//                              
typedef struct
{

    tANI_U8  bssIdx;

    /*
                                                                                                       
                                                                                 
    */
    tANI_U8  cfpCount;

    /*                                                                             */
    tANI_U8 cfpPeriod;

}tUpdateCFParams, *tpUpdateCFParams;



//                                
//                                                                                       
//                                 

/*
              
 
                    


                                                                                                          
                                                                        
      
                      


                                                                                                              
                                                                                        
      
                       

                                        
*/


//                                
typedef struct
{
    tANI_U8 channelNumber;
#ifndef WLAN_FEATURE_VOWIFI    
    tANI_U8 localPowerConstraint;
#endif /*                      */
    ePhyChanBondState secondaryChannelOffset;
    tANI_U8 peSessionId;
#if defined WLAN_FEATURE_VOWIFI
    tPowerdBm txMgmtPower; //                                                             
    tPowerdBm maxTxPower;
    tSirMacAddr selfStaMacAddr;
                        //                                                                              
#endif
    /*                                                                                        
                                                                                  */
    /*                                                                                            
                                                                                                           
     */
    tSirMacAddr bssId;

    eHalStatus status;

}tSwitchChannelParams, *tpSwitchChannelParams;

typedef void (*tpSetLinkStateCallback)(tpAniSirGlobal pMac, void *msgParam );

typedef struct sLinkStateParams
{
    //                       
    tSirMacAddr bssid;
    tSirMacAddr selfMacAddr;
    tSirLinkState state;
    tpSetLinkStateCallback callback;
    void *callbackArg;
#ifdef WLAN_FEATURE_VOWIFI_11R
    int ft;
    void * session;
#endif
} tLinkStateParams, * tpLinkStateParams;


typedef struct
{
  tANI_U16 staIdx;
  tANI_U16 tspecIdx; //                                                             
  tSirMacTspecIE   tspec;
  eHalStatus       status;
  tANI_U8          sessionId;          //                                     
} tAddTsParams, *tpAddTsParams;

typedef struct
{
  tANI_U16 staIdx;
  tANI_U16 tspecIdx; //                                                                
  tSirMacAddr bssId; //                 
  
} tDelTsParams, *tpDelTsParams;

#ifdef WLAN_FEATURE_VOWIFI_11R

#define HAL_QOS_NUM_TSPEC_MAX 2
#define HAL_QOS_NUM_AC_MAX 4

typedef struct
{
  tANI_U16 staIdx;
  tANI_U16 tspecIdx; //                                                             
  tSirMacTspecIE   tspec[HAL_QOS_NUM_AC_MAX];
  eHalStatus       status[HAL_QOS_NUM_AC_MAX];
  tANI_U8          sessionId;          //                                     
}tAggrAddTsParams, *tpAggrAddTsParams;

#endif /*                         */


typedef tSirRetStatus (*tHalMsgCallback)(tpAniSirGlobal pMac, tANI_U32 mesgId, void *mesgParam );


typedef struct
{
  tANI_U16 bssIdx;
  tANI_BOOLEAN highPerformance;
  tSirMacEdcaParamRecord acbe; //            
  tSirMacEdcaParamRecord acbk; //           
  tSirMacEdcaParamRecord acvi; //      
  tSirMacEdcaParamRecord acvo; //      
} tEdcaParams, *tpEdcaParams;

/*
                     
*/

eHalStatus halMsg_setPromiscMode(tpAniSirGlobal pMac);


//
//                                  
//                              
//
typedef struct sAddBAParams
{

    //              
    tANI_U16 staIdx;

    //                 
    tSirMacAddr peerMacAddr;

    //                                
    //                                   
    tANI_U8 baDialogToken;

    //                                    
    //                                         
    tANI_U8 baTID;

    //                               
    //                 
    tANI_U8 baPolicy;

    //                                                     
    //                                                    
    //                                                   
    //                                                   
    //                                                    
    //                                               
    tANI_U16 baBufferSize;

    //                   
    //                              
    tANI_U16 baTimeout;

    //                                           
    //                                                 
    //                           
    tANI_U16 baSSN;

    //                
    //               
    //              
    tANI_U8 baDirection;

    //
    //                                                   
    //                                                       
    //
    //                                                   
    //                                 
    eHalStatus status;

    //                                                          
    tANI_U8 respReqd;
    tANI_U8    sessionId; //                                      
                          //                                
                          //                

} tAddBAParams, * tpAddBAParams;


//
//                                  
//                              
//
typedef struct sDelBAParams
{

    //              
    tANI_U16 staIdx;

    //                                              
    tANI_U8 baTID;

    //                
    //               
    //              
    tANI_U8 baDirection;

    //                                        
    //                                     
    //
    //                                                   
    //                                                       
    //
    //                                                   
    //                                 
    //                  

} tDelBAParams, * tpDelBAParams;


//
//                                  
//                                   
//
typedef struct sSet_MIMOPS
{
    //              
    tANI_U16 staIdx;

    //                      
    tSirMacHTMIMOPowerSaveState htMIMOPSState;
    //                                                        
    //                                      
    eHalStatus status;
    tANI_U8     fsendRsp;

} tSetMIMOPS, * tpSetMIMOPS;


//
//                                  
//                                  
//
typedef struct sExitBmpsParams
{
    tANI_U8     sendDataNull;
    eHalStatus  status;
    tANI_U8     bssIdx;
} tExitBmpsParams, *tpExitBmpsParams;

//
//                                  
//                                    
//
typedef struct sUapsdParams
{
    tANI_U8     bkDeliveryEnabled:1;
    tANI_U8     beDeliveryEnabled:1;
    tANI_U8     viDeliveryEnabled:1;
    tANI_U8     voDeliveryEnabled:1;
    tANI_U8     bkTriggerEnabled:1;
    tANI_U8     beTriggerEnabled:1;
    tANI_U8     viTriggerEnabled:1;
    tANI_U8     voTriggerEnabled:1;
    eHalStatus  status;
    tANI_U8     bssIdx;
}tUapsdParams, *tpUapsdParams;

//
//                                  
//                                   
//
typedef struct sExitUapsdParams
{
    eHalStatus  status;
    tANI_U8     bssIdx;
}tExitUapsdParams, *tpExitUapsdParams;

//
//                                  
//                               
//
typedef struct sBADeleteParams
{

    //              
    tANI_U16 staIdx;

    //                                                 
    tSirMacAddr peerMacAddr;

    //                                                      
    tANI_U8 baTID;

    //                
    //               
    //              
    tANI_U8 baDirection;

    tANI_U32 reasonCode;

    tSirMacAddr  bssId; //                      
                        //                                  
} tBADeleteParams, * tpBADeleteParams;


//                               
typedef struct sBaActivityInd
{
    tANI_U16 baCandidateCnt;
    //                                                                  

    tSirMacAddr  bssId; //                      
} tBaActivityInd, * tpBaActivityInd;


//                                             
typedef struct sIbssPeerInactivityInd
{
   tANI_U8     bssIdx;
   tANI_U8     staIdx;
   tSirMacAddr staAddr;
}tIbssPeerInactivityInd, *tpIbssPeerInactivityInd;


typedef struct tHalIndCB
{

    tHalMsgCallback pHalIndCB;

}tHalIndCB,*tpHalIndCB;

/*                                                                               */
#define HALMSG_NUMBYTES_STATION_BITMAP(x) (((x / 32) + ((x % 32)?1:0)) * 4)

typedef struct sControlTxParams
{
    tANI_BOOLEAN stopTx;
    /*                                                                       
                                                 
     */
    tANI_U8 fCtrlGlobal;
    /*                                           */
    tANI_U8 ctrlSta;
    /*                                                          */
    tANI_U8 ctrlBss;

    /*                                                                      
                                            
                                                 
     */
    tANI_U32 bssBitmap;
    /*                                                                      
                                                   
     */
    tANI_U32 beaconBitmap;

    /* 
                                                                      
                                        
     */
} tTxControlParams, * tpTxControlParams;

typedef struct sEnterBmpsParams
{
    //                                       
    tANI_U8         bssIdx;
    tANI_U64 tbtt;
    tANI_U8 dtimCount;
    //                                                             
    //                                                      
    tANI_U8 dtimPeriod;

    //                        
    tANI_U8  bRssiFilterEnable;
    tANI_U32 rssiFilterPeriod;
    tANI_U32 numBeaconPerRssiAverage;

    eHalStatus status;
    tANI_U8 respReqd;
}tEnterBmpsParams, *tpEnterBmpsParams;

//             
typedef struct sEnterBmpsRspParams
{
    /*                    */
    tANI_U32   status;
    tANI_U8    bssIdx;
}tEnterBmpsRspParams, *tpEnterBmpsRspParams;
//
//                                  
//                                         
//
typedef struct sMaxTxPowerParams
{
    tSirMacAddr bssId;  //                                                                   
                        //                                                                              
    tSirMacAddr selfStaMacAddr;
    //           
    //                                
    //            
    //                                             
    tPowerdBm  power;
}tMaxTxPowerParams, *tpMaxTxPowerParams;

typedef struct sMaxTxPowerPerBandParams
{
    eCsrBand   bandInfo;
    tPowerdBm  power;
}tMaxTxPowerPerBandParams, *tpMaxTxPowerPerBandParams;

typedef struct sAddStaSelfParams
{
   tSirMacAddr selfMacAddr;
   tVOS_CON_MODE currDeviceMode;
   tANI_U32 status;
}tAddStaSelfParams, *tpAddStaSelfParams;

typedef struct sDelStaSelfParams
{
   tSirMacAddr selfMacAddr;

   tANI_U32 status;
}tDelStaSelfParams, *tpDelStaSelfParams;

typedef struct sP2pPsParams
{
   tANI_U8   opp_ps;
   tANI_U32  ctWindow;
   tANI_U8   count; 
   tANI_U32  duration;
   tANI_U32  interval;
   tANI_U32  single_noa_duration;
   tANI_U8   psSelection;
}tP2pPsParams, *tpP2pPsParams;

typedef struct sTdlsLinkEstablishParams
{
   tANI_U16  staIdx;
   tANI_U8   isResponder;
   tANI_U8   uapsdQueues;
   tANI_U8   maxSp;
   tANI_U8   isBufsta;
   tANI_U32  status;
}tTdlsLinkEstablishParams, *tpTdlsLinkEstablishParams;

static inline void halGetTxTSFtimer(tpAniSirGlobal pMac, 
                                                tSirMacTimeStamp *pTime)
{
}

extern void SysProcessMmhMsg(tpAniSirGlobal pMac, tSirMsgQ* pMsg);

/*                                  */
typedef __ani_attr_pre_packed struct sBeaconFilterMsg
{
    tANI_U16    capabilityInfo;
    tANI_U16    capabilityMask;
    tANI_U16    beaconInterval;
    tANI_U16    ieNum;
    tANI_U8     bssIdx;
    tANI_U8     reserved;
} __ani_attr_packed tBeaconFilterMsg, *tpBeaconFilterMsg;

typedef __ani_attr_pre_packed struct sEidByteInfo
{
    tANI_U8     offset;
    tANI_U8     value;
    tANI_U8     bitMask;
    tANI_U8     ref;
} __ani_attr_packed tEidByteInfo, *tpEidByteInfo;


/*                                                                      
          */
typedef __ani_attr_pre_packed struct sBeaconFilterIe
{
    tANI_U8         elementId;
    tANI_U8         checkIePresence;
    tEidByteInfo    byte;
} __ani_attr_packed tBeaconFilterIe, *tpBeaconFilterIe;

typedef __ani_attr_pre_packed struct sRemBeaconFilterMsg  
{
    tANI_U8  ucIeCount;
    tANI_U8  ucRemIeId[1];
}  __ani_attr_packed tRemBeaconFilterMsg, *tpRemBeaconFilterMsg;

#endif /*               */


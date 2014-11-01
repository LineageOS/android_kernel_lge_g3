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

#if !defined( __LIM_SESSION_H )
#define __LIM_SESSION_H


/*                                                                          

                     

                                               

                      
                                                                          */


/*                                                                          
               
                                                                          */



/*                                                                           
                                        
                                                                          */
#define NUM_WEP_KEYS 4

/*                                                                           
                   
                                                                          */
typedef struct 
{
    tSirMacBeaconInterval   beaconInterval;
    tANI_U8                 fShortPreamble;   
    tANI_U8                 llaCoexist;    
    tANI_U8                 llbCoexist;
    tANI_U8                 llgCoexist;
    tANI_U8                 ht20Coexist;
    tANI_U8                 llnNonGFCoexist;
    tANI_U8                 fRIFSMode;
    tANI_U8                 fLsigTXOPProtectionFullSupport;
    tANI_U8                 gHTObssMode; 
}tBeaconParams, *tpBeaconParams;

typedef struct sPESession           //                        
{
    /*                                         */
    tANI_U8                 available;
    tANI_U8                 peSessionId;
    tANI_U8                 smeSessionId;
    tANI_U16                transactionId;

    //                                                   
    //                                   
    tSirMacAddr             bssId;
    tSirMacAddr             selfMacAddr;
    tSirMacSSid             ssId;
    tANI_U8                 bssIdx;
    tANI_U8                 valid;
    tLimMlmStates           limMlmState;            //         
    tLimMlmStates           limPrevMlmState;        //                  
    tLimSmeStates           limSmeState;            //         
    tLimSmeStates           limPrevSmeState;        //                  
    tLimSystemRole          limSystemRole;
    tSirBssType             bssType;
    tANI_U8                 operMode;               //                   
    tSirNwType              nwType;
    tpSirSmeStartBssReq     pLimStartBssReq;        //                          
    tpSirSmeJoinReq         pLimJoinReq;            //                       
    tpSirSmeJoinReq         pLimReAssocReq;         //                         
    tpLimMlmJoinReq         pLimMlmJoinReq;         //                      
#if  defined (WLAN_FEATURE_VOWIFI_11R) || defined (FEATURE_WLAN_CCX) || defined(FEATURE_WLAN_LFR)
    void                    *pLimMlmReassocRetryReq; //                         
#endif
    void                    *pLimMlmReassocReq;      //                         
    tANI_U16                channelChangeReasonCode;
    tANI_U8                 dot11mode;
    tANI_U8                 htCapability;
    /*                                                */
    tANI_U8                 htSupportedChannelWidthSet;
    /*                         
                                               
                                                                      
     */
    tANI_U8                 htRecommendedTxWidthSet;
    /*                                         */
    ePhyChanBondState       htSecondaryChannelOffset;
    tSirRFBand              limRFBand;
    tANI_U8                 limIbssActive;          //                      

    /*                                                                                  */
    tAniAuthType            limCurrentAuthType;
    tANI_U16                limCurrentBssCaps;
    tANI_U8                 limCurrentBssQosCaps;
    tANI_U16                limCurrentBssPropCap;
    tANI_U8                 limSentCapsChangeNtf;
    tANI_U16                limAID;

    /*                               */
    tSirMacAddr             limReAssocbssId;
    tSirMacChanNum          limReassocChannelId;
    /*                                                                                             */
    tANI_U8                 reAssocHtSupportedChannelWidthSet;
    tANI_U8                 reAssocHtRecommendedTxWidthSet;
    ePhyChanBondState       reAssocHtSecondaryChannelOffset;
    tSirMacSSid             limReassocSSID;
    tANI_U16                limReassocBssCaps;
    tANI_U8                 limReassocBssQosCaps;
    tANI_U16                limReassocBssPropCap;

    //                                     
    void                   *limAssocResponseData;
    


    /*                        */


    /*
                                                         
                                            
                                                                           
    */
    tANI_U16                staId;
    tANI_U16                statypeForBss;          //                                   
    tANI_U8                 shortSlotTimeSupported;
    tANI_U8                 dtimPeriod;
    tSirMacRateSet       rateSet;
    tSirMacRateSet       extRateSet;
    tSirMacHTOperatingMode  htOperMode;
    tANI_U8                 currentOperChannel;
    tANI_U8                 currentReqChannel;
    tANI_U8                 LimRxedBeaconCntDuringHB;
    
    //                                                                              
    tANI_U64                lastBeaconTimeStamp;
    //                                                              
    tANI_U32                currentBssBeaconCnt;
    tANI_U8                 lastBeaconDtimCount;
    tANI_U8                 lastBeaconDtimPeriod;

    tANI_U32                bcnLen;
    tANI_U8                 *beacon;                //                                                        

    tANI_U32                assocReqLen;
    tANI_U8                 *assocReq;              //                                                                   

    tANI_U32                assocRspLen;
    tANI_U8                 *assocRsp;              //                                                             
    tAniSirDph              dph;
    void *                  *parsedAssocReq;        //                                                              
#ifdef WLAN_FEATURE_VOWIFI_11R    
    tANI_U32                RICDataLen;             //                                                         
    tANI_U8                 *ricData;
#endif
#ifdef FEATURE_WLAN_CCX    
    tANI_U32                tspecLen;               //                                                          
    tANI_U8                 *tspecIes;
#endif
    tANI_U32                encryptType;

    tANI_BOOLEAN            bTkipCntrMeasActive;    //                                                        

    tANI_U8                 gLimProtectionControl;  //                       

    tANI_U8                 gHTNonGFDevicesPresent;

    //                               
    tCfgProtection          cfgProtection;

    //                                 
    tLimProtStaParams          gLim11bParams;

    //                              
    tLimProtStaParams          gLim11aParams;

    //                                            
    tLimProtStaParams          gLim11gParams;

    //                              
    tLimProtStaParams       gLimNonGfParams;

    //                               
    tLimProtStaParams          gLimHt20Params;

    //                                                 
    tLimProtStaParams          gLimLsigTxopParams;

    //                                                  
    tLimNoShortParams         gLimNoShortParams;

    //                                                   
    tLimNoShortSlotParams   gLimNoShortSlotParams;


    //                
    tLimProtStaParams  gLimOlbcParams;

    //                
    tLimProtStaParams  gLimOverlap11gParams;

    tLimProtStaParams  gLimOverlap11aParams;
    tLimProtStaParams gLimOverlapHt20Params;
    tLimProtStaParams gLimOverlapNonGfParams;

    //                      
    tCacheParams     protStaCache[LIM_PROT_STA_CACHE_SIZE];

    tANI_U8                 privacy;
    tAniAuthType            authType;
    tSirKeyMaterial         WEPKeyMaterial[NUM_WEP_KEYS];

    tDot11fIERSN            gStartBssRSNIe;
    tDot11fIEWPA            gStartBssWPAIe;
    tSirAPWPSIEs            APWPSIEs;
    tANI_U8                 apUapsdEnable;
    tSirWPSPBCSession       *pAPWPSPBCSession;
    tANI_U32                DefProbeRspIeBitmap[8];
    tANI_U32                proxyProbeRspEn;
    tDot11fProbeResponse    probeRespFrame;
    tANI_U8                 ssidHidden;
    tANI_BOOLEAN            fwdWPSPBCProbeReq;
    tANI_U8                 wps_state;

    tANI_U8            limQosEnabled:1; //   
    tANI_U8            limWmeEnabled:1; //   
    tANI_U8            limWsmEnabled:1; //   
    tANI_U8            limHcfEnabled:1;
    tANI_U8            lim11dEnabled:1;
#ifdef WLAN_FEATURE_11W
    tANI_U8            limRmfEnabled:1; //   
#endif
    tANI_U32           lim11hEnable;

    tPowerdBm  maxTxPower;   //                                           
    tVOS_CON_MODE      pePersona;
#if defined WLAN_FEATURE_VOWIFI
    tPowerdBm  txMgmtPower;
#endif

#ifdef WLAN_FEATURE_VOWIFI_11R
    tAniBool            is11Rconnection;
#endif

#ifdef FEATURE_WLAN_CCX
    tAniBool            isCCXconnection;
    tCcxPEContext       ccxContext;
#endif
#if defined WLAN_FEATURE_VOWIFI_11R || defined FEATURE_WLAN_CCX || defined(FEATURE_WLAN_LFR)
    tAniBool            isFastTransitionEnabled;
#endif
#ifdef FEATURE_WLAN_LFR
    tAniBool            isFastRoamIniFeatureEnabled;
#endif
    tSirNoAParam p2pNoA;
    tSirP2PNoaAttr p2pGoPsUpdate;
    tANI_U32 defaultAuthFailureTimeout;
    tSirP2PNoaStart p2pGoPsNoaStartInd;

    /*                    
                                                                            
                                                                               
                                                                           
                                            
      
                                                                           
                                     
      
                                                                            
                                                                          
                                                 
     */
    tSirMacEdcaParamRecord gLimEdcaParams[MAX_NUM_AC];   //             
    tSirMacEdcaParamRecord gLimEdcaParamsBC[MAX_NUM_AC]; //                  
    tSirMacEdcaParamRecord gLimEdcaParamsActive[MAX_NUM_AC]; 

    tANI_U8  gLimEdcaParamSetCount;

    tBeaconParams beaconParams;
#ifdef WLAN_FEATURE_11AC
    tANI_U8 vhtCapability;
    tANI_U8 vhtTxChannelWidthSet;
    tLimOperatingModeInfo  gLimOperatingMode;
    tLimWiderBWChannelSwitchInfo  gLimWiderBWChannelSwitch;
    tANI_U8    vhtCapabilityPresentInBeacon;
    tANI_U8    apCenterChan;
    tANI_U8    apChanWidth;
    tANI_U8    txBFIniFeatureEnabled;
#endif
    tANI_U8            spectrumMgtEnabled;
    /*                                                             */
    //                                 
    tLimSpecMgmtInfo   gLimSpecMgmt;
    //                                         
    tLimChannelSwitchInfo  gLimChannelSwitch;
    /*                                                                 */

    /*                                                        */
    tANI_BOOLEAN LimHBFailureStatus;
    tANI_U32           gLimPhyMode;
    tANI_U8            amsduSupportedInBA;
    tANI_U8          txLdpcIniFeatureEnabled;
    /* 
                                                              
                                                              
                      
     */
    tANI_U8    *gpLimPeerIdxpool;
    tANI_U8    freePeerIdxHead;
    tANI_U8    freePeerIdxTail;
    tANI_U16  gLimNumOfCurrentSTAs;
#ifdef FEATURE_WLAN_TDLS
    tANI_U32  peerAIDBitmap[2];
#endif
    tANI_BOOLEAN fWaitForProbeRsp;
    tANI_BOOLEAN fIgnoreCapsChange;
    tANI_U8    oxygenNwkIniFeatureEnabled;
    tANI_BOOLEAN fDeauthReceived;
#ifdef FEATURE_WLAN_DIAG_SUPPORT_LIM
    tANI_S8 rssi;
#endif
    tANI_U8 isAmsduSupportInAMPDU;
    tANI_BOOLEAN isCiscoVendorAP;
}tPESession, *tpPESession;

#define LIM_MAX_ACTIVE_SESSIONS 4


/*                                                                          
                                        
                                                                          */


/*                                                                          
  
                                                                     

                                                                              
                                                                     
    
                                                                   
                                                           
                                                                                   
  
                                                                                                      
  
     
  
                                                                            */
tpPESession peCreateSession(tpAniSirGlobal pMac, tANI_U8 *bssid , tANI_U8* sessionId, tANI_U16 numSta);


/*                                                                          
                                                                          

                                                                              
                                                                    
    
                                                                   
                                                       
                                                                                  
  
                                                                                                
  
     
                                                                            */
tpPESession peFindSessionByBssid(tpAniSirGlobal pMac,  tANI_U8*  bssid,    tANI_U8* sessionId);



/*                                                                          
                                                                            

                                                           
                                                                     
                                                                    
                                                            
                                                                                                
     
                                                                            */
tpPESession peFindSessionByBssIdx(tpAniSirGlobal pMac,  tANI_U8 bssIdx);




/*                                                                          
                                                                                           

                                                                              
                                                                                  
    
                                                                   
                                                               
                                                                                  
  
                                                                                                
  
     
                                                                            */
tpPESession peFindSessionByPeerSta(tpAniSirGlobal pMac, tANI_U8*  sa, tANI_U8* sessionId);

/*                                                                          
                                                                                   

                                                            
                                                                         
    
                                                                   
                                                                                           
  
                                                                                                
  
     
                                                                            */
 tpPESession peFindSessionBySessionId(tpAniSirGlobal pMac , tANI_U8 sessionId);

/*                                                                          
                                                                      

                                                                             
                                                                    
   
                                                                  
                                                     
                                                                                  

                                                                                                

                                                                          */
 tpPESession peFindSessionByStaId(tpAniSirGlobal pMac,  tANI_U8  staid,    tANI_U8* sessionId);
 




/*                                                                          
                                                                         

    
                                                                   
                                                                                    
    
     
                                                                            */
void peDeleteSession(tpAniSirGlobal pMac, tpPESession psessionEntry);


/*                                                                          
                                                                            

    
                                                                   
                                                                                    
    
     
                                                                            */


#endif //                               






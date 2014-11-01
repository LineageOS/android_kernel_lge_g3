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

#ifndef WLAN_QCT_WLANSAP_INTERNAL_H
#define WLAN_QCT_WLANSAP_INTERNAL_H

/*===========================================================================

               W L A N   S A P  P A L   L A Y E R 
                       I N T E R N A L  A P I


DESCRIPTION
  This file contains the internal API exposed by the wlan SAP PAL layer 
  module.


  Copyright (c) 2008 QUALCOMM Incorporated. All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/


/*                                                                           

                                           


                                                                       
                                                                


                                                                                                                                                                                                                                  


                                          
                                                                                   
                                      

                                                                           */



/*                                                                           

                                                  

                                                                           */

/*                                                                            
                
                                                                            */
#include "vos_api.h" 
#include "vos_packet.h" 

//                                
#include "csrApi.h"
#include "sapApi.h"
#include "sapFsm_ext.h"
#include "sapChSelect.h"

/*                                                                            
                                         
                                                                            */
 #ifdef __cplusplus
 extern "C" {
 #endif 
 

/*                                                                            
           
                                                                            */
#define SAP_DEBUG
//                                                       
#define WLANSAP_SECURITY_ENABLED_STATE VOS_TRUE
//                                             
#define VOS_GET_SAP_CB(ctx) vos_get_context( VOS_MODULE_ID_SAP, ctx) 

#define VOS_GET_HAL_CB(ctx) vos_get_context( VOS_MODULE_ID_PE, ctx) 
//                  
#define ANI_EAPOL_KEY_RSN_NONCE_SIZE      32

extern sRegulatoryChannel *regChannels;
extern const tRfChannelProps rfChannels[NUM_RF_CHANNELS];

/*                                                                            
            
                                                                            */
typedef struct sSapContext tSapContext;
//                            
/*                                                                            
                                                            
                                                                            */
/*                                                                            
                                       
                                                                            */
//                                                             
//                                            


/*                                      */
typedef enum {
    eSAP_DISCONNECTED,
    eSAP_CH_SELECT,
    eSAP_STARTING,
    eSAP_STARTED,
    eSAP_DISCONNECTING
} eSapFsmStates_t;

/*                                                                            
                                     
                                                                            */
 /*                                                                            
                                   
                                                                            */
/*                */
typedef struct sSapQosCfg {
    v_U8_t              WmmIsEnabled;
} tSapQosCfg;

typedef struct sSapContext {

    vos_lock_t          SapGlobalLock;

    //                                  
    v_U32_t             channel;
 
    //                                    
    v_U8_t              sessionId;

    //                                                
    v_U8_t              key_type;
    v_U8_t              key_length;
    v_U8_t              key_material[32];

    //                                       
    v_U8_t              self_mac_addr[VOS_MAC_ADDRESS_LEN];

    //           
    v_U8_t              ownSsid[MAX_SSID_LEN];
    v_U32_t             ownSsidLen;

    //                                          
    v_U8_t              ucSecEnabled;

    //                                  
    tCsrRoamProfile     csrRoamProfile; 
    v_U32_t             csrRoamId;

    //           
    tANI_BOOLEAN        isSapSessionOpen;

    //                          
    tpWLAN_SAPEventCB   pfnSapEventCallback;

    //                                        
    v_PVOID_t           pvosGCtx; 

    //                                                                                      
    eSapFsmStates_t     sapsMachine;

    //                                           
    tCsrSSIDInfo        SSIDList[2];

    //                             
    tCsrBssid           bssid;

    //                       
    eSapMacAddrACL      eSapMacAddrAclMode;
    v_MACADDR_t         acceptMacList[MAX_ACL_MAC_ADDRESS];
    v_U8_t              nAcceptMac;
    v_MACADDR_t         denyMacList[MAX_ACL_MAC_ADDRESS];
    v_U8_t              nDenyMac;

    //           
    tSapQosCfg        SapQosCfg;

    v_PVOID_t         pUsrContext;

    v_U32_t           nStaWPARSnReqIeLength;
    v_U8_t            pStaWpaRsnReqIE[MAX_ASSOC_IND_IE_LEN]; 
    tSirAPWPSIEs      APWPSIEs;
    tSirRSNie         APWPARSNIEs;

    v_U32_t           nStaAddIeLength;
    v_U8_t            pStaAddIE[MAX_ASSOC_IND_IE_LEN]; 
    v_U8_t            *channelList;
    tSapChannelListInfo SapChnlList;
} *ptSapContext;


/*                                                                            
                                             
                                                                            */
//                                                            
extern ptSapContext  gpSapCtx;

/*                                                                            
                                      
                                                                            */
/*                     */ 
typedef struct sWLAN_SAPEvent {
    v_PVOID_t params;   /*                                             */
    v_U32_t   event;    /*                                   */
    v_U32_t   u1;       /*                                                         */
    v_U32_t   u2;       /*                                                         */
} tWLAN_SAPEvent, *ptWLAN_SAPEvent; 

/*                                                                            
                                          
                                                                            */

/*                                                                          

                                    

              
                                             

               
        

             

      
                                                               
                                                                          
           
                                                      
   
              
                                                                  

                                 
  
               

                                                                            */
eHalStatus
WLANSAP_ScanCallback
(
  tHalHandle halHandle, 
  void *pContext,
  v_U32_t scanID, 
  eCsrScanStatus scanStatus
);

/*                                                                          

                                    

              
                                                  

               
        

             

      
                                                                
                                                                                        
                                                              
                                                                                   
                                                                 
                               
   
              
                                                                  

                                 
  
               
  
                                                                            */
eHalStatus
WLANSAP_RoamCallback
(
  void *pContext, 
  tCsrRoamInfo *pCsrRoamInfo,
  v_U32_t roamId, 
  eRoamCmdStatus roamStatus, 
  eCsrRoamResult roamResult
);

/*                                                                          

                             

              
                                            
    
               
    
             

      
                                              
                                                                

              
                                                              

                                                                               
                                
                                               

               

                                                                            */
VOS_STATUS 
WLANSAP_CleanCB
( 
  ptSapContext  pSapCtx,
  v_U32_t freeFlag /*                   */
);
/*                                                                          

                        

              
                                                              
    
               
    
             

      
                                                 
                          
                                        
   
              
                                     

                                                                               
                                
                                               

               

                                                                            */

VOS_STATUS
SapFsm
(
    ptSapContext sapContext, /*                  */    
    ptWLAN_SAPEvent sapEvent, /*                     */
    v_U8_t *status    /*                            */
);

/*                                                                          

                                     

              
                                                                
    
               
    
             

      
                                                               
                                        
   
              
        

               

                                                                            */
void 
WLANSAP_pmcFullPwrReqCB
( 
  void *callbackContext, 
  eHalStatus status
);

/*                                                                          

                              

              
                                                                                        

               

             

      
                                        
                                       
                                             
   
              
                                                  

               

                                                                            */

v_U8_t sapSelectChannel(tHalHandle halHandle, ptSapContext pSapCtx, tScanResultHandle pScanResult);

/*                                                                          

                               

              
                                   

               

             

      
                                         
                                     
                                              
   
              
                          

              

                                                                            */

VOS_STATUS
sapSignalHDDevent( ptSapContext sapContext, tCsrRoamInfo * pCsrRoamInfo, eSapHddEvent sapHddevent, void *);

/*                                                                          

                    

              
                             

               
    
             

      
                                         
                                     

             
                          

               

                                                                            */
VOS_STATUS
sapFsm
(
    ptSapContext sapContext,    /*                  */
    ptWLAN_SAPEvent sapEvent   /*                     */
);

/*                                                                          

                                    

              
                          

               

             

      
                                                          
                            
                                                                

             
                          

              

                                                                            */
eSapStatus
sapconvertToCsrProfile(tsap_Config_t *pconfig_params, eCsrRoamBssType bssType, tCsrRoamProfile *profile);

/*                                                                          

                                

              
                          

               

             

      
                                                               

             
                          

              

                                                                            */
void sapFreeRoamProfile(tCsrRoamProfile *profile);

/*                                                                          

                                 

              
                                                                         

               

             

      
                                         
                                           

             
                          

               

                                                                            */
VOS_STATUS
sapIsPeerMacAllowed(ptSapContext sapContext, v_U8_t *peerMac);

/*                                                                          

                            

              
                                                  

               

             

      
                                             
                                                       

             
        

               

                                                                            */
void
sapSortMacList(v_MACADDR_t *macList, v_U8_t size);

/*                                                                          

                            

             
                                            
                                                                             
                                                                              
                                                                   
                                                                                
                                 

              

            

      
                                                                      
                                                                               
                                                        
                                                             

             
         

              

                                                                            */
void
sapAddMacToACL(v_MACADDR_t *macList, v_U8_t *size, v_U8_t *peerMac);

/*                                                                          

                                 

              
                                                 
                                                                             

               

             

      
                                                                      
                                                                                                              
                                                                            
                                                                                                                        
                                                                                                 

             
         

               

                                                                            */
void
sapRemoveMacFromACL(v_MACADDR_t *macList, v_U8_t *size, v_U8_t index);

/*                                                                          

                         

              
                                                    
                     
    
               

             

      
                                                                      
                                         

             
         

               

                                                                            */
void 
sapPrintACL(v_MACADDR_t *macList, v_U8_t size);

/*                                                                          

                              

              
                                                  

               

             

      
                                                                  
                                         
                                                 
             
                                                                  
                                                                                                      
                                                                                                

             
                                                                  
                                                                      

               

                                                                            */
eSapBool
sapSearchMacList(v_MACADDR_t *macList, v_U8_t num_mac, v_U8_t *peerMac, v_U8_t *index);


/*                                                                          

                                   

              
                                                 

               

             

      
                                         
                                           

             
                          

               

                                                                            */
VOS_STATUS
sap_AcquireGlobalLock( ptSapContext  pSapCtx );

/*                                                                          

                                 

              
                                                 

               

             

      
                                         
                                           

             
                          

               

                                                                            */
VOS_STATUS
sap_ReleaseGlobalLock( ptSapContext  pSapCtx );

/*                                                                          
                                          

                                                                            

                       

                              

                                  

            
                                                                            */
eCsrPhyMode sapConvertSapPhyModeToCsrPhyMode( eSapPhyMode sapPhyMode );

#ifdef __cplusplus
}
#endif 
#endif /*                                     */


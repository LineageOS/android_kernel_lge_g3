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

#ifndef WLAN_QCT_WLANBAP_INTERNAL_H
#define WLAN_QCT_WLANBAP_INTERNAL_H

/*===========================================================================

               W L A N   B T - A M P  P A L   L A Y E R 
                       I N T E R N A L  A P I
                
                   
DESCRIPTION
  This file contains the internal API exposed by the wlan BT-AMP PAL layer 
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

/*                                                     */ 
#include "btampHCI.h" 
#include "bapApi.h" 

//                                  
#include "fsmDefs.h"
//                     
#include "btampFsm_ext.h"
#include "bapRsn8021xFsm.h"
#include "bapRsnErrors.h"

#include "csrApi.h"
#include "sirApi.h"
#include "wniApi.h"
#include "palApi.h"
/*                                                                            
                                         
                                                                            */
 #ifdef __cplusplus
 extern "C" {
 #endif 
 

/*                                                                            
           
                                                                            */
//                                
//                           
//           
//                 

//                                                       
#define WLANBAP_SECURITY_ENABLED_STATE VOS_TRUE

//                                             
#define VOS_GET_BAP_CB(ctx) vos_get_context( VOS_MODULE_ID_BAP, ctx) 
//                                           
#define VOS_GET_HAL_CB(ctx) vos_get_context( VOS_MODULE_ID_SME, ctx) 

//                                                  
//                                                                           
/*                                                         */
//                                                 
//                                          
#define WLANBAP_CONNECTION_ACCEPT_TIMEOUT  0x1F40

/*                                                           */
#ifdef FEATURE_WLAN_BTAMP_UT
#define WLANBAP_LINK_SUPERVISION_TIMEOUT   0x7D00
#else
#define WLANBAP_LINK_SUPERVISION_TIMEOUT   0x3E80  //           
#endif
#define WLANBAP_LINK_SUPERVISION_RETRIES   2

/*                                                               */
#define WLANBAP_LOGICAL_LINK_ACCEPT_TIMEOUT 0x1F40

/*                                    */
#define WLANBAP_BREDR_BASEBAND_SLOT_TIME  1 //      

/*                                                              */
#define WLANBAP_CON_ACCEPT_TIMEOUT_MAX_RANGE  0xB540

/*                                                              */
#define WLANBAP_CON_ACCEPT_TIMEOUT_MIN_RANGE   0x01

/*                                 */
#define WLANBAP_BE_FLUSH_TIMEOUT 10

/*                         */
#define WLANBAP_LLC_HEADER_LEN   8 

/*                                                          */
#define WLANBAP_LLC_PROTO_TYPE_SIZE            2

/*                                                     */
#define WLANBAP_LLC_OUI_SIZE                   3

/*                                                  */
#define WLANBAP_LLC_OUI_OFFSET                 3

/*                                                            */
#define WLANBAP_LLC_PROTO_TYPE_OFFSET  (WLANBAP_LLC_OUI_OFFSET +  WLANBAP_LLC_OUI_SIZE)

#define WLANBAP_MAX_NUM_TRIPLETS               5

#define WLANBAP_MAX_SIZE_TRIPLETS              3
/*                                                                            
            
                                                                            */
/*                                                                            
                                                            
                                                                            */
typedef struct sBtampHCI_Buffer_Size {
//                         
    /*                                    */
    v_U16_t      HC_ACL_Data_Packet_Length;  
    v_U8_t       HC_SCO_Packet_Length;
    v_U16_t      HC_Total_Num_ACL_Packets;
    v_U16_t      HC_Total_Num_SCO_Packets;
} tBtampHCI_Buffer_Size;

typedef struct sBtampHCI_Data_Block_Size {
//                         
    v_U8_t       status;
    /*                                    */
    v_U16_t      HC_Max_ACL_Data_Packet_Length;  
    v_U16_t      HC_Data_Block_Length;
    v_U16_t      HC_Total_Num_Data_Blocks;
} tBtampHCI_Data_Block_Size;

typedef struct sBtampHCI_Version_Info {
//                         
    v_U8_t       HC_HCI_Version;
    v_U16_t      HC_HCI_Revision;  
    v_U8_t       HC_PAL_Version;  /*                      */
    v_U16_t      HC_PAL_Sub_Version; /*                                 */ 
    v_U16_t      HC_Manufac_Name; /*                         */  
} tBtampHCI_Version_Info;

typedef struct sBtampHCI_Supported_Cmds {
//                         
    v_U8_t       HC_Support_Cmds[64]; /*                   */
} tBtampHCI_Supported_Cmds;

typedef struct sBtampHCI_AMP_Info {
//                         
    v_U8_t       HC_AMP_Status; /* */
    v_U32_t      HC_Total_BW; /*                              */
    v_U32_t      HC_Max_Guaranteed_BW; /*             */
    v_U32_t      HC_Min_Latency; /*                                */
    v_U32_t      HC_Max_PDU_Size; /*                             */
    v_U8_t       HC_Controller_Type; /*                             */
    v_U16_t      HC_PAL_Capabilities;  /*                                        */
    v_U16_t      HC_AMP_Assoc_Length;  /*                          */
                                       /*                                    */
    v_U16_t      HC_Max_Flush_Timeout;  /*                                           */
    v_U16_t      HC_BE_Flush_Timeout;  /*                                              */
} tBtampHCI_AMP_Info;

typedef struct sBtampHCI_AMP_Assoc {
//                         
    v_U8_t       HC_cnct_country[3];   /*                   */
    v_U8_t       HC_cnct_num_triplets;
    v_U8_t       HC_cnct_triplets[WLANBAP_MAX_NUM_TRIPLETS][WLANBAP_MAX_SIZE_TRIPLETS];
    v_U8_t       HC_mac_addr[6];
    v_U32_t      HC_pal_capabilities;
    v_U8_t       HC_pref_country[3];   /*                    */
    v_U8_t       HC_pref_num_triplets;
    v_U8_t       HC_pref_triplets[WLANBAP_MAX_NUM_TRIPLETS][WLANBAP_MAX_SIZE_TRIPLETS];
    v_U8_t       HC_pal_version;
    v_U16_t      HC_pal_CompanyID;
    v_U16_t      HC_pal_subversion;
}  tBtampHCI_AMP_Assoc, *tpBtampHCI_AMP_Assoc ;

typedef struct sBtampTLVHCI_Location_Data_Info {
    v_U8_t       loc_domain_aware;
    v_U8_t       loc_domain[3];
    v_U8_t       loc_options;
} tBtampTLVHCI_Location_Data_Info;

/*                                                                            
                                                                
                                                                            */
typedef struct sBtampLogLinkCtx {
    v_U8_t       present;  /*         */

    v_U8_t       log_link_index;  /*                                          */
    v_U16_t      log_link_handle;  /*                                         */

    /*                                                       */
    tBtampTLVFlow_Spec btampFlowSpec;   

    /*                      */
    WLANTL_ACEnumType btampAC;   

    /*          */
    v_U8_t    ucTID;

    /*                             */
    v_U8_t    ucUP;

    /*                                                                       
                */
    v_U32_t   uTxPktCompleted;    

}  tBtampLogLinkCtx, *tpBtampLogLinkCtx ;

/*                                                                            
                                   
                                                                            */
/*                   */
typedef struct sBtampQosCfg {
    v_U8_t                    bWmmIsEnabled;
} tBtampQosCfg;

/*                                                                            
                                       
                                                                            */
//                                                             
//                                            
//                                                            


//                                       
#define BTAMP_VALID_LOG_LINK(a) ( a > 0 && a < WLANBAP_MAX_LOG_LINKS ? 1 : 0)  

/*                                           */
//                                                        
typedef struct{
  BTAMPFSM_ENTRY_FLAG_T disconnectedEntry;
  BTAMPFSM_STATEVAR_T stateVar;
  BTAMPFSM_INST_ID_T inst_id;
} BTAMPFSM_INSTANCEDATA_T;

/*                    */
typedef enum{
    BT_RESPONDER,
    BT_INITIATOR
} tWLAN_BAPRole;

/*                    */
typedef enum{
    WLAN_BAPLogLinkClosed,
    WLAN_BAPLogLinkOpen,
    WLAN_BAPLogLinkInProgress,
} tWLAN_BAPLogLinkState;

typedef struct{
    v_U8_t       phyLinkHandle;
    v_U8_t       txFlowSpec[18];
    v_U8_t       rxFlowSpec[18];
} tBtampLogLinkReqInfo;

/*                                                                            
                                     
                                                                            */
#undef BTAMP_MULTIPLE_PHY_LINKS
typedef struct sBtampContext {
#ifndef BTAMP_MULTIPLE_PHY_LINKS

    //                                        
    v_PVOID_t                 pvosGCtx; 

    //                                                   
    tWLAN_BAPbapPhysLinkMachine   bapPhysLinkMachine;

    //                
    tWLAN_BAPRole             BAPDeviceRole;
    //                                    
    v_U8_t                    sessionId;

    //                                           
    //                                  
    tCsrSSIDInfo              SSIDList[2];
    //                             
    tCsrBssid                 bssid;
    //                                  
    tCsrRoamProfile           csrRoamProfile; 
    tANI_U32                  csrRoamId;

    //           
    tBtampQosCfg              bapQosCfg;

    /*                                         */
    v_U8_t                    ucSecEnabled;

    //                        
    v_U8_t                    mAssociated;
    //                   
    v_U8_t                    mAssociatedStatus;
    tCsrBssid                 assocBssid;  
    tBssSystemRole            systemRole; 

    //           
    v_U8_t                    ownSsid[32];
    v_U32_t                   ownSsidLen;

    //                      
    v_U8_t                    assocSsid[32];
    v_U32_t                   assocSsidLen;

    //                       
    v_U8_t                    gNeedPhysLinkCompEvent;
    //                 
    v_U8_t                    gPhysLinkStatus;
    //               
    v_U8_t                    gDiscRequested;
    //             
    v_U8_t                    gDiscReason;

    //                                             
    v_PVOID_t                 pAppHdl;  //                    
    //                                               
    //                               
    v_PVOID_t                 pHddHdl;
    /*                                                       */
    v_U8_t                    phy_link_handle;  
    //                                                
    v_U8_t                    phy_link_srm;

    //                                                
    v_U8_t                    key_type;
    v_U8_t                    key_length;
    v_U8_t                    key_material[32];
    
    /*                             
                                                                            
                                                                  
       */
    v_U8_t                    link_quality;
    
    /*                        */
    vos_timer_t               bapConnectionAcceptTimer; 
    /*                       */
    vos_timer_t               bapLinkSupervisionTimer; 
    /*                          */
    vos_timer_t               bapLogicalLinkAcceptTimer; 
    /*                        */
    vos_timer_t               bapBEFlushTimer; 

    /*                           */
    vos_timer_t               bapTxPktMonitorTimer; 

    /*                                                            
                                                                
     */
    v_U16_t                   bapConnectionAcceptTimerInterval; 

    /*                                                            */
    v_U16_t                   bapLinkSupervisionTimerInterval; 

    /*                                                               */
    v_U16_t                   bapLogicalLinkAcceptTimerInterval; 

    /*                                 */
    v_U32_t                   bapBEFlushTimerInterval; 

    //                                 
    v_U32_t                   channel;

    //                                
    v_U8_t                    ucSTAId;

    //                                       
    v_U8_t                    self_mac_addr[6];
    v_U8_t                    peer_mac_addr[6];

    //                           
    /*                                                   */
    v_U8_t                    current_log_link_index; /*                 */  
    v_U8_t                    total_log_link_index; /*                     */  
    /*                  */
    tBtampLogLinkCtx          btampLogLinkCtx[WLANBAP_MAX_LOG_LINKS];  
    
    //                                                                      
    WLANBAP_STAFetchPktCBType pfnBtampFetchPktCB;
    WLANBAP_STARxCBType       pfnBtamp_STARxCB;
    WLANBAP_TxCompCBType      pfnBtampTxCompCB;

    /*                                                      */ 
    tpWLAN_BAPEventCB         pBapHCIEventCB;

    //                                
    v_U8_t                    event_mask_page_2[8];

    //                                   
    //                                                                      
    //                                                            
    //                                                                         
    tBtampHCI_AMP_Assoc       btamp_AMP_Assoc;

    //                 
    tBtampHCI_AMP_Assoc       btamp_Remote_AMP_Assoc;

    tBtampTLVHCI_Location_Data_Info  btamp_Location_Data_Info;

    union
    {
        tAuthRsnFsm authFsm;
        tSuppRsnFsm suppFsm;
    }uFsm;
    //                      
    tANI_BOOLEAN lsReqPktPending;
    tANI_BOOLEAN dataPktPending;
    tANI_U8 retries;
    vos_pkt_t *pPacket;
    vos_pkt_t *lsReqPacket;
    vos_pkt_t *lsRepPacket;
    v_U16_t    lsPktln;
    v_U16_t    lsPending;
    WLANTL_MetaInfoType  metaInfo;   
    tANI_BOOLEAN isBapSessionOpen;

    tWLAN_BAPLogLinkState  btamp_logical_link_state;

    tBtampLogLinkReqInfo   btamp_logical_link_req_info;

    tANI_BOOLEAN           btamp_async_logical_link_create;

    tANI_BOOLEAN           btamp_logical_link_cancel_pending;

    tANI_BOOLEAN           btamp_session_on;

#else //                                  

    //                                        
    v_PVOID_t                 pvosGCtx; 

    //                                            

    //                                             
    v_PVOID_t                 pAppHdl;  //                    
    //                                               
    //                               
    v_PVOID_t                 pHddHdl;
    /*                                                       */
    v_U8_t                    phy_link_handle;  
    //                                                
    v_U8_t                    phy_link_srm;

    //                                
    v_U8_t                    ucSTAId;

    //                                       
    v_U8_t                    self_mac_addr[6];
    v_U8_t                    peer_mac_addr[6];

    //                           
    /*                                                   */
    v_U8_t                    current_log_link_index; /*                 */  
    v_U8_t                    total_log_link_index; /*                     */  
    /*                  */
    tBtampLogLinkCtx          btampLogLinkCtx[WLANBAP_MAX_LOG_LINKS];  
    
    //                                                                      
    WLANBAP_STAFetchPktCBType pfnBtampFetchPktCB;
    WLANBAP_STARxCBType       pfnBtamp_STARxCB;
    WLANBAP_TxCompCBType      pfnBtampTxCompCB;

    /*                                                      */ 
    tpWLAN_BAPEventCB         pBapHCIEventCB;

    //                                   
    //                                                                      
    //                                                            
    //                                                                         
    tBtampHCI_AMP_Assoc       btamp_AMP_Assoc;
    //                       
    tANI_BOOLEAN lsReqPktPending;   
    tANI_U8 retries;
    vos_pkt_t *pPacket;
    vos_pkt_t *lsReqPacket;
    vos_pkt_t *lsRepPacket;
    v_U16_t    lsPktln;
    WLANTL_MetaInfoType*  metaInfo; 
    tANI_BOOLEAN isBapSessionOpen;
    //                             
#endif //                        
    WLANBAP_ConfigType   config;
    /*                                                                         
                                                         */
    vos_lock_t           bapLock;
    //                              
    v_U8_t               ucDataTrafficMode;
}*ptBtampContext; 
//                               

/*                                                                            
                    
                                                                            */

typedef struct sBtampLsPktData {
    v_U32_t    BufLen;
    v_U8_t     pBuf[1]; //                   
}tBtampLsPktData, *ptBtampLsPktData;

typedef struct sBtampLsPkt {
    v_U8_t     SrcMac[6];
    v_U8_t     DstMac[6];
    tBtampLsPktData Data;
} tBtampLsPkt, *ptBtampLsPkt;

/*                                                                            
                                                 
                                                                            */
//                                                                   
typedef struct sBtampContext tBtampSessCtx;

/*                                                                            
                                      
                                                                            */
/*                     */ 
typedef struct sWLAN_BAPEvent {
    v_U32_t   event;  /*                                   */
    v_PVOID_t params;  /*                                             */
    v_U32_t   u1;  /*                                                         */
    v_U32_t   u2;  /*                                                         */
} tWLAN_BAPEvent, *ptWLAN_BAPEvent; 
 
//                                  
#include "btampFsm.h"


/*                                                                            
                                             
                                                                            */
//                                                            
//                              
extern ptBtampContext  gpBtampCtx;

//                                       
extern tBtampHCI_AMP_Info        btampHCI_AMP_Info;
//                                                   
extern tBtampHCI_Data_Block_Size btampHCI_Data_Block_Size;
//                                           
extern tBtampHCI_Version_Info   btampHCI_Version_Info;
//                                                  
extern tBtampHCI_Supported_Cmds  btampHCI_Supported_Cmds;


/*                                                                            
                        
                                                                            */

/*                                     */

/*                                                             */

/*                                                                            

                                    

                
                                                  
    
                                                                     
                                                  
                                                                      
                                                


             

      
                                                                
                                                                    
                                     

          
                                                                       
                                                                      
                                                                    
                                                       
                                                                          
                                                                           
                                                  

       
                                                                        
                                                      


  
               
                                                            

                                                                            */
VOS_STATUS 
WLANBAP_STAFetchPktCB 
( 
  v_PVOID_t             pvosGCtx,
  v_U8_t*               pucSTAId,
  v_U8_t                ucAC,
  vos_pkt_t**           vosDataBuff,
  WLANTL_MetaInfoType*  tlMetaInfo
);

/*                                                                            

                             

                
                                             
    
                                                                      
                         

             

      
                                                                   
                                                                      
                                
                                                                    
                                              
                              
  
               
                                                            

                                                                            */
VOS_STATUS 
WLANBAP_STARxCB
(
  v_PVOID_t              pvosGCtx, 
  vos_pkt_t*             vosDataBuff,
  v_U8_t                 ucSTAId, 
  WLANTL_RxMetaInfoType* pRxMetaInfo
);

/*                                                                            

                              

                
                                                 
    
                                                                     
                       

             

      
                                                                   
                                                                          
                                
                                                                        
                                               

  
               
                                                            

                                                                            */
VOS_STATUS 
WLANBAP_TxCompCB
( 
  v_PVOID_t      pvosGCtx,
  vos_pkt_t*     vosDataBuff,
  VOS_STATUS     wTxSTAtus 
);

/*                                                          */

/*              */

/*                                                                            

                
                                                     
    
                                                                          
                                                                        

             

      
                                                                        
                                                                             
                                                                              
                                                          
  
               
                                                            

                                                                            */
WLANTL_BAPRxCBType WLANBAP_TLRsnRxCallback
(
 v_PVOID_t         pvosGCtx,
 vos_pkt_t*        vosDataBuff
); 

/*                         */

/*                                                                            

                
                                                                     
                                                                             
    
                                                                          
                                                             

             

      
                                                                        
                                                                             
                                                              
                                            
                                                 
  
               
                                                            

                                                                            */
VOS_STATUS WLANBAP_TLFlushCompCallback
( 
  v_PVOID_t     pvosGCtx,
  v_U8_t        ucStaId, 
  v_U8_t        ucTID, 
  v_U8_t        status
);

/*                                                                            
                                          
                                                                            */
/*                                                                            

                                    

              
                                                  

               
        

             

      
                                                                
                                                                                  
                                                              
                                                                                   
                                                                 
                               
   
              
                                                              

                                 
  
               
  
                                                                            */
eHalStatus
WLANBAP_RoamCallback
(
  void *pContext, 
  tCsrRoamInfo *pCsrRoamInfo,
  tANI_U32 roamId, 
  eRoamCmdStatus roamStatus, 
  eCsrRoamResult roamResult
);

/*                                                                            
                                
                                                                            */

/*                                                                          

                             

              
                                            
    
               
    
             

      
                                                
                                                                
   
              
                                                              

                                                                               
                                
                                               

               
  
                                                                            */
VOS_STATUS 
WLANBAP_CleanCB
( 
  ptBtampContext  pBtampCtx,
  v_U32_t freeFlag //                     
);

/*                                                                          

                                     

              
                                                                        
                                                    
    
    
               
    
             

      
                                                   
   
       
                                                              
                                       
                                                              
   
              
                                                              

                                                                        
                                               

               
  
                                                                            */
VOS_STATUS 
WLANBAP_GetCtxFromStaId
( 
  v_U8_t         ucSTAId,  /*                                     */
  ptBtampHandle *hBtampHandle,  /*                                                */ 
  ptBtampContext *hBtampContext, /*                                                   */ 
  v_PVOID_t     *hHddHdl /*                                 */
);

/*                                                                          

                                         

              
                                                                 
                                                                  
    
    
               
    
             

      
                                                              
                                       
                                                                   
    
       
                                                    
                                                           
   
              
                                                              

                                                                        
                                               

               
  
                                                                            */
VOS_STATUS 
WLANBAP_GetStaIdFromLinkCtx
( 
  ptBtampHandle  btampHandle,  /*                       */ 
  v_U8_t         phy_link_handle,  /*                          */
  v_U8_t        *pucSTAId,  /*                                     */
  v_PVOID_t     *hHddHdl /*                              */
);

/*                                                                          

                                         

              
                                                                       
    
    
               
    
             

      
                                           
                                                      
                                                       
    
       
                                                                
                                                           
   
              
                                                              

                                                                        
                                               

               
  
                                                                            */
VOS_STATUS 
WLANBAP_CreateNewPhyLinkCtx
( 
  ptBtampHandle  btampHandle,
  v_U8_t         phy_link_handle, /*                                         */
  v_PVOID_t      pHddHdl,   /*                                    */
  ptBtampContext *hBtampContext, /*                                                   */ 
  tWLAN_BAPRole  BAPDeviceRole  /*                                                            */
);

/*                                                                          

                                           

              
                                                                            
    
    
               
    
             

      
                                              
                                                         
    
   
              
                                                              

                                                                        
                                               

               
  
                                                                            */
VOS_STATUS 
WLANBAP_UpdatePhyLinkCtxStaId
( 
  ptBtampContext pBtampContext, /*                        */ 
  v_U8_t         ucSTAId
);

/*                                                                          

                                         

              
                                                                        
    
    
               
    
             

      
                                                           
                                              
    
       
                                                      
   
              
                                                              

                                                                        
                                               

               
  
                                                                            */
VOS_STATUS 
WLANBAP_CreateNewLogLinkCtx
( 
  ptBtampContext pBtampContext, /*                                             */ 
  v_U8_t         phy_link_handle, /*                                         */
  v_U8_t         tx_flow_spec[18],
  v_U8_t         rx_flow_spec[18],
  v_U16_t         *pLog_link_handle /*                                     */
);

 /*                                                                          

                                     

              
                                                            
    
    
               
    
             

      
                                                               
                                    
    
   
              
        

               
  
                                                                            */
void 
WLANBAP_pmcFullPwrReqCB
( 
  void *callbackContext, 
  eHalStatus status
);
  
/*                                                                           

                                 

              

                                                                            

             

                                                                      
                                                       
                                                 
                                                                  
                                                 
   
              

                                                              

                                                      
                                             
                                               

               
  
                                                                            */
VOS_STATUS
WLANBAP_RxProcLsPkt
( 
  ptBtampHandle     btampHandle, 
  v_U8_t            phy_link_handle,  /*                                                  */
  v_U16_t            RxProtoType,     /*                                       */
  vos_pkt_t         *vosRxLsBuff
);


/*                                                                            

                                            

              
                                                                                             
                                         
                                                                        
    

               
        

             

      
                                                                              
                                                                     
                                                          
                                                             
    
                 
                                                              

                                                       
                                
  
               
  
                                                                            */
VOS_STATUS
WLANBAP_TxLinkSupervision
( 
  ptBtampHandle     btampHandle, 
  v_U8_t            phy_link_handle,  /*                                                  */
  vos_pkt_t         *vosDataBuff,
  v_U16_t           protoType
);

/*                                                                          

                                   

              
                                                                         
        
               
    
             

                                                                         
                                                                    
   
              
        

               
  
                                                                            */
void
WLANBAP_ReadMacConfig
( 
  ptBtampContext  pBtampCtx 
);

/*                                                                          

                                        

              
                                                                 
                                                                      
        
               
    
             

                                                                         
                                                                    
   
              
        

               
  
                                                                            */
void
WLANBAP_NeedBTCoexPriority
( 
  ptBtampContext  pBtampCtx, 
  v_U32_t         needCoexPriority
);


/*                                                                          

                                

              
                                                                                          
        
               
    
             

                                                                         
                                                                   
                              
                              
   
              
        

               
  
                                                                            */
VOS_STATUS WLANBAP_RxCallback
(
    v_PVOID_t               pvosGCtx, 
    vos_pkt_t              *pPacket,
    WLANTL_BAPFrameEnumType frameType
);


/*                                                                           

                                         

              

                                                                                                   

             

                                                                      
   
              

                                                              

                                                      
                                             
                                               

               
  
                                                                            */
#define TX_LS_DATALEN   32

VOS_STATUS
WLANBAP_InitLinkSupervision
( 
  ptBtampHandle     btampHandle
);


/*                                                                           

                                           

              

                                                                                                    

             

                                                                      
   
              

                                                              

                                                      
                                             
                                               

               
  
                                                                            */
VOS_STATUS
WLANBAP_DeInitLinkSupervision
( 
  ptBtampHandle     btampHandle 
);

void WLAN_BAPEstablishLogicalLink(ptBtampContext btampContext);

 #ifdef __cplusplus
 }


#endif 


#endif /*                                     */


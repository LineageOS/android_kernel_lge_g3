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

#ifndef WLAN_QCT_WLANBAP_H
#define WLAN_QCT_WLANBAP_H

/*===========================================================================

               W L A N   B T - A M P  P A L   L A Y E R 
                       E X T E R N A L  A P I
                
                   
DESCRIPTION
  This file contains the external API exposed by the wlan BT-AMP PAL layer 
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
#include "wlan_qct_tl.h"

/*                                                     */ 
#include "btampHCI.h" 

/*                                                                            
                                         
                                                                            */
 #ifdef __cplusplus
 extern "C" {
 #endif 
 

/*                                                                            
                           
   
                                                                         
              
   
                                                                            */

/*                                                                           
                         
                                                                           */      
#if 0
/*                                */
    BTAMP_TLV_HCI_CREATE_PHYSICAL_LINK_CMD,
    BTAMP_TLV_HCI_ACCEPT_PHYSICAL_LINK_CMD,
    BTAMP_TLV_HCI_DISCONNECT_PHYSICAL_LINK_CMD,
    BTAMP_TLV_HCI_CREATE_LOGICAL_LINK_CMD,
    BTAMP_TLV_HCI_ACCEPT_LOGICAL_LINK_CMD,
    BTAMP_TLV_HCI_DISCONNECT_LOGICAL_LINK_CMD,
    BTAMP_TLV_HCI_LOGICAL_LINK_CANCEL_CMD,
    BTAMP_TLV_HCI_FLOW_SPEC_MODIFY_CMD,
/*
                                     
*/
    BTAMP_TLV_HCI_RESET_CMD,
    BTAMP_TLV_HCI_SET_EVENT_MASK_CMD,
    BTAMP_TLV_HCI_FLUSH_CMD,
    BTAMP_TLV_HCI_READ_CONNECTION_ACCEPT_TIMEOUT_CMD,
    BTAMP_TLV_HCI_WRITE_CONNECTION_ACCEPT_TIMEOUT_CMD,
    BTAMP_TLV_HCI_READ_LINK_SUPERVISION_TIMEOUT_CMD,
    BTAMP_TLV_HCI_WRITE_LINK_SUPERVISION_TIMEOUT_CMD,
/*                                            */
    BTAMP_TLV_HCI_READ_LOGICAL_LINK_ACCEPT_TIMEOUT_CMD,
    BTAMP_TLV_HCI_WRITE_LOGICAL_LINK_ACCEPT_TIMEOUT_CMD,
    BTAMP_TLV_HCI_SET_EVENT_MASK_PAGE_2_CMD,
    BTAMP_TLV_HCI_READ_LOCATION_DATA_CMD,
    BTAMP_TLV_HCI_WRITE_LOCATION_DATA_CMD,
    BTAMP_TLV_HCI_READ_FLOW_CONTROL_MODE_CMD,
    BTAMP_TLV_HCI_WRITE_FLOW_CONTROL_MODE_CMD,
    BTAMP_TLV_HCI_READ_BEST_EFFORT_FLUSH_TO_CMD,
    BTAMP_TLV_HCI_WRITE_BEST_EFFORT_FLUSH_TO_CMD,
/*                                                                 */
    BTAMP_TLV_HCI_SET_SHORT_RANGE_MODE_CMD,
/*                                                   */
/*
                        
*/
    BTAMP_TLV_HCI_READ_LOCAL_VERSION_INFORMATION_CMD,
    BTAMP_TLV_HCI_READ_LOCAL_SUPPORTED_COMMANDS_CMD,
    BTAMP_TLV_HCI_READ_BUFFER_SIZE_CMD,
/*                             */
    BTAMP_TLV_HCI_READ_DATA_BLOCK_SIZE_CMD,
/*
                 
*/
    BTAMP_TLV_HCI_READ_FAILED_CONTACT_COUNTER_CMD,
    BTAMP_TLV_HCI_RESET_FAILED_CONTACT_COUNTER_CMD,
    BTAMP_TLV_HCI_READ_LINK_QUALITY_CMD,
    BTAMP_TLV_HCI_READ_RSSI_CMD,
    BTAMP_TLV_HCI_READ_LOCAL_AMP_INFORMATION_CMD,
    BTAMP_TLV_HCI_READ_LOCAL_AMP_ASSOC_CMD,
    BTAMP_TLV_HCI_WRITE_REMOTE_AMP_ASSOC_CMD,
/*
              
*/
    BTAMP_TLV_HCI_READ_LOOPBACK_MODE_CMD,
    BTAMP_TLV_HCI_WRITE_LOOPBACK_MODE_CMD,
#endif

/*                                                                           
                      
                                                                           */      
#if 0
/*            */
    BTAMP_TLV_HCI_COMMAND_COMPLETE_EVENT,
    BTAMP_TLV_HCI_COMMAND_STATUS_EVENT,
    BTAMP_TLV_HCI_HARDWARE_ERROR_EVENT,
    BTAMP_TLV_HCI_FLUSH_OCCURRED_EVENT,
    BTAMP_TLV_HCI_LOOPBACK_COMMAND_EVENT,
    BTAMP_TLV_HCI_DATA_BUFFER_OVERFLOW_EVENT,
    BTAMP_TLV_HCI_QOS_VIOLATION_EVENT,
/*                 */
    BTAMP_TLV_HCI_GENERIC_AMP_LINK_KEY_NOTIFICATION_EVENT,
    BTAMP_TLV_HCI_PHYSICAL_LINK_COMPLETE_EVENT ,
    BTAMP_TLV_HCI_CHANNEL_SELECTED_EVENT ,
    BTAMP_TLV_HCI_DISCONNECT_PHYSICAL_LINK_COMPLETE_EVENT ,
    BTAMP_TLV_HCI_PHYSICAL_LINK_LOSS_WARNING_EVENT ,
    BTAMP_TLV_HCI_PHYSICAL_LINK_RECOVERY_EVENT ,
    BTAMP_TLV_HCI_LOGICAL_LINK_COMPLETE_EVENT ,
    BTAMP_TLV_HCI_DISCONNECT_LOGICAL_LINK_COMPLETE_EVENT ,
    BTAMP_TLV_HCI_FLOW_SPEC_MODIFY_COMPLETE_EVENT ,
    BTAMP_TLV_HCI_SHORT_RANGE_MODE_CHANGE_COMPLETE_EVENT ,
#endif


/*                                                                            
           
                                                                            */
/*                                                                           
                                                              
                                                                           */      
#define WLANBAP_HCI_COMMAND_PACKET      0x01     /*                                                                
                                                                        */
#define WLANBAP_HCI_ACL_DATA_PACKET     0x02     /*                                                                 
                                                                        */
#define WLANBAP_HCI_SCO_DATA_PACKET     0x03     /*                                                                 
                                                                        */
#define WLANBAP_HCI_EVENT_PACKET        0x04     /*                                                              
                                                                        */
/*                                                                           
                                  
                                                                           */      
#define WLANBAP_MAX_80211_PAL_PDU_SIZE                1492

/*                                                                           
                         
                                                                           */      
#define WLANBAP_FLOW_CONTROL_MODE_PACKET_BASED        0x00
#define WLANBAP_FLOW_CONTROL_MODE_BLOCK_BASED         0x01

/*                                                                           
                       
                                                                           */      
//                    
#define WLANBAP_QUALCOMM_COMPANY_ID                     29

//                       
//                                              
//                                                                     
//                                                                    
//                                                                    
//                                                                    
//                                                                    
//                                                                    
#define WLANBAP_HCI_VERSION                              5
#define WLANBAP_HCI_REVISION                             0 
#define WLANBAP_PAL_VERSION                              0x01
#define WLANBAP_PAL_SUBVERSION                           0x00

//                   
#define WLANBAP_HCI_AMP_STATUS_POWERED_DOWN              0x00
#define WLANBAP_HCI_AMP_STATUS_NOT_SHARED                0x01
#define WLANBAP_HCI_AMP_STATUS_SHARED                    0x02
#define WLANBAP_HCI_AMP_STATUS_RESERVED                  0x03

//                                       
#define WLANBAP_HCI_PKT_START_NON_FLUSH                  0x00
#define WLANBAP_HCI_PKT_CONT                             0x01
#define WLANBAP_HCI_PKT_START_FLUSH                      0x02
#define WLANBAP_HCI_PKT_AMP                              0x03

/*                                                                           
                                        

                                                                              
                                                                        
                                                                 
                                                                             
                          

                                                                           */      
//                                               

#define WLANBAP_PAL_SUPPORTED_HCI_CMDS {  \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x02, 0x0c, \
    0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x88, 0x3c, \
    0x00, 0x00, 0x00, 0x40, 0x00, 0xff, 0xff, 0x07, \
    0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  \
}

/*                                                                           
                                                      
                                                                           */      
#define WLANBAP_STATUS_SUCCESS                        (0x00) /*                        */

#define WLANBAP_ERROR_UNKNOWN_HCI_CMND                (0x01)  
#define WLANBAP_ERROR_NO_CNCT                         (0x02)  /*                        */
#define WLANBAP_ERROR_HARDWARE_FAILURE                (0x03) 
#define WLANBAP_ERROR_PAGE_TIMEOUT                    (0x04)  
/*                                                 */ 
#define WLANBAP_ERROR_AUTHENT_FAILURE                 (0x05)  
#define WLANBAP_ERROR_KEY_MISSING                     (0x06) 
#define WLANBAP_ERROR_MEMORY_FULL                     (0x07) 
#define WLANBAP_ERROR_CNCT_TIMEOUT                    (0x08)  /*               */
#define WLANBAP_ERROR_MAX_NUM_CNCTS                   (0x09)  /*               */   
#define WLANBAP_ERROR_MAX_NUM_SCO_CNCTS               (0x0a)      
#define WLANBAP_ERROR_MAX_NUM_ACL_CNCTS               (0x0b)      
#define WLANBAP_ERROR_CMND_DISALLOWED                 (0x0c)  /*             */    
#define WLANBAP_ERROR_HOST_REJ_RESOURCES              (0x0d)  /*               */ 
#define WLANBAP_ERROR_HOST_REJ_SECURITY               (0x0e)      
#define WLANBAP_ERROR_HOST_REJ_PERSONAL_DEV           (0x0f)      
#define WLANBAP_ERROR_HOST_TIMEOUT                    (0x10)      
#define WLANBAP_ERROR_UNSUPPORT_FEAT_PARAM            (0x11)      
#define WLANBAP_ERROR_INVALID_HCI_CMND_PARAM          (0x12)     
#define WLANBAP_ERROR_TERM_CNCT_USER_ENDED            (0x13)      
#define WLANBAP_ERROR_TERM_CNCT_LOW_RESOURCE          (0x14)      
#define WLANBAP_ERROR_TERM_CNCT_POWER_OFF             (0x15)      
/*                                                                    */    
#define WLANBAP_ERROR_TERM_BY_LOCAL_HOST              (0x16) /*               */  
#define WLANBAP_ERROR_REPEATED_ATTEMPTS               (0x17)      
#define WLANBAP_ERROR_PAIRING_NOT_ALLOWED             (0x18)      
#define WLANBAP_ERROR_UNKNOWN_LMP_PDU                 (0x19)      
#define WLANBAP_ERROR_UNSUPPORTED_REMOTE_FEAT         (0x1a)      
#define WLANBAP_ERROR_SCO_REJ                         (0x1b)      
#define WLANBAP_ERROR_SCO_INTERVAL_REJ                (0x1c)      
#define WLANBAP_ERROR_SCO_AIR_MODE_REJ                (0x1d)      
#define WLANBAP_ERROR_INVALID_LMP_PARAMETER           (0x1e)      
#define WLANBAP_ERROR_UNSPECIFIED_ERROR               (0x1f)      
#define WLANBAP_ERROR_UNSUPPORTED_LMP_PARAM           (0x20)      
#define WLANBAP_ERROR_ROLE_CHANGE_NOT_ALLOWED         (0x21)      
#define WLANBAP_ERROR_LMP_RESPONSE_TIMEOUT            (0x22)      
#define WLANBAP_ERROR_LMP_ERROR_TRANS_COLLISION       (0x23)      
#define WLANBAP_ERROR_LMP_PDU_NOT_ALLOWED             (0x24)      
#define WLANBAP_ERROR_ENCRYPTION_MODE_NOT_ACCEPTABLE  (0x25)      
#define WLANBAP_ERROR_UNIT_KEY_USED                   (0x26)      
#define WLANBAP_ERROR_QOS_IS_NOT_SUPPORTED            (0x27)      
#define WLANBAP_ERROR_INSTANT_PASSED                  (0x28)      
#define WLANBAP_ERROR_UNIT_KEY_PAIRING_UNSUPPORTED    (0x29)      

#define WLANBAP_ERROR_DIFFERENT_TRANS_COLLISION       (0x2A)      

/*                                                           */

#define WLANBAP_ERROR_QOS_UNACCEPTABLE_PARAMETER      (0x2C)      
#define WLANBAP_ERROR_QOS_REJECTED                    (0x2D)      
#define WLANBAP_ERROR_CHANNEL_CLASSIFICATION_NS       (0x2E)      
#define WLANBAP_ERROR_INSUFFICIENT_SECURITY           (0x2F)      
#define WLANBAP_ERROR_PARM_OUT_OF_MANDATORY_RANGE     (0x30)      
                                        
/*                                                           */

#define WLANBAP_ERROR_ROLE_SWITCH_PENDING             (0x32)      

/*                                                           */

#define WLANBAP_ERROR_RESERVED_SLOT_VIOLATION         (0x34)      
#define WLANBAP_ERROR_ROLE_SWITCH_FAILED              (0x35)      
#define WLANBAP_ERROR_EIR_TOO_LARGE                   (0x36)      
#define WLANBAP_ERROR_SSP_NOT_SUPPORTED_BY_HOST       (0x37)      
#define WLANBAP_ERROR_HOST_BUSY_PAIRING               (0x38)      
#define WLANBAP_ERROR_NO_SUITABLE_CHANNEL             (0x39)
#define WLANBAP_ERROR_CONTROLLER_BUSY                 (0x3A)      

/*                                                                            
                                         
                                                                            */
#define WLANBAP_EVENT_MASK_NONE                    0x0000000000000000 //                             
#define WLANBAP_EVENT_MASK_PHY_LINK_COMPLETE_EVENT 0x0000000000000001 //                            
#define WLANBAP_EVENT_MASK_CHANNEL_SELECTED_EVENT  0x0000000000000002 //                      
#define WLANBAP_EVENT_MASK_DISC_PHY_LINK_EVENT     0x0000000000000004 //                                 
#define WLANBAP_EVENT_MASK_PHY_LINK_LOSS_EARLY_WARNING_EVENT 0x0000000000000008 //                                      
#define WLANBAP_EVENT_MASK_PHY_LINK_RECOVERY_EVENT 0x0000000000000010 //                            
#define WLANBAP_EVENT_MASK_LOG_LINK_COMPLETE_EVENT 0x0000000000000020 //                           
#define WLANBAP_EVENT_MASK_DISC_LOG_LINK_COMPLETE_EVENT 0x0000000000000040 //                                         
#define WLANBAP_EVENT_MASK_FLOW_SPEC_MOD_COMPLETE_EVENT 0x0000000000000080 //                               
#define WLANBAP_EVENT_MASK_NUM_COMPLETED_DATA_BLOCKS_EVENT 0x0000000000000100 //                                     
#define WLANBAP_EVENT_MASK_AMP_START_TEST_EVENT    0x0000000000000200 //                    
#define WLANBAP_EVENT_MASK_AMP_TEST_END_EVENT      0x0000000000000400 //                  
#define WLANBAP_EVENT_MASK_AMP_RCVR_REPORT_EVENT   0x0000000000000800 //                         
#define WLANBAP_EVENT_MASK_SHORT_RANGE_MODE_CHANGE_COMPLETE_EVENT 0x0000000000001000 //                                      
#define WLANBAP_EVENT_MASK_AMP_STATUS_CHANGE_EVENT 0x0000000000002000 //                       
#define WLANBAP_EVENT_MASK_RESERVED                0xFFFFFFFFFFFFC000 //                       

/*                                                                            
            
                                                                            */

/*                                                                            
                                       
                                                                            */
typedef v_PVOID_t tBtampHandle, *ptBtampHandle;

/*                                                                            
                                                 
                                                                            */
//                                              
//                                                                   
//                                           


/*                                                                           
                 
                                                                           */      
typedef struct sBtampHCI_Event {
    v_U8_t  bapHCIEventCode;  /*                                    */
    union { 
        tBtampTLVHCI_Channel_Selected_Event  btampChannelSelectedEvent;
        tBtampTLVHCI_Command_Complete_Event btampCommandCompleteEvent ;
        tBtampTLVHCI_Command_Status_Event btampCommandStatusEvent ;
        tBtampTLVHCI_Data_Buffer_Overflow_Event btampDataBufferOverflowEvent ;
        tBtampTLVHCI_Disconnect_Logical_Link_Complete_Event btampDisconnectLogicalLinkCompleteEvent ;
        tBtampTLVHCI_Disconnect_Physical_Link_Complete_Event btampDisconnectPhysicalLinkCompleteEvent ;
        /*                                                                                       */
        tBtampTLVHCI_Flow_Spec_Modify_Complete_Event btampFlowSpecModifyCompleteEvent ;
        /*                                                                                       */
        tBtampTLVHCI_Flush_Occurred_Event btampFlushOccurredEvent ;
        tBtampTLVHCI_Generic_AMP_Link_Key_Notification_Event btampGenericAMPLinkKeyNotificationEvent ;
        tBtampTLVHCI_Hardware_Error_Event btampHardwareErrorEvent ;
        tBtampTLVHCI_Logical_Link_Complete_Event btampLogicalLinkCompleteEvent ;
        tBtampTLVHCI_Loopback_Command_Event btampLoopbackCommandEvent ;
        tBtampTLVHCI_Physical_Link_Complete_Event btampPhysicalLinkCompleteEvent ;
        tBtampTLVHCI_Physical_Link_Loss_Warning_Event btampPhysicalLinkLossWarningEvent ;
        tBtampTLVHCI_Physical_Link_Recovery_Event btampPhysicalLinkRecoveryEvent ;
        tBtampTLVHCI_Qos_Violation_Event btampQosViolationEvent ;
        tBtampTLVHCI_Short_Range_Mode_Change_Complete_Event btampShortRangeModeChangeCompleteEvent ;
        tBtampTLVHCI_Num_Completed_Pkts_Event btampNumOfCompletedPktsEvent;
        tBtampTLVHCI_Num_Completed_Data_Blocks_Event btampNumOfCompletedDataBlocksEvent;
        tBtampTLVHCI_Enhanced_Flush_Complete_Event btampEnhancedFlushCompleteEvent ;
    } u;
} tBtampHCI_Event, *tpBtampHCI_Event;

/*              */
typedef struct 
{
 /*                           */
 v_U8_t   vDA[VOS_MAC_ADDR_SIZE];

 /*                      */
 v_U8_t   vSA[VOS_MAC_ADDR_SIZE];

 /*              */
 v_U16_t  usLenType;  /*                                                   */
                      /*                                                
                                                         */
}WLANBAP_8023HeaderType;


/* 
                                                       
                                   
 */ 
#if 0
/*                                                                         */
/*                   */
/*                                    */

#endif

/* 
                                                                  
                                                                
                                                                            
 */
/*                                                                         */
#if 0
/*                                                                   */
/*                            */
/*                                                                       */
/*                                              */
/*                                                                                                        */
/*                                                                               */
/*                                                                           */
/*                                                */
/*                                   */
/*                                                                           */
/*                                  */
/*                                                       */
/*                                      */
/*                                                                                                    */
/*                                              */
/*                                   */
/*                                                                                                                           */
/*                                                            */
/*                                                                                                                                        */
/*                                                                                                                    */
/*                                                                                  */
/*                                                           */
/*                                                                      */
/*                                              */
/*                                                                                                                                                                                                                               */
/*                                                                                                                                                    */ 
/*                                                                                                                                                                                        */ 
/*                                                      */
/*                                               */
/*                                  */

#endif

/*                            */
typedef struct 
{
 /*                                                   */
 v_U8_t   ucPreferredChannel;

}WLANBAP_ConfigType;

/*                                                                            
                                          
                                                                            */

/*                                                                            
                                            
                                                                            */

/*                                                                          

                          

              
                                                                    
                                                                      
                                     
    
               
    
             

      
                                                                         
                                                                    
   
              
                                                              

                                                                               
                                
                                               

               
  
                                                                            */
VOS_STATUS 
WLANBAP_Open
( 
  v_PVOID_t  pvosGCtx 
);

/*                                                                          

                           

              
                                                                        
                                                            
                       
    
               
    
             

      
                                                                         
                                                                    
   
              
                                                              

                                                                               
                                
                                               

                                                             
    
               
  
                                                                            */
VOS_STATUS 
WLANBAP_Start
( 
  v_PVOID_t  pvosGCtx 
);

/*                                                                          

                          

              
                                                                                    
                                                                         
                                       
    
               
    
             

      
                                                                         
                                                                    
   
              
                                                              

                                                                               
                                
                                               

               
  
                                                                            */
VOS_STATUS 
WLANBAP_Stop
( 
  v_PVOID_t  pvosGCtx 
);

/*                                                                          

                           

              
                                                                                 
                                
    
               
    
             

      
                                                                         
                                                                    
   
              
                                                              

                                                                               
                                
                                               

               
  
                                                                            */
VOS_STATUS 
WLANBAP_Close
( 
  v_PVOID_t  pvosGCtx 
);

/*                                                                            
                                                 
                                                                            */

/*                                                                          

                                

              
                                                                 
                                                                      
               
    
                                                           
                                      
    
    
               
    
             

      
                                                          
   
              
                                                              

                                                                               
                                
                                               

               
  
                                                                            */
VOS_STATUS 
WLANBAP_GetNewHndl
( 
  ptBtampHandle *hBtampHandle  /*                                        */ 
);

/*                                                                          

                                 

              
                                                                             
                                            
    
    
               
    
             

      
                                                   
   
              
                                                              

                                                                               
                                     
                                               

               
  
                                                                            */
VOS_STATUS 
WLANBAP_ReleaseHndl
( 
  ptBtampHandle btampHandle  /*                                */ 
);

/*                                                                            
                               
                                                                            */

/*                                                                            
                       
                                                                            */

/*                                                                            

                                             

                
                                                                  
    
                                                            
                                                                      
                                                                         
                          
    
                                                                         
                                                                          
                                                       


             

      
                                                                       
                                                                             
                                           

          
                                                                          
                                                                           
                                                  

       
                                                                        
                                                      


  
               
                                                            

                                                                            */

typedef VOS_STATUS (*WLANBAP_STAFetchPktCBType)( 
                                            v_PVOID_t             pHddHdl,
                                            WLANTL_ACEnumType     ucAC,
                                            vos_pkt_t**           vosDataBuff,
                                            WLANTL_MetaInfoType*  tlMetaInfo);

 

/*                                                                            

                                       

                
                                                             
    
                                                            
                                                                
                                                                 

             

      
                                                                       
                                                                          
                                           

                                                                     
                                              
                                                          
  
               
                                                            

                                                                            */
typedef VOS_STATUS (*WLANBAP_STARxCBType)( v_PVOID_t      pHddHdl,
                                          vos_pkt_t*         vosDataBuff,
                                          WLANTL_RxMetaInfoType* pRxMetaInfo);



/*                                                                            

                                       

                
                                                                 
    
                                                            
                                                                 
                                                            

             

      
                                                                       
                                                                         
                                                                     
                                                                         
                                               

  
               
                                                            

                                                                            */
typedef VOS_STATUS (*WLANBAP_TxCompCBType)( v_PVOID_t      pHddHdl,
                                           vos_pkt_t*     vosDataBuff,
                                           VOS_STATUS     wTxSTAtus );

/*                                                                            
                        
                                                                            */

/*                                                                          

                                       

              
                                                                    
                                                                    
                                                                   
                                                                   
                                                                    
    
                                                                      
                                                                      
                                                                        
    
               
    
             

      
                                                                      
   
              
                                                              

                                                                               
                                
                                               

               
  
                                                                            */
VOS_STATUS 
WLANBAP_RegisterDataPlane
( 
  ptBtampHandle btampHandle,  /*               */ 
  WLANBAP_STAFetchPktCBType pfnBtampFetchPktCB, 
  WLANBAP_STARxCBType pfnBtamp_STARxCB,
  WLANBAP_TxCompCBType pfnBtampTxCompCB,
  //                                                                                     
  v_PVOID_t      pHddHdl   /*                      */
);
//      

/*                                                                           

                                    

              

                                                                      
                                                                    


             

                                                                      
                                                                      

                                                       
                                                                    
                                                 
                                                                        
   
              

                                                              

                                                      
                                             
                                               

               
  
                                                                            */
VOS_STATUS
WLANBAP_XlateTxDataPkt
( 
  ptBtampHandle     btampHandle,  /*                                           
                                                            */ 
  v_U8_t            phy_link_handle,  /*                                                  */
  WLANTL_ACEnumType           *pucAC,        /*                    */
  WLANTL_MetaInfoType  *tlMetaInfo, /*                                                                  */
  vos_pkt_t        *vosDataBuff
);

/*                                                                           

                                    

              

                                                                    
                                                             
                                                                        


             

                                                                      
                                                       
                                                                  
                                                 
   
              

                                                              

                                                      
                                             
                                               

               
  
                                                                            */
VOS_STATUS
WLANBAP_XlateRxDataPkt
( 
  ptBtampHandle     btampHandle, 
  v_U8_t            phy_link_handle,  /*                                                  */
  WLANTL_ACEnumType           *pucAC,        /*                                                  */
  vos_pkt_t        *vosDataBuff
);

/*                                                                           

                                   

              

                                                                        
                                                                       
                                                    

               

                                                                    

             

                                                                      
                                                                          
                                                                    
                                                                            
                                                                            
                                        
                                                                     
                                                          
   
              

                                                              

                                                      
                                             
                                               

               
  
                                                                            */
VOS_STATUS
WLANBAP_STAPktPending 
( 
  ptBtampHandle  btampHandle,  /*                                                           */ 
  v_U8_t         phy_link_handle,  /*                                                  */
  WLANTL_ACEnumType ucAc   /*                                                     */
);

/*                                                                            
                                        
                                                                            */

/*                                                                            

                                     

              
                                                         
                                    
                                 
                                 
                           
                         
                 
                    

               
        

             

      
                                                                    
                                                                         
                        
                                                                                      
                                        
                                                                                
   
              
                                                              

                                                         
                                
  
               
  
                                                                            */
typedef VOS_STATUS (*tpWLAN_BAPEventCB)
(
  v_PVOID_t      pHddHdl,   /*                                        
                                                                              
                                                                              
                                                                               
                                                                          */ 
                            /*                                         */
                            /*                                   */
  tpBtampHCI_Event pBapHCIEvent, /*                                  */
                                /*                                              */
  v_BOOL_t assoc_specific_event /*                                                 */
);   


/*                                                                            
                                           
                                                                            */

/*                                                                            

                                             

              
                                       
                                   

               
        

             

      
                                                                      
                                                  
                                                                                

   
              
                                                              

                                                           
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPRegisterBAPCallbacks 
( 
  ptBtampHandle           btampHandle, /*                                  */
                            /*                                    */
                            /*                                         */
                            /*                                   */
  tpWLAN_BAPEventCB       pBapHCIEventCB, /*                                                     */ 
  v_PVOID_t               pAppHdl  //                    
);



/*                                                                            
                                            
                                                                            */

/*                                */

/*                                                                            
                                                                         
                                                                              
                                                                              
                                                                            */

/*                                                                            

                                          

              
                                                          

               
        

             

      
                                                                              
                                                                          
                                                                      
                               
                                                                                
                                                                               

          
                                                                    
                                                                       
                                                               
   
              
                                                              

                                                                  
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPPhysicalLinkCreate 
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Create_Physical_Link_Cmd   *pBapHCIPhysLinkCreate,
  v_PVOID_t      pHddHdl,   /*                                    */
                            /*                                            */
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */
);

/*                                                                            

                                          

              
                                                          

               
        

             

      
                                                                              
                                                                                
                                                                               
   
          
                                                                    
                                                                       
                                                               
   
              
                                                              

                                                                  
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPPhysicalLinkAccept 
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Accept_Physical_Link_Cmd   *pBapHCIPhysLinkAccept,
  v_PVOID_t      pHddHdl,   /*                                    */
                            /*                                            */
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */
);

/*                                                                            

                                              

              
                                                              

               
        

             

      
                                                                              
                                                                                        
   
          
                                                                    
                                                                       
                                                               
   
              
                                                              

                                                                      
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPPhysicalLinkDisconnect 
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Disconnect_Physical_Link_Cmd   *pBapHCIPhysLinkDisconnect,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */
);

/*                                                                            

                                         

              
                                                         

               
        

             

      
                                                                              
                                                                              
   
          
                                                                    
                                                                       
                                                               
   
              
                                                              

                                                                 
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPLogicalLinkCreate
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Create_Logical_Link_Cmd   *pBapHCILogLinkCreate,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */
);

/*                                                                            

                                         

              
                                                         

               
        

             

      
                                                                              
                                                                              
   
          
                                                                    
                                                                       
                                                               
   
              
                                                              

                                                                 
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPLogicalLinkAccept
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Accept_Logical_Link_Cmd   *pBapHCILogLinkAccept,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */
);

/*                                                                            

                                             

              
                                                             

               
        

             

      
                                                                              
                                                                                      
   
          
                                                                    
                                                                       
                                                               
   
              
                                                              

                                                                     
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPLogicalLinkDisconnect
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Disconnect_Logical_Link_Cmd   *pBapHCILogLinkDisconnect,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */
);

/*                                                                            

                                         

              
                                                         

               
        

             

      
                                                                              
                                                                              
   
          
                                                                      
                                                                       
                                                                 
                                                                     
                                                                       
                                              
   
              
                                                              

                                                                 
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPLogicalLinkCancel
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Logical_Link_Cancel_Cmd   *pBapHCILogLinkCancel,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */
);

/*                                                                            

                                      

              
                                                         
                                                                          
                   

               
        

             

      
                                                                              
                                                                            
   
          
                                                                    
                                                                       
                                                               
   
              
                                                              

                                                                  
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPFlowSpecModify
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Flow_Spec_Modify_Cmd   *pBapHCIFlowSpecModify,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */
);

/*                                       */

/*                                                                            

                             

              
                                            
                                                                 
                                                             

               
        

             

      
                                                                              
   
              
                                                              

                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPReset
( 
  ptBtampHandle btampHandle
);

/*                                                                            

                                    

              
                                                                            
                                                                            

               
        

             

      
                                                                              
                                                                        
   
          
                                                                      
                                                                       
                                                                 
   
              
                                                              

                                                                
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPSetEventMask
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Set_Event_Mask_Cmd   *pBapHCISetEventMask,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */
);

/*                                                                            

                             

              
                                           
                                                                 
                                                                               
                   

               
        

             

      
                                                                              
                                                        
   
              
                                                              

                                                         
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPFlush
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Flush_Cmd     *pBapHCIFlush
);

/*                                                                            

                                     

              
                                                    
                                                                                
                                                                       

               
        

             

      
                                                                              
                                                                 
          
                                                                      
                                                                       
                                                                 
   
              
                                                              

                                                         
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_EnhancedBAPFlush
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Enhanced_Flush_Cmd     *pBapHCIFlush,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */

);

/*                                                                            

                                                   

              
                                                                             
                                                                        
                       

               
        

             

      
                                                                              
   
          
                                                                      
                                                                       
                                                                 
   
              
                                                              

                                                                               
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPReadConnectionAcceptTimeout
( 
  ptBtampHandle btampHandle,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                   */
);

/*                                                                            

                                                    

              
                                                                              
                                                                        
                       

               
        

             

      
                                                                              
                                                                                                   
   
          
                                                                      
                                                                       
                                                                 
   
              
                                                              

                                                                                
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPWriteConnectionAcceptTimeout
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Write_Connection_Accept_Timeout_Cmd   *pBapHCIWriteConnectionAcceptTimeout,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */
);

/*                                                                            

                                                  

              
                                                                            
                                                                        
                       

               
        

             

      
                                                                              
   
          
                                                                      
                                                                       
                                                                 
   
              
                                                              

                                                                              
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPReadLinkSupervisionTimeout
( 
  ptBtampHandle btampHandle,
  /*                                                                  */
  tBtampTLVHCI_Read_Link_Supervision_Timeout_Cmd *pBapHCIReadLinkSupervisionTimeout,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                  */
);

/*                                                                            

                                                   

              
                                                                             
                                                                        
                       

               
        

             

      
                                                                              
                                                                                                 
   
          
                                                                      
                                                                       
                                                                 
   
              
                                                              

                                                                               
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPWriteLinkSupervisionTimeout
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Write_Link_Supervision_Timeout_Cmd   *pBapHCIWriteLinkSupervisionTimeout,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */
);

/*                                            */


/*                                                                            

                                                    

              
                                                                               
                                                                        
                       

               
        

             

      
                                                                              
   
          
                                                                      
                                                                       
                                                                 
   
              
                                                              

                                                                                
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPReadLogicalLinkAcceptTimeout
( 
  ptBtampHandle btampHandle,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                  */
);

/*                                                                            

                                                     

              
                                                                                
                                                                        
                       

               
        

             

      
                                                                              
                                                                                                      
   
          
                                                                      
                                                                       
                                                                 
   
              
                                                              

                                                                                 
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPWriteLogicalLinkAcceptTimeout
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Write_Logical_Link_Accept_Timeout_Cmd   *pBapHCIWriteLogicalLinkAcceptTimeout,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */
);

/*                                                                            

                                         

              
                                                                                   
                                                                            

               
        

             

      
                                                                              
                                                                                    
   
          
                                                                      
                                                                       
                                                                 
   
              
                                                              

                                                                     
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPSetEventMaskPage2
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Set_Event_Mask_Page_2_Cmd   *pBapHCISetEventMaskPage2,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */
);

/*                                                                            

                                        

              
                                                                 
                                                                        
                       

               
        

             

      
                                                                              
   
          
                                                                      
                                                                       
                                                                 
   
              
                                                              

                                                                    
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPReadLocationData
( 
  ptBtampHandle btampHandle,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                  */
);

/*                                                                            

                                         

              
                                                                  
                                                                        
                       

               
        

             

      
                                                                              
                                                                                  
   
          
                                                                      
                                                                       
                                                                 
   
              
                                                              

                                                                     
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPWriteLocationData
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Write_Location_Data_Cmd   *pBapHCIWriteLocationData,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */
);


/*                                                                            

                                           

              
                                                                     
                                                                        
                       

               
        

             

      
                                                                              
   
          
                                                                      
                                                                       
                                                                 
   
              
                                                              

                                                                       
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPReadFlowControlMode
( 
  ptBtampHandle btampHandle,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                  */
);

/*                                                                            

                                            

              
                                                                      
                                                                        
                       

               
        

             

      
                                                                              
                                                                                         
   
          
                                                                      
                                                                       
                                                                 
   
              
                                                              

                                                                        
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPWriteFlowControlMode
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Write_Flow_Control_Mode_Cmd   *pBapHCIWriteFlowControlMode,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */
);

/*                                                                            

                                                  

              
                                                                             
                                                                        
                       

               
        

             

      
                                                                              
   
          
                                                                      
                                                                       
                                                                 
   
              
                                                              

                                                                 
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPReadBestEffortFlushTimeout
( 
  ptBtampHandle btampHandle,
  /*                                                          */
  tBtampTLVHCI_Read_Best_Effort_Flush_Timeout_Cmd   *pBapHCIReadBEFlushTO,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                  */
);

/*                                                                            

                                                   

              
                                                                         
                                                                        
                       

               
        

             

      
                                                                              
                                                                             
   
          
                                                                      
                                                                       
                                                                 
   
              
                                                              

                                                                  
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPWriteBestEffortFlushTimeout
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Write_Best_Effort_Flush_Timeout_Cmd   *pBapHCIWriteBEFlushTO,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */
);


/*                                                                            

                                         

              
                                                                                  
                                                                            

               
        

             

      
                                                                              
                                                                                
   
          
                                                                      
                                                                       
                                                                 
   
              
                                                              

                                                                  
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPSetShortRangeMode
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Set_Short_Range_Mode_Cmd   *pBapHCIShortRangeMode,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */
);


/*                                                                            

                                          

             
                                                                               
                                                                                 
                   

              
       

            

      
                                                                              

          
                                                                     
                                                                      
                                                                 

              
                                                            

                                                        
                                

              

                                                                            */
VOS_STATUS
WLAN_BAPVendorSpecificCmd0
(
  ptBtampHandle btampHandle,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */
);

/*                                                                            

                                          

             
                                                                               
                                                                                 
                   

              
       

            

      
                                                                              

          
                                                                     
                                                                      
                                                                 

              
                                                            

                                                        
                                

              

                                                                            */
VOS_STATUS
WLAN_BAPVendorSpecificCmd1
(
  ptBtampHandle btampHandle,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */
);

/*                                                   */


/*                          */

/*                                                                            

                                            

              
                                                                      
                                                                        
                       

               
        

             

      
                                                                              
                                                              
                                                    
   
          
                                                                      
                                                                       
                                                                 
   
              
                                                              

                                                                        
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPReadLocalVersionInfo
( 
  ptBtampHandle btampHandle,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                  */
);

/*                                                                            

                                              

              
                                                                    
                                                                        
                       

               
        

             

      
                                                                              
                                                              
                                                    
   
          
                                                                      
                                                                       
                                                                 
   
              
                                                              

                                                                          
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPReadLocalSupportedCmds
( 
  ptBtampHandle btampHandle,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                  */
);

/*                                                                            

                                      

              
                                                               
                                                                        
                       

               
        

             

      
                                                                              
   
          
                                                                      
                                                                       
                                                                 
   
              
                                                              

                                                                  
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPReadBufferSize
( 
  ptBtampHandle btampHandle,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                  */
);

/*                                                                            

                                         

              
                                                                   
                                                                        
                       

               
        

             

      
                                                                              
   
          
                                                                      
                                                                       
                                                                 
   
              
                                                              

                                                                     
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPReadDataBlockSize
( 
  ptBtampHandle btampHandle,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                  */
);

/*
                 
*/

/*                                                                            

                                                

              
                                                                          
                                                                        
                       

               
        

             

      
                                                                              
                                                                                                 
   
          
                                                                      
                                                                       
                                                                 
   
              
                                                              

                                                                      
                                                       
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPReadFailedContactCounter
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Read_Failed_Contact_Counter_Cmd  *pBapHCIReadFailedContactCounter,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                  */
);

/*                                                                            

                                                 

              
                                                                           
                                                                        
                       

               
        

             

      
                                                                              
                                                                                                   
   
          
                                                                      
                                                                       
                                                                 
   
              
                                                              

                                                                            
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPResetFailedContactCounter
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Reset_Failed_Contact_Counter_Cmd *pBapHCIResetFailedContactCounter,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */
);

/*                                                                            

                                       

              
                                                                
                                                                        
                       

               
        

             

      
                                                                              
                                                                              
   
          
                                                                      
                                                                       
                                                                 
   
              
                                                              

                                                              
                                                    
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPReadLinkQuality
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Read_Link_Quality_Cmd *pBapHCIReadLinkQuality,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */
);

/*                                                                            

                                

              
                                                        
                                                                        
                       

               
        

             

      
                                                                              
                                                               
   
          
                                                                      
                                                                       
                                                                 
   
              
                                                              

                                                      
                                             
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPReadRSSI
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Read_RSSI_Cmd *pBapHCIReadRSSI,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */
);

/*                                                                            

                                        

              
                                                                         
                                                                        
                       

               
        

             

      
                                                                              
                                                                                 
   
          
                                                                      
                                                                       
                                                                 
   
              
                                                              

                                                               
                                                    
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPReadLocalAMPInfo
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Read_Local_AMP_Information_Cmd *pBapHCIReadLocalAMPInfo,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */
);

/*                                                                            

                                         

              
                                                                   
                                                                        
                       

               
        

             

      
                                                                              
                                                                                   
    
          
                                                                      
                                                                       
                                                                 
   
   
              
                                                              

                                                             
                                                          
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPReadLocalAMPAssoc
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Read_Local_AMP_Assoc_Cmd   *pBapHCIReadLocalAMPAssoc,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */
);

/*                                                                            

                                           

              
                                                                     
                                                                        
                       

               
        

             

      
                                                                              
                                                                                       
    
          
                                                                      
                                                                       
                                                                 
   
              
                                                              

                                                                       
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPWriteRemoteAMPAssoc
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Write_Remote_AMP_ASSOC_Cmd   *pBapHCIWriteRemoteAMPAssoc,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */
);

/*
              
*/

/*                                                                            

                                        

              
                                                                 
                                                                        
                       

               
        

             

      
                                                                              
                                                                      
   
          
                                                                      
                                                                       
                                                                 
   
              
                                                              

                                                               
                                                     
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPReadLoopbackMode
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Read_Loopback_Mode_Cmd  *pBapHCIReadLoopbackMode,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */
);

/*                                                                            

                                         

              
                                                                  
                                                                        
                       

               
        

             

      
                                                                              
                                                                                  
    
          
                                                                      
                                                                       
                                                                 
   
              
                                                              

                                                                     
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPWriteLoopbackMode
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Write_Loopback_Mode_Cmd   *pBapHCIWriteLoopbackMode,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */
);


/*                                                                            

                                 

              
                                                                              
                            
   
                                                               


               
        

             

      
                                                                              
                                                               
   
      
                                                                                         
   
              
                                                              

                                                        
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPSetConfig
( 
  ptBtampHandle btampHandle,
  WLANBAP_ConfigType *pConfig
);

/*                                                                           

                                        

              

                                                                            
                                                                         
                          


             

                                                                      
 
                                           
 
                                                       
   
              

                                                              

                                                      
                                             
                                               

               
  
                                                                            */
VOS_STATUS
WLANBAP_GetAcFromTxDataPkt
( 
  ptBtampHandle     btampHandle,  /*                                           
                                                            */
  void              *pHciData,     /*                               */
  WLANTL_ACEnumType *pucAC        /*                    */
);

/*                                                                            

                               

              
                                                            
   


               
        

             

      
                                                                              
    
   
      
                                                                          
   
              
                                                              

                                                                   
                                
  
               
  
                                                                            */
VOS_STATUS
WLAN_BAPGetMask( ptBtampHandle btampHandle, 
                 v_U8_t       *pEvent_mask_page_2);

/*                                                                            

                                  

              
                                                                               
   


               
        

             

      
                                                                              
    
   
              
                                                              

                                             
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPDisconnect
( 
  ptBtampHandle btampHandle
);

/*                                                                            

                                 

              
                                                                           
   


               
        

             

      
                                                                              
    
   
              
                                                              

                                    
                                       
  
               
  
                                                                            */
v_BOOL_t WLAN_BAPSessionOn
( 
  ptBtampHandle btampHandle
);

#ifdef __cplusplus
 }
#endif 


#endif /*                            */


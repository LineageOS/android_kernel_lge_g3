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

/*===========================================================================

                      b a p A p i I n f o . C
                                               
  OVERVIEW:
  
  This software unit holds the implementation of the WLAN BAP modules
  Information functions.
  
  The functions externalized by this module are to be called ONLY by other 
  WLAN modules (HDD) that properly register with the BAP Layer initially.

  DEPENDENCIES: 

  Are listed for each API below. 
  
  
  Copyright (c) 2008 QUALCOMM Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*                                                                           

                                           


                                                                       
                                                                


                                                                                                                                                                                                                           


                                      
                                                                             
                                    

                                                                           */

/*                                                                            
                
                                                                            */
//                        
#include "vos_trace.h"
#include "sme_Api.h"
/*                            */ 
#include "bapApi.h" 
#include "bapInternal.h" 

//                 
/*                                                                            
                                         
                                                                            */


/*                                                                            
                    
                                                                            */

/*                                                                            
                          
                                                                            */

/*                                                                            
                              
                                                                            */

/*                                                                            
                                               
                                                                            */

/*                                                                            
                                    
                                                                           */

/*                                                                            
                                          
                                                                            */


/*                          */

/*                                                                            

                                            

              
                                                                      
                                                                        
                       

               
        

             

      
                                                                              
                                                              
                                                    
   
          
                                                                      
                                                                       
                                                                 
   
              
                                                              

                                                                        
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPReadLocalVersionInfo
( 
  ptBtampHandle btampHandle,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                  */
)
{

    /*                 */ 
    if (btampHandle == NULL) {
      return VOS_STATUS_E_FAULT;
    }


    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "%s: btampHandle value: %p", __func__,  btampHandle);


    /*                                                */ 
    pBapHCIEvent->bapHCIEventCode = BTAMP_TLV_HCI_COMMAND_COMPLETE_EVENT;
    pBapHCIEvent->u.btampCommandCompleteEvent.present = 1;
    pBapHCIEvent->u.btampCommandCompleteEvent.num_hci_command_packets = 1;
    pBapHCIEvent->u.btampCommandCompleteEvent.command_opcode 
        = BTAMP_TLV_HCI_READ_LOCAL_VERSION_INFO_CMD;
    pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_Local_Version_Info.status 
        = WLANBAP_STATUS_SUCCESS;
    pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_Local_Version_Info.HC_HCI_Version
        = WLANBAP_HCI_VERSION;
    pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_Local_Version_Info.HC_HCI_Revision
        = WLANBAP_HCI_REVISION;
    pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_Local_Version_Info.HC_PAL_Version
        = WLANBAP_PAL_VERSION;
    pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_Local_Version_Info.HC_Manufac_Name
        = WLANBAP_QUALCOMM_COMPANY_ID;
    pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_Local_Version_Info.HC_PAL_Sub_Version
        = WLANBAP_PAL_SUBVERSION;

    return VOS_STATUS_SUCCESS;
} /*                              */

/*                                                                            

                                              

              
                                                                    
                                                                        
                       

               
        

             

      
                                                                              
                                                              
                                                    
   
          
                                                                      
                                                                       
                                                                 
   
              
                                                              

                                                                          
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPReadLocalSupportedCmds
( 
  ptBtampHandle btampHandle,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                  */
)
{
    v_U8_t supportedCmds[] = WLANBAP_PAL_SUPPORTED_HCI_CMDS;
    /*                                                                       */


    /*                 */ 
    if (btampHandle == NULL) {
      return VOS_STATUS_E_FAULT;
    }


    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "%s: btampHandle value: %p", __func__,  btampHandle);


    /*                                                */ 
    pBapHCIEvent->bapHCIEventCode = BTAMP_TLV_HCI_COMMAND_COMPLETE_EVENT;
    pBapHCIEvent->u.btampCommandCompleteEvent.present = 1;
    pBapHCIEvent->u.btampCommandCompleteEvent.num_hci_command_packets = 1;
    pBapHCIEvent->u.btampCommandCompleteEvent.command_opcode 
        = BTAMP_TLV_HCI_READ_LOCAL_SUPPORTED_CMDS_CMD;
    pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_Local_Supported_Cmds.status 
        = WLANBAP_STATUS_SUCCESS;
    /*                                       */ 
    vos_mem_copy( 
            pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_Local_Supported_Cmds.HC_Support_Cmds,
            supportedCmds,
            sizeof( supportedCmds));    

    return VOS_STATUS_SUCCESS;
} /*                                */

/*                                                                            

                                      

              
                                                               
                                                                        
                       

               
        

             

      
                                                                              
                                                                            
   
          
                                                                      
                                                                       
                                                                 
   
              
                                                              

                                                                  
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPReadBufferSize
( 
  ptBtampHandle btampHandle,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                  */
)
{
    /*                 */ 
    if (btampHandle == NULL) {
      return VOS_STATUS_E_FAULT;
    }


    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "%s: btampHandle value: %p", __func__,  btampHandle);


    /*                                                */ 
    pBapHCIEvent->bapHCIEventCode = BTAMP_TLV_HCI_COMMAND_COMPLETE_EVENT;
    pBapHCIEvent->u.btampCommandCompleteEvent.present = 1;
    pBapHCIEvent->u.btampCommandCompleteEvent.num_hci_command_packets = 1;
    pBapHCIEvent->u.btampCommandCompleteEvent.command_opcode 
        = BTAMP_TLV_HCI_READ_BUFFER_SIZE_CMD;
    pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_Buffer_Size.status 
        = WLANBAP_STATUS_SUCCESS;
    /*                                   */ 
    pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_Buffer_Size.HC_ACL_Data_Packet_Length
        = WLANBAP_MAX_80211_PAL_PDU_SIZE;
    pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_Buffer_Size.HC_SCO_Packet_Length
        = 0; /*                                      */
    pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_Buffer_Size.HC_Total_Num_ACL_Packets
        = 16;
    pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_Buffer_Size.HC_Total_Num_SCO_Packets
        = 0;

    return VOS_STATUS_SUCCESS;
} /*                        */

/*                                                                            

                                         

              
                                                                   
                                                                        
                       

               
        

             

      
                                                                              
   
          
                                                                      
                                                                       
                                                                 
   
              
                                                              

                                                                     
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPReadDataBlockSize
( 
  ptBtampHandle btampHandle,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                  */
)
{
   /*                 */ 
   if ((btampHandle == NULL) || (NULL == pBapHCIEvent))
   {
     return VOS_STATUS_E_FAULT;
   }


   VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "%s: btampHandle value: %p", __func__,  btampHandle);


   /*                                                */ 
   pBapHCIEvent->bapHCIEventCode = BTAMP_TLV_HCI_COMMAND_COMPLETE_EVENT;
   pBapHCIEvent->u.btampCommandCompleteEvent.present = 1;
   pBapHCIEvent->u.btampCommandCompleteEvent.num_hci_command_packets = 1;
   pBapHCIEvent->u.btampCommandCompleteEvent.command_opcode 
       = BTAMP_TLV_HCI_READ_DATA_BLOCK_SIZE_CMD;
   pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_Data_Block_Size.status 
       = WLANBAP_STATUS_SUCCESS;
   /*                                  */ 
   pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_Data_Block_Size.HC_Data_Block_Length
       = WLANBAP_MAX_80211_PAL_PDU_SIZE;
   pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_Data_Block_Size.HC_Max_ACL_Data_Packet_Length
       = WLANBAP_MAX_80211_PAL_PDU_SIZE;
   pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_Data_Block_Size.HC_Total_Num_Data_Blocks
       = 16;

    return VOS_STATUS_SUCCESS;
} /*                           */


/*                                                                            

                                 

              
                                                                              
                            
   
                                                               


               
        

             

      
                                                                              
                                                               
   
      
                                                                                         
   
              
                                                              

                                                        
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPSetConfig
( 
  ptBtampHandle btampHandle,
  WLANBAP_ConfigType *pConfig
)
{
   ptBtampContext btampContext;
   /*                 */ 
   if ((NULL == btampHandle)|| (NULL == pConfig)) 
   {
     return VOS_STATUS_E_FAULT;
   }
   btampContext = (ptBtampContext) btampHandle; /*                    */ 

   btampContext->config.ucPreferredChannel = pConfig->ucPreferredChannel;
   return VOS_STATUS_SUCCESS;
}

/*                                                                            

                               

              
                                                            
   


               
        

             

      
                                                                              
    
   
      
                                                                          
   
              
                                                              

                                                                   
                                
  
               
  
                                                                            */
VOS_STATUS
WLAN_BAPGetMask( ptBtampHandle btampHandle, 
                 v_U8_t       *pEvent_mask_page_2)
{
   ptBtampContext btampContext;
   /*                 */ 
   if ((NULL == btampHandle)|| (NULL == pEvent_mask_page_2)) 
   {
     return VOS_STATUS_E_FAULT;
   }
   btampContext = (ptBtampContext) btampHandle; /*                    */ 

   vos_mem_copy( pEvent_mask_page_2, 
                 btampContext->event_mask_page_2, 
                 8 );
   return VOS_STATUS_SUCCESS;
}

/*                                                                            

                                  

              
                                                                               
   


               
        

             

      
                                                                              
    
   
              
                                                              

                                             
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPDisconnect
( 
  ptBtampHandle btampHandle
)
{
    ptBtampContext btampContext = (ptBtampContext) btampHandle;
    tWLAN_BAPEvent bapEvent; /*                     */
    v_U8_t status;    /*                               */
    VOS_STATUS  vosStatus;
    /*                                                                       */

    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_FATAL, "%s: btampHandle value: %p", __func__,  btampHandle);

    /*                 */ 
    if (btampHandle == NULL) 
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_FATAL,
                     "btampHandle is NULL in %s", __func__);

      return VOS_STATUS_E_FAULT;
    }

    /*                             */ 
    bapEvent.event = eWLAN_BAP_MAC_INDICATES_MEDIA_DISCONNECTION;
    bapEvent.params = NULL;


    /*              */ 
    vosStatus = btampFsm(btampContext, &bapEvent, &status);


        /*                             */ 
    bapEvent.event =  eWLAN_BAP_MAC_READY_FOR_CONNECTIONS;
    bapEvent.params = NULL;

        /*              */ 
    vosStatus = btampFsm(btampContext, &bapEvent, &status);


    return VOS_STATUS_SUCCESS;
}

/*                                                                            

                                 

              
                                                                           
   


               
        

             

      
                                                                              


              
                                                              

                                    
                                       
  
               
  
                                                                            */
v_BOOL_t WLAN_BAPSessionOn
( 
  ptBtampHandle btampHandle
)
{
   ptBtampContext btampContext = (ptBtampContext) btampHandle;
   /*                                                                       */

   VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "%s: btampHandle value: %p", __func__,  btampHandle);

   /*                 */ 
   if (btampHandle == NULL) 
   {
       VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                    "btampHandle is NULL in %s", __func__);

       //                             
       return VOS_FALSE;
   }

   return btampContext->btamp_session_on;
}

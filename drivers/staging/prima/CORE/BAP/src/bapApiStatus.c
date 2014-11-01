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

                      b a p A p i S t a t u s . C
                                               
  OVERVIEW:
  
  This software unit holds the implementation of the WLAN BAP modules
  Status functions.
  
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
)
{

    return VOS_STATUS_SUCCESS;
} /*                                  */

/*                                                                            

                                                 

              
                                                                           
                                                                        
                       

               
        

             

      
                                                                              
                                                                                                   
   
          
                                                                      
                                                                       
                                                                 
   
              
                                                              

                                                                            
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPResetFailedContactCounter
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Reset_Failed_Contact_Counter_Cmd *pBapHCIResetFailedContactCounter,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */
)
{

    return VOS_STATUS_SUCCESS;
} /*                                   */

/*                                                                            

                                       

              
                                                                
                                                                        
                       

               
        

             

      
                                                                              
                                                                              
                                                                       
   
          
                                                                      
                                                                       
                                                                 
   
              
                                                              

                                                              
                                                    
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPReadLinkQuality
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Read_Link_Quality_Cmd *pBapHCIReadLinkQuality,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */
)
{
    ptBtampContext btampContext = (ptBtampContext) btampHandle;
    v_U8_t         phyLinkHandle;
    /*                                                                       */

    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH,
               "%s: btampHandle value: %p", __func__,  btampHandle);

    /*                 */ 
    if ((NULL == btampHandle) || (NULL == pBapHCIReadLinkQuality) ||
        (NULL == pBapHCIEvent))
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH,
                   "Invalid input parameters in %s", __func__);
        return VOS_STATUS_E_FAULT;
    }

    /*                                                 
                                                                      
                               */
    phyLinkHandle = (v_U8_t) pBapHCIReadLinkQuality->log_link_handle;

    pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_Link_Quality.log_link_handle
        = phyLinkHandle;
    pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_Link_Quality.link_quality = 0;

    if (phyLinkHandle != btampContext->phy_link_handle)
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH,
                   "Invalid Physical link handle in %s", __func__);
        pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_Link_Quality.status
            = WLANBAP_ERROR_INVALID_HCI_CMND_PARAM;
    }
    else
    {
        /*                                                           
                                                                   */
        pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_Link_Quality.link_quality
            = btampContext->link_quality;

        pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_Link_Quality.status
            = WLANBAP_STATUS_SUCCESS;
    }

    /*                                                      */ 
    pBapHCIEvent->bapHCIEventCode = BTAMP_TLV_HCI_COMMAND_COMPLETE_EVENT;
    pBapHCIEvent->u.btampCommandCompleteEvent.present = TRUE;
    pBapHCIEvent->u.btampCommandCompleteEvent.num_hci_command_packets = 1;
    pBapHCIEvent->u.btampCommandCompleteEvent.command_opcode 
        = BTAMP_TLV_HCI_READ_LINK_QUALITY_CMD;

    return VOS_STATUS_SUCCESS;
} /*                         */

/*                                                                            

                                

              
                                                        
                                                                        
                       

               
        

             

      
                                                                              
                                                               
                                                         
   
          
                                                                      
                                                                       
                                                                 
   
              
                                                              

                                                      
                                             
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPReadRSSI
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Read_RSSI_Cmd *pBapHCIReadRSSI,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */
)
{
    VOS_STATUS     vosStatus;
    ptBtampContext btampContext = (ptBtampContext) btampHandle;
    v_U8_t         phyLinkHandle;
    /*                                                                       */

    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH,
               "%s: btampHandle value: %p", __func__,  btampHandle);

    /*                 */ 
    if ((NULL == btampHandle) || (NULL == pBapHCIReadRSSI) ||
        (NULL == pBapHCIEvent))
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH,
                   "Invalid input parameters in %s", __func__);
        return VOS_STATUS_E_FAULT;
    }

    /*                                                 
                                                                      
                               */
    phyLinkHandle = (v_U8_t) pBapHCIReadRSSI->log_link_handle;

    pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_RSSI.phy_link_handle
        = phyLinkHandle;
    pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_RSSI.rssi = 0;

    if (phyLinkHandle != btampContext->phy_link_handle)
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH,
                   "Invalid Physical link handle in %s", __func__);
        pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_RSSI.status
            = WLANBAP_ERROR_INVALID_HCI_CMND_PARAM;
    }
    else
    {
        /*                                                     */
        vosStatus = WLANTL_GetRssi(btampContext->pvosGCtx, btampContext->ucSTAId,
                        &pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_RSSI.rssi);

        if (VOS_STATUS_SUCCESS == vosStatus)
        {
            /*                                              */
            pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_RSSI.status
                = WLANBAP_STATUS_SUCCESS;
        }
        else
        {
            /*                                                       */
            pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_RSSI.status
                = WLANBAP_ERROR_UNSPECIFIED_ERROR;
        }
    }

    /*                                                      */ 
    pBapHCIEvent->bapHCIEventCode = BTAMP_TLV_HCI_COMMAND_COMPLETE_EVENT;
    pBapHCIEvent->u.btampCommandCompleteEvent.present = TRUE;
    pBapHCIEvent->u.btampCommandCompleteEvent.num_hci_command_packets = 1;
    pBapHCIEvent->u.btampCommandCompleteEvent.command_opcode 
        = BTAMP_TLV_HCI_READ_RSSI_CMD;

    return VOS_STATUS_SUCCESS;
} /*                  */

/*                                                                            

                                        

              
                                                                         
                                                                        
                       

               
        

             

      
                                                                              
                                                                                 
   
          
                                                                      
                                                                       
                                                                 
   
              
                                                              

                                                               
                                                    
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPReadLocalAMPInfo
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Read_Local_AMP_Information_Cmd *pBapHCIReadLocalAMPInfo,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */
)
{
    /*                 */ 
    if (btampHandle == NULL) {
      return VOS_STATUS_E_FAULT;
    }

    /*                 */ 
    if (pBapHCIReadLocalAMPInfo == NULL) {
      return VOS_STATUS_E_FAULT;
    }

    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "%s: btampHandle value: %p", __func__,  btampHandle);


    /*                                                */ 
    pBapHCIEvent->bapHCIEventCode = BTAMP_TLV_HCI_COMMAND_COMPLETE_EVENT;
    pBapHCIEvent->u.btampCommandCompleteEvent.present = 1;
    pBapHCIEvent->u.btampCommandCompleteEvent.num_hci_command_packets = 1;
    pBapHCIEvent->u.btampCommandCompleteEvent.command_opcode 
        = BTAMP_TLV_HCI_READ_LOCAL_AMP_INFORMATION_CMD;
    pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_Local_AMP_Info.status 
        = WLANBAP_STATUS_SUCCESS;
    pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_Local_AMP_Info.HC_AMP_Status
        = WLANBAP_HCI_AMP_STATUS_NOT_SHARED;
    pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_Local_AMP_Info.HC_Total_BW
        = 24000;
    pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_Local_AMP_Info.HC_Max_Guaranteed_BW
        = 12000;
    pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_Local_AMP_Info.HC_Min_Latency
        = 100;
    pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_Local_AMP_Info.HC_Max_PDU_Size
        = WLANBAP_MAX_80211_PAL_PDU_SIZE;
    pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_Local_AMP_Info.HC_Controller_Type 
        = 1;
#if 0
AMP Info PAL_Capabilities: Size: 2 Octets

Value    Parameter Description
0xXXXX   Bit 0: "Service Type = Guaranteed" is not supported by PAL = 0
                "Service Type = Guaranteed" is supported by PAL = 1
         Bits 15-1: Reserved (shall be set to 0)
         (See EFS in Generic AMP FIPD [1])
#endif // 
    pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_Local_AMP_Info.HC_PAL_Capabilities
        = 0x00; //                            
    pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_Local_AMP_Info.HC_AMP_Assoc_Length
        = 248;
        //     
    pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_Local_AMP_Info.HC_Max_Flush_Timeout
        = 10000;  //   
    pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_Local_AMP_Info.HC_BE_Flush_Timeout
        = 10000; //  

    return VOS_STATUS_SUCCESS;
} /*                          */

/*                                                                            

                                         

              
                                                                   
                                                                        
                       

               
        

             

      
                                                                              
                                                                                   
    
          
                                                                      
                                                                       
                                                                 
   
   
              
                                                              

                                                             
                                                          
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPReadLocalAMPAssoc
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Read_Local_AMP_Assoc_Cmd   *pBapHCIReadLocalAMPAssoc,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */
)
{
    VOS_STATUS  vosStatus;
    ptBtampContext btampContext = (ptBtampContext) btampHandle; /*                    */ 
    tHalHandle hHal;
    tBtampAMP_ASSOC btamp_ASSOC; 
    v_U32_t nConsumed = 0;
    /*                                                                       */


    /*                 */ 
    if ((pBapHCIReadLocalAMPAssoc == NULL) || (NULL == btampHandle))
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                     "param is NULL in %s", __func__);

        return VOS_STATUS_E_FAULT;
    }
    hHal = VOS_GET_HAL_CB(btampContext->pvosGCtx);
    if (NULL == hHal) 
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                     "hHal is NULL in %s", __func__);
      return VOS_STATUS_E_FAULT;
    }

    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "%s: btampHandle value: %p", __func__,  btampHandle);

    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, 
            "In %s, phy_link_handle = %d", __func__, 
            pBapHCIReadLocalAMPAssoc->phy_link_handle); 

    /*                                                                    
                                                            
     */
    WLANBAP_ReadMacConfig (btampContext);

    /*                                                              
                                                                    
                                              
     */ 
    /*                              */ 
    btamp_ASSOC.AMP_Assoc_MAC_Addr.present = 1;
    vos_mem_copy( 
            btamp_ASSOC.AMP_Assoc_MAC_Addr.mac_addr,   
            btampContext->self_mac_addr, 
            sizeof(btampContext->self_mac_addr)); 

    /*                             */
    vos_mem_copy(btampContext->btamp_AMP_Assoc.HC_mac_addr,
                 btampContext->self_mac_addr, 
                 sizeof(btampContext->self_mac_addr)); 


    /*                                                                    */ 
    /*                                   */
    //                            
    if (1)  
    { 
        /*                                         */ 
        /*                                                      */ 
        btamp_ASSOC.AMP_Assoc_Preferred_Channel_List.present = 1; 
        memcpy (btamp_ASSOC.AMP_Assoc_Preferred_Channel_List.country, "XXX", 3); 
        /*                             */
        vos_mem_copy(btampContext->btamp_AMP_Assoc.HC_pref_country, "XXX", 3);

        btamp_ASSOC.AMP_Assoc_Preferred_Channel_List.num_triplets = 2; 
        btamp_ASSOC.AMP_Assoc_Preferred_Channel_List.triplets[0][0] = 201; 
        btamp_ASSOC.AMP_Assoc_Preferred_Channel_List.triplets[0][1] = 254; 
        btamp_ASSOC.AMP_Assoc_Preferred_Channel_List.triplets[0][2] = 0; 

        if (( BT_INITIATOR == btampContext->BAPDeviceRole ) &&
            ( 0 != btampContext->channel ))
        {
          btamp_ASSOC.AMP_Assoc_Preferred_Channel_List.triplets[1][0] = btampContext->channel;
          btamp_ASSOC.AMP_Assoc_Preferred_Channel_List.triplets[1][1] = 0x01; //                                                    
          btamp_ASSOC.AMP_Assoc_Preferred_Channel_List.triplets[1][2] = 0x11;
        }
        else
        {
            if (btampContext->config.ucPreferredChannel)
            {
                btamp_ASSOC.AMP_Assoc_Preferred_Channel_List.triplets[1][0] = btampContext->config.ucPreferredChannel;
                btamp_ASSOC.AMP_Assoc_Preferred_Channel_List.triplets[1][1] = 
                    0x0B - btampContext->config.ucPreferredChannel + 1;  
            }
            else
            {
                btamp_ASSOC.AMP_Assoc_Preferred_Channel_List.triplets[1][0] = 0x01; 
                btamp_ASSOC.AMP_Assoc_Preferred_Channel_List.triplets[1][1] = 0x0B; //                         
            }

            btamp_ASSOC.AMP_Assoc_Preferred_Channel_List.triplets[1][2] = 0x11;
        }
    } else 
    { 
        /*                                         */ 
        /*                                 */ 
        btamp_ASSOC.AMP_Assoc_Preferred_Channel_List.present = 1; 
        memcpy (btamp_ASSOC.AMP_Assoc_Preferred_Channel_List.country, "XXX", 3); 
        btamp_ASSOC.AMP_Assoc_Preferred_Channel_List.num_triplets = 1; 
        btamp_ASSOC.AMP_Assoc_Preferred_Channel_List.triplets[0][0] = 201; 
        btamp_ASSOC.AMP_Assoc_Preferred_Channel_List.triplets[0][1] = 254; 
        btamp_ASSOC.AMP_Assoc_Preferred_Channel_List.triplets[0][2] = 0;

    }  

    /*                             */
    btampContext->btamp_AMP_Assoc.HC_pref_num_triplets   = btamp_ASSOC.AMP_Assoc_Preferred_Channel_List.num_triplets;
    btampContext->btamp_AMP_Assoc.HC_pref_triplets[0][0] = btamp_ASSOC.AMP_Assoc_Preferred_Channel_List.triplets[0][0];
    btampContext->btamp_AMP_Assoc.HC_pref_triplets[0][1] = btamp_ASSOC.AMP_Assoc_Preferred_Channel_List.triplets[0][1]; 
    btampContext->btamp_AMP_Assoc.HC_pref_triplets[0][2] = btamp_ASSOC.AMP_Assoc_Preferred_Channel_List.triplets[0][2];
    btampContext->btamp_AMP_Assoc.HC_pref_triplets[1][0] = btamp_ASSOC.AMP_Assoc_Preferred_Channel_List.triplets[1][0];
    btampContext->btamp_AMP_Assoc.HC_pref_triplets[1][1] = btamp_ASSOC.AMP_Assoc_Preferred_Channel_List.triplets[1][1]; 
    btampContext->btamp_AMP_Assoc.HC_pref_triplets[1][2] = btamp_ASSOC.AMP_Assoc_Preferred_Channel_List.triplets[1][2];

    /*                                                                      */
    /*                                                                         */
    /*                                                                   */ 
    /*                                    */ 
    btamp_ASSOC.AMP_Assoc_Connected_Channel.present = 1; 
    memcpy (btamp_ASSOC.AMP_Assoc_Connected_Channel.country, "XXX", 3); 
    btamp_ASSOC.AMP_Assoc_Connected_Channel.num_triplets = 2; 
    btamp_ASSOC.AMP_Assoc_Connected_Channel.triplets[0][0] = 201; 
    btamp_ASSOC.AMP_Assoc_Connected_Channel.triplets[0][1] = 254; 
    btamp_ASSOC.AMP_Assoc_Connected_Channel.triplets[0][2] = 0; 
    //                                                               
    btamp_ASSOC.AMP_Assoc_Connected_Channel.triplets[1][0] = (0 != btampContext->channel)?btampContext->channel:0x01; 
    btamp_ASSOC.AMP_Assoc_Connected_Channel.triplets[1][1] = 0x01; 
    btamp_ASSOC.AMP_Assoc_Connected_Channel.triplets[1][2] = 0x11;


    /*                                   */ 
    btamp_ASSOC.AMP_Assoc_PAL_Capabilities.present = 1;

#if 0
AMP ASSOC Pal Capabilities: Size: 4 Octets

   Value             Description
     4               TypeID for 802.11 PAL Capabilities

     4               Length

   0xXXXXXXXX        Bit 0:
                         0 signifies the PAL is not capable of utilizing
                           received Activity Reports
                         1 signifies the PAL is capable of utilizing
                           received Activity Reports
                     Bit 1:
                         0 signifies the PAL is not capable of utilizing
                           scheduling information sent in an Activity Report
                         1 signifies the PAL is capable of utilizing
                           scheduling information sent in an Activity Report
                     Bits 2..31 Reserved

#endif // 

    btamp_ASSOC.AMP_Assoc_PAL_Capabilities.pal_capabilities 
//                                                                    
        //       
        = 0x00;

    /*                              */ 
    btamp_ASSOC.AMP_Assoc_PAL_Version.present = 1;

    /*                                        */ 
    btamp_ASSOC.AMP_Assoc_PAL_Version.pal_version = WLANBAP_PAL_VERSION;
    btamp_ASSOC.AMP_Assoc_PAL_Version.pal_CompanyID = WLANBAP_QUALCOMM_COMPANY_ID;  //                    
    btamp_ASSOC.AMP_Assoc_PAL_Version.pal_subversion = WLANBAP_PAL_SUBVERSION;

    //                            
    vosStatus = btampPackAMP_ASSOC(
            hHal, 
            &btamp_ASSOC, 
            pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_Read_Local_AMP_Assoc.AMP_assoc_fragment, 
            248, 
            &nConsumed);

    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "%s: nConsumed value: %d", __func__,  nConsumed); 

    /*                                                */ 
    pBapHCIEvent->bapHCIEventCode = BTAMP_TLV_HCI_COMMAND_COMPLETE_EVENT;
    pBapHCIEvent->u.btampCommandCompleteEvent.present = 1;
    pBapHCIEvent->u.btampCommandCompleteEvent.num_hci_command_packets = 1;
    pBapHCIEvent->u.btampCommandCompleteEvent.command_opcode 
        = BTAMP_TLV_HCI_READ_LOCAL_AMP_ASSOC_CMD;
    /*                            */
    if(pBapHCIReadLocalAMPAssoc->phy_link_handle != 
       btampContext->phy_link_handle) { 
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, 
                "%s: Wrong Physical Link handle in Read Local AMP Assoc cmd: current: %x, new: %x", __func__,  
                btampContext->phy_link_handle, 
                pBapHCIReadLocalAMPAssoc->phy_link_handle);

        pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_Read_Local_AMP_Assoc.status
            = WLANBAP_ERROR_NO_CNCT;
    } else
        pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_Read_Local_AMP_Assoc.status 
            = WLANBAP_STATUS_SUCCESS;
    pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_Read_Local_AMP_Assoc.phy_link_handle 
        = pBapHCIReadLocalAMPAssoc->phy_link_handle;
    /*                                                                           */ 
    pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_Read_Local_AMP_Assoc.remaining_length 
        = nConsumed;

    return VOS_STATUS_SUCCESS;
} /*                           */

/*                                                                            

                                           

              
                                                                     
                                                                        
                       

               
        

             

      
                                                                              
                                                                                       
    
          
                                                                      
                                                                       
                                                                 
   
              
                                                              

                                                                       
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPWriteRemoteAMPAssoc
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Write_Remote_AMP_ASSOC_Cmd   *pBapHCIWriteRemoteAMPAssoc,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */
)
{
    tWLAN_BAPEvent bapEvent; /*                     */
    VOS_STATUS  vosStatus;
    tBtampHCI_Event bapHCIEvent;

    /*                                                         */ 
    //                                                 
    ptBtampContext btampContext = (ptBtampContext) btampHandle; /*                    */ 
    v_U8_t status;    /*                               */

    /*                 */ 
    if (pBapHCIWriteRemoteAMPAssoc == NULL) {
      return VOS_STATUS_E_FAULT;
    }

    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "%s: btampHandle value: %p", __func__,  btampHandle);

    /*                             */ 
    bapEvent.event = eWLAN_BAP_HCI_WRITE_REMOTE_AMP_ASSOC;
    bapEvent.params = pBapHCIWriteRemoteAMPAssoc;

    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "%s: btampContext value: %p", __func__,  btampContext);

    /*              */ 
    vosStatus = btampFsm(btampContext, &bapEvent, &status);
  
    /*                                                */ 
    pBapHCIEvent->bapHCIEventCode = BTAMP_TLV_HCI_COMMAND_COMPLETE_EVENT;
    pBapHCIEvent->u.btampCommandCompleteEvent.present = 1;
    pBapHCIEvent->u.btampCommandCompleteEvent.num_hci_command_packets = 1;
    pBapHCIEvent->u.btampCommandCompleteEvent.command_opcode 
        = BTAMP_TLV_HCI_WRITE_REMOTE_AMP_ASSOC_CMD;
    pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Write_Remote_AMP_Assoc.status 
        = status;
    pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Write_Remote_AMP_Assoc.phy_link_handle 
        = pBapHCIWriteRemoteAMPAssoc->phy_link_handle;

    if(WLANBAP_ERROR_NO_SUITABLE_CHANNEL == status)
    {
        /*                                                      */ 
        bapHCIEvent.bapHCIEventCode = BTAMP_TLV_HCI_PHYSICAL_LINK_COMPLETE_EVENT;
        bapHCIEvent.u.btampPhysicalLinkCompleteEvent.present = 1;
        bapHCIEvent.u.btampPhysicalLinkCompleteEvent.status = status;
        bapHCIEvent.u.btampPhysicalLinkCompleteEvent.phy_link_handle 
            = btampContext->phy_link_handle;
        bapHCIEvent.u.btampPhysicalLinkCompleteEvent.ch_number 
            = 0;
    
        vosStatus = (*btampContext->pBapHCIEventCB) 
            (  
             btampContext->pHddHdl,   /*                                             */
             &bapHCIEvent, /*                                  */
             VOS_TRUE /*                                       */ 
            );
    }

    /*     */ 

    return VOS_STATUS_SUCCESS;
} /*                             */







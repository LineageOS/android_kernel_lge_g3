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

                      b a p A p i H C B B . C
                                               
  OVERVIEW:
  
  This software unit holds the implementation of the WLAN BAP modules
  Host Controller and Baseband functions.
  
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
#include "vos_trace.h"

//                                          
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


/*                                       */

/*                                                                            

                             

              
                                            
                                                                 
                                                             

               
        

             

      
                                                                              
   
              
                                                              

                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPReset
( 
  ptBtampHandle btampHandle
)
{
    VOS_STATUS  vosStatus;
    tBtampHCI_Event bapHCIEvent; /*                                  */
    ptBtampContext btampContext = (ptBtampContext) btampHandle;
    tHalHandle     hHal = NULL;
    /*                                                                       */

    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "%s: btampHandle value: %p", __func__,  btampHandle);

    /*                 */ 
    if (btampHandle == NULL) 
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                     "btampHandle is NULL in %s", __func__);

      return VOS_STATUS_E_FAULT;
    }

    /*                   */ 
    hHal = VOS_GET_HAL_CB(btampContext->pvosGCtx);
    if (NULL == hHal) 
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                     "hHal is NULL in %s", __func__);

        return VOS_STATUS_E_FAULT;
    }

    //                    
    /*                                            */
    if( TRUE == btampContext->isBapSessionOpen )
    {
        sme_RoamDisconnect(hHal,
                       btampContext->sessionId,
                       //                                           
                       //                                                          
                       eCSR_DISCONNECT_REASON_UNSPECIFIED);
    }

    /*                                 */
    /*                                 */
    btampContext->bapConnectionAcceptTimerInterval = WLANBAP_CONNECTION_ACCEPT_TIMEOUT;  
    /*                                */
    btampContext->bapLinkSupervisionTimerInterval = WLANBAP_LINK_SUPERVISION_TIMEOUT;  
    /*                                   */
    btampContext->bapLogicalLinkAcceptTimerInterval = WLANBAP_LOGICAL_LINK_ACCEPT_TIMEOUT;  
    /*                                 */
    btampContext->bapBEFlushTimerInterval = WLANBAP_BE_FLUSH_TIMEOUT;  


    /*                                                           */ 
    bapHCIEvent.bapHCIEventCode = BTAMP_TLV_HCI_COMMAND_COMPLETE_EVENT;
    bapHCIEvent.u.btampCommandCompleteEvent.present = 1;
    bapHCIEvent.u.btampCommandCompleteEvent.num_hci_command_packets = 1;
    bapHCIEvent.u.btampCommandCompleteEvent.command_opcode 
        = BTAMP_TLV_HCI_RESET_CMD;
    bapHCIEvent.u.btampCommandCompleteEvent.cc_event.Reset.status 
        = WLANBAP_STATUS_SUCCESS;

    vosStatus = (*btampContext->pBapHCIEventCB) 
        (  
         //                                                                            
         btampContext->pAppHdl,   /*                                             */
         &bapHCIEvent, /*                                  */
         VOS_FALSE /*                                       */ 
        );

    return vosStatus;
} /*               */

/*                                                                            

                                    

              
                                                                            
                                                                            

               
        

             

      
                                                                              
                                                                        
   
          
                                                                      
                                                                       
                                                                 
   
              
                                                              

                                                                
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPSetEventMask
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Set_Event_Mask_Cmd   *pBapHCISetEventMask,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */
)
{

    return VOS_STATUS_SUCCESS;
} /*                      */

/*                                                                            

                             

              
                                           
                                                                 
                                                                               
                   

               
        

             

      
                                                                              
                                                        
   
              
                                                              

                                                         
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPFlush
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Flush_Cmd     *pBapHCIFlush
)
{
    VOS_STATUS  vosStatus;
    tBtampHCI_Event bapHCIEvent; /*                                  */
    ptBtampContext btampContext = (ptBtampContext) btampHandle;
    /*                                                                       */

    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "%s: btampHandle value: %p", __func__,  btampHandle);

    /*                 */ 
    if (btampHandle == NULL) {
      return VOS_STATUS_E_FAULT;
    }

    /*                                                           */ 
    bapHCIEvent.bapHCIEventCode = BTAMP_TLV_HCI_COMMAND_COMPLETE_EVENT;
    bapHCIEvent.u.btampCommandCompleteEvent.present = 1;
    bapHCIEvent.u.btampCommandCompleteEvent.num_hci_command_packets = 1;
    bapHCIEvent.u.btampCommandCompleteEvent.command_opcode 
        = BTAMP_TLV_HCI_FLUSH_CMD;
    bapHCIEvent.u.btampCommandCompleteEvent.cc_event.Flush.status
        = WLANBAP_STATUS_SUCCESS;

    vosStatus = (*btampContext->pBapHCIEventCB) 
        (  
         //                                                                            
         btampContext->pAppHdl,   /*                                             */
         &bapHCIEvent, /*                                  */
         VOS_FALSE /*                                       */ 
        );

    return vosStatus;
} /*               */

/*                                                                            

                                     

              
                                                    
                                                                                
                                                                       

               
        

             

      
                                                                              
                                                                 
          
                                                                      
                                                                       
                                                                 
   
              
                                                              

                                                         
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_EnhancedBAPFlush
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Enhanced_Flush_Cmd     *pBapHCIFlush,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */

)
{
    VOS_STATUS  vosStatus = VOS_STATUS_SUCCESS;
    tBtampHCI_Event bapHCIEvent; /*                                  */
    ptBtampContext btampContext;
    /*                                                                       */

    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "%s: btampHandle value: %p", __func__,  btampHandle);

    /*                 */ 
    /*                 */ 
    if ((NULL == btampHandle) || (NULL == pBapHCIEvent))
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH,
                   "Invalid input parameters in %s", __func__);
        return VOS_STATUS_E_FAULT;
    }

    btampContext = (ptBtampContext) btampHandle;
    /*                                             */
    bapHCIEvent.bapHCIEventCode = BTAMP_TLV_HCI_COMMAND_STATUS_EVENT;
    bapHCIEvent.u.btampCommandStatusEvent.present = 1;
    bapHCIEvent.u.btampCommandStatusEvent.num_hci_command_packets = 1;
    bapHCIEvent.u.btampCommandStatusEvent.command_opcode
        = BTAMP_TLV_HCI_ENHANCED_FLUSH_CMD;
    bapHCIEvent.u.btampCommandStatusEvent.status = WLANBAP_STATUS_SUCCESS;

    /*                                                           */ 
    pBapHCIEvent->bapHCIEventCode = BTAMP_TLV_HCI_ENHANCED_FLUSH_COMPLETE_EVENT;
    pBapHCIEvent->u.btampEnhancedFlushCompleteEvent.present = 1;
    pBapHCIEvent->u.btampEnhancedFlushCompleteEvent.log_link_handle = 
        pBapHCIFlush->log_link_handle;

    vosStatus = (*btampContext->pBapHCIEventCB) 
        (  
         //                                                                            
         btampContext->pAppHdl,   /*                                             */
         &bapHCIEvent, /*                                  */
         VOS_FALSE /*                                       */ 
        );

    return vosStatus;
} /*                       */

/*                                                                            

                                                   

              
                                                                             
                                                                        
                       

               
        

             

      
                                                                              
   
          
                                                                      
                                                                       
                                                                 
   
              
                                                              

                                                                               
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPReadConnectionAcceptTimeout
( 
  ptBtampHandle btampHandle,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                   */
)
{
    ptBtampContext btampContext = (ptBtampContext) btampHandle;
    /*                                                                       */

    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH,
               "%s: btampHandle value: %p", __func__,  btampHandle);

    /*                 */ 
    if ((NULL == btampHandle) || (NULL == pBapHCIEvent))
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH,
                   "Invalid input parameters in %s", __func__);
        return VOS_STATUS_E_FAULT;
    }

    /*                                                      */ 
    pBapHCIEvent->bapHCIEventCode = BTAMP_TLV_HCI_COMMAND_COMPLETE_EVENT;
    pBapHCIEvent->u.btampCommandCompleteEvent.present = TRUE;
    pBapHCIEvent->u.btampCommandCompleteEvent.num_hci_command_packets = 1;
    pBapHCIEvent->u.btampCommandCompleteEvent.command_opcode 
        = BTAMP_TLV_HCI_READ_CONNECTION_ACCEPT_TIMEOUT_CMD;
    pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_Connection_Accept_TO.status
        = WLANBAP_STATUS_SUCCESS;
    pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_Connection_Accept_TO.connection_accept_timeout
        = btampContext->bapConnectionAcceptTimerInterval;

    return VOS_STATUS_SUCCESS;
} /*                                     */

/*                                                                            

                                                    

              
                                                                              
                                                                        
                       

               
        

             

      
                                                                              
                                                                                                   
   
          
                                                                      
                                                                       
                                                                 
   
              
                                                              

                                                                                
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPWriteConnectionAcceptTimeout
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Write_Connection_Accept_Timeout_Cmd   *pBapHCIWriteConnectionAcceptTimeout,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */
)
{
    ptBtampContext btampContext = (ptBtampContext) btampHandle;
    /*                                                                       */

    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH,
               "%s: btampHandle value: %p", __func__,  btampHandle);

    /*                 */ 
    if ((NULL == btampHandle) || (NULL == pBapHCIWriteConnectionAcceptTimeout)
        || (NULL == pBapHCIEvent))
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH,
                   "Invalid input parameters in %s", __func__);
        return VOS_STATUS_E_FAULT;
    }

    /*                                             */
    if ((pBapHCIWriteConnectionAcceptTimeout->connection_accept_timeout >
         WLANBAP_CON_ACCEPT_TIMEOUT_MAX_RANGE) || 
        (pBapHCIWriteConnectionAcceptTimeout->connection_accept_timeout <
         WLANBAP_CON_ACCEPT_TIMEOUT_MIN_RANGE))
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH,
                   "Out of range for connection accept timeout parameters in %s",
                   __func__);
        pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Write_Connection_Accept_TO.status
            = WLANBAP_ERROR_INVALID_HCI_CMND_PARAM;
    }
    else
    {
        /*                                                        */
        btampContext->bapConnectionAcceptTimerInterval = 
            pBapHCIWriteConnectionAcceptTimeout->connection_accept_timeout;

        /*                                          */
        pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Write_Connection_Accept_TO.status
            = WLANBAP_STATUS_SUCCESS;
    }

    /*                                                      */ 
    pBapHCIEvent->bapHCIEventCode = BTAMP_TLV_HCI_COMMAND_COMPLETE_EVENT;
    pBapHCIEvent->u.btampCommandCompleteEvent.present = TRUE;
    pBapHCIEvent->u.btampCommandCompleteEvent.num_hci_command_packets = 1;
    pBapHCIEvent->u.btampCommandCompleteEvent.command_opcode 
        = BTAMP_TLV_HCI_WRITE_CONNECTION_ACCEPT_TIMEOUT_CMD;

    return VOS_STATUS_SUCCESS;
} /*                                      */


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
)
{
    ptBtampContext btampContext = (ptBtampContext) btampHandle;
    v_U8_t         phyLinkHandle;
    /*                                                                       */

    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH,
               "%s: btampHandle value: %p", __func__,  btampHandle);

    /*                 */ 
    if ((NULL == btampHandle) || (NULL == pBapHCIReadLinkSupervisionTimeout) ||
        (NULL == pBapHCIEvent))
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH,
                   "Invalid input parameters in %s", __func__);
        return VOS_STATUS_E_FAULT;
    }

    /*                                                 
                                              */
    phyLinkHandle = (v_U8_t) pBapHCIReadLinkSupervisionTimeout->log_link_handle;

    if (phyLinkHandle != btampContext->phy_link_handle)
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH,
                   "Invalid Physical link handle in %s", __func__);
        pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_Link_Supervision_TO.link_supervision_timeout
            = 0x00; /*               */
        pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_Link_Supervision_TO.log_link_handle
            = pBapHCIReadLinkSupervisionTimeout->log_link_handle;
        pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_Link_Supervision_TO.status
            = WLANBAP_ERROR_INVALID_HCI_CMND_PARAM;
    }
    else
    {
        pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_Link_Supervision_TO.link_supervision_timeout
            = btampContext->bapLinkSupervisionTimerInterval;
        pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_Link_Supervision_TO.log_link_handle
            = pBapHCIReadLinkSupervisionTimeout->log_link_handle;
        pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_Link_Supervision_TO.status
            = WLANBAP_STATUS_SUCCESS;
    }

    /*                                                      */ 
    pBapHCIEvent->bapHCIEventCode = BTAMP_TLV_HCI_COMMAND_COMPLETE_EVENT;
    pBapHCIEvent->u.btampCommandCompleteEvent.present = TRUE;
    pBapHCIEvent->u.btampCommandCompleteEvent.num_hci_command_packets = 1;
    pBapHCIEvent->u.btampCommandCompleteEvent.command_opcode 
        = BTAMP_TLV_HCI_READ_LINK_SUPERVISION_TIMEOUT_CMD;

    return VOS_STATUS_SUCCESS;
} /*                                    */

/*                                                                            

                                                   

              
                                                                             
                                                                        
                       

               
        

             

      
                                                                              
                                                                                                 
   
          
                                                                      
                                                                       
                                                                 
   
              
                                                              

                                                                               
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPWriteLinkSupervisionTimeout
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Write_Link_Supervision_Timeout_Cmd   *pBapHCIWriteLinkSupervisionTimeout,
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
    if ((NULL == btampHandle) || (NULL == pBapHCIWriteLinkSupervisionTimeout) ||
        (NULL == pBapHCIEvent))
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH,
                   "Invalid input parameters in %s", __func__);
        return VOS_STATUS_E_FAULT;
    }

    /*                                                 
                                              */
    phyLinkHandle = (v_U8_t) pBapHCIWriteLinkSupervisionTimeout->log_link_handle;

    if (phyLinkHandle != btampContext->phy_link_handle)
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH,
                   "Invalid Physical link handle in %s", __func__);
        pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Write_Link_Supervision_TO.log_link_handle
            = pBapHCIWriteLinkSupervisionTimeout->log_link_handle;
        pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Write_Link_Supervision_TO.status
            = WLANBAP_ERROR_INVALID_HCI_CMND_PARAM;
    }
    else
    {
        /*                              */
        btampContext->bapLinkSupervisionTimerInterval =
            pBapHCIWriteLinkSupervisionTimeout->link_supervision_timeout;

        pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Write_Link_Supervision_TO.log_link_handle
            = pBapHCIWriteLinkSupervisionTimeout->log_link_handle;
        pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Write_Link_Supervision_TO.status
            = WLANBAP_STATUS_SUCCESS;
    }

    /*                                                      */ 
    pBapHCIEvent->bapHCIEventCode = BTAMP_TLV_HCI_COMMAND_COMPLETE_EVENT;
    pBapHCIEvent->u.btampCommandCompleteEvent.present = TRUE;
    pBapHCIEvent->u.btampCommandCompleteEvent.num_hci_command_packets = 1;
    pBapHCIEvent->u.btampCommandCompleteEvent.command_opcode 
        = BTAMP_TLV_HCI_WRITE_LINK_SUPERVISION_TIMEOUT_CMD;

    return VOS_STATUS_SUCCESS;
} /*                                     */

/*                                            */


/*                                                                            

                                                    

              
                                                                               
                                                                        
                       

               
        

             

      
                                                                              
   
          
                                                                      
                                                                       
                                                                 
   
              
                                                              

                                                                                
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPReadLogicalLinkAcceptTimeout
( 
  ptBtampHandle btampHandle,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                  */
)
{
    ptBtampContext btampContext = (ptBtampContext) btampHandle;
    /*                                                                       */

    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH,
               "%s: btampHandle value: %p", __func__,  btampHandle);

    /*                 */ 
    if ((NULL == btampHandle) || (NULL == pBapHCIEvent))
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH,
                   "Invalid input parameters in %s", __func__);
        return VOS_STATUS_E_FAULT;
    }

    /*                                                      */ 
    pBapHCIEvent->bapHCIEventCode = BTAMP_TLV_HCI_COMMAND_COMPLETE_EVENT;
    pBapHCIEvent->u.btampCommandCompleteEvent.present = TRUE;
    pBapHCIEvent->u.btampCommandCompleteEvent.num_hci_command_packets = 1;
    pBapHCIEvent->u.btampCommandCompleteEvent.command_opcode 
        = BTAMP_TLV_HCI_READ_LOGICAL_LINK_ACCEPT_TIMEOUT_CMD;
    pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_Logical_Link_Accept_TO.status
        = WLANBAP_STATUS_SUCCESS;
    pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_Logical_Link_Accept_TO.logical_link_accept_timeout
        = btampContext->bapLogicalLinkAcceptTimerInterval;

    return VOS_STATUS_SUCCESS;
} /*                                      */

/*                                                                            

                                                     

              
                                                                                
                                                                        
                       

               
        

             

      
                                                                              
                                                                                                      
   
          
                                                                      
                                                                       
                                                                 
   
              
                                                              

                                                                                 
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPWriteLogicalLinkAcceptTimeout
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Write_Logical_Link_Accept_Timeout_Cmd   *pBapHCIWriteLogicalLinkAcceptTimeout,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */
)
{
    ptBtampContext btampContext = (ptBtampContext) btampHandle;
    /*                                                                       */

    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH,
               "%s: btampHandle value: %p", __func__,  btampHandle);

    /*                 */ 
    if ((NULL == btampHandle) || (NULL == pBapHCIWriteLogicalLinkAcceptTimeout)
        || (NULL == pBapHCIEvent))
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH,
                   "Invalid input parameters in %s", __func__);
        return VOS_STATUS_E_FAULT;
    }

    /*                                             */
    if ((pBapHCIWriteLogicalLinkAcceptTimeout->logical_link_accept_timeout >
         WLANBAP_CON_ACCEPT_TIMEOUT_MAX_RANGE) || 
        (pBapHCIWriteLogicalLinkAcceptTimeout->logical_link_accept_timeout <
         WLANBAP_CON_ACCEPT_TIMEOUT_MIN_RANGE))
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH,
                   "Out of range for logical connection accept timeout parameters in %s",
                   __func__);
        pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Write_Logical_Link_Accept_TO.status
            = WLANBAP_ERROR_INVALID_HCI_CMND_PARAM;
    }
    else
    {
        /*                                                        */
        btampContext->bapLogicalLinkAcceptTimerInterval = 
            pBapHCIWriteLogicalLinkAcceptTimeout->logical_link_accept_timeout;

        /*                                          */
        pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Write_Logical_Link_Accept_TO.status
            = WLANBAP_STATUS_SUCCESS;
    }

    /*                                                      */ 
    pBapHCIEvent->bapHCIEventCode = BTAMP_TLV_HCI_COMMAND_COMPLETE_EVENT;
    pBapHCIEvent->u.btampCommandCompleteEvent.present = TRUE;
    pBapHCIEvent->u.btampCommandCompleteEvent.num_hci_command_packets = 1;
    pBapHCIEvent->u.btampCommandCompleteEvent.command_opcode 
        = BTAMP_TLV_HCI_WRITE_LOGICAL_LINK_ACCEPT_TIMEOUT_CMD;

    return VOS_STATUS_SUCCESS;
} /*                                       */

/*                                                                            

                                         

              
                                                                                   
                                                                            

               
        

             

      
                                                                              
                                                                                    
   
          
                                                                      
                                                                       
                                                                 
   
              
                                                              

                                                                     
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPSetEventMaskPage2
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Set_Event_Mask_Page_2_Cmd   *pBapHCISetEventMaskPage2,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */
)
{
    ptBtampContext btampContext = (ptBtampContext) btampHandle;
    /*                                                                       */

    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH,
               "%s: btampHandle value: %p", __func__,  btampHandle);

    /*                 */ 
    if ((NULL == btampHandle) || (NULL == pBapHCISetEventMaskPage2)
        || (NULL == pBapHCIEvent))
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH,
                   "Invalid input parameters in %s", __func__);
        return VOS_STATUS_E_FAULT;
    }


    /*                          */
    vos_mem_copy(  
            btampContext->event_mask_page_2, 
            pBapHCISetEventMaskPage2->event_mask_page_2, 
            8 );

    /*                                          */
    pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Set_Event_Mask_Page_2.status
        = WLANBAP_STATUS_SUCCESS;

    /*                                                      */ 
    pBapHCIEvent->bapHCIEventCode = BTAMP_TLV_HCI_COMMAND_COMPLETE_EVENT;
    pBapHCIEvent->u.btampCommandCompleteEvent.present = TRUE;
    pBapHCIEvent->u.btampCommandCompleteEvent.num_hci_command_packets = 1;
    pBapHCIEvent->u.btampCommandCompleteEvent.command_opcode 
        = BTAMP_TLV_HCI_SET_EVENT_MASK_PAGE_2_CMD;

    return VOS_STATUS_SUCCESS;
} /*                           */

/*                                                                            

                                        

              
                                                                 
                                                                        
                       

               
        

             

      
                                                                              
   
          
                                                                      
                                                                       
                                                                 
   
              
                                                              

                                                                    
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPReadLocationData
( 
  ptBtampHandle btampHandle,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                  */
)
{
    ptBtampContext btampContext;
    /*                                                                       */

    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH,
               "%s: btampHandle value: %p", __func__,  btampHandle);

    /*                 */ 
    if ((NULL == btampHandle) || (NULL == pBapHCIEvent))
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH,
                   "Invalid input parameters in %s", __func__);
        return VOS_STATUS_E_FAULT;
    }

    btampContext = (ptBtampContext) btampHandle;

    pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_Location_Data.loc_domain_aware
        = btampContext->btamp_Location_Data_Info.loc_domain_aware;

    pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_Location_Data.loc_options
        = btampContext->btamp_Location_Data_Info.loc_options;

    vos_mem_copy(  
            pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_Location_Data.loc_domain,
            btampContext->btamp_Location_Data_Info.loc_domain, 
            3 );

    /*                                          */
    pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_Location_Data.status
        = WLANBAP_STATUS_SUCCESS;

    /*                                                      */ 
    pBapHCIEvent->bapHCIEventCode = BTAMP_TLV_HCI_COMMAND_COMPLETE_EVENT;
    pBapHCIEvent->u.btampCommandCompleteEvent.present = TRUE;
    pBapHCIEvent->u.btampCommandCompleteEvent.num_hci_command_packets = 1;
    pBapHCIEvent->u.btampCommandCompleteEvent.command_opcode 
        = BTAMP_TLV_HCI_READ_LOCATION_DATA_CMD;

    return VOS_STATUS_SUCCESS;
} /*                          */

/*                                                                            

                                         

              
                                                                  
                                                                        
                       

               
        

             

      
                                                                              
                                                                                  
   
          
                                                                      
                                                                       
                                                                 
   
              
                                                              

                                                                     
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPWriteLocationData
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Write_Location_Data_Cmd   *pBapHCIWriteLocationData,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */
)
{
    ptBtampContext btampContext;
    /*                                                                       */

    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH,
               "%s: btampHandle value: %p", __func__,  btampHandle);

    /*                 */ 
    if ((NULL == btampHandle) || (NULL == pBapHCIWriteLocationData)
        || (NULL == pBapHCIEvent))
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH,
                   "Invalid input parameters in %s", __func__);
        return VOS_STATUS_E_FAULT;
    }

    btampContext = (ptBtampContext) btampHandle;

    btampContext->btamp_Location_Data_Info.loc_domain_aware = 
        pBapHCIWriteLocationData->loc_domain_aware;
    
    btampContext->btamp_Location_Data_Info.loc_options = 
        pBapHCIWriteLocationData->loc_options;

    vos_mem_copy(  
            btampContext->btamp_Location_Data_Info.loc_domain, 
            pBapHCIWriteLocationData->loc_domain, 
            3 );

    /*                                          */
    pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Write_Location_Data.status
        = WLANBAP_STATUS_SUCCESS;

    /*                                                      */ 
    pBapHCIEvent->bapHCIEventCode = BTAMP_TLV_HCI_COMMAND_COMPLETE_EVENT;
    pBapHCIEvent->u.btampCommandCompleteEvent.present = TRUE;
    pBapHCIEvent->u.btampCommandCompleteEvent.num_hci_command_packets = 1;
    pBapHCIEvent->u.btampCommandCompleteEvent.command_opcode 
        = BTAMP_TLV_HCI_WRITE_LOCATION_DATA_CMD;

    return VOS_STATUS_SUCCESS;
} /*                           */


/*                                                                            

                                           

              
                                                                     
                                                                        
                       

               
        

             

      
                                                                              
   
          
                                                                      
                                                                       
                                                                 
   
              
                                                              

                                                                       
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPReadFlowControlMode
( 
  ptBtampHandle btampHandle,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                  */
)
{
    /*                                                                       */

    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH,
               "%s: btampHandle value: %p", __func__,  btampHandle);

    /*                 */ 
    if ((NULL == btampHandle) || (NULL == pBapHCIEvent))
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH,
                   "Invalid input parameters in %s", __func__);
        return VOS_STATUS_E_FAULT;
    }

    /*                                                      */ 
    pBapHCIEvent->bapHCIEventCode = BTAMP_TLV_HCI_COMMAND_COMPLETE_EVENT;
    pBapHCIEvent->u.btampCommandCompleteEvent.present = TRUE;
    pBapHCIEvent->u.btampCommandCompleteEvent.num_hci_command_packets = 1;
    pBapHCIEvent->u.btampCommandCompleteEvent.command_opcode 
        = BTAMP_TLV_HCI_READ_FLOW_CONTROL_MODE_CMD;
    pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_Flow_Control_Mode.status
        = WLANBAP_STATUS_SUCCESS;
    pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Read_Flow_Control_Mode.flow_control_mode
        = WLANBAP_FLOW_CONTROL_MODE_BLOCK_BASED;

    return VOS_STATUS_SUCCESS;
} /*                             */

/*                                                                            

                                            

              
                                                                      
                                                                        
                       

               
        

             

      
                                                                              
                                                                                         
   
          
                                                                      
                                                                       
                                                                 
   
              
                                                              

                                                                        
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPWriteFlowControlMode
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Write_Flow_Control_Mode_Cmd   *pBapHCIWriteFlowControlMode,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */
)
{

    return VOS_STATUS_SUCCESS;
} /*                              */

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
)
{

    return VOS_STATUS_SUCCESS;
} /*                                    */

/*                                                                            

                                                   

              
                                                                         
                                                                        
                       

               
        

             

      
                                                                              
                                                                             
   
          
                                                                      
                                                                       
                                                                 
   
              
                                                              

                                                                  
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPWriteBestEffortFlushTimeout
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Write_Best_Effort_Flush_Timeout_Cmd   *pBapHCIWriteBEFlushTO,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */
)
{

    return VOS_STATUS_SUCCESS;
} /*                                     */


/*                                                                            

                                         

              
                                                                                  
                                                                            

               
        

             

      
                                                                              
                                                                                
   
          
                                                                      
                                                                       
                                                                 
   
              
                                                              

                                                                  
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPSetShortRangeMode
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Set_Short_Range_Mode_Cmd   *pBapHCIShortRangeMode,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */
)
{
    ptBtampContext btampContext = (ptBtampContext) btampHandle;
    BTAMPFSM_INSTANCEDATA_T *instanceVar = &(btampContext->bapPhysLinkMachine);
    /*                                                                       */

    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH,
               "%s: btampHandle value: %p", __func__,  btampHandle);

    /*                 */
    if ((NULL == btampHandle) || (NULL == pBapHCIEvent))
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH,
                   "Invalid input parameters in %s", __func__);
        return VOS_STATUS_E_FAULT;
    }

    /*                                                                   
                                                            */

    /*                                             */
    pBapHCIEvent->bapHCIEventCode = BTAMP_TLV_HCI_COMMAND_STATUS_EVENT;
    pBapHCIEvent->u.btampCommandStatusEvent.present = 1;
    pBapHCIEvent->u.btampCommandStatusEvent.num_hci_command_packets = 1;
    pBapHCIEvent->u.btampCommandStatusEvent.command_opcode
        = BTAMP_TLV_HCI_SET_SHORT_RANGE_MODE_CMD;

    if (CONNECTED != instanceVar->stateVar)
    {
        /*                                           */
        pBapHCIEvent->u.btampCommandStatusEvent.status =
            WLANBAP_ERROR_CMND_DISALLOWED;
        return VOS_STATUS_SUCCESS;
    }
    else if (pBapHCIShortRangeMode->phy_link_handle != btampContext->phy_link_handle)
    {
       /*                              */
        pBapHCIEvent->u.btampCommandStatusEvent.status =
            WLANBAP_ERROR_NO_CNCT;
        return VOS_STATUS_SUCCESS;
    }
    else if (pBapHCIShortRangeMode->short_range_mode > 0x01)
    {
        /*                        */
        pBapHCIEvent->u.btampCommandStatusEvent.status =
            WLANBAP_ERROR_INVALID_HCI_CMND_PARAM;
        return VOS_STATUS_SUCCESS;
    }

    pBapHCIEvent->u.btampCommandStatusEvent.status = WLANBAP_STATUS_SUCCESS;

    /*                                                                             */
    (*btampContext->pBapHCIEventCB)
        (
         btampContext->pHddHdl,   /*                                               */
         pBapHCIEvent, /*                                  */
         VOS_FALSE /*                                       */
        );

    /*                                                         */ 
    pBapHCIEvent->bapHCIEventCode = BTAMP_TLV_HCI_SHORT_RANGE_MODE_CHANGE_COMPLETE_EVENT;
    pBapHCIEvent->u.btampShortRangeModeChangeCompleteEvent.present = 1;

    pBapHCIEvent->u.btampShortRangeModeChangeCompleteEvent.status =
        WLANBAP_STATUS_SUCCESS; /*                    */

    /*                                                  */
    pBapHCIEvent->u.btampShortRangeModeChangeCompleteEvent.phy_link_handle =
        pBapHCIShortRangeMode->phy_link_handle;

    pBapHCIEvent->u.btampShortRangeModeChangeCompleteEvent.short_range_mode =
        pBapHCIShortRangeMode->short_range_mode; /*                    */

    /*                                                                   */
    if (pBapHCIShortRangeMode->short_range_mode != btampContext->phy_link_srm)
    {
        /*                                                          
                                                                            
         */
        #if 0
        //                                   
        if (VOS_STATUS_SUCCESS != HALSetShortRangeMode(pBapHCIShortRangeMode->short_range_mode))
        #else
        if (0)
        #endif
        {
            pBapHCIEvent->u.btampShortRangeModeChangeCompleteEvent.status =
                WLANBAP_ERROR_HARDWARE_FAILURE;
            pBapHCIEvent->u.btampShortRangeModeChangeCompleteEvent.short_range_mode =
                btampContext->phy_link_srm; /*                              */
        }
        else
        {
            /*                                                                */
            btampContext->phy_link_srm = pBapHCIShortRangeMode->short_range_mode;
        }
    }

    return VOS_STATUS_SUCCESS;
} /*                           */

/*                                                                            

                                          

             
                                                                               
                                                                                 
                   

                                 
                                                                                        

              
       

            

      
                                                                              

          
                                                                     
                                                                      
                                                                 

              
                                                            

                                                        
                                

              

                                                                            */
VOS_STATUS
WLAN_BAPVendorSpecificCmd0
(
  ptBtampHandle btampHandle,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */
)
{
    ptBtampContext btampContext = (ptBtampContext) btampHandle;
    BTAMPFSM_INSTANCEDATA_T *instanceVar = &(btampContext->bapPhysLinkMachine);
    /*                                                                       */

    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH,
               "%s: btampHandle value: %p", __func__,  btampHandle);

    /*                 */
    if ((NULL == btampHandle) || (NULL == pBapHCIEvent))
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH,
                   "Invalid input parameters in %s", __func__);
        return VOS_STATUS_E_FAULT;
    }

    /*                                                            
                                                                         */

    /*                                             */
    pBapHCIEvent->bapHCIEventCode = BTAMP_TLV_HCI_COMMAND_STATUS_EVENT;
    pBapHCIEvent->u.btampCommandStatusEvent.present = 1;
    pBapHCIEvent->u.btampCommandStatusEvent.num_hci_command_packets = 1;
    pBapHCIEvent->u.btampCommandStatusEvent.command_opcode
        = BTAMP_TLV_HCI_VENDOR_SPECIFIC_CMD_0;

    if ( (BT_INITIATOR != btampContext->BAPDeviceRole) ||
         (CONNECTING != instanceVar->stateVar) )
    {
        /*                                                     */
        pBapHCIEvent->u.btampCommandStatusEvent.status =
            WLANBAP_ERROR_CMND_DISALLOWED;
        return VOS_STATUS_SUCCESS;
    }

    /*                                                       */
    WLANBAP_NeedBTCoexPriority(btampContext, 1);

    pBapHCIEvent->u.btampCommandStatusEvent.status = WLANBAP_STATUS_SUCCESS;

    /*                                                                              */
    (*btampContext->pBapHCIEventCB)
        (
         btampContext->pHddHdl,   /*                                               */
         pBapHCIEvent, /*                                  */
         VOS_FALSE /*                                       */
        );

    /*                                                                  */
    pBapHCIEvent->bapHCIEventCode = BTAMP_TLV_HCI_COMMAND_COMPLETE_EVENT;
    pBapHCIEvent->u.btampCommandCompleteEvent.present = 1;
    pBapHCIEvent->u.btampCommandCompleteEvent.num_hci_command_packets = 1;
    pBapHCIEvent->u.btampCommandCompleteEvent.command_opcode
        = BTAMP_TLV_HCI_VENDOR_SPECIFIC_CMD_0;
    pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Vendor_Specific_Cmd_0.status
        = WLANBAP_STATUS_SUCCESS;

    return VOS_STATUS_SUCCESS;
} /*                            */

/*                                                                            

                                          

             
                                                                               
                                                                                 
                   

                                 
                                     

              
       

            

      
                                                                              

          
                                                                     
                                                                      
                                                                 

              
                                                            

                                                        
                                

              

                                                                            */
VOS_STATUS
WLAN_BAPVendorSpecificCmd1
(
  ptBtampHandle btampHandle,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */
)
{
    ptBtampContext btampContext = (ptBtampContext) btampHandle;
    /*                                                                       */

    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH,
               "%s: btampHandle value: %p", __func__,  btampHandle);

    /*                 */
    if ((NULL == btampHandle) || (NULL == pBapHCIEvent))
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH,
                   "Invalid input parameters in %s", __func__);
        return VOS_STATUS_E_FAULT;
    }


    btampContext->btamp_async_logical_link_create = TRUE;


    /*                                                                  */
    pBapHCIEvent->bapHCIEventCode = BTAMP_TLV_HCI_COMMAND_COMPLETE_EVENT;
    pBapHCIEvent->u.btampCommandCompleteEvent.present = 1;
    pBapHCIEvent->u.btampCommandCompleteEvent.num_hci_command_packets = 1;
    pBapHCIEvent->u.btampCommandCompleteEvent.command_opcode
        = BTAMP_TLV_HCI_VENDOR_SPECIFIC_CMD_1;
    pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Vendor_Specific_Cmd_1.status
        = WLANBAP_STATUS_SUCCESS;

    return VOS_STATUS_SUCCESS;
} /*                            */

/*                                                                            

                
                                                                      
                                                                             
    
                                                                          
                                                             

             

      
                                                                        
                                                                             
                                                              
                                            
                                                 
  
               
                                                            

                                                                            */
VOS_STATUS WLANBAP_TLFlushCompCallback
( 
  v_PVOID_t     pvosGCtx,
  v_U8_t        ucStaId, 
  v_U8_t        ucTID, 
  v_U8_t        status
)
{

    return VOS_STATUS_SUCCESS;
} //                            


/*                                                   */


 



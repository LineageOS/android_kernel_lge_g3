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

/*                                                                           

                                           


                                                                       
                                                                


                                                                                                                                         


                                      
                                                                             
                                    

                                                                           */

/*                                                                            
                
                                                                            */
//                                       
#include "sirParams.h"
//                     
 
#include "wlan_qct_tl.h"
#include "vos_trace.h"
//                                          
#include "sme_Api.h"
#include "ccmApi.h"

/*                            */ 
#include "bapApi.h" 
#include "bapInternal.h" 

//                                  
#include "bapRsnTxRx.h"
//                    
#include "bapApiTimer.h"

#if defined(ANI_OS_TYPE_ANDROID)
#include "bap_hdd_main.h"
#endif

//                 
/*                                                                            
                                         
                                                                            */
//                                                                     


/*                                                                            
                    
                                                                            */

/*                                                                            
                          
                                                                            */
//                                                   
static tWLAN_BAPbapPhysLinkMachine bapPhysLinkMachineInitial 
    = BTAMPFSM_INSTANCEDATA_INIT;

/*                                                                            
                                             
                                                                            */
//                         
//                                                            
//                       
ptBtampContext  gpBtampCtx; 

//                                       
tBtampHCI_AMP_Info        btampHCI_AMP_Info;
//                                                   
tBtampHCI_Data_Block_Size btampHCI_Data_Block_Size;
//                                           
tBtampHCI_Version_Info   btampHCI_Version_Info;
//                                                  
tBtampHCI_Supported_Cmds  btampHCI_Supported_Cmds;

static unsigned char pBtStaOwnMacAddr[WNI_CFG_BSSID_LEN];

 /*                                                                       */
#define WLAN_BAP_SSID_MAX_LEN 21 
static char pBtStaOwnSsid[WLAN_BAP_SSID_MAX_LEN];

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
WLANBAP_Open
( 
  v_PVOID_t  pvosGCtx 
)
{
  ptBtampContext  pBtampCtx = NULL; 
  /*                                                                       */

  /*                                                                        
                                                    
                                                                           */
  vos_alloc_context(pvosGCtx, VOS_MODULE_ID_BAP, (v_VOID_t**)&pBtampCtx, sizeof(tBtampContext));

  pBtampCtx = VOS_GET_BAP_CB(pvosGCtx);
  if ( NULL == pBtampCtx ) 
  {
    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                 "Invalid BAP pointer from pvosGCtx on WLANBAP_Open");
                 //                                                                
    return VOS_STATUS_E_FAULT;
  }

  /*                                                                        
                                                     
                                                                           */
  VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "WLANBAP_Open");

  WLANBAP_CleanCB(pBtampCtx, 0 /*            */);

  //                                          
  pBtampCtx->pvosGCtx = pvosGCtx;
   
  //                                                    
  gpBtampCtx = pBtampCtx;
  
  /*                                                                        
                               
                                                                           */

  return VOS_STATUS_SUCCESS;
}/*              */


/*                                                                          

                           

              
                                                                        
                                                            
                       
    
               
    
             

      
                                                                         
                                                                    
   
              
                                                              

                                                                               
                                
                                               

                                                             
    
               
  
                                                                            */
VOS_STATUS 
WLANBAP_Start
( 
  v_PVOID_t  pvosGCtx 
)
{
  ptBtampContext  pBtampCtx = NULL; 
  VOS_STATUS      vosStatus;
  /*                                                                       */

  /*                                                                        
                
                              
                                                                           */
  pBtampCtx = VOS_GET_BAP_CB(pvosGCtx);
  if ( NULL == pBtampCtx ) 
  {
    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                "Invalid BAP pointer from pvosGCtx on WLANBAP_Start");
    return VOS_STATUS_E_FAULT;
  }

  /*                                                                        
                                              
                                                                          */
  VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "WLANBAP_Start TL register");

  /*                                                                        
                                                           
                                                                          */
  VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "WLANBAP_Start CSR Register");


  /*                                            */
  WLANBAP_InitConnectionAcceptTimer (pBtampCtx );
  WLANBAP_InitLinkSupervisionTimer(pBtampCtx);

  vosStatus = vos_timer_init( 
          &pBtampCtx->bapTxPktMonitorTimer,
          VOS_TIMER_TYPE_SW, /*               */
          WLANBAP_TxPacketMonitorHandler,
          pBtampCtx);

  vosStatus = vos_lock_init(&pBtampCtx->bapLock);
  if(!VOS_IS_STATUS_SUCCESS(vosStatus))
  {
     VOS_TRACE(VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,"Lock Init Fail");
  }

  return vosStatus;
}/*               */

/*                                                                          

                          

              
                                                                                    
                                                                         
                                       
    
               
    
             

      
                                                                         
                                                                    
   
              
                                                              

                                                                               
                                
                                               

               
  
                                                                            */
VOS_STATUS 
WLANBAP_Stop
( 
  v_PVOID_t  pvosGCtx 
)
{
  ptBtampContext  pBtampCtx = NULL; 
  VOS_STATUS  vosStatus = VOS_STATUS_SUCCESS;
  /*                                                                       */

  /*                                                                        
                
                              
                                                                           */
  pBtampCtx = VOS_GET_BAP_CB(pvosGCtx);
  if ( NULL == pBtampCtx ) 
  {
    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                 "Invalid BAP pointer from pvosGCtx on WLANBAP_Stop");
    return VOS_STATUS_E_FAULT;
  }

  /*                                                                        
                                          
                                                                           */
  vosStatus = WLANBAP_DeinitConnectionAcceptTimer(pBtampCtx);
  if ( VOS_STATUS_SUCCESS != vosStatus)
  {
    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
               "Couldn't destroy  bapConnectionAcceptTimer");
  }

  vosStatus = WLANBAP_DeinitLinkSupervisionTimer(pBtampCtx);
  if ( VOS_STATUS_SUCCESS != vosStatus)
  {
    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
               "Couldn't destroy  bapLinkSupervisionTimer");
  }

  vosStatus = vos_timer_destroy ( 
    &pBtampCtx->bapTxPktMonitorTimer );
  if ( VOS_STATUS_SUCCESS != vosStatus)
  {
    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
               "Couldn't destroy  bapTxPktMonitorTimer");
  }
  vos_lock_destroy(&pBtampCtx->bapLock);
  return VOS_STATUS_SUCCESS;
}/*              */

/*                                                                          

                           

              
                                                                                 
                                
    
               
    
             

      
                                                                         
                                                                    
   
              
                                                              

                                                                               
                                
                                               

               
  
                                                                            */
VOS_STATUS 
WLANBAP_Close
( 
  v_PVOID_t  pvosGCtx 
)
{
  ptBtampContext  pBtampCtx = NULL; 
  /*                                                                       */

  /*                                                                        
                
                              
                                                                           */
  pBtampCtx = VOS_GET_BAP_CB(pvosGCtx);
  if ( NULL == pBtampCtx ) 
  {
    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                 "Invalid BAP pointer from pvosGCtx on WLANBAP_Close");
    return VOS_STATUS_E_FAULT;
  }

  /*                                                                        
                               
                                                                           */
  VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "WLANBAP_Close");
  WLANBAP_CleanCB(pBtampCtx, 1 /*                               */);
#if  defined(ANI_OS_TYPE_ANDROID) && defined(WLAN_BTAMP_FEATURE)
  BSL_Deinit(pvosGCtx);
#endif
  /*                                                                        
                                      
                                                                           */
  vos_free_context(pvosGCtx, VOS_MODULE_ID_BAP, pBtampCtx);
  return VOS_STATUS_SUCCESS;
}/*               */

/*                                                                            
                                                 
                                                                            */

/*                                                                          

                                

              
                                                                 
                                                                      
               
    
                                                           
                                      
    
    
               
    
             

      
                                                          
   
              
                                                              

                                                                               
                                
                                               

               
  
                                                                            */

VOS_STATUS 
WLANBAP_GetNewHndl
( 
   ptBtampHandle *hBtampHandle  /*                                        */ 
)
{
  ptBtampContext  btampContext = NULL; 
  /*                                                                        
                       
                                                                           */
  if ( NULL == hBtampHandle) 
  {
    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                 "Invalid BAP handle pointer in WLANBAP_GetNewHndl");
    return VOS_STATUS_E_FAULT;
  }

#ifndef BTAMP_MULTIPLE_PHY_LINKS
  /*                                                                        
                                               
                                                                           */
  if ( NULL == gpBtampCtx ) 
  {
    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                "Invalid BAP pointer in WLANBAP_GetNewHndl");
    return VOS_STATUS_E_FAULT;
  }

  //                                           
  /*                                                                           */ 
  *hBtampHandle = (ptBtampHandle) gpBtampCtx; 
  btampContext = gpBtampCtx; 

  /*                                                                    
                                                          
   */
  WLANBAP_ReadMacConfig (btampContext);
  return VOS_STATUS_SUCCESS;
#else //                                  
#endif //                        
}/*                    */


/*                                                                          

                                 

              
                                                                            
                                            
    
    
               
    
             

      
                                                   
   
              
                                                              

                                                                    
                                     
                                               

               
  
                                                                            */
VOS_STATUS 
WLANBAP_ReleaseHndl
( 
  ptBtampHandle btampHandle  /*                                */ 
)
{
  /*                       */ 
  ptBtampContext  btampContext = (ptBtampContext) btampHandle; 
  tHalHandle halHandle;
  eHalStatus halStatus = eHAL_STATUS_SUCCESS;
  /*                                                                        
                       
                                                                           */
  if ( NULL == btampHandle) 
  {
    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                 "Invalid BAP handle value in WLANBAP_ReleaseHndl");
    return VOS_STATUS_E_FAULT;
  }

  /*                         */ 
  /*                                                            */
  /*                          
                                                               
                                    
                                                
                     */ 

  halHandle = VOS_GET_HAL_CB(btampContext->pvosGCtx);
  if(NULL == halHandle)
  {
     VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                  "halHandle is NULL in %s", __func__);
     return VOS_STATUS_E_FAULT;
  }

  if( btampContext->isBapSessionOpen == TRUE )
  {
    halStatus = sme_CloseSession(halHandle, 
            btampContext->sessionId, NULL, NULL);
    if(eHAL_STATUS_SUCCESS == halStatus)
    {
      btampContext->isBapSessionOpen = FALSE;
    }
  }

  /*                          */ 

  return VOS_STATUS_SUCCESS;
}/*                     */

/*                                                                            
                                    
                                                                            */

/*                                                                          

                             

              
                                            
    
               
    
             

      
                                                
                                                                
   
              
                                                              

                                                                               
                                
                                               

               
  
                                                                            */
VOS_STATUS 
WLANBAP_CleanCB
( 
  ptBtampContext  pBtampCtx,
  v_U32_t freeFlag //                     
)
{
  v_U16_t         i; /*                    */
  tpBtampLogLinkCtx  pLogLinkContext = NULL;
 
  /*                                                                        
                                   
                                                                           */

  if ( NULL == pBtampCtx ) 
  {
    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                 "Invalid BAP pointer in WLANBAP_CleanCB");
    return VOS_STATUS_E_FAULT;
  }

  /*                                                                        
                                                     
                                                                           */
  VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "WLANBAP_CleanCB");

  //                                                  
  vos_mem_set( pBtampCtx, sizeof( *pBtampCtx), 0);

  pBtampCtx->pvosGCtx = NULL;

  //                                                             
  //                                                           
   
  //                                                             
  vos_mem_copy( 
          &pBtampCtx->bapPhysLinkMachine,
          &bapPhysLinkMachineInitial,   /*                             */
          sizeof( pBtampCtx->bapPhysLinkMachine));

  VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "%s: Initializing State: %d", __func__, bapPhysLinkMachineInitial.stateVar);   
  VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "%s: Initialized State: %d", __func__,  pBtampCtx->bapPhysLinkMachine.stateVar); 

  //                                                                                                              
#ifdef BAP_DEBUG
  /*                                      */
  VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH,
            "WLAN BAP Context Monitor: pBtampCtx value = %x in %s:%d", pBtampCtx, __func__, __LINE__ );
#endif //         


  pBtampCtx->sessionId = 0;
  pBtampCtx->pAppHdl = NULL; //                    
  pBtampCtx->pHddHdl = NULL; //                    
  /*                                                       */
  pBtampCtx->phy_link_handle = 0;  
  pBtampCtx->channel = 0; 
  pBtampCtx->BAPDeviceRole = BT_RESPONDER;  
  pBtampCtx->ucSTAId = 0;  

  //                       
  pBtampCtx->gNeedPhysLinkCompEvent = VOS_FALSE;
  //                 
  pBtampCtx->gPhysLinkStatus = WLANBAP_STATUS_SUCCESS;
  //               
  pBtampCtx->gDiscRequested = VOS_FALSE;
  //             
  pBtampCtx->gDiscReason = WLANBAP_STATUS_SUCCESS;

  /*                                 */
  pBtampCtx->bapConnectionAcceptTimerInterval = WLANBAP_CONNECTION_ACCEPT_TIMEOUT;  
  /*                                */
  pBtampCtx->bapLinkSupervisionTimerInterval = WLANBAP_LINK_SUPERVISION_TIMEOUT;  
  /*                                   */
  pBtampCtx->bapLogicalLinkAcceptTimerInterval = WLANBAP_LOGICAL_LINK_ACCEPT_TIMEOUT;  
  /*                                 */
  pBtampCtx->bapBEFlushTimerInterval = WLANBAP_BE_FLUSH_TIMEOUT;  

  //                                       
  vos_mem_copy( 
          pBtampCtx->self_mac_addr, 
          pBtStaOwnMacAddr,   /*                                         */
          sizeof(pBtampCtx->self_mac_addr)); 

  vos_mem_set( 
          pBtampCtx->peer_mac_addr, 
          sizeof(pBtampCtx->peer_mac_addr),
          0); 

  //                           
  pBtampCtx->current_log_link_index = 0;  /*                 */  
  pBtampCtx->total_log_link_index = 0;  /*                     */  

  //                                    
  for (i = 0; i < WLANBAP_MAX_LOG_LINKS ; i++) 
  {
     pLogLinkContext = &pBtampCtx->btampLogLinkCtx[i];
     pLogLinkContext->present = 0; 
     pLogLinkContext->uTxPktCompleted = 0;
     pLogLinkContext->log_link_handle = 0;
  }


  //                                                                      
  pBtampCtx->pfnBtampFetchPktCB = NULL;   
  pBtampCtx->pfnBtamp_STARxCB = NULL;   
  pBtampCtx->pfnBtampTxCompCB = NULL;   
  /*                                                      */ 
  pBtampCtx->pBapHCIEventCB = NULL;   

  /*                                */ 
  vos_mem_set( 
          pBtampCtx->event_mask_page_2, 
          sizeof(pBtampCtx->event_mask_page_2),
          0); 

  /*                                    */ 
  pBtampCtx->btamp_Location_Data_Info.loc_options = 0x58;   
  /*                                    */ 
  pBtampCtx->ucDataTrafficMode = WLANBAP_FLOW_CONTROL_MODE_BLOCK_BASED;

  return VOS_STATUS_SUCCESS;
}/*                 */

/*                                                                          

                                     

              
                                                                        
                                                    
    
    
               
    
             

      
                                                   
   
       
                                                              
                                       
                                                              
   
              
                                                              

                                                                        
                                               

               
  
                                                                            */
VOS_STATUS 
WLANBAP_GetCtxFromStaId
( 
  v_U8_t         ucSTAId,  /*                                     */
  ptBtampHandle *hBtampHandle,  /*                                                */ 
  ptBtampContext *hBtampContext, /*                                                   */ 
  v_PVOID_t     *hHddHdl /*                                 */
)
{
#ifndef BTAMP_MULTIPLE_PHY_LINKS

    /*                                                        */ 
    /*                                       */ 
    //                                               
    //                            
    *hBtampHandle = (v_VOID_t*)gpBtampCtx;  
  
    //                           
    *hBtampContext = gpBtampCtx;

    /*                                 */
    //                              
    *hHddHdl = gpBtampCtx->pHddHdl;  

    return VOS_STATUS_SUCCESS;
#else //                                  

#endif //                        
}/*                         */

/*                                                                          

                                         

              
                                                                 
                                                                  
    
    
               
    
             

      
                                                              
                                       
                                                                   
    
       
                                                    
                                                           
   
              
                                                              

                                                                        
                                               

               
  
                                                                            */
VOS_STATUS 
WLANBAP_GetStaIdFromLinkCtx
( 
  ptBtampHandle  btampHandle,  /*                       */ 
  v_U8_t         phy_link_handle,  /*                          */
  v_U8_t        *pucSTAId,  /*                                     */
  v_PVOID_t     *hHddHdl /*                              */
)
{
#ifndef BTAMP_MULTIPLE_PHY_LINKS
    ptBtampContext           pBtampCtx = (ptBtampContext) btampHandle; 
    
    /*                                                                        
                           
                                                                              */
    if ( NULL == pBtampCtx) 
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                     "Invalid BAP handle value in %s", __func__);
        return VOS_STATUS_E_FAULT;
    }

    /*                                                           
                                                                     
     */ 
    /*                                     */
    *pucSTAId = pBtampCtx->ucSTAId;  

    /*                              */
    *hHddHdl = pBtampCtx->pHddHdl;  

    return VOS_STATUS_SUCCESS;
#else //                                  

#endif //                        
}/*                             */

/*                                                                          

                                         

              
                                                                       
    
    
               
    
             

      
                                           
                                                      
                                                       
    
       
                                                                
                                                           
   
              
                                                              

                                                                        
                                               

               
  
                                                                            */
VOS_STATUS 
WLANBAP_CreateNewPhyLinkCtx
( 
  ptBtampHandle  btampHandle,
  v_U8_t         phy_link_handle, /*                                         */
  v_PVOID_t      pHddHdl,   /*                                    */
  ptBtampContext *hBtampContext, /*                                                   */ 
  tWLAN_BAPRole  BAPDeviceRole
)
{
#ifndef BTAMP_MULTIPLE_PHY_LINKS
  ptBtampContext  pBtampCtx = gpBtampCtx;
  /*                                                                       */

  /*                                                     */
  WLANBAP_ReadMacConfig (pBtampCtx);

  /*                                                                        
                                             
                                                                          */
  pBtampCtx->ucSecEnabled = WLANBAP_SECURITY_ENABLED_STATE;

  /*                                                                        
                                                                 
                                                                          */
  pBtampCtx->phy_link_srm = 0;

  /*                                                                        
                                
                                                                          */
  pBtampCtx->current_log_link_index = 0;
  pBtampCtx->total_log_link_index = 0;

  /*                                                                        
                                                               
                                                                          */
  //                              
  pBtampCtx->csrRoamProfile.SSIDs.numOfSSIDs = 1; //                        
  pBtampCtx->csrRoamProfile.SSIDs.SSIDList = pBtampCtx->SSIDList;  //              
  pBtampCtx->csrRoamProfile.SSIDs.SSIDList[0].SSID.length = 0;
  pBtampCtx->csrRoamProfile.SSIDs.SSIDList[0].handoffPermitted = VOS_FALSE;
  pBtampCtx->csrRoamProfile.SSIDs.SSIDList[0].ssidHidden = VOS_FALSE;

  pBtampCtx->csrRoamProfile.BSSIDs.numOfBSSIDs = 1; //                        
  pBtampCtx->csrRoamProfile.BSSIDs.bssid = &pBtampCtx->bssid;  

  //                                                             
  //                                                                                         
  //                                                                                                   
  pBtampCtx->csrRoamProfile.negotiatedAuthType = eCSR_AUTH_TYPE_RSN_PSK;   
  pBtampCtx->csrRoamProfile.negotiatedUCEncryptionType = eCSR_ENCRYPT_TYPE_AES;  

  pBtampCtx->phy_link_handle = phy_link_handle;
  /*                                                          */ 
  //                           

  pBtampCtx->pHddHdl = pHddHdl;

  *hBtampContext = pBtampCtx;
  VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                 "Btamp Ctxt = %p", pBtampCtx);

  return VOS_STATUS_SUCCESS;
#else //                                  

#endif //                        
}/*                             */

/*                                                                          

                                           

              
                                                                            
    
    
               
    
             

      
                                              
                                                         
    
   
              
                                                              

                                                                        
                                               

               
  
                                                                            */
VOS_STATUS 
WLANBAP_UpdatePhyLinkCtxStaId
( 
  ptBtampContext pBtampContext, /*                        */ 
  v_U8_t         ucSTAId
)
{
#ifndef BTAMP_MULTIPLE_PHY_LINKS

    /*                                                                        
                           
                                                                              */
    if ( NULL == pBtampContext) 
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                     "Invalid BAP handle value in %s", __func__);
        return VOS_STATUS_E_FAULT;
    }

    /*                                     */
    pBtampContext->ucSTAId = ucSTAId;  

    return VOS_STATUS_SUCCESS;
#else //                                  

#endif //                        
}/*                               */

v_U8_t 
bapAllocNextLogLinkIndex
( 
  ptBtampContext pBtampContext, /*                                             */ 
  v_U8_t         phy_link_handle /*                                         */
)
{
  return ++(pBtampContext->current_log_link_index) % WLANBAP_MAX_LOG_LINKS; 
}/*                          */

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
)
{
#ifndef BTAMP_MULTIPLE_PHY_LINKS
  v_U16_t         i; /*                    */
  tpBtampLogLinkCtx        pLogLinkContext;
  v_U32_t         retval;
  /*                                                                       */

  /*                                                                        
                                                 
                                                                          */
  i = pBtampContext->current_log_link_index 
      = bapAllocNextLogLinkIndex(pBtampContext, phy_link_handle);
  pBtampContext->total_log_link_index++; 

  *pLog_link_handle = (i << 8) + ( v_U16_t ) phy_link_handle ; /*                                     */
  VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO,
                    " %s:*pLog_link_handle=%x", __func__,*pLog_link_handle);

  /*                                                                        
                                                                    
                                                                          */
  //                                                                      

#ifdef BAP_DEBUG
  /*                                      */
  VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH,
            "WLAN BAP Context Monitor: pBtampContext value = %p in %s:%d", pBtampContext, __func__, __LINE__ );
#endif //         

  /*                                                                        
                                           
                                                                          */
  pLogLinkContext = &(pBtampContext->btampLogLinkCtx[i]);

  /*                                                 */
  retval = btampUnpackTlvFlow_Spec((void *)pBtampContext, tx_flow_spec,
                          WLAN_BAP_PAL_FLOW_SPEC_TLV_LEN,
                          &pLogLinkContext->btampFlowSpec);
  if (retval != BTAMP_PARSE_SUCCESS)
  {
    /*                                          */
    return VOS_STATUS_E_BADMSG;
  }

  /*                                                         
                                                                    
                                   
                                                                      
                                                     
     */
  pLogLinkContext->log_link_handle = *pLog_link_handle;

  //                             
  pLogLinkContext->present = VOS_TRUE;
  //                                        
  pLogLinkContext->btampAC = 1;
  //                                                      
  pLogLinkContext->ucTID = 0;   //                                        
  pLogLinkContext->ucUP = 0;
  pLogLinkContext->uTxPktCompleted = 0;

  return VOS_STATUS_SUCCESS;
#else //                                  

#endif //                        
}/*                             */

/*                                                                          

                                     

              
                                                            
    
    
               
    
             

      
                                                               
                                    
    
   
              
        

               
  
                                                                            */
void 
WLANBAP_pmcFullPwrReqCB
( 
  void *callbackContext, 
  eHalStatus status
)
{

}/*                         */


/*                                                                          

                                   

              
                                                                         
        
               
    
             

                                                                         
                                                                    
   
              
        

               
  
                                                                            */
void
WLANBAP_ReadMacConfig
( 
  ptBtampContext  pBtampCtx 
)
{
  tANI_U32        len = WNI_CFG_BSSID_LEN;
  tHalHandle      pMac = NULL;

  /*                                                                        
                                                
                                                                          */
  if (NULL == pBtampCtx) 
  {
      VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                   "pBtampCtx is NULL in %s", __func__);

      return;
  }

  pMac = (tHalHandle)vos_get_context( VOS_MODULE_ID_SME, pBtampCtx->pvosGCtx);
  if (NULL == pMac) 
  {
      VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                   "pMac is NULL in %s", __func__);

      return;
  }

  ccmCfgGetStr( pMac, WNI_CFG_STA_ID, pBtStaOwnMacAddr, &len );

  VOS_ASSERT( WNI_CFG_BSSID_LEN == len );
  
  /*                                */
  VOS_SNPRINTF( pBtStaOwnSsid, WLAN_BAP_SSID_MAX_LEN,
            "AMP-%02x-%02x-%02x-%02x-%02x-%02x",
            pBtStaOwnMacAddr[0], pBtStaOwnMacAddr[1], pBtStaOwnMacAddr[2], 
            pBtStaOwnMacAddr[3], pBtStaOwnMacAddr[4], pBtStaOwnMacAddr[5]);

  /*                                                                        
                                         
                                                                          */
  vos_mem_copy( 
          pBtampCtx->self_mac_addr, 
          pBtStaOwnMacAddr,
          sizeof(pBtampCtx->self_mac_addr)); 
 
  /*                                                                        
                      
                                                                          */
  pBtampCtx->ownSsidLen = 21; 
  vos_mem_copy(
          pBtampCtx->ownSsid, 
          pBtStaOwnSsid,
          pBtampCtx->ownSsidLen); 
}

/*                                                                          

                                        

              
                                                                 
                                                                      

               

             

                                                                         
                                                                    
   
              
        

               
  
                                                                            */
//       
static int gBapCoexPriority;

void
WLANBAP_NeedBTCoexPriority
( 
  ptBtampContext  pBtampCtx, 
  v_U32_t         needCoexPriority
)
{
  tHalHandle      pMac = NULL;
  tSmeBtAmpEvent  btAmpEvent;


  /*                                                                        
                                   
                                                                          */
  pMac = (tHalHandle)vos_get_context( VOS_MODULE_ID_SME, pBtampCtx->pvosGCtx);

  //                                           
  if (needCoexPriority != gBapCoexPriority) {
    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, 
            "Calling %s with needCoexPriority=%d.", __func__, needCoexPriority);
 
    gBapCoexPriority = needCoexPriority;
    switch ( needCoexPriority)
    {
      case 0:  /*      */
          btAmpEvent.btAmpEventType = BTAMP_EVENT_CONNECTION_TERMINATED;
          pBtampCtx->btamp_session_on = FALSE;
          sme_sendBTAmpEvent(pMac, btAmpEvent);

          break;

      case 1:  /*             */
          btAmpEvent.btAmpEventType = BTAMP_EVENT_CONNECTION_START;
          pBtampCtx->btamp_session_on = TRUE;
          sme_sendBTAmpEvent(pMac, btAmpEvent);

          break;

      case 2:  /*            */
          btAmpEvent.btAmpEventType = BTAMP_EVENT_CONNECTION_STOP;
          sme_sendBTAmpEvent(pMac, btAmpEvent);

          break;

      default:
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                   "%s: Invalid Coexistence priority request: %d",
                   __func__, needCoexPriority);
    }

  }
}


/*                                                                          

                                

              
                                                                                          
        
               
    
             

                                                                         
                                                                   
                              
                              
   
              
        

               
  
                                                                            */
VOS_STATUS WLANBAP_RxCallback
(
    v_PVOID_t               pvosGCtx, 
    vos_pkt_t              *pPacket,
    WLANTL_BAPFrameEnumType frameType
)
{
    ptBtampContext  pBtampCtx = NULL; 

    pBtampCtx = VOS_GET_BAP_CB(pvosGCtx);
    if ( NULL == pBtampCtx ) 
    {
      VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                  "Invalid BAP pointer from pvosGCtx on WLANBAP_Start");
      return VOS_STATUS_E_FAULT;
    }

    switch (frameType)
    {
      case WLANTL_BT_AMP_TYPE_LS_REQ:  /*              */
      case WLANTL_BT_AMP_TYPE_LS_REP:
      {
          /*                                            */
          VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH,
                     "%s: link Supervision packet received over TL: %d, => BAP",
                     __func__, frameType);
          WLANBAP_RxProcLsPkt((ptBtampHandle)pBtampCtx,
                               pBtampCtx->phy_link_handle,
                               frameType,
                               pPacket);
          break;
      }

      case WLANTL_BT_AMP_TYPE_AR: /*              */
      case WLANTL_BT_AMP_TYPE_SEC:
      {
          /*                               */
          bapRsnRxCallback (pvosGCtx, pPacket);
          break;
      }

      default:
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                   "%s: Invalid frametype from TL: %d, => BAP",
                   __func__, frameType);
    }

    return ( VOS_STATUS_SUCCESS );
}

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

                      s a p M o d u l e . C

  OVERVIEW:

  This software unit holds the implementation of the WLAN SAP modules
  functions providing EXTERNAL APIs. It is also where the global SAP module
  context gets initialised

  DEPENDENCIES:

  Are listed for each API below.

  Copyright (c) 2010 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary
===========================================================================*/

/*                                                                           

                                           


                                                                       
                                                                



                                                         
                                                                                            
                                                  
                                                                   
                                                               

                                                                           */

/*          */

/*                                                                            
                
                                                                            */
#include "wlan_qct_tl.h"
#include "vos_trace.h"

//                                          
#include "sme_Api.h"

//                    

#include "sapInternal.h"
#include "smeInside.h"

/*                                                                            
                                         
                                                                            */
#define SAP_DEBUG

/*                                                                            
                    
                                                                            */


/*                                                                            
                          
                                                                            */

/*                                                                            
                                            
                                                                            */
//                         
//                                                            
ptSapContext  gpSapCtx;

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
WLANSAP_Open
(
    v_PVOID_t pvosGCtx
)
{

    ptSapContext  pSapCtx = NULL;
    /*                                                                       */
    VOS_ASSERT(pvosGCtx);
    /*                                                                        
                                                   
                                                                            */
    vos_alloc_context(pvosGCtx, VOS_MODULE_ID_SAP, (v_VOID_t **)&pSapCtx, sizeof(tSapContext));

    if (NULL == pSapCtx)
    {
        VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                   "%s: Invalid SAP pointer from pvosGCtx", __func__);
        return VOS_STATUS_E_FAULT;
    }

    vos_mem_zero(pSapCtx, sizeof(tSapContext));

    /*                                                                        
                                                         
                                                                            */
    VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH, "WLANSAP_Open");

    WLANSAP_CleanCB(pSapCtx, 0 /*            */);

    //                                          
    pSapCtx->pvosGCtx = pvosGCtx;

    //                                                    
    gpSapCtx = pSapCtx;

    /*                                                                        
                                   
                                                                               */

    return VOS_STATUS_SUCCESS;
}//             

/*                                                                          
                           

             
                                                                       
                                                           
                   

              

            

      
                                                                      
                                                                 

              
                                                            

                                                                             
                              
                               

              
                                                                            */

VOS_STATUS
WLANSAP_Start
(
    v_PVOID_t  pvosGCtx
)
{
    ptSapContext  pSapCtx = NULL;

    /*                                                                       */

    VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH,
                 "WLANSAP_Start invoked successfully\n");
    /*                                                                        
                    
                                 
                                                                            */
    pSapCtx = VOS_GET_SAP_CB(pvosGCtx);
    if ( NULL == pSapCtx )
    {
        VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                   "%s: Invalid SAP pointer from pvosGCtx", __func__);
        return VOS_STATUS_E_FAULT;
    }

    /*                                                                        
                                                 
                                                                           */
    pSapCtx->ucSecEnabled = WLANSAP_SECURITY_ENABLED_STATE;


    /*                                                                        
                                                                   
                                                                            */
    //                            
    pSapCtx->csrRoamProfile.SSIDs.numOfSSIDs = 1; //                      
    pSapCtx->csrRoamProfile.SSIDs.SSIDList = pSapCtx->SSIDList;  //            
    pSapCtx->csrRoamProfile.SSIDs.SSIDList[0].SSID.length = 0;
    pSapCtx->csrRoamProfile.SSIDs.SSIDList[0].handoffPermitted = VOS_FALSE;
    pSapCtx->csrRoamProfile.SSIDs.SSIDList[0].ssidHidden = pSapCtx->SSIDList[0].ssidHidden;

    pSapCtx->csrRoamProfile.BSSIDs.numOfBSSIDs = 1; //                      
    pSapCtx->csrRoamProfile.BSSIDs.bssid = &pSapCtx->bssid;

    //                                                             
    pSapCtx->csrRoamProfile.negotiatedAuthType = eCSR_AUTH_TYPE_OPEN_SYSTEM; //                    

    if( !VOS_IS_STATUS_SUCCESS( vos_lock_init( &pSapCtx->SapGlobalLock)))
    {
        VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                 "WLANSAP_Start failed init lock\n");
        return VOS_STATUS_E_FAULT;
    }



    return VOS_STATUS_SUCCESS;
}/*               */

/*                                                                          

                          

             
                                                                                   
                                                                        
                                      

              

            

      
                                                                      
                                                                 

              
                                                            

                                                                             
                              
                               

              
                                                                            */
VOS_STATUS
WLANSAP_Stop
(
    v_PVOID_t  pvosGCtx
)
{

    ptSapContext  pSapCtx = NULL;

    /*                                                                        
                    
                                 
                                                                            */
    VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH,
                "WLANSAP_Stop invoked successfully ");

    pSapCtx = VOS_GET_SAP_CB(pvosGCtx);
    if (NULL == pSapCtx)
    {
        VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                   "%s: Invalid SAP pointer from pvosGCtx", __func__);
        return VOS_STATUS_E_FAULT;
    }

    sapFreeRoamProfile(&pSapCtx->csrRoamProfile);

    if( !VOS_IS_STATUS_SUCCESS( vos_lock_destroy( &pSapCtx->SapGlobalLock ) ) )
    {
        VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                 "WLANSAP_Stop failed destroy lock\n");
        return VOS_STATUS_E_FAULT;
    }
    /*                                                                        
                                            
                                                                            */

    return VOS_STATUS_SUCCESS;
}/*              */

/*                                                                          
                           

             
                                                                                
                               

              

            

      
                                                                      
                                                                 

              
                                                            

                                                                             
                              
                               

              
                                                                            */
VOS_STATUS
WLANSAP_Close
(
    v_PVOID_t  pvosGCtx
)
{
    ptSapContext  pSapCtx = NULL;
    /*                                                                       */

    /*                                                                        
                    
                                 
                                                                            */
    VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH,
                 "WLANSAP_Close invoked");

    pSapCtx = VOS_GET_SAP_CB(pvosGCtx);
    if (NULL == pSapCtx)
    {
        VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                   "%s: Invalid SAP pointer from pvosGCtx", __func__);
        return VOS_STATUS_E_FAULT;
    }

    /*                                                                        
                                  
                                                                            */
    VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH, "WLANSAP_Close");
    WLANSAP_CleanCB(pSapCtx, VOS_TRUE /*                               */);

    /*                                                                        
                                         
                                                                            */
    vos_free_context(pvosGCtx, VOS_MODULE_ID_SAP, pSapCtx);

    return VOS_STATUS_SUCCESS;
}/*               */

/*                                                                            
                                   
                                                                            */

/*                                                                          

                             

             
                                            

              

            

      
                                                                      
                                                                 

              
                                                            

                                                                             
                              
                               

              
                                                                            */
VOS_STATUS
WLANSAP_CleanCB
(
    ptSapContext  pSapCtx,
    v_U32_t freeFlag //                     
)
{
    /*                                                                        
                                      
                                                                            */

    if (NULL == pSapCtx)
    {
        VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                   "%s: Invalid SAP pointer", __func__);
        return VOS_STATUS_E_FAULT;
    }

    /*                                                                        
                                                         
                                                                            */
    VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH, "WLANSAP_CleanCB");

    vos_mem_zero( pSapCtx, sizeof(tSapContext));

    pSapCtx->pvosGCtx = NULL;

    pSapCtx->sapsMachine= eSAP_DISCONNECTED;

    VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH, "%s: Initializing State: %d, sapContext value = %p",
            __func__, pSapCtx->sapsMachine, pSapCtx);
    pSapCtx->sessionId = 0;
    pSapCtx->channel = 0;

    return VOS_STATUS_SUCCESS;
}//                

/*                                                                          
                                     

             
                                                           

              

            

      
                                                              
                                   

              
        

              
                                                                            */
void
WLANSAP_pmcFullPwrReqCB
(
    void *callbackContext,
    eHalStatus status
)
{
    if(HAL_STATUS_SUCCESS(status))
    {
         //                                     
    }
    else
    {
        VOS_TRACE(VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_FATAL,
               "WLANSAP_pmcFullPwrReqCB: PMC failed to put the chip in Full power\n");

    }

}//                        
/*                                                                          
                              

             
                                                         

              

            

      
                                                          

              
                              
                                                                            */

v_U8_t WLANSAP_getState
(
    v_PVOID_t  pvosGCtx
)
{
    ptSapContext  pSapCtx = NULL;

    pSapCtx = VOS_GET_SAP_CB(pvosGCtx);

    if ( NULL == pSapCtx )
    {
        VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH,
                   "%s: Invalid SAP pointer from pvosGCtx", __func__);
        return VOS_STATUS_E_FAULT;
    }
    return pSapCtx->sapsMachine;
}

/*                                                                          
                              

             
                                                                               
                   

              

            

      
                                                          
                                                                                                      
                                                                                                
                                                                                            

              
                                                            

                                                                             
                              
                               

              
                                                                            */
VOS_STATUS
WLANSAP_StartBss
(
    v_PVOID_t  pvosGCtx,//        
    tpWLAN_SAPEventCB pSapEventCallback,
    tsap_Config_t *pConfig,
    v_PVOID_t  pUsrContext
)
{
    tWLAN_SAPEvent sapEvent;    /*                    */
    VOS_STATUS vosStatus = VOS_STATUS_SUCCESS;
    ptSapContext  pSapCtx = NULL;
    tANI_BOOLEAN restartNeeded;
    tHalHandle hHal;

    /*                                                                       */

    /*                                                                        
                    
                                 
                                                                            */
    VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH,
                 "WLANSAP_StartBss");

    if (VOS_STA_SAP_MODE == vos_get_conparam ())
    {
        pSapCtx = VOS_GET_SAP_CB(pvosGCtx);
        if ( NULL == pSapCtx )
        {
            VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH,
                       "%s: Invalid SAP pointer from pvosGCtx", __func__);
            return VOS_STATUS_E_FAULT;
        }
        pSapCtx->sapsMachine = eSAP_DISCONNECTED;

        /*                                         */
        pSapCtx->channel = pConfig->channel;
        pSapCtx->pUsrContext = pUsrContext;

        //                                                                                                   
        pSapCtx->csrRoamProfile.BSSIDs.numOfBSSIDs = 1;
        vos_mem_copy(pSapCtx->csrRoamProfile.BSSIDs.bssid,
                     pSapCtx->self_mac_addr,
                     sizeof( tCsrBssid ) );

        //                          
        vos_mem_copy(pSapCtx->csrRoamProfile.BSSIDs.bssid,
                    pConfig->self_macaddr.bytes, sizeof(v_MACADDR_t));
        vos_mem_copy(pSapCtx->self_mac_addr,
                    pConfig->self_macaddr.bytes, sizeof(v_MACADDR_t));

        //                                          
        sapconvertToCsrProfile( pConfig, eCSR_BSS_TYPE_INFRA_AP, &pSapCtx->csrRoamProfile);
        hHal = (tHalHandle)VOS_GET_HAL_CB(pvosGCtx);
        if (NULL == hHal)
        {
            VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH,
                       "%s: Invalid MAC context from pvosGCtx", __func__);
        }
        else
        {
            //                                                           
            //                                                       
            //                                                           
            //                 
            if ((0 == sme_GetConcurrentOperationChannel(hHal)) &&
                pConfig->ieee80211d)
            {
                /*                                         */
                sme_setRegInfo(hHal, pConfig->countryCode);
                sme_ResetCountryCodeInformation(hHal, &restartNeeded);
            }
        }

        //                                           
        pSapCtx->eSapMacAddrAclMode = pConfig->SapMacaddr_acl;
        vos_mem_copy(pSapCtx->acceptMacList, pConfig->accept_mac, sizeof(pConfig->accept_mac));
        pSapCtx->nAcceptMac = pConfig->num_accept_mac;
        sapSortMacList(pSapCtx->acceptMacList, pSapCtx->nAcceptMac);
        vos_mem_copy(pSapCtx->denyMacList, pConfig->deny_mac, sizeof(pConfig->deny_mac));
        pSapCtx->nDenyMac = pConfig->num_deny_mac;
        sapSortMacList(pSapCtx->denyMacList, pSapCtx->nDenyMac);

        /*                                     */
        sapEvent.event = eSAP_HDD_START_INFRA_BSS;
        sapEvent.params = 0;//                  

        /*                                       */
        pSapCtx->pfnSapEventCallback = pSapEventCallback;

        /*             */
        vosStatus = sapFsm(pSapCtx, &sapEvent);
     }
     else
     {
        VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                "SoftAp role has not been enabled");
     }

    return vosStatus;
}//                 

/*                                                                          
                               

             
                                                                               
                

              

            

      
                                                          
                                                                             
                                                  

              
                                                            

                                                                             
                              
                               

              
                                                                            */
VOS_STATUS
WLANSAP_SetMacACL
(
    v_PVOID_t  pvosGCtx,   //        
    tsap_Config_t *pConfig
)
{
    VOS_STATUS vosStatus = VOS_STATUS_SUCCESS;
    ptSapContext  pSapCtx = NULL;

    VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH,
                 "WLANSAP_SetMacACL");

    if (VOS_STA_SAP_MODE == vos_get_conparam ())
    {
        pSapCtx = VOS_GET_SAP_CB(pvosGCtx);
        if ( NULL == pSapCtx )
        {
            VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH,
                       "%s: Invalid SAP pointer from pvosGCtx", __func__);
            return VOS_STATUS_E_FAULT;
        }

        //                                           
        pSapCtx->eSapMacAddrAclMode = pConfig->SapMacaddr_acl;

        if (eSAP_DENY_UNLESS_ACCEPTED == pSapCtx->eSapMacAddrAclMode)
        {
            vos_mem_copy(pSapCtx->acceptMacList, pConfig->accept_mac,
                                                 sizeof(pConfig->accept_mac));
            pSapCtx->nAcceptMac = pConfig->num_accept_mac;
            sapSortMacList(pSapCtx->acceptMacList, pSapCtx->nAcceptMac);
        }
        else if (eSAP_ACCEPT_UNLESS_DENIED == pSapCtx->eSapMacAddrAclMode)
        {
            vos_mem_copy(pSapCtx->denyMacList, pConfig->deny_mac,
                                               sizeof(pConfig->deny_mac));
            pSapCtx->nDenyMac = pConfig->num_deny_mac;
            sapSortMacList(pSapCtx->denyMacList, pSapCtx->nDenyMac);
        }
    }
    else
    {
        VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                       "%s : SoftAp role has not been enabled", __func__);
        return VOS_STATUS_E_FAULT;
    }

    return vosStatus;
}//                 

/*                                                                          
                             

             
                                                                        
                   

              

            

      
                                                                      
                                                                 

              
                                                            

                                                                              
                              
                               

              
                                                                            */
VOS_STATUS
WLANSAP_StopBss
(
 v_PVOID_t  pvosGCtx
)
{
    tWLAN_SAPEvent sapEvent;    /*                    */
    VOS_STATUS vosStatus = VOS_STATUS_SUCCESS;
    ptSapContext  pSapCtx = NULL;
    /*                                                                       */

    /*                                                                        
                    
                                 
                                                                            */
    VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH,
                 "WLANSAP_StopBss");

    if ( NULL == pvosGCtx )
    {
        VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                   "%s: Invalid Global VOSS handle", __func__);
        return VOS_STATUS_E_FAULT;
    }

    pSapCtx = VOS_GET_SAP_CB(pvosGCtx);

    if (NULL == pSapCtx )
    {
        VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                   "%s: Invalid SAP pointer from pvosGCtx", __func__);
        return VOS_STATUS_E_FAULT;
    }

    /*                                     */
    sapEvent.event = eSAP_HDD_STOP_INFRA_BSS;
    sapEvent.params = 0;

    /*             */
    vosStatus = sapFsm(pSapCtx, &sapEvent);

    return vosStatus;
}

/*                                                                          
                                      

             
                                                                                                         

              
       

            

      
                                                             
                                                                                                                             
                                                                       
          
                                                                                                                    

                                                                            

              
                                                                

                                

              
                                                                            */
VOS_STATUS
WLANSAP_GetAssocStations
(
    v_PVOID_t pvosGCtx,
    VOS_MODULE_ID modId,
    tpSap_AssocMacAddr pAssocStas
)
{
    ptSapContext  pSapCtx = VOS_GET_SAP_CB(pvosGCtx);

    /*                                                                        
                  
                               
                                                                              */
    if (NULL == pSapCtx)
    {
      VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                 "%s: Invalid SAP pointer from pvosGCtx", __func__);
      return VOS_STATUS_E_FAULT;
    }

    sme_RoamGetAssociatedStas( VOS_GET_HAL_CB(pSapCtx->pvosGCtx), pSapCtx->sessionId,
                                modId,
                                pSapCtx->pUsrContext,
                                (v_PVOID_t *)pSapCtx->pfnSapEventCallback,
                                (v_U8_t *)pAssocStas );

    return VOS_STATUS_SUCCESS;
}


/*                                                                          
                                             

             
                                                                                                   

              
       

            

      
                                                     
                                                              

              
                                                                

                                
                                                                                       

              
                                                                            */
VOS_STATUS
WLANSAP_RemoveWpsSessionOverlap

(
    v_PVOID_t pvosGCtx,
    v_MACADDR_t pRemoveMac
)
{
  ptSapContext  pSapCtx = VOS_GET_SAP_CB(pvosGCtx);

  /*                                                                        
                
                             
                                                                          */
  if (NULL == pSapCtx)
  {
    VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
               "%s: Invalid SAP pointer from pvosGCtx", __func__);
    return VOS_STATUS_E_FAULT;
  }

  sme_RoamGetWpsSessionOverlap( VOS_GET_HAL_CB(pSapCtx->pvosGCtx), pSapCtx->sessionId,
                                pSapCtx->pUsrContext,
                                (v_PVOID_t *)pSapCtx->pfnSapEventCallback,
                                pRemoveMac);

  return VOS_STATUS_SUCCESS;
}

/*                                                                          
                                          

             
                                                                              

              
       

            

      
                                                     

              
                                                                

                                

              
                                                                            */
VOS_STATUS
WLANSAP_getWpsSessionOverlap
(
 v_PVOID_t pvosGCtx
)
{
    v_MACADDR_t pRemoveMac = VOS_MAC_ADDR_ZERO_INITIALIZER;

    ptSapContext  pSapCtx = VOS_GET_SAP_CB(pvosGCtx);

    /*                                                                        
                  
                               
                                                                              */
    if (NULL == pSapCtx)
    {
      VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                 "%s: Invalid SAP pointer from pvosGCtx", __func__);
      return VOS_STATUS_E_FAULT;
    }

    sme_RoamGetWpsSessionOverlap( VOS_GET_HAL_CB(pSapCtx->pvosGCtx), pSapCtx->sessionId,
                                pSapCtx->pUsrContext,
                                (v_PVOID_t *)pSapCtx->pfnSapEventCallback,
                                pRemoveMac);

    return VOS_STATUS_SUCCESS;
}


/*                                                                */
VOS_STATUS
WLANSAP_SetMode ( v_PVOID_t  pvosGCtx, v_U32_t mode)
{
    ptSapContext  pSapCtx = VOS_GET_SAP_CB(pvosGCtx);

    if (NULL == pSapCtx)
    {
        VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                   "%s: Invalid SAP pointer from pvosGCtx", __func__);
        return VOS_STATUS_E_FAULT;
    }

    pSapCtx->eSapMacAddrAclMode = (eSapMacAddrACL)mode;
    return VOS_STATUS_SUCCESS;
}

/*                                                                              */

VOS_STATUS
WLANSAP_ClearACL( v_PVOID_t  pvosGCtx)
{
    ptSapContext  pSapCtx = VOS_GET_SAP_CB(pvosGCtx);
    v_U8_t i;

    if (NULL == pSapCtx)
    {
        return VOS_STATUS_E_RESOURCES;
    }

    if (pSapCtx->denyMacList != NULL)
    {
        for (i = 0; i < (pSapCtx->nDenyMac-1); i++)
        {
            vos_mem_zero((pSapCtx->denyMacList+i)->bytes, sizeof(v_MACADDR_t));

        }
    }
    sapPrintACL(pSapCtx->denyMacList, pSapCtx->nDenyMac);
    pSapCtx->nDenyMac  = 0;

    if (pSapCtx->acceptMacList!=NULL)
    {
        for (i = 0; i < (pSapCtx->nAcceptMac-1); i++)
        {
            vos_mem_zero((pSapCtx->acceptMacList+i)->bytes, sizeof(v_MACADDR_t));

        }
    }
    sapPrintACL(pSapCtx->acceptMacList, pSapCtx->nAcceptMac);
    pSapCtx->nAcceptMac = 0;

    return VOS_STATUS_SUCCESS;
}

VOS_STATUS
WLANSAP_ModifyACL
(
    v_PVOID_t  pvosGCtx,
    v_U8_t *pPeerStaMac,
    eSapACLType listType,
    eSapACLCmdType cmd
)
{
    eSapBool staInWhiteList=eSAP_FALSE, staInBlackList=eSAP_FALSE;
    v_U8_t staWLIndex, staBLIndex;
    ptSapContext  pSapCtx = VOS_GET_SAP_CB(pvosGCtx);

    if (NULL == pSapCtx)
    {
       VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                  "%s: Invalid SAP Context", __func__);
       return VOS_STATUS_E_FAULT;
    }

    VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_LOW,"Modify ACL entered\n"
            "Before modification of ACL\n"
            "size of accept and deny lists %d %d",
            pSapCtx->nAcceptMac, pSapCtx->nDenyMac);
    VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH,"*** WHITE LIST ***");
    sapPrintACL(pSapCtx->acceptMacList, pSapCtx->nAcceptMac);
    VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH,"*** BLACK LIST ***");
    sapPrintACL(pSapCtx->denyMacList, pSapCtx->nDenyMac);

    /*                                                                              
                                                           */
    staInWhiteList = sapSearchMacList(pSapCtx->acceptMacList, pSapCtx->nAcceptMac, pPeerStaMac, &staWLIndex);
    staInBlackList = sapSearchMacList(pSapCtx->denyMacList, pSapCtx->nDenyMac, pPeerStaMac, &staBLIndex);

    if (staInWhiteList && staInBlackList)
    {
        VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                "Peer mac "MAC_ADDRESS_STR" found in white and black lists."
                "Initial lists passed incorrect. Cannot execute this command.",
                MAC_ADDR_ARRAY(pPeerStaMac));
        return VOS_STATUS_E_FAILURE;

    }

    switch(listType)
    {
        case eSAP_WHITE_LIST:
            VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_LOW, "cmd %d", cmd);
            if (cmd == ADD_STA_TO_ACL)
            {
                //           
                //                                          
                if (pSapCtx->nAcceptMac == MAX_ACL_MAC_ADDRESS)
                {
                    VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                            "White list is already maxed out. Cannot accept "MAC_ADDRESS_STR,
                            MAC_ADDR_ARRAY(pPeerStaMac));
                    return VOS_STATUS_E_FAILURE;
                }
                if (staInWhiteList)
                {
                    //                                                                 
                    VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_WARN,
                            "MAC address already present in white list "MAC_ADDRESS_STR,
                            MAC_ADDR_ARRAY(pPeerStaMac));
                } else
                {
                    if (staInBlackList)
                    {
                        //                                                         
                        VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_WARN,
                                "STA present in black list so first remove from it");
                        sapRemoveMacFromACL(pSapCtx->denyMacList, &pSapCtx->nDenyMac, staBLIndex);
                    }
                    VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO,
                            "... Now add to the white list");
                    sapAddMacToACL(pSapCtx->acceptMacList, &pSapCtx->nAcceptMac, pPeerStaMac);
                    VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_LOW, "size of accept and deny lists %d %d",
                            pSapCtx->nAcceptMac, pSapCtx->nDenyMac);
                }
            }
            else if (cmd == DELETE_STA_FROM_ACL)
            {
                if (staInWhiteList)
                {
                    VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO, "Delete from white list");
                    sapRemoveMacFromACL(pSapCtx->acceptMacList, &pSapCtx->nAcceptMac, staWLIndex);
                    VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_LOW, "size of accept and deny lists %d %d",
                            pSapCtx->nAcceptMac, pSapCtx->nDenyMac);
                }
                else
                {
                    VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_WARN,
                            "MAC address to be deleted is not present in the white list "MAC_ADDRESS_STR,
                            MAC_ADDR_ARRAY(pPeerStaMac));
                    return VOS_STATUS_E_FAILURE;
                }
            }
            else
            {
                VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR, "Invalid cmd type passed");
                return VOS_STATUS_E_FAILURE;
            }
            break;

        case eSAP_BLACK_LIST:
            VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_LOW,
                    "cmd %d", cmd);
            if (cmd == ADD_STA_TO_ACL)
            {
                //           
                //                                          
                if (pSapCtx->nDenyMac == MAX_ACL_MAC_ADDRESS)
                {
                    VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                            "Black list is already maxed out. Cannot accept "MAC_ADDRESS_STR,
                            MAC_ADDR_ARRAY(pPeerStaMac));
                    return VOS_STATUS_E_FAILURE;
                }
                if (staInBlackList)
                {
                    //                                           
                    VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_WARN,
                            "MAC address already present in black list "MAC_ADDRESS_STR,
                            MAC_ADDR_ARRAY(pPeerStaMac));
                } else
                {
                    if (staInWhiteList)
                    {
                        //                                                         
                        VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_WARN,
                                "Present in white list so first remove from it");
                        sapRemoveMacFromACL(pSapCtx->acceptMacList, &pSapCtx->nAcceptMac, staWLIndex);
                    }
                    VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO,
                            "... Now add to black list");
                    sapAddMacToACL(pSapCtx->denyMacList, &pSapCtx->nDenyMac, pPeerStaMac);
                    VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_LOW,"size of accept and deny lists %d %d",
                            pSapCtx->nAcceptMac, pSapCtx->nDenyMac);
                }
            }
            else if (cmd == DELETE_STA_FROM_ACL)
            {
                if (staInBlackList)
                {
                    VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO, "Delete from black list");
                    sapRemoveMacFromACL(pSapCtx->denyMacList, &pSapCtx->nDenyMac, staBLIndex);
                    VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_LOW,"no accept and deny mac %d %d",
                            pSapCtx->nAcceptMac, pSapCtx->nDenyMac);
                }
                else
                {
                    VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_WARN,
                            "MAC address to be deleted is not present in the black list "MAC_ADDRESS_STR,
                            MAC_ADDR_ARRAY(pPeerStaMac));
                    return VOS_STATUS_E_FAILURE;
                }
            }
            else
            {
                VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR, "Invalid cmd type passed");
                return VOS_STATUS_E_FAILURE;
            }
            break;

        default:
            {
                VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                        "Invalid list type passed %d",listType);
                return VOS_STATUS_E_FAILURE;
            }
    }
    VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_LOW,"After modification of ACL");
    VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH,"*** WHITE LIST ***");
    sapPrintACL(pSapCtx->acceptMacList, pSapCtx->nAcceptMac);
    VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO_HIGH,"*** BLACK LIST ***");
    sapPrintACL(pSapCtx->denyMacList, pSapCtx->nDenyMac);
    return VOS_STATUS_SUCCESS;
}

/*                                                                          
                                 

             
                                                                                 

              
       

            

      
                                                                 
                                                                    

              
                                                                

                                

              
                                                                            */
VOS_STATUS
WLANSAP_DisassocSta
(
    v_PVOID_t  pvosGCtx,
    v_U8_t *pPeerStaMac
)
{
    ptSapContext  pSapCtx = VOS_GET_SAP_CB(pvosGCtx);

    /*                                                                        
                  
                               
                                                                              */
    if (NULL == pSapCtx)
    {
        VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                   "%s: Invalid SAP pointer from pvosGCtx", __func__);
        return VOS_STATUS_E_FAULT;
    }

    sme_RoamDisconnectSta(VOS_GET_HAL_CB(pSapCtx->pvosGCtx), pSapCtx->sessionId,
                            pPeerStaMac);

    return VOS_STATUS_SUCCESS;
}

/*                                                                          
                               

             
                                                                                   

              
       

            

      
                                                                 
                                                                      

              
                                                                

                                

              
                                                                            */
VOS_STATUS
WLANSAP_DeauthSta
(
    v_PVOID_t  pvosGCtx,
    v_U8_t *pPeerStaMac
)
{
    eHalStatus halStatus = eHAL_STATUS_FAILURE;
    VOS_STATUS vosStatus = VOS_STATUS_E_FAULT;
    ptSapContext  pSapCtx = VOS_GET_SAP_CB(pvosGCtx);

    /*                                                                        
                  
                               
                                                                              */
    if (NULL == pSapCtx)
    {
        VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                   "%s: Invalid SAP pointer from pvosGCtx", __func__);
        return vosStatus;
    }

    halStatus = sme_RoamDeauthSta(VOS_GET_HAL_CB(pSapCtx->pvosGCtx), pSapCtx->sessionId,
                            pPeerStaMac);

    if (halStatus == eHAL_STATUS_SUCCESS)
    {
        vosStatus = VOS_STATUS_SUCCESS;
    }
    return vosStatus;
}
/*                                                                          
                                     

             
                                                        

              
       

            

      
                                        
                                      
                                                       

              
                                                                

                                

              
                                                                            */
VOS_STATUS
WLANSAP_SetChannelRange(tHalHandle hHal,v_U8_t startChannel, v_U8_t endChannel,
                              v_U8_t operatingBand)
{

    v_U8_t    validChannelFlag =0;
    v_U8_t    loopStartCount =0;
    v_U8_t    loopEndCount =0;
    v_U8_t    bandStartChannel =0;
    v_U8_t    bandEndChannel =0;

    VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO,
         "WLANSAP_SetChannelRange:startChannel %d,EndChannel %d,Operatingband:%d",
         startChannel,endChannel,operatingBand);

    /*                                                                        
                  
                                                                              */
    if (( WNI_CFG_SAP_CHANNEL_SELECT_OPERATING_BAND_STAMIN > operatingBand) ||
          (WNI_CFG_SAP_CHANNEL_SELECT_OPERATING_BAND_STAMAX < operatingBand))
    {
         VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                     "Invalid operatingBand on WLANSAP_SetChannelRange");
        return VOS_STATUS_E_FAULT;
    }
    if (( WNI_CFG_SAP_CHANNEL_SELECT_START_CHANNEL_STAMIN > startChannel) ||
         (WNI_CFG_SAP_CHANNEL_SELECT_START_CHANNEL_STAMAX < startChannel))
    {
        VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                    "Invalid startChannel value on WLANSAP_SetChannelRange");
        return VOS_STATUS_E_FAULT;
    }
    if (( WNI_CFG_SAP_CHANNEL_SELECT_END_CHANNEL_STAMIN > endChannel) ||
         (WNI_CFG_SAP_CHANNEL_SELECT_END_CHANNEL_STAMAX < endChannel))
    {
        VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                      "Invalid endChannel value on WLANSAP_SetChannelRange");
        return VOS_STATUS_E_FAULT;
    }
    switch(operatingBand)
    {
       case RF_SUBBAND_2_4_GHZ:
          bandStartChannel = RF_CHAN_1;
          bandEndChannel = RF_CHAN_14;
          break;

       case RF_SUBBAND_5_LOW_GHZ:
          bandStartChannel = RF_CHAN_36;
          bandEndChannel = RF_CHAN_64;
          break;

       case RF_SUBBAND_5_MID_GHZ:
          bandStartChannel = RF_CHAN_100;
#ifndef FEATURE_WLAN_CH144
          bandEndChannel = RF_CHAN_140;
#else
          bandEndChannel = RF_CHAN_144;
#endif /*                    */
          break;

       case RF_SUBBAND_5_HIGH_GHZ:
          bandStartChannel = RF_CHAN_149;
          bandEndChannel = RF_CHAN_165;
          break;

       default:
          VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                   "Invalid operatingBand value on WLANSAP_SetChannelRange");
          break;
    }

    /*                                                */
    for(loopStartCount = bandStartChannel ; loopStartCount <= bandEndChannel ;
    loopStartCount++)
    {
       if(rfChannels[loopStartCount].channelNum == startChannel )
       {
          /*                               */
          break;
       }
    }
    /*                                              */
    for(loopEndCount = bandStartChannel ; loopEndCount <= bandEndChannel ;
    loopEndCount++)
    {
        if(rfChannels[loopEndCount].channelNum == endChannel )
        {
          /*                             */
            break;
        }
    }
    if((loopStartCount > bandEndChannel)||(loopEndCount > bandEndChannel))
    {
       VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                  "%s: Invalid startChannel-%d or EndChannel-%d for band -%d",
                   __func__,startChannel,endChannel,operatingBand);
       /*                                                                  
                                                  */
       startChannel = rfChannels[bandStartChannel].channelNum;
       endChannel = rfChannels[bandEndChannel].channelNum;
    }

    /*                                                  */
    for( loopStartCount = bandStartChannel; loopStartCount <= bandEndChannel; loopStartCount++ )
    {
       if((startChannel <= rfChannels[loopStartCount].channelNum)&&
             (endChannel >= rfChannels[loopStartCount].channelNum ))
       {
          if( regChannels[loopStartCount].enabled )
          {
             validChannelFlag = 1;
             break;
          }
       }
    }
    if(0 == validChannelFlag)
    {
       VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
         "%s-No active channels present in the given range for the current region",
         __func__);
       /*                                                                     
                                                                                                         */
       startChannel = rfChannels[bandStartChannel].channelNum;
       endChannel = rfChannels[bandEndChannel].channelNum;
    }

    if (ccmCfgSetInt(hHal, WNI_CFG_SAP_CHANNEL_SELECT_OPERATING_BAND,
       operatingBand, NULL, eANI_BOOLEAN_FALSE)==eHAL_STATUS_FAILURE)
    {
         VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
          "Could not pass on WNI_CFG_SAP_CHANNEL_SELECT_OPERATING_BAND to CCn");
         return VOS_STATUS_E_FAULT;
    }
    if (ccmCfgSetInt(hHal, WNI_CFG_SAP_CHANNEL_SELECT_START_CHANNEL,
        startChannel, NULL, eANI_BOOLEAN_FALSE)==eHAL_STATUS_FAILURE)
    {

       VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
          "Could not pass on WNI_CFG_SAP_CHANNEL_SELECT_START_CHANNEL to CCM");
       return VOS_STATUS_E_FAULT;

    }
    if (ccmCfgSetInt(hHal, WNI_CFG_SAP_CHANNEL_SELECT_END_CHANNEL,
       endChannel, NULL, eANI_BOOLEAN_FALSE)==eHAL_STATUS_FAILURE)
    {

       VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
          "Could not pass on WNI_CFG_SAP_CHANNEL_SELECT_START_CHANNEL to CCM");
       return VOS_STATUS_E_FAULT;
    }
    return VOS_STATUS_SUCCESS;
}

/*                                                                          
                                       

             
                                                                          
                                                                                    
                                                                              
                                                                                
                                                                                          
                                                                                      
                                                                                           
                                                                                                

              
       

            

      
                                                 
                                                                                                                     
                            

              
                                                                

                                

              
                                                                            */
VOS_STATUS
WLANSAP_SetCounterMeasure
(
    v_PVOID_t pvosGCtx,
    v_BOOL_t bEnable
)
{
    ptSapContext  pSapCtx = VOS_GET_SAP_CB(pvosGCtx);

    /*                                                                        
                  
                               
                                                                              */
    if (NULL == pSapCtx)
    {
        VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                   "%s: Invalid SAP pointer from pvosGCtx", __func__);
        return VOS_STATUS_E_FAULT;
    }

    sme_RoamTKIPCounterMeasures(VOS_GET_HAL_CB(pSapCtx->pvosGCtx), pSapCtx->sessionId, bEnable);

    return VOS_STATUS_SUCCESS;
}

/*                                                                          

                                

             
                                                                       

              
       

            

      
                                                 
                                                     

              
                                                                

                                

              
                                                                            */
VOS_STATUS
WLANSAP_SetKeySta
(
    v_PVOID_t pvosGCtx, tCsrRoamSetKey *pSetKeyInfo
)
{
    VOS_STATUS vosStatus = VOS_STATUS_SUCCESS;
    ptSapContext  pSapCtx = NULL;
    v_PVOID_t hHal = NULL;
        eHalStatus halStatus = eHAL_STATUS_FAILURE;
        v_U32_t roamId=0xFF;

    if (VOS_STA_SAP_MODE == vos_get_conparam ( ))
    {
        pSapCtx = VOS_GET_SAP_CB(pvosGCtx);
        if (NULL == pSapCtx)
        {
            VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                       "%s: Invalid SAP pointer from pvosGCtx", __func__);
            return VOS_STATUS_E_FAULT;
        }
        hHal = VOS_GET_HAL_CB(pSapCtx->pvosGCtx);
        if (NULL == hHal)
        {
            VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                       "%s: Invalid HAL pointer from pvosGCtx", __func__);
            return VOS_STATUS_E_FAULT;
        }
        halStatus = sme_RoamSetKey(hHal, pSapCtx->sessionId, pSetKeyInfo, &roamId);

        if (halStatus == eHAL_STATUS_SUCCESS)
        {
            vosStatus = VOS_STATUS_SUCCESS;
        } else
        {
            vosStatus = VOS_STATUS_E_FAULT;
        }
    }
    else
        vosStatus = VOS_STATUS_E_FAULT;

    return vosStatus;
}

/*                                                                          
                               

             
                                                                          

              
       

            

      
                                                 
                                                        

              
                                                                

                                

              
                                                                            */
VOS_STATUS
WLANSAP_DelKeySta
(
     v_PVOID_t pvosGCtx,
    tCsrRoamRemoveKey *pRemoveKeyInfo
)
{
    VOS_STATUS vosStatus = VOS_STATUS_SUCCESS;
    ptSapContext  pSapCtx = NULL;
    v_PVOID_t hHal = NULL;
    eHalStatus halStatus = eHAL_STATUS_FAILURE;
    v_U32_t roamId=0xFF;
    tCsrRoamRemoveKey RemoveKeyInfo;

    if (VOS_STA_SAP_MODE == vos_get_conparam ( ))
    {
        pSapCtx = VOS_GET_SAP_CB(pvosGCtx);
        if (NULL == pSapCtx)
        {
            VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                       "%s: Invalid SAP pointer from pvosGCtx", __func__);
            return VOS_STATUS_E_FAULT;
        }

        hHal = VOS_GET_HAL_CB(pSapCtx->pvosGCtx);
        if (NULL == hHal)
        {
            VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                       "%s: Invalid HAL pointer from pvosGCtx", __func__);
            return VOS_STATUS_E_FAULT;
        }

        vos_mem_zero(&RemoveKeyInfo, sizeof(RemoveKeyInfo));
        RemoveKeyInfo.encType = pRemoveKeyInfo->encType;
        vos_mem_copy(RemoveKeyInfo.peerMac, pRemoveKeyInfo->peerMac, WNI_CFG_BSSID_LEN);
        RemoveKeyInfo.keyId = pRemoveKeyInfo->keyId;

        halStatus = sme_RoamRemoveKey(hHal, pSapCtx->sessionId, &RemoveKeyInfo, &roamId);

        if (HAL_STATUS_SUCCESS(halStatus))
        {
            vosStatus = VOS_STATUS_SUCCESS;
        }
        else
        {
            vosStatus = VOS_STATUS_E_FAULT;
        }
    }
    else
        vosStatus = VOS_STATUS_E_FAULT;

    return vosStatus;
}

VOS_STATUS
WLANSap_getstationIE_information(v_PVOID_t pvosGCtx,
                                 v_U32_t   *pLen,
                                 v_U8_t    *pBuf)
{
    VOS_STATUS vosStatus = VOS_STATUS_E_FAILURE;
    ptSapContext  pSapCtx = NULL;
    v_U32_t len = 0;

    if (VOS_STA_SAP_MODE == vos_get_conparam ( )){
        pSapCtx = VOS_GET_SAP_CB(pvosGCtx);
        if (NULL == pSapCtx)
        {
            VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                       "%s: Invalid SAP pointer from pvosGCtx", __func__);
            return VOS_STATUS_E_FAULT;
        }
        if (pLen)
        {
            len = *pLen;
            *pLen = pSapCtx->nStaWPARSnReqIeLength;
            VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO,
                       "%s: WPAIE len : %x", __func__, *pLen);
            if(pBuf)
            {
                if(len >= pSapCtx->nStaWPARSnReqIeLength)
                {
                    vos_mem_copy( pBuf, pSapCtx->pStaWpaRsnReqIE, pSapCtx->nStaWPARSnReqIeLength);
                    VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO,
                               "%s: WPAIE: %02x:%02x:%02x:%02x:%02x:%02x",
                               __func__,
                               pBuf[0], pBuf[1], pBuf[2],
                               pBuf[3], pBuf[4], pBuf[5]);
                    vosStatus = VOS_STATUS_SUCCESS;
                }
            }
        }
    }

    if( VOS_STATUS_E_FAILURE == vosStatus)
    {
        VOS_TRACE(VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                  "%s: Error unable to populate the RSNWPAIE",
                  __func__);
    }

    return vosStatus;

}

/*                                                                          
                               

             
                                                            

              
       

            

      
                                                 
                                                  

              
                                                                

                                

              
                                                                            */
VOS_STATUS
WLANSAP_Set_WpsIe
(
 v_PVOID_t pvosGCtx, tSap_WPSIE *pSap_WPSIe
)
{
    ptSapContext  pSapCtx = NULL;
    v_PVOID_t hHal = NULL;

    VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO,
            "%s, %d", __func__, __LINE__);

    if(VOS_STA_SAP_MODE == vos_get_conparam ( )) {
        pSapCtx = VOS_GET_SAP_CB(pvosGCtx);
        if ( NULL == pSapCtx )
        {
            VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                       "%s: Invalid SAP pointer from pvosGCtx", __func__);
            return VOS_STATUS_E_FAULT;
        }

        hHal = VOS_GET_HAL_CB(pSapCtx->pvosGCtx);
        if ( NULL == hHal ){
            VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                       "%s: Invalid HAL pointer from pvosGCtx", __func__);
            return VOS_STATUS_E_FAULT;
        }

        if ( sap_AcquireGlobalLock( pSapCtx ) == VOS_STATUS_SUCCESS )
        {
            if (pSap_WPSIe->sapWPSIECode == eSAP_WPS_BEACON_IE)
            {
                vos_mem_copy(&pSapCtx->APWPSIEs.SirWPSBeaconIE, &pSap_WPSIe->sapwpsie.sapWPSBeaconIE, sizeof(tSap_WPSBeaconIE));
            }
            else if (pSap_WPSIe->sapWPSIECode == eSAP_WPS_PROBE_RSP_IE)
            {
                vos_mem_copy(&pSapCtx->APWPSIEs.SirWPSProbeRspIE, &pSap_WPSIe->sapwpsie.sapWPSProbeRspIE, sizeof(tSap_WPSProbeRspIE));
            }
            else
            {
                sap_ReleaseGlobalLock( pSapCtx );
                return VOS_STATUS_E_FAULT;
            }
            sap_ReleaseGlobalLock( pSapCtx );
            return VOS_STATUS_SUCCESS;
        }
        else
            return VOS_STATUS_E_FAULT;
    }
    else
        return VOS_STATUS_E_FAULT;
}

/*                                                                          
                                 

             
                                                                

              
       

            

      
                                                 

              
                                                                

                                

              
                                                                            */
VOS_STATUS
WLANSAP_Update_WpsIe
(
 v_PVOID_t pvosGCtx
)
{
    VOS_STATUS vosStatus = VOS_STATUS_E_FAULT;
    ptSapContext  pSapCtx = NULL;
    eHalStatus halStatus = eHAL_STATUS_FAILURE;
    v_PVOID_t hHal = NULL;

    VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
            "%s, %d", __func__, __LINE__);

    if(VOS_STA_SAP_MODE == vos_get_conparam ( )){
        pSapCtx = VOS_GET_SAP_CB(pvosGCtx);
        if ( NULL == pSapCtx )
        {
            VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                       "%s: Invalid SAP pointer from pvosGCtx", __func__);
            return VOS_STATUS_E_FAULT;
        }

        hHal = VOS_GET_HAL_CB(pSapCtx->pvosGCtx);
        if ( NULL == hHal ){
            VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                       "%s: Invalid HAL pointer from pvosGCtx", __func__);
            return VOS_STATUS_E_FAULT;
        }

        halStatus = sme_RoamUpdateAPWPSIE( hHal, pSapCtx->sessionId, &pSapCtx->APWPSIEs);

        if(halStatus == eHAL_STATUS_SUCCESS) {
            vosStatus = VOS_STATUS_SUCCESS;
        } else
        {
            vosStatus = VOS_STATUS_E_FAULT;
        }

    }

    return vosStatus;
}

/*                                                                          
                                   

             
                                                                                 

              
       

            

      
                                                 

       
                                                                              

              
                                                                

                                

              
                                                                            */
VOS_STATUS
WLANSAP_Get_WPS_State
(
 v_PVOID_t pvosGCtx, v_BOOL_t *bWPSState
)
{
    ptSapContext  pSapCtx = NULL;
    v_PVOID_t hHal = NULL;

    VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_INFO,
        "%s, %d", __func__, __LINE__);

    if(VOS_STA_SAP_MODE == vos_get_conparam ( )){

        pSapCtx = VOS_GET_SAP_CB(pvosGCtx);
        if ( NULL == pSapCtx )
        {
            VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                       "%s: Invalid SAP pointer from pvosGCtx", __func__);
             return VOS_STATUS_E_FAULT;
        }

        hHal = VOS_GET_HAL_CB(pSapCtx->pvosGCtx);
        if ( NULL == hHal ){
            VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                       "%s: Invalid HAL pointer from pvosGCtx", __func__);
            return VOS_STATUS_E_FAULT;
        }

        if ( sap_AcquireGlobalLock(pSapCtx ) == VOS_STATUS_SUCCESS )
        {
            if(pSapCtx->APWPSIEs.SirWPSProbeRspIE.FieldPresent & SIR_WPS_PROBRSP_SELECTEDREGISTRA_PRESENT)
                *bWPSState = eANI_BOOLEAN_TRUE;
            else
                *bWPSState = eANI_BOOLEAN_FALSE;

            sap_ReleaseGlobalLock( pSapCtx  );

            return VOS_STATUS_SUCCESS;
        }
        else
            return VOS_STATUS_E_FAULT;
    }
    else
        return VOS_STATUS_E_FAULT;

}

VOS_STATUS
sap_AcquireGlobalLock
(
    ptSapContext  pSapCtx
)
{
    VOS_STATUS vosStatus = VOS_STATUS_E_FAULT;

    if( VOS_IS_STATUS_SUCCESS( vos_lock_acquire( &pSapCtx->SapGlobalLock) ) )
    {
            vosStatus = VOS_STATUS_SUCCESS;
    }

    return (vosStatus);
}

VOS_STATUS
sap_ReleaseGlobalLock
(
    ptSapContext  pSapCtx
)
{
    VOS_STATUS vosStatus = VOS_STATUS_E_FAULT;

    if( VOS_IS_STATUS_SUCCESS( vos_lock_release( &pSapCtx->SapGlobalLock) ) )
    {
        vosStatus = VOS_STATUS_SUCCESS;
    }

    return (vosStatus);
}

/*                                                                          
                                   

             
                                                                                                        

              
       

            

      
                                                         
                                             
                                           

              
                                                                

                                

              
                                                                            */
VOS_STATUS WLANSAP_Set_WPARSNIes(v_PVOID_t pvosGCtx, v_U8_t *pWPARSNIEs, v_U32_t WPARSNIEsLen)
{

    ptSapContext  pSapCtx = NULL;
    eHalStatus halStatus = eHAL_STATUS_FAILURE;
    v_PVOID_t hHal = NULL;

    if(VOS_STA_SAP_MODE == vos_get_conparam ( )){
        pSapCtx = VOS_GET_SAP_CB(pvosGCtx);
        if ( NULL == pSapCtx )
        {
            VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                       "%s: Invalid SAP pointer from pvosGCtx", __func__);
            return VOS_STATUS_E_FAULT;
        }

        hHal = VOS_GET_HAL_CB(pSapCtx->pvosGCtx);
        if ( NULL == hHal ){
            VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                       "%s: Invalid HAL pointer from pvosGCtx", __func__);
            return VOS_STATUS_E_FAULT;
        }

        pSapCtx->APWPARSNIEs.length = (tANI_U16)WPARSNIEsLen;
        vos_mem_copy(pSapCtx->APWPARSNIEs.rsnIEdata, pWPARSNIEs, WPARSNIEsLen);

        halStatus = sme_RoamUpdateAPWPARSNIEs( hHal, pSapCtx->sessionId, &pSapCtx->APWPARSNIEs);

        if(halStatus == eHAL_STATUS_SUCCESS) {
            return VOS_STATUS_SUCCESS;
        } else
        {
            return VOS_STATUS_E_FAULT;
        }
    }

    return VOS_STATUS_E_FAULT;
}

VOS_STATUS WLANSAP_GetStatistics(v_PVOID_t pvosGCtx, tSap_SoftapStats *statBuf, v_BOOL_t bReset)
{
    if (NULL == pvosGCtx)
    {
        return VOS_STATUS_E_FAULT;
    }

    return (WLANTL_GetSoftAPStatistics(pvosGCtx, statBuf, bReset));
}

/*                                                                          

                                

             
                                                                        

              
       

            

    
                                                     
                                                       
                                   

              
                                                                

                                

              
                                                                            */
VOS_STATUS WLANSAP_SendAction( v_PVOID_t pvosGCtx, const tANI_U8 *pBuf,
                               tANI_U32 len, tANI_U16 wait )
{
    ptSapContext  pSapCtx = NULL;
    v_PVOID_t hHal = NULL;
    eHalStatus halStatus = eHAL_STATUS_FAILURE;

    if( VOS_STA_SAP_MODE == vos_get_conparam ( ) )
    {
        pSapCtx = VOS_GET_SAP_CB( pvosGCtx );
        if (NULL == pSapCtx)
        {
            VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                       "%s: Invalid SAP pointer from pvosGCtx", __func__);
            return VOS_STATUS_E_FAULT;
        }
        hHal = VOS_GET_HAL_CB(pSapCtx->pvosGCtx);
        if( ( NULL == hHal ) || ( eSAP_TRUE != pSapCtx->isSapSessionOpen ) )
        {
            VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                       "%s: HAL pointer (%p) NULL OR SME session is not open (%d)",
                       __func__, hHal, pSapCtx->isSapSessionOpen );
            return VOS_STATUS_E_FAULT;
        }

        halStatus = sme_sendAction( hHal, pSapCtx->sessionId, pBuf, len, 0 , 0);

        if ( eHAL_STATUS_SUCCESS == halStatus )
        {
            return VOS_STATUS_SUCCESS;
        }
    }

    VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
               "Failed to Send Action Frame");

    return VOS_STATUS_E_FAULT;
}

/*                                                                          

                                     

             
                                                                            
                           

              
       

            

    
                                                     
                                                  
                                                                          
                                                                 
                                                              

              
                                                                

                                

              
                                                                            */
VOS_STATUS WLANSAP_RemainOnChannel( v_PVOID_t pvosGCtx,
                                    tANI_U8 channel, tANI_U32 duration,
                                    remainOnChanCallback callback,
                                    void *pContext )
{
    ptSapContext  pSapCtx = NULL;
    v_PVOID_t hHal = NULL;
    eHalStatus halStatus = eHAL_STATUS_FAILURE;

    if( VOS_STA_SAP_MODE == vos_get_conparam ( ) )
    {
        pSapCtx = VOS_GET_SAP_CB( pvosGCtx );
        if (NULL == pSapCtx)
        {
            VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                       "%s: Invalid SAP pointer from pvosGCtx", __func__);
            return VOS_STATUS_E_FAULT;
        }
        hHal = VOS_GET_HAL_CB(pSapCtx->pvosGCtx);
        if( ( NULL == hHal ) || ( eSAP_TRUE != pSapCtx->isSapSessionOpen ) )
        {
            VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                       "%s: HAL pointer (%p) NULL OR SME session is not open (%d)",
                       __func__, hHal, pSapCtx->isSapSessionOpen );
            return VOS_STATUS_E_FAULT;
        }

        halStatus = sme_RemainOnChannel( hHal, pSapCtx->sessionId,
                          channel, duration, callback, pContext, TRUE );

        if( eHAL_STATUS_SUCCESS == halStatus )
        {
            return VOS_STATUS_SUCCESS;
        }
    }

    VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
               "Failed to Set Remain on Channel");

    return VOS_STATUS_E_FAULT;
}

/*                                                                          

                                           

             
                                                       

              
       

            

    
                                                     

              
                                                                

                                

              
                                                                            */
VOS_STATUS WLANSAP_CancelRemainOnChannel( v_PVOID_t pvosGCtx )
{
    ptSapContext  pSapCtx = NULL;
    v_PVOID_t hHal = NULL;
    eHalStatus halStatus = eHAL_STATUS_FAILURE;

    if( VOS_STA_SAP_MODE == vos_get_conparam ( ) )
    {
        pSapCtx = VOS_GET_SAP_CB( pvosGCtx );
        if (NULL == pSapCtx)
        {
            VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                       "%s: Invalid SAP pointer from pvosGCtx", __func__);
            return VOS_STATUS_E_FAULT;
        }
        hHal = VOS_GET_HAL_CB(pSapCtx->pvosGCtx);
        if( ( NULL == hHal ) || ( eSAP_TRUE != pSapCtx->isSapSessionOpen ) )
        {
            VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                       "%s: HAL pointer (%p) NULL OR SME session is not open (%d)",
                       __func__, hHal, pSapCtx->isSapSessionOpen );
            return VOS_STATUS_E_FAULT;
        }

        halStatus = sme_CancelRemainOnChannel( hHal, pSapCtx->sessionId );

        if( eHAL_STATUS_SUCCESS == halStatus )
        {
            return VOS_STATUS_SUCCESS;
        }
    }

    VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                    "Failed to Cancel Remain on Channel");

    return VOS_STATUS_E_FAULT;
}

/*                                                                          

                                       

             
                                                                         
                                                                            

              
       

            

    
                                                     
                                                             
                                                                                
                                     

              
                                                                

                                

              
                                                                            */
VOS_STATUS WLANSAP_RegisterMgmtFrame( v_PVOID_t pvosGCtx, tANI_U16 frameType,
                                      tANI_U8* matchData, tANI_U16 matchLen )
{
    ptSapContext  pSapCtx = NULL;
    v_PVOID_t hHal = NULL;
    eHalStatus halStatus = eHAL_STATUS_FAILURE;

    if( VOS_STA_SAP_MODE == vos_get_conparam ( ) )
    {
        pSapCtx = VOS_GET_SAP_CB( pvosGCtx );
        if (NULL == pSapCtx)
        {
            VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                       "%s: Invalid SAP pointer from pvosGCtx", __func__);
            return VOS_STATUS_E_FAULT;
        }
        hHal = VOS_GET_HAL_CB(pSapCtx->pvosGCtx);
        if( ( NULL == hHal ) || ( eSAP_TRUE != pSapCtx->isSapSessionOpen ) )
        {
            VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                       "%s: HAL pointer (%p) NULL OR SME session is not open (%d)",
                       __func__, hHal, pSapCtx->isSapSessionOpen );
            return VOS_STATUS_E_FAULT;
        }

        halStatus = sme_RegisterMgmtFrame(hHal, pSapCtx->sessionId,
                          frameType, matchData, matchLen);

        if( eHAL_STATUS_SUCCESS == halStatus )
        {
            return VOS_STATUS_SUCCESS;
        }
    }

    VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                    "Failed to Register MGMT frame");

    return VOS_STATUS_E_FAULT;
}

/*                                                                          

                                         

             
                                                              

              
       

            

    
                                                     
                                                                
                                                                                
                                     

              
                                                                

                                

              
                                                                            */
VOS_STATUS WLANSAP_DeRegisterMgmtFrame( v_PVOID_t pvosGCtx, tANI_U16 frameType,
                                      tANI_U8* matchData, tANI_U16 matchLen )
{
    ptSapContext  pSapCtx = NULL;
    v_PVOID_t hHal = NULL;
    eHalStatus halStatus = eHAL_STATUS_FAILURE;

    if( VOS_STA_SAP_MODE == vos_get_conparam ( ) )
    {
        pSapCtx = VOS_GET_SAP_CB( pvosGCtx );
        if (NULL == pSapCtx)
        {
            VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                       "%s: Invalid SAP pointer from pvosGCtx", __func__);
            return VOS_STATUS_E_FAULT;
        }
        hHal = VOS_GET_HAL_CB(pSapCtx->pvosGCtx);
        if( ( NULL == hHal ) || ( eSAP_TRUE != pSapCtx->isSapSessionOpen ) )
        {
            VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                       "%s: HAL pointer (%p) NULL OR SME session is not open (%d)",
                       __func__, hHal, pSapCtx->isSapSessionOpen );
            return VOS_STATUS_E_FAULT;
        }

        halStatus = sme_DeregisterMgmtFrame( hHal, pSapCtx->sessionId,
                          frameType, matchData, matchLen );

        if( eHAL_STATUS_SUCCESS == halStatus )
        {
            return VOS_STATUS_SUCCESS;
        }
    }

    VOS_TRACE( VOS_MODULE_ID_SAP, VOS_TRACE_LEVEL_ERROR,
                    "Failed to Deregister MGMT frame");

    return VOS_STATUS_E_FAULT;
}

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
#include <vos_mq.h>
#include "vos_sched.h"
#include <vos_api.h>
#include "sirTypes.h"
#include "sirApi.h"
#include "sirMacProtDef.h"
#include "sme_Api.h"
#include "macInitApi.h"
#include "wlan_qct_sys.h"
#include "wlan_qct_tl.h"
#include "wlan_hdd_misc.h"
#include "i_vos_packet.h"
#include "vos_nvitem.h"
#include "wlan_qct_wda.h"
#include "wlan_hdd_main.h"
#include <linux/vmalloc.h>
#include "wlan_hdd_cfg80211.h"

#include "sapApi.h"



#ifdef WLAN_BTAMP_FEATURE
#include "bapApi.h"
#include "bapInternal.h"
#include "bap_hdd_main.h"
#endif //                  


/*                                                                           
                                         
                                                                           */
/*                                                                    
                                                                       
                                                                         
                                  */
#define VOS_WDA_TIMEOUT 15000

/*                                                        */
#define VOS_WDA_STOP_TIMEOUT WDA_STOP_TIMEOUT 

/*                                                                */
#define VOS_WDA_RESP_TIMEOUT WDA_STOP_TIMEOUT

/*                                                                           
                   
                                                                           */
static VosContextType  gVosContext;
static pVosContextType gpVosContext;

/*                                                                           
                      
                                                                           */
v_VOID_t vos_sys_probe_thread_cback ( v_VOID_t *pUserData );

v_VOID_t vos_core_return_msg(v_PVOID_t pVContext, pVosMsgWrapper pMsgWrapper);

v_VOID_t vos_fetch_tl_cfg_parms ( WLANTL_ConfigInfoType *pTLConfig, 
    hdd_config_t * pConfig );


/*                                                                           
  
                                                  
    
                                                                           
                                                                    
                
                                                                   
                                                                
                       
  
                                                                   
 
  
                                                                          
                              
              
                                                                        
          
                
  
                                                                           */
VOS_STATUS vos_preOpen ( v_CONTEXT_t *pVosContext )
{
   if ( pVosContext == NULL)
      return VOS_STATUS_E_FAILURE;

   /*                          */
   *pVosContext = NULL;
   gpVosContext = &gVosContext;

   if (NULL == gpVosContext)
   {
     /*                                          */
      VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL,
                 "%s: Failed to allocate VOS Context", __func__);
      VOS_ASSERT(0);
      return VOS_STATUS_E_RESOURCES;
   }

   vos_mem_zero(gpVosContext, sizeof(VosContextType));

   *pVosContext = gpVosContext;

   return VOS_STATUS_SUCCESS;

} /*              */

  
/*                                                                           
  
                                                    
    
                                                       
  
                                                                     
 
  
                                                
                  
          
                    
                 
                                                                           */
VOS_STATUS vos_preClose( v_CONTEXT_t *pVosContext )
{

   VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
                "%s: De-allocating the VOS Context", __func__);

   if (( pVosContext == NULL) || (*pVosContext == NULL)) 
   {
      VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                "%s: vOS Context is Null", __func__);
      return VOS_STATUS_E_FAILURE;
   }

   if (gpVosContext != *pVosContext)
   {
      VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                "%s: Context mismatch", __func__);
      return VOS_STATUS_E_FAILURE;
   }

   *pVosContext = gpVosContext = NULL;

   return VOS_STATUS_SUCCESS;

} /*               */

/*                                                                           
  
                                            
    
                                                     
                                 
  
                                                      
     
                                           
     
                                                                        
                                 
      
  
                                                              
 
  
                                                                          
                              
  
                                                                        
                                                    

          
                                                                        
          
                   
  
                                                                           */
VOS_STATUS vos_open( v_CONTEXT_t *pVosContext, v_SIZE_t hddContextSize )

{
   VOS_STATUS vStatus      = VOS_STATUS_SUCCESS;
   int iter                = 0;
   tSirRetStatus sirStatus = eSIR_SUCCESS;
   tMacOpenParameters macOpenParms;
   WLANTL_ConfigInfoType TLConfig;

   VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO_HIGH,
               "%s: Opening VOSS", __func__);

   if (NULL == gpVosContext)
   {
      VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL,
                    "%s: Trying to open VOSS without a PreOpen", __func__);
      VOS_ASSERT(0);
      return VOS_STATUS_E_FAILURE;
   }

   /*                             */
   vos_timer_module_init();

   /*                            */
   if (vos_event_init(&gpVosContext->ProbeEvent) != VOS_STATUS_SUCCESS)
   {
      VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL,
                    "%s: Unable to init probeEvent", __func__);
      VOS_ASSERT(0);
      return VOS_STATUS_E_FAILURE;
   }
   if (vos_event_init( &(gpVosContext->wdaCompleteEvent) ) != VOS_STATUS_SUCCESS )
   {
      VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL,
                  "%s: Unable to init wdaCompleteEvent", __func__);
      VOS_ASSERT(0);
    
      goto err_probe_event;
   }

   /*                                   */
   vStatus = vos_mq_init(&gpVosContext->freeVosMq);
   if (! VOS_IS_STATUS_SUCCESS(vStatus))
   {

      /*                                            */
      VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL,
                "%s: Failed to initialize VOS free message queue", __func__);
      VOS_ASSERT(0);
      goto err_wda_complete_event;
   }

   for (iter = 0; iter < VOS_CORE_MAX_MESSAGES; iter++)
   {
      (gpVosContext->aMsgWrappers[iter]).pVosMsg = 
         &(gpVosContext->aMsgBuffers[iter]); 
      INIT_LIST_HEAD(&gpVosContext->aMsgWrappers[iter].msgNode);
      vos_mq_put(&gpVosContext->freeVosMq, &(gpVosContext->aMsgWrappers[iter]));
   }

   /*                            */
   vStatus= vos_sched_open(gpVosContext, &gpVosContext->vosSched,
                           sizeof(VosSchedContext));

   if (!VOS_IS_STATUS_SUCCESS(vStatus))
   {
      /*                                            */
      VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL,
                "%s: Failed to open VOS Scheduler", __func__);
      VOS_ASSERT(0);
      goto err_msg_queue;
   }

   /*
                                                                            
                                                                            
   */

   /*                    */
   vos_mem_set(&macOpenParms, sizeof(macOpenParms), 0);
   /*                                                
                                        
   */
   macOpenParms.frameTransRequired = 1;
   macOpenParms.driverType         = eDRIVER_TYPE_PRODUCTION;
   vStatus = WDA_open( gpVosContext, gpVosContext->pHDDContext, &macOpenParms );

   if (!VOS_IS_STATUS_SUCCESS(vStatus))
   {
      /*                                            */
      VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL,
                "%s: Failed to open WDA module", __func__);
      VOS_ASSERT(0);
      goto err_sched_close;
   }

   /*                                           */
   vStatus = vos_packet_open( gpVosContext, &gpVosContext->vosPacket,
                              sizeof( vos_pkt_context_t ) );

   if ( !VOS_IS_STATUS_SUCCESS( vStatus ) )
   {
      /*                                            */
      VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL,
                "%s: Failed to open VOS Packet Module", __func__);
      VOS_ASSERT(0);
      goto err_wda_close;
   }

   /*                     */
   vStatus = sysOpen(gpVosContext);

   if (!VOS_IS_STATUS_SUCCESS(vStatus))
   {
      /*                                            */
      VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL,
                "%s: Failed to open SYS module", __func__);
      VOS_ASSERT(0);
      goto err_packet_close;
   }

#ifndef CONFIG_ENABLE_LINUX_REG
   /*                          */
   vStatus = vos_nv_open();
   if (!VOS_IS_STATUS_SUCCESS(vStatus))
   {
     //                                
     VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL,
                "%s: Failed to initialize the NV module", __func__);
     goto err_sys_close;
   }
#endif

   /*                                                                */
   
   /*                             */

   /*                                                
                                         */
   macOpenParms.frameTransRequired = 1;
   sirStatus = macOpen(&(gpVosContext->pMACContext), gpVosContext->pHDDContext,
                         &macOpenParms);
   
   if (eSIR_SUCCESS != sirStatus)
   {
     /*                                            */
     VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL,
               "%s: Failed to open MAC", __func__);
     VOS_ASSERT(0);
     goto err_nv_close;
   }

   /*                             */
   vStatus = sme_Open(gpVosContext->pMACContext);
   if (!VOS_IS_STATUS_SUCCESS(vStatus))
   {
     /*                                            */
     VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL,
               "%s: Failed to open SME", __func__);
     VOS_ASSERT(0);
     goto err_mac_close;
   }

   /*                                              */
   vos_fetch_tl_cfg_parms ( &TLConfig, 
       ((hdd_context_t*)(gpVosContext->pHDDContext))->cfg_ini);

   vStatus = WLANTL_Open(gpVosContext, &TLConfig);
   if (!VOS_IS_STATUS_SUCCESS(vStatus))
   {
     /*                                            */
     VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL,
               "%s: Failed to open TL", __func__);
     VOS_ASSERT(0);
     goto err_sme_close;
   }

   VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO_HIGH,
               "%s: VOSS successfully Opened", __func__);

   *pVosContext = gpVosContext;

   return VOS_STATUS_SUCCESS;


err_sme_close:
   sme_Close(gpVosContext->pMACContext);

err_mac_close:
   macClose(gpVosContext->pMACContext);

err_nv_close:

#ifndef CONFIG_ENABLE_LINUX_REG
   vos_nv_close();

err_sys_close:
#endif

   sysClose(gpVosContext);

err_packet_close:
   vos_packet_close( gpVosContext );

err_wda_close:
   WDA_close(gpVosContext);

err_sched_close:
   vos_sched_close(gpVosContext);


err_msg_queue:
   vos_mq_deinit(&gpVosContext->freeVosMq);

err_wda_complete_event:
   vos_event_destroy( &gpVosContext->wdaCompleteEvent );

err_probe_event:
   vos_event_destroy(&gpVosContext->ProbeEvent);

   return VOS_STATUS_E_FAILURE;

} /*            */

/*                                                                           

                         

                                                 
            
                

                                      


                                      


                                                                         
                              

                                                                       
                                                    


                                                                     

               

                                                                           */
VOS_STATUS vos_preStart( v_CONTEXT_t vosContext )
{
   VOS_STATUS vStatus          = VOS_STATUS_SUCCESS;
   pVosContextType pVosContext = (pVosContextType)vosContext;
   
   VOS_TRACE(VOS_MODULE_ID_SYS, VOS_TRACE_LEVEL_INFO,
             "vos prestart");

   if (gpVosContext != pVosContext)
   {
      VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                "%s: Context mismatch", __func__);
      VOS_ASSERT(0);
      return VOS_STATUS_E_INVAL;
   }

   if (pVosContext->pMACContext == NULL)
   {
       VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
            "%s: MAC NULL context", __func__);
       VOS_ASSERT(0);
       return VOS_STATUS_E_INVAL;
   }

   if (pVosContext->pWDAContext == NULL)
   {
       VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
          "%s: WDA NULL context", __func__);
       VOS_ASSERT(0);
       return VOS_STATUS_E_INVAL;
   }

   /*                  */
   vStatus = macPreStart(gpVosContext->pMACContext);
   if ( !VOS_IS_STATUS_SUCCESS(vStatus) )
   {
      VOS_TRACE(VOS_MODULE_ID_SYS, VOS_TRACE_LEVEL_FATAL,
             "Failed at macPreStart ");
      return VOS_STATUS_E_FAILURE;
   }

   /*               */
   ccmStart(gpVosContext->pMACContext);

   /*                      */
   vos_event_reset(&gpVosContext->wdaCompleteEvent);   
    

   /*                  */
   vStatus = WDA_preStart(gpVosContext);
   if (!VOS_IS_STATUS_SUCCESS(vStatus))
   {
      VOS_TRACE(VOS_MODULE_ID_SYS, VOS_TRACE_LEVEL_FATAL,
             "Failed to WDA prestart");
      macStop(gpVosContext->pMACContext, HAL_STOP_TYPE_SYS_DEEP_SLEEP);
      ccmStop(gpVosContext->pMACContext);
      VOS_ASSERT(0);
      return VOS_STATUS_E_FAILURE;
   }

   /*                                     */
   vStatus = vos_wait_single_event( &gpVosContext->wdaCompleteEvent,
                                    VOS_WDA_TIMEOUT );
   if ( vStatus != VOS_STATUS_SUCCESS )
   {
      if ( vStatus == VOS_STATUS_E_TIMEOUT )
      {
         VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
          "%s: Timeout occurred before WDA complete\n", __func__);
      }
      else
      {
         VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
           "%s: WDA_preStart reporting other error", __func__);
      }
      VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
           "%s: Test MC thread by posting a probe message to SYS", __func__);
      wlan_sys_probe();

      macStop(gpVosContext->pMACContext, HAL_STOP_TYPE_SYS_DEEP_SLEEP);
      ccmStop(gpVosContext->pMACContext);
      VOS_ASSERT( 0 );
      return VOS_STATUS_E_FAILURE;
   }

   return VOS_STATUS_SUCCESS;
}

/*                                                                           
  
                                                  
    
                                                                       
            
                                         
      
                            
      
           
      
          
      
                                      
  
  
                                      
 
  
                                                                          
                              
  
                                                                        
                                                    

          
                                                                        
          
                    
                
  
                                                                           */
VOS_STATUS vos_start( v_CONTEXT_t vosContext )
{
  VOS_STATUS vStatus          = VOS_STATUS_SUCCESS;
  tSirRetStatus sirStatus     = eSIR_SUCCESS;
  pVosContextType pVosContext = (pVosContextType)vosContext;
  tHalMacStartParameters halStartParams;

  VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
            "%s: Starting Libra SW", __func__);

  /*                                         */
  if (gpVosContext != pVosContext)
  {
     VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
           "%s: mismatch in context", __func__);
     return VOS_STATUS_E_FAILURE;
  }

  if (( pVosContext->pWDAContext == NULL) || ( pVosContext->pMACContext == NULL)
     || ( pVosContext->pTLContext == NULL))
  {
     if (pVosContext->pWDAContext == NULL)
        VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, 
            "%s: WDA NULL context", __func__);
     else if (pVosContext->pMACContext == NULL)
        VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, 
            "%s: MAC NULL context", __func__);
     else
        VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, 
            "%s: TL NULL context", __func__);
     
     return VOS_STATUS_E_FAILURE;
  }

  /*                                                              
                                                                         */

  /*                             */

  vos_event_reset( &(gpVosContext->wdaCompleteEvent) );

  vStatus = WDA_NVDownload_Start(pVosContext);

  if ( vStatus != VOS_STATUS_SUCCESS )
  {
     VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                 "%s: Failed to start NV Download", __func__);
     return VOS_STATUS_E_FAILURE;
  }

  vStatus = vos_wait_single_event( &(gpVosContext->wdaCompleteEvent),
                                   VOS_WDA_TIMEOUT );

  if ( vStatus != VOS_STATUS_SUCCESS )
  {
     if ( vStatus == VOS_STATUS_E_TIMEOUT )
     {
        VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
         "%s: Timeout occurred before WDA_NVDownload_start complete", __func__);
     }
     else
     {
        VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
         "%s: WDA_NVDownload_start reporting other error", __func__);
     }
     VOS_ASSERT(0);
     vos_event_reset( &(gpVosContext->wdaCompleteEvent) );
     if (vos_is_logp_in_progress(VOS_MODULE_ID_VOSS, NULL))
     {
         VOS_BUG(0);
     }
     WDA_setNeedShutdown(vosContext);
     return VOS_STATUS_E_FAILURE;
  }

  VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
            "%s: WDA_NVDownload_start correctly started", __func__);

  /*               */
  vStatus = WDA_start(pVosContext);
  if ( vStatus != VOS_STATUS_SUCCESS )
  {
     VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                 "%s: Failed to start WDA", __func__);
     return VOS_STATUS_E_FAILURE;
  }
  VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
            "%s: WDA correctly started", __func__);

  /*               */
  vos_mem_zero((v_PVOID_t)&halStartParams, sizeof(tHalMacStartParameters));

  /*               */
  sirStatus = macStart(pVosContext->pMACContext,(v_PVOID_t)&halStartParams);

  if (eSIR_SUCCESS != sirStatus)
  {
    VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL,
              "%s: Failed to start MAC", __func__);
    goto err_wda_stop;
  }
   
  VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
            "%s: MAC correctly started", __func__);

  /*           */
  vStatus = sme_Start(pVosContext->pMACContext);

  if (!VOS_IS_STATUS_SUCCESS(vStatus))
  {
    VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL,
               "%s: Failed to start SME", __func__);
    goto err_mac_stop;
  }

  VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
            "%s: SME correctly started", __func__);

  /*           */
  vStatus = WLANTL_Start(pVosContext);
  if (!VOS_IS_STATUS_SUCCESS(vStatus))
  {
    VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL,
               "%s: Failed to start TL", __func__);
    goto err_sme_stop;
  }

  VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
            "TL correctly started");
  VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
            "%s: VOSS Start is successful!!", __func__);

  return VOS_STATUS_SUCCESS;


err_sme_stop:
  sme_Stop(pVosContext->pMACContext, HAL_STOP_TYPE_SYS_RESET);
    
err_mac_stop:
  macStop( pVosContext->pMACContext, HAL_STOP_TYPE_SYS_RESET );

err_wda_stop:   
  vos_event_reset( &(gpVosContext->wdaCompleteEvent) );
  vStatus = WDA_stop( pVosContext, HAL_STOP_TYPE_RF_KILL);
  if (!VOS_IS_STATUS_SUCCESS(vStatus))
  {
     VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
         "%s: Failed to stop WDA", __func__);
     VOS_ASSERT( VOS_IS_STATUS_SUCCESS( vStatus ) );
     WDA_setNeedShutdown(vosContext);
  }
  else
  {
    vStatus = vos_wait_single_event( &(gpVosContext->wdaCompleteEvent),
                                     VOS_WDA_TIMEOUT );
    if( vStatus != VOS_STATUS_SUCCESS )
    {
       if( vStatus == VOS_STATUS_E_TIMEOUT )
       {
          VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL,
           "%s: Timeout occurred before WDA_stop complete", __func__);

       }
       else
       {
          VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL,
           "%s: WDA_stop reporting other error", __func__);
       }
       VOS_ASSERT( 0 );
       WDA_setNeedShutdown(vosContext);
    }
  }

  return VOS_STATUS_E_FAILURE;
   
} /*             */


/*                   */
VOS_STATUS vos_stop( v_CONTEXT_t vosContext )
{
  VOS_STATUS vosStatus;

  /*                                                                  
                                                                               */
  vos_event_reset( &(gpVosContext->wdaCompleteEvent) );

  vosStatus = WDA_stop( vosContext, HAL_STOP_TYPE_RF_KILL );

  if (!VOS_IS_STATUS_SUCCESS(vosStatus))
  {
     VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
         "%s: Failed to stop WDA", __func__);
     VOS_ASSERT( VOS_IS_STATUS_SUCCESS( vosStatus ) );
     WDA_setNeedShutdown(vosContext);
  }
  else
  {
    vosStatus = vos_wait_single_event( &(gpVosContext->wdaCompleteEvent),
                                       VOS_WDA_STOP_TIMEOUT );

    if ( vosStatus != VOS_STATUS_SUCCESS )
    {
       if ( vosStatus == VOS_STATUS_E_TIMEOUT )
       {
          VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
           "%s: Timeout occurred before WDA complete", __func__);
       }
       else
       {
          VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
           "%s: WDA_stop reporting other error", __func__ );
       }
       VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
           "%s: Test MC thread by posting a probe message to SYS", __func__);
       wlan_sys_probe();
       WDA_setNeedShutdown(vosContext);
    }
  }

  /*                                */
  vosStatus = sysStop( vosContext);
  if (!VOS_IS_STATUS_SUCCESS(vosStatus))
  {
     VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
         "%s: Failed to stop SYS", __func__);
     VOS_ASSERT( VOS_IS_STATUS_SUCCESS( vosStatus ) );
  }

  vosStatus = WLANTL_Stop( vosContext );
  if (!VOS_IS_STATUS_SUCCESS(vosStatus))
  {
     VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
         "%s: Failed to stop TL", __func__);
     VOS_ASSERT( VOS_IS_STATUS_SUCCESS( vosStatus ) );
  }


  return VOS_STATUS_SUCCESS;
}


/*                    */
VOS_STATUS vos_close( v_CONTEXT_t vosContext )
{
  VOS_STATUS vosStatus;

#ifdef WLAN_BTAMP_FEATURE
  vosStatus = WLANBAP_Close(vosContext);
  if (!VOS_IS_STATUS_SUCCESS(vosStatus))
  {
     VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
         "%s: Failed to close BAP", __func__);
     VOS_ASSERT( VOS_IS_STATUS_SUCCESS( vosStatus ) );
  }
#endif //                   


  vosStatus = WLANTL_Close(vosContext);
  if (!VOS_IS_STATUS_SUCCESS(vosStatus))
  {
     VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
         "%s: Failed to close TL", __func__);
     VOS_ASSERT( VOS_IS_STATUS_SUCCESS( vosStatus ) );
  }
   
  vosStatus = sme_Close( ((pVosContextType)vosContext)->pMACContext);
  if (!VOS_IS_STATUS_SUCCESS(vosStatus))
  {
     VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
         "%s: Failed to close SME", __func__);
     VOS_ASSERT( VOS_IS_STATUS_SUCCESS( vosStatus ) );
  }

  vosStatus = macClose( ((pVosContextType)vosContext)->pMACContext);
  if (!VOS_IS_STATUS_SUCCESS(vosStatus))
  {
     VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
         "%s: Failed to close MAC", __func__);
     VOS_ASSERT( VOS_IS_STATUS_SUCCESS( vosStatus ) );
  }

  ((pVosContextType)vosContext)->pMACContext = NULL;

#ifndef CONFIG_ENABLE_LINUX_REG
  vosStatus = vos_nv_close();
  if (!VOS_IS_STATUS_SUCCESS(vosStatus))
  {
     VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
         "%s: Failed to close NV", __func__);
     VOS_ASSERT( VOS_IS_STATUS_SUCCESS( vosStatus ) );
  }
#endif

  vosStatus = sysClose( vosContext );
  if (!VOS_IS_STATUS_SUCCESS(vosStatus))
  {
     VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
         "%s: Failed to close SYS", __func__);
     VOS_ASSERT( VOS_IS_STATUS_SUCCESS( vosStatus ) );
  }

  if ( TRUE == WDA_needShutdown(vosContext ))
  {
     /*                                                          */
     vosStatus = WDA_shutdown( vosContext, VOS_TRUE );
     if (VOS_IS_STATUS_SUCCESS( vosStatus ) )
     {
        hdd_set_ssr_required( HDD_SSR_REQUIRED );
     }
     else
     {
        VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL,
                               "%s: Failed to shutdown WDA", __func__ );
        VOS_ASSERT( VOS_IS_STATUS_SUCCESS( vosStatus ) );
     }
  } 
  else 
  {
     vosStatus = WDA_close( vosContext );
     if (!VOS_IS_STATUS_SUCCESS(vosStatus))
     {
        VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
            "%s: Failed to close WDA", __func__);
        VOS_ASSERT( VOS_IS_STATUS_SUCCESS( vosStatus ) );
     }
  }
  
  /*                                                             */
  vosStatus = vos_packet_close( vosContext );
  if (!VOS_IS_STATUS_SUCCESS(vosStatus))
  {
     VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
         "%s: Failed to close VOSS Packet", __func__);
     VOS_ASSERT( VOS_IS_STATUS_SUCCESS( vosStatus ) );
  }


  vos_mq_deinit(&((pVosContextType)vosContext)->freeVosMq);

  vosStatus = vos_event_destroy(&gpVosContext->wdaCompleteEvent);
  if (!VOS_IS_STATUS_SUCCESS(vosStatus))
  {
     VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
         "%s: failed to destroy wdaCompleteEvent", __func__);
     VOS_ASSERT( VOS_IS_STATUS_SUCCESS( vosStatus ) );
  }

  vosStatus = vos_event_destroy(&gpVosContext->ProbeEvent);
  if (!VOS_IS_STATUS_SUCCESS(vosStatus))
  {
     VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
         "%s: failed to destroy ProbeEvent", __func__);
     VOS_ASSERT( VOS_IS_STATUS_SUCCESS( vosStatus ) );
  }

  return VOS_STATUS_SUCCESS;
}
                  

/*                                                                            
  
                                                  
  
                                                                       
                                                                         
                                                             

                                                
  
                                                                            
                      
                                             
  
                                                                       
                      
              
                                                                            */
v_VOID_t* vos_get_context( VOS_MODULE_ID moduleId, 
                           v_CONTEXT_t pVosContext )
{
  v_PVOID_t pModContext = NULL;

  if (pVosContext == NULL)
  {
    VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
        "%s: vos context pointer is null", __func__);
    return NULL;
  }

  if (gpVosContext != pVosContext)
  {
    VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, 
        "%s: pVosContext != gpVosContext", __func__);
    return NULL;
  }

  switch(moduleId)
  {
    case VOS_MODULE_ID_TL:  
    {
      pModContext = gpVosContext->pTLContext;
      break;
    }

#ifdef WLAN_BTAMP_FEATURE
    case VOS_MODULE_ID_BAP:
    {
        pModContext = gpVosContext->pBAPContext;
        break;
    }    
#endif //                  

    case VOS_MODULE_ID_SAP:
    {
      pModContext = gpVosContext->pSAPContext;
      break;
    }

    case VOS_MODULE_ID_HDD_SOFTAP:
    {
      pModContext = gpVosContext->pHDDSoftAPContext;
      break;
    }

    case VOS_MODULE_ID_HDD:
    {
      pModContext = gpVosContext->pHDDContext;
      break;
    }

    case VOS_MODULE_ID_SME:
    case VOS_MODULE_ID_PE:
    case VOS_MODULE_ID_PMC:
    {
      /* 
                                                           
      */
      pModContext = gpVosContext->pMACContext;
      break;
    }

    case VOS_MODULE_ID_WDA:
    {
      /*                */
      pModContext = gpVosContext->pWDAContext;
      break;
    }

    case VOS_MODULE_ID_VOSS:
    {
      /*                           */
      pModContext = gpVosContext;
      break;
    }

    default:
    {
      VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,"%s: Module ID %i "
          "does not have its context maintained by VOSS", __func__, moduleId);
      VOS_ASSERT(0);
      return NULL;
    }
  }

  if (pModContext == NULL )
  {
    VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,"%s: Module ID %i "
          "context is Null", __func__, moduleId);
  }

  return pModContext;

} /*                  */


/*                                                                            
  
                                                           
  
                                                                       
                             
  
                                                         
  
                                                                    
                       
                      
                                              
  
                                                                         
              
                                                                            */
v_CONTEXT_t vos_get_global_context( VOS_MODULE_ID moduleId, 
                                    v_VOID_t *moduleContext )
{
  if (gpVosContext == NULL)
  {
    VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, 
        "%s: global voss context is NULL", __func__);
  }

  return gpVosContext;

} /*                          */


v_U8_t vos_is_logp_in_progress(VOS_MODULE_ID moduleId, v_VOID_t *moduleContext)
{
  if (gpVosContext == NULL)
  {
    VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, 
        "%s: global voss context is NULL", __func__);
    return 1;
  }

   return gpVosContext->isLogpInProgress;
}

void vos_set_logp_in_progress(VOS_MODULE_ID moduleId, v_U8_t value)
{
  if (gpVosContext == NULL)
  {
    VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, 
        "%s: global voss context is NULL", __func__);
    return;
  }

   gpVosContext->isLogpInProgress = value;
}

v_U8_t vos_is_load_unload_in_progress(VOS_MODULE_ID moduleId, v_VOID_t *moduleContext)
{
  if (gpVosContext == NULL)
  {
    VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, 
        "%s: global voss context is NULL", __func__);
    return 0; 
  }

   return gpVosContext->isLoadUnloadInProgress;
}

void vos_set_load_unload_in_progress(VOS_MODULE_ID moduleId, v_U8_t value)
{
  if (gpVosContext == NULL)
  {
    VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, 
        "%s: global voss context is NULL", __func__);
    return;
  }

   gpVosContext->isLoadUnloadInProgress = value;
}

v_U8_t vos_is_reinit_in_progress(VOS_MODULE_ID moduleId, v_VOID_t *moduleContext)
{
  if (gpVosContext == NULL)
  {
    VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
        "%s: global voss context is NULL", __func__);
    return 1;
  }

   return gpVosContext->isReInitInProgress;
}

void vos_set_reinit_in_progress(VOS_MODULE_ID moduleId, v_U8_t value)
{
  if (gpVosContext == NULL)
  {
    VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
        "%s: global voss context is NULL", __func__);
    return;
  }

   gpVosContext->isReInitInProgress = value;
}


/*                                                                            
  
                                                                                
  
                                                                      
                       
  
                                                        
  
                                                                        
  
                                                                        
                                                                     
                                                                  
                                                     
  
                                                             
                      
                                                                        
                                                                    
                                               
                                                                  
                                                              
                                                    

                                                                     
                                                                    

                                                                           
                                                                         
                                                           
            
                                                                       
                                     
              
                                           
  
                                                                            */
VOS_STATUS vos_alloc_context( v_VOID_t *pVosContext, VOS_MODULE_ID moduleID, 
                              v_VOID_t **ppModuleContext, v_SIZE_t size )
{
  v_VOID_t ** pGpModContext = NULL;

  if ( pVosContext == NULL) {
    VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, 
        "%s: vos context is null", __func__);
    return VOS_STATUS_E_FAILURE;
  }

  if (( gpVosContext != pVosContext) || ( ppModuleContext == NULL)) {
    VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, 
        "%s: context mismatch or null param passed", __func__);
    return VOS_STATUS_E_FAILURE;
  }

  switch(moduleID)
  {
    case VOS_MODULE_ID_TL:  
    {
      pGpModContext = &(gpVosContext->pTLContext); 
      break;
    }

#ifdef WLAN_BTAMP_FEATURE
    case VOS_MODULE_ID_BAP:
    {
        pGpModContext = &(gpVosContext->pBAPContext);
        break;
    }    
#endif //                  

    case VOS_MODULE_ID_SAP:
    {
      pGpModContext = &(gpVosContext->pSAPContext);
      break;
    }

    case VOS_MODULE_ID_WDA:
    {
      pGpModContext = &(gpVosContext->pWDAContext);
      break;
    }
    case VOS_MODULE_ID_SME:
    case VOS_MODULE_ID_PE:
    case VOS_MODULE_ID_PMC:
    case VOS_MODULE_ID_HDD:
    case VOS_MODULE_ID_HDD_SOFTAP:
    default:
    {     
      VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, "%s: Module ID %i "
          "does not have its context allocated by VOSS", __func__, moduleID);
      VOS_ASSERT(0);
      return VOS_STATUS_E_INVAL;
    }
  }

  if ( NULL != *pGpModContext)
  {
    /*
                                          
                                
    */
    VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
               "%s: Module ID %i context has already been allocated",
                __func__, moduleID);
    return VOS_STATUS_E_EXISTS;
  }
  
  /*
                                                
  */
  
  *ppModuleContext = kmalloc(size, GFP_KERNEL);

  
  if ( *ppModuleContext == NULL)
  {
    VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,"%s: Failed to "
        "allocate Context for module ID %i", __func__, moduleID);
    VOS_ASSERT(0);
    return VOS_STATUS_E_NOMEM;
  }
  
  if (moduleID==VOS_MODULE_ID_TL)
  {
     vos_mem_zero(*ppModuleContext, size);
  }

  *pGpModContext = *ppModuleContext;

  return VOS_STATUS_SUCCESS;

} /*                     */


/*                                                                            
  
                                                                     
                                                  
  
                                                                  
                       
  
                                                        
  
                                                                  
  
                                                                      
                      
                                                                        
                                                                   
                      
            
                                                                       
                           
                                 
                                                                     
                                                                 
            
                                                                   
                                                                      
                                            
              
                                     
  
                                                                            */
VOS_STATUS vos_free_context( v_VOID_t *pVosContext, VOS_MODULE_ID moduleID,
                             v_VOID_t *pModuleContext )
{
  v_VOID_t ** pGpModContext = NULL;

  if (( pVosContext == NULL) || ( gpVosContext != pVosContext) ||
      ( pModuleContext == NULL))
  {
    VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
        "%s: Null params or context mismatch", __func__);
    return VOS_STATUS_E_FAILURE;
  }
  

  switch(moduleID)
  {
    case VOS_MODULE_ID_TL:  
    {
      pGpModContext = &(gpVosContext->pTLContext); 
      break;
    }

#ifdef WLAN_BTAMP_FEATURE
    case VOS_MODULE_ID_BAP:
    {
        pGpModContext = &(gpVosContext->pBAPContext);
        break;
    }
#endif //                  
 
    case VOS_MODULE_ID_SAP:
    {
      pGpModContext = &(gpVosContext->pSAPContext); 
      break;
    }

    case VOS_MODULE_ID_WDA:
    {
      pGpModContext = &(gpVosContext->pWDAContext);
      break;
    }
    case VOS_MODULE_ID_HDD:
    case VOS_MODULE_ID_SME:
    case VOS_MODULE_ID_PE:
    case VOS_MODULE_ID_PMC:
    case VOS_MODULE_ID_HDD_SOFTAP:
    default:
    {     
      VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, "%s: Module ID %i "
          "does not have its context allocated by VOSS", __func__, moduleID);
      VOS_ASSERT(0);
      return VOS_STATUS_E_INVAL;
    }
  }

  if ( NULL == *pGpModContext)
  {
    /*
                                                       
    */
    VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,"%s: Module ID %i "
        "context has not been allocated or freed already", __func__,moduleID);
    return VOS_STATUS_E_FAILURE;
  }
  
  if (*pGpModContext != pModuleContext)
  {
    VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, 
        "%s: pGpModContext != pModuleContext", __func__);
    return VOS_STATUS_E_FAILURE;
  } 
  
  if(pModuleContext != NULL)
      kfree(pModuleContext);

  *pGpModContext = NULL;

  return VOS_STATUS_SUCCESS;

} /*                    */
                                                 

/*                                                                            
  
                                                                  

                                                                              
                                                
  
      
            
           
            
           
        
  
                                                                         
                        
         
                                                                           
                                                                           
                                                                         
                                                                              
                                                         
  
                                                                       
                               
          
                                                                          
                                            
          
                                                                   
          
                                                                       
                             

     
  
                                                                            */
VOS_STATUS vos_mq_post_message( VOS_MQ_ID msgQueueId, vos_msg_t *pMsg )
{
  pVosMqType      pTargetMq   = NULL;
  pVosMsgWrapper  pMsgWrapper = NULL;

  if ((gpVosContext == NULL) || (pMsg == NULL))
  {
    VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
        "%s: Null params or global vos context is null", __func__);
    VOS_ASSERT(0);
    return VOS_STATUS_E_FAILURE;
  }

  switch (msgQueueId)
  {
    //                                             
    case  VOS_MQ_ID_SME: 
    {
       pTargetMq = &(gpVosContext->vosSched.smeMcMq);
       break;
    }

    //                                            
    case VOS_MQ_ID_PE:  
    {
       pTargetMq = &(gpVosContext->vosSched.peMcMq);
       break;
    }

    //                                             
    case VOS_MQ_ID_WDA: 
    {
       pTargetMq = &(gpVosContext->vosSched.wdaMcMq);
       break;
    }

    //                                             
    case VOS_MQ_ID_WDI:
    {
       pTargetMq = &(gpVosContext->vosSched.wdiMcMq);
       break;
    }

    //                                            
    case VOS_MQ_ID_TL: 
    {
       pTargetMq = &(gpVosContext->vosSched.tlMcMq);
       break;
    }

    //                                                        
    case VOS_MQ_ID_SYS:
    {
       pTargetMq = &(gpVosContext->vosSched.sysMcMq);
       break;
    }

    default:

    VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
              ("%s: Trying to queue msg into unknown MC Msg queue ID %d"),
              __func__, msgQueueId);

    return VOS_STATUS_E_FAILURE;
  }

  VOS_ASSERT(NULL !=pTargetMq);
  if (pTargetMq == NULL)
  {
     VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, 
         "%s: pTargetMq == NULL", __func__);
     return VOS_STATUS_E_FAILURE;
  } 

  /*
                                   
  */
  pMsgWrapper = vos_mq_get(&gpVosContext->freeVosMq);

  if (NULL == pMsgWrapper)
  {
    VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
              "%s: VOS Core run out of message wrapper", __func__);

    return VOS_STATUS_E_RESOURCES;
  }
  
  /*
                         
  */
  vos_mem_copy( (v_VOID_t*)pMsgWrapper->pVosMsg, 
                (v_VOID_t*)pMsg, sizeof(vos_msg_t));

  vos_mq_put(pTargetMq, pMsgWrapper);

  set_bit(MC_POST_EVENT_MASK, &gpVosContext->vosSched.mcEventFlag);
  wake_up_interruptible(&gpVosContext->vosSched.mcWaitQueue);

  return VOS_STATUS_SUCCESS;

} /*                      */


/*                                                                            
  
                                                                             

                                                                           
                                                                           
                         
  
      
           
                
       
  
                                                                         
                        
         
                                                                                
                                                                           
                                                                              
                                                                             
                                                                
  
                                                                       
                               
          
                                                                          
                                            
          
                                                                   
          
                                                                       
                             

     
  
                                                                            */
VOS_STATUS vos_tx_mq_serialize( VOS_MQ_ID msgQueueId, vos_msg_t *pMsg )
{
  pVosMqType      pTargetMq   = NULL;
  pVosMsgWrapper  pMsgWrapper = NULL;

  if ((gpVosContext == NULL) || (pMsg == NULL))
  {
    VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
        "%s: Null params or global vos context is null", __func__);
    VOS_ASSERT(0);
    return VOS_STATUS_E_FAILURE;
  }

  switch (msgQueueId)
  {
    //                                             
    case  VOS_MQ_ID_TL: 
    {
       pTargetMq = &(gpVosContext->vosSched.tlTxMq);
       break;
    }

    //                                             
    case VOS_MQ_ID_WDI:  
    {
       pTargetMq = &(gpVosContext->vosSched.wdiTxMq);
       break;
    }
    
    //                                                        
    case VOS_MQ_ID_SYS:
    {
       pTargetMq = &(gpVosContext->vosSched.sysTxMq);
       break;
    }

    default:

    VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
              "%s: Trying to queue msg into unknown Tx Msg queue ID %d",
               __func__, msgQueueId);

    return VOS_STATUS_E_FAILURE;
  }

  if (pTargetMq == NULL)
  {
     VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, 
         "%s: pTargetMq == NULL", __func__);
     return VOS_STATUS_E_FAILURE;
  } 
    

  /*
                                   
  */
  pMsgWrapper = vos_mq_get(&gpVosContext->freeVosMq);

  if (NULL == pMsgWrapper)
  {
    VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL,
              "%s: VOS Core run out of message wrapper", __func__);

    return VOS_STATUS_E_RESOURCES;
  }

  /*
                         
  */
  vos_mem_copy( (v_VOID_t*)pMsgWrapper->pVosMsg, 
                (v_VOID_t*)pMsg, sizeof(vos_msg_t));

  vos_mq_put(pTargetMq, pMsgWrapper);

  set_bit(TX_POST_EVENT_MASK, &gpVosContext->vosSched.txEventFlag);
  wake_up_interruptible(&gpVosContext->vosSched.txWaitQueue);

  return VOS_STATUS_SUCCESS;

} /*                      */

/*                                                                            

                                                                             

                                                                          
                                                                          
                         

      
           
            
       

                                                                         
                        

                                                                               
                                                                           
                                                                             
                                                                            
                                                                

                                                                       
                               

                                                                         
                                            

                                                              

                                                                       
                             

     

                                                                            */

VOS_STATUS vos_rx_mq_serialize( VOS_MQ_ID msgQueueId, vos_msg_t *pMsg )
{
  pVosMqType      pTargetMq   = NULL;
  pVosMsgWrapper  pMsgWrapper = NULL;
  if ((gpVosContext == NULL) || (pMsg == NULL))
  {
    VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
        "%s: Null params or global vos context is null", __func__);
    VOS_ASSERT(0);
    return VOS_STATUS_E_FAILURE;
  }

  switch (msgQueueId)
  {

    case VOS_MQ_ID_SYS:
    {
       pTargetMq = &(gpVosContext->vosSched.sysRxMq);
       break;
    }

    //                                             
    case VOS_MQ_ID_WDI:
    {
       pTargetMq = &(gpVosContext->vosSched.wdiRxMq);
       break;
    }

    default:

    VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
              "%s: Trying to queue msg into unknown Rx Msg queue ID %d",
               __func__, msgQueueId);

    return VOS_STATUS_E_FAILURE;
  }

  if (pTargetMq == NULL)
  {
     VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
         "%s: pTargetMq == NULL", __func__);
     return VOS_STATUS_E_FAILURE;
  }


  /*
                                   
  */
  pMsgWrapper = vos_mq_get(&gpVosContext->freeVosMq);

  if (NULL == pMsgWrapper)
  {
    VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
              "%s: VOS Core run out of message wrapper", __func__);

    return VOS_STATUS_E_RESOURCES;
  }

  /*
                         
  */
  vos_mem_copy( (v_VOID_t*)pMsgWrapper->pVosMsg,
                (v_VOID_t*)pMsg, sizeof(vos_msg_t));

  vos_mq_put(pTargetMq, pMsgWrapper);

  set_bit(RX_POST_EVENT_MASK, &gpVosContext->vosSched.rxEventFlag);
  wake_up_interruptible(&gpVosContext->vosSched.rxWaitQueue);

  return VOS_STATUS_SUCCESS;

} /*                      */

v_VOID_t 
vos_sys_probe_thread_cback 
( 
  v_VOID_t *pUserData 
)
{
  if (gpVosContext != pUserData)
  {
     VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, 
         "%s: gpVosContext != pUserData", __func__);
     return;
  } 

  if (vos_event_set(&gpVosContext->ProbeEvent)!= VOS_STATUS_SUCCESS)
  {
     VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, 
         "%s: vos_event_set failed", __func__);
     return;
  }

} /*                              */

v_VOID_t vos_WDAComplete_cback
(
  v_VOID_t *pUserData
)
{

  if (gpVosContext != pUserData)
  {
     VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
         "%s: gpVosContext != pUserData", __func__);
     return;
  }

  if (vos_event_set(&gpVosContext->wdaCompleteEvent)!= VOS_STATUS_SUCCESS)
  {
     VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
         "%s: vos_event_set failed", __func__);
     return;
  }

} /*                         */

v_VOID_t vos_core_return_msg
(
  v_PVOID_t      pVContext, 
  pVosMsgWrapper pMsgWrapper
)
{
  pVosContextType pVosContext = (pVosContextType) pVContext;
  
  VOS_ASSERT( gpVosContext == pVosContext);

  if (gpVosContext != pVosContext)
  {
     VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, 
         "%s: gpVosContext != pVosContext", __func__);
     return;
  } 

  VOS_ASSERT( NULL !=pMsgWrapper );

  if (pMsgWrapper == NULL)
  {
     VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, 
         "%s: pMsgWrapper == NULL in function", __func__);
     return;
  } 
  
  /*
                                                 
  */
  INIT_LIST_HEAD(&pMsgWrapper->msgNode);
  vos_mq_put(&pVosContext->freeVosMq, pMsgWrapper);

} /*                       */


/* 
                                                                          
                                    
   
                                                      

          
      

*/
v_VOID_t 
vos_fetch_tl_cfg_parms 
( 
  WLANTL_ConfigInfoType *pTLConfig,
  hdd_config_t * pConfig
)
{
  if (pTLConfig == NULL)
  {
   VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, "%s NULL ptr passed in!", __func__);
   return;
  }

  pTLConfig->ucAcWeights[0] = pConfig->WfqBkWeight;
  pTLConfig->ucAcWeights[1] = pConfig->WfqBeWeight;
  pTLConfig->ucAcWeights[2] = pConfig->WfqViWeight;
  pTLConfig->ucAcWeights[3] = pConfig->WfqVoWeight;
  pTLConfig->ucReorderAgingTime[0] = pConfig->BkReorderAgingTime;/*            */
  pTLConfig->ucReorderAgingTime[1] = pConfig->BeReorderAgingTime;/*            */
  pTLConfig->ucReorderAgingTime[2] = pConfig->ViReorderAgingTime;/*            */
  pTLConfig->ucReorderAgingTime[3] = pConfig->VoReorderAgingTime;/*            */
  pTLConfig->uDelayedTriggerFrmInt = pConfig->DelayedTriggerFrmInt;
  pTLConfig->uMinFramesProcThres = pConfig->MinFramesProcThres;

}

v_BOOL_t vos_is_apps_power_collapse_allowed(void* pHddCtx)
{
  return hdd_is_apps_power_collapse_allowed((hdd_context_t*) pHddCtx);
}

void vos_abort_mac_scan(void)
{
    hdd_context_t *pHddCtx = NULL;
    v_CONTEXT_t pVosContext        = NULL;

    /*                             */
    pVosContext = vos_get_global_context(VOS_MODULE_ID_SYS, NULL);
    if(!pVosContext) {
       hddLog(VOS_TRACE_LEVEL_FATAL, "%s: Global VOS context is Null", __func__);
       return;
    }
    
    /*                     */
    pHddCtx = (hdd_context_t *)vos_get_context(VOS_MODULE_ID_HDD, pVosContext );
    if(!pHddCtx) {
       hddLog(VOS_TRACE_LEVEL_FATAL, "%s: HDD context is Null", __func__);
       return;
    }

    hdd_abort_mac_scan(pHddCtx);
    return;
}

/*                                                                           

                                      

                                     

                                                                        
                            


                                        


                                                                      

                                                 

                                                                           */
VOS_STATUS vos_shutdown(v_CONTEXT_t vosContext)
{
  VOS_STATUS vosStatus;

#ifdef WLAN_BTAMP_FEATURE
  vosStatus = WLANBAP_Close(vosContext);
  if (!VOS_IS_STATUS_SUCCESS(vosStatus))
  {
     VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
         "%s: Failed to close BAP", __func__);
     VOS_ASSERT( VOS_IS_STATUS_SUCCESS( vosStatus ) );
  }
#endif //                   

  vosStatus = WLANTL_Close(vosContext);
  if (!VOS_IS_STATUS_SUCCESS(vosStatus))
  {
     VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
         "%s: Failed to close TL", __func__);
     VOS_ASSERT( VOS_IS_STATUS_SUCCESS( vosStatus ) );
  }

  vosStatus = sme_Close( ((pVosContextType)vosContext)->pMACContext);
  if (!VOS_IS_STATUS_SUCCESS(vosStatus))
  {
     VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
         "%s: Failed to close SME", __func__);
     VOS_ASSERT( VOS_IS_STATUS_SUCCESS( vosStatus ) );
  }

  vosStatus = macClose( ((pVosContextType)vosContext)->pMACContext);
  if (!VOS_IS_STATUS_SUCCESS(vosStatus))
  {
     VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
         "%s: Failed to close MAC", __func__);
     VOS_ASSERT( VOS_IS_STATUS_SUCCESS( vosStatus ) );
  }

  ((pVosContextType)vosContext)->pMACContext = NULL;

  vosStatus = sysClose( vosContext );
  if (!VOS_IS_STATUS_SUCCESS(vosStatus))
  {
     VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
         "%s: Failed to close SYS", __func__);
     VOS_ASSERT( VOS_IS_STATUS_SUCCESS( vosStatus ) );
  }

 /*                                                             */
  vosStatus = vos_packet_close( vosContext );
  if (!VOS_IS_STATUS_SUCCESS(vosStatus))
  {
     VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
         "%s: Failed to close VOSS Packet", __func__);
     VOS_ASSERT( VOS_IS_STATUS_SUCCESS( vosStatus ) );
  }

  vos_mq_deinit(&((pVosContextType)vosContext)->freeVosMq);

  vosStatus = vos_event_destroy(&gpVosContext->wdaCompleteEvent);
  if (!VOS_IS_STATUS_SUCCESS(vosStatus))
  {
     VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
         "%s: failed to destroy wdaCompleteEvent", __func__);
     VOS_ASSERT( VOS_IS_STATUS_SUCCESS( vosStatus ) );
  }

  vosStatus = vos_event_destroy(&gpVosContext->ProbeEvent);
  if (!VOS_IS_STATUS_SUCCESS(vosStatus))
  {
     VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
         "%s: failed to destroy ProbeEvent", __func__);
     VOS_ASSERT( VOS_IS_STATUS_SUCCESS( vosStatus ) );
  }

  return VOS_STATUS_SUCCESS;
}

/*                                                                           

                                                           

                       

                                        


                                                    

                                                 

                                                                           */
VOS_STATUS vos_wda_shutdown(v_CONTEXT_t vosContext)
{
  VOS_STATUS vosStatus;
  vosStatus = WDA_shutdown(vosContext, VOS_FALSE);

  if (!VOS_IS_STATUS_SUCCESS(vosStatus))
  {
     VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
         "%s: failed to shutdown WDA", __func__);
     VOS_ASSERT( VOS_IS_STATUS_SUCCESS( vosStatus ) );
  }
  return vosStatus;
}
/* 
                                                                

                                                                     
                                                              
                                                                    
                                                      
                         
        
           
         
                                                               
                                               

*/
VOS_STATUS vos_wlanShutdown(void)
{
   VOS_STATUS vstatus;
   vstatus = vos_watchdog_wlan_shutdown();
   return vstatus;
}
/* 
                                                             

                                                                     
                                                              
                             
                                                                       
                                                                        
                                             
        
           
         
                                                               
                                               

*/
VOS_STATUS vos_wlanReInit(void)
{
   VOS_STATUS vstatus;
   vstatus = vos_watchdog_wlan_re_init();
   return vstatus;
}
/* 
                                                              

                                                                  
                                                                          
                                                                  

                                                                         

        
           
         
                                                               
                                               
                                                     
                                                         


*/
VOS_STATUS vos_wlanRestart(void)
{
   VOS_STATUS vstatus;
   hdd_context_t *pHddCtx = NULL;
   v_CONTEXT_t pVosContext        = NULL;

   /*                                                 */
   if(vos_is_load_unload_in_progress( VOS_MODULE_ID_VOSS, NULL)) 
   {
      VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, 
               "%s: Driver load/unload is in progress, retry later.", __func__);
      return VOS_STATUS_E_AGAIN;
   }

   /*                             */
   pVosContext = vos_get_global_context(VOS_MODULE_ID_VOSS, NULL);
   if(!pVosContext) {
      VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL, 
               "%s: Global VOS context is Null", __func__);
      return VOS_STATUS_E_FAILURE;
   }
    
   /*                     */
   pHddCtx = (hdd_context_t *)vos_get_context(VOS_MODULE_ID_HDD, pVosContext );
   if(!pHddCtx) {
      VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL, 
               "%s: HDD context is Null", __func__);
      return VOS_STATUS_E_FAILURE;
   }

   /*                   */
   vstatus = wlan_hdd_restart_driver(pHddCtx);
   return vstatus;
}


/* 
                        

                                                            

        
                                   
                               
                               
                               
                               
         
           
*/
v_VOID_t vos_fwDumpReq(tANI_U32 cmd, tANI_U32 arg1, tANI_U32 arg2,
                        tANI_U32 arg3, tANI_U32 arg4)
{
   WDA_HALDumpCmdReq(NULL, cmd, arg1, arg2, arg3, arg4, NULL);
}

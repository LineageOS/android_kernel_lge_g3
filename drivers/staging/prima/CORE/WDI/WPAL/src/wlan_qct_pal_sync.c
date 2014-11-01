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



/**=========================================================================
  
  \file  wlan_qct_pal_sync.c
  
  \brief Implementation trace/logging APIs PAL exports. wpt = (Wlan Pal Type) wpal = (Wlan PAL)
               
   Definitions for platform Windows and with legacy UMAC.
  
   Copyright 2010 (c) Qualcomm, Incorporated.  All Rights Reserved.
   
   Qualcomm Confidential and Proprietary.
  
  ========================================================================*/

#include "wlan_qct_pal_sync.h"
#include "wlan_qct_pal_trace.h"

#include "wlan_qct_os_status.h"

/* 
               

      
                                          

                                                                

                                                            

*/
wpt_status wpalMutexInit(wpt_mutex *pMutex)
{
    /*                                                     */

   if( vos_lock_init( (vos_lock_t*)pMutex  ) != VOS_STATUS_SUCCESS )
   {
      WPAL_TRACE(eWLAN_MODULE_PAL, eWLAN_PAL_TRACE_LEVEL_ERROR, 
                 " mutex init fail\n");
      return eWLAN_PAL_STATUS_E_FAILURE;
   }

   return eWLAN_PAL_STATUS_SUCCESS;
}

/*                                                                           
                                    

                                                                     

                                                                
                                                                           */
wpt_status wpalMutexDelete(wpt_mutex *pMutex)
{
    /*                                                     */

   if( vos_lock_destroy( (vos_lock_t*)pMutex  ) != VOS_STATUS_SUCCESS )
   {
      WPAL_TRACE(eWLAN_MODULE_PAL, eWLAN_PAL_TRACE_LEVEL_ERROR, 
                 " mutex delete fail\n");
      return eWLAN_PAL_STATUS_E_FAILURE;
   }

   return eWLAN_PAL_STATUS_SUCCESS;
}

/*                                                                           
                                                                              

                                                                     

                                                                
                                                                           */
wpt_status wpalMutexAcquire(wpt_mutex *pMutex)
{
    /*                                                     */

   if( vos_lock_acquire( (vos_lock_t*)pMutex  ) != VOS_STATUS_SUCCESS )
   {
      WPAL_TRACE(eWLAN_MODULE_PAL, eWLAN_PAL_TRACE_LEVEL_ERROR, 
                 " mutex acquire fail\n");
      return eWLAN_PAL_STATUS_E_FAILURE;
   }

   return eWLAN_PAL_STATUS_SUCCESS;
}

/*                                                                           
                                      

                                                                     

                                                                
                                                                           */
wpt_status wpalMutexRelease(wpt_mutex *pMutex)
{
    /*                                                     */

   if( vos_lock_release( (vos_lock_t*)pMutex ) != VOS_STATUS_SUCCESS )
   {
      WPAL_TRACE(eWLAN_MODULE_PAL, eWLAN_PAL_TRACE_LEVEL_ERROR, 
                 " mutex release\n");
      return eWLAN_PAL_STATUS_E_FAILURE;
   }

   return eWLAN_PAL_STATUS_SUCCESS;
}

/*                                                                           
                                     

                                                                     

                                                                
                                                                        */
wpt_status wpalEventInit(wpt_event *pEvent)
{
   /*                                                     */

   if( vos_event_init( (vos_event_t*)pEvent ) != VOS_STATUS_SUCCESS )
   {
      WPAL_TRACE(eWLAN_MODULE_PAL, eWLAN_PAL_TRACE_LEVEL_ERROR, 
                 " create event fail\n");
      return eWLAN_PAL_STATUS_E_FAILURE;
   }

   return eWLAN_PAL_STATUS_SUCCESS;
}

/*                                                                           
                                     

                                                                     

                                                                
                                                                        */

wpt_status wpalEventDelete(wpt_event *pEvent)
{
   /*                                                     */

   if( vos_event_destroy( (vos_event_t*)pEvent ) != VOS_STATUS_SUCCESS )
   {
      WPAL_TRACE(eWLAN_MODULE_PAL, eWLAN_PAL_TRACE_LEVEL_ERROR, 
                 " delete event fail\n");
      return eWLAN_PAL_STATUS_E_FAILURE;
   }

   return eWLAN_PAL_STATUS_SUCCESS;
}

/*                                                                           
                                                  

          
                                                                  
                                                          
                                                

                                                                                   
                                                                          
                                                                  
                                  
                                                                                    
                                 
                                    
                                                                               
                                                                          
                                                                            
                                                                                
                         
             
                                                                                  
                                                       
              
                                                                                   
                                                                           */
wpt_status wpalEventWait(wpt_event *pEvent, wpt_uint32 timeout)
{
   /*                                                     */

   wpt_status status = eWLAN_PAL_STATUS_E_FAILURE;
   VOS_STATUS  vos_status = VOS_STATUS_E_FAILURE;

   /*                                                 */
   timeout = ( timeout == WLAN_PAL_WAIT_INFINITE ? 0 : timeout );

   vos_status = vos_wait_single_event( (vos_event_t*)pEvent, timeout );

   status = WPAL_VOS_TO_WPAL_STATUS( vos_status );

   return status;
}

/*                                                                           
                                                        
          
                                                                  
           
                                                           
                                                                           */
wpt_status wpalEventSet(wpt_event *pEvent)
{
   /*                                                     */

   return ( WPAL_VOS_TO_WPAL_STATUS(vos_event_set( (vos_event_t*)pEvent )) );
}

/*                                                                           
                                                              
          
                                                                  
           
                                                           
                                                                           */
wpt_status wpalEventReset(wpt_event *pEvent)
{
   /*                                                     */

   return ( WPAL_VOS_TO_WPAL_STATUS(vos_event_reset( (vos_event_t*)pEvent )) );
}



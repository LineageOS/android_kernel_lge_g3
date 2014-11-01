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


/*============================================================================
  FILE:         vos_event.c

  OVERVIEW:     This source file contains definitions for vOS event APIs
                The five APIs mentioned in this file are used for
                initializing, setting, resetting, destroying an event and
                waiting on an occurance of an event among multiple events.

  DEPENDENCIES:

                Copyright (c) 2007 QUALCOMM Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/

/*                                                                            
                         

                                                                            */

/*                                                                            
                
                                                                            */
#include "vos_event.h"
#include "vos_trace.h"

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
VOS_STATUS vos_event_init ( vos_event_t* event )
{

   //                       
   if ( NULL == event )
   {
      VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL,
                "NULL event passed into %s", __func__);
      VOS_ASSERT(0);
      return VOS_STATUS_E_FAULT;
   }

   //                                      
   if ( LINUX_EVENT_COOKIE == event->cookie )
   {
      VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL,
                "Initialized event passed into %s", __func__);
      VOS_ASSERT(0);
      return VOS_STATUS_E_BUSY;
   }

   //                     
   init_completion(&event->complete);
   event->cookie = LINUX_EVENT_COOKIE;

   return VOS_STATUS_SUCCESS;
}

/*                                                                          

                                            

                                                                  
                     

                                                                         
                                                                         
                                              

                                                        

                                                                    

                                                                          
                                         

                                                            

                                                                   
                         

     

                                                                           */

VOS_STATUS vos_event_set ( vos_event_t* event )
{

   //                       
   if ( NULL == event )
   {
      VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL,
                "NULL event passed into %s", __func__);
      VOS_ASSERT(0);
      return VOS_STATUS_E_FAULT;
   }

   //                                               
   if ( LINUX_EVENT_COOKIE != event->cookie )
   {
      VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL,
                "Uninitialized event passed into %s", __func__);
      VOS_ASSERT(0);
      return VOS_STATUS_E_INVAL;
   }

   complete(&event->complete);

   return VOS_STATUS_SUCCESS;
}

/*                                                                          

                                                                               
                                            

                                                                       
                                                                              
                                   

                                                                             
                                                 

                                                            

                                                                         
                        

                                                                          
                                         

                                                            

                                                                   
                         


     

                                                                           */
VOS_STATUS vos_event_reset ( vos_event_t* event )
{

   //                       
   if ( NULL == event )
   {
      VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL,
                "NULL event passed into %s", __func__);
      VOS_ASSERT(0);
      return VOS_STATUS_E_FAULT;
   }

   //                                                        
   if ( LINUX_EVENT_COOKIE != event->cookie )
   {
      VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL,
                "Uninitialized event passed into %s", __func__);
      VOS_ASSERT(0);
      return VOS_STATUS_E_INVAL;
   }

   //                     
   INIT_COMPLETION(event->complete);
   return VOS_STATUS_SUCCESS;
}


/*                                                                            

                                                                    

                                                                  
                                                                   
                                   

                                                                        
                                                                      
                                                                       
                                 

                                                           

                                                                     

                                                                          
                                                                       
                                                                        
                                                                

                                                                           
                                                                     
                           

                                                                          
                                                                      
                                                               

                                                                           
                              

                                                                          
                                                              

                                                                            
                                                     

                                                                         
                                                    

                                                                          

                                                                        
                         

     
                                                                             */
VOS_STATUS vos_wait_events ( vos_event_t* events,
                             v_U8_t numEvents, v_U32_t timeout,
                             v_U8_t *pEventIndex )
{

   return vos_wait_single_event(events,timeout);
}

/*                                                                          

                                                                               
                                                                                
             

                                                                
                                                                             
                                                     

                                                                       
                                                                             
                                                                           
                                                                     
                                                              
                      

                                                  

                                                                

                                                                       

                                                           

                                                                   
                         

                                                                        
                                                                           
                                                              
     

                                                   
                                                                           */

VOS_STATUS vos_event_destroy ( vos_event_t* event )
{
   //                       
   if ( NULL == event )
   {
      VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL,
                "NULL event passed into %s", __func__);
      VOS_ASSERT(0);
      return VOS_STATUS_E_FAULT;
   }

   //                                                        
   if ( LINUX_EVENT_COOKIE != event->cookie )
   {
      VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL,
                "Uninitialized event passed into %s", __func__);
      VOS_ASSERT(0);
      return VOS_STATUS_E_INVAL;
   }

   //                                         
   complete_all(&event->complete);

   //                  
   memset(event, 0, sizeof(vos_event_t));

   return VOS_STATUS_SUCCESS;
}

/*                                                                            

                                                                      

                                          

                                                 

                                                                          
                                                                       
                                                                        
                                                                

                                                                        
              

                                                                        
                        

                                                                       

                                                            

                                

                                                                            */
VOS_STATUS vos_wait_single_event ( vos_event_t* event, v_U32_t timeout)
{

   if (in_interrupt())
   {
      VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL,
                "%s cannot be called from interrupt context!!!", __func__);
      VOS_ASSERT(0);
      return VOS_STATUS_E_FAULT;
   }

   //                       
   if ( NULL == event )
   {
      VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL,
                "NULL event passed into %s", __func__);
      VOS_ASSERT(0);
      return VOS_STATUS_E_FAULT;
   }

   //                                                     
   if ( LINUX_EVENT_COOKIE != event->cookie )
   {
      VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL,
                "Uninitialized event passed into %s", __func__);
      VOS_ASSERT(0);
      return VOS_STATUS_E_INVAL;
   }

   if (timeout)
   {
      long ret;
      ret =
         wait_for_completion_timeout(&event->complete,
                                                   msecs_to_jiffies(timeout));
      if ( 0 >= ret )
      {
        return VOS_STATUS_E_TIMEOUT;
      }
   }
   else
   {
      int ret;
      ret = wait_for_completion_interruptible(&event->complete);
      if ( 0 != ret )
      {
         //                           
         return VOS_STATUS_E_TIMEOUT;
      }
   }

   return VOS_STATUS_SUCCESS;
}

/*                                                                            

                                                                   
                                                                        
                                               

                                                                  
                                                                   
                                   

                                                                        
                                                                      
                                                                       
                                 

                                                           

                                     

                                                                          
                                                                       
                                                                        
                                                                

                                                                           
                                                                     
                           

                                                                          
                                                                      
                                                               

                                                                           
                              

                                                                          
                                                                           
                                                                           
                                                                      
                   

                                                                         
                                                    

                                                                          

                              

                                                                            */
VOS_STATUS vos_wait_multiple_events( vos_event_t **events, v_U8_t numEvents,
   v_U32_t timeout, v_U8_t *pEventIndex )
{
   //                    
   return VOS_STATUS_E_FAILURE;
}

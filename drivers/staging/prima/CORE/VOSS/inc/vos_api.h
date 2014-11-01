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

#if !defined( __VOS_API_H )
#define __VOS_API_H

/**=========================================================================
  
  \file  vos_Api.h
  
  \brief virtual Operating System Services (vOSS) API
               
   Header file that inludes all the vOSS API definitions.
  
   Copyright 2008 (c) Qualcomm, Incorporated.  All Rights Reserved.
   
   Qualcomm Confidential and Proprietary.
  
  ========================================================================*/
 /*                                                                            

                                             
   
   
                                                                        
                                                                 
   
   
                                    
   
   
                                      
                                                                             
                                         
                                      
                                                                           */

/*                                                                          
               
                                                                          */
//                                                        
#include <vos_types.h>
#include <vos_status.h>
#include <vos_memory.h>
#include <vos_list.h>
#include <vos_getBin.h>
#include <vos_trace.h>
#include <vos_event.h>
#include <vos_lock.h>
#include <vos_nvitem.h>
#include <vos_mq.h>
#include <vos_packet.h>
#include <vos_threads.h>
#include <vos_timer.h>
#include <vos_pack_align.h>

/*                                                                          
                                        
                                                                          */

/*                                                                           
  
                                                  
    
                                                                           
                                                                    
                
                                                                   
                                                                
                       
  
                                                                   
 
  
                                                                          
                              
              
                                                                        
          
                
  
                                                                           */
VOS_STATUS vos_preOpen ( v_CONTEXT_t *pVosContext );

VOS_STATUS vos_preClose( v_CONTEXT_t *pVosContext );


VOS_STATUS vos_preStart( v_CONTEXT_t vosContext );


VOS_STATUS vos_open( v_CONTEXT_t *pVosContext, v_SIZE_t hddContextSize );



VOS_STATUS vos_start( v_CONTEXT_t vosContext ); 

VOS_STATUS vos_stop( v_CONTEXT_t vosContext );

VOS_STATUS vos_close( v_CONTEXT_t vosContext );

/*                                                                                */
VOS_STATUS vos_shutdown( v_CONTEXT_t vosContext );

/*                               */
VOS_STATUS vos_wda_shutdown( v_CONTEXT_t vosContext );

/*                                                                            
  
                                                  
  
                                                                       
                                                                         
                                                             

                                                
  
                                                                            
                      
                                             
  
                                                                       
                      
              
                                                                            */
v_VOID_t *vos_get_context( VOS_MODULE_ID moduleId, 
                           v_CONTEXT_t vosContext );


/*                                                                            
  
                                                           
  
                                                                       
                             
  
                                                         
  
                                                                    
                       
                      
                                              
  
                                                                         
              
                                                                            */
v_CONTEXT_t vos_get_global_context( VOS_MODULE_ID moduleId, 
                                    v_VOID_t *moduleContext );

v_U8_t vos_is_logp_in_progress(VOS_MODULE_ID moduleId, v_VOID_t *moduleContext);
void vos_set_logp_in_progress(VOS_MODULE_ID moduleId, v_U8_t value);

v_U8_t vos_is_load_unload_in_progress(VOS_MODULE_ID moduleId, v_VOID_t *moduleContext);
void vos_set_load_unload_in_progress(VOS_MODULE_ID moduleId, v_U8_t value);

v_U8_t vos_is_reinit_in_progress(VOS_MODULE_ID moduleId, v_VOID_t *moduleContext);
void vos_set_reinit_in_progress(VOS_MODULE_ID moduleId, v_U8_t value);

/*                                                                            
  
                                                                                
  
                                                                      
                       
  
                                                        
  
                                                                        
  
                                                                        
                                                                     
                                                                  
                                                     
  
                                                             
                      
                                                                        
                                                                    
                                               
                                                                  
                                                              
                                                    

                                                                     
                                                                    

                                                                           
                                                                         
                                                           
            
                                                                       
                                     
              
                                           
  
                                                                            */
VOS_STATUS vos_alloc_context( v_VOID_t *pVosContext, VOS_MODULE_ID moduleID, 
                              v_VOID_t **ppModuleContext, v_SIZE_t size );


/*                                                                            
  
                                                                     
                                                  
  
                                                                  
                       
  
                                                        
  
                                                                  
  
                                                                      
                      
                                                                        
                                                                   
                      
            
                                                                       
                           
                                 
                                                                     
                                                                 
            
                                                                   
                                                                      
                                            
              
                                     
  
                                                                            */
VOS_STATUS vos_free_context( v_VOID_t *pVosContext, VOS_MODULE_ID moduleID,
                             v_VOID_t *pModuleContext );
                             
v_BOOL_t vos_is_apps_power_collapse_allowed(void* pHddCtx);
void vos_abort_mac_scan(void);

/* 
                                                                

                                                                     
                                                              
                                                                    
                                                      
                         

        
           
         
                                                               
                                               

*/
VOS_STATUS vos_wlanShutdown(void);

/* 
                                                             

                                                                     
                                                              
                             
                                                                       
                                                                        
                                             
        
           
         
                                                               
                                               

*/
VOS_STATUS vos_wlanReInit(void);

/* 
                                                              

                                                                  
                                                                          
                                                                  

        
           
         
                                                               
                                               

*/
VOS_STATUS vos_wlanRestart(void);

/* 
                        

                                                            

        
                                   
                               
                               
                               
                               
         
           
*/
v_VOID_t vos_fwDumpReq(tANI_U32 cmd, tANI_U32 arg1, tANI_U32 arg2,
                        tANI_U32 arg3, tANI_U32 arg4);
#endif //                           

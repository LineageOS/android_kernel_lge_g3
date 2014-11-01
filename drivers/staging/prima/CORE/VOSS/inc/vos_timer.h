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

#if !defined( __VOS_TIMER_H )
#define __VOS_TIMER_H

/**=========================================================================
  
  \file  vos_timer.h
  
  \brief virtual Operating System Servies (vOS)
               
   Definitions for vOSS Timer services
  
   Copyright 2008 (c) Qualcomm, Incorporated.  All Rights Reserved.
   
   Qualcomm Confidential and Proprietary.
  
  ========================================================================*/

/*          */

/*                                                                          
               
                                                                          */
#include <vos_types.h>
#include <vos_status.h>
#include <vos_lock.h>
#include <i_vos_timer.h>

#ifdef TIMER_MANAGER
#include "wlan_hdd_dp_utils.h"
#endif

/*                                                                           
                                        
                                                                          */
#define VOS_TIMER_STATE_COOKIE 0x12

/*                                                                           
                   
                                                                          */
//                                                                        
//                                      
typedef v_VOID_t ( *vos_timer_callback_t )( v_PVOID_t userData );

typedef enum
{
   //                                                            
   //                                  
   VOS_TIMER_TYPE_SW,
   
   //                                                                
   //                           
   //                                        
   VOS_TIMER_TYPE_WAKE_APPS

} VOS_TIMER_TYPE;

typedef enum
{
   VOS_TIMER_STATE_UNUSED = VOS_TIMER_STATE_COOKIE,
   VOS_TIMER_STATE_STOPPED,
   VOS_TIMER_STATE_STARTING,
   VOS_TIMER_STATE_RUNNING,
} VOS_TIMER_STATE;

#ifdef TIMER_MANAGER
struct vos_timer_s;
typedef struct timer_node_s
{
   hdd_list_node_t pNode;
   char* fileName;
   unsigned int lineNum;
   struct vos_timer_s *vosTimer;
}timer_node_t;
#endif

typedef struct vos_timer_s
{
#ifdef TIMER_MANAGER
   timer_node_t *ptimerNode;
#endif

   vos_timer_platform_t platformInfo;
   vos_timer_callback_t callback;
   v_PVOID_t            userData;
   vos_lock_t           lock;
   VOS_TIMER_TYPE       type;
   VOS_TIMER_STATE      state;
} vos_timer_t;

/*                                                                          
                                        
                                                                          */
#ifdef TIMER_MANAGER
void vos_timer_manager_init(void);
void vos_timer_exit(void);
#endif

/*                                                                           
  
                                                                         

                                  
  
                     
  
     
  
                                                                           */
VOS_TIMER_STATE vos_timer_getCurrentState( vos_timer_t *pTimer );

/*                                                                          
  
                                                    

                                           
  
                                                                           
                                                                         
                             
  
                                                                        
                                                                          
                                                                        
                                              
  
                                                            
                                                                     
                                                                    
                                             
  
                                                                        
                                                                        
                            
  
                                                                 
  
                                                                        
                                 
        
                                                                      
                                                     
                                                                       
                                                               
                                
  
                                                                     
                 
         
                                                                     
                                                                          
  
                                                                      
                              
  
                                                                        
                                                 

                                                                        
                   

                                                                         
                                                                     
                                                  

                                                                 
     
  
                                                                           */
#ifdef TIMER_MANAGER
#define vos_timer_init(timer, timerType, callback, userdata) \
      vos_timer_init_debug(timer, timerType, callback, userdata, __FILE__, __LINE__)
      
VOS_STATUS vos_timer_init_debug( vos_timer_t *timer, VOS_TIMER_TYPE timerType, 
                           vos_timer_callback_t callback, v_PVOID_t userData, 
                           char* fileName, v_U32_t lineNum );      
#else
VOS_STATUS vos_timer_init( vos_timer_t *timer, VOS_TIMER_TYPE timerType, 
                           vos_timer_callback_t callback, v_PVOID_t userData );
#endif

/*                                                                           
  
                                                          

                                                                     
                                                                  
                                           
   
                                                           
                                                                     
                                              
  
                                                             
                                                                     
                                                               
                      
  
                                                  
  
                                                                
  
                                                                         
                                                                      
                                                                        
                    

                                                                       
          
                                                                 
     
  
                                                                           */
VOS_STATUS vos_timer_destroy( vos_timer_t *timer );


/*                                                                          
  
                                                      

                                                                       
                                                                    
                       
   
                                                              
                                                                 
                        
  
                                               
  
                                                                         
                                                                        
  
                                                              
  
                                                                            
                                                                        
                                                              

                                                                       
          
                                                                 
     
  
                                                                           */
VOS_STATUS vos_timer_start( vos_timer_t *timer, v_U32_t expirationTime );


/*                                                                          
  
                                             

                                                                          
                                                              
   
                                                                        
                                                                       
  
                                               
    
                                                              
  
                                                                          
                                                                   
                  

                                                                       
          
                                                                 
     
  
                                                                          */
VOS_STATUS vos_timer_stop( vos_timer_t *timer );


/*                                                                          
  
                                                                         

                                                                         
                                                                             
                                                                           
             
    
                                                                        
                                  
  
     
  
                                                                          */
v_TIME_t vos_timer_get_system_ticks( v_VOID_t );


/*                                                                          
 
                                                                          

                                                                                 
                                                
    
                                                     
  
     
  
                                                                          */
v_TIME_t vos_timer_get_system_time( v_VOID_t );



#endif //                            

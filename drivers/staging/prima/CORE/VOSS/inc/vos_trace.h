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


#if !defined( __VOS_TRACE_H )
#define __VOS_TRACE_H

/*                                                                          
  
                    
  
                                               
               
                                                     
  
                                                                          */

/*          */

/*                                                                          
               
                                                                          */
#include  <vos_types.h>  //                     
#include  <stdarg.h>       //               

/*                                                                           
                    
                                                                          */

typedef enum 
{
   //                                                                      
   //                                                               
   VOS_TRACE_LEVEL_NONE = 0,
   
   //                                                                      
   //                                                                         
   //                                                                 
   VOS_TRACE_LEVEL_FATAL,
   VOS_TRACE_LEVEL_ERROR, 
   VOS_TRACE_LEVEL_WARN,  
   VOS_TRACE_LEVEL_INFO,
   VOS_TRACE_LEVEL_INFO_HIGH,
   VOS_TRACE_LEVEL_INFO_MED,
   VOS_TRACE_LEVEL_INFO_LOW,
   VOS_TRACE_LEVEL_DEBUG,

   //                                                                            
   //                                                              
   VOS_TRACE_LEVEL_ALL, 

   
   //                                                           
   //                          
   VOS_TRACE_LEVEL_MAX   
    
} VOS_TRACE_LEVEL;

/*                                                                           
                                        
                                                                          */
#define ASSERT_BUFFER_SIZE ( 512 )

//                                                                       
//                                                   
#define VOS_ENABLE_TRACING 
#define WCONN_TRACE_KMSG_LOG_BUFF

#include  <i_vos_trace.h>   

/*                                                                          
                                        
                                                                          */


/*                                                                            
  
                                                                           
  
                                                                             
  
                                                                
                                                                         
                                                                       
                             
         
                  
    
     
                                                                            */
void vos_trace_setLevel( VOS_MODULE_ID module, VOS_TRACE_LEVEL level );

/*                                                                             
  
                                                   
  
                                                                       
                                                         
  
                                                                            
                                                                            
                                      
         
                                                                            
                                                                            
                                                          

                                                                                 
    
                                                                              
    
                          
                                                                            */
v_BOOL_t vos_trace_getLevel( VOS_MODULE_ID module, VOS_TRACE_LEVEL level );

#ifdef WCONN_TRACE_KMSG_LOG_BUFF
/*                                                                          
                                                            
                                                                 

                       
                                                                           */
void vos_wconn_trace_init(void);

/*                                                                          
                                                               
                                                                    

                       
                                                                           */
void vos_wconn_trace_exit(void);
#endif

#endif

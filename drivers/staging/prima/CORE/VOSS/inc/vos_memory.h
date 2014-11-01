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

#if !defined( __VOS_MEMORY_H )
#define __VOS_MEMORY_H
 
/**=========================================================================
  
  \file  vos_memory.h
  
  \brief virtual Operating System Servies (vOSS)
               
   Memory management functions
  
   Copyright 2008 (c) Qualcomm, Incorporated.  All Rights Reserved.
   
   Qualcomm Confidential and Proprietary.
  
  ========================================================================*/

/*          */

/*                                                                          
               
                                                                          */
#include <vos_types.h>

/*                                                                           
                                        
                                                                          */

#ifdef MEMORY_DEBUG
v_VOID_t vos_mem_init(v_VOID_t);
v_VOID_t vos_mem_exit(v_VOID_t);
void vos_mem_clean(void);
#endif

/*                                                                           
                   
                                                                          */


/*                                                                          
                                        
                                                                          */

/*                                                                            
  
                                                  

                                                                            
         
  
                                                            
  
                                                                      
                                                                          
                                                    
    
     
  
                                                                            */
#ifdef MEMORY_DEBUG
#define vos_mem_malloc(size) vos_mem_malloc_debug(size, __FILE__, __LINE__)
v_VOID_t * vos_mem_malloc_debug( v_SIZE_t size, char* fileName, v_U32_t lineNum);
#else
v_VOID_t * vos_mem_malloc( v_SIZE_t size );
#endif


/*                                                                            
  
                                          

                                                         
  
                                                                   
                        
  
                 
    
     
  
                                                                            */
v_VOID_t vos_mem_free( v_VOID_t *ptr );


/*                                                                            

                                                                    
  
                                                     
    
                                                 
    
                                       
    
                      
  
                    
  
                                                                            */
v_VOID_t vos_mem_set( v_VOID_t *ptr, v_SIZE_t numBytes, v_BYTE_t value );


/*                                                                            

                                      
  
                                                                           
             
  
                                                             
    
                                            
    
                                       
    
                      
  
                   
  
                                                                            */
v_VOID_t vos_mem_zero( v_VOID_t *ptr, v_SIZE_t numBytes );


/*                                                                            
  
                                     

                                                                      
                                                                          
                                                                      
                                                                    
                                                                    
                                                                           

                                                                   
  
                                                                
  
                                            
  
                   
    
                    
  
                                                                            */
v_VOID_t vos_mem_copy( v_VOID_t *pDst, const v_VOID_t *pSrc, v_SIZE_t numBytes );


/*                                                                            
  
                                     

                                                                       
                                                           
                                          

                                                                   
  
                                                                
  
                                            
  
                   
    
                    
  
                                                                            */
v_VOID_t vos_mem_move( v_VOID_t *pDst, const v_VOID_t *pSrc, v_SIZE_t numBytes );

/*                                                                             

                         

                                             
    
                                                                         
                  
    
                                                                   

                                                                      
    
                                                     
    
                                                                       
                                                         
    
                                                                                 */
v_BOOL_t vos_mem_compare( v_VOID_t *pMemory1, v_VOID_t *pMemory2, v_U32_t numBytes ); 


/*                                                                             

                          

                                              
    
                                                                         
                  

                                                                   

                                                                      
    
                                                     
    
                                                                       
                                                         
                                 
                                                              
                                                                
    
                                                                                 */
v_SINT_t vos_mem_compare2( v_VOID_t *pMemory1, v_VOID_t *pMemory2, v_U32_t numBytes );


/*                                                                            
  
                                                          

                                                                            
                                                                              
                                                                         
                                                              

                                                                           
                
  
                                                            
  
                                                                      
                                                                          
                                                    
    
     
  
                                                                            */
#ifdef MEMORY_DEBUG
#define vos_mem_dma_malloc(size) vos_mem_dma_malloc_debug(size, __FILE__, __LINE__)
v_VOID_t * vos_mem_dma_malloc_debug( v_SIZE_t size, char* fileName, v_U32_t lineNum);
#else
v_VOID_t * vos_mem_dma_malloc( v_SIZE_t size );
#endif


/*                                                                            
  
                                                  

                                                                          
  
                                                                   
                        
  
                 
    
     
  
                                                                            */
v_VOID_t vos_mem_dma_free( v_VOID_t *ptr );

#ifdef DMA_DIRECT_ACCESS
/*                                                                            
  
                                                                  

                                                     

  
                                                                         
                        
  
                 
    
     
  
                                                                            */
v_VOID_t vos_mem_set_dma_ptr(unsigned char *dmaBuffer);
#endif /*                   */
#endif //              

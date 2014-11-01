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

#if !defined( __I_VOS_TYPES_H )
#define __I_VOS_TYPES_H
#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/completion.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <asm/div64.h>

/**=========================================================================
  
  \file  i_vos_Types.h
  
  \brief virtual Operating System Servies (vOSS) Types
               
   Linux specific basic type definitions 
  
   Copyright 2008 (c) Qualcomm, Incorporated.  All Rights Reserved.
   
   Qualcomm Confidential and Proprietary.
  
  ========================================================================*/

/*          */

/*                                                                          
               
                                                                          */

/*                                                                           
                                        
                                                                          */
/*
                        
  
                               
                               
                                                         
                                   
  
                              
  
                               
                              
                                                     
  
 */
   
//                                                 
//                                                                        
//                                                                       
//                                  
#if defined( _MSC_VER )

#define VOS_COMPILER_MSC
#define VOS_OS_WINMOBILE    //                                                    

#elif defined( __GNUC__ )

#define VOS_COMPILER_GNUC
#define VOS_OS_LINUX       //                                            

#endif


//                                                                            
//                                                                         
//                                                                         
//                       
//                                                                          
//                                                                    
//                                                                       
//                                                
//
//                                                                        
//                                                                            
//                                                                   
//                                             
//
//                                                                         
//                                                                     
//                                                                      
//                                                         
//
//                                                                    
//                                                       
//
//
//                                    
//
//               
//  
//                   
//              
//                              
//
//
//                                                                           
//                    
//
//                                


//                                                                                  
//                                                                                  
//                                                                                   
//                                                                              
//                                                                                  
//                                        
#if defined( VOS_COMPILER_MSC )


#define VOS_INLINE_FN  __inline

//                                                             
//                                   
#define VOS_PACK_STRUCT_1
#define VOS_PACK_STRUCT_2
#define VOS_PACK_STRUCT_4
#define VOS_PACK_STRUCT_8
#define VOS_PACK_STRUCT_16

#elif defined( VOS_COMPILER_GNUC )

#define VOS_INLINE_FN  static inline

#else
#error "Compiling with an unknown compiler!!"
#endif

 

/*                                                                           
                   
                                                                          */

//                      
typedef unsigned char v_U8_t;
typedef unsigned char v_UCHAR_t;
typedef unsigned char v_BYTE_t;

//                       
typedef unsigned short v_U16_t;
typedef unsigned short v_USHORT_t;

//                       
typedef unsigned long  v_U32_t;
//                            
typedef unsigned long  v_ULONG_t;

//                       
typedef long long v_U64_t;

//                        
typedef unsigned int  v_UINT_t;

//                    
typedef signed char  v_S7_t;
typedef signed char  v_SCHAR_t;

//                     
typedef signed short v_S15_t;
typedef signed short v_SSHORT_t;

//                     
typedef signed long v_S31_t;
typedef signed long v_SLONG_t;

//                      
typedef signed int   v_SINT_t;
                              
//               
typedef unsigned char v_BOOL_t;

//           
#define v_VOID_t void

#endif //                 

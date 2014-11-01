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

/*==========================================================================
 *
 *  @file:     aniCompiler.h
 *
 *  @brief:    This file tries to abstract the differences among compilers.
 *             Supported compilers are: 
 *             ARM RVCT compiler
 *
 *  @author:   Kumar Anand
 *
 *             Copyright (C) 2010, Qualcomm, Inc. 
 *             All rights reserved.
 *
 *=========================================================================*/
#ifndef __ANI_COMPILER_ABSTRACT_H
#define __ANI_COMPILER_ABSTRACT_H

/*
                        
  
                               
                               
                                                         
                                   
  
                              
  
                               
                              
                                                     
  
                          
  
                                                                       
                              
                                                               
  
                                     
  
                         
                     
                          
                      
 */

/*
                                                                         
                                                                           
                                                                         
                                                                       
                 
  
                                                                        
                                                                         
                                                                       
                                                                    
                                                                          
                                               
 */

#undef __ANI_COMPILER_PRAGMA_PACK_STACK
#undef __ANI_COMPILER_PRAGMA_PACK

#if defined(_MSC_VER)
#define __ANI_COMPILER_PRAGMA_PACK_STACK        1
#define __ANI_COMPILER_PRAGMA_PACK              1
#define __ani_attr_pre_packed 
#define __ani_attr_packed 
#define __ani_attr_aligned_2
#define __ani_attr_aligned_4
#define __ani_attr_aligned_8
#define __ani_attr_aligned_16
#define __ani_attr_aligned_32
#define PACKED
#define PACKED_POST
#define ALIGN(__value)
#elif defined(__INTEL_COMPILER) || defined(__ICC) || defined(__ECC)
#define __ANI_COMPILER_PRAGMA_PACK              1
#define __ani_attr_pre_packed 
#define __ani_attr_packed 
#define __ani_attr_aligned_2
#define __ani_attr_aligned_4
#define __ani_attr_aligned_8
#define __ani_attr_aligned_16
#define __ani_attr_aligned_32
#define PACKED
#define PACKED_POST
#define ALIGN(__value)
#elif defined(__GNUC__)
#define __ani_attr_pre_packed 
#define __ani_attr_packed                       __attribute__((packed))
#define __ani_attr_aligned_2                    __attribute__((aligned(2)))
#define __ani_attr_aligned_4                    __attribute__((aligned(4)))
#define __ani_attr_aligned_8                    __attribute__((aligned(8)))
#define __ani_attr_aligned_16                   __attribute__((aligned(16)))
#define __ani_attr_aligned_32                   __attribute__((aligned(32)))
#ifndef PACKED
#define PACKED
#endif
#ifndef PACKED_POST
#define PACKED_POST                             __attribute__((packed))
#endif
#ifndef ALIGN
#define ALIGN(__value)                          __attribute__((aligned(__value)))
#endif
#elif defined(ANI_COMPILER_TYPE_RVCT)
/*                        */

/* 
                                                                                                                            
                                                                                                                    
 */
#if (__ARMCC_VERSION > 400000) 
#define __ani_attr_packed
#define __ani_attr_pre_packed                   __packed
#define __ani_attr_aligned_2                    __attribute__((aligned(2)))
#define __ani_attr_aligned_4                    __attribute__((aligned(4)))
#define __ani_attr_aligned_8                    __attribute__((aligned(8)))
#define __ani_attr_aligned_16                   __attribute__((aligned(16)))
#define __ani_attr_aligned_32                   __attribute__((aligned(32)))
#define PACKED                                  __packed
#define PACKED_POST
#define ALIGN(__value)                          __align(__value)
#define PREPACK                                 __packed
#define POSTPACK
#else
#define __ani_attr_packed
#define __ani_attr_pre_packed                   __packed
#define __ani_attr_aligned_2                    __align(2)
#define __ani_attr_aligned_4                    __align(4)
#define __ani_attr_aligned_8                    __align(8)
#define __ani_attr_aligned_16                   __align(16)
#define __ani_attr_aligned_32                   __align(32)
#define PACKED                                  __packed
#define PACKED_POST
#define ALIGN(__value)                          __align(__value)
#endif

#else
#error "Unknown compiler"
#endif

#ifndef PACKED_PRE
#define PACKED_PRE   __ani_attr_pre_packed
#endif

#ifndef ALIGN_4
#define ALIGN_4    __ani_attr_aligned_4
#endif

#endif //                         


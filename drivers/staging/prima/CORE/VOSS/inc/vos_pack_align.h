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

#if !defined( __VOS_PACK_ALIGN_H )
#define __VOS_PACK_ALIGN_H

/**=========================================================================
  
  \file  vos_pack_align.h
  
  \brief virtual Operating System Servies (vOS) pack and align primitives
               
   Definitions for platform independent means of packing and aligning
   data structures
  
   Copyright 2009 (c) Qualcomm, Incorporated.  All Rights Reserved.
   
   Qualcomm Confidential and Proprietary.
  
  ========================================================================*/

/*
 
                                                                           
                                                                             
                                                                            
                                                                           
                                            

                                                                            
  
                        
  
                                                  
  
                
                                                                 
              
  
                                                    
  
                                                                         

                                                                         

                
                                                                                                    
              
  
*/

#if defined __GNUC__

  #define VOS_PACK_START 
  #define VOS_PACK_END 

  #define VOS_PACK_PRE 
  #define VOS_PACK_POST  __attribute__((__packed__))

  #define VOS_ALIGN_PRE(__value)
  #define VOS_ALIGN_POST(__value)  __attribute__((__aligned__(__value)))

#elif defined __arm

  #define VOS_PACK_START 
  #define VOS_PACK_END 

  #define VOS_PACK_PRE  __packed
  #define VOS_PACK_POST

  #define VOS_ALIGN_PRE(__value)  __align(__value)
  #define VOS_ALIGN_POST(__value)

#elif defined _MSC_VER

#define VOS_PACK_START  __pragma(pack(push,1))
#define VOS_PACK_END  __pragma(pack(pop))

  #define VOS_PACK_PRE 
  #define VOS_PACK_POST

  #define VOS_ALIGN_PRE(__value)  __declspec(align(__value))
  #define VOS_ALIGN_POST(__value)

#else

  #error Unsupported compiler!!!

#endif

#endif //                    

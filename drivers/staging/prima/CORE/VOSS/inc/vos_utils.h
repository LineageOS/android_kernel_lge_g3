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
#if !defined( __VOS_UTILS_H )
#define __VOS_UTILS_H
 
/**=========================================================================
  
  \file  vos_utils.h
  
  \brief virtual Operating System Services (vOSS) utility APIs
               
   Various utility functions
  
   Copyright 2008 (c) Qualcomm, Incorporated.  All Rights Reserved.
   
   Qualcomm Confidential and Proprietary.
  
  ========================================================================*/

/*          */

/*                                                                          
               
                                                                          */
#include <vos_types.h>
#include <vos_status.h>
//                     

/*                                                                           
                                        
                                                                          */
#define VOS_DIGEST_SHA1_SIZE    20
#define VOS_DIGEST_MD5_SIZE     16

/*                                                                           
                   
                                                                          */


/*                                                                          
                                        
                                                                          */

VOS_STATUS vos_crypto_init( v_U32_t *phCryptProv );

VOS_STATUS vos_crypto_deinit( v_U32_t hCryptProv );



/* 
                     

            
                                                        
  
  
                                                                                
                                                                   
         
   
                                                      
*/
VOS_STATUS vos_rand_get_bytes( v_U32_t handle, v_U8_t *pbBuf, v_U32_t numBytes );


/* 
                    
  
            
                                                  
  
         
                                                                      
                       
  
               
                      
  
                                
                                
                                 
                              
                                                
  
                                                      
  
 */
VOS_STATUS vos_sha1_hmac_str(v_U32_t cryptHandle, /*        */
           v_U8_t *text, /*                        */
           v_U32_t textLen, /*                       */
           v_U8_t *key, /*                               */
           v_U32_t keyLen, /*                              */
           v_U8_t digest[VOS_DIGEST_SHA1_SIZE]);/*                               */

/* 
                   
  
            
                                                 
  
         
                                                                      
                       
  
               
                      
  
                                
                                
                                 
                              
                                               
  
                                                      
  
 */
VOS_STATUS vos_md5_hmac_str(v_U32_t cryptHandle, /*        */
           v_U8_t *text, /*                        */
           v_U32_t textLen, /*                       */
           v_U8_t *key, /*                               */
           v_U32_t keyLen, /*                              */
           v_U8_t digest[VOS_DIGEST_MD5_SIZE]);/*                               */



VOS_STATUS vos_encrypt_AES(v_U32_t cryptHandle, /*        */
                           v_U8_t *pText, /*                        */
                           v_U8_t *Encrypted,
                           v_U8_t *pKey); /*                               */


VOS_STATUS vos_decrypt_AES(v_U32_t cryptHandle, /*        */
                           v_U8_t *pText, /*                        */
                           v_U8_t *pDecrypted,
                           v_U8_t *pKey); /*                               */

#endif //                            

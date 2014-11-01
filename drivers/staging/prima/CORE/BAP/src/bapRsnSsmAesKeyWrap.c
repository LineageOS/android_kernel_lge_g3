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

#include "vos_types.h"
#include "bapRsnSsmServices.h"
#include "bapRsnSsmEapol.h"
#include "bapRsnErrors.h"
#include "bapInternal.h"
#include "bapRsn8021xFsm.h"
#include "bapRsn8021xAuthFsm.h"
#include "vos_utils.h"
#include "vos_memory.h"
#include "vos_timer.h"
#include "bapRsnTxRx.h"
#include "bapRsnSsmAesKeyWrap.h"

#if 0

#include <assert.h>
#include <stdlib.h>
#include <openssl/aes.h>

#include <aniAsfHdr.h>
#include <aniUtils.h>
#include <aniErrors.h>
#include <aniAsfLog.h>

#include "aniSsmAesKeyWrap.h"
#include "aniSsmUtils.h"
#endif

#define ANI_SSM_AES_KEY_WRAP_IC_SIZE    ANI_SSM_AES_KEY_WRAP_BLOCK_SIZE

typedef struct tag_aes_key {
    tANI_U32 eK[64], dK[64];
    int Nr;
} AES_KEY;

static tANI_U8 gAniSsmAesKeyWrapIv[] = {
    0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6
};

static int
aes(v_U32_t cryptHandle, tANI_U8 *keyBytes, tANI_U32 keyLen,
    tANI_U8 a[ANI_SSM_AES_KEY_WRAP_BLOCK_SIZE],
    tANI_U8 ri[ANI_SSM_AES_KEY_WRAP_BLOCK_SIZE],
    tANI_U8 b[AES_BLOCK_SIZE]);

static int
aes_1(v_U32_t cryptHandle, tANI_U8 *keyBytes, tANI_U32 keyLen,
      tANI_U8 at[ANI_SSM_AES_KEY_WRAP_BLOCK_SIZE],
      tANI_U8 ri[ANI_SSM_AES_KEY_WRAP_BLOCK_SIZE],
      tANI_U8 b[AES_BLOCK_SIZE]);

static int
xor(tANI_U8 a[ANI_SSM_AES_KEY_WRAP_BLOCK_SIZE], tANI_U32 t);

/* 
                                                               
                                                     
                                                                  
                                                             
                                                                    
         
   
                                              
                                                                      
                                   
                                      
                                              
                                                                    
                                                           
                               
  
                                           
 */
int
aniSsmAesKeyWrap(v_U32_t cryptHandle, tANI_U8 *plainText, tANI_U32 len,
                 tANI_U8 *keyEncKey, tANI_U32 keyEncKeyLen,
                 tANI_U8 **cipherTextPtr)
{
    int i, j, n;
    int retVal;

    tANI_U8 a[ANI_SSM_AES_KEY_WRAP_BLOCK_SIZE];
    tANI_U8 *r = NULL;
    tANI_U32 t;

    tANI_U8 b[ANI_SSM_AES_KEY_WRAP_BLOCK_SIZE*2];

    n = len / ANI_SSM_AES_KEY_WRAP_BLOCK_SIZE;
    if ((len % ANI_SSM_AES_KEY_WRAP_BLOCK_SIZE) != 0) {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                 "Illegal number of input bytes to AES Key Wrap!");
        return ANI_E_ILLEGAL_ARG;
    }

    //                                               
    r = vos_mem_malloc((n + 1) * ANI_SSM_AES_KEY_WRAP_BLOCK_SIZE);
    if (r == NULL) {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                 "Could not allocate space for R");
        return ANI_E_MALLOC_FAILED;
    }

    vos_mem_copy(a, gAniSsmAesKeyWrapIv, sizeof(a));
    vos_mem_copy(r + ANI_SSM_AES_KEY_WRAP_BLOCK_SIZE, plainText, len);

    for (j = 0; j <= 5; j++) {
        for (i = 1; i <= n; i++) {

            retVal = aes(cryptHandle, keyEncKey, keyEncKeyLen, 
                         a,
                         r + i*ANI_SSM_AES_KEY_WRAP_BLOCK_SIZE,
                         b);

           if( !ANI_IS_STATUS_SUCCESS( retVal) ) goto error;

            vos_mem_copy(a, b, ANI_SSM_AES_KEY_WRAP_BLOCK_SIZE);
            t = n*j + i;
            xor(a, t);
            vos_mem_copy(r + i*ANI_SSM_AES_KEY_WRAP_BLOCK_SIZE,
                   b + sizeof(b) - ANI_SSM_AES_KEY_WRAP_BLOCK_SIZE,
                   ANI_SSM_AES_KEY_WRAP_BLOCK_SIZE);
        }
    }

    vos_mem_copy(r, a, ANI_SSM_AES_KEY_WRAP_BLOCK_SIZE);
    *cipherTextPtr = r;

    return ANI_OK;

 error:
    if (r != NULL)
        vos_mem_free(r);

    return retVal;

}

/* 
                                                                 
                                                          
                                                                  
                                                                      
                                                                    
                  
  
                                                 
                                                                       
                                   
                                      
                                              
                                                                   
                                                           
                               
  
                                           
 */
int
aniSsmAesKeyUnwrap(v_U32_t cryptHandle, tANI_U8 *cipherText, tANI_U32 len,
                   tANI_U8 *keyEncKey, tANI_U32 keyEncKeyLen,
                   tANI_U8 **plainTextPtr)
{
    int i, j;
    int retVal;

    tANI_U8 a[ANI_SSM_AES_KEY_WRAP_BLOCK_SIZE];
    tANI_U8 *r = NULL;
    tANI_U32 n;
    tANI_U32 t;

    tANI_U8 b[ANI_SSM_AES_KEY_WRAP_BLOCK_SIZE*2];

    n = len / ANI_SSM_AES_KEY_WRAP_BLOCK_SIZE - 1;
    if ((len % ANI_SSM_AES_KEY_WRAP_BLOCK_SIZE) != 0) {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                   "Illegal number of input bytes to AES Key Unwrap!");
        return ANI_E_ILLEGAL_ARG;
    }

    //                                               
    r = vos_mem_malloc((n + 1) *  ANI_SSM_AES_KEY_WRAP_BLOCK_SIZE);
    if (r == NULL) {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                   "Could not allocate space for R");
        return ANI_E_MALLOC_FAILED;
    }

    vos_mem_copy(a, cipherText, sizeof(a));
    vos_mem_copy(r + ANI_SSM_AES_KEY_WRAP_BLOCK_SIZE, 
           cipherText + ANI_SSM_AES_KEY_WRAP_BLOCK_SIZE, 
           len - ANI_SSM_AES_KEY_WRAP_BLOCK_SIZE);

    for (j = 5; j >= 0; j--) {
        for (i = n; i >= 1; i--) {

            t = n*j + i;
            xor(a, t);
            retVal = aes_1(cryptHandle, keyEncKey, keyEncKeyLen, 
                           a,
                           r + i*ANI_SSM_AES_KEY_WRAP_BLOCK_SIZE,
                           b);
           if( !ANI_IS_STATUS_SUCCESS( retVal) ) goto error;

            vos_mem_copy(a, b, ANI_SSM_AES_KEY_WRAP_BLOCK_SIZE);
            vos_mem_copy(r + i*ANI_SSM_AES_KEY_WRAP_BLOCK_SIZE,
                   b + sizeof(b) - ANI_SSM_AES_KEY_WRAP_BLOCK_SIZE,
                   ANI_SSM_AES_KEY_WRAP_BLOCK_SIZE);
        }
    }

    if (vos_mem_compare2(a, gAniSsmAesKeyWrapIv, ANI_SSM_AES_KEY_WRAP_BLOCK_SIZE) != 0) {
        retVal = ANI_E_MIC_FAILED;
        goto error;
    }

    *plainTextPtr = r;

    return ANI_OK;

 error:
    if (r != NULL)
        vos_mem_free(r);

    return retVal;
}

static int
aes(v_U32_t cryptHandle, tANI_U8 *keyBytes, tANI_U32 keyLen,
    tANI_U8 a[ANI_SSM_AES_KEY_WRAP_BLOCK_SIZE],
    tANI_U8 ri[ANI_SSM_AES_KEY_WRAP_BLOCK_SIZE],
    tANI_U8 b[AES_BLOCK_SIZE]) {

    int retVal = 0;

//                   

    tANI_U8 in[AES_BLOCK_SIZE];
    tANI_U8 *out;

    VOS_ASSERT (AES_BLOCK_SIZE == ANI_SSM_AES_KEY_WRAP_BLOCK_SIZE*2);

    //                       
    vos_mem_copy(in, a, ANI_SSM_AES_KEY_WRAP_BLOCK_SIZE);
    vos_mem_copy(in + ANI_SSM_AES_KEY_WRAP_BLOCK_SIZE, 
           ri, ANI_SSM_AES_KEY_WRAP_BLOCK_SIZE);
    out = b;

#if 0
    retVal = AES_set_encrypt_key(keyBytes, keyLen*8, &aesKey);
    if (retVal != 0) {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                   "AES_set_encrypt_key returned %d", retVal);
        return ANI_E_FAILED;
    }

    AES_encrypt(in, out, &aesKey);
#else //                           
    retVal = vos_encrypt_AES(cryptHandle, /*        */
                             in, /*       */
                             out, /*        */
                             keyBytes); /*     */
    if (retVal != 0) {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                   "vos_encrypt_AES returned %d", retVal);
        return ANI_E_FAILED;
    }
#endif
    return ANI_OK;
}

static int
aes_1(v_U32_t cryptHandle, tANI_U8 *keyBytes, tANI_U32 keyLen,
      tANI_U8 at[ANI_SSM_AES_KEY_WRAP_BLOCK_SIZE],
      tANI_U8 ri[ANI_SSM_AES_KEY_WRAP_BLOCK_SIZE],
      tANI_U8 b[AES_BLOCK_SIZE]) {

    int retVal;

//                   

    tANI_U8 in[AES_BLOCK_SIZE];
    tANI_U8 *out;

    VOS_ASSERT (AES_BLOCK_SIZE == ANI_SSM_AES_KEY_WRAP_BLOCK_SIZE*2);

    //                       
    vos_mem_copy(in, at, ANI_SSM_AES_KEY_WRAP_BLOCK_SIZE);
    vos_mem_copy(in + ANI_SSM_AES_KEY_WRAP_BLOCK_SIZE, 
           ri, ANI_SSM_AES_KEY_WRAP_BLOCK_SIZE);
    out = b;

#if 0
    retVal = AES_set_decrypt_key(keyBytes, keyLen*8, &aesKey);
    if (retVal != 0) {
        ANI_SSM_LOG_E("AES_set_encrypt_key returned %d", retVal);
        assert(0 && "AES_set_encrypt_key failed!");
        return ANI_E_FAILED;
    }

    AES_decrypt(in, out, &aesKey);
#else
    retVal = vos_decrypt_AES(cryptHandle, /*        */
                             in, /*       */
                             out, /*        */
                             keyBytes); /*     */
    if (retVal != 0) {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                   "vos_decrypt_AES returned %d", retVal);
    }
#endif
    return ANI_OK;
}

//                        



/*
                                                                
   
 */
static inline char *aniAsfWr32(char *cp, tANI_U32 x)
{
    tAniU32ValAry r;
    int i;

    r.val = vos_cpu_to_be32(x);
    i = sizeof(tANI_U32) - 1;
    cp[3] = r.ary[i--];
    cp[2] = r.ary[i--];
    cp[1] = r.ary[i--];
    cp[0] = r.ary[i];

    return (cp + sizeof(tANI_U32));
}

//                         

/*
                                                                
   
 */
char *aniAsfPut32(char *cp, tANI_U32 x)
{
    return(aniAsfWr32(cp, x));
}


static int
xor(tANI_U8 a[ANI_SSM_AES_KEY_WRAP_BLOCK_SIZE], tANI_U32 t)
{
    tANI_U8 tmp[4];
    aniAsfPut32((char *)tmp, t);
    a[ANI_SSM_AES_KEY_WRAP_BLOCK_SIZE-1] ^= tmp[3];
    a[ANI_SSM_AES_KEY_WRAP_BLOCK_SIZE-2] ^= tmp[2];
    a[ANI_SSM_AES_KEY_WRAP_BLOCK_SIZE-3] ^= tmp[1];
    a[ANI_SSM_AES_KEY_WRAP_BLOCK_SIZE-4] ^= tmp[0];
    return ANI_OK;
}


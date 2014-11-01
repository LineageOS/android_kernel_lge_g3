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
#include "vos_trace.h"
#include <bapRsnErrors.h>

#include <bapRsnSsmReplayCtr.h>
#include "vos_status.h"
#include "vos_memory.h"
#include "vos_utils.h"
#include "vos_packet.h"

//                        

/*
                                                                 
                                                                       
                                                                    
                                                                      
                                                                   
                                                                         
 */
struct sAniSsmReplayCtr {
    v_U8_t size;
    v_U8_t *buf;
    v_U32_t currentValue;
    v_U8_t init;
};

static int
updateCtrBuf(tAniSsmReplayCtr *ctr);

/* 
                        
  
                                                             
                                                                   
                                                                    
                                                                    
                                                                  
         
  
                                                                  
                                    
                                                                  
                                                                   
                                                                  
                                                               
                                                                      
                                                                 
               
  
                                           
 */
int
aniSsmReplayCtrCreate(v_U32_t cryptHandle, tAniSsmReplayCtr **ctrPtr, 
                      v_U8_t size,
                      int initValue)
{
    tAniSsmReplayCtr *ctr;

    ctr = vos_mem_malloc( sizeof(tAniSsmReplayCtr) );
    if( NULL == ctr )
    {
        return ANI_E_MALLOC_FAILED;
    }

    ctr->buf = vos_mem_malloc( size );
    if (ctr->buf == NULL) 
    {
        VOS_ASSERT( 0 );
        vos_mem_free(ctr);
        return ANI_E_MALLOC_FAILED;
    }

    ctr->size = size;

    //                                                              
    //                                                              
    if (initValue < 0 && ctr->size <= 4)
        initValue = 0;

    //                                                            
    //                                          
    if (initValue < 0) 
    {
        if( !VOS_IS_STATUS_SUCCESS( vos_rand_get_bytes(cryptHandle, ctr->buf, ctr->size) ) )
        {
            return ANI_ERROR;
        }
    } 
    else {
        ctr->currentValue = initValue - 1;
    }

    *ctrPtr = ctr;

    return ANI_OK;
}

static int
updateCtrBuf(tAniSsmReplayCtr *ctr)
{

    v_U32_t numBytes;
    v_U32_t offset;
    v_U32_t tmp;

    tmp = vos_cpu_to_be32( ctr->currentValue );

    numBytes = (4 <= ctr->size) ? 4 : ctr->size;
    offset = 4 - numBytes;
    vos_mem_copy(ctr->buf + ctr->size - numBytes, 
           ((v_U8_t *) &tmp) + offset, numBytes);

    return ANI_OK;
}

/* 
                     
  
                                                  
                                                       
  
                                                               
                                                               
  
                                                               
                                                             
                                                                 
                                                             
  
                                        
                                          
  
                                                           
                                                                      
                                                           
 */
int
aniSsmReplayCtrCmp(tAniSsmReplayCtr *ctr, v_U8_t *value)
{
    return vos_mem_compare2(ctr->buf, value, ctr->size);
}

/* 
                        
  
                                                                
                                                                  
                                                                   
                                                     
  
                                        
                                                  
  
                                           
 */
int
aniSsmReplayCtrUpdate(tAniSsmReplayCtr *ctr,
                      v_U8_t *value)
{
    vos_mem_copy(ctr->buf, value, ctr->size);

    return ANI_OK;
}

/* 
                      
  
                                                                   
                                                                    
          
  
                                        
                                                             
                                                           
  
                                           
 */
int
aniSsmReplayCtrNext(tAniSsmReplayCtr *ctr,
                    v_U8_t *value)
{
    ctr->currentValue++;
    updateCtrBuf(ctr);
    vos_mem_copy(value, ctr->buf, ctr->size);

    return ANI_OK;
}

/* 
                      
  
                                    
  
                                         
  
                                           
 */
int
aniSsmReplayCtrFree(tAniSsmReplayCtr *ctr)
{

    if (ctr->buf != NULL)
        vos_mem_free(ctr->buf);

    vos_mem_free(ctr);

    return ANI_OK;
}

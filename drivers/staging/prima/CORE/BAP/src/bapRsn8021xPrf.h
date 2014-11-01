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

#ifndef __AAG_PRF_H_
#define __AAG_PRF_H_

#include "vos_types.h"
#include <bapRsnAsfPacket.h>
#include <bapRsnSsmEapol.h>
#include "csrApi.h"


/*
                                                                    
                             
 */
#define AAG_PRF_MAX_OUTPUT_SIZE 80

#define AAG_RSN_PTK_TK1_OFFSET \
    (AAG_RSN_EAPOL_MIC_KEY_LEN + AAG_RSN_EAPOL_ENC_KEY_LEN)

//                                    

#define AAG_RSN_PTK_PRF_LEN_TKIP   512 //    
#define AAG_RSN_PTK_PRF_LEN_CCMP   384 //    
#define AAG_RSN_PTK_PRF_LEN_WEP104 384 //    
#define AAG_RSN_PTK_PRF_LEN_WEP40  384 //    

//                                 

#define AAG_RSN_GMK_SIZE 16

#define AAG_RSN_GTK_PRF_LEN_TKIP   256 //    
#define AAG_RSN_GTK_PRF_LEN_CCMP   128 //    
#define AAG_RSN_GTK_PRF_LEN_WEP104 128 //    
#define AAG_RSN_GTK_PRF_LEN_WEP40  128 //    

//                                                     

#define AAG_RSN_KEY_MATERIAL_LEN_CCMP   16
#define AAG_RSN_KEY_MATERIAL_LEN_TKIP   32
#define AAG_RSN_KEY_MATERIAL_LEN_WEP104 13
#define AAG_RSN_KEY_MATERIAL_LEN_WEP40  5

/* 
                       
  
                                                                     
                                         
  
                                    
  
                                                                    
                                 
 */
int
aagGetKeyMaterialLen(eCsrEncryptionType cipherType);

/* 
            
  
                                                                    
           
  
                                                                     
                                            
                                                               
                                     
                                                       
                                                    
                                                         
                                                      
  
                                           
 */
int
aagPtkPrf(v_U32_t cryptHandle,
          v_U8_t result[AAG_PRF_MAX_OUTPUT_SIZE],
          v_U32_t prfLen,
          tAniPacket *pmk, 
          tAniMacAddr authAddr,
          tAniMacAddr suppAddr,
          v_U8_t aNonce[ANI_EAPOL_KEY_RSN_NONCE_SIZE],
          v_U8_t sNonce[ANI_EAPOL_KEY_RSN_NONCE_SIZE]);


/* 
            
  
                                                                 
           
  
                                                                     
                                            
                                                               
                                  
                                                       
                                                                          
  
                                           
 */
int
aagGtkPrf(v_U32_t cryptHandle,
          v_U8_t result[AAG_PRF_MAX_OUTPUT_SIZE],
          v_U32_t prfLen,
          v_U8_t gmk[AAG_RSN_GMK_SIZE],
          tAniMacAddr authAddr,
          v_U8_t gNonce[ANI_EAPOL_KEY_RSN_NONCE_SIZE]);

/* 
         
  
                                                     
  
                                                                     
                                            
                                       
                                      
                                                           
                                            
                            
                                        
                                                               
  
                                           
 */
int
aagPrf(v_U32_t cryptHandle,
       v_U8_t result[AAG_PRF_MAX_OUTPUT_SIZE],
       v_U8_t *key, v_U8_t keyLen,
       v_U8_t *a, v_U8_t aLen,
       v_U8_t *b, v_U8_t bLen,
       v_U32_t prfLen);

#endif //            

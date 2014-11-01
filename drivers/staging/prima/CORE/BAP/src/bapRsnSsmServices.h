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

#ifndef _ANI_SSM_SERVICES_H_
#define _ANI_SSM_SERVICES_H_

#include "vos_types.h"
#include "sirApi.h"

#define ANI_SSM_MAX_KEYS_INFO_SIZE 512
#define ANI_SSM_MAX_GROUP_SIZE     32
#define ANI_SSM_MAX_USERID_SIZE    64

#define ANI_SSM_RSN_PMK_LEN 32
#define ANI_SSM_RSN_PSK_LEN ANI_SSM_RSN_PMK_LEN
#define ANI_SSM_RSN_PSK_LEN_HEX (ANI_SSM_RSN_PSK_LEN + ANI_SSM_RSN_PSK_LEN)
#define ANI_SSM_MAX_PASSPHRASE_LEN 128
#define ANI_SSM_MAX_AUTHZONE_LEN  32
#define ANI_SSM_MAX_LANDPG_URL_LEN 128
#define ANI_SSM_MAX_GUEST_PORTAL_PWD_LEN 32

#define ANI_SSM_IE_RSN_OUI {0x00, 0x0F, 0xAC}
#define ANI_SSM_IE_WPA_OUI {0x00, 0x50, 0xF2}

#define ANI_SSM_IE_RSN_ELEM_ID  48
#define ANI_SSM_IE_WPA_ELEM_ID  221

/*
                                                                 
                                                    
 */
#define ANI_RSN_IE_MAX_LEN 257

/*
                     
                                     
 */
#define ANI_AAG_PMKID_SIZE 16

#define ANI_SSM_AUTH_BITMASK   0x00010000
#define ANI_SSM_IEMODE_BITMASK 0xC0000000
#define ANI_SSM_ENCR_BITMASK   0x00000001
#define ANI_SSM_IEMODE_SHIFT   (30)


//                                            
typedef enum eAniSsmAuthType {
    eANI_SSM_AT_UNDERFLOW = -1,

    //                                                              
    //                         
    eANI_SSM_AT_NONE       = 0,
    eANI_SSM_AT_SHARED_KEY = 1,
    eANI_SSM_AT_LEGACY_EAP = 2,
    eANI_SSM_AT_RSN_PSK    = 3,
    eANI_SSM_AT_RSN_EAP    = 4,

    eANI_SSM_AT_OVERFLOW
} tAniSsmAuthType;

//                                        
typedef enum eAniSsmCipherType {
    eANI_SSM_CT_UNDERFLOW = -1,

    //                                                              
    //                         
    eANI_SSM_CT_NONE       = 0,
    eANI_SSM_CT_WEP40      = 1,
    eANI_SSM_CT_WEP104     = 2,
    eANI_SSM_CT_WPA_WEP40  = 3,
    eANI_SSM_CT_WPA_WEP104 = 4,
    eANI_SSM_CT_TKIP       = 5,
    eANI_SSM_CT_CCMP       = 6,

    eANI_SSM_CT_OVERFLOW
} tAniSsmCipherType;


//          
typedef enum eAniSsmWpaModes {
    eANI_SSM_WPA_UNDERFLOW = -1,

    eANI_SSM_WPA_DISABLE = 0,
    eANI_SSM_WPA_1 = 1,
    eANI_SSM_WPA_2 = 2,

    eANI_SSM_WPA_OVERFLOW = ((eANI_SSM_WPA_2 | eANI_SSM_WPA_1)  + 1)
} tAniSsmWpaModes;

typedef struct sAniSsmGroup {
    v_U16_t len; //                                       
    v_U8_t  group[1];
} tAniSsmGroup;

typedef struct sAniSsmUserId {
    v_U16_t len; //                                        
    v_U8_t  userId[1];
} tAniSsmUserId;

/*
                     
                                     
 */
typedef v_U8_t tAniSsmPmkId[ANI_AAG_PMKID_SIZE];

/* 
                       
  
                                                                      
                                                                  
                       
  
                                                                        
                                                  
  
                                           
 */
int
aniSsmInitStaticConf(char *configFileName);

/* 
                        
  
                                                               
                                                                      
                                                                     
                                                                      
                                                                        
  
                                                       
  
                                             
                                                        
  
                                           
 */
v_BOOL_t
aniSsmIsStaMacAllowed(const tAniMacAddr staMac, const tAniSSID *ssid);

/* 
                         
  
                                                                   
                                                                      
          
  
                                                                      
                                                                   
                                                                      
                                                                     
                                                            
                                                               
  
                                                              
                                                             
                                                                    
                                                                
                                                                     
                                                                
                                                                  
                                                                 
                                                                      
         
   
                                                    
                                                                
                                                                     
                        
                                                                
           
                                                           
                                
  
                                                                   
                             
  
                               
 */
v_BOOL_t
aniSsmIsSecModeAllowed(v_U32_t secMode,
                       tAniAuthType macAuthType, 
                       v_U8_t ieLen,
                       v_U8_t *ieData,
                       v_BOOL_t bpIndicator,
                       v_BOOL_t wpsEnabled);

/* 
                        
  
                                                                      
                                                                  
                                                            
              
  
                                            
                                                              
  
                                                                      
                                                    
 */
int
aniSsmGenRsnSuiteList(v_U32_t secMode,
                      v_U8_t ieData[ANI_RSN_IE_MAX_LEN]);

/* 
                         
  
                                                                 
                                                            
                                                                  
                       
  
                                                              
  
                                                                    
                                              
  
                                                                     
                                                    
                                           
                                                                 
                                                                  
                                                           
                                                              
  
                                                                      
                                                    
 */
int
aniSsmGenRsnSuiteForBp(const v_U8_t *apIeData,
                       v_U8_t apIeLen,
                       v_U32_t bpRsnFlag,
                       v_BOOL_t bpPskFlag,
                       v_U8_t ieData[ANI_RSN_IE_MAX_LEN]);

/* 
                    
  
                                                                
                 
  
                                           
  
                                                            
 */
v_U8_t *
aniSsmSecMode2Str(v_U32_t secMode);

/* 
               
  
                                                           
  
                             
                                    
  
                                           
 */
v_U8_t *
aniSsmIe2Str(const v_U8_t *ieData, v_U8_t ieLen);

#endif /*                      */

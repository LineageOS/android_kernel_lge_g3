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
#ifndef _ANI_SSM_EAPOL_H_
#define _ANI_SSM_EAPOL_H_

#include "vos_types.h"
#include "vos_trace.h"
#include "vos_packet.h"
#include <bapRsnAsfPacket.h>

#define ANI_ETH_P_EAPOL 0x0003
#define ANI_ETH_P_IP 0x0800

/* 
                                                
 */
#define ANI_EAPOL_TYPE_PACKET 0
#define ANI_EAPOL_TYPE_START 1
#define ANI_EAPOL_TYPE_LOGOFF 2
#define ANI_EAPOL_TYPE_KEY 3
#define ANI_EAPOL_TYPE_ASF_ALERT 4

#define EAPOL_VERSION_1 0x01

#define EAPOL_RX_HEADER_SIZE    18
#define EAPOL_TX_HEADER_SIZE    26  //                
#define SNAP_HEADER_SIZE   8

#define ANI_EAPOL_KEY_DESC_TYPE_LEGACY_RC4   1
//                                                                 
//                                                              
//                         
#define ANI_EAPOL_KEY_DESC_TYPE_RSN        254
//                                              
#define ANI_EAPOL_KEY_DESC_TYPE_RSN_NEW      2

#define ANI_EAPOL_KEY_RSN_REPLAY_CTR_SIZE  8
#define ANI_EAPOL_KEY_RSN_NONCE_SIZE      32
#define ANI_EAPOL_KEY_RSN_IV_SIZE         16
#define ANI_EAPOL_KEY_RSN_RSC_SIZE         8
#define ANI_EAPOL_KEY_RSN_ID_SIZE          8
#define ANI_EAPOL_KEY_RSN_MIC_SIZE        16
#define ANI_EAPOL_KEY_RSN_ENC_KEY_SIZE 16

#define ANI_EAPOL_KEY_DESC_VERS_RC4    1
#define ANI_EAPOL_KEY_DESC_VERS_AES    2

#define ANI_EAPOL_KEY_RC4_REPLAY_CTR_SIZE 8
#define ANI_EAPOL_KEY_RC4_IV_SIZE        16
#define ANI_EAPOL_KET_RC4_SIGN_SIZE      16

#define ANI_SSM_IE_RSN_KEY_DATA_ENCAPS_ID       0xDD
#define ANI_SSM_IE_RSN_GROUP_KEY_DATA_ENCAPS_ID 1
#define ANI_SSM_GROUP_KEY_KDE_TX_BIT            0x04

typedef struct sAniEapolLegacyRc4KeyDesc {
    v_U16_t keyLen;
    v_U8_t  replayCounter[ANI_EAPOL_KEY_RC4_REPLAY_CTR_SIZE];
    v_U8_t  keyIv[ANI_EAPOL_KEY_RC4_IV_SIZE];
    tANI_BOOLEAN unicastFlag; //                                  
    v_U8_t  keyId; //                                                     
    v_U8_t  signature[ANI_EAPOL_KET_RC4_SIGN_SIZE];
    v_U8_t  *key;
} tAniEapolLegacyRc4KeyDesc;

typedef struct sAniRsnKeyInfo {
    v_U32_t keyDescVers;
    tANI_BOOLEAN unicastFlag; //              
    v_U16_t keyId;
    tANI_BOOLEAN installFlag;
    tANI_BOOLEAN ackFlag;
    tANI_BOOLEAN micFlag;
    tANI_BOOLEAN secureFlag;
    tANI_BOOLEAN errorFlag;
    tANI_BOOLEAN requestFlag;
    tANI_BOOLEAN encKeyDataFlag; //                       
} tAniRsnKeyInfo;

typedef struct sAniEapolRsnKeyDesc {
    tAniRsnKeyInfo info;
    v_U16_t keyLen;
    v_U8_t  replayCounter[ANI_EAPOL_KEY_RSN_REPLAY_CTR_SIZE];
    v_U8_t  keyNonce[ANI_EAPOL_KEY_RSN_NONCE_SIZE];
    v_U8_t  keyIv[ANI_EAPOL_KEY_RSN_IV_SIZE];
    v_U8_t  keyRecvSeqCounter[ANI_EAPOL_KEY_RSN_RSC_SIZE];
    v_U8_t  keyId[ANI_EAPOL_KEY_RSN_ID_SIZE];
    v_U8_t  keyMic[ANI_EAPOL_KEY_RSN_MIC_SIZE];
    v_U16_t keyDataLen;
    v_U8_t  *keyData;
} tAniEapolRsnKeyDesc;

/* 
                     
  
            
                                                                    
              
  
         
                                                                  
                                        
  
               
                                                                   
  
                                                                
                                                                   
                                                           
  
                                           
 */
int
aniEapolWriteStart(tAniPacket *packet,
                   tAniMacAddr dstMac,
                   tAniMacAddr srcMac);

/* 
                         
  
            
                                                        
                                                                      
                                                        
  
         
                                                      
  
               
                                                                 
                                                            
                               
  
                                                      
                                                                   
                                                           
  
                                           
 */
int
aniEapolWriteEapPacket(tAniPacket *eapPacket, 
                       tAniMacAddr dstMac, 
                       tAniMacAddr srcMac);

/* 
                
  
            
                                                              
                        
  
                                                    
                                                                   
                                                                       
                        
  
                                                               
                                                                    
              
                                                               
              
                                                                 
         
  
                                                                        
           
 */
int 
aniEapolParse(tAniPacket *packet,
              v_U8_t **dstMac, 
              v_U8_t **srcMac, 
              v_U8_t **type);

/* 
                   
  
                                                               
                                                                     
                                                                    
             
  
                                       
                                            
                                       
                                          
                                         
                                
                                                                  
                                                             
                                                                      
                                
                            
                                                      
  
                                           
  
 */
int
aniEapolWriteKey(v_U32_t cryptHandle,
                 tAniPacket *packet,
                 tAniMacAddr dstMac, 
                 tAniMacAddr srcMac, 
                 int descType,
                 void *keyDescData,
                 v_U8_t *micKey,
                 v_U32_t micKeyLen);

/* 
                   
  
                                                                     
                                                                        
                                    
  
                                                                    
                                                                  
                                                                   
                                                                     
                
  
                                                                    
                                                                   
                                                                  
                
                                                    
                                         
                                
                                                                
                                                              
                                                                
                                                                   
                     
  
                                           
 */
int
aniEapolParseKey(tAniPacket *packet,
                 int *descType,
                 void **keyDescData);

/* 
                      
  
                                                  
                                          
                                        
                            
                                                      
  
                                                                    
                   
 */
int
aniEapolKeyCheckMic(v_U32_t cryptHandle,
                    tAniPacket *eapolFrame,
                    int descType,
                    void *keyDescData,
                    v_U8_t *micKey,
                    v_U32_t micKeyLen);

/* 
                      
  
                                                                        
  
                                          
                                        
  
                                           
 */
int
aniEapolKeyFreeDesc(int descType, void *keyDescData);

v_U8_t *
aniEapolType2Str(v_U8_t type);

v_U8_t *
aniEapolHdr2Str(v_U8_t *hdr);

/* 
                     
  
                                                         
  
                                          
                                        
  
                                           
 */
int
aniEapolKeyLogDesc(int descType, void *keyDescData);

void bapRsnEapolHandler( v_PVOID_t pvFsm, tAniPacket *packet, v_BOOL_t fIsAuth );
//                                       
int bapRsnFormPktFromVosPkt( tAniPacket **ppPacket, vos_pkt_t *pVosPacket );

#endif //                 

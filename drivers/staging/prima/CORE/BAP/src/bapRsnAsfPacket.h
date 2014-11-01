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
#ifndef _ANI_ASF_PACKET_H_
#define _ANI_ASF_PACKET_H_

#include "vos_types.h"
#include "palTypes.h"

#define ANI_ETH_FRAME_LEN 1516
#define ANI_DEFAULT_PACKET_SIZE (ANI_ETH_FRAME_LEN*2)

/* 
                                                               
                                                                  
                                                                     
                                                                    
                                                                     
                                                                     
                                      
 */
typedef struct tAniPacket tAniPacket;

/* 
                       
  
            
                                                                      
                                                                      
                                   
  
         
                                                          
                                                                    
                   
  
                                                               
                                     
  
                                                                   
                                 
                        
 */
int
aniAsfPacketAllocate(tAniPacket **packetPtr);

/* 
                        
  
                                                                     
                                                                     
                
  
                                                                   
                         
                                                                 
  
                                           
 */
int
aniAsfPacketDuplicate(tAniPacket **newPacketPtr, tAniPacket *oldPacket);

/* 
                               
  
            
                                                                   
                                                                   
                                                                    
                                                                
                                                                    
                       
  
         
                                                          
                                                                      
                                
  
                                                               
                                     
                                                       
                                                                     
                                                  
  
                                                                   
                                 
                        
 */
int
aniAsfPacketAllocateExplicit(tAniPacket **packetPtr,
                          v_U32_t size,
                          v_U32_t offset);

/* 
                   
  
            
                                                                
          
  
                                   
  
                                                                   
                                         
 */
int
aniAsfPacketFree(tAniPacket *packet);

/* 
                   
  
            
                                                                  
           
                                                                    
  
                                                             
 */
v_U8_t *aniAsfPacket2Str(tAniPacket *packet);

/* 
                           
  
            
                                                              
                                                                  
                           
  
                                                                   
                                                              
             
  
         
                                                                    
                                                                   
                                                                      
                          
  
                                        
                                                                     
                                           
  
                                                                
                                                                 
                                 
 */
int
aniAsfPacketAppendBuffer(tAniPacket *destPacket,
                      const v_U8_t *buf,
                      v_U32_t len);

/* 
                            
  
            
                                                                 
                                                                    
                                  
  
                                                                   
                                                              
             
  
         
                                                                     
                                                                   
                                                                      
                          
  
                                         
                                                                      
                                            
  
                                                                
                                                                 
                                
 */
int
aniAsfPacketPrependBuffer(tAniPacket *destPacket,
                       const v_U8_t *buf,
                       v_U32_t len);

/* 
                              
  
            
                                                               
                       
  
         
                                                                  
                         
  
                                                                     
        
  
                                        
                                           
  
                                                                     
            
 */
int
aniAsfPacketCanAppendBuffer(tAniPacket *destPacket,
                         v_U32_t len);

/* 
                               
  
            
                                                                
                       
  
         
                                                                  
                   
  
                                                                     
        
  
                                         
                                            
  
                                                                     
            
 */
int
aniAsfPacketCanPrependBuffer(tAniPacket *destPacket,
                          v_U32_t len);

/* 
                                
  
            
                                                               
                                                        
  
                                                      
                                             
  
                                                                     
            
 */
int
aniAsfPacketTruncateFromFront(tAniPacket *packet,
                              v_U32_t len);

/* 
                               
  
            
                                                              
                                                        
  
                                                     
                                             
  
                                                                     
            
 */
int
aniAsfPacketTruncateFromRear(tAniPacket *packet,
                             v_U32_t len);

/* 
                     
  
            
                                                               
  
                                             
  
                                                                
 */
int
aniAsfPacketGetLen(tAniPacket *packet);

/* 
                       
  
            
                                                                
          
  
                                               
                                                                   
             
  
                                                                   
                                                                   
                                  
 */
int
aniAsfPacketGetBytes(tAniPacket *packet, v_U8_t **rawBytesPtr);

/* 
                   
  
                                                                   
                      
  
                                        
                                       
                                                    
  
                                                                      
                                
 */
int
aniAsfPacketGetN(tAniPacket *packet, int n, v_U8_t **bytesPtr);

/* 
                    
  
            
                                                                      
                              
  
                                    
  
                                            
 */
int
aniAsfPacketEmpty(tAniPacket *packet);

/* 
                            
  
            
                                                                   
                                         
  
                                    
                                                                     
                                                                      
                  
  
                                            
 */
int
aniAsfPacketEmptyExplicit(tAniPacket *packet,
                          v_U32_t offset);


/* 
                         
  
            
                                                                      
                                                
  
                                       
                                                                 
  
                                           
 */
int
aniAsfPacketPrependHdr(tAniPacket *packet, v_U16_t msgType);

/* 
                    
  
            
                                                                      
                                                                    
                                                                    
  
                                        
                                  
  
                                           
 */
int
aniAsfPacketGet32(tAniPacket *packet, v_U32_t *val);

/* 
                       
  
            
                                              
                                                                    
  
                                       
                                 
  
                                           
 */
int
aniAsfPacketAppend32(tAniPacket *packet, v_U32_t val);

/* 
                    
  
            
                                                                      
                                                                    
                                                                    
  
                                        
                                  
  
                                           
 */
int
aniAsfPacketGet16(tAniPacket *packet, v_U16_t *val);

/* 
                        
  
            
                                                 
                                                                    
  
                                       
                                  
  
                                           
 */
int
aniAsfPacketPrepend16(tAniPacket *packet, v_U16_t val);

/* 
                       
  
            
                                              
                                                                    
  
                                       
                                 
  
                                           
 */
int
aniAsfPacketAppend16(tAniPacket *packet, v_U16_t val);

/* 
                   
  
            
                                                                     
                                                                    
                                                                    
  
                                        
                                  
  
                                           
 */
int
aniAsfPacketGet8(tAniPacket *packet, v_U8_t *val);

/* 
                       
  
            
                                                
                                                                    
  
                                        
                                  
  
                                           
 */
int
aniAsfPacketPrepend8(tAniPacket *packet, v_U8_t val);

/* 
                      
  
            
                                             
                                                                    
  
                                       
                                 
  
                                           
 */
int
aniAsfPacketAppend8(tAniPacket *packet, v_U8_t val);

/* 
                     
  
            
                                                      
  
                                        
                                                            
  
                                                                  
                                            
 */
int
aniAsfPacketGetMac(tAniPacket *packet, tAniMacAddr macAddr);

/* 
                       
  
            
                                                                     
                                                                     
                                                                  
                                                                
                
  
            
                                                        
                                                                     
                                                                   
                                                                 
  
                                                                      
                                                
  
                                           
 */
int
aniAsfPacketMoveLeft(tAniPacket *packet, v_U32_t count);

/* 
                        
  
            
                                                                    
                                                                     
                                                                  
                                                                
                
  
            
                                                        
                                                                     
                                                                   
                                                                 
  
                                                                      
                                                
  
                                           
 */
int
aniAsfPacketMoveRight(tAniPacket *packet, v_U32_t count);

/* 
                               
  
            
                                                          
                 
  
            
                                                        
                                                                 
                                                                  
                                                                 
                                                           
                                                                
                                      
  
                                               
                                                                   
             
  
                                                                   
                                                                   
                                  
 */
int
aniAsfPacketGetBytesFromTail(tAniPacket *packet, v_U8_t **rawBytesPtr);


#endif //                   

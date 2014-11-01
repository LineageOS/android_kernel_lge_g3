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
#include <bapRsnAsfPacket.h>
#include <bapRsnErrors.h>
#include "vos_memory.h"
#include "vos_packet.h"

/*
                                                                      
                                                                    
                                                                     
                                                                 
                     
 */
#define ANI_INTERNAL_DEFAULT_PACKET_SIZE (ANI_DEFAULT_PACKET_SIZE + 4)

#define TAIL_SPACE(packet) \
    ((packet)->buf + (packet)->size - (packet)->tail)

#define HEAD_SPACE(packet) \
     ((packet)->head - (packet)->buf)

#define ANI_CHECK_RANGE(x , upper) \
                ( (x) <= (upper) )

/* 
                                                               
                                                                     
                                                                     
                                                                    
                                                                     
                                                                     
                                      
 */
struct tAniPacket {
    v_U8_t *buf;
    v_U32_t size;
    v_U8_t *head;
    v_U8_t *tail;
    v_U8_t *recordHeader;
    v_U32_t len;
};

/* 
                       
  
            
                                                                      
                                                                      
                                   
  
         
                                                             
                                                                    
                   
  
                                                               
                                        
  
                                                                   
                                 
                        
 */
int
aniAsfPacketAllocate(tAniPacket **packetPtr)
{
  return aniAsfPacketAllocateExplicit(packetPtr,
                                   ANI_INTERNAL_DEFAULT_PACKET_SIZE,
                                   ANI_INTERNAL_DEFAULT_PACKET_SIZE/2);
}

/* 
                               
  
            
                                                                   
                                                                   
                                                                    
                                                                
                                                                    
                       
  
         
                                                             
                                                                      
                                
  
                                                               
                                        
                                                       
                                                                     
                                                  
  
                                                                   
                                 
                        
 */
int
aniAsfPacketAllocateExplicit(tAniPacket **packetPtr,
                             v_U32_t size,
                             v_U32_t offset)
{
  tAniPacket *packet = NULL;
  v_U32_t maxHead = size;

  *packetPtr = NULL;
  if (size == 0)
    return ANI_E_ILLEGAL_ARG;

  VOS_ASSERT(ANI_CHECK_RANGE(offset, maxHead));
  if (!ANI_CHECK_RANGE(offset, maxHead))
    return ANI_E_ILLEGAL_ARG;

  packet = (tAniPacket *) vos_mem_malloc( sizeof(tAniPacket) );

  if (packet == NULL) 
  {
      VOS_ASSERT( 0 );
      return ANI_E_MALLOC_FAILED;
  }

  //                                                            
  size = (size + 4) & 0xfffffffc;

  packet->buf = (v_U8_t *)vos_mem_malloc( sizeof(v_U8_t) * size );
  if (packet->buf == NULL) 
  {
      vos_mem_free( packet );
      VOS_ASSERT( 0 );
      return ANI_E_MALLOC_FAILED;
  }

  packet->size = size; //                                  
  packet->head = packet->buf + offset;
  packet->tail = packet->head;
  packet->len = 0;

  *packetPtr = packet;
  return ANI_OK;
}

/* 
                        
  
                                                                     
                                                                     
                
  
                                                                   
                         
                                                                 
  
                                                                    
            
 */
int
aniAsfPacketDuplicate(tAniPacket **newPacketPtr, tAniPacket *oldPacket)
{
    int retVal;
    int recordPos;
    tAniPacket *packet = NULL;

    if (oldPacket == NULL)
        return ANI_E_NULL_VALUE;

    retVal = aniAsfPacketAllocateExplicit(&packet,
                                          oldPacket->size,
                                          oldPacket->head - oldPacket->buf);
    if (retVal != ANI_OK)
        return retVal;

    retVal = aniAsfPacketAppendBuffer(packet,
                                      oldPacket->head,
                                      oldPacket->len);
    if (retVal != ANI_OK) 
    {
        VOS_ASSERT( 0 );
        aniAsfPacketFree(packet);
        return ANI_E_FAILED;
    }

    if (oldPacket->recordHeader != NULL) 
    {
        recordPos = oldPacket->recordHeader - oldPacket->buf;
        packet->recordHeader = packet->buf + recordPos;
    }
    *newPacketPtr = packet;

    return ANI_OK;
}

/* 
                   
  
            
                                                                   
          
  
                                   
  
                                                                   
                                         
 */
int
aniAsfPacketFree(tAniPacket *packet)
{
  if (packet == NULL)
    return ANI_E_NULL_VALUE;

  if (packet->buf != NULL)
    vos_mem_free( packet->buf );

  vos_mem_free( packet );

  return ANI_OK;
}


/* 
                           
  
            
                                                              
                                                                        
                           
  
                                                                      
                                                                 
             
  
         
                                                                    
                                                                   
                                                                      
                          
  
                                        
                                                                     
                                           
  
                                                                
                                                                 
                                 
 */
int
aniAsfPacketAppendBuffer(tAniPacket *destPacket,
                      const v_U8_t *buf,
                      v_U32_t len)
{
  if (aniAsfPacketCanAppendBuffer(destPacket, len) != ANI_OK)
      return ANI_E_FAILED;

  if (buf == NULL)
    return ANI_E_NULL_VALUE;

  vos_mem_copy(destPacket->tail, buf, len);
  destPacket->tail += len;
  destPacket->len += len;
  return ANI_OK;
}

/* 
                            
  
            
                                                                 
                                                                    
                                  
  
                                                                   
                                                              
             
  
         
                                                                     
                                                                   
                                                                      
                          
  
                                         
                                                                      
                                            
  
                                                                
                                                                 
                                
 */
int
aniAsfPacketPrependBuffer(tAniPacket *destPacket,
                       const v_U8_t *buf,
                       v_U32_t len)
{
  if (aniAsfPacketCanPrependBuffer(destPacket, len) != ANI_OK)
      return ANI_E_FAILED;

  if (buf == NULL)
      return ANI_E_NULL_VALUE;

  destPacket->head -= len;
  destPacket->len += len;
  vos_mem_copy(destPacket->head, buf, len);
  return ANI_OK;

}

/* 
                              
  
            
                                                               
                       
  
         
                                                                  
                         
  
                                                                     
        
  
                                        
                                           
  
                                                                     
            
 */
int
aniAsfPacketCanAppendBuffer(tAniPacket *destPacket,
                         v_U32_t len)
{
  if (destPacket == NULL)
    return ANI_E_FAILED;

  if ((int)len <= TAIL_SPACE(destPacket))
      return ANI_OK;
  else
      return ANI_E_FAILED;
}

/* 
                               
  
            
                                                                
                       
  
         
                                                                  
                   
  
                                                                     
        
  
                                         
                                            
  
                                                                     
            
 */
int
aniAsfPacketCanPrependBuffer(tAniPacket *destPacket,
                          v_U32_t len)
{
  if (destPacket == NULL)
      return ANI_E_FAILED;

  if (!(len > 0))
      return ANI_E_FAILED;

  if ((int)len <= HEAD_SPACE(destPacket))
      return ANI_OK;
  else
      return ANI_E_FAILED;
}

/* 
                                
  
            
                                                               
                                                        
  
                                                      
                                             
  
                                                                     
            
 */
int
aniAsfPacketTruncateFromFront(tAniPacket *packet,
                           v_U32_t len)
{
    if (packet == NULL)
        return ANI_E_NULL_VALUE;

    if (!ANI_CHECK_RANGE(len, packet->len))
        return ANI_E_FAILED;

    packet->head += len;
    packet->len -= len;

    return ANI_OK;
}

/* 
                               
  
            
                                                              
                                                        
  
                                                     
                                             
  
                                                                     
            
 */
int
aniAsfPacketTruncateFromRear(tAniPacket *packet,
                          v_U32_t len)
{
    if (packet == NULL)
        return ANI_E_NULL_VALUE;

    if (!ANI_CHECK_RANGE(len, packet->len))
        return ANI_E_FAILED;

    packet->tail -= len;
    packet->len -= len;

    return ANI_OK;
}

/* 
                     
  
            
                                                               
  
                                             
  
                                                                
 */
int
aniAsfPacketGetLen(tAniPacket *packet)
{
    if (packet == NULL)
        return ANI_E_NULL_VALUE;

    return packet->len;
}

/* 
                       
  
            
                                                                
          
  
                                               
                                                                   
             
  
                                                                   
                                                                   
                                  
 */
int
aniAsfPacketGetBytes(tAniPacket *packet, v_U8_t **rawBytesPtr)
{
    if (packet == NULL)
        return ANI_E_NULL_VALUE;

    *rawBytesPtr = packet->head;
    return packet->len;
}

/* 
                   
  
                                                                   
                      
  
                                        
                                       
                                                    
  
                                                                      
                                
 */
int
aniAsfPacketGetN(tAniPacket *packet, int n, v_U8_t **bytesPtr)
{
    int retVal;
    v_U8_t *bytes = NULL;

    if (packet == NULL)
        return ANI_E_NULL_VALUE;

    retVal = aniAsfPacketGetBytes(packet, &bytes);
    if (retVal < n)
        return ANI_E_SHORT_PACKET;

    aniAsfPacketTruncateFromFront(packet, n);

    *bytesPtr = bytes;

    return ANI_OK;
}

/* 
                    
  
            
                                                                      
                              
  
                                    
  
                                            
 */
int
aniAsfPacketEmpty(tAniPacket *packet)
{
    return aniAsfPacketEmptyExplicit(packet, packet->size/2);
}

/* 
                            
  
            
                                                                   
                                         
  
                                    
                                                                     
                                                                      
                  
  
                                            
 */
int
aniAsfPacketEmptyExplicit(tAniPacket *packet,
                       v_U32_t offset)
{
    if (packet == NULL)
        return ANI_E_NULL_VALUE;

    VOS_ASSERT(ANI_CHECK_RANGE(offset, packet->size));
    if (!ANI_CHECK_RANGE(offset, packet->size))
        return ANI_E_ILLEGAL_ARG;

    packet->head = packet->buf + offset;
    packet->tail = packet->head;
    packet->len = 0;

    return ANI_OK;
}



/* 
                         
  
            
                                                                      
                                                
  
                                       
                                                                 
  
                                           
 */
int
aniAsfPacketPrependHdr(tAniPacket *packet, v_U16_t msgType)
{
    int retVal;
    int length;

    if (packet == NULL)
        return ANI_E_NULL_VALUE;

    length = 4;

    length = 2 + 2 + packet->len;

    retVal = aniAsfPacketPrepend16(packet, length);
    if (retVal < 0)
        return retVal;

    retVal = aniAsfPacketPrepend16(packet, msgType);
    if (retVal < 0)
        return retVal;

    return ANI_OK;
}

/* 
                    
  
            
                                                                      
                                                                    
                                                                    
  
                                        
                                  
  
                                           
 */
int
aniAsfPacketGet32(tAniPacket *packet, v_U32_t *val)
{
    v_U8_t u32Arr[4];

    if (packet == NULL)
        return ANI_E_NULL_VALUE;

    if (val == NULL)
        return ANI_E_NULL_VALUE;

    if (packet->len < 4)
        return ANI_E_SHORT_PACKET;

    //                                                 
    u32Arr[0] = packet->head[0];
    u32Arr[1] = packet->head[1];
    u32Arr[2] = packet->head[2];
    u32Arr[3] = packet->head[3];
    *val = vos_be32_to_cpu( *(v_U32_t *)u32Arr );
    aniAsfPacketTruncateFromFront(packet, 4);

    return ANI_OK;
}

/* 
                       
  
            
                                              
                                                                    
  
                                       
                                 
  
                                           
 */
int
aniAsfPacketAppend32(tAniPacket *packet, v_U32_t val)
{
    v_U8_t *p8;

    if (packet == NULL)
        return ANI_E_NULL_VALUE;

    if (TAIL_SPACE(packet) < 4)
        return ANI_E_FAILED;

    val = vos_cpu_to_be32( val );
    p8 = (v_U8_t *)&val;
    packet->tail[0] =  p8[0];
    packet->tail[1] =  p8[1];
    packet->tail[2] =  p8[2];
    packet->tail[3] =  p8[3];
    aniAsfPacketMoveRight(packet, 4);

    return ANI_OK;
}

/* 
                    
  
            
                                                                      
                                                                    
                                                                    
  
                                        
                                  
  
                                           
 */
int
aniAsfPacketGet16(tAniPacket *packet, v_U16_t *val)
{
    v_U8_t u16Arr[2];

    if (packet == NULL)
        return ANI_E_NULL_VALUE;

    if (val == NULL)
        return ANI_E_NULL_VALUE;

    if (packet->len < 2)
        return ANI_E_SHORT_PACKET;

    u16Arr[0] = packet->head[0];
    u16Arr[1] = packet->head[1];
    *val = vos_be16_to_cpu( *(v_U16_t *)u16Arr );
    aniAsfPacketTruncateFromFront(packet, 2);

    return ANI_OK;
}

/* 
                        
  
            
                                                 
                                                                    
  
                                       
                                  
  
                                           
 */
int
aniAsfPacketPrepend16(tAniPacket *packet, v_U16_t val)
{
    v_U8_t *p8;

    if (packet == NULL)
        return ANI_E_NULL_VALUE;

    if (HEAD_SPACE(packet) < 2)
        return ANI_E_FAILED;

    aniAsfPacketMoveLeft(packet, 2);
    val = vos_cpu_to_be16( val );
    p8 = (v_U8_t *)&val;
    packet->head[0] =  p8[0];
    packet->head[1] =  p8[1];

    return ANI_OK;
}

/* 
                       
  
            
                                              
                                                                    
  
                                       
                                 
  
                                           
 */
int
aniAsfPacketAppend16(tAniPacket *packet, v_U16_t val)
{
    v_U8_t *p8;

    if (packet == NULL)
        return ANI_E_NULL_VALUE;

    if (TAIL_SPACE(packet) < 2)
        return ANI_E_FAILED;

    val = vos_cpu_to_be16( val );
    p8 = (v_U8_t *)&val;
    packet->tail[0] =  p8[0];
    packet->tail[1] =  p8[1];
    aniAsfPacketMoveRight(packet, 2);

    return ANI_OK;
}

/* 
                   
  
            
                                                                     
                                                                    
                                                                    
  
                                        
                                  
  
                                           
 */
int
aniAsfPacketGet8(tAniPacket *packet, v_U8_t *val)
{
    if (packet == NULL)
        return ANI_E_NULL_VALUE;

    if (val == NULL)
        return ANI_E_NULL_VALUE;

    if (packet->len < 1)
        return ANI_E_SHORT_PACKET;

    *val = *(packet->head);
    aniAsfPacketTruncateFromFront(packet, 1);

    return ANI_OK;
}

/* 
                       
  
            
                                                
                                                                    
  
                                        
                                  
  
                                           
 */
int
aniAsfPacketPrepend8(tAniPacket *packet, v_U8_t val)
{
    if (packet == NULL)
        return ANI_E_NULL_VALUE;

    VOS_ASSERT(HEAD_SPACE(packet) >= 1);
    if (HEAD_SPACE(packet) < 1)
        return ANI_E_FAILED;

    aniAsfPacketMoveLeft(packet, 1);
    *(packet->head) = val;

    return ANI_OK;
}

/* 
                      
  
            
                                             
                                                                    
  
                                       
                                 
  
                                           
 */
int
aniAsfPacketAppend8(tAniPacket *packet, v_U8_t val)
{
    if (packet == NULL)
        return ANI_E_NULL_VALUE;

    if (TAIL_SPACE(packet) < 1)
        return ANI_E_FAILED;

    *(packet->tail) = val;
    aniAsfPacketMoveRight(packet, 1);

    return ANI_OK;
}

/* 
                     
  
            
                                                      
  
                                        
                                                            
  
                                                                  
                                            
 */
int
aniAsfPacketGetMac(tAniPacket *packet, tAniMacAddr macAddr)
{
    if (packet->len < sizeof(tAniMacAddr))
        return ANI_E_SHORT_PACKET;

    vos_mem_copy(macAddr, packet->head, sizeof(tAniMacAddr));

    packet->head += sizeof(tAniMacAddr);
    packet->len -= sizeof(tAniMacAddr);

    return ANI_OK;
}

/* 
                       
  
            
                                                                     
                                                                     
                                                                  
                                                                
                
  
            
                                                        
                                                                     
                                                                   
                                                                 
  
                                                                      
                                                
  
                                           
 */
int
aniAsfPacketMoveLeft(tAniPacket *packet, v_U32_t count)
{
    if (aniAsfPacketCanPrependBuffer(packet, count) != ANI_OK)
        return ANI_E_FAILED;

    packet->head -= count;
    packet->len += count;

    return ANI_OK;
}

/* 
                        
  
            
                                                                    
                                                                     
                                                                  
                                                                
                
  
            
                                                        
                                                                     
                                                                   
                                                                 
  
                                                                      
                                                
  
                                           
 */
int
aniAsfPacketMoveRight(tAniPacket *packet, v_U32_t count)
{
    if (aniAsfPacketCanAppendBuffer(packet, count) != ANI_OK)
        return ANI_E_FAILED;

    packet->tail += count;
    packet->len += count;

    return ANI_OK;
}

/* 
                               
  
            
                                                          
                 
  
            
                                                        
                                                                 
                                                                  
                                                                 
                                                           
                                                                
                                      
  
                                               
                                                                   
             
  
                                                                   
                                                                   
                                  
 */
int
aniAsfPacketGetBytesFromTail(tAniPacket *packet, v_U8_t **rawBytesPtr)
{
    if (packet == NULL)
        return ANI_E_NULL_VALUE;

    *rawBytesPtr = packet->tail;
    return 0; //                                          
}


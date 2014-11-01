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



/**=========================================================================
  
  \file  wlan_qct_pal_packet.c
  
  \brief Implementation for PAL packet. wpt = (Wlan Pal Type) wpal = (Wlan PAL)
               
   Definitions for platform with VOSS packet support and LA.
  
   Copyright 2010 (c) Qualcomm, Incorporated.  All Rights Reserved.
   
   Qualcomm Confidential and Proprietary.
  
  ========================================================================*/

#include "wlan_qct_pal_packet.h"
#include "wlan_qct_pal_api.h"
#include "wlan_qct_pal_trace.h"
#include "vos_packet.h"
#include "vos_trace.h"
#include "vos_list.h"

#include <linux/skbuff.h>
#include "dma-mapping.h"

/*                   */
#define WPAL_ETHERNET_PAKCET_HEADER_SIZE     14

/*                                             */
#define WPAL_802_11_PACKET_HEADER_SIZE    24 

/*                            */
#define WPAL_TO_VOS_PKT(p) ((vos_pkt_t *)(p))


typedef struct
{
  void*      pPhyAddr;
  wpt_uint32 uLen;
}wpt_iterator_info;

/*                                     */
static wpalPacketLowPacketCB wpalPacketAvailableCB;

/*                                                        
                                                              
                                   
                                 */
static wpt_log_data_stall_type wpalTrasportStallInfo;

/*
                                                      
*/
wpt_status wpalPacketInit(void *pPalContext)
{
   return eWLAN_PAL_STATUS_SUCCESS;
}


/*
                                                       
*/
wpt_status wpalPacketClose(void *pPalContext)
{
   return eWLAN_PAL_STATUS_SUCCESS;
}

/*                                                                           
                                                         
           
                                     
                                          
           
              
                                                                           */
VOS_STATUS wpalPacketRXLowResourceCB(vos_pkt_t *pPacket, v_VOID_t *userData)
{
   VOS_STATUS   vosStatus = VOS_STATUS_E_FAILURE;
   void*        pData     = NULL;

   if (NULL == pPacket)
   {
      WPAL_TRACE(eWLAN_MODULE_PAL, eWLAN_PAL_TRACE_LEVEL_ERROR, 
                  "Get new RX PAL packet fail");
      return VOS_STATUS_E_FAILURE;
   }
   vosStatus = vos_pkt_reserve_head_fast( pPacket, &pData,
                                          VPKT_SIZE_BUFFER );
   if(VOS_STATUS_SUCCESS != vosStatus)
   {
      WPAL_TRACE(eWLAN_MODULE_PAL, eWLAN_PAL_TRACE_LEVEL_ERROR, 
                  "Prepare RX packet for DXE fail");
      return VOS_STATUS_E_FAILURE;
   }

   if((NULL == wpalPacketAvailableCB) || (NULL == userData))
   {
      WPAL_TRACE(eWLAN_MODULE_PAL, eWLAN_PAL_TRACE_LEVEL_ERROR, 
                  "Invalid ARG for new RX packet");
      return VOS_STATUS_E_FAILURE;
   }

   wpalPacketAvailableCB( (wpt_packet *)pPacket, userData );

   return VOS_STATUS_SUCCESS;
}

/*                                                                           
                                                     
           
                                                            
                              
           
                                                     
                                                                           */
wpt_packet * wpalPacketAlloc(wpt_packet_type pktType, wpt_uint32 nPktSize,
                             wpalPacketLowPacketCB rxLowCB, void *usrData)
{
   VOS_STATUS   vosStatus = VOS_STATUS_E_FAILURE;
   wpt_packet*  pPkt      = NULL;
   vos_pkt_t*   pVosPkt   = NULL;
   void*        pData     = NULL;
   v_U16_t      allocLen;
   /*                                                                      */

   switch (pktType)
   {
   case eWLAN_PAL_PKT_TYPE_TX_802_11_MGMT:
      vosStatus = vos_pkt_get_packet(&pVosPkt, VOS_PKT_TYPE_TX_802_11_MGMT,
                                       nPktSize, 1, VOS_FALSE, 
                                       NULL, NULL /*           */);
      break;

   case eWLAN_PAL_PKT_TYPE_RX_RAW:
      /*                                                         
                                                                         
                                                                    
                        
                                                                          
                                                                          
                                       
                                                                            
                                                                          
                                                                        
                                                                        
                                                                          
                                    
       */
      wpalPacketAvailableCB = rxLowCB;

      vosStatus = vos_pkt_get_packet(&pVosPkt, VOS_PKT_TYPE_RX_RAW,
                                       nPktSize, 1, VOS_FALSE, 
                                       wpalPacketRXLowResourceCB, usrData);

#ifndef FEATURE_R33D
      /*                                          */
      if( vosStatus == VOS_STATUS_SUCCESS )
      {
        wpalPacketAvailableCB = NULL;
        vosStatus =  vos_pkt_reserve_head_fast( pVosPkt, &pData, nPktSize ); 
      }
#endif /*              */
      if((NULL != pVosPkt) && (VOS_STATUS_E_RESOURCES != vosStatus))
      {
         vos_pkt_get_packet_length(pVosPkt, &allocLen);
         if (nPktSize != allocLen)
         {
            WPAL_TRACE(eWLAN_MODULE_PAL, eWLAN_PAL_TRACE_LEVEL_ERROR,
                       "RX packet alloc has problem, discard this frame, Len %d", allocLen);
            vos_pkt_return_packet(pVosPkt);
            return NULL;
         }
      }
      break;

   default:
      WPAL_TRACE(eWLAN_MODULE_PAL, eWLAN_PAL_TRACE_LEVEL_ERROR, 
                  " try to allocate unsupported packet type (%d)\n", pktType);
      break;
   }

   if(VOS_IS_STATUS_SUCCESS(vosStatus))
   {
      pPkt = (wpt_packet *)pVosPkt;
   }


   return pPkt;
}/*               */



/*                                                                           
                                                                     
                                                                                 
           
                                      
           
                                          
                                                                           */
wpt_status wpalPacketFree(wpt_packet *pPkt)
{
   VOS_STATUS vosStatus;

   if(NULL != pPkt->pInternalData)
   {
      wpalMemoryFree(pPkt->pInternalData);
   }
   vosStatus = vos_pkt_return_packet(WPAL_TO_VOS_PKT(pPkt));

   //                                                                
   return (wpt_status)vosStatus;
}/*              */


/*                                                                           
                                                                              
                              
           
                                               
           
                                                                             
                                                          
                                                                           */
wpt_uint32 wpalPacketGetLength(wpt_packet *pPkt)
{
   v_U16_t len = 0, pktLen = 0;

   //                                
   if (unlikely(NULL == pPkt))
   {
      WPAL_TRACE(eWLAN_MODULE_PAL, eWLAN_PAL_TRACE_LEVEL_ERROR,
                "%s : NULL packet pointer", __func__);
      return eWLAN_PAL_STATUS_E_INVAL;
   }


   if( WPAL_PACKET_GET_BD_POINTER(pPkt) )
   {
      len = WPAL_PACKET_GET_BD_LENGTH(pPkt);
   }
   if( VOS_IS_STATUS_SUCCESS(vos_pkt_get_packet_length(WPAL_TO_VOS_PKT(pPkt), &pktLen)) )
   {
      len += pktLen;
   }
   else
   {
      WPAL_TRACE(eWLAN_MODULE_PAL, eWLAN_PAL_TRACE_LEVEL_ERROR, "%s  failed\n",
         __func__);
   }

   return ((wpt_uint32)len);
}/*                   */


/*                                                                           
                                                                                
                                                                            
                                                                           
                                          
           
                                       
                                                    
           
                                                                  
                                                                           */
wpt_status wpalPacketRawTrimHead(wpt_packet *pPkt, wpt_uint32 size)
{
   wpt_status status = eWLAN_PAL_STATUS_SUCCESS;

   //                                
   if (unlikely(NULL == pPkt))
   {
      WPAL_TRACE(eWLAN_MODULE_PAL, eWLAN_PAL_TRACE_LEVEL_ERROR,
                "%s : NULL packet pointer", __func__);
      return eWLAN_PAL_STATUS_E_INVAL;
   }

   if ((eWLAN_PAL_PKT_TYPE_TX_802_11_MGMT == WPAL_PACKET_GET_TYPE(pPkt)) ||
               (eWLAN_PAL_PKT_TYPE_RX_RAW == WPAL_PACKET_GET_TYPE(pPkt)))
   {
       //                            
   }
   else
   {
      WPAL_TRACE(eWLAN_MODULE_PAL, eWLAN_PAL_TRACE_LEVEL_ERROR,
                "%s : neither 80211 managment packet nor RAW packet", __func__);
      VOS_ASSERT(0);
      return eWLAN_PAL_STATUS_E_INVAL;
   }

   if( !VOS_IS_STATUS_SUCCESS(vos_pkt_trim_head(WPAL_TO_VOS_PKT(pPkt), (v_SIZE_t)size)) )
   {
      WPAL_TRACE(eWLAN_MODULE_PAL, eWLAN_PAL_TRACE_LEVEL_ERROR, "%s  Invalid trim(%d)\n",
         __func__, size);
      status = eWLAN_PAL_STATUS_E_INVAL;
   }

   return status;
}/*                     */

/*                                                                           
                                                            
           
                                       
                                                           
           
                                                           
                                                                           */
wpt_status wpalPacketRawTrimTail(wpt_packet *pPkt, wpt_uint32 size)
{
   wpt_status status = eWLAN_PAL_STATUS_SUCCESS;

   //                                
   if (unlikely(NULL == pPkt))
   {
      WPAL_TRACE(eWLAN_MODULE_PAL, eWLAN_PAL_TRACE_LEVEL_ERROR,
                "%s : NULL packet pointer", __func__);
      return eWLAN_PAL_STATUS_E_INVAL;
   }

   if ((eWLAN_PAL_PKT_TYPE_TX_802_11_MGMT == WPAL_PACKET_GET_TYPE(pPkt)) ||
               (eWLAN_PAL_PKT_TYPE_RX_RAW == WPAL_PACKET_GET_TYPE(pPkt)))
   {
       //                            
   }
   else
   {
      WPAL_TRACE(eWLAN_MODULE_PAL, eWLAN_PAL_TRACE_LEVEL_ERROR,
                "%s : neither 80211 managment packet nor RAW packet", __func__);
      VOS_ASSERT(0);
      return eWLAN_PAL_STATUS_E_INVAL;
   }

   if( !VOS_IS_STATUS_SUCCESS(vos_pkt_trim_tail(WPAL_TO_VOS_PKT(pPkt), (v_SIZE_t)size)) )
   {
      WPAL_TRACE(eWLAN_MODULE_PAL, eWLAN_PAL_TRACE_LEVEL_ERROR, "%s  Invalid trim(%d)\n",
         __func__, size);
      status = eWLAN_PAL_STATUS_E_INVAL;
   }

   return status;
}/*                     */


/*                                                                           
                                                                                            
                                                                                      
                 
           
                                       
           
                    
                                                           
                                                                           */
wpt_uint8 *wpalPacketGetRawBuf(wpt_packet *pPkt)
{
   wpt_uint8 *pRet = NULL;

   //                                
   if (unlikely(NULL == pPkt))
   {
      WPAL_TRACE(eWLAN_MODULE_PAL, eWLAN_PAL_TRACE_LEVEL_ERROR,
                "%s : NULL packet pointer", __func__);
      return NULL;
   }

   //                                                                     
   if( (eWLAN_PAL_PKT_TYPE_RX_RAW == WPAL_PACKET_GET_TYPE(pPkt)) ||
       (eWLAN_PAL_PKT_TYPE_TX_802_11_MGMT == WPAL_PACKET_GET_TYPE(pPkt)) )
   {
      vos_pkt_peek_data(WPAL_TO_VOS_PKT(pPkt), 0, (v_VOID_t**)&pRet, 1);
      WPAL_ASSERT(NULL != pRet);
   }            

   return pRet;
}/*                   */


/*                                                                           
                                                                                         
                                                                                           
                            
           
                                       
           
                    
                                                           
                                                                           */
wpt_status wpalPacketSetRxLength(wpt_packet *pPkt, wpt_uint32 len)
{
   //                                
   if (unlikely(NULL == pPkt))
   {
      WPAL_TRACE(eWLAN_MODULE_PAL, eWLAN_PAL_TRACE_LEVEL_ERROR,
                "%s : NULL packet pointer", __func__);
      return eWLAN_PAL_STATUS_E_INVAL;
   }

   /*                                */
   if( (eWLAN_PAL_PKT_TYPE_RX_RAW != WPAL_PACKET_GET_TYPE(pPkt)))
   {
     WPAL_TRACE(eWLAN_MODULE_PAL, eWLAN_PAL_TRACE_LEVEL_ERROR, 
                "%s  Invalid packet type(%d)\n",  __func__, 
                WPAL_PACKET_GET_TYPE(pPkt));
     return eWLAN_PAL_STATUS_E_INVAL;
   }

   if(VOS_IS_STATUS_SUCCESS(vos_pkt_set_rx_length(WPAL_TO_VOS_PKT(pPkt), len)))
   {
      return eWLAN_PAL_STATUS_SUCCESS;
   }
   else
   {
      return eWLAN_PAL_STATUS_E_INVAL;
   }
}/*                     */

/*
                                                                    
                                                          
                                                                 
                                                                                
                                           
*/
WPT_STATIC WPT_INLINE void* itGetOSPktAddrForDevice( wpt_packet *pPacket )
{
   struct sk_buff *skb;
   /*                                                                  */
   if ( VOS_STATUS_SUCCESS != 
        vos_pkt_get_os_packet(WPAL_TO_VOS_PKT(pPacket), (void**)&skb, VOS_FALSE ))
   {
     return NULL;
   }
   else
   {
     /*                                  
                                             */
     return (void*)dma_map_single( NULL, skb->data, skb->len, DMA_TO_DEVICE );
   }
}/*                       */

WPT_STATIC WPT_INLINE void* itGetOSPktAddrFromDevice( wpt_packet *pPacket )
{

   struct sk_buff *skb;

   /*                                                                  */
   if ( VOS_STATUS_SUCCESS != 
        vos_pkt_get_os_packet(WPAL_TO_VOS_PKT(pPacket), (void**)&skb, VOS_FALSE ))
   {
     return NULL;
   }
   else
   {
     if(skb->data == skb->tail)
     {
#ifdef WLAN_BUG_ON_SKB_ERROR
       wpalDevicePanic();
#else
       WPAL_TRACE(eWLAN_MODULE_PAL, eWLAN_PAL_TRACE_LEVEL_FATAL,
                "%s: skb->data == skb->tail. Attempting recovery "
                "skb:%p, head:%p, tail:%p, data:%p",
                  __func__, skb, skb->head, skb->tail, skb->data);

      skb->data = skb->head;
#endif
     }
     /*                                  
                                             */
     return (void*)dma_map_single( NULL, skb->data, skb->len, DMA_FROM_DEVICE );
   }
}/*                        */

/*
                                                                      
                                                          
*/
WPT_STATIC WPT_INLINE void itReturnOSPktAddrForDevice( wpt_packet *pPacket,  void* addr, wpt_uint32 size )
{
 
   dma_unmap_single( NULL, (dma_addr_t)addr, size, DMA_TO_DEVICE );
}

WPT_STATIC WPT_INLINE void itReturnOSPktAddrFromDevice( wpt_packet *pPacket, void* addr, wpt_uint32 size  )
{

   dma_unmap_single( NULL, (dma_addr_t)addr, size, DMA_FROM_DEVICE ); 
}


/*                                                                           
                                                                              
           
                                                          
                                         
           
                                          
                                                                           */
wpt_status wpalIteratorInit(wpt_iterator *pIter, wpt_packet *pPacket)
{
   wpt_status         status     = eWLAN_PAL_STATUS_SUCCESS;
   wpt_iterator_info* pCurInfo   = NULL;
   wpt_iterator_info* pNextInfo  = NULL;
   wpt_iterator_info* pPktInfo   = NULL;

   //                                
   if (unlikely((NULL == pPacket)||(NULL==pIter)))
   {
      WPAL_TRACE(eWLAN_MODULE_PAL, eWLAN_PAL_TRACE_LEVEL_ERROR,
                "%s : NULL input pointers %x %x", __func__, pPacket, pIter);
      return eWLAN_PAL_STATUS_E_INVAL;
   }

   pPktInfo = (wpt_iterator_info*)pPacket->pInternalData;
   if (unlikely(NULL == pPktInfo))
   {
      WPAL_TRACE(eWLAN_MODULE_PAL, eWLAN_PAL_TRACE_LEVEL_ERROR,
                "%s : Invalid Packet Info", __func__);
      return eWLAN_PAL_STATUS_E_INVAL;
   }

   //                                                                     
   //                          
   if ( NULL == WPAL_PACKET_GET_BD_PHYS(pPacket) )
   {
     pCurInfo   = pPktInfo;
     pNextInfo           = NULL;
   }
   else
   {
     /*                                    */
     pCurInfo = wpalMemoryAllocate( sizeof(wpt_iterator_info) );

     //                               
     if (unlikely(NULL == pCurInfo))
     {
        WPAL_TRACE(eWLAN_MODULE_PAL, eWLAN_PAL_TRACE_LEVEL_ERROR,
                  "%s : Failed to allocate memory ", __func__);
        return eWLAN_PAL_STATUS_E_INVAL;
     }

     pCurInfo->pPhyAddr = WPAL_PACKET_GET_BD_PHYS(pPacket);
     pCurInfo->uLen     = WPAL_PACKET_GET_BD_LENGTH(pPacket);

     pNextInfo           = pPktInfo;
   }      

   pIter->pCur     = (void*)pCurInfo; 
   pIter->pNext    = (void*)pNextInfo;
   pIter->pContext = NULL;

   return status;
}/*                */

/*                                                                           
                                                        
           
                                                          
                                         
                                                                            
                                                                       
                                                         
           
                                          
                                                                           */
wpt_status wpalIteratorNext(wpt_iterator *pIter, wpt_packet *pPacket, void **ppAddr, wpt_uint32 *pLen)
{
   wpt_iterator_info* pCurInfo  = NULL;
   /*                                                                       */
   
   /*                                                                         
                 
                                                                            */
   if (unlikely(( NULL == pIter )||( NULL == pPacket ) || 
      ( NULL == ppAddr ) || ( NULL == pLen )))
   {
     WPAL_TRACE(eWLAN_MODULE_PAL, eWLAN_PAL_TRACE_LEVEL_ERROR, 
                "%s  Invalid input parameters \n",  __func__ );
     return eWLAN_PAL_STATUS_E_INVAL;
   }

   pCurInfo = (wpt_iterator_info*)pIter->pCur; 
   /*                                                                         
                                                                         
                                                                            */
   if( pIter->pCur == NULL )
   {
      *ppAddr = NULL; 
      *pLen   = 0;
      return eWLAN_PAL_STATUS_SUCCESS;
   }

   /*                                                */
   *ppAddr = pCurInfo->pPhyAddr; 
   *pLen   = pCurInfo->uLen;
    
   if( NULL == pIter->pNext )
   {
     /*                             */
     pPacket->pInternalData = pIter->pCur; 
     pIter->pCur            = NULL; 
   }
   else
   {
     /*                                                       */
     wpalMemoryFree(pCurInfo); 
  
     /*                                                               
                                         */
     pIter->pCur     = pIter->pNext;
     pIter->pNext    = NULL;
   }
   
   return eWLAN_PAL_STATUS_SUCCESS;
}

/*                                                                           
                                                                        
                                                                             
                                                
 
           
                                         
 
           
                                          
                                                                           */
wpt_status wpalLockPacketForTransfer( wpt_packet *pPacket)
{
   void*              pPhyData   = NULL;
   wpt_iterator_info* pInfo      = NULL;
   v_U16_t            uLenData   = 0;

   //                                
   if (unlikely(NULL == pPacket))
   {
      WPAL_TRACE(eWLAN_MODULE_PAL, eWLAN_PAL_TRACE_LEVEL_ERROR,
                "%s : NULL input pointer", __func__);
      return eWLAN_PAL_STATUS_E_INVAL;
   }

   switch(WPAL_PACKET_GET_TYPE(pPacket))
   {
      /*                                                                        
                                                 */
   case eWLAN_PAL_PKT_TYPE_TX_802_11_MGMT:
      {
         /*                                                                
                       */
         pPhyData = (void*)itGetOSPktAddrForDevice( pPacket );   
      }
      break;
         /*                                                               
                                           */
   case eWLAN_PAL_PKT_TYPE_TX_802_11_DATA:
   case eWLAN_PAL_PKT_TYPE_TX_802_3_DATA:
      {
         /*                                                                
                       */
         pPhyData = (void*)itGetOSPktAddrForDevice( pPacket );
      }
      break;

      /*                                                                         
                */
   case eWLAN_PAL_PKT_TYPE_RX_RAW:
      {
         /*                                                                  
                       */
         pPhyData = (void*)itGetOSPktAddrFromDevice( pPacket );
      }
      break;

   default:
      {
         WPAL_TRACE(eWLAN_MODULE_PAL, eWLAN_PAL_TRACE_LEVEL_ERROR, 
                    " WLAN_PAL: %s: Invalid packet type %d!",  __func__, 
                    WPAL_PACKET_GET_TYPE(pPacket) ); 
         WPAL_ASSERT(0); 
         return eWLAN_PAL_STATUS_E_FAILURE;
      }
   }

   /*                 */
   vos_pkt_get_packet_length(WPAL_TO_VOS_PKT(pPacket),&uLenData);

    /*                                    */
   pInfo = wpalMemoryAllocate( sizeof(wpt_iterator_info) );

   //                               
   if (unlikely(NULL == pInfo))
   {
      WPAL_TRACE(eWLAN_MODULE_PAL, eWLAN_PAL_TRACE_LEVEL_ERROR,
                "%s : Failed to allocate memory ", __func__);
      return eWLAN_PAL_STATUS_E_INVAL;
   }

   pInfo->pPhyAddr = pPhyData;
   pInfo->uLen     = uLenData;

   pPacket->pInternalData = pInfo;
   return eWLAN_PAL_STATUS_SUCCESS;
}/*                         */

/*                                                                           
                                                                            
                                         
           
                                         
 
           
                                          
                                                                           */
wpt_status wpalUnlockPacket( wpt_packet *pPacket)
{

   wpt_iterator_info* pInfo;

   //                                
   if (unlikely(NULL == pPacket))
   {
      WPAL_TRACE(eWLAN_MODULE_PAL, eWLAN_PAL_TRACE_LEVEL_ERROR,
                "%s : NULL input pointer pPacket", __func__);
      return eWLAN_PAL_STATUS_E_INVAL;
   }

   pInfo  = (wpt_iterator_info*)pPacket->pInternalData;

   //               
   if (unlikely(NULL == pInfo))
   {
      WPAL_TRACE(eWLAN_MODULE_PAL, eWLAN_PAL_TRACE_LEVEL_FATAL,
                "%s : NULL input pointer pInfo", __func__);
      return eWLAN_PAL_STATUS_E_INVAL;
   }

   switch(WPAL_PACKET_GET_TYPE(pPacket))
   {
      /*                                                                        
                                                 */
   case eWLAN_PAL_PKT_TYPE_TX_802_11_MGMT:
      {
         /*                                                                
                       */
        itReturnOSPktAddrForDevice(pPacket, pInfo->pPhyAddr, pInfo->uLen);   
      }
      break;
         /*                                                               
                                           */
   case eWLAN_PAL_PKT_TYPE_TX_802_11_DATA:
   case eWLAN_PAL_PKT_TYPE_TX_802_3_DATA:
      {
         /*                                                                
                       */
         itReturnOSPktAddrForDevice(pPacket, pInfo->pPhyAddr, pInfo->uLen);   
      }
      break;

      /*                                                                         
                */
   case eWLAN_PAL_PKT_TYPE_RX_RAW:
      {
         /*                                                                  
                       */
         if(NULL == pInfo->pPhyAddr)
         {
            WPAL_TRACE(eWLAN_MODULE_PAL, eWLAN_PAL_TRACE_LEVEL_ERROR, 
                       " WLAN_PAL: %s: RX frame was not locked properly",  __func__); 
         }
         else
         {
            itReturnOSPktAddrFromDevice(pPacket, pInfo->pPhyAddr, pInfo->uLen);   
         }
      }
      break;

   default:
      {
         WPAL_TRACE(eWLAN_MODULE_PAL, eWLAN_PAL_TRACE_LEVEL_ERROR, 
                    " WLAN_PAL: %s: Invalid packet type %d!",  __func__, 
                    WPAL_PACKET_GET_TYPE(pPacket) ); 
         WPAL_ASSERT(0); 
         return eWLAN_PAL_STATUS_E_FAILURE;
      }
   }

  wpalMemoryFree(pInfo);
  pPacket->pInternalData = NULL;
  return eWLAN_PAL_STATUS_SUCCESS;
}/*                */

/*                                                                           
                                                                     
           
                                         
 
           
                                
                                  
                                
                                                                           */
wpt_status wpalIsPacketLocked( wpt_packet *pPacket)
{

   wpt_iterator_info* pInfo;

   /*                                 */
   if (NULL == pPacket)
   {
      WPAL_TRACE(eWLAN_MODULE_PAL, eWLAN_PAL_TRACE_LEVEL_WARN,
                "%s : NULL input pointer", __func__);
      return eWLAN_PAL_STATUS_E_INVAL;
   }

   /*                        */
   pInfo  = (wpt_iterator_info*)pPacket->pInternalData;
   return (NULL == pInfo)? eWLAN_PAL_STATUS_E_FAILURE : 
                    eWLAN_PAL_STATUS_SUCCESS;
}/*                  */

/*                                                                           
                                                                    
         
                                              

          
                               
                                                                           */
wpt_status wpalGetNumRxRawPacket(wpt_uint32 *numRxResource)
{
   *numRxResource = (wpt_uint32)vos_pkt_get_num_of_rx_raw_pkts();

   return eWLAN_PAL_STATUS_SUCCESS;
}

/*                                                                           
                                                                          
                                                         

          
                                                                
                                                        
                                                                        
                                                      

           
           

                                                                           */
void wpalPacketStallUpdateInfo
(
   v_U32_t                         *powerState,
   v_U32_t                         *numFreeBd,
   wpt_log_data_stall_channel_type *channelInfo,
   v_U8_t                           channelNum
)
{
   /*                                        */
   if(NULL != powerState)
   {
      wpalTrasportStallInfo.PowerState = *powerState;
   }

   /*                               */
   if(NULL != numFreeBd)
   {
      wpalTrasportStallInfo.numFreeBd  = *numFreeBd;
   }

   /*                            */
   if(NULL != channelInfo)
   {
      wpalMemoryCopy(&wpalTrasportStallInfo.dxeChannelInfo[channelNum],
                     channelInfo,
                     sizeof(wpt_log_data_stall_channel_type));
   }

   return;
}

#ifdef FEATURE_WLAN_DIAG_SUPPORT
/*                                                                           
                                                               
                                                                

          
            

           
            

                                                                           */
void wpalPacketStallDumpLog
(
   void
)
{
   vos_log_data_stall_type  *log_ptr = NULL;

   WLAN_VOS_DIAG_LOG_ALLOC(log_ptr, vos_log_data_stall_type, LOG_TRSP_DATA_STALL_C);
   if(log_ptr)
   {
      log_ptr->PowerState = wpalTrasportStallInfo.PowerState;
      log_ptr->numFreeBd  = wpalTrasportStallInfo.numFreeBd;
      wpalMemoryCopy(&log_ptr->dxeChannelInfo[0],
                     &wpalTrasportStallInfo.dxeChannelInfo[0],
                     WPT_NUM_TRPT_CHANNEL * sizeof(vos_log_data_stall_channel_type));
      pr_info("Stall log dump");
      WLAN_VOS_DIAG_LOG_REPORT(log_ptr);
   }

   return;
}
#endif /*                           */

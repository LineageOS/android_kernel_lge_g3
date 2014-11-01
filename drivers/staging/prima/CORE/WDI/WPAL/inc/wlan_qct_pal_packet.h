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


#if !defined( __WLAN_QCT_PAL_PACKET_H )
#define __WLAN_QCT_PAL_PACKET_H

/*                                                                          
  
                              
  
                                                 
               
                                        
  
                                                                          */

#include "wlan_qct_pal_type.h"
#include "wlan_qct_pal_status.h"
#include "vos_types.h"
#ifdef FEATURE_WLAN_DIAG_SUPPORT
#include "vos_diag_core_log.h"
#endif /*                           */

//                                               
//             
//                       
//                                                         
//                                                           
//                                     
//
//                                                               
//                                                            
//                                                                       
//                                                                         
//                                 
//                                               
//
//        
//                                                                   
//                                                                 
//                     
//                                                       
//                                                         
//                                      
//                                                                
//                                       
//                             

//                                                            
//                                               
//                                                   
#define VPKT_SIZE_BUFFER  ((30 * 128) + 32)

/*                                        */
#define WPT_NUM_TRPT_CHANNEL      4
/*                                    */
#define WPT_TRPT_CHANNEL_NAME     4

typedef enum
{
   //                                                     
   eWLAN_PAL_PKT_TYPE_TX_802_11_MGMT,
   //                                               
   eWLAN_PAL_PKT_TYPE_TX_802_11_DATA,
   //                                              
   eWLAN_PAL_PKT_TYPE_TX_802_3_DATA,
   //                                                       
   eWLAN_PAL_PKT_TYPE_RX_RAW
} wpt_packet_type;


typedef struct swpt_packet
{
   /*
                                            
                                            
                                                                         
                       
   */
   void *pBD;
   //                                             
   void *pBDPhys; 
   //                                                     
   void *pOSStruct;  
   void *pktMetaInfo;
   wpt_packet_type pktType;
   //                                                                
   wpt_uint16 BDLength;

   //                                                      
   void *pInternalData; 
} wpt_packet;

typedef struct swpt_iterator
{
   void *pNext;  
   void *pCur;
   void *pContext;
} wpt_iterator;

/*                                                              */
typedef struct
{
   char         channelName[WPT_TRPT_CHANNEL_NAME];
   v_U32_t      numDesc;
   v_U32_t      numFreeDesc;
   v_U32_t      numRsvdDesc;
   v_U32_t      headDescOrder;
   v_U32_t      tailDescOrder;
   v_U32_t      ctrlRegVal;
   v_U32_t      statRegVal;
   v_U32_t      numValDesc;
   v_U32_t      numInvalDesc;
} wpt_log_data_stall_channel_type;

/*                       */
typedef struct
{
   v_U32_t                          PowerState;
   v_U32_t                          numFreeBd;
   wpt_log_data_stall_channel_type  dxeChannelInfo[WPT_NUM_TRPT_CHANNEL];
} wpt_log_data_stall_type;

//                               
#define WPAL_PACKET_SET_BD_POINTER(pPkt, pBd)   ( (pPkt)->pBD = (pBd) )
#define WPAL_PACKET_GET_BD_POINTER(pPkt)  ( (pPkt)->pBD )
//                                 
#define WPAL_PACKET_SET_BD_PHYS(pPkt, pBdPhys)   ( (pPkt)->pBDPhys = (pBdPhys) )
#define WPAL_PACKET_GET_BD_PHYS(pPkt)  ( (pPkt)->pBDPhys )
#define WPAL_PACKET_SET_BD_LENGTH(pPkt, len)   ( (pPkt)->BDLength = (len) )
#define WPAL_PACKET_GET_BD_LENGTH(pPkt)  ( (pPkt)->BDLength )
#define WPAL_PACKET_SET_METAINFO_POINTER(pPkt, p) ( (pPkt)->pktMetaInfo = (p) )
#define WPAL_PACKET_GET_METAINFO_POINTER(pPkt) ( (pPkt)->pktMetaInfo )
#define WPAL_PACKET_SET_TYPE(pPkt, type)  ( (pPkt)->pktType = (type) )
#define WPAL_PACKET_GET_TYPE(pPkt)  ( (pPkt)->pktType )
#define WPAL_PACKET_SET_OS_STRUCT_POINTER(pPkt, pStruct)   ( (pPkt)->pOSStruct = (pStruct) )
#define WPAL_PACKET_GET_OS_STRUCT_POINTER(pPkt)  ( (pPkt)->pOSStruct )
#define WPAL_PACKET_IS_FLAT_BUF(pktType) ( (eWLAN_PAL_PKT_TYPE_RX_RAW == (pktType)) || \
                                           (eWLAN_PAL_PKT_TYPE_TX_802_11_MGMT == (pktType)) )

/*                                                                  */
typedef void ( *wpalPacketLowPacketCB )( wpt_packet *pPacket, void *usrData );


/*                                                                           
                                                                                                
                                                           
                                                                                 
           
                                          
           
                                           
                                                                           */
wpt_status wpalPacketInit(void *pPalContext);

/*                                                                           
                                                                    
                                                                                 
           
                                          
           
                                           
                                                                           */
wpt_status wpalPacketClose(void *pPalContext);


/*                                                                           
                                                     
           
                                          
                                                            
                                                                 
           
                                                     
                                                                           */
wpt_packet * wpalPacketAlloc(wpt_packet_type pktType, wpt_uint32 nPktSize,
                             wpalPacketLowPacketCB rxLowCB, void *usrdata);

/*                                                                           
                                                                     
                                                   
           
                                          
                                               
           
                                          
                                                                           */
wpt_status wpalPacketFree(wpt_packet *pPkt);

/*                                                                           
                                                              
           
                                                        
                                               
           
                                                                                        
                                                
                                                                           */
wpt_uint32 wpalPacketGetLength(wpt_packet *pPkt);

/*                                                                           
                                                                              
                                                         
                                                                  
           
                                       
                                                    
           
                                                       
                                                                           */
wpt_status wpalPacketRawTrimHead(wpt_packet *pPkt, wpt_uint32 size);

/*                                                                           
                                                                              
                                                                       
                                                                              
           
                                       
                                                           
           
                                                           
                                                                           */
wpt_status wpalPacketRawTrimTail(wpt_packet *pPkt, wpt_uint32 size);


/*                                                                           
                                                                                              
                                                                       
           
                                       
           
                    
                                                           
                                                                           */
extern wpt_uint8 *wpalPacketGetRawBuf(wpt_packet *pPkt);


/*                                                                           
                                                                                         
                                                                                           
                            
           
                                       
           
                    
                                                           
                                                                           */
extern wpt_status wpalPacketSetRxLength(wpt_packet *pPkt, wpt_uint32 len);


/*                                                                           
                                                                              
           
                                                          
                                         
           
                                          
                                                                           */
wpt_status wpalIteratorInit(wpt_iterator *pIter, wpt_packet *pPacket);

/*                                                                           
                                                        
           
                                                          
                                         
                                                                                                                                            
                                                         
           
                                          
                                                                           */
wpt_status wpalIteratorNext(wpt_iterator *pIter, wpt_packet *pPacket, void **ppAddr, wpt_uint32 *pLen);


/*                                                                           
                                                                                
                                                                          
                              
 
           
                                         
 
           
                                          
                                                                           */
wpt_status wpalLockPacketForTransfer( wpt_packet *pPacket);

/*                                                                           
                                                                             
                                                                      
           
                                         
 
           
                                          
                                                                           */
wpt_status wpalUnlockPacket( wpt_packet *pPacket);

/*                                                                           
                                                                   
                                  
           
                                         
 
           
                                         
                                                                           */
wpt_int32 wpalPacketGetFragCount(wpt_packet *pPkt);

/*                                                                           
                                                                     
           
                                         
 
           
                                
                                  
                                
                                                                           */
wpt_status wpalIsPacketLocked( wpt_packet *pPacket);

/*                                                                           
                                                                    
         
                                              

          
                               
                                                                           */
wpt_status wpalGetNumRxRawPacket(wpt_uint32 *numRxResource);

/*                                                                           
                                                                          
                                                         

          
                                                                
                                                        
                                                                        
                                                      

           
           

                                                                           */
void wpalPacketStallUpdateInfo
(
   v_U32_t                         *powerState,
   v_U32_t                         *numFreeBd,
   wpt_log_data_stall_channel_type *channelInfo,
   v_U8_t                           channelNum
);

#ifdef FEATURE_WLAN_DIAG_SUPPORT
/*                                                                           
                                                               
                                                                

          
            

           
            

                                                                           */
void wpalPacketStallDumpLog
(
   void
);
#endif /*                           */

#endif //                        

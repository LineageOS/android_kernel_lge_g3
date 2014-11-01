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

#if !defined( __I_VOS_PACKET_H )
#define __I_VOS_PACKET_H

/**=========================================================================

  \file        i_vos_packet.h

  \brief       virtual Operating System Servies (vOSS)

   Network Protocol packet/buffer internal include file

   Copyright 2009 (c) Qualcomm, Incorporated.  All Rights Reserved.

   Qualcomm Confidential and Proprietary.

  ========================================================================*/

/*          */

/*                                                                          
               
                                                                          */
#include <vos_types.h>
#include <vos_list.h>
#include <linux/skbuff.h>
#include <linux/list.h>

#include <wlan_qct_pal_packet.h>
#include <wlan_qct_wdi_ds.h>

/*                                                                          
                                        
                                                                          */

//                                                                     
//                                                                    
//                                                                    
//                                                                        
//                                                                    
//                                                       

//                                                                  
//                                        
#define VPKT_NUM_RX_RAW_PACKETS (1024)

//                                                                    
//                                                           
#define VPKT_NUM_TX_MGMT_PACKETS (  6 )

//                                                              
//                                                             
//                            
#define VPKT_NUM_TX_DATA_PACKETS ( 128 )

//                                                                  
//                                                                
//                                                                    
//          
#define VPKT_NUM_VOS_PKT_BUFFERS \
                ( VPKT_NUM_RX_RAW_PACKETS \
                + VPKT_NUM_TX_MGMT_PACKETS \
                + VPKT_NUM_TX_DATA_PACKETS )

//                                                            
//                                                   
#define VPKT_RX_REPLENISH_THRESHOLD (  VPKT_NUM_RX_RAW_PACKETS >> 2 )

//                                                                        
//                                                      
#define VPKT_MAGIC_NUMBER 0x56504B54  /*               */

/*                                                                          
                   
                                                                          */



//                                         
struct vos_pkt_t
{

   //                                               
   wpt_packet palPacket;

   //                                              
   struct list_head node;

   //                                                  
   struct vos_pkt_t *pNext;

   //                                            
   struct sk_buff *pSkb;

   //            
   VOS_PKT_TYPE packetType;

   //          
   v_TIME_t timestamp;

   //                   
   v_VOID_t *pvUserData[ VOS_PKT_USER_DATA_ID_MAX ];

   //                                                             
   v_U32_t magic;
};


//                                                         
//                                                           
//                                                          
//                                                         
typedef struct
{
   vos_pkt_get_packet_callback callback;

   v_VOID_t *userData;

   v_SIZE_t dataSize;
   v_SIZE_t numPackets;
   v_BOOL_t zeroBuffer;

} vos_pkt_low_resource_info;



//                                                                 
//                                        
//                                       
//                                                
//                                     
//                                       
typedef struct vos_pkt_context_s
{
   //                              
   v_CONTEXT_t vosContext;

   //                                             
   struct vos_pkt_t vosPktBuffers[ VPKT_NUM_VOS_PKT_BUFFERS ];

   //                                                                 
   //                                                              
   //                                                                 
   //                      
   struct list_head rxReplenishList;
   struct list_head rxRawFreeList;
   struct list_head txDataFreeList;
   struct list_head txMgmtFreeList;

   //                                                                        
   //                                                      
   v_U32_t uctxDataFreeListCount;

   //                                                         
   //                          
   v_SIZE_t rxReplenishListCount;
   //                                                                        
   //                                  
   v_SIZE_t rxRawFreeListCount;

   //                                                                       
   //                                              
   v_SIZE_t numOfRxRawPackets;

   //                                                                 
   //                                                                
   //                              
   vos_pkt_low_resource_info rxRawLowResourceInfo;
   vos_pkt_low_resource_info txDataLowResourceInfo;
   vos_pkt_low_resource_info txMgmtLowResourceInfo;

   struct mutex rxReplenishListLock;
   struct mutex rxRawFreeListLock;
   struct mutex txDataFreeListLock;
   struct mutex txMgmtFreeListLock;

   /*                                                  */
   WDI_DS_TxMetaInfoType txMgmtMetaInfo[VPKT_NUM_TX_MGMT_PACKETS];
   WDI_DS_TxMetaInfoType txDataMetaInfo[VPKT_NUM_TX_DATA_PACKETS];
   WDI_DS_RxMetaInfoType rxMetaInfo[VPKT_NUM_RX_RAW_PACKETS];

} vos_pkt_context_t;



/*                                                                           

                                                              

                                                               
         

                                                          

                                                               
                                                           

                                                               
                         

                                                                 
                                              

                                                                       
                                                            

                                                                       
                               

                                                                       
                  

                                                                     
                

                        

                                                                           */
VOS_STATUS vos_packet_open( v_VOID_t *pVosContext,
                            vos_pkt_context_t *pVosPacketContext,
                            v_SIZE_t vosPacketContextSize );


/*                                                                           

                                                          

                                                                  
                                                                   
               

                                                          

                                                                     

                                                                     
                        

                                                                       

                       

                                                                           */
VOS_STATUS vos_packet_close( v_PVOID_t pVosContext );

#endif  //                             

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

#if !defined( __VOS_PKT_H )
#define __VOS_PKT_H

/**=========================================================================
  
  \file        vos_packet.h
  
  \brief       virtual Operating System Services (vOSS) network Packet APIs
               
   Network Protocol packet/buffer support interfaces 
  
   Copyright 2008 (c) Qualcomm, Incorporated.  All Rights Reserved.
   
   Qualcomm Confidential and Proprietary.
  
  ========================================================================*/

/*          */

/*                                                                          
               
                                                                          */
#include <vos_types.h>
#include <vos_status.h>

/*                                                                           
                                        
                                                                          */

/*                                                                           
                   
                                                                          */
struct vos_pkt_t;
typedef struct vos_pkt_t vos_pkt_t;


//             
typedef struct
{         
   //                 
   v_VOID_t *pData;
   
   //                            
   v_U32_t  numBytes;
   
} vos_pkt_data_vector_t;



//                   
typedef enum
{
   //                                                           
   VOS_PKT_TYPE_TX_802_11_MGMT,
   
   //                                                     
   VOS_PKT_TYPE_TX_802_11_DATA,
   
   //                                                    
   VOS_PKT_TYPE_TX_802_3_DATA,
   
   //                                                             
   VOS_PKT_TYPE_RX_RAW,

   //                        
   VOS_PKT_TYPE_MAXIMUM

} VOS_PKT_TYPE;

//                                                                         
//                                               
typedef enum
{
   VOS_PKT_USER_DATA_ID_TL =0,  
   VOS_PKT_USER_DATA_ID_BAL,  
   VOS_PKT_USER_DATA_ID_WDA,   
   VOS_PKT_USER_DATA_ID_HDD,
   VOS_PKT_USER_DATA_ID_BAP,
   VOS_PKT_USER_DATA_ID_BSL,

   VOS_PKT_USER_DATA_ID_MAX
   
} VOS_PKT_USER_DATA_ID;

/*                                                                         
  
                                                       

                                                                      
                                                                     
                                      
  
                                                               
  
                                                                         
             
  
         
    
     
  
                                                                          */
typedef VOS_STATUS ( *vos_pkt_get_packet_callback )( vos_pkt_t *pPacket, 
                                                     v_VOID_t *userData );

/*
                                             
                                                                        
                              
 */
#include "i_vos_packet.h"

/*                                                                          
                                        
                                                                          */
  
/*                                                                           
  
                                                  

                                                              
  
                                                                         
                                                                       
                                                             
                    
                                                                            
         
                                                                                 
                              
     
                                                                              
                                         
          
     
                                                            
  
                                                        
                              
                                                                          
                                            
                               
                                                                    
                                              
                                                                                                         
          
                               
                                                                            
                                      
        
                                                                           
                                                                          
                                                                          
                     
                    
                                                                       
                                                       
                        
                                                                           
                                                                      
          
                                                                         
                                                                    
                                                                    
                                             
          
                                                                        
                                                                      
                                                                       
                                                                       
          
                                                                          
                                                                        
                                                                           
                                                                        
                                                                        
                             

                                                                         
                                                               
                                                                             
          
                                                                     
                                                                  
                                                                  
                                                                        
                               
          
                                                                                   
    
     
  
                                                                          */  
VOS_STATUS vos_pkt_get_packet( vos_pkt_t **ppPacket, VOS_PKT_TYPE pktType, 
                               v_SIZE_t dataSize, v_SIZE_t numPackets,
                               v_BOOL_t zeroBuffer,
                               vos_pkt_get_packet_callback callback,
                               v_VOID_t *userData );
                               

/*                                                                           
  
                                                                           
                    

                                                                         
                                                       
  
                                                                             
                                                                          
    
                                                                         
                                                                       
                                                             
                    
                                                                            
         
                                                                            
                        
     
                                                                             
                          
          
     
                                                                               
                                                                            
                                                                              
                                                              
                                                                  
                               
                                                                            
                                      
        
                                                                           
                                                                         
                                                                       
         
                                                                         
                                                                      
                                                                            
                                                                        
                                                                             
                    
                                                                       
                                                       
                        
                                                                           
                                                                      
          
                                                                         
                                                                    
                                          
          
                                                                        
                                                                      
                                                                       
                                                                       
          
                                                                          
                                                                        
                                                                           
                                                                        
                                                                        
                                       
          
                                                                         
                                                               
                                                                             
          
                                                                     
                                                                  
                                                                  
                                                                        
                               
          
                                                                         
                             
    
                             
  
                                                                          */  
VOS_STATUS vos_pkt_wrap_data_packet( vos_pkt_t **ppPacket, VOS_PKT_TYPE pktType, 
                                     v_VOID_t *pOSPacket,
                                     vos_pkt_get_packet_callback callback,
                                     v_VOID_t *userData );


/*                                                                           
  
                                                                         

                                                                       
                                                                         
                             
  
                                                                          
                                                                             
                                                                       
  
                                                                           
                                                                            
                                                                          
                                                                         
                                                 
  
                                                                
         
                                                                               
                                                                            
                                                                              
                                                              
                                                                  
         
                                                                         
                                                                          
                                                                               
                                                                   
                                       
  
                                                                            
                           
                    
                                                                         
                                                        
                                                                          
          
                                                                                  
    
                             
  
                                                                              */
VOS_STATUS vos_pkt_set_os_packet( vos_pkt_t *pPacket, v_VOID_t *pOSPacket );



/*                                                                           
  
                                                                         

                                                                  
                                                                         
                             
  
                                                                
         
                                                                            
                                                                             
                                                                            
                         
         
                                                                            
                                                                            
                                                                             
                                                                           
                                                         
         
                                                                          
                                                                     
                       
                    
                                                                         
                                                        
                                                                          
          
                                                                             
                   
    
                                                                                  
  
                                                                              */
VOS_STATUS vos_pkt_get_os_packet( vos_pkt_t *pPacket, v_VOID_t **ppOSPacket, 
                                  v_BOOL_t clearOSPacket );


/*                                                                           
  
                                                                         
                                                       

                                                                       
                                                                        
                                                                         
                                                                        
            
  
                                                                          
  
                                                                          
                         
                  
                                                                      
                                
                                        
                                               
                                                 
                                            
                  
                                                                            
                           
  
                    
    
     
  
                                                                              */
v_VOID_t vos_pkt_get_user_data_ptr( vos_pkt_t *pPacket, VOS_PKT_USER_DATA_ID userID,
                                    v_VOID_t **ppUserData );


/*                                                                           
  
                                                                          
                                              

                                                                      
                                                                        
                                                                         
                                                                        
            
  
                                                                           
                                                                 
  
                                                           
  
                                                                          
                         
                  
                                                                      
                                
                                        
                                               
                                                 
                                            
                  
                                                                            
                             
  
                    
    
     
  
                                                                              */
v_VOID_t vos_pkt_set_user_data_ptr( vos_pkt_t *pPacket, VOS_PKT_USER_DATA_ID userID,
                                    v_VOID_t *pUserData );

/*                                                                           
  
                                                                       

                                                                         
  
                                                                          
                                                                 
                                                                        
                                                            
  
                                                                          
                                                       
  
         
    
     
  
                                                                              */
VOS_STATUS vos_pkt_return_packet( vos_pkt_t *pPacket );


/*                                                                           
  
                                                                        

                                                                         
                                                                          
               
  
                                                        
  
                                                  
  
                                                                         
                                      
                             
                                                         
                                                               
       
    
         
    
     
  
                                                                              */
VOS_STATUS vos_pkt_chain_packet( vos_pkt_t *pPacket, vos_pkt_t *pChainPacket, 
                                 v_BOOL_t chainAfter );


/*                                                                           
  
                                                                        
                                                       

                                                                            
                                                                          
                             
  
                                          
  
                                                                         
                                                                
                                                                     
                                          
                           
                                                                           
                                                                       
                                                                              
                                                              
                         
        
                                                      
                                                                         
                                                                        
                    
                                                            
                                                                             
                                                                               
                                                                               
                                                   
         
                    
                                                                       
                                                                            
                      
         
    
     
  
                                                                              */
VOS_STATUS vos_pkt_walk_packet_chain( vos_pkt_t *pPacket, vos_pkt_t **ppChainedPacket,
                                      v_BOOL_t unchainPacket );


/*                                                                           
  
                                                                        

                                                                          
                                                         

                                                                        
                              
                   
                                                                              
  
                                                                             
                                                                              
                                                                             
                                          
  
                                                                         
                          
  
                                               
  
         
    
     
  
                                                                              */
VOS_STATUS vos_pkt_get_data_vector( vos_pkt_t *pPacket, vos_pkt_data_vector_t *pVector, 
                                    v_SIZE_t *pNumVectors );


/*                                                                           
  
                                                                    

                                                                       
                                                                      
                                                                    
  
                                                        
  
                                                                             
                                                                   
         
                                                                           
                        
  
                                                                             
                                                                          
                                             
                             
                                                                         
                                                                        
                                                                      
                                                             
                             
         
    
     
  
                                                                              */
VOS_STATUS vos_pkt_extract_data( vos_pkt_t *pPacket, v_SIZE_t pktOffset, 
                                 v_VOID_t *pOutputBuffer, v_SIZE_t *pOutputBufferSize );


/*                                                                           
  
                                                                              

                                                                         
                                                                      
                                                                      
                                         
  
                                                                     
                      
           
                                                                           
                        
  
                                                                           
                                                                             
                                                               
                             
                                                                        
                                                                          
                         
          
                                                                           
                                 
          
                                                                           
                                                                            
                                                                            
    
                            
  
                                                                              */
VOS_STATUS vos_pkt_extract_data_chain( vos_pkt_t *pPacket, v_VOID_t *pOutputBuffer, 
                                       v_SIZE_t *pOutputBufferSize );



/*                                                                           
  
                                                                     

                                                                        
                                                                              
                                                                      
                  
  
                                                                        
                                                                         
                                                                           
         
  
                                               
  
                                                                       
  
                                                                        
                                                                  
                        
                                                                                               
  
         
    
     
  
                                                                              */
VOS_STATUS vos_pkt_peek_data( vos_pkt_t *pPacket, v_SIZE_t pktOffset,
                              v_VOID_t **ppPacketData, v_SIZE_t numBytes );
                        
                        
/*                                                                           
  
                                                                       

                                                      
  
                                                               
  
                                                                             
  
         
    
     
  
                                                                              */
VOS_STATUS vos_pkt_get_packet_type( vos_pkt_t *pPacket, VOS_PKT_TYPE *pPacketType );


/*                                                                           
  
                                                                           

                                                                  
  
                                                                 
  
                                                                              
                                            
         
    
     
  
                                                                              */
VOS_STATUS vos_pkt_get_packet_length( vos_pkt_t *pPacket, v_U16_t *pPacketSize );


/*                                                                           
  
                                                                              

                                                                       
  
                                                                              
                                                                            
                     
  
                                                                              
                                            
         
    
     
  
                                                                              */
VOS_STATUS vos_pkt_get_packet_chain_length( vos_pkt_t *pPacketChain, v_SIZE_t *pPacketChainSize );




/*                                                                           
  
                                                                         

                                                                             
                                                                     
  
                                             
  
                                                                              
  
                                                                                   
  
         
    
     
  
                                                                              */ 
VOS_STATUS vos_pkt_push_head( vos_pkt_t *pPacket, v_VOID_t *pData, v_SIZE_t dataSize );


/*                                                                           
  
                                                                              

                                                                             
                                                                               
                                                                       
                                    
  
                                                                        
           
  
                                       
  
                                             
  
                                                                            
                                                                             
                                                                             
                                                 
  
                                                                            
                                                                           
                                                     
  
         
    
     
  
                                                                              */ 
VOS_STATUS vos_pkt_reserve_head( vos_pkt_t *pPacket, v_VOID_t **ppData, 
                                 v_SIZE_t dataSize );

/*                                                                           
  
                                                                               

                                                                             
                                                                               
                                                                       
                                    
  
                                                                        
           
 
                                       
 
                                       
  
                                             
  
                                                                            
                                                                             
                                                                             
                                                 
  
                                                                            
                                                                           
                                                     
  
         
    
     
  
                                                                              */ 
VOS_STATUS vos_pkt_reserve_head_fast( vos_pkt_t *pPacket,
                                 v_VOID_t **ppData,
                                 v_SIZE_t dataSize );

/*                                                                           
  
                                                                            

                                                                      
                                                                
  
                                                 
  
                                                                            
                                       
                 
                                                                           
                                                                          
                                               
  
         
    
     
  
                                                                              */
VOS_STATUS vos_pkt_pop_head( vos_pkt_t *pPacket, v_VOID_t *pData, v_SIZE_t dataSize );


/*                                                                           
  
                                                                             

                                                                          
                                                                            
                                                                            
                                                                            
                
  
                                                                
  
                                                 
  
                                                                        
                           
                    
                                                                    
                                                                           
                                                                
                                                                     
  
         
    
     
  
                                                                              */
VOS_STATUS vos_pkt_trim_head( vos_pkt_t *pPacket, v_SIZE_t dataSize );


/*                                                                           
  
                                                                       

                                                                           
                                                                       
    
                                             
  
                                                                              
  
                                                                                   
  
         
    
     
  
                                                                              */ 
VOS_STATUS vos_pkt_push_tail( vos_pkt_t *pPacket, v_VOID_t *pData, v_SIZE_t dataSize );


/*                                                                           
  
                                                                            

                                                                           
                                                                               
                                                                       
                                    
  
                                                                        
           
  
                                             
  
                                                                            
                                                                             
                                                                             
                                                 
  
                                                                            
                                                                           
                                                     
  
         
    
     
  
                                                                              */
VOS_STATUS vos_pkt_reserve_tail( vos_pkt_t *pPacket, v_VOID_t **ppData, 
                                 v_SIZE_t dataSize );


/*                                                                           
  
                                                                          

                                                                    
                                                                
  
                                                 
  
                                                                            
                                       
                 
                                                                          
                                                                          
                                               
  
         
    
     
  
                                                                              */
VOS_STATUS vos_pkt_pop_tail( vos_pkt_t *pPacket, v_VOID_t *pData, v_SIZE_t dataSize );


/*                                                                           
  
                                                                           

                                                                          
                                                                          
                                                                      
                                                                   
  
                                                                
  
                                                 
  
                                                                        
                           
                    
                                                                    
                                                                           
                                                                
                                                                     
  
         
    
     
  
                                                                              */
VOS_STATUS vos_pkt_trim_tail( vos_pkt_t *pPacket, v_SIZE_t dataSize );

/*                                                                           
  
                                                                           
                       

                                                  

                                                        
  
                                                             

                                                              
    
     
  
                                                                              */
VOS_STATUS vos_pkt_get_timestamp( vos_pkt_t *pPacket, v_TIME_t* pTstamp );

/*                                                                           
  
                                                                      
                                                                                 
                                                                                     
                                                                               
                                                                                   
                                                                                
                                                                                 
                                                                             
 
                                                                    
                                                                          
                                                                 
                                                   
  
                                                          

                                                      

                                                   

                                                      
    
     
  
                                                                              */
VOS_STATUS vos_pkt_flatten_rx_pkt( vos_pkt_t **ppPacket );

/*                                                                           
  
                                                                       

                                                                                
                                                                  

                                                 

                                                                          


         

     

                                                                             */
VOS_STATUS vos_pkt_set_rx_length( vos_pkt_t *pPacket, v_SIZE_t pktLen );

/*                                                                           
  
                                                                            
                                        
                                                                                       
   
                                                              
                                                                              
                                               
                                                                    
                                                             

                                                
  
                                                      

                                               
    
     
  
                                                                              */
VOS_STATUS vos_pkt_get_available_buffer_pool
(
   VOS_PKT_TYPE  pktType,
   v_SIZE_t     *vosFreeBuffer
);

/* 
                                                                        
                                                                

                                                                      
                                                                       
                                          

        
           
         
                                                 

*/
v_SIZE_t vos_pkt_get_num_of_rx_raw_pkts(void);

#endif  //                        

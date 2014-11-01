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


#if !defined( WLAN_HDD_SOFTAP_TX_RX_H )
#define WLAN_HDD_SOFTAP_TX_RX_H

/**===========================================================================
  
  \file  wlan_hdd_softap_tx_rx.h
  
  \brief Linux HDD SOFTAP Tx/RX APIs
         Copyright 2008 (c) Qualcomm, Incorporated.
         All Rights Reserved.
         Qualcomm Confidential and Proprietary.
  
  ==========================================================================*/
  
/*                                                                            
               
                                                                           */ 
#include <wlan_hdd_hostapd.h>

/*                                                                            
                                        
                                                                           */ 
#define HDD_SOFTAP_TX_BK_QUEUE_MAX_LEN (82*2)
#define HDD_SOFTAP_TX_BE_QUEUE_MAX_LEN (78*2)
#define HDD_SOFTAP_TX_VI_QUEUE_MAX_LEN (74*2)
#define HDD_SOFTAP_TX_VO_QUEUE_MAX_LEN (70*2)

/*                            */
#define HDD_SOFTAP_BK_WEIGHT_DEFAULT                        1
#define HDD_SOFTAP_BE_WEIGHT_DEFAULT                        3
#define HDD_SOFTAP_VI_WEIGHT_DEFAULT                        8
#define HDD_SOFTAP_VO_WEIGHT_DEFAULT                        18

/*                                                                            
                   
                                                                           */ 
 
/*                                                                            
                                        
                                                                           */ 

/*                                                                             
                                                                                  
                      

                                                        
                                                               
  
                                                        
                                                                      
                                                                             */
extern int hdd_softap_hard_start_xmit(struct sk_buff *skb, struct net_device *dev);

/*                                                                             
                                                                        
                                                               
                                                                   

                                                   
                   
                                                                             */
extern void hdd_softap_tx_timeout(struct net_device *dev);

/*                                                                             
                                                                        
                         

                                                        
  
                                                         
                                                                             */
extern struct net_device_stats* hdd_softap_stats(struct net_device *dev);

/*                                                                             
                                                                    
                

                                                     
                                                                   
                                                
                                                                             */
extern VOS_STATUS hdd_softap_init_tx_rx( hdd_adapter_t *pAdapter );

/*                                                                             
                                                                      
                

                                                     
                                                                   
                                                
                                                                             */
extern VOS_STATUS hdd_softap_deinit_tx_rx( hdd_adapter_t *pAdapter );

/*                                                                             
                                                                                     
                

                                                   
                                             
                                                                        
                                                                   
                                                
                                                                             */
extern VOS_STATUS hdd_softap_init_tx_rx_sta( hdd_adapter_t *pAdapter, v_U8_t STAId, v_MACADDR_t *pmacAddrSTA);

/*                                                                             
                                                                                        
                

                                                   
                                              
                                                                   
                                                
                                                                             */
extern VOS_STATUS hdd_softap_deinit_tx_rx_sta ( hdd_adapter_t *pAdapter, v_U8_t STAId );

/*                                                                             
                                                                       
                

                                                     
                                                                   
                                                
                                                                             */
extern VOS_STATUS hdd_softap_disconnect_tx_rx( hdd_adapter_t *pAdapter );

/*                                                                             
                                                                
                                                                    
                                                                  

                                                      
                                                                    
                                                        

                                                                       
                                                    
                                                                             */
extern VOS_STATUS hdd_softap_tx_complete_cbk( v_VOID_t *vosContext, 
                                       vos_pkt_t *pVosPacket, 
                                       VOS_STATUS vosStatusIn );

/*                                                                             
                                                                               
                                  

                                                     
                                                                     
                                                                       
                                                                  
                                                               
  
                                                                    
                                                                       
                                                    
                                                                             */
extern VOS_STATUS hdd_softap_tx_fetch_packet_cbk( v_VOID_t *vosContext,
                                           v_U8_t *pStaId,
                                           WLANTL_ACEnumType    ucAC,
                                           vos_pkt_t **ppVosPacket,
                                           WLANTL_MetaInfoType *pPktMetaInfo );

/*                                                                             
                                                                             
                                                                          
                                                                     
            

                                                  
                                                                       
  
                                                                      
                                                  
                                                                               */
extern VOS_STATUS hdd_softap_tx_low_resource_cbk( vos_pkt_t *pVosPacket, 
                                           v_VOID_t *userData );

/*                                                                             
                                                                          
                                                                 
                       

                                                     
                                                                        
                                       
                                                                          

                                                                        
                                                    
                                                                             */
extern VOS_STATUS hdd_softap_rx_packet_cbk( v_VOID_t *vosContext, 
                                     vos_pkt_t *pVosPacket, 
                                     v_U8_t staId,
                                     WLANTL_RxMetaInfoType* pRxMetaInfo );

/*                                                                             
                                                                      

                                                   
                                                 
                                                                   
                                                
                                                                             */
extern VOS_STATUS hdd_softap_DeregisterSTA( hdd_adapter_t *pAdapter, tANI_U8 staId );

/*                                                                             
                                                                  

                                                   
                                                 
                                                                                 
                                                               
                                                      
                                                        
                                                    
                                                   
                                                                   
                                                
                                                                              */
extern VOS_STATUS hdd_softap_RegisterSTA( hdd_adapter_t *pAdapter,
                                       v_BOOL_t fAuthRequired,
                                       v_BOOL_t fPrivacyBit,    
                                       v_U8_t staId,
                                       v_U8_t ucastSig,
                                       v_U8_t bcastSig,
                                       v_MACADDR_t *pPeerMacAddress,
                                       v_BOOL_t fWmmEnabled);

/*                                                                             
                                                                                        

                                                   
                                                              
                                                                   
                                                
                                                                              */
extern VOS_STATUS hdd_softap_Register_BC_STA( hdd_adapter_t *pAdapter, v_BOOL_t fPrivacyBit);

/*                                                                             
                                                                                          

                                                   
                                                                   
                                                
                                                                              */
extern VOS_STATUS hdd_softap_Deregister_BC_STA( hdd_adapter_t *pAdapter);

/*                                                                             
                                                                                       

                                                   
                                                                   
                                                
                                                                              */
extern VOS_STATUS hdd_softap_stop_bss( hdd_adapter_t *pHostapdAdapter);


/*                                                                             
                                                                                             

                                                   
                                                              
                                          
                                                                   
                                                
                                                                              */
extern VOS_STATUS hdd_softap_change_STA_state( hdd_adapter_t *pAdapter, v_MACADDR_t *pDestMacAddress, WLANTL_STAStateType state);

/*                                                                             
                                                                                 

                                                   
                                                              
                                    
                                                                   
                                                
                                                                              */
extern VOS_STATUS hdd_softap_GetStaId( hdd_adapter_t *pAdapter, v_MACADDR_t *pMacAddress, v_U8_t *staId);

/*                                                                             
                                                                                                 

                                                   
                                    
                                                                  
                                                
                                                                              */
extern VOS_STATUS hdd_softap_GetConnectedStaId( hdd_adapter_t *pAdapter, v_U8_t *staId);

/*                                                                           

                                     
                                                                
                                                  

                                                   
                                                                  

                                                                             */
VOS_STATUS hdd_start_trafficMonitor( hdd_adapter_t *pAdapter );

/*                                                                           

                                    
                                                                 
                                                  

                                                   
                                                                  

                                                                             */
VOS_STATUS hdd_stop_trafficMonitor( hdd_adapter_t *pAdapter );

#endif    //                                            

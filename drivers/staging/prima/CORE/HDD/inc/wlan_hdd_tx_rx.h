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

#if !defined( WLAN_HDD_TX_RX_H )
#define WLAN_HDD_TX_RX_H

/*                                                                            

                         

                             
                                                                            */

/*                                                                           
               
                                                                           */
#include <wlan_hdd_includes.h>
#include <vos_api.h>
#include <linux/skbuff.h>
#include <wlan_qct_tl.h>

/*                                                                           
                                        
                                                                           */
#define HDD_ETHERTYPE_802_1_X              ( 0x888E )
#define HDD_ETHERTYPE_802_1_X_FRAME_OFFSET ( 12 )
#define HDD_ETHERTYPE_802_1_X_SIZE         ( 2 )
#ifdef FEATURE_WLAN_WAPI
#define HDD_ETHERTYPE_WAI                  ( 0x88b4 )
#endif

#define HDD_80211_HEADER_LEN      24
#define HDD_80211_HEADER_QOS_CTL  2
#define HDD_LLC_HDR_LEN           6
#define HDD_FRAME_TYPE_MASK       0x0c 
#define HDD_FRAME_SUBTYPE_MASK    0xf0 
#define HDD_FRAME_TYPE_DATA       0x08
#define HDD_FRAME_TYPE_MGMT       0x00
#define HDD_FRAME_SUBTYPE_QOSDATA 0x80
#define HDD_FRAME_SUBTYPE_DEAUTH  0xC0
#define HDD_FRAME_SUBTYPE_DISASSOC 0xA0
#define HDD_DEST_ADDR_OFFSET      6

#define HDD_MAC_HDR_SIZE          6

#define HDD_PSB_CFG_INVALID                   0xFF
#define HDD_PSB_CHANGED                       0xFF
#define SME_QOS_UAPSD_CFG_BK_CHANGED_MASK     0xF1
#define SME_QOS_UAPSD_CFG_BE_CHANGED_MASK     0xF2
#define SME_QOS_UAPSD_CFG_VI_CHANGED_MASK     0xF4
#define SME_QOS_UAPSD_CFG_VO_CHANGED_MASK     0xF8

/*                                                                            
                   
                                                                           */ 
 
/*                                                                            
                                        
                                                                           */ 

/*                                                                             
                                                                           
                      

                                                        
                                                        
  
                                                        
                                                                      
                                                                             */
extern int hdd_hard_start_xmit(struct sk_buff *skb, struct net_device *dev);

extern int hdd_mon_hard_start_xmit(struct sk_buff *skb, struct net_device *dev);
/*                                                                             
                                                                 
                                                               
                                                                   

                                                   
                   
                                                                             */
extern void hdd_tx_timeout(struct net_device *dev);

/*                                                                             
                                                                 
                         

                                                        
  
                                                         
                                                                             */
extern struct net_device_stats* hdd_stats(struct net_device *dev);

/*                                                                             
                                                             
                

                                                     
                                                                   
                                                
                                                                             */
extern VOS_STATUS hdd_init_tx_rx( hdd_adapter_t *pAdapter );

/*                                                                             
                                                               
                

                                                     
                                                                   
                                                
                                                                             */
extern VOS_STATUS hdd_deinit_tx_rx( hdd_adapter_t *pAdapter );

/*                                                                             
                                                                       
                

                                                     
                                                                   
                                                
                                                                             */
extern VOS_STATUS hdd_disconnect_tx_rx( hdd_adapter_t *pAdapter );

/*                                                                             
                                                                
                                                                    
                                                                  

                                                      
                                                                    
                                                        

                                                                       
                                                    
                                                                             */
extern VOS_STATUS hdd_tx_complete_cbk( v_VOID_t *vosContext, 
                                       vos_pkt_t *pVosPacket, 
                                       VOS_STATUS vosStatusIn );

/*                                                                             
                                                                        
                                  

                                                     
                                                                     
                                                                       
                                                                  
                                                               
  
                                                                    
                                                                       
                                                    
                                                                             */
extern VOS_STATUS hdd_tx_fetch_packet_cbk( v_VOID_t *vosContext,
                                           v_U8_t *pStaId,
                                           WLANTL_ACEnumType    ucAC,
                                           vos_pkt_t **ppVosPacket,
                                           WLANTL_MetaInfoType *pPktMetaInfo );

/*                                                                             
                                                                      
                                                                          
                                                                     
            

                                                  
                                                                       
  
                                                                      
                                                  
                                                                               */
extern VOS_STATUS hdd_tx_low_resource_cbk( vos_pkt_t *pVosPacket, 
                                           v_VOID_t *userData );

/*                                                                             
                                                                   
                                                                 
                       

                                                     
                                                                        
                                       
                                                                          

                                                                        
                                                    
                                                                             */
extern VOS_STATUS hdd_rx_packet_cbk( v_VOID_t *vosContext, 
                                     vos_pkt_t *pVosPacket, 
                                     v_U8_t staId,
                                     WLANTL_RxMetaInfoType* pRxMetaInfo );


/*                                                                             
                                                                 

                                                  
                                                    
                                       
                                                                             */
extern v_BOOL_t hdd_IsEAPOLPacket( vos_pkt_t *pVosPacket );

/*                                                                             
                                                                          
                                           

                                           
                                                                             */
void hdd_mon_tx_mgmt_pkt(hdd_adapter_t* pAdapter);

/*                                                                             
                                                                                    

                                        
                                                                             */
void hdd_mon_tx_work_queue(struct work_struct *work);

/*                                                                             
                                                                           
                                                         
                                               
                                       
                                                      
                                                                             */
VOS_STATUS hdd_Ibss_GetStaId(hdd_station_ctx_t *pHddStaCtx,
                                  v_MACADDR_t *pMacAddress, v_U8_t *staId);

/*                                                                             
                                                 
                                                                    
                                 
                                                      
                                                                             */
void hdd_tx_rx_pkt_cnt_stat_timer_handler( void *pAdapter);

/*                                                                          
                                           
                                                                      
                                               
                       
                         
                                                                           */
void hdd_wmm_acquire_access_required(hdd_adapter_t *pAdapter,
                                     WLANTL_ACEnumType acType);
#endif    //                                     

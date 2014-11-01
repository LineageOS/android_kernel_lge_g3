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

#ifndef __WLAN_QCT_DTS_H
#define __WLAN_QCT_DTS_H

#include "wlan_qct_wdi.h"

/**=========================================================================
 *     
 *       \file  wlan_qct_wdi_dts.h
 *          
 *       \brief define Datas Trnasport Service API 
 *                               
 * WLAN Device Abstraction layer interface for Transport drivers (SDIO/DXE)
 * DESCRIPTION
 * This file contains the API exposed by the 
 * wlan device abstarction layer module for abstracting DXE/SDIO.
 *
 * Copyright (c) 2008 QUALCOMM Incorporated. All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 * Example usage for DXE.
 * ----------------------
 * On Platform init
 *         DAL  will then invoke WDTS_open
 *
 * On DAL init
 *     DAL will invike WDTS_start
 *
 * On transmit:
 *         DAL will invoke WDTS_TxPacket API
 * 
 * On transmit complete:
 *         DXE will serialize into TX thread
 *         In TX thread it will invoke
 *
 * On receive:
 *        DXE will serialize into RX thread
 *        In TX thread it will invoke WDTS_RXPacket API 
 *
 * On DXE ring full:
 *        DXE will serialize into TX thread
 *        In TX thread it will invoke WDTS_OOResourceNotification API 
 */

typedef enum
{
   WDTS_CHANNEL_TX_LOW_PRI,
   WDTS_CHANNEL_TX_HIGH_PRI,
   WDTS_CHANNEL_RX_LOW_PRI,
   WDTS_CHANNEL_RX_HIGH_PRI,
   WDTS_CHANNEL_MAX
}  WDTS_ChannelType;

typedef enum
{
   WDTS_POWER_STATE_FULL,
   WDTS_POWER_STATE_IMPS,
   WDTS_POWER_STATE_BMPS,
   WDTS_POWER_STATE_DOWN,
   WDTS_POWER_STATE_MAX
} WDTS_PowerStateType;


typedef wpt_status (*WDTS_TxCompleteCbType)(void *pContext, wpt_packet *pFrame, wpt_status status);
typedef wpt_status (*WDTS_RxFrameReadyCbType) (void *pContext, wpt_packet *pFrame, WDTS_ChannelType channel);
typedef wpt_status (*WDTS_LowResourceCbType)(void *pContext, WDTS_ChannelType channel, wpt_boolean on);
typedef void  (*WDTS_SetPSCbType)(wpt_status  status, unsigned int dxePhyAddr);
/*                                   
                                                                             
                                   
              
                                      
                                                                                     
                      
  
 */
typedef void  (*WDTS_SetPowerStateCbType)(wpt_status   status,
                                          unsigned int dxePhyAddr,
                                          void*        pUserData);

typedef struct {
  void * (*open)(void);
  wpt_status (*start) (void *pContext);
  wpt_status (*register_client)(void *pContext, WDTS_RxFrameReadyCbType, 
      WDTS_TxCompleteCbType, WDTS_LowResourceCbType, void *clientData);
  wpt_status (*xmit) (void *pContext, wpt_packet *packet, WDTS_ChannelType channel);
  wpt_status (*txComplete) (void *pContext, wpt_uint32 ucTxResReq);
  wpt_status (*setPowerState) (void *pContext, WDTS_PowerStateType   powerState, 
                               WDTS_SetPSCbType cBack);
  void (*channelDebug)(wpt_boolean displaySnapshot,
                       wpt_uint8   debugFlags);
  wpt_status (*stop) (void *pContext);
  wpt_status (*close) (void *pContext);
  wpt_uint32 (*getFreeTxDataResNumber) (void *pContext);
} WDTS_TransportDriverTrype;

typedef struct {
   WDTS_SetPowerStateCbType cback;
   void*        pUserData;
} WDTS_SetPowerStateCbInfoType;

/*                     
                                                             
               
                                         
                                              
                       
 */
void WDTS_GetTrafficStats(WDI_TrafficStatsType** pStats, wpt_uint32 *len);

/*                            
                                                                      
                    
                       
 */
void WDTS_DeactivateTrafficStats(void);

/*                          
                                                                       
                    
                       
 */
void WDTS_ActivateTrafficStats(void);

/*                       
                                                          
                    
                       
 */
void WDTS_ClearTrafficStats(void);

/*                     
                                                         
              
                                                                   
                      
  
                                                 
                                                        
 
 */
wpt_status WDTS_openTransport( void *pContext);




/*                      
                                                      
              
                                                                  
                     
  
                                                 
                                                        
  
 */
wpt_status WDTS_startTransport( void *pContext);




/*                         
                                                                                                      
              
                                                                                    
                                
                                                 
                                                        
  
 */
wpt_status WDTS_TxPacket(void *pContext, wpt_packet *pFrame);

/*                           
                                                                                              
              
                                                                                    
                                               
                                                 
                                                        
  
 */
wpt_status WDTS_CompleteTx(void *pContext, wpt_uint32 ucTxResReq);

/*                               
                                                                                    
              
                                                                                    
                                             
                                                              
                                                        
  
 */
wpt_status WDTS_SetPowerState(void *pContext, WDTS_PowerStateType powerState,
                              WDTS_SetPowerStateCbType cback);

/*                            
                                            
                                                   
                                                           
              
                                                 
                                                  
                                                      
                                               
                                       
  
                                                           
                     
  
 */
void WDTS_ChannelDebug(wpt_boolean displaySnapshot, wpt_uint8 debugFlags);

/*                    
                                      
              
                                                                                    
                                                 
                                                        
  
 */
wpt_status WDTS_Stop(void *pContext);

/*                     
                                       
              
                                                                                    
                                                 
                                                        
  
 */
wpt_status WDTS_Close(void *pContext);

/*                                            
              
                                                                                     
                                                               
  
 */
wpt_uint32 WDTS_GetFreeTxDataResNumber(void *pContext);

/*                                                               
                                                                                
                                                                               
                                                                                 
                                                                                                               
 */
void WDTS_FillRateInfo(wpt_uint8 macEff, wpt_int16 startRateIndex, wpt_int16 endRateIndex);
#endif

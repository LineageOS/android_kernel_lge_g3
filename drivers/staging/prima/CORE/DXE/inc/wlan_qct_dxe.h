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

#ifndef WLAN_QCT_DXE_H
#define WLAN_QCT_DXE_H

/**=========================================================================
  
  @file  wlan_qct_dxe.h
  
  @brief 
               
   This file contains the external API exposed by the wlan data transfer abstraction layer module.
   Copyright (c) 2008 QUALCOMM Incorporated. All Rights Reserved.
   Qualcomm Confidential and Proprietary
========================================================================*/

/*                                                                           

                                           


                                                                       
                                                                


                                   


                                          
                                                                                  
                                       

                                                                           */

/*                                                                           

                                                  

                                                                           */

/*                                                                            
                
                                                                            */
#include "wlan_qct_pal_api.h"
#include "wlan_qct_pal_packet.h"
#include "wlan_qct_pal_status.h"
#include "wlan_qct_pal_type.h"
#include "wlan_qct_pal_msg.h"
#include "wlan_qct_pal_sync.h"
#include "wlan_qct_wdi_dts.h"

/*                                                                            
                     
                                                                            */
/*                                          */

//                                  

/*                                
                                          */
#define WLANDXE_DEFAULT_RX_OS_BUFFER_SIZE  (VPKT_SIZE_BUFFER)

/*                                                                     
                  
                                                                      
                                                                   
                                                                  
                                          */
#ifdef WINDOWS_DT
#define WLANDXE_LO_PRI_RES_NUM 240
#else
#define WLANDXE_LO_PRI_RES_NUM 128
#endif


/*                                                                    
                   */
#define WLANDXE_HI_PRI_RES_NUM 10

typedef enum
{
   WLANDXE_POWER_STATE_FULL,
   WLANDXE_POWER_STATE_IMPS,
   WLANDXE_POWER_STATE_BMPS,
   WLANDXE_POWER_STATE_BMPS_PENDING,
   WLANDXE_POWER_STATE_DOWN,
   WLANDXE_POWER_STATE_MAX
} WLANDXE_PowerStateType;

typedef enum
{
   WLANDXE_RIVA_POWER_STATE_ACTIVE,
   WLANDXE_RIVA_POWER_STATE_IMPS_UNKNOWN,
   WLANDXE_RIVA_POWER_STATE_BMPS_UNKNOWN,
   WLANDXE_RIVA_POWER_STATE_DOWN_UNKNOWN,
   WLANDXE_RIVA_POWER_STATE_MAX
} WLANDXE_RivaPowerStateType;

/*                                                                          
              
                                 

                 
                                   

               
                                                                     
                                                           
                                                          

           
                  
                                                                           */
typedef WDTS_RxFrameReadyCbType WLANDXE_RxFrameReadyCbType;

/*                                                                          
              
                                

                 
                               

               
                                                                  
                                                            
                                                       

           
                 
                                                                           */
typedef WDTS_TxCompleteCbType WLANDXE_TxCompleteCbType;

/*                                                                          
              
                                

                 
                                     

               
                                                               
                                                              
                                                    

           
                
                                                                           */
typedef WDTS_LowResourceCbType WLANDXE_LowResourceCbType;

/*                                                                          
              
                                  

                 
                                                                         
                                                                                

               
                                           
                                                                              
                         

           
          
                                                                           */
typedef WDTS_SetPSCbType  WLANDXE_SetPowerStateCbType;

/*                                                                         
                                        
                                                                           */
/*                                                                          
                   
                  

                 
                                                  
                                                                                               

               
                                                               

           
                                                  
                                                                           */
void *WLANDXE_Open
(
   void
);

/*                                                                          
                   
                                

                 
                                                                                 

               
                                                                        
                                                                                     
                                                                                
                                                                                                   
                                                                         

           
                
                                                                           */
wpt_status WLANDXE_ClientRegistration
(
   void                       *pDXEContext,
   WDTS_RxFrameReadyCbType     rxFrameReadyCB,
   WDTS_TxCompleteCbType       txCompleteCB,
   WDTS_LowResourceCbType      lowResourceCB,
   void                       *userContext
);

/*                                                                          
                   
                   

                 
                           
                                               

               
                                                                        

           
                
                                                                           */
wpt_status WLANDXE_Start
(
   void  *pDXEContext
);

/*                                                                          
                   
                     

                 
                                              

               
                                                      
                                                          
                                           

           
                
                                                                           */
wpt_status WLANDXE_TxFrame
(
   void                 *pDXEContext,
   wpt_packet           *pPacket,
   WDTS_ChannelType      channel
);


/*                                                                          
                   
                        

                 
                                                                   

               
                                     
                                                               

           
                
                                                                           */
wpt_status
WLANDXE_CompleteTX
(
  void* pDXEContext,
  wpt_uint32 ucTxResReq
);

/*                                                                          
                   
                  

                 
                                                 

               
                                                      

           
                
                                                                           */
wpt_status WLANDXE_Stop
(
   void *pDXEContext
);

/*                                                                          
                   
                   

                 
                        
                                
                         
                                   
                                  

               
                                                      

           
                
                                                                           */
wpt_status WLANDXE_Close
(
   void *pDXEContext
);

/*                                                                          
                   
                       

                 
         

               
                                                      

           
                
                                                                           */
wpt_status WLANDXE_TriggerTX
(
   void *pDXEContext
);

/*                                                                          
                   
                           

                 
                                                                     

               
                                                              
                                         

           
                
                                                                           */
wpt_status WLANDXE_SetPowerState
(
   void                    *pDXEContext,
   WDTS_PowerStateType      powerState,
   WDTS_SetPSCbType         cBack
);

/*                                                                          
                   
                                    

                 
                                                                            

               
                                                      

           
                                                              
                                                                           */
wpt_uint32 WLANDXE_GetFreeTxDataResNumber
(
   void *pDXEContext
);

/*                                                                          
                  
                        

                
                                             
                                                    
                                                            

               
                                                 
                                                  
                                                      
                                               
                                       

           
        

                                                                           */
void WLANDXE_ChannelDebug
(
   wpt_boolean    displaySnapshot,
   wpt_uint8      debugFlags
);

#ifdef WLANDXE_TEST_CHANNEL_ENABLE
/*                                                                          
                   
                      

                 
                                       

               
          

           
          

                                                                           */
void WLANDXE_UnitTestStartDXE
(
   void
);

/*                                                                          
                   

                 

               

           

                                                                           */
void WLANDXE_UnitTestDataTransfer
(
   void
);

/*                                                                          
                   

                 

               

           

                                                                           */
void WLANDXE_UnitTestEventHandle
(
   void     *dxeCB
);
#endif /*                             */
#endif /*                */

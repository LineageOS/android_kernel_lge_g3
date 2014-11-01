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

#ifndef WLAN_QCT_PAL_DEVICE_H
#define WLAN_QCT_PAL_DEVICE_H
/* ====================================================================================================================

   @file   wlan_qct_pal_device.h

   @brief
    This file contains the external API exposed by WLAN PAL Device specific functionalities
    Copyright (c) 2011 Qualcomm Incorporated. All Rights Reserved
    Qualcomm Confidential and Properietary

 * ==================================================================================================================*/

/*                                                                                                                     
                                       

                                                                        
                                                                

                                                        
                                                                                                                       
                                                     
                                                                                                                     */

/*                                                                                                                     
                                           
                                                                                                                     */
#include "wlan_qct_pal_type.h"
#include "wlan_qct_pal_status.h"
#include "wlan_qct_pal_trace.h"

/*                                                                                                                     
                                               
                                                                                                                     */
#define     DXE_INTERRUPT_TX_COMPLE      0x02
#define     DXE_INTERRUPT_RX_READY       0x04
#define     WPAL_ISR_CLIENT_MAX          0x08

#define     WPAL_SMSM_WLAN_TX_ENABLE          0x00000400
#define     WPAL_SMSM_WLAN_TX_RINGS_EMPTY     0x00000200

typedef enum
{
   WPAL_DEBUG_START_HEALTH_TIMER = 1<<0,
   WPAL_DEBUG_TX_DESC_RESYNC     = 1<<1,
} WPAL_DeviceDebugFlags;
/*                                                                                                                     
                   
                 

                 
                               
                                                    

               
                                                                        

           
          
                                                                                                                     */
typedef void (* wpalIsrType)(void *usrCtxt);

/*                                                                                                                     
                                  
                                                                                                                     */
/*                                                                                                                     
                  

                

              

                 

         

                                                                                                                     */
wpt_status wpalDeviceInit
(
   void                 *deviceCB
);

/*                                                                                                                     
                  

                

              

                 

         

                                                                                                                     */
wpt_status wpalDeviceClose
(
   void                 *deviceC
);

/*                                                                           
                                                         
                                                                            */
/* 
                                                              
                                                  

                                                               
                                                                    
                                                                     
                                                                         
  
                                                                        
                                                
                                                                 
                                                       

                                                    
*/
wpt_status wpalRegisterInterrupt
(
   wpt_uint32                           intType,
   wpalIsrType                          callbackFunction,
   void                                *usrCtxt
);

/* 
                                                                
                                             

                                                       
  
                                                                        

              
*/

void wpalUnRegisterInterrupt
(
   wpt_uint32      intType
);

/* 
                                                              
                                                     

                                                               
                                                                    
                                                                  
                                                               
                                                                     
                          
  
                                                                        

                                              
*/
wpt_status wpalEnableInterrupt
(
   wpt_uint32                          intType
);

/* 
                                                               
                                                      

                                                               
                                                                    
                                                                     
                                                               
                                                                     
                          
  
                                                                        

                                               
*/
wpt_status wpalDisableInterrupt
(
   wpt_uint32                           intType
);

/* 
                                                            
                                                    

                                                           
                                            

                                                      
*/
wpt_status wpalReadRegister
(
   wpt_uint32                           address,
   wpt_uint32                          *data
);

/* 
                                                           
                                                   

                                                           
                                                          

                                                   
*/
wpt_status wpalWriteRegister
(
   wpt_uint32                           address,
   wpt_uint32                           data
);

/* 
                                                               
                                                     

                                                               
                                                            
                                        
                                                      

                                                   
*/
wpt_status wpalReadDeviceMemory
(
   wpt_uint32                            address,
   wpt_uint8                            *DestBuffer,
   wpt_uint32                            len
);

/* 
                                                                
                                                      

                                                                  
                                                                   
                           
                                                         

                                                      
*/
wpt_status wpalWriteDeviceMemory
(
   wpt_uint32                            address,
   wpt_uint8                            *srcBuffer,
   wpt_uint32                            len
);

/* 
                                                             
                                                               
                    

                                                    
                                               

                                                
*/
wpt_status wpalNotifySmsm
(
   wpt_uint32                            clrSt,
   wpt_uint32                            setSt
);

/* 
                                                    

              

              
*/
void wpalActivateRxInterrupt(void);

/* 
                                                        

              

              
*/
void wpalInactivateRxInterrupt(void);

#endif /*                      */

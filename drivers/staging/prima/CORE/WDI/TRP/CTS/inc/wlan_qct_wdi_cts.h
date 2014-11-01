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

#ifndef WLAN_QCT_WDI_CTS_H
#define WLAN_QCT_WDI_CTS_H

/*===========================================================================

         W L A N   C O N T R O L    T R A N S P O R T   S E R V I C E  
                       E X T E R N A L  A P I
                
                   
DESCRIPTION
  This file contains the external API exposed by the wlan control transport
  service module.
  
      
  Copyright (c) 2008 QUALCOMM Incorporated. All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/


/*                                                                           

                                           


                                                                       
                                                                


                                   


                                   
                                                                             
                                   

                                                                           */



/*                                                                           

                                                  

                                                                           */

/*                                                                            
                
                                                                            */
#include "wlan_qct_pal_type.h" 

/*                                                                            
                                         
                                                                            */
 #ifdef __cplusplus
 extern "C" {
 #endif 

/*                                                                            
                     
                                                                            */

/*                                      */
typedef void*  WCTS_HandleType;

/*                                                                            
                       
                                                                            */
typedef enum
{
   WCTS_EVENT_OPEN,
   WCTS_EVENT_CLOSE,
   WCTS_EVENT_MAX
} WCTS_NotifyEventType;

/*                                                                            
                       
                                                                            */

/*                                                                           
                    
 
                 
 
                                                                           
                                                   
 
              

      
                                                              
                                              
                                                    
    
  
               
        
                                                                           */
typedef void (*WCTS_NotifyCBType) (WCTS_HandleType        wctsHandle, 
                                   WCTS_NotifyEventType   wctsEvent,
                                   void*                  wctsNotifyCBData);

/*                                                                           
                   
 
                 
 
                                                                           
                                                                               
                                                                          
                                                                          
                                                                          
 
              

      
                                                         
                           
                                  
                                                   
    
  
               
        
                                                                           */
typedef void (*WCTS_RxMsgCBType) (WCTS_HandleType       wctsHandle, 
                                  void*                 pMsg,
                                  wpt_uint32            uLen,
                                  void*                 wctsRxMsgCBData);

/*                                                                            
                                       
                                                                            */
typedef struct
{
     WCTS_NotifyCBType      wctsNotifyCB;
     void*                  wctsNotifyCBData;
     WCTS_RxMsgCBType       wctsRxMsgCB;
     void*                  wctsRxMsgCBData;
} WCTS_TransportCBsType;

/*                                                                        
                                              
                                                                           */
/* 
                                                                           
                                                                            
                                                 


                                                                   
                                                                         
                                                                           
                                                                       
                                                        
 
      
                                                                             
                                                                          
                                                 

*/
WCTS_HandleType  
WCTS_OpenTransport 
( 
  const wpt_uint8*         szName,
  wpt_uint32               uSize,  
  WCTS_TransportCBsType*   wctsCBs
);

/* 
                                                                
                                                            
                                                          
                                                      
                             


                                             
 
      
                        
*/
wpt_uint32
WCTS_CloseTransport 
(
  WCTS_HandleType      wctsHandle
);

/* 
                                                              
                                                
 
                                                              
                                                              
                                                              
                           

                                                               
                                                          
                                    

                                             
                                     
                                       

      
                        
*/
wpt_uint32
WCTS_SendMessage 
(
  WCTS_HandleType      wctsHandle,
  void*                pMsg,
  wpt_uint32           uLen
);

/* 
                                                               
                                          

                                           

     
                        
*/
wpt_uint32
WCTS_ClearPendingQueue
(
   WCTS_HandleType      wctsHandle
);
#endif /*                            */

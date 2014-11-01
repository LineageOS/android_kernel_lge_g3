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

#ifndef WLAN_QCT_WLANBAP_API_TIMER_H
#define WLAN_QCT_WLANBAP_API_TIMER_H

/*===========================================================================

               W L A N   B T - A M P  P A L   L A Y E R 
                    T I M E R  S E R V I C E S  A P I
                
                   
DESCRIPTION
  This file contains the timer APIs used by the wlan BT-AMP PAL layer 
  module.
  
      
  Copyright (c) 2008 QUALCOMM Incorporated. All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/


/*                                                                           

                                           


                                                                       
                                                                


                                                                                                                                                                                                                    


                                   
                                                                             
                                   

                                                                           */



/*                                                                           

                                                  

                                                                           */

/*                                                                            
                
                                                                            */
//                                             
//                                                               
#include "bapInternal.h" 

/*                                  */ 
//                              
//                   
//                     

/*                           */ 
#include "ccmApi.h"

/*                                                                            
                                         
                                                                            */
 #ifdef __cplusplus
 extern "C" {
 #endif 
 

/*                                                                            
           
                                                                            */
//           
//                 


/*                                                                            
            
                                                                            */


/*                                                                            
                                             
                                                                            */


/*                                                                            
                        
                                                                            */

/*                                                                            
                                
                                                                            */

#if 0
/*                                                                          

                                                

              
                                            
    
               
    
             

      
                                                 
                               
   
              
                                                              

                                                          
                                               

               
  
                                                                            */
VOS_STATUS 
WLANBAP_StartConnectionAcceptTimer
( 
  ptBtampContext  pBtampCtx,
  v_U32_t interval
);
#endif //  

/*                        */
VOS_STATUS WLANBAP_InitConnectionAcceptTimer 
    ( ptBtampContext  pBtampCtx);

VOS_STATUS WLANBAP_DeinitConnectionAcceptTimer
    ( ptBtampContext  pBtampCtx);

VOS_STATUS WLANBAP_StartConnectionAcceptTimer 
    (ptBtampContext  pBtampCtx, v_U32_t interval);

VOS_STATUS WLANBAP_StopConnectionAcceptTimer 
    ( ptBtampContext  pBtampCtx);

v_VOID_t WLANBAP_ConnectionAcceptTimerHandler 
    ( v_PVOID_t userData );

/*                       */
VOS_STATUS WLANBAP_InitLinkSupervisionTimer 
    ( ptBtampContext  pBtampCtx);

VOS_STATUS WLANBAP_DeinitLinkSupervisionTimer 
    ( ptBtampContext  pBtampCtx);

VOS_STATUS WLANBAP_StartLinkSupervisionTimer 
    (ptBtampContext  pBtampCtx, v_U32_t interval);

VOS_STATUS WLANBAP_StopLinkSupervisionTimer 
    ( ptBtampContext  pBtampCtx);

v_VOID_t WLANBAP_LinkSupervisionTimerHandler 
    ( v_PVOID_t userData );

/*                          */
VOS_STATUS WLANBAP_InitLogicalLinkAcceptTimer 
    ( ptBtampContext  pBtampCtx);

VOS_STATUS WLANBAP_DeinitLogicalLinkAcceptTimer 
    ( ptBtampContext  pBtampCtx);

VOS_STATUS WLANBAP_StartLogicalLinkAcceptTimer 
    (ptBtampContext  pBtampCtx, v_U32_t interval);

VOS_STATUS WLANBAP_StopLogicalLinkAcceptTimer 
    ( ptBtampContext  pBtampCtx);

v_VOID_t WLANBAP_LogicalLinkAcceptTimerHandler 
    ( v_PVOID_t userData );

/*                        */
VOS_STATUS WLANBAP_InitBEFlushTimer 
    ( ptBtampContext  pBtampCtx);

VOS_STATUS WLANBAP_DeinitBEFlushTimer 
    ( ptBtampContext  pBtampCtx);

VOS_STATUS WLANBAP_StartBEFlushTimer 
    (ptBtampContext  pBtampCtx, v_U32_t interval);

VOS_STATUS WLANBAP_StopBEFlushTimer 
    ( ptBtampContext  pBtampCtx);

v_VOID_t WLANBAP_BEFlushTimerHandler 
    ( v_PVOID_t userData );

/*                                 */
v_VOID_t 
WLANBAP_TxPacketMonitorHandler
( 
  v_PVOID_t userData 
);

/*                               */
VOS_STATUS 
WLANBAP_StartTxPacketMonitorTimer
( 
  ptBtampContext  pBtampCtx
);

/*                              */
VOS_STATUS 
WLANBAP_StopTxPacketMonitorTimer 
( 
  ptBtampContext  pBtampCtx
);

#ifdef __cplusplus
 }
#endif 


#endif /*                                      */


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

#ifndef WLAN_QCT_TLI_BA_H
#define WLAN_QCT_TLI_BA_H

/*===========================================================================

               W L A N   T R A N S P O R T   L A Y E R 
               B L O C K   A C K    I N T E R N A L  A P I
                
                   
DESCRIPTION
  This file contains the internal declarations used within wlan transport 
  layer module for BA session support, AMSDU de-aggregation and 
  MSDU reordering.
        
  Copyright (c) 2008 QUALCOMM Incorporated. All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/


/*                                                                           

                                           


                                                                       
                                                                


                                   


                                   
                                                                             
                                             
                                   

                                                                           */



/*                                                                           

                                                  

                                                                           */

/*                                                                            
                
                                                                            */
#include "wlan_qct_tli.h" 

/*                                                                           
                                    
                                                            
                                                                            */
typedef enum
{
  WLANTL_OPCODE_INVALID         = 0,
  WLANTL_OPCODE_QCUR_FWDBUF     = 1,
  WLANTL_OPCODE_FWDBUF_FWDCUR   = 2,
  WLANTL_OPCODE_QCUR            = 3,
  WLANTL_OPCODE_FWDBUF_QUEUECUR = 4,
  WLANTL_OPCODE_FWDBUF_DROPCUR  = 5,
  WLANTL_OPCODE_FWDALL_DROPCUR  = 6,
  WLANTL_OPCODE_FWDALL_QCUR     = 7,
  WLANTL_OPCODE_TEARDOWN        = 8,
  WLANTL_OPCODE_DROPCUR         = 9,
  WLANTL_OPCODE_MAX
}WLANTL_OpCodeEnumType;

void WLANTL_InitBAReorderBuffer
(
   v_PVOID_t   pvosGCtx
);

/*                                                                          

                                 

              
                                                                 
    
               
                                                                      
                   
    
             

      
                                                                        
                                                                    
                                                                         
                           
                                                   
                                                 

   
              
                                                              

                                                          
                                                                               
                                                                                 
                                                                             
                                   
                                              
    
               
  
                                                                            */
VOS_STATUS
WLANTL_BaSessionAdd 
( 
  v_PVOID_t   pvosGCtx, 
  v_U16_t     sessionID,
  v_U32_t     ucSTAId,
  v_U8_t      ucTid, 
  v_U32_t     uBufferSize,
  v_U32_t     winSize,
  v_U32_t     SSN 
);

/*                                                                          

                                 

              
                                                                   
    
               
    
             

      
                                                                        
                                                                    
                                                                         
                           
                                                   
   
              
                                                              

                                                          
                                                                               
                                                                                 
                                                                             
                                   
                                              
    
               
  
                                                                            */
VOS_STATUS
WLANTL_BaSessionDel 
( 
  v_PVOID_t           pvosGCtx, 
  v_U16_t             ucSTAId,
  v_U8_t              ucTid
);

/*                                                                          
                                 

              
                                                                       
                                                   
    

               
         
             

          
                                                
                                                                    
                                                                           
                     
   
     
                                                                         
                                                                 
                                         
                            
      
              
                                                              

               
  
                                                                            */
VOS_STATUS
WLANTL_AMSDUProcess
( 
  v_PVOID_t   pvosGCtx,
  vos_pkt_t** ppVosDataBuff, 
  v_PVOID_t   pvBDHeader,
  v_U8_t      ucSTAId,
  v_U8_t      ucMPDUHLen,
  v_U16_t     usMPDULen
);

/*                                                                          
                                

              
                    

               
         
             

     
   
                                                
                                       
                          
      
              
                                                              

               
  
                                                                            */
VOS_STATUS
WLANTL_MSDUReorder
( 
   WLANTL_CbType    *pTLCb,
   vos_pkt_t        **vosDataBuff, 
   v_PVOID_t        pvBDHeader,
   v_U8_t           ucSTAId,
   v_U8_t           ucTid
);


/*                                                                          
                     
                                                                            */

/*                                                                          
                                       

              
                                 

               
         
             

          
                                                
   
     
                                       
                          
      
              
                                                              

               
  
                                                                            */
VOS_STATUS
WLANTL_AMSDUCompleteFrame
( 
  vos_pkt_t*  vosDataBuff,
  v_U8_t      ucMPDUHLen,
  v_U16_t     usMPDULen
);

/*                                                                          

                                 

              
                                                                              
    
               
    
             

      
                                                             
                                           
                              
   
              
                                                              

                                              

    
               
  
                                                                            */
VOS_STATUS
WLANTL_QueueCurrent
(
  WLANTL_BAReorderType*  pwBaReorder,
  vos_pkt_t**            vosDataBuff,
  v_U8_t                 ucSlotIndex
);

/*                                                                          

                                   

              
                                                                          
                        
    
               
    
             

      
                                               
                                                             

       
                                                                        
   
              
                                                              

                                              

    
               
  
                                                                            */
VOS_STATUS WLANTL_ChainFrontPkts
( 
   v_U32_t                fwdIndex,
   v_U8_t                 opCode,
   vos_pkt_t              **vosDataBuff,
   WLANTL_BAReorderType   *pwBaReorder,
   WLANTL_CbType          *pTLCb
);

/*                                                                          
 
                                       
 
               
                                                                                   
              
                
                
              

     
                                                              
                                               
                               
                 
               
        
 
                                             
                
       
                                                   
                                                                             */
void WLANTL_FillReplayCounter
(
   WLANTL_BAReorderType*  pwBaReorder,
   v_U64_t                replayCounter,
   v_U8_t                 ucSlotIndex
);

#endif /*                        */

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

#if !defined( __VOS_MQ_H )
#define __VOS_MQ_H

/**=========================================================================

  \file  vos_mq.h

  \brief virtual Operating System Services (vOSS) message queue APIs

   Message Queue Definitions and API
<<<<<<< HEAD:CORE/VOSS/inc/vos_mq.h
  
   Copyright 2008 (c) Qualcomm, Incorporated.  All Rights Reserved.
   
   Qualcomm Confidential and Proprietary.
  
=======

   Copyright 2008 (c) Qualcomm Technologies, Inc.  All Rights Reserved.

   Qualcomm Technologies Confidential and Proprietary.

>>>>>>> f7413b6... wlan: voss: remove obsolete "INTEGRATED_SOC" featurization:prima/CORE/VOSS/inc/vos_mq.h
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
  
//                     
//                                                                        
//                            
// 
//                                                            
//                                                 

typedef struct vos_msg_s
{
    v_U16_t type;
    /*
                                                                          
                              
     */
    v_U16_t reserved;
    /* 
                                                      
                                                  
                                                               
                                                 
       
                                                                   
     */
    void *bodyptr;
    
    v_U32_t bodyval;    
    
} vos_msg_t;


/*                                                                         
                                        
                                                                          */

//                   
typedef enum
{
  //                                             
  VOS_MQ_ID_SME = VOS_MODULE_ID_SME,

  //                                            
  VOS_MQ_ID_PE = VOS_MODULE_ID_PE,

  //                                             
  VOS_MQ_ID_WDA = VOS_MODULE_ID_WDA,

  //                                            
  VOS_MQ_ID_TL = VOS_MODULE_ID_TL,

  //                                                        
  VOS_MQ_ID_SYS = VOS_MODULE_ID_SYS,

  //                                             
  VOS_MQ_ID_WDI = VOS_MODULE_ID_WDI,

} VOS_MQ_ID;


/*                                                                            
  
                                                                  

                                                                              
                                                
  
      
            
           
            
           
        
  
                                                                         
                        
         
                                                                           
                                                                           
                                                                         
                                                                              
                                                         
  
                                                                       
                               
          
                                                                          
                                            
          
                                                                   
          
                                                                       
                             

     
  
                                                                            */
VOS_STATUS vos_mq_post_message( VOS_MQ_ID msgQueueId, vos_msg_t *message );


/*                                                                            
  
                                                                             

                                                                           
                                                                           
                         
  
      
           
                
       
  
                                                                         
                        
         
                                                                                
                                                                           
                                                                              
                                                                             
                                                                
  
                                                                       
                               
          
                                                                          
                                            
          
                                                                   
          
                                                                       
                             

     
  
                                                                            */
VOS_STATUS vos_tx_mq_serialize( VOS_MQ_ID msgQueueId, vos_msg_t *message );

/*                                                                            

                                                                             

                                                                          
                                                                          
                         

      
            
       

                                                                         
                        

                                                                               
                                                                           
                                                                             
                                                                            
                                                                

                                                                       
                               

                                                                         
                                            

                                                              

                                                                       
                             

     

                                                                            */
VOS_STATUS vos_rx_mq_serialize( VOS_MQ_ID msgQueueId, vos_msg_t *message );


#endif //                       

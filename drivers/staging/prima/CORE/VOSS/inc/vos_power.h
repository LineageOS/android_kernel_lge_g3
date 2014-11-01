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

#ifndef _VOS_POWER_H_
#define _VOS_POWER_H_

/* 
       
             

        
                                                                      
                                                                      
                                                                     
                                                                      
                                           

*/

/*===========================================================================

  Copyright (c) 2008 QUALCOMM Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Incorporated.

===========================================================================*/

/*                                                                           

                                               

                                                                     
                                                              

          

                                   
                                                                             
                                                             
                                                                           */

/*                                                                           

                                        

                                                                           */
#include "vos_api.h"

/*                                                                           

                                             

                                                                           */
typedef enum
{
  VOS_CHIP_RESET_CMD53_FAILURE,              /*                                 */
  VOS_CHIP_RESET_FW_EXCEPTION,               /*                              */
  VOS_CHIP_RESET_MUTEX_READ_FAILURE,         /*                                       */
  VOS_CHIP_RESET_MIF_EXCEPTION,              /*                                                            */
  VOS_CHIP_RESET_UNKNOWN_EXCEPTION           /*                                        */
}vos_chip_reset_reason_type;

typedef enum
{
  VOS_CALL_SYNC,    /*                          */
  VOS_CALL_ASYNC    /*                           */    

} vos_call_status_type;

typedef v_VOID_t (*vos_power_cb_type)
(
  v_PVOID_t  user_data,    /*             */ 
  VOS_STATUS result        /*                     
                                                            
                                                                */
);

/*                                                                           

                                       

                                                                           */

/* 
                                                                  

                                                                       
                                                                           
                                                                         
                                                            

                                                                           
              

                                                                          
                                                                                
                                                                       

          
                                      
                                                                            
                                                                           
                                              
                                                                              
                                                                                        
                                                                                   
                                                                                       
                                                                                         

*/
VOS_STATUS vos_chipPowerUp
(
  vos_call_status_type* status,
  vos_power_cb_type     callback,
  v_PVOID_t             user_data
);

/* 
                                                                      

                                                                       
                                                                           
                                                                         
                                                            

                                                                          
                                                                                
                                                                       

          
                                      
                                                                            
                                                                           
                                              
                                                                              
                                                                                        
                                                                                   
                                                                                       
                                                                                         

*/
VOS_STATUS vos_chipPowerDown
(
  vos_call_status_type* status,
  vos_power_cb_type     callback,
  v_PVOID_t             user_data
);

/* 
                                                             

                                                                       
                                                                           
                                                                         
                                                            

                                                                           
                                                                     

                                                                           
              

                                                                          
                                                         
                                                                                  
                                                                                
                                                                       

          
                                      
                                                                 
                                                                            
                                                                           
                                              
                                                                              
                                                                                        
                                                                                   
                                                                                       
                                                                                         

*/
VOS_STATUS vos_chipReset
(
  vos_call_status_type* status,
  v_BOOL_t              soft,
  vos_power_cb_type     callback,
  v_PVOID_t             user_data,
  vos_chip_reset_reason_type    reason
);

/* 
                                                                        

                                                                       
                                                                           
                                                                         
                                                            

                                                                          
                                                                                
                                                                       

          
                                      
                                                                            
                                                                           
                                              
                                                                              
                                                                                        
                                                                                   
                                                                                       
                                                                                         

*/
VOS_STATUS vos_chipVoteOnPASupply
(
  vos_call_status_type* status,
  vos_power_cb_type     callback,
  v_PVOID_t             user_data
);

/* 
                                                                        
                                                                                
                                                            

                                                                       
                                                                           
                                                                         
                                                            

                                                                          
                                                                                
                                                                       

          
                                      
                                                                            
                                                                           
                                              
                                                                              
                                                                                        
                                                                                      
                                                                                    
                                                                                       
                                                                                         

*/
VOS_STATUS vos_chipVoteOffPASupply
(
  vos_call_status_type* status,
  vos_power_cb_type     callback,
  v_PVOID_t             user_data
);

/* 
                                                                   
                       

                                                                       
                                                                           
                                                                         
                                                            

                                                                          
                                                                                
                                                                       

          
                                      
                                                                            
                                                                           
                                              
                                                                              
                                                                                        
                                                                                   
                                                                                       
                                                                                         

*/
VOS_STATUS vos_chipAssertDeepSleep
(
  vos_call_status_type* status,
  vos_power_cb_type     callback,
  v_PVOID_t             user_data
);

/* 
                                                                             
                 

                                                                       
                                                                           
                                                                         
                                                            

                                                                          
                                                                                
                                                                       

          
                                      
                                                                            
                                                                           
                                              
                                                                              
                                                                                        
                                                                                   
                                                                                       
                                                                                         

*/
VOS_STATUS vos_chipDeAssertDeepSleep
(
  vos_call_status_type* status,
  vos_power_cb_type     callback,
  v_PVOID_t             user_data
);

/* 
                                                                        

                                                                       
                                                                           
                                                                         
                                                            

                                                                          
                                                                                
                                                                       

          
                                      
                                                                            
                                                                           
                                              
                                                                              
                                                                                        
                                                                                   
                                                                                       
                                                                                         

*/
VOS_STATUS vos_chipVoteOnRFSupply
(
  vos_call_status_type* status,
  vos_power_cb_type     callback,
  v_PVOID_t             user_data
);

/* 
                                                                        
                                                                                
                                                                             

                                                                       
                                                                           
                                                                         
                                                            

                                                                          
                                                                                
                                                                       

          
                                      
                                                                            
                                                                           
                                              
                                                                              
                                                                                        
                                                                                      
                                                                                    
                                                                                       
                                                                                         

*/
VOS_STATUS vos_chipVoteOffRFSupply
(
  vos_call_status_type* status,
  vos_power_cb_type     callback,
  v_PVOID_t             user_data
);
/* 
                                                                                
                           

                                                                       
                                                                           
                                                                         
                                                            

                                                                          
                                                                                
                                                                       

          
                                      
                                                                            
                                                                           
                                              
                                                                              
                                                                                        
                                                                                   
                                                                                       
                                                                                         

*/
VOS_STATUS vos_chipVoteOnBBAnalogSupply
(
  vos_call_status_type* status,
  vos_power_cb_type     callback,
  v_PVOID_t             user_data
);

/* 
                                                                                       

                                                                       
                                                                           
                                                                         
                                                            

                                                                          
                                                                                
                                                                       

          
                                      
                                                                            
                                                                           
                                              
                                                                              
                                                                                        
                                                                                      
                                                                                    
                                                                                       
                                                                                         

*/
VOS_STATUS vos_chipVoteOffBBAnalogSupply
(
  vos_call_status_type* status,
  vos_power_cb_type     callback,
  v_PVOID_t             user_data
);
/* 
                                                                                    
                                                                           

                                                                       
                                                                           
                                                                         
                                                            

                                                                          
                                                                                
                                                                       

          
                                      
                                                                            
                                                                           
                                              
                                                                              
                                                                                        
                                                                                   
                                                                                       
                                                                                         

*/
VOS_STATUS vos_chipVoteOnXOBuffer
(
  vos_call_status_type* status,
  vos_power_cb_type     callback,
  v_PVOID_t             user_data
);

/* 
                                                                           

                                                                       
                                                                           
                                                                         
                                                            

                                                                          
                                                                                
                                                                       

          
                                      
                                                                            
                                                                           
                                              
                                                                              
                                                                                        
                                                                                      
                                                                                    
                                                                                       
                                                                                         

*/
VOS_STATUS vos_chipVoteOffXOBuffer
(
  vos_call_status_type* status,
  vos_power_cb_type     callback,
  v_PVOID_t             user_data
);

/* 
                                                                         

                                                                       
                                                                           
                                                                         
                                                            

                                                                          
                                                                                
                                                                       
                                                                          

          
                                      
                                                                            
                                                                           
                                              
                                                                              
                                                                                        
                                                                                      
                                                                                    
                                                                                       
                                                                                         

*/
VOS_STATUS vos_chipVoteXOCore
(
  vos_call_status_type* status,
  vos_power_cb_type     callback,
  v_PVOID_t             user_data,
  v_BOOL_t              force_enable
);


/* 
                                                                                               
  
                                                                       
                                                                           
                                                                         
                                                            

                                                                                 
                                                                                
                                                                                  
              
   
                                                                          
                                                                                
                                                                       
                                                                                            
          
                                      
                                                                            
                                                                           
                                              
                                                                              
                                                                                        
                                                                                      
                                                                                    
                                                                                       
                                                                                         

*/
VOS_STATUS vos_chipVoteFreqFor1p3VSupply
(
  vos_call_status_type* status,
  vos_power_cb_type     callback,
  v_PVOID_t             user_data,
  v_U32_t               freq
);


#endif /*               */

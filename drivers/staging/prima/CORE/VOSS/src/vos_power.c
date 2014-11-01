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
#include <vos_power.h>

#ifdef MSM_PLATFORM
#include <mach/mpp.h>
#include <mach/vreg.h>
#include <linux/err.h>
#include <linux/delay.h>
#endif //            

#include <vos_sched.h>

//                                                                       
#define VOS_PWR_WIFI_ON_OFF_HACK
#ifdef VOS_PWR_WIFI_ON_OFF_HACK
#define VOS_PWR_SLEEP(ms) msleep(ms)
#else
#define VOS_PWR_SLEEP(ms)
#endif

/*                                                                           

                                             

                                                                           */

#define CHIP_POWER_ON         1
#define CHIP_POWER_OFF        0

//                                  
#define WLAN_LOW_SD_CONFIG_CLOCK_FREQ 400000

/*                                                                           

                                       

                                                                           */

/* 
                                                                  

                                                                       
                                                                           
                                                                         
                                                            

                                                                           
              

                                                                          
                                                                                
                                                                       

          
                                      
                                                                            
                                                                           
                                              
                                                                              
                                                                                        
                                                                                   
                                                                                       
                                                                                         

*/
VOS_STATUS vos_chipPowerUp
(
  vos_call_status_type* status,
  vos_power_cb_type     callback,
  v_PVOID_t             user_data
)
{
   return VOS_STATUS_SUCCESS;
}

/* 
                                                                      

                                                                       
                                                                           
                                                                         
                                                            

                                                                          
                                                                                
                                                                       

          
                                      
                                                                            
                                                                           
                                              
                                                                              
                                                                                        
                                                                                   
                                                                                       
                                                                                         

*/
VOS_STATUS vos_chipPowerDown
(
  vos_call_status_type* status,
  vos_power_cb_type     callback,
  v_PVOID_t             user_data
)
{
   return VOS_STATUS_SUCCESS;
}

/* 
                                                             

                                                                       
                                                                           
                                                                         
                                                            

                                                                           
                                                                     

                                                                           
              

                                                                          
                                                         
                                                                                  
                                                                                
                                                                       

          
                                      
                                                                 
                                                                            
                                                                           
                                              
                                                                              
                                                                                        
                                                                                   
                                                                                       
                                                                                         

*/
VOS_STATUS vos_chipReset
(
  vos_call_status_type* status,
  v_BOOL_t              soft,
  vos_power_cb_type     callback,
  v_PVOID_t             user_data,
  vos_chip_reset_reason_type   reason
)
{
   VOS_STATUS vstatus;
   vstatus = vos_watchdog_chip_reset(reason);
   return vstatus;
}


/* 
                                                                        

                                                                       
                                                                           
                                                                         
                                                            

                                                                          
                                                                                
                                                                       

          
                                      
                                                                            
                                                                           
                                              
                                                                              
                                                                                        
                                                                                   
                                                                                       
                                                                                         

*/
VOS_STATUS vos_chipVoteOnPASupply
(
  vos_call_status_type* status,
  vos_power_cb_type     callback,
  v_PVOID_t             user_data
)
{
   return VOS_STATUS_SUCCESS;
}


/* 
                                                                        
                                                                                
                                                            

                                                                       
                                                                           
                                                                         
                                                            

                                                                          
                                                                                
                                                                       

          
                                      
                                                                            
                                                                           
                                              
                                                                              
                                                                                        
                                                                                      
                                                                                    
                                                                                       
                                                                                         

*/
VOS_STATUS vos_chipVoteOffPASupply
(
  vos_call_status_type* status,
  vos_power_cb_type     callback,
  v_PVOID_t             user_data
)
{
   return VOS_STATUS_SUCCESS;
}


/* 
                                                                   
                       

                                                                       
                                                                           
                                                                         
                                                            

                                                                          
                                                                                
                                                                       

          
                                      
                                                                            
                                                                           
                                              
                                                                              
                                                                                        
                                                                                   
                                                                                       
                                                                                         

*/
VOS_STATUS vos_chipAssertDeepSleep
(
  vos_call_status_type* status,
  vos_power_cb_type     callback,
  v_PVOID_t             user_data
)
{
   return VOS_STATUS_SUCCESS;
}


/* 
                                                                             
                 

                                                                       
                                                                           
                                                                         
                                                            

                                                                          
                                                                                
                                                                       

          
                                      
                                                                            
                                                                           
                                              
                                                                              
                                                                                        
                                                                                   
                                                                                       
                                                                                         

*/
VOS_STATUS vos_chipDeAssertDeepSleep
(
  vos_call_status_type* status,
  vos_power_cb_type     callback,
  v_PVOID_t             user_data
)
{
   return VOS_STATUS_SUCCESS;
}

/* 
                                                                                        
                             

                                                                       
                                                                           
                                                                         
                                                            

                                                                          
                                                                                
                                                                       

          
                                      
                                                                            
                                                                           
                                              
                                                                              
                                                                                        
                                                                                   
                                                                                       
                                                                                         

*/
VOS_STATUS vos_chipExitDeepSleepVREGHandler
(
   vos_call_status_type* status,
   vos_power_cb_type     callback,
   v_PVOID_t             user_data
)
{
   return VOS_STATUS_SUCCESS;
}

/* 
                                                                        

                                                                       
                                                                           
                                                                         
                                                            

                                                                          
                                                                                
                                                                       

          
                                      
                                                                            
                                                                           
                                              
                                                                              
                                                                                        
                                                                                   
                                                                                       
                                                                                         

*/
VOS_STATUS vos_chipVoteOnRFSupply
(
  vos_call_status_type* status,
  vos_power_cb_type     callback,
  v_PVOID_t             user_data
)
{
   return VOS_STATUS_SUCCESS;
}

/* 
                                                                        
                                                                                
                                                                             

                                                                       
                                                                           
                                                                         
                                                            

                                                                          
                                                                                
                                                                       

          
                                      
                                                                            
                                                                           
                                              
                                                                              
                                                                                        
                                                                                      
                                                                                    
                                                                                       
                                                                                         

*/
VOS_STATUS vos_chipVoteOffRFSupply
(
  vos_call_status_type* status,
  vos_power_cb_type     callback,
  v_PVOID_t             user_data
)
{
   return VOS_STATUS_SUCCESS;
}

/* 
                                                                                
                           

                                                                       
                                                                           
                                                                         
                                                            

                                                                          
                                                                                
                                                                       

          
                                      
                                                                            
                                                                           
                                              
                                                                              
                                                                                        
                                                                                   
                                                                                       
                                                                                         

*/
VOS_STATUS vos_chipVoteOnBBAnalogSupply
(
  vos_call_status_type* status,
  vos_power_cb_type     callback,
  v_PVOID_t             user_data
)
{
   return VOS_STATUS_SUCCESS;
}

/* 
                                                                                       

                                                                       
                                                                           
                                                                         
                                                            

                                                                          
                                                                                
                                                                       

          
                                      
                                                                            
                                                                           
                                              
                                                                              
                                                                                        
                                                                                      
                                                                                    
                                                                                       
                                                                                         

*/
VOS_STATUS vos_chipVoteOffBBAnalogSupply
(
  vos_call_status_type* status,
  vos_power_cb_type     callback,
  v_PVOID_t             user_data
)
{
   return VOS_STATUS_SUCCESS;
}

/* 
                                                                                    
                                                                           

                                                                       
                                                                           
                                                                         
                                                            

                                                                          
                                                                                
                                                                       

          
                                      
                                                                            
                                                                           
                                              
                                                                              
                                                                                        
                                                                                   
                                                                                       
                                                                                         

*/
VOS_STATUS vos_chipVoteOnXOBuffer
(
  vos_call_status_type* status,
  vos_power_cb_type     callback,
  v_PVOID_t             user_data
)
{
   return VOS_STATUS_SUCCESS;
}

/* 
                                                                           

                                                                       
                                                                           
                                                                         
                                                            

                                                                          
                                                                                
                                                                       

          
                                      
                                                                            
                                                                           
                                              
                                                                              
                                                                                        
                                                                                      
                                                                                    
                                                                                       
                                                                                         

*/
VOS_STATUS vos_chipVoteOffXOBuffer
(
  vos_call_status_type* status,
  vos_power_cb_type     callback,
  v_PVOID_t             user_data
)
{
   return VOS_STATUS_SUCCESS;
}

/* 
                                                              

                                                                       
                                                                           
                                                                         
                                                            

                                                                          
                                                                                
                                                                       
                                                        

          
                                      
                                                                            
                                                                           
                                              
                                                                              
                                                                                        
                                                                                      
                                                                                    
                                                                                       
                                                                                         

*/
VOS_STATUS vos_chipVoteXOCore
(
  vos_call_status_type* status,
  vos_power_cb_type     callback,
  v_PVOID_t             user_data,
  v_BOOL_t              force_enable
)
{

   return VOS_STATUS_SUCCESS;
}


/* 
                                                                                               
  
                                                                       
                                                                           
                                                                         
                                                            

                                                                                 
                                                                                
                                                                                  
              
   
                                                                          
                                                                                
                                                                       
                                                                                            
          
                                      
                                                                            
                                                                           
                                              
                                                                              
                                                                                        
                                                                                      
                                                                                    
                                                                                       
                                                                                         

*/
VOS_STATUS vos_chipVoteFreqFor1p3VSupply
(
  vos_call_status_type* status,
  vos_power_cb_type     callback,
  v_PVOID_t             user_data,
  v_U32_t               freq
)
{
    return VOS_STATUS_SUCCESS;
}

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
#if !defined( WLAN_QCT_SYS_H__ )
#define WLAN_QCT_SYS_H__

/**===========================================================================

  \file  wlan_qct_sys.h

  \brief System module API

               Copyright 2008 (c) Qualcomm Technologies, Inc.
               All Rights Reserved.
               Qualcomm Technologies Confidential and Proprietary.

  ==========================================================================*/

/*          */

/*                                                                           
               
                                                                           */
#include <vos_types.h>
#include <vos_status.h>
#include <vos_mq.h>

/*                                                                           
                                        
                                                                           */


/*                                                                           
                   
                                                                           */

  /*                                                                            

                                                        

                                                                       
                        

                                                                       
                                         

                 

                                                          

                                                                            */
typedef v_VOID_t ( * sysResponseCback ) ( v_VOID_t *pUserData );



typedef enum
{
   SYS_MSG_ID_MC_START,
   SYS_MSG_ID_MC_THR_PROBE,
   SYS_MSG_ID_MC_TIMER,

   SYS_MSG_ID_TX_THR_PROBE,
   SYS_MSG_ID_TX_TIMER,

   SYS_MSG_ID_RX_TIMER,

   SYS_MSG_ID_MC_STOP,
   SYS_MSG_ID_FTM_RSP,

} SYS_MSG_ID;

/*                                                                           
                                        
                                                                           */

/*                                                                           
                                        
                                                                           */

/*                                                                            

                                                                          

                                                                            
                                                                           
                                                                           
                               

                                                                         
                                                                          
                                                                           
                                                             

                                                                  
                                                           

                                                             

         

     

                                                                            */
VOS_STATUS sysBuildMessageHeader( SYS_MSG_ID sysMsgId, vos_msg_t *pMsg );

/*                                                                            

                                                      

                                                                       
                                

                                                                       
                                          

                                                                            
                                                                         

                                                                            
                                             

                                                                       
                         

     

                                                                            */
VOS_STATUS sysOpen( v_CONTEXT_t pVosContext );


/*                                                                            

                                                                

                                                                       
                                                                

                                                 

                                                                      
                                            

                                                                       
                                                           

                              

                                                                     
                                            

     

                                                                            */
VOS_STATUS sysMcStart( v_CONTEXT_t pVosContext, sysResponseCback userCallback,
                       v_VOID_t *pUserData );


/*                                                                            

                                         

                                     

                                                 

                                                 

                                                         

                                                                    

     

                                                                            */
VOS_STATUS sysStop( v_CONTEXT_t pVosContext );


/*                                                                            

                                           

                                                                      
                                                                            
                                                            

                                                 

                                                        

                                                                    

                             

                                                                            */
VOS_STATUS sysClose( v_CONTEXT_t pVosContext );


/*                                                                            

                                                                  

                                                                              
                                                                          
                                                                        
            

                                                                     
                                                                        
                                          

                                                 

                                                                      
                                                          

                                                                       
                                                           

                              
                                                                

                             

                                                                            */
v_VOID_t sysMcThreadProbe( v_CONTEXT_t pVosContex, sysResponseCback userCallback,
                           v_VOID_t *pUserData );

/*                                                                            

                                                 

                                                                 
                                                                          
                                                               

                                                                    
                                                                        
                 

                                                 

                                                                      
                                             

                                                                       
                                                           

                              
                                                                

                             

                                                                            */
v_VOID_t sysTxThreadProbe( v_CONTEXT_t pVosContex, sysResponseCback userCallback,
                           v_VOID_t *pUserData );

/*                                                                            

                                                                               

                                                                     
                                                                          
                                                                       
        

                                                                         
                                                                            
                                                                           
                             

                                                                        
                                                                          
                                                               

                                                 

                                                       

                                                                        

                                                                           
                                           
     

                                                                            */
VOS_STATUS sysMcProcessMsg( v_CONTEXT_t pVosContext, vos_msg_t* pMsg );

/*                                                                            

                                                                  

                                                        
                                                                          
                                                                       
        

                                                                         
                                                                            
                                                                           
                             

                                                                        
                                                                          
                                                               

                                                 

                                                       

                                                                        

                                                                           
                                           

     

                                                                            */
VOS_STATUS sysTxProcessMsg( v_CONTEXT_t pVContext, vos_msg_t* pMsg );

/*                                                                            

                                                                  

                                                        
                                                                          
                                                                       
        

                                                                         
                                                                            
                                                                           
                             

                                                                        
                                                                          
                                                               

                                                 

                                                       

                                                                        

                                                                           
                                           

     

                                                                            */
VOS_STATUS sysRxProcessMsg( v_CONTEXT_t pVContext, vos_msg_t* pMsg );

/*                                                                            

                                                                              

                                                                             
                                                                             
                                                               

                                                 

                                        

                  

                                                                            */
v_VOID_t sysMcFreeMsg( v_CONTEXT_t pVosContext, vos_msg_t* pMsg );

/*                                                                            

                                                                 

                                                                
                                                                             
                                                               

                                                 

                                        

                  

                                                                            */
v_VOID_t sysTxFreeMsg( v_CONTEXT_t pVContext, vos_msg_t* pMsg );

/*                                                                            

                                                      

                                                                        
                                                                                             


                                                   


                  

                                                                            */

void wlan_sys_ftm(void *pMsgPtr);
void wlan_sys_probe(void);


#endif  //                 


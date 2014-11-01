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

/*===========================================================================

                      b a p A p i D e b u g . C
                                               
  OVERVIEW:
  
  This software unit holds the implementation of the WLAN BAP modules
  Debug functions.
  
  The functions externalized by this module are to be called ONLY by other 
  WLAN modules (HDD) that properly register with the BAP Layer initially.

  DEPENDENCIES: 

  Are listed for each API below. 
  
  
  Copyright (c) 2008 QUALCOMM Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*                                                                           

                                           


                                                                       
                                                                


                                                                                                                                                                                                                 


                                      
                                                                             
                                    

                                                                           */

/*                                                                            
                
                                                                            */
//                        
#include "vos_trace.h"

/*                            */ 
#include "bapApi.h" 
#include "bapInternal.h" 

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

/*                                                                            
                                               
                                                                            */

/*                                                                            
                                    
                                                                           */

/*                                                                            
                                          
                                                                            */

/*
              
*/

/*                                                                            

                                        

              
                                                                 
                                                                        
                       

               
        

             

      
                                                                              
                                                                      
   
          
                                                                      
                                                                       
                                                                 
   
              
                                                              

                                                               
                                                     
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPReadLoopbackMode
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Read_Loopback_Mode_Cmd  *pBapHCIReadLoopbackMode,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */
)
{

    return VOS_STATUS_SUCCESS;
} /*                          */

/*                                                                            

                                         

              
                                                                  
                                                                        
                       

               
        

             

      
                                                                              
                                                                                  
    
          
                                                                      
                                                                       
                                                                 
   
              
                                                              

                                                                     
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPWriteLoopbackMode
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Write_Loopback_Mode_Cmd   *pBapHCIWriteLoopbackMode,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */
)
{

    return VOS_STATUS_SUCCESS;
} /*                           */






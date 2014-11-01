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

#if !defined( __VOS_STATUS_H )
#define __VOS_STATUS_H

/**=========================================================================
  
  \file  vos_Status.h
  
  \brief virtual Operating System Services (vOSS) Status codes
               
   Basic status codes/definitions used by vOSS 
  
   Copyright 2008 (c) Qualcomm, Incorporated.  All Rights Reserved.
   
   Qualcomm Confidential and Proprietary.
  
  ========================================================================*/

/*          */

/*                                                                          
               
                                                                          */

/*                                                                           
                                        
                                                                          */


/*                                                                           
                   
                                                                          */

typedef enum
{
   //                    
   VOS_STATUS_SUCCESS,
   
   //                                                                 
   //                                     
   VOS_STATUS_E_RESOURCES,
  
   //                                                           
   //                      
   VOS_STATUS_E_NOMEM,  
   
   //                                                                
   VOS_STATUS_E_AGAIN,
   
   //                                                               
   //                                                            
   VOS_STATUS_E_INVAL,
   
   //                                                            
   //                                                          
   //                                        
   VOS_STATUS_E_FAULT,

   //                                                               
   //                                      
   VOS_STATUS_E_ALREADY,
    
   //                                                               
   //                         
   VOS_STATUS_E_BADMSG,

   //                                                     
   VOS_STATUS_E_BUSY,

   //                                                   
   VOS_STATUS_E_CANCELED,

   //                                                  
   VOS_STATUS_E_ABORTED,
   
   //                                                                   
   //                                       
   VOS_STATUS_E_NOSUPPORT,
   
   //                             
   VOS_STATUS_E_PERM,
   
   //                                              
   VOS_STATUS_E_EMPTY,
  
   //                                                              
   //                                       
   VOS_STATUS_E_EXISTS,
   
   //                     
   VOS_STATUS_E_TIMEOUT,
   
   //                                                              
   //                                    
   VOS_STATUS_E_FAILURE   

} VOS_STATUS;

//                                                                  
//                                                                 
//                                                            
//           
// 
//                  
// 
//                                             
// 
#define VOS_IS_STATUS_SUCCESS( status ) ( VOS_STATUS_SUCCESS == ( status ) )



#endif //                           

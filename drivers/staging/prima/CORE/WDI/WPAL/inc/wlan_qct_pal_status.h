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

#if !defined( __WLAN_QCT_PAL_STATUS_H )
#define __WLAN_QCT_PAL_STATUS_H

/**=========================================================================
  
  \file  wlan_qct_pal_status.h
  
  \brief define status PAL exports. wpt = (Wlan Pal Type)
               
   Definitions for platform independent. 
  
   Copyright 2010 (c) Qualcomm, Incorporated.  All Rights Reserved.
   
   Qualcomm Confidential and Proprietary.
  
  ========================================================================*/

typedef enum
{
   //                    
   eWLAN_PAL_STATUS_SUCCESS,
   
   //                                                                 
   //                                     
   eWLAN_PAL_STATUS_E_RESOURCES,
  
   //                                                           
   //                      
   eWLAN_PAL_STATUS_E_NOMEM,  
      
   //                                                               
   //                                                            
   eWLAN_PAL_STATUS_E_INVAL,
   
   //                                                            
   //                                                          
   //                                        
   eWLAN_PAL_STATUS_E_FAULT,

   //                                                     
   eWLAN_PAL_STATUS_E_BUSY,

   //                                                   
   eWLAN_PAL_STATUS_E_CANCELED,

   //                                                  
   eWLAN_PAL_STATUS_E_ABORTED,
   
   //                                                                   
   //                                       
   eWLAN_PAL_STATUS_E_NOSUPPORT,
   
   //                                              
   eWLAN_PAL_STATUS_E_EMPTY,
  
   //                                                              
   //                                       
   eWLAN_PAL_STATUS_E_EXISTS,
   
   //                     
   eWLAN_PAL_STATUS_E_TIMEOUT,
   
   //                                                              
   //                                    
   eWLAN_PAL_STATUS_E_FAILURE,
} wpt_status;


#define WLAN_PAL_IS_STATUS_SUCCESS(status) ( eWLAN_PAL_STATUS_SUCCESS == (status) )

#endif //                        

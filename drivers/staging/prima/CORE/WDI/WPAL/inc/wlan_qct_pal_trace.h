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

#if !defined( __WLAN_QCT_PAL_TRACE_H )
#define __WLAN_QCT_PAL_TRACE_H

/**=========================================================================
  
  \file  wlan_qct_pal_api.h
  
  \brief define general APIs PAL exports. wpt = (Wlan Pal Type) wpal = (Wlan PAL)
               
   Definitions for platform independent
  
   Copyright 2010 (c) Qualcomm, Incorporated.  All Rights Reserved.
   
   Qualcomm Confidential and Proprietary.
  
  ========================================================================*/

#include "wlan_qct_pal_type.h"
#include "wlan_qct_pal_status.h"

typedef enum
{
   //                                                                      
   //                                                               
   eWLAN_PAL_TRACE_LEVEL_NONE = 0,
   
   //                                                                      
   //                                                                         
   //                                                                 
   eWLAN_PAL_TRACE_LEVEL_FATAL,
   eWLAN_PAL_TRACE_LEVEL_ERROR, 
   eWLAN_PAL_TRACE_LEVEL_WARN,  
   eWLAN_PAL_TRACE_LEVEL_INFO,
   eWLAN_PAL_TRACE_LEVEL_INFO_HIGH,
   eWLAN_PAL_TRACE_LEVEL_INFO_MED,
   eWLAN_PAL_TRACE_LEVEL_INFO_LOW,
   
   //                                                                            
   //                                                              
   eWLAN_PAL_TRACE_LEVEL_ALL, 
   
   //                                                           
   //                          
   eWLAN_PAL_TRACE_LEVEL_COUNT
} wpt_tracelevel;

#include "wlan_qct_os_trace.h"

/*                                                                            
  
                                                                          
  
                                                                             
  
                                                                     
                                                               
                                                                         
                                                                       
                             
                                                                         
                             
         
                  
    
     
                                                                            */
void wpalTraceSetLevel( wpt_moduleid module, wpt_tracelevel level,
                        wpt_boolean on );

/*                                                                             
  
                               
  
                                                                       
                                                         
  
                                                                           
                                                                            
                                      
         
                                                                            
                                                                                        
                                                          

                                                                                       
    
                                                                                    
    
      
                                                                            */
wpt_boolean wpalTraceCheckLevel( wpt_moduleid module, wpt_tracelevel level );


/*                                                                            
  
                                                                      
                                         
  
                                                                     
             
  
             

                  
    
     
                                                                            */
void wpalTraceDisplay(void);

#define WPAL_BUG VOS_BUG
#endif //                       

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

#if !defined( __WLAN_QCT_PAL_SYNC_H )
#define __WLAN_QCT_PAL_SYNC_H

/**=========================================================================
  
  \file  wlan_pal_sync.h
  
  \brief define synchronization objects PAL exports. wpt = (Wlan Pal Type) wpal = (Wlan PAL)
               
   Definitions for platform independent. 
  
   Copyright 2010 (c) Qualcomm, Incorporated.  All Rights Reserved.
   
   Qualcomm Confidential and Proprietary.
  
  ========================================================================*/

#include "wlan_qct_pal_type.h"
#include "wlan_qct_pal_status.h"
#include "wlan_qct_os_sync.h"


#define WLAN_PAL_WAIT_INFINITE    0xFFFFFFFF

/*                                                                           
                                             
          
                                                                  
           
                                                           
                                                                           */
wpt_status wpalMutexInit(wpt_mutex *pMutex);

/*                                                                           
                                               
          
                                                                  
           
                                                           
                                                                           */
wpt_status wpalMutexDelete(wpt_mutex *pMutex);

/*                                                                           
                                                                                          
          
                                                                  
           
                                                           
                                                                           */
wpt_status wpalMutexAcquire(wpt_mutex *pMutex);

/*                                                                           
                                                  
          
                                                                  
           
                                                           
                                                                           */
wpt_status wpalMutexRelease(wpt_mutex *pMutex);

/*                                                                           
                                              
          
                                                                  
           
                                                           
                                                                           */
wpt_status wpalEventInit(wpt_event *pEvent);

/*                                                                           
                                                
          
                                                                  
           
                                                           
                                                                           */
wpt_status wpalEventDelete(wpt_event *pEvent);

/*                                                                           
                                           
          
                                                                  
                                                                                         
           
                                                           
                                                                           */
wpt_status wpalEventWait(wpt_event *pEvent, wpt_uint32 timeout);

/*                                                                           
                                                        
          
                                                                  
           
                                                           
                                                                           */
wpt_status wpalEventSet(wpt_event *pEvent);

/*                                                                           
                                                              
          
                                                                  
           
                                                           
                                                                           */
wpt_status wpalEventReset(wpt_event *pEvent);


#endif //                      

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



/**=========================================================================
  
  \file  wlan_qct_pal_timer.c
  
  \brief Implementation trace/logging APIs PAL exports. wpt = (Wlan Pal Type) wpal = (Wlan PAL)
               
   Definitions for platform Windows.
  
   Copyright 2010-2011 (c) Qualcomm, Incorporated.  All Rights Reserved.
   
   Qualcomm Confidential and Proprietary.
  
  ========================================================================*/

#include "wlan_qct_pal_timer.h"
#include "wlan_qct_pal_trace.h"
#include "wlan_qct_os_status.h"
#include "vos_threads.h"

#include <linux/delay.h>

/*                                                                           
                                                    

                                                                         
                                                                           */
static void wpalTimerCback( void * userData )
{
   wpt_timer *pTimer = (wpt_timer *)userData;

   if(NULL != pTimer->callback)
   {
      pTimer->callback(pTimer->pUserData);
   }
   else
   {
      WPAL_TRACE( eWLAN_MODULE_PAL, eWLAN_PAL_TRACE_LEVEL_WARN, " %s pTimer(%d) callback after deleted \n",
         __func__, (wpt_uint32)pTimer );
   }
}/*              */

/*                                                                           
                                                     

                                                               
                                      
                                                                         

                                                                       
                                                                           */
wpt_status wpalTimerInit(wpt_timer * pTimer, wpal_timer_callback callback, void *pUserData)
{
   /*               */
   if( pTimer == NULL || callback == NULL )
   {
      WPAL_TRACE( eWLAN_MODULE_PAL, eWLAN_PAL_TRACE_LEVEL_ERROR, " %s Wrong param pTimer(%d) callback(%d)\n",
         __func__, (wpt_uint32)pTimer, (wpt_uint32)callback );
      return eWLAN_PAL_STATUS_E_INVAL;
   }

   if ( vos_timer_init( &pTimer->timer.timerObj, VOS_TIMER_TYPE_SW, 
                        wpalTimerCback, (void*)pTimer ) == VOS_STATUS_SUCCESS )
   {
      pTimer->callback = callback;
      pTimer->pUserData = pUserData;
      return eWLAN_PAL_STATUS_SUCCESS;
   }

   return eWLAN_PAL_STATUS_E_FAILURE;
}/*             */


/*                                                                           
                                                          

                                                                

                                                                
                                                                           */
wpt_status wpalTimerDelete(wpt_timer *pTimer)
{
   wpt_status status;

   /*               */
   if( pTimer == NULL )
   {
      WPAL_TRACE( eWLAN_MODULE_PAL, eWLAN_PAL_TRACE_LEVEL_ERROR, " %s Wrong param pTimer(%d)\n",
         __func__, (wpt_uint32)pTimer );
      return eWLAN_PAL_STATUS_E_INVAL;
   }

   status = WPAL_VOS_TO_WPAL_STATUS( vos_timer_destroy(&pTimer->timer.timerObj));

   if( status == eWLAN_PAL_STATUS_SUCCESS )
   {
      pTimer->callback = NULL;
      pTimer->pUserData = NULL;
   }

   return status;
}/*               */


/*                                                                           
                                                                  

                                                                  
                                                                         

           
                                                           
                                                                           */
wpt_status wpalTimerStart(wpt_timer * pTimer, wpt_uint32 timeout)
{
   /*               */
   if( pTimer == NULL )
   {
      WPAL_TRACE( eWLAN_MODULE_PAL, eWLAN_PAL_TRACE_LEVEL_ERROR, " %s Wrong param pTimer(%d)\n",
         __func__, (wpt_uint32)pTimer );
      return eWLAN_PAL_STATUS_E_INVAL;
   }
   return ( WPAL_VOS_TO_WPAL_STATUS( vos_timer_start( &pTimer->timer.timerObj,
                                                     timeout ) ) );
}/*              */


/*                                                                           
                                                                              
                                                                 

                                                                  

           
                                                           
                                                                           */
wpt_status wpalTimerStop(wpt_timer * pTimer)
{
   /*               */
   if( pTimer == NULL )
   {
      WPAL_TRACE( eWLAN_MODULE_PAL, eWLAN_PAL_TRACE_LEVEL_ERROR, " %s Wrong param pTimer(%d)\n",
         __func__, (wpt_uint32)pTimer );
      return eWLAN_PAL_STATUS_E_INVAL;
   }
   return (WPAL_VOS_TO_WPAL_STATUS( vos_timer_stop( &pTimer->timer.timerObj )));
}/*             */

/*                                                                           
                                                                  

                                                                  

           
                       
                                                                           */
WPAL_TIMER_STATE wpalTimerGetCurStatus(wpt_timer * pTimer)
{
   /*               */
   if( pTimer == NULL )
   {
      WPAL_TRACE( eWLAN_MODULE_PAL, eWLAN_PAL_TRACE_LEVEL_ERROR, " %s Wrong param pTimer(%d)\n",
         __func__, (wpt_uint32)pTimer );
      return eWLAN_PAL_STATUS_E_INVAL;
   }
   return vos_timer_getCurrentState( &pTimer->timer.timerObj );
}/*                     */

/*                                                                           
                                                                  

           
                                    
                                                                           */
wpt_uint32 wpalGetSystemTime(void)
{
   return vos_timer_get_system_time();
}/*                 */

/*                                                                           
                                              
          
                                                                    
           
                                                           
                                                                           */
wpt_status wpalSleep(wpt_uint32 timeout)
{
   vos_sleep( timeout );
   return eWLAN_PAL_STATUS_SUCCESS;
}

/*                                                                           
                                                       
          
                                                                     
           
            
                                                                           */
void wpalBusyWait(wpt_uint32 usecDelay)
{
   vos_busy_wait(usecDelay);
   return;
}

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
  @file VossWrapper.c

  @brief This source file contains the various function definitions for the 
  RTOS abstraction layer, implemented for VOSS

  Copyright (c) 2008 QUALCOMM Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*                                                                            
    
                                             
   
   
                                                                           
                                                                 
   
   
                                    
   
   
                                      
                                                                             
                                                                         
                                
                                                                              
                                                                         
                                                                            
                                               
                                                                             
                                           
                                                                             
                                                                           
                             
                                                                               
                                                    
                                    
     
                                                                           */ 

/*                                                                           
                
                                                                           */
#include "VossWrapper.h"

#ifdef WLAN_DEBUG
#define TIMER_NAME (timer_ptr->timerName)
#else
#define TIMER_NAME "N/A"
#endif

/*                                                                      
                 
  
            
  
         
  
               
  
        
  
           
  
                                                      
  
 */ 
v_ULONG_t tx_time_get( void )
{
   return(vos_timer_get_system_ticks());

} //               


/*                                                                      
                       
  
            
  
         
  
               
  
        
  
           
  
                      
  
 */
v_UINT_t tx_timer_activate(TX_TIMER *timer_ptr)
{
   VOS_STATUS status;
  
    //                                                                         
    //                               

    //                                                            
    //                             

    //                                                                            
    //                              

    if(NULL == timer_ptr)
        return TX_TIMER_ERROR;

    //                                
    if (TX_AIRGO_TMR_SIGNATURE != timer_ptr->tmrSignature) {
        VOS_ASSERT( timer_ptr->tmrSignature == 0 );

        return TX_TIMER_ERROR;

    }

    //                                 
    VOS_ASSERT(0 != strlen(TIMER_NAME));

    VOS_TRACE(VOS_MODULE_ID_SYS, VOS_TRACE_LEVEL_INFO, 
            "Timer %s being activated\n", TIMER_NAME);

    status = vos_timer_start( &timer_ptr->vosTimer, 
         timer_ptr->initScheduleTimeInMsecs );

   if (VOS_STATUS_SUCCESS == status)
   {
      VOS_TRACE(VOS_MODULE_ID_SYS, VOS_TRACE_LEVEL_INFO, 
            "Timer %s now activated\n", TIMER_NAME);
      return TX_SUCCESS;
   }
   else if (VOS_STATUS_E_ALREADY == status)
   {
      //                                                                    
      VOS_TRACE(VOS_MODULE_ID_SYS, VOS_TRACE_LEVEL_INFO, 
            "Timer %s is already running\n", TIMER_NAME);
      return TX_SUCCESS;
   }
   else
   {
      VOS_TRACE(VOS_MODULE_ID_SYS, VOS_TRACE_LEVEL_ERROR, 
            "Timer %s fails to activate\n", TIMER_NAME);
      return TX_TIMER_ERROR;
   }
} /*                         */


/*                                                                      
                     
  
            
  
         
  
               
  
        
  
           
  
                      
  
 */
v_UINT_t tx_timer_change(TX_TIMER *timer_ptr, 
      v_ULONG_t initScheduleTimeInTicks, v_ULONG_t rescheduleTimeInTicks)
{
   //                                
   if (TX_AIRGO_TMR_SIGNATURE != timer_ptr->tmrSignature) {
       VOS_ASSERT( timer_ptr->tmrSignature == 0 );

       return TX_TIMER_ERROR;      
   }

    //                                                    
    if (VOS_TIMER_STATE_STOPPED == vos_timer_getCurrentState(&timer_ptr->vosTimer))
    {
       timer_ptr->initScheduleTimeInMsecs = TX_MSECS_IN_1_TICK * initScheduleTimeInTicks;
       timer_ptr->rescheduleTimeInMsecs = TX_MSECS_IN_1_TICK * rescheduleTimeInTicks;
       return TX_SUCCESS;
    }
    else
    {
       return TX_TIMER_ERROR;
    }
} /*                       */

/*                                                                      
                             
  
            
  
         
  
               
  
        
  
           
  
                      
  
 */
v_UINT_t tx_timer_change_context(TX_TIMER *timer_ptr, tANI_U32 expiration_input)
{

    //                                
    if (TX_AIRGO_TMR_SIGNATURE != timer_ptr->tmrSignature) {
       VOS_ASSERT( timer_ptr->tmrSignature == 0 );

       return TX_TIMER_ERROR;      
    }

    //                                                    
    if (VOS_TIMER_STATE_STOPPED == vos_timer_getCurrentState(&timer_ptr->vosTimer))
    {
       timer_ptr->expireInput = expiration_input;
       return TX_SUCCESS;
    }
    else
    {
       return TX_TIMER_ERROR;
    }
} /*                       */


/*                                                                      
                        
  
            
  
         
  
               
  
        
  
           
  
                
  
 */
static v_VOID_t tx_main_timer_func( v_PVOID_t functionContext )
{
   TX_TIMER *timer_ptr = (TX_TIMER *)functionContext;


   if (NULL == timer_ptr)
   {
       VOS_ASSERT(0);
       return;
   }


   if (NULL == timer_ptr->pExpireFunc)
   {
       VOS_ASSERT(0);
       return;
   }

   VOS_TRACE(VOS_MODULE_ID_SYS, VOS_TRACE_LEVEL_INFO, 
             "Timer %s triggered", TIMER_NAME);

   //                                                                 
   //                          
   (* timer_ptr->pExpireFunc)( timer_ptr->pMac, timer_ptr->expireInput );

   //                                      
   if (0 != timer_ptr->rescheduleTimeInMsecs)
   {
      VOS_STATUS status;
      status = vos_timer_start( &timer_ptr->vosTimer, 
                                timer_ptr->rescheduleTimeInMsecs );
      timer_ptr->rescheduleTimeInMsecs = 0;

      if (VOS_STATUS_SUCCESS != status)
      {
         VOS_TRACE(VOS_MODULE_ID_SYS, VOS_TRACE_LEVEL_WARN, 
             "Unable to reschedule timer %s; status=%d", TIMER_NAME, status);
      }
   }
} /*                       */

#ifdef TIMER_MANAGER
v_UINT_t tx_timer_create_intern_debug( v_PVOID_t pMacGlobal, TX_TIMER *timer_ptr,
   char *name_ptr, 
   v_VOID_t ( *expiration_function )( v_PVOID_t, tANI_U32 ),
   tANI_U32 expiration_input, v_ULONG_t initScheduleTimeInTicks, 
   v_ULONG_t rescheduleTimeInTicks, v_ULONG_t auto_activate, 
   char* fileName, v_U32_t lineNum)
{
    VOS_STATUS status;

    if (NULL == expiration_function)
    {
        VOS_TRACE(VOS_MODULE_ID_SYS, VOS_TRACE_LEVEL_ERROR,
                "NULL timer expiration");
        VOS_ASSERT(0);
        return TX_TIMER_ERROR;
    }

    if(NULL == name_ptr)
    {

        VOS_TRACE(VOS_MODULE_ID_SYS, VOS_TRACE_LEVEL_ERROR,
                "NULL name pointer for timer");
        VOS_ASSERT(0);
        return TX_TIMER_ERROR;
    }

    if (!initScheduleTimeInTicks)
        return TX_TICK_ERROR;

    if (!timer_ptr)
        return TX_TIMER_ERROR;

    //                           
    timer_ptr->pExpireFunc = expiration_function;
    timer_ptr->expireInput = expiration_input;
    timer_ptr->initScheduleTimeInMsecs =
        TX_MSECS_IN_1_TICK * initScheduleTimeInTicks;
    timer_ptr->rescheduleTimeInMsecs =
        TX_MSECS_IN_1_TICK * rescheduleTimeInTicks;
    timer_ptr->pMac = pMacGlobal;

    //                                                   
    timer_ptr->tmrSignature = TX_AIRGO_TMR_SIGNATURE;

#ifdef WLAN_DEBUG
    //                     
    strlcpy(timer_ptr->timerName, name_ptr, sizeof(timer_ptr->timerName));
#endif //                                           

    status = vos_timer_init_debug( &timer_ptr->vosTimer, VOS_TIMER_TYPE_SW, 
          tx_main_timer_func, (v_PVOID_t)timer_ptr, fileName, lineNum);
    if (VOS_STATUS_SUCCESS != status)
    {
       VOS_TRACE(VOS_MODULE_ID_SYS, VOS_TRACE_LEVEL_ERROR,
             "Cannot create timer for %s\n", TIMER_NAME);
       return TX_TIMER_ERROR;
    }

    if(0 != rescheduleTimeInTicks)
    {
        VOS_TRACE(VOS_MODULE_ID_SYS, VOS_TRACE_LEVEL_INFO, 
                  "Creating periodic timer for %s\n", TIMER_NAME);
    }

    //                                
    if (auto_activate)
    {
        tx_timer_activate(timer_ptr);
    }

    return TX_SUCCESS;

} //                         
#else
v_UINT_t tx_timer_create_intern( v_PVOID_t pMacGlobal, TX_TIMER *timer_ptr,
   char *name_ptr, 
   v_VOID_t ( *expiration_function )( v_PVOID_t, tANI_U32 ),
   tANI_U32 expiration_input, v_ULONG_t initScheduleTimeInTicks, 
   v_ULONG_t rescheduleTimeInTicks, v_ULONG_t auto_activate )
{
    VOS_STATUS status;

    if((NULL == name_ptr) || (NULL == expiration_function))
        return TX_TIMER_ERROR;

    if (!initScheduleTimeInTicks)
        return TX_TICK_ERROR;

    if (!timer_ptr)
        return TX_TIMER_ERROR;

    //                           
    timer_ptr->pExpireFunc = expiration_function;
    timer_ptr->expireInput = expiration_input;
    timer_ptr->initScheduleTimeInMsecs =
        TX_MSECS_IN_1_TICK * initScheduleTimeInTicks;
    timer_ptr->rescheduleTimeInMsecs =
        TX_MSECS_IN_1_TICK * rescheduleTimeInTicks;
    timer_ptr->pMac = pMacGlobal;

    //                                                   
    timer_ptr->tmrSignature = TX_AIRGO_TMR_SIGNATURE;

#ifdef WLAN_DEBUG
    //                     
    strlcpy(timer_ptr->timerName, name_ptr, sizeof(timer_ptr->timerName));
#endif //                                           

    status = vos_timer_init( &timer_ptr->vosTimer, VOS_TIMER_TYPE_SW, 
          tx_main_timer_func, (v_PVOID_t)timer_ptr );
    if (VOS_STATUS_SUCCESS != status)
    {
       VOS_TRACE(VOS_MODULE_ID_SYS, VOS_TRACE_LEVEL_ERROR,
             "Cannot create timer for %s\n", TIMER_NAME);
       return TX_TIMER_ERROR;
    }

    if(0 != rescheduleTimeInTicks)
    {
        VOS_TRACE(VOS_MODULE_ID_SYS, VOS_TRACE_LEVEL_INFO, 
                  "Creating periodic timer for %s\n", TIMER_NAME);
    }

    //                                
    if (auto_activate)
    {
        tx_timer_activate(timer_ptr);
    }

    return TX_SUCCESS;

} //                         
#endif


/*                                                                      
                         
  
            
  
         
  
               
  
        
  
           
  
                      
  
 */
v_UINT_t tx_timer_deactivate(TX_TIMER *timer_ptr)
{
   VOS_STATUS vStatus;
   VOS_TRACE(VOS_MODULE_ID_SYS, VOS_TRACE_LEVEL_INFO, 
             "tx_timer_deactivate() called for timer %s\n", TIMER_NAME);

   //                                
   if (TX_AIRGO_TMR_SIGNATURE != timer_ptr->tmrSignature)
   {
      return TX_TIMER_ERROR;      
   }

   //                                                                    
   vStatus = vos_timer_stop( &timer_ptr->vosTimer );
   if (VOS_STATUS_SUCCESS != vStatus)
   {
      VOS_TRACE(VOS_MODULE_ID_SYS, VOS_TRACE_LEVEL_INFO_HIGH, 
                "Unable to stop timer %s; status =%d\n", 
                TIMER_NAME, vStatus);
   }

   return TX_SUCCESS;

} /*                           */

v_UINT_t tx_timer_delete( TX_TIMER *timer_ptr )
{
   VOS_TRACE(VOS_MODULE_ID_SYS, VOS_TRACE_LEVEL_INFO, 
             "tx_timer_delete() called for timer %s\n", TIMER_NAME);

   //                                
   if (TX_AIRGO_TMR_SIGNATURE != timer_ptr->tmrSignature)
   {
      return TX_TIMER_ERROR;      
   }

   vos_timer_destroy( &timer_ptr->vosTimer );
   return TX_SUCCESS;     
} /*                       */



/*                                                                      
                      
  
            
  
         
  
               
  
        
  
           
  
                      
  
 */
v_BOOL_t tx_timer_running(TX_TIMER *timer_ptr)
{
   VOS_TRACE(VOS_MODULE_ID_SYS, VOS_TRACE_LEVEL_INFO, 
             "tx_timer_running() called for timer %s\n", TIMER_NAME);

   //                                
   if (TX_AIRGO_TMR_SIGNATURE != timer_ptr->tmrSignature)
      return VOS_FALSE;      

   if (VOS_TIMER_STATE_RUNNING == 
       vos_timer_getCurrentState( &timer_ptr->vosTimer ))
   {
       return VOS_TRUE;
   }
   return VOS_FALSE;

} /*                        */

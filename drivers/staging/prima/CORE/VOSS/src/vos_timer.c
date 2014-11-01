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

  \file  vos_timer.c

  \brief virtual Operating System Servies (vOS)

   Definitions for vOSS Timer services
<<<<<<< HEAD:CORE/VOSS/src/vos_timer.c
  
   Copyright 2008 (c) Qualcomm, Incorporated.  All Rights Reserved.
   
   Qualcomm Confidential and Proprietary.
  
=======

   Copyright 2008 (c) Qualcomm Technologies, Inc.  All Rights Reserved.

   Qualcomm Technologies Confidential and Proprietary.

>>>>>>> f7413b6... wlan: voss: remove obsolete "INTEGRATED_SOC" featurization:prima/CORE/VOSS/src/vos_timer.c
  ========================================================================*/

/*          */

/*                                                                          
               
                                                                          */
#include <vos_timer.h>
#include <vos_lock.h>
#include <vos_api.h>
#include "wlan_qct_sys.h"
#include "vos_sched.h"

/*                                                                           
                                        
                                                                          */

#define LINUX_TIMER_COOKIE 0x12341234
#define LINUX_INVALID_TIMER_COOKIE 0xfeedface
#define TMR_INVALID_ID ( 0 )

/*                                                                           
                   
                                                                          */

/*                                                                            
                              
                                                                            */
static unsigned int        persistentTimerCount;
static vos_lock_t          persistentTimerCountLock;
//                                              

/*                                                                          
                                        
                                                                          */
//                                                         

//                                                    
//                                                                       
static void tryAllowingSleep( VOS_TIMER_TYPE type )
{
   if ( VOS_TIMER_TYPE_WAKE_APPS == type )
   {
     //                                               
      persistentTimerCount--;
      if ( 0 == persistentTimerCount )
      {
         //                                                                 
         //                             
         //                                       
      }
      //                                              
   }
}


/*                                                                            
  
                                                                         
                                                 

                                                                      
                                                              
  
  
                                                                
                                  

                                                                                
                                                               

                                   

                                   

                                   
  
                  
                                                                            */

static void vos_linux_timer_callback ( v_U32_t data ) 
{
   vos_timer_t *timer = ( vos_timer_t *)data; 
   vos_msg_t msg;
   VOS_STATUS vStatus;
   unsigned long flags;
   
   vos_timer_callback_t callback=NULL;
   v_PVOID_t userData=NULL;
   int threadId;
   VOS_TIMER_TYPE type=VOS_TIMER_TYPE_SW;
   
   VOS_ASSERT(timer);

   if (timer == NULL)
   {
     VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, "%s Null pointer passed in!",__func__);
     return;
   }

   threadId = timer->platformInfo.threadID;
   spin_lock_irqsave( &timer->platformInfo.spinlock,flags );
   
   switch ( timer->state )
   {
   case VOS_TIMER_STATE_STARTING:
      //                                                                      
      //                                                                   
      //                               
      timer->state = VOS_TIMER_STATE_STOPPED;
      vStatus = VOS_STATUS_E_ALREADY;
      break;
   case VOS_TIMER_STATE_STOPPED:
      vStatus = VOS_STATUS_E_ALREADY;
      break;
   case VOS_TIMER_STATE_UNUSED:
      vStatus = VOS_STATUS_E_EXISTS;
      break;
   case VOS_TIMER_STATE_RUNNING:
      //                                                                          
      //                                  
      timer->state = VOS_TIMER_STATE_STOPPED;
      //                                                        
      //                                                                            
      //               
      callback = timer->callback;
      userData = timer->userData;
      threadId = timer->platformInfo.threadID;
      type = timer->type;
      vStatus = VOS_STATUS_SUCCESS;
      break;
   default:
      VOS_ASSERT(0);
      vStatus = VOS_STATUS_E_FAULT;
      break;
   }
   
   spin_unlock_irqrestore( &timer->platformInfo.spinlock,flags );
   
   if ( VOS_STATUS_SUCCESS != vStatus )
   {
      VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, 
                "TIMER callback called in a wrong state=%d", timer->state);
      return;
   }

   tryAllowingSleep( type );

   if (callback == NULL)
   {
       VOS_ASSERT(0);
       VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                 "%s: No TIMER callback, Could not enqueue timer to any queue",
                 __func__);
       return;
   }

   //                                                             
   if ( vos_sched_is_tx_thread( threadId ) )
   {
      VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO, 
          "TIMER callback: running on TX thread");
         
      //                          
      sysBuildMessageHeader( SYS_MSG_ID_TX_TIMER, &msg );
      msg.bodyptr  = callback;
      msg.bodyval  = (v_U32_t)userData; 
       
      if(vos_tx_mq_serialize( VOS_MQ_ID_SYS, &msg ) == VOS_STATUS_SUCCESS)
         return;
   }
   else if ( vos_sched_is_rx_thread( threadId ) )
   {
      VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO, 
          "TIMER callback: running on RX thread");
         
      //                          
      sysBuildMessageHeader( SYS_MSG_ID_RX_TIMER, &msg );
      msg.bodyptr  = callback;
      msg.bodyval  = (v_U32_t)userData; 
       
      if(vos_rx_mq_serialize( VOS_MQ_ID_SYS, &msg ) == VOS_STATUS_SUCCESS)
         return;
   }
   else 
   {
      VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,
          "TIMER callback: running on MC thread");
                    
      //                           
      sysBuildMessageHeader( SYS_MSG_ID_MC_TIMER, &msg );
      msg.bodyptr  = callback;
      msg.bodyval  = (v_U32_t)userData; 
       
      if(vos_mq_post_message( VOS_MQ_ID_SYS, &msg ) == VOS_STATUS_SUCCESS)
        return;
   }     

   VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, 
             "%s: Could not enqueue timer to any queue", __func__);
   VOS_ASSERT(0);
}

/*                                                                           
  
                                                                         

                                  
  
                     
  
     
  
                                                                           */
VOS_TIMER_STATE vos_timer_getCurrentState( vos_timer_t *pTimer )
{
   if ( NULL == pTimer )
   {
      VOS_ASSERT(0);
      return VOS_TIMER_STATE_UNUSED;
   }

   switch ( pTimer->state )
   {
      case VOS_TIMER_STATE_STOPPED:
      case VOS_TIMER_STATE_STARTING:
      case VOS_TIMER_STATE_RUNNING:
      case VOS_TIMER_STATE_UNUSED:
         return pTimer->state;
      default:
         VOS_ASSERT(0);
         return VOS_TIMER_STATE_UNUSED;
   }    
}

/*                                                                            
  
                                                                   

                                                                      
                                               

     
  
                                                                            */

void vos_timer_module_init( void )
{
   VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO, 
         "Initializing the VOSS timer module");
   vos_lock_init( &persistentTimerCountLock );
}

#ifdef TIMER_MANAGER
#include "wlan_hdd_dp_utils.h"

hdd_list_t vosTimerList;

static void vos_timer_clean(void);

void vos_timer_manager_init()
{
   /*                                                 */
   hdd_list_init(&vosTimerList, 1000);  
   return;
}

static void vos_timer_clean()
{
    v_SIZE_t listSize;
    unsigned long flags;
        
    hdd_list_size(&vosTimerList, &listSize);
    
    if (listSize)
    {
       hdd_list_node_t* pNode;
       VOS_STATUS vosStatus;

       timer_node_t *ptimerNode;
       VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                 "%s: List is not Empty. listSize %d ",
                 __func__, (int)listSize);

       do
       {
          spin_lock_irqsave(&vosTimerList.lock, flags);
          vosStatus = hdd_list_remove_front(&vosTimerList, &pNode);
          spin_unlock_irqrestore(&vosTimerList.lock, flags);
          if (VOS_STATUS_SUCCESS == vosStatus)
          {
             ptimerNode = (timer_node_t*)pNode;
             VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_FATAL,
                       "Timer Leak@ File %s, @Line %d", 
                       ptimerNode->fileName, (int)ptimerNode->lineNum);

             vos_mem_free(ptimerNode);
          }
       } while (vosStatus == VOS_STATUS_SUCCESS);
    }
}

void vos_timer_exit()
{
    vos_timer_clean();
    hdd_list_destroy(&vosTimerList);
}
#endif
  
/*                                                                          
  
                                                    

                                           
  
                                                                           
                                                                         
                             
  
                                                                        
                                                                          
                                                                        
                                              
  
                                                            
                                                                     
                                                                    
                                             
  
                                                                        
                                                                        
                            
  
                                                                 
  
                                                                        
                                 
        
                                                                      
                                                     
                                                                       
                                                               
                                
  
                                                                     
                 
         
                                                                     
                                                                          
  
                                                                      
                              
  
                                                                        
                                                 

                                                                        
                   

                                                                         
                                                                     
                                                  

                                                                 
     
  
                                                                           */
#ifdef TIMER_MANAGER
VOS_STATUS vos_timer_init_debug( vos_timer_t *timer, VOS_TIMER_TYPE timerType, 
                           vos_timer_callback_t callback, v_PVOID_t userData, 
                           char* fileName, v_U32_t lineNum )
{
   VOS_STATUS vosStatus;
    unsigned long flags;
   //                          
   if ((timer == NULL) || (callback == NULL)) 
   {
      VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, 
                "%s: Null params being passed",__func__);
      VOS_ASSERT(0);
      return VOS_STATUS_E_FAULT;
   }

   timer->ptimerNode = vos_mem_malloc(sizeof(timer_node_t));

   if(timer->ptimerNode == NULL)
   {
      VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, 
                "%s: Not able to allocate memory for timeNode",__func__);
      VOS_ASSERT(0);
      return VOS_STATUS_E_FAULT;
   }

   vos_mem_set(timer->ptimerNode, sizeof(timer_node_t), 0);

    timer->ptimerNode->fileName = fileName;
    timer->ptimerNode->lineNum   = lineNum;
    timer->ptimerNode->vosTimer = timer;

    spin_lock_irqsave(&vosTimerList.lock, flags);
    vosStatus = hdd_list_insert_front(&vosTimerList, &timer->ptimerNode->pNode);
    spin_unlock_irqrestore(&vosTimerList.lock, flags);
    if(VOS_STATUS_SUCCESS != vosStatus)
    {
         VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, 
             "%s: Unable to insert node into List vosStatus %d\n", __func__, vosStatus);
    }
   
   //                                                
   //                                             
   spin_lock_init(&timer->platformInfo.spinlock);
   init_timer(&(timer->platformInfo.Timer));
   timer->platformInfo.Timer.function = vos_linux_timer_callback;
   timer->platformInfo.Timer.data = (unsigned long)timer;
   timer->callback = callback;
   timer->userData = userData;
   timer->type = timerType;
   timer->platformInfo.cookie = LINUX_TIMER_COOKIE;
   timer->platformInfo.threadID = 0;
   timer->state = VOS_TIMER_STATE_STOPPED;
   
   return VOS_STATUS_SUCCESS;
}
#else
VOS_STATUS vos_timer_init( vos_timer_t *timer, VOS_TIMER_TYPE timerType, 
                           vos_timer_callback_t callback, v_PVOID_t userData )
{
   //                          
   if ((timer == NULL) || (callback == NULL)) 
   {
      VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, 
                "%s: Null params being passed",__func__);
      VOS_ASSERT(0);
      return VOS_STATUS_E_FAULT;
   }
   
   //                                                
   //                                             
   spin_lock_init(&timer->platformInfo.spinlock);
   init_timer(&(timer->platformInfo.Timer));
   timer->platformInfo.Timer.function = vos_linux_timer_callback;
   timer->platformInfo.Timer.data = (unsigned long)timer;
   timer->callback = callback;
   timer->userData = userData;
   timer->type = timerType;
   timer->platformInfo.cookie = LINUX_TIMER_COOKIE;
   timer->platformInfo.threadID = 0;
   timer->state = VOS_TIMER_STATE_STOPPED;
   
   return VOS_STATUS_SUCCESS;
}
#endif


/*                                                                           
  
                                                          

                                                                     
                                                                  
                                           
   
                                                           
                                                                     
                                              
  
                                                             
                                                                     
                                                               
                      
  
                                                  
  
                                                                
  
                                                                         
                                                                      
                                                                        
                    

                                                                       
          
                                                                 
     
  
                                                                           */
#ifdef TIMER_MANAGER
VOS_STATUS vos_timer_destroy ( vos_timer_t *timer )
{
   VOS_STATUS vStatus=VOS_STATUS_SUCCESS;
   unsigned long flags;
   
   //                          
   if ( NULL == timer )
   {
      VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, 
                "%s: Null timer pointer being passed",__func__);
      VOS_ASSERT(0);
      return VOS_STATUS_E_FAULT;
   }
       
   //                                                 
   if ( LINUX_TIMER_COOKIE != timer->platformInfo.cookie )
   {
      VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, 
                "%s: Cannot destroy uninitialized timer",__func__);
      return VOS_STATUS_E_INVAL;
   }
   
   spin_lock_irqsave(&vosTimerList.lock, flags);
   vStatus = hdd_list_remove_node(&vosTimerList, &timer->ptimerNode->pNode);
   spin_unlock_irqrestore(&vosTimerList.lock, flags);
   if(vStatus != VOS_STATUS_SUCCESS)
   {
      VOS_ASSERT(0);
      return VOS_STATUS_E_INVAL;
   }
   vos_mem_free(timer->ptimerNode);
   

   spin_lock_irqsave( &timer->platformInfo.spinlock,flags );
   
   switch ( timer->state )
   {
      case VOS_TIMER_STATE_STARTING:
         vStatus = VOS_STATUS_E_BUSY;
         break;
      case VOS_TIMER_STATE_RUNNING:
         /*                      */
         del_timer(&(timer->platformInfo.Timer));
         vStatus = VOS_STATUS_SUCCESS;
         break;
      case VOS_TIMER_STATE_STOPPED:
         vStatus = VOS_STATUS_SUCCESS;
         break;
      case VOS_TIMER_STATE_UNUSED:
         vStatus = VOS_STATUS_E_ALREADY;
         break;
      default:
         vStatus = VOS_STATUS_E_FAULT;
         break;
   }

   if ( VOS_STATUS_SUCCESS == vStatus )
   {
      timer->platformInfo.cookie = LINUX_INVALID_TIMER_COOKIE;
      timer->state = VOS_TIMER_STATE_UNUSED;
      spin_unlock_irqrestore( &timer->platformInfo.spinlock,flags );
      return vStatus;
   }

   spin_unlock_irqrestore( &timer->platformInfo.spinlock,flags );


   VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, 
             "%s: Cannot destroy timer in state = %d",__func__, timer->state);
   VOS_ASSERT(0);

   return vStatus;   
}

#else
VOS_STATUS vos_timer_destroy ( vos_timer_t *timer )
{
   VOS_STATUS vStatus=VOS_STATUS_SUCCESS;
   unsigned long flags;
   
   //                          
   if ( NULL == timer )
   {
      VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, 
                "%s: Null timer pointer being passed",__func__);
      VOS_ASSERT(0);
      return VOS_STATUS_E_FAULT;
   }
       
   //                                                 
   if ( LINUX_TIMER_COOKIE != timer->platformInfo.cookie )
   {
      VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, 
                "%s: Cannot destroy uninitialized timer",__func__);
      return VOS_STATUS_E_INVAL;
   }
   spin_lock_irqsave( &timer->platformInfo.spinlock,flags );
   
   switch ( timer->state )
   {
      case VOS_TIMER_STATE_STARTING:
         vStatus = VOS_STATUS_E_BUSY;
         break;
      case VOS_TIMER_STATE_RUNNING:
         /*                      */
         del_timer(&(timer->platformInfo.Timer));
         vStatus = VOS_STATUS_SUCCESS;
         break;
      case VOS_TIMER_STATE_STOPPED:
         vStatus = VOS_STATUS_SUCCESS;
         break;
      case VOS_TIMER_STATE_UNUSED:
         vStatus = VOS_STATUS_E_ALREADY;
         break;
      default:
         vStatus = VOS_STATUS_E_FAULT;
         break;
   }

   if ( VOS_STATUS_SUCCESS == vStatus )
   {
      timer->platformInfo.cookie = LINUX_INVALID_TIMER_COOKIE;
      timer->state = VOS_TIMER_STATE_UNUSED;
      spin_unlock_irqrestore( &timer->platformInfo.spinlock,flags );
      return vStatus;
   }

   spin_unlock_irqrestore( &timer->platformInfo.spinlock,flags );

   VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, 
             "%s: Cannot destroy timer in state = %d",__func__, timer->state);
   VOS_ASSERT(0);

   return vStatus;   
}
#endif

/*                                                                          
  
                                                      

                                                                       
                                                                    
                       
   
                                                              
                                                                 
                        
  
                                               
  
                                                                         
  
                                                              
  
                                                                            
                                                                        
                                                              

                                                                       
          
                                                                 
     
  
                                                                           */
VOS_STATUS vos_timer_start( vos_timer_t *timer, v_U32_t expirationTime )
{
   unsigned long flags;
     
   VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO_HIGH, 
             "Timer Addr inside voss_start : 0x%p ", timer );
   
   //                          
   if ( NULL == timer )
   {
      VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
          "%s Null timer pointer being passed", __func__);
      VOS_ASSERT(0);
      return VOS_STATUS_E_INVAL;
   }
      
   //                                                 
   if ( LINUX_TIMER_COOKIE != timer->platformInfo.cookie )
   {
      VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
          "%s: Cannot start uninitialized timer",__func__);
      if ( LINUX_INVALID_TIMER_COOKIE != timer->platformInfo.cookie )
      {
         VOS_ASSERT(0);
      }
      return VOS_STATUS_E_INVAL;
   }

   //                                                   
   if ( expirationTime < 10 )
   {
      VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                "%s: Cannot start a "
                "timer with expiration less than 10 ms", __func__);
      VOS_ASSERT(0);
      return VOS_STATUS_E_INVAL;
   }
      
   //                                                      
   spin_lock_irqsave( &timer->platformInfo.spinlock,flags );

   //                                   
   if ( VOS_TIMER_STATE_STOPPED != timer->state )
   {  
      spin_unlock_irqrestore( &timer->platformInfo.spinlock,flags );
      VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO_HIGH, 
                "%s: Cannot start timer in state = %d ",__func__, timer->state);
      return VOS_STATUS_E_ALREADY;
   }
      
   //                
   mod_timer( &(timer->platformInfo.Timer),
              jiffies + msecs_to_jiffies(expirationTime)); 

   timer->state = VOS_TIMER_STATE_RUNNING;

   //                                                      
   timer->platformInfo.threadID  = current->pid;

   if ( VOS_TIMER_TYPE_WAKE_APPS == timer->type )
   {
      persistentTimerCount++;
      if ( 1 == persistentTimerCount )
      {
         //                                                                  
         //                                        
      }
   }

   spin_unlock_irqrestore( &timer->platformInfo.spinlock,flags );
  
   return VOS_STATUS_SUCCESS;
}


/*                                                                          
  
                                             

                                                                          
                                                              
   
                                                                        
                                                                       
  
                                               
    
                                                              
  
                                                                       
          
                                                                 
     
  
                                                                          */
VOS_STATUS vos_timer_stop ( vos_timer_t *timer )
{
   unsigned long flags;

   VOS_TRACE( VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO_HIGH, 
               "%s: Timer Addr inside voss_stop : 0x%p",__func__,timer );

   //                          
   if ( NULL == timer )
   {
      VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
          "%s Null timer pointer being passed", __func__);
      VOS_ASSERT(0);
      return VOS_STATUS_E_INVAL;
   }

   //                                                 
   if ( LINUX_TIMER_COOKIE != timer->platformInfo.cookie )
   {
      VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, 
          "%s: Cannot stop uninitialized timer",__func__);
      if ( LINUX_INVALID_TIMER_COOKIE != timer->platformInfo.cookie )
      {
         VOS_ASSERT(0);
      }
      return VOS_STATUS_E_INVAL;
   }
      
   //                                  
   spin_lock_irqsave( &timer->platformInfo.spinlock,flags );

   if ( VOS_TIMER_STATE_RUNNING != timer->state )
   {
      spin_unlock_irqrestore( &timer->platformInfo.spinlock,flags );
      VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO_HIGH,
                "%s: Cannot stop timer in state = %d",
                __func__, timer->state);
      return VOS_STATUS_SUCCESS;
   }
   
   timer->state = VOS_TIMER_STATE_STOPPED;

   del_timer(&(timer->platformInfo.Timer));
       
   spin_unlock_irqrestore( &timer->platformInfo.spinlock,flags );
      
   tryAllowingSleep( timer->type );
   
   return VOS_STATUS_SUCCESS;
}


/*                                                                          
  
                                                                         

                                                                         
                                                                             
                                                                           
             
    
                                                                        
                                  
  
     
  
                                                                          */
v_TIME_t vos_timer_get_system_ticks( v_VOID_t )
{
   return( jiffies_to_msecs(jiffies) / 10 );
}


/*                                                                          
 
                                                                          

                                                                                 
                                                
    
                                                     
  
     
  
                                                                          */
v_TIME_t vos_timer_get_system_time( v_VOID_t )
{
   struct timeval tv;
   do_gettimeofday(&tv);
   return tv.tv_sec*1000 + tv.tv_usec/1000;  
}

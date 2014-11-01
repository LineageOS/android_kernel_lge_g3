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

/*============================================================================
  FILE:         vos_lock.c

  OVERVIEW:     This source file contains definitions for vOS lock APIs
                The four APIs mentioned in this file are used for 
                initializing , acquiring, releasing and destroying a lock.
                the lock are implemented using critical sections

  DEPENDENCIES: 
 
                Copyright (c) 2007 QUALCOMM Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/

/*                                                                            
                         

                                                                            */

/*                                                                            
                
                                                                            */

#include "vos_lock.h"
#include "vos_memory.h"
#include "vos_trace.h"


/*                                                                            
                                         
                                                                            */

/*                                                                            
                    
                                                                            */

#define LINUX_LOCK_COOKIE 0x12345678
enum
{
   LOCK_RELEASED = 0x11223344,
   LOCK_ACQUIRED,
   LOCK_DESTROYED
};

/*                                                                            
                          
                                                                            */

/*                                                                            
                              
                                                                            */

/*                                                                            
                                         
                                                                            */

/*                                                                          
  
                                                  
  
                                                                    
                                                                       
               

                                                                  
                                           
  
                                                                  
            
 
                                                               
  
                                                                     
                              

                                                                        
                  

                                                                         
                                                                      
                                                   

                                                              

                                                                         
                         

                                                                        
                                               
     
   
                                           
                                                                            */
VOS_STATUS vos_lock_init ( vos_lock_t *lock )
{

   //                         
   if ( lock == NULL)
   {
       VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, "%s: NULL pointer passed in",__func__);
       return VOS_STATUS_E_FAULT; 
   }
   //                                     
   if ( LINUX_LOCK_COOKIE == lock->cookie )
   {
       VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, "%s: already initialized lock",__func__);
       return VOS_STATUS_E_BUSY;
   }
      
   if (in_interrupt())
   {
      VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, "%s cannot be called from interrupt context!!!", __func__);
      return VOS_STATUS_E_FAULT; 
   }
      
   //                     
   mutex_init( &lock->m_lock ); 
   lock->cookie = LINUX_LOCK_COOKIE;
   lock->state  = LOCK_RELEASED;
   lock->processID = 0;
   lock->refcount = 0;
      
   return VOS_STATUS_SUCCESS;
}

/*                                                                          
  
                                             

                                                                           
                                                                             
                                                                            
                                                                 
  
                                          
  
                                                                     
                             
  
                                                                          
                                        
          
                                                            

                                                                         
                         
          
     
                                                                          */
VOS_STATUS vos_lock_acquire ( vos_lock_t* lock )
{
      int rc;
      //                         
      if ( lock == NULL )
      {
         VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, "%s: NULL pointer passed in",__func__);
         return VOS_STATUS_E_FAULT;
      }
      //                                              
      if ( LINUX_LOCK_COOKIE != lock->cookie )
      {
         VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, "%s: uninitialized lock",__func__);
         return VOS_STATUS_E_INVAL;
      }

      if (in_interrupt())
      {
         VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, "%s cannot be called from interrupt context!!!", __func__);
         return VOS_STATUS_E_FAULT; 
      }
      if ((lock->processID == current->pid) && 
          (lock->state == LOCK_ACQUIRED))
      {
         lock->refcount++;
#ifdef VOS_NESTED_LOCK_DEBUG
         VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,"%s: %x %d %d", __func__, lock, current->pid, lock->refcount);
#endif
         return VOS_STATUS_SUCCESS;
      }
      //               
      mutex_lock( &lock->m_lock );
      rc = mutex_is_locked( &lock->m_lock );
      if (rc == 0)
      {
         VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                "%s: unable to lock mutex (rc = %d)", __func__, rc);
         return VOS_STATUS_E_FAILURE;
      }
 
      
#ifdef VOS_NESTED_LOCK_DEBUG
      VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,"%s: %x %d", __func__, lock, current->pid);
#endif
      if ( LOCK_DESTROYED != lock->state ) 
      {
         lock->processID = current->pid;
         lock->refcount++;
         lock->state    = LOCK_ACQUIRED;
         return VOS_STATUS_SUCCESS;
      }
      else
      {
         //                          
         VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, "%s: Lock is already destroyed", __func__);
         mutex_unlock(&lock->m_lock);
         return VOS_STATUS_E_FAILURE;
      }
}


/*                                                                          
  
                                             

                                                                   
                         

                                                                   
                                     

                                   
  
                                                                 
  
                                                                          
                                        
                   
                                                                          
                                        
                   
                                                                       
                                        

                                                                         
                         
    
     
                                                                          */
VOS_STATUS vos_lock_release ( vos_lock_t *lock )
{
      //                         
      if ( lock == NULL )
      {
         VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, "%s: NULL pointer passed in",__func__);
         return VOS_STATUS_E_FAULT;
      }

      //                                                
      if ( LINUX_LOCK_COOKIE != lock->cookie )
      {
         VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, "%s: uninitialized lock",__func__);
         return VOS_STATUS_E_INVAL;
      }

      if (in_interrupt())
      {
         VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, "%s cannot be called from interrupt context!!!", __func__);
         return VOS_STATUS_E_FAULT; 
      }

      //                                       
      //                                            
      //                                     
      if ( lock->processID != current->pid )
      {
         VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, "%s: current task pid does not match original task pid!!",__func__);
#ifdef VOS_NESTED_LOCK_DEBUG
         VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,"%s: Lock held by=%d being released by=%d", __func__, lock->processID, current->pid);
#endif

         return VOS_STATUS_E_PERM;
      }
      if ((lock->processID == current->pid) && 
          (lock->state == LOCK_ACQUIRED))
      {
         if (lock->refcount > 0) lock->refcount--;
      }
#ifdef VOS_NESTED_LOCK_DEBUG
      VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,"%s: %x %d %d", __func__, lock, lock->processID, lock->refcount);
#endif
      if (lock->refcount) return VOS_STATUS_SUCCESS;
         
      lock->processID = 0;
      lock->refcount = 0;
      lock->state = LOCK_RELEASED;
      //                  
      mutex_unlock( &lock->m_lock );
#ifdef VOS_NESTED_LOCK_DEBUG
      VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_INFO,"%s: Freeing lock %x %d %d", lock, lock->processID, lock->refcount);
#endif
      return VOS_STATUS_SUCCESS;
}

/*                                                                          
  
                                                                          
                                                                          
        

                                                                   
                                                                           
                                                    
   
                                                                      
                                                                              
                                                                          
                                                                        
                                                              
                      
  
                                                
  
                                                               
  
                                                                         
                                                                      
                               

                                                                      
          
                                                            

                                                                         
                         
     
                                                                          */
VOS_STATUS vos_lock_destroy( vos_lock_t *lock )
{
      //                         
      if ( NULL == lock )
      {
         VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, "%s: NULL pointer passed in", __func__);
         return VOS_STATUS_E_FAULT; 
      }

      if ( LINUX_LOCK_COOKIE != lock->cookie )
      {
         VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, "%s: uninitialized lock", __func__);
         return VOS_STATUS_E_INVAL;
      }

      if (in_interrupt())
      {
         VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, "%s cannot be called from interrupt context!!!", __func__);
         return VOS_STATUS_E_FAULT; 
      }

      //                          
      if (!mutex_trylock(&lock->m_lock))
      {
         VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, "%s: lock is not released", __func__);
         return VOS_STATUS_E_BUSY;
      }
      lock->cookie = 0;
      lock->state = LOCK_DESTROYED;
      lock->processID = 0;
      lock->refcount = 0;

      mutex_unlock(&lock->m_lock);

         
      return VOS_STATUS_SUCCESS;
}


/*                                                                          
  
                                                            
  
                                                                              
                                                                       
               

                                                                       
                                           
  
                                                                  
            
 
                                                                
  
                                                                          
                              
                                                                            */

VOS_STATUS vos_spin_lock_init(vos_spin_lock_t *pLock)
{
   spin_lock_init(pLock);
   
   return VOS_STATUS_SUCCESS;
}

/*                                                                          
  
                                                       

                                                                                
                                                                          
                                                                            
                                                                 
  
                                           
  
                                                                     
                             
      
     
                                                                          */
VOS_STATUS vos_spin_lock_acquire(vos_spin_lock_t *pLock)
{
   spin_lock(pLock);
   return VOS_STATUS_SUCCESS;
}
/*                                                                          
  
                                                  

                                                                        
                         

                                                                   
                                     

                                    
  
                                                                 
  
     
                                                                          */
VOS_STATUS vos_spin_lock_release(vos_spin_lock_t *pLock)
{
   spin_unlock(pLock);
   return VOS_STATUS_SUCCESS;
}


/*                                                                          
  
                                                              

                                                 
  
                                                                 
  
     
                                                                          */
VOS_STATUS vos_spin_lock_destroy(vos_spin_lock_t *pLock)
{

   return VOS_STATUS_SUCCESS;
}

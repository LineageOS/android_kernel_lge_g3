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
  
  \file  vos_mq.c
  
  \brief virtual Operating System Services (vOSS) message queue APIs
               
   Message Queue Definitions and API
  
   Copyright 2008 (c) Qualcomm, Incorporated.  All Rights Reserved.
   
   Qualcomm Confidential and Proprietary.
  
  ========================================================================*/

/*          */

/*                                                                          
               
                                                                          */
#include <vos_mq.h>
#include "vos_sched.h"
#include <vos_api.h>

/*                                                                           
                                        
                                                                          */

/*                                                                           
                   
                                                                          */
  
/*                                                                          
                                        
                                                                          */
  
/*                                                                           
  
                                                        
    
                                                              
      
                                            
  
                                                                              
                              
          
                                                                          
                                    
          
                   
  
                                                                           */
__inline VOS_STATUS vos_mq_init(pVosMqType pMq)
{

  /*                        */
  if (pMq == NULL) {
     VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, 
         "%s: NULL pointer passed",__func__);
     return VOS_STATUS_E_FAILURE;
  }

  /* 
                            
  */
  spin_lock_init(&pMq->mqLock);

  /*
                                           
  */
  INIT_LIST_HEAD(&pMq->mqList);

  return VOS_STATUS_SUCCESS;
   
} /*              */

/*                                                                           
  
                                                            
    
                                                                 
      
                                            
  
              
          
                     
  
                                                                           */
__inline void vos_mq_deinit(pVosMqType pMq)
{
  /* 
                            
  */
  if (pMq == NULL) {
     VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, 
         "%s: NULL pointer passed",__func__);
     return ;
  }

  /*                                                                 */

}/*                 */


/*                                                                           
  
                                                           
    
                                                                  
      
                                            
  
                                                          
  
              
          
                  
  
                                                                           */
__inline void vos_mq_put(pVosMqType pMq, pVosMsgWrapper pMsgWrapper)
{
  unsigned long flags;

  /* 
                            
  */
  if ((pMq == NULL) || (pMsgWrapper == NULL)) {
     VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, 
         "%s: NULL pointer passed",__func__);
     return ;
  }

  spin_lock_irqsave(&pMq->mqLock, flags);

  list_add_tail(&pMsgWrapper->msgNode, &pMq->mqList);

  spin_unlock_irqrestore(&pMq->mqLock, flags);

} /*              */


/*                                                                           
  
                                                                            
    
                                                                        
                        
      
                                            
  
                                        
          
                  
  
                                                                           */
__inline pVosMsgWrapper vos_mq_get(pVosMqType pMq)
{
  pVosMsgWrapper pMsgWrapper = NULL;

  /* 
                            
  */
  struct list_head * listptr;
  unsigned long flags;
  
  if (pMq == NULL) {
     VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, 
         "%s: NULL pointer passed",__func__);
     return NULL;
  }
 
  spin_lock_irqsave(&pMq->mqLock, flags);

  if( list_empty(&pMq->mqList) )
  {
    VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_WARN,
             "%s: VOS Message Queue is empty",__func__);
  }
  else
  {
    listptr = pMq->mqList.next;
    pMsgWrapper = (pVosMsgWrapper)list_entry(listptr, VosMsgWrapper, msgNode);
    list_del(pMq->mqList.next);
  }

  spin_unlock_irqrestore(&pMq->mqLock, flags);

  return pMsgWrapper;

} /*              */


/*                                                                           
  
                                                      
  
                                                             
      
                                            
  
                                        
          
                  
  
                                                                           */
__inline v_BOOL_t vos_is_mq_empty(pVosMqType pMq)
{
  v_BOOL_t  state = VOS_FALSE;
  unsigned long flags;

  /* 
                            
  */
  if (pMq == NULL)
  {
     VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, 
         "%s: NULL pointer passed",__func__);
     return VOS_STATUS_E_FAILURE;
  }

  spin_lock_irqsave(&pMq->mqLock, flags);
  state = list_empty(&pMq->mqList)?VOS_TRUE:VOS_FALSE;
  spin_unlock_irqrestore(&pMq->mqLock, flags);

  return state;

} /*              */



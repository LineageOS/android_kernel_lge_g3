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

                       wlan_qct_wda_legacy.c

  OVERVIEW:

  This software unit holds the implementation of the WLAN Device Adaptation
  Layer for the legacy functionalities that were part of the old HAL.

  The functions externalized by this module are to be called ONLY by other
  WLAN modules that properly register with the Transport Layer initially.

  DEPENDENCIES:

  Are listed for each API below.


  Copyright (c) 2008 QUALCOMM Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*                        */
/*                                    */
#include "limApi.h"
#include "pmmApi.h"
#include "cfgApi.h"
#include "wlan_qct_wda_debug.h"

/*                      */

#define HAL_MMH_MB_MSG_TYPE_MASK    0xFF00

//                                                              
/* 
                 
  
            
                                      
  
         
  
                 
  
  
        
  
                                                 
                                   
                             
 */

tSirRetStatus
wdaPostCtrlMsg(tpAniSirGlobal pMac, tSirMsgQ *pMsg)
{
   if(VOS_STATUS_SUCCESS != vos_mq_post_message(VOS_MQ_ID_WDA, (vos_msg_t *) pMsg))
      return eSIR_FAILURE;
   else
      return eSIR_SUCCESS;
} //             

/* 
                
  
            
                                        
  
         
  
               
  
  
        
  
                                                 
                                   
                             
 */

tSirRetStatus
wdaPostCfgMsg(tpAniSirGlobal pMac, tSirMsgQ *pMsg)
{
   tSirRetStatus rc = eSIR_SUCCESS;

   do
   {
      //                                                               
      //                                                  

      cfgProcessMbMsg(pMac, (tSirMbMsg*)pMsg->bodyptr);
      rc = eSIR_SUCCESS;
   } while (0);

   return rc;
} //                


//                                                              
/* 
                  
  
            
                                                                 
                                     
  
         
  
               
                                                                         
  
        
                                                                        
                                                                       
                                                                          
                                                                     
                                          
  
                                             
               
 */

tSirRetStatus uMacPostCtrlMsg(void* pSirGlobal, tSirMbMsg* pMb)
{
   tSirMsgQ msg;
   tpAniSirGlobal pMac = (tpAniSirGlobal)pSirGlobal;


   tSirMbMsg* pMbLocal;
   msg.type = pMb->type;
   msg.bodyval = 0;

   WDALOG3(wdaLog(pMac, LOG3, FL("msgType %d, msgLen %d\n" ),
        pMb->type, pMb->msgLen));

   //                                                     
   //                                                            
   //                                                           
   //            

   //                                                                            
   pMbLocal = vos_mem_malloc(pMb->msgLen);
   if ( NULL == pMbLocal )
   {
      WDALOGE( wdaLog(pMac, LOGE, FL("Buffer Allocation failed!\n")));
      return eSIR_FAILURE;
   }

   vos_mem_copy((void *)pMbLocal, (void *)pMb, pMb->msgLen);
   msg.bodyptr = pMbLocal;

   switch (msg.type & HAL_MMH_MB_MSG_TYPE_MASK)
   {
   case WDA_MSG_TYPES_BEGIN:    //                                
      wdaPostCtrlMsg(pMac, &msg);
      break;

   case SIR_LIM_MSG_TYPES_BEGIN:    //                                
      limPostMsgApi(pMac, &msg);
      break;

   case SIR_CFG_MSG_TYPES_BEGIN:    //                                
      wdaPostCfgMsg(pMac, &msg);
      break;

   case SIR_PMM_MSG_TYPES_BEGIN:    //                                
      pmmPostMessage(pMac, &msg);
      break;

   case SIR_PTT_MSG_TYPES_BEGIN:
      WDALOGW( wdaLog(pMac, LOGW, FL("%s:%d: message type = 0x%X"),
               __func__, __LINE__, msg.type));
      vos_mem_free(msg.bodyptr);
      break;


   default:
      WDALOGW( wdaLog(pMac, LOGW, FL("Unknown message type = "
             "0x%X\n"),
             msg.type));

      //                    
      vos_mem_free(msg.bodyptr);
      break;
   }

   return eSIR_SUCCESS;

} //                  


/*                                                          
                                      
  
                                   
                                                            
 */
tBssSystemRole wdaGetGlobalSystemRole(tpAniSirGlobal pMac)
{
   v_VOID_t * pVosContext = vos_get_global_context(VOS_MODULE_ID_WDA, NULL);
   tWDA_CbContext *wdaContext = 
                       vos_get_context(VOS_MODULE_ID_WDA, pVosContext);
   if(NULL == wdaContext)
   {
      VOS_TRACE( VOS_MODULE_ID_WDA, VOS_TRACE_LEVEL_ERROR,
                           "%s:WDA context is NULL", __func__); 
      VOS_ASSERT(0);
      return eSYSTEM_UNKNOWN_ROLE;
   }
   WDALOG1( wdaLog(pMac, LOG1, FL(" returning  %d role\n"),
             wdaContext->wdaGlobalSystemRole));
   return  wdaContext->wdaGlobalSystemRole;
}


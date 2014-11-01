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

                      b a p A p i E x t . C
                                               
  OVERVIEW:
  
  This software unit holds the implementation of the external interfaces
  required by the WLAN BAP module.  It is currently a temporary 
  respository for API routines which should be furnished by CSR
  or TL, but aren't yet implemented.
  
  The functions provide by this module are called by the rest of 
  the BT-AMP PAL module.

  DEPENDENCIES: 

  Are listed for each API below. 
  
  
  Copyright (c) 2008 QUALCOMM Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*                                                                           

                                           


                                                                       
                                                                


                                                                                                                                                                                                               


                                      
                                                                             
                                    

                                                                           */

/*                                                                            
                
                                                                            */
//                                 
#include "bapApiExt.h"

//                 
/*                                                                            
                                         
                                                                            */

/*                                                                            
                    
                                                                            */

/*                                                                            
                          
                                                                            */

/*                                                                            
                                             
                                                                            */


/*                                                                            
                              
                                                                            */

/*                                                                            
                                               
                                                                            */

/*                                                                            
                                    
                                                                           */

/*                                                                            
                                          
                                                                            */

/*                                                                            
                                    
                                                                            */

/*                                                                          

                                       

              
                                            
    
               
    
             

      
                                                 
                                                   
                                                                 
   
              
                                                              

                                                                                       
                                
                                               

               
  
                                                                            */
VOS_STATUS 
WLANBAP_GetCurrentChannel
( 
  ptBtampContext  pBtampCtx,
  v_U32_t *channel, //                            
  v_U32_t *activeFlag   //                        
)
{
  //                   
  tHalHandle halHandle;

  /*                                                                        
                                   
                                                                           */

  if (( NULL == pBtampCtx ) || (NULL == channel) || (NULL == activeFlag))
  {
    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                 "Invalid BAP pointer in %s", __func__);
    return VOS_STATUS_E_FAULT;
  }

  halHandle =  VOS_GET_HAL_CB(pBtampCtx->pvosGCtx);

  if(NULL == halHandle)
  {
     VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                  "halHandle is NULL in %s", __func__);
     return VOS_STATUS_E_FAULT;
  }

  if (ccmCfgGetInt(halHandle, WNI_CFG_CURRENT_CHANNEL, channel) 
          != eHAL_STATUS_SUCCESS ) 
  {
    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                 "Get CFG failed in %s", __func__);
    return VOS_STATUS_E_FAULT;
  }

  *activeFlag  = FALSE;  //                        

  return VOS_STATUS_SUCCESS;
}/*                           */



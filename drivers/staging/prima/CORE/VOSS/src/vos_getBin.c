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

/**=============================================================================
  vos_getBin.c
  \brief
  Description...
   Copyright (c) 2012-2013 Qualcomm Atheros, Inc.
   All Rights Reserved.
   Qualcomm Atheros Confidential and Proprietary.
  ==============================================================================*/
/*          */
/*                                                                              
               
                                                                              */
#include <vos_getBin.h>
#include <linux/fs.h>       //                            
#include <vos_api.h>
#include <vos_sched.h>
#include <wlan_hdd_misc.h>
#include <wlan_hdd_main.h>
/*                                                                              
                                        
                                                                              */
/*                                                                              
                   
                                                                              */
extern tVOS_CONCURRENCY_MODE hdd_get_concurrency_mode ( void );

/*                                                                              
                                        
                                                                              */
/*                                                                            
                                                              
                                                                             
                                            
      
                 
                           
                                                                       
                                                                   
                                                  
                                                     
                                                                          
                                                        
                                                                   
                                                     
                                              
                                         
                                             
                                                               
     
                                                                            */
VOS_STATUS vos_get_binary_blob( VOS_BINARY_ID binaryId,
                                v_VOID_t *pBuffer, v_SIZE_t *pBufferSize )
{
  VOS_STATUS VosSts = VOS_STATUS_SUCCESS;
    char *pFileName;

    v_CONTEXT_t pVosContext = vos_get_global_context(VOS_MODULE_ID_SYS,NULL);

    //                                         
    switch (binaryId)
    {
        case VOS_BINARY_ID_CONFIG:
           pFileName = WLAN_CFG_FILE;
           break;
        case VOS_BINARY_ID_COUNTRY_INFO:
           pFileName = WLAN_COUNTRY_INFO_FILE;
           break;
        case VOS_BINARY_ID_HO_CONFIG:
           pFileName = WLAN_HO_CFG_FILE;
           break;
        case VOS_BINARY_ID_DICT_CONFIG:
           pFileName = WLAN_DICT_FILE;
           break;
        default:
           VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR, "Invalid binaryID");
           return VosSts;
    }
    if(0 == *pBufferSize )
    {
       /*                                                                         */
       VosSts = hdd_get_cfg_file_size(((VosContextType*)(pVosContext))->pHDDContext,pFileName,pBufferSize);

       if ( !VOS_IS_STATUS_SUCCESS( VosSts ))
       {
          VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                                    "%s : vos_open failed\n",__func__);

          return VOS_STATUS_E_FAILURE;
       }
       VosSts = VOS_STATUS_E_NOMEM;
    }
    else
    {
       if(NULL != pBuffer) {
          //                                  
          VosSts = hdd_read_cfg_file(((VosContextType*)(pVosContext))->pHDDContext,pFileName,pBuffer,pBufferSize);
       }
       else {
             VosSts = VOS_STATUS_E_FAILURE;
       }
    }

    return VosSts;
}


tVOS_CON_MODE vos_get_conparam( void )
{
    tVOS_CON_MODE con_mode;
    con_mode = hdd_get_conparam ( );
    return con_mode;
}
tVOS_CONCURRENCY_MODE vos_get_concurrency_mode( void )
{
    tVOS_CONCURRENCY_MODE con_mode;
    con_mode = hdd_get_concurrency_mode ( );
    return con_mode;
}

v_BOOL_t vos_concurrent_sessions_running(void)
{
    v_U8_t i=0;
    v_U8_t j=0;
    hdd_context_t *pHddCtx;
    v_CONTEXT_t pVosContext = vos_get_global_context( VOS_MODULE_ID_HDD, NULL );

    if (NULL != pVosContext)
    {
       pHddCtx = vos_get_context( VOS_MODULE_ID_HDD, pVosContext);
       if (NULL != pHddCtx)
       {
          for (i=0; i < VOS_MAX_NO_OF_MODE; i++)
          {
             j += pHddCtx->no_of_sessions[i];
          }
       }
    }

    return (j>1);
}


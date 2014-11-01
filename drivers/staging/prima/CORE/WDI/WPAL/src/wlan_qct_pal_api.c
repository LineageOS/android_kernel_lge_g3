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
  
  \file  wlan_qct_pal_api.c
  
  \brief Implementation general APIs PAL exports.
   wpt = (Wlan Pal Type) wpal = (Wlan PAL)
               
   Definitions for platform Windows.
  
   Copyright 2010 (c) Qualcomm, Incorporated.  All Rights Reserved.
   
   Qualcomm Confidential and Proprietary.
  
  ========================================================================*/

#include "wlan_qct_pal_api.h"
#include "wlan_qct_pal_trace.h"
#include "wlan_qct_pal_device.h"
#include "vos_trace.h"
#ifndef MEMORY_DEBUG
#include "vos_memory.h"
#endif /*              */
#include "vos_sched.h"
#include "vos_api.h"

#include "dma-mapping.h"
#include <mach/subsystem_restart.h>
#include <linux/wcnss_wlan.h>

typedef struct sPalStruct
{
   /*                         */
   void* devHandle;
} tPalContext;

#define WPAL_GET_NDIS_HANDLE(p)  ( ((tPalContext *)(p))->devHandle )

tPalContext gContext;

//                                                     
typedef struct
{
   wpt_uint32 length;
   //                                                             
   wpt_uint32 offset;   
   wpt_uint32 phyAddr;
} tPalDmaMemInfo;

/*                                                                           

                                     

                                                                           */

/* 
                        
                                            
                                                                 
                                           
                                                                          
                                       
                                                                                 
                                                               
   
                                                                         
 */
wpt_status wpalOpen(void **ppPalContext, void *pOSContext)
{
   wpt_status status;

   gContext.devHandle = pOSContext;

   status = wpalDeviceInit(pOSContext);
   if (!WLAN_PAL_IS_STATUS_SUCCESS(status))
   {
      WPAL_TRACE(eWLAN_MODULE_PAL, eWLAN_PAL_TRACE_LEVEL_FATAL,
                 "%s: wpalDeviceInit failed with status %u",
                 __func__, status);
   }

   return status;
}

/* 
                                 
                                                        
                                                     
   
                                                                         
 */
wpt_status wpalClose(void *pPalContext)
{
   wpalDeviceClose(gContext.devHandle);
   gContext.devHandle = NULL;

   return eWLAN_PAL_STATUS_SUCCESS;
}

#ifndef MEMORY_DEBUG
/* 
                                               
                                          
   
                                                   
                                  
 */
void *wpalMemoryAllocate(wpt_uint32 size)
{
   return vos_mem_malloc( size );
}

/* 
                                                 
                                          
 */
void wpalMemoryFree(void *pv)
{
   vos_mem_free( pv );
}
#endif /*              */
/* 
                                       
                                              
                                               
                                      
   
                     
                                   
                                         
 */
wpt_status wpalMemoryCopy(void * dest, void * src, wpt_uint32 size)
{
   vos_mem_copy( dest, src, size );

   return eWLAN_PAL_STATUS_SUCCESS;
}

/* 
                                             
                                 
                                 
                                         
   
                      
                                                           
                                     
 */
wpt_boolean wpalMemoryCompare(void * buf1, void * buf2, wpt_uint32 size)
{
   return (wpt_boolean)vos_mem_compare( buf1, buf2, size );
}


/*                                                                           
                                 
           
                                         
                                     
           
           
                                                                           */
void wpalMemoryZero(void *buf, wpt_uint32 size)
{
   vos_mem_zero( buf, size );
}

/* 
                                                        
                                            
                                      
                                                                               
         
 */
void wpalMemoryFill(void *buf, wpt_uint32 size, wpt_byte bFill)
{
   vos_mem_set( buf, size, bFill );
}

/* 
                                                                                   
                                          
                                                                              
   
                                                                      
                                               
 */
void *wpalDmaMemoryAllocate(wpt_uint32 size, void **ppPhysicalAddr)
{
   void *pv = NULL;
   dma_addr_t PhyAddr;
   wpt_uint32 uAllocLen = size + sizeof(tPalDmaMemInfo);
   
   pv = dma_alloc_coherent(NULL, uAllocLen, &PhyAddr, GFP_KERNEL);
   if ( NULL == pv ) 
   {
     WPAL_TRACE(eWLAN_MODULE_PAL, eWLAN_PAL_TRACE_LEVEL_ERROR, 
                 "%s Unable to allocate DMA buffer\n", __func__);
     return NULL;
   }

   
   ((tPalDmaMemInfo *)pv)->length  = uAllocLen;
   ((tPalDmaMemInfo *)pv)->phyAddr = PhyAddr;
   ((tPalDmaMemInfo *)pv)->offset  = sizeof(tPalDmaMemInfo);
   pv                              = (wpt_byte *)pv + sizeof(tPalDmaMemInfo);
   *ppPhysicalAddr                 = (void*)PhyAddr + sizeof(tPalDmaMemInfo);


   return (pv);
}/*                     */


/* 
                                                        
                                               
 */
void wpalDmaMemoryFree(void *pv)
{
   tPalDmaMemInfo *pMemInfo = (tPalDmaMemInfo *)(((wpt_byte *)pv) -
                                      sizeof(tPalDmaMemInfo));
    if(pv)
    { 
        pv = (wpt_byte *)pv - pMemInfo->offset;
        dma_free_coherent(NULL, pMemInfo->length, pv, pMemInfo->phyAddr);
    }

}/*                 */

/* 
                                                                
                  
                                    
                                                            
          
                                                       
 */
wpt_status wpalDbgReadRegister(wpt_uint32 regAddr, wpt_uint32 *pregValue)
{
   if (NULL == pregValue)
   {
      return eWLAN_PAL_STATUS_E_INVAL;
   }

   return wpalReadRegister(regAddr, pregValue);
}

/*  
                                                               
                              
                                    
                                        
          
                                                       
*/
wpt_status wpalDbgWriteRegister(wpt_uint32 regAddr, wpt_uint32 regValue)
{
   return wpalWriteRegister(regAddr, regValue);
}

/*  
          
                                                             
                                      
                       
                                 
          
                                                       
*/
wpt_status wpalDbgReadMemory(wpt_uint32 memAddr, wpt_uint8 *buf, wpt_uint32 len)
{
   return wpalReadDeviceMemory(memAddr, buf, len);
}

/*  
          
                                                                               
                                   
                                   
                             
          
                                                       
*/
wpt_status wpalDbgWriteMemory(wpt_uint32 memAddr, wpt_uint8 *buf, wpt_uint32 len)
{
   return wpalWriteDeviceMemory(memAddr, buf, len);
}

/*                                                                           
                                              

                                                                           
                             

           
           
           
                                                       
                                                                           */
wpt_status wpalDriverShutdown(void)
{
    VOS_STATUS vosStatus;
    vosStatus = vos_wlanShutdown();

    if (VOS_STATUS_SUCCESS == vosStatus) {
        return eWLAN_PAL_STATUS_SUCCESS; 
    }
    return eWLAN_PAL_STATUS_E_FAILURE; 
}

/*                                                                           
                                             

                                                                        
           

           
           
           
                                                       
                                                                           */
wpt_status wpalDriverReInit(void)
{
    VOS_STATUS vosStatus;

    vosStatus = vos_wlanReInit();
    if (VOS_STATUS_SUCCESS == vosStatus) {
        return eWLAN_PAL_STATUS_SUCCESS; 
    }
    return eWLAN_PAL_STATUS_E_FAILURE; 
}

/*                                                                           
                                                

                                                          

          
           
           
                                                       
                                                                           */
wpt_status wpalRivaSubystemRestart(void)
{
    /*                                                       
                                                            
           */
    if (vos_is_load_unload_in_progress(VOS_MODULE_ID_WDI, NULL))
    {
         WPAL_TRACE(eWLAN_MODULE_PAL, eWLAN_PAL_TRACE_LEVEL_FATAL,
                 "%s: loading/unloading in progress,"
                 " SSR will be done at the end of unload", __func__);
         return eWLAN_PAL_STATUS_E_FAILURE;
    }
    if (0 == subsystem_restart("wcnss")) 
    {
        return eWLAN_PAL_STATUS_SUCCESS;
    }
    return eWLAN_PAL_STATUS_E_FAILURE;
}

/*                                                                           
                                                 

          
           
           
           
                                                                           */
void wpalWlanReload(void)
{
   vos_wlanRestart();
   return;
}

/*                                                                           
                                                       

          
           
           
           
                                                                           */
void wpalWcnssResetIntr(void)
{
#ifdef HAVE_WCNSS_RESET_INTR
   wcnss_reset_intr();
#endif
   return;
}

/*                                                                           
                                                          

          
                                   
                               
                               
                               
                               
           
           
                                                                           */
void wpalFwDumpReq(wpt_uint32 cmd, wpt_uint32 arg1, wpt_uint32 arg2,
                    wpt_uint32 arg3, wpt_uint32 arg4)
{
   vos_fwDumpReq(cmd, arg1, arg2, arg3, arg4);
   return;
}

/*                                                                           
                                           
                                         

          
           

           
           
                                                                           */
void wpalDevicePanic(void)
{
   BUG_ON(0);
   return;
}
/*                                                                           
                                                                  

          
           
           
             
                                                                            */
int  wpalIsWDresetInProgress(void)
{
   return isWDresetInProgress();
}


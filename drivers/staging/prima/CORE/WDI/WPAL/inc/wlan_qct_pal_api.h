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

#if !defined( __WLAN_QCT_PAL_API_H )
#define __WLAN_QCT_PAL_API_H

/**=========================================================================
  
  \file  wlan_qct_pal_api.h
  
  \brief define general APIs PAL exports. wpt = (Wlan Pal Type) wpal = (Wlan PAL)
               
   Definitions for platform independent
  
   Copyright 2010 (c) Qualcomm, Incorporated.  All Rights Reserved.
   
   Qualcomm Confidential and Proprietary.
  
  ========================================================================*/

#include "wlan_qct_pal_type.h"
#include "wlan_qct_pal_status.h"

#ifdef MEMORY_DEBUG
#include "vos_memory.h"
#endif /*              */

/*                                                          */

//                                    
#define WPAL_MAX( _x, _y ) ( ( (_x) > (_y) ) ? (_x) : (_y) )  

//                                   
#define WPAL_MIN( _x, _y ) ( ( (_x) < (_y) ) ? (_x) : (_y)  )  

//                                                             
#define WPAL_CEIL_DIV( _a, _b ) (( 0 != (_a) % (_b) ) ? ( (_a) / (_b) + 1 ) : ( (_a) / (_b) ))

//                                                           
#define WPAL_FLOOR_DIV( _a, _b ) ( ( (_a) - ( (_a) % (_b) ) ) / (_b) )

#define WPAL_SWAP_U16(_x) \
   ( ( ( (_x) << 8 ) & 0xFF00 ) | ( ( (_x) >> 8 ) & 0x00FF ) )

#define WPAL_SWAP_U32(_x) \
  (( ( ( (_x) << 24 ) & 0xFF000000 ) | ( ( (_x) >> 24 ) & 0x000000FF ) ) | \
   ( ( ( (_x) << 8 ) & 0x00FF0000 ) | ( ( (_x) >> 8 ) & 0x0000FF00 ) ))

//                                                        
#ifndef ANI_BIG_BYTE_ENDIAN

//                                     
#define WPAL_CPU_TO_BE32(_x) WPAL_SWAP_U32(_x)
#define WPAL_BE32_TO_CPU(_x) WPAL_SWAP_U32(_x)
#define WPAL_CPU_TO_BE16(_x) WPAL_SWAP_U16(_x)
#define WPAL_BE16_TO_CPU(_x) WPAL_SWAP_U16(_x)
#define WPAL_CPU_TO_LE32(_x) (_x)
#define WPAL_LE32_TO_CPU(_x) (_x)
#define WPAL_CPU_TO_LE16(_x) (_x)
#define WPAL_LE16_TO_CPU(_x) (_x)

#else //                           

//                                  
#define WPAL_CPU_TO_BE32(_x) (_x)
#define WPAL_BE32_TO_CPU(_x) (_x)
#define WPAL_CPU_TO_BE16(_x) (_x)
#define WPAL_BE16_TO_CPU(_x) (_x)
#define WPAL_CPU_TO_LE32(_x) WPAL_SWAP_U32(_x)
#define WPAL_LE32_TO_CPU(_x) WPAL_SWAP_U32(_x)
#define WPAL_CPU_TO_LE16(_x) WPAL_SWAP_U16(_x)
#define WPAL_LE16_TO_CPU(_x) WPAL_SWAP_U16(_x)

#endif //                           


/*                                                                         */
/*                                                                           
                              
           
                                                                                    
                                                                                  
                                                                                   
                                                                                
                                                               
           
                                                          
                                                                           */
wpt_status wpalOpen(void **ppPalContext, void *pOSContext);

/*                                                                           
                           
           
                                                    
           
                                                          
                                                                           */
wpt_status wpalClose(void *pPalContext);


/*                                                                         */
#ifdef MEMORY_DEBUG
/*                                                                */
#define wpalMemoryAllocate vos_mem_malloc
#define wpalMemoryFree     vos_mem_free
#else

/*                                                                           
                                         
           
                                         
           
                                          
                                     
                                                                           */
void *wpalMemoryAllocate(wpt_uint32 size);

/*                                                                           
                                           
           
                                         
           
           
                                                                           */
void wpalMemoryFree(void *pv);
#endif /*              */

/*                                                                           
                                 
           
                                             
                                              
                                     
           
                               
                                     
                                                                           */
wpt_status wpalMemoryCopy(void * dest, void * src, wpt_uint32 size);


/*                                                                           
                                       
           
                                
                                
                                        
           
                                                        
                                  
                                                                           */
wpt_boolean wpalMemoryCompare(void * buf1, void * buf2, wpt_uint32 size);

/*                                                                           
                                 
           
                                         
                                     
           
           
                                                                           */
void wpalMemoryZero(void *buf, wpt_uint32 size);


/*                                                                           
                                                  
           
                                         
                                     
                                                                                    
           
           
                                                                           */
void wpalMemoryFill(void *buf, wpt_uint32 size, wpt_byte bFill);


/*                                                                           
                                                                             
           
                                        
                                         
                                                                             
           
                                                            
                                     
                                                                             */
void *wpalDmaMemoryAllocate(wpt_uint32 size, void **ppPhysicalAddr);

/*                                                                           
                                                  
           
                                        
                                              
           
           
                                                                           */
void wpalDmaMemoryFree(void *pv);



/*                                                                           
                                                              
           
                                 
                                                        
           
                                                       
                                                                           */
wpt_status wpalDbgReadRegister(wpt_uint32 regAddr, wpt_uint32 *pregValue);

/*                                                                           
                                                                             
           
                                 
                                     
           
                                                       
                                                                           */
wpt_status wpalDbgWriteRegister(wpt_uint32 regAddr, wpt_uint32 regValue);

/*                                                                           
                                                            
           
                                  
                    
                              
           
                                                       
                                                                           */
wpt_status wpalDbgReadMemory(wpt_uint32 memAddr, wpt_uint8 *buf, wpt_uint32 len);

/*                                                                           
                                                                               
           
                               
                                
                          
           
                                                       
                                                                           */
wpt_status wpalDbgWriteMemory(wpt_uint32 memAddr, wpt_uint8 *buf, wpt_uint32 len);

/*                                                                           
                                              

                                                                           
                             

           
           
           
                                                       
                                                                           */
wpt_status wpalDriverShutdown(void);

/*                                                                           
                                             

                                                                        
           

           
           
           
                                                       
                                                                           */
wpt_status wpalDriverReInit(void);

/*                                                                           
                                                

                                                          

          
           
           
                                                       
                                                                           */
wpt_status wpalRivaSubystemRestart(void);

/*                                                                           
                                                 

          
           
           
           
                                                                           */
void wpalWlanReload(void);

/*                                                                           
                                                       

          
           
           
           
                                                                           */
void wpalWcnssResetIntr(void);

/*                                                                           
                                                          

          
                                   
                               
                               
                               
                               
           
           
                                                                           */
void wpalFwDumpReq(wpt_uint32 cmd, wpt_uint32 arg1, wpt_uint32 arg2,
                    wpt_uint32 arg3, wpt_uint32 arg4);

/*                                                                           
                                           
                                         

          
           

           
           
                                                                           */
void wpalDevicePanic(void);

/*                                                                           
                                                                  

          
           
           
             
                                                                          */
int  wpalIsWDresetInProgress(void);
#endif //                     

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


/** ------------------------------------------------------------------------- * 
    ------------------------------------------------------------------------- *  

  
    \file palTimer.h
  
    \brief Define data structure and ptototype for PAL timer.
  
    $Id$ 
  
  
    Copyright (C) 2006 Airgo Networks, Incorporated
    ... description...
  
   ========================================================================== */

#if !defined( PALTIMER_H__ )
#define PALTIMER_H__


/*
         
                                                           
                                                                                                                        
*/

typedef void * tPalTimerHandle;

#define PAL_INVALID_TIMER_HANDLE (NULL)

typedef void (*palTimerCallback)(void *);

#define PAL_TIMER_TO_MS_UNIT      1000
#define PAL_TIMER_TO_SEC_UNIT     1000000

#ifndef FEATURE_WLAN_PAL_TIMER_DISABLE
//                   
//                                                             
//                                                                                 
//                                                                   
#ifdef TIMER_MANAGER
#define palTimerAlloc(hHdd, phPalTimer, pCallback, pContext) \
              palTimerAlloc_debug(hHdd, phPalTimer, pCallback, pContext, __FILE__, __LINE__)
eHalStatus palTimerAlloc_debug( tHddHandle hHdd, tPalTimerHandle *phPalTimer, 
                          palTimerCallback pCallback, void *pContext, char* fileName, v_U32_t lineNum  );              
#else
eHalStatus palTimerAlloc(tHddHandle hHdd, tPalTimerHandle *phPalTimer, palTimerCallback pCallback, void *pContext);
#endif
//                                 
//                                                                    
eHalStatus palTimerFree(tHddHandle, tPalTimerHandle);
//                
//                                                                                                             
//                                                                           
eHalStatus palTimerStart(tHddHandle, tPalTimerHandle, tANI_U32 uExpireTime, tANI_BOOLEAN fRestart);
//                                                                                                 
//                                                                                               
eHalStatus palTimerStop(tHddHandle, tPalTimerHandle); 
#endif

#endif

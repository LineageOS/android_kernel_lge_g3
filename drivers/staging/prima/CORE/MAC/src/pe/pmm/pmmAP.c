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

/*
  
                                                        
                                              
  
                            
                        
            
                                                          
                                                                       
  
 */

#include "sirCommon.h"

#include "aniGlobal.h"

#include "schApi.h"
#include "limApi.h"
#include "cfgApi.h"
#include "wniCfgSta.h"

#include "pmmApi.h"
#include "pmmDebug.h"

/* 
                 
  
            
               
  
         
  
               
  
        
  
                                              
                                                                           
                                                                      
               
 */
void pmmGenerateTIM(tpAniSirGlobal pMac, tANI_U8 **pPtr, tANI_U16 *timLength, tANI_U8 dtimPeriod)
{
    tANI_U8 *ptr = *pPtr;
    tANI_U32 val = 0;
    tANI_U32 minAid = 1; //                                   
    tANI_U32 maxAid = HAL_NUM_STA;


    //                                
    tANI_U8 N1 = minAid / 8;
    tANI_U8 N2 = maxAid / 8;
    if (N1 & 1) N1--;

    *timLength = N2 - N1 + 4;
    val = dtimPeriod;

    /* 
                                       
                                                                              
                                                                                        
                                                                                          
                                                                                        
     */
    *ptr++ = SIR_MAC_TIM_EID;
    *ptr++ = (tANI_U8)(*timLength);
    *ptr++ = 0xFF; //                                                 
    *ptr++ = (tANI_U8)val;

    *ptr++ = 0xFF; //                                                      
    ptr += (N2 - N1 + 1);

    PELOG2(sirDumpBuf(pMac, SIR_PMM_MODULE_ID, LOG2, *pPtr, (*timLength)+2);)
    *pPtr = ptr;
}

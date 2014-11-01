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
 
#include "palTypes.h"
#include "wniCfgSta.h"
#include "aniGlobal.h"
#include "cfgApi.h"
#include "sirParams.h"
#include "limUtils.h"
#include "limTimerUtils.h"
#include "limSession.h"
#include "limSessionUtils.h"

#define LIM_START_PEER_IDX   1

/* 
                       
  
           
                                                     
                                                        
                                                       
           
  
        
  
              
     
  
       
     
  
                                                 
               
 */

void
limInitPeerIdxpool(tpAniSirGlobal pMac,tpPESession pSessionEntry)
{
    tANI_U8 i;
    tANI_U8 maxAssocSta = pMac->lim.gLimAssocStaLimit;

    pSessionEntry->gpLimPeerIdxpool[0]=0;

#ifdef FEATURE_WLAN_TDLS
    //                                                                       
    //                                                                          
    //                                                                        
    if (pSessionEntry->limSystemRole == eLIM_STA_ROLE )
    {
        pSessionEntry->freePeerIdxHead = DPH_STA_HASH_INDEX_PEER + 1;
    }
    else
#endif
    {
        pSessionEntry->freePeerIdxHead=LIM_START_PEER_IDX;
    }

    for (i=pSessionEntry->freePeerIdxHead;i<maxAssocSta; i++)
    {
        pSessionEntry->gpLimPeerIdxpool[i]         = i+1;
    }
    pSessionEntry->gpLimPeerIdxpool[i]         =  0;

    pSessionEntry->freePeerIdxTail=i;

}


/* 
                     
  
           
                                                                     
                                        
                                                          
                                                                           
  
        
  
              
     
  
       
  
                                                 
                                                       
 */

tANI_U16
limAssignPeerIdx(tpAniSirGlobal pMac, tpPESession pSessionEntry)
{
    tANI_U16 peerId;

    //                                                                     
    //                                                                          
    if (peGetCurrentSTAsCount(pMac) >= pMac->lim.gLimAssocStaLimit)
    {
        //                                     
        return 0;
    }

    /*                          */

    if (pSessionEntry->freePeerIdxHead)
    {
        peerId=pSessionEntry->freePeerIdxHead;
        pSessionEntry->freePeerIdxHead = pSessionEntry->gpLimPeerIdxpool[pSessionEntry->freePeerIdxHead];
        if (pSessionEntry->freePeerIdxHead==0)
            pSessionEntry->freePeerIdxTail=0;
        pSessionEntry->gLimNumOfCurrentSTAs++;
        //                                                                                                                                                                          
        return peerId;
    }

    return 0; /*                         */
}


/* 
                      
  
           
                                                        
                                                                
                
  
        
  
              
     
  
       
  
                                                 
                                                                        
  
               
 */

void
limReleasePeerIdx(tpAniSirGlobal pMac, tANI_U16 peerIdx, tpPESession pSessionEntry)
{
    pSessionEntry->gLimNumOfCurrentSTAs--;

    /*                             */
    if (pSessionEntry->freePeerIdxTail)
    {
        pSessionEntry->gpLimPeerIdxpool[pSessionEntry->freePeerIdxTail]=(tANI_U8)peerIdx;
        pSessionEntry->freePeerIdxTail=(tANI_U8)peerIdx;
    }
    else
    {
        pSessionEntry->freePeerIdxTail=pSessionEntry->freePeerIdxHead=(tANI_U8)peerIdx;
    }
    pSessionEntry->gpLimPeerIdxpool[(tANI_U8)peerIdx]=0;
    //                                                                                                                                                            

}

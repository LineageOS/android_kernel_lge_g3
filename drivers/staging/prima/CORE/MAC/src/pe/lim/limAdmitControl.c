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
#include "limDebug.h"
#include "sysDef.h"
#include "limApi.h"
#include "cfgApi.h" //                 
#include "limTrace.h"
#include "limSendSmeRspMessages.h"
#include "limTypes.h"


#define ADMIT_CONTROL_LOGLEVEL        LOG1
#define ADMIT_CONTROL_POLICY_LOGLEVEL LOG1
#define ADMIT_CONTROL_MIN_INTERVAL    1000 //                                      

/*                                                  
                                                              
 */
#define LIM_TOTAL_BW_11A   54000000
#define LIM_MIN_BW_11A     6000000
#define LIM_TOTAL_BW_11B   11000000
#define LIM_MIN_BW_11B     1000000
#define LIM_TOTAL_BW_11G   LIM_TOTAL_BW_11A
#define LIM_MIN_BW_11G     LIM_MIN_BW_11B

//                   
#define LIM_CONVERT_SIZE_BITS(numBytes) ((numBytes) * 8)
#define LIM_CONVERT_RATE_MBPS(rate)     ((rate)/1000000)

/*                                                                       
                                                                           
                                                                        
                                                                        
 */
#define LIM_STA_BW_ADJUST(aniPeer, phyMode, bw) \
            (((aniPeer) && ((phyMode) != WNI_CFG_PHY_MODE_11B)) \
              ?   ((bw)/2) : (bw))


//                                                                              
//             

static tSirRetStatus
limCalculateSvcInt(tpAniSirGlobal, tSirMacTspecIE *, tANI_U32 *);
#if 0 //                          
static tSirRetStatus
limValidateTspecHcca(tpAniSirGlobal, tSirMacTspecIE *);
#endif
static tSirRetStatus
limValidateTspecEdca(tpAniSirGlobal, tSirMacTspecIE *, tpPESession);
static tSirRetStatus
limValidateTspec(tpAniSirGlobal, tSirMacTspecIE *, tpPESession);
static void
limComputeMeanBwUsed(tpAniSirGlobal, tANI_U32 *, tANI_U32, tpLimTspecInfo, tpPESession);
static void
limGetAvailableBw(tpAniSirGlobal, tANI_U32 *, tANI_U32 *, tANI_U32, tANI_U32);
static tSirRetStatus
limAdmitPolicyOversubscription(tpAniSirGlobal, tSirMacTspecIE *, tpLimAdmitPolicyInfo, tpLimTspecInfo, tpPESession);
static tSirRetStatus
limTspecFindByStaAddr(tpAniSirGlobal, tANI_U8 *, tSirMacTspecIE*, tpLimTspecInfo, tpLimTspecInfo *);
static tSirRetStatus
limValidateAccessPolicy(tpAniSirGlobal, tANI_U8, tANI_U16, tpPESession);


/*                                                               
                      
                                                          
                                 
                                     
                                           
                                                
                                                               */

static tSirRetStatus
limCalculateSvcInt(
    tpAniSirGlobal  pMac,
    tSirMacTspecIE *pTspec,
    tANI_U32            *pSvcInt)
{
    tANI_U32 msduSz, dataRate;
    *pSvcInt = 0;

    //                                                        
    if ((pTspec->minSvcInterval != 0) || (pTspec->maxSvcInterval != 0))
    {
        *pSvcInt = (pTspec->maxSvcInterval != 0)
                    ? pTspec->maxSvcInterval : pTspec->minSvcInterval;
        return eSIR_SUCCESS;
    }
    
    /*                                                                
                                                                     
                                                    
     */
    if (pTspec->nomMsduSz != 0) 
        msduSz = (pTspec->nomMsduSz & 0x7fff);
    else if (pTspec->maxMsduSz != 0) 
        msduSz = pTspec->maxMsduSz;
    else
    {
        PELOGE(limLog(pMac, LOGE, FL("MsduSize not specified"));)
        return eSIR_FAILURE;
    }

    /*                                                
                                             
     */
    if      (pTspec->meanDataRate != 0) dataRate = pTspec->meanDataRate;
    else if (pTspec->peakDataRate != 0) dataRate = pTspec->peakDataRate;
    else if (pTspec->minDataRate  != 0) dataRate = pTspec->minDataRate;
    else
    {
        PELOGE(limLog(pMac, LOGE, FL("DataRate not specified"));)
        return eSIR_FAILURE;
    }

    *pSvcInt = LIM_CONVERT_SIZE_BITS(msduSz) / LIM_CONVERT_RATE_MBPS(dataRate);
    return eSIR_FAILURE;
}

#if 0 //                          
/*                                                               
                        
                                                 
                                                                  
                            
                                   
                              
                                                               */
static tSirRetStatus
limValidateTspecHcca(
    tpAniSirGlobal  pMac,
    tSirMacTspecIE *pTspec)
{
    tANI_U32 maxPhyRate, minPhyRate;
    tANI_U32 phyMode;

    tSirRetStatus retval = eSIR_SUCCESS;
    /*                                     */
    if (pTspec->tsinfo.traffic.tsid < SIR_MAC_HCCA_TSID_MIN)
    {
        limLog(pMac, LOGW, FL("tsid %d must be >%d)"),
               pTspec->tsinfo.traffic.tsid, SIR_MAC_HCCA_TSID_MIN);
        retval =  eSIR_FAILURE;
    }
    /*
                                                                              
                                                                           
                                                                             
                                 
     */
    if (pTspec->tsinfo.traffic.userPrio !=
        (pTspec->tsinfo.traffic.tsid - SIR_MAC_HCCA_TSID_MIN))
    {
        limLog(pMac, LOGE, FL("TSid=0x%x, userPrio=%d: is not allowed"),
               pTspec->tsinfo.traffic.tsid, pTspec->tsinfo.traffic.userPrio);
        retval = eSIR_FAILURE;
    }
    //                                    
    if (pTspec->inactInterval == 0)
    {
        PELOGW(limLog(pMac, LOGW, FL("inactInterval unspecified!"));)
        retval =  eSIR_FAILURE;
    }
    //                                                           
    if ((pTspec->delayBound != 0) && (pTspec->surplusBw == 0))
    {
        limLog(pMac, LOGW, FL("delayBound %d, but surplusBw unspecified!"),
               pTspec->delayBound);
        retval =  eSIR_FAILURE;
    }
    //                                                                        
    limGetPhyMode(pMac, &phyMode);
    //                                                                        
    //                                                 
    limGetAvailableBw(pMac, &maxPhyRate, &minPhyRate, phyMode,
                      1 /*                       */);
    if ((pTspec->minPhyRate == 0)
        || (pTspec->minPhyRate > maxPhyRate)
        || (pTspec->minPhyRate < minPhyRate))
    {
        limLog(pMac, LOGW, FL("minPhyRate (%d) invalid"),
               pTspec->minPhyRate);
        retval =  eSIR_FAILURE;
    }
    /*                                                                      
                                        
     */
    if ((pTspec->minDataRate  == 0) ||
        (pTspec->meanDataRate == 0) ||
        (pTspec->peakDataRate == 0))
    {
        limLog(pMac, LOGW, FL("DataRate must be specified (min %d, mean %d, peak %d)"),
               pTspec->minDataRate, pTspec->meanDataRate, pTspec->peakDataRate);
        retval =  eSIR_FAILURE;
    }

    //                                                   
    if (pTspec->meanDataRate > pTspec->minPhyRate)
    {
        limLog(pMac, LOGW, FL("Data rate (%d) is more than Phyrate %d"),
               pTspec->meanDataRate, pTspec->minPhyRate);
        return eSIR_FAILURE;
    }

    /*                                                                   
                                                                            
                                                                             
                                                           
     */
    if ((pTspec->minSvcInterval != 0) || (pTspec->maxSvcInterval != 0))
    {
        //                        
        if (pTspec->maxSvcInterval < pTspec->minSvcInterval)
        {
            limLog(pMac, LOGW, FL("maxSvcInt %d  > minSvcInterval %d!!"),
                   pTspec->maxSvcInterval, pTspec->minSvcInterval);
            retval =  eSIR_FAILURE;
        }
        if (pTspec->maxSvcInterval < ADMIT_CONTROL_MIN_INTERVAL)
        {
            limLog(pMac, LOGW, FL("maxSvcInt %d must be >%d"),
                   pTspec->maxSvcInterval, ADMIT_CONTROL_MIN_INTERVAL);
            retval =  eSIR_FAILURE;
        }
    }
    else //                             
    {
        /*                                                              
                                                             
                                                                            
                                                                         
         */
         if (pTspec->nomMsduSz == 0)
         {
             PELOGW(limLog(pMac, LOGW, FL("No svcInt and no MsduSize specified"));)
             retval = eSIR_FAILURE;
         }
    }

    limLog(pMac, ADMIT_CONTROL_LOGLEVEL, FL("return status %d"), retval);
    return retval;
}

#endif //                           

/*                                                               
                        
                                                
                                                                  
                            
                                    
                              
                                                               */
static tSirRetStatus
limValidateTspecEdca(
    tpAniSirGlobal  pMac,
    tSirMacTspecIE *pTspec,
    tpPESession  psessionEntry)
{
    tANI_U32           maxPhyRate, minPhyRate;
    tANI_U32 phyMode;
    tSirRetStatus retval = eSIR_SUCCESS;

    limGetPhyMode(pMac, &phyMode, psessionEntry);

    //                                                                        
    //                                                 
    limGetAvailableBw(pMac, &maxPhyRate, &minPhyRate, phyMode,
                      1 /*                       */);
    //                                                          
    if ((pTspec->nomMsduSz    == 0) ||
        (pTspec->meanDataRate == 0) ||
        (pTspec->surplusBw    == 0) ||
        (pTspec->minPhyRate   == 0) ||
        (pTspec->minPhyRate   > maxPhyRate))
    {
        limLog(pMac, LOGW, FL("Invalid EDCA Tspec: NomMsdu %d, meanDataRate %d, surplusBw %d, minPhyRate %d"),
               pTspec->nomMsduSz, pTspec->meanDataRate, pTspec->surplusBw, pTspec->minPhyRate);
        retval = eSIR_FAILURE;
    }

    limLog(pMac, ADMIT_CONTROL_LOGLEVEL, FL("return status %d"), retval);
    return retval;
}

/*                                                               
                    
                                 
                            
                                     
                              
                                                               */

static tSirRetStatus
limValidateTspec(
    tpAniSirGlobal  pMac,
    tSirMacTspecIE *pTspec,
     tpPESession psessionEntry)
{
    tSirRetStatus retval = eSIR_SUCCESS;
    switch (pTspec->tsinfo.traffic.accessPolicy)
    {
        case SIR_MAC_ACCESSPOLICY_EDCA:
            if ((retval = limValidateTspecEdca(pMac, pTspec, psessionEntry)) != eSIR_SUCCESS)
                PELOGW(limLog(pMac, LOGW, FL("EDCA tspec invalid"));)
            break;

        case SIR_MAC_ACCESSPOLICY_HCCA:
#if 0 //                            
            if ((retval = limValidateTspecHcca(pMac, pTspec)) != eSIR_SUCCESS)
                PELOGW(limLog(pMac, LOGW, FL("HCCA tspec invalid"));)
            break;
#endif
       case SIR_MAC_ACCESSPOLICY_BOTH:
         //                                                                         
        default:
            limLog(pMac, LOGW, FL("AccessType %d not supported"),
                   pTspec->tsinfo.traffic.accessPolicy);
            retval = eSIR_FAILURE;
            break;
    }
    return retval;
}

//                                                                             
//                     


/*                                                               
                        
                                             
                            
                                       
                                           
                                         
                              
                                                               */

static void
limComputeMeanBwUsed(
    tpAniSirGlobal    pMac,
    tANI_U32              *pBw,
    tANI_U32               phyMode,
    tpLimTspecInfo    pTspecInfo,
    tpPESession psessionEntry)
{
    tANI_U32 ctspec;
    *pBw = 0;
    for (ctspec = 0; ctspec < LIM_NUM_TSPEC_MAX; ctspec++, pTspecInfo++)
    {
        if (pTspecInfo->inuse)
        {
            tpDphHashNode pSta = dphGetHashEntry(pMac, pTspecInfo->assocId, &psessionEntry->dph.dphHashTable);
            if (pSta == NULL)
            {
                //                                   
                limLog(pMac, LOGE, FL("Tspec %d (assocId %d): dphNode not found"),
                       ctspec, pTspecInfo->assocId);
                continue;
            }
            //                                                               
            *pBw += LIM_STA_BW_ADJUST(pSta->aniPeer, phyMode, pTspecInfo->tspec.meanDataRate);
        }
    }
}

/*                                                               
                     
                                                                                  
                       
                            
                                          
                                          
                                           
                                             
                              
                                                               */

static void
limGetAvailableBw(
    tpAniSirGlobal    pMac,
    tANI_U32              *pMaxBw,
    tANI_U32              *pMinBw,
    tANI_U32               phyMode,
    tANI_U32               bw_factor)
{
    switch (phyMode)
    {
        case WNI_CFG_PHY_MODE_11B:
            *pMaxBw = LIM_TOTAL_BW_11B;
            *pMinBw = LIM_MIN_BW_11B;
            break;

        case WNI_CFG_PHY_MODE_11A:
            *pMaxBw = LIM_TOTAL_BW_11A;
            *pMinBw = LIM_MIN_BW_11A;
            break;

        case WNI_CFG_PHY_MODE_11G:
        case WNI_CFG_PHY_MODE_NONE:
        default:
            *pMaxBw = LIM_TOTAL_BW_11G;
            *pMinBw = LIM_MIN_BW_11G;
            break;
    }
    *pMaxBw *= bw_factor;
}

/*                                                               
                                  
                                                                
                                                                                      
                                                                                    
                                                                                  
                            
                                         
                                               
                                             
                              
                                                               */

/*
                                                            
                                                                                
                                                                              
                                                                            
 */
static tSirRetStatus
limAdmitPolicyOversubscription(
    tpAniSirGlobal        pMac,
    tSirMacTspecIE       *pTspec,
    tpLimAdmitPolicyInfo  pAdmitPolicy,
    tpLimTspecInfo        pTspecInfo,
    tpPESession psessionEntry)
{
    tANI_U32 totalbw, minbw, usedbw;
    tANI_U32 phyMode;

    //                                      
    limGetPhyMode(pMac, &phyMode, psessionEntry);

    //                                                                       
    limComputeMeanBwUsed(pMac, &usedbw, phyMode, pTspecInfo, psessionEntry);

    //                                                                        
    //                                                                                          
    limGetAvailableBw(pMac, &totalbw, &minbw, phyMode, pAdmitPolicy->bw_factor);

    if (usedbw > totalbw) //                           
        return eSIR_FAILURE;

    if ((totalbw - usedbw) < pTspec->meanDataRate)
    {
        limLog(pMac, ADMIT_CONTROL_POLICY_LOGLEVEL,
               FL("Total BW %d, Used %d, Tspec request %d not possible"),
               totalbw, usedbw, pTspec->meanDataRate);
        return eSIR_FAILURE;
    }
    return eSIR_SUCCESS;
}

/*                                                               
                  
                                                                                    
                            
                                       
                              
                                                               */

tSirRetStatus limAdmitPolicy(
    tpAniSirGlobal    pMac,
    tSirMacTspecIE   *pTspec,
    tpPESession psessionEntry)
{
    tSirRetStatus retval = eSIR_FAILURE;
    tpLimAdmitPolicyInfo pAdmitPolicy = &pMac->lim.admitPolicyInfo;

    switch (pAdmitPolicy->type)
    {
        case WNI_CFG_ADMIT_POLICY_ADMIT_ALL:
            retval = eSIR_SUCCESS;
            break;

        case WNI_CFG_ADMIT_POLICY_BW_FACTOR:
            retval = limAdmitPolicyOversubscription(pMac, pTspec,
                        &pMac->lim.admitPolicyInfo, &pMac->lim.tspecInfo[0], psessionEntry);
            if (retval != eSIR_SUCCESS)
                PELOGE(limLog(pMac, LOGE, FL("rejected by BWFactor policy"));)
            break;

        case WNI_CFG_ADMIT_POLICY_REJECT_ALL:
            retval = eSIR_FAILURE;
            break;

        default:
            retval = eSIR_SUCCESS;
            limLog(pMac, LOGE, FL("Admit Policy %d unknown, admitting all traffic"),
                   pAdmitPolicy->type);
            break;
    }
    return retval;
}

/*                                                               
                  
                                 
                            
                               
                              
                                                               */

//                                                                             
//                           
void limTspecDelete(tpAniSirGlobal pMac, tpLimTspecInfo pInfo)
{
    if (pInfo == NULL)
        return;
        //      
    limLog(pMac, ADMIT_CONTROL_LOGLEVEL, FL("tspec entry = %d"), pInfo->idx);
    limLog(pMac, ADMIT_CONTROL_LOGLEVEL, FL("delete tspec %08X"),pInfo);
    pInfo->inuse = 0;

    //                                             
#if 0
    if ((pInfo->tspec.tsinfo.traffic.direction == SIR_MAC_DIRECTION_UPLINK) ||
        (pInfo->tspec.tsinfo.traffic.direction == SIR_MAC_DIRECTION_BIDIR))
        queue[pInfo->staid][pInfo->tspec.tsinfo.traffic.userPrio][SCH_UL_QUEUE].ts = 0;
#endif

    return;
}

/*                                                               
                         
                               
                            
                                                  
                                        
                                         
                                     
                              
                                                               */

//                                     
static tSirRetStatus
limTspecFindByStaAddr(
    tpAniSirGlobal    pMac,
    tANI_U8               *pAddr,
    tSirMacTspecIE    *pTspecIE,
    tpLimTspecInfo    pTspecList,
    tpLimTspecInfo   *ppInfo)
{
    int ctspec;

    *ppInfo = NULL;

    for (ctspec = 0; ctspec < LIM_NUM_TSPEC_MAX; ctspec++, pTspecList++)
    {
        if ((pTspecList->inuse)
            && (vos_mem_compare(pAddr, pTspecList->staAddr, sizeof(pTspecList->staAddr)))
            && (vos_mem_compare((tANI_U8 *) pTspecIE, (tANI_U8 *) &pTspecList->tspec,
                                            sizeof(tSirMacTspecIE))))
        {
            *ppInfo = pTspecList;
            return eSIR_SUCCESS;
        }
    }
    return eSIR_FAILURE;
}

/*                                                               
                         
                                               
                            
                                         
                                        
                                         
                                     
                              
                                                               */

tSirRetStatus
limTspecFindByAssocId(
    tpAniSirGlobal    pMac,
    tANI_U16               assocId,
    tSirMacTspecIE *pTspecIE,
    tpLimTspecInfo    pTspecList,
    tpLimTspecInfo   *ppInfo)
{
    int ctspec;

    *ppInfo = NULL;

    limLog(pMac, ADMIT_CONTROL_LOGLEVEL, FL("Trying to find tspec entry for assocId = %d"), assocId);
    limLog(pMac, ADMIT_CONTROL_LOGLEVEL, FL("pTsInfo->traffic.direction = %d, pTsInfo->traffic.tsid = %d"),
                pTspecIE->tsinfo.traffic.direction, pTspecIE->tsinfo.traffic.tsid);

    for (ctspec = 0; ctspec < LIM_NUM_TSPEC_MAX; ctspec++, pTspecList++)
    {
        if ((pTspecList->inuse)
            && (assocId == pTspecList->assocId)
            && (vos_mem_compare((tANI_U8 *)pTspecIE, (tANI_U8 *)&pTspecList->tspec,
                sizeof(tSirMacTspecIE))))
        {
            *ppInfo = pTspecList;
            return eSIR_SUCCESS;
        }
    }
    return eSIR_FAILURE;
}

/*                                                               
                
                                                                           
                                        
                                 
                                   
                                           
                                       
                                                
                                                               */

tSirRetStatus
limFindTspec(
    tpAniSirGlobal    pMac,
    tANI_U16               assocId,    
    tSirMacTSInfo   *pTsInfo,
    tpLimTspecInfo    pTspecList,
    tpLimTspecInfo   *ppInfo)
{
    int ctspec;

    *ppInfo = NULL;

    limLog(pMac, ADMIT_CONTROL_LOGLEVEL, FL("Trying to find tspec entry for assocId = %d"), assocId);
    limLog(pMac, ADMIT_CONTROL_LOGLEVEL, FL("pTsInfo->traffic.direction = %d, pTsInfo->traffic.tsid = %d"),
                pTsInfo->traffic.direction, pTsInfo->traffic.tsid);

    for (ctspec = 0; ctspec < LIM_NUM_TSPEC_MAX; ctspec++, pTspecList++)
    {
        if ((pTspecList->inuse)
            && (assocId == pTspecList->assocId)
            && (pTsInfo->traffic.direction == pTspecList->tspec.tsinfo.traffic.direction)
            && (pTsInfo->traffic.tsid == pTspecList->tspec.tsinfo.traffic.tsid))
        {
            *ppInfo = pTspecList;
            return eSIR_SUCCESS;
        }
    }
    return eSIR_FAILURE;
}

/*                                                               
               
                                                          
                             
                                   
                                     
                               
                                      
                               

                                                
                                                               */

tSirRetStatus limTspecAdd(
    tpAniSirGlobal    pMac,
    tANI_U8           *pAddr,
    tANI_U16          assocId,
    tSirMacTspecIE    *pTspec,
    tANI_U32          interval,
    tpLimTspecInfo    *ppInfo)
{
    tpLimTspecInfo pTspecList = &pMac->lim.tspecInfo[0];
    *ppInfo = NULL;    

    //                     
    if (assocId >= pMac->lim.maxStation)
    {
        PELOGE(limLog(pMac, LOGE, FL("Invalid assocId 0x%x"), assocId);)
        return eSIR_FAILURE;
    }

    //                            
    {
      *ppInfo = NULL;

      if(eSIR_SUCCESS == limFindTspec(pMac, assocId, &pTspec->tsinfo, pTspecList, ppInfo))
      {
            //                  
            limLog(pMac, ADMIT_CONTROL_LOGLEVEL, FL("updating TSPEC table entry = %d"),
                        (*ppInfo)->idx);
      }
      else
      {
          /*                                                          
                                                  
           */ 
          tANI_U8 ctspec = 0;
          for (ctspec = 0 , pTspecList = &pMac->lim.tspecInfo[0]; ctspec < LIM_NUM_TSPEC_MAX; ctspec++, pTspecList++)
          {
              if (! pTspecList->inuse)
              {
                  limLog(pMac, LOG1, FL("Found free slot in TSPEC list. Add to TSPEC table entry %d"), ctspec);
                  break;
              }
          }

          if (ctspec >= LIM_NUM_TSPEC_MAX)
              return eSIR_FAILURE;

          //                           
          pTspecList->idx = ctspec;
      }
    }

    //                      
    pTspecList->tspec = *pTspec;
    pTspecList->assocId = assocId;
    vos_mem_copy(pTspecList->staAddr, pAddr, sizeof(pTspecList->staAddr));

    //                                  
    if (pTspec->tsinfo.traffic.accessPolicy == SIR_MAC_ACCESSPOLICY_EDCA)
    {
        pTspecList->inuse = 1;
        *ppInfo = pTspecList;
        limLog(pMac, ADMIT_CONTROL_LOGLEVEL, FL("added entry for EDCA AccessPolicy"));
        return eSIR_SUCCESS;
    }

    /*
                                                                     
                                                                           
                                                                          
                                                                          
                                                                           
                                                                               
             
     */
#if 0
    if ((pTspec->tsinfo.traffic.direction == SIR_MAC_DIRECTION_UPLINK) ||
        (pTspec->tsinfo.traffic.direction == SIR_MAC_DIRECTION_BIDIR))
        queue[staid][pTspec->tsinfo.traffic.userPrio][SCH_UL_QUEUE].ts = 1;
#endif
    pTspecList->inuse = 1;
    *ppInfo = pTspecList;
    limLog(pMac, ADMIT_CONTROL_LOGLEVEL, FL("added entry for HCCA AccessPolicy"));
    return eSIR_SUCCESS;
}

/*                                                               
                           
                              
                            
                                              
                                         
                              
                                                               */

static tSirRetStatus
limValidateAccessPolicy(
    tpAniSirGlobal  pMac,
    tANI_U8              accessPolicy,
    tANI_U16              assocId,
    tpPESession psessionEntry)
{
    tSirRetStatus retval = eSIR_FAILURE;
    tpDphHashNode pSta = dphGetHashEntry(pMac, assocId, &psessionEntry->dph.dphHashTable);

    if ((pSta == NULL) || (! pSta->valid))
    {
        PELOGE(limLog(pMac, LOGE, FL("invalid station address passed"));)
        return eSIR_FAILURE;
    }

    switch (accessPolicy)
    {
        case SIR_MAC_ACCESSPOLICY_EDCA:
            if (pSta->wmeEnabled || pSta->lleEnabled)
                retval = eSIR_SUCCESS;
            break;

        case SIR_MAC_ACCESSPOLICY_HCCA:
        case SIR_MAC_ACCESSPOLICY_BOTH:
#if 0 //                                      
            //                                                        
            if (pSta->wsmEnabled || pSta->lleEnabled)
                retval = eSIR_SUCCESS;
            break;
#endif  //                            
        default:
            PELOGE(limLog(pMac, LOGE, FL("Invalid accessPolicy %d"), accessPolicy);)
            break;
    }

    if (retval != eSIR_SUCCESS)
        limLog(pMac, LOGW, FL("failed (accPol %d, staId %d, lle %d, wme %d, wsm %d)"),
               accessPolicy, pSta->staIndex, pSta->lleEnabled, pSta->wmeEnabled, pSta->wsmEnabled);

    return retval;
}

/*                                                               
                        
                                                                            
                                   
                            
                                                
                                            
                                          
                                                                                        
                                                                                   
                                                                               
                                          
                                                                                 
                              
                                                               */

tSirRetStatus limAdmitControlAddTS(
    tpAniSirGlobal          pMac,
    tANI_U8                     *pAddr,
    tSirAddtsReqInfo       *pAddts,
    tSirMacQosCapabilityStaIE *pQos,
    tANI_U16                     assocId, //                                   
    tANI_U8                    alloc, //                                  
                                   //                                          
    tSirMacScheduleIE      *pSch,
    tANI_U8                   *pTspecIdx, //                             
    tpPESession psessionEntry
    )
{
    tpLimTspecInfo pTspecInfo;
    tSirRetStatus  retval;
    tANI_U32            svcInterval;
    (void) pQos;

    //                            
    //                                                               
    //                                                      

    limLog(pMac, ADMIT_CONTROL_LOGLEVEL, FL("tsid %d, directn %d, start %d, intvl %d, accPolicy %d, up %d"),
           pAddts->tspec.tsinfo.traffic.tsid, pAddts->tspec.tsinfo.traffic.direction,
           pAddts->tspec.svcStartTime, pAddts->tspec.minSvcInterval,
           pAddts->tspec.tsinfo.traffic.accessPolicy, pAddts->tspec.tsinfo.traffic.userPrio);

    //                          
    retval = (alloc)
              ? limTspecFindByAssocId(pMac, assocId, &pAddts->tspec, &pMac->lim.tspecInfo[0], &pTspecInfo)
              : limTspecFindByStaAddr(pMac, pAddr, &pAddts->tspec, &pMac->lim.tspecInfo[0], &pTspecInfo);

    if (retval == eSIR_SUCCESS)
    {
        limLog(pMac, ADMIT_CONTROL_LOGLEVEL, FL("duplicate tspec (index %d)!"), pTspecInfo->idx);
        return eSIR_FAILURE;
    }

    //                                                      
    if (limValidateTspec(pMac, &pAddts->tspec, psessionEntry) != eSIR_SUCCESS)
    {
        PELOGW(limLog(pMac, LOGW, FL("tspec validation failed"));)
        return eSIR_FAILURE;
    }

    //                                           
    if (limCalculateSvcInt(pMac, &pAddts->tspec, &svcInterval) != eSIR_SUCCESS)
    {
        PELOGW(limLog(pMac, LOGW, FL("SvcInt calculate failed"));)
        return eSIR_FAILURE;
    }

    //                                                                      
    if (limAdmitPolicy(pMac, &pAddts->tspec, psessionEntry) != eSIR_SUCCESS)
    {
        PELOGW(limLog(pMac, LOGW, FL("tspec rejected by admit control policy"));)
        return eSIR_FAILURE;
    }

    //                                
    if (pSch != NULL)
    {
        vos_mem_set((tANI_U8 *) pSch, sizeof(*pSch), 0);
        pSch->svcStartTime   = pAddts->tspec.svcStartTime;
        pSch->svcInterval    = svcInterval;
        pSch->maxSvcDuration = (tANI_U16) pSch->svcInterval; //            
        pSch->specInterval   = 0x1000; //                   

        pSch->info.direction   = pAddts->tspec.tsinfo.traffic.direction;
        pSch->info.tsid        = pAddts->tspec.tsinfo.traffic.tsid;
        pSch->info.aggregation = 0; //                                        
    }

    //                                    
    if (! alloc)
        return eSIR_SUCCESS;

    //                                                                 
    if (limValidateAccessPolicy(pMac, (tANI_U8) pAddts->tspec.tsinfo.traffic.accessPolicy, assocId, psessionEntry) != eSIR_SUCCESS)
    {
        limLog(pMac, LOGW, FL("AccessPolicy %d is not valid in current mode"),
               pAddts->tspec.tsinfo.traffic.accessPolicy);
        return eSIR_FAILURE;
    }

    //                  
    if (limTspecAdd(pMac, pAddr, assocId, &pAddts->tspec, svcInterval, &pTspecInfo)
        != eSIR_SUCCESS)
    {
        PELOGE(limLog(pMac, LOGE, FL("no space in tspec list"));)
        return eSIR_FAILURE;
    }

    //                                           
    *pTspecIdx = pTspecInfo->idx;

    return eSIR_SUCCESS;
}

/*                                                               
                           
                                                       
                            
                                           
                                      
                                             
                                           
                              
                                                               */

tSirRetStatus
limAdmitControlDeleteTS(
    tpAniSirGlobal    pMac,
    tANI_U16               assocId,
    tSirMacTSInfo    *pTsInfo,
    tANI_U8               *pTsStatus,
    tANI_U8             *ptspecIdx)
{
    tpLimTspecInfo pTspecInfo = NULL;

    if (pTsStatus != NULL)
        *pTsStatus = 0;

    if (limFindTspec(pMac, assocId, pTsInfo, &pMac->lim.tspecInfo[0], &pTspecInfo) == eSIR_SUCCESS)
    {
        if(pTspecInfo != NULL)    
        {
          limLog(pMac, ADMIT_CONTROL_LOGLEVEL, FL("Tspec entry %d found"), pTspecInfo->idx);
        
          *ptspecIdx = pTspecInfo->idx;
          limTspecDelete(pMac, pTspecInfo);
          return eSIR_SUCCESS;
        }
    }
    return eSIR_FAILURE;
}

/*                                                               
                            
                                             
                            
                           
                              
                                                               */

tSirRetStatus
limAdmitControlDeleteSta(
    tpAniSirGlobal    pMac,
    tANI_U16 assocId)
{
    tpLimTspecInfo pTspecInfo = &pMac->lim.tspecInfo[0];
    int ctspec;

    for (ctspec = 0; ctspec < LIM_NUM_TSPEC_MAX; ctspec++, pTspecInfo++)
    {
        if (assocId == pTspecInfo->assocId)
        {
            limTspecDelete(pMac, pTspecInfo);
            limLog(pMac, ADMIT_CONTROL_LOGLEVEL, FL("Deleting TSPEC %d for assocId %d"),
                   ctspec, assocId);
        }
    }
    limLog(pMac, ADMIT_CONTROL_LOGLEVEL, FL("assocId %d done"), assocId);

    return eSIR_SUCCESS;
}

/*                                                               
                       
                       
                            
                              
                                                               */
tSirRetStatus limAdmitControlInit(tpAniSirGlobal pMac)
{
    vos_mem_set(pMac->lim.tspecInfo, LIM_NUM_TSPEC_MAX * sizeof(tLimTspecInfo), 0);
    return eSIR_SUCCESS;
}

/*                                                               
                        
                                                           
                            
                              
                                                               */

tSirRetStatus limUpdateAdmitPolicy(tpAniSirGlobal    pMac)
{
    tANI_U32 val;
    if (wlan_cfgGetInt(pMac, WNI_CFG_ADMIT_POLICY, &val) != eSIR_SUCCESS)
    {
        limLog(pMac, LOGP, FL("Unable to get CFG_ADMIT_POLICY"));
        return eSIR_FAILURE;
    }
    pMac->lim.admitPolicyInfo.type = (tANI_U8) val;
    if (wlan_cfgGetInt(pMac, WNI_CFG_ADMIT_BWFACTOR, &val) != eSIR_SUCCESS)
    {
        limLog(pMac, LOGP, FL("Unable to get CFG_ADMIT_BWFACTOR"));
        return eSIR_FAILURE;
    }
    pMac->lim.admitPolicyInfo.bw_factor = (tANI_U8) val;

    PELOG1(limLog(pMac, LOG1, FL("LIM: AdmitPolicy %d, bw_factor %d"),
          pMac->lim.admitPolicyInfo.type, pMac->lim.admitPolicyInfo.bw_factor);)

    return eSIR_SUCCESS;
}


/*                                                               
                      
                               
                            
                                 
                                   
                                   
                                       
                                        
                              
                                                               */

tSirRetStatus
limSendHalMsgAddTs(
  tpAniSirGlobal pMac,
  tANI_U16       staIdx,
  tANI_U8         tspecIdx,
  tSirMacTspecIE tspecIE,
  tANI_U8        sessionId)
{
    tSirMsgQ msg;
    tpAddTsParams pAddTsParam;

    pAddTsParam = vos_mem_malloc(sizeof(tAddTsParams));
    if (NULL == pAddTsParam)
    {
       PELOGW(limLog(pMac, LOGW, FL("AllocateMemory() failed"));)
       return eSIR_MEM_ALLOC_FAILED;          
    }

    vos_mem_set((tANI_U8 *)pAddTsParam, sizeof(tAddTsParams), 0);
    pAddTsParam->staIdx = staIdx;
    pAddTsParam->tspecIdx = tspecIdx;
    vos_mem_copy(&pAddTsParam->tspec, &tspecIE, sizeof(tSirMacTspecIE));
    pAddTsParam->sessionId = sessionId;
 
    msg.type = WDA_ADD_TS_REQ;
    msg.bodyptr = pAddTsParam;
    msg.bodyval = 0;

    /*                                                      
                               
     */
    SET_LIM_PROCESS_DEFD_MESGS(pMac, false);
    MTRACE(macTraceMsgTx(pMac, sessionId, msg.type));

    if(eSIR_SUCCESS != wdaPostCtrlMsg(pMac, &msg))
    {
       PELOGW(limLog(pMac, LOGW, FL("wdaPostCtrlMsg() failed"));)
       SET_LIM_PROCESS_DEFD_MESGS(pMac, true);
       vos_mem_free(pAddTsParam);
       return eSIR_FAILURE;
    }
  return eSIR_SUCCESS;
}

/*                                                               
                      
                               
                            
                                 
                                   
                                 
                              
                                                               */

tSirRetStatus
limSendHalMsgDelTs(
  tpAniSirGlobal pMac,
  tANI_U16       staIdx,
  tANI_U8         tspecIdx,
  tSirDeltsReqInfo delts,
  tANI_U8        sessionId,
  tANI_U8        *bssId)
{
  tSirMsgQ msg;
  tpDelTsParams pDelTsParam;

  pDelTsParam = vos_mem_malloc(sizeof(tDelTsParams));
  if (NULL == pDelTsParam)
  {
     limLog(pMac, LOGP, FL("AllocateMemory() failed"));
     return eSIR_MEM_ALLOC_FAILED;
  }

  msg.type = WDA_DEL_TS_REQ;
  msg.bodyptr = pDelTsParam;
  msg.bodyval = 0;
  vos_mem_set((tANI_U8 *)pDelTsParam, sizeof(tDelTsParams), 0);

  //                           
  pDelTsParam->staIdx = staIdx;
  pDelTsParam->tspecIdx = tspecIdx;
  vos_mem_copy(&pDelTsParam->bssId, bssId, sizeof(tSirMacAddr));

  PELOGW(limLog(pMac, LOGW, FL("calling wdaPostCtrlMsg()"));)
  MTRACE(macTraceMsgTx(pMac, sessionId, msg.type));

  if(eSIR_SUCCESS != wdaPostCtrlMsg(pMac, &msg))
  {
     PELOGW(limLog(pMac, LOGW, FL("wdaPostCtrlMsg() failed"));)
     vos_mem_free(pDelTsParam);
     return eSIR_FAILURE;
  }
  return eSIR_SUCCESS;  
}

/*                                                               
                             
                                                           
                                                                
                                                            
                                      
 
                            
                          
                                                             */
void limProcessHalAddTsRsp(tpAniSirGlobal pMac, tpSirMsgQ limMsg)
{
    tpAddTsParams  pAddTsRspMsg = NULL;
    tpDphHashNode  pSta = NULL;
    tANI_U16  assocId =0;
    tSirMacAddr  peerMacAddr;
    tANI_U8   rspReqd = 1;
    tpPESession  psessionEntry = NULL;


    /*                                                    
                                        
     */
    SET_LIM_PROCESS_DEFD_MESGS(pMac, true);

    if (NULL == limMsg->bodyptr)
    {
        limLog(pMac, LOGP, FL("Received WDA_ADD_TS_RSP with NULL "));
        goto end;
    }

    pAddTsRspMsg = (tpAddTsParams) (limMsg->bodyptr);

    //                                                                               
    //                                           
    psessionEntry = peFindSessionBySessionId(pMac, pAddTsRspMsg->sessionId);

    if(psessionEntry == NULL)
    {
        PELOGE(limLog(pMac, LOGE,FL("Session does Not exist with given sessionId :%d "), pAddTsRspMsg->sessionId);)
        limSendSmeAddtsRsp(pMac, rspReqd, eSIR_SME_ADDTS_RSP_FAILED, psessionEntry, pAddTsRspMsg->tspec, 
              pMac->lim.gLimAddtsReq.sessionId, pMac->lim.gLimAddtsReq.transactionId);
        goto end;
    }

    if(pAddTsRspMsg->status == eHAL_STATUS_SUCCESS)
    {
        PELOG1(limLog(pMac, LOG1, FL("Received successful ADDTS response from HAL "));)
        //                                                                      
        limSendSmeAddtsRsp(pMac, rspReqd, eSIR_SME_SUCCESS, psessionEntry, pAddTsRspMsg->tspec,
                psessionEntry->smeSessionId, psessionEntry->transactionId);
        goto end;
    }
    else
    {
        PELOG1(limLog(pMac, LOG1, FL("Received failure ADDTS response from HAL "));)

        //                                      
        //                                               
#if 0  
        cfgLen = sizeof(tSirMacAddr);
        if (wlan_cfgGetStr(pMac, WNI_CFG_BSSID, peerMacAddr, &cfgLen) != eSIR_SUCCESS)
        {
            limLog(pMac, LOGP, FL("Fail to retrieve BSSID "));
            goto end;
        }
#endif //                 
        sirCopyMacAddr(peerMacAddr,psessionEntry->bssId);

        //                                       
        limSendDeltsReqActionFrame(pMac, peerMacAddr, rspReqd, &pAddTsRspMsg->tspec.tsinfo, &pAddTsRspMsg->tspec,
                //                             
                psessionEntry);

        //             
        //                                                     
        pSta = dphLookupAssocId(pMac, pAddTsRspMsg->staIdx, &assocId, 
                &psessionEntry->dph.dphHashTable);    
        if (pSta != NULL)
            limAdmitControlDeleteTS(pMac, assocId, &pAddTsRspMsg->tspec.tsinfo, NULL, (tANI_U8 *)&pAddTsRspMsg->tspecIdx);

        //                   
        //                                                                              
        limSendSmeAddtsRsp(pMac, rspReqd, eSIR_SME_ADDTS_RSP_FAILED, psessionEntry, pAddTsRspMsg->tspec,
                psessionEntry->smeSessionId, psessionEntry->transactionId);
        goto end;
   }

end:
    if( pAddTsRspMsg != NULL )
        vos_mem_free(pAddTsRspMsg);
    return;
}


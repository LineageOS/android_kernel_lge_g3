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
#include "cfgPriv.h"
#include "cfgDebug.h"
#include "wlan_qct_wda.h"

//                                                                     
//                 
//                                                                      
static tCfgCtl   __gCfgEntry[CFG_PARAM_MAX_NUM]                ;
static tANI_U32  __gCfgIBufMin[CFG_STA_IBUF_MAX_SIZE]          ;
static tANI_U32  __gCfgIBufMax[CFG_STA_IBUF_MAX_SIZE]          ;
static tANI_U32  __gCfgIBuf[CFG_STA_IBUF_MAX_SIZE]             ;
static tANI_U8   __gCfgSBuf[CFG_STA_SBUF_MAX_SIZE]             ;
static tANI_U8   __gSBuffer[CFG_MAX_STR_LEN]                   ;
static tANI_U32  __gParamList[WNI_CFG_MAX_PARAM_NUM + 
                              WNI_CFG_GET_PER_STA_STAT_RSP_NUM];

static void Notify(tpAniSirGlobal, tANI_U16, tANI_U32);


//                                                                      
tANI_U32 cfgNeedRestart(tpAniSirGlobal pMac, tANI_U16 cfgId)
{
    if (!pMac->cfg.gCfgEntry)
    {
        PELOGE(cfgLog(pMac, LOGE, FL("gCfgEntry is NULL"));)
        return 0;
    }
    return !!(pMac->cfg.gCfgEntry[cfgId].control & CFG_CTL_RESTART) ;
}

//                                                                      
tANI_U32 cfgNeedReload(tpAniSirGlobal pMac, tANI_U16 cfgId)
{
    if (!pMac->cfg.gCfgEntry)
    {
        PELOGE(cfgLog(pMac, LOGE, FL("gCfgEntry is NULL"));)
        return 0;
    }
    return !!(pMac->cfg.gCfgEntry[cfgId].control & CFG_CTL_RELOAD) ;
}

//                                                                      
/* 
                 
  
            
                               
  
         
                                                                     
                                    
  
               
        
  
        
                                                             
  
              
                
 */

void
wlan_cfgInit(tpAniSirGlobal pMac)
{
    //                        
    pMac->cfg.gCfgStatus = CFG_INCOMPLETE;
  
     //                          
    PELOGW(cfgLog(pMac, LOGW, FL("Sending CFG_DNLD_REQ"));)
    cfgSendHostMsg(pMac, WNI_CFG_DNLD_REQ, WNI_CFG_DNLD_REQ_LEN,
                   WNI_CFG_DNLD_REQ_NUM, 0, 0, 0);

} /*                        */


//                                                                     
tSirRetStatus cfgInit(tpAniSirGlobal pMac)
{
   pMac->cfg.gCfgIBufMin  = __gCfgIBufMin;
   pMac->cfg.gCfgIBufMax  = __gCfgIBufMax;
   pMac->cfg.gCfgIBuf     = __gCfgIBuf;
   pMac->cfg.gCfgSBuf     = __gCfgSBuf;
   pMac->cfg.gSBuffer     = __gSBuffer;
   pMac->cfg.gCfgEntry    = __gCfgEntry;
   pMac->cfg.gParamList   = __gParamList;
        
   return (eSIR_SUCCESS);
}

//                                                                      
void cfgDeInit(tpAniSirGlobal pMac)
{
   pMac->cfg.gCfgIBufMin  = NULL;
   pMac->cfg.gCfgIBufMax  = NULL;
   pMac->cfg.gCfgIBuf     = NULL;
   pMac->cfg.gCfgSBuf     = NULL;
   pMac->cfg.gSBuffer     = NULL;
   pMac->cfg.gCfgEntry    = NULL;
   pMac->cfg.gParamList   = NULL;
}

//                                                                      
/* 
              
  
            
                                                          
  
         
  
               
                                                                       
                                                                       
                                                                    
             
  
                                                                        
                                                      
  
        
  
                                            
                                          
  
                                                                  
                                                            
 */

tSirRetStatus
cfgSetInt(tpAniSirGlobal pMac, tANI_U16 cfgId, tANI_U32 value)
{
    tANI_U32      index;
    tANI_U32      control, mask;
    tSirRetStatus  retVal;

    if (cfgId >= CFG_PARAM_MAX_NUM)
    {
        PELOGE(cfgLog(pMac, LOGE, FL("Invalid cfg id %d"), cfgId);)
        return eSIR_CFG_INVALID_ID;
    }
    if (!pMac->cfg.gCfgEntry)
    {
        PELOGE(cfgLog(pMac, LOGE, FL("gCfgEntry is NULL"));)
        return eSIR_CFG_INVALID_ID;
    }

    control  = pMac->cfg.gCfgEntry[cfgId].control;
    index    = control & CFG_BUF_INDX_MASK;
    retVal   = eSIR_SUCCESS;

    if (index >= CFG_STA_IBUF_MAX_SIZE)
    {
        PELOGE(cfgLog(pMac, LOGE, FL("cfg index out of bounds %d"), index);)
        retVal = eSIR_CFG_INVALID_ID;
        return retVal;
    }

    //                            
    if ((control & CFG_CTL_VALID) == 0)
    {
        PELOGE(cfgLog(pMac, LOGE, FL("Not valid cfg id %d"), cfgId);)
        retVal = eSIR_CFG_INVALID_ID;
    }
    else if ((pMac->cfg.gCfgIBufMin[index] > value) ||
             (pMac->cfg.gCfgIBufMax[index] < value))
    {
        PELOGE(cfgLog(pMac, LOGE, FL("Value %d out of range [%d,%d] cfg id %d"),
               value, pMac->cfg.gCfgIBufMin[index],
               pMac->cfg.gCfgIBufMax[index], cfgId);)
        retVal = eSIR_CFG_INVALID_ID;
    }
    else
    {
            //                    
            pMac->cfg.gCfgIBuf[index] = value;

            //                             
            mask = control & CFG_CTL_NTF_MASK;
            if ((mask & CFG_CTL_NTF_HW) != 0)
                PELOGE(cfgLog(pMac, LOGE, FL("CFG Notify HW not supported!!!"));)

            //                                  
            if ((mask & CFG_CTL_NTF_MASK) != 0)
                Notify(pMac, cfgId, mask);

    }

    return (retVal);

} /*                   */

//                                                                      
/* 
                  
  
            
                                                           
  
         
  
               
  
        
  
                                         
  
                                                               
                                                         
 */

tSirRetStatus
cfgCheckValid(tpAniSirGlobal pMac, tANI_U16 cfgId)
{
    tANI_U32      control;

    if (cfgId >= CFG_PARAM_MAX_NUM)
    {
        PELOG3(cfgLog(pMac, LOG3, FL("Invalid cfg id %d"), cfgId);)
        return(eSIR_CFG_INVALID_ID);
    }
    if (!pMac->cfg.gCfgEntry)
    {
        PELOGE(cfgLog(pMac, LOGE, FL("gCfgEntry is NULL"));)
        return eSIR_CFG_INVALID_ID;
    }

    control = pMac->cfg.gCfgEntry[cfgId].control;

    //                            
    if ((control & CFG_CTL_VALID) == 0)
    {
        PELOG3(cfgLog(pMac, LOG3, FL("Not valid cfg id %d"), cfgId);)
        return(eSIR_CFG_INVALID_ID);
    }
    else
        return(eSIR_SUCCESS);

} /*                         */

//                                                                      
/* 
                   
  
            
                                                        
  
         
  
               
  
        
  
                                         
                                                               
  
                                                               
                                                         
 */

tSirRetStatus
wlan_cfgGetInt(tpAniSirGlobal pMac, tANI_U16 cfgId, tANI_U32 *pValue)
{
    tANI_U32      index;
    tANI_U32      control;
    tSirRetStatus  retVal;

    if (cfgId >= CFG_PARAM_MAX_NUM)
    {
        PELOGE(cfgLog(pMac, LOGE, FL("Invalid cfg id %d"), cfgId);)
        retVal = eSIR_CFG_INVALID_ID;
        return retVal;
    }
    if (!pMac->cfg.gCfgEntry)
    {
        PELOGE(cfgLog(pMac, LOGE, FL("gCfgEntry is NULL"));)
        return eSIR_CFG_INVALID_ID;
    }

    control = pMac->cfg.gCfgEntry[cfgId].control;
    index   = control & CFG_BUF_INDX_MASK;
    retVal  = eSIR_SUCCESS;

    if (index >= CFG_STA_IBUF_MAX_SIZE)
    {
        PELOGE(cfgLog(pMac, LOGE, FL("cfg index out of bounds %d"), index);)
        retVal = eSIR_CFG_INVALID_ID;
        return retVal;
    }

    //                            
    if ((control & CFG_CTL_VALID) == 0)
    {
        PELOGE(cfgLog(pMac, LOGE, FL("Not valid cfg id %d"), cfgId);)
        retVal = eSIR_CFG_INVALID_ID;
    }
    else {
        //                  
        if (index < CFG_STA_IBUF_MAX_SIZE)
            *pValue = pMac->cfg.gCfgIBuf[index];
    }

    return (retVal);

} /*                          */

#ifdef NOT_CURRENTLY_USED
//                                                                      
/* 
                    
  
            
                                                                  
  
         
  
               
                                         
                                                               
              
  
        
  
                                            
                                    
  
                                                               
                                                         
 */

tSirRetStatus
cfgIncrementInt(tpAniSirGlobal pMac, tANI_U16 cfgId, tANI_U32 value)
{
    tANI_U32      index;
    tANI_U32      control;
    tSirRetStatus  retVal;

    if (cfgId >= CFG_PARAM_MAX_NUM)
    {
        PELOGE(cfgLog(pMac, LOGE, FL("Invalid cfg id %d"), cfgId);)
        retVal = eSIR_CFG_INVALID_ID;
    }
    if (!pMac->cfg.gCfgEntry)
    {
        PELOGE(cfgLog(pMac, LOGE, FL("gCfgEntry is NULL"));)
        return eSIR_CFG_INVALID_ID;
    }

    control  = pMac->cfg.gCfgEntry[cfgId].control;
    index    = control & CFG_BUF_INDX_MASK;
    retVal   = eSIR_SUCCESS;

    //                            
    if ((control & CFG_CTL_VALID) == 0)
    {
        PELOGE(cfgLog(pMac, LOGE, FL("Not valid cfg id %d"), cfgId);)
        retVal = eSIR_CFG_INVALID_ID;
    }
    else
    {
            //                        
            pMac->cfg.gCfgIBuf[index] += value;

    }

    return (retVal);
}
#endif //                   

//                                                                      
/* 
              
  
            
                                                     
  
          
                                                                        
                                                                        
                                                                              
                                                                             
                
  
               
                                   
  
        
  
                                            
                                           
                                  
  
                                                               
                                                         
                                                         
  
 */

tSirRetStatus cfgSetStr(tpAniSirGlobal pMac, tANI_U16 cfgId, tANI_U8 *pStr, 
                                          tANI_U32 length)
{
   return cfgSetStrNotify( pMac, cfgId, pStr, length, TRUE );
}

//                                                                      
/* 
                    
  
            
                                                     
  
         
  
               
                                                                     
            
                                                               
              
  
        
  
                                            
                                           
                                  
                                             
  
                                                               
                                                         
                                                         
  
 */

tSirRetStatus
cfgSetStrNotify(tpAniSirGlobal pMac, tANI_U16 cfgId, tANI_U8 *pStr, 
                                          tANI_U32 length, int notifyMod)
{
    tANI_U8       *pDst, *pDstEnd;
    tANI_U32      index, paramLen, control, mask;
    tSirRetStatus  retVal;

    if (cfgId >= CFG_PARAM_MAX_NUM)
    {
        PELOGE(cfgLog(pMac, LOGE, FL("Invalid cfg id %d"), cfgId);)
        return eSIR_CFG_INVALID_ID;
    }
    if (!pMac->cfg.gCfgEntry)
    {
        PELOGE(cfgLog(pMac, LOGE, FL("gCfgEntry is NULL"));)
        return eSIR_CFG_INVALID_ID;
    }

    control  = pMac->cfg.gCfgEntry[cfgId].control;
    index    = control & CFG_BUF_INDX_MASK;
    retVal   = eSIR_SUCCESS;

    //                            
    if ((control & CFG_CTL_VALID) == 0)
    {
        PELOGE(cfgLog(pMac, LOGE, FL("Invalid cfg id %d"), cfgId);)
        retVal = eSIR_CFG_INVALID_ID;
    }
    else if (index >= CFG_STA_SBUF_MAX_SIZE)
    {
        PELOGE(cfgLog(pMac, LOGE, FL("Invalid Sbuf index %d (max size %d)"),
               index, CFG_STA_SBUF_MAX_SIZE);)
        retVal = eSIR_CFG_INVALID_ID;
    }
    else
    {
            pDst = &pMac->cfg.gCfgSBuf[index];
            paramLen = *pDst++;
            if (length > paramLen)
            {
                PELOGE(cfgLog(pMac, LOGE, FL("Invalid length %d (>%d) cfg id %d"),
                       length, paramLen, cfgId);)
                retVal = eSIR_CFG_INVALID_LEN;
            }
            else
            {
                *pDst++ = (tANI_U8)length;
                pDstEnd = pDst + length;
                while (pDst < pDstEnd)
                {
                    *pDst++ = *pStr++;
                }

                if(notifyMod)
                {
                    //                             
                    mask = control & CFG_CTL_NTF_MASK;
                    if ((mask & CFG_CTL_NTF_HW) != 0)
                    {
                        PELOGE(cfgLog(pMac, LOGE, FL("CFG Notify HW not supported!!!"));)
                    }

                    //                                  
                    if ( (mask & CFG_CTL_NTF_MASK) != 0)
                    {
                        Notify(pMac, cfgId, mask);
                    }
                }
            }

        }

    return (retVal);

} /*                           */

//                                                                      
/* 
                   
  
            
                                                     
  
         
  
               
                                                               
              
  
        
  
                                            
                                             
                                                 
                                                                   
  
                                                               
                                                         
                                                         
  
 */

tSirRetStatus
wlan_cfgGetStr(tpAniSirGlobal pMac, tANI_U16 cfgId, tANI_U8 *pBuf, tANI_U32 *pLength)
{
    tANI_U8             *pSrc, *pSrcEnd;
    tANI_U32            index, control;
    tSirRetStatus  retVal;

    if (cfgId >= CFG_PARAM_MAX_NUM)
    {
        PELOGE(cfgLog(pMac, LOGE, FL("Invalid cfg id %d"), cfgId);)
        retVal = eSIR_CFG_INVALID_ID;
        return retVal;
    }
    if (!pMac->cfg.gCfgEntry)
    {
        PELOGE(cfgLog(pMac, LOGE, FL("gCfgEntry is NULL"));)
        return eSIR_CFG_INVALID_ID;
    }

    control  = pMac->cfg.gCfgEntry[cfgId].control;
    index    = control & CFG_BUF_INDX_MASK;
    retVal   = eSIR_SUCCESS;

    if (index >= CFG_STA_SBUF_MAX_SIZE)
    {
        PELOGE(cfgLog(pMac, LOGE, FL("cfg index out of bounds %d"), index);)
        retVal = eSIR_CFG_INVALID_ID;
        return retVal;
    }

    //                            
    if ((control & CFG_CTL_VALID) == 0)
    {
        PELOGE(cfgLog(pMac, LOGE, FL("Not valid cfg id %d"), cfgId);)
        retVal = eSIR_CFG_INVALID_ID;
    }
    else
    {
        //           
        pSrc  = &pMac->cfg.gCfgSBuf[index];
        pSrc++;                               //                     
        if (*pLength < *pSrc)
        {
            PELOGE(cfgLog(pMac, LOGE, FL("Invalid length %d (<%d) cfg id %d"),
                   *pLength, *pSrc, cfgId);)
            retVal = eSIR_CFG_INVALID_LEN;
        }
        else
        {
            *pLength = *pSrc++;               //                      
            pSrcEnd = pSrc + *pLength;
            while (pSrc < pSrcEnd)
                *pBuf++ = *pSrc++;
        }
    }

    return (retVal);

} /*                          */

//                                                                      
/* 
                         
  
            
                                                          
  
         
  
               
                                                               
              
  
        
  
                                            
                                                                    
  
                                                               
                                                         
  
 */

tSirRetStatus
wlan_cfgGetStrMaxLen(tpAniSirGlobal pMac, tANI_U16 cfgId, tANI_U32 *pLength)
{
    tANI_U32            index, control;
    tSirRetStatus  retVal;

    if (cfgId >= CFG_PARAM_MAX_NUM)
    {
        PELOGE(cfgLog(pMac, LOGE, FL("Invalid cfg id %d"), cfgId);)
        retVal = eSIR_CFG_INVALID_ID;
    }
    if (!pMac->cfg.gCfgEntry)
    {
        PELOGE(cfgLog(pMac, LOGE, FL("gCfgEntry is NULL"));)
        return eSIR_CFG_INVALID_ID;
    }

    control  = pMac->cfg.gCfgEntry[cfgId].control;
    index    = control & CFG_BUF_INDX_MASK;
    retVal   = eSIR_SUCCESS;

    if (index >= CFG_STA_SBUF_MAX_SIZE)
    {
        PELOGE(cfgLog(pMac, LOGE, FL("cfg index out of bounds %d"), index);)
        retVal = eSIR_CFG_INVALID_ID;
        return retVal;
    }

    //                            
    if ((control & CFG_CTL_VALID) == 0)
    {
        PELOGE(cfgLog(pMac, LOGE, FL("Not valid cfg id %d"), cfgId);)
        retVal = eSIR_CFG_INVALID_ID;
    }
    else
    {
        *pLength = pMac->cfg.gCfgSBuf[index];
    }

    return (retVal);

} /*                                */

//                                                                      
/* 
                      
  
            
                                                  
  
         
  
               
                                                               
              
  
        
  
                                            
                                                                    
  
                                                               
                                                         
  
 */

tSirRetStatus
wlan_cfgGetStrLen(tpAniSirGlobal pMac, tANI_U16 cfgId, tANI_U32 *pLength)
{
    tANI_U32            index, control;
    tSirRetStatus  retVal;

    if (cfgId >= CFG_PARAM_MAX_NUM)
    {
        PELOGE(cfgLog(pMac, LOGE, FL("Invalid cfg id %d"), cfgId);)
        retVal = eSIR_CFG_INVALID_ID;
    }
    if (!pMac->cfg.gCfgEntry)
    {
        PELOGE(cfgLog(pMac, LOGE, FL("gCfgEntry is NULL"));)
        return eSIR_CFG_INVALID_ID;
    }

    control  = pMac->cfg.gCfgEntry[cfgId].control;
    index    = control & CFG_BUF_INDX_MASK;
    retVal   = eSIR_SUCCESS;

    if (index >= CFG_STA_SBUF_MAX_SIZE-1)
    {
        PELOGE(cfgLog(pMac, LOGE, FL("cfg index out of bounds %d"), index);)
        retVal = eSIR_CFG_INVALID_ID;
        return retVal;
    }

    //                            
    if ((control & CFG_CTL_VALID) == 0)
    {
        PELOGE(cfgLog(pMac, LOGE, FL("Not valid cfg id %d"), cfgId);)
        retVal = eSIR_CFG_INVALID_ID;
    }
    else
    {
        *pLength = pMac->cfg.gCfgSBuf[index+1];
    }

    return (retVal);

} /*                             */



/*                                                             
                                 
                                                               
            
                         
                                                              */
static tPowerdBm
cfgGetDot11dTransmitPower(tpAniSirGlobal pMac, tANI_U16   cfgId,
                                        tANI_U32 cfgLength, tANI_U8 channel)
{
    tANI_U8    *pCountryInfo = NULL;
    tANI_U8    count = 0;
    tPowerdBm  maxTxPwr = WDA_MAX_TXPOWER_INVALID;
    
    /*                                 */
    if(cfgLength < sizeof(tSirMacChanInfo))
    {
        PELOGE(cfgLog(pMac, LOGE, FL("Invalid CFGLENGTH %d while getting 11d txpower"), cfgLength);)
        goto error;
    }

    pCountryInfo = vos_mem_malloc(cfgLength);
    if ( NULL == pCountryInfo )
    {
        cfgLog(pMac, LOGP, FL(" failed to allocate memory"));
        goto error;
    }
    /*                                                                                               
                                                         
     */
    if (wlan_cfgGetStr(pMac, cfgId, pCountryInfo, &cfgLength) != eSIR_SUCCESS)
    {
        vos_mem_free(pCountryInfo);
        pCountryInfo = NULL;
            
        cfgLog(pMac, LOGP, FL("Failed to retrieve 11d configuration parameters while retrieving 11d tuples"));
        goto error;
    }
    /*                                     */
    while(count <= (cfgLength - (sizeof(tSirMacChanInfo))))
    {
        tANI_U8    firstChannel, maxChannels;

        firstChannel = pCountryInfo[count++];
        maxChannels = pCountryInfo[count++];
        maxTxPwr = pCountryInfo[count++];

        if((channel >= firstChannel) && 
            (channel < (firstChannel + maxChannels)))
        {
            break;
        }
    }

error:
    if (NULL != pCountryInfo)
        vos_mem_free(pCountryInfo);
       
    return maxTxPwr;
}


/*                                                                       
                                        

                                                        

            
               
              
                                                                        */
tPowerdBm cfgGetRegulatoryMaxTransmitPower(tpAniSirGlobal pMac, tANI_U8 channel)
{
    tANI_U32    cfgLength = 0;
    tANI_U16    cfgId = 0;
    tPowerdBm  maxTxPwr;
    eRfBandMode rfBand = eRF_BAND_UNKNOWN;

    if ((channel >= SIR_11A_CHANNEL_BEGIN) &&
        (channel <= SIR_11A_CHANNEL_END))
        rfBand = eRF_BAND_5_GHZ;
    else
        rfBand = eRF_BAND_2_4_GHZ;

    
    /*                                                                                  */
    switch (rfBand)
    {
        case eRF_BAND_2_4_GHZ:
            cfgId = WNI_CFG_MAX_TX_POWER_2_4;
            cfgLength = WNI_CFG_MAX_TX_POWER_2_4_LEN;
            PELOG2(cfgLog(pMac, LOG2, FL("HAL: Reading CFG for 2.4 GHz channels to get regulatory max tx power"));)
            break;

        case eRF_BAND_5_GHZ:
            cfgId = WNI_CFG_MAX_TX_POWER_5;
            cfgLength = WNI_CFG_MAX_TX_POWER_5_LEN;
            PELOG2(cfgLog(pMac, LOG2, FL("HAL: Reading CFG for 5.0 GHz channels to get regulatory max tx power"));)
            break;

        case eRF_BAND_UNKNOWN:
        default:
            PELOG2(cfgLog(pMac, LOG2, FL("HAL: Invalid current working band for the device"));)
            return WDA_MAX_TXPOWER_INVALID; //                      
    }

    maxTxPwr = cfgGetDot11dTransmitPower(pMac, cfgId, cfgLength, channel);

    return (maxTxPwr);
}

//                                                                      
/* 
                       
  
            
  
         
  
               
  
        
  
              
               
 */

tSirRetStatus
cfgGetCapabilityInfo(tpAniSirGlobal pMac, tANI_U16 *pCap,tpPESession sessionEntry)
{
    tANI_U32 val = 0;
    tpSirMacCapabilityInfo pCapInfo;
    tLimSystemRole systemRole = limGetSystemRole(sessionEntry);

    *pCap = 0;
    pCapInfo = (tpSirMacCapabilityInfo) pCap;

    if (systemRole == eLIM_STA_IN_IBSS_ROLE)
        pCapInfo->ibss = 1; //         
    else if ( (systemRole == eLIM_AP_ROLE) ||(systemRole == eLIM_BT_AMP_AP_ROLE)||(systemRole == eLIM_BT_AMP_STA_ROLE) ||
             (systemRole == eLIM_STA_ROLE) )
        pCapInfo->ess = 1; //        
    else if (limGetSystemRole(sessionEntry) == eLIM_P2P_DEVICE_ROLE )
    {
        pCapInfo->ess = 0;
        pCapInfo->ibss = 0;
    }
    else
        cfgLog(pMac, LOGP, FL("can't get capability, role is UNKNOWN!!"));


    if(systemRole == eLIM_AP_ROLE)
    {
        val = sessionEntry->privacy;
    }
    else
    {
        //            
        if (wlan_cfgGetInt(pMac, WNI_CFG_PRIVACY_ENABLED, &val) != eSIR_SUCCESS)
        {
            cfgLog(pMac, LOGP, FL("cfg get WNI_CFG_PRIVACY_ENABLED failed"));
            return eSIR_FAILURE;
        }
    }
    if (val)
        pCapInfo->privacy = 1;

    //                   
    if (wlan_cfgGetInt(pMac, WNI_CFG_SHORT_PREAMBLE, &val) != eSIR_SUCCESS)
    {
        cfgLog(pMac, LOGP, FL("cfg get WNI_CFG_SHORT_PREAMBLE failed"));
        return eSIR_FAILURE;
    }
    if (val)
        pCapInfo->shortPreamble = 1;


    //         
    pCapInfo->pbcc = 0;

    //                    
    pCapInfo->channelAgility = 0;
    //                                                                            
    if(sessionEntry->dot11mode == WNI_CFG_DOT11_MODE_11B)
        return eSIR_SUCCESS;

    //                    
    if (systemRole == eLIM_AP_ROLE)
    {
        pCapInfo->shortSlotTime = sessionEntry->shortSlotTimeSupported;
    }
    else
    {
        if (wlan_cfgGetInt(pMac, WNI_CFG_11G_SHORT_SLOT_TIME_ENABLED, &val)
                       != eSIR_SUCCESS)
        {
            cfgLog(pMac, LOGP,
                   FL("cfg get WNI_CFG_11G_SHORT_SLOT_TIME failed"));
            return eSIR_FAILURE;
        }
        /*                                                                                                      
                                                                                                                 
                                                                                                                  
                                                                                                                 
                                                                                                                     
         */
        if (val)
        {
            pCapInfo->shortSlotTime = sessionEntry->shortSlotTimeSupported;
        }
    }

    //                        
    if((eLIM_STA_IN_IBSS_ROLE != systemRole) &&
            sessionEntry->lim11hEnable )
    {
      if (wlan_cfgGetInt(pMac, WNI_CFG_11H_ENABLED, &val) != eSIR_SUCCESS)
      {
          cfgLog(pMac, LOGP, FL("cfg get WNI_CFG_11H_ENABLED failed"));
          return eSIR_FAILURE;
      }
      if (val)
          pCapInfo->spectrumMgt = 1;
    }

    //        
    if (wlan_cfgGetInt(pMac, WNI_CFG_QOS_ENABLED, &val) != eSIR_SUCCESS)
    {
        cfgLog(pMac, LOGP, FL("cfg get WNI_CFG_QOS_ENABLED failed"));
        return eSIR_FAILURE;
    }
    if (val)
        pCapInfo->qos = 1;

    //         
    if (wlan_cfgGetInt(pMac, WNI_CFG_APSD_ENABLED, &val) != eSIR_SUCCESS)
    {
        cfgLog(pMac, LOGP, FL("cfg get WNI_CFG_APSD_ENABLED failed"));
        return eSIR_FAILURE;
    }
    if (val)
        pCapInfo->apsd = 1;

#if defined WLAN_FEATURE_VOWIFI
    if ((limGetSystemRole(sessionEntry) == eLIM_STA_ROLE) )
    {
      if (wlan_cfgGetInt(pMac, WNI_CFG_RRM_ENABLED, &val) != eSIR_SUCCESS)
      {
        cfgLog(pMac, LOGP, FL("cfg get WNI_CFG_RRM_ENABLED failed"));
        return eSIR_FAILURE;
      }
#if defined WLAN_VOWIFI_DEBUG
      PELOGE(cfgLog( pMac, LOGE, "RRM = %d",val );)
#endif
      if (val)
        pCapInfo->rrm = 1;
    }
#endif
    //         
    //                               
    
    //              
    if (wlan_cfgGetInt(pMac, WNI_CFG_BLOCK_ACK_ENABLED, &val) != eSIR_SUCCESS)
    {
        cfgLog(pMac, LOGP, FL("cfg get WNI_CFG_BLOCK_ACK_ENABLED failed"));
        return eSIR_FAILURE;
    }
    pCapInfo->delayedBA = (tANI_U16)((val >> WNI_CFG_BLOCK_ACK_ENABLED_DELAYED) & 1);
    pCapInfo->immediateBA = (tANI_U16)((val >> WNI_CFG_BLOCK_ACK_ENABLED_IMMEDIATE) & 1);

    return eSIR_SUCCESS;
}

//                                                                     
/* 
                       
  
            
                                                          
                                            
  
         
  
               
  
                                                           
                                                                 
                                                     
  
                                                 
                                              
               
 */

void
cfgSetCapabilityInfo(tpAniSirGlobal pMac, tANI_U16 caps)
{
}


//                                                                      
/* 
               
  
            
                        
  
         
  
               
        
  
        
                                                       
  
              
  
                
  
 */

void
cfgCleanup(tpAniSirGlobal pMac)
{
    //                        
    pMac->cfg.gCfgStatus = CFG_INCOMPLETE;

} /*                      */

//                                                                      
/* 
           
  
            
                                                                       
  
         
  
               
  
        
  
                                              
                                     
  
                
  
 */

static void
Notify(tpAniSirGlobal pMac, tANI_U16 cfgId, tANI_U32 ntfMask)
{

    tSirMsgQ    mmhMsg;

    mmhMsg.type = SIR_CFG_PARAM_UPDATE_IND;
    mmhMsg.bodyval = (tANI_U32)cfgId;
    mmhMsg.bodyptr = NULL;

    MTRACE(macTraceMsgTx(pMac, NO_SESSION, mmhMsg.type));

    if ((ntfMask & CFG_CTL_NTF_SCH) != 0)
        schPostMessage(pMac, &mmhMsg);

    if ((ntfMask & CFG_CTL_NTF_LIM) != 0)
        limPostMsgApi(pMac, &mmhMsg);

    if ((ntfMask & CFG_CTL_NTF_HAL) != 0)
        wdaPostCtrlMsg(pMac, &mmhMsg);

    //           

} /*                  */

//                                                                      








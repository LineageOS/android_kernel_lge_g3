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

#include "wniApi.h"
#include "wniCfgSta.h"
#include "cfgApi.h"
#include "sirApi.h"
#include "schApi.h"
#include "utilsApi.h"
#include "limTypes.h"
#include "limUtils.h"
#include "limAssocUtils.h"
#include "limSecurityUtils.h"
#include "limSerDesUtils.h"



/* 
                                   
  
           
                                                  
                                                       
  
        
                                                        
  
              
  
       
  
                                                 
                                            
                                                     
 */

static tANI_U8
limIsRSNieValidInSmeReqMessage(tpAniSirGlobal pMac, tpSirRSNie pRSNie)
{
    tANI_U8  startPos = 0;
    tANI_U32 privacy, val;
    int len;

    if (wlan_cfgGetInt(pMac, WNI_CFG_PRIVACY_ENABLED,
                  &privacy) != eSIR_SUCCESS)
    {
        limLog(pMac, LOGP,
               FL("Unable to retrieve POI from CFG"));
    }

    if (wlan_cfgGetInt(pMac, WNI_CFG_RSN_ENABLED,
                  &val) != eSIR_SUCCESS)
    {
        limLog(pMac, LOGP,
               FL("Unable to retrieve RSN_ENABLED from CFG"));
    }

    if (pRSNie->length && (!privacy || !val))
    {
        //                                  
        /* 
                                                        
                                                             
                                 
         */
        PELOG1(limLog(pMac, LOG1, FL("RSN ie len %d but PRIVACY %d RSN %d"),
               pRSNie->length, privacy, val);)
    }

    if (pRSNie->length)
    {
        if ((pRSNie->rsnIEdata[0] != DOT11F_EID_RSN) &&
            (pRSNie->rsnIEdata[0] != DOT11F_EID_WPA)
#ifdef FEATURE_WLAN_WAPI
            && (pRSNie->rsnIEdata[0] != DOT11F_EID_WAPI)
#endif
            )
        {
            limLog(pMac, LOGE, FL("RSN/WPA/WAPI EID %d not [%d || %d]"),
                   pRSNie->rsnIEdata[0], DOT11F_EID_RSN, 
                   DOT11F_EID_WPA);
            return false;
        }

        len = pRSNie->length;
        startPos = 0;
        while(len > 0)
        {
        //                         
            if (pRSNie->rsnIEdata[startPos] == DOT11F_EID_RSN) 
            {
                if((pRSNie->rsnIEdata[startPos+1] > DOT11F_IE_RSN_MAX_LEN) ||
                    (pRSNie->rsnIEdata[startPos+1] < DOT11F_IE_RSN_MIN_LEN))
                {
                   limLog(pMac, LOGE, FL("RSN IE len %d not [%d,%d]"),
                          pRSNie->rsnIEdata[startPos+1], DOT11F_IE_RSN_MIN_LEN,
                          DOT11F_IE_RSN_MAX_LEN);
                   return false;
                }
            }
            else if(pRSNie->rsnIEdata[startPos] == DOT11F_EID_WPA)
            {
                //                         
                if (SIR_MAC_MAX_IE_LENGTH > startPos)
                {
                    if (startPos <= (SIR_MAC_MAX_IE_LENGTH - sizeof(tANI_U32)))
                        val = sirReadU32((tANI_U8 *) &pRSNie->rsnIEdata[startPos + 2]);
                    if((pRSNie->rsnIEdata[startPos + 1] < DOT11F_IE_WPA_MIN_LEN) ||
                        (pRSNie->rsnIEdata[startPos + 1] > DOT11F_IE_WPA_MAX_LEN) ||
                        (SIR_MAC_WPA_OUI != val))
                    {
                       limLog(pMac, LOGE,
                              FL("WPA IE len %d not [%d,%d] OR data 0x%x not 0x%x"),
                              pRSNie->rsnIEdata[startPos+1], DOT11F_IE_WPA_MIN_LEN,
                              DOT11F_IE_WPA_MAX_LEN, val, SIR_MAC_WPA_OUI);

                       return false;
                    }
                }
            }
#ifdef FEATURE_WLAN_WAPI
            else if(pRSNie->rsnIEdata[startPos] == DOT11F_EID_WAPI)
            {
                if((pRSNie->rsnIEdata[startPos+1] > DOT11F_IE_WAPI_MAX_LEN) ||
                 (pRSNie->rsnIEdata[startPos+1] < DOT11F_IE_WAPI_MIN_LEN))
                {
                    limLog(pMac, LOGE,
                           FL("WAPI IE len %d not [%d,%d]"),
                           pRSNie->rsnIEdata[startPos+1], DOT11F_IE_WAPI_MIN_LEN, 
                           DOT11F_IE_WAPI_MAX_LEN);

                    return false;
                }
            }
#endif
            else
            {
                //                                              
                return false;
            }
            startPos += 2 + pRSNie->rsnIEdata[startPos+1];  //                           
            len -= startPos;
        }//     

    }

    return true;
} /*                                          */

/* 
                                   
  
           
                                                  
                                                       
  
        
                                                      
  
              
  
       
  
                                                 
                                            
                                                     
 */

static tANI_U8
limIsAddieValidInSmeReqMessage(tpAniSirGlobal pMac, tpSirAddie pAddie)
{
    int left = pAddie->length;
    tANI_U8 *ptr = pAddie->addIEdata;
    tANI_U8 elem_id, elem_len;

    if (left == 0)
        return true;

    while(left >= 2)
    {
        elem_id  = ptr[0];
        elem_len = ptr[1];
        left -= 2;
        if(elem_len > left)
        {
            limLog( pMac, LOGE, 
               FL("****Invalid Add IEs eid = %d elem_len=%d left=%d*****"),
                                               elem_id,elem_len,left);
            return false;
        }
 
        left -= elem_len;
        ptr += (elem_len + 2);
    }
    //                                 
 
    
    return true;
} /*                                          */

/* 
                                              
  
           
                                                  
                                                       
  
        
                                                        
  
              
  
       
  
                                                 
                                            
                                                     
 */

tANI_U8
limSetRSNieWPAiefromSmeStartBSSReqMessage(tpAniSirGlobal pMac, 
                                          tpSirRSNie pRSNie,
                                          tpPESession pSessionEntry)
{
    tANI_U8  wpaIndex = 0;
    tANI_U32 privacy, val;

    if (wlan_cfgGetInt(pMac, WNI_CFG_PRIVACY_ENABLED,
                  &privacy) != eSIR_SUCCESS)
    {
        limLog(pMac, LOGP,
               FL("Unable to retrieve POI from CFG"));
    }

    if (wlan_cfgGetInt(pMac, WNI_CFG_RSN_ENABLED,
                  &val) != eSIR_SUCCESS)
    {
        limLog(pMac, LOGP,
               FL("Unable to retrieve RSN_ENABLED from CFG"));
    }

    if (pRSNie->length && (!privacy || !val))
    {
        //                                  
        /* 
                                                        
                                                             
                                 
         */
        PELOG1(limLog(pMac, LOG1, FL("RSN ie len %d but PRIVACY %d RSN %d"),
               pRSNie->length, privacy, val);)
    }

    if (pRSNie->length)
    {
        if ((pRSNie->rsnIEdata[0] != SIR_MAC_RSN_EID) &&
            (pRSNie->rsnIEdata[0] != SIR_MAC_WPA_EID))
        {
            limLog(pMac, LOGE, FL("RSN/WPA EID %d not [%d || %d]"),
                   pRSNie->rsnIEdata[0], SIR_MAC_RSN_EID, 
                   SIR_MAC_WPA_EID);
            return false;
        }

        //                         
        if ((pRSNie->rsnIEdata[0] == SIR_MAC_RSN_EID) &&
#if 0 //                        
            (pRSNie->rsnIEdata[1] > SIR_MAC_RSN_IE_MAX_LENGTH) ||
#endif
             (pRSNie->rsnIEdata[1] < SIR_MAC_RSN_IE_MIN_LENGTH))
        {
            limLog(pMac, LOGE, FL("RSN IE len %d not [%d,%d]"),
                   pRSNie->rsnIEdata[1], SIR_MAC_RSN_IE_MIN_LENGTH, 
                   SIR_MAC_RSN_IE_MAX_LENGTH);
            return false;
        }

        if (pRSNie->length > pRSNie->rsnIEdata[1] + 2)
        {
            if (pRSNie->rsnIEdata[0] != SIR_MAC_RSN_EID)
            {
                limLog(pMac,
                       LOGE,
                       FL("First byte[%d] in rsnIEdata is not RSN_EID"),
                       pRSNie->rsnIEdata[1]);
                return false;
            }

            limLog(pMac,
                   LOG1,
                   FL("WPA IE is present along with WPA2 IE"));
            wpaIndex = 2 + pRSNie->rsnIEdata[1];
        }
        else if ((pRSNie->length == pRSNie->rsnIEdata[1] + 2) &&
                 (pRSNie->rsnIEdata[0] == SIR_MAC_RSN_EID))
        {
            limLog(pMac,
                   LOG1,
                   FL("Only RSN IE is present"));
            dot11fUnpackIeRSN(pMac,&pRSNie->rsnIEdata[2],
                              (tANI_U8)pRSNie->length,&pSessionEntry->gStartBssRSNIe);
        }
        else if ((pRSNie->length == pRSNie->rsnIEdata[1] + 2) &&
                 (pRSNie->rsnIEdata[0] == SIR_MAC_WPA_EID))
        {
            limLog(pMac,
                   LOG1,
                   FL("Only WPA IE is present"));

            dot11fUnpackIeWPA(pMac,&pRSNie->rsnIEdata[6],(tANI_U8)pRSNie->length-4,
                                &pSessionEntry->gStartBssWPAIe);
        }

        //                         
        if(wpaIndex +4 < SIR_MAC_MAX_IE_LENGTH )
        {
            val = sirReadU32((tANI_U8 *) &pRSNie->rsnIEdata[wpaIndex + 2]);

            if ((pRSNie->rsnIEdata[wpaIndex] == SIR_MAC_WPA_EID) &&
#if 0 //                        
                (pRSNie->rsnIEdata[wpaIndex + 1] > SIR_MAC_WPA_IE_MAX_LENGTH) ||
#endif
                ((pRSNie->rsnIEdata[wpaIndex + 1] < SIR_MAC_WPA_IE_MIN_LENGTH) ||
                (SIR_MAC_WPA_OUI != val)))
            {
                limLog(pMac, LOGE,
                  FL("WPA IE len %d not [%d,%d] OR data 0x%x not 0x%x"),
                  pRSNie->rsnIEdata[1], SIR_MAC_RSN_IE_MIN_LENGTH,
                  SIR_MAC_RSN_IE_MAX_LENGTH, val, SIR_MAC_WPA_OUI);

                return false;
            }
            else
            {
                /*                                  */
                dot11fUnpackIeRSN(pMac,&pRSNie->rsnIEdata[2],
                      (tANI_U8)pRSNie->length,&pSessionEntry->gStartBssRSNIe);

                dot11fUnpackIeWPA(pMac,&pRSNie->rsnIEdata[wpaIndex + 6],
                                 pRSNie->rsnIEdata[wpaIndex + 1]-4,
                                    &pSessionEntry->gStartBssWPAIe);

            }
        }
        else
        {
            return false;
        }
    }

    return true;
} /*                                                     */




/* 
                                      
  
           
                                                     
                                                       
  
        
                                                                  
  
              
  
       
  
                                                    
                                                         
                                                              
 */

static tANI_U8
limIsBssDescrValidInSmeReqMessage(tpAniSirGlobal pMac,
                                  tpSirBssDescription pBssDescr)
{
    tANI_U8 valid = true;

    if (limIsAddrBC(pBssDescr->bssId) ||
        !pBssDescr->channelId)
    {
        valid = false;
        goto end;
    }

end:
    return valid;
} /*                                             */



/* 
                          
  
           
                                                             
                                                    
  
        
                                                         
  
              
  
       
  
                                                               
                                                                   
                          
 */

tANI_U8
limIsSmeStartReqValid(tpAniSirGlobal pMac, tANI_U32 *pMsg)
{
    tANI_U8 valid = true;

    if (((tpSirSmeStartReq) pMsg)->length != sizeof(tSirSmeStartReq))
    {
        /* 
                                              
                     
         */
        limLog(pMac, LOGW,
               FL("Invalid length %d in eWNI_SME_START_REQ"),
               ((tpSirSmeStartReq) pMsg)->length);

        valid = false;
        goto end;
    }

end:
    return valid;
} /*                                 */



/* 
                             
  
           
                                                             
                                                        
  
        
                                                         
  
              
  
       
  
                                                       
                                                                     
                                                                       
                          
 */

tANI_U8
limIsSmeStartBssReqValid(tpAniSirGlobal pMac,
                         tpSirSmeStartBssReq pStartBssReq)
{
    tANI_U8   i = 0;
    tANI_U8 valid = true;

    PELOG1(limLog(pMac, LOG1,
           FL("Parsed START_BSS_REQ fields are bssType=%d, channelId=%d, SSID len=%d, rsnIE len=%d, nwType=%d, rateset len=%d"),
           pStartBssReq->bssType,
           pStartBssReq->channelId,
           pStartBssReq->ssId.length,
           pStartBssReq->rsnIE.length,
           pStartBssReq->nwType,
           pStartBssReq->operationalRateSet.numRates);)

    switch (pStartBssReq->bssType)
    {
        case eSIR_INFRASTRUCTURE_MODE:
            /* 
                                                                  
                                     
                         
             */
            limLog(pMac, LOGE,
                   FL("Invalid bssType %d in eWNI_SME_START_BSS_REQ"),
                   pStartBssReq->bssType);
            valid = false;
            goto end;
            break;

        case eSIR_IBSS_MODE:
            break;

        /*                          */
        case eSIR_BTAMP_STA_MODE:              
            break;
            
        /*                          */
        case eSIR_BTAMP_AP_MODE:
            break;

        /*                          */
        case eSIR_INFRA_AP_MODE:
            break;
        
        default:
            /* 
                                                                  
                                              
                        
             */
            limLog(pMac, LOGW,
               FL("Invalid bssType %d in eWNI_SME_START_BSS_REQ"),
               pStartBssReq->bssType);

            valid = false;
            goto end;
    }

    /*                                               */
    if (pStartBssReq->bssType == eSIR_IBSS_MODE)
    {
        if (!pStartBssReq->ssId.length ||
            (pStartBssReq->ssId.length > SIR_MAC_MAX_SSID_LENGTH))
        {
            //                           
            //                     
            limLog(pMac, LOGW,
                FL("Invalid SSID length in eWNI_SME_START_BSS_REQ"));

            valid = false;
            goto end;
        }
    }


    if (!limIsRSNieValidInSmeReqMessage(pMac, &pStartBssReq->rsnIE))
    {
        valid = false;
        goto end;
    }

    if (pStartBssReq->nwType != eSIR_11A_NW_TYPE &&
        pStartBssReq->nwType != eSIR_11B_NW_TYPE &&
        pStartBssReq->nwType != eSIR_11G_NW_TYPE)
    {
        valid = false;
        goto end;
    }

    if (pStartBssReq->nwType == eSIR_11A_NW_TYPE)
    {
        for (i = 0; i < pStartBssReq->operationalRateSet.numRates; i++)
            if (!sirIsArate(pStartBssReq->operationalRateSet.rate[i] & 0x7F))
        {
            //                          
            //                     
            limLog(pMac, LOGW,
                   FL("Invalid operational rates in eWNI_SME_START_BSS_REQ"));
            sirDumpBuf(pMac, SIR_LIM_MODULE_ID, LOG2,
                       pStartBssReq->operationalRateSet.rate,
                       pStartBssReq->operationalRateSet.numRates);

            valid = false;
            goto end;
        }
    }
    //                                                                      
    else if (pStartBssReq->nwType == eSIR_11G_NW_TYPE)
    {
        for (i = 0; i < pStartBssReq->operationalRateSet.numRates; i++)
            if (!sirIsGrate(pStartBssReq->operationalRateSet.rate[i] & 0x7F))
        {
            //                          
            //                     
            limLog(pMac, LOGW,
                   FL("Invalid operational rates in eWNI_SME_START_BSS_REQ"));
            sirDumpBuf(pMac, SIR_LIM_MODULE_ID, LOG2,
                       pStartBssReq->operationalRateSet.rate,
                       pStartBssReq->operationalRateSet.numRates);

            valid = false;
            goto end;
        }
    }
    else
    {
        for (i = 0; i < pStartBssReq->operationalRateSet.numRates; i++)
            if (!sirIsBrate(pStartBssReq->operationalRateSet.rate[i] & 0x7F))
        {
            //                          
            //                     
            limLog(pMac, LOGW,
                   FL("Invalid operational rates in eWNI_SME_START_BSS_REQ"));
            sirDumpBuf(pMac, SIR_LIM_MODULE_ID, LOG2,
                       pStartBssReq->operationalRateSet.rate,
                       pStartBssReq->operationalRateSet.numRates);

            valid = false;
            goto end;
        }
    }

end:
    return valid;
} /*                                    */



/* 
                         
  
           
                                                             
                                                   
  
        
                                                         
  
              
  
       
  
                                                     
                                                              
                                                                  
                          
 */

tANI_U8
limIsSmeJoinReqValid(tpAniSirGlobal pMac, tpSirSmeJoinReq pJoinReq)
{
    tANI_U8 valid = true;


    if (!limIsRSNieValidInSmeReqMessage(pMac, &pJoinReq->rsnIE))
    {
        limLog(pMac, LOGE,
               FL("received SME_JOIN_REQ with invalid RSNIE"));
        valid = false;
        goto end;
    }

    if (!limIsAddieValidInSmeReqMessage(pMac, &pJoinReq->addIEScan))
    {
        limLog(pMac, LOGE,
               FL("received SME_JOIN_REQ with invalid additional IE for scan"));
        valid = false;
        goto end;
    }

    if (!limIsAddieValidInSmeReqMessage(pMac, &pJoinReq->addIEAssoc))
    {
        limLog(pMac, LOGE,
               FL("received SME_JOIN_REQ with invalid additional IE for assoc"));
        valid = false;
        goto end;
    }


    if (!limIsBssDescrValidInSmeReqMessage(pMac,
                                           &pJoinReq->bssDescription))
    {
        //                                                  
        //              
        limLog(pMac, LOGE,
               FL("received SME_JOIN_REQ with invalid bssInfo"));

        valid = false;
        goto end;
    }

    /*
                                                   
                                   
    */
    if ( vos_mem_compare( (tANI_U8* ) pJoinReq->selfMacAddr,
                       (tANI_U8 *) pJoinReq->bssDescription.bssId, 
                       (tANI_U8) (sizeof(tSirMacAddr))))
    {
        //              
        limLog(pMac, LOGE,
               FL("received SME_JOIN_REQ with Self Mac and BSSID Same"));

        valid = false;
        goto end;
    }

end:
    return valid;
} /*                                */



/* 
                             
  
           
                                                             
                                                       
  
        
                                                         
  
              
  
       
  
                                                       
                                                                    
                                                                   
                                 
                                 
 */

tANI_U8
limIsSmeDisassocReqValid(tpAniSirGlobal pMac,
                         tpSirSmeDisassocReq pDisassocReq, tpPESession psessionEntry)
{
    if (limIsGroupAddr(pDisassocReq->peerMacAddr) &&
         !limIsAddrBC(pDisassocReq->peerMacAddr))
        return false;


    return true;
} /*                                    */



/* 
                             
  
           
                                                             
                                                       
  
        
                                                         
  
              
  
       
  
                                                       
                                                                    
                                                                   
                                 
                                 
 */

tANI_U8
limIsSmeDisassocCnfValid(tpAniSirGlobal pMac,
                         tpSirSmeDisassocCnf pDisassocCnf, tpPESession psessionEntry)
{
    if (limIsGroupAddr(pDisassocCnf->peerMacAddr))
        return false;

    return true;
} /*                                    */



/* 
                           
  
           
                                                             
                                                     
  
        
                                                         
  
              
  
       
  
                                                     
                                                                
                                                                         
                               
 */

tANI_U8
limIsSmeDeauthReqValid(tpAniSirGlobal pMac, tpSirSmeDeauthReq pDeauthReq, tpPESession psessionEntry)
{
    if (limIsGroupAddr(pDeauthReq->peerMacAddr) &&
         !limIsAddrBC(pDeauthReq->peerMacAddr))
        return false;

    return true;
} /*                                  */



/* 
                         
  
           
                                                             
                                                   
  
        
                                                         
  
              
  
       
  
                                                            
                                                                  
                          
 */

tANI_U8
limIsSmeScanReqValid(tpAniSirGlobal pMac, tpSirSmeScanReq pScanReq)
{
    tANI_U8 valid = true;
    tANI_U8 i = 0;

    for (i = 0; i < pScanReq->numSsid; i++)
    {
        if (pScanReq->ssId[i].length > SIR_MAC_MAX_SSID_LENGTH)
        {
            valid = false;
            goto end;    
        }
    }
    if ((pScanReq->bssType > eSIR_AUTO_MODE) ||
        (limIsGroupAddr(pScanReq->bssId) && !limIsAddrBC(pScanReq->bssId)) ||
        (!(pScanReq->scanType == eSIR_PASSIVE_SCAN || pScanReq->scanType == eSIR_ACTIVE_SCAN)) || 
        (pScanReq->channelList.numChannels > SIR_MAX_NUM_CHANNELS))
    {
        valid = false;
        goto end;
    }

    /*
                                      
     */

    if ((pScanReq->scanType == eSIR_ACTIVE_SCAN) && 
        (pScanReq->maxChannelTime < pScanReq->minChannelTime))
    {
        PELOGW(limLog(pMac, LOGW, FL("Max Channel Time < Min Channel Time"));)
        valid = false;
        goto end;
    }

end:
    return valid;
} /*                                */



/* 
                         
  
           
                                                             
                                                   
  
        
                                                         
  
              
  
       
  
                                                            
                                                                  
                          
 */

tANI_U8
limIsSmeAuthReqValid(tpSirSmeAuthReq pAuthReq)
{
    tANI_U8 valid = true;

    if (limIsGroupAddr(pAuthReq->peerMacAddr) ||
        (pAuthReq->authType > eSIR_AUTO_SWITCH) ||
        !pAuthReq->channelNumber)
    {
        valid = false;
        goto end;
    }

end:
    return valid;
} /*                                */



/* 
                               
  
           
                                                             
                                                          
  
        
                                                         
  
              
  
       
  
                                                                 
                                                                         
                          
 */

tANI_U8
limIsSmeSetContextReqValid(tpAniSirGlobal pMac, tpSirSmeSetContextReq  pSetContextReq)
{
    tANI_U8 i = 0;
    tANI_U8 valid = true;
    tpSirKeys pKey = pSetContextReq->keyMaterial.key;

    if ((pSetContextReq->keyMaterial.edType != eSIR_ED_WEP40) &&
        (pSetContextReq->keyMaterial.edType != eSIR_ED_WEP104) &&
        (pSetContextReq->keyMaterial.edType != eSIR_ED_NONE) &&
#ifdef FEATURE_WLAN_WAPI
        (pSetContextReq->keyMaterial.edType != eSIR_ED_WPI) && 
#endif
        !pSetContextReq->keyMaterial.numKeys)
    {
        /* 
                                                  
                     
         */
        limLog(pMac, LOGW,
           FL("No keys present in SME_SETCONTEXT_REQ for edType=%d"),
           pSetContextReq->keyMaterial.edType);

        valid = false;
        goto end;
    }

    if (pSetContextReq->keyMaterial.numKeys &&
        (pSetContextReq->keyMaterial.edType == eSIR_ED_NONE))
    {
        /* 
                                               
                     
         */
        limLog(pMac, LOGW,
           FL("Keys present in SME_SETCONTEXT_REQ for edType=%d"),
           pSetContextReq->keyMaterial.edType);

        valid = false;
        goto end;
    }

    if (pSetContextReq->keyMaterial.edType >= eSIR_ED_NOT_IMPLEMENTED)
    {
        /* 
                                        
                     
         */
        limLog(pMac, LOGW,
               FL("Invalid edType=%d in SME_SETCONTEXT_REQ"),
               pSetContextReq->keyMaterial.edType);

        valid = false;
        goto end;
    }
    else if (pSetContextReq->keyMaterial.edType > eSIR_ED_NONE)
    {
        tANI_U32 poi;

        if (wlan_cfgGetInt(pMac, WNI_CFG_PRIVACY_ENABLED,
                      &poi) != eSIR_SUCCESS)
        {
            limLog(pMac, LOGP,
                   FL("Unable to retrieve POI from CFG"));
        }

        if (!poi)
        {
            /* 
                                     
                                                            
                                                                 
                                     
             */
            PELOG1(limLog(pMac, LOG1,
               FL("Privacy is not enabled, yet non-None EDtype=%d in SME_SETCONTEXT_REQ"),
               pSetContextReq->keyMaterial.edType);)
        }
    }

    for (i = 0; i < pSetContextReq->keyMaterial.numKeys; i++)
    {
        if (((pSetContextReq->keyMaterial.edType == eSIR_ED_WEP40) &&
             (pKey->keyLength != 5)) ||
            ((pSetContextReq->keyMaterial.edType == eSIR_ED_WEP104) &&
             (pKey->keyLength != 13)) ||
            ((pSetContextReq->keyMaterial.edType == eSIR_ED_TKIP) &&
             (pKey->keyLength != 32)) ||
#ifdef FEATURE_WLAN_WAPI 
            ((pSetContextReq->keyMaterial.edType == eSIR_ED_WPI) &&
             (pKey->keyLength != 32)) ||
#endif 
            ((pSetContextReq->keyMaterial.edType == eSIR_ED_CCMP) &&
             (pKey->keyLength != 16)))
        {
            /* 
                                                     
                         
             */
            limLog(pMac, LOGW,
               FL("Invalid keyLength =%d for edType=%d in SME_SETCONTEXT_REQ"),
               pKey->keyLength, pSetContextReq->keyMaterial.edType);

            valid = false;
            goto end;
        }
        pKey++;
    }

end:
    return valid;
} /*                                      */



/* 
                            
  
           
                                                             
                                                       
  
        
                                                         
  
              
  
       
  
                                                              
                                                                      
                          
 */

tANI_U8
limIsSmeStopBssReqValid(tANI_U32 *pMsg)
{
    tANI_U8 valid = true;

    return valid;
} /*                                   */


/* 
                                 
  
           
                                                         
                                                              
                           
  
         
  
        
  
              
     
  
       
     
  
                                                                   
                              
                                       
 */

tANI_U8*
limGetBssIdFromSmeJoinReqMsg(tANI_U8 *pBuf)
{
    if (!pBuf)
        return NULL;

    pBuf += sizeof(tANI_U32); //                    


    pBuf += limGetU16(pBuf) + sizeof(tANI_U16); //            

    pBuf  += sizeof(tANI_U16);                 //                               

    return (pBuf);
} /*                                        */



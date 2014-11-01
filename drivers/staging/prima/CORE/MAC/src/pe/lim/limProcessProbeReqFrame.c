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

#include "wniCfgSta.h"
#include "aniGlobal.h"
#include "cfgApi.h"

#include "utilsApi.h"
#include "limTypes.h"
#include "limUtils.h"
#include "limAssocUtils.h"
#include "limSerDesUtils.h"
#include "parserApi.h"
#include "limSession.h"

#ifdef WLAN_FEATURE_P2P_INTERNAL
void limSendP2PProbeResponse(tpAniSirGlobal pMac, tANI_U8 *pBd, 
                      tpPESession psessionEntry);
#endif
void

limSendSmeProbeReqInd(tpAniSirGlobal pMac,
                      tSirMacAddr peerMacAddr,
                      tANI_U8 *pProbeReqIE,
                      tANI_U32 ProbeReqIELen,
                      tpPESession psessionEntry);
                      
/* 
                       
  
           
                                                       
  
        
                                                                       
  
              
  
  
       
  
                                                 
                                                                
                                                                                 
                                                          
  
               
 */

void limGetWPSPBCSessions(tpAniSirGlobal pMac, tANI_U8 *addr, 
                          tANI_U8 *uuid_e, eWPSPBCOverlap *overlap, 
                          tpPESession psessionEntry)
{
    int count = 0;
    tSirWPSPBCSession *pbc;
    tANI_TIMESTAMP curTime;

    curTime = (tANI_TIMESTAMP)(palGetTickCount(pMac->hHdd) / PAL_TICKS_PER_SECOND);

    vos_mem_set((tANI_U8 *)addr, sizeof(tSirMacAddr), 0);
    vos_mem_set((tANI_U8 *)uuid_e, SIR_WPS_UUID_LEN, 0);

    for (pbc = psessionEntry->pAPWPSPBCSession; pbc; pbc = pbc->next) {

        if (curTime > pbc->timestamp + SIR_WPS_PBC_WALK_TIME)
            break;

        count++;
        if(count > 1)
            break;
            
        vos_mem_copy((tANI_U8 *)addr, (tANI_U8 *)pbc->addr, sizeof(tSirMacAddr));
        vos_mem_copy((tANI_U8 *)uuid_e, (tANI_U8 *)pbc->uuid_e, SIR_WPS_UUID_LEN);
        }

    if (count > 1)
    {
        *overlap = eSAP_WPSPBC_OVERLAP_IN120S;    //          
    }
    else if(count == 0)
    {
        *overlap = eSAP_WPSPBC_NO_WPSPBC_PROBE_REQ_IN120S;    //                                       
    } else
    {
         *overlap = eSAP_WPSPBC_ONE_WPSPBC_PROBE_REQ_IN120S;   //                                    
    }

    PELOGE(limLog(pMac, LOGE, FL("overlap = %d"), *overlap);)
    PELOGE(sirDumpBuf(pMac, SIR_LIM_MODULE_ID, LOGE, addr, sizeof(tSirMacAddr));)
    PELOGE(sirDumpBuf(pMac, SIR_LIM_MODULE_ID, LOGE, uuid_e, SIR_WPS_UUID_LEN);)

    return;
}

/* 
                              
  
           
                                                                                                  
  
        
  
   
              
  
  
       
  
                                                 
                                                                        
  
               
 */
static void limRemoveTimeoutPBCsessions(tpAniSirGlobal pMac, tSirWPSPBCSession *pbc)
{
     tSirWPSPBCSession *prev;

        while (pbc) {
                prev = pbc;
                pbc = pbc->next;

        PELOG4(limLog(pMac, LOG4, FL("WPS PBC sessions remove"));)
        PELOG4(sirDumpBuf(pMac, SIR_LIM_MODULE_ID, LOG4, prev->addr, sizeof(tSirMacAddr));)
        PELOG4(sirDumpBuf(pMac, SIR_LIM_MODULE_ID, LOG4, prev->uuid_e, SIR_WPS_UUID_LEN);)
        
        vos_mem_free(prev);
       }
}

void limRemovePBCSessions(tpAniSirGlobal pMac, tSirMacAddr pRemoveMac,tpPESession psessionEntry)
{
    tSirWPSPBCSession *pbc, *prev = NULL;
    prev = pbc = psessionEntry->pAPWPSPBCSession;

    while (pbc) {
        if (vos_mem_compare((tANI_U8 *)pbc->addr,
                            (tANI_U8 *)pRemoveMac, sizeof(tSirMacAddr))) {
          prev->next = pbc->next;
          if (pbc == psessionEntry->pAPWPSPBCSession)
            psessionEntry->pAPWPSPBCSession = pbc->next;
            vos_mem_free(pbc);
            return;
        }
        prev = pbc;
        pbc = pbc->next;
    }

}

/* 
                           
  
           
                                                                         
  
        
                                                                                      
                                                                              
                                                     
   
              
  
  
       
  
                                                 
                                                                
                                                       
                                                          
  
               
 */

static void limUpdatePBCSessionEntry(tpAniSirGlobal pMac,
                                     tANI_U8 *addr, tANI_U8 *uuid_e, 
                                     tpPESession psessionEntry)
{
    tSirWPSPBCSession *pbc, *prev = NULL;

    tANI_TIMESTAMP curTime;

    curTime = (tANI_TIMESTAMP)(palGetTickCount(pMac->hHdd) / PAL_TICKS_PER_SECOND);
            
    PELOG4(limLog(pMac, LOG4, FL("Receive WPS probe reques curTime=%d"), curTime);)
    PELOG4(sirDumpBuf(pMac, SIR_LIM_MODULE_ID, LOG4, addr, sizeof(tSirMacAddr));)
    PELOG4(sirDumpBuf(pMac, SIR_LIM_MODULE_ID, LOG4, uuid_e, SIR_WPS_UUID_LEN);)

    pbc = psessionEntry->pAPWPSPBCSession;

    while (pbc) {
        if (vos_mem_compare((tANI_U8 *)pbc->addr, (tANI_U8 *)addr, sizeof(tSirMacAddr)) &&
            vos_mem_compare((tANI_U8 *)pbc->uuid_e, (tANI_U8 *)uuid_e, SIR_WPS_UUID_LEN)) {
            if (prev)
                prev->next = pbc->next;
            else
                psessionEntry->pAPWPSPBCSession = pbc->next;
            break;
        }
        prev = pbc;
        pbc = pbc->next;
    }

    if (!pbc) {
        pbc = vos_mem_malloc(sizeof(tSirWPSPBCSession));
        if ( NULL == pbc )
        {
            PELOGE(limLog(pMac, LOGE, FL("memory allocate failed!"));)
            return;
        }
        vos_mem_copy((tANI_U8 *)pbc->addr, (tANI_U8 *)addr, sizeof(tSirMacAddr));
    
        if (uuid_e)
            vos_mem_copy((tANI_U8 *)pbc->uuid_e, (tANI_U8 *)uuid_e, SIR_WPS_UUID_LEN);
    }
    
    pbc->next = psessionEntry->pAPWPSPBCSession;
    psessionEntry->pAPWPSPBCSession = pbc;
    pbc->timestamp = curTime;
    
    /*                                    */
    prev = pbc;
    pbc = pbc->next;

    while (pbc) {
        if (curTime > pbc->timestamp + SIR_WPS_PBC_WALK_TIME) {
            prev->next = NULL;
            limRemoveTimeoutPBCsessions(pMac, pbc);
           break;
        }
        prev = pbc;
        pbc = pbc->next;
    }
}
#if 0
/* 
                   
  
           
                                                                          
  
        
                                                                      
  
              
  
  
       
  
                                                 
                                                          
  
               
 */

void limWPSPBCTimeout(tpAniSirGlobal pMac, tpPESession psessionEntry)
{
    tANI_TIMESTAMP curTime;
    tSirWPSPBCSession *pbc, *prev = NULL;

    curTime = (tANI_TIMESTAMP)(palGetTickCount(pMac->hHdd) / PAL_TICKS_PER_SECOND);
    
    PELOG3(limLog(pMac, LOG3, FL("WPS PBC cleanup timeout curTime=%d"), curTime);)

    prev = psessionEntry->pAPWPSPBCSession; 
    if(prev)
        pbc = prev->next;
    else
        return;
            
    while (pbc) {
        if (curTime > pbc->timestamp + SIR_WPS_PBC_WALK_TIME) {
            prev->next = NULL;
            limRemoveTimeoutPBCsessions(pMac, pbc);
            break;
        }
        prev = pbc;
        pbc = pbc->next;
    }

    if(prev)
    {
         if (curTime > prev->timestamp + SIR_WPS_PBC_WALK_TIME) {
            psessionEntry->pAPWPSPBCSession = NULL;  
            limRemoveTimeoutPBCsessions(pMac, prev);
         }
    }

}
#endif
/* 
                 
  
           
                                             
  
        
                                               
  
              
  
  
       
  
                                                 
                                                          
  
               
 */

void limWPSPBCClose(tpAniSirGlobal pMac, tpPESession psessionEntry)
{

    limRemoveTimeoutPBCsessions(pMac, psessionEntry->pAPWPSPBCSession);

}

/* 
                   
  
           
                                                            
                                 
  
        
                                                                          
   
       
  
                  
  
                  
 */

tANI_BOOLEAN limCheck11bRates(tANI_U8 rate)
{
    if ( ( 0x02 == (rate))
      || ( 0x04 == (rate))
      || ( 0x0b == (rate))
      || ( 0x16 == (rate))
       )
       {
           return TRUE;
       } 
    return FALSE;
}
  
/* 
                          
  
           
                                                           
                                 
  
        
                                                                    
                       
                                                                    
                 
  
              
                                                                      
                                                    
  
       
                                                                 
                                                      
  
                                                 
                                                                            
  
               
 */

void
limProcessProbeReqFrame(tpAniSirGlobal pMac, tANI_U8 *pRxPacketInfo,tpPESession psessionEntry)
{
    tANI_U8             *pBody;
    tpSirMacMgmtHdr     pHdr;
    tANI_U32            frameLen;
    tSirProbeReq        probeReq;
    tAniSSID            ssId;
    tSirMsgQ            msgQ;
    tSirSmeProbeReq     *pSirSmeProbeReq;
    tANI_U32            wpsApEnable=0, tmp;

    do{
        //                                       
        if (pMac->lim.gLimProbeRespDisableFlag)
            break;

        //                                                                               
        if((psessionEntry->pePersona == VOS_P2P_GO_MODE) && ((pMac->lim.gpLimRemainOnChanReq )
                                  || (pMac->lim.gLimHalScanState != eLIM_HAL_IDLE_SCAN_STATE)))
        {
           limLog(pMac, LOG3,
              FL("While GO is scanning, don't send probe response on diff channel"));
           break;
        }

       pHdr = WDA_GET_RX_MAC_HEADER(pRxPacketInfo);

        if ( (psessionEntry->limSystemRole == eLIM_AP_ROLE) ||
             (psessionEntry->limSystemRole == eLIM_BT_AMP_AP_ROLE)||
             (psessionEntry->limSystemRole == eLIM_BT_AMP_STA_ROLE)|| 
             ( (psessionEntry->limSystemRole == eLIM_STA_IN_IBSS_ROLE) &&
             (WDA_GET_RX_BEACON_SENT(pRxPacketInfo)) ) )
        {
           frameLen = WDA_GET_RX_PAYLOAD_LEN(pRxPacketInfo);

            PELOG3(limLog(pMac, LOG3, FL("Received Probe Request %d bytes from "), frameLen);
            limPrintMacAddr(pMac, pHdr->sa, LOG3);)

            //                                        
           pBody = WDA_GET_RX_MPDU_DATA(pRxPacketInfo);

            //                          
            if (sirConvertProbeReqFrame2Struct(pMac, pBody, frameLen, &probeReq)==eSIR_FAILURE)
            {
                PELOGW(limLog(pMac, LOGW, FL("Parse error ProbeRequest, length=%d, SA is:"), frameLen);)
                limPrintMacAddr(pMac, pHdr->sa, LOGW);
                pMac->sys.probeError++;
                break;
            }
            else
            {
                if (psessionEntry->pePersona == VOS_P2P_GO_MODE)
                {
                    tANI_U8 i = 0, rate_11b = 0, other_rates = 0;
                    //                                   
                    for ( i = 0 ; i < probeReq.supportedRates.numRates;
                                                                  i++ )
                    {
                        if (limCheck11bRates(probeReq.supportedRates.rate[i] & 0x7f))
                        {
                            rate_11b++; 
                        }
                        else
                        {
                            other_rates++;
                        }
                    }

                    //                                  
                    for ( i = 0 ; i < probeReq.extendedRates.numRates; i++ )
                    {
                        if (limCheck11bRates(probeReq.extendedRates.rate[i] & 0x7f))
                        {
                            rate_11b++; 
                        }
                        else
                        {
                            other_rates++;
                        }
                    }

                    if ( (rate_11b > 0) && (other_rates == 0) )
                    {
                        PELOG3(limLog(pMac, LOG3, 
                               FL("Received a probe request frame with only 11b rates, SA is: "));
                               limPrintMacAddr(pMac, pHdr->sa, LOG3);)
                        return;
                    }
                }
                if ((psessionEntry->limSystemRole == eLIM_AP_ROLE))
                {
                  
                    if ( (psessionEntry->APWPSIEs.SirWPSProbeRspIE.FieldPresent &
                                               SIR_WPS_PROBRSP_VER_PRESENT) &&
                         (probeReq.wscIePresent ==  1) &&
                         (probeReq.probeReqWscIeInfo.DevicePasswordID.id == 
                                                  WSC_PASSWD_ID_PUSH_BUTTON) &&
                         (probeReq.probeReqWscIeInfo.UUID_E.present == 1))
                    {
                        if(psessionEntry->fwdWPSPBCProbeReq)
                        {
                            PELOG4(sirDumpBuf(pMac, SIR_LIM_MODULE_ID, LOG4,
                                              pHdr->sa, sizeof(tSirMacAddr));)
                            PELOG4(sirDumpBuf(pMac, SIR_LIM_MODULE_ID, LOG4, pBody, frameLen);)
                            limSendSmeProbeReqInd(pMac, pHdr->sa, pBody, frameLen, psessionEntry);
                        } 
                        else
                        {
                            limUpdatePBCSessionEntry(pMac,
                                pHdr->sa, probeReq.probeReqWscIeInfo.UUID_E.uuid, psessionEntry);
                        }
                    }
                }
                else
                {
                    if (wlan_cfgGetInt(pMac, (tANI_U16) WNI_CFG_WPS_ENABLE, &tmp) != eSIR_SUCCESS)
                        limLog(pMac, LOGP,"Failed to cfg get id %d", WNI_CFG_WPS_ENABLE );

                    wpsApEnable = tmp & WNI_CFG_WPS_ENABLE_AP;
                    if ((wpsApEnable) &&
                        (probeReq.wscIePresent ==  1) &&
                        (probeReq.probeReqWscIeInfo.DevicePasswordID.id == WSC_PASSWD_ID_PUSH_BUTTON))
                    {
                        //                                                         
                        pSirSmeProbeReq = vos_mem_malloc(sizeof(tSirSmeProbeReq));
                        if ( NULL == pSirSmeProbeReq )
                        {
                            //          
                            limLog(pMac, LOGP,
                               FL("call to AllocateMemory failed for eWNI_SME_PROBE_REQ"));
                            return;
                        }
                        msgQ.type = eWNI_SME_PROBE_REQ;
                        msgQ.bodyval = 0;
                        msgQ.bodyptr = pSirSmeProbeReq;

                        pSirSmeProbeReq->messageType = eWNI_SME_PROBE_REQ;
                        pSirSmeProbeReq->length = sizeof(tSirSmeProbeReq);
                        pSirSmeProbeReq->sessionId = psessionEntry->smeSessionId;
                        vos_mem_copy(pSirSmeProbeReq->peerMacAddr, pHdr->sa, sizeof(tSirMacAddr));
                        pSirSmeProbeReq->devicePasswdId = probeReq.probeReqWscIeInfo.DevicePasswordID.id;
                        MTRACE(macTraceMsgTx(pMac, psessionEntry->peSessionId, msgQ.type));
                       if (limSysProcessMmhMsgApi(pMac, &msgQ,  ePROT) != eSIR_SUCCESS){
                            PELOG3(limLog(pMac, LOG3, FL("couldnt send the probe req to wsm "));)
                        }
                    }
                }
            }

            ssId.length = psessionEntry->ssId.length;
             /*                                                   */
            vos_mem_copy(ssId.ssId,
                         psessionEntry->ssId.ssId,
                         psessionEntry->ssId.length);

            //                                                 
            //                                  
            if (probeReq.ssId.length)
            {
                if (!ssId.length)
                    goto multipleSSIDcheck;

                if (vos_mem_compare((tANI_U8 *) &ssId,
                              (tANI_U8 *) &(probeReq.ssId), (tANI_U8) (ssId.length + 1)) )
                {
                    limSendProbeRspMgmtFrame(pMac, pHdr->sa, &ssId, DPH_USE_MGMT_STAID,
                                             DPH_NON_KEEPALIVE_FRAME, psessionEntry,
                                             probeReq.p2pIePresent);
                    break;
                }
                else if (psessionEntry->pePersona == VOS_P2P_GO_MODE)
                {
                    tANI_U8   direct_ssid[7] = "DIRECT-";
                    tANI_U8   direct_ssid_len = 7;
                    if (vos_mem_compare((tANI_U8 *) &direct_ssid,
                              (tANI_U8 *) &(probeReq.ssId.ssId), (tANI_U8) (direct_ssid_len)) )
                    {
                        limSendProbeRspMgmtFrame(pMac, pHdr->sa, &ssId, DPH_USE_MGMT_STAID,
                                             DPH_NON_KEEPALIVE_FRAME, psessionEntry,
                                             probeReq.p2pIePresent);
                        break;
                    }
                }
                else
                {
                   PELOG3(limLog(pMac, LOG3,
                       FL("Ignoring ProbeReq frame with unmatched SSID received from "));
                    limPrintMacAddr(pMac, pHdr->sa, LOG3);)
                    pMac->sys.probeBadSsid++;
                }
            }
            else
            {
                {
                    //                                     
                    //                                       
                    //                                                               
                    if(psessionEntry->ssidHidden)
                      /*                                                                        
                                                         */
                        //                
                           return;
                    limSendProbeRspMgmtFrame(pMac, pHdr->sa, &ssId, DPH_USE_MGMT_STAID,
                                             DPH_NON_KEEPALIVE_FRAME, psessionEntry,
                                             probeReq.p2pIePresent);
                }
                break;
            }
multipleSSIDcheck:
            {
               PELOG3(limLog(pMac, LOG3,
                   FL("Ignoring ProbeReq frame with unmatched SSID received from "));
                limPrintMacAddr(pMac, pHdr->sa, LOG3);)
                pMac->sys.probeBadSsid++;
            }
        }
        else
        {
            //                                    
            PELOG3(limLog(pMac, LOG3, FL("Ignoring Probe Request frame received from "));
            limPrintMacAddr(pMac, pHdr->sa, LOG3);)
            pMac->sys.probeIgnore++;
            break;
        }
    }while(0);

    return;
} /*                                   */

/* 
                           
  
           
                                                                         
                                 
  
        
                                                                 
                              
  
                                                            
                                                                             
                                                    
  
               
 */

static void
limIndicateProbeReqToHDD(tpAniSirGlobal pMac, tANI_U8 *pBd,
                         tpPESession psessionEntry)
{
    tpSirMacMgmtHdr     pHdr;
    tANI_U32            frameLen;

    limLog( pMac, LOG1, "Received a probe request frame");

    pHdr = WDA_GET_RX_MAC_HEADER(pBd);
    frameLen = WDA_GET_RX_PAYLOAD_LEN(pBd);

    //                          
    limSendSmeMgmtFrameInd( pMac, pHdr->fc.subType,
               (tANI_U8*)pHdr, (frameLen + sizeof(tSirMacMgmtHdr)), 
               psessionEntry->smeSessionId, WDA_GET_RX_CH(pBd),
               psessionEntry, 0);
#ifdef WLAN_FEATURE_P2P_INTERNAL
    limSendP2PProbeResponse(pMac, pBd, psessionEntry);
#endif
} /*                                    */

/* 
                                       
  
           
                                                           
                                 
  
        
                                                                    
                                                                    
                                                                   
                       
  
                                                            
                                                                             
                                                    
  
               
 */

void
limProcessProbeReqFrame_multiple_BSS(tpAniSirGlobal pMac, tANI_U8 *pBd,  tpPESession psessionEntry)
{
    tANI_U8 i;

    if (psessionEntry != NULL)
    {
        if ((eLIM_AP_ROLE == psessionEntry->limSystemRole)
#ifdef WLAN_FEATURE_P2P_INTERNAL
         || (psessionEntry->limSystemRole == eLIM_P2P_DEVICE_ROLE)
#endif
          )
        {
            limIndicateProbeReqToHDD(pMac, pBd, psessionEntry);
        }
        limProcessProbeReqFrame(pMac,pBd,psessionEntry);
        return;
    }

    for(i =0; i < pMac->lim.maxBssId;i++)
    {
        psessionEntry = peFindSessionBySessionId(pMac,i);
        if ( (psessionEntry != NULL) )
        {
            if ((eLIM_AP_ROLE == psessionEntry->limSystemRole)
#ifdef WLAN_FEATURE_P2P_INTERNAL
             || (psessionEntry->limSystemRole == eLIM_P2P_DEVICE_ROLE)
#endif
              )
            {
                limIndicateProbeReqToHDD(pMac, pBd, psessionEntry);
            }
            if ( (eLIM_AP_ROLE == psessionEntry->limSystemRole) ||
                (eLIM_STA_IN_IBSS_ROLE == psessionEntry->limSystemRole) ||
                (eLIM_BT_AMP_AP_ROLE == psessionEntry->limSystemRole) ||
                (eLIM_BT_AMP_STA_ROLE == psessionEntry->limSystemRole)
               )
            {
                limProcessProbeReqFrame(pMac,pBd,psessionEntry);
            }
        }
    }

} /*                                                */

/* 
                          
  
           
                           
                                                  
  
         
  
        
  
              
     
  
       
                                                                    
           
  
                                                                              
                                         
                                                           
                                                           
                                                   
  
               
 */
void
limSendSmeProbeReqInd(tpAniSirGlobal pMac,
                      tSirMacAddr peerMacAddr,
                      tANI_U8 *pProbeReqIE,
                      tANI_U32 ProbeReqIELen, 
                      tpPESession psessionEntry)
{
    tSirSmeProbeReqInd     *pSirSmeProbeReqInd;
    tSirMsgQ                msgQ;   

    pSirSmeProbeReqInd = vos_mem_malloc(sizeof(tSirSmeProbeReqInd));
    if ( NULL == pSirSmeProbeReqInd )
    {
        //          
        limLog(pMac, LOGP,
            FL("call to AllocateMemory failed for eWNI_SME_PROBE_REQ"));
            return;
    }
    
    msgQ.type =  eWNI_SME_WPS_PBC_PROBE_REQ_IND;
    msgQ.bodyval = 0;
    msgQ.bodyptr = pSirSmeProbeReqInd;
    
    pSirSmeProbeReqInd->messageType =  eWNI_SME_WPS_PBC_PROBE_REQ_IND;
    pSirSmeProbeReqInd->length = sizeof(tSirSmeProbeReq);
    pSirSmeProbeReqInd->sessionId = psessionEntry->smeSessionId;

    vos_mem_copy(pSirSmeProbeReqInd->bssId, psessionEntry->bssId, sizeof(tSirMacAddr));
    vos_mem_copy(pSirSmeProbeReqInd->WPSPBCProbeReq.peerMacAddr, peerMacAddr, sizeof(tSirMacAddr));

    MTRACE(macTraceMsgTx(pMac, psessionEntry->peSessionId, msgQ.type));
    pSirSmeProbeReqInd->WPSPBCProbeReq.probeReqIELen = (tANI_U16)ProbeReqIELen;
    vos_mem_copy(pSirSmeProbeReqInd->WPSPBCProbeReq.probeReqIE, pProbeReqIE, ProbeReqIELen);
    
    if (limSysProcessMmhMsgApi(pMac, &msgQ,  ePROT) != eSIR_SUCCESS){
                            PELOGE(limLog(pMac, LOGE, FL("couldnt send the probe req to hdd"));)
    } 
        
} /*                                 */
#ifdef WLAN_FEATURE_P2P_INTERNAL
void limSendP2PProbeResponse(tpAniSirGlobal pMac, tANI_U8 *pBd, 
                      tpPESession psessionEntry)
{
    tAniSSID            ssId = { P2P_WILDCARD_SSID_LEN, P2P_WILDCARD_SSID };
    tANI_U8             *pBody;
    tpSirMacMgmtHdr     pHdr;
    tANI_U32            frameLen;
    tSirProbeReq        probeReq;

    pHdr =  WDA_GET_RX_MAC_HEADER(pBd);
    //                                        
    pBody =  WDA_GET_RX_MPDU_DATA(pBd);   

    if( (pBody[0] == 0) && (pBody[1] == ssId.length) &&
      (vos_mem_compare(ssId.ssId, pBody + 2,
                       ssId.length)))
    {
        //                          
        frameLen = WDA_GET_RX_PAYLOAD_LEN(pBd);
        if (eSIR_FAILURE == sirConvertProbeReqFrame2Struct(pMac, pBody, frameLen, &probeReq))
        {
            PELOGW(limLog(pMac, LOGW, FL("Parse error ProbeRequest, length=%d, SA is:"), frameLen);)
            limPrintMacAddr(pMac, pHdr->sa, LOGW);
            pMac->sys.probeError++;
            return;
        }

        if (psessionEntry->pePersona == VOS_P2P_GO_MODE) 
        {
            ssId.length = psessionEntry->ssId.length;
            vos_mem_copy(ssId.ssId, psessionEntry->ssId.ssId,psessionEntry->ssId.length);
            limSendProbeRspMgmtFrame(pMac, pHdr->sa, &ssId, DPH_USE_MGMT_STAID, DPH_NON_KEEPALIVE_FRAME, 
                                     psessionEntry, probeReq.p2pIePresent );
        } 
        else 
        {
            limSendProbeRspMgmtFrame(pMac, pHdr->sa, &ssId, DPH_USE_MGMT_STAID, DPH_NON_KEEPALIVE_FRAME,
                                     psessionEntry, probeReq.p2pIePresent );
        }
    }
}
#endif //                                
